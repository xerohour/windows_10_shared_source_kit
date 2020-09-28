/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    bus.c

     3-10-2002

Abstract:

    Provides bus driver functionality for the usbhub

Module:

    Entry points:

         PNP event                       Function
    ----------------------------------------------------
    Entry  -->linked to DriverEntry      NULL
    Unload -->linked to Unload           NULL
    Start  -->linked to PNP START        UsbhBusPnpStart
    Stop   -->linked to PNP STOP         UsbhBusPnpStop
    Add    -->linked to AddDevice        UsbhBusAddInit, UsbhBusAdd
    Remove -->linked to PNP REMOVE       UsbhBusRemove

    Pause -->temporarily stops the engine
    Resume-->restarts the engine

Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/

/*

    USBHUB Isr state machine


+---------------------+          (pending)
[BS_WAITING_FOR_CHANGE] <------------------------ UsbhPostInterrupt
[    UsbhHubIsr       ]                                   |
[BS_QUERY_CHANGE      ]                                   ^
+---------------------+                                   |
          |                                               |
          |                                               |
+---------------------+                                   |
[BS_QUERY_CHANGE      ]                                   |
[UsbhHubIsrWorker     ]                                   |
[    UsbhQueryChange  ]                                   |
[    UsbhAckChange    ]---------------------------------->+
[    UsbhPCE_QueueChange  ]
[BS_WAITING_FOR_CHANGE]---fails->UsbhProcessHubError-->BS_ERROR
+---------------------+                             +->BS_STOPPED
          |
          |
          |
          |
          |
 UsbhProcessChangeWorker
          |
          |--CONNECT_CHANGE
          |--RESET_CHANGE
          |--ENABLE_CHANGE
          |--OVERCURRENT_CHANGE
          |--SUSPEND_CHANGE

*************************************************************************
*/

#define BS_WAITING_FOR_CHANGE   0
#define BS_QUERY_CHANGE         1
#define BS_STOPPED              2
#define BS_ERROR                3

/*
Possible Port Status
    CONNECT         c
    ENABLE          e
    OVERCURRENT     o
    SUSPEND         s
    .......
    PDO             p

Possible Port States based on port status register, presence of PDO

PS_WAIT_CONNECT     UsbhWaitConnect
PS_ENUMERATE_1      UsbhEnumerate1
PS_ENUMERATE_2      UsbhEnumerate2

Possible Port Change conditions

  |--CONNECT_CHANGE
  |--RESET_CHANGE
  |--ENABLE_CHANGE
  |--OVERCURRENT_CHANGE
  |--SUSPEND_CHANGE
  |--TIMEOUT_CHANGE


Port State Doc Template
===========================================================================
PS_xxx   - current state
    xxx_CHANGE  - change passed in
        ceosp  - current ports status
            **scenario
        00000 --->function------------------------------->PS_xxx-next state

===========================================================================

Enumeration State Machine

              +--------------------------+
              [PS_WAIT_CONNECT           ]
              [UsbhProcessChangeWorker   ]
              [ UsbhWaitConnect          ] <----------------+
              [  UsbhAcquireEnumBusLock             ]                  |
              [PS_ENUMERATE_1            ]                  |
              +--------------------------+                  |
                           |                                |
                        lock_bus                            |
       +---------<---------|<------------------------+      |
       |               reset_port                           |
   max_retry               |                                |
       |      +--------------------------+                  |
       |      [PS_ENUMERATE_1            ]                  |
       |      [UsbhProcessChangeWorker   ]                  |
       |      [ UsbhEnumerate1           ] err_retry        |
       |      [  USBPORT!CreateDevice    ]---->-------+     |
       |      [PS_ENUMERATE_2            ]                  |
       |      +--------------------------+                  |
       |                   |                                |
 disable_port          reset_port                           |
       |                   |                                |
       |      +--------------------------+                  |
       |      [PS_ENUMERATE_2            ]                  |
       |      [UsbhProcessChangeWorker   ]                  |
       |      [ UsbhEnumerate2           ] err_retry        |
       |      [  USBPORT!InitializeDevice]---->-------+     |
       |      [                          ]                  |
       |      +--------------------------+                  |
       |                   |                                |
       |                   |                                |
       |                   |                                |
       |      +-------------------------+                   |
       |      [UsbhReleaseEnumBusLock            ]                   |
       +----->[ UsbhCreatePdo           ]----->-------------+
              [PS_WAIT_CONNECT          ]
              +-------------------------+
                           |
                      UsbhBusCheck



*******************************************************************************

We have to worry about multiple threads of execution accessing the PDO at the same time,
the most significant problem is making sure the PDO does not get deleted while a thread
is using it.

Possible Threads accessing PDO

Bus enum engine (bus.c, pchange.c)----------------------------------------------------->
Power engine (power.c)------------------------------------------------------>
Pnp operations suddenly invoked by pnp outside of our control--------------->
IOCTL apis from user mode that access the pdo thru the hub fdo-------------->
HUB Hard reset [ESD] (reset.c) ------------------------------------------------------->
Driver IOCTL request to the PDO


Pnp PDO Events:

PdoEv_PdoSurpriseRemove:
    Sent when the PDO receives a IRP_MN_SURPRISE_REMOVE.

PdoEv_RegisterPdo:
    Sent when the device associated with a PDO is enumerated and reported to Pnp.

PdoEv_Disconnect:
    Inidicates the device associated with the PDO has been physically disconnected.

PdoEv_ErrorDisconnect:
    Occurs when an error is encountered creating the PDO.  This is valid only for PDOs that have
    not been reported to Pnp.

PdoEv_BusPnpStop:
    Occurs when the bus receives a BE_BusPnpStop message. PnpStop and IRP_MN_STOP_DEVICE are not always the
    same thing, however, the 'device' presence is not changed by this message.

PdoEv_PdoRemove:
    Sent when the PDO receives an IRP_MN_SURPRISE_REMOVAL message.

PdoEv_CreatePdo
    Initial state transition when a PDO is created.

PdoEv_QBR
    Occurs when a PDO is reported to pnp.

PdoEv_QBR_Removed
    Occurs when a PDO is reported gone to Pnp.

PdoEv_BusPause
    Occurs whenever the bus handles  BE_BusPaused event.

PdoEv_BusRemove
    Occurs when the bus handles BE_BusRemove event.

PdoEv_Internal
    Internal transition -- occurs when we read state of another process.


These are in addition to the normal contentions inherent in PnP

Pdo Pnp State Transitions
--------------------------------------------------------------------------------------------
*PdoEv_ErrorDisconnect

Pdo_Created -> Pdo_Deleted       DisconnectPdo()
    This occurs if an error is encountered creating the PDO device object.  There are certain
    elements of the PDO as well as the PDO itself that are required for us to report it to
    Pnp.  If an error occurs here we delete it.

.............................................................................................
*PdoEv_BusPause

Pdo_Created -> Pdo_Deleted
    If we enter Pause mode we drop any PDOs that are semi enumerated, we will see a fresh connect
    for these when we resume.

.............................................................................................
*PdoEv_PdoSurpriseRemove

Pdo_PnpRefHwAbsent -> Pdo_HwAbsent
    This occurs when pnp decides the hardware is removed before we detect that it is disconnected.
    This can happen when removing a chain of hubs and devices.

Pdo_PnpRefHwPresent -> Pdo_PnpRefHwPresent
    This occurs when both the hub and the pdo are removed about the same time.

Pdo_HwAbsent Pdo_HwAbsent
    This is the normal case.

.............................................................................................
*PdoEv_BusPnpStop

Pdo_Created -> Pdo_Deleted
Pdo_HwPresent -> Pdo_Deleted
    All un'refed Pdo (PDOs pnp is unaware of are deleted). All ref'ed PDOs are left unchanged.



PnpPdoState State Descriptions:
----------------------------------------------------------------------------------------------


Pdo_Created - device object created by bus engine
                            * physically present
                                ;created
                                ;ID_STRINGS = yes
                                ;PDO refs hub HubFDO (UsbhConnectPdoToHub)
                                ;IncPdoIoCount
                                ;HUBFDO refs pdo, PORTDATA->pdo = pdo

Pdo_HwPresent - in the queue to be reported to pnp
                            * physically present
                                ;attributes of Pdo_Created
                                ;---on PdoPresentList
                                ;---setupdevice=yes

Pdo_PnpRefHwPresent - reported (obRef'ed) in a QBR
                            * physically present
                                ;attributes of Pdo_HwPresent
                                ;----returned to PnP in QBR (ref'ed by pnp)

Pdo_PnpRefHwAbsent - in queue to be reported gone
                            * physically gone
                                ;created
                                ;ID_STRINGS = yes
                                ;PDO refs hub HubFDO (UsbhConnectPdoToHub)
                                ;IncPdoIoCount
                                xHUBFDO refs pdo, PORTDATA->pdo != pdo
                                xnot on PdoPresentList
                                xon PdoRemovedList
                                ;setupdevice=yes
                                ;returned to PnP in QBR (still ref'ed by pnp)

Pdo_HwAbsent - reported gone in QBR
                            * physically gone
                                ;created
                                ;ID_STRINGS = yes
                                ;PDO refs hub HubFDO (UsbhConnectPdoToHub)
                                ;IncPdoIoCount
                                xHUBFDO refs pdo, PORTDATA->pdo != pdo
                                xnot on PdoPresentList
                                xnot on PdoRemovedList
                                ;setupdevice=yes
                                xnot returned to PnP in QBR (not ref'ed by pnp) (reported gone)


Pdo_Deleted - iodelete device called
                            * physically gone
                                ;created
                                ;ID_STRINGS = no
                                xPDO refs hub HubFDO (UsbhConnectPdoToHub)
                                ;IncPdoIoCount
                                xHUBFDO refs pdo, PORTDATA->pdo != pdo
                                xnot on PdoPresentList
                                xnot on PdoRemovedList
                                xsetupdevice=no
                                xnot returned to PnP in QBR (not ref'ed by pnp)



                NOT PNP                                     +         PNP
                                                            |


              Pdo_Created---->Pdo_HwPresent---+-------------+-->Pdo_PnpRefHwPresent
                                                            |
                                                            |
                                               PNP_REMOVE   |
              Pdo_Deleted<----Pdo_HwAbsent<-----------------+-----Pdo_PnpRefHwAbsent
                                                            |
                                                            +


                        old state                   new state


**********************************************************************************


*/



#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "bus.tmh"
#endif

CCHAR usbfile_bus_c[] = "bus.c";

#define USBFILE usbfile_bus_c

VOID
HalReturnToFirmware (
    __in ULONG Routine
    );

VOID
UsbhBusIoInvalidateDeviceRelations(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    Wrapper for IoInvalidateDeviceRelations(BusRelations).


Arguments:

Irql: DPC
    IoInvalidateDeviceRelations can be called at DPC

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    // make sure its a context
    ASSERT_HUB_CONTEXT(Sc);

    hubExt = FdoExt(HubFdo);


    LOG(HubFdo, LOG_POWER, 'Iov', HubFdo, Sc);

    DbgTrace((UsbhDebugTrace, "Call IoInvalidateDeviceRelations\n"));
    IoInvalidateDeviceRelations(hubExt->PhysicalDeviceObject, BusRelations);

}


VOID
UsbhNotifyPnpOfFailure_Action(
    PDEVICE_OBJECT HubFdo,
    NTSTATUS StartStatus
    )
/*++

Routine Description:

   Indicates a failure to Pnp.  This is only valid if we were successfully started <BS_Paused>.
   We really have no way of knowing if the actual start succeeds

Arguments:

Irql: PASSIVE

Return Value:

    status of start attempt

--*/

{
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;

    UsbhAssertPassive();
    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));

    // we already successfully started so we have to tell
    // PnP the device is no longer functioning
    LOG(HubFdo, LOG_POWER, 'notF', HubFdo, 0);

    // wait for PnP
    //WAIT_EVENT_TIMEOUT(&hubExt->PnpQcapsComplete, MILLISECONDS_TO_100_NS_UNITS(500), nts);
    nts = UsbhWaitEventWithTimeout(HubFdo, &hubExt->PnpQcapsComplete, 500, 'qCAP');
    // we should not normally timeout here since we only invoke the callback if we
    // report/ success to PnP.

    // this assert may indicate a bug somewhere or start_failed for some other reason.
    UsbhAssert(HubFdo, nts != STATUS_TIMEOUT);

    UsbhInvalidateHubDeviceState(hubExt, PNP_DEVICE_FAILED);
}


NTSTATUS
UsbhFinishStart(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Finishes initialization and start of the bus engine.  This may be called directly from
    PnP start or from a workitem in the case of USB20

Arguments:

Irql: PASSIVE

Return Value:

    status of start attempt

--*/

{
    USHORT pn;
    NTSTATUS nts;

    UsbhAssertPassive();

    LOG(HubFdo, LOG_POWER, 'fBST', HubFdo, 0);
    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));

    do {
        // power the hubs ports allowing us to see any devices attached
        nts = UsbhSyncPowerOnPorts(HubFdo);
        if (NT_ERROR(nts)) {
            REPORT_START_FAILURE(HubFdo, nts, DeviceGeneralFailure, "PowerOnPorts");
            break;
        }

        UsbhEnablePortIndicators(HubFdo);

        // before posting the first transfer check all the ports and simulate a
        // connect change for any that show devices connected without a connect
        // change.  Sometimes hubs will not send a connect change if the device is
        // already connected at init time.

        for (pn=1; pn<=N_PORTS(HubFdo); pn++) {
            USB_HUB_PORT_STATE ps;
            USBD_STATUS usbdStatus;

            nts = UsbhQueryPortState(HubFdo,
                                     pn,
                                     &ps,
                                     &usbdStatus);

            if (NT_ERROR(nts)) {

                LOG(HubFdo, LOG_POWER, 'ini1', pn+1, nts);

                if (!Usb_Disconnected(nts)) {
                    // we don't expect errors here so log an exception
                    // this will cause start to fail
                    HUB_EXCEPTION(USBFILE, HubFdo, pn,
                              HubErr_GetPortStatusInitFailed,
                              nts, usbdStatus, NULL, 0);
                }

                // since we are marked started, BusStop will run
                DbgTrace((UsbhDebugTrace,"%!FUNC! %!STATUS!<\n", nts));
                break;
            }

            if (ps.Status.Connect && !ps.Change.ConnectChange) {

                // queue a fake connect change for this port

                LOG(HubFdo, LOG_POWER, 'ini2', pn, nts);

                //UsbhAssert(HubFdo, !ps.Status.Enable);
                if (ps.Status.Enable) {
                    PHUB_PORT_DATA pd;

                    // port was left in the enabled state for some reason.  This can happen after the HCT runs
                    // and re-enables the hub.
                    // if we detect this then we just disable the port before queueing op the artificail connect
                    // change.
                    pd = UsbhGetPortData(HubFdo, pn);

                    if (pd) {
                        UsbhDisablePort(HubFdo, pd);
                    }

                }

                // queue the change message to the bus engine.

                UsbhQueueSoftConnectChange(HubFdo, pn, StateContext, FALSE);

            }

        }
    } WHILE (0);


    if (NT_SUCCESS(nts)) {
        UsbhEnableHardReset(HubFdo, StateContext);
    }

    DbgTrace((UsbhDebugTrace,"%!FUNC! %!STATUS!<<<\n", nts));
    LOG(HubFdo, LOG_POWER, 'fnsS', HubFdo, nts);


    return nts;

}




NTSTATUS
UsbhBusPnpStart(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    starts the hub driver enumeration state machine

Arguments:

Irql: PASSIVE

Return Value:

    nt status

--*/

{
    PIRP irp;
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;
    USHORT p;
    HUB_BUS_STATE bs;

    // state context for start -- this must be dynamically allocated because its
    // asynchronous in some cases

    hubExt = FdoExt(HubFdo);

    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));
    LOG(HubFdo, LOG_BUS, 'bSTR', 0, 0);

    // dpc used for retries on allocation failures
    hubExt->IsrRetryTimer = UsbhAllocateTimerObject(HubFdo, UsbhIsrRetryTimer, 'IsrR');
    hubExt->ResetIrqPipeRetryTimer = UsbhAllocateTimerObject(HubFdo, UsbhResetIrqPipeRetryTimer, 'RIrq');
    hubExt->HubInitTimeoutTimer = UsbhAllocateTimerObject(HubFdo, UsbHubhInitTimeoutTimer, 'InTo');
    hubExt->HubInitRetryTimer = UsbhAllocateTimerObject(HubFdo, UsbhInitRetryTimer, 'InRe');

    if (hubExt->IsrRetryTimer == NULL ||
        hubExt->ResetIrqPipeRetryTimer == NULL ||
        hubExt->HubInitTimeoutTimer == NULL ||
        hubExt->HubInitRetryTimer == NULL) {
        goto UsbhBusPnpStart_Fail;
    }


    irp = IoAllocateIrp(hubExt->TopOfStackDeviceObject->StackSize, FALSE);

    // get an irp
    if (irp == NULL) {
        goto UsbhBusPnpStart_Fail;
    }

    hubExt->IrqIrp = irp;

    // get a urb
    UsbhAllocatePool_Z(hubExt->IrqUrb,
                       NonPagedPool,
                       sizeof(struct _URB));

    if (hubExt->IrqUrb == NULL) {
        goto UsbhBusPnpStart_Fail;
    }

    // get a transfer buffer
    hubExt->IrqBuffer = NULL;
    if (hubExt->IrqPipeInformation.MaximumPacketSize) {
        UsbhAllocatePool_Z(hubExt->IrqBuffer,
                           NonPagedPool,
                           hubExt->IrqPipeInformation.MaximumPacketSize);
    }

    if (hubExt->IrqBuffer == NULL) {
        goto UsbhBusPnpStart_Fail;
    }

    // allocate port data
    UsbhAssert(HubFdo, N_PORTS(HubFdo));

    hubExt->PortData = NULL;
    if (N_PORTS(HubFdo)) {
        UsbhAllocatePool_Z(hubExt->PortData,
                           NonPagedPool,
                           sizeof(struct _HUB_PORT_DATA)*N_PORTS(HubFdo));
    }

    if (hubExt->PortData == NULL) {
        goto UsbhBusPnpStart_Fail;
    }

    hubExt->NumberOfPorts = N_PORTS(HubFdo);

    // Pre-allocate the hard reset work item
    hubExt->HardResetWorkItem = IoAllocateWorkItem(HubFdo);
    if (!hubExt->HardResetWorkItem) {
            goto UsbhBusPnpStart_Fail;
    }

    pd = hubExt->PortData;
    for (p=0; p<N_PORTS(HubFdo); p++) {

        // init port data structure
        pd->Sig = SIG_HUBPORTDATA;
        UsbhInitStateCtx(HubFdo, PDC(pd), HubCtx_PortData, PDC(pd));
        UsbhInitStateCtx(HubFdo, PDOC(pd), HubCtx_Overcurrent, PDOC(pd));

        pd->HubFdo = HubFdo;
        pd->PortNumber = p+1;
        pd->CurrentState = PS_WAIT_CONNECT;

        KeInitializeTimer(&pd->ResetTimer);
        KeInitializeDpc(&pd->ResetTimerDpc,
                        UsbhResetPortTimerDpc,
                        pd);

        // initial state is signaled because pd->ResetTimeoutPending is NULL
        INIT_EVENT_SIGNALED(&pd->ResetTimeoutEvent);

        INIT_EVENT_NOT_SIGNALED(&pd->PortChangeWorkerDone);
        pd->PortChangeWorkItemReference = 1;
        pd->PortChangeWorkerState = PortChangeWorker_Idle;
        KeInitializeSpinLock(&pd->PortChangeWorkerSpin);

        // Pre-allocate the port change work item
        pd->PortChangeWorkItem = IoAllocateWorkItem(HubFdo);
        if (!pd->PortChangeWorkItem) {
            goto UsbhBusPnpStart_Fail;
        }

        // initialize port change queues
        UsbhInitPortChangeList(HubFdo, pd);

        // blinking lights
        UsbhInitPortPindicator(HubFdo, pd);

        pd++;
    }

    // **
    // we need to deal with the USB 2.0 bus boot problems here.
    // the hub registers a callback when started and this
    // call back is what posts the initial interrupt transfer
    // This allows the port driver to control when the hubs actually
    // begin to enumerate devices.
    // **

    //->not signaled
    // we need to know when pnp has completed start, the only way to tell is to
    // look at the next irp sent which is Query Caps.

    bs = UsbhDispatch_BusEvent(HubFdo, StateContext, BE_BusPnpAsyncStart);

    LOG(HubFdo, LOG_BUS, '>STR', 0, 0);

    return STATUS_SUCCESS;

UsbhBusPnpStart_Fail:

    if (hubExt->HardResetWorkItem) {
        IoFreeWorkItem(hubExt->HardResetWorkItem);
        hubExt->HardResetWorkItem = NULL;
    }

    if (hubExt->PortData) {
        pd = hubExt->PortData;
        for (p=0; p<N_PORTS(HubFdo); p++) {
            if (pd->PortChangeWorkItem) {
                IoFreeWorkItem(pd->PortChangeWorkItem);
                pd->PortChangeWorkItem = NULL;
            }
            pd++;
        }
    }

    UsbhFreeTimerObject(HubFdo, &hubExt->IsrRetryTimer);
    UsbhFreeTimerObject(HubFdo, &hubExt->ResetIrqPipeRetryTimer);
    UsbhFreeTimerObject(HubFdo, &hubExt->HubInitTimeoutTimer);
    UsbhFreeTimerObject(HubFdo, &hubExt->HubInitRetryTimer);

    if (hubExt->IrqIrp) {
        IoFreeIrp(hubExt->IrqIrp);
        hubExt->IrqIrp = NULL;
    }

    UsbhFreePool(hubExt->IrqUrb);
    UsbhFreePool(hubExt->IrqBuffer);
    UsbhFreePool(hubExt->PortData);

    return STATUS_INSUFFICIENT_RESOURCES;

}


VOID
UsbhBusRemove(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Frees Bus resources

    called in context of IRP_MN_REMOVE, undoes add

Arguments:

Irql: PASSIVE

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    HUB_BUS_STATE bs;
    PHUB_PORT_DATA pd;
    USHORT p;

    hubExt = FdoExt(HubFdo);

    bs = UsbhDispatch_BusEvent(HubFdo, StateContext, BE_BusRemove);
    UsbhAssert(HubFdo, bs == BS_BusRemove);

    if (hubExt->HardResetWorkItem) {
        IoFreeWorkItem(hubExt->HardResetWorkItem);
        hubExt->HardResetWorkItem = NULL;
    }

    if (hubExt->PortData) {
        pd = hubExt->PortData;
        for (p=0; p<N_PORTS(HubFdo); p++) {
            if (pd->PortChangeWorkItem) {
                IoFreeWorkItem(pd->PortChangeWorkItem);
                pd->PortChangeWorkItem = NULL;
            }
            pd++;
        }
    }

    UsbhFreePool(hubExt->PortData);
}


VOID
UsbhBusAddInit(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    sets the initial state for the bus enum engine.

Arguments:

Irql: PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    hubExt->CurrentBusState = BS_BusInit;
    KeInitializeSemaphore(&hubExt->BusStateLock, 1, 1);

    InitializeListHead(&hubExt->PdoPresentListHead);
    InitializeListHead(&hubExt->PdoRemovedListHead);
    InitializeListHead(&hubExt->PdoAbsentListHead);

    InitializeListHead(&hubExt->WorkItemListHead);
    KeInitializeSpinLock(&hubExt->WorkItemListSpin);


    KeInitializeSpinLock(&hubExt->DevHandleSpin);

    KeInitializeSpinLock(&hubExt->PortResetSpin);

    KeInitializeSpinLock(&hubExt->SyncDeviceResetSpin);

    KeInitializeSpinLock(&hubExt->UsbEnumLockSpin);

    KeInitializeSemaphore(&hubExt->PindicatorLock, 1, 1);

    // we can get this pnp irp any time after ADD
    hubExt->FdoFlags.DeviceCaps = 0;
    INIT_EVENT_NOT_SIGNALED(&hubExt->PnpQcapsComplete);

    INIT_EVENT_SIGNALED(&hubExt->IrqDoneEvent);
}


NTSTATUS
UsbhBusAdd(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    sets the initial state for the bus enum engine.

Arguments:

Irql: PASSIVE

Return Value:

    the state transitioned to by this call (may not be the current state).

--*/
{
    HUB_BUS_STATE bs;

    bs = UsbhDispatch_BusEvent(HubFdo, StateContext, BE_BusAdd);
    UsbhAssert(HubFdo, bs == BS_BusStop);

    return STATUS_SUCCESS;
}


VOID
UsbhBusPnpStop(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    stops the hub driver enumeration state machine

    this is a synchronous function, enumeration machine is stopped
    and all resources are freed on return

    This function is associated with pnp stop and remove

Arguments:

Irql: PASSIVE

Return Value:

    none

--*/
{
    HUB_BUS_STATE bs;

    // hard reset sits above the bus state machine we must disable it before we try to stop
    UsbhDisableHardReset(HubFdo, Sc);

    bs = UsbhDispatch_BusEvent(HubFdo, Sc, BE_BusPnpStop);

    return;
}


ULONG
UsbhResetIrqPipeRetryTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    )
/*++

Routine Description:

    This routine attempts to queue an Reset IRQ Pipe workitem -- it keeps trying until it succeeds.  This code
    would only run in the case where allocation failures are enabled.

Arguments:

//ntst

  Context - workitem context.

   Parameter1 - signature.

   Parameter2 - not used.

Irql: DISPATCH

Return Value:

--*/
{
    ULONG retry;

    UsbhAssert(HubFdo, Parameter1 == SIG_HUBIRQRST_WORKITEM);

    retry = UsbhQueueCriticalWorkItem(HubFdo,
                              NULL, // use auto retry
                              UsbhHubResetIrqPipeWorker,
                              Context,
                              PortNumber,
                              SIG_HUBIRQRST_WORKITEM);

    return retry;
}


ULONG
UsbhIsrRetryTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    )
/*++

Routine Description:

    This routine attempts to queue an hub ISR reset workitem -- it keeps trying until it succeeds.  This code
    would only run in the case where allocation failures are enabled.

Arguments:

//ntst

  Context - workitem context.

   Parameter1 - signature.

   Parameter2 - not used.

Irql: DISPATCH

Return Value:

--*/
{
    ULONG retry;

    UsbhAssert(HubFdo, Parameter1 == SIG_HUBISR_WORKITEM);

    retry = UsbhQueueCriticalWorkItem(HubFdo,
                              NULL, // use auto retry
                              UsbhHubIsrWorker,
                              Context,
                              PortNumber,
                              SIG_HUBISR_WORKITEM);

    return retry;
}

NTSTATUS
UsbhHubIsr(
    PDEVICE_OBJECT PNull,
    PIRP Irp,
    PVOID Context
    )
