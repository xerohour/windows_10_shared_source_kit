/*
Module: SSDEV.C


name:USB Device Selective Suspend

    Copyright (c) Microsoft Corporation. All rights reserved

created:

    1-2-2007

Abstract:

    This code implements the Device Selective Suspend 2.0 functionality in USBHUB.SYS.


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

** begin ss doc

IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION (USB IDLE Irp)

Handling of Idle Notification request (IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION) sent to the PDO.

**
These behaviors are taken from WinXP(SP2) and will be considered the 'documented' behavior of
this API for Windows XP.
                                        IDLE_IRP                                WAKE_IRP
case                                    completion code                         completion code
..................................................................................................................
1
-CallbackInfo invalid                   STATUS_NO_CALLBACK_ACTIVE

2
-Another idle irp is pending            STATUS_DEVICE_BUSY

3
-Pdo set to D0                          STATUS_SUCCESS
(transition to D0)

4
-Hub(Parent) Idle request completes     (status of hubs request propigated)
(D0 as a result of its own
idle irp being completed)

5
-Hub(Parent) aborts global suspend      STATUS_CANCELED

6
-Hub(Parent) gets a
IRP_MN_REMOVE_DEVICE                    STATUS_CANCELED

7
-Pdo gets a IRP_MN_REMOVE_DEVICE        STATUS_CANCELED
-Pdo gets a IRP_MN_STOP_DEVICE          STATUS_CANCELED

8
-Pdo set to D3                          STATUS_POWER_STATE_INVALID              STATUS_POWER_STATE_INVALID

9
-Device Signals Resume                  -                                       STATUS_SUCCESS

10
-Device does not enter D2 in            *STATUS_CANCELED (in WinXP)
context of the callback (veto)          *-(irp remains pending in Vista)

11
-Pdo gets S0                            -

12
-Pdo gets Sx                            -

13
-Pdo gets D0 after Sx                   STATUS_SUCCESS (same as 3)

...................................................................................................................

(preliminary)
*denotes new behaviors for undefined cases in XP.



-------------------------------------------------------------------------------------------------------------------
SS Reg Keys

Legacy Flag (Windows XP)
DisableSelectiveSuspend (Disable in UI)

This is a global key value found in HKLM\System\CCS\Services\usb.

The key is a DWORD. A value of 1 disables selective suspend while a value of 0 enables selective suspend.
The default value is 0 if no value is present in the registry.

The effect of this key is global to all USB host controllers in the system.  If this key is set to one
selective suspend is disabled and the check box "Allow the computer to turn..." does not appear on the
property page for the root hub.

----------------------------------------------------------------------------------------------------------
HcDisableSelectiveSuspend

This key is instanced for USB host controller enumerated by PCI.  It is found in the software branch for
the given instance

key HKLM\System\CCS\Enum\ {36FC9E60-C465-11CF-8056-444553540000} \ *Driver*
The value of *Driver* can found in the hardware branch

key HKLM\System\CCS\Enum\ *DeviceID* \ *InstanceID* \Device Parameters \Driver

The key is a DWORD. A value of 1 disables selective suspend for this controller, while a value of 0 enable
s selective suspend.  The default value is 0 if no value is present in the registry.
The UI check box "Allow the computer to turn..." modifies this key.

The value of the UI check box reflects the setting of this key.

+--------------+----------------+
|  Key Value   |  UI check box  |
+..............|................+
| No key       |   checked      |
| 0            |   checked      |
| 1            |   clear        |
+--------------+----------------+

----------------------------------------------------------------------------------------------------------

IOCTL_INTERNAL_USB_NOTIFY_IDLE_READY (USB IDLE Irp)

Handling of Idle Ready notification sent to the PDO.


Irp completion cases, the device handle always transitions to NOT_IDLE_READY when the
Irp is completed for whatever reason.

case                                        completion code
........................................................................................................

-no irp pending                             STATUS_PENDING

-Another idle_ex irp is pending             STATUS_DEVICE_BUSY

-Pdo set to D0                              STATUS_SUCCESS
(transition to D0)

-Global Resume on the BUS                   STATUS_SUCCESS

-Device is disconnected                     STATUS_NO_SUCH_DEVICE

-Hub(Parent) gets a
IRP_MN_REMOVE_DEVICE                        STATUS_NO_SUCH_DEVICE

-Pdo gets a IRP_MN_REMOVE_DEVICE            STATUS_CANCELED
-Pdo gets a IRP_MN_STOP_DEVICE              STATUS_CANCELED


.........................................................................................................

** end ss doc

</pre>

*/


#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "ssdev.tmh"
#endif

CCHAR usbfile_ssdev_c[] = "ssdev.c";

#define USBFILE usbfile_ssdev_c


