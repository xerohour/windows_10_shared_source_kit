/*++

Copyright (c) 1999, 2000  Microsoft Corporation

Module Name:

    int.c

Abstract:

    interrupt service routine

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1999, 2000 Microsoft Corporation.  All Rights Reserved.


Revision History:

    7-19-99 : created, jdunn

--*/



#include "common.h"


BOOLEAN
EHCI_InterruptService (
     __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    BOOLEAN usbInt;
    PHC_OPERATIONAL_REGISTER hcOp;
    ULONG enabledIrqs, frameNumber;
    USBSTS irqStatus;
    FRINDEX frameIndex;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return FALSE;
    }

    // assume it is not ours
    usbInt = FALSE;

    if (EHCI_HardwarePresent(DeviceData, FALSE) == FALSE) {
        return FALSE;
    }
    // get a mask of possible interrupts
    enabledIrqs = READ_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul);

    irqStatus.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);
    // just look at the IRQ status bits
    irqStatus.ul &= HcInterruptStatusMask;
    // AND with the enabled IRQs
    irqStatus.ul &= enabledIrqs;

    // irqStatus now possibly contains bits set for any currently
    // enabled interrupts

    if (irqStatus.ul != 0)  {

        //DeviceData->IrqStatus = irqStatus.ul;
        InterlockedOr(&DeviceData->IrqStatus, irqStatus.ul);

        WRITE_REGISTER_ULONG(&hcOp->UsbStatus.ul,
                             irqStatus.ul);

        if (irqStatus.HostSystemError) {

            DeviceData->HostSystemErrorCount++;

            DeviceData->HostSystemErrorResetNeeded = 1;
        }

        // This code maintains the 32-bit 1 ms frame counter

        // bugbug this code does not handle varaible frame list
        // sizes
        frameIndex.ul = READ_REGISTER_ULONG(&hcOp->UsbFrameIndex.ul);

        frameNumber = (ULONG) frameIndex.FrameListCurrentIndex;
        // shut off the microframes
        frameNumber >>= 3;

        // frame number is 12 bits, mask off any bogus bits
        frameNumber &= 0xFFF;

        // did the sign bit change ?
        if ((DeviceData->LastFrame ^ frameNumber) & 0x0400) {
            // Yes
            DeviceData->FrameNumberHighPart += 0x0800 -
                ((frameNumber ^ DeviceData->FrameNumberHighPart) & 0x0400);
        }

        // remember the last frame number
        DeviceData->LastFrame = frameNumber;

        // inications are that this came from the
        // USB controller
        usbInt = TRUE;

        // disable all interrupts until the DPC for ISR runs
        //WRITE_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul,
        //                     0);
    }

    return usbInt;
}

BOOLEAN
EHCI_InterruptDpcSyncRoutine (
    __in PVOID SynchronizeContext
    )
/*++

Routine Description:

    This routine checks for a pending Interrupt on Async Advance.  If an
    Interrupt on Async Advance is pending it is acknowledged and added
    to the set of interrupts to be processed by EHCI_InterruptDpcEx().

    The execution of this routine is synchronized with
    EHCI_InterruptService() (i.e. their execution is mutually exclusive)
    to prevent a single occurrence of a pending Interrupt on Async
    Advance from being processed twice by EHCI_InterruptDpcEx().

    The purpose of this routine is to provide a failsafe mechanism for
    processing a pending Interrupt on Async Advance in the event that
    EHCI_InterruptService() is not executing as expected for some
    unknown reason.

Arguments:

    SynchronizeContext - A pointer to the EHCI miniport DEVICE_DATA.

Return Value:

    TRUE if a pending Interrupt on Async Advance was acknowledged, else
    FALSE.  This return value is currently not used.

--*/
{
    PDEVICE_DATA DeviceData;
    PHC_OPERATIONAL_REGISTER hcOp;
    USBSTS irqStatus;
    USBINTR irqEnable;

    DeviceData = (PDEVICE_DATA) SynchronizeContext;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return FALSE;
    }

    // If IntOnAsyncAdvance is set in the current interrupt status bits
    // then acknowledge the IntOnasyncAdvance interrupt here now and add
    // that to the set of interrupts to be processed now in case there
    // is a problem preventing EHCI_InterruptService() from executing as
    // expected.

    irqStatus.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);

    irqEnable.ul = READ_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul);

    irqStatus.ul &= irqEnable.ul;

    if (irqStatus.IntOnAsyncAdvance) {

        InterlockedOr(&DeviceData->IrqStatus, HcInt_IntOnasyncAdvance);

        WRITE_REGISTER_ULONG(&hcOp->UsbStatus.ul, HcInt_IntOnasyncAdvance);

        return TRUE;

    } else {

        return FALSE;
    }
}