/*++

Routine Description:

    Processes an interrupt transfer from the hub

    IrqCancelFlag - this is our own cancel flag we use to track if an Irq Irp
    is pending in the port driver.  It has three states:
        -1 = Irq is not pending and the enum state machine is stopping
         0 = Irq is not pending
         1 = Irq is pending in the port driver

        *dec

        1->0 = cancel, check status
        *inc
        0->1 Post transfer
        -1->0 do not re-post transfer, signal done event

Arguments:

Irql: DISPATCH

Return Value:

--*/
{
    PDEVICE_OBJECT hubFdo = Context;
    PDEVICE_EXTENSION_HUB  hubExt;
    PURB urb;
    ULONG pn;
    NTSTATUS nts;
    LONG cancelFlag;
    PHUB_PORT_DATA pd;
    KIRQL irql;
    PSTATE_CONTEXT sc;

    DbgTrace((UsbhDebugTrace,"%!FUNC!\n"));
    hubExt = FdoExt(hubFdo);

    urb = hubExt->IrqUrb;

    // good old change indication
    LOG(hubFdo, LOG_BUS, 'chID', urb, Irp);
    // irp is no longer pending, value will be -1 if we are stopping
    // or zero if we are not
    cancelFlag = InterlockedDecrement(&hubExt->IrqCancelFlag);

    LOG(hubFdo, LOG_BUS, 'chcf', urb, cancelFlag);

    nts = Irp->IoStatus.Status;
    LOG(hubFdo, LOG_BUS, 'chI2', nts, urb->UrbHeader.Status);

    if (NT_SUCCESS(nts) &&
        urb->UrbBulkOrInterruptTransfer.TransferBufferLength == 0) {

        LOG(hubFdo, LOG_BUS, 'irZR', nts, urb->UrbHeader.Status);
        HUB_EXCEPTION(USBFILE, hubFdo, 0, HubErr_InterruptZeroBytes,
                                  nts, urb->UrbHeader.Status, NULL, 0);

        // map this to an error so we can handle it as such
       // nts = STATUS_UNSUCCESSFUL;

    }


    if (NT_ERROR(nts) ||
        USBD_ERROR(urb->UrbHeader.Status) ||
        cancelFlag == -1) {

        if ((nts == STATUS_CANCELLED && urb->UrbHeader.Status == USBD_STATUS_CANCELED) ||
            (nts == STATUS_UNSUCCESSFUL && urb->UrbHeader.Status == USBD_STATUS_DEV_NOT_RESPONDING)) {
            //
            // Make sure we don't leave thread blocked.
            // we know...
            // USBD_STATUS_DEV_NOT_RESPONDING
            // STATUS_CANCELLED
            //

            for (pn = 1; pn <= N_PORTS(hubFdo); pn++) {
                pd = UsbhGetPortData(hubFdo, (USHORT)pn);
                Usbh_CheckPortHwPendingStatus(hubFdo, pd, FALSE);
            }
        }

        if (Usb_Disconnected(nts)) {
            // if the hub is unplugged then no action is necessary
            // we don't bother to repost or talk to the hub anymore

            SET_EVENT(&hubExt->IrqDoneEvent);
            return STATUS_MORE_PROCESSING_REQUIRED;
        }

        if (nts == STATUS_CANCELLED || cancelFlag == -1) {
            // if the irp was cancelled then we are waiting for
            // it to complete so we can stop or pause

            LOG(hubFdo, LOG_BUS, 'chI3', nts, cancelFlag);

            SET_EVENT(&hubExt->IrqDoneEvent);
            return STATUS_MORE_PROCESSING_REQUIRED;
        }

        hubExt->IrqErrorCount++;

        // three strikes and we throw an exception
        if (hubExt->IrqErrorCount >= 3) {
            SET_EVENT(&hubExt->IrqDoneEvent);
            // log a hub hardware error and reset
            // reset!
            HUB_EXCEPTION(USBFILE, hubFdo, 0, InterruptPipeFailure_Reset,
                          nts, urb->UrbHeader.Status, NULL, 0);

            return STATUS_MORE_PROCESSING_REQUIRED;
        }

        // pipe may be stalled, we need to reset it for the
        // next transfer to work
        // since we only have one xfer at a time we don't need to bother with
        // AbortPipe

        hubExt->FdoFlags.ResetIrqPipe = 1;

        // Queue the ISR Reset worker
        UsbhQueueCriticalWorkItem(hubFdo,
                              hubExt->ResetIrqPipeRetryTimer,
                              UsbhHubResetIrqPipeWorker,
                              PNP_CONTEXT(hubFdo),
                              0,
                              SIG_HUBIRQRST_WORKITEM);

        return STATUS_MORE_PROCESSING_REQUIRED;

    }

    // a successful transfer resets the error count
    hubExt->IrqErrorCount = 0;

    // what port?
    // take the first change bit we see
    for (pn = 0; pn <= N_PORTS(hubFdo); pn++) {
        if (IsBitSet(hubExt->IrqBuffer, pn)) {
            break;
        }
    }

    if (pn > N_PORTS(hubFdo)) {
        // hub reports bogus change bit

        // bit zero indicates no port.
        // we also treat an invalid port number like a hub status change (no port)
        // and check the hub itself.
        LOG(hubFdo, LOG_BUS, 'BdPc', pn, 0);
        HUB_EXCEPTION(USBFILE, hubFdo, 0, HubErr_InvalidChangeBitReported,
                                  0, 0, NULL, 0);
        pn = 0;
    }

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!\n", pn));

    KeAcquireSpinLock(&HubG.HubIsrListSpin, &irql);

    sc = PNP_CONTEXT(hubFdo);

    UsbhAssert(HubFdo, sc->HubIsrWorkerState == HubIsrWorker_NotQueued);

    // Add this change notification to the global list of notifications
    sc->PortNumber = pn;
    sc->HubIsrWorkerState = HubIsrWorker_Queued;
    InsertTailList(&HubG.HubIsrListHead, &sc->HubIsrListLink);

    // Reference the work-item for this hub
    UsbhIncrementHubIsrWorkerReference(hubFdo);

    KeReleaseSpinLock(&HubG.HubIsrListSpin, irql);

    // Queue the ISR worker
    UsbhQueueCriticalWorkItem(hubFdo,
                              hubExt->IsrRetryTimer,
                              UsbhHubIsrWorker,
                              PNP_CONTEXT(hubFdo),
                              pn,
                              SIG_HUBISR_WORKITEM);

    // Notify any waiting thread that there is a port change notification to process
    SET_EVENT(&HubG.HubIsrWorkerQueued);

    return STATUS_MORE_PROCESSING_REQUIRED;
}


VOID
UsbhPostInterrupt(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

Irql: ANY

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PURB urb;
    PIRP irp;
    PIO_STACK_LOCATION nextStk;
    NTSTATUS nts;
    CCHAR stackSize;

    DbgTrace((UsbhDebugTrace,"%!FUNC!\n"));

    LOG(HubFdo, LOG_BUS, 'irq>', 0, 0);
    hubExt = FdoExt(HubFdo);
    urb = hubExt->IrqUrb;
    irp = hubExt->IrqIrp;

    UsbhFatalAssert(HubFdo,urb!=NULL, HubFdo);
    UsbhFatalAssert(HubFdo,irp!=NULL, HubFdo);

    // null out device handle in case we are the root hub
    urb->UrbHeader.UsbdDeviceHandle = NULL;

    // set up the control transfer
    urb->UrbBulkOrInterruptTransfer.Hdr.Length =
        sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);
    urb->UrbBulkOrInterruptTransfer.Hdr.Function =
        URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER;

    urb->UrbBulkOrInterruptTransfer.PipeHandle =
        hubExt->IrqPipeInformation.PipeHandle;
    urb->UrbBulkOrInterruptTransfer.TransferFlags = USBD_SHORT_TRANSFER_OK;
    urb->UrbBulkOrInterruptTransfer.TransferBufferLength =
            hubExt->IrqPipeInformation.MaximumPacketSize;
    urb->UrbBulkOrInterruptTransfer.TransferBuffer =
            hubExt->IrqBuffer;

    // reinitialize the transfer buffer so we don't act on stale data.
    // The really only happens if there is a bug somewhere else but it
    // won't hurt us to take this extra safety step.
    RtlZeroMemory(hubExt->IrqBuffer, hubExt->IrqPipeInformation.MaximumPacketSize);

    urb->UrbBulkOrInterruptTransfer.TransferBufferMDL = NULL;

    stackSize = hubExt->TopOfStackDeviceObject->StackSize;

    IoInitializeIrp(irp,
                    (USHORT) (sizeof(struct _IRP) + stackSize *
                        sizeof(struct _IO_STACK_LOCATION)),
                    (CCHAR) stackSize);

    nextStk = IoGetNextIrpStackLocation(irp);
    nextStk->Parameters.Others.Argument1 = urb;
    nextStk->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
    nextStk->Parameters.DeviceIoControl.IoControlCode =
        IOCTL_INTERNAL_USB_SUBMIT_URB;

    if (InterlockedIncrement(&hubExt->IrqCancelFlag)) {
        // 0->1 irq is pending
        UsbhSetCompletionRoutine(HubFdo,
                                 irp,    // Irp
                                 UsbhHubIsr,
                                 HubFdo, // context
                                 TRUE,   // invoke on success
                                 TRUE,  // invoke on error
                                 TRUE); // invoke on cancel
        nts = IoCallDriver(hubExt->TopOfStackDeviceObject, irp);
        LOG(HubFdo, LOG_BUS, 'iqq>', nts, 0);
    } else {
        //-1->0 don't post interrupt, we want to stop the enum engine
        LOG(HubFdo, LOG_BUS, 'iqs>', 0, 0);

        InterlockedDecrement(&hubExt->IrqCancelFlag);
        SET_EVENT(&hubExt->IrqDoneEvent);
    }

    return;

}


NTSTATUS
UsbhResetIrqPipe(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Perform a synchronous reset of the hubs IRQ pipe

Irql: PASSIVE

Arguments:

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PURB urb;
    NTSTATUS nts;

    UsbhAssertPassive();
    hubExt = FdoExt(HubFdo);

    UsbhAllocatePool_Z(urb, NonPagedPool, sizeof(struct _URB_PIPE_REQUEST));

    if (urb) {
        // null out device handle in case we are the root hub
        urb->UrbHeader.UsbdDeviceHandle = NULL;

        // set up the control transfer
        urb->UrbPipeRequest.Hdr.Length =
            sizeof(struct _URB_PIPE_REQUEST);
        urb->UrbPipeRequest.Hdr.Function =
            URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL;

        urb->UrbPipeRequest.PipeHandle =
            hubExt->IrqPipeInformation.PipeHandle;

        nts = UsbhSyncSendInternalIoctl(HubFdo,
                                      IOCTL_INTERNAL_USB_SUBMIT_URB,
                                      urb,
                                      NULL);

        LOG(HubFdo, LOG_BUS, 'sySU', nts, urb->UrbPipeRequest.Hdr.Status);

        UsbhFreePool(urb);
    } else {
        nts = STATUS_INSUFFICIENT_RESOURCES;
    }

    return nts;

}


NTSTATUS
UsbhQueryChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PUSB_HUB_HW_PORT_CHANGE PortChange,
    USBD_STATUS *ReturnUsbdStatus
    )
/*++

Routine Description:

    synchronously query port status and change bits, we only
    report one change even if multiple changes are asserted.

    the other changes will be re-asserted when we post another interrupt
    transfer since they don't get ACKed.

    Caller invokes reset if necessary

Irql: PASSIVE

Arguments:

Return Value:

    optional: USBD status code if pointer is passed in

    nt status

--*/
{
    USB_HUB_PORT_STATE portState;
    NTSTATUS nts;
    USBD_STATUS usbdStatus;
    PHUB_PORT_DATA pd;

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!\n", PortNumber));

    usbdStatus = USBD_STATUS_SUCCESS;

    pd = UsbhGetPortData(HubFdo, (USHORT)PortNumber);

    PortChange->us = 0;

    nts = UsbhQueryPortState(HubFdo,
                             PortNumber,
                             &portState,
                             &usbdStatus);

    if (NT_SUCCESS(nts)) {

        USHORT changeBit;
        USHORT tmp = portState.Change.us;

        // mask off all but one change bit
        changeBit = 1;
        while ((changeBit & tmp) == 0) {
            changeBit = changeBit<<1;

            // bad change bit reported by the hub
            if (changeBit & UNUSED_HW_CHANGE_BITS) {
                //#t26
                // this indicates a problem on the hub.
                DbgTrace((UsbhDebugTrace, "change indicated -- nothing has changed\n"));
                LOG(HubFdo, LOG_BUS, 'cNOc', 0, 0);

                // Root hub ports may generate spurious port change notifications
                // immediately after resume that do not affect functionality of the root hub.
                // We try to ingore these to avoid triggering a unneccessary hard reset
                // of the root hub on resume.
                if (FdoExt(HubFdo)->FdoFlags.HubIsRoot) {
                    pd->SpuriousChangeCount++;

                    // Only reset the root hub if we see 3 spurious change notifications in a row.
                    if (pd->SpuriousChangeCount < 3) {
                        break;
                    }
                }

                usbdStatus = USBD_STATUS_HUB_INTERNAL_ERROR;
                HUB_EXCEPTION(USBFILE, HubFdo, PortNumber, HubErr_NoChangeBit,
                          nts, usbdStatus, &portState, sizeof(struct _USB_HUB_PORT_STATE));
                changeBit = 0;
                // we indicate an error which will cause the change to be
                // ignored
                nts = STATUS_UNSUCCESSFUL;
                break;
            }
        }

        if (changeBit & VALID_HW_CHANGE_BITS) {
            PortChange->us |= changeBit;
        }
    }

    if (PortChange->us) {
        pd->SpuriousChangeCount = 0;
    }

    LOG(HubFdo, LOG_BUS, 'Pchg', PortChange->us, nts);
    SET_OPTIONAL_STATUS(ReturnUsbdStatus, usbdStatus);

    return nts;

}


NTSTATUS
UsbhQueryPortState(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PUSB_HUB_PORT_STATE PortState,
    USBD_STATUS *ReturnUsbdStatus
    )
/*++

Routine Description:

    Fetches the DWORD port status and port change bits from the hub

Irql: PASSIVE

Arguments:

Return Value:

    optional: returns last usbd status received

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_EXTENSION_PDO pdoExt = NULL;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    EX_DATA portExData;
    NTSTATUS nts;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    USBD_STATUS usbdStatus2 = USBD_STATUS_SUCCESS;
    PDEVICE_OBJECT pdo = NULL;
    USB_HUB_PORT_STATE bootPortState;

    hubExt = FdoExt(HubFdo);

    // if we are currently surprise or soft removed return an error that reflects this.  We want
    // to keep the bus engine from having to act on potentially bogus state values when it process
    // changes.
    // -- changes could have been queued .
    // Test #113 reproduces this.
    if (hubExt->FdoFlags.HubHwNotPresent) {
         LOG(HubFdo, LOG_BUS, 'surE', 0, 0);
         SET_OPTIONAL_STATUS(ReturnUsbdStatus, USBD_STATUS_DEVICE_GONE);
         return NT_DEVICE_DISCONNECTED_STATUS;
    }

    if (hubExt->FdoFlags.HubHardResetPending) {
         LOG(HubFdo, LOG_BUS, 'hrEP', 0, 0);
         SET_OPTIONAL_STATUS(ReturnUsbdStatus, USBD_STATUS_HUB_INTERNAL_ERROR);
         return STATUS_ADAPTER_HARDWARE_ERROR;
    }

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

    //
    // If this is a boot disk and there is a state change
    // we better know about it and catch it here
    pdo = UsbhLatchPdo(HubFdo, PortNumber, NULL, PdoBusSurpriseRemove_Tag);
    if (pdo != NULL) {
        pdoExt = PdoExt(pdo);
    }

    if (NT_SUCCESS(nts) &&
        pdoExt != NULL &&
        pdoExt->IsBootDevice) {

        bootPortState.Change.us = portExData.State.Change;
        bootPortState.Status.us = portExData.State.Status;

        if ( bootPortState.Change.OvercurrentChange == 1 ||
                bootPortState.Status.Overcurrent == 1) {
            // Lets trigger a hub reset here. That would in the worst case
            // turn off the machine. In the best case it would cause a successful
            // recovery
            UsbhDispatch_HardResetEvent(HubFdo, RESET_CONTEXT(HubFdo), HRE_RequestReset);

        }

        // The boot device was plugged out inform downstream hubs
        if (bootPortState.Status.Connect == 0 &&
            bootPortState.Change.ConnectChange == 1) {

            DbgTrace((UsbhPwBootTrace,"Boot device removed from #%p %!FUNC!\n", HubFdo));

            //
            // SQMSurpriseRemovalUnplug needs to be set here to indicate a surprise
            // removal unplug has occurred.  The reason it is done here is because
            // UsbhQueryPortState() is not on the resume path.  If
            // SQMSurpriseRemovalUnplug is set on the resume path then a system
            // suspend/resume will cause the surprise removal count to be incorrectly
            // updated.
            //
            HubG.SQMSurpriseRemovalUnplug = TRUE;
            UsbhFlushPortChange(HubFdo, PortNumber, &usbdStatus2);
            UsbhDispatch_HardResetEvent(HubFdo, RESET_CONTEXT(HubFdo), HRE_RequestReset);
        }

    }

    if (pdo != NULL) {
        UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoBusSurpriseRemove_Tag);
    }

    LOG(HubFdo, LOG_BUS, 'Qpst', nts, usbdStatus);

    if (NT_SUCCESS(nts) &&
        portExData.nBytes != sizeof(struct _USB_HUB_PORT_STATE)) {
        // hub shorted us on the data
        // throw a hardware exception and return an error
        LOG(HubFdo, LOG_BUS, 'Qps1', portExData.nBytes, usbdStatus);

        HUB_EXCEPTION(USBFILE, HubFdo, PortNumber, HubErr_BadControlData,
                          nts, usbdStatus, &portExData, sizeof(struct _EX_DATA));

        SET_OPTIONAL_STATUS(ReturnUsbdStatus, usbdStatus);

        return STATUS_UNSUCCESSFUL;
    }

    if (NT_ERROR(nts) &&
        !Usb_Disconnected(nts)) {
        // throw an exception so we know why it failed
        LOG(HubFdo, LOG_BUS, 'Qps2', portExData.nBytes, usbdStatus);
        HUB_EXCEPTION(USBFILE, HubFdo, PortNumber, HubErr_GetPortStatusFailed,
                          nts, usbdStatus, &portExData, sizeof(struct _EX_DATA));
    }

    if (NT_SUCCESS(nts)) {
        // return status and change bits
        PortState->Change.us = portExData.State.Change;
        PortState->Status.us = portExData.State.Status;

        LOG(HubFdo, LOG_BUS, 'Ppst', PortState->Change.us,
            PortState->Status.us);

    }

    SET_OPTIONAL_STATUS(ReturnUsbdStatus, usbdStatus);

    return nts;

}


NTSTATUS
UsbhQueryHubState(
    PDEVICE_OBJECT HubFdo,
    PUSB_HUB_STATE HubState,
    USBD_STATUS *ReturnUsbdStatus
    )
/*++

Routine Description:

    Fetches the DWORD hub status and change bits from the hub

    **The hub must be in the configured state for this to be valid. See 11.24.2.7

Irql: PASSIVE

Arguments:

Return Value:

    optional: returns last usbd status received

    nt status

--*/
{
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    EX_DATA hubExData;
    NTSTATUS nts;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;

    hubExData.nBytes = sizeof(struct _USB_HUB_STATE);
    HubState->Change.AsUshort16 = 0;
    HubState->Status.AsUshort16 = 0;

    hubExData.PortNumber = 0;
    hubExData.State.Change = 0;
    hubExData.State.Status = 0;

    // setup packet for Get Port Status
    // 11.24.2.7
    setupPacket.bmRequestType.B = 0xa0; //10100000B
    setupPacket.bRequest =
        hubExData.bRequest = USB_REQUEST_GET_STATUS;
    setupPacket.wValue.W = 0;
    setupPacket.wIndex.W = 0;
    setupPacket.wLength = hubExData.nBytes;
    // spec specifies 4
    UsbhAssert(HubFdo, hubExData.nBytes == 4);

    nts = UsbhSyncSendCommand(HubFdo,
                              &setupPacket,
                              (PUCHAR)&hubExData.State,
                              &hubExData.nBytes,
                              USBH_CONTROL_PIPE_TIMEOUT,
                              &usbdStatus);

    LOG(HubFdo, LOG_BUS, 'Qhst', nts, usbdStatus);

    if (NT_SUCCESS(nts) &&
        hubExData.nBytes != sizeof(struct _USB_HUB_PORT_STATE)) {
        // hub shorted us on the data
        // throw a hardware exception and return an error
        LOG(HubFdo, LOG_BUS, 'Qps1', hubExData.nBytes, usbdStatus);

        HUB_EXCEPTION(USBFILE, HubFdo, 0, HubErr_BadControlData,
                          nts, usbdStatus, &hubExData, sizeof(struct _EX_DATA));

        SET_OPTIONAL_STATUS(ReturnUsbdStatus, usbdStatus);

        return STATUS_UNSUCCESSFUL;
    }

    if (NT_ERROR(nts) &&
        !Usb_Disconnected(nts)) {
        // throw an exception so we know why it failed
        LOG(HubFdo, LOG_BUS, 'Qps2', hubExData.nBytes, usbdStatus);
        HUB_EXCEPTION(USBFILE, HubFdo, 0, HubErr_GetPortStatusFailed,
                          nts, usbdStatus, &hubExData, sizeof(struct _EX_DATA));
    }

    if (NT_SUCCESS(nts)) {
        // return status and change bits
        HubState->Change.AsUshort16 = hubExData.State.Change;
        HubState->Status.AsUshort16 = hubExData.State.Status;

        LOG(HubFdo, LOG_BUS, 'Hpst', HubState->Change.AsUshort16,
            HubState->Status.AsUshort16);

    }

    SET_OPTIONAL_STATUS(ReturnUsbdStatus, usbdStatus);

    return nts;

}



VOID
UsbhResetPortTimerDpc(
    PKDPC Dpc,
    PVOID DeferredContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2
    )
/*++

Routine Description:

    Timeout for a reset port request.  We queue an artificial change to the state
    machine when the timeout occurs so enumeration can be retried or failed

Irql: DISPATCH

Arguments:

    Dpc - Pointer to the DPC object.

    DeferredContext - the PORT_DATA structure for the port that was reset.

    SystemArgument1 - not used.

    SystemArgument2 - not used.

Return Value:

    None.

--*/
{
    PHUB_PORT_DATA portData = DeferredContext;
    USB_HUB_PORT_EVENT pc;
    PDEVICE_EXTENSION_HUB  hubExt;
    KIRQL irql;
    PPCQ_TIMEOUT_OBJECT tmoObj;

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!\n", portData->PortNumber));

    LOG(portData->HubFdo, LOG_BUS, 'Rdpc', DeferredContext, portData->PortNumber);


    DbgTrace((UsbhDebugTrace,"#%d Port Reset timed out\n", portData->PortNumber));

    // queue an artificial change indication for this port
    hubExt = FdoExt(portData->HubFdo);

    // release any pending cancel request by setting this event
    KeAcquireSpinLock(&hubExt->PortResetSpin, &irql);
    LOG(portData->HubFdo, LOG_BUS, 'Rdp1', DeferredContext,
        portData->PortNumber);

    UsbhAssert(portData->HubFdo, portData->ResetTimerPending != NULL);
    tmoObj = portData->ResetTimerPending;
    UsbhFatalAssert(portData->HubFdo,tmoObj != NULL, NULL);
    UsbhFatalAssert(portData->HubFdo,tmoObj->Sig == SIG_TMOOBJ, tmoObj);

    LOG(portData->HubFdo, LOG_BUS, 'Rtmo', tmoObj, portData->PortNumber);

    // this should not be signalled
    UsbhAssert(portData->HubFdo, !UsbhEventSignaled(&portData->ResetTimeoutEvent));

    // we consider the reset no longer in progress when it times out
    portData->PortHwPendingState = PortHw_TimeoutPending;
    KeReleaseSpinLock(&hubExt->PortResetSpin, irql);

    // queue the timeout change
    pc.ul = 0;
    pc.TimeoutChange = 1;
    tmoObj->TmoFrame = Usbh_UsbdReadFrameCounter(portData->HubFdo);

    // NOTE: REF_HUB call must occur before the following call to
    // UsbhReferenceListRemove to insure hub remains loaded across
    // call to queue reset timeout to PCQ
    REF_HUB(portData->HubFdo, &pc, Qtmo);
    UsbhReferenceListRemove(portData->HubFdo, &portData->ResetTimer);

    // release any thread attempting to cancel this timeout
    SET_EVENT(&portData->ResetTimeoutEvent);

    UsbhPCE_QueueChangeObject(portData->HubFdo,
                    portData->PortNumber,
                    pc,
                    PDC(portData),
                    PcqObj_Timeout,
                    tmoObj);


    LOG(portData->HubFdo, LOG_BUS, 'Rset', portData->PortNumber,
           tmoObj);

    DEREF_HUB(portData->HubFdo, &pc);
}


VOID
UsbhWaitForResetTimeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    )
/*++

Routine Description:

    Waits on a pending reset timeout in the event that we cannot cancel it

    Irql: PASSIVE

Arguments:

Return Value:

    None.

--*/
{
    NTSTATUS nts;

    LOG(PortData->HubFdo, LOG_BUS, 'cWT1', 0,  PortData->PortNumber);

    // the event should be signaled if it has run  if it running we should
    // be able to wait on the event

    //WAIT_EVENT_TIMEOUT(&PortData->ResetTimeoutEvent, MILLISECONDS_TO_100_NS_UNITS(5000), nts);
    nts = UsbhWaitEventWithTimeout(HubFdo, &PortData->ResetTimeoutEvent, 5000, 'caW1');

    LOG(PortData->HubFdo, LOG_BUS, 'cWT2', nts,  PortData->PortNumber);

    if (nts == STATUS_TIMEOUT) {

        // tmoObj should have been freed

        // timeout has finished, reset the PortData ptr so we know its not
        // valid anymore. It is still in the queue though.
        // tmoObj = InterlockedExchangePointer(&PortData->ResetTimerPending, NULL);
        // LOG(PortData->HubFdo, LOG_BUS, 'cWT3', tmoObj,  PortData->PortNumber);

        //UsbhAssert(HubFdo, tmoObj->Sig == SIG_TMOOBJ);
        //tmoObj->CancelFlag = 1;

        LOG(HubFdo, LOG_BUS, 'toCA', PortData->PortNumber, PortData);
#if DBG
        // after 5 sec break in...
        // if we hit this breakpoint some kind of deadlock condition
        // may have occurred -- this would be a bug in the hub driver.
        // if we didn't deadlock then we still need to figure out why
        // it took so long
        UsbhKdPrint((0,"failed to cancel reset timer (5sec) pd= %p\n",PortData));
        NT_ASSERTMSG("Failed to cancel reset timer (5sec)", FALSE);
#endif
    }


}


VOID
UsbhCancelResetTimeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    ULONG Control
    )
