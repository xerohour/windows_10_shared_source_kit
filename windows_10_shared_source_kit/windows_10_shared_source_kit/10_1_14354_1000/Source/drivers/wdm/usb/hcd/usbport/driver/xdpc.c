/*++

    Copyright (c) Microsoft Corporation. All rights reserved.

    xdpc.c

    Contains generic transfer dpc support routines.

--*/

#include "common.h"

#ifdef USBPORT_WPP
#include "xdpc.tmh"
#endif


/*

    xDpc State Table

    x = invalid


  State/Event                   |Ev_Xdpc_InitDpc         |Ev_Xdpc_Enable          |Ev_Xdpc_Disable         |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Undef                      |XDPC_Disabled           |x                       |x                       |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Enabled                    |x                       |x                       |XDPC_Disabled           |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_DpcQueued                  |x                       |x                       |XDPC_DpcQueuedDisabled  |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Signaled                   |x                       |x                       |XDPC_RunningDisabled    |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_RunningDisabled            |x                       |x                       |x                       |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_DpcQueuedDisabled          |x                       |x                       |x                       |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Disabled                   |x                       |XDPC_Enabled            |x                       |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Running                    |x                       |x                       |XDPC_RunningDisabled    |
--------------------------------+------------------------+------------------------+------------------------+

xDpc State Table(2)


  State/Event                   |Ev_Xdpc_TimeCheck       |Ev_Xdpc_TimeExpired     |Ev_Xdpc_Worker          |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Undef                      |                        |                        |x                       |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Enabled                    |                        |                        |x                       |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_DpcQueued                  |                        |                        |XDPC_Running            |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Signaled                   |                        |                        |x                       |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_RunningDisabled            |                        |                        |x                       |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_DpcQueuedDisabled          |                        |                        |XDPC_Disabled           |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Disabled                   |                        |                        |x                       |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Running                    |                        |                        |x                       |
--------------------------------+------------------------+------------------------+------------------------+




xDpc State Table(3)


  State/Event                   |Ev_Xdpc_End             |Ev_Xdpc_Signal          |Ev_                     |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Undef                      |x                       |x                       |                        |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Enabled                    |x                       |XDPC_DpcQueued          |                        |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_DpcQueued                  |x                       |XDPC_DpcQueued          |                        |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Signaled                   |XDPC_DpcQueued          |XDPC_Signaled           |                        |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_RunningDisabled            |XDPC_Disabled           |XDPC_RunningDisabled    |                        |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_DpcQueuedDisabled          |x                       |XDPC_DpcQueuedDisabled  |                        |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Disabled                   |x                       |XDPC_Disabled           |                        |
--------------------------------+------------------------+------------------------+------------------------+
XDPC_Running                    |XDPC_Enabled            |XDPC_Signaled           |                        |
--------------------------------+------------------------+------------------------+------------------------+




*/

VOID
USBPORT_XdpcInitialize(
    __inout PDEVICE_OBJECT HcFdo
    )
/*++

--*/
{
    PDEVICE_EXTENSION devExt;

    devExt = GetHcFdoExt(HcFdo);

    InitializeListHead(&devExt->Fdo.XdpcListHead);

    //
    // DPC/XDPC: 1 of 4
    //
    USBPORT_Xdpc_InitDpc(HcFdo,
                         &devExt->Fdo.UsbMapDpc,
                         USBPORT_Xdpc_Worker_MapDpc,
                         USBPORT_Core_UsbMapDpc_Worker,
                         1000);

    //
    // DPC/XDPC: 2 of 4
    //
    USBPORT_Xdpc_InitDpc(HcFdo,
                         &devExt->Fdo.UsbHcIntDpc,
                         USBPORT_Xdpc_Worker_HcIntDpc,
                         USBPORT_Core_UsbHcIntDpc_Worker,
                         1000);

    //
    // DPC/XDPC: 3 of 4
    //
    USBPORT_Xdpc_InitDpc(HcFdo,
                         &devExt->Fdo.UsbDoneDpc,
                         USBPORT_Xdpc_Worker_DoneDpc,
                         USBPORT_Core_UsbDoneDpc_Worker,
                         1000);

    //
    // DPC/XDPC: 4 of 4
    //
    USBPORT_Xdpc_InitDpc(HcFdo,
                         &devExt->Fdo.UsbIocDpc,
                         USBPORT_Xdpc_Worker_IocDpc,
                         USBPORT_Core_UsbIocDpc_Worker,
                         1000);
}


