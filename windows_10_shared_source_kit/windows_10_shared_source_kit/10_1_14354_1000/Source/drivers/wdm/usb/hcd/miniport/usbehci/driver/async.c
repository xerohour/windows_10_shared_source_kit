/*++

Copyright (c) 1999, 2000 Microsoft Corporation

Module Name:

   async.c

Abstract:

   miniport transfer code for control and bulk

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1999, 2000 Microsoft Corporation.  All Rights Reserved.


Revision History:

    1-1-00 : created, jdunn

--*/

#include "common.h"
#include "ehcierrlog.h"

//implements the following miniport functions:

//non paged
//EHCI_OpenControlEndpoint
//EHCI_InterruptTransfer
//EHCI_OpenControlEndpoint
//EHCI_InitializeTD
//EHCI_InitializeQH

//
// Set this var to the endpointData structure and it will trigger a
// debug break the next time this endpoint is polled.
//
PVOID EHCI_EpTrap = NULL;


VOID
EHCI_EnableAsyncList(
    __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

    Enables the processing for the async schedule

--*/
{
    KIRQL irql;

    // Must acquire the AcfListSpin spinlock prior to calling
    // EHCI_iEnableAsyncList() or EHCI_iDisableAsyncList()
    //
    EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);

    EHCI_iEnableAsyncList(DeviceData);

    EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);
}


VOID
EHCI_DisableAsyncList(
    __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

    Disables the processing for the async schedule

--*/
{
    KIRQL irql;

    // Must acquire the AcfListSpin spinlock prior to calling
    // EHCI_iEnableAsyncList() or EHCI_iDisableAsyncList()
    //
    EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);

    EHCI_iDisableAsyncList(DeviceData);

    EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);
}


VOID
EHCI_LinkControlTransferToQueue(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    PHCD_TRANSFER_DESCRIPTOR setupTd, statusTd;
    HC_QTD_TOKEN token;

    qh = EndpointData->QueueHead;
    setupTd = EndpointData->SetupTD;
    statusTd = EndpointData->StatusTD;

    LOGENTRY(DeviceData, G, '_L2Q', qh, statusTd, setupTd);

    qh->HwQH.CurrentTD.HwAddress = setupTd->PhysicalAddress;
    qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = setupTd->PhysicalAddress;
    qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = setupTd->PhysicalAddress;

    // Initialize the Token in the QH Overlay
    //
    token = qh->HwQH.Overlay.qTD.Token;
    token.BytesToTransfer = 0;
    token.Active = 0;
    token.ErrorCounter = 0;
    token.SplitXstate = 0;
    token.C_Page = 0;
    qh->HwQH.Overlay.qTD.Token = token;

    // Activate the Status Phase HW_QUEUE_ELEMENT_TD
    //
    token = statusTd->HwTD.Token;
    token.Active = 1;
    statusTd->HwTD.Token = token;

    // Activate the Setup Phase HW_QUEUE_ELEMENT_TD
    //
    token = setupTd->HwTD.Token;
    token.Active = 1;
    setupTd->HwTD.Token = token;
}


PHCD_QUEUEHEAD_DESCRIPTOR
EHCI_InitializeQH(
     __in PDEVICE_DATA DeviceData,
     __in PENDPOINT_DATA EndpointData,
     __out_bcount(HCD_QUEUEHEAD_DESCRIPTOR_PAD) PHCD_QUEUEHEAD_DESCRIPTOR Qh,
     HW_32BIT_PHYSICAL_ADDRESS HwPhysAddress
    )
/*++

Routine Description:

   Initialize an QH for inserting in to the
   schedule

   returns a ptr to the QH passed in

Arguments:


--*/
{
    HC_QTD_TOKEN token;

    RtlZeroMemory(Qh, HCD_QUEUEHEAD_DESCRIPTOR_PAD);

    // make double sure we have the prober alignment
    // on the QH structures
    EHCI_ASSERT(DeviceData, (HwPhysAddress & HW_LINK_FLAGS_MASK) == 0);
    EHCI_AssertHwAlignment(DeviceData, HwPhysAddress, sizeof(HW_QUEUEHEAD_DESCRIPTOR));

    Qh->PhysicalAddress = HwPhysAddress;
    ENDPOINT_DATA_PTR(Qh->EndpointData) = EndpointData;
    Qh->Sig = SIG_HCD_QH;

    // init the hw descriptor
    Qh->HwQH.EpChars.DeviceAddress = EndpointData->Parameters.DeviceAddress;
    Qh->HwQH.EpChars.EndpointNumber = EndpointData->Parameters.EndpointAddress;

    switch (EndpointData->Parameters.DeviceSpeed) {
    case LowSpeed:
        Qh->HwQH.EpChars.EndpointSpeed = HcEPCHAR_LowSpeed;
        LOGENTRY(DeviceData, G, '_iLS', EndpointData, 0, 0);
        break;
    case FullSpeed:
        Qh->HwQH.EpChars.EndpointSpeed = HcEPCHAR_FullSpeed;
        LOGENTRY(DeviceData, G, '_iFS', EndpointData, 0, 0);
        break;
    case HighSpeed:
        Qh->HwQH.EpChars.EndpointSpeed = HcEPCHAR_HighSpeed;
        LOGENTRY(DeviceData, G, '_iHS', EndpointData, 0, 0);
        break;
    default:
        USBPORT_BUGCHECK(DeviceData);
    }

    Qh->HwQH.EpChars.MaximumPacketLength =
        EndpointData->Parameters.MaxPacketSize;

    Qh->HwQH.EpCaps.HighBWPipeMultiplier = 1;
    if (EndpointData->Parameters.DeviceSpeed == HcEPCHAR_HighSpeed) {
        Qh->HwQH.EpCaps.HubAddress =  0;
        Qh->HwQH.EpCaps.PortNumber = 0;
    } else {
        Qh->HwQH.EpCaps.HubAddress =
            EndpointData->Parameters.TtDeviceAddress;
        Qh->HwQH.EpCaps.PortNumber =
            EndpointData->Parameters.TtPortNumber;
        if (EndpointData->Parameters.TransferType == Control) {
            Qh->HwQH.EpChars.ControlEndpointFlag = 1;
        }
        LOGENTRY(DeviceData, G, '_iTT',
            EndpointData->Parameters.TtPortNumber,
            EndpointData->Parameters.TtDeviceAddress,
            Qh->HwQH.EpChars.ControlEndpointFlag);
    }

    // init the overlay are such that we are in the 'advance queue'
    // state with the next queue Tds pointing to terminate links
    Qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = EHCI_TERMINATE_BIT;
    Qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = EHCI_TERMINATE_BIT;

    // Active = 0
    // Halted = 0
    //
    token.ul = 0;
    Qh->HwQH.Overlay.qTD.Token = token;

    return Qh;
}


PHCD_TRANSFER_DESCRIPTOR
EHCI_InitializeTD(
     __in PDEVICE_DATA DeviceData,
     __in PENDPOINT_DATA EndpointData,
     __out PHCD_TRANSFER_DESCRIPTOR Td,
     HW_32BIT_PHYSICAL_ADDRESS HwPhysAddress
    )
/*++

Routine Description:

   Initialize an ED for insertin in to the
   schedule

   returns a ptr to the ED passed in

Arguments:


--*/
{
    RtlZeroMemory(Td, sizeof(*Td));

    // make double sure we have the prober alignment
    // on the TD structures
    EHCI_ASSERT(DeviceData, (HwPhysAddress & HW_LINK_FLAGS_MASK) == 0);
    EHCI_AssertHwAlignment(DeviceData, HwPhysAddress, sizeof(HW_QUEUE_ELEMENT_TD));

    Td->PhysicalAddress = HwPhysAddress;
    ENDPOINT_DATA_PTR(Td->EndpointData) = EndpointData;
    Td->Sig = SIG_HCD_TD;
    TRANSFER_CONTEXT_PTR(Td->TransferContext) = FREE_TD_CONTEXT;

    return Td;
}


USB_MINIPORT_STATUS
EHCI_ControlTransfer(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PTRANSFER_PARAMETERS TransferParameters,
     __inout PTRANSFER_CONTEXT TransferContext,
     __inout PTRANSFER_SG_LIST TransferSGList
    )
/*++

Routine Description:

    Initialize a control transfer


    NOTES:

    HW pointers nextTD and AltNextTD are shadowed in
    NextHcdTD and AltNextHcdTD.


--*/
{
    PHCD_TRANSFER_DESCRIPTOR prevTd, td, setupTd, statusTd;
    HC_QTD_TOKEN token;
    ULONG lengthMapped, dataTDCount = 0;
    ULONG nextToggle;
    ULONG cf = 0;

    EHCI_ASSERT(DeviceData, EndpointData->CurEpState == ENDPOINT_ACTIVE);
    EHCI_ASSERT(DeviceData, EndpointData->FreeTds >= 6);

    if (EndpointData == EHCI_EpTrap) {
        TEST_TRAP(DeviceData);
    }
    // we can do any control transfer with six TDs
    if (EndpointData->FreeTds < 6)  {
        return USBMP_STATUS_BUSY;
    }

    // one control transfer at a time
    // this allows us to avoid using LockQueueHead
    if (!IsListEmpty(&EndpointData->SmodeTransferListHead)) {
        return USBMP_STATUS_BUSY;
    }

    EHCI_IncPendingTransfer(DeviceData, EndpointData, TransferContext);

    nextToggle = HcTOK_Toggle1;

    //
    // first prepare a TD for the setup packet
    //

    LOGENTRY(DeviceData, G, '_CTR', EndpointData, TransferParameters, 0);

    //
    // allocate setup stage, we use a static TD for this
    //
    TransferContext->PendingTds++;
    if (EndpointData->SetupTD == NULL) {
        EndpointData->SetupTD = setupTd = EHCI_ALLOC_TD(DeviceData, EndpointData);
    } else {
        setupTd = EndpointData->SetupTD;
    }

    if (setupTd == NULL) {
        EHCI_ASSERTMSG(DeviceData, "EndpointData->SetupTD should not be NULL", FALSE);
        return USBMP_STATUS_BUSY;
    }

    // Initialize the TD, including setting BufferPage64[0-4] to zero.
    //
    INITIALIZE_TD_FOR_TRANSFER(setupTd, TransferContext);

    //
    // Move setup data into TD (8 chars long)
    //
    RtlCopyMemory(&setupTd->Packet[0],
                  &TransferParameters->SetupPacket[0],
                  8);

    // this will set the offset and phys address bits at
    // the same time
    setupTd->HwTD.BufferPage[0].ul = (ULONG)(((PCHAR) &setupTd->Packet[0])
                        - ((PCHAR) &setupTd->HwTD)) + setupTd->PhysicalAddress;

    token = setupTd->HwTD.Token;
    token.BytesToTransfer = 8;
    token.Pid = HcTOK_Setup;
    token.DataToggle = HcTOK_Toggle0;
    token.InterruptOnComplete = 1;
    //token.Active = 1; // Activated by EHCI_LinkControlTransferToQueue()
    setupTd->HwTD.Token = token;

    LOGENTRY(DeviceData,
             G, '_set',
             setupTd,
             *((PLONG) &TransferParameters->SetupPacket[0]),
             *((PLONG) &TransferParameters->SetupPacket[4]));

    // allocate the status phase TD now so we can
    // point the data TDs to it
    if (EndpointData->StatusTD == NULL) {
        EndpointData->StatusTD = statusTd = EHCI_ALLOC_TD(DeviceData, EndpointData);
    } else {
        statusTd = EndpointData->StatusTD;
    }
    TransferContext->PendingTds++;

    if (statusTd == NULL) {
        EHCI_ASSERTMSG(DeviceData, "EndpointData->StatusTD should not be NULL", FALSE);
        return USBMP_STATUS_BUSY;
    }

    // Initialize the TD, including setting BufferPage64[0-4] to zero.
    //
    INITIALIZE_TD_FOR_TRANSFER(statusTd, TransferContext);

    // point setup to status
    SET_ALTNEXT_TD(DeviceData, setupTd, statusTd);

    //
    // now setup the data phase
    //

    td = prevTd = setupTd;
    lengthMapped = 0;
    while (lengthMapped < TransferParameters->TransferBufferLength) {

        //
        // fields for data TD
        //

        td = EHCI_ALLOC_TD(DeviceData, EndpointData);
        if (!td) {
            goto EHCI_ControlTransferNoTds;
        }
        INITIALIZE_TD_FOR_TRANSFER(td, TransferContext);
        dataTDCount++;
        TransferContext->PendingTds++;
        SET_NEXT_TD(DeviceData, prevTd, td);

        token = td->HwTD.Token;

        // use direction specified in transfer
        if (TEST_FLAG(TransferParameters->TransferFlags, USBD_TRANSFER_DIRECTION_IN)) {
            token.Pid = HcTOK_In;
        } else {
            token.Pid = HcTOK_Out;
        }

        token.DataToggle = nextToggle;
        token.InterruptOnComplete = 1;
        token.Active = 1;
        td->HwTD.Token = token;

        SET_ALTNEXT_TD(DeviceData, td, statusTd);

        LOGENTRY(DeviceData,
            G, '_dta', td, lengthMapped,
            TransferParameters->TransferBufferLength);

        lengthMapped =
            EHCI_MapAsyncTransferToTd(DeviceData,
                                      EndpointData->Parameters.MaxPacketSize,
                                      lengthMapped,
                                      &nextToggle,
                                      TransferContext,
                                      td,
                                      TransferSGList);

        // calculate next data toggle
        // if number of packets is odd the nextToggle is 0
        // otherwise it is 1

        prevTd = td;
    }

    // last td prepared points to status
    SET_NEXT_TD(DeviceData, td, statusTd);

    //
    // now do the status phase
    //

    LOGENTRY(DeviceData, G, '_sta', statusTd, 0, dataTDCount);

    // do the status phase

    // no buffer
    statusTd->HwTD.BufferPage[0].ul = 0;

    token = statusTd->HwTD.Token;
    token.BytesToTransfer = 0;
    statusTd->TransferLength = 0;
    // status stage is always toggle 1
    token.DataToggle = HcTOK_Toggle1;
    //token.Active = 1; // Activated by EHCI_LinkControlTransferToQueue()
    token.InterruptOnComplete = 1;

    // status phase is opposite data dirrection
    if (TEST_FLAG(TransferParameters->TransferFlags, USBD_TRANSFER_DIRECTION_IN)) {
        token.Pid = HcTOK_Out;
    } else {
        token.Pid = HcTOK_In;
    }
    statusTd->HwTD.Token = token;

    // put the request on the hardware queue
    LOGENTRY(DeviceData, G,
        '_Tal',  TransferContext->PendingTds, td->PhysicalAddress, td);

    // td points to last TD in this transfer, point it at original setup (first TD)
    SET_NEXT_TD(DeviceData, statusTd, setupTd);
    SET_ALTNEXT_TD(DeviceData, statusTd, setupTd);

    // set the active bit in the setup Phase TD, this will
    // activate the transfer

#if DBG
    cf = EHCI_Get32BitFrameNumber(DeviceData);
#endif
    LOGENTRY(DeviceData, G, '_ctr',  cf, 0, 0);

    EHCI_EnableAsyncList(DeviceData);

    EHCI_LinkControlTransferToQueue(DeviceData,
                                    EndpointData);

    return USBMP_STATUS_SUCCESS;

 EHCI_ControlTransferNoTds:

    // Should never get here!
    USBPORT_BUGCHECK(DeviceData);
    return USBMP_STATUS_BUSY;
}


USB_MINIPORT_STATUS
EHCI_BulkTransfer(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PTRANSFER_PARAMETERS TransferUrb,
     __inout PTRANSFER_CONTEXT TransferContext,
     __inout PTRANSFER_SG_LIST TransferSGList
     )
{
    EHCI_ASSERT(DeviceData, EndpointData->CurEpState == ENDPOINT_ACTIVE);

    EHCI_ASSERT(DeviceData, EndpointData->sMode);

    if (EndpointData == EHCI_EpTrap) {
        TEST_TRAP(DeviceData);
    }

    return EHCI_sMode_Transfer(DeviceData,
                             EndpointData,
                             TransferUrb,
                             TransferContext,
                             TransferSGList,
                             TRUE);
}


USB_MINIPORT_STATUS
EHCI_OpenBulkOrControlEndpoint(
     __inout PDEVICE_DATA DeviceData,
     BOOLEAN Control,
     __inout PENDPOINT_PARAMETERS EndpointParameters,
     __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PUCHAR buffer;
    HW_32BIT_PHYSICAL_ADDRESS phys, qhPhys;
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    ULONG i;
    ULONG tdCount, bytes;
    PHCD_TRANSFER_DESCRIPTOR setupTd;
    HC_QTD_TOKEN token;

    LOGENTRY(DeviceData, G, '_opC', 0, 0, EndpointParameters);

    EndpointData->HwSyncState = Qh_Not_Busy;

    InitializeListHead(&EndpointData->DoneTdList);

    buffer = EndpointParameters->CommonBufferVa;
    phys = EndpointParameters->CommonBufferPhys;
    // how much did we get
    bytes = EndpointParameters->CommonBufferBytes;

    // 256 byte block used to check for overlay sync
    // problems
    EndpointData->QhChkPhys = phys;
    EndpointData->QhChk = buffer;
    // use descriptor pad value to maintain alignment on a 4k boundary
    RtlZeroMemory(buffer, HCD_QUEUEHEAD_DESCRIPTOR_PAD);
    phys += HCD_QUEUEHEAD_DESCRIPTOR_PAD;
    buffer += HCD_QUEUEHEAD_DESCRIPTOR_PAD;
    bytes -= HCD_QUEUEHEAD_DESCRIPTOR_PAD;

    // make the Ed
    qh = (PHCD_QUEUEHEAD_DESCRIPTOR) buffer;
    qhPhys = phys;
    // how much did we get

    phys += HCD_QUEUEHEAD_DESCRIPTOR_PAD;
    buffer += HCD_QUEUEHEAD_DESCRIPTOR_PAD;
    bytes -= HCD_QUEUEHEAD_DESCRIPTOR_PAD;

    tdCount = bytes/sizeof(HCD_TRANSFER_DESCRIPTOR);
    EHCI_ASSERT(DeviceData, tdCount >= TDS_PER_CONTROL_ENDPOINT);

    if (EndpointParameters->TransferType == Control) {
        SET_FLAG(EndpointData->Flags, EHCI_EDFLAG_NOHALT);
    }

    EndpointData->TdList = (PHCD_TD_LIST) buffer;
    EndpointData->TdCount = tdCount;
    for (i=0; i<tdCount; i++) {
        // this function asserts 4kb alignment rule for the indivudual TDs
        EHCI_InitializeTD(DeviceData,
                          EndpointData,
                          &EndpointData->TdList->Td[i],
                          phys);

        // alignemnt will be maintained as long as HCD_QUEUEHEAD_DESCRIPTOR_PAD = HCD_TRANSFER_DESCRIPTOR
        // = 256
        EHCI_ASSERT(DeviceData, HCD_QUEUEHEAD_DESCRIPTOR_PAD == sizeof(HCD_TRANSFER_DESCRIPTOR));

        phys += sizeof(HCD_TRANSFER_DESCRIPTOR);
    }
    EndpointData->FreeTds = tdCount;

    EndpointData->QueueHead =
        EHCI_InitializeQH(DeviceData,
                          EndpointData,
                          qh,
                          qhPhys);

    if (Control) {

        // allocate a dummy TD for short tranfsers
        // the dummy TD is usd to mark the end of the cuurent transfer
        //
        setupTd = EHCI_ALLOC_TD(DeviceData, EndpointData);
        if (!setupTd) {
            goto EHCI_OpenBulkOrControlEndpointNoTds;
        }
        EndpointData->SetupTD = setupTd;

        // use data toggle in the TDs for control
        qh->HwQH.EpChars.DataToggleControl = HcEPCHAR_Toggle_From_qTD;
        EHCI_ASSERT(DeviceData, tdCount >= TDS_PER_CONTROL_ENDPOINT);

        qh->HwQH.CurrentTD.HwAddress = setupTd->PhysicalAddress;
        qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = EHCI_TERMINATE_BIT;
        qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = EHCI_TERMINATE_BIT;

        // QH: Active = 0, Halted = 0
        //
        token.ul = 0;
        qh->HwQH.Overlay.qTD.Token = token;

        setupTd->HwTD.Next_qTD.HwAddress = EHCI_TERMINATE_BIT;
        TRANSFER_DESCRIPTOR_PTR(setupTd->NextHcdTD) = NULL;
        setupTd->HwTD.AltNext_qTD.HwAddress = EHCI_TERMINATE_BIT;
        TRANSFER_DESCRIPTOR_PTR(setupTd->AltNextHcdTD) = NULL;

        // Setup TD: Active = 0, Halted = 0
        //
        token.ul = 0;
        setupTd->HwTD.Token = token;

        for (i=0; i<5; i++) {
            setupTd->HwTD.BufferPage[i].ul = setupTd->PhysicalAddress;
        }

        //SET_FLAG(dummyTd->Flags, TD_FLAG_DUMMY);

    } else {
        qh->HwQH.EpChars.DataToggleControl = HcEPCHAR_Ignore_Toggle;
        EHCI_ASSERT(DeviceData, tdCount >= TDS_PER_BULK_ENDPOINT);
        //qh->HwQH.EpChars.NakReloadCount = 4;
        EHCI_sMode_InitEndpoint(DeviceData, EndpointData);
    }

    return USBMP_STATUS_SUCCESS;


EHCI_OpenBulkOrControlEndpointNoTds:

    // Should never get here!
    USBPORT_BUGCHECK(DeviceData);
    return USBMP_STATUS_NO_RESOURCES;
}


VOID
EHCI_InsertQueueHeadInHWAsyncList(
     __inout PDEVICE_DATA               DeviceData,
     __inout PHCD_QUEUEHEAD_DESCRIPTOR  AsyncQhToInsert
    )