/*++

Routine Description:

    Synchronously cancels a pending reset timeout.

    If the reset is in progress a reasonable amount of time is allowed for the reset to finish.
    Although we can cancel the timeout we cannot abort the reset if it is in progress on the hub.
    In most cases we are cancelling due to some error but we want to avoid the race between reset
    completing and our canceling.  The major danger here is that the device could come up enabled
    at address 0 when we are enumerating another device.

I have seen the condition below using the babble boy, its probably not as common in real life.  It
is detected when the reset completes while we are in WAIT_CONNECT (11000).

                                           timeout_cancelled
                     reset_driven          (reset canceled)
                          |                         |
 ------+---------------+--+-------------------------+------------+---------
       ^               ^                                         ^
       |               |                                         |
   connect(1)   connect(1-0-1)                            reset_completes
       |               |                                         |

Irql: PASSIVE

Arguments:

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    KIRQL irql;
    PPCQ_TIMEOUT_OBJECT tmoObj;
    BOOLEAN DereferenceTimer = TRUE;

    LOG(PortData->HubFdo, LOG_BUS, 'caRT', PortData, PortData->PortNumber);

    hubExt = FdoExt(PortData->HubFdo);

    if (KeCancelTimer(&PortData->ResetTimer)) {

        // We pulled it from the queue before it fired.
        // this spinlock is used to synchronize with the timeout dpc, since we canceled,
        // we don't really need it here but we use it to be consistent.
        KeAcquireSpinLock(&hubExt->PortResetSpin, &irql);

        // if we cancel the dpc we should alwyas have a tmObj
        UsbhAssert(HubFdo, PortData->ResetTimerPending != NULL);
        tmoObj = InterlockedExchangePointer(&PortData->ResetTimerPending, NULL);
        LOG(PortData->HubFdo, LOG_BUS, 'caR1', PortData, tmoObj);
        UsbhAssert(HubFdo, tmoObj != NULL);
        UsbhAssert(HubFdo, tmoObj->Sig == SIG_TMOOBJ);
        // assert Dpc has not run
        UsbhAssert(HubFdo, PortData->PortHwPendingState != PortHw_TimeoutPending);
        PortData->PortHwPendingState = PortHw_Waiting;
        UsbhFreePool(tmoObj);

        // if the reset or the reset timeout fired this flag should be cleared
        if (Control == USB_RESET_COMPLETED) {
           PortData->PortHwPendingState = PortHw_Waiting;
        }
        LOG(PortData->HubFdo, LOG_BUS, 'caXX', PortData,
                PortData->PortNumber);
        SET_EVENT(&PortData->ResetTimeoutEvent);

        if (PortData->PortHwPendingState == PortHw_ResetPending) {
            // allow some time for the hardware to finish
            PortData->PortHwPendingState = PortHw_Waiting;
            KeReleaseSpinLock(&hubExt->PortResetSpin, irql);

            //Test #100

            // reset is spec'ed to complete within 10ms -- we allow 50 just to be safe.
            // if the reset does complete after this period for some reason we detect and
            // handle the case as a hub hardware error and hard reset the hub.
            UsbhWait(HubFdo, 50);

        } else {

            KeReleaseSpinLock(&hubExt->PortResetSpin, irql);
        }

        // clear port enable
        if (Control == CANCEL_RESET_IN_PROGRESS) {
            // if a reset was in progress the port may have been enabled if the reset
            // managed to start just after the connect change -- return the port to the expected
            // state.
            UsbhDisablePort(HubFdo, PortData);
        }

    } else {
        KIRQL rs_irql;
        // timeout is:
        // 1. running
        // 2. has run
        // 3. has run and queued a timeout

        DereferenceTimer = FALSE;
        KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);

        if (PortData->PortHwPendingState == PortHw_TimeoutPending) {

            KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);

            // if we did not remove the timeout then we must wait for it to run
            if (!UsbhCancelQueuedTimeout(HubFdo, PortData)) {
                // timeout was not in the queue
                UsbhWaitForResetTimeout(HubFdo, PortData);
            }

        } else {

            // has run but has not queued the timeout change, we have to wait in this case

            LOG(PortData->HubFdo, LOG_BUS, 'cwa4', 0,  PortData->PortNumber);

            KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);

            UsbhWaitForResetTimeout(HubFdo, PortData);

            // Remove the timeout event from the queue, which may
            // have been added during UsbhWaitForResetTimeout()
            UsbhCancelQueuedTimeout(HubFdo, PortData);
        }

    }

    KeAcquireSpinLock(&hubExt->PortResetSpin, &irql);
    UsbhAssert(HubFdo, PortData->PortHwPendingState == PortHw_Waiting);
    KeReleaseSpinLock(&hubExt->PortResetSpin, irql);

    if (DereferenceTimer) {
        //rTMR
        UsbhReferenceListRemove(PortData->HubFdo, &PortData->ResetTimer);
    }
}

VOID
UsbhFlushQueuedDriverReset(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    )
/*++

Routine Description:

    Scans the PCQ and removes a pending driver reset event if one
    is queued.

    It is assumed that the driver reset event has already been
    signaled when this function is called

Irql: DPC

Arguments:

Return Value:

    none

--*/
{
    PLIST_ENTRY le;
    PPORT_CHANGE_CONTEXT pcData;
    KIRQL pc_irql;

    KeAcquireSpinLock(&PortData->PortChangeListSpin, &pc_irql);
    PortData->PortChangeListLock = 1;

    le = PortData->PortChangeList.Blink;

    while (le != &PortData->PortChangeList) {

        pcData = CONTAINING_RECORD(le,
                                   PORT_CHANGE_CONTEXT,
                                   ChangeLink);

        LOG(PortData->HubFdo, LOG_BUS, 'fdr1', pcData,  PortData->PortNumber);

        UsbhAssert(HubFdo, pcData->Sig == SIG_PCDATA);

        if (pcData->PortEvent.ul == DRIVER_RESET_CHANGE_BIT) {

            LOG(PortData->HubFdo, LOG_BUS, 'fdr2', pcData,  PortData->PortNumber);

            RemoveEntryList(&pcData->ChangeLink);
            UsbhFreePortChangeQueueObject(HubFdo, PortData, pcData, TRUE);
            break;
        }

        le = pcData->ChangeLink.Blink;

        LOG(PortData->HubFdo, LOG_BUS, 'fdr3', pcData,  PortData->PortNumber);
    }

    PortData->PortChangeListLock = 0;
    KeReleaseSpinLock(&PortData->PortChangeListSpin, pc_irql);

}


BOOLEAN
UsbhCancelQueuedTimeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    )
{
    PLIST_ENTRY le;
    PPORT_CHANGE_CONTEXT pcData;
    KIRQL pc_irql;
    BOOLEAN timeout_cancelled = FALSE;

    KeAcquireSpinLock(&PortData->PortChangeListSpin, &pc_irql);
    PortData->PortChangeListLock = 1;

    le = PortData->PortChangeList.Blink;

    while (le != &PortData->PortChangeList) {

        pcData = CONTAINING_RECORD(le,
                                   PORT_CHANGE_CONTEXT,
                                   ChangeLink);

        LOG(PortData->HubFdo, LOG_BUS, 'cqt1', pcData,  PortData->PortNumber);

        UsbhAssert(HubFdo, pcData->Sig == SIG_PCDATA);

        if (pcData->PortEvent.ul == TIMEOUT_CHANGE_BIT) {

            LOG(PortData->HubFdo, LOG_BUS, 'cwa2', pcData,  PortData->PortNumber);

            RemoveEntryList(&pcData->ChangeLink);
            UsbhFreePortChangeQueueObject(HubFdo, PortData, pcData, FALSE);
            PortData->PortHwPendingState = PortHw_Waiting;
            timeout_cancelled = TRUE;
            break;
        }

        le = pcData->ChangeLink.Blink;

        LOG(PortData->HubFdo, LOG_BUS, 'cwa3', pcData,  PortData->PortNumber);
    }

    PortData->PortChangeListLock = 0;
    KeReleaseSpinLock(&PortData->PortChangeListSpin, pc_irql);

    return timeout_cancelled;
}





VOID
UsbhDisablePort(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    )
/*++

Routine Description:

    Disables a USB port. Failure to disable a port is severe, it can leave a
    device in an unaddressed state on the bus.
    CLEAR_FEATURE, PORT_ENABLE

    If the API fails an exception is generated a full reset is invoked

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    USHORT nBytes;
    NTSTATUS nts;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;

    ASSERT_PORT_DATA(HubFdo, PortData);

    nBytes = 0;

    // setup packet for Port Disable
    // 11.24.2.2
    // 11.24.2.7.1.2
    setupPacket.bmRequestType.B = 0x23; //00100011B
    setupPacket.bRequest = USB_REQUEST_CLEAR_FEATURE;
    setupPacket.wValue.W = PORT_ENABLE;
    setupPacket.wIndex.W = PortData->PortNumber;
    setupPacket.wLength = 0;

    nts = UsbhSyncSendCommand(HubFdo,
                              &setupPacket,
                              NULL,
                              &nBytes,
                              USBH_CONTROL_PIPE_TIMEOUT,
                              &usbdStatus);

    if (NT_ERROR(nts) &&
        !Usb_Disconnected(nts)) {
        // throw an exception so we know why it failed
        // reset!
        HUB_EXCEPTION(USBFILE, HubFdo, PortData->PortNumber,
                          HubErr_PortDisableFailed_Reset,
                          nts, usbdStatus, NULL, 0);
    }

    return;

}


NTSTATUS
UsbhResumePort(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    USBD_STATUS *UsbdStatus
    )
/*++

Routine Description:

    Resumes a USB port currently suspended.

    CLEAR_FEATURE, PORT_SUSPEND

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    USHORT nBytes;
    NTSTATUS nts;

    ASSERT_PORT_DATA(HubFdo, PortData);
    LOG(HubFdo, LOG_BUS, 'RSMp', PortData->PortNumber, 0);

    nBytes = 0;

    // setup packet for Port Resume
    // 11.24.2.2
    setupPacket.bmRequestType.B = 0x23; //00100011B
    setupPacket.bRequest = USB_REQUEST_CLEAR_FEATURE;
    setupPacket.wValue.W = PORT_SUSPEND;
    setupPacket.wIndex.W = PortData->PortNumber;
    setupPacket.wLength = 0;

    nts = UsbhSyncSendCommand(HubFdo,
                              &setupPacket,
                              NULL,
                              &nBytes,
                              USBH_CONTROL_PIPE_TIMEOUT,
                              UsbdStatus);


    LOG(HubFdo, LOG_BUS, 'RSMx', PortData->PortNumber, nts);

    return nts;

}


VOID
UsbhClearTt(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    USHORT DeviceAddress
    )
/*++

Routine Description:

    Attempts to reset the TT for a multi TT usb hub.

    If the hub is single TT or usb 1.0 this function is a nop.
    If the request fails then we just throw an exception to track the error.

    This function is useful if the TT encounters errors for split transactions. It must be
    called specifying the device address and endpoint that needs to be reset so it can
    target a particular device downstream of a TT.

Irql: PASSIVE

Arguments:

    PortData - indicates port (and TT) we are trying to reset.

    DeviceHandle - device handle for the device or NULL, NULL indicates no address
            has been assigned

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    USHORT nBytes;
    USBD_STATUS usbdStatus;
    NTSTATUS nts;
    USB_HUB_CLEAR_TT_BUFFER_VALUE v;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_BUS, 'reTT', PortData, 0);
    DbgTrace((UsbhDebugTrace,"#%d reset TT\n", PN(PortData)));

    ASSERT_PORT_DATA(HubFdo, PortData);

    if (!hubExt->FdoFlags.MultiTTmode) {
        LOG(HubFdo, LOG_BUS, 'naTT', PortData, 0);
        DbgTrace((UsbhDebugTrace,"#%d SKIP reset TT (single TT)\n", PN(PortData)));
        return;
    }

    nBytes = 0;

    // setup packet for clear tt buffer
    // 11.24.2.3
    setupPacket.bmRequestType.B = 0x23; //00100011B
    setupPacket.bRequest = USB_REQUEST_CLEAR_TT_BUFFER;

    // setup packet for reset tt buffer
    // 11.24.2.9
    //setupPacket.bmRequestType.B = 0x23; //00100011B
    //setupPacket.bRequest = USB_REQUEST_RESET_TT;

    v.us = 0;
    // in this case we are always dealing with control
    v.EndpointType = USB_ENDPOINT_TYPE_CONTROL;

    do {

        LOG(HubFdo, LOG_BUS, 'rsDA', DeviceAddress, 0);
        v.DeviceAddress = DeviceAddress;
        UsbhKdPrint((1, ">RESET Tt Address %d\n", DeviceAddress));

        setupPacket.wValue.W = v.us;
        setupPacket.wIndex.W = PortData->PortNumber;
        setupPacket.wLength = 0;

        nts = UsbhSyncSendCommand(HubFdo,
                                  &setupPacket,
                                  NULL,
                                  &nBytes,
                                  USBH_CONTROL_PIPE_TIMEOUT,
                                  &usbdStatus);

        if (NT_ERROR(nts) &&
            !Usb_Disconnected(nts)) {
            // throw an exception so we know why it failed
            HUB_EXCEPTION(USBFILE, HubFdo, PortData->PortNumber, HubErr_PortResetTtFailed,
                              nts, usbdStatus, &setupPacket, sizeof(struct _USB_DEFAULT_PIPE_SETUP_PACKET));
            break;
        }
    } WHILE (0);

    return;

}


VOID
UsbhResetPort(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PPCQ_TIMEOUT_OBJECT TimeoutObject
    )
/*++

Routine Description:

    Initiates a port reset and sets the timeout timer for the port.  If an
    error occurs requesting the reset an exception is generated but the timeout
    timer is still set so the state machine can handle the failure.

Irql: PASSIVE

Arguments:

    ResetPhase -

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    USHORT nBytes;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    NTSTATUS nts, tmr_nts;
    KIRQL irql;
    PPCQ_TIMEOUT_OBJECT tmoObj;

    LOG(HubFdo, LOG_BUS, 'rePT', PortData, 0);
    DbgTrace((UsbhDebugTrace,"#%d UsbhResetPort\n", PN(PortData)));
    hubExt = FdoExt(HubFdo);

    ASSERT_PORT_DATA(HubFdo, PortData);

    tmoObj = TimeoutObject;

    UsbhAssert(HubFdo, tmoObj->Sig == SIG_TMOOBJ);
    nBytes = 0;

    // setup packet for Port Reset
    // 11.24.2.7.1.5
    setupPacket.bmRequestType.B = 0x23; //00100011B
    setupPacket.bRequest = USB_REQUEST_SET_FEATURE;
    setupPacket.wValue.W = PORT_RESET;
    setupPacket.wIndex.W = PortData->PortNumber;
    setupPacket.wLength = 0;

    nts = UsbhSyncSendCommand(HubFdo,
                              &setupPacket,
                              NULL,
                              &nBytes,
                              USBH_CONTROL_PIPE_TIMEOUT,
                              &usbdStatus);

    if (NT_ERROR(nts) &&
        !Usb_Disconnected(nts)) {
        // throw an exception so we know why it failed
        HUB_EXCEPTION(USBFILE, HubFdo, PortData->PortNumber, HubErr_ResetPortFailed,
                          nts, usbdStatus, NULL, 0);
    }



    // Do this before we acquire PortResetSpin
    LOG(HubFdo, LOG_BUS, 'rePx', PN(PortData), PortData->PortHwPendingState);

    Usbh_CheckPortHwPendingStatus(HubFdo, PortData, FALSE);

    KeAcquireSpinLock(&hubExt->PortResetSpin, &irql);
    // regardless of status set the timeout timer

    UsbhAssert(HubFdo, PortData->ResetTimerPending == NULL);
    //UsbhAssert(HubFdo, PortData->PortHwPendingState == PortHw_Waiting);

    PortData->PortHwPendingState = PortHw_ResetPending;

    UsbhAssert(HubFdo, hubExt->CurrentBusState == BS_BusRun);
    //if(!hubExt->FdoFlags.BusPaused) {

    tmoObj->StartFrame = PortData->TmResetStart = PortData->TmResetEnd =
        Usbh_UsbdReadFrameCounter(HubFdo);

    LOG(HubFdo, LOG_BUS, 'reQT', PortData, tmoObj);
    PortData->ResetTimerPending = tmoObj;

    //this event is set when the timeout Dpc exits or if we manage to
    // cancel the DPC.
    RESET_EVENT(&PortData->ResetTimeoutEvent);

    KeReleaseSpinLock(&hubExt->PortResetSpin, irql);

    // The original deadman timeout in the hub driver was 5 seconds
    // I have made this settable for troubleshooting purposes.  Debugger output can
    // cause this timeout to fire.
    //
    // callback is UsbhResetPortTimerDpc
    UsbhAssert(HubFdo, HubG.ResetPortTimeoutInMilliseconds != 0);
    tmr_nts = UsbhSetTimer(HubFdo, HubG.ResetPortTimeoutInMilliseconds, &PortData->ResetTimer,
                       &PortData->ResetTimerDpc, rTMR, TRUE);

    // reset timer already queued?
    UsbhAssert(HubFdo, tmr_nts != STATUS_DEVICE_BUSY);
    if (NT_ERROR(tmr_nts)) {
        // low resource sim will hit this path
        TEST_TRAP();
    }
    //} else {
    //    KeReleaseSpinLock(&hubExt->PortResetSpin, irql);
    //}

    return;

}


NTSTATUS
UsbhHubAckPortChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PUSB_HUB_HW_PORT_CHANGE PortChange,
    USBD_STATUS *UsbdStatus
    )
/*++

Routine Description:

    clear the change indication from the hub

Irql: PASSIVE

Arguments:

Return Value:

    nt status

--*/
{
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    USHORT fs;
    USHORT nBytes = 0;
    NTSTATUS nts;

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!>\n", PortNumber));

    // the feature selector based on change bit
    if (PortChange->ConnectChange) {
        fs = C_PORT_CONNECT;
    } else if (PortChange->EnableChange) {
        fs = C_PORT_ENABLE;
    } else if (PortChange->SuspendChange) {
        fs = C_PORT_SUSPEND;
    } else if (PortChange->OvercurrentChange) {
        fs = C_PORT_OVER_CURRENT;
    } else if (PortChange->ResetChange) {
        fs = C_PORT_RESET;
    } else {
        // not a change bit the hub recognizes, this is a nop

        return STATUS_SUCCESS;
    }

    // setup packet for Clear Port Feature
    // 11.24.2.2
    setupPacket.bmRequestType.B = 0x23; //00100011B
    setupPacket.bRequest = USB_REQUEST_CLEAR_FEATURE;
    setupPacket.wValue.W = fs;
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


    LOG(HubFdo, LOG_BUS, 'AKcg', fs, nts);
    DbgTrace((UsbhDebugTrace,"#%d %!FUNC! %!STATUS!<\n", PortNumber, nts));

    return nts;

}


NTSTATUS
UsbhHubAckHubChange(
    PDEVICE_OBJECT HubFdo,
    PUSB_HUB_STATE HubChange,
    USBD_STATUS *UsbdStatus
    )
/*++

Routine Description:

    clear the change indication from the hub

Irql: PASSIVE

Arguments:

Return Value:

    nt status

--*/
{
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    USHORT fs;
    USHORT nBytes = 0;
    NTSTATUS nts;

    DbgTrace((UsbhDebugTrace," %!FUNC!>\n"));

    // the feature selector based on change bit
    if (HubChange->Change.OverCurrentChange) {
        fs = C_HUB_OVER_CURRENT;
    } else if (HubChange->Change.LocalPowerChange) {
        fs = C_HUB_LOCAL_POWER;
    } else {
        // not a change bit the hub recognizes, this is a nop

        return STATUS_SUCCESS;
    }

    // setup packet for Clear Hub Feature
    // 11.24.2.1
    setupPacket.bmRequestType.B = 0x20; //00100000B
    setupPacket.bRequest = USB_REQUEST_CLEAR_FEATURE;
    setupPacket.wValue.W = fs;

    setupPacket.wIndex.W = 0;
    setupPacket.wLength = 0;

    nts = UsbhSyncSendCommand(HubFdo,
                              &setupPacket,
                              NULL,
                              &nBytes,
                              USBH_CONTROL_PIPE_TIMEOUT,
                              UsbdStatus);


    LOG(HubFdo, LOG_BUS, 'AKhg', fs, nts);
    DbgTrace((UsbhDebugTrace,"%!FUNC! %!STATUS!<\n", nts));

    return nts;

}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhHubResetIrqPipeWorker(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG PortNumber,
    __in PVOID Context
    )
/*++

Routine Description:

    Resets the status change notification interrupt pipe.

    This routine is NOT reentrant

Irql: PASSIVE

Arguments:

    Context is global hub state context

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USBD_STATUS usbStatus = 0;
    NTSTATUS nts;
    ULONG inISR;
    PSSH_BUSY_HANDLE bh;
    PSTATE_CONTEXT sc = Context;

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!>\n", PortNumber));
    LOG(HubFdo, LOG_BUS, 'irqR', Context, PortNumber);

    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, hubExt->CurrentBusState == BS_BusRun);
    // The hubs interrupt is considered in the global context even though we have
    // a workitem dispatched for it.
    // Currently there is no reason to keep a separate context for this
    ASSERT_HUB_CONTEXT(sc);
    UsbhAssert(HubFdo, sc == PNP_CONTEXT(HubFdo));

    UsbhAssertPassive();

    bh = UsbhIncHubBusy(HubFdo, sc, HubFdo, UsbhBusyResetIrqPipe_Tag, NO_RESUME_HUB);

    //reentrance check
    inISR = InterlockedIncrement(&hubExt->InISR);
    UsbhAssert(HubFdo, inISR == 1);

    if (hubExt->FdoFlags.ResetIrqPipe) {
        // reset pipe is requested by ISR
        hubExt->FdoFlags.ResetIrqPipe = 0;

        LOG(HubFdo, LOG_BUS, 'irqR', 0, 0);
        DbgTrace((UsbhDebugTrace,"#%d %!FUNC! ResetIrqPipe\n", PortNumber));

        // perform synchronous reset and clear stall on the IRQ pipe
        nts = UsbhResetIrqPipe(HubFdo);
        LOG(HubFdo, LOG_BUS, 'iRES', 0, nts);

        if (NT_ERROR(nts)) {
            if (Usb_Disconnected(nts)) {
                // device gone, take no further action
                SET_EVENT(&hubExt->IrqDoneEvent);
                inISR = InterlockedDecrement(&hubExt->InISR);
                UsbhAssert(HubFdo, inISR == 0);
                DbgTrace((UsbhDebugTrace,"#%d %!FUNC!<<\n", PortNumber));

                UsbhDecHubBusy(HubFdo, sc, bh);
                return;

            } else {
                inISR = InterlockedDecrement(&hubExt->InISR);
                UsbhAssert(HubFdo, inISR == 0);
                // can't reset the pipe, throw an exception and reset the hub
                SET_EVENT(&hubExt->IrqDoneEvent);
                // log a hub hardware error and reset
                // reset!
                // The following reset causes a downstream hub reset while an
                // upstream hub has not finishes reset. Resulting the hub to
                // enter the HRS_ResetStop function
                if (!hubExt->IsBootDevice) {
                    HUB_RESET_EXCEPTION(USBFILE, HubFdo, 0, InterruptPipeFailure_Reset,
                              nts, usbStatus, NULL, 0);
                }
                // reset
                DbgTrace((UsbhDebugTrace,"#%d %!FUNC!<<<\n", PortNumber));

                UsbhDecHubBusy(HubFdo, sc, bh);
                return;
            }
        }
        // reset successful

        // repost the interrupt transfer in case the problem was temporary.
        // *an error here will re-enter this function so we don't check status
        inISR = InterlockedDecrement(&hubExt->InISR);
        UsbhAssert(HubFdo, inISR == 0);
        UsbhDecHubBusy(HubFdo, sc, bh);

        UsbhPostInterrupt(HubFdo);
        DbgTrace((UsbhDebugTrace,"#%d %!FUNC!<<<<\n", PortNumber));
    }
}

VOID
UsbhHubIsrWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context
    )
/*++

Routine Description:

    Dispatches an interrupt transfer from the hub at passive level,
    if it has not already been processed by another thread.

Irql: PASSIVE

Arguments:

    Context is global hub state context

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PSTATE_CONTEXT sc = Context;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);

    KeAcquireSpinLock(&HubG.HubIsrListSpin, &irql);

    LOG(HubFdo, LOG_BUS, 'ISRw', HubFdo, sc);

    // Check if the notification has already been processed by a waiting thread
    if (sc->HubIsrWorkerState == HubIsrWorker_Queued) {
        LOG(HubFdo, LOG_BUS, 'ISdq', HubFdo, sc);

        // Remove the change notification from the global list
        RemoveEntryList(&sc->HubIsrListLink);
        sc->HubIsrWorkerState = HubIsrWorker_NotQueued;
        KeReleaseSpinLock(&HubG.HubIsrListSpin, irql);

        // Process the change notification
        UsbhHubProcessIsr(HubFdo, PortNumber, sc);
    } else {
        // Notification was already processed
        KeReleaseSpinLock(&HubG.HubIsrListSpin, irql);
    }

    // Dereference the ISR work-item for this hub
    UsbhDecrementHubIsrWorkerReference(HubFdo);
}

VOID
UsbhIncrementHubIsrWorkerReference(
    __in PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Increments the reference count for the hub's ISR work-items

Irql: Any

Arguments:

    HubFdo - Device Object for the hub

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);
    UsbhAssert(HubFdo, hubExt->IsrWorkerReference >= 1);
    InterlockedIncrement(&hubExt->IsrWorkerReference);
    LOG(HubFdo, LOG_BUS, 'HIR+', HubFdo, hubExt->IsrWorkerReference);
}

VOID
UsbhDecrementHubIsrWorkerReference(
    __in PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Decrements the reference count for the hub's ISR work-items.
    Signals completion of all worker threads if the reference count
    goes to zero.

Irql: Any

Arguments:

    HubFdo - Device Object for the hub

Return Value:

    none

--*/
{
    LONG ReferenceCount;
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);
    UsbhAssert(HubFdo, hubExt->IsrWorkerReference >= 1);
    ReferenceCount = InterlockedDecrement(&hubExt->IsrWorkerReference);

    LOG(HubFdo, LOG_BUS, 'HIR-', HubFdo, hubExt->IsrWorkerReference);

    if (ReferenceCount == 0) {
        SET_EVENT(&hubExt->HubIsrWorkerDone);
    }
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhDecrementHubIsrWorkerReferenceAndWait(
    __in PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Decrements the reference count for the hub's ISR work-items,
    and then waits for the reference count to go to zero.

    Used to insure that all the hub's ISR work-items have completed
    before the FDO is removed.

Irql: PASSIVE

Arguments:

    HubFdo - Device Object for the hub

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_BUS, 'HIRw', HubFdo, hubExt->IsrWorkerReference);

    UsbhDecrementHubIsrWorkerReference(HubFdo);
    WAIT_EVENT(&hubExt->HubIsrWorkerDone);
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
UsbhWaitWhileProcessingGlobalHubIsrList(
    __in PDEVICE_OBJECT HubFdo,
    __in PKEVENT WaitEvent
    )