VOID
USBPORT_XdpcEnableAll(
    __inout PDEVICE_OBJECT HcFdo
    )
/*++

--*/
{
    PDEVICE_EXTENSION devExt;

    devExt = GetHcFdoExt(HcFdo);

    USBPORT_Xdpc_Enable(HcFdo, &devExt->Fdo.UsbHcIntDpc);
    USBPORT_Xdpc_Enable(HcFdo, &devExt->Fdo.UsbDoneDpc);
    USBPORT_Xdpc_Enable(HcFdo, &devExt->Fdo.UsbMapDpc);
    USBPORT_Xdpc_Enable(HcFdo, &devExt->Fdo.UsbIocDpc);
}


VOID
USBPORT_XdpcDisableAll(
    __inout PDEVICE_OBJECT HcFdo
    )
/*++

--*/
{
    PDEVICE_EXTENSION devExt;

    devExt = GetHcFdoExt(HcFdo);

    USBPORT_Xdpc_Disable(HcFdo, &devExt->Fdo.UsbHcIntDpc);
    USBPORT_Xdpc_Disable(HcFdo, &devExt->Fdo.UsbDoneDpc);
    USBPORT_Xdpc_Disable(HcFdo, &devExt->Fdo.UsbMapDpc);
    USBPORT_Xdpc_Disable(HcFdo, &devExt->Fdo.UsbIocDpc);
}


XDPC_STATE
USBPORT_Xdpc_iSetState(
     __inout PDEVICE_OBJECT HcFdo,
     __inout PXDPC_CONTEXT Xdpc,
     __in XDPC_EVENT Event,
     __in XDPC_STATE NewState
    )
/*++
    set the state for an xdpc object, returns the previous state.
--*/
{
    PXDPC_STATE_LOG_ENTRY newLog;

    LOGENTRY(HcFdo, LOG_TRANSFER, 'xSt0', Xdpc, Event, NewState);

    Xdpc->LogIdx=(Xdpc->LogIdx+1) % NUM_XDPC_STATE_LOG_ENTRIES;
    newLog = &Xdpc->Log[Xdpc->LogIdx];

    newLog->Event = Event;
    newLog->CurrentState = Xdpc->State;
    Xdpc->State = newLog->NextState = NewState;

    if (NewState ==  XDPC_Disabled) {
        PUSB_IOREQUEST_CONTEXT usbIoReq;

        usbIoReq = &Xdpc->UsbIoRequest;
        LOGENTRY(HcFdo, LOG_TRANSFER, 'xSt2', Xdpc, Event, usbIoReq);

        // drop ref on FDO
        if (USBPORT_IsLinked(&usbIoReq->IoRequestLink)) {
            USBPORT_Core_DecIoRequestCount(HcFdo, usbIoReq, TRUE);
        }
    }

    return newLog->CurrentState;
}


/*

    Event Handling Routines

*/

BOOLEAN
USBPORT_Xdpc_iTimeExpired(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT Xdpc
    )
/*++
Ev_Xdpc_TimeCheck,
Ev_Xdpc_TimeExpired

Routine Description:

returns TRUE if the specified dpc has been executing to long

--*/
{
    BOOLEAN overtime;

    overtime = USBPORT_Core_CheckTimeout(HcFdo, &Xdpc->Timeout);

#if DBG
    if (overtime) {
        LOGENTRY(HcFdo, LOG_TRANSFER, 'DPOV', Xdpc, 0, overtime);
        USBPORT_KdPrint((0, "USB Dpc %p in overtime\n", Xdpc));
        TEST_TRAP();
    }
#endif

    return overtime;
}