/*++

Routine Description:

    Insert an Asynchronous (Control or Bulk) QH into the hardware
    Asynchronous List which is reachable through the EHCI host
    controller ASYNCLISTADDR (Current Asynchronous List Address)
    Register.

    The hardware Asynchronous List looks like this:

    /----------------------------------------------------------\
    |                                                          |
    \--> [Persistent QH.HLink] --> [QH.HLink] --> [QH.HLink] --/

                               /|\
                                |

    AsyncQhToInsert will be inserted here, immediately after the
    Persistent QH.

Arguments:

--*/
{
    PHCD_QUEUEHEAD_DESCRIPTOR   persistentQh;
    PHCD_QUEUEHEAD_DESCRIPTOR   persistentQhDummy;
    PHCD_QUEUEHEAD_DESCRIPTOR   nextQh;
    HW_LINK_POINTER             newLink;
    PENDPOINT_DATA              EndpointData;
    KIRQL                       irql;

    if (!TEST_FLAG(AsyncQhToInsert->QhFlags, EHCI_QH_FLAG_IN_HW_ASYNC_SCHEDULE)) {

        // Get a pointer to the Persistent QH, i.e. the dummy QH that is
        // always in the Asynchronous schedule with the Head of Reclamation
        // List Flag set in the QH HC_ENDPOINT_CHARACTERSITICS.
        //
        persistentQh = DeviceData->AsyncQueueHead;
        persistentQhDummy = DeviceData->AsyncQueueHeadDummy;

        // Get a pointer the QH which is currently immediately following
        // the Persistent QH.
        //
        nextQh = QH_DESCRIPTOR_PTR(persistentQh->NextQh);

        if (DeviceData->AsyncQueueHeadDummyEnable &&
            nextQh == persistentQhDummy) {

            HW_LINK_POINTER hwLink;

            // If the next QH after the Persistent QH is the Persistent
            // QH Dummy then the Asynchronous schedule should currently
            // be empty except for the two Persistent QHs.

            // Debug verify that the software and hardware pointers
            // between the two Persistent QH are still cross linked.

            EHCI_ASSERT(DeviceData,
                        QH_DESCRIPTOR_PTR(persistentQh->PrevQh) ==
                        persistentQhDummy);

            EHCI_ASSERT(DeviceData,
                        QH_DESCRIPTOR_PTR(persistentQhDummy->NextQh) ==
                        persistentQh);

            EHCI_ASSERT(DeviceData,
                        QH_DESCRIPTOR_PTR(persistentQhDummy->PrevQh) ==
                        persistentQh);

            hwLink.HwAddress = persistentQhDummy->PhysicalAddress;
            SET_QH(hwLink.HwAddress);

            EHCI_ASSERT(DeviceData,
                        persistentQh->HwQH.HLink.HwAddress ==
                        hwLink.HwAddress);

            hwLink.HwAddress = persistentQh->PhysicalAddress;
            SET_QH(hwLink.HwAddress);

            EHCI_ASSERT(DeviceData,
                        persistentQhDummy->HwQH.HLink.HwAddress ==
                        hwLink.HwAddress);

            // Instead of inserting the AsyncQhToInsert between the
            // Persistent QH and the Persistent QH Dummy, only link the
            // AsyncQhToInsert to the Persistent QH, effectively
            // removing the Persistent QH Dummy from the Asynchronous
            // schedule.
            //
            nextQh = persistentQh;
        }

        // Set the HLink of AsyncQhToInsert to point to nextQh.
        //
        newLink.HwAddress = nextQh->PhysicalAddress;
        SET_QH(newLink.HwAddress);
        AsyncQhToInsert->HwQH.HLink.HwAddress = newLink.HwAddress;

        // Set the HLink of the Persistent QH to point to AsyncQhToInsert.
        //
        newLink.HwAddress = AsyncQhToInsert->PhysicalAddress;
        SET_QH(newLink.HwAddress);

        persistentQh->HwQH.HLink.HwAddress = newLink.HwAddress;

        // Link the AsyncQhToInsert software next and previous pointers to
        // nextQh and persistentQh.
        //
        QH_DESCRIPTOR_PTR(AsyncQhToInsert->NextQh) = nextQh;
        QH_DESCRIPTOR_PTR(AsyncQhToInsert->PrevQh) = persistentQh;

        // Link the persistentQh and nextQh software next and previous
        // pointers to AsyncQhToInsert.
        //
        QH_DESCRIPTOR_PTR(persistentQh->NextQh) = AsyncQhToInsert;
        QH_DESCRIPTOR_PTR(nextQh->PrevQh) = AsyncQhToInsert;

        SET_FLAG(AsyncQhToInsert->QhFlags, EHCI_QH_FLAG_IN_HW_ASYNC_SCHEDULE);

        if (DeviceData->OptimizeInactiveQhFlush != FALSE) {

            // Note down the current Async Schedule Enable Count which will later
            // be used to determine if the queue head needs a cache flush.
            //
            // If EndpointInScheduleCount doesn't match CurrentAsyncEnableCount
            // when the queue head is about to be released, it means that
            // the async schedule was at least once disabled completely and that
            // queue head was not added back to the active schedule since that time,
            // implying that it should no longer be in the HC cache
            //

            EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);

            EndpointData = ENDPOINT_DATA_PTR(AsyncQhToInsert->EndpointData);
            if (EndpointData) {
                EndpointData->EndpointInScheduleCount = DeviceData->CurrentAsyncEnableCount;
            }

            EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);
        }
    }
}


VOID
EHCI_RemoveQueueHeadFromHWAsyncList(
     __inout PDEVICE_DATA               DeviceData,
     __inout PHCD_QUEUEHEAD_DESCRIPTOR  AsyncQhToRemove
    )
/*++

Routine Description:

    Remove an Asynchronous (Control or Bulk) QH from the hardware
    Asynchronous List which is reachable through the EHCI host controller
    ASYNCLISTADDR (Current Asynchronous List Address) Register.

Arguments:

--*/
{
    PHCD_QUEUEHEAD_DESCRIPTOR   persistentQh;
    PHCD_QUEUEHEAD_DESCRIPTOR   persistentQhDummy;
    PHCD_QUEUEHEAD_DESCRIPTOR   nextQh;
    PHCD_QUEUEHEAD_DESCRIPTOR   prevQh;
    HW_LINK_POINTER             newLink;

    if (TEST_FLAG(AsyncQhToRemove->QhFlags, EHCI_QH_FLAG_IN_HW_ASYNC_SCHEDULE)) {

        // Get a pointer to the Persistent QH, i.e. the dummy QH that is
        // always in the Asynchronous schedule with the Head of Reclamation
        // List Flag set in the QH HC_ENDPOINT_CHARACTERSITICS.
        //
        persistentQh = DeviceData->AsyncQueueHead;
        persistentQhDummy = DeviceData->AsyncQueueHeadDummy;

        // The following code has been disabled for now due to a
        // possible indication that doing this could possibly
        // contribute to instances of the Interrupt on Async Advance
        // hanging issue.  The causal relationship here is tenuous at
        // best, but worth a try at this point to see if might possibly
        // help reduce occurrences of the Interrupt on Async Advance
        // hanging issue.
        //
        // Setting the HLink of AsyncQhToRemove to point to the
        // Persistent QH would seem to always be a safe operation to
        // do, but Section 4.8.2 Removing Queue Heads from Asynchronous
        // Schedule and Figure 4-10 Generic Queue Head Unlink Scenario
        // raise a slight question about this.
        //
        // "When the host controller observes the doorbell bit being set
        // to a one, it makes a note of the local reachable schedule
        // information. In this example, the local reachable schedule
        // information includes both queue heads (A & B). It is
        // sufficient that the host controller can set the status bit
        // (and clear the doorbell bit) as soon as it has traversed
        // beyond current reachable schedule information (i.e. traversed
        // beyond queue head (B) in this example)."
        //
        // Suppose the driver is unlinking (A) and sets the HLink of
        // (A) to point to the Persistent QH instead of leaving it
        // point to (B).  Could the HC now possibly advance from (A) to
        // the Persistent QH instead of advancing to (B) and get
        // confused about setting the Interrupt on Async Advance status
        // bit as it didn't immediately traverse beyond (B) ?
        //
        // The potential for this to be an actual race condition inside
        // the implementation of any specific EHCI controller is
        // completely unverified.

        //
        // Code that is now disabled:
        //

/*
        // Set the HLink of AsyncQhToRemove to point to the Persistent
        // QH.  This should guarantee that at any point in time after a
        // single Interrupt on Async Advance that the host controller is
        // accessing a QH that is currently reachable through the EHCI
        // host controller ASYNCLISTADDR (Current Asynchronous List
        // Address) Register.
        //
        newLink.HwAddress = persistentQh->PhysicalAddress;
        SET_QH(newLink.HwAddress);
        AsyncQhToRemove->HwQH.HLink.HwAddress = newLink.HwAddress;
*/

        // Get pointers to the next and previous QHs relative to
        // AsyncQhToRemove.
        //
        nextQh = QH_DESCRIPTOR_PTR(AsyncQhToRemove->NextQh);
        prevQh = QH_DESCRIPTOR_PTR(AsyncQhToRemove->PrevQh);

        if (DeviceData->AsyncQueueHeadDummyEnable &&
            nextQh == persistentQh && prevQh == persistentQh) {

            HW_LINK_POINTER hwLink;

            // If the next and previous QHs relative to the
            // AsyncQhToRemove are both equal to the Persistent QH then
            // the Asynchronous schedule currently contains only the
            // AsyncQhToRemove in addition to the Persistent QH.

            EHCI_ASSERT(DeviceData,
                        QH_DESCRIPTOR_PTR(persistentQh->PrevQh) ==
                        AsyncQhToRemove);

            EHCI_ASSERT(DeviceData,
                        QH_DESCRIPTOR_PTR(persistentQh->NextQh) ==
                        AsyncQhToRemove);

            hwLink.HwAddress = persistentQh->PhysicalAddress;
            SET_QH(hwLink.HwAddress);

            EHCI_ASSERT(DeviceData,
                        persistentQhDummy->HwQH.HLink.HwAddress ==
                        hwLink.HwAddress);

            // Link the Persistent QH hardware pointer back to the
            // Persistent QH Dummy again as the only two Queue Heads in
            // the Asynchronous schedule.
            //
            prevQh = persistentQh;
            nextQh = persistentQhDummy;

            QH_DESCRIPTOR_PTR(persistentQh->PrevQh) = persistentQhDummy;
        }

        // Set the HLink of prevQh to point to nextQh to link the HW
        // pointers around AsyncQhToRemove.
        //
        newLink.HwAddress = nextQh->PhysicalAddress;
        SET_QH(newLink.HwAddress);
        prevQh->HwQH.HLink.HwAddress = newLink.HwAddress;

        // Link the prevQh and nextQh software next and previous
        // pointers to point to each other.
        //
        QH_DESCRIPTOR_PTR(prevQh->NextQh) = nextQh;
        QH_DESCRIPTOR_PTR(nextQh->PrevQh) = prevQh;

        CLEAR_FLAG(AsyncQhToRemove->QhFlags, EHCI_QH_FLAG_IN_HW_ASYNC_SCHEDULE);
    }
}


VOID
EHCI_InsertQueueHeadInAsyncList(
     __inout PDEVICE_DATA DeviceData,
     __inout PHCD_QUEUEHEAD_DESCRIPTOR Qh
    )
/*++

Routine Description:

    Called by:

    EHCI_SetAsyncEndpointState() from EHCI_SetEndpointState() when a
    Control or Bulk endpoint is set to ENDPOINT_ACTIVE.

    EHCI_StartSendOnePacket() when a QH needs to be inserted into the
    host controller hardware schedule for test tool single packet
    transfer.

    This routine is always called with the USBPORT Fdo.CoreFunctionSpin
    spinlock held.

Arguments:

--*/
{
    PENDPOINT_DATA  epData;

    LOGENTRY(DeviceData, G, '_Ain', 0, Qh, 0);

    EHCI_ASSERT(DeviceData, !TEST_FLAG(Qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE));

    epData = ENDPOINT_DATA_PTR(Qh->EndpointData);

    if (epData) {

        EHCI_ASSERT(DeviceData, epData->Sig == SIG_EP_DATA);
        EHCI_ASSERT(DeviceData, epData->QueueHead == Qh);

        if (!DeviceData->AsyncQhIdleRemoval || epData->SmodeTransfers > 0) {

            // If Async Qh idle removal is not enabled then do the
            // actual HW schedule insertion now instead of waiting for
            // EHCI_IncPendingTransfer() to insert the Qh.
            //
            // Also if there are already transfers pending on the Qh
            // then do the actual HW schedule insertion now.
            //
            EHCI_InsertQueueHeadInHWAsyncList(DeviceData, Qh);
        }

    } else {

        // QH with no Endpoint Data for test tool single packet
        // transfer.
        //
        EHCI_ASSERT(DeviceData, Qh->Sig == SIG_HCD_DQH);

        // Do the actual HW schedule insertion.
        //
        EHCI_InsertQueueHeadInHWAsyncList(DeviceData, Qh);
    }

    SET_FLAG(Qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE);

    // Insertion is considered immediate.
    //
    if (epData) {
        epData->StateChangeTc.QuadPart = 0;
        epData->HwSyncState = Qh_Not_Busy;
    }
}


VOID
EHCI_RemoveQueueHeadFromAsyncList(
     __inout PDEVICE_DATA DeviceData,
     __inout PHCD_QUEUEHEAD_DESCRIPTOR Qh
    )
/*++

Routine Description:

    Called by:

    EHCI_SetAsyncEndpointState() from EHCI_SetEndpointState() when a
    Control or Bulk endpoint is set to ENDPOINT_PAUSE or
    ENDPOINT_REMOVE.

    EHCI_EndSendOnePacket() when a QH needs to be removed from the host
    controller hardware schedule for test tool single packet transfer.

    This routine is always called with the USBPORT Fdo.CoreFunctionSpin
    spinlock held.

Arguments:

--*/
{
    PENDPOINT_DATA              epData;
    KIRQL                       irql;

    LOGENTRY(DeviceData, G, '_rmA', Qh, 0, 0);

    // Quick exit if the QH is not currently in the schedule.
    //
    if (!TEST_FLAG(Qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE)) {
        LOGENTRY(DeviceData, G, '_rA1', Qh, 0, 0);
        EHCI_ASSERT(DeviceData, !TEST_FLAG(Qh->QhFlags, EHCI_QH_FLAG_IN_HW_ASYNC_SCHEDULE));
        return;
    }

    epData = ENDPOINT_DATA_PTR(Qh->EndpointData);

    if (epData) {

        EHCI_ASSERT(DeviceData, epData->Sig == SIG_EP_DATA);
        EHCI_ASSERT(DeviceData, epData->QueueHead == Qh);

    } else {

        // QH with no Endpoint Data for test tool single packet
        // transfer.
        //
        EHCI_ASSERT(DeviceData, Qh->Sig == SIG_HCD_DQH);
    }

    // Do the actual HW schedule removal.
    //
    EHCI_RemoveQueueHeadFromHWAsyncList(DeviceData, Qh);

    CLEAR_FLAG(Qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE);

    // Flush the HW cache after an unlink.  The schedule should be
    // enabled if we are removing a QH.
    //
    // Skip the flush for the test tool single packet transfer case.
    //
    if (epData) {
        EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);
        EHCI_iAsyncCacheFlush(DeviceData, epData, FALSE);
        EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);
    }
}


// figure out which sgentry a particular offset in to
// a client buffer falls
#define GET_SG_INDEX(sg, i, offset)\
    for((i)=0; (i) < (sg)->SgCount; (i)++) {\
        if ((offset) >= (sg)->SgEntry[(i)].StartOffset &&\
            (offset) < (sg)->SgEntry[(i)].StartOffset+\
                (sg)->SgEntry[(i)].Length) {\
            break;\
        }\
    }

#define GET_SG_OFFSET(sg, i, offset, sgoffset)\
    (sgoffset) = (offset) - (sg)->SgEntry[(i)].StartOffset


ULONG
EHCI_MapAsyncTransferToTd(
    __inout PDEVICE_DATA DeviceData,
    ULONG MaxPacketSize,
    ULONG LengthMapped,
    __inout_opt PULONG NextToggle,
    __inout PTRANSFER_CONTEXT TransferContext,
    __inout PHCD_TRANSFER_DESCRIPTOR Td,
    __inout PTRANSFER_SG_LIST SgList
    )
/*++

Routine Description:

    Maps a data buffer to TDs according to EHCI rules

    An EHCI TD can cover up to 20k with 5 page crossing.
    Note that 20k is the most data a single td can describe

    Each sg entry represents one 4k EHCI 'page'

x = pagebreak
c = current ptr
b = buffer start
e = buffer end


    {..sg[sgIdx]..}
b...|---
    x--c----
    [  ]
        \
         sgOffset
[      ]
        \
         LengthMapped


Worst case for 20k transfer has 5 page breaks and requires
but 6 bp entries

    {..sg0..}{..sg1..}{..sg2..}{..sg3..}{..sg4..}{..sg5..}
    |        |        |        |        |        |        |
    x--------x--------x--------x--------x--------x--------x
        b-------------------------------------------->e
    <..bp0..><..bp1..><..bp2..><..bp3..><..bp4..>



case 1: (<6 sg entries remain)
    (A)- transfer < 16k,  page breaks (if c=b sgOffset = 0)

    {..sg0..}{..sg1..}{..sg2..}{..sg3..}{..sg4..}
    |        |        |        |        |        |
    x--------x--------x--------x--------x--------x
        b------------------------------------>e
    <..bp0..><..bp1..><..bp2..><..bp3..><..bp4..>
        [.................iTD.................]

    (B)- last part of a transfer

             {..sgN..}{.sgN+1.}{.sgN+2.}{.sgN+3.}{.sgN+4.}
             |        |        |        |        |        |
             x--------x--------x--------x--------x--------x
       b.....|.c------------------------------------->e
             <..bp0..><..bp1..><..bp2..><..bp3..><..bp4..>
               [..................iTD.................]

case 2:  (>5 sg entries remain)
    (A)- transfer > 20k, first part of a large transfer

    {..sg0..}{..sg1..}{..sg2..}{..sg3..}{..sg4..}{..sg5..}
    |        |        |        |        |        |        |
    x--------x--------x--------x--------x--------x--------x
        b-------------------------------------------->e
    <..bp0..><..bp1..><..bp2..><..bp3..><..bp4..>
        [....................iTD................]

    (B)- continuation of a large transfer


Interesting DMA tests (USBTEST):

    length, offset - cases hit


Arguments:

Returns:

    LengthMapped

--*/
{
    ULONG sgIdx, sgOffset, bp, i;
    ULONG lengthThisTd;
    PTRANSFER_PARAMETERS tp;
    HC_QTD_TOKEN token;

    // A TD can have up to 5 page crossing.  This means we
    // can put 5 sg entries in to one TD.

    // point to first entry

    LOGENTRY(DeviceData, G, '_Mpr', TransferContext,
        0, LengthMapped);

    EHCI_ASSERT(DeviceData, SgList->SgCount != 0);

    tp = TransferContext->TransferParameters;

    GET_SG_INDEX(SgList, sgIdx, LengthMapped);
    LOGENTRY(DeviceData, G, '_Mpp', SgList, 0, sgIdx);
    EHCI_ASSERT(DeviceData, sgIdx < SgList->SgCount);

    if ((SgList->SgCount-sgIdx) < 6) {
        // first case, <6 entries left
        // ie <20k, we can fit this in
        // a single TD.

#if DBG
        if (sgIdx == 0) {
            // case 1A
            // USBT dma test length 4096, offset 0
            // will hit this case

            LOGENTRY(DeviceData, G, '_c1a', SgList, 0, sgIdx);
        } else {
            // case 1B
            // USBT dma test length 8192 offset 512
            // will hit this case
            LOGENTRY(DeviceData, G, '_c1b', SgList, 0, sgIdx);

        }
#endif
        lengthThisTd = tp->TransferBufferLength - LengthMapped;

        // compute offset into this TD
        GET_SG_OFFSET(SgList, sgIdx, LengthMapped, sgOffset);
        LOGENTRY(DeviceData, G, '_sgO', sgOffset, sgIdx, LengthMapped);

        // adjust for the amount of buffer consumed by the
        // previous TD

        // sets current offset and address at the same time
        Td->HwTD.BufferPage[0].ul =
            SgList->SgEntry[sgIdx].LogicalAddress.Hw64.u.LowPart + sgOffset;

        Td->HwTD.BufferPage64[0] =
            SgList->SgEntry[sgIdx].LogicalAddress.Hw64.u.HighPart;

        i = sgIdx+1;
        for (bp = 1; bp < 5 && i < SgList->SgCount; bp++,i++) {
            Td->HwTD.BufferPage[bp].ul =
                SgList->SgEntry[i].LogicalAddress.Hw64.u.LowPart;

            Td->HwTD.BufferPage64[bp] =
                SgList->SgEntry[i].LogicalAddress.Hw64.u.HighPart;

            EHCI_ASSERT(DeviceData, Td->HwTD.BufferPage[bp].CurrentOffset == 0);
        }

        LOGENTRY(DeviceData, G, '_sg1', Td->HwTD.BufferPage[0].ul,
                 Td->HwTD.BufferPage64[0], 0);

    } else {
        // second case, >=6 entries left
        // we will need more than one TD
        ULONG adjust, packetCount;
#if DBG
        if (sgIdx == 0) {
            // case 2A
            // USBT dma test length 8192 offset 512
            // will hit this case
            LOGENTRY(DeviceData, G, '_c2a', SgList, 0, sgIdx);

        } else {
            // case 2B
            // USBT dma test length 12288 offset 1
            // will hit this case
            LOGENTRY(DeviceData, G, '_c2b', SgList, 0, sgIdx);

        }
#endif
        // sg offset is the offset in to the current TD to start
        // using
        // ie it is the number of bytes already consumed by the
        // previous td
        GET_SG_OFFSET(SgList, sgIdx, LengthMapped, sgOffset);
        LOGENTRY(DeviceData, G, '_sgO', sgOffset, sgIdx, LengthMapped);
#if DBG
        if (sgIdx == 0) {
             EHCI_ASSERT(DeviceData, sgOffset == 0);
        }
#endif
        //
        // consume the next 4 sgEntries
        //

        // sets currentOffset at the same time
        Td->HwTD.BufferPage[0].ul =
            SgList->SgEntry[sgIdx].LogicalAddress.Hw64.u.LowPart + sgOffset;

        Td->HwTD.BufferPage64[0] =
            SgList->SgEntry[sgIdx].LogicalAddress.Hw64.u.HighPart;

        lengthThisTd = EHCI_PAGE_SIZE - Td->HwTD.BufferPage[0].CurrentOffset;

        i = sgIdx+1;
        for (bp = 1; bp < 5; bp++,i++) {
            Td->HwTD.BufferPage[bp].ul =
                SgList->SgEntry[i].LogicalAddress.Hw64.u.LowPart;

            Td->HwTD.BufferPage64[bp] =
                SgList->SgEntry[i].LogicalAddress.Hw64.u.HighPart;

            EHCI_ASSERT(DeviceData, Td->HwTD.BufferPage[bp].CurrentOffset == 0);
            EHCI_ASSERT(DeviceData, i < SgList->SgCount);
            lengthThisTd += EHCI_PAGE_SIZE;
        }

        // round TD length down to the highest multiple
        // of max_packet size

        packetCount = lengthThisTd/MaxPacketSize;
        LOGENTRY(DeviceData, G, '_sg2', MaxPacketSize, packetCount, lengthThisTd);

        adjust = lengthThisTd - packetCount*MaxPacketSize;

        if (adjust) {
            lengthThisTd-=adjust;
            LOGENTRY(DeviceData, G, '_adj', adjust, lengthThisTd, 0);
        }

        if (NextToggle) {
        // calculate next data toggle if requested
        // two cases
        // case 1: prev NextToggle is 1
            // if number of packets is odd the nextToggle is 0
            // otherwise it is 1
        // case 2: prev NextToggle is 0
            // if number of packets is odd the nextToggle is 1
            // otherwise it is 0

        // so if packet count is even the value remains unchanged
        // otherwise we have to toggle it.
            if (packetCount % 2) {
                // packet count this TD is odd
                *NextToggle = (*NextToggle) ? 0 : 1;
            }
        }

        EHCI_ASSERT(DeviceData, lengthThisTd != 0);
        EHCI_ASSERT(DeviceData, lengthThisTd >= SgList->SgEntry[sgIdx].Length);
    }

    LengthMapped += lengthThisTd;

    token = Td->HwTD.Token;
    token.BytesToTransfer = lengthThisTd;
    Td->HwTD.Token = token;

    Td->TransferLength = lengthThisTd;

    LOGENTRY(DeviceData, G, '_Mp1', LengthMapped, lengthThisTd, Td);

    return LengthMapped;
}


VOID
EHCI_SetAsyncEndpointState(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     MP_ENDPOINT_STATE NewState
    )
