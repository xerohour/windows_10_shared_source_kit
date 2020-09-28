/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    hub.c

     3-10-2002

Abstract:

    Provides general hub functionality for usbhub.sys. These are
    the utility routines along with the main dispatch table and
    driver entry points.


Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/

#include "hubdef.h"
#include "pch.h"

// For DRM support - (Digital Rights Management)
// NOTE: including only ksdrmhlp.h allows usbhub to build with ntddk.h instead of wdm.h
#include <ksdrmhlp.h>

// For ACPI IOCTL interface
#include <acpiioct.h>

//#include <windef.h>
//#include <ks.h>

// Misc Globals

#ifdef HUB_WPP
#include "hub.tmh"
#endif

CCHAR usbfile_hub_c[] = "hub.c";

#define USBFILE usbfile_hub_c


typedef NTSTATUS HUB_GEN_HANDLER(PDEVICE_OBJECT DeviceObject, PIRP Irp);

typedef struct _HUBFDO_GEN_DISPATCH {

    UCHAR MajorFunction;
    HUB_GEN_HANDLER *GenHandler;

} HUB_GEN_DISPATCH, *PHUB_GEN_DISPATCH;

/* dispatch for major functions handed to FDO */

const HUB_GEN_DISPATCH HubFdoGenDispatch[IRP_MJ_MAXIMUM_FUNCTION+1] = {
        IRP_MJ_CREATE,                   UsbhFdoSuccess,
        IRP_MJ_CREATE_NAMED_PIPE,        UsbhTrapBadIrp,
        IRP_MJ_CLOSE,                    UsbhFdoSuccess,
        IRP_MJ_READ,                     UsbhTrapBadIrp,
        IRP_MJ_WRITE,                    UsbhTrapBadIrp,
        IRP_MJ_QUERY_INFORMATION,        UsbhTrapBadIrp,
        IRP_MJ_SET_INFORMATION,          UsbhTrapBadIrp,
        IRP_MJ_QUERY_EA,                 UsbhTrapBadIrp,
        IRP_MJ_SET_EA,                   UsbhTrapBadIrp,
        IRP_MJ_FLUSH_BUFFERS,            UsbhTrapBadIrp,
        IRP_MJ_QUERY_VOLUME_INFORMATION, UsbhTrapBadIrp,
        IRP_MJ_SET_VOLUME_INFORMATION,   UsbhTrapBadIrp,
        IRP_MJ_DIRECTORY_CONTROL,        UsbhTrapBadIrp,
        IRP_MJ_FILE_SYSTEM_CONTROL,      UsbhTrapBadIrp,
        IRP_MJ_DEVICE_CONTROL,           UsbhFdoDeviceControl,
        IRP_MJ_INTERNAL_DEVICE_CONTROL,  UsbhFdoInternalDeviceControl,
        IRP_MJ_SHUTDOWN,                 UsbhTrapBadIrp,
        IRP_MJ_LOCK_CONTROL,             UsbhTrapBadIrp,
        IRP_MJ_CLEANUP,                  UsbhTrapBadIrp,
        IRP_MJ_CREATE_MAILSLOT,          UsbhTrapBadIrp,
        IRP_MJ_QUERY_SECURITY,           UsbhTrapBadIrp,
        IRP_MJ_SET_SECURITY,             UsbhTrapBadIrp,
        IRP_MJ_POWER,                    UsbhFdoPower,
        IRP_MJ_SYSTEM_CONTROL,           UsbhFdoSystemControl,
        IRP_MJ_DEVICE_CHANGE,            UsbhTrapBadIrp,
        IRP_MJ_QUERY_QUOTA,              UsbhTrapBadIrp,
        IRP_MJ_SET_QUOTA,                UsbhTrapBadIrp,
        IRP_MJ_PNP,                      UsbhFdoPnp
};


const HUB_GEN_DISPATCH HubPdoGenDispatch[IRP_MJ_MAXIMUM_FUNCTION+1] = {
        IRP_MJ_CREATE,                   UsbhPdoSuccess,
        IRP_MJ_CREATE_NAMED_PIPE,        UsbhTrapBadIrp,
        IRP_MJ_CLOSE,                    UsbhPdoSuccess,
        IRP_MJ_READ,                     UsbhTrapBadIrp,
        IRP_MJ_WRITE,                    UsbhTrapBadIrp,
        IRP_MJ_QUERY_INFORMATION,        UsbhTrapBadIrp,
        IRP_MJ_SET_INFORMATION,          UsbhTrapBadIrp,
        IRP_MJ_QUERY_EA,                 UsbhTrapBadIrp,
        IRP_MJ_SET_EA,                   UsbhTrapBadIrp,
        IRP_MJ_FLUSH_BUFFERS,            UsbhTrapBadIrp,
        IRP_MJ_QUERY_VOLUME_INFORMATION, UsbhTrapBadIrp,
        IRP_MJ_SET_VOLUME_INFORMATION,   UsbhTrapBadIrp,
        IRP_MJ_DIRECTORY_CONTROL,        UsbhTrapBadIrp,
        IRP_MJ_FILE_SYSTEM_CONTROL,      UsbhTrapBadIrp,
        IRP_MJ_DEVICE_CONTROL,           UsbhPdoDeviceControl,
        IRP_MJ_INTERNAL_DEVICE_CONTROL,  UsbhPdoInternalDeviceControl,
        IRP_MJ_SHUTDOWN,                 UsbhTrapBadIrp,
        IRP_MJ_LOCK_CONTROL,             UsbhTrapBadIrp,
        IRP_MJ_CLEANUP,                  UsbhTrapBadIrp,
        IRP_MJ_CREATE_MAILSLOT,          UsbhTrapBadIrp,
        IRP_MJ_QUERY_SECURITY,           UsbhTrapBadIrp,
        IRP_MJ_SET_SECURITY,             UsbhTrapBadIrp,
        IRP_MJ_POWER,                    UsbhPdoPower,
        IRP_MJ_SYSTEM_CONTROL,           UsbhPdoSystemControl,
        IRP_MJ_DEVICE_CHANGE,            UsbhTrapBadIrp,
        IRP_MJ_QUERY_QUOTA,              UsbhTrapBadIrp,
        IRP_MJ_SET_QUOTA,                UsbhTrapBadIrp,
        IRP_MJ_PNP,                      UsbhPdoPnp
};


NTSTATUS
UsbhSetTimer(
    PDEVICE_OBJECT HubFdo,
    ULONG MsTime,
    PKTIMER Timer,
    PKDPC Dpc,
    ULONG Tag,
    BOOLEAN ReferenceTimer
    )
/*++
Routine Description:

    sets a timer DPC with the millisecond value

Arguments:

Return Value:

    STATUS_DEVICE_BUSY - if the timer is already in the queue (TRUE returned from KeSetTimer)
    STATUS_SUCCESS - if timer was queued
    and other error indicates tiner was not queued.



-- */
{
    LONG dueTime;
    ULONG timerIncrement;
    BOOLEAN inQ;
    NTSTATUS nts;

    //LOG(HubFdo, LOG_HUB, 'sTmr', MsTime, Timer);

    if (ReferenceTimer) {
        nts = UsbhReferenceListAdd(HubFdo, Timer, Tag);
        if (NT_ERROR(nts)) {
            return nts;
        }
    }

    timerIncrement = KeQueryTimeIncrement() - 1;
    dueTime = -1 * (timerIncrement + MILLISECONDS_TO_100_NS_UNITS(MsTime));

    inQ = KeSetTimer(Timer,
                      RtlConvertLongToLargeInteger(dueTime),
                      Dpc);

    if (inQ) {
        // already in the queue
        if (ReferenceTimer) {
            UsbhReferenceListRemove(HubFdo, Timer);
        }
        nts = STATUS_DEVICE_BUSY;
    }

    //LOG(HubFdo, LOG_HUB, 'sTm>', inQ, dueTime);
    UsbhAssert(HubFdo, inQ == FALSE);
    nts = STATUS_SUCCESS;

    return nts;

}


ULONG
UsbhRevSig(
    ULONG Sig
    )
/*++
Routine Description:
    reverses a signature tag ie 'xfoo' ('o''o''f''x') becomes  'oofx' or
    'x''f''o''o'

    This is to allow sigs in the ULONG form '1234' to appear in the debugger
    and in data structures as a string of bytes in the same form ie
    '1''2''3''4'.
--*/
{
    LOGSIG sig, rsig;

    sig.l = Sig;

    rsig.b.Byte0 = sig.b.Byte3;
    rsig.b.Byte1 = sig.b.Byte2;
    rsig.b.Byte2 = sig.b.Byte1;
    rsig.b.Byte3 = sig.b.Byte0;

    return rsig.l;
}


NTSTATUS
UsbhWaitEventWithTimeoutEx(
    __in PDEVICE_OBJECT HubFdo,
    __inout PKEVENT Event,
    __in LONG Milliseconds,
    __in ULONG LogSig,
    __in USBHUB_FATAL_TIMEOUT_CODE TimeoutCode,
    __in_opt PHUB_PORT_DATA pd
    )
/*++

Routine Description:

   Waits on a specified event with a given timeout in milliseconds.

Arguments:

    HubFdo - Hub

    Event

    Milliseconds - ms to wait, a value of zero indicates not timeout (override to disable timeouts for testing)

    LogSig - signature for hub log,

    Timeout_FatalDbgCode - indcates where the timeout occured an non-zero value here will cause the
        function to generate a bugcheck 0xFE.

    Timeout_FatalDbgContext - option context pointer loged with the bugcheck

Return Value:

    return code from KeWaitForSingleObject
--*/
{
    LARGE_INTEGER timeout;
    LARGE_INTEGER zero_timeout = {0};
    LARGE_INTEGER t1, t2, tx, tmoLg;
    LARGE_INTEGER timerIncrement;
    NTSTATUS nts = STATUS_SUCCESS;
    PSSH_BUSY_HANDLE bh;

    LOG(HubFdo, LOG_HUB, LogSig, Milliseconds, Event);

    timerIncrement = RtlConvertLongToLargeInteger(KeQueryTimeIncrement() - 1);

    tmoLg = RtlConvertLongToLargeInteger(Milliseconds);
    tmoLg = RtlExtendedIntegerMultiply(tmoLg, 10000);
    tmoLg = RtlLargeIntegerAdd(tmoLg, timerIncrement);
    LOG(HubFdo, LOG_HUB, 'WTE1', NULL, Milliseconds);

    timeout = RtlExtendedIntegerMultiply(tmoLg, -1);

    // round up to the next highest timer increment
    //time.LowPart = -1 * (10000 * 500 + timerIncerent);


    //usb_frame = Usbh_UsbdReadFrameCounter(HubFdo);
    t1.QuadPart = KeQueryInterruptTime();
    LOG(HubFdo, LOG_HUB, 'wtT1', t1.LowPart, t1.HighPart);

    if ((Milliseconds) && (TimeoutCode != Timeout_Not_Fatal) && (pd)) {
        PVOID WaitObjects[2];

        bh = UsbhIncHubBusy(HubFdo, PDC(pd), pd, UsbhWaitEvent_Tag, NO_RESUME_HUB);

        WaitObjects[0]=Event;
        WaitObjects[1]=&pd->PortChangeListInsertEvent;

        do {
            nts = KeWaitForMultipleObjects(sizeof(WaitObjects)/sizeof(WaitObjects[0]),
                                           WaitObjects,
                                           WaitAny,
                                           Executive,
                                           KernelMode,
                                           FALSE,
                                           &timeout,
                                           NULL);

            if (nts == STATUS_WAIT_0) {
                // Event has completed; we can exit
                break;
            } else if (nts == STATUS_WAIT_1) {

                //
                //  Check if the main event is signaled too. If
                //  yes then just break
                //
                nts = KeWaitForSingleObject(Event,
                                            Executive,
                                            KernelMode,
                                            FALSE,
                                            &zero_timeout);
                if ( nts == STATUS_SUCCESS ) {
                    break;
                }


                // There is something in the port change queue that needs
                // to be dispatched; go ahead and run the queue in case the
                // work item thread is being starved

                // determine how much time has elapsed
                t2.QuadPart = KeQueryInterruptTime();
                tx.QuadPart = t2.QuadPart-t1.QuadPart;

                // Subtract elapsed time to compute new timeout value
                timeout.QuadPart = (tmoLg.QuadPart - tx.QuadPart) * -1;

                //
                // Synchronize with the worker thread. But include the
                // wait for the main event. This would avoid any lock
                // order dependencies in the calling thread.
                //

                WaitObjects[1]= &pd->PortChangeQueueLock;

                nts = KeWaitForMultipleObjects(sizeof(WaitObjects)/sizeof(WaitObjects[0]),
                                               WaitObjects,
                                               WaitAny,
                                               Executive,
                                               KernelMode,
                                               FALSE,
                                               &timeout,
                                               NULL);
                if (nts == STATUS_WAIT_0) {
                    // Event has completed; we can exit
                    break;
                } else if (nts == STATUS_WAIT_1) {
                    LOG(HubFdo, LOG_PCQ, 'WTRn', pd, pd->PortNumber);
                    pd->TimeoutCode = TimeoutCode;
                    UsbhPCE_wRun(HubFdo, PDC(pd), pd);
                } else if (nts == STATUS_TIMEOUT) {
                    break;
                } else{
                    UsbhAssert(HubFdo, NULL);
                }

                WaitObjects[1] = &pd->PortChangeListInsertEvent;

            } else if (nts == STATUS_TIMEOUT) {
                // Timed-out, time to bugcheck
                break;
            } else {
                // Shouldn't get here
                UsbhAssert(HubFdo, NULL);
            }

            // determine how much time has elapsed
            t2.QuadPart = KeQueryInterruptTime();
            tx.QuadPart = t2.QuadPart-t1.QuadPart;

            // See if we've timed out
            if (tx.QuadPart >= tmoLg.QuadPart) {
                nts = STATUS_TIMEOUT;
                break;
            }

            // Subtract elapsed time to compute new timeout value
            timeout.QuadPart = (tmoLg.QuadPart - tx.QuadPart) * -1;
        } WHILE (1);

        UsbhDecHubBusy(HubFdo, PDC(pd), bh);

    } else if ((Milliseconds) && (TimeoutCode == Timeout_Not_Fatal)) {
        nts = KeWaitForSingleObject(Event,
                                    Executive,
                                    KernelMode,
                                    FALSE,
                                    &timeout);

    } else if (TimeoutCode == Timeout_Not_Fatal_Process_Queue) {
        PVOID WaitObjects[2];

        WaitObjects[0]=Event;
        WaitObjects[1]=&pd->PortChangeListInsertEvent;

        do {
            nts = KeWaitForMultipleObjects(sizeof(WaitObjects)/sizeof(WaitObjects[0]),
                                           WaitObjects,
                                           WaitAny,
                                           Executive,
                                           KernelMode,
                                           FALSE,
                                           Milliseconds ? &timeout : NULL,
                                           NULL);

            if (nts == STATUS_WAIT_0) {
                // Event has completed; we can exit
                break;
            } else if (nts == STATUS_WAIT_1) {

                //
                //  Check if the main event is signaled too. If
                //  yes then just break
                //
                nts = KeWaitForSingleObject(Event,
                                            Executive,
                                            KernelMode,
                                            FALSE,
                                            &zero_timeout);
                if ( nts == STATUS_SUCCESS ) {
                    break;
                }

                // There is something in the port change queue that needs
                // to be dispatched; go ahead and run the queue in case the
                // work item thread is being starved

                //
                // Synchronize with the worker thread. But include the
                // wait for the main event. This would avoid any lock
                // order dependencies in the calling thread.
                //
                WaitObjects[1]= &pd->PortChangeQueueLock;

                nts = KeWaitForMultipleObjects(sizeof(WaitObjects)/sizeof(WaitObjects[0]),
                                               WaitObjects,
                                               WaitAny,
                                               Executive,
                                               KernelMode,
                                               FALSE,
                                               NULL,
                                               NULL);
                if (nts == STATUS_WAIT_0) {
                    // Event has completed; we can exit
                    break;
                } else if (nts == STATUS_WAIT_1) {
                    LOG(HubFdo, LOG_PCQ, 'WTRn', pd, pd->PortNumber);
                    UsbhPCE_wRun(HubFdo, PDC(pd), pd);
                } else{
                    UsbhAssert(HubFdo, NULL);
                }

                WaitObjects[1] = &pd->PortChangeListInsertEvent;

            } else if (nts == STATUS_TIMEOUT) {
                break;
            } else {
                // Shouldn't get here
                UsbhAssert(HubFdo, NULL);
            }
        } WHILE (1);

    } else {
        LOG(HubFdo, LOG_HUB, 'wtTX', 0, 0);

        nts = KeWaitForSingleObject(Event,
                                    Executive,
                                    KernelMode,
                                    FALSE,
                                    NULL);
    }

    //usb_frame = Usbh_UsbdReadFrameCounter(HubFdo);
    t2.QuadPart = KeQueryInterruptTime();
    LOG(HubFdo, LOG_HUB, 'wtT2', t2.LowPart, t2.HighPart);

    tx.QuadPart = t2.QuadPart-t1.QuadPart;
    LOG(HubFdo, LOG_HUB, 'WTEx', nts, tx.LowPart);
    // compare tmoLg to tx to get the resume time
    // tx = the actual resume time
    // tmoLg = the indicated timeout time

    // if this check fails the we did not reset the event after we timed out or the math is wrong somehow
    if (nts == STATUS_TIMEOUT && tx.QuadPart < tmoLg.QuadPart) {
        LOG(HubFdo, LOG_HUB, 'WT1!', tx.HighPart, tx.LowPart);
        LOG(HubFdo, LOG_HUB, 'WT2!', tmoLg.HighPart, tmoLg.LowPart);
        TEST_TRAP();
    }

    if ( (nts == STATUS_TIMEOUT) && (TimeoutCode != Timeout_Not_Fatal) && (TimeoutCode != Timeout_Not_Fatal_Process_Queue) ) {
        //
        // This is a fatal timeout. But before we declare fatal time out
        // Let's just check one final time if the event has been set.
        // This can happen in cases like virtual machines where the system
        // time can jump
        //
        nts = KeWaitForSingleObject(Event,
                                    Executive,
                                    KernelMode,
                                    FALSE,
                                    &zero_timeout);
        if ( nts != STATUS_SUCCESS ) {
            UsbhTrapFatalTimeout_x9f(HubFdo, TimeoutCode, pd);
        }


    }

    return nts;
}


NTSTATUS
UsbhWaitEventWithTimeout(
    __in PDEVICE_OBJECT HubFdo,
    __inout PKEVENT Event,
    __in LONG Milliseconds,
    __in ULONG LogSig
    )
/*++

Routine Description:

   see UsbhWaitEventWithTimeoutEx

--*/
{
    NTSTATUS nts;

    nts = UsbhWaitEventWithTimeoutEx(HubFdo,
                                     Event,
                                     Milliseconds,
                                     LogSig,
                                     Timeout_Not_Fatal,
                                     NULL);


    return nts;
}




VOID
UsbhRawWait(
    ULONG Milliseconds
    )
/*++

Routine Description:

   blocks the calling thread the specified number of milliseconds.
   If Milliseconds is zero we don't wait

Arguments:

    ms to wait

Return Value:

    none
--*/
{
    LARGE_INTEGER waitTime, currentTime, endTime, frequency;

    if (Milliseconds == 0) {
        return;
    }

    UsbhAssertPassive();

    // Use high resolution timer if it is available
    if (HubG.GlobalUseEnhancedTimer == 1) {
        if (UsbhHighResWait(Milliseconds)) {
            return;
        }
    }

    //
    // Compute end of wait
    //
    currentTime = KeQueryPerformanceCounter(&frequency);
    endTime = RtlExtendedIntegerMultiply(frequency, Milliseconds);
    endTime = RtlExtendedLargeIntegerDivide(endTime, 1000, NULL);
    endTime = RtlLargeIntegerAdd(currentTime, endTime);

    //
    // Convert wait time from milliseconds to 100-nanoseconds relative time
    //
    waitTime = RtlConvertLongToLargeInteger(Milliseconds * -10000);

    do {
        KeDelayExecutionThread(KernelMode, FALSE, &waitTime);

        //
        // See if the end of wait has elapsed
        //
        currentTime = KeQueryPerformanceCounter(&frequency);

        if (RtlLargeIntegerGreaterThanOrEqualTo(currentTime, endTime)) {
            break;
        }

        //
        // KeDelayExecutionThread returned early.  Compute remaining time in the wait and
        // delay execution again.
        //
        waitTime = RtlLargeIntegerSubtract(endTime, currentTime);
        frequency = RtlExtendedLargeIntegerDivide(frequency, 1000, NULL);
        waitTime = RtlLargeIntegerDivide(waitTime,frequency,NULL);

        if (RtlLargeIntegerEqualToZero(waitTime)) {
            //
            // Less than 1ms left in wait; wait at least 1 more millsecond
            //
            waitTime = RtlConvertLongToLargeInteger(-10000);
        } else {
            //
            // Convert remaining time to 100-nanosecond relative time
            //
            waitTime = RtlExtendedIntegerMultiply(waitTime, -10000);
        }

    } WHILE (1);
}


VOID
UsbhWait(
    PDEVICE_OBJECT HubFdo,
    ULONG Milliseconds
    )
/*++

Routine Description:

   blocks the calling thread the specified number of milliseconds.
   If Milliseconds is zero we don't wait

Arguments:

    ms to wait

Return Value:

    none
--*/
{
    LOG(HubFdo, LOG_HUB, 'wait', Milliseconds, 0);

    UsbhRawWait(Milliseconds);
}