VOID
USBPORT_Xdpc_iSignal(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT XdpcToSignal,
    __inout_opt PHCD_TRANSFER_CONTEXT Transfer
    )
 /*++

 --*/
{
    XDPC_STATE state;
    BOOLEAN inserted;

    ASSERT_XDPC(XdpcToSignal);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'Xsi1', XdpcToSignal, 0, 0);

    state = XdpcToSignal->State;
    switch(XdpcToSignal->State) {
    case XDPC_Enabled:
        
        // queue the dpc
        inserted = USBPORT_Xdpc_InsertQueueDpc(HcFdo, XdpcToSignal);
        
        // If the specified DPC object is not currently in the queue,
        // KeInsertQueueDpc queues the DPC and returns TRUE
        if (inserted) {

            USBPORT_Xdpc_iSetState(HcFdo, XdpcToSignal, Ev_Xdpc_Signal, XDPC_DpcQueued);
        } else {
            // the failure case, stay signalled
            USBPORT_Xdpc_iSetState(HcFdo, XdpcToSignal, Ev_Xdpc_Error, XDPC_Signaled);
        }
        break;

    case XDPC_Running:

        USBPORT_Xdpc_iSetState(HcFdo, XdpcToSignal, Ev_Xdpc_Signal, XDPC_Signaled);
        break;

    // nop states
    case XDPC_RunningDisabled:
    case XDPC_Disabled:
    case XDPC_Signaled:
    case XDPC_DpcQueued:
        USBPORT_Xdpc_iSetState(HcFdo, XdpcToSignal, Ev_Xdpc_Signal, state);
        break;

    default:
        USBPORT_ASSERTMSG("Invalid XdpcToSignal->State",FALSE);
        break;

    }
}

VOID
USBPORT_Xdpc_Signal(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT XdpcToSignal,
    __inout_opt PHCD_TRANSFER_CONTEXT Transfer
    )
 /*++

    Acquires the lock and signals the indicated Dpc to run

 --*/
{
    KIRQL irql;

    irql = USBPORT_TxAcquireLock(HcFdo, XdpcToSignal);

    USBPORT_Xdpc_iSignal(HcFdo, XdpcToSignal, Transfer);

    USBPORT_TxReleaseLock(HcFdo, XdpcToSignal, irql);
}


VOID
USBPORT_Xdpc_Enable(
     __inout PDEVICE_OBJECT HcFdo,
     __inout PXDPC_CONTEXT Xdpc
    )
/*++
Ev_Xdpc_Enable

--*/
{
    KIRQL irql;

    ASSERT_XDPC(Xdpc);

    irql = USBPORT_TxAcquireLock(HcFdo, Xdpc);

    switch(Xdpc->State) {
    case XDPC_Disabled:

        USBPORT_Xdpc_iSetState(HcFdo, Xdpc, Ev_Xdpc_Enable, XDPC_Enabled);

        USBPORT_Core_IncIoRequestCount(HcFdo, &Xdpc->UsbIoRequest, Xdpc, UsbIoTag_Xdpc, FALSE);

        USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);
        break;

    default:
        USBPORT_ASSERTMSG("Unexpected Xdpc->State",FALSE);
        USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);
        break;

    }
}


VOID
USBPORT_Xdpc_InitDpc(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT Xdpc,
    __in PKDEFERRED_ROUTINE XdpcWorker,
    __in PXDPC_WORKER_ROUTINE DpcWorker,
    __in ULONG TimeLimit_ms
    )