/*++

Routine Description:

    Processes pending port change notifications until the specified wait event
    is signaled.  Used to avoid work-item starvation of the critical work queue
    during system resume.

Irql: PASSIVE

Arguments:

    WaitEvent -

Return Value:

    none

--*/
{
    PVOID WaitObjects[2];
    PSTATE_CONTEXT sc;
    NTSTATUS nts;
    KIRQL irql;
    PLIST_ENTRY le;

    UsbhAssertPassive();

    WaitObjects[0]=WaitEvent;
    WaitObjects[1]=&HubG.HubIsrWorkerQueued;

    LOG(HubFdo, LOG_BUS, 'WWPG', HubFdo, WaitEvent);

    do {
        nts = KeWaitForMultipleObjects(sizeof(WaitObjects)/sizeof(WaitObjects[0]),
                                       WaitObjects,
                                       WaitAny,
                                       Executive,
                                       KernelMode,
                                       FALSE,
                                       NULL,
                                       NULL);

        sc = NULL;

        if (nts == STATUS_WAIT_0) {
            // Event has completed; we can exit
            break;
        } else if (nts == STATUS_WAIT_1) {
            // Change notification has been added to the global notification list
            // Try to process it in case the critical work queue is blocked

            KeAcquireSpinLock(&HubG.HubIsrListSpin, &irql);

            LOG(HubFdo, LOG_BUS, 'WWP1', HubFdo, NULL);

            // See if there is anything on the list to process
            if (!IsListEmpty(&HubG.HubIsrListHead)) {
                le = RemoveHeadList(&HubG.HubIsrListHead);
                sc = CONTAINING_RECORD(le,
                                       STATE_CONTEXT,
                                       HubIsrListLink);

                // Mark the change notification as no longer in the global list
                sc->HubIsrWorkerState = HubIsrWorker_NotQueued;
            }

            KeReleaseSpinLock(&HubG.HubIsrListSpin, irql);

            if (sc) {
                LOG(HubFdo, LOG_BUS, 'WWP2', sc->HubFdo, sc->PortNumber);
                // Process the change notification
                UsbhHubProcessIsr(sc->HubFdo, sc->PortNumber, sc);
            }
        }
    } WHILE (1);
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhHubProcessIsr(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG PortNumber,
    __in PSTATE_CONTEXT sc
    )
/*++

Routine Description:

    Processes an interrupt transfer from the hub at passive level.
    This function Queries and acks the change then queues it for
    processing by a worker routine

    This routine is NOT reentrant

Irql: PASSIVE

Arguments:

    Context is global hub state context

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USHORT pn = 0;
    USBD_STATUS usbStatus = 0;
    NTSTATUS nts;
    ULONG inISR;
    PSSH_BUSY_HANDLE bh;

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!>\n", PortNumber));
    LOG(HubFdo, LOG_BUS, 'irqW', sc, PortNumber);

    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, hubExt->CurrentBusState == BS_BusRun);
    // The hubs interrupt is considered in the global context even though we have
    // a workitem dispatched for it.
    // Currently there is no reason to keep a separate context for this
    ASSERT_HUB_CONTEXT(sc);
    UsbhAssert(HubFdo, sc == PNP_CONTEXT(HubFdo));

    UsbhAssertPassive();

    //UsbhAssert(HubFdo, hubExt->IsrRetryTimer->State == TimerObj_Disabled);

    bh = UsbhIncHubBusy(HubFdo, sc, HubFdo, UsbhBusyIsr_Tag, NO_RESUME_HUB);

    //reentrance check
    inISR = InterlockedIncrement(&hubExt->InISR);
    UsbhAssert(HubFdo, inISR == 1);

    // 11.23.2.1 Hub Descriptor
    // The Port Number field in the descriptor is a byte limiting us to 255 ports.
    // a value larger that this is invalid, we treat it as a hub status change instead.

    // it is difficult to convince the compiler and prefast we know what we are doing,
    // this seems to work
    pn = (USHORT) (PortNumber & 0x000000ff);

    LOG(HubFdo, LOG_BUS, 'irqw', pn, 0);

    // what port? pn of 0 means the hub itself
    if (pn) {
         // 1..255 port is interrupt

        PHUB_PORT_DATA pd;

        pd = UsbhGetPortData(HubFdo, pn);
        if (pd == NULL) {

            nts = STATUS_INVALID_PARAMETER;
            LOG(HubFdo, LOG_BUS, 'iwwX', 0, nts);
            // we are stuck until we can successfully re-post the interrupt,
            // generate a harderr. The harderr routine will cause us to reset
            HUB_EXCEPTION(USBFILE, HubFdo, pn, HubErr_TransactionFailure_Reset,
                nts, 0, NULL, 0);
            // reset!

            SET_EVENT(&hubExt->IrqDoneEvent);
            goto hubIsr_exit;
        }

        do {

            USB_HUB_HW_PORT_CHANGE pc;
            USB_HUB_PORT_EVENT pev;

            // what changed?
            nts = UsbhQueryChange(HubFdo, pn, &pc, &usbStatus);
            LOG(HubFdo, LOG_BUS, 'QUch', pn, nts);
            if (NT_ERROR(nts)) {
                break;
            }

            UsbhEtwLogHubEventWithExtraData(HubFdo, &USBHUB_ETW_EVENT_HUB_PORT_STATUS_CHANGE, PortNumber, &pc, sizeof(pc));

            if (pc.us) {
                // ack the change (if there was one) so it does not get reported again
                nts = UsbhHubAckPortChange(HubFdo, pn, &pc, &usbStatus);
                LOG(HubFdo, LOG_BUS, 'Achg', pn, nts);
                if (NT_ERROR(nts)) {
                    break;
                }

                // increment counter of connect changes for debounce handling
                if (pc.ConnectChange) {
                    LOG(HubFdo, LOG_BUS, 'BNC+', pn, 0);
                    InterlockedIncrement(&pd->ConnectCount);
                }

                pev.ul = 0;
                pev.HwChange = pc.us;

                // wake up the port if necessary
                UsbhPCE_HardwareWake(HubFdo, sc, pn);
                // queue the change for processing
                UsbhPCE_QueueChange(HubFdo, pn, pev, sc);
                LOG(HubFdo, LOG_BUS, 'Qchg', pn, &pev);
            }

            UsbhDecHubBusy(HubFdo, sc, bh);

            // enable another change indication from the hub
            // we don't check for an error here, instead the
            // error case is handled in the irp completion.
            inISR = InterlockedDecrement(&hubExt->InISR);
            UsbhAssert(HubFdo, inISR == 0);
            UsbhPostInterrupt(HubFdo);
            LOG(HubFdo, LOG_BUS, 'iw>1', 0, hubExt->InISR);

            DbgTrace((UsbhDebugTrace,"#%d %!FUNC!<*\n", PortNumber));

            return;

        } WHILE (0);

        // Note we are depending on getting a quick and accurate indication when
        // our own device is disconnected otherwise we will generate
        // exceptions when the hub is unplugged.

// enumtst 27
        UsbhAssert(HubFdo, NT_ERROR(nts));
        UsbhAssert(HubFdo, sc);
        UsbhAssert(HubFdo, pd);
        // since we don't re-post in the error case we need to signal the Done
        // event.
        SET_EVENT(&hubExt->IrqDoneEvent);

        if (Usb_Disconnected(nts)) {

            LOG(HubFdo, LOG_BUS, 'iww1', 0, nts);
            // we have been unplugged, take us back to PS_WAIT_CONNECT but
            // don't bother trying to talk to the hub anymore.

            // pnpdtest will hit this path
            // but we should add a test case to the BVT.

            // queue the a pause change for processing -- this will cause the
            // hub to abort any transactions in progress and ignore any software changes
            // queued after this.

            UsbhQueuePauseChange(HubFdo, pn, sc);
            // this will queue a worker to shutdown the pcq
            UsbhPCE_HW_Stop(HubFdo, pn, sc);

        } else {
            LOG(HubFdo, LOG_BUS, 'iww2', 0, nts);
            // we are stuck until we can successfully re-post the interrupt,
            // generate a harderr. The harderr routine will cause us to reset
            HUB_RESET_EXCEPTION(USBFILE, HubFdo, pn, HubErr_TransactionFailure_Reset,
                nts, usbStatus, NULL, 0);
            // reset!
        }
    } else {
        // 0 indicates hub status change
        USB_HUB_STATE hc;

        nts = UsbhQueryHubState(HubFdo, &hc, &usbStatus);
        LOG(HubFdo, LOG_BUS, 'QUhs', 0, nts);
        LOG(HubFdo, LOG_BUS, 'QUhc', hc.Change.AsUshort16, hc.Status.AsUshort16);

        if (NT_SUCCESS(nts)) {

            // update local power status reported
            if (hc.Status.LocalPowerLost) {
                hubExt->PortPower_HubStatus = USB_100ma;
            } else {
                hubExt->PortPower_HubStatus = USB_500ma;
            }

            if (hc.Change.LocalPowerChange) {
                // Update max power allowed for each port
                hubExt->MilliampsPowerPerPort = hubExt->PortPower_HubStatus;
            }

            // ack the change so it does not get reported again
            nts = UsbhHubAckHubChange(HubFdo, &hc, &usbStatus);
            LOG(HubFdo, LOG_BUS, 'AHch', 0, nts);

            //**
            // This is the 'somebody tripped on the hubs power cord' scenario.
            //
            // Classic behavior for local power status change is to ack the change and
            // continue on normally.  There really is not much more we can do about it.
            // We throw an exception so that we know that the change occurred.
            // We do not currently notify the user of this condition.

            // Given that most hub OEMs don't like the fact that we distinguish between
            // bus vs. self power (to the point of always reporting self power)  I'm not
            // inclined to change this.

            //**
            // Classic behavior for this change is to do a hard reset (aka ESD recovery)
            // of the hub if the hub reports a status of over-current.  If the hub does
            // not report a status of over-current then the change is considered a
            // 'glitch' or transient event and is ignored.  No additional action is taken
            // to recover.

            if (NT_SUCCESS(nts) && !hc.Status.OverCurrent &&
                (hubExt->PerPortOvercurrent || !hc.Change.OverCurrentChange)) {

                // enable another change indication from the hub
                // we don't check for an error here, instead the
                // error case is handled in the irp completion.

                if (hc.Change.OverCurrentChange) {
                    HUB_EXCEPTION(USBFILE, HubFdo, pn, HubErr_HubOverCurrentChangeDetected,
                        nts, usbStatus, &hc, sizeof(struct _USB_HUB_STATE));
                } else if (hc.Change.LocalPowerChange) {
                    HUB_EXCEPTION(USBFILE, HubFdo, pn, HubErr_HubLocalPowerChangeDetected,
                        nts, usbStatus, &hc, sizeof(struct _USB_HUB_STATE));
                } else {
                    // unknown change or no change at all
                    HUB_EXCEPTION(USBFILE, HubFdo, pn, HubErr_HubChangeDetected,
                        nts, usbStatus, &hc, sizeof(struct _USB_HUB_STATE));
                }

                inISR = InterlockedDecrement(&hubExt->InISR);

                UsbhDecHubBusy(HubFdo, sc, bh);

                UsbhAssert(HubFdo, inISR == 0);
                UsbhPostInterrupt(HubFdo);
                LOG(HubFdo, LOG_BUS, 'iw>3', 0, hubExt->InISR);
                DbgTrace((UsbhDebugTrace," %!FUNC!<*\n"));

                return;

            }
        }

        // unable to ack the hub change or this is overcurrent

        // since we don't re-post in the error case we need to signal the Done
        // event.
        SET_EVENT(&hubExt->IrqDoneEvent);

        if (hc.Status.OverCurrent || (!hubExt->PerPortOvercurrent && hc.Change.OverCurrentChange)) {
            LOG(HubFdo, LOG_BUS, 'hGov', 0, nts);
            // we are stuck until we can successfully re-post the interrupt,
            // generate a harderr. The harderr routine will cause us to reset

            if (hubExt->OvercurrentDetected == FALSE) {

                hubExt->OvercurrentDetected = TRUE;

                UsbhQueueWorkItem(HubFdo,
                                  DelayedWorkQueue,
                                  UsbhSetHubOvercurrentDetectedKey,
                                  0,
                                  0,
                                  SIG_OVERC_WORKITEM);
            }

            HUB_EXCEPTION(USBFILE, HubFdo, pn, HubErr_GlobalOvercurrent_Reset,
                nts, usbStatus, &hc, sizeof(struct _USB_HUB_STATE));

            // reset!

        } else {
            UsbhAssert(HubFdo, NT_ERROR(nts));

            if (!Usb_Disconnected(nts)) {
                LOG(HubFdo, LOG_BUS, 'iwh2', 0, nts);
                // we are stuck until we can successfully re-post the interrupt,
                // generate a harderr. The harderr routine will cause us to reset

                HUB_EXCEPTION(USBFILE, HubFdo, pn, HubErr_TransactionFailure_Reset,
                    nts, usbStatus, NULL, 0);
                // reset!
            }
        }
    }

hubIsr_exit:

    LOG(HubFdo, LOG_BUS, 'iw>2', 0, hubExt->InISR);
    inISR = InterlockedDecrement(&hubExt->InISR);
    UsbhAssert(HubFdo, inISR == 0);

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!<<*\n", PortNumber));

    UsbhDecHubBusy(HubFdo, sc, bh);
}


PUSB_DEVICE_HANDLE
UsbhGetDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PVOID Object
    )
/*++

Routine Description:

    returns the device handle for the hub device, NULL if no device error

Irql: PASSIVE

Arguments:

Return Value:

    device handle (opaque) or NULL

--*/
{
    PIO_STACK_LOCATION stk;
    PIRP irp;
    NTSTATUS nts;
    KEVENT event;
    PDEVICE_EXTENSION_HUB  hubExt;
    IO_STATUS_BLOCK ioStatus;
    PUSB_DEVICE_HANDLE devHandle = NULL;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_EX,
                                        hubExt->TopOfStackDeviceObject,
                                        NULL,
                                        0,
                                        NULL,
                                        0,
                                        TRUE,  // INTERNAL
                                        &event,
                                        &ioStatus);

    if (!irp) {
        return NULL;
    }

    stk = IoGetNextIrpStackLocation(irp);
    UsbhFatalAssert(HubFdo,stk!=NULL, stk);

    stk->Parameters.Others.Argument1 = &devHandle;
    stk->Parameters.Others.Argument2 = Object;
    // arg three is the ioControlCode

    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, irp);

    if (nts == STATUS_PENDING) {

        KeWaitForSingleObject(
            &event,
            Suspended,
            KernelMode,
            FALSE,
            NULL);

    } else {
        ioStatus.Status = nts;
    }

    LOG(HubFdo, LOG_BUS, 'gdhR', ioStatus.Status, devHandle);
    if (NT_SUCCESS(ioStatus.Status)) {
        return devHandle;
    }

    return NULL;

}


NTSTATUS
UsbhGetTtDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE *DeviceHandle,
    PVOID Object,
    PUSHORT TtPort
    )
/*++

Routine Description:

    returns the device handle for the Tt device, if no TT is in the
    chain, also returns the tt port.

    The only way to know the correct TT is to call down the chain to get
    it.  This is only necessary if we are not the hub with the TT.

Irql: PASSIVE

Arguments:

    TtPort - pointer to the port number on the multi TT hub
            associated with this PDO.

Return Value:

    device handle (opaque) or NULL

--*/
{
    PIO_STACK_LOCATION stk;
    PIRP irp;
    NTSTATUS nts;
    KEVENT event;
    PDEVICE_EXTENSION_HUB  hubExt;
    IO_STATUS_BLOCK ioStatus;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    LOG(HubFdo, LOG_BUS, 'gth0', DeviceHandle, Object);

    if (hubExt->FdoFlags.HubIsRoot) {
        // root hub is not a TT
        LOG(HubFdo, LOG_BUS, 'gth1', DeviceHandle, 0);
        *DeviceHandle = NULL;
        return STATUS_SUCCESS;
    }

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_USB_GET_TT_DEVICE_HANDLE,
                                        hubExt->TopOfStackDeviceObject,
                                        NULL,
                                        0,
                                        NULL,
                                        0,
                                        TRUE,  // INTERNAL
                                        &event,
                                        &ioStatus);

    if (!irp) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    stk = IoGetNextIrpStackLocation(irp);
    UsbhFatalAssert(HubFdo,stk!=NULL, stk);

    stk->Parameters.Others.Argument1 = DeviceHandle;
    stk->Parameters.Others.Argument2 = Object;
    // arg three is the ioControlCode
    stk->Parameters.Others.Argument4 = TtPort;

    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, irp);

    if (nts == STATUS_PENDING) {

        KeWaitForSingleObject(
            &event,
            Suspended,
            KernelMode,
            FALSE,
            NULL);

    } else {
        ioStatus.Status = nts;
    }

    LOG(HubFdo, LOG_BUS, 'gthX', *DeviceHandle, ioStatus.Status);

    return ioStatus.Status;

}



UCHAR
UsbhGetPortStatusBits(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PUSB_HUB_PORT_STATE PortState,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Return the one byte field indicated current state of the port

    state mask is
         ceosp
      xxx11001

    bits 5,6 and 7 are reserved MBZ
    bit 0 - p PDO present               0x01
    bit 1 - s port in suspend           0x02
    bit 2 - o overcurrent is present    0x04
    bit 3 - e enabled                   0x08
    bit 4 - c device connected          0x10

Irql: PASSIVE

Arguments:

    PortData - data structure in hub fdoext

    Pdo - PDO if it is latched and connected for this port. A latched
        PDO won't be deleted.

Return Value:

    port status bitmask defined above

--*/
{

//#define PORT_PDO_BIT                0x01
//#define PORT_SUSPEND_BIT            0x02
//#define PORT_OVERCURRENT_BIT        0x04
//#define PORT_ENABLE_BIT             0x08
//#define PORT_CONNECT_BIT            0x10

    UCHAR b = 0;

    if (Pdo) {
        b |= PORT_PDO_BIT;
    }

    if (PortState->Status.Connect) {
        b |= PORT_CONNECT_BIT;
    }

    if (PortState->Status.Suspend) {
        b |= PORT_SUSPEND_BIT;
    }

    if (PortState->Status.Overcurrent) {
        b |= PORT_OVERCURRENT_BIT;
    }

    if (PortState->Status.Enable) {
        b |= PORT_ENABLE_BIT;
    }

    return b;
}




NTSTATUS
UsbhGetUsbDeviceFlags(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    This routine returns any device specific 'workaround' flags we may be aware of.

    Device flags define behaviors for USB devices.  These behaviors are stored
    in a global registry key based on the device vid,pid,rev so that they may
    be applied during enumeration of the device.

    In WinNT5.1(XP) these flags are stored in the registry under usbflags.
    In Longhorn these flags may be stored elsewhere.

Arguments:

Return Value:

    none, failure to access registry falls back to default behavior

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    BOOLEAN isBillboard = FALSE;


    pdoExt = PdoExt(Pdo);

    // default value for all flags is zero
    pdoExt->DeviceFlags.ul = 0;
    pdoExt->DeviceFlags.DevFlagsInitialized = 1;
    // default to query for MSOS.
    pdoExt->DeviceFlags.QueryMsOs = 1;

    if (!UsbhValidateDeviceDescriptor(HubFdo,
                &pdoExt->DeviceDescriptor,
                sizeof(struct _USB_DEVICE_DESCRIPTOR),
                NULL,
                &isBillboard)) {
        pdoExt->EnumFailureMsgId = USBENUM_BAD_DEVICE_DESCRIPTOR;
        return STATUS_INVALID_PARAMETER;
    }

    if (isBillboard) {
        pdoExt->PdoFlags.DeviceIsBillboard = 1;
    }

    // get USB device class flags
    UsbhGetRegUsbClassFlags(HubFdo, Pdo);

    // get the flags from the registry, keep a pointer to the
    // key in the devext for future reference
    pdoExt->DeviceUsbRegPath = UsbhGetRegUsbDeviceFlags(HubFdo, Pdo);

    if (pdoExt->DeviceUsbRegPath  == NULL) {
        pdoExt->EnumFailureMsgId = USBENUM_REGISTRY_FAILURE;
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // apply any hardcoded fixes for this device
    UsbhCheckDeviceErrata(HubFdo, Pdo);


    UsbhCheck4GlobalErrata(HubFdo, Pdo);


    // this is where we add support for the 'workaround' database
    // query the database here for any device flags, the database will override
    // the registry.
    //<>

    return STATUS_SUCCESS;
}


//
// DeviceClass, Subclass and Protocol for composite device using
// interface association descriptor
//

#define USBHUB_INTERFACE_ASSOCIATION_DESCRIPTOR_ID(DevDesc) \
    (DevDesc->bDeviceClass == 0xEF && DevDesc->bDeviceSubClass == 0x02 && \
    DevDesc->bDeviceProtocol == 0x01)

_Must_inspect_result_
__drv_aliasesMem
__drv_allocatesMem(Mem)
NTSTATUS
UsbhGetDeviceInformation(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    This function fetches descriptors from a newly enumerated device
    so we can generate device ids

Irql: PASSIVE

Arguments:

    Pdo -- a latched Pdo

Return Value:

    sets
    PdoFlags.DeviceIsComposite
    PdoFlags.DeviceIsHub

    nt status

--*/
{
    ULONG cfgLength;
    ULONG devLength;
    PUSB_DEVICE_DESCRIPTOR devDs;
    PUSB_CONFIGURATION_DESCRIPTOR cfgDs;
    ULONG retry = 0;
    USBD_STATUS usbdStatus;
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;
    BOOLEAN extendedValidation = FALSE;
    USHORT bufferLength;


    pdoExt = PdoExt(Pdo);

    // allocate descriptor buffers
    devLength = sizeof(struct _USB_DEVICE_DESCRIPTOR);
    // This is the exact same sequence as the original usbhub driver
    // I'm leaving it this way for compatibility sake.
    //cfgLength = sizeof(USB_CONFIGURATION_DESCRIPTOR);
    cfgLength = bufferLength = 255;

    devDs = &pdoExt->DeviceDescriptor;
    cfgDs = NULL;

cfg_Retry:

    UsbhAllocatePool_Z(cfgDs, NonPagedPool, cfgLength);

    if (!cfgDs) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    nts = Usbh_HubGetUsbDescriptors(HubFdo, pdoExt->DeviceHandle,
                (PUCHAR)devDs, &devLength, (PUCHAR)cfgDs, &cfgLength);


    if (NT_SUCCESS(nts) && cfgLength < cfgDs->wTotalLength && !retry) {
        cfgLength = bufferLength = cfgDs->wTotalLength;
        UsbhFreePool(cfgDs);
        retry++;
        goto cfg_Retry;
    }

    if (NT_ERROR(nts)) {
        pdoExt->EnumFailureMsgId = USBENUM_CONFIG_DESCRIPTOR_FAILURE;

        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_GET_DESCRIPTORS_FAILURE, pdoExt->PortNumber, nts);
        HUB_EXCEPTION(USBFILE, HubFdo, pdoExt->PortNumber, BadConfigDescriptor,
                      nts, 0xFFFFFFFF, cfgDs, cfgLength);
        UsbhFreePool(cfgDs);
        return nts;
    }


    // get device info and hacks:
    // this has to be done early so the flags can effect the rest of enumeration
    // for this device
    nts = UsbhGetUsbDeviceFlags(HubFdo, Pdo);

    if (NT_ERROR(nts)) {
        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_REGISTRY_FAILURE, pdoExt->PortNumber, nts);
        return nts;
    }

    // perform validation on these descriptors
    if(HubG.GlobalEnableExtendedValidation){
        extendedValidation = TRUE;
    }

    if (!UsbhValidateConfigurationDescriptor(cfgDs,
                                             cfgLength,
                                             &usbdStatus,
                                             extendedValidation)) {

        pdoExt->EnumFailureMsgId = USBENUM_BAD_CONFIG_DESCRIPTOR;

        nts = STATUS_INVALID_PARAMETER;

        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_CONFIG_DESC_VALIDATION_FAILURE, pdoExt->PortNumber, nts);

        // throw an exception, device will fail to enumerate
        HUB_EXCEPTION(USBFILE, HubFdo, pdoExt->PortNumber, BadConfigDescriptor,
                      nts, usbdStatus, cfgDs, cfgLength);

        UsbhFreePool(cfgDs);
        return nts;
    }

    //
    // Don't fail enumeration if the device returned a different wTotalLength
    // or partial descriptor the second time, continue with whatever we've
    // got.
    //
    if (NT_SUCCESS(nts) && bufferLength < cfgDs->wTotalLength) {
        cfgDs->wTotalLength = bufferLength;
    }

    if (devDs->bNumConfigurations > 1) {
        pdoExt->SqmProperties.SupportsMultipleConfigurations = 1;
    }

    if (devDs->bNumConfigurations == 1 &&
        cfgDs->bNumInterfaces > 1 &&
        (devDs->bDeviceClass == 0 ||
         USBHUB_INTERFACE_ASSOCIATION_DESCRIPTOR_ID(devDs)) ) {

        // one configuration
        // composite device and no class codes in the device descriptor

        // class ids generated are constant

        pdoExt->PdoFlags.DeviceIsComposite = 1;

    } else {
        PUSB_INTERFACE_DESCRIPTOR ifDs;

        // single interface device
        // or
        // multi config device
        // or
        // device descriptor specifies class code

        ifDs = UsbhParseConfigurationDescriptorEx(
                    HubFdo,
                    cfgDs,
                    cfgDs,
                    -1, // interface, don't care
                    -1, // alt setting, don't care
                    -1, // don't care
                    -1, // subclass, don't care
                    -1);// protocol, don't care

        if (ifDs) {
            if (ifDs->bInterfaceClass == USB_DEVICE_CLASS_HUB) {
                pdoExt->PdoFlags.DeviceIsHub = 1;
            } else if ((devDs->bcdUSB > 0x0200) &&
                       (pdoExt->DeviceFlags.DisableUASP == 0)) {

                //
                // The UASP spec recommends that devices implement BOT as the "default"
                // interface descriptor so that they can achieve interoperability with the vast
                // majority of hosts out there.  Then, the spec recommends specifying an alternate
                // setting that contain the UASP class codes.
                // If such an alternate setting exists, we should enumerate the PDO using Compatible IDs built
                // from the alternate setting interface descriptor instead of the default one.
                //
                // In order to mitigate the risk of changing the enumeration behavior
                // with devices that may just happen to have such an alternate setting, but don't intend to
                // implement UASP, we will only support this feature on devices reporting a bcdUSB version
                // greater that 2.0.
                //
                PUSB_INTERFACE_DESCRIPTOR uaspDescriptor;
                uaspDescriptor =
                    UsbhParseConfigurationDescriptorEx(HubFdo,
                                                       cfgDs,
                                                       ifDs,
                                                       ifDs->bInterfaceNumber,
                                                       -1,
                                                       USB_DEVICE_CLASS_STORAGE,
                                                       USB_STORAGE_SUBCLASS_SCSI,
                                                       USB_STORAGE_PROTOCOL_UAS);

                if ((uaspDescriptor != NULL) &&
                    (pdoExt->Speed == UsbHighSpeed)) {
                    // UAS devices are only allowed to operate at high-speed on the EHCI stack
                    pdoExt->PdoFlags.DecorateSerialNumber = 1;
                    ifDs = uaspDescriptor;

                }

            }

            pdoExt->CompatIdInterfaceDescriptor = *ifDs;
        }

        // set up compat id class codes

    }

    if (cfgDs->bmAttributes & USB_CONFIG_REMOTE_WAKEUP) {
        // set if the device can wake-up the USB
        pdoExt->PdoFlags.UsbWakeupSupported = 1;
    }

    // save for later
    UsbhAssert(HubFdo, pdoExt->CachedConfigDescriptor == NULL);
    pdoExt->CachedConfigDescriptor = cfgDs;

    return nts;

}


BOOLEAN
UsbhGetAlternateUsbDescriptors(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PBOS_DESC_SET_INFO BosInfo
    )