ULONG
EHCI_InterruptDpcEx (
     __inout PDEVICE_DATA DeviceData,
     BOOLEAN EnableInterrupts
    )
/*++

Routine Description:

    process an interrupt

Arguments:

    DeviceData - A pointer to the EHCI miniport DEVICE_DATA.

    EnableInterrupts - TRUE if the host controller interrupts should be
    reenabled.  If the host controller has been stopped and the IRQ has
    been disconnected this will be FALSE.

Return Value:

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    USBSTS irqStatus, deviceDataIrqStatus;
    FRINDEX frameIndex;
    ULONG irqFlags = 0;
    BOOLEAN invRH = FALSE;
    USHORT p;
    PORTSC port;
    EHCI_PORT_EVENT_CONTEXT portResumeContext;
    KIRQL irql;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return irqFlags;
    }

    // Get the current interrupts status bits and frame number for
    // diagnostic logging.
    //
    frameIndex.ul = READ_REGISTER_ULONG(&hcOp->UsbFrameIndex.ul);
    irqStatus.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);

    if (irqStatus.IntOnAsyncAdvance && EnableInterrupts &&
        DeviceData->Interrupt != NULL) {

        EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);

        DeviceData->CurrentIntOnAsyncAdvancePendingCount++;

        EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);

        DeviceData->StatIntOnAsyncAdvancePendingCount++;

        // Do not call the EHCI_InterruptDpcSyncRoutine() unless it
        // reasonably obvious that EHCI_InterruptService() is not
        // executing to processs the pending Interrupt on Async Advance.
        //
        if (DeviceData->CurrentIntOnAsyncAdvancePendingCount > 16) {

            DeviceData->StatIntOnAsyncAdvanceSyncRoutineCount++;

            // Check for and acknowledge a pending Interrupt on Async
            // Advance which has not yet been seen and acknowledged by
            // EHCI_InterruptService().
            //
            KeSynchronizeExecution(DeviceData->Interrupt,
                                   EHCI_InterruptDpcSyncRoutine,
                                   DeviceData);
        }
    }

    // Get the interrupts status bits as last seen and acknowledged by
    // EHCI_InterruptService().
    //
    deviceDataIrqStatus.ul = InterlockedExchange(&DeviceData->IrqStatus, 0);

    LOGENTRY(DeviceData, G, '_idp', deviceDataIrqStatus.ul,
             irqStatus.ul, frameIndex.ul);

    // Only process the interrupts status bits as last seen and
    // acknowledged by EHCI_InterruptService().
    //
    irqStatus.ul = deviceDataIrqStatus.ul;

    // AND with the enabled IRQs, these are the interrupts
    // we are interested in
    irqStatus.ul &= DeviceData->EnabledInterrupts.ul;

    if (irqStatus.UsbInterrupt ||
        irqStatus.UsbError ||
        irqStatus.IntOnAsyncAdvance) {
        LOGENTRY(DeviceData, G, '_iEP', irqStatus.ul, 0, 0);

        USBPORT_INVALIDATE_ENDPOINT(DeviceData, NULL);
    }

    if (irqStatus.IntOnAsyncAdvance) {
        LOGENTRY(DeviceData, G, '_db!', irqStatus.ul, 0, 0);

        EHCI_AnswerAsyncDoorbell(DeviceData);
    }

    // resume detect for global suspend of root hub
    if ((irqStatus.UsbInterrupt || irqStatus.PortChangeDetect) &&
        DeviceData->HcSuspendedFlag) {
        // treat general usbint or port change detect as wakeup event in suspended
        irqFlags |= USBMP_IRQ_SSP_RESUME_DETECT;
    }
    // if root hub is not in global suspend, check for ports signaling resume
    // and schedule async callbacks.
    else if (irqStatus.PortChangeDetect) {
        // Need to determine if port change is resume signaling or something else
        for (p=0; p<DeviceData->NumberOfPorts ; p++) {
            port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[p].ul);

            if (port.ForcePortResume && !TEST_BIT(DeviceData->PortInResume, p)) {
                // Device has signalled resume, need to time completion of resume signal
                LOGENTRY(DeviceData, G, '_rsg', p+1, port.ul, DeviceData);

                SET_BIT(DeviceData->PortInResume, p);

                portResumeContext.PortNumber = p+1; // USB ports are 1-based
                portResumeContext.AsyncIdleRef = EHCI_RefAsyncIdle(DeviceData, RH_PortResumeComplete, NULL);


                USBPORT_REQUEST_ASYNC_CALLBACK_EX(DeviceData,
                                      50, // callback in ms,
                                      &portResumeContext,
                                      sizeof(struct _EHCI_PORT_EVENT_CONTEXT),
                                      EHCI_RH_PortResumeComplete,
                                      NULL,
                                      TRUE);
            } else {
                if (port.PortConnectChange || port.PortEnableChange || port.OvercurrentChange ||
                    ((port.PortSuspend == 0) && TEST_BIT(DeviceData->PortSuspended,p))) {
                    LOGENTRY(DeviceData, G, '_chg', p+1, port.ul, DeviceData);
                    invRH = TRUE;
                }
            }
        }
        // mask off the port status change until the port driver
        // enables it again.
        DeviceData->EnabledInterrupts.PortChangeDetect = 0;
    }

    if (invRH) {
        // this will wake us from global suspend of root hub
        irqFlags |= USBMP_IRQ_INV_ROOTHUB;
    }

    // since ehci does not provide a way to globally mask
    // interrupts we must mask off all interrupts in our ISR.
    // When the ISR DPC completes we re-enable the set of
    // currently enabled interrupts.

    if (EnableInterrupts) {

        LOGENTRY(DeviceData, G, '_iEE', 0, 0, DeviceData->EnabledInterrupts.ul);

        WRITE_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul,
                             DeviceData->EnabledInterrupts.ul);
    }

    return irqFlags;
}


VOID
EHCI_InterruptDpc (
     __inout PDEVICE_DATA DeviceData,
     BOOLEAN EnableInterrupts
    )
/*++

Routine Description:

    process an interrupt

Arguments:

Return Value:

--*/
{
    EHCI_InterruptDpcEx(DeviceData, EnableInterrupts);
}

