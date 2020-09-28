/*++

Copyright (c) 1999, 2000 Microsoft Corporation

Module Name:

   async.c

Abstract:

   miniport transfer code for sstool (single step tool) interface

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

typedef struct _SS_PACKET_CONTEXT {
    MP_HW_POINTER    Qh;
    MP_HW_POINTER    Td;
    MP_HW_POINTER    Data;
    ULONG padTo8[2];
} SS_PACKET_CONTEXT, *PSS_PACKET_CONTEXT;

#define EHCI_TEST_TD_ALIGNMENT    256

C_ASSERT((sizeof(SS_PACKET_CONTEXT) <= EHCI_TEST_TD_ALIGNMENT));
C_ASSERT((sizeof(HCD_QUEUEHEAD_DESCRIPTOR) <= EHCI_TEST_TD_ALIGNMENT));
C_ASSERT((sizeof(HCD_TRANSFER_DESCRIPTOR) <= EHCI_TEST_TD_ALIGNMENT));


//implements the following miniport functions:

//non paged
//EHCI_StartSendOnePacket
//EHCI_EndSendOnePacket

USB_MINIPORT_STATUS
USBMPFN
EHCI_StartSendOnePacket(
     __inout PDEVICE_DATA DeviceData,
     __in PMP_PACKET_PARAMETERS PacketParameters,
     __inout_bcount(*PacketLength) PUCHAR PacketData,
     __in PULONG PacketLength,
     __inout_xcount(WorkSpaceLength) PUCHAR WorkspaceVirtualAddress,
     HW_32BIT_PHYSICAL_ADDRESS WorkspacePhysicalAddress,
     ULONG WorkSpaceLength,
     OUT USBD_STATUS *UsbdStatus
    )
/*++

Routine Description:

    insert structures to transmit a single packet -- this is for debug
    tool purposes only so we can be a little creative here.

    This MINIPORT_StartSendOnePacket() routine is always called with
    the USBPORT Fdo.CoreFunctionSpin spinlock held.

Arguments:

Return Value:

--*/
{
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    PHCD_TRANSFER_DESCRIPTOR td;
    HC_QTD_TOKEN token;
    PUCHAR pch, data;
    PSS_PACKET_CONTEXT context;
    HW_LINK_POINTER hwQh, tdLink;
    ULONG phys, qhPhys, tdPhys, dataPhys, i, pchLength;
    ULONG siz;
    KIRQL irql;

    // allocate an TD from the scratch space and
    // initialize it
    phys = WorkspacePhysicalAddress;
    pch = WorkspaceVirtualAddress;
    pchLength = WorkSpaceLength;

    LOGENTRY(DeviceData, G, '_ssS', phys, 0, pch);

    EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);
    DeviceData->SstoolApiActive = 1;
    EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);

    // specify a TD alignment to work around HW bugs.
    siz = EHCI_TEST_TD_ALIGNMENT;

    context = (PSS_PACKET_CONTEXT) pch;
    pch += siz;
    phys += siz;
    pchLength -= siz;

    // carve out a qh
    qhPhys = phys;
    qh = (PHCD_QUEUEHEAD_DESCRIPTOR) pch;
    pch += siz;
    phys += siz;
    pchLength -= siz;
    LOGENTRY(DeviceData, G, '_ssQ', qh, 0, qhPhys);

    // carve out a TD
    tdPhys = phys;
    td = (PHCD_TRANSFER_DESCRIPTOR) pch;
    pch += siz;
    phys += siz;
    pchLength -= siz;
    LOGENTRY(DeviceData, G, '_ssT', td, 0, tdPhys);


    // use the rest for data
    LOGENTRY(DeviceData, G, '_ssD', PacketData, *PacketLength, 0);

    dataPhys = phys;
    data = pch;
    if (*PacketLength > pchLength) {
        *UsbdStatus = USBD_STATUS_NOT_SUPPORTED;
        return USBMP_STATUS_NOT_SUPPORTED;
    }

    RtlCopyMemory(data, PacketData, *PacketLength);
    pch+=*PacketLength;
    phys+=*PacketLength;

    // init qh
    RtlZeroMemory(qh, sizeof(*qh));
    qh->PhysicalAddress = qhPhys;
    ENDPOINT_DATA_PTR(qh->EndpointData) = NULL;
    qh->Sig = SIG_HCD_DQH;

    hwQh.HwAddress = qh->PhysicalAddress;
    SET_QH(hwQh.HwAddress);

    //qh->HwQH.EpChars.HeadOfReclimationList = 1;

    // manual Toggle
    qh->HwQH.EpChars.DataToggleControl = HcEPCHAR_Toggle_From_qTD;

    // init the hw descriptor
    qh->HwQH.EpChars.DeviceAddress =
        PacketParameters->DeviceAddress;
    qh->HwQH.EpChars.EndpointNumber =
        PacketParameters->EndpointAddress;


    qh->HwQH.EpCaps.HighBWPipeMultiplier = 1;
    qh->HwQH.EpCaps.HubAddress = 0;
    qh->HwQH.EpCaps.PortNumber = 0;

    // link back to ourselves
    //qh->HwQH.HLink.HwAddress = hwQh.HwAddress;

    switch (PacketParameters->Speed) {
    case ss_Low:
        qh->HwQH.EpChars.EndpointSpeed = HcEPCHAR_LowSpeed;
        qh->HwQH.EpCaps.HubAddress = PacketParameters->HubDeviceAddress;
        qh->HwQH.EpCaps.PortNumber = PacketParameters->PortTTNumber;
        if((ss_Setup == PacketParameters->Type) ||
           (0 == qh->HwQH.EpChars.EndpointNumber)) {
            qh->HwQH.EpChars.ControlEndpointFlag = 1;
        }
        break;
    case ss_Full:
        qh->HwQH.EpChars.EndpointSpeed = HcEPCHAR_FullSpeed;
        qh->HwQH.EpCaps.HubAddress = PacketParameters->HubDeviceAddress;
        qh->HwQH.EpCaps.PortNumber = PacketParameters->PortTTNumber;
        if((ss_Setup == PacketParameters->Type) ||
           (0 == qh->HwQH.EpChars.EndpointNumber)) {
            qh->HwQH.EpChars.ControlEndpointFlag = 1;
        }
        break;
    case ss_High:
        qh->HwQH.EpChars.EndpointSpeed = HcEPCHAR_HighSpeed;
        break;
    default:
        USBPORT_BUGCHECK(DeviceData);
    }