/*++

Routine Description:

    This routine is only called by EHCI_SetEndpointState().

    This routine is only called for Control, Bulk, and Interrupt
    endpoints.  This routine is not called for Isochronous endpoints.

    This routine is always called with the USBPORT Fdo.CoreFunctionSpin
    spinlock held.

Arguments:

Return Value:

--*/
{
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    ENDPOINT_TRANSFER_TYPE epType;

    qh = EndpointData->QueueHead;

    epType = EndpointData->Parameters.TransferType;

    LOGENTRY(DeviceData, G, '_sae', qh, EndpointData, 0);

    // The EndpointData->StateChangeTc will be initialized to the
    // current value of KeQueryTickCount() in the codes paths where
    // EHCI_iAsyncCacheFlush() calls EHCI_RingAsyncDoorbell().
    //
    EndpointData->StateChangeTc.QuadPart = 0;

    switch (NewState) {
        case ENDPOINT_ACTIVE:
            if (epType == Interrupt) {
                // Insert the Interrupt QH into the periodic schedule.
                EHCI_InsertQueueHeadInPeriodicList(DeviceData,
                    EndpointData);

            } else {
                // Insert the Control or Bulk QH into the async schedule.
                EHCI_InsertQueueHeadInAsyncList(DeviceData,
                                                EndpointData->QueueHead);
            }
            break;

        case ENDPOINT_PAUSE:
            if (epType == Interrupt) {
                // Remove the Interrupt QH from the periodic schedule.
                EHCI_RemoveQueueHeadFromPeriodicList(DeviceData,
                    EndpointData);
            } else {
                // Remove the Control or Bulk QH from the async schedule.
                EHCI_RemoveQueueHeadFromAsyncList(DeviceData,
                    EndpointData->QueueHead);
            }
            break;

        case ENDPOINT_REMOVE:
            qh->QhFlags |= EHCI_QH_FLAG_QH_REMOVED;

            if (epType == Interrupt) {
                // Remove the Interrupt QH from the periodic schedule.
                EHCI_RemoveQueueHeadFromPeriodicList(DeviceData,
                    EndpointData);
            } else {
                // Remove the Control or Bulk QH from the async schedule.
                EHCI_RemoveQueueHeadFromAsyncList(DeviceData,
                    EndpointData->QueueHead);
            }
            break;

        default:
            // invalid endppoint state, we will likely crash
            EHCI_ASSERTMSG(DeviceData, "Invalid Endpoint State", FALSE);
    }

    //EndpointData->State = State;
}


MP_ENDPOINT_STATUS
EHCI_GetAsyncEndpointStatus(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    HC_QTD_TOKEN token;
    MP_ENDPOINT_STATUS status;

    // Get the QH and the current value of the QH Token from the Overlay
    //
    qh = EndpointData->QueueHead;
    token = qh->HwQH.Overlay.qTD.Token;

    status = EndpointData->EpStatus;

    if (EndpointData->EpStatus == ENDPOINT_STATUS_HALT) {
        EHCI_ASSERT(DeviceData, token.Halted == 1);
    }

    LOGENTRY(DeviceData, G, '_gps', EndpointData, status, 0);

    return status;
}


VOID
EHCI_SetAsyncEndpointStatus(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     MP_ENDPOINT_STATUS Status
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    HC_QTD_TOKEN token;
    PENDPOINT_SLOT slot;

    // Get the QH and the current value of the QH Token from the Overlay
    //
    qh = EndpointData->QueueHead;
    token = qh->HwQH.Overlay.qTD.Token;

    LOGENTRY(DeviceData, G, '_se0', EndpointData, Status, 0);

    switch(Status) {
    case ENDPOINT_STATUS_RUN:
        if (EndpointData->EpStatus == ENDPOINT_STATUS_HALT) {
            EndpointData->EpStatus = ENDPOINT_STATUS_RUN;
            EHCI_ASSERT(DeviceData, token.Halted == 1);
            // fix up queue head currentTD before going to run state
            slot = EndpointData->CurrentSlot;

            LOGENTRY(DeviceData, G, '_se1', EndpointData, slot, qh);

            if (slot != NULL) {
                LOGENTRY(DeviceData, G, '_se2', EndpointData, slot, qh);
                EHCI_ASSERT(DeviceData, slot->State == slot_Busy);
                EHCI_sMode_ActivateQh(DeviceData, EndpointData, slot);

                // EHCI_sMode_ActivateQh() updates the QH Token but
                // does not clear the Halted bit.  Refresh the Token
                // value and clear the Halted bit.
                //
                token = qh->HwQH.Overlay.qTD.Token;
                token.Halted = 0;
                qh->HwQH.Overlay.qTD.Token = token;

            } else {
                EHCI_ASSERT(DeviceData, token.Active == 0);
                // just clear halted bit
                token.Halted = 0;
                qh->HwQH.Overlay.qTD.Token = token;
            }

        } else {
            EHCI_ASSERT(DeviceData, EndpointData->EpStatus == ENDPOINT_STATUS_RUN);

            // Clear the QH Halted bit.
            //
            // It is possible for an Active transfer on the QH to become
            // Halted simultaneously with the endpoint being set to
            // ENDPOINT_PAUSE to cancel the currently Active transfer.
            //
            // In that case the Halted QH will not be detected unless an
            // additional transfer is queued on the QH and the endpoint
            // is polled.
            //
            // This code path can be hit if the endpoint is restored by
            // USBPORT_CloneDevice() before the Halted QH is detected.
            //
            token.Halted = 0;
            qh->HwQH.Overlay.qTD.Token = token;
        }
        break;

    case ENDPOINT_STATUS_HALT:
        EndpointData->EpStatus = ENDPOINT_STATUS_HALT;
        TEST_TRAP(DeviceData);
        EHCI_ASSERTMSG(DeviceData, "Invalid Endpoint Status", FALSE);
        // hw won't let us do this
        // qh->HwQH.Overlay.qTD.Token.Halted = 1;

        break;
    }
}


VOID
EHCI_ProcessDoneAsyncTd(
    __inout PDEVICE_DATA DeviceData,
    __inout PHCD_TRANSFER_DESCRIPTOR Td,
    __inout_opt PTRANSFER_CONTEXT AbortTransferContext
    )
/*++

Routine Description:

    process a completed TD

Parameters

--*/
{
    PTRANSFER_CONTEXT transferContext;
    PENDPOINT_DATA endpointData;
    PTRANSFER_PARAMETERS tp;
    HC_QTD_TOKEN token;
    USBD_STATUS usbdStatus;
    ULONG byteCount;

    transferContext = TRANSFER_CONTEXT_PTR(Td->TransferContext);
    ASSERT_TRANSFER(DeviceData, transferContext);

    tp = transferContext->TransferParameters;
    transferContext->PendingTds--;
    endpointData = transferContext->EndpointData;

    if (TEST_FLAG(Td->Flags, TD_FLAG_SKIP)) {
        LOGENTRY(DeviceData, G, '_Ktd', transferContext,
                         0,
                         Td);

        goto free_it;
    }

    token = Td->HwTD.Token;

    // completion status for this TD?
    // since the endpoint halts on error the  error
    // bits should have been written back to the TD
    // we use these bits to dermine the error
    if (token.Halted == 1) {
        usbdStatus = EHCI_GetErrorFromTD(DeviceData,
                                         endpointData,
                                         Td);
    } else {
        usbdStatus = USBD_STATUS_SUCCESS;
    }

    LOGENTRY(DeviceData, G, '_Dtd', transferContext,
                         usbdStatus,
                         Td);

    byteCount = EHCI_TdBytesTransferred(Td->TransferLength, token.BytesToTransfer);

    LOGENTRY(DeviceData, G, '_tln', byteCount,
             Td->TransferLength, token.BytesToTransfer);

    if (token.Pid != HcTOK_Setup) {

        // data or status phase of a control transfer or a bulk/int
        // data transfer
        LOGENTRY(DeviceData, G, '_Idt', Td, transferContext, byteCount);

        transferContext->BytesTransferred += byteCount;
    }

    // note that we only set transferContext->UsbdStatus
    // if we find a TD with an error this will cause us to
    // record the last TD with an error as the error for
    // the transfer.
    if (USBD_STATUS_SUCCESS != usbdStatus) {

        // map the error to code in USBDI.H
        transferContext->UsbdStatus =
            usbdStatus;

        LOGENTRY(DeviceData, G, '_tER', transferContext->UsbdStatus, 0, 0);
    }

free_it:

    if (!TEST_FLAG(Td->Flags, TD_FLAG_CONTROL_SKIP)) {

        // mark the TD free
        EHCI_FREE_TD(DeviceData, endpointData, Td);
    }

    if (transferContext->PendingTds == 0) {
        PHCD_QUEUEHEAD_DESCRIPTOR qh;
        qh = endpointData->QueueHead;

        // all TDs for this transfer are done
        // clear the HAVE_TRANSFER flag to indicate
        // we can take another
        EHCI_DecPendingTransfer(DeviceData, endpointData, transferContext);

// FOR DEBUG ONLY
//if (transferContext->BytesTransferred == 0 &&
//    endpointData->Parameters.TransferType == Bulk) {
//    TEST_TRAP(DeviceData);
//}
        LOGENTRY(DeviceData, G, '_cpt',
            transferContext->UsbdStatus,
            transferContext,
            transferContext->BytesTransferred);

        if (DeviceData->TerminateQH) {
            qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = EHCI_TERMINATE_BIT;
            qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = EHCI_TERMINATE_BIT;
        }

        if (AbortTransferContext == NULL) {

            USBPORT_COMPLETE_TRANSFER(DeviceData,
                                      endpointData,
                                      tp,
                                      transferContext->UsbdStatus,
                                      transferContext->BytesTransferred);
        } else {
            // if this is an abort_transfer request then we don't need to indicate
            // completion
            EHCI_ASSERT(DeviceData, transferContext == AbortTransferContext);
        }
    }
}


USBD_STATUS
EHCI_GetErrorFromTD(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    __in PHCD_TRANSFER_DESCRIPTOR Td
    )
/*++

Routine Description:

    Maps the error bits in the TD to a USBD_STATUS code

Arguments:

Return Value:

--*/

{
    HC_QTD_TOKEN token;

    token = Td->HwTD.Token;

    LOGENTRY(DeviceData, G, '_eTD', token.ul, Td, 0);

    EHCI_ASSERT(DeviceData, token.Halted == 1);
        //ULONG MissedMicroFrame:1; // 2
        //ULONG XactErr:1;          // 3
        //ULONG BabbleDetected:1;   // 4
        //ULONG DataBufferError:1;  // 5

    if (token.XactErr) {
        LOGENTRY(DeviceData, G, '_mp1', 0, 0, 0);

        return USBD_STATUS_XACT_ERROR;
    }

    if (token.BabbleDetected) {
        LOGENTRY(DeviceData, G, '_mp2', 0, 0, 0);

        return USBD_STATUS_BABBLE_DETECTED;
    }

    if (token.DataBufferError) {
        LOGENTRY(DeviceData, G, '_mp3', 0, 0, 0);

        return USBD_STATUS_DATA_BUFFER_ERROR;
    }

    if (token.MissedMicroFrame) {
        LOGENTRY(DeviceData, G, '_mp6', 0, 0, 0);
        return USBD_STATUS_XACT_ERROR;
    }

    // no bit set -- treat as a stall
    LOGENTRY(DeviceData, G, '_mp4', 0, 0, 0);
    return USBD_STATUS_STALL_PID;
}


VOID
EHCI_AbortControlTransfer(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PTRANSFER_CONTEXT AbortTransferContext
    )
/*++

Routine Description:

    Called when the endpoint 'needs attention'

    The goal here is to determine which TDs, if any,
    have completed and complete ant associated transfers

Arguments:

Return Value:

--*/
{
    PHCD_TRANSFER_DESCRIPTOR td, currentTd, statusTd, setupTd;
    HC_QTD_TOKEN token;
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    HW_32BIT_PHYSICAL_ADDRESS curTdPhys;
    PTRANSFER_CONTEXT transfer;

    qh = EndpointData->QueueHead;

    // The endpoint should not be in the schedule

    LOGENTRY(DeviceData, G, '_abr', qh, AbortTransferContext, EndpointData->SetupTD);
    EHCI_ASSERT(DeviceData, !TEST_FLAG(qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE));

    TEST_TRAP(DeviceData);

    // abort the current control transfer, if we have one.

    // should have one transfer
    EHCI_ASSERT(DeviceData, !IsListEmpty(&EndpointData->SmodeTransferListHead));

    curTdPhys =  qh->HwQH.CurrentTD.HwAddress & ~HW_LINK_FLAGS_MASK;

    EHCI_ASSERT(DeviceData, curTdPhys != 0);
    currentTd = (PHCD_TRANSFER_DESCRIPTOR)
                    USBPORT_PHYSICAL_TO_VIRTUAL(curTdPhys,
                                                DeviceData,
                                                EndpointData);

    statusTd = EndpointData->StatusTD;
    setupTd = EndpointData->SetupTD;
    EHCI_ASSERT(DeviceData, statusTd);
    EHCI_ASSERT(DeviceData, setupTd);
    LOGENTRY(DeviceData, G, '+ap1', statusTd, setupTd,  currentTd);

    // process all TDs that have the active bit still set

    InsertTailList(&EndpointData->DoneTdList, &setupTd->DoneLink);
    SET_FLAG(setupTd->Flags, TD_FLAG_CONTROL_SKIP);

    // De-Activate the Setup Phase HW_QUEUE_ELEMENT_TD
    //
    token = setupTd->HwTD.Token;
    token.Active = 0;
    setupTd->HwTD.Token = token;

    InsertTailList(&EndpointData->DoneTdList, &statusTd->DoneLink);
    SET_FLAG(statusTd->Flags, TD_FLAG_CONTROL_SKIP);

    // De-Activate the Status Phase HW_QUEUE_ELEMENT_TD
    //
    token = statusTd->HwTD.Token;
    token.Active = 0;
    statusTd->HwTD.Token = token;

    // process the data phase here
    td = TRANSFER_DESCRIPTOR_PTR(setupTd->NextHcdTD);
    LOGENTRY(DeviceData, G, '_ap3', td, 0, currentTd);

    while (td != statusTd) {

        EHCI_ASSERT(DeviceData, !TEST_FLAG(td->Flags, TD_FLAG_DUMMY));
        EHCI_ASSERT(DeviceData, !TEST_FLAG(td->Flags, TD_FLAG_CONTROL_SKIP));
        // TDs between head and current should not be active

        transfer = TRANSFER_CONTEXT_PTR(td->TransferContext);
        LOGENTRY(DeviceData, G, '_at1', td, 0, transfer);

        token = td->HwTD.Token;
        if (token.Active == 1) {
            // if the TD is active then it must have been
            // skipped due to a short xfer condition
            LOGENTRY(DeviceData, G, '_atS', td, 0, 0);
            SET_FLAG(td->Flags, TD_FLAG_SKIP);
        }

        SET_FLAG(td->Flags, TD_FLAG_DONE);

        InsertTailList(&EndpointData->DoneTdList, &td->DoneLink);

        td = TRANSFER_DESCRIPTOR_PTR(td->NextHcdTD);
    }

    // complete the current transfer, this will update the length and error code
    while (!IsListEmpty(&EndpointData->DoneTdList)) {
        PLIST_ENTRY le;

        le = RemoveHeadList(&EndpointData->DoneTdList);

        td = (PHCD_TRANSFER_DESCRIPTOR) CONTAINING_RECORD(
                     le,
                     struct _HCD_TRANSFER_DESCRIPTOR,
                     DoneLink);

        EHCI_ASSERT(DeviceData, (td->Flags & (TD_FLAG_XFER | TD_FLAG_DONE)));
        EHCI_ProcessDoneAsyncTd(DeviceData, td, AbortTransferContext);
    }

    EHCI_ASSERT(DeviceData, AbortTransferContext->PendingTds == 0);

    // reset qh overlay to point at setup/status TD
    qh->HwQH.CurrentTD.HwAddress = statusTd->PhysicalAddress;

    if (!DeviceData->TerminateQH) {
        qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = setupTd->PhysicalAddress;
        qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = setupTd->PhysicalAddress;
    }

    // Re-Initialize the Token in the QH Overlay
    //
    token = qh->HwQH.Overlay.qTD.Token;
    token.BytesToTransfer = 0;

    // if this is a control endpoint the we need to clear the
    // halt condition
    if (TEST_FLAG(EndpointData->Flags, EHCI_EDFLAG_NOHALT)) {
        LOGENTRY(DeviceData, G, '_hp4', qh, 0, 0);

        EndpointData->EpStatus = ENDPOINT_STATUS_RUN;

        token.Active = 0;
        token.Halted = 0;
        token.ErrorCounter = 3;
    }

    qh->HwQH.Overlay.qTD.Token = token;
}


USB_MINIPORT_STATUS
EHCI_PokeAsyncEndpoint(
     __in PDEVICE_DATA DeviceData,
     __in PENDPOINT_PARAMETERS EndpointParameters,
     __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PHCD_QUEUEHEAD_DESCRIPTOR qh;

    qh = EndpointData->QueueHead;
    EHCI_ASSERT(DeviceData, qh != NULL);

    EndpointData->Parameters = *EndpointParameters;

    qh->HwQH.EpChars.DeviceAddress =
        EndpointData->Parameters.DeviceAddress;

    qh->HwQH.EpChars.MaximumPacketLength =
        EndpointData->Parameters.MaxPacketSize;

    if (EndpointData->Parameters.DeviceSpeed == HcEPCHAR_HighSpeed) {
        qh->HwQH.EpCaps.HubAddress =  0;
        qh->HwQH.EpCaps.PortNumber = 0;
    } else {
        qh->HwQH.EpCaps.HubAddress =
            EndpointData->Parameters.TtDeviceAddress;
    }

    return USBMP_STATUS_SUCCESS;
}


_Requires_lock_not_held_(DeviceData->AcfListSpin) _Acquires_lock_(DeviceData->AcfListSpin)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_raises_(DISPATCH_LEVEL)
VOID
EHCI_AcquireAsyncDoorbellLock(
     _Inout_ PDEVICE_DATA DeviceData,
     _Out_ __deref _IRQL_saves_ PKIRQL Irql
     )
/*++

Routine Description:

    This routine acquires the AcfListSpin spinlock which provides
    synchronized access to enabling and disabling the host controller
    Asynchronous Schedule, setting and responding to the host controller
    Interrupt on Async Advance Doorbell request, and updating the
    associated host controller driver AcfListHead endpoint list.
    (Acf == Async Cache Flush).

    The AcfListSpin spinlock also provides synchronized access to the
    AsyncIdleRefListHead as used by EHCI_RefAsyncIdle(),
    EHCI_DerefAsyncIdle(), and EHCI_iDisableAsyncList().

--*/
{
    // Real purpose here.
    //
    KeAcquireSpinLock(&DeviceData->AcfListSpin, Irql);

    // Debug checking.
    //
    EHCI_ASSERT(DeviceData, DeviceData->AcfLockThread == NULL);
    DeviceData->AcfLockThread = KeGetCurrentThread();
    EHCI_ASSERT(DeviceData, DeviceData->AcfLockThread != NULL);
}


_Requires_lock_held_(DeviceData->AcfListSpin) _Releases_lock_(DeviceData->AcfListSpin)
_IRQL_requires_(DISPATCH_LEVEL)
VOID
EHCI_ReleaseAsyncDoorbellLock(
     _Inout_ PDEVICE_DATA DeviceData,
     _In_ _IRQL_restores_ KIRQL Irql
     )
/*++

Routine Description:

    This routine releases the AcfListSpin spinlock which provides
    synchronized access to enabling and disabling the host controller
    Asynchronous Schedule, setting and responding to the host controller
    Interrupt on Async Advance Doorbell request, and updating the
    associated host controller driver AcfListHead endpoint list.
    (Acf == Async Cache Flush).

    The AcfListSpin spinlock also provides synchronized access to the
    AsyncIdleRefListHead as used by EHCI_RefAsyncIdle(),
    EHCI_DerefAsyncIdle(), and EHCI_iDisableAsyncList().

--*/
{
    // Debug checking.
    //
    EHCI_ASSERT(DeviceData, DeviceData->AcfLockThread != NULL);
    DeviceData->AcfLockThread = NULL;

    // Real purpose here.
    //
    KeReleaseSpinLock(&DeviceData->AcfListSpin, Irql);
}


VOID
EHCI_AnswerAsyncDoorbell(
     __inout PDEVICE_DATA DeviceData
     )
/*++

Routine Description:

    This routine is called by the host controller Interrupt DPC routine
    to process an Interrupt on Async Advance interrupt notification.

    The EpAtTheDoor endpoint is the endpoint that is currently waiting
    for the Interrupt on Async Advance interrupt notification.

    The HwSyncState of the EpAtTheDoor endpoint is changed from Qh_Busy
    to Qh_Not_Busy to indicate that the endpoint QueueHead should no
    longer be accessible by the host controller on the async schedule
    list and should now safe for updating or deletion by the host
    controller driver.

    If there are additional endpoints waiting on the AcfListHead list
    then EHCI_RingAsyncDoorbell() is called to request another
    Interrupt on Async Advance interrupt notification to process the
    next endpoint on the list.

    The AcfListSpin spinlock is acquired here and held around all of the
    processing done here.

--*/
{
    PENDPOINT_DATA epData;
    KIRQL irql;

    // Acquire the AcfListSpin spinlock around all of the processing
    // done here.
    //
    EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);

    epData = DeviceData->EpAtTheDoor;

    if (epData) {

        if (DeviceData->OptimizeInactiveQhFlush != FALSE) {

            // This is a non-fatal assert. Hitting it indicates that the optimization
            // for reducing the number of cache flushes isn't working as expected and
            // needs to be investigated. Continuing beyond the assert should allow the
            // system to work fine.
            //

            NT_ASSERTMSG("Endpoint should not have required a doorbell",
                         epData->EndpointInScheduleCount == DeviceData->CurrentAsyncEnableCount);
        }
    }

    DeviceData->StatAnswerDoorbell++;

    // Check to see if we received the doorbell after we temporarily enabled
    // the doorbell retry.
    //

    if (DeviceData->DoorbellRetryRequired == FALSE &&
        DeviceData->CurrentDoorbellRetryEnableValue != FALSE &&
        DeviceData->CurrentIntOnAsyncAdvancePendingCount == 0) {

        // This indicates that the Host Controller required Retry/multiple
        // doorbell requests to respond
        //

        // Enable retry for the rest of the boot cycle and log an entry
        // in the system event log for the SQM code to pick up and report
        //

        DeviceData->DoorbellRetryRequired = TRUE;
        EHCI_WriteErrorLogEntry(DeviceData,
                                USBEHCI_ASYNC_DOORBELL_RETRY_WAS_REQUIRED,
                                TRUE);

    }

    // Release the currently waiting EpAtTheDoor endpoint.
    //
    DeviceData->EpAtTheDoor = NULL;

    if (epData) {

        EHCI_ASSERT(DeviceData, epData->Sig == SIG_EP_DATA);

        epData->DoorbellCount--;

        if (epData->DoorbellCount == 0) {

            // We have received the required number of doorbell responses
            // for this queue head and hence the transition is complete
            // for this QH
            //

            EHCI_UpdateEndpointAsFreedFromHwCache(DeviceData, epData);

        } else {

            // We need to request more doorbell acknowledgements from the
            // HC for it to completely evict its cache. Here we add the
            // queue head to the head of the Acf list so that the next
            // call to EHCI_RingAsyncDoorbell will request a doorbell for this
            // queue head again.
            //

            InsertHeadList(&DeviceData->AcfListHead, &epData->AcfLink);
        }
    }

    EHCI_SetAsyncDoorbellState(DeviceData, AsyncDoorbellState_NotWaiting);

    // Process the next waiting endpoint on the AcfListHead list.
    //
    if (!IsListEmpty(&DeviceData->AcfListHead)) {
        EHCI_RingAsyncDoorbell(DeviceData);
    }

    EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);
}