NTSTATUS
UsbhTrapBadIrp(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )
 /*++

Routine Description:

    Traps on an irps that we should never see.

    Thrown when the hub receives an irp it should not (i.e. it did not register
    the irp in the driverobject IRP_MJ_ dispatch table).  For this to happen the
    system must have been corrupted somehow.
    Arguments:

    bugcheck parameters:

    Parameter3 - DeviceObject
    Parameter4 - Irp

Return Value:

    ntStatus
--*/
{
#if DBG
    UsbhKdPrint((0, "UsbhTrapBadIrp devobj %x BAD IRP:%x\n", DeviceObject, Irp));
    NT_ASSERTMSG("Bad Irp",FALSE);
#endif

    if(DeviceObject)
    {
        USBHUB_TriggerCallBacks(DeviceObject);
    }

    KeBugCheckEx(BUGCODE_USB_DRIVER,
                 USBBUGCODE_RESERVED_USBHUB, //parm1 = hub
                 USBHUB_BAD_IRP,
                 (ULONG_PTR) DeviceObject,
                 (ULONG_PTR) Irp);

    // Unreachable code
}

#define UsbhTrapFatalMsg "UsbhTrapFatal"

__declspec(noreturn) void
UsbhTrapFatal_Dbg(
    __in_opt PDEVICE_OBJECT DeviceObject,
    __in_opt PVOID Context
    )
 /*++

Routine Description:

    see UsbhFdoTrapFatal

Arguments:

    Context - optional.

Return Value:

    ntStatus
--*/
{

    UsbhKdPrint((0, "UsbhTrapFatal FATAL ERROR context:%x\n", Context));

    UsbhKdPrint((0, ">we bugcheck here<\n"));

    if(DeviceObject)
    {
        USBHUB_TriggerCallBacks(DeviceObject);
    }
    // only bugcheck if we don't have debug bits
    // throw bugcheck for OCA data collection
    KeBugCheckEx(BUGCODE_USB_DRIVER,
                 USBBUGCODE_RESERVED_USBHUB,
                 USBHUB_PDO_TRAP_FATAL,
                 (ULONG_PTR) Context,
                 (ULONG_PTR) NULL);


}


#define UsbhFdoTrapFatalMsg  "UsbhFdoTrapFatal"

__declspec(noreturn) void
UsbhPdoTrapFatal_Dbg(
    __in_opt PDEVICE_OBJECT Pdo,
    __in_opt PVOID Context
    )
 /*++

Routine Description:

    see UsbhFdoTrapFatal

Arguments:

    Context - option pointer passed to bugcheck function

Return Value:

    ntStatus
--*/
{

    UsbhKdPrint((0, "UsbhPdoTrapFatal FATAL ERROR pdo devobj:%x context:%x\n",
        Pdo, Context));

    UsbhKdPrint((0, ">we bugcheck here<"));

    if(Pdo)
    {
        USBHUB_TriggerCallBacks(Pdo);
    }
    // only bugcheck if we don't have debug bits
    // throw bugcheck for OCA data collection
    KeBugCheckEx(BUGCODE_USB_DRIVER,
                 USBBUGCODE_RESERVED_USBHUB,
                 USBHUB_PDO_TRAP_FATAL,
                 (ULONG_PTR) Pdo,
                 (ULONG_PTR) Context);


}

__drv_aliasesMem
PDEVICE_EXTENSION_HUB
FdoExt(
    PDEVICE_OBJECT DeviceObject
    )
{
    PDEVICE_EXTENSION_HUB hubExt;

    UsbhAssert(NULL, DeviceObject != NULL);

    if (DeviceObject == NULL) {
        UsbhTrapFatal(DeviceObject,DeviceObject);
    } else {
        hubExt = DeviceObject->DeviceExtension;

        if (hubExt == NULL) {
            UsbhTrapFatal(DeviceObject,hubExt);
        }

        UsbhAssert(NULL, hubExt->ExtensionType == EXTENSION_TYPE_HUB);
        if (hubExt->ExtensionType != EXTENSION_TYPE_HUB) {
            UsbhTrapFatal(DeviceObject,hubExt);
        }

        return hubExt;
    }
}

__drv_aliasesMem
PDEVICE_EXTENSION_PDO
PdoExt(
    PDEVICE_OBJECT DeviceObject
    )
{
    PDEVICE_EXTENSION_PDO pdoExt;

    UsbhAssert(NULL, DeviceObject != NULL);

    if (DeviceObject == NULL) {
        UsbhTrapFatal(DeviceObject,DeviceObject);
    } else {
        pdoExt = (PDEVICE_EXTENSION_PDO) DeviceObject->DeviceExtension;

        if (pdoExt == NULL) {
            UsbhTrapFatal(DeviceObject,pdoExt);
        }

        ASSERT_PDOEXT(pdoExt);
        if (pdoExt->ExtensionType != EXTENSION_TYPE_PDO) {
            UsbhTrapFatal(DeviceObject,pdoExt);
        }

        return pdoExt;
    }
}


#if DBG

ULONG
_cdecl
UsbhKdPrintX(
    __in ULONG Level,
    PCSTR Format,
    ...
    )
/*++

Routine Description:

    Debug Print function.


Arguments:

Return Value:


--*/
{
    va_list list;
    int i;
    int arg[6];

    if (HubG.GlobalDebugLevel >= Level) {
        DbgPrint("USBHUB.SYS: ");
        va_start(list, Format);
        for (i=0; i<6; i++)
            arg[i] = va_arg(list, int);

        DbgPrint((PCH)Format, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
    }

    return 0;
}

#endif


PHUB_PORT_DATA
UsbhGetPortData(
    PDEVICE_OBJECT HubFdo,
    USHORT Port
    )
/*++

Routine Description:

    returns a pointer to the port data for a port, if port
    number is invalid NULL is returned

Arguments:

Return Value:

   pointer to port data

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;

    LOG(HubFdo, LOG_HUB, 'gpDT', Port, 0);

    UsbhAssert(HubFdo, Port != 0);
    if (Port == 0) {
        return NULL;
    }

    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, Port <= N_PORTS(HubFdo));
    if (Port > N_PORTS(HubFdo)) {
        return NULL;
    }

    if(!hubExt->PortData){
        //
        // Allocation failed, or we failed in AddDevice before we alloc
        //
        return NULL;
    }

    pd = hubExt->PortData;
    pd+=(Port-1);

    LOG(HubFdo, LOG_HUB, 'gpD>', Port, pd);
    UsbhAssert(HubFdo, pd->Sig == SIG_HUBPORTDATA);

    return pd;
}


USHORT
UsbhNumberOfPorts(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    returns the number of ports for a hub based on the
    saved hub descriptor this will be 0 if error.

Arguments:

Return Value:

   number of ports on the hub

--*/

{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, hubExt->FdoFlags.HubDescriptorValid);
    // it is not legitimate to have 0 ports, but may return zero if an error
    // occurred durning init.
    //UsbhAssert(HubFdo, hubExt->HubDescriptor.bNumberOfPorts);
    return hubExt->HubDescriptor.bNumberOfPorts;

}


VOID
UsbhDisconnectPdoFromHub(
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Detaches a PDO to its' parent hub.

    This code maintains the backward attachment that PnP does
    not.  Child_PDO->Parent_Fdo

    The connection to the parent is broken and the parent hub is
    allowed to complete the remove.
    This is to handle the case where the parent is removed and
    before the children.

    This is called when the PDO processes a REMOVE or surprise
    remove

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_OBJECT hubFdo;

    hubFdo = PdoExt(Pdo)->HubFdo;
    // we should have a reference if we are removing it
    // don't pass it as a parameter to the assert though
    UsbhAssert(NULL, hubFdo != NULL);
    hubExt = FdoExt(hubFdo);

    UsbhAssert(NULL, PdoExt(Pdo)->CleanupFlags.DisconnectFromHub);
    LOG(hubFdo, LOG_HUB, 'PDO-', Pdo, hubFdo);
    // lose our reference to the hub
    PdoExt(Pdo)->HubFdo = NULL;
    //UsbhReferenceListRemove(hubFdo, Pdo);

}