VOID
USBMPFN
EHCI_DisableInterrupts(
     __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp = NULL;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    // mask off all interrupts
    WRITE_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul,
                         0);
}


VOID
USBMPFN
EHCI_FlushInterrupts(
     __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp = NULL;
    USBSTS irqStatus;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    // flush any outstanding interrupts
    irqStatus.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);
    WRITE_REGISTER_ULONG(&hcOp->UsbStatus.ul,
                         irqStatus.ul);
}


VOID
USBMPFN
EHCI_EnableInterrupts(
     __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp = NULL;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    // activate the controllers interrupt
    WRITE_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul,
                         DeviceData->EnabledInterrupts.ul);
}


VOID
EHCI_RH_DisableIrq(
     __inout PDEVICE_DATA DeviceData
    )
{
    PHC_OPERATIONAL_REGISTER hcOp = NULL;
    USBINTR enabledIrqs;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    // clear the port change interrupt
    enabledIrqs.ul =
        READ_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul);

    enabledIrqs.PortChangeDetect =
        DeviceData->EnabledInterrupts.PortChangeDetect = 0;

    if (enabledIrqs.UsbInterrupt) {
        WRITE_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul,
                             enabledIrqs.ul);
    }
}


VOID
EHCI_RH_EnableIrq(
     __inout PDEVICE_DATA DeviceData
    )
{
    PHC_OPERATIONAL_REGISTER hcOp = NULL;
    USBINTR enabledIrqs;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    // enable the port change interrupt
    enabledIrqs.ul =
        READ_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul);

    enabledIrqs.PortChangeDetect =
            DeviceData->EnabledInterrupts.PortChangeDetect = 1;

    if (enabledIrqs.UsbInterrupt) {
        WRITE_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul,
                             enabledIrqs.ul);
    }

}


