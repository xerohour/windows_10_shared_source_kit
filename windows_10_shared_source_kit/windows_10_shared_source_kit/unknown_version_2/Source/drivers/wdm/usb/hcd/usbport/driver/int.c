/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    int.c

Abstract:

    code to handle adapter interrupts

Environment:

    kernel mode only

Notes:

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs

--*/

#include "common.h"


BOOLEAN
USBPORT_InterruptService(
    PKINTERRUPT Interrupt,
    PVOID Context
    )

/*++

Routine Description:

    This is the interrupt service routine for the PORT driver.

Arguments:

    Interrupt - A pointer to the interrupt object for this interrupt.

    Context - A pointer to the device object.

Return Value:

    Returns TRUE if the interrupt was expected (and therefore processed);
    otherwise, FALSE is returned.

--*/

{
    PDEVICE_OBJECT fdoDeviceObject = Context;
    PDEVICE_EXTENSION devExt;
    BOOLEAN usbInt = FALSE;

    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);


    // by definition, if we are in any other power state than D0 then
    // the interrupt could not be from the controller.  To handle this
    // case we use our internal flag that indicates interrupts are
    // disabled
    if (devExt->Fdo.Flags.IrqEnabled == 0) {
        return FALSE;
    }

    // check flag and calldown to miniport
    if (devExt->Fdo.MpStateFlags & MP_STATE_STARTED) {
        usbInt = MPf_InterruptService(devExt);
    }
//#if DBG
//      else {
//        // interrupt before we have started,
//        // it had better not be ours
//        TEST_TRAP();
//    }
//#endif

    if (usbInt) {
        LONG pnpIrqState;
        BOOLEAN queued;

        // synwith isr

        devExt->Fdo.StatPciInterruptCount++;
        pnpIrqState = InterlockedIncrement(&devExt->Fdo.IoConnectInterruptState);

        // This means the irq was called after we called IoDisconnecteInterrupt
        USBPORT_ASSERT(pnpIrqState != 0);
        if (pnpIrqState) {
            queued = KeInsertQueueDpc(&devExt->Fdo.IsrDpc, NULL, 0);
            if (FALSE == queued) {
                InterlockedDecrement(&devExt->Fdo.IoConnectInterruptState);
            }
        }
    }

    return usbInt;
}


VOID
USBPORT_IsrDpc(
    PKDPC Dpc,
    PVOID DeferredContext,
    PVOID SystemArgument1,
    PVOID Simulated
    )