NTSTATUS
UsbhConnectPdoToHub(
    PDEVICE_OBJECT Pdo,
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Attaches a PDO to its' parent hub.

    This code maintains the backward attachment that PnP does
    not.  Child_PDO->Parent_Fdo

    The hub driver initializes an event in the FDO extension.
    This event is signaled when all references to the hub are
    gone ie all Pdos has been 'detached' from the parent

    This is called by the hub driver when it creates the PDO.

    ** A side effect of this code is that it allows us to track
    ** down any orphaned PDOs by looking at the PDO ref list.

Arguments:

Return Value:

    success if we can connect

    PDO->FDO

--*/
{
//    NTSTATUS nts;

    PdoExt(Pdo)->HubFdo = NULL;

    //nts = UsbhReferenceListAdd(HubFdo, Pdo, PDOt);

    LOG(HubFdo, LOG_HUB, 'PDO+', Pdo, 0);

//    if (NT_SUCCESS(nts)) {
    PdoExt(Pdo)->CleanupFlags.DisconnectFromHub = 1;
    PdoExt(Pdo)->HubFdo = HubFdo;
//    }

//    return nts;
    return STATUS_SUCCESS;
}



NTSTATUS
UsbhSyncSendCommand(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PUCHAR Buffer,
    PUSHORT BufferLength,
    ULONG MillisecondTimeout,
    USBD_STATUS *ReturnUsbdStatus
    )
/*++

Routine Description:

    Synchronously sends a command on the hub control pipe with an
    option timeout.  This function uses a new generic control interface
    in USBDI to send the 8 setup byte packet.

Arguments:

    HubFdo -

    SetupPacket - 8 byte control packet

    Buffer - buffer, may be NULL

    BufferLength - ptr to buffer length, in = size of buffer, out = bytes
            returned

    MillisecondTimeout - option timeout 0 = no timeout

    UsbdStatus - ptr to return usb status of the operation

Return Value:

    status

--*/
{
    NTSTATUS nts;
    PURB urb;
    KEVENT event = {0};
    PIRP irp;
    IO_STATUS_BLOCK ioStatus;
    PDEVICE_EXTENSION_HUB hubExt;
    PIO_STACK_LOCATION nextStk;
    ULONG ps = 0;

    hubExt = FdoExt(HubFdo);

    // check the status on our upstream port before issuing the request
    nts = UsbhFdoCheckUpstreamConnectionState(HubFdo, &ps);

    if (NT_ERROR(nts)) {
        // don't bother to issue the request if the hub is disconnected
        // or the port is disabled
        SET_OPTIONAL_STATUS(ReturnUsbdStatus, USBD_STATUS_DEVICE_GONE);
        return nts;
    }


    UsbhAllocatePool_Z(urb,
                       NonPagedPool,
                       sizeof(struct _URB_CONTROL_TRANSFER_EX));

    LOG(HubFdo, LOG_HUB, 'ssc0', urb, 0);

    if (urb == NULL) {
        SET_OPTIONAL_STATUS(ReturnUsbdStatus,
            USBD_STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    INIT_EVENT_NOT_SIGNALED(&event);

    irp = IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_USB_SUBMIT_URB,
                                        hubExt->TopOfStackDeviceObject,
                                        NULL,
                                        0,
                                        NULL,
                                        0,
                                        TRUE,  // INTERNAL
                                        &event,
                                        &ioStatus);

    LOG(HubFdo, LOG_HUB, 'ssc1', irp, &ioStatus);

    if (irp == NULL) {
        UsbhFreePool(urb);
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    // null out device handle in case we are the root hub
    urb->UrbHeader.UsbdDeviceHandle = NULL;

    // set up the control transfer
    urb->UrbControlTransferEx.Hdr.Length =
        sizeof(struct _URB_CONTROL_TRANSFER_EX);
    urb->UrbControlTransferEx.Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER_EX;

    urb->UrbControlTransferEx.TransferFlags = (USBD_DEFAULT_PIPE_TRANSFER |
                                                USBD_SHORT_TRANSFER_OK);
    if (SetupPacket->bmRequestType.Dir ==  BMREQUEST_DEVICE_TO_HOST) {
        urb->UrbControlTransferEx.TransferFlags |= USBD_TRANSFER_DIRECTION_IN;
    }

    urb->UrbControlTransferEx.TransferBufferLength = *BufferLength;
    urb->UrbControlTransferEx.TransferBuffer = Buffer;
    urb->UrbControlTransferEx.TransferBufferMDL = NULL;
    urb->UrbControlTransferEx.Timeout = MillisecondTimeout;
    RtlCopyMemory(&urb->UrbControlTransferEx.SetupPacket,
                  SetupPacket,
                  sizeof(*SetupPacket));

    DbgTrace((UsbhDebugTrace,
        "%!FUNC! setup packet @ %p\nbmRequestType 0x%02.2x\nbRequest 0x%02.2x\nwValue  LowByte 0x%02.2x  HiByte 0x%02.2x\n  (0x%04.4x)\nwIndex  LowByte 0x%02.2x  HiByte 0x%02.2x\n  (0x%04.4x)\nwLength 0x%04.4x\n",
        SetupPacket,
        SetupPacket->bmRequestType.B,
        SetupPacket->bRequest,
        SetupPacket->wValue.LowByte,  SetupPacket->wValue.HiByte, SetupPacket->wValue.W,
        SetupPacket->wIndex.LowByte,  SetupPacket->wIndex.HiByte, SetupPacket->wIndex.W,
        SetupPacket->wLength));

    nextStk = IoGetNextIrpStackLocation(irp);
    nextStk->Parameters.Others.Argument1 = urb;

    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, irp);

    if (nts == STATUS_PENDING) {
        WAIT_EVENT(&event);
    } else {
        ioStatus.Status = nts;
    }

    nts = ioStatus.Status;
    LOG(HubFdo, LOG_HUB, 'ssc2', nts, urb->UrbControlTransferEx.Hdr.Status);


    if (urb->UrbControlTransferEx.Hdr.Status != USBD_STATUS_SUCCESS &&
        NT_SUCCESS(nts)) {
        // this would indicate a bug in the bus driver
        UsbhAssertMsg(HubFdo, "Mismatched USBDSTATUS and NTSTATUS", FALSE);
    }

    // return the length and URB status
    SET_OPTIONAL_STATUS(ReturnUsbdStatus, urb->UrbControlTransferEx.Hdr.Status);
    if (BufferLength) {
        *BufferLength = (USHORT)urb->UrbControlTransferEx.TransferBufferLength;
    }

    UsbhFreePool(urb);

    LOG(HubFdo, LOG_HUB, 'ssc>', nts, 0);

    return nts;
}


// This code is normally never compliled.
// set USER_C_FLAGS=-DUSBHUB_BUGCHECK_TEST
// and then build a private binary to enable this test code.
// Manually set UsbhGenDispatchBugcheck in the debugger to cause
// USBHUB to bugcheck.
//
#ifdef USBHUB_BUGCHECK_TEST
ULONG UsbhGenDispatchBugcheck = 0;
#endif

NTSTATUS
UsbhGenDispatch(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    Dispatch routine for all Irps, we branch out to the appropriate handler
    based on if the Irp was sent to FDO or PDO

    This routine handles Irps to both the PDO and the FDO.

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HEADER devExt;
    PIO_STACK_LOCATION ioStack;

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    devExt = (PDEVICE_EXTENSION_HEADER) DeviceObject->DeviceExtension;

#ifdef USBHUB_BUGCHECK_TEST
    if (UsbhGenDispatchBugcheck) {
        UsbhTrapBadIrp(DeviceObject, Irp);
    }
#endif

    switch(devExt->ExtensionType) {
    case EXTENSION_TYPE_HUB:
        if (ioStack->MajorFunction <= IRP_MJ_MAXIMUM_FUNCTION) {
            nts = HubFdoGenDispatch[ioStack->MajorFunction].
                    GenHandler(DeviceObject, Irp);
        } else {
            // we don't get here unless there is a bug, the driverObject
            // dispatch table has functions for all the possible IRPs we
            // can support.
            nts = STATUS_INVALID_PARAMETER;
            UsbhTrapBadIrp(DeviceObject, Irp);

            // Unreachable code
            //UsbhCompleteIrp(Irp, nts);
        }
        break;

    case EXTENSION_TYPE_PDO:
        if (ioStack->MajorFunction <= IRP_MJ_MAXIMUM_FUNCTION) {
            nts = HubPdoGenDispatch[ioStack->MajorFunction].
                    GenHandler(DeviceObject, Irp);
        } else {
            // we don't get here unless there is a bug, the driverobject
            // dispatch table has functions for all the possible IRPs we
            // can support.
            nts = STATUS_INVALID_PARAMETER;
            UsbhTrapBadIrp(DeviceObject, Irp);

            // Unreachable code
            //UsbhCompleteIrp(Irp, nts);
        }
        break;

    default:
        // not or our extension corrupted
        nts = STATUS_INVALID_PARAMETER;
        UsbhCompleteIrp(Irp, nts);
    }

    return nts;
}


NTSTATUS
UsbhFdoSuccess(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    completes an Irp with STATUS_SUCCESS

Arguments:

Return Value:

    ntStatus
--*/
{
    LOG(HubFdo, LOG_HUB, 'Fscc', 0, Irp);

    UsbhCompleteIrp(Irp, STATUS_SUCCESS);

    return STATUS_SUCCESS;
}


NTSTATUS
UsbhFdoInternalDeviceControl(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
/*++

Routine Description:

    Handles our internal IOCTL Apis.

    These are IOCTLs sent to or thru the hubs FDO. An ioctl sent to the hub PDO will pass thru this function.
    These are typically messages propugated thru the stack from the port driver to hubs or hubs to other hubs.

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    ULONG ioControlCode;
    PDEVICE_EXTENSION_HUB hubExt;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, ioStack->MajorFunction == IRP_MJ_INTERNAL_DEVICE_CONTROL);

    ioControlCode = ioStack->Parameters.DeviceIoControl.IoControlCode;

    switch (ioControlCode) {
    case IOCTL_INTERNAL_USB_REQ_GLOBAL_SUSPEND:
    case IOCTL_INTERNAL_USB_REQ_GLOBAL_RESUME:
        UsbhCompleteIrp(Irp, STATUS_NOT_SUPPORTED);
        return STATUS_NOT_SUPPORTED;

    case IOCTL_INTERNAL_USB_PROPAGATE_RESUME:
        return UsbhInternalIoctlPropagateResume(HubFdo, Irp, ioStack);

    case IOCTL_INTERNAL_USB_NOTIFY_HUB_PWR_LOSS:
        hubExt = FdoExt(HubFdo);
        hubExt->HubVbusState = Vbus_Interrupted;
        UsbhCompleteIrp(Irp, STATUS_SUCCESS);
        return STATUS_SUCCESS;

    default:
        nts = UsbhFdoChainIrp(HubFdo, Irp);

    }

    return nts;
}


NTSTATUS
UsbhFdoDeviceControl(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
/*++

Routine Description:

    Handles our USER mode IOCTL Apis.  These are APIs that allow user mode
    applications to query the state of the bus and walk the device tree. The
    most famous being USBVIEW.

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    ULONG ioControlCode;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, ioStack->MajorFunction == IRP_MJ_DEVICE_CONTROL);

    ioControlCode = ioStack->Parameters.DeviceIoControl.IoControlCode;

    LOG(HubFdo, LOG_HUB, 'FDio', (ioControlCode>>2)&0xfff, Irp);

    if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
        nts = STATUS_INVALID_PARAMETER;
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, GenErr_UserIoctlFailed_InvalidIrql,
                         nts, (ioControlCode>>2)&0xfff, NULL, 0);
        UsbhCompleteIrp(Irp, nts);

        return nts;
    }

    // All IOCTLs will trigger resume from selective suspend, this is a precaution to  maintain compatibility
    // with the older version of the driver.  Some of the IOCTLs will also mark the hub busy during the duration of
    // the API to ensure we do not attempt suspend until the API is finished.
    Usbh_SSH_Event(HubFdo, SSH_Event_ResumeHub, API_CONTEXT(HubFdo));

    switch (ioControlCode) {
    case IOCTL_USB_GET_NODE_INFORMATION:
        return UsbhIoctlGetNodeInformation(HubFdo, Irp, ioStack);

    case IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME:
        return UsbhIoctlGetNodeConnectionDriverKeyName(HubFdo, Irp, ioStack);

    case IOCTL_USB_GET_NODE_CONNECTION_INFORMATION:
        return UsbhIoctlGetNodeConnectionInfo(HubFdo, Irp, ioStack);

    case IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX:
        return UsbhIoctlGetNodeConnectionInfoEx(HubFdo, Irp, ioStack);

    case IOCTL_USB_GET_NODE_CONNECTION_ATTRIBUTES:
        return UsbhIoctlGetNodeConnectionAttributes(HubFdo, Irp, ioStack);

    case IOCTL_USB_GET_NODE_CONNECTION_NAME:
        return UsbhIoctlGetNodeConnectionName(HubFdo, Irp, ioStack);

    case IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION:
        return UsbhIoctlGetDescriptorFromNodeConnection(HubFdo, Irp, ioStack);

    case IOCTL_USB_GET_HUB_CAPABILITIES:
        return UsbhIoctlGetHubCaps(HubFdo, Irp, ioStack);

    case IOCTL_USB_GET_HUB_CAPABILITIES_EX:
        return UsbhIoctlGetHubCapsEx(HubFdo, Irp, ioStack);

    case IOCTL_USB_GET_HUB_INFORMATION_EX:
        return UsbhIoctlGetHubInformationEx(HubFdo, Irp, ioStack);

    case IOCTL_USB_GET_PORT_CONNECTOR_PROPERTIES:
        return UsbhIoctlGetPortConnectorProperties(HubFdo, Irp, ioStack);

    case IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX_V2:
        return UsbhIoctlGetNodeConnectionInfoExV2(HubFdo, Irp, ioStack);

    case IOCTL_KS_PROPERTY:
        nts = STATUS_INVALID_DEVICE_REQUEST;
        UsbhCompleteIrp(Irp, nts);
        return nts;

    case IOCTL_USB_HUB_CYCLE_PORT:
        return UsbhIoctlCyclePort(HubFdo, Irp, ioStack);

    case IOCTL_USB_RESET_HUB:
        return UsbhIoctlResetStuckHub(HubFdo, Irp, ioStack);

    default:
        nts = UsbhFdoChainIrp(HubFdo, Irp);

    }

    return nts;
}


NTSTATUS
UsbhSyncSendDeviceIoctl(
    PDEVICE_OBJECT HubFdo,
    ULONG Ioctl,
    PVOID IoBuffer,
    ULONG IoBufferLength,
    BOOLEAN Internal
    )
/*++

Routine Description:

    Sends a standard ioctl request to the to-of-stack -- the hubs PDO

Arguments:

IRQL: Passive

Return Value:

    nt status
--*/
{
    NTSTATUS nts;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIRP irp;
    PDEVICE_EXTENSION_HUB hubExt;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_HUB, 'ctl1', Ioctl, Internal);
    LOG(HubFdo, LOG_HUB, 'ctl2', IoBuffer, IoBufferLength);

    INIT_EVENT_NOT_SIGNALED(&event);

    irp = IoBuildDeviceIoControlRequest(Ioctl,
                                        hubExt->TopOfStackDeviceObject,
                                        IoBuffer,
                                        IoBufferLength,
                                        IoBuffer,
                                        IoBufferLength,
                                        Internal,
                                        &event,
                                        &ioStatus);

    if (NULL == irp) {
        LOG(HubFdo, LOG_HUB, 'ctl!', Ioctl, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    LOG(HubFdo, LOG_HUB, 'ctlI', irp, 0);
    // track this request -- we don't want to unload out from under it

    nts = REF_HUB(HubFdo, irp, syIO);
    if (NT_SUCCESS(nts)) {
        nts = IoCallDriver(hubExt->TopOfStackDeviceObject, irp);

        if (nts == STATUS_PENDING) {
            WAIT_EVENT(&event);

            nts = ioStatus.Status;
        }
        LOG(HubFdo, LOG_HUB, 'ctl3', irp, nts);

        DEREF_HUB(HubFdo, irp);
    } else {
        LOG(HubFdo, LOG_HUB, 'ctl4', irp, nts);
        IoCompleteRequest(irp, IO_NO_INCREMENT);
    }

    LOG(HubFdo, LOG_HUB, 'Sctl', Ioctl, nts);
    return nts;
}



NTSTATUS
UsbhSyncSendInternalIoctl(
    PDEVICE_OBJECT HubFdo,
    ULONG Ioctl,
    PVOID Argument1,
    PVOID Argument2
    )
 /*++

Routine Description:

    Sends an internal IOCTL request down the stack start with the top of our
    PDO stack ie the hubs PDO

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    PIRP irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION ioStack;
    PDEVICE_EXTENSION_HUB hubExt;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    INIT_EVENT_NOT_SIGNALED(&event);

    irp = IoBuildDeviceIoControlRequest( Ioctl,
                                         hubExt->TopOfStackDeviceObject,
                                         NULL,
                                         0,
                                         NULL,
                                         0,
                                         TRUE,  // INTERNAL
                                         &event,
                                         &ioStatus);

    if (NULL == irp) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    ioStack = IoGetNextIrpStackLocation(irp);
    ioStack->Parameters.Others.Argument1 = Argument1;
    ioStack->Parameters.Others.Argument2 = Argument2;

    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, irp);

    if (nts == STATUS_PENDING) {

        WAIT_EVENT(&event);
        // status is in the irp
        nts = ioStatus.Status;
    }

    return nts;
}



NTSTATUS
UsbhGetControllerName(
    PDEVICE_OBJECT HubFdo,
    PUSB_HUB_NAME HubNameBuffer,
    ULONG HubNameBufferLength
    )
/*++

Routine Description:

    Fetches the controller symbolic name through the USBD bus interface

This function returns the controllers Unicode symbolic device name.

Parameters:


A pointer to a structure to receive the symbolic name.  ActualLength is set
to the size of the controller name even if there is not enough room to return
the whole name.

typedef struct _USB_HUB_NAME {
   ULONG ActualLength;
   WCHAR HubName[1];
} USB_HUB_NAME, *PUSB_HUB_NAME;

ioStackLocation->Parameters.Others.Argument2
The size of the buffer passed in Argument1 in bytes

IRQL: Any

Return Value:

    nt status code

--*/
{
    PUSB_BUS_INFORMATION_LEVEL_1 level_1 = NULL;
    ULONG lenToCopy, level_1_siz, actualLength;
    NTSTATUS nts;

    actualLength = 0;

    level_1_siz = sizeof(USB_BUS_INFORMATION_LEVEL_1);
    UsbhAllocatePool_Z(level_1, PagedPool, level_1_siz);

    nts = STATUS_INSUFFICIENT_RESOURCES;
    if (level_1 != NULL) {

        USBPORT_USBD_BUSIF_QUERY_BUS_INFO(HubFdo,
                                          nts,
                                          1,
                                          level_1,
                                          &level_1_siz,
                                          &actualLength);

        if (nts == STATUS_BUFFER_TOO_SMALL) {
            // ask again with the actual length returned
            level_1_siz = actualLength;
            UsbhFreePool(level_1);
            UsbhAllocatePool_Z(level_1, PagedPool, level_1_siz);

            if (level_1 != NULL) {
                USBPORT_USBD_BUSIF_QUERY_BUS_INFO(HubFdo,
                                              nts,
                                              1,
                                              level_1,
                                              &level_1_siz,
                                              &actualLength);

            }
        }
    }

    if (NT_SUCCESS(nts)) {

        // extra codepath here is to keep prefast happy
        UsbhAssert(HubFdo, level_1);

        if (level_1) {

            // not sure if BufferLength includes size of the
            // ActualLength field, we will assume it does
            if (HubNameBufferLength >= sizeof(HubNameBuffer->ActualLength)) {
                HubNameBuffer->ActualLength = level_1->ControllerNameLength;

                if ((HubNameBufferLength - sizeof(HubNameBuffer->ActualLength))
                    >= level_1->ControllerNameLength) {
                    lenToCopy = level_1->ControllerNameLength;
                } else {
                    lenToCopy =
                        HubNameBufferLength - sizeof(HubNameBuffer->ActualLength);
                }

                // copy what we can
                RtlCopyMemory(&HubNameBuffer->HubName[0],
                              &level_1->ControllerNameUnicodeString[0],
                              lenToCopy);
            } else {
                nts = STATUS_BUFFER_TOO_SMALL;
            }
        } else {
            nts = STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    UsbhFreePool(level_1);

    return nts;
}


NTSTATUS
UsbhGetDeviceBusInfo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_BUS_NOTIFICATION UserBuffer,
    ULONG UserBufferLength
    )
/*++

Routine Description:

  Fetches the PDO relative bus information through the USB bus interface

typedef struct _USB_BUS_NOTIFICATION {
    USB_NOTIFICATION_TYPE NotificationType;
    ULONG TotalBandwidth;
    ULONG ConsumedBandwidth;
    ULONG ControllerNameLength;
} USB_BUS_NOTIFICATION, *PUSB_BUS_NOTIFICATION;

Parameters:

   HubFdo - hub the device is attached to

   Pdo - pdo associated with the device, if NULL the info relative to the hub
   device is returned


IRQL: Any

Return Value:

    nt status code

--*/
{
    PUSB_BUS_INFORMATION_LEVEL_1 level_1 = NULL;
    ULONG level_1_siz, actualLength;
    NTSTATUS nts;
    PVOID deviceBusContext = NULL;
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_HUB, 'gdbi', UserBufferLength, UserBuffer);
    DbgTrace((UsbhDebugTrace,">%!FUNC!  UserBufferLength = %d\n", UserBufferLength));

    if (UserBufferLength < sizeof(struct _USB_BUS_NOTIFICATION)) {
        LOG(HubFdo, LOG_HUB, 'gdbE', UserBufferLength, UserBuffer);
        DbgTrace((UsbhDebugTrace,"<%!FUNC!  STATUS_BUFFER_TOO_SMALL\n"));
        return STATUS_BUFFER_TOO_SMALL;
    }
// bugbug is this pdo properly latched?
    if (Pdo) {
        pdoExt = PdoExt(Pdo);

        nts = Usbh_HubGetDeviceBusContext(HubFdo,
                                          pdoExt->DeviceHandle,
                                          &deviceBusContext);

        LOG(HubFdo, LOG_HUB, 'gdb1', nts, deviceBusContext);
        DbgTrace((UsbhDebugTrace,">%!FUNC!  Usbh_HubGetDeviceBusContext %!STATUS! %p\n",
            nts, deviceBusContext));


    } else {
        PUSB_BUS_INTERFACE bus = &hubExt->UsbdBusInterface;

        LOG(HubFdo, LOG_HUB, 'gdb2', 0, 0);
        DbgTrace((UsbhDebugTrace,">%!FUNC! deviceBusContext = bus->BusContext\n"));
        deviceBusContext = bus->BusContext;
        Usbh_UsbdInterfaceReference(HubFdo, deviceBusContext);

        nts = STATUS_SUCCESS;
    }


    if (NT_SUCCESS(nts)) {

        LOG(HubFdo, LOG_HUB, 'gdb3', 0, 0);
        level_1_siz = sizeof(struct _USB_BUS_INFORMATION_LEVEL_1);

        do {
            UsbhAllocatePool_Z(level_1, PagedPool, level_1_siz);

            if (level_1 != NULL) {
                // init to zero (prefix)
                actualLength = 0;
                USBPORT_USBD_BUSIF_DEVICE_QUERY_BUS_INFO(HubFdo,
                                                  deviceBusContext,
                                                  nts,
                                                  1,
                                                  level_1,
                                                  &level_1_siz,
                                                  &actualLength);

                LOG(HubFdo, LOG_HUB, 'gdb4', level_1, actualLength);
                if (nts == STATUS_BUFFER_TOO_SMALL) {
                    // ask again with the actual length returned
                    level_1_siz = actualLength;
                    LOG(HubFdo, LOG_HUB, 'gdb5', level_1_siz, nts);
                    UsbhFreePool(level_1);
                }
            } else {
                nts = STATUS_INSUFFICIENT_RESOURCES;
            }

        } while (nts == STATUS_BUFFER_TOO_SMALL);

        Usbh_UsbdInterfaceDereference(HubFdo, deviceBusContext);
    }

    if (NT_SUCCESS(nts) && level_1) {

        UserBuffer->TotalBandwidth = level_1->TotalBandwidth;
        UserBuffer->ConsumedBandwidth = level_1->ConsumedBandwidth;

        /* length of the UNICODE symbolic name (in bytes) for the controller
           that this device is attached to.
           not including NULL */
        UserBuffer->ControllerNameLength = level_1->ControllerNameLength;

        LOG(HubFdo, LOG_HUB, 'gdX1', level_1->TotalBandwidth,
                level_1->ConsumedBandwidth);
        LOG(HubFdo, LOG_HUB, 'gdX2', level_1->ControllerNameLength, 0);
        DbgTrace((UsbhDebugTrace,"<%!FUNC! bw = %d(%d), controller name len = %d\n",
                level_1->TotalBandwidth,
                level_1->ConsumedBandwidth,
                level_1->ControllerNameLength));
    }

    if (level_1) {
        UsbhFreePool(level_1);
    }

    DbgTrace((UsbhDebugTrace,"<%!FUNC! %!STATUS!\n", nts));

    return nts;
}


VOID
UsbhOutOfBandwidthTimerDpc(
    PKDPC Dpc,
    PVOID DeferredContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2
    )
 /*++

Routine Description:

    Dpc that fires after an out-of bandwidth error occurs causing us to display
    the bandwidth popup

Arguments:

Return Value:

    none
--*/
{
    PUSB_OOBANDWIDTH_TIMER oobwTimer = DeferredContext;
    PHUB_PORT_DATA pd;
    PUSB_OOBANDWIDTH_TIMER curTmr;

    UsbhAssert(NULL, oobwTimer->Sig == SIG_OOBWOBJ);
    pd = oobwTimer->Pd;
    UsbhAssert(NULL, pd->Sig == SIG_HUBPORTDATA);

    // if this timer is still current then we remove it and throw
    // an exception

    // oldTmr = pd->OutOfBandwidthTimer
    // if (pd->OutOfBandwidthTimer == oobwTimer)
    //     pd->OutOfBandwidthTimer = NULL;
    curTmr = InterlockedCompareExchangePointer(&pd->OutOfBandwidthTimer,
                                               NULL,
                                               oobwTimer);

    LOG(oobwTimer->HubFdo, LOG_HUB, 'ooT1', pd, oobwTimer);
    LOG(oobwTimer->HubFdo, LOG_HUB, 'ooT2', pd, curTmr);
    // if this timer was current and PDO still exists, generate the popup
    if ((oobwTimer == curTmr) && (pd->PnpPdo)) {
        LOG(oobwTimer->HubFdo, LOG_HUB, 'ooT3', pd, curTmr);
        // timer has not been canceled and won't be
        UsbhSetConnectionStatus(oobwTimer->HubFdo, pd, DeviceNotEnoughBandwidth);

        // throw the out-of bandwidth exception
        HUB_EXCEPTION(USBFILE,  oobwTimer->HubFdo, pd->PortNumber,
                      UsbErr_InsufficientBandwidth_Popup,
                      oobwTimer->NtStatus,
                      oobwTimer->UsbdStatus,
                      oobwTimer, sizeof(struct _USB_OOBANDWIDTH_TIMER));


    }
    // release our grip on the FDO
    // OutOfBwTimer_obtm
    UsbhReferenceListRemove(oobwTimer->HubFdo, &oobwTimer->Ktimer);
    // OutOfBwObject_OBWt
    UsbhReferenceListRemove(oobwTimer->HubFdo, oobwTimer);
    UsbhFreePool(oobwTimer);
}


VOID
UsbhCancelOutOfBandwidthTimer(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber
    )
 /*++

Routine Description:

    cancels the out-of-bandwidth popup timer if running for a given port.

    This routine cancels only the 'current timer'

Arguments:

Return Value:

    none
--*/
{
    PUSB_OOBANDWIDTH_TIMER oobwTimer;
    PHUB_PORT_DATA pd;

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (!pd) {
        return;
    }

    // timer will expire without generating a popup
    oobwTimer = InterlockedExchangePointer(&pd->OutOfBandwidthTimer, NULL);
    LOG(HubFdo, LOG_HUB, 'ooTx', pd, oobwTimer);

}


VOID
UsbhSetOutOfBandwidthTimer(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    NTSTATUS NtStatus,
    USBD_STATUS UsbdStatus
    )
 /*++

Routine Description:

    Sets the out-of-bandwidth timer for a given port. If a successful select_config
    or select_interface does not occur a message about bandwidth is displayed

Arguments:

Return Value:

    none
--*/
{
    PUSB_OOBANDWIDTH_TIMER oobwTimer;
    PVOID tmp;
    NTSTATUS nts;
    PHUB_PORT_DATA pd;

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (!pd) {
        return;
    }

    UsbhAllocatePool_Z(oobwTimer, NonPagedPool, sizeof(struct _USB_OOBANDWIDTH_TIMER));

    if (oobwTimer) {
        // reference the hub, if we can't lock the hub in memory if we cannot
        // set the timer
        nts = UsbhReferenceListAdd(HubFdo, oobwTimer, OutOfBwObject_OBWt);

        if (NT_SUCCESS(nts)) {

            NTSTATUS tmr_nts;

            oobwTimer->Sig = SIG_OOBWOBJ;
            oobwTimer->HubFdo = HubFdo;
            oobwTimer->Pd = pd;
            oobwTimer->NtStatus = NtStatus;
            oobwTimer->UsbdStatus = UsbdStatus;

            //init timer
            KeInitializeTimer(&oobwTimer->Ktimer);
            KeInitializeDpc(&oobwTimer->Dpc,
                            UsbhOutOfBandwidthTimerDpc,
                            oobwTimer);


            tmr_nts = UsbhSetTimer(HubFdo,
                         1000,
                         &oobwTimer->Ktimer,
                         &oobwTimer->Dpc,
                         OutOfBwTimer_obtm,
                         TRUE);

            if (NT_ERROR(tmr_nts)) {
                // low resource sim will hit this path, treat the same as error path above
                TEST_TRAP();
                UsbhFreePool(oobwTimer);
            } else {

                // make this incarnation 'current', we may end up with multiple
                // timeouts, only the last one counts
                tmp = InterlockedExchangePointer(&pd->OutOfBandwidthTimer,
                                                 oobwTimer);

                LOG(HubFdo, LOG_HUB, 'ooNT', pd, tmp);
            }

        } else {
            // could not reference the hub
            UsbhFreePool(oobwTimer);
        }
    }
}


ULONG
UsbhCalculateInterfaceBandwidth(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSBD_INTERFACE_INFORMATION UsbInterface
    )
 /*++

Routine Description:

    Calculate bandwidth consumed by a particular interface based on the
    PIPE_INFORMATION (similar endpoint descriptor) and device speed.

    The USPORT bus interface API replaces the function USBD_CalculateBandwidth
    which did not handle high speed.

Arguments:

Return Value:

    bandwidth consumed by the interface in bits/ms
--*/
{
    ULONG i, pipeBw, bandwidth;
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = PdoExt(Pdo);
    bandwidth = 0;

    // we'll need to walk through the interface
    // and figure out how much BW it requires

    for (i=0; i<UsbInterface->NumberOfPipes; i++) {

        pipeBw = Usbh_HubCaculatePipeBandwidth(
            HubFdo,
            &UsbInterface->Pipes[i],
            pdoExt->Speed);

        bandwidth += pipeBw;
    }

    // convert to bits/ms from bits/s
    return bandwidth/1000;

}


NTSTATUS
UsbhSelectConfigOrInterfaceComplete(
    PDEVICE_OBJECT PNull,
    PIRP Irp,
    PVOID Context
    )
 /*++

Routine Description:

    Handles Completion of select_interface and select_config
    requests

    we hold a io ref on the PDO which locks the hub in memory

Arguments:

    standard OS args for a completion routine

Return Value:

    STATUS_SUCCESS.

--*/
{
    PDEVICE_OBJECT pdo = Context;
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    PIO_STACK_LOCATION ioStack;

    pdoExt = PdoExt(pdo);
    hubExt = FdoExt(pdoExt->HubFdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    if (NT_SUCCESS(Irp->IoStatus.Status)) {
        // cancel out-of-bandwidth timer if
        UsbhCancelOutOfBandwidthTimer(pdoExt->HubFdo, pdoExt->PortNumber);

    } else {
        PURB urb;
        PHUB_PORT_DATA pd;

        urb = ioStack->Parameters.Others.Argument1;
        pd = UsbhGetPortData(pdoExt->HubFdo, pdoExt->PortNumber);

        // cancel any previous timer
        UsbhCancelOutOfBandwidthTimer(pdoExt->HubFdo, pdoExt->PortNumber);

        if (pd && urb->UrbHeader.Status == USBD_STATUS_NO_BANDWIDTH) {

            // set the bandwidth error popup timer
            UsbhSetOutOfBandwidthTimer(pdoExt->HubFdo, pdoExt->PortNumber,
                Irp->IoStatus.Status, urb->UrbHeader.Status);

            pd->LastBandwidthRequested = 0;
            // calculate bandwidth requested
            switch (urb->UrbHeader.Function) {
            case URB_FUNCTION_SELECT_INTERFACE:
                pd->LastBandwidthRequested =
                    UsbhCalculateInterfaceBandwidth(
                        pdoExt->HubFdo,
                        pdo,
                        &urb->UrbSelectInterface.Interface);
                break;

            case URB_FUNCTION_SELECT_CONFIGURATION:
                // we need to walk through the config and get the bandwidth
                // for all interfaces
                {
                PUCHAR pch, end;
                PUSBD_INTERFACE_INFORMATION xf;

                end = (PUCHAR) urb;
                end += urb->UrbHeader.Length;
                pch = (PUCHAR) &urb->UrbSelectConfiguration.Interface;
                xf = (PUSBD_INTERFACE_INFORMATION) pch;

                while (pch < end) {
                    pd->LastBandwidthRequested +=
                        UsbhCalculateInterfaceBandwidth(
                            pdoExt->HubFdo,
                            pdo,
                            (PUSBD_INTERFACE_INFORMATION)xf);

                    pch += xf->Length;
                    xf = (PUSBD_INTERFACE_INFORMATION) pch;
                }
                }
                break;

            }
        }

    }

    // regardless of status release the PDO
    UsbhDecPdoIoCount(pdo, Irp);

    // propagate the Pending bit if needed
    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }

    return STATUS_SUCCESS;

}


NTSTATUS
UsbhFdoSelectInterfacePdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

   Filters the select_interface API. Provides the not-enough-bw popup

Arguments:

Return Value:

    nt status code

--*/
{
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    PURB urb;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    urb = ioStack->Parameters.Others.Argument1;

    // hook the completion to catch bandwidth errors
    IoCopyCurrentIrpStackLocationToNext(Irp);

    UsbhSetCompletionRoutine(HubFdo,
                             Irp,
                             UsbhSelectConfigOrInterfaceComplete,
                             Pdo,
                             TRUE,
                             TRUE,
                             TRUE);

    UsbhAssert(HubFdo, hubExt->TopOfBusStack);
    nts = IoCallDriver(hubExt->TopOfBusStack, Irp);
    // don't release the io count on the PDO since we have a completion
    // routine.
    return nts;
}


NTSTATUS
UsbhFdoGetMsDescriptorPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

   Fails the query for the MS OS descriptor if the device does not
   support the thing.

   If the device does support this then we paste in the vendor code
   and pass the request on

Arguments:

Return Value:

    nt status code

--*/
{
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    //PURB urb;
    struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST *urb;
    PVOID UrbTransferBuffer;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    urb = ioStack->Parameters.Others.Argument1;

    if (urb->Index == MSOS20_DESCRIPTOR_INDEX) {

        if (pdoExt->MsOs20Info.Flags.DescriptorSetHeader == 0) {
            urb->Hdr.Status = USBD_STATUS_NOT_SUPPORTED;
            UsbhCompleteIrp(Irp, STATUS_NOT_SUPPORTED);
            UsbhDecPdoIoCount(Pdo, Irp);
            return STATUS_NOT_SUPPORTED;
        }

        if (urb->TransferBufferMDL != NULL) {
            UrbTransferBuffer = MmGetSystemAddressForMdlSafe(urb->TransferBufferMDL,
                                                             NormalPagePriority);
        } else {
            UrbTransferBuffer = urb->TransferBuffer;
        }

        if (UrbTransferBuffer == NULL) {
            nts = STATUS_INVALID_PARAMETER;
            urb->Hdr.Status = USBD_STATUS_INVALID_PARAMETER;
        } else {
            ULONG bytesTransferred;

            bytesTransferred = min(pdoExt->MsOs20Info.DescriptorSet->wTotalLength,
                                   urb->TransferBufferLength);

            RtlCopyMemory(UrbTransferBuffer,
                          pdoExt->MsOs20Info.DescriptorSet,
                          bytesTransferred);

            urb->TransferBufferLength = bytesTransferred;

            nts = STATUS_SUCCESS;
            urb->Hdr.Status = USBD_STATUS_SUCCESS;
        }

        UsbhCompleteIrp(Irp, nts);
        UsbhDecPdoIoCount(Pdo, Irp);

    } else if (pdoExt->DeviceFlags.MsOsSupported) {

        urb->Request = (UCHAR) pdoExt->DeviceFlags.MsOsDescriptorVc;
        // forward directly to the bus driver
        IoSkipCurrentIrpStackLocation(Irp);
        UsbhAssert(HubFdo, hubExt->TopOfBusStack);
        nts = IoCallDriver(hubExt->TopOfBusStack, Irp);
        UsbhDecPdoIoCount(Pdo, Irp);

    } else {
        urb->Hdr.Status = USBD_STATUS_NOT_SUPPORTED;

        UsbhCompleteIrp(Irp, STATUS_NOT_SUPPORTED);
        UsbhDecPdoIoCount(Pdo, Irp);

        nts = STATUS_NOT_SUPPORTED;
    }

    return nts;
}


NTSTATUS
UsbhFdoValidateConnectionPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

   Part of RESET_PIPE filter validates the state of the device connection
   before sending down the RESET_PIPE.  We use this to return a better
   indication of when the device has been disconnected

Arguments:

Return Value:

    nt status code

--*/
{
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    PURB urb;
    USBD_STATUS usbdStatus;
    PHUB_PORT_DATA pd;
    USB_HUB_PORT_STATE portState = {0};

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    pd = UsbhGetPortData(HubFdo, pdoExt->PortNumber);
    LOG(HubFdo, LOG_HUB, 'ckCK', pd, pdoExt->PortNumber);
    ioStack = IoGetCurrentIrpStackLocation(Irp);
    urb = ioStack->Parameters.Others.Argument1;

    if (hubExt->CurrentFdoPnpState != FDO_WaitPnpStop) {
        // Reset pipe requests are only valid when the hub is started
        nts = NT_DEVICE_DISCONNECTED_STATUS;
        usbdStatus = USBD_STATUS_DEVICE_GONE;
    }
    else if (pd == NULL) {
        // skip query if no portdata available
        nts = STATUS_INVALID_PARAMETER;
        usbdStatus = USBD_STATUS_INVALID_PARAMETER;
    } else {

        // check connection status, if device not connected complete the irp here,
        // there is no point in putting the reset transaction on the bus

        nts = UsbhQueryPortState(HubFdo,
                                 pd->PortNumber,
                                 &portState,
                                 &usbdStatus);
    }

    if (NT_ERROR(nts)) {
        // propagate the error
        urb->UrbHeader.Status = usbdStatus;
        UsbhCompleteIrp(Irp, nts);
        UsbhDecPdoIoCount(Pdo, Irp);

        LOG(HubFdo, LOG_HUB, 'ckC1', pd, nts);

        return nts;
    }

    // if connect bit is clear report disconnect
    // or
    // if connect bit plus connect and enable clear
    // report a disconnect
    if (!portState.Status.Connect ||
        (portState.Status.Connect == 0 &&
         portState.Status.Enable == 0)) {
        // report disconnected
        urb->UrbHeader.Status = USBD_STATUS_DEVICE_GONE;
        nts = NT_DEVICE_DISCONNECTED_STATUS;
        UsbhCompleteIrp(Irp, nts);
        UsbhDecPdoIoCount(Pdo, Irp);

        LOG(HubFdo, LOG_HUB, 'ckC2', pd, nts);
        return nts;
    }

    // forward all other URBs directly to the bus driver
    IoSkipCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, hubExt->TopOfBusStack);
    nts = IoCallDriver(hubExt->TopOfBusStack, Irp);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;

}



NTSTATUS
UsbhFdoSelectConfigurationPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

   Filters the select_config API. Provides the not-enough-power popup

Arguments:

Return Value:

    nt status code

--*/
{
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    PURB urb;
    PUSB_CONFIGURATION_DESCRIPTOR cd;
    USBD_STATUS usbdStatus;
    PHUB_PORT_DATA pd;

    DbgTrace((UsbhDebugTrace, "%!FUNC! %p\n", Pdo));
    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    pd = UsbhGetPortData(HubFdo, pdoExt->PortNumber);

    if (pd) {
        ioStack = IoGetCurrentIrpStackLocation(Irp);
        urb = ioStack->Parameters.Others.Argument1;

        cd = urb->UrbSelectConfiguration.ConfigurationDescriptor;

        // null check, check here to keep prefix happy
        // NULL is valid -- it means deconfigure the device
        if (cd != NULL) {

            // if we have a descriptor try to validate it

            // valid descriptor?
            if (!UsbhValidateConfigurationDescriptor(cd,
                    (ULONG)cd->wTotalLength, &usbdStatus, FALSE)) {

                // bad parameter, short out the API
                urb->UrbHeader.Status = usbdStatus;
                nts = STATUS_INVALID_PARAMETER;

                HUB_EXCEPTION(USBFILE,
                              HubFdo,
                              pdoExt->PortNumber,
                              InvalidDeviceConfigDescriptor,
                              nts,
                              urb->UrbHeader.Status,
                              cd,
                              (ULONG) (cd->bLength < sizeof(USB_CONFIGURATION_DESCRIPTOR) ?
                                           cd->bLength : sizeof(USB_CONFIGURATION_DESCRIPTOR)));

                UsbhCompleteIrp(Irp, nts);
                UsbhDecPdoIoCount(Pdo, Irp);

                return nts;
            }

            // enough power?
            pd->LastPowerRequested = ((LONG)cd->MaxPower)*2;
            DbgTrace((UsbhDebugTrace, ">>>>>>>>>>>>>>device for PDO %p requesting %d milliamps\n",
                Pdo, pd->LastPowerRequested));
            if (pd->LastPowerRequested > hubExt->MilliampsPowerPerPort) {
                // need more power than we have

                nts = STATUS_INSUFFICIENT_RESOURCES;
                urb->UrbHeader.Status = USBD_STATUS_INSUFFICIENT_RESOURCES;

                // set connection status to not enough power for USBUI
                // to find later
                UsbhSetConnectionStatus(HubFdo, pd, DeviceNotEnoughPower);

                HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, NotEnoughPower_Popup,
                              nts, urb->UrbHeader.Status, cd, sizeof(USB_CONFIGURATION_DESCRIPTOR));
                UsbhCompleteIrp(Irp, nts);
                UsbhDecPdoIoCount(Pdo, Irp);

                return nts;
            }
        }else {
             pd->LastPowerRequested = 0;
        }

    } else {
        nts = STATUS_INVALID_PARAMETER;
        UsbhCompleteIrp(Irp, nts);
        UsbhDecPdoIoCount(Pdo, Irp);

        return nts;
    }

    // hook the completion to catch bandwidth errors
    IoCopyCurrentIrpStackLocationToNext(Irp);

    UsbhSetCompletionRoutine(HubFdo,
                             Irp,
                             UsbhSelectConfigOrInterfaceComplete,
                             Pdo,
                             TRUE,
                             TRUE,
                             TRUE);

    UsbhAssert(HubFdo, hubExt->TopOfBusStack);
    nts = IoCallDriver(hubExt->TopOfBusStack, Irp);
    // don't release the io count on the PDO since we have a completion
    // routine.
    return nts;

}


NTSTATUS
UsbhFdoUrbPdoFilter(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    Some Urb APIs are handled by the hub so we are vulnerable to bogus URBs here
    but if we crash the caller should be on the stack.

    Urbs are passed to the bus driver for processing see UsbhFdoReturnRootHubPdo
    these urbs are passed to HcdTopOfStack.

    If the failio flag is set ie the device is removed then this API will be
    failed before we get here.

Arguments:

Return Value:

    nt status code

--*/
{
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    PURB urb;
    USHORT function;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    urb = ioStack->Parameters.Others.Argument1;
    function = urb->UrbHeader.Function;

    //LOG(HubFdo, LOG_HUB, 'fURB', Irp, urb);

    // for urb functions it is our job to set the device handle field
    // in the URB header before processing or passing on
    urb->UrbHeader.UsbdDeviceHandle = pdoExt->DeviceHandle;

    // some APIs require us to handle all or part of them

    //LOG(HubFdo, LOG_HUB, 'furb', function, 0);
    switch (function) {
    case URB_FUNCTION_SELECT_CONFIGURATION:
        return UsbhFdoSelectConfigurationPdo(HubFdo, Pdo, Irp);

    case URB_FUNCTION_SELECT_INTERFACE:
        return UsbhFdoSelectInterfacePdo(HubFdo, Pdo, Irp);

        // This is an extra API exported to query the oversized feature descriptor
        // in segments.  It is not supported by the hub it should be supported by the
        // usb port driver.
        // The hub driver does not service URBs -- it should only filter them.
        // Our only job here is to intercept this API if it is sent to a device that
        // does not support the MSOS descriptor and to fill in the vendor code for those
        // that do.
    case URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR:
        return UsbhFdoGetMsDescriptorPdo(HubFdo, Pdo, Irp);

    case URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL:
    case URB_FUNCTION_SYNC_RESET_PIPE:
    case URB_FUNCTION_SYNC_CLEAR_STALL:
        // we use this opportunity to check the status of the connection since
        // these APIs are synchronous we can block while check the hub port.
        // These APIs are typically called by clients on transaction errors and
        // usually occur during a surprise remove.  This extra check allows us
        // to report to the client driver the current status of the connection
        // more accurately.
        return UsbhFdoValidateConnectionPdo(HubFdo, Pdo, Irp);
    case URB_FUNCTION_GET_STATUS_FROM_DEVICE:

        //
        // Fail the URB_FUNCTION_GET_STATUS_FROM_DEVICE if the USB class driver
        // has indicated the device doesn't support it
        //

        if (pdoExt->DeviceFlags.FailGetStatusRequest == 1) {

            DbgTrace((UsbhDebugTrace, "URB_FUNCTION_GET_STATUS_FROM_DEVICE is not supported by device=%p %!FUNC!\n", pdoExt->Pdo));
            nts = STATUS_NOT_SUPPORTED;
            UsbhCompleteIrp(Irp, nts);
            UsbhDecPdoIoCount(Pdo, Irp);

            return nts;
        }

    }

    // forward all other URBs directly to the bus driver
    IoSkipCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, hubExt->TopOfBusStack);
    nts = IoCallDriver(hubExt->TopOfBusStack, Irp);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;

}


NTSTATUS
UsbhFdoCheckUpstreamConnectionState(
    PDEVICE_OBJECT HubFdo,
    PULONG IoctlPortStatus
    )
/*++

Routine Description:

    checks the state of the connection on our upstream port

Return Value:

    returns NT_DEVICE_DISCONNECTED_STATUS in the event the physical connection
    to our hub is broken.

--*/
{
    NTSTATUS nts;

    nts = UsbhSyncSendInternalIoctl(HubFdo,
                                    IOCTL_INTERNAL_USB_GET_PORT_STATUS,
                                    IoctlPortStatus,
                                    NULL);

    LOG(HubFdo, LOG_HUB, 'upsS', *IoctlPortStatus, nts);

    return nts;
}


NTSTATUS
UsbhFdoReturnDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    BOOLEAN ExApi
    )
/*++

Routine Description:

This function returns the device handle associated with the callers PDO. The
device handle is an opaque structure that is used as a parameter for other
APIs/

Parameters:
ioStackLocation->Parameters.Others.Argument1
A pointer to a device handle (pointer to pointer).

ioStackLocation->Parameters.Others.Argument2
(optional) object associated wth this reference.


ExApi - if True this is the upgraded API that returns the handle with a refernce


Return Value:

    nt status code

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PIO_STACK_LOCATION ioStack;
    PUSB_DEVICE_HANDLE *pdh;
    PVOID refObj;
    NTSTATUS nts = STATUS_INVALID_PARAMETER;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    pdoExt = PdoExt(Pdo);

    pdh = ioStack->Parameters.Others.Argument1;

    if (pdh) {

        if (ExApi) {
            refObj = ioStack->Parameters.Others.Argument2;
            *pdh = UsbhRefPdoDeviceHandle(HubFdo,
                                          Pdo,
                                          refObj,
                                          Devh_ReturnDH_Tag);

            if (*pdh != NULL) {
                nts = STATUS_SUCCESS;
            }

        }  else {
            nts = STATUS_SUCCESS;
        }
    }

    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;

}


NTSTATUS
UsbhFdoReturnTtDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

This function returns the Tt (Hs) device handle associated with the callers PDO. The
device handle is an opaque structure that is used as a parameter for other
APIs/

Parameters:
ioStackLocation->Parameters.Others.Argument1
A pointer to a device handle (pointer to pointer).

ioStackLocation->Parameters.Others.Argument2
(optional) object associated wth this reference.

ioStackLocation->Parameters.Others.Argument3
ptr to ushort that receives the port number associated with the TT.


Return Value:

    nt status code

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    PIO_STACK_LOCATION ioStack;
    PUSB_DEVICE_HANDLE *pdh;
    PVOID refObj;
    NTSTATUS nts;
    PUSHORT ttPort;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    pdoExt = PdoExt(Pdo);
    hubExt = FdoExt(HubFdo);
    LOG(HubFdo, LOG_HUB, 'tDH1', Pdo, Irp);

    pdh = ioStack->Parameters.Others.Argument1;
    refObj = ioStack->Parameters.Others.Argument2;
    ttPort = ioStack->Parameters.Others.Argument4;

    do {
        if (!pdh || !ttPort) {
            nts = STATUS_INVALID_PARAMETER;
            break;
        }

        LOG(HubFdo, LOG_HUB, 'tDH2', refObj, pdh);
        LOG(HubFdo, LOG_HUB, 'tDH3', ttPort, *ttPort);

        if (hubExt->FdoFlags.HubIsTT) {
            // devh handle is our handle
            *pdh = UsbhGetDeviceHandle(HubFdo, refObj);
            // tt index is port number
            *ttPort = pdoExt->PortNumber;

            LOG(HubFdo, LOG_HUB, 'tDH4', *pdh, *ttPort);

            if (*pdh) {
                nts = STATUS_SUCCESS;
            } else {
                nts = STATUS_UNSUCCESSFUL;
            }
            break;
        }

        if (hubExt->FdoFlags.HubIsRoot) {
            // we must be on a 1.1 controller
            *pdh = NULL;
            *ttPort = 0;
            nts = STATUS_SUCCESS;
            LOG(HubFdo, LOG_HUB, 'tDHr', 0, 0);
            break;
        }

        // pass on to the next hub
        IoCopyCurrentIrpStackLocationToNext(Irp);
        UsbhDecPdoIoCount(Pdo, Irp);

        return IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    } WHILE (0);


    LOG(HubFdo, LOG_HUB, 'tDHX', 0, nts);

    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;

}


NTSTATUS
UsbhGetHubPdoName(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_HUB_NAME NameBuffer,
    ULONG NameBufferLength,
    PULONG LengthReturned
    )
/*++

Routine Description:

    Helper function used to return the Symbolic name for a PDO with the leading
    icky suff stripped off.

    The symbolic name is returned only if the PDO is for a USB hub.

    assuming the string is \n\name strip of '\n\' where
    n is zero or more chars

Parameters:

    NameBuffer - pointer to USB_HUB_NAME format structure
    NameBufferLength - size of this buffer

IRQL: Any

Return Value:

    nt status code

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    LOG(HubFdo, LOG_HUB, 'gnn>', NameBuffer, NameBufferLength);

    *LengthReturned = 0;

    // you cannot be too careful these days
    if (NameBuffer == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    if (NameBufferLength < sizeof(struct _USB_HUB_NAME)) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    if (pdoExt->PdoFlags.DeviceIsHub &&
        pdoExt->PdoFlags.SymbolicLink) {

        PUNICODE_STRING hubNameUnicodeString;
        ULONG length, offset=0;
        WCHAR *pwch;

        // Device is a hub, get the name of the hub

        hubNameUnicodeString = &pdoExt->SymbolicLinkName;

        // assuming the string is \n\name strip of '\n\' where
        // n is zero or more chars

        pwch = &hubNameUnicodeString->Buffer[0];

        UsbhAssert(HubFdo, *pwch == '\\');
        if (*pwch == '\\') {
            pwch++;
            while (*pwch != '\\' && *pwch) {
                pwch++;
            }
            UsbhAssert(HubFdo, *pwch == '\\');
            if (*pwch == '\\') {
                pwch++;
            }
            offset = (ULONG)((PUCHAR)pwch -
                (PUCHAR)&hubNameUnicodeString->Buffer[0]);
        }

        // Strip off the '\DosDevices\' prefix.
        // Length does not include a terminating NULL.

        length = hubNameUnicodeString->Length - offset;
        RtlZeroMemory(NameBuffer, NameBufferLength);

        if (NameBufferLength >= length +
            sizeof(struct _USB_HUB_NAME)) {
            RtlCopyMemory(&NameBuffer->HubName[0],
                          &hubNameUnicodeString->Buffer[offset/2],
                          length);

            *LengthReturned = length + sizeof(struct _USB_HUB_NAME);
            NameBuffer->ActualLength = *LengthReturned;
            nts = STATUS_SUCCESS;

        } else {

            // Output buffer is too small to hold the entire
            // string.  Return just the length needed to hold
            // the entire string.

            NameBuffer->ActualLength =
                length + sizeof(struct _USB_HUB_NAME);

            NameBuffer->HubName[0] = UNICODE_NULL;

            *LengthReturned =  sizeof(struct _USB_HUB_NAME);
            nts = STATUS_SUCCESS;

        }

    } else {

        // Device is not a hub or does not currently have a symbolic link
        // allocated, just return a NULL terminated string.

        NameBuffer->ActualLength = sizeof(struct _USB_HUB_NAME);
        NameBuffer->HubName[0] = UNICODE_NULL;

        *LengthReturned = sizeof(struct _USB_HUB_NAME);

        nts = STATUS_SUCCESS;
    }

    LOG(HubFdo, LOG_HUB, 'gnn<', *LengthReturned, nts);

    return nts;

}



NTSTATUS
UsbhGetTopOfBusStack(
    PDEVICE_OBJECT HubFdo
    )
 /*++

Routine Description:

    'Get root hub Pdo'

    Returns the top of BUS stack, the device object which we will forward all
    bus transfers to.

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_OBJECT rootHubPdo, oldRefObj;
    PDEVICE_OBJECT topOfBusStack = NULL;

    hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();

    if (hubExt->FdoFlags.HubIsRoot) {
        // if we are root set our own top-of-device stack and
        // take a reference so that the remove code doesn't
        // have to distinguish between root and not root.
        // We don't bother with the ref/deref the root pdo since we
        // don't use it.

        topOfBusStack = hubExt->TopOfStackDeviceObject;

        REF_HUB(HubFdo, topOfBusStack, TOSr);
        ObReferenceObject(topOfBusStack);

        oldRefObj = InterlockedExchangePointer(
            &hubExt->TopOfBusStack,
            topOfBusStack);

        LOG(HubFdo, LOG_HUB, 'rrDO', oldRefObj, topOfBusStack);
        UsbhAssert(HubFdo, oldRefObj == NULL);

        nts = STATUS_SUCCESS;

    } else {
        // call down the stack to get this value
        nts = UsbhSyncSendInternalIoctl(HubFdo,
                                  IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO,
                                  &rootHubPdo,
                                  &topOfBusStack);

        if (NT_SUCCESS(nts)) {
            // on success we expect these objects to be ref'ed
            // since we hang on to the TOS ref make a note in our
            // own list
            REF_HUB(HubFdo, topOfBusStack, TOSr);

            oldRefObj = InterlockedExchangePointer(
                &hubExt->TopOfBusStack,
                topOfBusStack);

            UsbhAssert(HubFdo, oldRefObj == NULL);

            // drop ref on the root hub pdo , we don't use it
            ObDereferenceObject(rootHubPdo);
        }
    }

    LOG(HubFdo, LOG_HUB, 'gPDO', nts, topOfBusStack);

    return nts;
}


VOID
UsbhLogStartFailure(
    PDEVICE_OBJECT HubFdo,
    NTSTATUS FailStatus,
    USB_CONNECTION_STATUS ConnectStatus,
    ULONG Line,
    PCSTR SzFile,
    PCSTR SzDebugText
    )
 /*++

Routine Description:

    Attempts to log the reason we failed to start

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_START_FAILDATA failData;
    ULONG nBytes;
    PUCHAR pch;

    UsbhAssertPassive();

    LOG(HubFdo, LOG_HUB, 'srt!', FailStatus, ConnectStatus);

    DbgTrace((UsbhDebugTrace, "Hub Start Failure (%s) NTSTATUS(%!STATUS!) \n@:%s line %d\n",
        SzDebugText, FailStatus, SzFile, Line));

    hubExt = FdoExt(HubFdo);
    nBytes = sizeof(struct _USB_START_FAILDATA)+ (ULONG)strlen(SzDebugText)+1+
            (USBHUB_LOG_CATPURE*sizeof(LOG_ENTRY));

    UsbhAllocatePool_Z(failData, NonPagedPool, nBytes);

    if (failData) {
        PDEBUG_LOG log;
        ULONG i;

        failData->LengthInBytes = nBytes;
        failData->NtStatus = FailStatus;
        failData->ConnectStatus = ConnectStatus;
        pch = &failData->DriverData[0];

        log = &hubExt->Log;

        if (log->LogStart != NULL) {

            PLOG_ENTRY pLogE;
            ULONG ilog;

            ilog = log->LogIdx;

            for (i=0; i< USBHUB_LOG_CATPURE; i++) {
                ilog &= log->LogSizeMask;
                pLogE = log->LogStart+ilog;

                UsbhAssert(NULL, pLogE <= log->LogEnd);

                ilog++;
                RtlCopyMemory(pch, pLogE, sizeof(LOG_ENTRY));
                pch+= sizeof(LOG_ENTRY);
            }
        }

        RtlCopyMemory(pch, SzDebugText, strlen(SzDebugText));

        // call down the failure to our PDO
        nts = UsbhSyncSendInternalIoctl(HubFdo,
                                        IOCTL_INTERNAL_USB_RECORD_FAILURE,
                                        failData,
                                        NULL);
    }

    UsbhFreePool(failData);

}


NTSTATUS
UsbhGetHubCount(
    PDEVICE_OBJECT HubFdo,
    ULONG *Count
    )
 /*++

Routine Description:

    returns the count of hubs between this hub and the top of the stack
    inclusive of the root.

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();

    // 1 for this hub
    *Count = 1;

    if (hubExt->FdoFlags.HubIsRoot) {
        nts = STATUS_SUCCESS;
        LOG(HubFdo, LOG_HUB, 'HcnR', 0, 0);
    } else {
        // call down the stack to get this value
        nts = UsbhSyncSendInternalIoctl(HubFdo,
                                  IOCTL_INTERNAL_USB_GET_HUB_COUNT,
                                  Count,
                                  NULL);
    }

    LOG(HubFdo, LOG_HUB, 'Hcnt', nts, *Count);

    return nts;
}



NTSTATUS
UsbhFdoChainIrp(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Chains an irp to the next driver in the stack

Arguments:

Return Value:

    ntStatus
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    IoSkipCurrentIrpStackLocation(Irp);

    return IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);
}


NTSTATUS
UsbhFdoSystemControl(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Process the WMI irp

Arguments:

Return Value:

    ntStatus
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    SYSCTL_IRP_DISPOSITION irpDisposition;
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, ioStack->MajorFunction == IRP_MJ_SYSTEM_CONTROL);

    LOG(HubFdo, LOG_HUB, 'FWMI', 0, Irp);

    nts = IoAcquireRemoveLock(&hubExt->RemoveLock, Irp);

    if (NT_ERROR(nts)) {
        // device is removed
        Irp->IoStatus.Status = nts;

        LOG(HubFdo, LOG_POWER, '!wmi', nts, Irp);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

    } else {

        nts = WmiSystemControl(
                    &hubExt->WmiLibInfo,
                    HubFdo,
                    Irp,
                    &irpDisposition);

        LOG(HubFdo, LOG_HUB, 'Fwmi', irpDisposition, nts);
        IoReleaseRemoveLock(&hubExt->RemoveLock, Irp);

        switch (irpDisposition) {
        case IrpProcessed:
            //
            // This irp has been processed and may be completed or pending.
            // returns status from WMI
            break;

        case IrpNotCompleted:
            //
            // This irp has not been completed, but has been fully processed.
            // we will complete it now
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            break;

        case IrpForward:
        case IrpNotWmi:
            //
            // This irp is either not a WMI irp or is a WMI irp targeted
            // at a device lower in the stack.
            nts = UsbhFdoChainIrp(HubFdo, Irp);
            break;

        default:
            //
            // We really should never get here, but if we its probably a bug in WMI
            // that they should know about.
            UsbhAssertFailure(HubFdo, Irp);
            nts = UsbhFdoChainIrp(HubFdo, Irp);
            break;
        }
    }
    return nts;
}


NTSTATUS
UsbhDeferIrpCompletion(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PVOID Context
    )
/*++

Routine Description:

    General purpose routine to defer an irp and signal the event passed in the
    Context field.  We us this function when we send irps synchronously down
    the stack.

Arguments:

Return Value:

    STATUS_MORE_PROCESSING_REQUIRED, so the OS does not complete the irp

--*/
{
    PKEVENT event = Context;

    KeSetEvent(event,
               1,
               FALSE);

    return STATUS_MORE_PROCESSING_REQUIRED;
}


NTSTATUS
UsbhFlushPortChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    USBD_STATUS *UsbdStatus
    )
/*++

Routine Description:

    Ack a connect change indication on a specific hub port

Arguments:

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    NTSTATUS nts;
    USHORT nBytes = 0;

    hubExt = FdoExt(HubFdo);

    // setup packet for Clear Port Feature
    // 11.24.2.2
    setupPacket.bmRequestType.B = 0x23; //00100011B
    setupPacket.bRequest = USB_REQUEST_CLEAR_FEATURE;
    setupPacket.wValue.W = C_PORT_CONNECT;
    // we don't use the selector, 0 is default based on automatic
    // defined behavior based on port state
    setupPacket.wIndex.W = PortNumber;
    setupPacket.wLength = 0;

    nts = UsbhSyncSendCommand(HubFdo,
                              &setupPacket,
                              NULL,
                              &nBytes,
                              USBH_CONTROL_PIPE_TIMEOUT,
                              UsbdStatus);

    LOG(HubFdo, LOG_HUB, 'flpC', nts,  PortNumber);

    return nts;
}



NTSTATUS
UsbhSetPortPower(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber
    )
/*++

Routine Description:

    Turn power on to an individual port.  Note that this simply sends the
    command to switch 'on'.  This function does not perform any waits for power
    stabilization.

Arguments:


Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    EX_DATA portExData;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);

    portExData.nBytes = 0;
    portExData.PortNumber = PortNumber;
    portExData.State.Change = 0;
    portExData.State.Status = 0;

    LOG(HubFdo, LOG_HUB, 'ppp0', PortNumber,  0);

    // setup packet for Set_Feature Port Power
    // 11.24.2.7
    setupPacket.bmRequestType.B = 0x23; //00100011B
    setupPacket.bRequest =
        portExData.bRequest = USB_REQUEST_SET_FEATURE;
    setupPacket.wValue.W = PORT_POWER;
    setupPacket.wIndex.W = PortNumber;
    setupPacket.wLength = (USHORT) portExData.nBytes;
    // spec specifies 0
    UsbhAssert(HubFdo, portExData.nBytes == 0);

    nts = UsbhSyncSendCommand(HubFdo,
                              &setupPacket,
                              NULL,
                              &portExData.nBytes,
                              USBH_CONTROL_PIPE_TIMEOUT,
                              &usbdStatus);

    LOG(HubFdo, LOG_HUB, 'ppp1', nts,  usbdStatus);

    if (NT_ERROR(nts) &&
        !Usb_Disconnected(nts)) {
        // throw an exception so we know why it failed
        HUB_EXCEPTION(USBFILE,  HubFdo, PortNumber, HubErr_SetPortPowerFailed,
                          nts, usbdStatus, &portExData, sizeof(portExData));
    }

    LOG(HubFdo, LOG_HUB, 'prtP', nts,  PortNumber);

    return nts;
}


NTSTATUS
UsbhGetPortStatus(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PUSB_HUB_PORT_STATE PortState,
    USBD_STATUS *ReturnUsbdStatus
    )
/*++

Routine Description:

    Queries for the port status

Irql: PASSIVE

Arguments:

Return Value:

    optional: returns last usbd status received

    nt status

--*/
{
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    EX_DATA portExData;
    NTSTATUS nts;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;

    portExData.nBytes = sizeof(struct _USB_HUB_PORT_STATE);
    PortState->Change.us = 0;
    PortState->Status.us = 0;

    portExData.PortNumber = PortNumber;
    portExData.State.Change = 0;
    portExData.State.Status = 0;

    // setup packet for Get Port Status
    // 11.24.2.7
    setupPacket.bmRequestType.B = 0xa3; //10100011B
    setupPacket.bRequest =
        portExData.bRequest = USB_REQUEST_GET_STATUS;
    setupPacket.wValue.W = 0;
    setupPacket.wIndex.W = PortNumber;
    setupPacket.wLength = portExData.nBytes;
    // spec specifies 4
    UsbhAssert(HubFdo, portExData.nBytes == 4);

    nts = UsbhSyncSendCommand(HubFdo,
                              &setupPacket,
                              (PUCHAR)&portExData.State,
                              &portExData.nBytes,
                              USBH_CONTROL_PIPE_TIMEOUT,
                              &usbdStatus);

    SET_OPTIONAL_STATUS(ReturnUsbdStatus, usbdStatus);

    return nts;
}

NTSTATUS
UsbhSuspendPort(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber
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
    NTSTATUS nts_portstatus;
    USHORT nBytes = 0;
    USB_HUB_PORT_STATE PortState;

    hubExt = FdoExt(HubFdo);

    UsbhKdPrint((1, "USB Suspend> Hub #%d Port #%d\n", hubExt->HubNumber, PortNumber));

    LOG(HubFdo, LOG_HUB, 'sup0', PortNumber,  0);

    // setup packet for Set_Feature Port Suspend
    // 11.24.2.12
    setupPacket.bmRequestType.B = 0x23; //00100011B
    setupPacket.bRequest = USB_REQUEST_SET_FEATURE;
    setupPacket.wValue.W = PORT_SUSPEND;
    setupPacket.wIndex.W = PortNumber;
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
        HUB_EXCEPTION(USBFILE,  HubFdo, PortNumber, HubErr_SuspendPortFailed,
                          nts, usbdStatus, NULL, 0);
    }

    LOG(HubFdo, LOG_HUB, 'susP', nts,  PortNumber);

    if (hubExt->FdoFlags2.QueryPortStatusAfterSuspend)
    {
        //
        // For integrated rate-matching hubs, query the port status immediately
        // after suspending the port. This apparently prevents these hubs
        // from issuing spurious resume notifications.
        //

        nts_portstatus = UsbhGetPortStatus(HubFdo,
                                           PortNumber,
                                           &PortState,
                                           &usbdStatus);
        LOG(HubFdo, LOG_HUB, 'supS', nts_portstatus,  usbdStatus);
    }
    return nts;
}