BOOLEAN
EHCI_IsAsyncDoorbellPending(
     __inout PDEVICE_DATA DeviceData
    )
/*

Routine Description:

    Returns a boolean value to indicate whether there is currently
    an active async doorbell request pending.

    Caller is required to hold the Acf spinlock prior to calling
    this routine.

*/
{
    return (DeviceData->AsyncDoorbellState != AsyncDoorbellState_NotWaiting);
}


VOID
EHCI_ClearCurrentAsyncStats(
    __inout PDEVICE_DATA DeviceData
    )
/*

Routine Description:

    Clears the various temporary counters used to track the progress
    of the current async doorbell request.

*/
{
    DeviceData->CurrentRingDoorbellDisablingTimerCount = 0;
    DeviceData->CurrentRingDoorbellEnablingTimerCount = 0;
    DeviceData->CurrentRingDoorbellEnabledTimerCount = 0;
    DeviceData->CurrentIntOnAsyncAdvancePendingCount = 0;
}


_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_SetAsyncDoorbellState(
     __inout PDEVICE_DATA DeviceData,
     __in ASYNC_DOORBELL_STATE NextState
     )
/*

Routine Description:

    This routine updates the Async Schedule Doorbell state after doing
    the following:

    1. Validates that the transition from the CurrentState to the NextState
       is an expected one. If an unexpected transition is detected, it will
       assert (NT_ASSERTMSG, and not EHCI_ASSERTMSG as the latter causes a fatal
       bugcheck even in a retail build), but note that the state is set to the
       NextState if the execution continues beyond the assert.

    2. Update the state and the counters used to track the type of async
       doorbell wait

    3. If the state has transitioned from a non-waiting state to the waiting state,
       it will complete any pending enable/disable operation on the async schedule,
       periodic schedule and the periodic prefectch state that may have been blocked
       due to a pending doorbell request.

    Current implementation assumes that the caller knows the NextState irrespective
    of the current state and hence a direct SetState() method is used instead of
    driving the state transitions off events generated by the caller.

*/
{
    ASYNC_DOORBELL_STATE CurrentState;

    CurrentState = DeviceData->AsyncDoorbellState;

    switch (CurrentState) {

        case AsyncDoorbellState_NotWaiting:

            // Nothing to verify as this state can be entered from any state
            //

            break;

        case AsyncDoorbellState_PendingScheduleEnable:

            NT_ASSERTMSG("Invalid doorbell state transition",
                         NextState != AsyncDoorbellState_PendingScheduleDisable &&
                         NextState != AsyncDoorbellState_RequestedWithRetry);
            break;

        case AsyncDoorbellState_PendingScheduleDisable:

            NT_ASSERTMSG("Invalid doorbell state transition",
                         NextState != AsyncDoorbellState_PendingScheduleEnable &&
                         NextState != AsyncDoorbellState_RequestedWithRetry);
            break;

        case AsyncDoorbellState_Requested:

            NT_ASSERTMSG("Invalid doorbell state transition",
                         NextState != AsyncDoorbellState_PendingScheduleEnable &&
                         NextState != AsyncDoorbellState_PendingScheduleDisable);
            break;

        case AsyncDoorbellState_RequestedWithRetry:

            NT_ASSERTMSG("Invalid doorbell state transition",
                         NextState != AsyncDoorbellState_PendingScheduleEnable &&
                         NextState != AsyncDoorbellState_PendingScheduleDisable);
            break;

        default:

            NT_ASSERTMSG("Invalid doorbell state transition", FALSE);
            break;

    }

    switch (NextState) {

        // Increment the corresponding counter only for the first iteration,
        // as this routine can get called repeatedly for the same endpoint
        // or queue head as part of the frequent retry logic in async doorbell
        // handling. Further updates to these counters are done in
        // EHCI_CheckController() and are cleared in EHCI_ClearCurrentAsyncStats()
        //

        case AsyncDoorbellState_NotWaiting:

            break;

        case AsyncDoorbellState_PendingScheduleEnable:

            if (DeviceData->CurrentRingDoorbellEnablingTimerCount == 0) {
                DeviceData->CurrentRingDoorbellEnablingTimerCount++;
            }
            break;

        case AsyncDoorbellState_PendingScheduleDisable:

            if (DeviceData->CurrentRingDoorbellDisablingTimerCount == 0) {
                DeviceData->CurrentRingDoorbellDisablingTimerCount++;
            }
            break;

        case AsyncDoorbellState_Requested:

            if (DeviceData->CurrentRingDoorbellEnabledTimerCount == 0) {
                DeviceData->CurrentRingDoorbellEnabledTimerCount++;
            }
            break;

        case AsyncDoorbellState_RequestedWithRetry:

            // no action required for the retry case
            //

            break;

        default:

            NT_ASSERTMSG("Invalid doorbell state transition", FALSE);
            break;

    }

    LOGENTRY(DeviceData, G, 'ADCu', DeviceData->AsyncDoorbellState, NextState, 0);

    DeviceData->AsyncDoorbellState = NextState;

    // Clear "Current" stats for tracking doorbell response when we transition from
    // any non-waiting state to waiting state.
    //
    // Also see if there are any pending requests to enable/disable either the async
    // or the periodic schedule
    //

    if (CurrentState != AsyncDoorbellState_NotWaiting &&
        NextState == AsyncDoorbellState_NotWaiting) {

        EHCI_ClearCurrentAsyncStats(DeviceData);

        if (DeviceData->HcSuspendedFlag == 0) {

            if (DeviceData->AsyncScheduleState == AsyncScheduleState_EnablePendingDoorbell) {
                EHCI_iEnableAsyncList(DeviceData);
            } else if (DeviceData->AsyncScheduleState == AsyncScheduleState_DisablePendingDoorbell) {
                EHCI_iDisableAsyncList(DeviceData);
            }

            if (DeviceData->PeriodicScheduleState == PeriodicScheduleState_EnablePendingDoorbell) {
                EHCI_iEnablePeriodicList(DeviceData);
            } else if (DeviceData->PeriodicScheduleState == PeriodicScheduleState_DisablePendingDoorbell) {
                EHCI_iDisablePeriodicList(DeviceData);
            }
        }
    }
}


_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_UpdateScheduleStates(
     __inout PDEVICE_DATA DeviceData,
     __in USBCMD cmd,
     __in USBSTS sts
     )
/*

    Calls the UpdateScheduleState() routines for async and periodic
    schedules

*/
{
    EHCI_UpdateAsyncScheduleState(DeviceData, cmd, sts);
    EHCI_UpdatePeriodicScheduleState(DeviceData, cmd, sts);
}


_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_UpdateAsyncScheduleState(
     __inout PDEVICE_DATA DeviceData,
     __in USBCMD cmd,
     __in USBSTS sts
     )
/*

Routine Description:

    Updates the async schedule state based on the current state of the command
    and status registers.

    Async Doorbell Spinlock is held by the caller

*/
{
    LOGENTRY(DeviceData, G, 'ASUp', cmd.ul, sts.ul, 0);

    if (cmd.ul == 0xffffffff) {

        EHCI_SetAsyncScheduleState(DeviceData, AsyncScheduleState_Disabled);
        return;
    }

    if (sts.AsyncScheduleStatus == 0 && cmd.AsyncScheduleEnable == 0) {

        //
        // Schedule is disabled and no pending request to enable it from the
        // software
        //

        if (DeviceData->StrictTransitionChecksEnabled != FALSE) {
            NT_ASSERTMSG("Invalid state cached in software",
                         DeviceData->AsyncScheduleState != AsyncScheduleState_EnableRequested);
        }

        EHCI_SetAsyncScheduleState(DeviceData, AsyncScheduleState_Disabled);

    } else if (sts.AsyncScheduleStatus == 1 && cmd.AsyncScheduleEnable == 1) {

        //
        // Schedule is enabled and no pending request to disable it from the
        // software
        //

        if (DeviceData->StrictTransitionChecksEnabled != FALSE) {
            NT_ASSERTMSG("Invalid state cached in software",
                         DeviceData->AsyncScheduleState != AsyncScheduleState_DisableRequested);
        }

        EHCI_SetAsyncScheduleState(DeviceData, AsyncScheduleState_Enabled);

    }

    if (sts.AsyncScheduleStatus != cmd.AsyncScheduleEnable) {

        //
        // Schedule is in transition. Ensure that there was a correct prior request posted
        // to the hardware
        //

        if (cmd.AsyncScheduleEnable) {








            if (DeviceData->StrictTransitionChecksEnabled != FALSE) {
                NT_ASSERTMSG("Invalid state cached in software",
                             DeviceData->AsyncScheduleState == AsyncScheduleState_EnableRequested ||
                             DeviceData->AsyncScheduleState == AsyncScheduleState_EnablePendingDoorbell ||
                             DeviceData->AsyncScheduleState == AsyncScheduleState_DisablePendingDoorbell);
            }

        } else {

            if (DeviceData->StrictTransitionChecksEnabled != FALSE) {
                NT_ASSERTMSG("Invalid state cached in software",
                             DeviceData->AsyncScheduleState == AsyncScheduleState_DisableRequested ||
                             DeviceData->AsyncScheduleState == AsyncScheduleState_EnablePendingDoorbell ||
                             DeviceData->AsyncScheduleState == AsyncScheduleState_DisablePendingDoorbell);
            }

        }
    }
}

_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_SetAsyncScheduleState(
     __inout PDEVICE_DATA DeviceData,
     __in ASYNC_SCHEDULE_STATE NextState
     )
/*

Routine Description:

    Sets the state of the async schedule state as specified by the caller.
    In addition, it may update the Async Schedule Enable count on a transition
    into the disabled state.

    Async Doorbell Spinlock is held by the caller

*/
{
    ASYNC_SCHEDULE_STATE CurrentState;

    CurrentState = DeviceData->AsyncScheduleState;

    LOGENTRY(DeviceData, G, 'ASTr', CurrentState, NextState, 0);

    // If the async schedule state has transitioned to Disabled,
    // increment the Async Schedule Enable Count which will later be
    // used to determine if a queue head has been added to the schedule
    // since the last time the schedule was enabled.
    //

    if (CurrentState != AsyncScheduleState_Disabled &&
        NextState == AsyncScheduleState_Disabled) {

        DeviceData->CurrentAsyncEnableCount++;

        if (DeviceData->CurrentAsyncEnableCount == 0) {

            //
            // EndpointData->EndpointInScheduleCount is initialized to ZERO.
            // DeviceData->CurrentAsyncEnableCount is initialized to ONE.
            //
            // In the event of a wrap around of DeviceData->CurrentAsyncEnableCount
            // it is ensured to here to be non-ZERO so that an unexpected match could
            // not happen with both DeviceData->CurrentAsyncEnableCount and
            // EndpointData->EndpointInScheduleCount being set to ZERO without the
            // endpoint/QH ever being added to the schedule.
            //
            // Note that in the worst case a false match will cause an
            // async cache flush request when the endpoint/QH is
            // removed from the schedule when the async cache flush
            // request is not necessary.  This should do no harm.
            //
            DeviceData->CurrentAsyncEnableCount = 1;
        }

    }

    //
    // If a previous enable/disable schedule operation was blocked due to a pending doorbell,
    // do not overwrite the state so that a retry will be made when doorbell arrives.
    //
    if ((DeviceData->AsyncScheduleState == AsyncScheduleState_EnablePendingDoorbell) &&
        (NextState == AsyncScheduleState_Disabled)) {
            return;
    }

    if ((DeviceData->AsyncScheduleState  == AsyncScheduleState_DisablePendingDoorbell) &&
        (NextState == AsyncScheduleState_Enabled)) {
            return;
    }

    DeviceData->AsyncScheduleState = NextState;
    return;
}


VOID
EHCI_UpdateEndpointAsFreedFromHwCache(
    __in PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA epData
    )
/*

Routine Description:

    Updates a given endpoint as freed from Hw cache

*/
{
    epData->HwSyncState = Qh_Not_Busy;
    epData->StateChangeTc.QuadPart = 0;
    epData->DoorbellCount = 0;

    if (DeviceData->OptimizeInactiveQhFlush != FALSE) {
        epData->EndpointInScheduleCount = 0;
    }
}


_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_ClearAsyncDoorbellList(
     __inout PDEVICE_DATA DeviceData
     )
/*++

Routine Description:

    This routine releases all endpoints which are currently waiting for
    an Interrupt on Async Advance interrupt notification.

    This set of endpoints includes the current EpAtTheDoor endpoint and
    all endpoints currently on the AcfListHead list.

    The HwSyncState of these endpoints is changed from Qh_Busy to
    Qh_Not_Busy to indicate that the endpoint QueueHeads should no
    longer be accessible by the host controller on the async schedule
    list and should now safe for updating or deletion by the host
    controller driver.

    This routine can be called by EHCI_iAsyncCacheFlush() and by
    EHCI_CheckHwSync_Async() when it is noted that both
    USBCMD.AsyncScheduleEnable and USBSTS.AsyncScheduleStatus are clear
    indicating that the host controller should not currently be
    processing the asynchronous schedule list and therefore no wait for
    an Interrupt on Async Advance interrupt notification is necessary,
    nor even possible in this state.

    USBCMD.AsyncScheduleEnable and USBSTS.AsyncScheduleStatus must both
    be zero when calling this routine.

    The AcfListSpin spinlock is assumed to be held when this routine is
    called and that is the responsibility of the caller.

--*/
{
    PENDPOINT_DATA              tmpEpData = NULL;
    PHC_OPERATIONAL_REGISTER    hcOp;
    HW_LINK_POINTER             persistentQh;

    LOGENTRY(DeviceData, G, '_cl0', 0, 0, 0);

    // Reset the async doorbell timeout counters to zero when the
    // Asynchronous Schedule is disabled.
    //
    EHCI_ClearCurrentAsyncStats(DeviceData);

    // Release the current EpAtTheDoor endpoint.
    //
    if (DeviceData->EpAtTheDoor) {
        tmpEpData = DeviceData->EpAtTheDoor;
        DeviceData->EpAtTheDoor = NULL;
        LOGENTRY(DeviceData, G, '_cl1', 0, tmpEpData, 0);
        EHCI_UpdateEndpointAsFreedFromHwCache(DeviceData, tmpEpData);
        tmpEpData->AcfLink.Flink = NULL;
        tmpEpData->AcfLink.Blink = NULL;
    }

    // Release all endpoints currently on the AcfListHead list.
    //
    while (!IsListEmpty(&DeviceData->AcfListHead)) {
        PLIST_ENTRY le;
        le = RemoveHeadList(&DeviceData->AcfListHead);
        LOGENTRY(DeviceData, G, '_cl2', le, 0, 0);
        tmpEpData = CONTAINING_RECORD(le,
                                      struct _ENDPOINT_DATA,
                                      AcfLink);

        EHCI_ASSERT(DeviceData, tmpEpData->Sig == SIG_EP_DATA);
        LOGENTRY(DeviceData, G, '_cl3', 0, tmpEpData, 0);
        EHCI_UpdateEndpointAsFreedFromHwCache(DeviceData, tmpEpData);
        tmpEpData->AcfLink.Flink = NULL;
        tmpEpData->AcfLink.Blink = NULL;
    }

    // Reset the AsyncListAddr register to point to the persistent QH to
    // ensure that the AsyncListAddr register does not point to an
    // unlinked QH.
    //
    hcOp = DeviceData->OperationalRegisters;
    persistentQh.HwAddress = DeviceData->AsyncQueueHead->PhysicalAddress;

    // EHCI Specification, Section 4.8 Asynchronous Schedule
    //
    // Software may only write this register with defined results when
    // the schedule is disabled i.e. Asynchronous Schedule Enable bit in
    // the USBCMD and the Asynchronous Schedule Status bit in the USBSTS
    // register are zero.
    //
    if (DeviceData->HcSuspendedFlag) {
        //
        // No need to update the AsyncListAddr, but we should update the cached
        // value so that it may be restored when the controller is resumed
        //
        DeviceData->AsyncListAddrSave = persistentQh.HwAddress;

    } else {

        WRITE_REGISTER_ULONG(&hcOp->AsyncListAddr, persistentQh.HwAddress);

    }

    EHCI_SetAsyncDoorbellState(DeviceData, AsyncDoorbellState_NotWaiting);
}


_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_RingAsyncDoorbell(
     __inout PDEVICE_DATA DeviceData
     )