/*++
Ev_Xdpc_InitDpc

--*/
{
    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    PUSB_IOREQUEST_CONTEXT usbIoReq;


    devExt = GetHcFdoExt(HcFdo);

    irql = USBPORT_TxAcquireLock(HcFdo, Xdpc);

    Xdpc->State = XDPC_Disabled;
    Xdpc->TxLockState = 0;
    KeInitializeDpc(&Xdpc->Dpc, XdpcWorker, Xdpc);
    KeInitializeThreadedDpc(&Xdpc->ThreadedDpc, XdpcWorker, Xdpc);

    usbIoReq = &Xdpc->UsbIoRequest;
    USB_ZAP_LINK(&usbIoReq->IoRequestLink);

    InsertTailList(&devExt->Fdo.XdpcListHead, &Xdpc->XdpcLink);

    Xdpc->DpcWorker = DpcWorker;
    Xdpc->Sig = SIG_XDPC;
    Xdpc->HcFdo = HcFdo;
    Xdpc->DpcLimit_ms = TimeLimit_ms;
    USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);
}


VOID
USBPORT_Xdpc_Disable(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT Xdpc
    )
/*++
Ev_Xdpc_Disable

--*/
{
    KIRQL irql;

    ASSERT_XDPC(Xdpc);

    irql = USBPORT_TxAcquireLock(HcFdo, Xdpc);

    switch(Xdpc->State) {
    case XDPC_Undef:
        // we may get here if start fails before USBPORT_XdpcInitialize is called
        USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);
        break;

    case XDPC_Disabled:
        USBPORT_Xdpc_iSetState(HcFdo, Xdpc, Ev_Xdpc_Disable, XDPC_Disabled);
        // we may get here if start fails after USBPORT_XdpcInitialize is called
        USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);
        break;

    case XDPC_Enabled:
        USBPORT_Xdpc_iSetState(HcFdo, Xdpc, Ev_Xdpc_Disable, XDPC_Disabled);

        USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);
        break;

    case XDPC_Running:
    case XDPC_Signaled:
        USBPORT_Xdpc_iSetState(HcFdo, Xdpc, Ev_Xdpc_Disable, XDPC_RunningDisabled);

        USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);
        break;

    case XDPC_DpcQueued:

        USBPORT_Xdpc_iSetState(HcFdo, Xdpc, Ev_Xdpc_Disable, XDPC_DpcQueuedDisabled);

        USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);
        break;

    default:
        USBPORT_ASSERTMSG("Unexpected Xdpc->State",FALSE);
        USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);
        break;

    }
}


VOID
USBPORT_Xdpc_iBegin(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT Xdpc
    )

/*++
Ev_Xdpc_Begin

    Sets the initial state for the transfer Dpc.



--*/
{
    PDEVICE_EXTENSION devExt;
    PLIST_ENTRY le;
    PHCD_TRANSFER_CONTEXT transfer;

    devExt = GetHcFdoExt(HcFdo);
    USBPORT_Core_InitTimeout(HcFdo, &Xdpc->Timeout, Xdpc, Xdpc->DpcLimit_ms);

    LOGENTRY(HcFdo, LOG_TRANSFER, 'xbg0', Xdpc, HcFdo, &devExt->Fdo.TxPriorityListHead);

    USBPORT_ASSERT(Xdpc->TxLockState);
    Xdpc->CurrentTransfer = NULL;

    if (IsListEmpty(&devExt->Fdo.TxPriorityListHead)) {
        LOGENTRY(HcFdo, LOG_TRANSFER, 'xbg1', Xdpc, HcFdo, 0);

        Xdpc->NextTransfer = NULL;
    } else {
        le = devExt->Fdo.TxPriorityListHead.Flink;
        transfer =  CONTAINING_RECORD(le,
                                      HCD_TRANSFER_CONTEXT,
                                      TxPriorityLink);

        LOGENTRY(HcFdo, LOG_TRANSFER, 'xbg2', transfer, HcFdo, 0);
        ASSERT_TRANSFER(transfer);

        Xdpc->NextTransfer = transfer;
    }
}


