/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    usb2.c

Abstract:

    functions for processing usb 2.0 specific requests

Environment:

    kernel mode only

Notes:

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs

--*/

#include "common.h"

#ifdef USBPORT_WPP
#include "usb2.tmh"
#endif


// taken from budgeter code

#ifdef USB2_INTERNAL

#define LARGEXACT (579)
#define USBPORT_MAX_REBALANCE 30



VOID
USBPORT_LockUSB2bw(
    PDEVICE_OBJECT HcFdo,
    PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

Serializes USB bw allocations

--*/
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, HcFdo);

    ASSERT_PASSIVE();
    LOGENTRY(HcFdo, LOG_PNP, 'LKu2', Endpoint, 0, 0);
    KeWaitForSingleObject(&devExt->Fdo.Usb2bwLock,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);

}

VOID
USBPORT_UnLockUSB2bw(
    PDEVICE_OBJECT HcFdo,
    PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

Serializes USB bw allocations

--*/
{
    PDEVICE_EXTENSION devExt;

    ASSERT_PASSIVE();
    GET_DEVICE_EXT(devExt, HcFdo);
    LOGENTRY(HcFdo, LOG_PNP, 'UKu2', Endpoint, 0, 0);

    KeReleaseSemaphore(&devExt->Fdo.Usb2bwLock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);
}


VOID
USBPORT_Rebalance(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __in PLIST_ENTRY ReblanceListHead
    )
/*++

Routine Description:

    **Main entry point for USB 2.0 endpoint (budget rebalance)**

    The rebalnce list contains all the endpoints that were effected by budgeting this new USB2 endpoint.
    We must re-schedule each of them.

    This process is serialized so we don't need to protect the reblance list itself.

    The rebalance opertaion for each endpoint is performed thru the ep state machine in order to detect conflicts
    with other state transitions for the endpoints involved.

--*/
{
    PLIST_ENTRY listEntry;
    PHCD_ENDPOINT endpoint;
    ULONG scheduleOffset;
    UCHAR sMask, cMask, period;
    ULONG bandwidth;
    PDEVICE_EXTENSION devExt;
    LIST_ENTRY interruptChangeList;
    LIST_ENTRY periodPromotionList;
    LIST_ENTRY isoChangeList;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ASSERT_PASSIVE();

    InitializeListHead(&interruptChangeList);
    InitializeListHead(&periodPromotionList);
    InitializeListHead(&isoChangeList);

    // bugbug
    // can the insertion of the new endpiont occurr after the modification
    // of the rebalnced endpoints?


    // bugbug, this list must be sorted such that the changes occurr
    // in the proper sequnence.

    // ???
    //    <------changes must travel this direction
    // iso--interrupt

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER,
        '2RB>', 0, 0, 0);


    while (!IsListEmpty(ReblanceListHead)) {

        listEntry = RemoveHeadList(ReblanceListHead);

        endpoint = (PHCD_ENDPOINT) CONTAINING_RECORD(
                    listEntry,
                    struct _HCD_ENDPOINT,
                    RebalanceLink);

        ASSERT_ENDPOINT(endpoint);
        endpoint->RebalanceLink.Flink = NULL;
        endpoint->RebalanceLink.Blink = NULL;

        sMask = USB2LIB_GetSMASK(endpoint->HsbEpContextPtr);
        cMask = USB2LIB_GetCMASK(endpoint->HsbEpContextPtr);
        bandwidth = USB2LIB_GetAllocedBusTime(endpoint->HsbEpContextPtr) * 8;

        scheduleOffset = USB2LIB_GetScheduleOffset(endpoint->HsbEpContextPtr);

        period = USB2LIB_GetNewPeriod(endpoint->HsbEpContextPtr);

        DbgTrace((UsbpDebugTrace,"'[RB-old] %p sMask = x%x cMask = x%x\n", endpoint,
                        endpoint->Parameters.InterruptScheduleMask,
                        endpoint->Parameters.SplitCompletionMask));
        DbgTrace((UsbpDebugTrace,"'[RB-old] Period x%x Offset x%x\n",
                endpoint->Parameters.Period,
                endpoint->Parameters.ScheduleOffset));

        DbgTrace((UsbpDebugTrace,"'[RB-new] %p sMask = x%x cMask = x%x\n",
                endpoint, sMask, cMask));
        DbgTrace((UsbpDebugTrace,"'[RB-new] Period x%x Offset x%x\n",
                period, scheduleOffset));

        USBPORT_KdPrint((1,"'[RB-old] %p sMask = x%x cMask = x%x\n", endpoint,
                        endpoint->Parameters.InterruptScheduleMask,
                        endpoint->Parameters.SplitCompletionMask));
        USBPORT_KdPrint((1,"'[RB-old] Period x%x Offset x%x\n",
                endpoint->Parameters.Period,
                endpoint->Parameters.ScheduleOffset));

        USBPORT_KdPrint((1,"'[RB-new] %p sMask = x%x cMask = x%x\n",
                endpoint, sMask, cMask));
        USBPORT_KdPrint((1,"'[RB-new] Period x%x Offset x%x\n",
                period, scheduleOffset));


        switch (endpoint->Parameters.TransferType) {
        case Interrupt:
            if (sMask == endpoint->Parameters.InterruptScheduleMask &&
                cMask == endpoint->Parameters.SplitCompletionMask &&
                scheduleOffset == endpoint->Parameters.ScheduleOffset &&
                period == endpoint->Parameters.Period) {

                USBPORT_KdPrint((1,"'[RB] interrupt no changes\n"));
                DbgTrace((UsbpDebugTrace,"'[RB] interrupt no changes\n"));
                USBPORT_ASSERT(bandwidth == endpoint->Parameters.Bandwidth);

            } else if (period != endpoint->Parameters.Period ||
                       scheduleOffset != endpoint->Parameters.ScheduleOffset) {

                USBPORT_KdPrint((1,"'[RB] interrupt period changes\n"));
                DbgTrace((UsbpDebugTrace,"'[RB] interrupt period changes\n"));
                InsertTailList(&periodPromotionList,
                               &endpoint->RebalanceLink);
            } else {
                USBPORT_KdPrint((1,"'[RB] interrupt c/s changes\n"));
                DbgTrace((UsbpDebugTrace,"'[RB] interrupt c/s changes\n"));

                InsertTailList(&interruptChangeList,
                               &endpoint->RebalanceLink);
            }
            break;

        case Isochronous:

            if (sMask == endpoint->Parameters.InterruptScheduleMask &&
                cMask == endpoint->Parameters.SplitCompletionMask &&
                scheduleOffset == endpoint->Parameters.ScheduleOffset &&
                period == endpoint->Parameters.Period) {

                USBPORT_KdPrint((1,"'[RB] iso no changes\n"));
                DbgTrace((UsbpDebugTrace,"'[RB] iso no changes\n"));
                USBPORT_ASSERT(bandwidth == endpoint->Parameters.Bandwidth);

            } else if (period != endpoint->Parameters.Period ||
                       scheduleOffset != endpoint->Parameters.ScheduleOffset) {
                 // currently not handled
                USBPORT_KdPrint((1,"'[RB] iso period changes\n"));
                DbgTrace((UsbpDebugTrace,"'[RB] iso period changes\n"));
                TEST_TRAP();
            } else {
                USBPORT_KdPrint((1,"'[RB] iso c/s changes\n"));
                DbgTrace((UsbpDebugTrace,"'[RB] iso c/s changes\n"));

                InsertTailList(&isoChangeList,
                               &endpoint->RebalanceLink);
            }
            break;
        }

    }

    // now do the period promotions, int changes and iso changes
    //
    // not clear if we could lump period and interrupt together so we don't. However this entire process is serialized
    // for each controller.
    DbgTrace((UsbpDebugTrace,"'[RB] period\n"));
    USBPORT_RebalanceEndpoints(FdoDeviceObject, &periodPromotionList);

    DbgTrace((UsbpDebugTrace,"'[RB] interrupt\n"));
    USBPORT_RebalanceEndpoints(FdoDeviceObject, &interruptChangeList);

    // now rebalance the iso endpoints
    DbgTrace((UsbpDebugTrace,"'[RB] iso\n"));
    USBPORT_RebalanceEndpoints(FdoDeviceObject, &isoChangeList);

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER,
        '2RB<', 0, 0, 0);
}