/*
    Use cancel 'safe' queue to handle the idle request irp queueing
*/

VOID
UsbhInsertPdoIdleIrp(
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
                               IdleIrpCsq);
    ASSERT_PDOEXT(pdoExt);

    InsertTailList(&pdoExt->IdleIrpList,
                   &Irp->Tail.Overlay.ListEntry);

}


VOID
UsbhRemovePdoIdleIrp(
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
                               IdleIrpCsq);
    ASSERT_PDOEXT(pdoExt);

    // our queue only has one entry

    RemoveEntryList(&Irp->Tail.Overlay.ListEntry);

}


PIRP
UsbhPeekNextPdoIdleIrp(
    PIO_CSQ Csq,
    PIRP Irp,
    PVOID PeekContext
    )
/*++

Routine Description:

    PIO_CSQ_PEEK_NEXT_IRP

    Reuls for peeNext the way I understand them -- these are not documented in
    the DDK text.

    if Irp == NULL start from list head
    else start peeking from the Irp following the Irp passed in.

    When we encounter an irp matching the context we return the irp

    if peekcontext == NULL then we return the next irp

    The DDK sample checks the FileObject in the irpstack even though a
    PeekContext is never specified.

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
                               IdleIrpCsq);

    UsbhFatalAssert(pdoExt->HubFdo,pdoExt != NULL, Csq);

    if (Irp == NULL) {
        // start at the head
        le = pdoExt->IdleIrpList.Flink;
    } else {
        // start at the next irp in the queue
        le = Irp->Tail.Overlay.ListEntry.Flink;
    }

    while (le != &pdoExt->IdleIrpList) {

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
UsbhAcquirePdoIdleIrpLock(
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
                              IdleIrpCsq);
    ASSERT_PDOEXT(pdoExt);

    KeAcquireSpinLock(&pdoExt->IdleIrpSpin, Irql);

}


VOID
UsbhReleasePdoIdleIrpLock(
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
                              IdleIrpCsq);
    ASSERT_PDOEXT(pdoExt);

    KeReleaseSpinLock(&pdoExt->IdleIrpSpin, Irql);
}


VOID
UsbhCompleteCanceledPdoIdleIrp(
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
                               IdleIrpCsq);
    ASSERT_PDOEXT(pdoExt);

    UsbhIdleIrp_Event(pdoExt->HubFdo, pdoExt->Pdo, Irp, Ev_IdleIrp_CsqCancel, 0);

}

ULONG
UsbhSS_WakeIrpRetryTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    )
/*++

Routine Description:

    This routine attempts to queue wake irp workitem -- it keeps trying until it succeeds.
    This code would only run in the case where allocation failures are enabled.

    We must di this since the device will not function until its wait wake irp can be
    completed.

Arguments:

//ntst
   Context - PortData structure.

   Parameter1 - SIG_HUBISR_WORKITEM.

   Parameter2 - not used

Irql: DISPATCH

Return Value:

--*/
{
    ULONG retry;

    // should only see this on low resource sim
    TEST_TRAP();

    retry = UsbhQueueCriticalWorkItem(HubFdo,
                                      NULL,
                                      UsbhSS_PdoWakeWorker,
                                      Context,
                                      0,
                                      SIG_PDO_WAKE_WORKITEM);
    return retry;
}

ULONG
UsbhIdleIrpRetryTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    )
/*++

Routine Description:

    This routine attempts to queue idle irp workitem -- it keeps trying until it succeeds.
    This code would only run in the case where allocation failures are enabled.

Arguments:

//ntst
   Context - PortData structure.

   Parameter1 - SIG_HUBISR_WORKITEM.

   Parameter2 - not used

Irql: DISPATCH

Return Value:

--*/
{
    ULONG retry;

    // should only see this on low resource sim
    TEST_TRAP();

    retry = UsbhQueueCriticalWorkItem(HubFdo,
                                      NULL,
                                      UsbhPdoIdleCC_Worker,
                                      Context,
                                      0,
                                      SIG_PDO_IDLE_WORKITEM);
    return retry;
}


VOID
UsbhSS_PdoWakeWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context
    )