NTSTATUS
UsbhSetHubRemoteWake(
    PDEVICE_OBJECT HubFdo,
    BOOLEAN EnableUsbWake
    )
/*++

Routine Description:

    Enables/Disables remote wake for a given hub

Arguments:

    EnableUsbWake - TRUE indicates send the set_feature(en), FALSE indicates
                clear_feature(dis).

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

    UsbhKdPrint((1, "USB Hub Wake Detect> Hub #%d \n", hubExt->HubNumber));

    setupPacket.bmRequestType.B = 0x00; //00000000B
    if (EnableUsbWake) {
        setupPacket.bRequest = USB_REQUEST_SET_FEATURE;
    } else {
        setupPacket.bRequest = USB_REQUEST_CLEAR_FEATURE;
    }

    LOG(HubFdo, LOG_HUB, 'Swak', hubExt->HubNumber,  setupPacket.bmRequestType.B);

    setupPacket.wValue.W = USB_FEATURE_REMOTE_WAKEUP;
    setupPacket.wIndex.W = 0;
    setupPacket.wLength = nBytes;
    // spec specifies 0
    UsbhAssert(HubFdo, nBytes == 0);

    nts = UsbhSyncSendCommand(HubFdo,
                              &setupPacket,
                              NULL,
                              &nBytes,
                              USBH_CONTROL_PIPE_TIMEOUT,
                              &usbdStatus);

    LOG(HubFdo, LOG_HUB, 'swk1', nts,  usbdStatus);

    if (NT_ERROR(nts) &&
        !Usb_Disconnected(nts)) {
        // throw an exception so we know why it failed
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_SetUsbWakeFailed,
                          nts, usbdStatus, NULL, 0);
    }

    LOG(HubFdo, LOG_HUB, 'swk2', nts,  0);

    return nts;
}


NTSTATUS
UsbhGetDescriptor(
    PDEVICE_OBJECT HubFdo,
    PUSHORT BufferLengthBytes,
    PVOID DescriptorBuffer,
    UCHAR DescriptorType,
    UCHAR DescriptorIndex,
    BOOLEAN ClassDescriptor
    )
/*++

Routine Description:

    get a descriptor from the hub

Arguments:


Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);


    // setup packet for Get_Descriptor

    // 9.4.3
    if (ClassDescriptor) {
        setupPacket.bmRequestType.B = 0xa0; //10100000B
    } else {
        setupPacket.bmRequestType.B = 0x80; //10000000B
    }
    setupPacket.bRequest = USB_REQUEST_GET_DESCRIPTOR;
    setupPacket.wValue.HiByte = DescriptorType;
    setupPacket.wValue.LowByte = DescriptorIndex;
    setupPacket.wIndex.W = 0;
    setupPacket.wLength = *BufferLengthBytes;
    // spec specifies 0

    nts = UsbhSyncSendCommand(HubFdo,
                              &setupPacket,
                              DescriptorBuffer,
                              BufferLengthBytes,
                              USBH_CONTROL_PIPE_TIMEOUT,
                              &usbdStatus);

    if (NT_ERROR(nts) &&
        !Usb_Disconnected(nts)) {
        // throw an exception so we know why it failed
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_GetDescriptorFailed,
                          nts, usbdStatus, DescriptorBuffer,
                          *BufferLengthBytes);
    }


    return nts;
}


NTSTATUS
UsbhGetStatus(
    PDEVICE_OBJECT HubFdo,
    PUSHORT StatusBits
    )
/*++

Routine Description:

    perform the standard USB get status request to a hub

Arguments:

    device status bits from the hub.

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    NTSTATUS nts;
    USHORT nBytes;

    hubExt = FdoExt(HubFdo);


    nBytes = sizeof(*StatusBits);

    // setup packet for Get_Status

    // 9.4.5
    setupPacket.bmRequestType.B = 0x80; //10000000B

    setupPacket.bRequest = USB_REQUEST_GET_STATUS;
    setupPacket.wValue.W = 0;
    setupPacket.wIndex.W = 0;
    setupPacket.wLength = nBytes;

    nts = UsbhSyncSendCommand(HubFdo,
                              &setupPacket,
                              (PUCHAR) StatusBits,
                              &nBytes,
                              USBH_CONTROL_PIPE_TIMEOUT,
                              &usbdStatus);

    if (NT_ERROR(nts) &&
        !Usb_Disconnected(nts)) {
        // throw an exception so we know why it failed
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_GetStatusFailed,
                          nts, usbdStatus, StatusBits,
                          nBytes);
    }


    return nts;
}


NTSTATUS
UsbhGetHubConfigurationDescriptor(
    PDEVICE_OBJECT HubFdo,
    PUSB_CONFIGURATION_DESCRIPTOR *ConfigurationDescriptor
    )
/*++

Routine Description:

    get the configuration descriptor from the hub

Arguments:


Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PUSB_CONFIGURATION_DESCRIPTOR cd;
    USHORT nBytes;
    LONG retry = -1;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);


    *ConfigurationDescriptor = NULL;

    // for compatibility we do the same thing the hub has always done which is
    // make the first request with a buffer of 255 bytes.
    //nBytes = sizeof(*cd);
    nBytes = 255;

UsbhGetHubConfigurationDescriptor_Retry:

    UsbhAllocatePool_Z(cd, NonPagedPool, nBytes);
    if (cd == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    nts = UsbhGetDescriptor(HubFdo,
                            &nBytes,
                            cd,
                            USB_CONFIGURATION_DESCRIPTOR_TYPE,
                            0,
                            FALSE);

    LOG(HubFdo, LOG_HUB, 'ghc1', nBytes,  nts);

    if (NT_ERROR(nts)) {
        UsbhFreePool(cd);
        return nts;
    }

    if (nBytes < sizeof(struct _USB_CONFIGURATION_DESCRIPTOR)) {
        // short data
        nts = STATUS_UNSUCCESSFUL;
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_GetConfigDescriptorFailed,
                          nts, 0xFFFFFFFF, cd, nBytes);

        UsbhFreePool(cd);
        return nts;
    }

    LOG(HubFdo, LOG_HUB, 'ghc2', nBytes,  cd->wTotalLength);

    if (cd->wTotalLength > nBytes) {
        if (retry) {
            retry++;
            LOG(HubFdo, LOG_HUB, 'ghcR', nBytes,  nts);
            nBytes = cd->wTotalLength;
            UsbhFreePool(cd);
            goto UsbhGetHubConfigurationDescriptor_Retry;
        } else {
            nts = STATUS_UNSUCCESSFUL;
            HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_GetConfigDescriptorFailed,
                          nts, 0xFFFFFFFF, cd, nBytes);

            UsbhFreePool(cd);
            return nts;
        }
    }

    LOG(HubFdo, LOG_HUB, 'ghc3', cd,  nts);

    // caller must free it
    *ConfigurationDescriptor = cd;

    return nts;
}


NTSTATUS
UsbhSyncPowerOnPorts(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Turn Power on to the USB ports on the hub.  This is where we perform the
    necessary wait and stabilization times required by the core specification.

Arguments:


Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts = STATUS_SUCCESS;
    USHORT p;

    hubExt = FdoExt(HubFdo);

    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));
    UsbhAssertPassive();

    // send the command to power each port

    for (p=1; p <= N_PORTS(HubFdo); p++) {
        // this routine throws an exception if it fails
        nts = UsbhSetPortPower(HubFdo, p);
        if (NT_ERROR(nts)) {
            break;
        }
    }

    // spec requires an minimum of 100ms stabilization plus 100ms debounce
    // plus power-on to power-good -- not to exceed 100ms so for consistency we
    // wait 300ms after applying power to all the ports.
    if (NT_SUCCESS(nts)) {
        UsbhWait(HubFdo, HubG.GlobalDefaultHubPowerUpTime);
    }

    LOG(HubFdo, LOG_HUB, 'sPOW', nts,  0);

    DbgTrace((UsbhDebugTrace,"%!FUNC! %!STATUS!<\n", nts));
    return nts;
}


NTSTATUS
UsbhGetHubClassDescriptor(
    PDEVICE_OBJECT HubFdo,
    PUSB_HUB_DESCRIPTOR HubDescriptor
    )
/*++

Routine Description:

    Get the hub descriptor and store it in the device extension. We also
    perform some sanity checking on it -- i.e. we don't allow hubs with zero
    ports.

    two type codes are defined for the hub descriptor,
    0 pre-dates the 1.0 specification
    0x29 the official version.

    The hub has always asked for these in order starting with the 0x00 first,
    we are keeping this behavior for compatibility

Arguments:


Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PUSB_HUB_DESCRIPTOR hd;
    USHORT nBytes;
    //UCHAR dt = 0x00; // type code from 2.0 spec
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));

    hd =  &hubExt->HubDescriptor;
    RtlZeroMemory(HubDescriptor, sizeof(struct _USB_HUB_DESCRIPTOR));
    nBytes = sizeof(struct _USB_HUB_DESCRIPTOR);

    nts = UsbhGetDescriptor(HubFdo,
                            &nBytes,
                            HubDescriptor,
                            // use only descriptor type and index 0
                            // Pre USB 1.0 hubs use 0x00 as type and we
                            // used to support it but not anymore.
                            0x29,
                            0,
                            TRUE);

    LOG(HubFdo, LOG_HUB, 'Hcls', nts,  nBytes);
    if (NT_ERROR(nts)) {

        if (!Usb_Disconnected(nts)) {
            HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_GetHubDescriptorFailed,
                              nts, 0xFFFFFFFF, &hd, nBytes);
        }

        return nts;
    }

    if (hd->bNumberOfPorts == 0) {
        nts = STATUS_INVALID_PARAMETER;
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_GetHubDescriptorFailed_NoPorts,
                          nts, 0xFFFFFFFF, &hd, nBytes);

        return nts;
    }

    hubExt->FdoFlags.HubDescriptorValid = 1;
    UsbhAssert(HubFdo, NT_SUCCESS(nts));

    LOG(HubFdo, LOG_HUB, 'Hcld', hd,  hd->bNumberOfPorts);

    return nts;

}


NTSTATUS
UsbhGetHubDeviceInformation(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

   Get information about the hub.

Arguments:


Return Value:

    nt status

    sets the device type and device speed vars in the hub device extension

--*/
{
    NTSTATUS nts;
    ULONG length, lengthCopied;
    PUSB_DEVICE_INFORMATION_0 level_0 = NULL;
    PDEVICE_EXTENSION_HUB  hubExt;
    PUSB_DEVICE_HANDLE hubDevHandle;

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));

    hubDevHandle = UsbhGetDeviceHandle(HubFdo, Sc);

    if (hubDevHandle == NULL) {
        return STATUS_UNSUCCESSFUL;
    }

    length = sizeof(struct _USB_DEVICE_INFORMATION_0);

    do {
        UsbhAssert(HubFdo, level_0 == NULL);

        UsbhAllocatePool_Z(level_0, PagedPool, length);
        if (level_0 == NULL) {
            // release the hub device handle so it may be freed
            Usbh_HubDerefDeviceHandle(HubFdo, hubDevHandle, Sc, Devh_ReturnDH_Tag);

            return STATUS_INSUFFICIENT_RESOURCES;
        }

        level_0->InformationLevel = 0;

        nts = Usbh_HubQueryDeviceInformation(HubFdo,
                                             hubDevHandle,
                                             level_0,
                                             length,
                                             &lengthCopied);

        if (nts == STATUS_BUFFER_TOO_SMALL) {
            length = level_0->ActualLength;
            UsbhFreePool(level_0);
            level_0 = NULL;
        }

    } while (nts == STATUS_BUFFER_TOO_SMALL);

    // release the hub device handle so it may be freed
    Usbh_HubDerefDeviceHandle(HubFdo, hubDevHandle, Sc, Devh_ReturnDH_Tag);

    UsbhAssert(HubFdo, level_0);

    if (NT_SUCCESS(nts)) {
        USBD_STATUS usbdStatus;

        hubExt->DeviceType = level_0->DeviceType;
        hubExt->DeviceSpeed = level_0->DeviceSpeed;
        RtlCopyMemory(&hubExt->DeviceDescriptor, &level_0->DeviceDescriptor,
            sizeof(struct _USB_DEVICE_DESCRIPTOR));
        LOG(HubFdo, LOG_HUB, 'inf0', level_0,  hubExt);
        LOG(HubFdo, LOG_HUB, 'infx', hubExt->DeviceSpeed,  hubExt->DeviceType);

        if (!UsbhValidateDeviceDescriptor(HubFdo,
                                          &hubExt->DeviceDescriptor,
                                          sizeof(struct _USB_DEVICE_DESCRIPTOR),
                                          &usbdStatus,
                                          NULL)) {

            // bad device descriptor
            nts = STATUS_INVALID_PARAMETER;
            HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_InvalidHubDeviceDescriptor,
                              nts, usbdStatus,
                              &hubExt->DeviceDescriptor,
                              sizeof(struct _USB_DEVICE_DESCRIPTOR));
        }
    }

    UsbhFreePool(level_0);
    level_0 = NULL;

    LOG(HubFdo, LOG_HUB, 'inf>', nts,  0);

    return nts;
}