ULONG
EHCI_Get32BitFrameNumber(
     __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

    This MINIPORT_Get32BitFrameNumber() routine is always called with
    the USBPORT Fdo.CoreFunctionSpin spinlock held.

--*/
{
    ULONG highPart, currentFrame, frameNumber;
    PHC_OPERATIONAL_REGISTER hcOp = NULL;
    FRINDEX frameIndex;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return 0xFFFFFFFF;
    }

    // get Hcd's high part of frame number
    highPart = DeviceData->FrameNumberHighPart;

    // bugbug this code does not handle varaible frame list
    // sizes
    frameIndex.ul = READ_REGISTER_ULONG(&hcOp->UsbFrameIndex.ul);

    frameNumber = (ULONG) frameIndex.FrameListCurrentIndex;
    // shift off the microframes
    frameNumber >>= 3;

    currentFrame = ((frameNumber & 0x0bff) | highPart) +
        ((frameNumber ^ highPart) & 0x0400);

    return currentFrame;

}

#ifdef VISTA_RTM

ULONG
EHCI_Get32BitMicroFrameNumber(
     __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

    This MINIPORT_Get32BitMicroFrameNumber() routine is always called
    with the USBPORT Fdo.CoreFunctionSpin spinlock held.

--*/
{
    ULONG highPart, currentFrame, frameNumber;
    ULONG currentMicroFrame, mf_bits;
    PHC_OPERATIONAL_REGISTER hcOp = NULL;
    FRINDEX frameIndex;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return 0xFFFFFFFF;
    }

    // get Hcd's high part of frame number
    highPart = DeviceData->FrameNumberHighPart;

    // bugbug this code does not handle varaible frame list
    // sizes
    frameIndex.ul = READ_REGISTER_ULONG(&hcOp->UsbFrameIndex.ul);

    frameNumber = (ULONG) frameIndex.FrameListCurrentIndex;
    mf_bits = frameNumber & 7;
    // shift off the microframes
    frameNumber >>= 3;

    currentFrame = ((frameNumber & 0x0bff) | highPart) +
        ((frameNumber ^ highPart) & 0x0400);
    currentMicroFrame = (currentFrame <<=3) | mf_bits;

    return currentMicroFrame;

}

#endif // VISTA_RTM


BOOLEAN
EHCI_HardwarePresent(
    __inout PDEVICE_DATA DeviceData,
    BOOLEAN Notify
    )
{
    ULONG tmp;
    PHC_OPERATIONAL_REGISTER hcOp;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return FALSE;
    }

    tmp = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);

    if (tmp == 0xffffffff) {
        if (Notify) {
            USBPORT_INVALIDATE_CONTROLLER(DeviceData, UsbMpControllerRemoved);
        }
        return FALSE;
    }

    return TRUE;
}