VOID
USBPORT_Xdpc_End(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT Xdpc
    )
/*++
Ev_Xdpc_End

    called when the dpc worker has completed traversing the priority list. The Dpc is requeued if we were signalled while runing.

    *The dpc may be reentered any time after this function drops the lock so it should be the very last thing each
    worker function does.
--*/
{
    KIRQL irql;
    BOOLEAN inserted;

    LOGENTRY(HcFdo, LOG_TRANSFER, 'xnd0', Xdpc, HcFdo, 0);
    ASSERT_XDPC(Xdpc);

    irql = USBPORT_TxAcquireLock(HcFdo, Xdpc);

    Xdpc->NextTransfer = NULL;
    Xdpc->CurrentTransfer = NULL;

    switch(Xdpc->State) {
    case XDPC_Signaled:
        LOGENTRY(HcFdo, LOG_TRANSFER, 'xnd2', Xdpc, HcFdo, 0);
        // queue the dpc
        inserted = USBPORT_Xdpc_InsertQueueDpc(HcFdo, Xdpc);

        // If the specified DPC object is not currently in the queue,
        // KeInsertQueueDpc queues the DPC and returns TRUE
        if (inserted) {
            LOGENTRY(HcFdo, LOG_TRANSFER, 'xnd4', Xdpc, HcFdo, 0);
            USBPORT_Xdpc_iSetState(HcFdo, Xdpc, Ev_Xdpc_End, XDPC_DpcQueued);
        } else {
            // the failure case, stay signalled
            LOGENTRY(HcFdo, LOG_TRANSFER, 'xnd6', Xdpc, HcFdo, 0);
            USBPORT_Xdpc_iSetState(HcFdo, Xdpc, Ev_Xdpc_Error, XDPC_Signaled);
        }
        USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);
        break;

    case XDPC_Running:
        LOGENTRY(HcFdo, LOG_TRANSFER, 'xnd8', Xdpc, HcFdo, 0);
        USBPORT_Xdpc_iSetState(HcFdo, Xdpc, Ev_Xdpc_End, XDPC_Enabled);
        USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);
        break;

    // transition to disable now that the worker is done
    case XDPC_RunningDisabled:
        LOGENTRY(HcFdo, LOG_TRANSFER, 'xndA', Xdpc, HcFdo, 0);
        USBPORT_Xdpc_iSetState(HcFdo, Xdpc, Ev_Xdpc_End, XDPC_Disabled);
        USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);
        break;

    default:
        LOGENTRY(HcFdo, LOG_TRANSFER, 'xndB', Xdpc, HcFdo, Xdpc->State);
        USBPORT_ASSERTMSG("Unexpected Xdpc->State",FALSE);
        USBPORT_TxReleaseLock(HcFdo, Xdpc, irql);
        break;

    }
}


FORCEINLINE
VOID
USBPORT_Xdpc_Worker_Common(
    PDEVICE_OBJECT  hcFdo,
    PXDPC_CONTEXT   Xdpc
    )