// jdxxx
//qh->HwQH.EpChars.EndpointSpeed = HcEPCHAR_HighSpeed;

    qh->HwQH.EpChars.MaximumPacketLength =
        PacketParameters->MaximumPacketSize;

    // init td
    RtlZeroMemory(td, sizeof(*td));
    for (i=0; i<5; i++) {
        td->HwTD.BufferPage[i].ul = 0x0badf000;
    }

    td->PhysicalAddress = tdPhys;
    td->Sig = SIG_HCD_TD;

    token.ul = 0;

    switch(PacketParameters->Type) {
    case ss_Setup:
        LOGENTRY(DeviceData, G, '_sSU', 0, 0, 0);
        token.Pid = HcTOK_Setup;
        break;
    case ss_In:
        LOGENTRY(DeviceData, G, '_ssI', 0, 0, 0);
        token.Pid = HcTOK_In;
        break;
    case ss_Out:
        token.Pid = HcTOK_Out;
        LOGENTRY(DeviceData, G, '_ssO', 0, 0, 0);
        break;
    case ss_Iso_In:
        break;
    case ss_Iso_Out:
        break;
    }

    switch(PacketParameters->Toggle) {
    case ss_Toggle0:
        token.DataToggle = HcTOK_Toggle0;
        break;
    case ss_Toggle1:
        token.DataToggle = HcTOK_Toggle1;
        break;
    }

    token.ErrorCounter = PacketParameters->ErrorCount;

    // point TD at the data
    td->HwTD.BufferPage[0].ul = dataPhys;
    token.BytesToTransfer = *PacketLength;

    token.Active = 1;

    td->HwTD.Token = token;

    // prime the overlay with td so that this td
    // becomes the current td.
    qh->HwQH.Overlay.qTD.Next_qTD.HwAddress =
        td->PhysicalAddress;

    tdLink.HwAddress = 0;
    SET_T_BIT(tdLink.HwAddress);
    td->HwTD.Next_qTD.HwAddress = tdLink.HwAddress;
    td->HwTD.AltNext_qTD.HwAddress = tdLink.HwAddress;

    QH_DESCRIPTOR_PTR(context->Qh) = qh;
    TRANSFER_DESCRIPTOR_PTR(context->Td) = td;
    HW_PTR(context->Data) = data;

    // Set the initial UsbdStatus value for when MINIPORT_EndSendOnePacket()
    // is called.
    //
    *UsbdStatus = USBD_STATUS_SUCCESS;

    // stick the QH in the schedule and wait for it to complete

    // swap the async qh, wait one frame then
    // replace the old value.

    // NOTE: This will interrupt normal bus operation for one ms

    //WRITE_REGISTER_ULONG(&hcOp->AsyncListAddr, hwQh.HwAddress);
    EHCI_InsertQueueHeadInAsyncList(DeviceData, qh);

    EHCI_EnableAsyncList(DeviceData);

    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