/*++

Routine Description:

    This routine runs at DISPATCH_LEVEL IRQL.

    If the controller was the source of the interrupt this
    routine will be called.

Arguments:

    Dpc - Pointer to the DPC object.

    DeferredContext - supplies the DeviceObject.

    SystemArgument1 - not used.

    SystemArgument2 - indicates a simulated interrupt

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT fdoDeviceObject;
    BOOLEAN enableIrq;
    ULONG irqFlags = 0;
    LONG pnpIrqState, inDpc;
    LARGE_INTEGER tickCount;
    ETW_EXECUTION_METRICS   execMetrics;

    ICE_START_TRACE();
    ETW_MEASURE_TIME_START(execMetrics);

    fdoDeviceObject = (PDEVICE_OBJECT) DeferredContext;
    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    inDpc = InterlockedIncrement(&devExt->Fdo.InIsrDpc);
    //allow isrdpc to be rentered
    //USBPORT_ASSERT(inDpc == 1);

    LOGENTRY(fdoDeviceObject, LOG_MISC, 'iDP+', fdoDeviceObject, 0, inDpc);
    if (devExt->UsbDbgBreakFlags & HC_BREAK_ON_ISR_DPC) {
        NT_ASSERTMSG("Break on Flag HC_BREAK_ON_ISR_DPC",FALSE);
    }


    USBPORT_WMI_EVENT(1,                                            // level
                      USBPORT_DPC_TIME,                             // enable flag
                      (USBPORT_EVENT_TYPE_DPC_START,                // event type
                       GUID_USB_PERFORMANCE_TRACING,                // trace guid
                       fdoDeviceObject,
                       0,
                       "USBPORT DPC running ..."));

    KeAcquireSpinLockAtDpcLevel(&devExt->Fdo.DM_TimerSpin);

    if (devExt->Fdo.DmTimerFlags.DecodesOff) {

        // bypass miniport if decodes are off
        KeReleaseSpinLockFromDpcLevel(&devExt->Fdo.DM_TimerSpin);

    } else {

        KeReleaseSpinLockFromDpcLevel(&devExt->Fdo.DM_TimerSpin);

        KeAcquireSpinLockAtDpcLevel(&devExt->Fdo.IsrDpcSpin);

        KeQueryTickCount(&tickCount);
        LOGENTRY(fdoDeviceObject, LOG_MISC, 'iDlk', fdoDeviceObject, tickCount.LowPart, tickCount.HighPart);
        //deltaTicks = tickCount.QuadPart - devExt->Fdo.LastIsrDpcTick.QuadPart;
        //LOGENTRY(fdoDeviceObject, LOG_MISC, 'iD+X', fdoDeviceObject, deltaTicks.LowPart, deltaTicks.HighPart);

        if (devExt->Fdo.Flags.IrqEnabled) {
            enableIrq = TRUE;
        } else {
            enableIrq = FALSE;
        }

        irqFlags = MPf_InterruptDpcEx(devExt, enableIrq);
        // we expect the rh port change irq to be disabled in this case
        if (TEST_FLAG(irqFlags, USBMP_IRQ_SSP_RESUME_DETECT) ||
            TEST_FLAG(irqFlags, USBMP_IRQ_INV_ROOTHUB)) {
            devExt->Fdo.Flags.RhIrqEnabled = 0;
        }

        KeQueryTickCount(&tickCount);
        LOGENTRY(fdoDeviceObject, LOG_MISC, 'iDuk', irqFlags, tickCount.LowPart, tickCount.HighPart);
        KeReleaseSpinLockFromDpcLevel(&devExt->Fdo.IsrDpcSpin);

    }

    // for neohub wait_wake irps are only completed bottom-to-top in associated
    // with system suspend so this request MUST originate in PCI.
    // We don't complete wake on HC interrupts to support suspend in S0 anymore.


    // process hardware state changes for endpoints triggered by the irq
    // this routine will abort transfers in the miniport for endpoints that have transitioned to PAUSE
    USBPORT_ProcessNeoStateChangeList(fdoDeviceObject);

    // To avoid reentrance and lock contention root hub is signalled after the DPCforIsr
    // returns from the miniport.
    if (TEST_FLAG(irqFlags, USBMP_IRQ_SSP_RESUME_DETECT) ||
        TEST_FLAG(irqFlags, USBMP_IRQ_INV_ROOTHUB)) {

        USBPORT_Ev_Rh_IntrEp_Invalidate(fdoDeviceObject);
    }

    
    // Increment a DpcGeneration count. This is used by 
    // devExt->Fdo.UsbHcIntDpc
    InterlockedIncrement(&devExt->Fdo.UsbHcIntDpcGenerationCount);
    
    // queue the USBINT dpc to poll the endpoints for completed transfers
    USBPORT_Xdpc_Signal(fdoDeviceObject, &devExt->Fdo.UsbHcIntDpc, NULL);

    KeQueryTickCount(&tickCount);
    LOGENTRY(fdoDeviceObject, LOG_MISC, 'iDP-', 0, tickCount.LowPart, tickCount.HighPart);

    USBPORT_WMI_EVENT(1,                                            // level
                      USBPORT_DPC_TIME,                             // enable flag
                      (USBPORT_EVENT_TYPE_DPC_END,                  // event type
                       GUID_USB_PERFORMANCE_TRACING,                // trace guid
                       fdoDeviceObject,
                       irqFlags,
                       "USBPORT DPC Completing"));

    InterlockedDecrement(&devExt->Fdo.InIsrDpc);

    // synchronize USBPORT_IsrDpc with IoDisconnectInterrupt
    // -1 interrupt disconnected
    pnpIrqState = InterlockedDecrement(&devExt->Fdo.IoConnectInterruptState);
    if (pnpIrqState == -1) {
        // irq disconnected while dpc running
        USBPORT_Core_DecIoRequestCount(fdoDeviceObject, &devExt->Fdo.UsbIoIsrDpc, TRUE);
    }

    ETW_STOP_AND_LOG_TIME(fdoDeviceObject, execMetrics, ISRDPC);
    ICE_STOP_TRACE();
    return;
}


VOID
#pragma prefast(suppress:__WARNING_FAILING_TO_RELEASE, "Esp bug 762")
USBPORT_MpInterrupts(
    PDEVICE_OBJECT FdoDeviceObject,
    BOOLEAN Enable
    )

/*++

Routine Description:

    Requests the miniport to globally DISABLE/ENABLE HW interrupts. This routine is used by the Power code to globally
    disable irqa when the controller is shut down or put in low power.

    MP_DisableInterrupts
    MP_EnableInterrupts

Arguments:

    Enable - TRUE means turn interrupts on. FALSE maeans turn them off.

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION devExt;
    KIRQL irql = 0;
    BOOLEAN sync; // synchronous call

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(MPF(devExt).MINIPORT_EnableInterrupts != NULL);
    USBPORT_ASSERT(MPF(devExt).MINIPORT_DisableInterrupts != NULL);

    sync =
        !(devExt->Fdo.HcOptionFlags & USB_MINIPORT_OPT_NO_IRQ_SYNC);

    if (sync) {
        KeAcquireSpinLock(&devExt->Fdo.IsrDpcSpin, &irql);
    }

    if (Enable) {
        devExt->Fdo.Flags.IrqEnabled = 1;

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'irqE',
            FdoDeviceObject, 0, 0);

        if (!devExt->Fdo.PowerFlags.MpRestartFailed) {

            MPF(devExt).MINIPORT_EnableInterrupts(devExt->Fdo.MiniportDeviceData);
        }
    } else {
        LOGENTRY(FdoDeviceObject, LOG_MISC, 'irqD',
            FdoDeviceObject, 0, 0);

        if (!devExt->Fdo.PowerFlags.MpRestartFailed) {

            MPF(devExt).MINIPORT_DisableInterrupts(devExt->Fdo.MiniportDeviceData);
        }

        devExt->Fdo.Flags.IrqEnabled = 0;
    }

    if (sync) {
        KeReleaseSpinLock(&devExt->Fdo.IsrDpcSpin, irql);
    }
}


VOID
USBPORT_SimulateInterruptDpc(
    PKDPC Dpc,
    PVOID DeferredContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2
    )
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
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION devExt;
    BOOLEAN queued;
    LONG pnpIrqState;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;
    ETW_EXECUTION_METRICS   execMetrics;

    ICE_START_TRACE();
    ETW_MEASURE_TIME_START(execMetrics);

    fdoDeviceObject = (PDEVICE_OBJECT)DeferredContext;

    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    pnpIrqState = InterlockedIncrement(&devExt->Fdo.IoConnectInterruptState);

    // This means the irq was simulated after we called IoDisconnecteInterrupt
    // This is OK since the interrupt is simulated
    //USBPORT_ASSERT(pnpIrqState != 0);

    if (pnpIrqState) {
        // queue the ISR dpc but indicate simulated irq
        queued = KeInsertQueueDpc(&devExt->Fdo.IsrDpc, NULL, (PVOID)1);
        if (FALSE == queued) {
            // dpc already queued possibly by hardware interrupt.
            InterlockedDecrement(&devExt->Fdo.IoConnectInterruptState);
        }
    } else {
        InterlockedDecrement(&devExt->Fdo.IoConnectInterruptState);
    }

    // Free the iorequest used to track the simulate interrupt request
    //
    usbIoRequest = InterlockedExchangePointer(&devExt->Fdo.SimulateInterruptIoRequest,
                                              NULL);

    USBPORT_Core_UnlinkAndFreeIoRequest(usbIoRequest, usbIoRequest);

    ETW_STOP_AND_LOG_TIME(fdoDeviceObject, execMetrics, SIMULATEDPC);
    ICE_STOP_TRACE();

    return;
}


VOID
USBPORT_SimulateInterrupt(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Simulates a hradware interrupt in i frame

Arguments:

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION devExt;
    LONG dueTime;
    ULONG timerIncrement;
    BOOLEAN queued;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // Allocate an iorequest to track the simulate interrupt request
    //
    usbIoRequest = USBPORT_Core_AllocIoRequest(FdoDeviceObject,
                                               NULL,
                                               UsbIoTag_SimIrq);

    if (!usbIoRequest) {
        return;
    }

    if (InterlockedCompareExchangePointer(&devExt->Fdo.SimulateInterruptIoRequest,
                                          usbIoRequest,
                                          NULL))
    {
        // There is an outstanding previously queued simulate interrupt
        // request.  No need to queue another one.
        //
        USBPORT_Core_UnlinkAndFreeIoRequest(usbIoRequest, usbIoRequest);

        return;
    }

    if (devExt->Fdo.SimulateInterruptHighResTimer != NULL) {
        // Interrupt in 2 milliseconds
        USBPORT_SetHighResTimer(devExt->Fdo.SimulateInterruptHighResTimer, 2);
    } else {

        timerIncrement = KeQueryTimeIncrement() - 1;

        dueTime = -1 * (timerIncrement + MILLISECONDS_TO_100_NS_UNITS(1));

        queued = KeSetTimer(&devExt->Fdo.SimulateInterruptTimer,
                            RtlConvertLongToLargeInteger(dueTime),
                            &devExt->Fdo.SimulateInterruptDpc);

        USBPORT_ASSERT(!queued);
    }

    return;
}

VOID
USBPORT_SignalInterrupt(
    __in PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Simulates a hradware interrupt in i frame

Arguments:

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION devExt;
    LONG pnpIrqState;
    BOOLEAN queued;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    pnpIrqState = InterlockedIncrement(&devExt->Fdo.IoConnectInterruptState);

    // This means the irq was simulated after we called IoDisconnecteInterrupt
    // This is OK since the interrupt is simulated
    //USBPORT_ASSERT(pnpIrqState != 0);

    if (pnpIrqState) {
        // queue the ISR dpc but indicate simulated irq
        queued = KeInsertQueueDpc(&devExt->Fdo.IsrDpc, NULL, (PVOID)1);
        if (FALSE == queued) {
            // dpc already queued possibly by hardware interrupt.
            InterlockedDecrement(&devExt->Fdo.IoConnectInterruptState);
        }
    } else {
        InterlockedDecrement(&devExt->Fdo.IoConnectInterruptState);
    }

    return;
}