/*++

Routine Description:

    This routine requests an Interrupt on Async Advance interrupt
    notification for the first endpoint on the head of the AcfListHead
    list.

    If there is a currently waiting EpAtTheDoor endpoint then no further
    processing can be done here now and there should already be an
    outstanding Interrupt on Async Advance interrupt notification
    request which should eventually be processed by
    EHCI_AnswerAsyncDoorbell() when the notification arrives.

    If there is no currently waiting EpAtTheDoor endpoint and the
    AcfListHead list is also empty then there is no work to be done
    here now, otherwise the first endpoint on the head of the
    AcfListHead list is removed for processing.

    If the host controller asynchronous schedule list processing is not
    currently enabled nor currently active then the endpoint being
    processed can be released immediately by setting its HwSyncState to
    Qh_Not_Busy.

    If the host controller asynchronous schedule list processing is
    currently enabled and currently active then the endpoint being
    processed becomes the EpAtTheDoor endpoint and an Interrupt on
    Async Advance interrupt notification is requested via setting the
    USBCMD.IntOnAsyncAdvanceDoorbell bit.  NOTE: This should be the
    only code path in the driver that ever sets the
    USBCMD.IntOnAsyncAdvanceDoorbell bit.

    If the host controller asynchronous schedule list processing is
    currently in a state of transition (USBCMD.AsyncScheduleEnable !=
    USBSTS.AsyncScheduleStatus) then the endpoint being processed
    becomes the EpAtTheDoor endpoint, however no Interrupt on Async
    Advance interrupt notification is requested here now and that is
    left to a subsequent EHCI_RingAsyncDoorbell() retry.

    The AcfListSpin spinlock is assumed to be held when this routine is
    called and that is the responsibility of the caller.  This routine
    is only called by EHCI_iAsyncCacheFlush() which also assumes that
    the AcfListSpin spinlock is held when it is called.

    This routine is always called with the USBPORT Fdo.CoreFunctionSpin
    spinlock held.

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    USBCMD cmd;
    USBSTS sts;
    PENDPOINT_DATA epData = NULL;

    hcOp = DeviceData->OperationalRegisters;

    LOGENTRY(DeviceData, G, '_rg0', hcOp, 0, DeviceData->AcfLockThread);
    EHCI_ASSERT(DeviceData, DeviceData->AcfLockThread != NULL);

    do {
        PLIST_ENTRY le;

        cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
        sts.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);
        LOGENTRY(DeviceData, G, '_rg1', 0, cmd.ul, sts.ul);

        EHCI_UpdateScheduleStates(DeviceData, cmd, sts);

        if (DeviceData->EpAtTheDoor && cmd.ul != 0xFFFFFFFF) {
           // an endpoint is currently waiting so we have to wait for it to finish
           LOGENTRY(DeviceData, G, '_rg2', DeviceData->EpAtTheDoor, cmd.ul, 0);
           EHCI_ASSERT(DeviceData, DeviceData->EpAtTheDoor->HwSyncState == Qh_Busy);
           break;
        }

        if (IsListEmpty(&DeviceData->AcfListHead)) {
            // nothing to sync
            LOGENTRY(DeviceData, G, '_rg3', hcOp, &DeviceData->AcfListHead, 0);
            break;
        }

        le = RemoveHeadList(&DeviceData->AcfListHead);
        epData =  CONTAINING_RECORD(le,
                                    struct _ENDPOINT_DATA,
                                    AcfLink);

        LOGENTRY(DeviceData, G, '_rg4', epData, 0, 0);
        EHCI_ASSERT(DeviceData, epData->Sig == SIG_EP_DATA);
        epData->AcfLink.Flink = NULL;
        epData->AcfLink.Blink = NULL;
        EHCI_ASSERT(DeviceData, epData->HwSyncState == Qh_Busy);

        if (cmd.ul == 0xFFFFFFFF) {

            // hardware is removed, mark this ep as done
            EHCI_UpdateEndpointAsFreedFromHwCache(DeviceData, epData);
            DeviceData->StatRingDoorbellHwRemovedCount++;

            break;
        }

        if (sts.AsyncScheduleStatus == 0 && cmd.AsyncScheduleEnable == 0) {

            // If the Asynchronous Schedule is currently disabled then
            // the endpoint QueueHead should be safe for immediate
            // removal.
            //
            LOGENTRY(DeviceData, G, '_rg5', epData, 0, 0);
            EHCI_UpdateEndpointAsFreedFromHwCache(DeviceData, epData);
            EHCI_ClearAsyncDoorbellList(DeviceData);

            DeviceData->StatRingDoorbellDisabledCount++;

            break;

        } else if (sts.AsyncScheduleStatus == 1 && cmd.AsyncScheduleEnable == 0) {

            // If the Asynchronous Schedule is currently enabled but
            // transitioning to disabled then take no action now and a
            // retry should occur later.
            //
            LOGENTRY(DeviceData, G, '_rg6', epData, 0, 0);
            DeviceData->EpAtTheDoor = epData;
            EHCI_ASSERT(DeviceData, epData->HwSyncState == Qh_Busy);
            DeviceData->StatRingDoorbellDisablingCount++;

            EHCI_SetAsyncDoorbellState(DeviceData, AsyncDoorbellState_PendingScheduleDisable);

            break;

        } else if (sts.AsyncScheduleStatus == 0 && cmd.AsyncScheduleEnable == 1) {

            // If the Asynchronous Schedule is currently disabled but
            // transitioning to enabled then take no action now and a
            // retry should occur later.
            //
            LOGENTRY(DeviceData, G, '_rg7', epData, 0, 0);
            DeviceData->EpAtTheDoor = epData;
            EHCI_ASSERT(DeviceData, epData->HwSyncState == Qh_Busy);

            DeviceData->StatRingDoorbellEnablingCount++;
            EHCI_SetAsyncDoorbellState(DeviceData, AsyncDoorbellState_PendingScheduleEnable);

            break;

        } else {

            // If the Asynchronous Schedule is currently enabled then
            // ring the Async Advance Doorbell.
            //
            // This is the only code path where the USBCMD register
            // IntOnAsyncAdvanceDoorbell bit should ever be written as
            // a 1.
            //

            DeviceData->EpAtTheDoor = epData;
            EHCI_ASSERT(DeviceData, epData->HwSyncState == Qh_Busy);

            // if we are ringing the doorbell the irq should be enabled
            EHCI_ASSERT(DeviceData, DeviceData->EnabledInterrupts.IntOnAsyncAdvance == 1);

            // Ring the doorbell only if this is a fresh request OR
            // if the doorbell retry mechanism is enabled
            //
            if (DeviceData->CurrentRingDoorbellEnabledTimerCount == 0 ||
                DeviceData->CurrentDoorbellRetryEnableValue != FALSE) {

                LOGENTRY(DeviceData, G, '_rg!', epData, epData->QueueHead, 0);
                // make the request
                cmd.IntOnAsyncAdvanceDoorbell = 1;
                WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

                if (DeviceData->CurrentRingDoorbellEnabledTimerCount == 0) {

                    LOGENTRY(DeviceData, G, 'rg!0', epData, epData->QueueHead, 0);
                    EHCI_SetAsyncDoorbellState(DeviceData, AsyncDoorbellState_Requested);
                    DeviceData->StatRingDoorbellEnabledCount++;

                } else {

                    LOGENTRY(DeviceData, G, 'rg!1', epData, epData->QueueHead, 0);
                    EHCI_SetAsyncDoorbellState(DeviceData, AsyncDoorbellState_RequestedWithRetry);
                    DeviceData->StatRingDoorbellEnabledWithRetryCount++;

                }

            } else {

                LOGENTRY(DeviceData, G, '_rgX',
                         epData,
                         epData->QueueHead,
                         DeviceData->CurrentRingDoorbellEnabledTimerCount
                         );

                DeviceData->StatRingDoorbellEnabledWithRetryIgnoredCount++;

            }

            break;

        }

    } WHILE (0);
}


_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_iAsyncCacheFlush(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __in BOOLEAN DoorbellRetry
     )
/*++

Routine Description:

    This routine is called by EHCI_RemoveQueueHeadFromAsyncList() from
    EHCI_SetEndpointState() when a Control or Bulk endpoint is set to
    ENDPOINT_PAUSE or ENDPOINT_REMOVE.

    If the host controller asynchronous schedule list processing is not
    currently enabled nor currently active then the endpoint can be
    released immediately by setting its HwSyncState to Qh_Not_Busy.
    Otherwise the endpoint HwSyncState is set to Qh_Busy, the endpoint
    is inserted on the tail of the AcfListHead list, and then
    EHCI_RingAsyncDoorbell() is called to process the AcfListHead list.

    This routine is also called by EHCI_CheckHwSync_Async() when
    USBPORT is waiting for a Control or Bulk endpoint HwSyncState to
    change from Qh_Busy to Qh_Not_Busy and the endpoint in question is
    the currently waiting EpAtTheDoor endpoint and sufficient time has
    elasped that EHCI_CheckHwSync_Async() determines that a retry
    should be attempted.  In this case the DoorbellRetry parameter will
    be set to TRUE.

    This routine is always called with the USBPORT Fdo.CoreFunctionSpin
    spinlock held.

    The AcfListSpin spinlock is assumed to be held when this routine is
    called and that is the responsibility of the caller.

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    USBCMD cmd;
    USBSTS sts;
    LARGE_INTEGER tickCount;
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    BOOLEAN registersUnavailable = FALSE;

    EHCI_ASSERT(DeviceData, EndpointData->Sig == SIG_EP_DATA);

    qh = EndpointData->QueueHead;
    LOGENTRY(DeviceData, G, '_af1', qh, EndpointData, DeviceData->AcfLockThread);
    EHCI_ASSERT(DeviceData, qh != NULL);
    EHCI_ASSERT(DeviceData, DeviceData->AcfLockThread != NULL);

    hcOp = DeviceData->OperationalRegisters;

    // if the async list is disabled then mark the operation complete for all queue heads on the list

    if (DeviceData->HcSuspendedFlag) {
        registersUnavailable = TRUE;
    } else {

        cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
        sts.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);

        EHCI_UpdateScheduleStates(DeviceData, cmd, sts);

        if ((sts.AsyncScheduleStatus == 0 && cmd.AsyncScheduleEnable == 0) ||
            // allow immediate sync if run bit is clear (ie controller stopped)
             cmd.HostControllerRun == 0 ||
             cmd.ul == 0xFFFFFFFF) {
            registersUnavailable = TRUE;
        }
    }

    if (registersUnavailable) {

        //
        // Avoid touching the registers since we may have powered
        // off the controller
        //
        DeviceData->StatAsyncCacheFlushIdle++;

        LOGENTRY(DeviceData, G, '_af2', qh, 0, 0);
        // mark this qh not busy
        EHCI_UpdateEndpointAsFreedFromHwCache(DeviceData, EndpointData);

        // Mark any others waiting as not_busy.  This will also reset
        // the AsyncListAddr register to point to the persistent QH to
        // ensure that the AsyncListAddr register does not point to an
        // unlinked QH.
        //

        EHCI_ClearAsyncDoorbellList(DeviceData);

        return;
    }

    if (DeviceData->OptimizeInactiveQhFlush != FALSE) {

        if (EndpointData->EndpointInScheduleCount != DeviceData->CurrentAsyncEnableCount) {

            // Hardware flush is no longer required on this queue head as this has
            // not been added to the schedule since the last disable of the async
            // schedule and hence should not be in the hardware cache
            //

            LOGENTRY(DeviceData, G, 'QhPr', EndpointData, qh, 0);
            EHCI_UpdateEndpointAsFreedFromHwCache(DeviceData, EndpointData);
            DeviceData->StatAsyncCacheFlushPrevented++;
            return;
        }
    }

    // queue the new request and ring the doorbell

    if (DoorbellRetry) {

        DeviceData->StatAsyncCacheFlushRetry++;

        // The retry case should only occur when the endpoint is the
        // current EpAtTheDoor endpoint and its HwSyncState should
        // already be Qh_Busy.
        //
        EHCI_ASSERT(DeviceData, DeviceData->EpAtTheDoor == EndpointData);
        EHCI_ASSERT(DeviceData, EndpointData->HwSyncState == Qh_Busy);
        EHCI_ASSERT(DeviceData, EndpointData->AcfLink.Blink == NULL);
        EHCI_ASSERT(DeviceData, EndpointData->AcfLink.Flink == NULL);
        DeviceData->EpAtTheDoor = NULL;

        // Set the endpoint state change tick count to the current tick
        // count at the time when the doorbell will be requested.  The
        // doorbell might not actually be rung at this time if there are
        // other endpoints waiting already waiting for a doorbell
        // response.
        //
        KeQueryTickCount(&tickCount);
        EndpointData->StateChangeTc.QuadPart = tickCount.QuadPart;

        // Move the endpoint from the EpAtTheDoor back to the head of
        // the waiting list.  Then EHCI_RingAsyncDoorbell() will move
        // it back to the EpAtTheDoor once again.
        //
        InsertHeadList(&DeviceData->AcfListHead, &EndpointData->AcfLink);
        LOGENTRY(DeviceData, G, '_af6', EndpointData, 0, 0);
        EHCI_RingAsyncDoorbell(DeviceData);

    } else {

        DeviceData->StatAsyncCacheFlushNormal++;

        // This is the normal case when a Control or Bulk endpoint is
        // first set to ENDPOINT_PAUSE or ENDPOINT_REMOVE.  The endpoint
        // HwSyncState should not yet be set to Qh_Busy and it is set to
        // Qh_Busy here.
        //
        EHCI_ASSERT(DeviceData, DeviceData->EpAtTheDoor != EndpointData);
        EHCI_ASSERT(DeviceData, EndpointData->HwSyncState == Qh_Not_Busy);
        EHCI_ASSERT(DeviceData, EndpointData->AcfLink.Blink == NULL);
        EHCI_ASSERT(DeviceData, EndpointData->AcfLink.Flink == NULL);

        // Set the endpoint state change tick count to the current tick
        // count at the time when the doorbell will be requested.  The
        // doorbell might not actually be rung at this time if there are
        // other endpoints waiting already waiting for a doorbell
        // response.
        //
        KeQueryTickCount(&tickCount);
        EndpointData->StateChangeTc.QuadPart = tickCount.QuadPart;

        // mark queue head as busy
        EndpointData->HwSyncState = Qh_Busy;

        // Each queue head removal may require more than one doorbell response
        // to ensure that the HC has cleared its cache completely. Its default
        // value will now be 2 and can be reverted back to the original value of
        // one doorbell per queue head removal using the reg key "USBEHCI_InternalFlags".
        //

        if (DeviceData->DisableTwoDoorbellWorkaround == FALSE) {
            EndpointData->DoorbellCount = 2;
        } else {
            EndpointData->DoorbellCount = 1;
        }

        // Insert the endpoint at the tail of the waiting list.
        //
        InsertTailList(&DeviceData->AcfListHead, &EndpointData->AcfLink);
        LOGENTRY(DeviceData, G, '_af5', EndpointData, 0, 0);
        EHCI_RingAsyncDoorbell(DeviceData);
    }
}


VOID
EHCI_PollActiveControlEndpoint(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
     )
/*++

Routine Description:

    The queue head is in the running state we will just process
    the TDs that are completed up to 'current'  if dummy goes
    current then all TDs will be complete

Arguments:

Return Value:

--*/

{
    PHCD_TRANSFER_DESCRIPTOR td, currentTd, statusTd, setupTd;
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    HC_QTD_TOKEN token;
    HW_32BIT_PHYSICAL_ADDRESS curTdPhys;
    PTRANSFER_CONTEXT transfer;
    ULONG cf = 0;

    EHCI_ASSERT(DeviceData, EndpointData->sMode == 0);

#if DBG
    cf = EHCI_Get32BitFrameNumber(DeviceData);
#endif

    qh = EndpointData->QueueHead;
    curTdPhys =  qh->HwQH.CurrentTD.HwAddress & ~HW_LINK_FLAGS_MASK;

    LOGENTRY(DeviceData, G, '_pol', qh, cf, curTdPhys);

    EHCI_ASSERT(DeviceData, curTdPhys != 0);
    currentTd = (PHCD_TRANSFER_DESCRIPTOR)
                    USBPORT_PHYSICAL_TO_VIRTUAL(curTdPhys,
                                                DeviceData,
                                                EndpointData);

    statusTd = EndpointData->StatusTD;
    setupTd = EndpointData->SetupTD;
    EHCI_ASSERT(DeviceData, statusTd);
    EHCI_ASSERT(DeviceData, setupTd);
    LOGENTRY(DeviceData, G, '_cp1', statusTd, setupTd,  currentTd);

    token = statusTd->HwTD.Token;
    if (token.Active) {
        // wait for status TD to complete
        LOGENTRY(DeviceData, G, '_cp2', statusTd, setupTd, currentTd);
        return;
    }

    InsertTailList(&EndpointData->DoneTdList, &setupTd->DoneLink);
    SET_FLAG(setupTd->Flags, TD_FLAG_CONTROL_SKIP);

    InsertTailList(&EndpointData->DoneTdList, &statusTd->DoneLink);
    SET_FLAG(statusTd->Flags, TD_FLAG_CONTROL_SKIP);

    // process the data phase here
    td = TRANSFER_DESCRIPTOR_PTR(setupTd->NextHcdTD);
    LOGENTRY(DeviceData, G, '_cp3', td, 0, currentTd);
    while (td != statusTd) {

        EHCI_ASSERT(DeviceData, !TEST_FLAG(td->Flags, TD_FLAG_DUMMY));
        EHCI_ASSERT(DeviceData, !TEST_FLAG(td->Flags, TD_FLAG_CONTROL_SKIP));
        // TDs between head and current should not be active

        transfer = TRANSFER_CONTEXT_PTR(td->TransferContext);
        LOGENTRY(DeviceData, G, '_dt1', td, 0, transfer);

        token = td->HwTD.Token;
        if (token.Active == 1) {
            // if the TD is active then it must have been
            // skipped due to a short xfer condition
            LOGENTRY(DeviceData, G, '_dtS', td, 0, 0);
            SET_FLAG(td->Flags, TD_FLAG_SKIP);
        }

        SET_FLAG(td->Flags, TD_FLAG_DONE);

        InsertTailList(&EndpointData->DoneTdList, &td->DoneLink);

        td = TRANSFER_DESCRIPTOR_PTR(td->NextHcdTD);
    }
}


VOID
EHCI_PollHaltedControlEndpoint(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
     )
/*++

Routine Description:

Arguments:

Return Value:

--*/

{
    PHCD_TRANSFER_DESCRIPTOR td, currentTd, statusTd, setupTd;
    HC_QTD_TOKEN token;
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    HW_32BIT_PHYSICAL_ADDRESS curTdPhys;
    PTRANSFER_CONTEXT transfer;

    EHCI_ASSERT(DeviceData, EndpointData->sMode == 0);

    // we are halted probably due to an error and
    // currentTd should be the offending TD
    qh = EndpointData->QueueHead;
    curTdPhys =  qh->HwQH.CurrentTD.HwAddress & ~HW_LINK_FLAGS_MASK;

    EHCI_ASSERT(DeviceData, curTdPhys != 0);
    currentTd = (PHCD_TRANSFER_DESCRIPTOR)
                    USBPORT_PHYSICAL_TO_VIRTUAL(curTdPhys,
                                                DeviceData,
                                                EndpointData);

    statusTd = EndpointData->StatusTD;
    setupTd = EndpointData->SetupTD;
    EHCI_ASSERT(DeviceData, statusTd);
    EHCI_ASSERT(DeviceData, setupTd);
    LOGENTRY(DeviceData, G, '_hp1', statusTd, setupTd,  currentTd);

    // clear the active bits on setup and status TDs in case we need to re-start
    InsertTailList(&EndpointData->DoneTdList, &setupTd->DoneLink);
    SET_FLAG(setupTd->Flags, TD_FLAG_CONTROL_SKIP);

    // De-Activate the Setup Phase HW_QUEUE_ELEMENT_TD
    //
    token = setupTd->HwTD.Token;
    token.Active = 0;
    setupTd->HwTD.Token = token;

    InsertTailList(&EndpointData->DoneTdList, &statusTd->DoneLink);
    SET_FLAG(statusTd->Flags, TD_FLAG_CONTROL_SKIP);

    // De-Activate the Status Phase HW_QUEUE_ELEMENT_TD
    //
    token = statusTd->HwTD.Token;
    token.Active = 0;
    statusTd->HwTD.Token = token;

    // process the data phase here
    td = TRANSFER_DESCRIPTOR_PTR(setupTd->NextHcdTD);
    LOGENTRY(DeviceData, G, '_hp3', td, 0, currentTd);
    while (td != statusTd) {

        EHCI_ASSERT(DeviceData, !TEST_FLAG(td->Flags, TD_FLAG_DUMMY));
        EHCI_ASSERT(DeviceData, !TEST_FLAG(td->Flags, TD_FLAG_CONTROL_SKIP));
        // TDs between head and current should not be active

        transfer = TRANSFER_CONTEXT_PTR(td->TransferContext);
        LOGENTRY(DeviceData, G, '_ht1', td, 0, transfer);
        token = td->HwTD.Token;
        if (token.Active == 1) {
            // if the TD is active then it must have been
            // skipped due to a short xfer condition
            LOGENTRY(DeviceData, G, '_htS', td, 0, 0);
            SET_FLAG(td->Flags, TD_FLAG_SKIP);
        }

        SET_FLAG(td->Flags, TD_FLAG_DONE);

        InsertTailList(&EndpointData->DoneTdList, &td->DoneLink);

        td = TRANSFER_DESCRIPTOR_PTR(td->NextHcdTD);
    }

    // reset qh overlay to point at setup/status TD
    qh->HwQH.CurrentTD.HwAddress = statusTd->PhysicalAddress;
    qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = setupTd->PhysicalAddress;
    qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = setupTd->PhysicalAddress;

    // Re-Initialize the Token in the QH Overlay
    //
    token = qh->HwQH.Overlay.qTD.Token;
    token.BytesToTransfer = 0;

    // if this is a control endpoint the we need to clear the
    // halt condition
    if (TEST_FLAG(EndpointData->Flags, EHCI_EDFLAG_NOHALT)) {
        LOGENTRY(DeviceData, G, '_hp4', qh, 0, 0);

        EndpointData->EpStatus = ENDPOINT_STATUS_RUN;

        token.Active = 0;
        token.Halted = 0;
        token.ErrorCounter = 3;
    }

    qh->HwQH.Overlay.qTD.Token = token;
}


VOID
EHCI_PollPerAsyEndpoint(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __in BOOLEAN AsyncEndpoint
    )
/*++

Routine Description:

    Called when the endpoint 'needs attention'

    This is where we poll bulk and interrupt endpoins.  BI endpoints
    use a 'dummy' TD to denote the end of the current transfer


Arguments:

Return Value:

--*/
{
    if (EndpointData == EHCI_EpTrap) {
        TEST_TRAP(DeviceData);
    }

    EHCI_ASSERT(DeviceData, EndpointData->sMode);

    EHCI_sMode_PollEndpoint(DeviceData, EndpointData, AsyncEndpoint);
}