/*++

Routine Description:

    Retrieve the alternate USB descriptor set that returned by the device after
    retrieving the MS OS 2.0 descriptor set

Irql: PASSIVE

Arguments:

Return Value:

    BOOLEAN

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_PDO pdoExt;
    BOOLEAN result;
    BOOLEAN isBillboard = FALSE;


    pdoExt = PdoExt(Pdo);

    // Get the alternate device descriptor
    nts = UsbhGetDeviceDescriptor(HubFdo, Pdo);

    if (!NT_SUCCESS(nts)) {
        return FALSE;
    }

    if (!UsbhValidateDeviceDescriptor(HubFdo,
                                      &pdoExt->DeviceDescriptor,
                                      sizeof(struct _USB_DEVICE_DESCRIPTOR),
                                      NULL,
                                      &isBillboard)) {
        pdoExt->EnumFailureMsgId = USBENUM_BAD_DEVICE_DESCRIPTOR;
        return FALSE;
    }

    if (isBillboard) {
        pdoExt->PdoFlags.DeviceIsBillboard = 1;
    }

    // Update the device descriptor cached by USBPORT
    UsbhBusIf_SetMsOs20Support(HubFdo,
                               pdoExt->DeviceHandle,
                               &pdoExt->DeviceDescriptor,
                               BosInfo->MsOs20DescriptorInfo->bMS_VendorCode);

    UsbhFreePool(pdoExt->CachedConfigDescriptor);


    nts = UsbhGetDeviceInformation(HubFdo, Pdo);

    if (!NT_SUCCESS(nts)) {
        return FALSE;
    }

    if ((pdoExt->DeviceDescriptor.bcdUSB > 0x0200) &&
        (!pdoExt->DeviceFlags.SkipBOSDescriptorQuery)) {
        ULONG bufferLength;

        RtlZeroBytes(BosInfo, sizeof(BOS_DESC_SET_INFO));
        UsbhFreePool(pdoExt->BosDescriptor);

        //
        // It's a 2.1 device, see if it has a BOS Descriptor
        //
        nts = UsbhGetBosDescriptor(HubFdo, Pdo, &bufferLength);

        if (!NT_SUCCESS(nts)) {
            return TRUE;
        }

        // Validate the new BOS descriptor
        result = UsbhValidateBOSDescriptorSet(pdoExt->HubFdo,
                                              pdoExt->BosDescriptor,
                                              bufferLength,
                                              BosInfo);
        if (result == FALSE) {
            pdoExt->EnumFailureMsgId = USBENUM_BAD_BOS_DESCRIPTOR;
            return FALSE;
        }
    }

    pdoExt->MsOs20Info.Flags.AlternateEnumeration = 1;

    return TRUE;
}

NTSTATUS
UsbhSetupDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USBD_STATUS *ReturnUsbdStatus
    )
/*++

Routine Description:

    Setup a new device for the bus.  This is where we do all the other misc
    initialization for a device.

Irql: PASSIVE

Arguments:

Return Value:

    sets up ids 'special' flags and descriptors in the PDO extension

    optional  UsbdStatus of the operation

    ntstatus

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    USB_HUB_PORT_STATE ps;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    BOOLEAN updateRequired = FALSE;
    PMSOS20_INFO msOs20Info;
    ULONG cfgLength;
    ULONG devLength;

    hubExt = FdoExt(HubFdo);
    UsbhAssert(HubFdo, Pdo);
    pdoExt = PdoExt(Pdo);

    LOG(HubFdo, LOG_BUS, 'setD', Pdo, pdoExt->PortNumber);

    // release any data allocated by previous call to setupdevice
    // NOTE: setupdevice is bypassed in the case where the PDO is already
    // enumerated -- ie device reset by function driver.
    UsbhFreePool(pdoExt->CachedConfigDescriptor);
    UsbhFreePool(pdoExt->DeviceUsbRegPath);
    UsbhFreePool(pdoExt->PnPDeviceDescription);
    UsbhFreePool(pdoExt->BosDescriptor);
    pdoExt->BillboardDescriptor = NULL;
    UsbhFreePool(pdoExt->MsOs20Info.DescriptorSet);
    UsbhFreeID(&(pdoExt)->SerialNumberId);
    pdoExt->PdoFlags.SerialNumber = 0;
    UsbhFreeID(&(pdoExt)->SupportedLanguageIds);
    UsbhFreeID(&(pdoExt)->GenericDevString);

    if(pdoExt->MsExtConfigDesc){
        UsbhFreePool(pdoExt->MsExtConfigDesc);
    }

    msOs20Info = &pdoExt->MsOs20Info;
    RtlZeroBytes(msOs20Info, sizeof(pdoExt->MsOs20Info));

    // determine the speed from the port status bits
    //
    // at this point we can determine the current operating speed of the
    // device based on port status.  If the device is in one of the root ports
    // of a USB 2 controller it will always be high speed, if it is in a HS
    // hub it may be high full or low.  if it is in a FS hub it may also be
    // it may be high speed capable.
    nts = UsbhQueryPortState(HubFdo, pdoExt->PortNumber,
                             &ps, &usbdStatus);

    SET_OPTIONAL_STATUS(ReturnUsbdStatus, usbdStatus);

    if (NT_ERROR(nts)) {
        pdoExt->EnumFailureMsgId = USBENUM_PORT_STATUS_FAILURE;
        return nts;
    }

    UsbhAssert(HubFdo, !(ps.Status.LowSpeed && ps.Status.HighSpeed));
    if (ps.Status.LowSpeed) {
        pdoExt->Speed = UsbLowSpeed;
    } else if (ps.Status.HighSpeed) {
        pdoExt->Speed = UsbHighSpeed;
    } else {
        pdoExt->Speed = UsbFullSpeed;
    }

    LOG(HubFdo, LOG_BUS, 'stD1', Pdo, pdoExt->Speed);

    // Get the cached device descriptor from USBPORT
    devLength = sizeof(pdoExt->DeviceDescriptor);
    cfgLength = 0;
    nts = Usbh_HubGetUsbDescriptors(HubFdo, pdoExt->DeviceHandle,
                (PUCHAR)&pdoExt->DeviceDescriptor, &devLength, NULL, &cfgLength);

    if (!NT_SUCCESS(nts)) {
        pdoExt->EnumFailureMsgId = USBENUM_DEVICE_DESCRIPTOR_FAILURE;
        return STATUS_INVALID_PARAMETER;
    }

    UsbhGetInitRegUsbDeviceFlags(HubFdo, Pdo);

    if (pdoExt->CachedMsOs20DeviceInfo.bAltEnumCmd != 0) {

        nts = UsbhSendMsOs20AltEnumCommand(HubFdo, Pdo, &pdoExt->CachedMsOs20DeviceInfo);

        if (!NT_SUCCESS(nts)) {
            pdoExt->EnumFailureMsgId = USBENUM_MSOS20_SET_ALT_ENUMERATION_FAILURE;
            return STATUS_INVALID_PARAMETER;
        }

        // Get the alternate device descriptor
        nts = UsbhGetDeviceDescriptor(HubFdo, Pdo);

        if (!NT_SUCCESS(nts)) {
            pdoExt->EnumFailureMsgId = USBENUM_DEVICE_DESCRIPTOR_FAILURE;
            return STATUS_INVALID_PARAMETER;
        }

        if (!UsbhValidateDeviceDescriptor(HubFdo,
                                          &pdoExt->DeviceDescriptor,
                                          sizeof(struct _USB_DEVICE_DESCRIPTOR),
                                          NULL,
                                          NULL)) {
            pdoExt->EnumFailureMsgId = USBENUM_BAD_DEVICE_DESCRIPTOR;
            return STATUS_INVALID_PARAMETER;
        }

        // Tell USB hub to cache the original device descriptor
        UsbhBusIf_SetMsOs20Support(HubFdo,
                                   pdoExt->DeviceHandle,
                                   &pdoExt->DeviceDescriptor,
                                   0);
    }

    // get device info, includes a copy of the device descriptor
    nts = UsbhGetDeviceInformation(HubFdo, Pdo);
    if (NT_ERROR(nts)) {
        return nts;
    }

    if ((pdoExt->DeviceDescriptor.bcdUSB > 0x0200) &&
        (!pdoExt->DeviceFlags.SkipBOSDescriptorQuery)) {
        ULONG bufferLength;

        //
        // It's a 2.1 device, see if it has a BOS Descriptor
        //
        nts = UsbhGetBosDescriptor(pdoExt->HubFdo, Pdo, &bufferLength);

        if (NT_SUCCESS(nts)) {
            BOOLEAN result;
            BOS_DESC_SET_INFO bosInfo;

            result = UsbhValidateBOSDescriptorSet (
                        HubFdo,
                        pdoExt->BosDescriptor,
                        bufferLength,
                        &bosInfo);

            if (result == TRUE) {
                pdoExt->SqmProperties.ValidBOSDescriptor = 1;
            }

            if (bosInfo.MsOs20DescriptorInfo != NULL) {
                // See if we need to do alternate enumeration for this device
                if ((bosInfo.MsOs20DescriptorInfo->bAltEnumCmd != 0) &&
                    (pdoExt->PdoFlags.MsOs20AltEnumCmdSent == FALSE)) {

                    // Store a copy of the descriptor set info to be cached now,
                    // as bosInfo will be zero'd out and the old BOS descriptor will be
                    // freed if there is an alternate BOS descriptor
                    RtlCopyMemory(&pdoExt->CachedMsOs20DeviceInfo,
                                  bosInfo.MsOs20DescriptorInfo,
                                  sizeof(MSOS20_DESCRIPTOR_SET_INFO));

                    pdoExt->MsOs20Info.Flags.AlternateEnumeration = 1;

                    // Send the alternate enumeration command
                    nts = UsbhSendMsOs20AltEnumCommand(HubFdo, Pdo, bosInfo.MsOs20DescriptorInfo);

                    if (!NT_SUCCESS(nts)) {
                        pdoExt->EnumFailureMsgId = USBENUM_MSOS20_SET_ALT_ENUMERATION_FAILURE;
                        return STATUS_INVALID_PARAMETER;
                    }

                    // Fetch and validate the alternate USB descriptors
                    result = UsbhGetAlternateUsbDescriptors(HubFdo, Pdo, &bosInfo);

                    if (result == FALSE) {
                        return STATUS_INVALID_PARAMETER;
                    }

                    // Cache the descriptor set info
                    RtlWriteRegistryValue(RTL_REGISTRY_CONTROL,
                                          pdoExt->DeviceUsbRegPath,
                                          L"MsOs20DescriptorSetInfo",
                                          REG_BINARY,
                                          &pdoExt->CachedMsOs20DeviceInfo,
                                          sizeof(pdoExt->CachedMsOs20DeviceInfo));
                }
            }

            if (bosInfo.MsOs20DescriptorInfo != NULL) {
                pdoExt->MsOs20Info.Flags.DescriptorSetInfo = 1;

                // Insure we don't query legacy MS OS descriptors
                pdoExt->DeviceFlags.QueryMsOs = 0;
                pdoExt->DeviceFlags.MsOsSupported = 0;

                if (bosInfo.MsOs20DescriptorInfo->wLength != 0) {

                    // Notify USBPORT that this device supports MS OS 2.0 Descriptors
                    UsbhBusIf_SetMsOs20Support(HubFdo,
                                               pdoExt->DeviceHandle,
                                               NULL,
                                               bosInfo.MsOs20DescriptorInfo->bMS_VendorCode);

                    // Retrieve the entire MS OS 2.0 Descriptor Set
                    nts = UsbhGetMsOs20DescriptorSet(HubFdo, Pdo, bosInfo.MsOs20DescriptorInfo);

                    if (!NT_SUCCESS(nts)) {
                        pdoExt->EnumFailureMsgId = USBENUM_MSOS20_DESCRIPTOR_SET_FAILURE;
                        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_MSOS20_DESC_FAILURE, pdoExt->PortNumber, nts);
                        return nts;
                    }

                    // Validate the MS OS 2.0 Descriptor Set
                    result = UsbhValidateMsOs20DescriptorSet(HubFdo,
                                                             Pdo,
                                                             bosInfo.MsOs20DescriptorInfo);

                    if (result == FALSE) {
                        pdoExt->EnumFailureMsgId = USBENUM_INVALID_MSOS20_DESCRIPTOR_SET;
                        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_MSOS20_DESC_VALIDATION_FAILURE, pdoExt->PortNumber, STATUS_INVALID_PARAMETER);
                        return STATUS_INVALID_PARAMETER;
                    }

                    if (pdoExt->MsOs20Info.Flags.CcgpDevice == 1) {
                        pdoExt->PdoFlags.DeviceIsComposite = 1;
                    }

                    if ((pdoExt->PortAttributes.DeviceNotRemovable == 1) &&
                        (pdoExt->PdoFlags.DeviceIsHub == 0) &&
                        (pdoExt->MsOs20Info.Flags.MinResumeTime == 1)) {

                        UCHAR resumeSignalingTime;

                            resumeSignalingTime = pdoExt->MsOs20Info.MinResumeTimeDescriptor->bResumeSignalingTime;

                            // if the device is not root attached, set the minimum resume signaling time for the root
                            // to the USB spec'd 20ms.
                            if (!hubExt->FdoFlags.HubIsRoot) {
                                resumeSignalingTime = 20;
                            }

                            UsbhBusIf_SetClearRootPortResumeTime(HubFdo,
                                                                 pdoExt->DeviceHandle,
                                                                 TRUE,
                                                                 resumeSignalingTime);

                            pdoExt->PdoFlags.ResumeTimeSet = 1;
                    }
                }
            }

            if (bosInfo.UsbDualRoleFeatures != NULL) {
                USB_DUAL_ROLE_FEATURES usbDualRoleFeatures;
                USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
                USBD_STATUS usbdStatus;
                USHORT dataReturned;

                pdoExt->DeviceFlags.DualRoleDevice = 1;
                UsbDualRoleFeaturesSetUlong(&pdoExt->UsbDualRoleFeatures, bosInfo.UsbDualRoleFeatures->USBFeatures);
                //
                // Publish the USB dual-role capabilities of the device being enumerated
                //
                WnfPublishUsbPartnerDualRoleFeatures(
                    pdoExt->UcmConnectorId,
                    TRUE,
                    bosInfo.UsbDualRoleFeatures->USBFeatures);

                UsbDualRoleFeaturesQueryLocalMachine(&usbDualRoleFeatures);

                //
                // Setup vendor command to be sent to device with our local dual-role capabilities
                //
                setupPacket.bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
                setupPacket.bmRequestType.Type = BMREQUEST_VENDOR;
                setupPacket.bmRequestType.Reserved = 0;
                setupPacket.bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;
                setupPacket.bRequest = bosInfo.UsbDualRoleFeatures->VendorCmdCode;
                setupPacket.wValue.W = LOWORD(UsbDualRoleFeaturesGetUlong(&usbDualRoleFeatures));
                setupPacket.wIndex.W = HIWORD(UsbDualRoleFeaturesGetUlong(&usbDualRoleFeatures));
                setupPacket.wLength = 0;
                dataReturned = 0;

                nts = UsbhSyncSendCommandToDevice(pdoExt->HubFdo,
                                                  Pdo,
                                                  &setupPacket,
                                                  NULL,
                                                  &dataReturned,
                                                  USBH_CONTROL_PIPE_TIMEOUT,
                                                  &usbdStatus);

                if (!NT_SUCCESS(nts)) {
                    LOG(HubFdo, LOG_BUS, 'Sd11', 0, 0);
                }
            }

            if (bosInfo.BillboardDescriptor != NULL) {
                nts = UsbhGetBillboardInfo(pdoExt->HubFdo,
                                           Pdo,
                                           bosInfo.BillboardDescriptor);
                if (!NT_SUCCESS(nts)) {
                    return nts;
                }

                pdoExt->BillboardDescriptor = bosInfo.BillboardDescriptor;
            }

            //
            // The MS OS Container ID Descriptor will have priority over
            // the BOS Container ID Descriptor
            //
            if ((pdoExt->ContainerIDValid == FALSE) &&
                (result == TRUE) &&
                (bosInfo.ContainerIDDescriptor != NULL)) {

                RtlCopyMemory(&pdoExt->ContainerID,
                                bosInfo.ContainerIDDescriptor->ContainerID,
                                sizeof(GUID));

                pdoExt->ContainerIDValid = TRUE;
            }

            if (bosInfo.USB30CapabilityDescriptor != NULL) {
                pdoExt->PdoFlags.DeviceIsSuperSpeedCapable = 1;
                pdoExt->SqmProperties.IsSuperSpeedCapable = 1;
            }
        }
    }

    if (!NT_SUCCESS(nts)) {
        return nts;
    }

    //
    // This must be called after UsbhGetUsbDeviceFlags if you want to avoid additional
    // queries for the MSOS descriptor to devices that do not support it.
    if (msOs20Info->Flags.DescriptorSetHeader == 0) {
        nts = UsbhGetMSOS_Descriptor(HubFdo, Pdo);
        if (NT_ERROR(nts) && nts != STATUS_NOT_SUPPORTED) {
            // not supported is OK with us. Another error may mean that the request killed the
            // device.  We won't ask it the next time.
            UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_MSOS_DESC_FAILURE, pdoExt->PortNumber, nts);
            return nts;
        }
    }

    //
    // Check if this device is owned by a virtual machine
    //
    nts = UsbhUpdateUxdSettings(HubFdo,
                                Pdo,
                                FALSE,
                                &updateRequired);
    if(NT_ERROR(nts)){
        return nts;
    }

    // generate USB information from the device info, this will be stored in PdoExt
    // until lit can be assigned

    nts = UsbhGetSerialNumber(HubFdo,
                              Pdo,
                              &pdoExt->SerialNumberId);
    if (NT_ERROR(nts)) {
        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_SERIAL_NUMBER_FAILURE, pdoExt->PortNumber, nts);
        return nts;
    }


    // did we get an SN?
    if (pdoExt->SerialNumberId.Buffer) {
        pdoExt->PdoFlags.SerialNumber = 1;
        DbgTrace((UsbhDebugTrace, "SN :%ws:\n", pdoExt->SerialNumberId.Buffer));
    }

    //
    // Look for a MS OS Extended config desc
    UsbhAssert(HubFdo, pdoExt->MsExtConfigDesc == NULL);
    if(pdoExt->DeviceFlags.MsOsSupported &&
       !pdoExt->PdoFlags.DeviceIsComposite){

        pdoExt->MsExtConfigDesc =
            UsbhGetMsOsExtendedConfigDesc(HubFdo,
                                          Pdo);
    }

    // Check for a container ID if the device is removable
    if (!pdoExt->PortAttributes.DeviceNotRemovable) {
        nts = UsbhGetMsOsContainerID(HubFdo, Pdo);
    }

    // STATUS_NOT_SUPPORTED means the device did not hang
    // on the Container ID request, so we can proceed with
    // enumeration.  Any other error means a device error
    // and we must reset and retry enumeration.
    if(NT_ERROR(nts) && (nts != STATUS_NOT_SUPPORTED) ){
        return nts;
    }

    // generate the id strings
    // but free the default ids first
    UsbhFreeID(&pdoExt->PnpHardwareId);
    UsbhFreeID(&pdoExt->PnpDeviceId);
    UsbhFreeID(&pdoExt->PnpCompatibleId);

    nts = UsbhBuildDeviceID(HubFdo, Pdo, &pdoExt->PnpDeviceId);

    if (NT_SUCCESS(nts)) {
        nts = UsbhBuildHardwareID(HubFdo, Pdo,
                &pdoExt->PnpHardwareId);

        if (NT_SUCCESS(nts)) {
            nts = UsbhBuildCompatibleID(HubFdo, Pdo,
                &pdoExt->PnpCompatibleId);

            if (NT_ERROR(nts)) {
                UsbhFreeID(&pdoExt->PnpHardwareId);
                UsbhFreeID(&pdoExt->PnpDeviceId);
            }
        } else {
            UsbhFreeID(&pdoExt->PnpDeviceId);
        }
    }

    // get the list of supported languages, this is allowed to fail
    UsbhGetLanguageIdString(HubFdo, Pdo, &pdoExt->SupportedLanguageIds);

    // most devices only support an english product Id so fetch this now -- if
    // the OS needs a different language it will tell us when it queries the
    // string.

    if (pdoExt->DeviceDescriptor.iProduct != 0 &&
        !pdoExt->DeviceFlags.DisableSerialNumber) {
        // if this fails we just don't have a language id
        UsbhGetProductIdString(pdoExt->HubFdo,
                                     Pdo,
                                     0x409,
                                     &pdoExt->ProductId);
    }


    if (NT_SUCCESS(nts)) {

        // determine if this is a high speed device operating at FULL speed,
        // generate an exception if it is.
        if (hubExt->DeviceSpeed == UsbFullSpeed &&
            pdoExt->Speed == UsbFullSpeed &&
            pdoExt->DeviceDescriptor.bcdUSB >= 0x0200) {

            USHORT nBytes;
            USB_DEVICE_QUALIFIER_DESCRIPTOR dqs;

            nBytes = sizeof(struct _USB_DEVICE_QUALIFIER_DESCRIPTOR);

            nts = UsbhGetQualifierDescriptorFromDevice(HubFdo,
                                                       Pdo,
                                                       NULL,
                                                       (PUCHAR) &dqs,
                                                       &nBytes);

            if (NT_SUCCESS(nts)) {
                // success means it is capable of high speed, we throw and
                // exception to generate the popup
                HUB_EXCEPTION(USBFILE, HubFdo, pdoExt->PortNumber, PnpErr_LegacyDevice,
                      nts, 0, &dqs, sizeof(struct _USB_DEVICE_QUALIFIER_DESCRIPTOR));

                pdoExt->PdoFlags.HsDeviceRunningAtFs = 1;
            }

            // eat the status
            nts = STATUS_SUCCESS;
        }
    }

    //
    nts = Usbh__TestPoint__Ulong(HubFdo, TstPt_SetupDeviceFailureRetry,
        nts, pdoExt->PortNumber);

    LOG(HubFdo, LOG_BUS, 'stD>', Pdo, nts);

    return nts;
}


VOID
UsbhBusConnectPdo(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    )
/*++

Routine Description:

    handles physical connect of a Pdo from the bus engine

    RegisterPdo

    This function puts the PDO on the device list reported in the next QBR.
    Once reported PNP will know about this device.

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_OBJECT pdo;
    HUB_PDO_HWPNP_STATE pdoState;
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    PSTATE_CONTEXT sc;

    UsbhAssert(HubFdo, !PortData->PdoLatched);
    sc = PortData->CurrentSc;

    UsbhAcquirePdoStateLock(HubFdo, sc, PdoEv_RegisterPdo);

    // pdo may be removed before we acquire the lock and to perform the actual connect
    // so we must check here to make sure the PDO is still valid.

    pdo = (PDEVICE_OBJECT) PortData->PnpPdo;
    if (pdo) {
        pdoState = GET_PDO_HWPNPSTATE(pdo);

        hubExt = FdoExt(HubFdo);

        UsbhSetConnectionStatus(HubFdo, PortData, DeviceConnected);
        // put the PDO on the device is present list and tell PnP we have a
        // device

        pdoExt = PdoExt(pdo);

        LOG(HubFdo, LOG_BUS, 'regP', pdo, pdoState);

        UsbhAssert(HubFdo, PortData != NULL);

        // see if we are re-registering
        if (pdoState == Pdo_PnpRefHwPresent && PortData) {

            // we should only hit this path on a successful reset
            UsbhAssert(HubFdo, PortData->PnpPdo == pdo);
            UsbhAssert(HubFdo, !PdoExt(pdo)->PdoFlags.EnumerationFailed);


            DbgTrace((UsbhDebugTrace, "#%d re-register (device reset) PDO %p\n",
                PN(PortData), PortData->PnpPdo));

            UsbhReleasePdoStateLock(HubFdo, sc);

            UsbhSignalSyncDeviceReset(HubFdo, pdo, PortData, STATUS_SUCCESS);
            return;
        }

        DbgTrace((UsbhDebugTrace, "#%d Register new PDO %p\n", pdoExt->PortNumber, pdo));
        UsbhAssert(HubFdo, pdoState == Pdo_Created);

        InsertTailList(&hubExt->PdoPresentListHead,
                       &pdoExt->PdoListLink);

        UsbhEtwLogDeviceInformation(pdoExt, &USBHUB_ETW_EVENT_DEVICE_ADD, TRUE);

        SET_PDO_HWPNPSTATE(pdo, Pdo_HwPresent, PdoEv_RegisterPdo);
        hubExt->BusRelationsState = BusRelationsInvalid;
        UsbhReleasePdoStateLock(HubFdo, sc);

        DbgTrace((UsbhDebugTrace,"*USB DEVICE *********************************\n"));
        if (pdoExt->PdoFlags.UsbWakeupSupported) {
            DbgTrace((UsbhDebugTrace,"** UsbWakeupSupported\n"));
        }
        DbgTrace((UsbhDebugTrace,"*USB DEVICE DESCRIPTOR:\n"));
        DbgTrace((UsbhDebugTrace,"*bLength %d\n",DD(pdoExt).bLength));
        DbgTrace((UsbhDebugTrace,"*%!USBDST!\n", DD(pdoExt).bDescriptorType));
        DbgTrace((UsbhDebugTrace,"**bcdUSB SPEC VERSION %04.4x\n", DD(pdoExt).bcdUSB));
        DbgTrace((UsbhDebugTrace,"***Device Speed %!USBSPEED!\n", pdoExt->Speed));
        DbgTrace((UsbhDebugTrace,"*bDeviceClass %02.2x\n", DD(pdoExt).bDeviceClass));
        DbgTrace((UsbhDebugTrace,"*bDeviceSubClass %02.2x\n", DD(pdoExt).bDeviceSubClass));
        DbgTrace((UsbhDebugTrace,"*bDeviceProtocol %02.2x\n", DD(pdoExt).bDeviceProtocol));
        DbgTrace((UsbhDebugTrace,"*bMaxPacketSize0 %d\n", DD(pdoExt).bMaxPacketSize0));
        DbgTrace((UsbhDebugTrace,"**idVendor %04.4x\n", DD(pdoExt).idVendor));
        DbgTrace((UsbhDebugTrace,"***idProduct %04.4x\n", DD(pdoExt).idProduct));
        DbgTrace((UsbhDebugTrace,"****bcdDevice %04.4x\n", DD(pdoExt).bcdDevice));
        DbgTrace((UsbhDebugTrace,"*iManufacturer %d\n", DD(pdoExt).iManufacturer));
        DbgTrace((UsbhDebugTrace,"*iProduct %d\n", DD(pdoExt).iProduct));
        DbgTrace((UsbhDebugTrace,"*iSerialNumber %d\n", DD(pdoExt).iSerialNumber));
        DbgTrace((UsbhDebugTrace,"*bNumConfigurations %d\n", DD(pdoExt).bNumConfigurations));

        if (pdoExt->PnpDeviceId.Buffer) {
            DbgTrace((UsbhDebugTrace,"*PnpDeviceId STR :%ws:\n",
                    pdoExt->PnpDeviceId.Buffer));
        }
        if (pdoExt->PnpHardwareId.Buffer) {
            DbgTrace((UsbhDebugTrace,"*PnpHardwareId STR :%ws:\n",
                    pdoExt->PnpHardwareId.Buffer));
        }
        if (pdoExt->PnpCompatibleId.Buffer) {
            DbgTrace((UsbhDebugTrace,"*PnpCompatibleId STR :%ws:\n",
                    pdoExt->PnpCompatibleId.Buffer));
        }
        if (pdoExt->UniqueId.Buffer) {
            DbgTrace((UsbhDebugTrace,"*UniqueId STR :%ws:\n",
                    pdoExt->UniqueId.Buffer));
        }
        if (pdoExt->SerialNumberId.Buffer) {
            DbgTrace((UsbhDebugTrace,"*SN STR %ws\n",
                    pdoExt->SerialNumberId.Buffer));
        }
        if (pdoExt->ProductId.Buffer) {
            DbgTrace((UsbhDebugTrace,"*Product STR :%ws:\n",
                    pdoExt->ProductId.Buffer));
        }
        DbgTrace((UsbhDebugTrace,"*********++++++++++++++++++++++++++++*******\n"));

        DbgTrace((UsbhDebugTrace, "IoInvalidateDeviceRelations (new PDO)\n"));
        UsbhBusIoInvalidateDeviceRelations(HubFdo, sc);

    } else {
        // PDO no longer present
        UsbhReleasePdoStateLock(HubFdo, sc);
    }
    return;
}


VOID
Usbh_BusPnpStop_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Handles PdoEv_BusPnpStop

    Context is global

Irql: ANY

Arguments:

Return Value:

    none

--*/
{
    HUB_PDO_HWPNP_STATE curState;
    USHORT pn;

    LOG(HubFdo, LOG_PNP, 'fSRv', 0, StateContext);

    UsbhAcquirePdoStateLock(HubFdo, StateContext, PdoEv_BusPnpStop);

    for (pn=1; pn <= N_PORTS(HubFdo); pn++) {

        PHUB_PORT_DATA pd;
        PDEVICE_OBJECT pdo;

        pd = UsbhGetPortData(HubFdo, pn);
        if (pd) {

            // we access Portdata directly because we hold the lock
            pdo = (PDEVICE_OBJECT) pd->PnpPdo;
            LOG(HubFdo, LOG_BUS, 'xPD1', pdo, 0);

            if (pdo != NULL) {
                PDEVICE_EXTENSION_PDO pdoExt;
                KIRQL irql;

                pdoExt = PdoExt(pdo);

                KeAcquireSpinLock(&pdoExt->GlobalListStateLock, &irql);

                if (pdoExt->GlobalListState == PdoInGlobalList) {
                    pdoExt->GlobalListState = PdoParentHubGone;
                    USBD_RemoveDeviceFromGlobalList(pdo);
                }

                KeReleaseSpinLock(&pdoExt->GlobalListStateLock, irql);

                curState = GET_PDO_HWPNPSTATE(pdo);
                switch(curState) {
                    // device disconnected while we were initializing the PDO
                case Pdo_Created:
                case Pdo_HwPresent:
                    // pnp does not know about it yet so drivers don't know about it either
                    // we can delete it
                    //-> Pdo_Deleted;
                    UsbhDeletePdo(HubFdo, pdo, curState ,PdoEv_BusPnpStop, StateContext);
                    UsbhAcquirePdoStateLock(HubFdo, StateContext, PdoEv_BusPnpStop);
                    break;

                // pnp stop is not a soft-stop it is called in the context of surprise remove and
                // remove, however, we leave the pdo intact until the actual remove is send at which time
                // we generate as BusRemove event for the PDO.
                case Pdo_PnpRefHwPresent:
                    SET_PDO_HWPNPSTATE(pdo, curState, PdoEv_BusPnpStop);
                    break;

                case Pdo_PnpRefHwAbsent:
                case Pdo_HwAbsent:
                    // should not be able to access this PDO

                default:
                    UsbhAssertFailure(HubFdo, pdo);
                    break;
                }
            }

        }
    }

    UsbhReleasePdoStateLock(HubFdo, StateContext);

}


VOID
Usbh_BusPnpFreeResources_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Handles PdoEv_BusPnpFreeResources

    Context is global

    This is called in the context of the hub handling IRP_MN_STOP_DEVICE
    to free any resources associated with the PDO (ie the devicehandle) in
    the event it has not been freed yet.

Irql: ANY

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    HUB_PDO_HWPNP_STATE curState;
    USHORT pn;

    LOG(HubFdo, LOG_PNP, 'fRSR', 0, StateContext);

    UsbhAcquirePdoStateLock(HubFdo, StateContext, PdoEv_BusPnpFreeResources);

    for (pn=1; pn <= N_PORTS(HubFdo); pn++) {

        PHUB_PORT_DATA pd;
        PDEVICE_OBJECT pdo;

        pd = UsbhGetPortData(HubFdo, pn);
        if (pd) {

            // we access Portdata directly because we hold the lock
            pdo = (PDEVICE_OBJECT) pd->PnpPdo;
            LOG(HubFdo, LOG_BUS, 'xPD2', pdo, 0);

            if (pdo != NULL) {
                curState = GET_PDO_HWPNPSTATE(pdo);
                pdoExt = PdoExt(pdo);
                switch(curState) {
                    // device disconnected while we were initializing the PDO
                case Pdo_Created:
                case Pdo_HwPresent:

                    // device was being enumerated when we where stopped.
                    // pnp does not know about it yet so drivers don't know about it either
                    // we can delete it. This will show as a fresh connect on restart.
                    //-> Pdo_Deleted;
                    UsbhDeletePdo(HubFdo, pdo, curState ,PdoEv_BusPnpFreeResources, StateContext);

                    UsbhAcquirePdoStateLock(HubFdo, StateContext, PdoEv_BusPnpFreeResources);
                    break;

                    // device disconnected after we reported the PDO
                case Pdo_PnpRefHwPresent:
                    UsbhReleasePdoStateLock(HubFdo, StateContext);

                    UsbhUnlinkPdoDeviceHandle(HubFdo, pdo, 'xDH1', TRUE);

                    UsbhAcquirePdoStateLock(HubFdo, StateContext, PdoEv_BusPnpFreeResources);
                    break;

                default:
                    break;
                }
            }

        }
    }

    UsbhReleasePdoStateLock(HubFdo, StateContext);

}