USBMPFN
EHCI_EndSendOnePacket(
     __inout PDEVICE_DATA DeviceData,
     __in PMP_PACKET_PARAMETERS PacketParameters,
     __out_bcount(*PacketLength) PUCHAR PacketData,
     __inout PULONG PacketLength,
     __inout_xcount(WorkSpaceLength) PUCHAR WorkspaceVirtualAddress,
     HW_32BIT_PHYSICAL_ADDRESS WorkspacePhysicalAddress,
     __in ULONG WorkSpaceLength,
     __out USBD_STATUS *UsbdStatus
    )
/*++

Routine Description:

    This MINIPORT_EndSendOnePacket() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.

Arguments:

Return Value:

--*/
{
    PSS_PACKET_CONTEXT context;
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    PHCD_TRANSFER_DESCRIPTOR td;
    HC_QTD_TOKEN Token;
    PUCHAR data;
    KIRQL irql;

    LOGENTRY(DeviceData, G, '_ssE', 0, 0, 0);

    context = (PSS_PACKET_CONTEXT) WorkspaceVirtualAddress;

    qh = QH_DESCRIPTOR_PTR(context->Qh);
    td = TRANSFER_DESCRIPTOR_PTR(context->Td);
    data = HW_PTR(context->Data);

    LOGENTRY(DeviceData, G, '_sE2', qh, td, *PacketLength);

    // If the transfer is not complete (i.e. it is still Active) when
    // MINIPORT_EndSendOnePacket() is called, AND UsbdStatus is not set
    // to USBD_STATUS_DEV_NOT_RESPONDING nor USBD_STATUS_CANCELED
    // indicating a transfer timeout, AND the endpoint is the default
    // control endpoint (i.e.  EndpointNumber is zero), then
    // MINIPORT_EndSendOnePacket() should:
    // (1) Return USBMP_STATUS_BUSY.
    //
    Token = td->HwTD.Token;
    if ((0 == qh->HwQH.EpChars.EndpointNumber) &&
        (USBD_STATUS_SUCCESS == *UsbdStatus) &&
        (1 == Token.Active) &&
        (1 != Token.Halted) &&
        (1 != Token.XactErr) &&
        (1 != Token.BabbleDetected) &&
        (1 != Token.DataBufferError)) {
        return USBMP_STATUS_BUSY;
    }

    // At this point the transfer has either completed or timed out.
    // Remove the QueueHead from the schedule.
    //
    if (*UsbdStatus != USBD_STATUS_CANCELED) {

        EHCI_RemoveQueueHeadFromAsyncList(DeviceData, qh);

        EHCI_DisableAsyncList(DeviceData);
    }

    // Check the transfer status from the QH Overlay, not from the TD.
    // The Write Back qTD action is only performed by the host
    // controller when the Active bit is set to zero.  If the transfer
    // request timed out while the Active bit was still set the QH
    // Overlay will not have been written back to the TD.
    //
    Token = qh->HwQH.Overlay.qTD.Token;

    // Return success or the appropriate error here.
    //
    if (Token.Active == 1) {
        if (*UsbdStatus != USBD_STATUS_CANCELED) {
            // The request timed out while the Active bit was still set.
            // Return USBMP_STATUS_BUSY so that MINIPORT_EndSendOnePacket()
            // is called one more time after a 10 ms wait after the
            // QueueHead was removed from the schedule.
            *UsbdStatus = USBD_STATUS_CANCELED;
            return USBMP_STATUS_BUSY;
        }
    } else if (Token.Halted == 1) {
        // A transfer error occurred.  Return the specific error.
        if (Token.XactErr) {
            *UsbdStatus = USBD_STATUS_XACT_ERROR;
        } else if (Token.BabbleDetected) {
            *UsbdStatus = USBD_STATUS_BABBLE_DETECTED;
        } else if (Token.DataBufferError) {
            *UsbdStatus = USBD_STATUS_DATA_BUFFER_ERROR;
        } else if (Token.MissedMicroFrame) {
            *UsbdStatus = USBD_STATUS_XACT_ERROR;
        } else {
            *UsbdStatus = USBD_STATUS_STALL_PID;
        }
    } else {
        *UsbdStatus = USBD_STATUS_SUCCESS;
    }

    // The length of the data actually transferred is the original
    // transfer length minus the remaining transfer length.
    //
    if (*PacketLength >= Token.BytesToTransfer) {
        *PacketLength = *PacketLength - Token.BytesToTransfer;
    } else {
        *PacketLength = 0;
    }

    LOGENTRY(DeviceData, G, '_sE3', Token.BytesToTransfer, td, *PacketLength);

    // If this was an IN transfer (and not a SETUP or OUT transfer)
    // then copy back the data that was actually transferred.
    //
    if (Token.Pid == HcTOK_In) {
        RtlCopyMemory(PacketData, data, *PacketLength);
    }

    EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);
    DeviceData->SstoolApiActive = 0;
    EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);

    return USBMP_STATUS_SUCCESS;
}