VOID
EHCI_PollControlEndpoint(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    Called when the endpoint 'needs attention'

    This is where we poll bulk and interrupt endpoins.  BI endpoints
    use a 'dummy' TD to denote the end of the current transfer


Arguments:

Return Value:

--*/

{
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    HC_QTD_TOKEN token;
    BOOLEAN active, halted;
    PHCD_TRANSFER_DESCRIPTOR td;
    PLIST_ENTRY listEntry;
    ULONG cf = 0;

    if (EndpointData == EHCI_EpTrap) {
        TEST_TRAP(DeviceData);
    }

    EHCI_ASSERT(DeviceData, EndpointData->sMode == 0);

    // this codepath is used only for control endpoints
    EHCI_QHCHK(DeviceData, EndpointData);

#if DBG
    cf = EHCI_Get32BitFrameNumber(DeviceData);
#endif

    if (EndpointData->SmodeTransfers == 0) {
        EHCI_ASSERT(DeviceData, IsListEmpty(&EndpointData->SmodeTransferListHead));
        // if we have no transfers queued then there is nothing to do
        LOGENTRY(DeviceData, G, '_poN', EndpointData, 0, cf);

        return;
    }

    // Get the QH and the current value of the QH Token from the Overlay
    //
    qh = EndpointData->QueueHead;
    token = qh->HwQH.Overlay.qTD.Token;

    if (TEST_FLAG(qh->QhFlags, EHCI_QH_FLAG_QH_REMOVED)) {
        // don't check endpoint if qh has been removed
        LOGENTRY(DeviceData, G, '_qRM', EndpointData, 0, cf);
        return;
    }

    LOGENTRY(DeviceData, G, '_poo', EndpointData, 0, cf);

    //
    // Active AND Halted    -- should never happen
    // !Active AND !Halted  -- advance queue head
    // Active AND !Halted   -- executing transaction in overlay
    // !Active AND Halted   -- queue had is stopped due to an error

    halted = (BOOLEAN) token.Halted;
    active = (BOOLEAN) token.Active;

    if (!active && halted) {
        // queue is halted
        EndpointData->EpStatus = ENDPOINT_STATUS_HALT;

        EHCI_PollHaltedControlEndpoint(DeviceData, EndpointData);
    } else {
        // queue is active
        EHCI_PollActiveControlEndpoint(DeviceData, EndpointData);
    }

    // now flush all completed TDs in order of completion from
    // our 'done' List

    while (!IsListEmpty(&EndpointData->DoneTdList)) {

        listEntry = RemoveHeadList(&EndpointData->DoneTdList);

        td = (PHCD_TRANSFER_DESCRIPTOR) CONTAINING_RECORD(
                     listEntry,
                     struct _HCD_TRANSFER_DESCRIPTOR,
                     DoneLink);

        EHCI_ASSERT(DeviceData, (td->Flags & (TD_FLAG_XFER | TD_FLAG_DONE)));
        EHCI_ProcessDoneAsyncTd(DeviceData, td, NULL);
    }
}


VOID
EHCI_AssertQhChk(
     __in PDEVICE_DATA DeviceData,
     __in PENDPOINT_DATA EndpointData
     )
{
    PULONG  p;
    ULONG   i;

    p = (PULONG)EndpointData->QhChk;

    for (i = 0; i < (HCD_QUEUEHEAD_DESCRIPTOR_PAD / sizeof(*p)); i++) {
        EHCI_ASSERT(DeviceData, *p == 0);
        p++;
    }
}


VOID
EHCI_SetNextTd(
    __in PDEVICE_DATA DeviceData,
    __inout PHCD_TRANSFER_DESCRIPTOR  LinkTd,
    __in PHCD_TRANSFER_DESCRIPTOR  NextTd,
    BOOLEAN SetAltNext
    )
{

    EHCI_ASSERT(DeviceData, LinkTd != NextTd);\

    if (SetAltNext) {

        do {
            LinkTd->HwTD.Next_qTD.HwAddress = NextTd->PhysicalAddress;
            LinkTd->HwTD.AltNext_qTD.HwAddress = NextTd->PhysicalAddress;
        } while (LinkTd->HwTD.Next_qTD.HwAddress  !=
                 LinkTd->HwTD.AltNext_qTD.HwAddress);
        TRANSFER_DESCRIPTOR_PTR(LinkTd->NextHcdTD) = NextTd;
        TRANSFER_DESCRIPTOR_PTR(LinkTd->AltNextHcdTD) = NextTd;

    } else {
        LinkTd->HwTD.Next_qTD.HwAddress = NextTd->PhysicalAddress;
        TRANSFER_DESCRIPTOR_PTR(LinkTd->NextHcdTD) = NextTd;
    }
}


VOID
EHCI_SetNextSlotTd(
    __in PDEVICE_DATA DeviceData,
    __inout PHCD_TRANSFER_DESCRIPTOR  LinkTd,
    __in PHCD_TRANSFER_DESCRIPTOR  NextTd,
    __in PHCD_TRANSFER_DESCRIPTOR  AltNextTd
    )
{
    EHCI_ASSERT(DeviceData, LinkTd != NextTd);\

    LinkTd->HwTD.Next_qTD.HwAddress = NextTd->PhysicalAddress;
    LinkTd->HwTD.AltNext_qTD.HwAddress = AltNextTd->PhysicalAddress;

    TRANSFER_DESCRIPTOR_PTR(LinkTd->NextHcdTD) = NextTd;
    TRANSFER_DESCRIPTOR_PTR(LinkTd->AltNextHcdTD) = AltNextTd;
    TRANSFER_DESCRIPTOR_PTR(LinkTd->SlotNextHcdTD) = NextTd;
}


VOID
EHCI_SetAltNextTd(
    __in PDEVICE_DATA DeviceData,
    __inout PHCD_TRANSFER_DESCRIPTOR  LinkTd,
    __in PHCD_TRANSFER_DESCRIPTOR  NextTd
    )
{
    EHCI_ASSERT(DeviceData, LinkTd != NextTd);

    LinkTd->HwTD.AltNext_qTD.HwAddress = NextTd->PhysicalAddress;

    TRANSFER_DESCRIPTOR_PTR(LinkTd->AltNextHcdTD) = NextTd;
}

/***********************************************************************************/

/*
Slot mode transfer support.  'Slot' mode uses a ring buffer to manage the TDs instead of linking the transfers
dynamically.  This method provides more stable hardware synchronization but may not perform as fast in all cases.
Slot mode is used for both async (bulk) and periodic (interrupt) transfers.

We support two slot modes:

1 - Large mode 2 transfers(slots) 256 Tds/slot  5MB/transfer

2 - Small mode 16 tranfsers(slots), 1 Td per slot. 20k/transfer

*/

#define INITIALIZE_TD_FOR_SLOT_TRANSFER(td, tc) \
        { ULONG i;\
        HC_QTD_TOKEN token;\
        TRANSFER_CONTEXT_PTR((td)->TransferContext) = (tc);\
        (td)->Flags |= TD_FLAG_XFER; \
        for (i=0; i<5; i++) {\
            (td)->HwTD.BufferPage[i].ul = 0x0bad0000;\
            (td)->HwTD.BufferPage64[i]  = 0x00000000;\
        }\
        token.ul = 0;\
        token.ErrorCounter = 3;\
        (td)->HwTD.Token = token;\
        }


VOID
EHCI_sMode_ResetSlot(
     __in PDEVICE_DATA DeviceData,
     __in PENDPOINT_DATA EndpointData,
     __inout PENDPOINT_SLOT Slot
     )
/*++

Routine Description:

    Frees a slot for use on another transfer

Arguments:

Return Value:

--*/

{
    PHCD_TRANSFER_DESCRIPTOR td;
    HC_QTD_TOKEN token;
    ULONG t, i;

    Slot->State = slot_NotBusy;
    // reset all TDs in the this slot
    for (t=0; t<EndpointData->TdsPerSlot; t++) {
        td = Slot->Tds[t];

        if (TEST_FLAG(td->Flags, TD_FLAG_SLOT_RESET)) {
            // Don't waste time re-initializing TDs that are still in
            // the initialized state.
            //
            break;
        }

        SET_FLAG(td->Flags, TD_FLAG_SLOT_RESET);
        CLEAR_FLAG(td->Flags, TD_FLAG_DONE);
        CLEAR_FLAG(td->Flags, TD_FLAG_SKIP);
        CLEAR_FLAG(td->Flags, TD_FLAG_XFER);
        CLEAR_FLAG(td->Flags, TD_FLAG_ERROR_SKIP);
        TRANSFER_CONTEXT_PTR(td->TransferContext) = NULL;

        for (i=0; i<5; i++) {
            td->HwTD.BufferPage[i].ul = 0x0bad0000;
            td->HwTD.BufferPage64[i]  = 0x00000000;
        }

        // Reinitialize the Active bit (clear it) and the ErrorCounter
        // fields of the HC_QTD_TOKEN.
        //
        token.ul = 0;
        token.ErrorCounter = 3;
        td->HwTD.Token = token;

        SET_NEXT_TD(DeviceData, td, TRANSFER_DESCRIPTOR_PTR(td->SlotNextHcdTD));
    }
}


VOID
EHCI_sMode_InitSlot(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     ULONG Idx,
     __inout PENDPOINT_SLOT Slot,
     __inout PHCD_TRANSFER_DESCRIPTOR FirstTd,
     __inout PHCD_TRANSFER_DESCRIPTOR NextSlotFirstTd
     )
/*++

Routine Description:

    Initialize Endpoint for 'large' transfer mode.

    This routine sets up the ring of TDs.

Arguments:

Return Value:

--*/

{
    PHCD_TRANSFER_DESCRIPTOR td, prevTd;
    ULONG t;

    LOGENTRY(DeviceData, G, '_lgS', Slot, FirstTd, NextSlotFirstTd);

    // allocate TDs for each slot
    EHCI_ASSERT(DeviceData, FirstTd);
    EHCI_ASSERT(DeviceData, NextSlotFirstTd);

    Slot->Tds[0] = Slot->FirstTd = FirstTd;
    Slot->State = slot_NotBusy;
    Slot->Idx = Idx;
    // init to 1
    Slot->Sequence = ++EndpointData->SlotSequence;

    // init slot Tds
    prevTd = Slot->FirstTd;

    // deduct one for the head TD
    EHCI_ASSERT(DeviceData, EndpointData->TdsPerSlot <= MAX_SLOT_TDS);
    for (t=1; t<EndpointData->TdsPerSlot; t++) {

        td = EHCI_ALLOC_TD(DeviceData, EndpointData);
        EHCI_ASSERT(DeviceData, td);
        Slot->Tds[t] = td;

        EHCI_SetNextSlotTd(DeviceData, prevTd, td, NextSlotFirstTd);

        prevTd = td;
    }
    // this is the last TD
    EHCI_SetNextSlotTd(DeviceData, prevTd, NextSlotFirstTd, NextSlotFirstTd);

    EHCI_sMode_ResetSlot(DeviceData, EndpointData, Slot);
}


VOID
EHCI_sMode_InitEndpoint(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
     )
/*++

Routine Description:

    Initialize Endpoint for 'large' slot transfer mode.

    This routine sets up the ring of TDs.

Arguments:

Return Value:

--*/

{
    PENDPOINT_SLOT slot, slot0;
    PHCD_TRANSFER_DESCRIPTOR firstTd0, firstTd1, headTd;
    HC_QTD_TOKEN token;
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    ULONG sIdx;

    EndpointData->TdsPerSlot = MAX_SLOT_TDS;
    EHCI_ASSERT(DeviceData, EndpointData->TdsPerSlot*2 <= EndpointData->TdCount);

    EndpointData->SlotSequence = 0;
    EndpointData->sMode = 1;
    EndpointData->SlotCount = 2;

    qh = EndpointData->QueueHead;

    LOGENTRY(DeviceData, G, '_lgI', qh, EndpointData->TdsPerSlot, 0);

    sIdx = 0;

     // allocate TDs for each slot
    headTd = firstTd0 = EHCI_ALLOC_TD(DeviceData, EndpointData);
    EHCI_ASSERT(DeviceData, firstTd0);

    while (sIdx < EndpointData->SlotCount) {

        slot = &EndpointData->Slots[sIdx];

        if (sIdx == EndpointData->SlotCount-1) {
            firstTd1 = headTd;
        } else {
            firstTd1 = EHCI_ALLOC_TD(DeviceData, EndpointData);
        }

        EHCI_ASSERT(DeviceData, firstTd1);

        EHCI_sMode_InitSlot(DeviceData, EndpointData, sIdx, slot, firstTd0, firstTd1);
        //EndpointData->Slot0 = slot0;

        firstTd0 = firstTd1;
        EndpointData->SlotPtr[sIdx] = slot;

        sIdx++;
    }

    slot0 = &EndpointData->Slots[0];

    if (!DeviceData->TerminateQH) {
        qh->HwQH.CurrentTD.HwAddress = slot0->FirstTd->PhysicalAddress;
        qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = slot0->FirstTd->PhysicalAddress;
        qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = slot0->FirstTd->PhysicalAddress;
    }

    token = qh->HwQH.Overlay.qTD.Token;
    token.BytesToTransfer = 0;
    token.Active = 0;
    qh->HwQH.Overlay.qTD.Token = token;
}


VOID
EHCI_sMode_PollEndpoint(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __in BOOLEAN AsyncEndpoint
     )
/*++

Routine Description:

    Process the TDs for active slots if all TDs are completed in the slot then complete
    the transfer.

Arguments:

    AsyncEndpoint - true for bulk/control false for interrupt

Return Value:

--*/

{
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    HC_QTD_TOKEN token;
    PENDPOINT_SLOT slot;

    qh = EndpointData->QueueHead;

    if (TEST_FLAG(qh->QhFlags, EHCI_QH_FLAG_QH_REMOVED)) {
        // don't check endpoint if qh has been removed
        LOGENTRY(DeviceData, G, '_qRM', EndpointData, 0, 0);

        return;
    }

    // Process the slots in order to avoid completing transfers out of order.
    slot = EndpointData->CurrentSlot;

    while (slot) {
        BOOLEAN complete;

        complete = EHCI_sMode_PollEndpointSlot(DeviceData, EndpointData, slot);

        if (!complete) {
            // current xfer did not complete yet
            break;
        }

        EHCI_ASSERT(DeviceData, slot != EndpointData->CurrentSlot);
        // check the next slot
        slot = EndpointData->CurrentSlot;
    }

    if (EndpointData->EpStatus == ENDPOINT_STATUS_HALT) {
        // if the endpoint is halted the current TD will point to the error
        // we must reset the overlay so we are ready for recovery (ie clear_halt)
        // this is not a problem if no transfers are active (qh will reset when the next transfer
        // is queued)
        //
        token = qh->HwQH.Overlay.Ov.Token;
        EHCI_ASSERT(DeviceData, token.Halted == 1);
        EHCI_ASSERT(DeviceData, token.Active == 0);
        if (EndpointData->CurrentSlot != NULL) {
            TEST_TRAP(DeviceData);
        }
    }

    return;
}


BOOLEAN
EHCI_sMode_PollEndpointSlot(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PENDPOINT_SLOT Slot
     )
/*++

Routine Description:

    Process TDs for a given slot.  If all TDs are complete conplete the transfer and release the slot.

Arguments:

Return Value:

--*/

{
    PHCD_TRANSFER_DESCRIPTOR td, currentTd;
    HC_QTD_TOKEN token;
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    HW_32BIT_PHYSICAL_ADDRESS curTdPhys;
    PTRANSFER_CONTEXT transfer = NULL;
    BOOLEAN tdError;
    ULONG tdIdx, byteCount;
    BOOLEAN shortPacket;
    USBD_STATUS usbdStatus;
    PTRANSFER_PARAMETERS tp = NULL;
    PENDPOINT_SLOT nextSlot;
    BOOLEAN slotComplete = FALSE;
    LARGE_INTEGER time;
    ULONG timeInMs;
    ULONG timeDiff;

transfer_retry:

    qh = EndpointData->QueueHead;
    curTdPhys =  qh->HwQH.CurrentTD.HwAddress & ~HW_LINK_FLAGS_MASK;

    LOGENTRY(DeviceData, G, '_Spl', qh, Slot, curTdPhys);

    EHCI_ASSERT(DeviceData, curTdPhys != 0);
    currentTd = (PHCD_TRANSFER_DESCRIPTOR)
                    USBPORT_PHYSICAL_TO_VIRTUAL(curTdPhys,
                                                DeviceData,
                                                EndpointData);

    // walk the soft list of TDs and complete all TDs that have the active bit clear
    // up to the currentTD

    // get the last known head
    LOGENTRY(DeviceData, G, '_hd1',
             0,
             0,
             currentTd);

    EHCI_ASSERT(DeviceData, Slot->State == slot_Busy);
    shortPacket = FALSE;
    tdError = FALSE;

    for (tdIdx = 0; tdIdx < EndpointData->TdsPerSlot; tdIdx++) {

        td = Slot->Tds[tdIdx];

        // Locally cache the current value of the HC_QTD_TOKEN from the
        // current HW_QUEUE_ELEMENT_TD.
        //
        token = td->HwTD.Token;

        LOGENTRY(DeviceData, G, '_SL1', td, token.ul, tdIdx);

        if (TEST_FLAG(td->Flags, TD_FLAG_DONE)) {
            // td already processed -- skip it
            LOGENTRY(DeviceData, G, '_SL2', td, 0, tdIdx);
            continue;
        }

        if (TEST_FLAG(td->Flags, TD_FLAG_SKIP)) {
            // skip this one as well, these are between the short and last td
            LOGENTRY(DeviceData, G, '_SL3', td, 0, tdIdx);
            break;
        }

        if (TEST_FLAG(td->Flags, TD_FLAG_SLOT_RESET)) {
            // last td
            LOGENTRY(DeviceData, G, '_SLx', td, 0, tdIdx);
            break;
        }

        transfer = TRANSFER_CONTEXT_PTR(td->TransferContext);
        tp = transfer->TransferParameters;

        LOGENTRY(DeviceData, G, '_SLt', td, tdError, shortPacket);

        // error OR short_packet flushes remaining TDs
        if (shortPacket || tdError) {
            LOGENTRY(DeviceData, G, '_SL4', td, 0, tdIdx);
            SET_FLAG(td->Flags, TD_FLAG_SKIP);
            if (tdError) {
                // error case should halt the endpoint
                SET_FLAG(td->Flags, TD_FLAG_ERROR_SKIP);
                EHCI_ASSERT(DeviceData, EndpointData->EpStatus == ENDPOINT_STATUS_HALT);
            }

            // De-Activate the current HW_QUEUE_ELEMENT_TD.
            //
            token.Active = 0;
            td->HwTD.Token = token;

            transfer->PendingTds--;
            continue;
        }

        // If the Active bit is set in the locally cached valued of the
        // HC_QTD_TOKEN from the current HW_QUEUE_ELEMENT_TD then break
        // out of the HW_QUEUE_ELEMENT_TD loop now since the host
        // controller still owns the current and remaining
        // HW_QUEUE_ELEMENT_TDs.
        //
        if (token.Active == 1) {
            // TD is still active.
            LOGENTRY(DeviceData, G, '_SLa', td, token.ul, tdIdx);
            break;
        }

        // If the Active bit is clear in the locally cached value of
        // the HC_QTD_TOKEN from the current HW_QUEUE_ELEMENT_TD then
        // the host controller no longer owns the current
        // HW_QUEUE_ELEMENT_TD and the cached value of the HC_QTD_TOKEN
        // from the current HW_QUEUE_ELEMENT_TD should remain consistent
        // with the in memory version of the HW_QUEUE_ELEMENT_TD.

        // process this completed TD
        SET_FLAG(td->Flags, TD_FLAG_DONE);
        transfer->PendingTds--;

        // completion status for this TD?
        // since the endpoint halts on error the  error
        // bits should have been written back to the TD
        // we use these bits to dermine the error
        if (token.Halted == 1) {
            ULONG xb;

            xb = token.BytesToTransfer;
            LOGENTRY(DeviceData, G, '_SLx', td, token.ul, xb);

            if (((token.MissedMicroFrame && DeviceData->MissedMicroframePerSecRetryCount > 0) ||
                (token.XactErr && DeviceData->SoftPerSecRetryCount > 0)) &&
                xb != 0) {

                // Get current Time in mili-seconds
                KeQuerySystemTime(&time);
                timeInMs = (ULONG)(time.QuadPart / 10000);

                if ( token.MissedMicroFrame ) {
                    timeDiff = timeInMs - EndpointData->PendingMissedMicroframeRetryCountInitTimeMs;
                    if (timeDiff > RETRY_COUNT_RESET_TIME_MS) {
                        // It has been too long since we reset the
                        // PendingMissedMicroframeRetryCount value. Reset that value
                        EndpointData->PendingMissedMicroframeRetryCountInitTimeMs = timeInMs;
                        EndpointData->PendingMissedMicroframeRetryCount
                            = (USHORT) DeviceData->MissedMicroframePerSecRetryCount;
                        LOGENTRY(DeviceData, G, '_SLb', td, token.ul, timeInMs);
                    }
                }

                if ( token.XactErr ) {
                    timeDiff = timeInMs - EndpointData->PendingSoftRetryCountInitTimeMs;
                    if (timeDiff > RETRY_COUNT_RESET_TIME_MS) {
                        // It has been too long since we reset the PendingSoftRetryCount value.
                        // reset that value
                        EndpointData->PendingSoftRetryCountInitTimeMs = timeInMs;
                        EndpointData->PendingSoftRetryCount
                             = (USHORT) DeviceData->SoftPerSecRetryCount;
                        LOGENTRY(DeviceData, G, '_SLc', td, token.ul, timeInMs);
                    }
                }

                if ( (token.MissedMicroFrame && EndpointData->PendingMissedMicroframeRetryCount > 0) ||
                     (token.XactErr && EndpointData->PendingSoftRetryCount > 0) ) {

                    // attempt on-the-fly reset of thisTD

                    // must be:
                    // 1 TD per transfer
                    // error is missed-microframe or XactErr
                    // BytesToTransfer != 0;

                    LOGENTRY(DeviceData, G, '_SLd', td, token.ul, qh->HwQH.Overlay.qTD.Token.ul);

                    if (token.MissedMicroFrame) {
                        EndpointData->PendingMissedMicroframeRetryCount--;
                    }

                    if (token.XactErr) {
                        EndpointData->PendingSoftRetryCount--;
                    }

                    token.SplitXstate = 0;
                    token.MissedMicroFrame = 0;
                    token.XactErr = 0;

                    token.ErrorCounter = 3; //DeviceData->MaxSlotErrorCount;

                    transfer->PendingTds++;
                    CLEAR_FLAG(td->Flags, TD_FLAG_DONE);

                    // Re-Activate the HC_QTD_TOKEN.
                    //
                    token.Halted = 0;
                    token.Active = 1;

                    // Re-Activate the current HW_QUEUE_ELEMENT_TD and
                    // the HW_QUEUEHEAD_DESCRIPTOR, then restart the
                    // HW_QUEUE_ELEMENT_TD processing loop.
                    //
                    td->HwTD.Token = token;
                    qh->HwQH.Overlay.qTD.Token = token;

                    goto transfer_retry;
                }
            }

            LOGENTRY(DeviceData, G, '_SL!', td, 0, 0);
            usbdStatus = EHCI_GetErrorFromTD(DeviceData,
                                             EndpointData,
                                             td);

            EndpointData->SlotTdErrorCount++;
            EndpointData->EpStatus = ENDPOINT_STATUS_HALT;
            tdError = TRUE;

        } else {
            usbdStatus = USBD_STATUS_SUCCESS;
        }

        LOGENTRY(DeviceData, G, '_SL5', transfer, usbdStatus,  td);

        byteCount = EHCI_TdBytesTransferred(td->TransferLength, token.BytesToTransfer);

        LOGENTRY(DeviceData, G, '_SL7', byteCount,
                 td->TransferLength, token.BytesToTransfer);

        transfer->BytesTransferred += byteCount;

        // note that we only set transfer->UsbdStatus
        // if we find a TD with an error this will cause us to
        // record the last TD with an error as the error for
        // the transfer.
        if (USBD_STATUS_SUCCESS != usbdStatus) {

            // map the error to code in USBDI.H
            transfer->UsbdStatus = usbdStatus;

            LOGENTRY(DeviceData, G, '_SL8', transfer->UsbdStatus, 0, 0);
        }

        if (token.Active == 0 &&
            token.BytesToTransfer != 0) {

            LOGENTRY(DeviceData, G, '_SL9', token.BytesToTransfer, 0, td);
            shortPacket = TRUE;
        }

        if (transfer->PendingTds == 0) {
            LOGENTRY(DeviceData, G, '_Sl1', 0, 0, 0);
            //last TD in this transfer
            break;
        }
    }

    // is transfer complete?
    if (transfer &&
        transfer->PendingTds == 0) {
        // all TDs for this transfer are done
        // clear the HAVE_TRANSFER flag to indicate
        // we can take another

        EHCI_DecPendingTransfer(DeviceData, EndpointData, transfer);

        if (shortPacket) {

            ULONG split_id;

            LOGENTRY(DeviceData, G, '_cps',
                transfer->UsbdStatus,
                transfer,
                transfer->BytesTransferred);

            split_id = tp->SequenceNumber;
            EHCI_sMode_ShortTransfer(DeviceData, EndpointData, transfer);

            EHCI_sMode_ResetSlot(DeviceData, EndpointData, Slot);

            nextSlot = EHCI_sMode_IncSlotIdx(DeviceData, EndpointData, Slot);

            slotComplete = TRUE;

            USBPORT_COMPLETE_TRANSFER(DeviceData,
                                      EndpointData,
                                      tp,
                                      transfer->UsbdStatus,
                                      transfer->BytesTransferred);

            // complete any split transfers here

            while (nextSlot->State == slot_Busy) {

                PHCD_TRANSFER_DESCRIPTOR split_td;
                PTRANSFER_CONTEXT split_transfer = NULL;
                PTRANSFER_PARAMETERS split_tp;

                split_td = nextSlot->Tds[0];
                LOGENTRY(DeviceData, G, '_sp1', split_td, split_id, nextSlot);

                split_transfer = TRANSFER_CONTEXT_PTR(split_td->TransferContext);
                split_tp = split_transfer->TransferParameters;

                if (split_tp->SequenceNumber != split_id) {
                    break;
                }

                EHCI_ASSERT(DeviceData, split_tp->MiniportFlags & MPTX_SPLIT_TRANSFER);
                TEST_TRAP(DeviceData);

                EHCI_DecPendingTransfer(DeviceData, EndpointData, split_transfer);

                EHCI_sMode_ResetSlot(DeviceData, EndpointData, nextSlot);

                nextSlot = EHCI_sMode_IncSlotIdx(DeviceData, EndpointData, nextSlot);

                USBPORT_COMPLETE_TRANSFER(DeviceData,
                                          EndpointData,
                                          split_tp,
                                          split_transfer->UsbdStatus,
                                          split_transfer->BytesTransferred);

            }

        } else {
            // not short

            LOGENTRY(DeviceData, G, '_cpt',
                transfer->UsbdStatus,
                transfer,
                transfer->BytesTransferred);


            EHCI_sMode_ResetSlot(DeviceData, EndpointData, Slot);

            EHCI_sMode_IncSlotIdx(DeviceData, EndpointData, Slot);

            // make the next slot current

            slotComplete = TRUE;

            USBPORT_COMPLETE_TRANSFER(DeviceData,
                                      EndpointData,
                                      tp,
                                      transfer->UsbdStatus,
                                      transfer->BytesTransferred);

        }
    }

    return slotComplete;
}


VOID
EHCI_sMode_ActivateQh(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PENDPOINT_SLOT Slot
     )
{
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    HC_QTD_TOKEN token;

    // Get the QH and the current value of the QH Token from the Overlay
    //
    qh = EndpointData->QueueHead;
    token = qh->HwQH.Overlay.qTD.Token;

    if (EndpointData->EpStatus == ENDPOINT_STATUS_HALT) {
        EHCI_ASSERT(DeviceData, token.Halted == 1);
    }

    qh->HwQH.CurrentTD.HwAddress = Slot->FirstTd->PhysicalAddress;

    qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = Slot->FirstTd->PhysicalAddress;
    qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = Slot->FirstTd->PhysicalAddress;
    // no transfers current, reinitialze the slot
    token.BytesToTransfer = 0;
    token.Active = 0;
    token.ErrorCounter = 3;
    token.SplitXstate = 0;
    token.C_Page = 0;
    qh->HwQH.Overlay.qTD.Token = token;
}


USB_MINIPORT_STATUS
EHCI_sMode_Transfer(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PTRANSFER_PARAMETERS TransferUrb,
     __inout PTRANSFER_CONTEXT TransferContext,
     __inout PTRANSFER_SG_LIST TransferSGList,
     __in BOOLEAN AsyncEndpoint
     )
{
    PHCD_TRANSFER_DESCRIPTOR td;
    PHCD_TRANSFER_DESCRIPTOR tailTd;
    HC_QTD_TOKEN token;
    ULONG lengthMapped;
    ULONG tdIdx = 0;
    PENDPOINT_SLOT slot;
    ULONG i;
    ULONG ioc = 0;

    EHCI_ASSERT(DeviceData, TransferUrb->TransferBufferLength <= MAX_BULK_TRANSFER_SIZE);
    LOGENTRY(DeviceData, G, '_SLT', EndpointData, TransferUrb, TransferUrb->TransferBufferLength);

    if ((EndpointData->SmodeTransfers) >= DeviceData->SlotsPerEndpoint) {
        return USBMP_STATUS_BUSY;
    }

    // When IoC is set for each TD in a transfer, it is possible that the ISR/DPC
    // sequence triggered by one of the penultimate TDs in the transfer might also
    // detect completion of the last TD and hence speed up the completion of the
    // transfer instead of waiting for the host controller to generate the
    // interrupt on the last TD. However, this gain (limited to around 6% in
    // experimental runs) is to be weighed against the cost of generating frequent
    // interrupts on multi TD transfer breaking the CPU idleness possibly
    // resulting in higher power utilization.
    //
    // Hence the approach is to configure this optimization via a new USB
    // power setting GUID_POWER_INTERRUPT_PER_TD_ENABLE, which, by default,
    // is enabled when the system is running on high performance power scheme
    // or balanced AC power scheme; and disabled otherwise.
    //
    // This is expected to help extend battery life hence benefit video playback
    // scenario on ARM/SoC platforms and boot on USB 2.0 with mobile platforms.
    //
    if (USBPORT_IS_INTERRUPT_PER_TD_ENABLED(DeviceData)) {
        ioc = 1;
    }

    // split transfer notes:
    // if this is a large split (IN)transfer we can only permit another transfer if is not part of the
    // split set.  This is necessary in order to handle the short packet condition that terminates
    // the logical transfer

    if (TEST_FLAG(TransferUrb->TransferFlags, USBD_TRANSFER_DIRECTION_IN)) {
        PTRANSFER_CONTEXT currentTx;
        PENDPOINT_SLOT currentSlot;

        currentSlot = EndpointData->CurrentSlot;
        if (currentSlot) {
            currentTx = currentSlot->TransferContext;

            // The previous logic below could lead to the split
            // transactions of one parent transaction being interleaved
            // with the split transactions of the next parent
            // transaction being scheduled.  This could lead to out of
            // order data being returned.
            //
            // This problem was observed using a Pinnacle System 700-USB
            // video and audio capture device.

            // Old logic:
            //
            // if (currentTx->TransferParameters->MiniportFlags != 0 &&
            //     TransferContext->TransferParameters->MiniportFlags != 0 &&
            //     (currentTx->TransferParameters->SequenceNumber ==
            //      TransferContext->TransferParameters->SequenceNumber)) {

            // New logic: Do not allow any additional transfers to be
            // queued if the current transfer is part of a split
            // transfer set and is not the final segment of the split
            // transfer set.
            //
            if ((currentTx->TransferParameters->MiniportFlags &
                (MPTX_SPLIT_TRANSFER | MPTX_SPLIT_TRANSFER_TAIL)) ==
                MPTX_SPLIT_TRANSFER) {

                //TEST_TRAP(DeviceData);
                LOGENTRY(DeviceData, G, '_BB0', currentTx, 0, 0);
                return USBMP_STATUS_BUSY;
            }
        }
    }

    // locate the free slot, take the free slot with the lowest seq number
    slot = NULL;
    for (i=0; i<EndpointData->SlotCount; i++) {

        if (EndpointData->Slots[i].State == slot_Busy) {
            continue;
        }

         // slot[i] is slot_NotBusy
        if (slot == NULL &&
            EndpointData->Slots[i].State == slot_NotBusy) {
            slot = &EndpointData->Slots[i];
            continue;
        }

        if (slot &&
            slot->Sequence > EndpointData->Slots[i].Sequence) {
            slot = &EndpointData->Slots[i];
        }

    }

    if (slot == NULL) {
        LOGENTRY(DeviceData, G, '_BBS', EndpointData, TransferUrb, 0);
        return USBMP_STATUS_BUSY;
    }

    // update list of pending transfers
    EHCI_IncPendingTransfer(DeviceData, EndpointData, TransferContext);

    EHCI_ASSERT(DeviceData, slot->State == slot_NotBusy);

    slot->State = slot_Busy;
    slot->TransferContext = TransferContext;
    slot->Sequence = ++EndpointData->SlotSequence;
    TransferContext->EpSlot = slot;

    // we have enough tds, program the transfer

    LOGENTRY(DeviceData, G, '_BIT', EndpointData, TransferUrb, slot);

    lengthMapped = 0;
    td = NULL;
    TransferContext->PendingTds = 0;

    while (lengthMapped < TransferUrb->TransferBufferLength) {

        LOGENTRY(DeviceData, G, '_dt1', tdIdx, TransferContext, lengthMapped);

        EHCI_ASSERT(DeviceData, tdIdx < MAX_SLOT_TDS);
        TransferContext->PendingTds++;
        td = slot->Tds[tdIdx];

        EHCI_ASSERT(DeviceData, td->Flags & TD_FLAG_SLOT_RESET);

        CLEAR_FLAG(td->Flags, TD_FLAG_SLOT_RESET);

        INITIALIZE_TD_FOR_SLOT_TRANSFER(td, TransferContext);

        token = td->HwTD.Token;

        // if this transfer is split then we must sterminate on short packet unless its the
        // tail end

        //if (TransferContext->MpFlags == MPTX_SPLIT_TRANSFER &&
        //    TEST_FLAG(TransferUrb->TransferFlags, USBD_TRANSFER_DIRECTION_IN)) {
        //    SET_T_BIT(td->HwTD.AltNext_qTD.HwAddress);
        //} else {
        //    CLEAR_T_BIT(td->HwTD.AltNext_qTD.HwAddress);
        //}

        // next and alt next fields are static

        //
        // fields for data TD
        //
        token.InterruptOnComplete = ioc;

        // EHCI_ASSERT(DeviceData, TEST_FLAG(TransferUrb->TransferFlags, USBD_TRANSFER_DIRECTION_IN));
        // use direction specified in transfer
        if (TEST_FLAG(TransferUrb->TransferFlags, USBD_TRANSFER_DIRECTION_IN)) {
            token.Pid = HcTOK_In;
        } else {
            token.Pid = HcTOK_Out;
        }

        token.DataToggle = HcTOK_Toggle1;
        //token.Active = 1;

        td->HwTD.Token = token;

        LOGENTRY(DeviceData, G, '_dt2', td, lengthMapped, TransferUrb->TransferBufferLength);

        lengthMapped =
            EHCI_MapAsyncTransferToTd(DeviceData,
                                      EndpointData->Parameters.MaxPacketSize,
                                      lengthMapped,
                                      NULL,
                                      TransferContext,
                                      td,
                                      TransferSGList);

        tdIdx++;
    }

    // special case the zero length transfer
    if (TransferUrb->TransferBufferLength == 0) {

        TransferContext->PendingTds++;

        td = slot->Tds[tdIdx];

        token = td->HwTD.Token;

        CLEAR_FLAG(td->Flags, TD_FLAG_SLOT_RESET);

        // Initialize the TD, including setting BufferPage64[0-4] to zero.
        //
        INITIALIZE_TD_FOR_SLOT_TRANSFER(td, TransferContext);

        EHCI_ASSERT(DeviceData, TransferContext->PendingTds == 1);

        // use direction specified in transfer
        if (TEST_FLAG(TransferUrb->TransferFlags, USBD_TRANSFER_DIRECTION_IN)) {
            token.Pid = HcTOK_In;
        } else {
            token.Pid = HcTOK_Out;
        }

        token.DataToggle = HcTOK_Toggle1;
        //token.Active = 1;

        // point to a dummy buffer
        td->HwTD.BufferPage[0].ul =
            td->PhysicalAddress;

        token.BytesToTransfer = 0;
        td->TransferLength = 0;

        tdIdx++;

    } else {

        token = td->HwTD.Token;
    }

    // IoC of the last TD should always be set
    token.InterruptOnComplete = 1;
    td->HwTD.Token = token;

    // put the request on the hardware queue
    LOGENTRY(DeviceData, G, '_LGA',  TransferContext->PendingTds, 0, slot);

    EHCI_ASSERT(DeviceData, TransferContext->PendingTds != 0);
    tailTd = slot->Tds[TransferContext->PendingTds-1];

    // point the next ptr of the tail td at the next slot
    SET_NEXT_TD(DeviceData, tailTd, TRANSFER_DESCRIPTOR_PTR(tailTd->AltNextHcdTD));

    if (EndpointData->CurrentSlot == NULL) {
        EndpointData->CurrentSlot = slot;

        EHCI_sMode_ActivateQh(DeviceData, EndpointData, slot);
    }

    // activate the programmed TDs in this slot
    for (i=0; i< TransferContext->PendingTds; i++) {
        td = slot->Tds[i];
        token = td->HwTD.Token;
        token.Active = 1;
        td->HwTD.Token = token;
    }

    // we now have a complete setup of TDs representing this transfer
    // (Next)td(1)->td(2)->td(3)->td(4)->...
    // (AltNext) all point to next slot

    // tell the hc we have async transfers available
    if (AsyncEndpoint) {
        EHCI_EnableAsyncList(DeviceData);
    } else {
        EHCI_EnablePeriodicList(DeviceData);
    }

    if (EndpointData->EpStatus == ENDPOINT_STATUS_HALT) {
        EHCI_ASSERT(DeviceData, EndpointData->QueueHead->HwQH.Overlay.qTD.Token.Halted == 1);
    }

    return USBMP_STATUS_SUCCESS;
}


VOID
EHCI_sMode_AbortSlot(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PENDPOINT_SLOT AbortSlot,
     __inout PTRANSFER_CONTEXT AbortTransferContext
    )
/*++

Routine Description:

    Aborts the transfer for a slot

Arguments:

Return Value:

--*/
{
    PHCD_QUEUEHEAD_DESCRIPTOR   qh;
    HC_QTD_TOKEN                qhToken;
    ULONG                       tdIdx;
    PHCD_TRANSFER_DESCRIPTOR    td;
    HC_QTD_TOKEN                tdToken;
    ULONG                       byteCount;

    LOGENTRY(DeviceData, G, '_Slr', 0, AbortTransferContext, AbortSlot);

    qh = EndpointData->QueueHead;

    // Scan the TDs for this slot and update the transfer context
    // BytesTransferred for any TDs that have transferred data but have
    // not yet been processed as completed TDs.
    //
    for (tdIdx = 0; tdIdx < EndpointData->TdsPerSlot; tdIdx++) {

        td = AbortSlot->Tds[tdIdx];

        if (TEST_FLAG(td->Flags, TD_FLAG_DONE)) {
            //
            // This TD has already been processed as completed by
            // EHCI_sMode_PollEndpointSlot().  Skip this TD but continue
            // checking the rest of the TDs for this slot.
            //
            continue;
        }

        if (TEST_FLAG(td->Flags, TD_FLAG_SKIP)) {
            //
            // This TD and all of the remaining TDs for this slot have
            // already been processed by EHCI_sMode_PollEndpointSlot().
            // Stop checking the rest of the TDs for this slot.
            //
            break;
        }

        if (TEST_FLAG(td->Flags, TD_FLAG_SLOT_RESET)) {
            //
            // This TD and none of the remaining TDs were in use for the
            // current transfer for this slot.  Stop checking the rest
            // of the TDs for this slot.
            //
            break;
        }

        tdToken = td->HwTD.Token;

        if (tdToken.Active == 1) {
            //
            // This TD was still active so the state of this TD should
            // not have been written back to this TD from the QH.  (See
            // section 4.10.4 Write Back qTD in the EHCI specification).
            // If any data was transferred for this TD it sould be
            // reflected in the current state of the QH Overlay.
            //
            // Calculate how many bytes were transferred for this TD and
            // add that to the transfer context BytesTransferred and then
            // stop checking the rest of the TDs for this slot.
            //

            qhToken = qh->HwQH.Overlay.qTD.Token;

            if (qhToken.Active &&
                qh->HwQH.CurrentTD.HwAddress == td->PhysicalAddress) {

                byteCount = EHCI_TdBytesTransferred(td->TransferLength, qhToken.BytesToTransfer);

                AbortTransferContext->BytesTransferred += byteCount;
            }

            break;
        }

        // After eliminating the other possibilities above it should be
        // the case that this TD was in use for the transfer for this
        // slot but it is no longer active and it also was not processed
        // by EHCI_sMode_PollEndpointSlot().
        //
        // Calculate how many bytes were transferred for this TD and
        // add that to the transfer context BytesTransferred.
        //
        byteCount = EHCI_TdBytesTransferred(td->TransferLength, tdToken.BytesToTransfer);

        AbortTransferContext->BytesTransferred += byteCount;
    }

    // Reset this slot and all of the TDs for this slot back to the clean
    // state.
    //
    EHCI_sMode_ResetSlot(DeviceData,
                         EndpointData,
                         AbortSlot);
}


VOID
EHCI_sMode_AbortAsyncTransfer(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PTRANSFER_CONTEXT AbortTransferContext
    )
/*++

Routine Description:

    Called when the a transfer needs to be freed that is on the hardware

Arguments:

Return Value:

--*/
{
    PENDPOINT_SLOT abortSlot, nextSlot, currentSlotSave;
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    HC_QTD_TOKEN token;
    ULONG split_id;

    qh = EndpointData->QueueHead;

    // The endpoint should not be in the schedule

    LOGENTRY(DeviceData, G, '_sa1', qh, AbortTransferContext, EndpointData);
    EHCI_ASSERT(DeviceData, !TEST_FLAG(qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE));

    currentSlotSave = EndpointData->CurrentSlot;

    // locate the slot for this transfer

    abortSlot = (PENDPOINT_SLOT) AbortTransferContext->EpSlot;
    EHCI_ASSERT(DeviceData, abortSlot != NULL);

    if (currentSlotSave) {
        // transfer should be after current (if we have current)
        EHCI_ASSERT(DeviceData, abortSlot->Sequence >= currentSlotSave->Sequence);
    }
    EHCI_sMode_AbortSlot(DeviceData, EndpointData, abortSlot, AbortTransferContext);
    // one less pending transfer
    EHCI_DecPendingTransfer(DeviceData, EndpointData, AbortTransferContext);

    split_id = AbortTransferContext->TransferParameters->SequenceNumber;
    LOGENTRY(DeviceData, G, '_sa2', split_id, AbortTransferContext, EndpointData);
    // Abort any split transfers
    nextSlot = EHCI_sMode_IncSlotIdx(DeviceData, EndpointData, abortSlot);

    // complete any split transfers here

    while (nextSlot->State == slot_Busy) {

        PHCD_TRANSFER_DESCRIPTOR split_td;
        PTRANSFER_CONTEXT split_transfer = NULL;
        PTRANSFER_PARAMETERS split_tp;

        split_td = nextSlot->Tds[0];
        LOGENTRY(DeviceData, G, '_sa3', split_td, split_id, nextSlot);

        split_transfer = TRANSFER_CONTEXT_PTR(split_td->TransferContext);
        LOGENTRY(DeviceData, G, '_sa4', split_transfer, split_id, nextSlot);

        split_tp = split_transfer->TransferParameters;

        if (split_tp->SequenceNumber != split_id) {
            break;
        }
        nextSlot->State = slot_Abort;

        nextSlot = EHCI_sMode_IncSlotIdx(DeviceData, EndpointData, nextSlot);
        LOGENTRY(DeviceData, G, '_sa5', 0, 0, nextSlot);

    }

    if (nextSlot->State == slot_Busy) {
        LOGENTRY(DeviceData, G, '_sa6', 0, 0, nextSlot);
        // out of order cancel
        // re-sequence the slots if we removed a transfer from the middle
        // this is only necessary if slot count > 2
        EHCI_KdPrint((DeviceData, 0, "'EHCI out-of-order cancel\n", DeviceData));
        //TEST_TRAP(DeviceData);
    }


    // update the queue head so that next slot is current
    qh->HwQH.CurrentTD.HwAddress = nextSlot->FirstTd->PhysicalAddress;
    if (nextSlot->State == slot_Busy) {
        qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = nextSlot->FirstTd->PhysicalAddress;
        qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = nextSlot->FirstTd->PhysicalAddress;
    } else {
        qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = EHCI_TERMINATE_BIT;
        qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = EHCI_TERMINATE_BIT;
    }

    // Re-initialize the Token in the QH Overlay
    //
    token = qh->HwQH.Overlay.qTD.Token;
    token.BytesToTransfer = 0;
    token.Active = 0;
    token.ErrorCounter = 3;
    token.SplitXstate = 0;
    token.C_Page = 0;
    qh->HwQH.Overlay.qTD.Token = token;

    EHCI_ASSERT(DeviceData, AbortTransferContext->PendingTds != 0);
}


ULONG
EHCI_CheckHwSync_Async(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    This routine is only called by EHCI_CheckHwSync().

    This routine is only called for Control and Bulk endpoints.

    A non-zero return value indicates the endpoint is not referenced by
    the host controller hardware.  This routine is used only to indicate
    a transition into or out of the internal HW list.

    A zero return value indicates the endpoint is busy and the
    transition is in progress.

    This routine is always called with the USBPORT Fdo.CoreFunctionSpin
    spinlock held.

--*/
{

    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    // defaulting to busy (still on HW)
    ULONG hw_done = Qh_Busy;
    PHC_OPERATIONAL_REGISTER hcOp;
    USBCMD cmd;
    USBSTS sts;
    KIRQL irql;
    BOOLEAN registersUnavailable = FALSE;

    hcOp = DeviceData->OperationalRegisters;
    qh = EndpointData->QueueHead;

    LOGENTRY(DeviceData, G, '_cp1', 0, EndpointData, qh);
    // for bulk, control we use hardware sync state stored in endpoint

    EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);

    //
    // We don't want to touch the hardware if it is powered down
    //
    if (DeviceData->HcSuspendedFlag) {
        registersUnavailable = TRUE;
    } else {

        cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
        sts.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);

        EHCI_UpdateScheduleStates(DeviceData, cmd, sts);

        // if the Async list if off then report this endpoint as ready.
        if (sts.AsyncScheduleStatus == 0 &&
            cmd.AsyncScheduleEnable == 0) {
            registersUnavailable = TRUE;

        }
    }

    if (registersUnavailable) {

        ULONG qhInSchedule;

        qhInSchedule = TEST_FLAG(qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE);
        LOGENTRY(DeviceData, G, '_cp2', 0, qhInSchedule, qh);
        switch (EndpointData->CurEpState) {
            case ENDPOINT_REMOVE:
            case ENDPOINT_PAUSE:
                EHCI_ASSERT(DeviceData, !qhInSchedule);
                break;
            case ENDPOINT_ACTIVE:
                EHCI_ASSERT(DeviceData, qhInSchedule);
                break;
        }

        // Flush the doorbell list since the schedule is off.  This will
        // also reset the AsyncListAddr register to point to the
        // persistent QH to ensure that the AsyncListAddr register does
        // not point to an unlinked QH.
        //
        EHCI_ClearAsyncDoorbellList(DeviceData);

        hw_done = Qh_Not_Busy;
        EHCI_UpdateEndpointAsFreedFromHwCache(DeviceData, EndpointData);
        LOGENTRY(DeviceData, G, '_cp3', 0, qhInSchedule, qh);

        EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);

    } else {
        // indicate state based on the hw sync field in the endpoint


        hw_done = EndpointData->HwSyncState;
        LOGENTRY(DeviceData, G, '_cp4', 0, hw_done, 0);
        if (hw_done == Qh_Not_Busy) {
            LOGENTRY(DeviceData, G, '_cp5', 0, hw_done, 0);
            // should not be on list if we are indicating complete
            EHCI_ASSERT(DeviceData, EndpointData->AcfLink.Blink == NULL);
            EHCI_ASSERT(DeviceData, EndpointData->AcfLink.Flink == NULL);
            EHCI_ASSERT(DeviceData, DeviceData->EpAtTheDoor != EndpointData);
            //EndpointData->StateChangeTc = 0;
            //EndpointData->HwSyncState = hw_done = Qh_Not_Busy;

            EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);

        } else {
            LARGE_INTEGER tickStateChange, tickNow;
            LARGE_INTEGER ticks;

            LOGENTRY(DeviceData, G, '_cp6', 0, hw_done, 0);

            // If the number of timer ticks since the doorbell was last
            // requested for this endpoint is greater than 2ms worth of
            // timer ticks then call EHCI_iAsyncCacheFlush() with the
            // DoorbellRetry parameter set to TRUE to request the
            // doorbell again for this endpoint.
            //
            KeQueryTickCount(&tickNow);

            tickStateChange = EndpointData->StateChangeTc;
            EHCI_ASSERT(DeviceData, tickStateChange.QuadPart != 0);
            LOGENTRY(DeviceData, G, '_cx1', tickStateChange.HighPart, tickStateChange.LowPart, qh);
            LOGENTRY(DeviceData, G, '_cx2', tickNow.HighPart, tickNow.LowPart, qh);

            if (tickNow.QuadPart > tickStateChange.QuadPart) {
                LARGE_INTEGER cycles_p_tick, time_to_complete;
                ULONG timerIncrement;

                ticks.QuadPart = tickNow.QuadPart - tickStateChange.QuadPart;

                timerIncrement = KeQueryTimeIncrement();
                cycles_p_tick = RtlConvertUlongToLargeInteger(timerIncrement);

                if (cycles_p_tick.QuadPart == 0) {
                    cycles_p_tick.QuadPart = 1;
                }
                time_to_complete.QuadPart = 20000 / cycles_p_tick.QuadPart;

                LOGENTRY(DeviceData, G, '_cx3', ticks.HighPart, ticks.LowPart, cycles_p_tick.LowPart);

                // if ticks > 2ms ; after 2 frames then indicate HW is done
                if (ticks.QuadPart > time_to_complete.QuadPart) {
                    LOGENTRY(DeviceData, G, '_cx4', 0, 0, qh);

                    // cehck for stale request, if this happens then try to ring again
                    if (DeviceData->EpAtTheDoor == EndpointData) {
                        LOGENTRY(DeviceData, G, '_cx5', 0, 0, qh);
                        EHCI_iAsyncCacheFlush(DeviceData, EndpointData, TRUE);
                    }
                }
            }

            EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);
        }
    }

    if (hw_done == Qh_Busy) {

        // Request USBPORT to queue a timer DPC to queue a call to the
        // USBPORT ISR DPC, which will cause USBPORT to process the
        // endpoint state change list again, which will cause
        // EHCI_CheckHwSync() to be called again.
        //
        USBPORT_INVALIDATE_CONTROLLER(DeviceData, UsbMpSimulateInterrupt);
    }

    LOGENTRY(DeviceData, G, '_cp7', 0, hw_done, 0);
    return hw_done;
}