/*++

    Common code shared by the 4 DPC dispatch routines:

    USBPORT_Xdpc_Worker_MapDpc()
    USBPORT_Xdpc_Worker_HcIntDpc()
    USBPORT_Xdpc_Worker_DoneDpc()
    USBPORT_Xdpc_Worker_IocDpc()

--*/
{
    KIRQL   irql;
    KIRQL   oldIrql;

    KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);

    irql = USBPORT_TxAcquireLock(hcFdo, Xdpc);
    LOGENTRY(hcFdo, LOG_TRANSFER, 'xdw2', Xdpc, hcFdo, Xdpc->State);
    
    switch(Xdpc->State) {
    case XDPC_DpcQueued:
        LOGENTRY(hcFdo, LOG_TRANSFER, 'xdw4', Xdpc, 0, 0);
        USBPORT_Xdpc_iSetState(hcFdo, Xdpc, Ev_Xdpc_Worker, XDPC_Running);

        // call the dpc specific handler, the handler will release the lock
        Xdpc->Thread = KeGetCurrentThread();
        Xdpc->DpcWorker(hcFdo, Xdpc, irql);
        break;

    case XDPC_DpcQueuedDisabled:
        LOGENTRY(hcFdo, LOG_TRANSFER, 'xdw6', Xdpc, 0, 0);
        USBPORT_Xdpc_iSetState(hcFdo, Xdpc, Ev_Xdpc_Worker, XDPC_Disabled);
        USBPORT_TxReleaseLock(hcFdo, Xdpc, irql);
        break;

    default:
        LOGENTRY(hcFdo, LOG_TRANSFER, 'xdw8', Xdpc, 0, Xdpc->State);
        USBPORT_ASSERTMSG("Unexpected Xdpc->State",FALSE);
        USBPORT_TxReleaseLock(hcFdo, Xdpc, irql);
        break;
    }

    KeLowerIrql(oldIrql);
}


//
// NOTE: The following 4 DPC dispatch routines are functionally
// identical.  Originally they were collapsed into a single DPC dispatch
// routine.  They were split into 4 separate DPC dispatch routines only
// for the purpose of aiding USBPORT.SYS DPC execution analysis using
// DPC dispatch trace tools.
//


VOID
USBPORT_Xdpc_Worker_MapDpc(
    PKDPC   Dpc,
    PVOID   DeferredContext,
    PVOID   SystemArgument1,
    PVOID   SystemArgument2
    )
/*++
Ev_Xdpc_Worker

DPC dispatch entry point routine for USBPORT_Core_UsbMapDpc_Worker() DPC/XDPC: 1 of 4

--*/
{
    ETW_EXECUTION_METRICS   execMetrics;
    PXDPC_CONTEXT   Xdpc;
    PDEVICE_OBJECT  hcFdo;

    ICE_START_TRACE();
    ETW_MEASURE_TIME_START(execMetrics);

    Xdpc = (PXDPC_CONTEXT)DeferredContext;

    ASSERT_XDPC(Xdpc);
    hcFdo = Xdpc->HcFdo;
    LOGENTRY(hcFdo, LOG_TRANSFER, 'xdA0', Xdpc, hcFdo, 0);

    USBPORT_Xdpc_Worker_Common(hcFdo, Xdpc);

    ETW_STOP_AND_LOG_TIME(hcFdo, execMetrics, MAPDPC);
    ICE_STOP_TRACE();
}


VOID
USBPORT_Xdpc_Worker_HcIntDpc(
    PKDPC   Dpc,
    PVOID   DeferredContext,
    PVOID   SystemArgument1,
    PVOID   SystemArgument2
    )
/*++
Ev_Xdpc_Worker

DPC dispatch entry point routine for USBPORT_Core_UsbHcIntDpc_Worker() DPC/XDPC: 2 of 4

--*/
{
    ETW_EXECUTION_METRICS   execMetrics;
    PXDPC_CONTEXT   Xdpc;
    PDEVICE_OBJECT  hcFdo;

    ICE_START_TRACE();
    ETW_MEASURE_TIME_START(execMetrics);

    Xdpc = (PXDPC_CONTEXT)DeferredContext;

    ASSERT_XDPC(Xdpc);
    hcFdo = Xdpc->HcFdo;
    LOGENTRY(hcFdo, LOG_TRANSFER, 'xdB0', Xdpc, hcFdo, 0);

    USBPORT_Xdpc_Worker_Common(hcFdo, Xdpc);

    ETW_STOP_AND_LOG_TIME(hcFdo, execMetrics, HCINTDPC);
    ICE_STOP_TRACE();
}