BOOLEAN
UsbhIsHighSpeedCapable(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Returns true if the device is high speed capable.  This means it is either
    running at high speed or could.

Arguments:

--*/
{
    NTSTATUS nts;
    USBD_STATUS usbdStatus;
    PDEVICE_EXTENSION_HUB hubExt;
    USHORT nBytes;
    USB_DEVICE_QUALIFIER_DESCRIPTOR dqs;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));

    if (hubExt->DeviceType != Usb20Device) {
        // not 20 therefore it cannot be high speed
        return FALSE;
    }

    if (hubExt->DeviceSpeed == UsbHighSpeed) {
        // it follows that the hub is high speed
        return TRUE;
    }

    // running at full speed, check for a qualifier
    UsbhAssert(HubFdo, hubExt->DeviceSpeed == UsbFullSpeed);

    nBytes = sizeof(struct _USB_DEVICE_QUALIFIER_DESCRIPTOR);

    setupPacket.bmRequestType.B = 0x80;
    setupPacket.bRequest = USB_REQUEST_GET_DESCRIPTOR;
    setupPacket.wValue.HiByte = USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE;
    setupPacket.wValue.LowByte = 0;
    setupPacket.wIndex.W = 0;
    setupPacket.wLength = nBytes;

    // failure here just means it is not high speed capable
    nts = UsbhSyncSendCommand(HubFdo,
                              &setupPacket,
                              (PUCHAR) &dqs,
                              &nBytes,
                              USBH_CONTROL_PIPE_TIMEOUT,
                              &usbdStatus);

    if (NT_SUCCESS(nts)) {
        return TRUE;
    }

    return FALSE;
}