VOID
USBPORT_RebalanceEndpoints(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __inout PLIST_ENTRY EndpointList
    )
/*++

Routine Description:

    Rebalances a list of endpoints of a given type.



--*/
{
    PLIST_ENTRY listEntry;
    PHCD_ENDPOINT endpoint;
    ULONG scheduleOffset;
    UCHAR sMask, cMask, period;
    ULONG bandwidth, bt;
    PDEVICE_EXTENSION devExt;
    PHCD_ENDPOINT nextEndpoint;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ASSERT_PASSIVE();

    while (!IsListEmpty(EndpointList)) {

        listEntry = RemoveHeadList(EndpointList);

        endpoint = (PHCD_ENDPOINT) CONTAINING_RECORD(
                    listEntry,
                    struct _HCD_ENDPOINT,
                    RebalanceLink);

        LOGENTRY(FdoDeviceObject, LOG_TRANSFER,
                'rbe+', endpoint, 0, 0);

        ASSERT_ENDPOINT(endpoint);
        endpoint->RebalanceLink.Flink = NULL;
        endpoint->RebalanceLink.Blink = NULL;

        //ACQUIRE_ENDPOINT_LOCK(endpoint, FdoDeviceObject, &epLock, 'Lex+');

        // notify the miniport of the changed parameters

        sMask = USB2LIB_GetSMASK(endpoint->HsbEpContextPtr);
        cMask = USB2LIB_GetCMASK(endpoint->HsbEpContextPtr);

        scheduleOffset = USB2LIB_GetScheduleOffset(endpoint->HsbEpContextPtr);
        period = USB2LIB_GetNewPeriod(endpoint->HsbEpContextPtr);
        bt = USB2LIB_GetAllocedBusTime(endpoint->HsbEpContextPtr);
        bandwidth = bt * 8;
        nextEndpoint = USB2LIB_GetNextEndpoint(endpoint->HsbEpContextPtr);
#if DBG
        if (nextEndpoint) {
            ASSERT_ENDPOINT(nextEndpoint);
        }
#endif
        DbgTrace((UsbpDebugTrace,"'[RB - %p] \n", endpoint));

        USBPORT_ASSERT(bandwidth == endpoint->Parameters.Bandwidth);

        endpoint->Parameters.InterruptScheduleMask = sMask;
        endpoint->Parameters.SplitCompletionMask = cMask;

        if (bt >= LARGEXACT) {
            endpoint->EpFlags.FatIso = 1;
        } else {
            endpoint->EpFlags.FatIso = 0;
        }

        // track new parameters resulting from period change
        endpoint->Parameters.Period = period;
        endpoint->Parameters.ScheduleOffset = scheduleOffset;
        endpoint->Parameters.Bandwidth = bandwidth;

        // The Ordinal for an Isoch endpoint might now be different even
        // if the period did not change so always update the Ordinal.
        //
        endpoint->Parameters.Ordinal =
            USBPORT_SelectOrdinal(FdoDeviceObject, endpoint);

        //MP_RebalanceEndpoint(devExt, endpoint);
        USBPORT_SetGlobalEndpointState(FdoDeviceObject, endpoint, Ev_gEp_Rebalance);


        //RELEASE_ENDPOINT_LOCK(endpoint, FdoDeviceObject, &epLock, 'Lex-');

    }
}