VOID
USBPORT_Xdpc_Worker_DoneDpc(
    PKDPC   Dpc,
    PVOID   DeferredContext,
    PVOID   SystemArgument1,
    PVOID   SystemArgument2
    )
/*++
Ev_Xdpc_Worker

DPC dispatch entry point routine for USBPORT_Core_UsbDoneDpc_Worker() DPC/XDPC: 3 of 4

--*/
{
    PXDPC_CONTEXT   Xdpc;
    PDEVICE_OBJECT  hcFdo;
    ETW_EXECUTION_METRICS execMetrics;

    ICE_START_TRACE();
    ETW_MEASURE_TIME_START(execMetrics);

    Xdpc = (PXDPC_CONTEXT)DeferredContext;

    ASSERT_XDPC(Xdpc);
    hcFdo = Xdpc->HcFdo;
    LOGENTRY(hcFdo, LOG_TRANSFER, 'xdC0', Xdpc, hcFdo, 0);

    USBPORT_Xdpc_Worker_Common(hcFdo, Xdpc);

    ETW_STOP_AND_LOG_TIME(hcFdo, execMetrics, DONEDPC);
    ICE_STOP_TRACE();
}


VOID
USBPORT_Xdpc_Worker_IocDpc(
    PKDPC   Dpc,
    PVOID   DeferredContext,
    PVOID   SystemArgument1,
    PVOID   SystemArgument2
    )
/*++
Ev_Xdpc_Worker

DPC dispatch entry point routine for USBPORT_Core_UsbIocDpc_Worker() DPC/XDPC: 4 of 4

--*/
{
    PXDPC_CONTEXT   Xdpc;
    PDEVICE_OBJECT  hcFdo;
    ETW_EXECUTION_METRICS execMetrics;

    ICE_START_TRACE();
    ETW_MEASURE_TIME_START(execMetrics);

    Xdpc = (PXDPC_CONTEXT)DeferredContext;

    ASSERT_XDPC(Xdpc);
    hcFdo = Xdpc->HcFdo;
    LOGENTRY(hcFdo, LOG_TRANSFER, 'xdD0', Xdpc, hcFdo, 0);

    USBPORT_Xdpc_Worker_Common(hcFdo, Xdpc);
    ETW_STOP_AND_LOG_TIME(hcFdo, execMetrics, IOCDPC);

    ICE_STOP_TRACE();
}


BOOLEAN
USBPORT_Xdpc_InsertQueueDpc(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT XdpcToSignal
    )
/*++
Routine Description:

    Queues either a normal DPC or a threaded DPC based on the whether or not 
    the current string of executing DPCs have been running too long.  This is to
    avoid triggering a DPC watchdog timeout.

Return Value:

    TRUE if DPC has been queued.  FALSE otherwise.

--*/

{
    KDPC_WATCHDOG_INFORMATION   watchdogInfo;
    ULONG                       watchdogThreshold;
    BOOLEAN                     inserted;
    PDEVICE_EXTENSION           devExt;

    if (NT_SUCCESS(KeQueryDpcWatchdogInformation(&watchdogInfo))) {
       
        watchdogThreshold = (watchdogInfo.DpcWatchdogLimit * USBPORT_REMAINING_DPC_TIMEOUT_THRESHOLD) / 100;
        
        //
        // Queue a threaded DPC if the DPC watchdog timeout threshold has been reached
        //
        if (watchdogInfo.DpcWatchdogCount < watchdogThreshold) {
            inserted = KeInsertQueueDpc(&XdpcToSignal->ThreadedDpc, 0, 0);

            devExt = GetHcFdoExt(HcFdo);
            devExt->Fdo.ThreadedDpcCount++;
        } else {       
            inserted = KeInsertQueueDpc(&XdpcToSignal->Dpc, 0, 0);
        }
    } else { 
        inserted = KeInsertQueueDpc(&XdpcToSignal->Dpc, 0, 0);
    }

    return inserted;
}