NTSTATUS
UsbhConfigureUsbHub(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Selects the configuration and the interface and gets our pipe handle for
    interrupt transfers.

Arguments:


Return Value:

    nt status

--*/
{
    NTSTATUS nts;
    PURB urb;
    PUSB_INTERFACE_DESCRIPTOR interfaceDescriptor;
    PDEVICE_EXTENSION_HUB  hubExt;
    PUSB_CONFIGURATION_DESCRIPTOR cd = NULL;
    USBD_STATUS usbdStatus;

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));

    nts = UsbhGetHubConfigurationDescriptor(HubFdo,
                                           &cd);

    if (NT_ERROR(nts)) {
        LOG(HubFdo, LOG_HUB, 'cfh1', nts,  0);
        UsbhAssert(HubFdo, cd == NULL);
        return nts;
    }

    if (cd == NULL) {
        nts = STATUS_INSUFFICIENT_RESOURCES;
        LOG(HubFdo, LOG_HUB, 'cfh2', nts,  0);
        return nts;
    }

    // valid length, check for garbage
    if (!UsbhValidateConfigurationDescriptor(cd,
            cd->wTotalLength, &usbdStatus, FALSE)) {

        HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_InvalidHubConfigDescriptor,
                          STATUS_UNSUCCESSFUL, usbdStatus,
                          cd, cd->wTotalLength);
        LOG(HubFdo, LOG_HUB, 'cfh3', nts,  0);
        UsbhFreePool(cd);
        return STATUS_UNSUCCESSFUL;
    }

    // set up the default power attribute
    if (cd->bmAttributes & USB_CONFIG_SELF_POWERED) {
        DbgTrace((UsbhDebugTrace, "config desc says hub is self powered\n"));
        hubExt->PortPower_CfgDescriptor = USB_500ma;
    } else {
        DbgTrace((UsbhDebugTrace, "config desc says hub is bus powered\n"));
        hubExt->PortPower_CfgDescriptor = USB_100ma;
    }

    // check the device for power required
    hubExt->MilliampsPowerPerPort = UsbhGetMaxPowerPerPort(HubFdo);

    // the behavior below is derived from the original hub driver
    //
    // The amount of current a bus powered hub will draw (mA)
    // should be calculated as follows:
    //
    // NumberOfExternalPorts * 100 + HubCntrolCurrent + power required for embeded functions
    //
    // this value cannot exceed 500 ma, the hub config descriptor should report this value but in most cases
    // does not so we set it to a value greater than 100ma  to insure that a bus powered hub cannot be connected
    // to another bus powered hub.
    if (hubExt->MilliampsPowerPerPort == USB_100ma) {
        cd->MaxPower = 250;
     }

    // if the hub supports TTs we try to find the
    // multi TT interface

    do {

        if (hubExt->FdoFlags.HubIsTT) {
            hubExt->FdoFlags.MultiTTsupported = 0;

            // 2.0 hubs may have multiple interfaces
            // one for per port TTs
            // one for a global TT
            // these are differentiated by the by portocol field, we attempt
            // to select the multi-TT version by default
            //

            // find a Multi TT interface
            interfaceDescriptor =
                    UsbhParseConfigurationDescriptorEx(
                        HubFdo,
                        cd,
                        (PVOID) cd,
                        -1, //interface, don't care
                        -1, //alt setting, don't care
                        USB_DEVICE_CLASS_HUB, // hub class
                        -1, // subclass, don't care
                        2); // multi TT protocol

            if (interfaceDescriptor != NULL) {
                // we have a multi-TT hub
                hubExt->FdoFlags.MultiTTsupported = 1;

                if (!hubExt->FdoFlags.ForceSingleTt) {

                    hubExt->FdoFlags.MultiTTmode = 1;
                    break;
                }

                DbgTrace((UsbhDebugTrace, "forcing single TT mode\n"));
            }

            hubExt->FdoFlags.MultiTTmode = 0;
            // locate the single TT protocol, may be zero or 1
            interfaceDescriptor =
                UsbhParseConfigurationDescriptorEx(
                    HubFdo,
                    cd,
                    (PVOID) cd,
                    -1, //interface, don't care
                    -1, //alt setting, don't care
                    USB_DEVICE_CLASS_HUB, // hub class
                    -1, // subclass, don't care
                    1); // single TT protocol 1

            if (interfaceDescriptor != NULL) {
                break;
            }

            if (hubExt->FdoFlags.HubIsRoot) {
                //
                // USBPORT will set the protocol to 1 for TT for the root hub
                // If we haven't found the interface by now, then we assume that
                // this hub does NOT support TTs.
                //
                hubExt->FdoFlags.HubIsTT = 0;
            }

            // locate the single TT protocol 0
            interfaceDescriptor =
                UsbhParseConfigurationDescriptorEx(
                    HubFdo,
                    cd,
                    (PVOID) cd,
                    -1, //interface, don't care
                    -1, //alt setting, don't care
                    USB_DEVICE_CLASS_HUB, // hub class
                    -1, // subclass, don't care
                    0); // single TT protocol


        } else {

            // just do what we always did to be safe
            interfaceDescriptor =
                    UsbhParseConfigurationDescriptorEx(
                        HubFdo,
                        cd,
                        (PVOID)cd,
                        -1, //interface, don't care
                        -1, //alt setting, don't care
                        USB_DEVICE_CLASS_HUB, // hub class
                        -1, // subclass, don't care
                        -1); // protocol, don't care
        }

    } WHILE (0);

    if (interfaceDescriptor == NULL) {
        // no interface for hub found, someone may have tried to load us as
        // the generic parent.
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_HubInterfaceNotFound,
                          STATUS_UNSUCCESSFUL, 0, cd,
                            cd->wTotalLength);

        LOG(HubFdo, LOG_HUB, 'cfh4', cd->wTotalLength,  cd);
        UsbhFreePool(cd);
        return STATUS_UNSUCCESSFUL;
    }

    if (interfaceDescriptor->bInterfaceClass != USB_DEVICE_CLASS_HUB) {
        // this would indicate a bug in the USBD parsing functions
        LOG(HubFdo, LOG_HUB, 'cfh5', interfaceDescriptor->bInterfaceClass,  cd);
        UsbhAssertMsg(HubFdo, "Descriptor Parsing Error", FALSE);
        UsbhFreePool(cd);
        return STATUS_UNSUCCESSFUL;
    }

    urb = UsbhCreateConfigurationRequestEx(
            HubFdo,
            cd,
            cd->wTotalLength,
            interfaceDescriptor,
            sizeof(struct _USB_INTERFACE_DESCRIPTOR)+sizeof(struct _USB_ENDPOINT_DESCRIPTOR));

    if (NULL == urb) {
        LOG(HubFdo, LOG_HUB, 'cfh6', 0, 0);
        UsbhFreePool(cd);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    nts = UsbhSyncSendInternalIoctl(HubFdo,
                                    IOCTL_INTERNAL_USB_SUBMIT_URB,
                                    urb,
                                    NULL);

    LOG(HubFdo, LOG_HUB, 'cfh7', 0, nts);

    if (NT_SUCCESS(nts)) {
        PUSBD_INTERFACE_INFORMATION xf;

        //
        // interface we selected
        //

        xf = &urb->UrbSelectConfiguration.Interface;

        //
        // Save the pipe handle for the Interrupt pipe
        //
        hubExt->IrqPipeInformation = xf->Pipes[0];
        hubExt->ConfigurationHandle =
            urb->UrbSelectConfiguration.ConfigurationHandle;


    }

    UsbhFreePool(cd);
    UsbhFreePool(urb);

    return nts;
}


NTSTATUS
UsbhInitializeTtHub(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    Performs USB 2 TT initialization for a hub

Arguments:

Return Value:

    nt status

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB  hubExt;
    PUSB_DEVICE_HANDLE hubDeviceHandle;
    ULONG ttCount;

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));

    UsbhAssert(HubFdo, hubExt->FdoFlags.HubIsTT);

    hubDeviceHandle = UsbhGetDeviceHandle(HubFdo, Sc);
    if (hubDeviceHandle == NULL) {
        return STATUS_UNSUCCESSFUL;
    }

    if (hubExt->FdoFlags.MultiTTmode) {
        ttCount = N_PORTS(HubFdo);
    } else {
        ttCount = 1;
    }

    nts = Usbh_HubInitialize20Hub(HubFdo, hubDeviceHandle, ttCount);

    LOG(HubFdo, LOG_HUB, 'inTT', nts,  ttCount);

    Usbh_HubDerefDeviceHandle(HubFdo, hubDeviceHandle, Sc, Devh_ReturnDH_Tag);

    return nts;
}


VOID
UsbhGetHubPowerStatus(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

   This routine will query the hub fort its current power status and update the PortPower_HubStatus field
   in the hub device extension.  We currently don't use this value but it is useful for diagnostic purposes
   and could be used to better determine the difference between bus and self powered hubs.

   This request is only valid AFTER the hub is configured, if called before the hubis configured it is a nop.


Arguments:

    HubFdo

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;
    USB_HUB_STATE hubState;
    USBD_STATUS usbdStatus;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));

    if (hubExt->ConfigurationHandle) {
     // this command is only valid if the hub is configured.
        hubState.Status.AsUshort16 = 0;
        hubState.Change.AsUshort16 = 0;

        nts = UsbhQueryHubState(HubFdo, &hubState, &usbdStatus);
        LOG(HubFdo, LOG_HUB, 'hstt', 0, nts);

        if (NT_SUCCESS(nts)) {

            // this value is not currently used
            if (hubState.Status.LocalPowerLost) {
                DbgTrace((UsbhDebugTrace, "getstatus(hub) says hub is bus powered\n"));
                hubExt->PortPower_HubStatus = USB_100ma;
            } else {
                DbgTrace((UsbhDebugTrace, "getstatus(hub) says hub is self powered\n"));
                hubExt->PortPower_HubStatus = USB_500ma;
            }

        }
    }else {
         hubExt->PortPower_HubStatus = 0;
    }
}


ULONG
UsbhGetMaxPowerPerPort(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    This routine is called after the hub is configured.

    Returns 100 if the hub is bus powered, this function is not failable.  We
    use the same algorithm as in the original hub: use bus/self status in
    this priority order(1 supercedes 2 ect)

    1 query registry
    2 query device for status
    3 use value in the config descriptor

    on entry the PortPower_CfgDescriptor in the hubExt is set to the value from the
    config descriptor.
    NOTE: I believe some hubs may change the value reported by GET_STATUS if power
    is removed from the hub but we do not support this (we never have).

Arguments:

Return Value:

    BOOLEAN true if bus powered

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;
    ULONG milliampsPerPort = 0;
    USHORT statusBits = 0;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));

    //
    // now attempt to get the status bits from the device
    //
    nts = UsbhGetStatus(HubFdo, &statusBits);

    if (NT_SUCCESS(nts)) {

        if (statusBits & USB_GETSTATUS_SELF_POWERED) {
            DbgTrace((UsbhDebugTrace, "getstatus desc says hub is self powered\n"));
            hubExt->PortPower_DeviceStatus = USB_500ma;
        } else {
            DbgTrace((UsbhDebugTrace, "getstatus desc says hub is bus powered\n"));
            hubExt->PortPower_DeviceStatus = USB_100ma;
        }

    }

    // if configured query the hub power status
    UsbhGetHubPowerStatus(HubFdo);

    // check the registry

    do {
        // reg value specified?
        if (hubExt->PortPower_Registry) {
            // note: reg allows a specific value for playing around with power budgeting concepts

            DbgTrace((UsbhDebugTrace, "forcing port power (registry) %d\n", hubExt->PortPower_Registry));
            milliampsPerPort = hubExt->PortPower_Registry;
            break;
        }

        // device status ?
        if (hubExt->PortPower_DeviceStatus) {
            milliampsPerPort = hubExt->PortPower_DeviceStatus;
            break;
        }

        // cfg descriptor?
        if (hubExt->PortPower_CfgDescriptor) {
            milliampsPerPort = hubExt->PortPower_CfgDescriptor;
            break;
        }

        // set default
        milliampsPerPort = USB_500ma;

    } WHILE (0);

    // should not be zero
    UsbhAssert(HubFdo, milliampsPerPort);

    return milliampsPerPort;
}


NTSTATUS
UsbhAcpiEvalAcpiMethodEx(
    PDEVICE_OBJECT HubFdo,
    PANSI_STRING AcpiDeviceName,
    ULONG AcpiMethodName,
    PACPI_EVAL_OUTPUT_BUFFER AcpiEvalOutputBuffer,
    ULONG AcpiEvalOutputSize
    )