/*++

Routine Description:

    This routine is used to complete the pending wake irp for a PDO
    when the device signals resume.

    We complete the irp on a worker thread because the power engine may
    renter us while attempting to resume.



Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_EXTENSION_PDO  pdoExt;
    PDEVICE_OBJECT pdo;
    PSTATE_CONTEXT sc;
    HUB_PDO_POWER_STATE pdoPowerState;
    HUB_FDO_POWERSTATE fdoPowerState;
    BOOLEAN wake_completed;

    hubExt = FdoExt(HubFdo);
    pdo = Context;

    pdoExt = PdoExt(pdo);
    sc = PDO_CONTEXT(pdo);

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!>\n", pdoExt->PortNumber));
    LOG(HubFdo, LOG_SSH, 'sspW', Context, pdoExt->PortNumber);

    do {
        // check the state of the PDO, both PnP and Power states are relevant.
        UsbhAcquirePdoStateLock(HubFdo, sc, PdoEv_Internal);

        if (GET_PDO_HWPNPSTATE(pdo) != Pdo_PnpRefHwPresent) {
            // pdo is removed or soon will be.

            UsbhReleasePdoStateLock(HubFdo, sc);
            break;
        }
        UsbhReleasePdoStateLock(HubFdo, sc);

        // sync with the hub power state
        //
        // we ignore this event if the hub is not in D0 to avoid unwanted wake
        // events during system suspend.
        UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_PDO_POWER, PdoDeviceWake_Tag);

        fdoPowerState = GET_FDO_POWER_STATE(sc);
        pdoPowerState = GET_PDO_POWER_STATE(pdo);

        if (fdoPowerState != FdoS0_D0) {
            UsbhReleaseFdoPwrLock(HubFdo, sc);
            break;
        }

        // do not wake if PDO is in D0
        if ((pdoPowerState == Pdo_D0) && !(pdoExt->PdoFlags.DeviceIsHub)) {
            UsbhReleaseFdoPwrLock(HubFdo, sc);
            break;
        }

        UsbhSetPdoPowerState(sc,  pdo, 0, pdoPowerState, PdoPwrEv_WakeInD0);
        UsbhReleaseFdoPwrLock(HubFdo, sc);

        wake_completed = UsbhCompletePdoWakeIrp(HubFdo, pdo, STATUS_SUCCESS);

        LOG(HubFdo, LOG_SSH, 'ssW1', wake_completed, pdo);

        if (wake_completed) {
            // complete the wake irp with success
            DbgTrace((UsbhDebugTrace, "(SS)Port #%d PDO (%p)**signaled wake**\n",
                pdoExt->PortNumber, pdo));
            UsbhKdPrint((1,"(SS)Port #%d PDO (%p)**signaled wake**\n",
                pdoExt->PortNumber, pdo));
        }
    } WHILE (0);

    // allow PDO to be deleted
    UsbhUnlatchPdo(HubFdo, pdo, NULL, 'sspW');

}


VOID
UsbhPdoIdleCC_Worker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context
    )
/*++

Routine Description:

    The following code supports the 'IDLE REQUEST' Irp for the root hub PDO
    (this technique was used successfully in usbport for XP)

    We use cancel safe queues to manage the cancelation support.  The only
    weird part is in handling the callback.  We dequeue the irp before we do
    the callback so it cannot be canceled directly by the CSQ but we need to
    hold int until the callback notification has completed. When the callback
    has completed we check the cancel flag again and complete the irp with the
    appropriate status.

    Process an Idle irp request for this PDO.  This is the worker routine
    associated with an idle irp request.

    This routine will issue the callback thru the irp.

    We are currently on separate thread at passive level, most drivers will
    submit a power request in the context of this callback.

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_EXTENSION_PDO  pdoExt;
    PDEVICE_OBJECT pdo;
    PIRP idleIrp;
    USB_IDLE_IRP_STATE state;
    KIRQL Irql;

    hubExt = FdoExt(HubFdo);
    pdo = Context;

    pdoExt = PdoExt(pdo);


    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!>\n", pdoExt->PortNumber));
    LOG(HubFdo, LOG_SSH, 'idlW', Context, pdoExt->PortNumber);

    // Make sure the current state is IdleIrp_WaitWorker

    KeAcquireSpinLock(&pdoExt->IdleIrpStateSpin, &Irql);

    state = pdoExt->IdleIrpState;

    KeReleaseSpinLock(&pdoExt->IdleIrpStateSpin, Irql);

    if (state != IdleIrp_WaitWorker) {
        UsbhUnlatchPdo(HubFdo, pdo, pdoExt, PdoLatchIdleWorker_Tag);
        LOG(HubFdo, LOG_SSH, 'idlN', Context, pdoExt->PortNumber);
        return;
    }

    // dequeue the pending idle irp, may be NULL if
    // no irp pending
    WAIT_EVENT(&pdoExt->IdleIrpCancelLock);    
    idleIrp = IoCsqRemoveNextIrp(&pdoExt->IdleIrpCsq, NULL);
    LOG(HubFdo, LOG_POWER, 'rmI1', pdo, idleIrp);

    if (idleIrp) {

        // invoke callback synchronously here
        PUSB_IDLE_CALLBACK_INFO idleCallbackInfo;

        UsbhIdleIrp_Event(HubFdo, pdo, idleIrp, Ev_IdleIrp_DequeueIrp, 0);
        //irp is IdleIrp_CB_Pending

        SET_EVENT(&pdoExt->IdleIrpCancelLock);    

        idleCallbackInfo = (PUSB_IDLE_CALLBACK_INFO)
                            IoGetCurrentIrpStackLocation(idleIrp)->\
                                Parameters.DeviceIoControl.Type3InputBuffer;




        // if the PDO has processed a system state power message (Sx) then we
        // skip the callback -- the idle irp will be completed on resume.
        if (pdoExt->Pdo_Sx == PowerSystemWorking) {

            // this structure is embedded in the buffer passed in so we may fault
            // here if the function driver frees the buffer.

            UsbhAssert(HubFdo, idleCallbackInfo);
            UsbhAssert(HubFdo, idleCallbackInfo->IdleCallback);
            LOG(HubFdo, LOG_POWER, 'Icb1', idleCallbackInfo, idleIrp);

            if (idleCallbackInfo && idleCallbackInfo->IdleCallback) {

                UsbhAssertPassive();
                LOG(HubFdo, LOG_POWER, 'Icb2', idleCallbackInfo->IdleCallback,
                    idleCallbackInfo->IdleContext);
                // in most cases this call will renter the hub by the power thread
                // when the function driber attempts to set the device to D2.

                //TEST_TRAP();

                idleCallbackInfo->IdleCallback(idleCallbackInfo->IdleContext);

            }
        }

        WAIT_EVENT(&pdoExt->IdleIrpCancelLock);
        UsbhIdleIrp_Event(HubFdo, pdo, idleIrp, Ev_IdleIrp_CB_Returned, pdoExt->IdleStatus);
        SET_EVENT(&pdoExt->IdleIrpCancelLock);    

    } else {
        SET_EVENT(&pdoExt->IdleIrpCancelLock);

        // indicate the worker has run
        UsbhIdleIrp_Event(HubFdo, pdo, idleIrp, Ev_IdleIrp_QueueEmpty, 0);
    }

    UsbhUnlatchPdo(HubFdo, pdo, pdoExt, PdoLatchIdleWorker_Tag);

}


NTSTATUS
UsbhIdleIrp_ReleaseIrp(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    NTSTATUS NtStatus
    )
/*++

Routine Description:

    Releases and completes an idle request irp

Arguments:

Return Value:

    nt status code set in the irp

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    LOG(HubFdo, LOG_SSH, 'ssCp', NtStatus, Irp);

    // Verify that we have a non-NULL IRP
    if (!Irp) {
        LOG(HubFdo, LOG_SSH, 'ssC0', NtStatus, Irp);
        return NtStatus;
    }

    pdoExt = PdoExt(Pdo);

    UsbhClearPdoIdleReady(HubFdo, Pdo, Irp);

    DbgTrace((UsbhDebugTrace, "(SS)Port #%d PDO (%p) IRP (%p) **complete idle** %08.8x\n",
                pdoExt->PortNumber, Pdo, Irp, NtStatus));
    UsbhKdPrint((1,"(SS)Port #%d PDO (%p) IRP (%p) **complete idle** %08.8x\n",
                pdoExt->PortNumber, Pdo, Irp, NtStatus));

    UsbhEtwLogDeviceIrpEvent(pdoExt, Irp, &USBHUB_ETW_EVENT_DEVICE_IDLE_NOTIFICATION_COMPLETE, NtStatus);
    UsbhCompleteIrp(Irp, NtStatus);
    UsbhDecPdoIoCount(Pdo, Irp);

    return NtStatus;
}

NTSTATUS
UsbhIdleIrp_NoIrp(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    USB_IDLE_IRP_EVENT Event,
    USB_IDLE_IRP_STATE CurrentState,
    KIRQL Irql
    )
/*++

Routine Description:

    Current State: waiting for function driver to submit an idle request.

Arguments:

Return Value:

    nt status code

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;
    PDEVICE_OBJECT latchPdo;

    pdoExt = PdoExt(Pdo);
    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_SSH, 'ssNI', Pdo, Irp);
    LOG(HubFdo, LOG_SSH, 'ssN2', 0, Event);

    switch(Event) {

    case Ev_IdleIrp_Enqueue:

        UsbhEtwLogDeviceIrpEvent(pdoExt, Irp, &USBHUB_ETW_EVENT_DEVICE_IDLE_NOTIFICATION_DISPATCH, STATUS_SUCCESS);

        if ((pdoExt->PdoFlags.DisableIdleSupport) || 
            (hubExt->SSH_EnabledStatus == SSH_DISABLED_VIA_USBPORT)) {

            UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_WaitWorker, Irql);

            IoMarkIrpPending(Irp);
            LOG(pdoExt->HubFdo, LOG_POWER, 'idI1', 0, Irp);
            IoCsqInsertIrp(&pdoExt->IdleIrpCsq, Irp, NULL);

            nts = STATUS_PENDING;
        } else {

            // lock down the pdo until this api is resolved the pending io count from
            // the irp will only hold the PDO until the irp is completed and this can
            // happen before we finish (while the WI is pending).

            latchPdo = UsbhLatchPdo(HubFdo, pdoExt->PortNumber, pdoExt,
                                PdoLatchIdleWorker_Tag);

            if (latchPdo == Pdo) {

                UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_WaitWorker, Irql);

                pdoExt->IdleStatus = STATUS_SUCCESS;

                IoMarkIrpPending(Irp);
                LOG(pdoExt->HubFdo, LOG_POWER, 'idI2', 0, Irp);
                IoCsqInsertIrp(&pdoExt->IdleIrpCsq, Irp, NULL);

                // queue a worker to process this request
                UsbhQueueCriticalWorkItem(HubFdo,
                                      &pdoExt->IdleRetryTimer,
                                      UsbhPdoIdleCC_Worker,
                                      Pdo,
                                      0,
                                      SIG_PDO_IDLE_WORKITEM);

                nts = STATUS_PENDING;

            } else {
                // if we cannot latch the PDO then it is being removed
                // complete the request now.
                UsbhFatalAssert(HubFdo,Irp != NULL, HubFdo);
                UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_NoIrp, Irql);

                LOG(HubFdo, LOG_SSH, 'idI3', Pdo, Irp);

                nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, NT_DEVICE_DISCONNECTED_STATUS);
            }
        }
        break;

    // these may be hit if the irp is completed/canceled just after the callback
    // completes
    case Ev_IdleIrp_Internal_Complete:
        UsbhFatalAssert(HubFdo,Irp != NULL, HubFdo);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_NoIrp, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, pdoExt->IdleStatus);
        break;

    case Ev_IdleIrp_CsqCancel:
        UsbhFatalAssert(HubFdo,Irp != NULL, HubFdo);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_NoIrp, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_CANCELLED);
        break;

    case Ev_IdleIrp_QueueEmpty:
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_NoIrp, Irql);
        nts = STATUS_SUCCESS;
        break;

    case Ev_IdleExIrp_Enqueue:
        UsbhFatalAssert(HubFdo,Irp != NULL, HubFdo);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleExIrp_IdleReady, Irql);

        UsbhSetPdoIdleReady(HubFdo, Pdo, Irp);

        IoMarkIrpPending(Irp);
        LOG(pdoExt->HubFdo, LOG_POWER, 'idI4', 0, Irp);
        IoCsqInsertIrp(&pdoExt->IdleIrpCsq, Irp, NULL);

        nts = STATUS_PENDING;
        break;

    default:
        UsbhAssertFailure(HubFdo, Irp);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, CurrentState, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_UNSUCCESSFUL);
    }

    return nts;
}


NTSTATUS
UsbhIdleIrp_WaitWorker(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    USB_IDLE_IRP_EVENT Event,
    USB_IDLE_IRP_STATE CurrentState,
    KIRQL Irql
    )
/*++

Routine Description:

    Current State: waiting for worker to run to process any pending idle
    requests in the queue.

Arguments:

Return Value:

    nt status code

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;

    LOG(HubFdo, LOG_SSH, 'ssWw', Pdo, Irp);

    pdoExt = PdoExt(Pdo);

    switch(Event) {

    case Ev_IdleIrp_Enqueue:
        // only allow one irp at a time
        UsbhFatalAssert(HubFdo,Irp != NULL, HubFdo);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_WaitWorker, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_DEVICE_BUSY);
        break;

    case Ev_IdleIrp_DequeueIrp:
        //

        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_CB_Pending, Irql);

        nts = STATUS_PENDING;
        break;

    case Ev_IdleIrp_Internal_Complete:
        UsbhFatalAssert(HubFdo,Irp != NULL, HubFdo);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_NoIrp, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, pdoExt->IdleStatus);
        break;

    case Ev_IdleIrp_CsqCancel:
        UsbhFatalAssert(HubFdo,Irp != NULL, HubFdo);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_NoIrp, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_CANCELLED);
        break;

    case Ev_IdleIrp_QueueEmpty:
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_NoIrp, Irql);
        nts = STATUS_SUCCESS;
        break;


    default:
        UsbhFatalAssert(HubFdo,Irp != NULL, HubFdo);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, CurrentState, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_UNSUCCESSFUL);
    }

    return nts;
}


NTSTATUS
UsbhIdleIrp_CB_Complete(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    USB_IDLE_IRP_EVENT Event,
    USB_IDLE_IRP_STATE CurrentState,
    KIRQL Irql
    )
/*++

Routine Description:

    Current State: waiting for client irp to be completed or canceled

Arguments:

Return Value:

    nt status code

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;

    LOG(HubFdo, LOG_SSH, 'ssWc', Pdo, Irp);

    pdoExt = PdoExt(Pdo);

    switch(Event) {

    case Ev_IdleIrp_Enqueue:
        // only allow one irp at a time

        UsbhFatalAssert(HubFdo,Irp != NULL, HubFdo);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_CB_Complete, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_DEVICE_BUSY);
        break;

     case Ev_IdleIrp_CsqCancel:
        UsbhFatalAssert(HubFdo,Irp != NULL, HubFdo);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_NoIrp, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_CANCELLED);
        break;

    case Ev_IdleIrp_Internal_Complete:
        UsbhFatalAssert(HubFdo,Irp != NULL, HubFdo);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_NoIrp, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, pdoExt->IdleStatus);
        break;

    default:
        UsbhAssertFailure(HubFdo, Irp);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, CurrentState, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_UNSUCCESSFUL);
    }

    return nts;
}


NTSTATUS
UsbhIdleIrp_CB_Canceled(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    USB_IDLE_IRP_EVENT Event,
    USB_IDLE_IRP_STATE CurrentState,
    KIRQL Irql
    )
/*++

Routine Description:

    Current State: waiting for client irp to be completed or canceled

Arguments:

Return Value:

    nt status code

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;

    LOG(HubFdo, LOG_SSH, 'ssWc', Pdo, Irp);

    pdoExt = PdoExt(Pdo);

    switch(Event) {


    case Ev_IdleIrp_Enqueue:
        UsbhFatalAssert(HubFdo,Irp!=NULL, HubFdo);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, CurrentState, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_DEVICE_BUSY);
        break;

    case Ev_IdleIrp_CB_Returned:
        // we canceled in the callback context
        UsbhFatalAssert(HubFdo,Irp!=NULL, HubFdo);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_NoIrp, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, pdoExt->IdleStatus);
        break;

    default:
        UsbhAssertFailure(HubFdo, Irp);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, CurrentState, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_UNSUCCESSFUL);
    }

    return nts;
}


NTSTATUS
UsbhIdleIrp_CB_Pending(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    USB_IDLE_IRP_EVENT Event,
    USB_IDLE_IRP_STATE CurrentState,
    KIRQL Irql
    )
/*++

Routine Description:

    Current State: waiting for function driver callback to complete

Arguments:

Return Value:

    nt status code

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;

    LOG(HubFdo, LOG_SSH, 'ssWc', Pdo, Irp);

    pdoExt = PdoExt(Pdo);

    switch(Event) {

    case Ev_IdleIrp_Enqueue:
        UsbhFatalAssert(HubFdo,Irp!=NULL, HubFdo);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_CB_Pending, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_DEVICE_BUSY);
        break;

    case Ev_IdleIrp_CB_Returned:
        UsbhFatalAssert(HubFdo,Irp!=NULL, HubFdo);

        // we own the irp, check the state and requeue if it is not canceled

        if (Irp->Cancel) {

            // irp was canceled while we owned it, complete it now
            UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_NoIrp, Irql);

            nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_CANCELLED);

        } else {

            // requeue so the function driver can cancel it

            UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_CB_Complete, Irql);

            // re-queue the irp so that the function driver can cancel
            IoCsqInsertIrp(&pdoExt->IdleIrpCsq, Irp, NULL);

            nts = STATUS_PENDING;
        }
        break;

    case Ev_IdleIrp_Internal_Complete:
        // trying to complete the request internally but the callback pending
        // we will complete it when the callback completes (ie the stack unwinds)

        UsbhAssert(HubFdo, Irp != NULL);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_CB_Canceled, Irql);

        nts = STATUS_PENDING;
        break;

    default:
        UsbhAssertFailure(HubFdo, Irp);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, CurrentState, Irql);
        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_UNSUCCESSFUL);
    }

    return nts;
}


NTSTATUS
UsbhIdleExIrp_IdleReady(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    USB_IDLE_IRP_EVENT Event,
    USB_IDLE_IRP_STATE CurrentState,
    KIRQL Irql
    )
/*++

Routine Description:

    Current State: waiting for function driver callback to complete

Arguments:

Return Value:

    nt status code

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;

    LOG(HubFdo, LOG_SSH, 'ssWc', Pdo, Irp);

    pdoExt = PdoExt(Pdo);

    switch(Event) {

    case Ev_IdleIrp_Enqueue:
        // someone using both APIs
        UsbhAssertMsg(HubFdo, "Unexpected Event Ev_IdleIrp_Enqueue", FALSE);

    case Ev_IdleExIrp_Enqueue:

        UsbhFatalAssert(HubFdo,Irp!=NULL, HubFdo);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleExIrp_IdleReady, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_DEVICE_BUSY);
        break;

    case Ev_IdleIrp_CsqCancel:
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_NoIrp, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_CANCELLED);
        break;

    case Ev_IdleIrp_Internal_Complete:

        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_NoIrp, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, pdoExt->IdleStatus);
        break;

    case Ev_IdleExIrp_GlobalResume:
        // global resume flushes the IDLE_READY api
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, IdleIrp_NoIrp, Irql);

        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_SUCCESS);
        break;

    default:
        UsbhAssertFailure(HubFdo, Irp);
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, CurrentState, Irql);
        nts = UsbhIdleIrp_ReleaseIrp(HubFdo, Pdo, Irp, STATUS_UNSUCCESSFUL);
    }

    return nts;
}



NTSTATUS
UsbhIdleIrp_Event(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    USB_IDLE_IRP_EVENT Event,
    NTSTATUS CompleteStatus
    )
/*++

Routine Description:

Arguments:

    HubFdo, Pdo - Device in question

    Irp - idle ioctl irp associated with this event

    Event - event to dispatch

    CompleteStatus - value for current complete status of idle irps

Return Value:

    nt status code

--*/
{
    USB_IDLE_IRP_STATE state;
    NTSTATUS nts;
    KIRQL irql;
    PDEVICE_EXTENSION_PDO pdoExt;

    LOG(HubFdo, LOG_SSH, 'ssEV', Event, Irp);

    pdoExt = PdoExt(Pdo);

    state = UsbhGetIdleIrpState(HubFdo, Pdo, &irql);
    pdoExt->IdleStatus = CompleteStatus;
    LOG(HubFdo, LOG_SSH, 'ssE1', Pdo, state);

    switch (state) {
    case IdleIrp_NoIrp:
        nts = UsbhIdleIrp_NoIrp(HubFdo, Pdo, Irp, Event, state, irql);
        break;

    case IdleIrp_WaitWorker:
        nts = UsbhIdleIrp_WaitWorker(HubFdo, Pdo, Irp, Event, state, irql);
        break;

    case IdleIrp_CB_Complete:
        nts = UsbhIdleIrp_CB_Complete(HubFdo, Pdo, Irp, Event, state, irql);
        break;

    case IdleIrp_CB_Canceled:
        nts = UsbhIdleIrp_CB_Canceled(HubFdo, Pdo, Irp, Event, state, irql);
        break;

    case IdleIrp_CB_Pending:
        nts = UsbhIdleIrp_CB_Pending(HubFdo, Pdo, Irp, Event, state, irql);
        break;

    case IdleExIrp_IdleReady:
        nts = UsbhIdleExIrp_IdleReady(HubFdo, Pdo, Irp, Event, state, irql);
        break;

    default:
        UsbhAssertFailure(HubFdo, Pdo);
        nts = STATUS_UNSUCCESSFUL;
        UsbhSetIdleIrpState(HubFdo, Pdo, Event, state, irql);
    }

    return nts;
}