VOID
Usbh_BusRemove_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Handles PdoEv_BusPnpStop

    Context is global

Irql: ANY

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    HUB_PDO_HWPNP_STATE curState;
    USHORT pn;

    LOG(HubFdo, LOG_PNP, 'fSRv', 0, StateContext);

    hubExt = FdoExt(HubFdo);

    // scan the PDO list for PDOs that are about to be reported gone.
    // they share the same fate as PDOs that are present because pnp
    // does not know they are gone

    UsbhAcquirePdoStateLock(HubFdo, StateContext, PdoEv_BusRemove);

    //
    // Unblock any PDOs that might be waiting for the parent to go to D0
    //
    if (hubExt->FdoFlags.HubInitialized == 1) {
        UsbhFdoUnblockAllPendedPdoD0Irps(HubFdo);
    }

    LOG(HubFdo, LOG_PNP, 'flR1', 0, StateContext);
    // for any PDOs on the deleted list PnP now knows they are gone
    while (!IsListEmpty(&hubExt->PdoRemovedListHead)) {

        PDEVICE_EXTENSION_PDO pdoExt;
        PLIST_ENTRY le;

        le = RemoveHeadList(&hubExt->PdoRemovedListHead);

        pdoExt = (PDEVICE_EXTENSION_PDO) CONTAINING_RECORD(
                le,
                struct _DEVICE_EXTENSION_PDO,
                PdoListLink);

        LOG(HubFdo, LOG_BUS, 'flR2', pdoExt, 0);
        ASSERT_PDOEXT(pdoExt);

        pdoExt->PdoListLink.Flink = NULL;
        pdoExt->PdoListLink.Blink = NULL;

        curState = GET_PDO_HWPNPSTATE(pdoExt->Pdo);
        UsbhAssert(HubFdo, curState == Pdo_PnpRefHwAbsent);

        //-> Pdo_Deleted;
        UsbhDeletePdo(HubFdo, pdoExt->Pdo, curState, PdoEv_BusRemove, StateContext);

        UsbhAcquirePdoStateLock(HubFdo, StateContext, PdoEv_BusRemove);
    }

    // Remove any PDOs on the absent list and mark them as HubRemoved

    while (!IsListEmpty(&hubExt->PdoAbsentListHead)) {

        PDEVICE_EXTENSION_PDO pdoExt;
        PLIST_ENTRY le;

        le = RemoveHeadList(&hubExt->PdoAbsentListHead);

        pdoExt = (PDEVICE_EXTENSION_PDO) CONTAINING_RECORD(
                le,
                struct _DEVICE_EXTENSION_PDO,
                PdoListLink);

        LOG(HubFdo, LOG_BUS, 'PDAb', pdoExt, 0);
        ASSERT_PDOEXT(pdoExt);

        pdoExt->PdoListLink.Flink = NULL;
        pdoExt->PdoListLink.Blink = NULL;

        curState = GET_PDO_HWPNPSTATE(pdoExt->Pdo);
        UsbhAssert(HubFdo, curState == Pdo_HwAbsent);

        // Indicate that the PDO's parent hub is gone.
        // This will allow the PDO handler for IRP_MN_REMOVE to know
        // that it cannot touch the hub FDO
        pdoExt->PdoFlags.HubRemoved = 1;
    }

    LOG(HubFdo, LOG_PNP, 'flR3', 0, StateContext);

    for (pn=1;
         hubExt->FdoFlags.HubDescriptorValid && pn <= N_PORTS(HubFdo);
         pn++) {

        PHUB_PORT_DATA pd;
        PDEVICE_OBJECT pdo;

        pd = UsbhGetPortData(HubFdo, pn);
        if (pd) {

            // we access Portdata directly because we hold the lock
            pdo = (PDEVICE_OBJECT) pd->PnpPdo;
            LOG(HubFdo, LOG_BUS, 'xPD3', pdo, 0);

            if (pdo != NULL) {
                UsbhSetPdo_FailIo(pdo);
                curState = GET_PDO_HWPNPSTATE(pdo);
                //-> Pdo_Deleted;
                UsbhDeletePdo(HubFdo, pdo, curState, PdoEv_BusRemove, StateContext);
                UsbhAcquirePdoStateLock(HubFdo, StateContext, PdoEv_BusRemove);
            }

        }
    }

    UsbhReleasePdoStateLock(HubFdo, StateContext);

}


VOID
Usbh_PdoSurpriseRemove_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Handles PdoEv_PdoSurpriseRemove

Irql: PASSIVE_LEVEL

Arguments:

Return Value:

    none

--*/
{
    HUB_PDO_HWPNP_STATE curState;
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    PHGSL lock;
    PHUB_PORT_DATA pd;

    pdoExt = PdoExt(Pdo);
    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_PNP, 'pSU!', 0, StateContext);

    UsbhAcquirePdoStateLock(HubFdo, StateContext, PdoEv_PdoSurpriseRemove);

    lock = &StateContext->gLock;
    UsbhAssert(HubFdo, lock->Locked);

    curState = GET_PDO_HWPNPSTATE(Pdo);
    switch(curState) {

    case Pdo_PnpRefHwAbsent:
        // this happens if we and pnp agree for different reasons.
        // - if we detected the hw is gone by checking the ports.
        // - pnp thinks its gone because the hub failed or was removed.

        // Pnp will still send a remove

        // Set the state and delete on the next remove.
        LOG(HubFdo, LOG_BUS, 'srp2', Pdo, curState);

        // remove from 'present' and 'removed' lists
        if (UsbhOnPdoPresentList(pdoExt->HubFdo, Pdo, lock) ||
            UsbhOnPdoRemovedList(pdoExt->HubFdo, Pdo, lock)) {

            // soft remove by pnp
            if (UsbhOnPdoRemovedList(pdoExt->HubFdo, Pdo, lock)) {
                // device is about to be reported removed
                //
                // The following is a change from XP.  Treat this as a soft remove and expect two removes
                // if the hub itself is removed then the PDO will be removed by Usbh_BusRemove_PdoEvent()

                //RemoveEntryList(&pdoExt->PdoListLink);
                //pdoExt->PdoListLink.Flink = NULL;
                //pdoExt->PdoListLink.Blink = NULL;

                SET_PDO_HWPNPSTATE(Pdo, Pdo_PnpRefHwAbsent, PdoEv_PdoSurpriseRemove_NOT);
                TEST_TRAP();
            } else {
                // I don't think this is valid
                RemoveEntryList(&pdoExt->PdoListLink);
                pdoExt->PdoListLink.Flink = NULL;
                pdoExt->PdoListLink.Blink = NULL;
                SET_PDO_HWPNPSTATE(Pdo, Pdo_HwAbsent, PdoEv_PdoSurpriseRemove);
                TEST_TRAP();
            }
        } else {
            // device has been reported gone to pnp, expect one remove to follow
            SET_PDO_HWPNPSTATE(Pdo, Pdo_HwAbsent, PdoEv_PdoSurpriseRemove);
        }

        UsbhReleasePdoStateLock(HubFdo, StateContext);

        // if we still have a device handle we can free it here. If we free it here any
        // bandwidth associated with it will be released.
        //
        // if we don't delete it here it will be cleaned up when the PDO
        // is deleted.

        UsbhUnlinkPdoDeviceHandle(HubFdo, Pdo, 'xDH2', TRUE);

        LOG(HubFdo, LOG_BUS, 'dHD2', Pdo, 0);
        break;

     // When is a suprise remove not a remove ?
     case Pdo_PnpRefHwPresent:

        // This case occurs if we get a suprise remove due to a start failure on the PDO
        // such a remove

        // hardware is still present, this happens when both the hub and the child are removed
        // simultaneously

        // if we get a verifier assert here it means the pnp thinks its gone even though we never
        // detected a removal and never reported it.
        // Look for some missed case that implies that the device is reported removed -- such as a
        // remove of the parent FDO.


        UsbhReleasePdoStateLock(HubFdo, StateContext);

        // implied transition back to D0 for soft removed PDO
        if (GET_PDO_DX(Pdo) != PowerDeviceD0) {
            PSSH_BUSY_HANDLE bh;

            bh = UsbhIncHubBusy(HubFdo, StateContext, Pdo, UsbhBusySoftRemove_Tag, RESUME_HUB);
            UsbhSyncResumeDeviceInternal(pdoExt->HubFdo, StateContext, Pdo);
            UsbhDecHubBusy(HubFdo, StateContext, bh);
            UsbhAcquireFdoPwrLock(HubFdo, StateContext, Ev_PDO_SURPRISE_REMOVE, PdoSurpriseRemove_Tag);
            UsbhSetPdoPowerState( StateContext,  Pdo, 0, Pdo_D0, PdoPwrEv_SoftRemoveInDx);
            UsbhReleaseFdoPwrLock(HubFdo, StateContext);
        }

        // Abort any outstanding transfer requests for the device
        UsbhPdoAbortAllDevicePipes(HubFdo, Pdo);

        pd = UsbhGetPortData(HubFdo, pdoExt->PortNumber);

        if (pd) {
            // Lock out pause/disable events until we have completed soft disconnect
            WAIT_EVENT(&pd->EnableDisableLock);

            // this will attempt to safely disconnect the pdo if the queue is idle
            UsbhPCE_SoftDisconnect(HubFdo, pdoExt->PortNumber, StateContext);

            // unblock pause/disable events
            SET_EVENT(&pd->EnableDisableLock);
        }

        break;

    // normal case for a device being reported disconnected
    case Pdo_HwAbsent:

        SET_PDO_HWPNPSTATE(Pdo, curState, PdoEv_PdoSurpriseRemove);

        if (!UsbhOnPdoAbsentList(pdoExt->HubFdo, Pdo, lock)) {
            InsertTailList(&hubExt->PdoAbsentListHead, &pdoExt->PdoListLink);
        }

        UsbhReleasePdoStateLock(HubFdo, StateContext);

        break;

    default:
        // this means we are disconnecting twice
        UsbhAssertFailure(HubFdo, Pdo);
        UsbhReleasePdoStateLock(HubFdo, StateContext);
        break;
    }

}


VOID
Usbh_PdoRemove_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Handles PdoEv_PdoRemove

Irql: ANY

Arguments:

Return Value:

    none

--*/
{
    HUB_PDO_HWPNP_STATE curState;
    PHGSL lock;
    PDEVICE_EXTENSION_PDO pdoExt;
    PHUB_PORT_DATA pd = NULL;

    // use pdo context for these
    LOG(HubFdo, LOG_PNP, 'pRMV', 0, StateContext);

    UsbhAcquirePdoStateLock(HubFdo, StateContext, PdoEv_PdoRemove);

    lock = &StateContext->gLock;
    UsbhAssert(HubFdo, lock->Locked);

    curState = GET_PDO_HWPNPSTATE(Pdo);

    // Don't touch the hub FDO if the hub has been removed
    if (!HubFdo) {
        // Hub is gone, delete the PDO without referencing the hub FDO
        // Note: PDO state lock is released by UsbhDeletePdo().
        UsbhDeletePdo(NULL, Pdo, curState, PdoEv_PdoRemove, StateContext);
        return;
    }

    pdoExt = PdoExt(Pdo);

    switch(curState) {

    // these are considered 'soft removes'
    case Pdo_PnpRefHwPresent:

        // the Pdo_PnpRefHwAbsent occurs if the device is removed during a failed
        // start -- we see a remove for the start failure followed by the remove for
        // the device.

        if (pdoExt->DeviceFlags.DisableOnSoftRemove) {
            pd = UsbhGetPortData(HubFdo, pdoExt->PortNumber);
        }


        SET_PDO_HWPNPSTATE(Pdo, curState, PdoEv_PdoRemove);
        UsbhReleasePdoStateLock(HubFdo, StateContext);

        // Cleanup our UDX/virtual machine registry keys
        UsbhDeleteUxdSettings(HubFdo,
                              Pdo,
                              UXD_EVENT_DISABLE);

        // soft remove optional disable port
        if (pd) {
            UsbhSetPdo_FailIo(Pdo);

            WAIT_EVENT(&pdoExt->DriverNotFoundLock);
            if (pdoExt->PdoFlags.PortDisabled == 0) {
                pdoExt->PdoFlags.PortDisabled = 1;
                UsbhDisablePort(HubFdo, pd);

                // Mark the port as idle so the parent hub can be selectively suspended
                UsbhSshSetPortsBusyState(HubFdo, pdoExt->PortNumber, PORT_IDLE, NO_RESUME_HUB);
            }
            SET_EVENT(&pdoExt->DriverNotFoundLock);
        }

        break;

    case Pdo_PnpRefHwAbsent:
        // the Pdo_PnpRefHwAbsent occurs if the device is removed during a failed
        // start -- we see a remove for the start failure followed by the remove for
        // the device.

        SET_PDO_HWPNPSTATE(Pdo, curState, PdoEv_PdoRemove);
        UsbhReleasePdoStateLock(HubFdo, StateContext);
        break;

    case Pdo_HwAbsent:
        // this is a remove in response to disconnect notification
        //->Pdo_Deleted;

        if (UsbhOnPdoAbsentList(pdoExt->HubFdo, Pdo, lock)) {
            RemoveEntryList(&pdoExt->PdoListLink);
            pdoExt->PdoListLink.Flink = NULL;
            pdoExt->PdoListLink.Blink = NULL;
        }

        UsbhDeletePdo(HubFdo, Pdo, curState, PdoEv_PdoRemove, StateContext);
        break;

    default:
        // this means we are disconnecting twice
        UsbhAssertFailure(HubFdo, Pdo);
        UsbhReleasePdoStateLock(HubFdo, StateContext);
        break;
    }

}


VOID
Usbh_Disconnect_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    HUB_PDO_HWPNP_EVENT PdoHwPnpEvent,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Handles PdoEv_Disconnect, PdoEv_PceDisconnect

Irql: ANY

Arguments:

    Pdo to be disconnected, the PDO must not be latched since the deletion
    routine waits on any outstanding requests.

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    HUB_PDO_HWPNP_STATE curState;

    UsbhAcquirePdoStateLock(HubFdo, StateContext, PdoHwPnpEvent);

    LOG(HubFdo, LOG_PNP, 'fSRv', 0, StateContext);

    hubExt = FdoExt(HubFdo);

    if (PdoExt(Pdo)->PdoFlags.SerialNumber) {
        USBD_MarkDeviceAsDisconnected(Pdo);
    }

    curState = GET_PDO_HWPNPSTATE(Pdo);
    switch(curState) {
        // device disconnected while we were initializing the PDO
    case Pdo_Created:
    case Pdo_HwPresent:
        // pnp does not know about it yet so drivers don't know about it either
        // we can delete it
        //-> Pdo_Deleted;
        UsbhDeletePdo(HubFdo, Pdo, curState ,PdoHwPnpEvent, StateContext);
        break;

    case Pdo_PnpRefHwPresent:
        // tell pnp it is gone
        //-> Pdo_PnpRefHwAbsent;
        UsbhDeregisterPdo(HubFdo, Pdo, curState, PdoHwPnpEvent, StateContext);
        break;

    case Pdo_Deleted:
    case Pdo_HwAbsent:
    default:
        // this means we are disconnecting twice
        UsbhAssertFailure(HubFdo, Pdo);
        UsbhReleasePdoStateLock(HubFdo, StateContext);
        break;
    }

}


VOID
Usbh_ErrorDisconnect_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Handles PdoEv_ErrorDisconnect

Irql: ANY

Arguments:

Return Value:

    none

--*/
{
    HUB_PDO_HWPNP_STATE curState;

    UsbhAcquirePdoStateLock(HubFdo, StateContext, PdoEv_ErrorDisconnect);

    LOG(HubFdo, LOG_PNP, 'fSRv', 0, StateContext);

    curState = GET_PDO_HWPNPSTATE(Pdo);
    switch(curState) {
        // device disconnected while we were initializing the PDO
    case Pdo_Created:
        // pnp does not know about it yet so drivers don't know about it either
        // we can delete it
        //-> Pdo_Deleted;
        UsbhDeletePdo(HubFdo, Pdo, curState , PdoEv_ErrorDisconnect, StateContext);
        break;

    default:
        // this means we are disconnecting twice
        UsbhAssertFailure(HubFdo, Pdo);
        UsbhReleasePdoStateLock(HubFdo, StateContext);
        break;
    }

}


VOID
Usbh_BusPause_PdoEvent(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Handles PdoEv_BusPause

Irql: ANY

Arguments:

Return Value:

    none

--*/
{
    HUB_PDO_HWPNP_STATE curState;

    //
    // Insure there are no pending driver reset requests
    // in the port change queue.  Driver reset requests
    // latch the PDO, which will cause UsbhDeletePdo to
    // block if it is called.
    //
    UsbhFlushQueuedDriverReset(HubFdo, PortData);

    UsbhAcquirePdoStateLock(HubFdo, StateContext, PdoEv_BusPause);

    LOG(HubFdo, LOG_PNP, 'fSRv', 0, StateContext);

    curState = GET_PDO_HWPNPSTATE(Pdo);
    switch(curState) {
        // device disconnected while we were initializing the PDO
    case Pdo_Created:
        DbgTrace((UsbhDebugTrace, "disconnect zombie PDO (PAUSE) %p\n", Pdo));
        LOG(HubFdo, LOG_BUS, 'dscO', Pdo, StateContext);
        UsbhDeletePdo(HubFdo, Pdo, curState, PdoEv_BusPause, StateContext);
        break;

    case Pdo_HwPresent:
        TEST_TRAP();
    case Pdo_PnpRefHwPresent:
    case Pdo_PnpRefHwAbsent:
        SET_PDO_HWPNPSTATE(Pdo, curState, PdoEv_BusPause);
        UsbhReleasePdoStateLock(HubFdo, StateContext);

        // if there is a driver reset pending we need to signal this as well
        UsbhSignalSyncDeviceReset(HubFdo, Pdo, PortData, STATUS_UNSUCCESSFUL);
        break;

    case Pdo_HwAbsent:
    case Pdo_Deleted:
    default:
        // this means we are disconnecting twice
        UsbhAssertFailure(HubFdo, Pdo);
        UsbhReleasePdoStateLock(HubFdo, StateContext);
        break;
    }

}


BOOLEAN
UsbhOnPdoPresentList(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHGSL Lock
    )
/*++

Routine Description:

    Look for PDO on the 'present list'

Irql:

Arguments:

Return Value:

    returns TRUE if the pdo is on the 'present list'. we use this function for validation

--*/
{

    PDEVICE_EXTENSION_HUB hubExt;

    UsbhAssert(HubFdo, Lock->Locked);

    hubExt = FdoExt(HubFdo);

    return (UsbhFindListPdo(HubFdo, Pdo, &hubExt->PdoPresentListHead, Lock));
}


BOOLEAN
UsbhOnPdoRemovedList(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHGSL Lock
    )
/*++

Routine Description:

    Look for PDO on the 'removed list'

Irql:

Arguments:

Return Value:

    returns TRUE if the pdo is on the 'removed list'. we use this function for validation

--*/
{

    PDEVICE_EXTENSION_HUB hubExt;

    UsbhAssert(HubFdo, Lock->Locked);

    hubExt = FdoExt(HubFdo);

    return (UsbhFindListPdo(HubFdo, Pdo, &hubExt->PdoRemovedListHead, Lock));
}

BOOLEAN
UsbhOnPdoAbsentList(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHGSL Lock
    )
/*++

Routine Description:

    Look for PDO on the 'absent list'

Irql:

Arguments:

Return Value:

    returns TRUE if the pdo is on the 'absent list'.

--*/
{

    PDEVICE_EXTENSION_HUB hubExt;

    UsbhAssert(HubFdo, Lock->Locked);

    hubExt = FdoExt(HubFdo);

    return (UsbhFindListPdo(HubFdo, Pdo, &hubExt->PdoAbsentListHead, Lock));
}

BOOLEAN
UsbhFindListPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PLIST_ENTRY ListHead,
    PHGSL Lock
    )
/*++

Routine Description:

    Look for PDO on the 'present' or 'remove' list

Irql:

Arguments:

Return Value:

    returns TRUE if the pdo is on the list. we use this function for validation

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PLIST_ENTRY le;

    UsbhAssert(HubFdo, Lock->Locked);

    pdoExt = PdoExt(Pdo);

    // walk the list
    pdoExt = NULL;

    for(le = ListHead->Flink;
       le != ListHead;
       le = le->Flink) {

       pdoExt = (PDEVICE_EXTENSION_PDO) CONTAINING_RECORD(
                   le,
                   struct _DEVICE_EXTENSION_PDO,
                   PdoListLink);

       LOG(HubFdo, LOG_BUS, 'drg1', pdoExt, 0);

       ASSERT_PDOEXT(pdoExt);

       if (Pdo == pdoExt->Pdo) {
           return TRUE;
       }
       pdoExt = NULL;
    }

    return FALSE;
}


VOID
UsbhDeregisterPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    HUB_PDO_HWPNP_STATE PdoState,
    HUB_PDO_HWPNP_EVENT PnpEvent,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Deregister a PDO with PnP.  Tell pnp a device is disconnected

    This function removes any PnP references to a PDO. That is, it tells PnP a
    PDO is physically gone from the bus.

    It also drops any references the hub has in the port data structures.

    Transitions to Pdo_PnpRefHwAbsent


Irql: PASSIVE, after lock is dropped

Arguments:

Return Value:

    returns the lock state

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    PHUB_PORT_DATA pd;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    //
    // If device is in boot/hiber/crash/paging path then do not deregister
    // the PDO so that the device doesn't get reenum'ed by PnP
    //
    if (pdoExt->IsBootDevice) {
        UsbhReleasePdoStateLock(HubFdo, StateContext);
        return;
    }

    pd = UsbhGetPortData(HubFdo, pdoExt->PortNumber);
    if (!pd) {
        UsbhAssertFailure(HubFdo, Pdo);
        UsbhReleasePdoStateLock(HubFdo, StateContext);
        return;
    }

    UsbhAssert(HubFdo, pd->PnpPdo == Pdo);

    // see if the PDO is on the PdoPresentList
    LOG(HubFdo, LOG_BUS, 'drgP', pd, Pdo);

    UsbhAssert(HubFdo, PdoState == Pdo_PnpRefHwPresent);
    UsbhAssert(HubFdo, UsbhOnPdoPresentList(HubFdo, Pdo, &StateContext->gLock));

    // We need to tell PnP that the device is gone.  Remove the device
    // from the 'present' list and put it on the removed list. We will
    // clear the PnP ref flag on the next QBR.

    RemoveEntryList(&pdoExt->PdoListLink);
    pdoExt->PdoListLink.Flink = NULL;
    pdoExt->PdoListLink.Blink = NULL;

    LOG(HubFdo, LOG_BUS, 'drg2', pdoExt, pdoExt->PdoFlags.ul);
    // PortPata->Pdo = NULL
    UsbhResetPortData(HubFdo, Pdo, &StateContext->gLock);

    // if the device is present check to see if we need to tell PnP
    // it was removed

    // tell pnp it is gone

    InsertTailList(&hubExt->PdoRemovedListHead,
                   &pdoExt->PdoListLink);

    SET_PDO_HWPNPSTATE(Pdo, Pdo_PnpRefHwAbsent, PnpEvent);

    //
    // no more io will be processed for this Pdo, start failing as soon as we possibly can.
    //
    // NOTE: setting the flag here will cause Io to be failed when we disconnect the device,
    // this is before Pnp is notified.  We will report NT_DEVICE_DISCONNECTED_STATUS but it is
    // possible the client driver could go into a loop submitting requests.  The alternative
    // is to pas the request into USBPORT.SYS where it will be queued on the 'BadRequestList'
    // until the Pnp REMOVE arrives.
    //
    // If the request is passed thru to USBPORT in this window we risk the driver unloading or freeing
    // the urb or Irp while the irp is pending. This will result in a 'bugcheck 0x44, 0xfe etc' in USBPORT
    // and is very difficult to diagnose.
    //
    UsbhSetPdo_FailIo(Pdo);

    hubExt->BusRelationsState = BusRelationsInvalid;
    UsbhReleasePdoStateLock(HubFdo, StateContext);

    UsbhSignalSyncDeviceReset(HubFdo, Pdo, pd, NT_DEVICE_DISCONNECTED_STATUS);

    // Tell SSH that the port is idle
    UsbhSshSetPortsBusyState(HubFdo, pdoExt->PortNumber, PORT_IDLE, NO_RESUME_HUB);

    // remove any device handle now since the device is not physically
    // present anymore.
    UsbhUnlinkPdoDeviceHandle(HubFdo, Pdo, 'xDH3', TRUE);

    // trigger an invalidate to delete Pdos

    DbgTrace((UsbhDebugTrace, "IoInvalidateDeviceRelations (remove PDO)\n"));
    UsbhBusIoInvalidateDeviceRelations(HubFdo, StateContext);

    return;
}


VOID
UsbhSoftDisconnectPdo(
    PDEVICE_OBJECT HubFdo,
    HUB_PDO_HWPNP_EVENT PnpEvent,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Similar to Deregister only Pnp is telling us the pdo is disconnected.

    pnp will send a remove to this PDO but we must break the connection to the bus


Irql: PASSIVE, after lock is dropped

Arguments:

Return Value:

    returns the lock state

--*/
{

    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    PHUB_PORT_DATA pd;
    PHGSL lock;
    HUB_PDO_HWPNP_STATE curState;
    PDEVICE_OBJECT pdo = (PDEVICE_OBJECT) StateContext->Parent;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(pdo);

    LOG(HubFdo, LOG_PNP, 'PSU!', 0, StateContext);

    pd = UsbhGetPortData(HubFdo, pdoExt->PortNumber);
    if (!pd) {
        UsbhAssertFailure(HubFdo, pdo);
        return;
    }

    UsbhCancelResetTimeout(HubFdo, pd, CANCEL_RESET_IN_PROGRESS);
    UsbhSignalSyncDeviceReset(HubFdo, pdo, pd, STATUS_UNSUCCESSFUL);

    // use pdo context for these
    UsbhAcquirePdoStateLock(HubFdo, StateContext, PnpEvent);

    lock = &StateContext->gLock;
    UsbhAssert(HubFdo, lock->Locked);

    //
    // We could be in any pnp pdo hw state (state can change before we take the lock)
    // at this point so we need to handle the case where we have reported the hardware
    // gone as well as the case where we have not.
    //

    LOG(HubFdo, LOG_BUS, 'drgP', pd, pdo);
    curState = GET_PDO_HWPNPSTATE(pdo);

    switch(curState) {
    case Pdo_PnpRefHwPresent:

        UsbhAssert(HubFdo, pd->PnpPdo == pdo);
        // see if the PDO is on the PdoPresentList
        UsbhAssert(HubFdo, UsbhOnPdoPresentList(HubFdo, pdo, &StateContext->gLock));
#if 0
        // Remove the pdo from our internal list
        RemoveEntryList(&pdoExt->PdoListLink);
        //pdoExt->PdoListLink.Flink = NULL;
        //pdoExt->PdoListLink.Blink = NULL;
        // this will allow the hub to clena up on a remove
        InsertTailList(&hubExt->PdoRemovedListHead,
                       &pdoExt->PdoListLink);



        LOG(HubFdo, LOG_BUS, 'drg2', pdoExt, pdoExt->PdoFlags.ul);

        UsbhResetPortData(HubFdo, pdo, &StateContext->gLock);

        // pnp knows it is gone and will remove it
        SET_PDO_HWPNPSTATE(pdo, Pdo_PnpRefHwAbsent, PnpEvent);

        UsbhReleasePdoStateLock(HubFdo, StateContext);

        // remove any device handle now since the device is not physically
        // present anymore.
        dh = InterlockedExchangePointer(&pdoExt->DeviceHandle,
                                        InvalidDeviceHandle);

        Usbh_HubRemoveUsbDevice(HubFdo, dh, 0);
#endif
        // this is a surprise remove that is not a remove since the PDO is
        // physically present we must retain it.

       SET_PDO_HWPNPSTATE(pdo, Pdo_PnpRefHwPresent, PnpEvent);

        UsbhReleasePdoStateLock(HubFdo, StateContext);

       // we may need to release the device handle here in case the port driver is being unloaded.
//
//        Usbh_HubRemoveUsbDevice(HubFdo, dh, 0);
        break;

    case Pdo_PnpRefHwAbsent:
        //test #113

        // we have already disconnected it
        UsbhAssert(HubFdo, pd->PnpPdo != pdo);
        // see if the PDO is on the PdoPresentList
        UsbhAssert(HubFdo, UsbhOnPdoRemovedList(HubFdo, pdo, &StateContext->gLock));

        // Remove the pdo from our internal list
        //RemoveEntryList(&pdoExt->PdoListLink);
        //pdoExt->PdoListLink.Flink = NULL;
        //pdoExt->PdoListLink.Blink = NULL;

        LOG(HubFdo, LOG_BUS, 'drg3', pdoExt, pdoExt->PdoFlags.ul);

        // pnp knows it is gone and will remove it
        SET_PDO_HWPNPSTATE(pdo, Pdo_PnpRefHwAbsent, PnpEvent);

        UsbhReleasePdoStateLock(HubFdo, StateContext);

        break;
    default:
        UsbhAssertFailure(HubFdo, pdo);
        UsbhReleasePdoStateLock(HubFdo, StateContext);
    }
    return;
}