/*++

Routine Description:

    This routine evaluates any ACPI method for a device in the ACPI namespace.

    This is accomplished by sending an IRP down the hub driver stack for the
    ACPI filter to handle.

Arguments:


Return Value:

   nt status

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    ACPI_EVAL_INPUT_BUFFER_COMPLEX_EX acpiEvalInputBuffer;
    ULONG offset = 0;
    KEVENT event;
    PIRP irp;
    IO_STATUS_BLOCK ioStatus;

    hubExt = FdoExt(HubFdo);

    // verify that device name, delimiter, method name, and zero terminator
    // will fit into the input buffer method name field
    if (AcpiDeviceName->Length + 1 + sizeof(ULONG) + 1 > sizeof(acpiEvalInputBuffer.MethodName)) {
        return STATUS_INVALID_PARAMETER;
    }

    // initialize input buffer
    RtlZeroMemory(&acpiEvalInputBuffer, sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX_EX));

    acpiEvalInputBuffer.Signature = ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE_EX;
    acpiEvalInputBuffer.Size = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX_EX);

    // build full method name from device name and four byte method name
    RtlCopyMemory(&acpiEvalInputBuffer.MethodName[offset],
                  AcpiDeviceName->Buffer,
                  AcpiDeviceName->Length * sizeof(CHAR));

    offset += AcpiDeviceName->Length;

    acpiEvalInputBuffer.MethodName[offset++] = '.';

    RtlCopyMemory(&acpiEvalInputBuffer.MethodName[offset],
                  &AcpiMethodName,
                  sizeof(ULONG));

    offset += sizeof(ULONG);

    acpiEvalInputBuffer.MethodName[offset] = '\0';

    // initialize sync event to wait on
    INIT_SYNC_EVENT_NOT_SIGNALED(&event);

    // build IRP to evaluate ACPI method
    irp = IoBuildDeviceIoControlRequest(IOCTL_ACPI_EVAL_METHOD_EX,
                                        hubExt->TopOfStackDeviceObject,
                                        &acpiEvalInputBuffer,
                                        sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX_EX),
                                        AcpiEvalOutputBuffer,
                                        AcpiEvalOutputSize,
                                        FALSE,
                                        &event,
                                        &ioStatus);

    if (irp == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // send IRP to top of hub device stack and wait for completion
    ObReferenceObject(hubExt->TopOfStackDeviceObject);

    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, irp);

    if (nts == STATUS_PENDING) {
        nts = WAIT_EVENT(&event);

        if (nts == STATUS_SUCCESS) {
            nts = ioStatus.Status;
        }
    }

    ObDereferenceObject(hubExt->TopOfStackDeviceObject);

    if (!NT_SUCCESS(nts)) {
        return nts;
    }

    // verify returned output buffer
    if ((AcpiEvalOutputBuffer->Signature != ACPI_EVAL_OUTPUT_BUFFER_SIGNATURE) ||
        (AcpiEvalOutputBuffer->Count == 0)) {
        return STATUS_ACPI_INVALID_DATA;
    }

    return nts;
}


NTSTATUS
UsbhAcpiEnumChildren(
    PDEVICE_OBJECT HubFdo,
    PACPI_ENUM_CHILDREN_OUTPUT_BUFFER AcpiEnumOutputBuffer,
    ULONG AcpiEnumOutputSize
    )
/*++

Routine Description:

    This routine enumerates child devices in the ACPI namespace under the
    supplied hub device object.

    This is accomplished by sending an IRP down the hub driver stack for the
    ACPI filter to handle.

Arguments:


Return Value:

   nt status

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    ACPI_ENUM_CHILDREN_INPUT_BUFFER acpiEnumInputBuffer;
    KEVENT event;
    PIRP irp;
    IO_STATUS_BLOCK ioStatus;

    hubExt = FdoExt(HubFdo);

    // initialize input buffer
    RtlZeroMemory(&acpiEnumInputBuffer, sizeof(ACPI_ENUM_CHILDREN_INPUT_BUFFER));

    acpiEnumInputBuffer.Signature = ACPI_ENUM_CHILDREN_INPUT_BUFFER_SIGNATURE;
    acpiEnumInputBuffer.Flags = ENUM_CHILDREN_IMMEDIATE_ONLY;

    // initialize sync event to wait on
    INIT_SYNC_EVENT_NOT_SIGNALED(&event);

    // build IRP to enumerate child ACPI namespace devices
    irp = IoBuildDeviceIoControlRequest(IOCTL_ACPI_ENUM_CHILDREN,
                                        hubExt->TopOfStackDeviceObject,
                                        &acpiEnumInputBuffer,
                                        sizeof(ACPI_ENUM_CHILDREN_INPUT_BUFFER),
                                        AcpiEnumOutputBuffer,
                                        AcpiEnumOutputSize,
                                        FALSE,
                                        &event,
                                        &ioStatus);

    if (irp == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // send IRP to top of hub device stack and wait for completion
    ObReferenceObject(hubExt->TopOfStackDeviceObject);

    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, irp);

    if (nts == STATUS_PENDING) {
        nts = WAIT_EVENT(&event);

        if (nts == STATUS_SUCCESS) {
            nts = ioStatus.Status;
        }
    }

    ObDereferenceObject(hubExt->TopOfStackDeviceObject);

    // verify returned output buffer based on supplied buffer size, where an
    // unextended buffer indicates that the required size is being queried
    if (AcpiEnumOutputSize == sizeof(ACPI_ENUM_CHILDREN_OUTPUT_BUFFER)) {
        if (nts != STATUS_BUFFER_OVERFLOW) {
            return STATUS_ACPI_INVALID_DATA;
        }

        // required buffer size if returned in the NumberOfChildren field
        if ((AcpiEnumOutputBuffer->Signature != ACPI_ENUM_CHILDREN_OUTPUT_BUFFER_SIGNATURE) ||
            (AcpiEnumOutputBuffer->NumberOfChildren < sizeof(ACPI_ENUM_CHILDREN_OUTPUT_BUFFER))) {
            return STATUS_ACPI_INVALID_DATA;
        }
    } else {
        if (!NT_SUCCESS(nts)) {
            return nts;
        }

        // should expect at least one child or something is wrong
        if ((AcpiEnumOutputBuffer->Signature != ACPI_ENUM_CHILDREN_OUTPUT_BUFFER_SIGNATURE) ||
            (AcpiEnumOutputBuffer->NumberOfChildren == 0) ||
            (ioStatus.Information != AcpiEnumOutputSize)) {
            return STATUS_ACPI_INVALID_DATA;
        }
    }

    return nts;
}

// ACPI method evaluation output size (arbitrarily large to hold any result)
#define ACPI_METHOD_OUTPUT_BUFFER_SIZE  1024

NTSTATUS
UsbhGetAcpiPortAttributes(
    PDEVICE_OBJECT HubFdo,
    PUSB_EXTENDED_PORT_ATTRIBUTES AttrList
    )
/*++

Routine Description:

    Retrieve extra information about the hub ports from ACPI.

    Attributes are stored in an array, indexed by port number.

    AttrList[0] - not used
    AttrList[1] - attributes for port 1
    AttrList[2] = attributes for port 2

    ...

Arguments:

Return Value:

    support for these attributes is optional so we don't fail unless we cannot
    get memory.

    nt status

--*/
{
    NTSTATUS nts;
    ACPI_ENUM_CHILDREN_OUTPUT_BUFFER acpiEnumOutputSizeBuffer;
    PACPI_ENUM_CHILDREN_OUTPUT_BUFFER acpiEnumOutputBuffer = NULL;
    ULONG acpiEnumOutputSize;
    PACPI_EVAL_OUTPUT_BUFFER acpiEvalOutputBuffer = NULL;
    ULONG acpiEvalOutputSize;
    PACPI_ENUM_CHILD acpiEnumChild;
    ULONG childIndex;
    ANSI_STRING acpiDeviceName;
    USHORT port;
    PACPI_METHOD_ARGUMENT acpiPackageElement;
    ULONG elementIndex;
    ACPI_UPC_PACKAGE acpiUpcPackage;
    PUCHAR acpiUpcPackageField;
    PACPI_PLD_BUFFER acpiPldBuffer;
    PDEVICE_EXTENSION_HUB  hubExt;

    // define field offset mapping for UPC package structure
    const struct _ACPI_UPC_PACKAGE_FIELD {
        ULONG Offset;
        ULONG Size;
    } acpiUpcPackageFields[] = {
        { FIELD_OFFSET(ACPI_UPC_PACKAGE, PortIsConnectable), sizeof(UCHAR) },
        { FIELD_OFFSET(ACPI_UPC_PACKAGE, PortConnectorType), sizeof(UCHAR) },
        { FIELD_OFFSET(ACPI_UPC_PACKAGE, Reserved0),         sizeof(ULONG) },
        { FIELD_OFFSET(ACPI_UPC_PACKAGE, Reserved1),         sizeof(ULONG) },
    };

    hubExt = FdoExt(HubFdo);

    // query size of output buffer required to enumerate ACPI device children
    nts = UsbhAcpiEnumChildren(HubFdo,
                               &acpiEnumOutputSizeBuffer,
                               sizeof(ACPI_ENUM_CHILDREN_OUTPUT_BUFFER));

    if (nts != STATUS_BUFFER_OVERFLOW) {
        return nts;
    }

    if (acpiEnumOutputSizeBuffer.NumberOfChildren < sizeof(ACPI_ENUM_CHILDREN_OUTPUT_BUFFER)) {
        return STATUS_ACPI_INVALID_DATA;
    }

    // allocate output buffer to hold all enumerated ACPI device children
    acpiEnumOutputSize = acpiEnumOutputSizeBuffer.NumberOfChildren;

    UsbhAllocatePool_Z(acpiEnumOutputBuffer, NonPagedPool, acpiEnumOutputSize);

    if (acpiEnumOutputBuffer == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // enumerate ACPI device children
    nts = UsbhAcpiEnumChildren(HubFdo,
                               acpiEnumOutputBuffer,
                               acpiEnumOutputSize);

    if (!NT_SUCCESS(nts)) {
        goto UsbhGetAcpiPortAttributes_Done;
    }

    // allocate output buffer to hold arbitrary results from method evaluation
    acpiEvalOutputSize = sizeof(ACPI_EVAL_OUTPUT_BUFFER) + ACPI_METHOD_OUTPUT_BUFFER_SIZE;

    UsbhAllocatePool_Z(acpiEvalOutputBuffer, NonPagedPool, acpiEvalOutputSize);

    if (acpiEvalOutputBuffer == NULL) {
        nts = STATUS_INSUFFICIENT_RESOURCES;
        goto UsbhGetAcpiPortAttributes_Done;
    }

    if (hubExt->AcpiAttributes == NULL) {
        UsbhAllocatePool_Z(hubExt->AcpiAttributes,
                           NonPagedPool,
                           sizeof(USB_ACPI_ATTRIBUTES)*(N_PORTS(HubFdo)+1));
    }

    // skip over first ACPI device child that represents the hub
    acpiEnumChild = &acpiEnumOutputBuffer->Children[0];

    for (childIndex = 1;
         childIndex < acpiEnumOutputBuffer->NumberOfChildren;
         childIndex++) {

        // get next ACPI child
        acpiEnumChild = ACPI_ENUM_CHILD_NEXT(acpiEnumChild);

        // skip ACPI child without any children since there is no ADR object
        if ((acpiEnumChild->Flags & ACPI_OBJECT_HAS_CHILDREN) == 0) {
            continue;
        }

        if (acpiEnumChild->NameLength == 0) {
            continue;
        }

        // initialize ACPI child device name
        acpiDeviceName.Buffer = acpiEnumChild->Name;
        acpiDeviceName.MaximumLength = (USHORT)acpiEnumChild->NameLength;
        acpiDeviceName.Length = acpiDeviceName.MaximumLength - 1;

        // evaluate ADR method on ACPI child
        nts = UsbhAcpiEvalAcpiMethodEx(HubFdo,
                                       &acpiDeviceName,
                                       (ULONG)'RDA_',
                                       acpiEvalOutputBuffer,
                                       acpiEvalOutputSize);

        if (!NT_SUCCESS(nts)) {
            nts = STATUS_SUCCESS;
            continue;
        }

        // validate that output buffer contains an integer argument
        if ((acpiEvalOutputBuffer->Count == 0) ||
            (acpiEvalOutputBuffer->Argument[0].Type != ACPI_METHOD_ARGUMENT_INTEGER)) {
            continue;
        }

        // map ADR address value to a valid port on the hub
        port = (USHORT)acpiEvalOutputBuffer->Argument[0].Argument;

        if ((port < 1) || (port > N_PORTS(HubFdo))) {
            continue;
        }

        // evaluate UPC (USB port capabilities) method on ACPI child
        nts = UsbhAcpiEvalAcpiMethodEx(HubFdo,
                                       &acpiDeviceName,
                                       (ULONG)'CPU_',
                                       acpiEvalOutputBuffer,
                                       acpiEvalOutputSize);

        if (!NT_SUCCESS(nts)) {
            UsbhEtwLogHubEvent(HubFdo,
                   &USBHUB_ETW_EVENT_HUB_PORT_ACPI_UPC_FAILURE,
                   port,
                   nts);

            nts = STATUS_SUCCESS;
            continue;
        }

        // validate that output buffer contains package arguments
        if (acpiEvalOutputBuffer->Count == 0) {
            UsbhEtwLogHubEvent(HubFdo,
                   &USBHUB_ETW_EVENT_HUB_PORT_ACPI_UPC_FAILURE,
                   port,
                   STATUS_ACPI_INCORRECT_ARGUMENT_COUNT);

            DbgTrace((UsbhDebugTrace,"%!FUNC! port %d has invalid _UPC\n", port));
            continue;
        }

        // traverse package elements to populate UPC package fields
        RtlZeroMemory(&acpiUpcPackage, sizeof(ACPI_UPC_PACKAGE));

        for (acpiPackageElement = acpiEvalOutputBuffer->Argument,
             elementIndex = 0;
             ((PUCHAR)acpiPackageElement < (PUCHAR)acpiEvalOutputBuffer + acpiEvalOutputBuffer->Length) &&
             (elementIndex < acpiEvalOutputBuffer->Count);
             acpiPackageElement = ACPI_METHOD_NEXT_ARGUMENT(acpiPackageElement),
             elementIndex++) {
            // check for excess package elements
            if (elementIndex >= RTL_NUMBER_OF_V1(acpiUpcPackageFields)) {
                nts = STATUS_ACPI_INCORRECT_ARGUMENT_COUNT;
                break;
            }

            // package elements should all be integers
            if (acpiPackageElement->Type != ACPI_METHOD_ARGUMENT_INTEGER) {
                nts = STATUS_ACPI_INVALID_ARGTYPE;
                break;
            }

            // get offset into UPC package field
            __analysis_assume(acpiUpcPackageFields[elementIndex].Offset <= sizeof(ACPI_UPC_PACKAGE) - sizeof(ULONG));

            acpiUpcPackageField = (PUCHAR)&acpiUpcPackage + acpiUpcPackageFields[elementIndex].Offset;

            // store package element data in UPC package field
            if (acpiUpcPackageFields[elementIndex].Size == sizeof(UCHAR)) {
                *acpiUpcPackageField = (UCHAR)(acpiPackageElement->Argument & 0xFF);
            } else if (acpiUpcPackageFields[elementIndex].Size == sizeof(ULONG)) {
                *(PULONG)acpiUpcPackageField = acpiPackageElement->Argument;
            } else {
                nts = STATUS_ACPI_INVALID_DATA;
                break;
            }
        }

        if (!NT_SUCCESS(nts)) {
            UsbhEtwLogHubEvent(HubFdo,
                   &USBHUB_ETW_EVENT_HUB_PORT_ACPI_UPC_FAILURE,
                   port,
                   nts);

            nts = STATUS_SUCCESS;
            continue;
        }

        if (hubExt->AcpiAttributes) {
            RtlCopyMemory(&hubExt->AcpiAttributes[port].UpcPackage, &acpiUpcPackage, sizeof(ACPI_UPC_PACKAGE));
            AttrList[port].ul |= USB_PORTATTR_ACPI_UPC_VALID_BIT;
        }

        // set port attributes based on UPC information
        AttrList[port].ul &= ~(USB_PORTATTR_NO_CONNECTOR_BIT |
                               USB_PORTATTR_SHARED_USB2_BIT |
                               USB_PORTATTR_MINI_CONNECTOR_BIT |
                               USB_PORTATTR_OEM_CONNECTOR_BIT);

        if (acpiUpcPackage.PortIsConnectable) {
            // port is connectable, so any device connected to it is removable
            AttrList[port].DeviceNotRemovable = 0;

            // select port connector type
            switch (acpiUpcPackage.PortConnectorType) {
            case ACPI_UPC_PORT_CONNECTOR_TYPE_A:
                // standard type-A connector is the default
                break;

            case ACPI_UPC_PORT_CONNECTOR_MINI_AB:
                AttrList[port].ul |= USB_PORTATTR_MINI_CONNECTOR_BIT;
                break;

            case ACPI_UPC_PORT_CONNECTOR_EXPRESS_CARD:
                AttrList[port].ul |= USB_PORTATTR_SHARED_USB2_BIT;
                break;

            case ACPI_UPC_PORT_CONNECTOR_OEM:
                AttrList[port].ul |= USB_PORTATTR_OEM_CONNECTOR_BIT;
                break;

            default:
                DbgTrace((UsbhDebugTrace,
                          "%!FUNC! port %d has invalid _UPC.PortConnectorType (0x%02x)\n",
                          port, acpiUpcPackage.PortConnectorType));
                break;
            }
        } else {
            // port is not connectable, so any device connected to it is not removable
            AttrList[port].DeviceNotRemovable = 1;

            // port has no connector when it is not connectable
            AttrList[port].ul |= USB_PORTATTR_NO_CONNECTOR_BIT;

            // no need to evaluate PLD method since it will not change the
            // final removable state of this port
            continue;
        }

        // evaluate PLD (physical location descriptor) method on ACPI child
        nts = UsbhAcpiEvalAcpiMethodEx(HubFdo,
                                       &acpiDeviceName,
                                       (ULONG)'DLP_',
                                       acpiEvalOutputBuffer,
                                       acpiEvalOutputSize);

        if (!NT_SUCCESS(nts)) {
            UsbhEtwLogHubEvent(HubFdo,
                   &USBHUB_ETW_EVENT_HUB_PORT_ACPI_PLD_FAILURE,
                   port,
                   nts);
            nts = STATUS_SUCCESS;
            continue;
        }

        // validate that output buffer contains a buffer argument
        if ((acpiEvalOutputBuffer->Count == 0) ||
            (acpiEvalOutputBuffer->Argument[0].Type != ACPI_METHOD_ARGUMENT_BUFFER) ||
            (acpiEvalOutputBuffer->Argument[0].DataLength < sizeof(ACPI_PLD_BUFFER))) {
            DbgTrace((UsbhDebugTrace,"%!FUNC! port %d has invalid _PLD\n", port));

            UsbhEtwLogHubEvent(HubFdo,
                   &USBHUB_ETW_EVENT_HUB_PORT_ACPI_PLD_FAILURE,
                   port,
                   nts);
            continue;
        }

        // map PLD buffer bitfield over buffer data
        acpiPldBuffer = (PACPI_PLD_BUFFER)acpiEvalOutputBuffer->Argument[0].Data;

        if (hubExt->AcpiAttributes) {
            RtlCopyMemory(&hubExt->AcpiAttributes[port].PldBuffer, acpiPldBuffer,  sizeof(ACPI_PLD_BUFFER));
            AttrList[port].ul |= USB_PORTATTR_ACPI_PLD_VALID_BIT;
        }

        // device on this port cannot be removed if the port itself is not
        // visible to the user, even if it is identified as connectable by UPC
        if (!acpiPldBuffer->UserVisible) {
            AttrList[port].DeviceNotRemovable = 1;
        }

        //
        // At least one port has a valid _PLD, so mark the hub as being in the ACPI namespace
        //
        hubExt->FdoFlags2.InAcpiNamespace = 1;
    }

UsbhGetAcpiPortAttributes_Done:

    // free output buffers
    if (acpiEvalOutputBuffer != NULL) {
        UsbhFreePool(acpiEvalOutputBuffer);
    }

    if (acpiEnumOutputBuffer != NULL) {
        UsbhFreePool(acpiEnumOutputBuffer);
    }

    return nts;
}


NTSTATUS
UsbhGetExtendedHubInformation(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Retrieve extra information about the hub ports from Registry, ACPI or the
    hub itself.

    Attributes are stored in an array. Indexed by port number.  entry 0 i snot used.

    attr[0] - not used
    attr[1] - attributes for port 1
    attr[2] = attributes for port 2

    ...

Arguments:

Return Value:

    support for these attributes is optional so we don't fail unless we cannot
    get memory.

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    PUSB_EXTENDED_PORT_ATTRIBUTES attrList;
    USHORT p;
    PHUB_PORT_DATA portData;

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));

    UsbhAllocatePool_Z(attrList, NonPagedPool, sizeof(union _USB_EXTENDED_PORT_ATTRIBUTES) *
                    (N_PORTS(HubFdo)+1));

    hubExt->ExtendedPortAttributes = attrList;

    if (attrList) {
        // attempt to fill in the attribute list, use priority order

        //TBD
        //UsbhGetDescriptorPortAttributes(HubFdo, attrList);
        //UsbhGetRegistryPortAttributes(HubFdo, attrList);


        for(p=1; p <= N_PORTS(HubFdo); p++) {
            // check for embedded devices
            // take the first change bit we see
            if (IsBitSet(hubExt->HubDescriptor.bRemoveAndPowerMask, p)) {
                DbgTrace((UsbhDebugTrace,"%!FUNC! device on %d is not removable\n", p));
                attrList[p].DeviceNotRemovable = 1;
            }
        }

        // evaluate ACPI port attributes at the end so that they have
        // the final word on the DeviceNotRemovable bits
        UsbhGetAcpiPortAttributes(HubFdo, attrList);

        // Populate the Connector ID for each port if we got it.
        if (hubExt->AcpiAttributes) {

            for(p=1; p <= N_PORTS(HubFdo); p++) {

                if (!attrList[p].AcpiPldValid) {
                    continue;
                }

                portData = UsbhGetPortData(HubFdo, p);

                if (!portData) {
                    continue;
                }

                portData->UcmConnectorId =
                    UCM_CONNECTOR_ID_FROM_ACPI_PLD(&hubExt->AcpiAttributes[p].PldBuffer);
            }
        }

        // Log the port attributes to ETW
        UsbhEtwLogPortInformation(HubFdo);

        nts = STATUS_SUCCESS;
    } else {
        nts = STATUS_INSUFFICIENT_RESOURCES;
    }

    return nts;
}


VOID
UsbhUninitialize(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    undue anything we did in initialize, called from Pnp stop

Arguments:


Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_OBJECT oldRefObj;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_HUB, 'hUNI', 0, 0);
    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));
    UsbhDisarmHubForWakeDetect(HubFdo);
    UsbhUnregisterPowerCallback(HubFdo);

    UsbhFreePool(hubExt->ExtendedPortAttributes);

    if (hubExt->AcpiAttributes) {
        UsbhFreePool(hubExt->AcpiAttributes);
    }

    oldRefObj = InterlockedExchangePointer(
            &hubExt->TopOfBusStack,
            NULL);

    LOG(HubFdo, LOG_HUB, 'huRF', 0, oldRefObj);

    if (oldRefObj) {
        // remove reference to top of the usb device stack
        DEREF_HUB(HubFdo, oldRefObj);
        ObDereferenceObject(oldRefObj);
    }
    LOG(HubFdo, LOG_HUB, 'hui>', 0, 0);

}