ULONG
EHCI_CheckHwSync(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    Called to see if an endpoint is dereferenced by the hardware.

    A non-zero return value indicates the endpoint is not referenced by
    the host controller hardware.  This routine is used only to indicate
    a transition into or out of the internal HW list.

    A zero return value indicates the endpoint is busy and the
    transition is in progress.

    This MINIPORT_PollHwSync() routine is always called with the USBPORT
    Fdo.CoreFunctionSpin spinlock held.

--*/
{

    ULONG hw_done = 0;

    if (!TEST_FLAG(EndpointData->Flags, EHCI_EDFLAG_HC_RESET)) {

        LOGENTRY(DeviceData, G, '_hw1', EndpointData, hw_done, 0);
        switch(EndpointData->Parameters.TransferType) {

            case Control:
            case Bulk:
                hw_done = EHCI_CheckHwSync_Async(DeviceData, EndpointData);
                break;

            case Interrupt:
                hw_done = EHCI_CheckHwSync_Periodic(DeviceData, EndpointData);
                break;

            case Isochronous:
                hw_done = EndpointData->HwSyncState;
                break;
        }

        LOGENTRY(DeviceData, G, '_hw2', EndpointData, hw_done, 0);

    } else {

        hw_done = Qh_Not_Busy;
    }

    return hw_done;
}


VOID
EHCI_DecPendingTransfer(
     __inout PDEVICE_DATA       DeviceData,
     __inout PENDPOINT_DATA     EndpointData,
     __inout PTRANSFER_CONTEXT  TransferContext
    )
/*++

Routine Description:

    Called by:

    EHCI_ProcessDoneAsyncTd() from EHCI_AbortControlTransfer() when a
    Control transfer is aborted.

    EHCI_ProcessDoneAsyncTd() from EHCI_PollControlEndpoint() when a
    Control transfer is completed normally.

    EHCI_sMode_PollEndpointSlot() from EHCI_sMode_PollEndpoint() from
    EHCI_PollPerAsyEndpoint() when a Bulk or Interrupt transfer is
    completed normally.

    EHCI_sMode_AbortAsyncTransfer() from EHCI_AbortTransfer() when a
    Bulk or Interrupt transfer is aborted.

    EHCI_InternalPollIsoEndpoint() from EHCI_SubmitIsoTransfer() or
    EHCI_PollIsoEndpoint() when a Full-Speed Isochronous transfer is
    completed normally.

    EHCI_InternalPollHsIsoEndpoint() from EHCI_SubmitIsoTransfer() or
    EHCI_PollIsoEndpoint() when a High-Speed Isochronous transfer is
    completed normally.

    EHCI_AbortIsoTransfer() from EHCI_AbortTransfer() when an
    Isochronous transfer is aborted.

--*/
{
    EHCI_ASSERT(DeviceData, TransferContext->Sig == SIG_EHCI_TRANSFER);

    switch (EndpointData->TransferTypeFlag) {

        case isoch_periodic:
            // An Isochronous transfer should be on the host controller
            // PendingIsochListHead list and on the endpoint
            // IsoTransferListHead list.
            //
            EHCI_ASSERT(DeviceData, !IsListEmpty(&DeviceData->PendingIsochListHead));
            EHCI_ASSERT(DeviceData, !IsListEmpty(&EndpointData->IsoTransferListHead));
            EHCI_ASSERT(DeviceData, EndpointData->IsochTransfers);
            EndpointData->IsochTransfers--;
            break;

        case int_periodic:
            // An Interupt transfer should be on the host controller
            // PendingSmodeINT_ListHead list and on the endpoint
            // SmodeTransferListHead list.
            //
            EHCI_ASSERT(DeviceData, !IsListEmpty(&DeviceData->PendingSmodeINT_ListHead));
            EHCI_ASSERT(DeviceData, !IsListEmpty(&EndpointData->SmodeTransferListHead));
            EHCI_ASSERT(DeviceData, EndpointData->SmodeTransfers);
            EndpointData->SmodeTransfers--;
            break;

        case bulk_async:
        case control_async:
            // A Control or Bulk transfer should be on the host
            // controller PendingSmodeCTB_ListHead list and on the
            // endpoint SmodeTransferListHead list.
            //
            EHCI_ASSERT(DeviceData, !IsListEmpty(&DeviceData->PendingSmodeCTB_ListHead));
            EHCI_ASSERT(DeviceData, !IsListEmpty(&EndpointData->SmodeTransferListHead));
            EHCI_ASSERT(DeviceData, EndpointData->SmodeTransfers);
            EndpointData->SmodeTransfers--;

            if (EndpointData->SmodeTransfers == 0) {
                //
                // On a transition from 1 transfer to 0 transfers remove
                // the Control or Bulk QH from the hardware Asynchronous
                // List which is reachable through the EHCI host
                // controller ASYNCLISTADDR (Current Asynchronous List
                // Address) Register.
                //
                if (DeviceData->AsyncQhIdleRemoval) {
                    EHCI_RemoveQueueHeadFromHWAsyncList(DeviceData,
                        EndpointData->QueueHead);
                }
            }

            break;
    }

    // Remove the transfer from the host controller
    // PendingIsochListHead, PendingSmodeINT_ListHead, or
    // PendingSmodeCTB_ListHead list.
    //
    EHCI_ASSERT(DeviceData, TransferContext->DD_TransferLink.Flink != NULL);
    EHCI_ASSERT(DeviceData, TransferContext->DD_TransferLink.Blink != NULL);
    RemoveEntryList(&TransferContext->DD_TransferLink);
    TransferContext->DD_TransferLink.Flink = NULL;
    TransferContext->DD_TransferLink.Blink = NULL;

    // Remove the transfer from the endpoint IsoTransferListHead or
    // SmodeTransferListHead list.
    //
    EHCI_ASSERT(DeviceData, TransferContext->TransferLink.Flink != NULL);
    EHCI_ASSERT(DeviceData, TransferContext->TransferLink.Blink != NULL);
    RemoveEntryList(&TransferContext->TransferLink);
    TransferContext->TransferLink.Flink = NULL;
    TransferContext->TransferLink.Blink = NULL;
}


VOID
EHCI_IncPendingTransfer(
     __inout PDEVICE_DATA       DeviceData,
     __inout PENDPOINT_DATA     EndpointData,
     __inout PTRANSFER_CONTEXT  TransferContext
    )
/*++

Routine Description:

    Called by:

    EHCI_ControlTransfer() when a new Control transfer is queued on the
    host controller hardware schedule.

    EHCI_sMode_Transfer() from EHCI_BulkTransfer() when a new Bulk
    transfer is queued on the host controller hardware schedule.

    EHCI_sMode_Transfer() from EHCI_InterruptTransfer() when a new
    Interrupt transfer is queued on the host controller hardware
    schedule.

    EHCI_SubmitIsoTransfer() when a new Isochronous transfer is queued
    on the host controller hardware schedule.

--*/
{
    EHCI_ASSERT(DeviceData, TransferContext->Sig == SIG_EHCI_TRANSFER);

    switch (EndpointData->TransferTypeFlag) {

        case isoch_periodic:
            // An Isochronous transfer is inserted on the host
            // controller PendingIsochListHead list and on the endpoint
            // IsoTransferListHead list.
            //
            InsertTailList(&DeviceData->PendingIsochListHead, &TransferContext->DD_TransferLink);
            InsertTailList(&EndpointData->IsoTransferListHead, &TransferContext->TransferLink);
            EndpointData->IsochTransfers++;
            break;

        case int_periodic:
            // An Interupt transfer is inserted on the host controller
            // PendingSmodeINT_ListHead list and on the endpoint
            // SmodeTransferListHead list.
            //
            InsertTailList(&DeviceData->PendingSmodeINT_ListHead, &TransferContext->DD_TransferLink);
            InsertTailList(&EndpointData->SmodeTransferListHead, &TransferContext->TransferLink);
            EndpointData->SmodeTransfers++;
            break;

        case bulk_async:
        case control_async:
            // A Control or Bulk transfer is inserted on the host
            // controller PendingSmodeCTB_ListHead list and on the
            // endpoint SmodeTransferListHead list.
            //
            InsertTailList(&DeviceData->PendingSmodeCTB_ListHead, &TransferContext->DD_TransferLink);
            InsertTailList(&EndpointData->SmodeTransferListHead, &TransferContext->TransferLink);
            EndpointData->SmodeTransfers++;

            if (EndpointData->SmodeTransfers == 1) {
                //
                // On a transition from 0 transfers to 1 transfer insert
                // the Control or Bulk QH into the hardware Asynchronous
                // List which is reachable through the EHCI host
                // controller ASYNCLISTADDR (Current Asynchronous List
                // Address) Register.
                //
                if (DeviceData->AsyncQhIdleRemoval) {
                    EHCI_InsertQueueHeadInHWAsyncList(DeviceData,
                        EndpointData->QueueHead);
                }
            }

            break;
    }
}


PENDPOINT_SLOT
EHCI_sMode_IncSlotIdx(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PENDPOINT_SLOT Slot
    )
/*++

Routine Description:

    Increments slot index (advances to next slot in the ring buffer).

--*/
{
    ULONG sIdx;
    PENDPOINT_SLOT nextSlot;
    PHCD_QUEUEHEAD_DESCRIPTOR Qh;

    sIdx = Slot->Idx;

    LOGENTRY(DeviceData, G, '_in1', 0, EndpointData, Slot);

    EHCI_ASSERT(DeviceData, Slot == EndpointData->SlotPtr[sIdx]);

    sIdx++;
    if (sIdx >= EndpointData->SlotCount) {
        sIdx = 0;
    }

    nextSlot = EndpointData->SlotPtr[sIdx];
    EHCI_ASSERT(DeviceData, nextSlot == EndpointData->SlotPtr[sIdx]);
    EHCI_ASSERT(DeviceData, nextSlot->State != slot_Abort);

    if (nextSlot->State == slot_Busy) {
        EndpointData->CurrentSlot = nextSlot;
    } else {
        EndpointData->CurrentSlot = NULL;

        if (DeviceData->TerminateQH) {
            Qh = EndpointData->QueueHead;
            Qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = EHCI_TERMINATE_BIT;
            Qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = EHCI_TERMINATE_BIT;
        }
    }

    LOGENTRY(DeviceData, G, '_in2', EndpointData->CurrentSlot, nextSlot, 0);

    return nextSlot;
}


VOID
EHCI_sMode_ShortTransfer(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PTRANSFER_CONTEXT TransferContext
    )
/*++

Routine Description:

    Travks short transfers
--*/
{
    LOGENTRY(DeviceData, G, '_shr',
            EndpointData,
            0,
            TransferContext);

    if (EndpointData == EHCI_EpTrap) {
        TEST_TRAP(DeviceData);
    }
}


/**

async list Idle changes (Intel Core Duo)

**/

PVOID
EHCI_AllocNpPool(
     __in ULONG NumberOfBytes
     )
{
    PVOID p = NULL;

    if (NumberOfBytes) {
        p = ExAllocatePoolWithTag(NonPagedPool, NumberOfBytes, EHCI_TAG);

        if (p) {
            RtlZeroMemory(p, NumberOfBytes);
        }
    }

    return p;
}


VOID
EHCI_iAssertQueueHead(
    __inout PDEVICE_DATA DeviceData,
    __inout PHCD_QUEUEHEAD_DESCRIPTOR Qh
    )
/*++

    Sanity check the qh

--*/
{
    LOGENTRY(DeviceData, G, '_aq1', Qh, 0, 0);

    EHCI_ASSERT(DeviceData, DeviceData->AcfLockThread != NULL);

    switch(Qh->Sig) {
    case SIG_HCD_IQH:
        break;

    case SIG_HCD_AQH:
        break;

    case SIG_HCD_QH:
    case SIG_HCD_DQH:
        break;

    default:
        EHCI_ASSERTMSG(DeviceData, "Invalid Qh->Sig", FALSE);
    }
}


//
// note: this returns the async queue head with the proper qh type bits
//

HW_32BIT_PHYSICAL_ADDRESS
EHCI_iGetAsyncListHwAddress(
    PDEVICE_DATA DeviceData
    )
{
    HW_LINK_POINTER asyncHwQh;
    PHCD_QUEUEHEAD_DESCRIPTOR asyncQh;

    EHCI_ASSERT(DeviceData, DeviceData->AcfLockThread != NULL);

    asyncQh = DeviceData->AsyncQueueHead;
    // this had better be valid
    EHCI_iAssertQueueHead(DeviceData, asyncQh);
    asyncHwQh.HwAddress = asyncQh->PhysicalAddress;

    EHCI_ASSERT(DeviceData, asyncHwQh.HwAddress != 0);

    SET_QH(asyncHwQh.HwAddress);

    return asyncHwQh.HwAddress;
}


_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_iEnableAsyncList(
    PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

    This routine enables the host controller Asynchronous Schedule
    processing.  Specifically it sets the USBCMD AsyncScheduleEnable
    bit.

    The AcfListSpin spinlock is assumed to be held when this routine is
    called and that is the responsibility of the caller.

    This routine is only called by EHCI_EnableAsyncList(),
    EHCI_RefAsyncIdle(), and EHCI_NotifyTransferQueueState() which all
    acquire the AcfListSpin spinlock prior to calling this routine.
*/
{
    PHC_OPERATIONAL_REGISTER    hcOp;
    USBCMD                      cmd;
    USBSTS                      sts;
    HW_32BIT_PHYSICAL_ADDRESS   asyncHwQhAddr;

    hcOp = DeviceData->OperationalRegisters;

    EHCI_ASSERT(DeviceData, DeviceData->AcfLockThread != NULL);

    cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
    sts.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);

    EHCI_UpdateScheduleStates(DeviceData, cmd, sts);

    do {

        if (cmd.ul == 0xffffffff) {
            break;
        }

        // If we are currently waiting for the doorbell request from the
        // host controller, prevent changes to the schedule (which is already
        // assumed to running when the doorbell is pending). When the doorbell
        // cycle completes and moves to "AsyncDoorbellState_NotWaiting" state, the
        // last request pending (enable/disable) will be retried
        //

        if (DeviceData->PreventCmdChangesWithDoorbellPending != FALSE &&
            DeviceData->CurrentDoorbellRetryEnableValue == FALSE &&
            EHCI_IsAsyncDoorbellPending(DeviceData) != FALSE) {

            EHCI_SetAsyncScheduleState(DeviceData, AsyncScheduleState_EnablePendingDoorbell);
            break;

        }

        // must be running to turn on the async list
        EHCI_ASSERT(DeviceData, cmd.HostControllerRun);

        asyncHwQhAddr = EHCI_iGetAsyncListHwAddress(DeviceData);

        LOGENTRY(DeviceData, G, '_eA1', cmd.ul, sts.ul, asyncHwQhAddr);

        if (cmd.AsyncScheduleEnable == 0)
        {
            if (DeviceData->ImmediateAsyncEnableChange == 0) {

                // Enhanced Host Controller Interface Specification
                // Revision: 1.0, Section 4.8 Asynchronous Schedule:
                //
                // "Software must not modify the Asynchronous Schedule
                // Enable bit unless the value of the Asynchronous
                // Schedule Enable bit equals that of the Asynchronous
                // Schedule Status bit."
                //
                ULONG   stallcount;

                for (stallcount = 0; stallcount < 15; stallcount++)
                {
                    if (cmd.AsyncScheduleEnable == sts.AsyncScheduleStatus)
                    {
                        break;
                    }
                    else
                    {
                        KeStallExecutionProcessor(25);

                        cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
                        sts.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);

                        LOGENTRY(DeviceData, G, '_eAS', cmd.ul, sts.ul, 0);
                    }
                }

                if (cmd.AsyncScheduleEnable != sts.AsyncScheduleStatus) {
                    // Stats used for debugging purposes.
                    DeviceData->StatAsyncEnableTimeoutCount++;
                }
            }

            if (sts.AsyncScheduleStatus == 0) {
                // if the async list hardware state is off re-write the async list head
                WRITE_REGISTER_ULONG(&hcOp->AsyncListAddr, asyncHwQhAddr);
            }

            DeviceData->StatAsyncEnableCount++;

            cmd.AsyncScheduleEnable = 1;
            cmd.IntOnAsyncAdvanceDoorbell = 0;

            WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

            EHCI_SetAsyncScheduleState(DeviceData, AsyncScheduleState_EnableRequested);

            {
                EVENT_DATA_DESCRIPTOR   EventDataDescriptor[1];

                // The first (and only in this case) data item is the
                // Host Controller data, which is filled in
                // EventDataDescriptor[] by USBPORT.
                //
                RtlZeroMemory(EventDataDescriptor,sizeof(EVENT_DATA_DESCRIPTOR));
                USBPORT_ETW_WRITE(DeviceData,
                                  &USBPORT_ETW_EVENT_HC_ASYNC_SCHEDULE_ENABLE,
                                  NULL,
                                  1,
                                  EventDataDescriptor);
            }
        }


    } WHILE (0);

    LOGENTRY(DeviceData, G, '_eAX', cmd.ul, sts.ul, 0);
}