/**
    Enumeration Functions

    Enumeration Process (based on Wn9x, Win2k and WinNT5.1(XP))

Enumeration Flow
    *= exception code for failure

CONNECT->

try=0;
if(PDO=CreatePDO)

    wait(100)  // port power stabilization
2:  try++
1:  if(reset port)

        if (createdevice)
            if (reset_port)
                if (try > 1)
                   wait(100) //was 50
                endif
                if (initdevice)
                    //device is addressed
                    if (setupdevice)
                        disable_port
                        WMI_Event - DeviceFailedEnumeration
                                    *SetupDeviceFailure
                        PDO = Unknown Device in devman
                    else
                        PDO = Device
                        STATUS_SUCCESS
                    endif
                else
                    disable_port
                    WMI_Event - DeviceFailedEnumeration
                                *InitializeDeviceFailure
                    PDO = Unknown Device in devman
                endif
            else
                if (try <3 && connect == 1)
                   goto 3
                else if (connect == 1)
                    disable_port
                    WMI_Event - DeviceFailedEnumeration
                                *Reset2Failure
                    PDO = Unknown Device in devman
                else
                    Delete(PDO)
                endif
            endif
        else
3:
            try++
            wait(500)

            if (try < 3)
                if (connect == 1)
                    goto 1
                else
                    Delete(PDO)
                endif
            else
                if (connect == 1)
                    disable_port
                    WMI_Event - DeviceFailedEnumeration
                                *CreateDeviceFailure
                    PDO = Unknown Device in devman
                else
                    Delete(PDO)
                endif
            endif

        endif
    else
        if (connect == 1)
            if (try < 3)
                goto 2
            else
               Disable_Port
               WMI_Event - DeviceFailedEnumeration
                           *Reset1Failure
               PDO = Unknown Device in devman
            endif
        else
            Delete(PDO)
        endif
    endif

else
    if (connect == 1)
        Disable_Port
        WMI_Event - DeviceFailedEnumeration
                    *CreatePdoFailure
        PDO = NULL, no device in devman
    endif
endif


**/



VOID
UsbhAssertBusLock(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber
    )
/*++

Routine Description:

    Verify that this FDO holds the bus lock

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);
    KeAcquireSpinLock(&hubExt->UsbEnumLockSpin, &irql);

    UsbhAssert(HubFdo, hubExt->UsbEnumLockPort == PortNumber);

    KeReleaseSpinLock(&hubExt->UsbEnumLockSpin, irql);

}


VOID
UsbhAcquireEnumBusLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    USHORT PortNumber
    )
/*++

Routine Description:

    Synchronously acquires the bus lock or waits until it is free.

    The bus enumeration lock is instanced per USB host controller and can only be held by
    one process at a time.  It is used to serialize the enumeration process
    for each controller since only one device may be at address 0 at a time.

Irql: PASSIVE

Arguments:

    Sc -- context acquiring the lock

    PortNumber -- the port number of the hub associated with the thread
    acquiring the lock.

Return Value:

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_HUB_BUS_INTERFACE bus;
    KIRQL irql;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    bus = &hubExt->HubBusInterface;

    LOG(HubFdo, LOG_BUS, 'aBUS', hubExt->UsbEnumLockPort, PortNumber);
    LOG(HubFdo, LOG_BUS, 'aBLC', hubExt->UsbEnumLockPort, Sc);
    if (!bus->AcquireBusSemaphore) {
        // very bad
        UsbhAssertMsg(HubFdo, "AcquireBusSemaphore function should not be NULL", FALSE);
    } else {

        KeAcquireSpinLock(&hubExt->UsbEnumLockSpin, &irql);
        // if lock is already held skip this
        if (hubExt->UsbEnumLockPort != PortNumber &&
            hubExt->UsbEnumLockContext != Sc) {

            KeReleaseSpinLock(&hubExt->UsbEnumLockSpin, irql);
            Usbh_SSH_Event(HubFdo, SSH_Event_ResumeHub, Sc);

            // there's a cookie! grab it!!
            bus->AcquireBusSemaphore(bus->BusContext);
            LOG(HubFdo, LOG_BUS, 'lBUS', hubExt->UsbEnumLockPort, PortNumber);

            UsbhAssert(HubFdo, hubExt->UsbEnumLockPort == 0);
            UsbhAssert(HubFdo, hubExt->UsbEnumLockContext == NULL);

            // keep track of which one of our ports holds the lock
            // for debugging
            hubExt->UsbEnumLockPort = PortNumber;
            hubExt->UsbEnumLockContext = Sc;
            Sc->ScThread = KeGetCurrentThread();

            InterlockedIncrement(&hubExt->UsbEnumerationLock);

            hubExt->EnumLockSshHandle = UsbhIncHubBusy(HubFdo, Sc, HubFdo, UsbhBusyBusLock_Tag, RESUME_HUB);
       } else {
            // already held
            LOG(HubFdo, LOG_BUS, 'xBUS', hubExt->UsbEnumLockPort, PortNumber);
            KeReleaseSpinLock(&hubExt->UsbEnumLockSpin, irql);
       }
    }
}


VOID
UsbhReleaseEnumBusLockEx(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    USHORT PortNumber,
    BOOLEAN AssertLockOwned
    )
/*++

Routine Description:

    Releases the bus lock allowing another port to enumerate

Irql: PASSIVE

Arguments:

    Sc -- context of lock owner.

    PortNumber -- the port number of this hub associated with the thread
    releasing the lock.

    AssertLockOwned -- flag indicated that the caller is exoected to won the lock

Return Value:

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_HUB_BUS_INTERFACE bus;
    KIRQL irql;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    bus = &hubExt->HubBusInterface;

    LOG(HubFdo, LOG_BUS, 'uBUS', hubExt->UsbEnumLockPort, PortNumber);

    if (!bus->ReleaseBusSemaphore) {
        // very bad
        UsbhAssertMsg(HubFdo, "ReleaseBusSemaphore function should not be NULL", FALSE);
    } else {
        LONG tmp;

        // context and port number should match with caller that is releasing the lock.
        KeAcquireSpinLock(&hubExt->UsbEnumLockSpin, &irql);
        if (AssertLockOwned) {
            UsbhAssert(HubFdo, hubExt->UsbEnumLockPort == PortNumber);
            UsbhAssert(HubFdo, hubExt->UsbEnumLockContext == Sc);
        }

        if (hubExt->UsbEnumLockPort == PortNumber &&
            hubExt->UsbEnumLockContext == Sc) {

            KeReleaseSpinLock(&hubExt->UsbEnumLockSpin, irql);
            UsbhDecHubBusy(HubFdo, Sc, hubExt->EnumLockSshHandle);
            hubExt->EnumLockSshHandle = NULL;

            hubExt->UsbEnumLockPort = 0;
            hubExt->UsbEnumLockContext = NULL;

            tmp = InterlockedDecrement(&hubExt->UsbEnumerationLock);
            UsbhAssert(HubFdo, tmp >= 0);
            bus->ReleaseBusSemaphore(bus->BusContext);
        } else {
            // lock not owned by this context
            LOG(HubFdo, LOG_BUS, 'uBSx', hubExt->UsbEnumLockPort, hubExt->UsbEnumLockContext);
            KeReleaseSpinLock(&hubExt->UsbEnumLockSpin, irql);
        }

    }
}

VOID
UsbhReleaseEnumBusLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    USHORT PortNumber
    )
{
    // this routine is called if we expect the lock to be owned by the caller
    UsbhReleaseEnumBusLockEx(HubFdo, Sc, PortNumber, TRUE);
}

NTSTATUS
UsbhCreateDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USHORT PortStatus,
    USHORT PortNumber
    )
/*++

Routine Description:

    This function creates a device on the USB. The device is not addressed on
    return from this function.  This function fetches the device descriptor and
    sets the max packet size for the default pipe to the correct value.

Arguments:

    DeviceData - the 'device handle' for this device

    PortStatus - current status bits for the port this device is on, these tell
        us the speed of the device

    PortNumber - physical port the device is attached to, this is also the tt
        number in the case of a USB 2.0 hub

Return Value:

    NT status code.

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    PUSB_HUB_BUS_INTERFACE bus;
    USB_CD_ERROR_INFORMATION cdErrorInfo;
    PUSB_DEVICE_HANDLE hubDeviceHandle = NULL;
    // we use a tmp value because the port driver could out some
    // random value here on an error
    PUSB_DEVICE_HANDLE newDeviceHandle;
    USHORT ttPortNumber = 0;
    USB_HUB_HW_PORT_STATUS ps;

    LOG(HubFdo, LOG_BUS, 'creD', PortStatus, PortNumber);
    ps.us = PortStatus;
    hubExt = FdoExt(HubFdo);
    bus = &hubExt->HubBusInterface;

    pdoExt = PdoExt(Pdo);

    // initialize return value to invalid (it should already be set)
    newDeviceHandle = UsbhInvalidDeviceHandle;

    if (ps.HighSpeed) {

        hubDeviceHandle = NULL;

    } else {
        // Hs with LS/FS device, the tt device handle will be ours
        if (hubExt->FdoFlags.HubIsTT) {

            LOG(HubFdo, LOG_BUS, 'crd1', PortStatus, PortNumber);
            hubDeviceHandle = UsbhGetDeviceHandle(HubFdo, HubFdo);
            ttPortNumber = PortNumber;

            LOG(HubFdo, LOG_BUS, 'crd2', hubDeviceHandle, PortNumber);

            if (hubDeviceHandle == NULL) {
                nts = STATUS_UNSUCCESSFUL;
                HUB_EXCEPTION(USBFILE, HubFdo, PortNumber, CreateDeviceFailure_NoHubHandle,
                            nts, 0xFFFFFFFF, NULL, 0);
                return nts;
            }

        } else {

            // Tt handle may be owned by a parent of this hub or we may be on a 1.1 bus
            // (with no TT), the only way to tell is to call down the chain of hubs

            LOG(HubFdo, LOG_BUS, 'crd3', PortStatus, PortNumber);
            nts = UsbhGetTtDeviceHandle(HubFdo, &hubDeviceHandle, HubFdo, &ttPortNumber);

            LOG(HubFdo, LOG_BUS, 'crd4', hubDeviceHandle, ttPortNumber);
            // if this API fails we cannot tell if there is a TT or not. so we cannot
            // enumerate the device.
            // The API may return success with a NULL handle which is valid if
            // there is no TT.
            if (NT_ERROR(nts)) {
                HUB_EXCEPTION(USBFILE, HubFdo, PortNumber, PnpErr_CreateDeviceFailure_GetTtFailed,
                            nts, 0xFFFFFFFF, NULL, 0);
                return nts;
            }
        }
    }

    // hub handle will be NULL is this is a 1.1 bus or nate 2.0 device

    if (!bus->CreateUsbDevice) {
        UsbhAssertMsg(HubFdo, "CreateUsbDevice function should not be NULL", FALSE);
        nts =  STATUS_NOT_IMPLEMENTED;
        if (hubDeviceHandle) {
            Usbh_HubDerefDeviceHandle(HubFdo, hubDeviceHandle, HubFdo, Devh_ReturnDH_Tag);
        }
    } else {
        cdErrorInfo.Version = CD_ERR_V1;

        if (bus->Version >= USB_BUSIF_HUB_VERSION_7) {

            PUSB_PORT_PATH  portPath;
            ULONG           depth;

            portPath = &pdoExt->PortPath;

            // Convert the hub extension TopologyAddress to a
            // USB_PORT_PATH for the device being created.
            //
            // PortPath[0] = TopologyAddress.RootHubPortNumber
            // PortPath[1] = TopologyAddress.HubPortNumber[0]
            // PortPath[2] = TopologyAddress.HubPortNumber[1]
            // PortPath[3] = TopologyAddress.HubPortNumber[2]
            // PortPath[4] = TopologyAddress.HubPortNumber[3]
            // PortPath[5] = TopologyAddress.HubPortNumber[4] *
            //
            // * At this point HubPortNumber[4] must be zero.

            portPath->PortPath[0] = hubExt->TopologyAddress.RootHubPortNumber;

            for (depth = 1; depth < 6; depth++)
            {
                portPath->PortPath[depth] = 0;
            }

            if (hubExt->TopologyAddress.RootHubPortNumber != 0) {
                for (depth = 1; depth < 5; depth++) {
                    if (hubExt->TopologyAddress.HubPortNumber[depth - 1] == 0) {
                        break;
                    } else {
                        portPath->PortPath[depth] =
                            hubExt->TopologyAddress.HubPortNumber[depth - 1];
                    }
                }
            } else {
                depth = 0;
            }

            // At this point either depth == 5, or depth is the 0-based
            // index of the first empty (zero) element of PortPath[].
            // Store the PortNumber of the new device at that index.
            //
            portPath->PortPathDepth = depth + 1;
            portPath->PortPath[depth] = PortNumber;

            nts = bus->CreateUsbDeviceV7(bus->BusContext,
                                         &newDeviceHandle,
                                         hubDeviceHandle,
                                         PortStatus,
                                         portPath,
                                         &cdErrorInfo,
                                         ttPortNumber,
                                         Pdo,
                                         &pdoExt->PhysicalDeviceObjectName);
        } else {

            nts = bus->CreateUsbDevice(bus->BusContext,
                                       &newDeviceHandle,
                                       hubDeviceHandle,
                                       PortStatus,
                                       PortNumber,
                                       &cdErrorInfo,
                                       ttPortNumber);
        }

        if (hubDeviceHandle) {
            Usbh_HubDerefDeviceHandle(HubFdo, hubDeviceHandle, HubFdo, Devh_ReturnDH_Tag);
        }

        if (NT_ERROR(nts)) {

            if (cdErrorInfo.PathError == CreateDevFailedGetDs) {
                if (cdErrorInfo.UlongArg1 == 0) {
                    pdoExt->EnumFailureMsgId = USBENUM_DEVICE_DESCRIPTOR_FAILURE;
                } else {
                    pdoExt->EnumFailureMsgId = USBENUM_BAD_DEVICE_DESCRIPTOR;
                }
            }

            LOG(HubFdo, LOG_BUS, 'cdf!', PortStatus, PortNumber);
            // throw an exception so we know why create device failed
            HUB_EXCEPTION(USBFILE, HubFdo, PortNumber, CreateDeviceFailure,
                      nts, 0xFFFFFFFF, &cdErrorInfo,
                      sizeof(struct  _USB_CD_ERROR_INFORMATION));

            UsbhEtwLogHubEventWithExtraData(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_CREATE_DEVICE_FAILURE, PortNumber, &cdErrorInfo, sizeof(cdErrorInfo));

            newDeviceHandle = UsbhInvalidDeviceHandle;
        }
    }

    if (NT_SUCCESS(nts)) {

        // link it to the PDO
        nts = UsbhLinkPdoDeviceHandle(HubFdo, Pdo, newDeviceHandle);

        if (NT_ERROR(nts)) {
            LOG(HubFdo, LOG_BUS, 'lnk!', newDeviceHandle, PortNumber);

            // we'll need to delete the handle now since we cannot track it
            Usbh_HubRemoveUsbDevice(HubFdo, newDeviceHandle, 0);
            newDeviceHandle = UsbhInvalidDeviceHandle;
        }

    } else {
        // test #98
        // test 98 will hand back a bogus value with an error,
        // you can uncomment this assert to track problems in the real port driver.
        // UsbhAssert(HubFdo, newDeviceHandle == InvalidDeviceHandle);
        UsbhAssert(HubFdo, newDeviceHandle == UsbhInvalidDeviceHandle);
    }

    return nts;
}


NTSTATUS
UsbhInitializeDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHUB_PORT_DATA PortData
    )
/*++

Routine Description:

   This function initializes a device (DeviceHandle) created by a call to
   UsbhCreateDevice.

   This function assigns the address to the USB device

Arguments:

    DeviceData - the 'device handle' for this device

Return Value:

    NT status code.

--*/
{
    NTSTATUS nts;
    USB_ID_ERROR_INFORMATION idErrorInfo;
    PSTATE_CONTEXT sc;
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = PdoExt(Pdo);
    sc = PNP_CONTEXT(HubFdo);

    LOG(HubFdo, LOG_BUS, 'iniD', pdoExt->DeviceHandle, PortData->PortNumber);

    RtlZeroMemory(&idErrorInfo, sizeof(struct _USB_ID_ERROR_INFORMATION));

    // Kingston Datatraveler 4GB will hang during enumeration if it sees a split transaction
    // for another device's control endpoint.  Delay here to mimic slower Windows 7 enumeration
    // timing, which masks this problem.  We only do this on WinPE, in case the boot device for
    // setup is the Kingston device.
    if (FdoExt(HubFdo)->FdoFlags.WinPE == 1) {
        UsbhWait(HubFdo, 50);
    }

    idErrorInfo.Version = ID_ERR_V1;
    nts = Usbh_HubInitializeDevice(HubFdo,
                                   Pdo,
                                   &idErrorInfo);

    if (NT_ERROR(nts)) {

        switch (idErrorInfo.PathError) {
        case InitDevFailedSetAddress:
            pdoExt->EnumFailureMsgId = USBENUM_SET_ADDRESS_FAILURE;
            break;

        case InitDevBadDeviceDescriptor:
            if (idErrorInfo.Arg1 == 0) {
                pdoExt->EnumFailureMsgId = USBENUM_DEVICE_DESCRIPTOR_FAILURE;
            } else {
                pdoExt->EnumFailureMsgId = USBENUM_BAD_DEVICE_DESCRIPTOR;
            }
            break;
        }

        // USBPORT frees the device handle on failure
        // drop the linkage
        UsbhUnlinkPdoDeviceHandle(HubFdo, Pdo, 'xDH8', FALSE);

        // throw an exception so we know why create device failed
        // the calling function is responsible for generating any
        // popup messages
        HUB_EXCEPTION(USBFILE, HubFdo, PortData->PortNumber, InitializeDeviceFailure,
                  nts, idErrorInfo.UsbdStatus, &idErrorInfo,
                  sizeof(struct _USB_ID_ERROR_INFORMATION));

        UsbhEtwLogHubEventWithExtraData(HubFdo, &USBHUB_ETW_EVENT_HUB_ENUM_INITIALIZE_DEVICE_FAILURE, PN(PortData), &idErrorInfo, sizeof(idErrorInfo));

        UsbhClearTt(HubFdo, PortData, (USHORT)idErrorInfo.UsbAddress);
    }

    pdoExt->PdoFlags.MsOs20AltEnumCmdSent = FALSE;

    return nts;
}


VOID
UsbhSignalSyncDeviceResetPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    NTSTATUS NtStatus
    )
/*++

Routine Description:

    Signal the device reset event if we have one.  This is a
    device level reenumeration not just a port reset.

Irql: ANY

Arguments:

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL reset_irql;

    hubExt = FdoExt(HubFdo);

    KeAcquireSpinLock(&hubExt->SyncDeviceResetSpin, &reset_irql);
    if (Pdo && PdoExt(Pdo)->SyncDeviceResetEvent) {

        PdoExt(Pdo)->SyncDeviceResetStatus = NtStatus;

        LOG(HubFdo, LOG_BUS, 'sigE', Pdo, 0);
        SET_EVENT(PdoExt(Pdo)->SyncDeviceResetEvent);
    }
    KeReleaseSpinLock(&hubExt->SyncDeviceResetSpin, reset_irql);

}


VOID
UsbhSignalSyncDeviceReset(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHUB_PORT_DATA PortData,
    NTSTATUS NtStatus
    )
/*++

Routine Description:

    Signal the device reset event if we have one.  This is a
    device level reenumeration not just a port reset.

Irql: ANY

Arguments:

--*/
{
    UsbhSignalSyncDeviceResetPdo(HubFdo, Pdo, NtStatus);

    UsbhSignalDriverResetEvent(HubFdo, PortData, FALSE);
}


VOID
Usbh_iSignalSyncDeviceReset(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHUB_PORT_DATA PortData,
    NTSTATUS NtStatus
    )
/*++

Routine Description:

    Signal the device reset event if we have one.  This is a
    device level reenumeration not just a port reset.

    This version is used if the port chnage list lock is already held.

Irql: ANY

Arguments:

--*/
{
    UsbhSignalSyncDeviceResetPdo(HubFdo, Pdo, NtStatus);

    Usbh_iSignalDriverResetEvent(HubFdo, PortData, FALSE);
}