USB_IDLE_IRP_STATE
UsbhGetIdleIrpState(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PKIRQL Irql
    )
/*++

Routine Description:

    Acquires the idle irp lock for a PDO.

Arguments:

Return Value:

    returns the current odle irp state

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PG_STATE_LOG_ENTRY log;
    USB_IDLE_IRP_STATE state;

    pdoExt = PdoExt(Pdo);

    KeAcquireSpinLock(&pdoExt->IdleIrpStateSpin, Irql);

    log = &pdoExt->IdleIrpLog[pdoExt->IdleIrpLogIdx];
    state = log->CurrentState = pdoExt->IdleIrpState;
    log->Event = 0; // no event yet

    return state;
}


VOID
UsbhSetIdleIrpState(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USB_IDLE_IRP_EVENT Event,
    USB_IDLE_IRP_STATE NewState,
    KIRQL Irql
    )
/*++

Routine Description:

    Releases the idle irp lock for a PDO and sets the state.

Arguments:

Return Value:

    returns the current idle irp state

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PG_STATE_LOG_ENTRY log;

    pdoExt = PdoExt(Pdo);

    log = &pdoExt->IdleIrpLog[pdoExt->IdleIrpLogIdx];

    log->Event = Event;
    log->NextState = pdoExt->IdleIrpState = NewState;
    pdoExt->IdleIrpLogIdx=(pdoExt->IdleIrpLogIdx+1) % G_NUM_STATE_LOG_ENTRIES;

    KeReleaseSpinLock(&pdoExt->IdleIrpStateSpin, Irql);

}


VOID
UsbhCompletePdoIdleIrp(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    NTSTATUS CompleteStatus
    )
/*++

Routine Description:

    completes a pending idle irp with status success
    (if we have one)

Arguments:

    CompleteStatus - status code used to complete the irp (if possible).

Return Value:

    none

--*/
{
    PIRP idleIrp;
    PDEVICE_EXTENSION_PDO pdoExt;
    USB_IDLE_IRP_STATE state;
    KIRQL irql;

    pdoExt = PdoExt(Pdo);

    // Synchronize with the idle callback worker thread
    WAIT_EVENT(&pdoExt->IdleIrpCancelLock);

    state = UsbhGetIdleIrpState(HubFdo, Pdo, &irql);

    // If callback is pending, the idle IRP is currently dequeued and not cancellable
    // In this case, transition to IdleIrp_CB_Canceled so the IRP will be cancelled after
    // the idle callback completes
    if (state == IdleIrp_CB_Pending) {
        pdoExt->IdleStatus = CompleteStatus;
        UsbhSetIdleIrpState(HubFdo, Pdo, Ev_IdleIrp_Check_CB, IdleIrp_CB_Canceled, irql);
    } else {
        UsbhSetIdleIrpState(HubFdo, Pdo, Ev_IdleIrp_Check_CB, state, irql);
    }

    idleIrp = IoCsqRemoveNextIrp(&pdoExt->IdleIrpCsq, NULL);
    SET_EVENT(&pdoExt->IdleIrpCancelLock);

    LOG(HubFdo, LOG_POWER, 'cpID', CompleteStatus, idleIrp);

    if (idleIrp) {
        UsbhIdleIrp_Event(HubFdo, Pdo, idleIrp,
            Ev_IdleIrp_Internal_Complete, CompleteStatus);
    }
}