NTSTATUS
USBPORT_AllocateBandwidthUSB20(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Computes the best schedule parameters for a USB2 endpoint. This routine will reference the endpoint in the
    context of the budget engine if the bw can be allocated.  This is true even for bulk and control. The ref is
    dropped when the bw is freed.


    There is one exception -- the root hub endpoint is not returned referenced.

Returns:

    STATUS_SUCCESS if allocation succeeds otherwie an error.

--*/
{
    PDEVICE_EXTENSION devExt;
    USB2LIB_BUDGET_PARAMETERS budget;
    BOOLEAN alloc_success;
    LONG startHframe;
    ULONG scheduleOffset, bt;
    UCHAR sMask, cMask, period;
    PREBALANCE_LIST rebalanceList;
    ULONG rebalanceListEntries;
    ULONG bytes;
    LIST_ENTRY endpointList;
    PVOID ttContext;
    PTRANSACTION_TRANSLATOR translator = NULL;
    PHCD_ENDPOINT nextEndpoint;
    NTSTATUS nts;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ASSERT_ENDPOINT(Endpoint);
    InitializeListHead(&endpointList);

    Endpoint->Parameters.ScheduleOffset = 0;
    LOGENTRY(FdoDeviceObject, LOG_TRANSFER,  'a2BW', Endpoint, 0, 0);

    do {
        alloc_success = FALSE;

        // ref the endpoint for the budget engine.  This is dropped when the bandwidth is released
        nts = USBPORT_ReferenceEndpoint(FdoDeviceObject, Endpoint,
                    &Endpoint->EpUsb2Handle, Endpoint, EPREF_USB2TAG);

        if (NT_ERROR(nts)) {
            break;
        }

        if (Endpoint->EpFlags.RootHub) {
            USBPORT_DereferenceEndpoint(FdoDeviceObject, Endpoint->EpUsb2Handle);
            nts = STATUS_SUCCESS;
            break;
        }

        // bulk and control are not tracked
        if (Endpoint->Parameters.TransferType == Bulk ||
            Endpoint->Parameters.TransferType == Control) {

            Endpoint->Parameters.ScheduleOffset = 0;
            nts = STATUS_SUCCESS;
            // keep ref for safety
            break;
        }


        // serialize access to the budgeter
        USBPORT_LockUSB2bw(FdoDeviceObject, Endpoint);

        USBPORT_ASSERT(Endpoint->Parameters.TransferType == Interrupt ||
                       Endpoint->Parameters.TransferType == Isochronous);

        DbgTrace((UsbpDebugTrace,"'ALLOCBW (EP) %p  >>>>>>>>>>>>\n", Endpoint));
        // period has been normalized to a value <=
        // USBPORT_MAX_INTEP_POLLING_INTERVAL in either mf of frames

        // call the engine to compute appropriate split masks
        // for this interrupt endpoint

        //
        DbgTrace((UsbpDebugTrace,"'(alloc) ep = %p\n", Endpoint));

        // set budget input parameters
        if (Endpoint->Parameters.TransferType == Interrupt) {
            budget.TransferType = Budget_Interrupt;
            budget.Period = Endpoint->Parameters.Period;
        } else {
            budget.TransferType = Budget_Iso;
            budget.Period = Endpoint->Parameters.Period;
        }
        budget.MaxPacket = Endpoint->Parameters.MaxPacketSize;
        if (Endpoint->Parameters.TransferDirection == In) {
            budget.Direction = Budget_In;
        } else {
            budget.Direction = Budget_Out;
        }
        switch (Endpoint->Parameters.DeviceSpeed) {
        case HighSpeed:
            budget.Speed = Budget_HighSpeed;
            break;
        case LowSpeed:
            budget.Speed = Budget_LowSpeed;
            break;
        case FullSpeed:
            budget.Speed = Budget_FullSpeed;
            break;
        }

        bytes = sizeof(PVOID) * USBPORT_MAX_REBALANCE;

        ALLOC_POOL_Z(rebalanceList,
                     NonPagedPoolNx,
                     bytes);

        if (rebalanceList == NULL) {
            LOGENTRY(FdoDeviceObject, LOG_TRANSFER,  'a2B1', Endpoint, 0, 0);
            USBPORT_DereferenceEndpoint(FdoDeviceObject, Endpoint->EpUsb2Handle);
            nts = STATUS_INSUFFICIENT_RESOURCES;
            USBPORT_UnLockUSB2bw(FdoDeviceObject, Endpoint);
            break;
        }


        // high speed endpoints will have no Tt context
        ttContext = NULL;
        if (Endpoint->Tt != NULL) {
            LOGENTRY(FdoDeviceObject, LOG_TRANSFER,  'a2B2', Endpoint, 0, Endpoint->Tt);
            ASSERT_TT(Endpoint->Tt);
            translator = Endpoint->Tt;
            ttContext = Endpoint->Tt->HsbTtContextPtr;
        }

        USBPORT_ASSERT(rebalanceList != NULL);

        rebalanceListEntries = USBPORT_MAX_REBALANCE;

        alloc_success = USB2LIB_AllocUsb2BusTime(
            devExt->Fdo.HsbControllerContextPtr,
            ttContext,
            Endpoint->HsbEpContextPtr,
            &budget,
            Endpoint, // context
            rebalanceList,
            &rebalanceListEntries);

        DbgTrace((UsbpDebugTrace,"'(alloc %d) rebalance count = %d\n", alloc_success, rebalanceListEntries));

        if (rebalanceListEntries > 0) {

            PHCD_ENDPOINT rebalanceEndpoint;
            ULONG i;

            // convert the rebalance entries to endpoints
            for (i=0; i< rebalanceListEntries; i++) {

                rebalanceEndpoint = rebalanceList->RebalanceContext[i];
                DbgTrace((UsbpDebugTrace,"'(alloc) rebalance Endpoint = %p \n",
                    rebalanceList->RebalanceContext[i]));

                USBPORT_ASSERT(rebalanceEndpoint->RebalanceLink.Flink == NULL);
                USBPORT_ASSERT(rebalanceEndpoint->RebalanceLink.Blink == NULL);
                InsertTailList(&endpointList,
                               &rebalanceEndpoint->RebalanceLink);

            }
        }

        // done with the budgets list
        UsbFreePool(rebalanceList);

        if (alloc_success) {
            ULONG bandwidth;

            // compute parameters for the miniport
            startHframe = USB2LIB_GetStartMicroFrame(Endpoint->HsbEpContextPtr);
            scheduleOffset = USB2LIB_GetScheduleOffset(Endpoint->HsbEpContextPtr);
            period = USB2LIB_GetNewPeriod(Endpoint->HsbEpContextPtr);
            sMask = USB2LIB_GetSMASK(Endpoint->HsbEpContextPtr);
            cMask = USB2LIB_GetCMASK(Endpoint->HsbEpContextPtr);
            // bw in bit times
            bt = USB2LIB_GetAllocedBusTime(Endpoint->HsbEpContextPtr);
            bandwidth = bt*8;
            nextEndpoint = USB2LIB_GetNextEndpoint(Endpoint->HsbEpContextPtr);

#if DBG
            if (nextEndpoint) {
                ASSERT_ENDPOINT(nextEndpoint);
            }
#endif

            Endpoint->Parameters.Period = period;
            Endpoint->Parameters.ScheduleOffset = scheduleOffset;
            Endpoint->Parameters.InterruptScheduleMask = sMask;
            Endpoint->Parameters.SplitCompletionMask = cMask;
            Endpoint->Parameters.Bandwidth = bandwidth;
            if (bt >= LARGEXACT) {
                Endpoint->EpFlags.FatIso = 1;
            }

            DbgTrace((UsbpDebugTrace,"'[NEW] %p sMask = x%x cMask = x%x hFrame x%x\n",
                Endpoint, sMask, cMask, startHframe));
            DbgTrace((UsbpDebugTrace,"'[NEW] Period x%x Offset x%x bw = %d\n",
                period, scheduleOffset, bandwidth));
            DbgTrace((UsbpDebugTrace,"'[NEW] BudgetNextEp x%p \n", nextEndpoint));

            nts = STATUS_SUCCESS;
        } else {

            DbgTrace((UsbpDebugTrace,"'[NEW] alloc failed\n"));
            nts = STATUS_UNSUCCESSFUL;

        }

        DbgTrace((UsbpDebugTrace,"'ALLOCBW (EP) <<<<<<<<<<<<<<<<<\n"));

        LOGENTRY(FdoDeviceObject, LOG_TRANSFER,  'a2RB', 0, &endpointList, alloc_success);

        DbgTrace((UsbpDebugTrace,"'REBLANCE (EP) >>>>>>>>>>>>>>>>>>>>\n"));
        // process the rebalanced endpoints
        USBPORT_Rebalance(FdoDeviceObject, &endpointList);
        DbgTrace((UsbpDebugTrace,"'REBLANCE (EP) <<<<<<<<<<<<<<<<<<<<<\n"));


        Endpoint->Parameters.Ordinal = USBPORT_SelectOrdinal(FdoDeviceObject, Endpoint);

        if (NT_ERROR(nts)) {
             // alloc failed, drop the ref
            USBPORT_DereferenceEndpoint(FdoDeviceObject, Endpoint->EpUsb2Handle);
        }

        USBPORT_UnLockUSB2bw(FdoDeviceObject, Endpoint);

    }       WHILE (0);

    return nts;
}


VOID
USBPORT_FreeBandwidthUSB20(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Frees the bw reserved for a give endpoint

Arguments:

Return Value:

    FALSE if no bandwidth availble

--*/
{
    PDEVICE_EXTENSION devExt;
    ULONG period, bandwidth;
    PREBALANCE_LIST rebalanceList;
    ULONG rebalanceListEntries;
    ULONG bytes;
    LIST_ENTRY endpointList;
    PVOID ttContext;
    PTRANSACTION_TRANSLATOR translator = NULL;
    ULONG scheduleOffset;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ASSERT_ENDPOINT(Endpoint);
    period = Endpoint->Parameters.Period;
    scheduleOffset = Endpoint->Parameters.ScheduleOffset;
    bandwidth = Endpoint->Parameters.Bandwidth;

    InitializeListHead(&endpointList);

    do {

        if (Endpoint->EpFlags.RootHub) {
            break;
        }

        if (Endpoint->Parameters.TransferType == Bulk ||
            Endpoint->Parameters.TransferType == Control) {

            USBPORT_DereferenceEndpoint(FdoDeviceObject, Endpoint->EpUsb2Handle);
            // these come out of our standard 10%
            break;
        }

        // serialize access to the budgeter
        USBPORT_LockUSB2bw(FdoDeviceObject, Endpoint);

        DbgTrace((UsbpDebugTrace,"'(free) Endpoint = %p\n", Endpoint));
        bytes = sizeof(PVOID) * USBPORT_MAX_REBALANCE;

        // This must succeed, if we can't get memory for the
        // rebalance list we cannot reorganize the schedule
        // as a result of the device leaving. This means the
        // whole schedule is busted and the bus will not function
        // at all after this occurs.
        //

        ALLOC_POOL_Z(rebalanceList,  NonPagedPoolNx, bytes);

        if (rebalanceList == NULL) {
            // if this fails we have no choice but to leave
            // the schedule hoplessly you-know-what'ed up.
            TEST_TRAP();
            USBPORT_DereferenceEndpoint(FdoDeviceObject, Endpoint->EpUsb2Handle);

            // release the budgeter
            USBPORT_UnLockUSB2bw(FdoDeviceObject, Endpoint);
            break;
        }

        USBPORT_ASSERT(rebalanceList != NULL);
        rebalanceListEntries = USBPORT_MAX_REBALANCE;

        // high speed endpoints will have no Tt context

        ttContext = NULL;
        if (Endpoint->Tt != NULL) {
            ASSERT_TT(Endpoint->Tt);
            translator = Endpoint->Tt;
            ttContext = Endpoint->Tt->HsbTtContextPtr;
        }

        USB2LIB_FreeUsb2BusTime(
                devExt->Fdo.HsbControllerContextPtr,
                ttContext,
                Endpoint->HsbEpContextPtr,
                rebalanceList,
                &rebalanceListEntries);

        DbgTrace((UsbpDebugTrace,"'[FREE] %p sMask = x%x cMask = x%x\n",
                Endpoint,
                Endpoint->Parameters.InterruptScheduleMask,
                Endpoint->Parameters.SplitCompletionMask));
        DbgTrace((UsbpDebugTrace,"'[FREE] Period x%x Offset x%x bw %d\n",
                Endpoint->Parameters.Period,
                Endpoint->Parameters.ScheduleOffset,
                Endpoint->Parameters.Bandwidth));

        DbgTrace((UsbpDebugTrace,"'rebalance count = %d\n",
            rebalanceListEntries));

        if (rebalanceListEntries > 0) {
            PHCD_ENDPOINT rebalanceEndpoint;
            ULONG rbIdx;

            // convert the rebalance entries to endpoints
            for (rbIdx=0; rbIdx< rebalanceListEntries; rbIdx++) {
                rebalanceEndpoint = rebalanceList->RebalanceContext[rbIdx];
                DbgTrace((UsbpDebugTrace,"'(free) rebalance Endpoint = %p\n",
                    rebalanceList->RebalanceContext[rbIdx]));

                if (rebalanceEndpoint != Endpoint) {
                    USBPORT_ASSERT(rebalanceEndpoint->RebalanceLink.Flink == NULL);
                    USBPORT_ASSERT(rebalanceEndpoint->RebalanceLink.Blink == NULL);
                    InsertTailList(&endpointList,
                                   &rebalanceEndpoint->RebalanceLink);
                }
            }
        }

        USBPORT_ASSERT(rebalanceList != NULL);
        UsbFreePool(rebalanceList);

        // process the rebalanced endpoints
        USBPORT_Rebalance(FdoDeviceObject,  &endpointList);

        USBPORT_DereferenceEndpoint(FdoDeviceObject, Endpoint->EpUsb2Handle);

        // release the budgeter
        USBPORT_UnLockUSB2bw(FdoDeviceObject, Endpoint);

    } WHILE (0);

    return;
}


ULONG
USBPORT_SelectOrdinal(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __in PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Selects an ordinal value for an USB2 endpoint.

    Endpoint Ordinal

    An endpoint ordinal is a schedule attribute of the endpoint.  The ordinal set is unique for each
    endpoint type,period,offset,speed combination.  The ordinal is used to indicate the relative
    order the endpoints should be visited by the host controller hardware.

    Interrupt Ordinals:

    We currently award these is order based on when the endpoint is opened.


Arguments:

Return Value:

    ordinal value

--*/
{
    PDEVICE_EXTENSION devExt;
    ULONG ordinal = 0;
    static StaticGlobalInterruptOrdinal = 0;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    if (!USBPORT_IS_USB20(devExt)) {
        return 0;
    }

    switch (Endpoint->Parameters.TransferType) {
    case Bulk:
    case Control:
        ordinal = 0;
        break;
    case Interrupt:
        // BUGBUG
        ordinal = StaticGlobalInterruptOrdinal++;
        break;
    case Isochronous:
        if (Endpoint->EpFlags.FatIso) {
            ordinal = 0;
        } else {
            if(Endpoint->Parameters.DeviceSpeed != HighSpeed){
                // Set the ordinal based on the start-split mask for this transfer.
                // this ensures that we schedule the transfers in the same order as
                // their start-splits.  Ordinal ranges from 1-8 depending on the least sig
                // start-split mask bit.
                ordinal = RtlFindLeastSignificantBit((ULONGLONG) Endpoint->Parameters.InterruptScheduleMask)+1;
            }
            else{
                ordinal = 1;
            }
        }
        break;
    }

    return ordinal;
}

#endif /* USB2_INTERNAL */