NTSTATUS
UsbhSyncResetDeviceInternal(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Given a port with an attached PDO, reset the associated USB device.

    The device is recreated ie we re-enumerated it but not in the context of PnP so the
    PDO is still valid.

    This is a synchronous function that runs at passive level.  This is the internal
    function used by the hub driver power and enumeration code to reset a device that
    may already have a driver loaded.

    This is also used to service driver requests for reset. Such requests are submitted in
    the pdo context.

    The PDO we are trying to restore is attached to the reset request.  The embedded event
    SyncDeviceResetEvent the pdoExt is Reset and signalled when the reset is completed.

Irql: PASSIVE

Arguments:

    HubFdo - Hub we are dealing with.

    Sc - state context for the request -- this is usually the PDO context.

    Pdo - Pdo to reset the port for, pdo should be latched.

Return Value:

    This function returns an error if the device currently attached is
    different from the one associated with the PDO or if we cannot reset the
    device.

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    KEVENT event;
    USB_HUB_PORT_EVENT pc;
    NTSTATUS nts;
    PUSB_DEVICE_HANDLE newDh, oldDh;
    PVOID curSyncResetEvent;
    KIRQL reset_irql;
    PHUB_PORT_DATA pd;
    BOOLEAN wrongBootDevice = FALSE;

    pdoExt = PdoExt(Pdo);

    DbgTrace((UsbhDebugTrace,"%!FUNC!\n"));
    DbgTrace((UsbhDebugTrace, "SyncReset USB device Port %d Pdo %p\n", pdoExt->PortNumber, Pdo));

    // NOTE:
    // the hub port is still servicing events and the device may detect a disconnect
    // during this process.


//TEST_TRAP();

    hubExt = FdoExt(HubFdo);

    UsbhEtwLogDeviceEvent(pdoExt, &USBHUB_ETW_EVENT_DEVICE_SYNC_RESET_DEVICE_INTERNAL_START, 0);

    KeAcquireSpinLock(&hubExt->SyncDeviceResetSpin, &reset_irql);
    INIT_EVENT_NOT_SIGNALED(&event);
    // see if we already servicing a driver reset, we can only service one at a time
    // per port.

    // inReset = pd->SyncDeviceResetEvent
    curSyncResetEvent = pdoExt->SyncDeviceResetEvent;
    if (curSyncResetEvent == NULL) {
         // this event may be signaled any time after we release the lock
         pdoExt->SyncDeviceResetEvent = &event;
         pdoExt->SyncDeviceResetStatus = STATUS_BOGUS;
         pdoExt->SyncDeviceResetThread = KeGetCurrentThread();
         KeReleaseSpinLock(&hubExt->SyncDeviceResetSpin, reset_irql);
    } else {
        // we are currently servicing a reset request on this port
        LOG(HubFdo, LOG_BUS, 'RDe4', 0, curSyncResetEvent);
        KeReleaseSpinLock(&hubExt->SyncDeviceResetSpin, reset_irql);
        UsbhEtwLogDeviceEvent(pdoExt, &USBHUB_ETW_EVENT_DEVICE_SYNC_RESET_DEVICE_INTERNAL_COMPLETE, STATUS_DEVICE_BUSY);
        return STATUS_DEVICE_BUSY;
    }

    // ref the original handle
    // this ref will prevent it from being removed
    oldDh = UsbhRefPdoDeviceHandle(HubFdo, Pdo, Pdo, Devh_ResetOld_Tag);

    // unlink the old handle but do not remove it yet
    UsbhUnlinkPdoDeviceHandle(HubFdo, Pdo, 'xDHb', FALSE);

    LOG(HubFdo, LOG_BUS, 'oldh', 0, oldDh);

    // Expected behavior in the port driver for Create, Init and Restore regarding
    // device handles:
    //
    // (this is documented in hubbusif.w)
    // NOTE: this is how it 'should' work -- I'm working on the assumption that we will
    // fix usbport if necessary.
    //
    // This table describes the state of the device handle on return from the USBPORT
    // function.
    //
    // Function     Fails                       Success                 notes
    //---------------------------------------------------------------------------------
    // Create       invalid                     valid
    //
    // Init         invalid                     valid                   invalid are freed by usbport
    //
    // Remove          -                        invalid
    //
    // Restore      old = valid                 old = invalid           invalid are freed by usbport
    //              new = valid *               new = valid
    //
    // * This is a USBPORT / USBHUB interface change for Win7.  On
    // Restore Device failure USBHUB now expects the new device handle
    // to remain valid.  USBHUB should subsequently call the USBPORT
    // RemoveUsbDevice() interface to release all resources allocated
    // for the new device handle on Restore Device failure.
    //

    // queue the special message to the bus engine. This is similar to the port connect state
    // function except we already have a PDO that it is already registered.
    //
    // The port reset is initiated as a bit change to the bus engine so that we may handle
    // all possible cases when a reset might be requested.
    // Requesting a reset in any state other than WAIT_CONNECT will fail.

    // Make sure the hub is running before we try to reset an attached device
    Usbh_SSH_Event(HubFdo, SSH_Event_ResumeHub, Sc);

    // regardless of outcome consider the device no longer armed
    pdoExt->PdoFlags.DeviceArmedForWake = 0;
    pc.ul = 0;
    pc.DriverResetChange = 1;

    //DbgTrace((UsbhDebugTrace, "Queue Driver Reset request %p\n", Pdo));

    //UsbhPCE_QueueChangeObject(HubFdo, pdoExt->PortNumber, pc, Sc, PcqObj_Pdo, Pdo);
    UsbhPCE_QueueDriverReset(HubFdo, pdoExt->PortNumber, pc, Sc, Pdo);

    LOG(HubFdo, LOG_BUS, 'RDwt', 0, 0);
    // wait on the device to be reenumerated by the bus engine
    //WAIT_EVENT(&event);
    pd = UsbhGetPortData(HubFdo, pdoExt->PortNumber);
    UsbhWaitEventWithTimeoutEx(HubFdo, &event, 0, 'RDWt', Timeout_Not_Fatal_Process_Queue, pd);

    nts = pdoExt->SyncDeviceResetStatus;
    DbgTrace((UsbhDebugTrace, "Driver Reset request complete %p %!STATUS!\n", Pdo, nts));
    UsbhAssert(HubFdo, nts != STATUS_BOGUS);

    // success or failure is indicated by SyncDeviceResetStatus.
    // if the reset succeeds we will have a new device handle.
    // if it fails the new device handle should be deleted if we have one.
    if (NT_ERROR(nts)) {

        // free any new handle if one was created
        LOG(HubFdo, LOG_BUS, 'RDel', 0, nts);

        UsbhUnlinkPdoDeviceHandle(HubFdo, Pdo, 'xDH8', TRUE);

        // **
        // restore the old handle(if we have one), it is no longer viable beacuse we have reset the port
        // this fact needs to be accounted for by usbport.  This handle is in a shadow state -- it could
        // be used in subsequent attempts to restore
        if (oldDh) {

            UsbhLinkPdoDeviceHandle(HubFdo, Pdo, oldDh);
            UsbhDerefPdoDeviceHandle(HubFdo, oldDh, Pdo, Devh_ResetOld_Tag);
        }

        if (!Usb_Disconnected(nts)) {
            HUB_EXCEPTION(USBFILE, HubFdo, pdoExt->PortNumber, SyncResetDevice_Failed,
                          nts, 0xFFFFFFFF, NULL, 0);
        }

        DbgTrace((UsbhDebugTrace, "Failed sync reset %p %!STATUS!\n", Pdo, nts));


    } else {

        // The devh may not be valid if a disconnect occurs after the reset wait
        // event is signaled.  The PDO is still latched so it won't be deleted but
        // we must acquire the bus lock before calling the restore function.
        // This is the 'classic' behavior of the original hub driver -- holding his lock
        // will prevent any new device handles from being created.

        UsbhAcquireEnumBusLock(HubFdo, Sc, pdoExt->PortNumber);

        newDh = UsbhRefPdoDeviceHandle(HubFdo, Pdo, Pdo, Devh_ResetNew_Tag);

        if (newDh) {

            if (oldDh) {

                // drop the original old dh ref
                // this must happen now so that RestoreDevice can flush and remove
                // the old handle.
                UsbhDerefPdoDeviceHandle(HubFdo, oldDh, Pdo, Devh_ResetOld_Tag);

                // attempt to restore the original device configuration
                // the old device handle is on completion by the port driver
                // regardless of success or failure.
                LOG(HubFdo, LOG_BUS, 'RSTR', oldDh, newDh);
                nts = Usbh_HubRestoreDevice(HubFdo, Pdo, oldDh, newDh);

                if (pdoExt->IsBootDevice) {

                    if ( NT_SUCCESS(nts) &&
                        !pdoExt->PdoFlags.DeviceIsHub &&
                        pdoExt->PdoFlags.SerialNumber) {

                        USB_ID_STRING idString;
                        SIZE_T lengthInBytes;

                        RtlZeroMemory(&idString, sizeof(USB_ID_STRING));

                        // If this call fails IdString would be NULL
                        // It is unlikely though because pdoExt->PdoFlags.SerialNumber
                        // is set to true
                        UsbhGetSerialNumber(HubFdo,Pdo, &idString);

                        if (idString.LengthInBytes != pdoExt->SerialNumberId.LengthInBytes) {
                            wrongBootDevice = TRUE;
                        } else if (idString.Buffer != NULL) {
                            lengthInBytes =
                                RtlCompareMemory(idString.Buffer,
                                                 pdoExt->SerialNumberId.Buffer,
                                                 idString.LengthInBytes);

                            if (lengthInBytes != idString.LengthInBytes) {
                                wrongBootDevice = TRUE;
                            }
                        }
                        UsbhFreeID(&idString);
                    }

                    if (nts == STATUS_UNSUCCESSFUL) {
                        wrongBootDevice = TRUE;
                    }
                }
                LOG(HubFdo, LOG_BUS, 'RDre', 0, nts);

                // Release the reference taken on new device handle
                // prior to the calling Usbh_HubRestoreDevice().
                //
                UsbhDerefPdoDeviceHandle(HubFdo, newDh, Pdo, Devh_ResetNew_Tag);

                // usbport cannot tell if the device is connected so we
                // don't check for this.
                if (NT_ERROR(nts)) {
                    HUB_EXCEPTION(USBFILE, HubFdo, pdoExt->PortNumber, UsbdRestoreDevice_Failed,
                                  nts, 0xFFFFFFFF, NULL, 0);

                    // On a Restore Device failure we expect that the
                    // old handle is still valid, and the new handle is
                    // also still valid.

                    // The new device handle will not be freed by
                    // USBPORT if Restore Device fails.  Do that now by
                    // calling UsbhUnlinkPdoDeviceHandle(), which
                    // unlinks the new device handle from the PDO,
                    // releases the PDO reference on the new device
                    // handle, and then calls into USBPORT through
                    // Usbh_HubRemoveUsbDevice() to remove the new
                    // device handle.
                    //
                    UsbhUnlinkPdoDeviceHandle(HubFdo, Pdo, 'xDHc', TRUE);

                    // Now that the new device handle has been unlinked
                    // from the PDO and freed, link the old device
                    // handle back to the PDO by calling
                    // UsbhLinkPdoDeviceHandle().  This will take a PDO
                    // reference on the old device handle again.
                    //
                    UsbhLinkPdoDeviceHandle(HubFdo, Pdo, oldDh);
                }

            } else {
                // no old device handle, device is restored in the unconfigured state,
                // this is the case on a re-start.  It also may work for a function driver
                // if it knows enough to reconfigure the device after reset.

                LOG(HubFdo, LOG_BUS, 'RDol', 0, nts);
                nts = STATUS_SUCCESS;

                // Release the reference taken on new device handle
                // prior to the calling Usbh_HubRestoreDevice(), which
                // wasn't actually called in this case as there was no
                // old device handle to restore.
                //
                UsbhDerefPdoDeviceHandle(HubFdo, newDh, Pdo, Devh_ResetNew_Tag);
            }
        } else {
            // no new handle created this time

            LOG(HubFdo, LOG_BUS, 'RD01', 0, oldDh);
            // restore the original handle to the PDO
            UsbhLinkPdoDeviceHandle(HubFdo, Pdo, oldDh);

            UsbhDerefPdoDeviceHandle(HubFdo, oldDh, Pdo, Devh_ResetOld_Tag);

            nts = STATUS_UNSUCCESSFUL;
        }

        if (NT_SUCCESS(nts) && pdoExt->IsBootDevice && !wrongBootDevice) {

            // If this is not a hub, it means that surprise removal
            // recovery was successful. Lets reset the timeout to the
            // original default value
            // We do not need to read it from Registry again because if it was
            // '0' the machine would had turned off.

            // Boot device has been replugged in.  If SQM surprise removal count flag is set then update
            // the SQM surprise remove count in <Control Key>\Usb\Ceip\BootPathSurpriseRemovalCount
            // reg value.
            //
            if ((HubG.SQMSurpriseRemovalUnplug) &&
               (!pdoExt->PdoFlags.DeviceIsHub)){
                // Notify any interested parties of a successful recovery from
                // surprise removal, through a WMI notification.

                WmiFireEvent(HubFdo,
                             (LPGUID)&GUID_USB_WMI_SURPRISE_REMOVAL_NOTIFICATION,
                             0,
                             0,
                             NULL);

                // Update the SQM BootPathSurpriseRemovalCount registry value only for the boot device,
                // ie, exclude any hub only plug in's
                //
                // A workitem must be queued here since it's possible that a registry write inside of
                // UsbhUpdateRegSurpriseRemovalCount() could trigger a page fault and the boot
                // device is not in D0 which will cause the system to hang.
                nts = UsbhQueueWorkItem(HubFdo,
                                        DelayedWorkQueue,
                                        UsbhUpdateRegSurpriseRemovalCount,
                                        &HubG.SQMSurpriseRemovalUnplug,
                                        pdoExt->PortNumber,
                                        SIG_SQM_WORKER);
                if (NT_ERROR(nts)) {
                // Note that this is not a fatal error and that the worst case is that the
                // SQM surprise removal count will not be incremented and written to the registry
                // for this failure case.  Also, set HubG.SQMSurpriseRemovalUnplug to FALSE to
                // indicate that the boot device has been plugged back in.
                HubG.SQMSurpriseRemovalUnplug = FALSE;
                DbgTrace((UsbhDebugTrace, "Failed to queue SQM SRem workitem - %!STATUS!\n", nts));
                }
            }
        }
        UsbhReleaseEnumBusLock(HubFdo, Sc, pdoExt->PortNumber);
    }

    // release and allow another reset
    KeAcquireSpinLock(&hubExt->SyncDeviceResetSpin, &reset_irql);
    UsbhAssert(HubFdo, pdoExt->SyncDeviceResetEvent != NULL);
    pdoExt->SyncDeviceResetEvent = NULL;
    KeReleaseSpinLock(&hubExt->SyncDeviceResetSpin, reset_irql);

    if (wrongBootDevice) {
        // This means that the user has plugged in the wrong device
        DbgTrace((UsbhPwBootTrace,"Wrong device plugged in to HubFdo=#%p %!FUNC!\n", HubFdo));
        LOG(HubFdo, LOG_BUS, 'SRWD', nts, HubFdo);
        UsbhWaitForBootDevice(HubFdo ,
                              Pdo,
                              HUB_RESET_DO_NOT_NEED_BIT,
                              HUB_RESET_NEED_BIT_SET);
        DbgTrace((UsbhPwBootTrace,"Wrong device removed from HubFdo=#%p %!FUNC!\n", HubFdo));
    }

    DbgTrace((UsbhDebugTrace,"<%!FUNC! %!STATUS!\n", nts));
    UsbhEtwLogDeviceEvent(pdoExt, &USBHUB_ETW_EVENT_DEVICE_SYNC_RESET_DEVICE_INTERNAL_COMPLETE, nts);
    return nts;

}


NTSTATUS
UsbhSyncResumePort(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    USHORT PortNumber
    )
/*++

Routine Description:

    Resume the port.

    This function is used internally to resume a suspended port when needed.  It is used by the
    power code to resume a port for a PDO that is 'selectively suspended'.

    If the port is not currently suspended this is a nop.

    This function resumes regardless of what PDO is attached, caller must validate the request.

Irql: PASSIVE

Arguments:

    HubFdo - Hub we are dealing with.

    Sc - state context for the request -- this is usually the PDO context.

    Pdo - Pdo to reset the port for, pdo should be latched.

Return Value:

    This function returns an error if the device currently attached is
    different from the one associated with the PDO or if we cannot resume the
    device.

--*/
{
    NTSTATUS nts;
    PHUB_PORT_DATA pd;

    DbgTrace((UsbhDebugTrace,"%!FUNC!\n"));
    DbgTrace((UsbhDebugTrace, "SyncResume USB device Port %d\n", PortNumber));


    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        UsbhAssert(HubFdo, pd);

        // Lock out pause/disable events until we have completed port resume
        WAIT_EVENT(&pd->EnableDisableLock);

        UsbhPCE_Resume(HubFdo, Sc, PortNumber);
        UsbhPCE_Enable(HubFdo, Sc, PortNumber);

        // Allow pause/disable events
        SET_EVENT(&pd->EnableDisableLock);

        LOG(HubFdo, LOG_POWER, 'PRWx', 0, pd->PortNumber);
        //WAIT_EVENT(&pd->PortResumeEvent);
        nts = UsbhWaitForPortResume(HubFdo, pd, Sc);

        if (!NT_SUCCESS(nts)) {
            HUB_EXCEPTION(USBFILE, HubFdo, PortNumber,
                          PowerErr_SyncResumePortFailed,
                          nts, 0, NULL, 0);
        }

//        UsbhWaitEventWithTimeoutEx(HubFdo, &pd->PortResumeEvent, USBHUB_X9F_TIMEOUT, 'PRWx',
//                    Timeout_SyncResumePort, pd);

//        nts = STATUS_SUCCESS;
    } else {
        nts = STATUS_INVALID_PARAMETER;
    }

    return nts;
}


NTSTATUS
UsbhSyncResumeDeviceInternal(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Resume a device.

    Given a PDO -- if it is still attached resume the associated port and return the Pdo to D0.

    This is used for explicit and implied transitions to D0.  It is used in the case where a
    selectively suspended device wants to wants to transition back to D0.

    If the port is not suspended then the PDO is just marked as D0.

Irql: PASSIVE

Arguments:

    HubFdo - Hub we are dealing with.

    Sc - state context for the request -- this is usually the PDO context.

    Pdo - Pdo to reset the port for, pdo should be latched.

Return Value:

    This function returns an error if the device currently attached is
    different from the one associated with the PDO or if we cannot resume the
    device.

--*/
{
    PDEVICE_OBJECT curPdo;
    NTSTATUS nts = STATUS_SUCCESS;
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;

// if the device is a selectively suspended hub then it won't be in 'D0'.
//    UsbhAssert(HubFdo, GET_PDO_DX(Pdo) != PowerDeviceD0);

    pdoExt = PdoExt(Pdo);

    curPdo = UsbhLatchPdo(HubFdo, pdoExt->PortNumber, NULL, PdoLatchResumePort_Tag);
    LOG(HubFdo, LOG_HUB, 'rsmp', pdoExt->PortNumber, curPdo);

    do {
        hubExt = FdoExt(HubFdo);

        if (curPdo == NULL ||
            Pdo != curPdo ||
            hubExt->FdoFlags.HubHardResetPending ) {

            nts = NT_DEVICE_DISCONNECTED_STATUS;
            break;
        }

        // make sure this is our PDO
        UsbhAssert(HubFdo, Pdo == curPdo);
        UsbhAcquirePdoStateLock(HubFdo, Sc, PdoEv_DeviceResume);

        if (GET_PDO_HWPNPSTATE(Pdo) == Pdo_PnpRefHwPresent) {
            UsbhReleasePdoStateLock(HubFdo, Sc);

            nts = UsbhSyncResumePort(HubFdo,
                                     Sc,
                                     pdoExt->PortNumber);

        } else {
            // device is being removed, fail the request now.

            nts = NT_DEVICE_DISCONNECTED_STATUS;
            UsbhReleasePdoStateLock(HubFdo, Sc);
            break;
        }

    } WHILE (0);

    if (curPdo) {
        UsbhUnlatchPdo(HubFdo, curPdo, NULL, PdoLatchResumePort_Tag);
    }

    if (NT_SUCCESS(nts)) {
        UsbhSet_Pdo_Dx(Pdo, PowerDeviceD0);
    }

    return nts;
}


NTSTATUS
UsbhCycleDevicePort(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USBD_STATUS *UsbdStatus
    )
/*++

Routine Description:

    This is an asynchronous operation, it queues an artificial connect change
    to the port so that the current PDO instance detached and the device
    is reenumerated as if it were disconnected and reconnected.

Irql: PASSIVE

Arguments:

Return Value:

    Success if we are able to queue the change

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PHUB_PORT_DATA pd;
    NTSTATUS nts;
    PDEVICE_OBJECT curPdo;
    PSTATE_CONTEXT sc;
    PDEVICE_EXTENSION_HUB hubExt;

    // TEST #128, #129 #130 cover these cases
    nts = STATUS_BOGUS;

    DbgTrace((UsbhDebugTrace, "Cycle USB device (PDO)\n"));
    pdoExt = PdoExt(Pdo);
    hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();

    // we do this from PDO context
    sc = PDO_CONTEXT(Pdo);

    do {

        pd = UsbhGetPortData(HubFdo, pdoExt->PortNumber);
        if (!pd) {
            LOG(HubFdo, LOG_BUS, 'CYe1', 0, 0);
            nts = STATUS_INVALID_PARAMETER;
            break;
        }

        if (hubExt->FdoFlags.HubHwNotPresent) {
            LOG(HubFdo, LOG_BUS, 'Cye2', 0, 0);
            *UsbdStatus = USBD_STATUS_DEVICE_GONE;
            nts = NT_DEVICE_DISCONNECTED_STATUS;
            break;
        }

        if (hubExt->FdoFlags.HubHardResetPending) {
            LOG(HubFdo, LOG_BUS, 'Cye3', 0, 0);
            *UsbdStatus = USBD_STATUS_HUB_INTERNAL_ERROR;
            nts =  STATUS_ADAPTER_HARDWARE_ERROR;
            break;
        }

        // attempt to latch this PDO before we cycle. We only want to permit
        // this if the device is still present and this PDO is still current.
        curPdo = UsbhLatchPdo(HubFdo, pdoExt->PortNumber, NULL, PdoLatchCyclePort_Tag);
        LOG(HubFdo, LOG_BUS, 'cycl', pdoExt->PortNumber, curPdo);

        if (curPdo == NULL) {
            // no latch taken
            nts = NT_DEVICE_DISCONNECTED_STATUS;
            break;
        }

        // make sure this is our PDO
        if (Pdo == curPdo) {

            UsbhAcquirePdoStateLock(HubFdo, sc, PdoEv_DeviceCycle);
            if (GET_PDO_HWPNPSTATE(Pdo) == Pdo_PnpRefHwPresent) {

                UsbhReleasePdoStateLock(HubFdo, sc);

                LOG(HubFdo, LOG_HUB, 'cycG', pdoExt->PortNumber, pdoExt->PortNumber);
                // track how many cycles have occurred on this port
                // for jasons stats.
                InterlockedIncrement(&pd->DriverCycleCount);

                // TEST #130
                nts = UsbhSyncResumePort(HubFdo, sc, pdoExt->PortNumber);

                LOG(HubFdo, LOG_HUB, 'cyRS', pdoExt->PortNumber, nts);

                //if (NT_SUCCESS(nts)) {
                    UsbhQueueSoftConnectChange(HubFdo,
                               pdoExt->PortNumber,
                               sc,
                               TRUE);
                //}
                nts = STATUS_SUCCESS;
            } else {
                // device is being removed, fail the request now.

                nts = NT_DEVICE_DISCONNECTED_STATUS;
                UsbhReleasePdoStateLock(HubFdo, sc);
            }
        }

        UsbhUnlatchPdo(HubFdo, curPdo, NULL, PdoLatchCyclePort_Tag);

    } WHILE (0);

    LOG(HubFdo, LOG_HUB, 'cycG', pdoExt->PortNumber, nts);
    ASSERT_NTS(HubFdo, nts);

    return nts;

}

NTSTATUS
UsbhWaitForPortResume(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA pd,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

Arguments:

Irql: PASSIVE

Return Value:


--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS resume_nts;
    KIRQL rs_irql;

    hubExt = FdoExt(HubFdo);

    // we have a fairly large timeout here since under certian stress conditions (verifier and
    // dma checking) we see a timeout due to the fact that the actual resume has not been
    // dispatched to the device.

    LOG(HubFdo, LOG_POWER, 'PRW1', 0, pd->PortNumber);

    pd->ResumeTimeout = FALSE;
    do {
        KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);
        if (pd->PortHwPendingState == PortHw_ResumePending) {
            KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);

            // resume_nts = UsbhWaitEventWithTimeout(HubFdo, &pd->PortResumeEvent, 500, 'PRWw');
            resume_nts = UsbhWaitEventWithTimeoutEx(HubFdo, &pd->PortResumeEvent, 500, 'PRWw', Timeout_Not_Fatal_Process_Queue, pd);
        } else {
            // port not waiting on resume
            KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);
            resume_nts = STATUS_SUCCESS;
        }

        if (resume_nts == STATUS_TIMEOUT) {
            //Test #2002, #2003, #2002, #2004, #2005

            // we handle this case better by dispatching a separate timeout event into the queue and
            // waiting again this allows us to detect the case where the system is running very slowly.
            // the timout will reset the event to the appropriate state so we can wait on it again.
            UsbhPCE_ResumeTimeout(HubFdo, StateContext, pd->PortNumber);
            continue;
        }
    } while (resume_nts == STATUS_TIMEOUT);

    // note that in most error cases we hit the timeout path to recover the port

    if (pd->ResumeTimeout) {
        //Test #2003, #2002, #2004, #2005

        //The resume failed to complete in a reasonable time.
        //Try to recover the device with a reset.

        // since this case can indicate a potential perf problem we generate a trap in debug test mode.
        //  ** debug trace output can put the resume time close to the threshold
        //TEST_TRAP();
        resume_nts = UsbhPortResumeTimeout(HubFdo, StateContext, pd);
    }

    return resume_nts;
}


NTSTATUS
UsbhPortResumeTimeout(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    PHUB_PORT_DATA Pd
    )
/*++
Description:

    Port resume failed on return to S0 or D0.  This could be a problem with
    the device, hub or system in general.

    First we attempt to recover the device.

Arguments:

Return:

    ntstatus not success if an error is encountered resuming the hub

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    USBD_STATUS usbdStatus;
    USB_HUB_PORT_STATE ps;
    PDEVICE_OBJECT pdo;
    USHORT port = Pd->PortNumber;
    UCHAR portStatusBits;

    hubExt = FdoExt(HubFdo);

    // get the current status on the port, this allows us to bypass it the hub is
    // disconnected.

    nts = UsbhQueryPortState(HubFdo,
                              Pd->PortNumber,
                              &ps,
                              &usbdStatus);

    LOG(HubFdo, LOG_POWER, 'rto1', nts, 0);

    if (Usb_Disconnected(nts)) {
        // hub is disconnected
        return nts;
    }

    // TEST #2002
    // TEST #2004
    HUB_EXCEPTION(USBFILE,  HubFdo, Pd->PortNumber, PowerErr_PortResumeTimeout,
                       nts, usbdStatus, NULL, 0);
TEST_TRAP();
    // sanity check the status, see if we need to fail the warm-start altogether
    //
    //

    // since the change engine is running now we need to latch
    // on the PDO in order to restore the device.
    pdo = UsbhLatchPdo(HubFdo, port, NULL, PdoLatchD0_Warm_Tag);
    LOG(HubFdo, LOG_POWER, 'rto2', port, pdo);

    if (pdo) {

        do {
            portStatusBits = UsbhGetPortStatusBits(HubFdo, Pd, &ps, pdo);

            LOG(HubFdo, LOG_POWER, 'PWRS', portStatusBits, port);

            // ceosp
            // 11001  0x19
            if (portStatusBits == 0x19) {
                // port resumed on its own, hub may have neglected to send a change or its in the
                // queue
                nts = STATUS_SUCCESS;
                LOG(HubFdo, LOG_POWER, 'rto3', portStatusBits, nts);

                break;
            }

            if (portStatusBits == 0x1f) {
                // special case bogus bits -- if we are seeing this it may be the root
                // regs are reading all ffffs
                nts = NT_DEVICE_DISCONNECTED_STATUS;
                LOG(HubFdo, LOG_POWER, 'rto4', portStatusBits, nts);


                // HUBEX
                break;
            }

            // initiate our own reset for this device
            nts = UsbhSyncResetDeviceInternal(HubFdo, Sc, pdo);
            if (NT_ERROR(nts)) {
                // queue a connect change for this port, this is either
                // not the same device or we were unable to
                // re-enumerate it.

                //Test #2008, #2004
                TEST_TRAP();
                LOG(HubFdo, LOG_POWER, 'rto5', 0, nts);

                // queue the change message to the bus engine and disable the
                // hub port.
                UsbhQueueSoftConnectChange(HubFdo, port, Sc, TRUE);
                PdoExt(pdo)->PdoFlags.NeedsReset = 0;

                // it is not necessarily an error for the device to be different,
                // log this as an exception in case the hw failed.
                HUB_EXCEPTION(USBFILE,  HubFdo, 0,
                      PowerErr_ResumeTimeoutRestoreFailed,
                      nts, 0, NULL, 0);

                nts = STATUS_SUCCESS;

            } else {
                // reset was successful
                PdoExt(pdo)->PdoFlags.NeedsReset = 0;

                if (PdoExt(pdo)->PdoFlags.DeviceIsHub) {
                    // Notify the child hub that it lost power, so it can be cold started when it
                    // receives the set D0 power request
                    UsbhSendSynchronousUsbIoctlRequest(HubFdo,
                        pdo,
                        IOCTL_INTERNAL_USB_NOTIFY_HUB_PWR_LOSS);
                }

                nts = STATUS_SUCCESS;
            }
        } WHILE (0);
        UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatchD0_Warm_Tag);
    } else {
        nts = STATUS_SUCCESS;
    }

    return nts;
}


VOID
UsbhWaitForBootDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    ULONG ConnectBit,
    ULONG ConnectChangeBit
    )
/*++

Routine Description:

    This routine wait for a boot device to enter the connect and
    connect change state specified by the 2 parameters.
    This function will loop until that state is hit or the timeout
    is reached in which case it will turn the system off

Arguments:
    HubFdo - The hub from which the boot device was removed
    Pdo - Physical device object of the boot device
    ConnectBit - The expected state of connect bit to break the loop
    ConnectChangeBit - The expected state of connect change bit to break the loop

Return Value:
    None

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = PdoExt(Pdo);

    DbgTrace((UsbhPwBootTrace,"Waiting for requested port change from HubFdo=#%p %!FUNC!\n", HubFdo));

    pdoExt->BootDeviceConnectBitExpected = ConnectBit;
    pdoExt->BootDeviceConnectChangeBitExpected = ConnectChangeBit;

    if(UsbhNotifyBootDeviceRemoval(pdoExt->ExBootDeviceHandle)) {
        KeWaitForSingleObject(&pdoExt->BootDeviceArrivalEvent,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);
    }

    DbgTrace((UsbhPwBootTrace,"Done Waiting for requested port change from HubFdo=#%p %!FUNC!\n", HubFdo));
}

NTSTATUS
UsbhGetBillboardInfo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR BillboardDescriptor
    )
/*++

Routine Description:

    Retrieves and validates the string descriptors for Billlboard
    device and the alternate modes. Also, publishes the WNF event
    to notify the UX about the presence of a billboard device

Irql: PASSIVE

Arguments:

Return Value:

    NTSTATUS

--*/
{


    NTSTATUS nts;
    PDEVICE_EXTENSION_PDO pdoExt;
    USHORT nBytes;
    PUSB_STRING_DESCRIPTOR sd;
    USBD_STATUS usbdStatus;
    UCHAR currentMode;

    pdoExt = PdoExt(Pdo);

    // allocate a temp buffer to hold a string descriptor
    UsbhAllocatePool_Z(sd, NonPagedPool, MAXIMUM_USB_STRING_LENGTH);

    if (!sd) {
        LOG(HubFdo, LOG_BUS, 'Gb1', 0, 0);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    nts = STATUS_SUCCESS;

    if (BillboardDescriptor->iAddtionalInfoURL != 0) {

        nBytes = MAXIMUM_USB_STRING_LENGTH;

        // Just query for US English as we are only validating
        // the descriptor, we are not really using it.
        nts = UsbhGetStringFromDevice(HubFdo,
                                      Pdo,
                                      &usbdStatus,
                                      (PUCHAR)sd,
                                      &nBytes,
                                      0x409, // good'ol american english
                                      BillboardDescriptor->iAddtionalInfoURL);

        if (NT_ERROR(nts)) {
            HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, UsbErr_GetBillboardStringDescrptorFailed,
                          nts, usbdStatus, sd, nBytes);
            LOG(HubFdo, LOG_BUS, 'Gb2', 0, nts);
            UsbhFreePool(sd);
            return nts;
        }


        if (!UsbhValidateStringDescriptor(HubFdo,
                                          sd,
                                          nBytes,
                                          &usbdStatus)) {
            HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, BadUsbString,
                          nts, usbdStatus, sd, nBytes);
            LOG(HubFdo, LOG_BUS, 'Gb3', 0, nts);
            UsbhFreePool(sd);
            return nts;
        }
    }

    for (currentMode = 0; currentMode < BillboardDescriptor->bNumberOfAlternateModes; currentMode++) {

        if (BillboardDescriptor->AlternateMode[currentMode].iAlternateModeSetting != 0) {

            nBytes = MAXIMUM_USB_STRING_LENGTH;

            // Just query for US English as we are only validating
            // the descriptor, we are not really using it.
            nts = UsbhGetStringFromDevice(HubFdo,
                                          Pdo,
                                          &usbdStatus,
                                          (PUCHAR)sd,
                                          &nBytes,
                                          0x409, // good'ol american english
                                          BillboardDescriptor->AlternateMode[currentMode].iAlternateModeSetting);

            if (NT_ERROR(nts)) {
                HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, UsbErr_GetAlternateModeStringDescrptorFailed,
                              nts, usbdStatus, sd, nBytes);
                LOG(HubFdo, LOG_BUS, 'Gb4', 0, nts);
                UsbhFreePool(sd);
                return nts;
            }


            if (!UsbhValidateStringDescriptor(HubFdo,
                                              sd,
                                              nBytes,
                                              &usbdStatus)) {
                HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, BadUsbString,
                              nts, usbdStatus, sd, nBytes);
                LOG(HubFdo, LOG_BUS, 'Gb5', 0, nts);
                UsbhFreePool(sd);
                return nts;
            }
        }
    }

    UsbhFreePool(sd);
    return nts;
}