VOID
UsbhSS_SignalPdoWake(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Wake up a PDO for Selective Suspend. This routine is used if we are
    not in a safe place to complete a ww irp.

    We queue a wake request that will be completed if the hub is still in S0_D0.
    This routine is used to signal wake for selective suspend.

Arguments:

Return Value:

    none

--*/
{

    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_OBJECT latchPdo;

    pdoExt = PdoExt(Pdo);

    // lock down the PDO while this runs
    latchPdo = UsbhLatchPdo(HubFdo, pdoExt->PortNumber, NULL, 'sspW');
    UsbhAssert(HubFdo, latchPdo == Pdo);

    LOG(HubFdo, LOG_SSH, 'sWAK', Pdo, 0);

    UsbhQueueCriticalWorkItem(HubFdo,
                              &pdoExt->D0WakeRetryTimer,
                              UsbhSS_PdoWakeWorker,
                              Pdo,
                              0,
                              SIG_PDO_WAKE_WORKITEM);

}


VOID
UsbhClearPdoIdleReady(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PVOID Object
    )
/*++

Routine Description:

   set a Pdos device handle to the 'IdleNotReady' state.

Arguments:

    Pdo - a latched Pdo

Return Value:

    none

--*/
{

    PDEVICE_EXTENSION_PDO pdoExt;
    PUSB_DEVICE_HANDLE dh;
    ULONG oldValue;

    pdoExt = PdoExt(Pdo);

    dh = UsbhRefPdoDeviceHandle(HubFdo, Pdo, Object, Devh_ClearIdle_Tag);
    if (dh) {

        LOG(HubFdo, LOG_IOCTL, 'rfI1', 0, dh);

        UsbhKdPrint((1, ">Pdo: %p CLEAR IDLE_READY\n", Pdo));
        pdoExt->IdleReadyState = 0;
        Usbh_HubSetDeviceHandleIdleReadyState(HubFdo, dh, pdoExt->IdleReadyState, &oldValue);

        UsbhDerefPdoDeviceHandle(HubFdo, dh, Object, Devh_ClearIdle_Tag);
    }


}


VOID
UsbhSetPdoIdleReady(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PVOID Object
    )
/*++

Routine Description:

     set a Pdos device handle to the 'IdleNotReady' state.

Arguments:

     Pdo - a latched Pdo


Return Value:

    none

--*/
{

    PDEVICE_EXTENSION_PDO pdoExt;
    PUSB_DEVICE_HANDLE dh;
    ULONG oldValue;

    pdoExt = PdoExt(Pdo);

    dh = UsbhRefPdoDeviceHandle(HubFdo, Pdo, Object, Devh_SetIdle_Tag);
    if (dh) {

        LOG(HubFdo, LOG_IOCTL, 'rfI2', 0, dh);

        UsbhKdPrint((1, ">Pdo: %p SET IDLE_READY\n", Pdo));
        pdoExt->IdleReadyState = 1;
        Usbh_HubSetDeviceHandleIdleReadyState(HubFdo, dh, pdoExt->IdleReadyState, &oldValue);

        UsbhDerefPdoDeviceHandle(HubFdo, dh, Object, Devh_SetIdle_Tag);
    }

}