NTSTATUS
UsbhInitialize(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    called from START_DEVICE, initializes an instance of the hub driver.

    This function performs the various tedious functions associated with
    setting up the hub driver before we start any modules.

Arguments:


Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    ULONG hubCount;
    USB20_HUB_CHAR_BITS hubChars;
    ULONG retry = 0;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_HUB, 'hINI', 0, 0);
    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));

    // note that we already know if we are root.
    // get the top of the device stack for forwarding bus requests
    nts = UsbhGetTopOfBusStack(HubFdo);

    if (NT_ERROR(nts)) {
        REPORT_START_FAILURE(HubFdo, nts, DeviceGeneralFailure,
            "GetRootHubPdo");
        return nts;
    }

    //
    // allow no more than five physical hubs plus the root for electrical
    // reasons. (7.1.16)
    //
    nts = UsbhGetHubCount(HubFdo, &hubCount);
    if (NT_ERROR(nts)) {
        REPORT_START_FAILURE(HubFdo, nts, DeviceGeneralFailure,
            "GetHubCount");
        return nts;
    }

    if (hubCount > 6) {
        nts = STATUS_UNSUCCESSFUL;
        REPORT_START_FAILURE(HubFdo, nts, DeviceHubNestedTooDeeply,
            "hubCount > 6");
        return nts;
    }

    // Get the speed, type and device descriptor from the bus driver.
    //
    // We need this information to determine how to configure the device.
    nts = UsbhGetHubDeviceInformation(HubFdo, StateContext);
    if (NT_ERROR(nts)) {
        REPORT_START_FAILURE(HubFdo, nts, DeviceGeneralFailure,
            "GetHubDeviceInformation");
        return nts;
    }


    // if the hub is a 2.0 device running at high speed then it must have one
    // or more TTs.  If it is a root hub, then it may support TTs, and that
    // will be determined later when we parse the configuration descriptor that
    // usbport created.

    if (hubExt->DeviceType == Usb20Device &&
        hubExt->DeviceSpeed == UsbHighSpeed) {

        hubExt->FdoFlags.HubIsTT = 1;
    }

    if (hubExt->DeviceType == Usb20Device) {

        // hub is USB 2, determine if it is also high speed capable
        // I'm not sure if there really is such a thing as a USB 2.0 that is
        // not but it is valid.
        if (UsbhIsHighSpeedCapable(HubFdo)) {
            hubExt->FdoFlags.HighSpeedCapable = 1;
        }

    }

    UsbhCheckHubErrata(hubExt);

    // Configure the USB hub, this opens our pipes and sets the configuration
    nts = UsbhConfigureUsbHub(HubFdo);
    if (NT_ERROR(nts)) {
        REPORT_START_FAILURE(HubFdo, nts, DeviceGeneralFailure,
        "ConfigureUsbHub");

        return nts;
    }

    do {
        // since the hub is now configured we can get the hub descriptor
        // note: the original hub driver retrieved his before configuring the
        // device but this is wrong since the hub may report a different descriptor
        // depending on the configuration and interface selected.
        nts = UsbhGetHubClassDescriptor(HubFdo, &hubExt->HubDescriptor);

        if (Usb_Disconnected(nts)) {
            // this can fail legitimately if the hub was pulled.
            return nts;
        }

        if (NT_ERROR(nts) && retry>3) {
            REPORT_START_FAILURE(HubFdo, nts, DeviceGeneralFailure,
                "GetHubClassDescriptor");

            return nts;
        }
        if (retry) {
            UsbhWait(HubFdo, 10);
        }
        retry++;
    } while (NT_ERROR(nts));

    UsbhAssert(HubFdo, hubExt->FdoFlags.HubDescriptorValid);

    // copy of wChars modified as appropriate
    hubChars.us = hubExt->HubDescriptor.wHubCharacteristics;
    // set power switch mode based on hub characteristics and USB rev.
    // we won't try to support power switching for USB 1.1 and earlier since
    // it is not reliable.
    LOG(HubFdo, LOG_HUB, 'hINI', hubChars.us, hubExt->DeviceDescriptor.bcdDevice );

    if (hubExt->DeviceDescriptor.bcdUSB >= 0x0200) {

        // xlate the port chars into our extension
        if (hubChars.PortPowerSwitched) {
            hubExt->PortPowerSwitched = TRUE;
        }

        if (hubChars.PerPortOvercurrent) {
            hubExt->PerPortOvercurrent = TRUE;
        }

        if (hubChars.PortIndicators) {
            hubExt->PortIndicators = TRUE;
        }

        if (hubExt->FdoFlags.HubIsTT) {
            switch(hubChars.TT_ThinkTime) {
            case 0:
                hubExt->TT_ThinkTime = 8;
                break;
            case 1:
                hubExt->TT_ThinkTime = 16;
                break;
            case 2:
                hubExt->TT_ThinkTime = 24;
                break;
            case 3:
                hubExt->TT_ThinkTime = 32;
                break;
            default:
                // default to 8
                hubExt->TT_ThinkTime = 8;
            }
        }
    }

    // get information about how much power this hub can supply to its
    // ports, its either 500ma or 100ma.
    // default is already set before calling this function

    UsbhGetHubPowerStatus(HubFdo);

    UsbhAssert(HubFdo, hubExt->MilliampsPowerPerPort > 0);

    // if this is a USB 20 hub at high speed then we need to initialize the TTs
    if (hubExt->FdoFlags.HubIsTT) {
        nts = UsbhInitializeTtHub(HubFdo, StateContext);
        if (NT_ERROR(nts)) {
            REPORT_START_FAILURE(HubFdo, nts, DeviceGeneralFailure,
                "InitializeTtHub");
            return nts;
        }
    }

    // get any extra information (not part of the specification) for this hub.
    // this includes things like the ports physical attributes reported thru
    // ACPI or extended descriptors.
    // combine this info with any contained in the hub descriptor
    nts = UsbhGetExtendedHubInformation(HubFdo);
    if (NT_ERROR(nts)) {
        REPORT_START_FAILURE(HubFdo, nts, DeviceGeneralFailure, "Get Port attributes");
        return nts;
    }


    Usbh_UsbdQueryContollerType(HubFdo);

    UsbhAssert(HubFdo, NT_SUCCESS(nts));

    // initialize misc flags based on reg keys
    if (HubG.GlobalDisableHardReset) {
        UsbhKdPrint((1, "** 'ESD' support DISABLED **\n"));
        hubExt->FdoFlags.EsdEnabled = 0;
    } else {
        UsbhKdPrint((1, "** 'ESD' support ENABLED **\n"));
        hubExt->FdoFlags.EsdEnabled = 1;
    }

    // successful start has us in D0 (implied D0)
    SET_FDO_SX(HubFdo, PowerSystemWorking);
    SET_FDO_DX(HubFdo, PowerDeviceD0);
    hubExt->CurrentFdoPwrState = FdoS0_D0;


    //
    // Initially the hub is in D0. Pending list is
    // disabled. When the Hub goes to D3, we will
    // enable it.
    //
    KeInitializeSpinLock(&hubExt->LockPdoD0List);
    hubExt->PendingPdoD0ListEnabled = FALSE;
    InitializeListHead(&hubExt->PendingPdoD0List);



    // set up WAIT_WAKE
    // Adriano sez "Arm Early, Arm Often" however we do not do this
    // instead we choose to arm the hub only when necessary.  Since we maintain
    // tight control over pnp and power we can do this without risking race conditions
    // with unsynchronized power irps.

    //
    // currently external hubs are always set up to wake the system on connect
    // events.
    // Root hubs default to not enabling wake on connect.
    // *The UHCI controller does not support an option to disable this
    //
    // The behavior most closely matches the USB stack in WinXP and Win2k.
    //

    INIT_EVENT_SIGNALED(&hubExt->PendingWakeIrpEvent);
    INIT_SYNC_EVENT_SIGNALED(&hubExt->IoctlLock);

    hubExt->HubWakeState = HUB_WAKESTATE_DISARMED;

    UsbhQueryParentHubConfig(HubFdo);

    UsbhQueryTopologyAddress(HubFdo);

    hubExt->IdleTimeoutInMilliSeconds = HubG.IdleTimeout;

    UsbhRegisterPowerCallback(HubFdo);

    hubExt->FdoFlags.HubInitialized = 1;

    return nts;
}


NTSTATUS
UsbhReinitialize(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Called on a restart, this routine will reconfigure the hub as part of a cold start.

    Note:
        we could use this routine on a return from hibernate instead of relying on restore
        device to reset the original vconfiguartion for the hub.

Arguments:


Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;

    LOG(HubFdo, LOG_HUB, 'hRNI', StateContext, 0);
    hubExt = FdoExt(HubFdo);

    // Configure the USB hub, this opens our pipes and sets the configuration
    nts = UsbhConfigureUsbHub(HubFdo);
    if (NT_ERROR(nts)) {
        REPORT_START_FAILURE(HubFdo, nts, DeviceGeneralFailure,
            "ConfigureUsbHub");
        return nts;
    }

    // we need to re-init the TTs as well
     // if this is a USB 20 hub at high speed then we need to initialize the TTs
    if (hubExt->FdoFlags.HubIsTT) {
        nts = UsbhInitializeTtHub(HubFdo, StateContext);
        if (NT_ERROR(nts)) {
            REPORT_START_FAILURE(HubFdo, nts, DeviceGeneralFailure,
                "InitializeTtHub");
            return nts;
        }
    }

    return nts;
}


PHUB_WORKITEM
UsbhAllocWorkItem(
    PDEVICE_OBJECT HubFdo,
    PHUB_WORKITEM_ROUTINE WorkerRoutine,
    PVOID Context,
    ULONG PortNumber,
    ULONG Sig,
    BOOLEAN ForwardProgress
    )
/*++

Routine Description:

    This is a wrapper function for IoAllocateWorkitem.  We alloc our own structure so that verifier
    can catch the leaks.  This also makes debugging easier.

Arguments:

    PortNumber - zero if hub
    Sig - 4 byte ascii tag for debugging

Return Value:

    NULL if item can not be allocated

--*/
{
    USB_WORKITEM_HANDLE ioWi;
    PHUB_WORKITEM hubWi;
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    if (SIG_SSH_WORKER != Sig) {
        //skip logging for SSP
        LOG(HubFdo, LOG_HUB, 'aWKM', PortNumber, Context);
    }

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!\n", PortNumber));

    UsbhAllocatePool_Z(hubWi, NonPagedPool, sizeof(struct _HUB_WORKITEM));
    if (hubWi) {
        ioWi = Usbh_HubAllocateWorkItem(HubFdo, HubFdo, ForwardProgress);

        if (ioWi) {
            hubWi->IoWi = ioWi;
            hubWi->WiSig = SIG_HUB_WORKITEM;
            hubWi->Sig = Sig;
            hubWi->PortNumber = PortNumber;
            hubWi->Context = Context;
            hubWi->HubFdo = HubFdo;
            hubWi->WorkerRoutine = WorkerRoutine;
            ExInterlockedInsertTailList(&hubExt->WorkItemListHead,
                                &hubWi->WiLink,
                                &hubExt->WorkItemListSpin);

            if (SIG_SSH_WORKER != hubWi->Sig) {
                //skip logging for SSP
                LOG(HubFdo, LOG_HUB, 'WKM+', 0, hubWi);
            }

            return hubWi;
        } else {
            UsbhFreePool(hubWi);
        }
    }

    return NULL;
}


VOID
UsbhHubWorker(
    PDEVICE_OBJECT HubFdo,
    PVOID Context,
    PIO_WORKITEM IoWorkItem
    )
/*++

Routine Description:

    Generic worker callback.

Irql: PASSIVE

Arguments:

    Context - pointer to hub workitem

Return Value:

    none

--*/
{
    PHUB_WORKITEM hubWi;
    PDEVICE_EXTENSION_HUB  hubExt;
    USB_WORKITEM_HANDLE ioWi;
    ULONG s;

    UsbhAssertPassive();
    hubWi = Context;

    ASSERT_HUBWI(NULL, hubWi);
    if (SIG_SSH_WORKER != hubWi->Sig) {
        //skip logging for SSP
        LOG(HubFdo, LOG_HUB, 'hWKR', hubWi->Sig, hubWi);
    }

    hubExt = FdoExt(hubWi->HubFdo);

    s = InterlockedIncrement((PLONG)&hubWi->State);
    UsbhAssert(HubFdo, s == 2);

    // note the thread we are on
    hubWi->WiThread = KeGetCurrentThread();
    // invoke the callback
    hubWi->WorkerRoutine(hubWi->HubFdo, hubWi->PortNumber, hubWi->Context);
    UsbhAssertPassive();

    // this is no longer considered queued
    INTERLOCKED_REMOVE_ENTRY_LIST(&hubExt->WorkItemListSpin, &hubWi->WiLink);

    ioWi = hubWi->IoWi;
    if (SIG_SSH_WORKER != hubWi->Sig) {
        //skip logging for SSP
        LOG(HubFdo, LOG_HUB, 'hWKf', 0, hubWi);
    }
    UsbhFreePool(hubWi);

    Usbh_HubFreeWorkItem(HubFdo, ioWi);

    UNREFERENCED_PARAMETER(IoWorkItem);
}


NTSTATUS
UsbhQueueWorkItem(
    PDEVICE_OBJECT HubFdo,
    WORK_QUEUE_TYPE QueueType,
    PHUB_WORKITEM_ROUTINE WorkerRoutine,
    PVOID Context,
    ULONG PortNumber,
    ULONG Sig
    )
/*++

Routine Description:

    Wrapper for UsbhQueueWorkItemEx()

Arguments:

    PortNumber - zero if hub
    Sig - 4 byte ascii tag for debugging
    WorkerRoutine - workitem callback routine
    Context - context passed to callback

Return Value:

    error if item cannot be allocated

--*/
{
    return UsbhQueueWorkItemEx(HubFdo,
                               QueueType,
                               WorkerRoutine,
                               Context,
                               PortNumber,
                               Sig,
                               NULL);
}


NTSTATUS
UsbhQueueWorkItemEx(
    PDEVICE_OBJECT HubFdo,
    WORK_QUEUE_TYPE QueueType,
    PHUB_WORKITEM_ROUTINE WorkerRoutine,
    PVOID Context,
    ULONG PortNumber,
    ULONG Sig,
    PHUB_WORKITEM HubWi
    )
/*++

Routine Description:

    This routine optionally allocates, and then queues a hub workitem, if the item cannot be allocated an error
    is returned.

    Unlike the kernel API the WorkerRoutine does not need to free the workitem -- just the context

Arguments:

    PortNumber - zero if hub
    Sig - 4 byte ascii tag for debugging
    WorkerRoutine - workitem callback routine
    Context - context passed to callback

Return Value:

    error if item cannot be allocated

--*/
{
    PHUB_WORKITEM hubWi;
    PDEVICE_EXTENSION_HUB  hubExt;
    ULONG s;
    BOOLEAN forwardProgress;

    hubExt = FdoExt(HubFdo);
    forwardProgress = hubExt->IsBootDevice;

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!\n", PortNumber));
    if (SIG_SSH_WORKER != Sig) {
        // ssp spams the log
        LOG(HubFdo, LOG_HUB, 'aQWM', Sig, Context);
    }

    if (!HubWi) {
        hubWi = UsbhAllocWorkItem(HubFdo,
                                  WorkerRoutine,
                                  Context,
                                  PortNumber,
                                  Sig,
                                  forwardProgress);
    } else {
        hubWi = HubWi;
    }

    if (hubWi) {
        s = InterlockedIncrement((PLONG)&hubWi->State);
        UsbhAssert(HubFdo, s == 1);
        UsbhAssert(HubFdo, hubWi->IoWi);

        DbgTrace((UsbhDebugTrace,"#%d %!FUNC! (IoQueue)\n", PortNumber));
        if (SIG_SSH_WORKER != Sig) {
            // ssp spams the log
            LOG(HubFdo, LOG_HUB, 'QTM+', Sig, hubWi);
        }

        return Usbh_HubQueueWorkItem(HubFdo,
                                     HubFdo,
                                     hubWi->IoWi,
                                     UsbhHubWorker,
                                     QueueType,
                                     hubWi,
                                     forwardProgress);
    }

    return STATUS_INSUFFICIENT_RESOURCES;
}


ULONG
UsbhQueueWorkItemWithRetry(
    PDEVICE_OBJECT HubFdo,
    PHUB_TIMER_OBJECT TimerObject,
    PHUB_WORKITEM_ROUTINE WorkerRoutine,
    WORK_QUEUE_TYPE QueueType,
    PVOID Context,
    ULONG PortNumber,
    ULONG Sig
    )
/*++

Routine Description:

    Same as UsbhQueueWorkitem but this one doesn't fail.  If we cannot allocate we queue a DPC and
    retry.

Arguments:

    PortNumber - zero if hub
    Sig - 4 byte ascii tag for debugging
    WorkerRoutine - workitem callback routine
    Context - context passed to callback

Return Value:

    returns retry status, zero = no retry

--*/
{

    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    ULONG retry = 0;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_HUB, 'QTMC', Sig, Context);
    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!>\n", PortNumber));

    //TestPt(TstPt_UsbHub_Fail_UsbhQueueDelayedWorkItem, nts);
    nts = STATUS_SUCCESS;
    if (NT_SUCCESS(nts)) {
        nts = UsbhQueueWorkItem(HubFdo,
                                //CriticalWorkQueue,
                                //DelayedWorkQueue,
                                QueueType,
                                WorkerRoutine,
                                Context,
                                PortNumber,
                                Sig);
    }


    if (NT_ERROR(nts)) {
        DbgTrace((UsbhDebugTrace,"! #%d %!FUNC! %!STATUS!\n", PortNumber, nts));
        LOG(HubFdo, LOG_HUB, 'QTC!', Sig, Context);
TEST_TRAP();
        // enable retries on first try
        if (TimerObject) {
            UsbhEnableTimerObject(HubFdo,
                                   TimerObject,
                                   0, // retry asap
                                   PortNumber,
                                   Context,
                                   Sig,
                                   0);
        }

        retry = 1;

    }

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!< retry = %d\n", PortNumber, retry));

    return retry;

}


ULONG
UsbhQueueDelayedWorkItem(
    PDEVICE_OBJECT HubFdo,
    PHUB_TIMER_OBJECT TimerObject,
    PHUB_WORKITEM_ROUTINE WorkerRoutine,
    PVOID Context,
    ULONG PortNumber,
    ULONG Sig
    )
/*++

Routine Description:

    Same as UsbhQueueWorkitem but this one doesn't fail.  If we cannot allocate we queue a DPC and
    retry, this function is a wrapper that queues the workitem at delayed priority.

    For most cases we use the delayed work queue -- it turns out that in most pnp cases its better to
    allow our worker routines to be pre-empted. This allows the user mode parts of PnP to 'catch up'
    to us and the result is shorter wait overall. This is particularly true where replicants are
    involved.

Arguments:

    PortNumber - zero if hub
    Sig - 4 byte ascii tag for debugging
    WorkerRoutine - workitem callback routine
    Context - context passed to callback

Return Value:

    returns retry status, zero = no retry

--*/
{

    return UsbhQueueWorkItemWithRetry(HubFdo,
                                     TimerObject,
                                     WorkerRoutine,
                                     DelayedWorkQueue,
                                     Context,
                                     PortNumber,
                                     Sig);
}


ULONG
UsbhQueueCriticalWorkItem(
    PDEVICE_OBJECT HubFdo,
    PHUB_TIMER_OBJECT TimerObject,
    PHUB_WORKITEM_ROUTINE WorkerRoutine,
    PVOID Context,
    ULONG PortNumber,
    ULONG Sig
    )
/*++

Routine Description:

    Same as UsbhQueueWorkitem but this one doesn't fail.  If we cannot allocate we queue a DPC and
    retry, this function is a wrapper that queues the workitem at critical priority.

Arguments:

    PortNumber - zero if hub
    Sig - 4 byte ascii tag for debugging
    WorkerRoutine - workitem callback routine
    Context - context passed to callback

Return Value:

    returns retry status, zero = no retry

--*/
{

    return UsbhQueueWorkItemWithRetry(HubFdo,
                                     TimerObject,
                                     WorkerRoutine,
                                     CriticalWorkQueue,
                                     Context,
                                     PortNumber,
                                     Sig);
}



PHUB_WORKITEM
UsbhFindPendingWorkItem(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    ULONG Sig
    )
/*++

Routine Description:

    Locates a pending workitem by sig and port number.

Arguments:

    PortNumber - zero if hub
    Sig - 4 byte ascii tag for debugging

Return Value:

    NULL if not found

--*/
{
    PHUB_WORKITEM hubWi = NULL;
    PDEVICE_EXTENSION_HUB  hubExt;
    KIRQL irql;
    PLIST_ENTRY le;

    hubExt = FdoExt(HubFdo);

    KeAcquireSpinLock(&hubExt->WorkItemListSpin, &irql);

    GET_HEAD_LIST(hubExt->WorkItemListHead, le);

    while (le &&
           le != &hubExt->WorkItemListHead) {

        hubWi = (PHUB_WORKITEM) CONTAINING_RECORD(
                    le,
                    struct _HUB_WORKITEM,
                    WiLink);

        LOG(HubFdo, LOG_HUB, 'lswi', hubWi, 0);

        ASSERT_HUBWI(HubFdo, hubWi);

        if (hubWi->PortNumber == PortNumber &&
            hubWi->Sig == Sig) {
            break;
        }
        le = hubWi->WiLink.Flink;
        hubWi = NULL;
    }

    KeReleaseSpinLock(&hubExt->WorkItemListSpin, irql);

    return hubWi;
}

NTSTATUS
UsbhQueryTopologyAddress(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Queries our parent for our address.  We will report this for our PDOs
    along with their port numbers when requested.

Arguments:

    HubFdo - hub instance (optional)

--*/
{
    PDEVICE_EXTENSION_HUB hubExt = FdoExt(HubFdo);

    return UsbhSyncSendInternalIoctl(HubFdo,
                                     IOCTL_INTERNAL_USB_GET_TOPOLOGY_ADDRESS,
                                     &hubExt->TopologyAddress,
                                     NULL);
}


__declspec(noreturn) void
UsbhTrapFatalTimeout_x9f(
    __in_opt PDEVICE_OBJECT HubFdo,
    __in USBHUB_FATAL_TIMEOUT_CODE TimeoutCode,
    __in_opt PVOID TimeoutContext
    )
 /*++

Routine Description:

    Traps timeouts that would normally result in bugcheck 0x9f

Arguments:

    Timeout_Code - indicates the source of the fatal timeout


Return Value:

    none
--*/
{

    UsbhKdPrint((0, "UsbhTrapFatalTimeout_x9f devobj, %x TimeoutCode %x TimeoutContext:%x\n",
        HubFdo, TimeoutCode, TimeoutContext));

    if(HubFdo)
    {
        USBHUB_TriggerCallBacks(HubFdo);
    }
    // only bugcheck if we don't have debug bits
    // throw bugcheck for OCA data collection
    KeBugCheckEx(BUGCODE_USB_DRIVER,
                 USBBUGCODE_RESERVED_USBHUB,
                 USBHUB_TRAP_FATAL_TIMEOUT,
                 (ULONG_PTR) TimeoutCode,
                 (ULONG_PTR) TimeoutContext);


}