_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_iDisableAsyncList(
     PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

    This routine disables the host controller Asynchronous Schedule
    processing.  Specifically it clears the USBCMD AsyncScheduleEnable
    bit.

    The AcfListSpin spinlock is assumed to be held when this routine is
    called and that is the responsibility of the caller.

    This routine is only called by EHCI_DisableAsyncList(),
    EHCI_DerefAsyncIdle(), and EHCI_NotifyTransferQueueState() which all
    acquire the AcfListSpin spinlock prior to calling this routine.
*/
{
    PHC_OPERATIONAL_REGISTER    hcOp;
    USBCMD                      cmd;
    USBSTS                      sts;

    hcOp = DeviceData->OperationalRegisters;

    // may be called for HCT

    EHCI_ASSERT(DeviceData, DeviceData->AcfLockThread != NULL);

    cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
    sts.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);

    EHCI_UpdateScheduleStates(DeviceData, cmd, sts);
    LOGENTRY(DeviceData, G, '_dsA', cmd.ul, sts.ul, 0);

    do {
        if (cmd.ul == 0xffffffff) {
            break;
        }

        if (DeviceData->PreventCmdChangesWithDoorbellPending != FALSE &&
            DeviceData->CurrentDoorbellRetryEnableValue == FALSE &&
            EHCI_IsAsyncDoorbellPending(DeviceData) != FALSE) {

            EHCI_SetAsyncScheduleState(DeviceData, AsyncScheduleState_DisablePendingDoorbell);
            LOGENTRY(DeviceData, G, '_dA0', 0, 0, cmd.ul);
            break;
        }

        if (!IsListEmpty(&DeviceData->AcfListHead) ||
            DeviceData->EpAtTheDoor != NULL) {
            // pending doorbell operations means it is unsafe to go idle
            LOGENTRY(DeviceData, G, '_dA1', 0, 0, cmd.ul);
            break;
        }

        if (DeviceData->SstoolApiActive) {
            // keep async list enabled if we are running diagnostics
            LOGENTRY(DeviceData, G, '_dA2', 0, 0, cmd.ul);
            break;
        }

        //
        // This check is for the case where EHCI_iDisableAsyncList() is
        // not called from EHCI_NotifyTransferQueueState().
        //
        if (!IsListEmpty(&DeviceData->PendingSmodeCTB_ListHead) ||
            DeviceData->AsyncIdleDetectEnabled == 0) {
            // control or bulk transfers present or idle is disabled
            LOGENTRY(DeviceData, G, '_dA3', 0, 0, cmd.ul);
            break;
        }

        if (!IsListEmpty(&DeviceData->AsyncIdleRefListHead)) {
            // other pending operations means it is unsafe to go idle
            LOGENTRY(DeviceData, G, '_dA4', 0, 0, cmd.ul);
            break;
        }

        if (cmd.AsyncScheduleEnable == 1)
        {
            if (DeviceData->ImmediateAsyncEnableChange == 0) {

                // Enhanced Host Controller Interface Specification
                // Revision: 1.0, Section 4.8 Asynchronous Schedule:
                //
                // "Software must not modify the Asynchronous Schedule
                // Enable bit unless the value of the Asynchronous
                // Schedule Enable bit equals that of the Asynchronous
                // Schedule Status bit."
                //
                ULONG   stallcount;

                for (stallcount = 0; stallcount < 15; stallcount++)
                {
                    if (cmd.AsyncScheduleEnable == sts.AsyncScheduleStatus)
                    {
                        break;
                    }
                    else
                    {
                        KeStallExecutionProcessor(25);

                        cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
                        sts.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);

                        LOGENTRY(DeviceData, G, '_dAS', cmd.ul, sts.ul, 0);
                    }
                }

                if (cmd.AsyncScheduleEnable != sts.AsyncScheduleStatus) {
                    // Stats used for debugging purposes.
                    DeviceData->StatAsyncDisableTimeoutCount++;
                }
            }

            DeviceData->StatAsyncDisableCount++;

            cmd.AsyncScheduleEnable = 0;
            cmd.IntOnAsyncAdvanceDoorbell = 0;

            WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

            EHCI_SetAsyncScheduleState(DeviceData, AsyncScheduleState_DisableRequested);

            // Note this just requests the async schedule to disable it
            // is not a synchronous function, the list may be running on
            // return from this function. We synchronize with the real
            // status in the flush function.
            //
            // The reason is that this function is used to optimize perf
            // by turning off the async list when no xfers are availble.
            // We don't want to block the diver waititing for the list
            // to disable.

            {
                EVENT_DATA_DESCRIPTOR   EventDataDescriptor[1];

                // The first (and only in this case) data item is the
                // Host Controller data, which is filled in
                // EventDataDescriptor[] by USBPORT.
                //
                RtlZeroMemory(EventDataDescriptor,sizeof(EVENT_DATA_DESCRIPTOR));
                USBPORT_ETW_WRITE(DeviceData,
                                  &USBPORT_ETW_EVENT_HC_ASYNC_SCHEDULE_DISABLE,
                                  NULL,
                                  1,
                                  EventDataDescriptor);
            }
        }

    } WHILE (0);

    LOGENTRY(DeviceData, G, '_dAX', cmd.ul, sts.ul, 0);
}


PEHCI_IDLE_REF_CONTEXT
EHCI_RefAsyncIdle(
    __inout PDEVICE_DATA DeviceData,
    __in EHCI_IDLE_REF_TYPE RefType,
    __in_opt PVOID RefObject
    )
/*++

Routine Description:

    This routine adds a reference to the AsyncIdleRefListHead list
    which prevents the host controller asynchronous schedule from being
    disabled.  It also calls EHCI_iEnableAsyncList() to ensure that the
    host controller asynchronous schedule is currently enabled.

    This is called by root hub port status register routines which do
    not involve transfer requests but which still require that the host
    controller asynchronous schedule is enabled for proper operation.

    The AcfListSpin spinlock is acquired here and held around all of the
    processing done here.

--*/
{
    PEHCI_IDLE_REF_CONTEXT rx;
    KIRQL irql;

    // Acquire the AcfListSpin spinlock around all of the processing
    // done here.
    //
    EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);

#ifndef USB_FORCE_MEMORY_STRESS
    rx = EHCI_AllocNpPool(sizeof(struct _EHCI_IDLE_REF_CONTEXT));
#endif

    LOGENTRY(DeviceData, G, '_rai', RefObject, RefType, rx);

    if (rx) {
        rx->Sig = SIG_ASYNC_IDLE_REF;
        rx->RefType = RefType;
        rx->RefObject = RefObject;
        // add it to the list

        InsertTailList(&DeviceData->AsyncIdleRefListHead, &rx->IdleRefLink);
    } else {
        // if we cannot properly ref the async idle feture then just
        // increment the refcount
        DeviceData->AsyncIdleRefNoRes++;
    }

    // ref'ing async idle turns on the list
    EHCI_iEnableAsyncList(DeviceData);

    EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);

    return rx;
}


VOID
EHCI_DerefAsyncIdle(
    __inout PDEVICE_DATA DeviceData,
    __in PEHCI_IDLE_REF_CONTEXT Rx
    )
/*++

Routine Description:

    This routine is the companion to EHCI_RefAsyncIdle().

    It removes a reference to the AsyncIdleRefListHead list when the
    processing is complete that required the host controller
    asynchronous schedule be enabled.

    The host controller asynchronous schedule cannot be disabled until
    all references have been removed from the AsyncIdleRefListHead list.

    The AcfListSpin spinlock is acquired here and held around all of the
    processing done here.

--*/
{
    KIRQL irql;

    // Acquire the AcfListSpin spinlock around all of the processing
    // done here.
    //
    EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);

    LOGENTRY(DeviceData, G, '_dai', &DeviceData->AsyncIdleRefListHead, 0, Rx);

    if (Rx) {

        EHCI_ASSERT(DeviceData, Rx->Sig == SIG_ASYNC_IDLE_REF);
        RemoveEntryList(&Rx->IdleRefLink);

        ExFreePool(Rx);
    } else if (DeviceData->AsyncIdleRefNoRes > 0) {
        DeviceData->AsyncIdleRefNoRes--;
    } else {
        EHCI_ASSERTMSG(DeviceData, "Rx should not be NULL", FALSE);
        USBPORT_BUGCHECK(DeviceData);
    }

    if (IsListEmpty(&DeviceData->AsyncIdleRefListHead) &&
        DeviceData->AsyncIdleRefNoRes == 0) {
        EHCI_iDisableAsyncList(DeviceData);
    }

    EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);
}
