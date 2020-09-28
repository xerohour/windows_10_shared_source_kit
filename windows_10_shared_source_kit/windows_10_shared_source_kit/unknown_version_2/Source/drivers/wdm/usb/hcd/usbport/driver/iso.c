/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    iso.c

Abstract:

    Constructs and handle iso transfers.

Environment:

    kernel mode only

Notes:

Revision History:

    01-01-00 : created
    01-12-04 : fix bugs

--*/

#include "common.h"

#ifdef USBPORT_WPP
#include "iso.tmh"
#endif


MP_HW_PHYSICAL_ADDRESS
USBPORT_GetPhysicalAddressFromOffset(
    PTRANSFER_SG_LIST SgList,
    ULONG Offset,
    PULONG Idx
    )
/*++

Routine Description:

Arguments:

Return Value:


--*/
{
    PTRANSFER_SG_ENTRY sg;
    ULONG i;
    MP_HW_PHYSICAL_ADDRESS p;

    for(i=0; i < SgList->SgCount; i++) {

        if (Offset >= SgList->SgEntry[i].StartOffset &&
            Offset < SgList->SgEntry[i].StartOffset +
                SgList->SgEntry[i].Length) {
            break;
        }

    }

    // i = idx of sg entry that this packet falls in
    sg = &SgList->SgEntry[i];

    // the 'offset' of the packet minus the start offset of the
    // sg entry is the offset into this  sg entry that the packet
    // starts

    //   {.sgN...}{.sgN+1.}{.sgN+2.}{.sgN+3.}     sg entries
    //      b--------------------------->e        client buffer
    //       <p0><p1><p2><p3><p4><p5><p6>         urb 'packets'
    //   x--------x--------x--------x--------x    physical pages
    //       <m0><m1><m2><m3><m4><m5><m6>

    *Idx = i;

    USBPORT_ASSERT(Offset >= sg->StartOffset);

    p = sg->LogicalAddress;
    p.Hw32 += (Offset - sg->StartOffset);

    return p;
}

USBD_STATUS
USBPORT_InitializeIsoTransfer(
    PDEVICE_OBJECT FdoDeviceObject,
    PTRANSFER_URB Urb,
    PHCD_TRANSFER_CONTEXT Transfer
    )
/*++

Routine Description:

    Initialize the iso transfer structure from the
    orginal client URB and SG list



       {.sgN...}{.sgN...}{..sgN..}              sg entries
          b--------------------------->e        client buffer
           <p0><p1><p2><p3><p4><p5><p6>         urb 'packets'
       x--------x--------x--------x--------x    physical pages
           <m0><m1><m2><m3><m4><m5><m6>


    The sg entries are not that useful to the USB controller
    HW since the HW deals in usb packets so we create a structure
    that describes the physical addresses assocaited with each
    packet.

Arguments:

Return Value:


--*/
{
    PDEVICE_EXTENSION devExt;
    PMINIPORT_ISO_TRANSFER isoTransfer;
    PUSBD_ISO_PACKET_DESCRIPTOR usbdPak;
    PMINIPORT_ISO_PACKET mpPak;
    PTRANSFER_SG_LIST sgList;
    ULONG p, i, cf, j;
    MP_HW_PHYSICAL_ADDRESS b0, b1;
    ULONG b1Idx, b0Idx;
    BOOLEAN highSpeed;
    ULONG period;
    PUSBD_DEVICE_HANDLE devH = NULL;
    ULONG MaxPacketSize;

    ASSERT_TRANSFER(Transfer);
    highSpeed = TEST_FLAG(Transfer->Flags, USBPORT_TXFLAG_HIGHSPEED);

    devH = (PUSBD_DEVICE_HANDLE)Transfer->UrbDeviceHandle;
    ASSERT_DEVICE_HANDLE(devH);

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    isoTransfer = Transfer->IsoTransfer;
    sgList = &Transfer->SgList;
    period = Transfer->IsoPeriod;

    LOGENTRY(FdoDeviceObject, LOG_ISO, 'iISO', Urb, Transfer, isoTransfer);

    isoTransfer->Sig = SIG_ISOCH;
    isoTransfer->PacketCount = Urb->u.Isoch.NumberOfPackets;
    isoTransfer->SystemAddress = sgList->MdlSystemAddress;

    DMA_KdPrint(("Urb BufferMdl 0x%p (%d)\n", Transfer->TransferBufferMdl, Transfer->Tp.TransferBufferLength));
    // note: proper start frame was computed when the transfer
    // was queued.

    // check the current frame if it is too late to transmit any
    // packets set the appropriate errors in the URB

    cf = MPf_Get32BitFrameNumber(devExt);

    LOGENTRY(FdoDeviceObject, LOG_ISO, 'isCf', cf,
             Urb->u.Isoch.StartFrame, isoTransfer);

    //
    // Log the delta
    //
    if(GREATER_THAN_EQUAL_TO(cf, Urb->u.Isoch.StartFrame)) {
        devH->PerformanceData.TotalIsoLatency += (cf - Urb->u.Isoch.StartFrame);
        devExt->Fdo.PerformanceData.TotalIsoLatency += (cf - Urb->u.Isoch.StartFrame);
    }

    if (highSpeed) {
        ULONG endFrame;

        // See Enhanced Host Controller Interface Specification for
        // Universal Serial Bus, 3.3 Isochronous (High-Speed) Transfer
        // Descriptor (iTD), 3.3.2 iTD Transaction Status and Control
        // List, Transaction X Length: "The maximum value this field may
        // contain is 0xC00 (3072)."
        //
        MaxPacketSize = 3072;

        // for high speed we are dealing with microframes
        // packets are microframes and may be periodic but we
        // still need an integer number of frames
        //
        // (8 packets per frame for period 1)
        // we don't support iso periods > 8

        // this needs to be failed
        if (period > 8) {
            Urb->u.Isoch.StartFrame = 0;
            isoTransfer->PacketCount = 0;
            DbgTrace((UsbpDebugTrace, "HIGH SPEED ISO bad parameters - packets\n"));
            TEST_TRAP();
            return USBD_STATUS_INVALID_PARAMETER;
        }

        if (period > 8 || ((isoTransfer->PacketCount * period) % 8) != 0) {
            // cause - to fail
            Urb->u.Isoch.StartFrame = 0;
            isoTransfer->PacketCount = 0;
            DbgTrace((UsbpDebugTrace, "HIGH SPEED ISO bad parameters - packets\n"));

            return USBD_STATUS_INVALID_PARAMETER;
        }

        endFrame = Urb->u.Isoch.StartFrame +
                (Urb->u.Isoch.NumberOfPackets * period)/8;

        LOGENTRY(FdoDeviceObject, LOG_ISO, 'perd', period,
                 endFrame, Transfer);

        for (i = 0; i < (Urb->u.Isoch.NumberOfPackets * period)/8; i++) {
            if (GREATER_THAN_EQUAL_TO(cf, (i + Urb->u.Isoch.StartFrame))) {
                // late packet, comput the packet
                p = i*(8/period);
                for (j=0; j<8/period; j++) {
                    usbdPak = &Urb->u.Isoch.IsoPacket[p+j];

                    if (usbdPak->Status == USBD_STATUS_NOT_SET) {
                        usbdPak->Status = USBD_STATUS_ISO_NA_LATE_USBPORT;

                        LOGENTRY(FdoDeviceObject, LOG_ISO, 'late', 
                                 cf, Urb->u.Isoch.StartFrame + i, Transfer);
                    }
                }
            }
        }
    } else {

        // See Enhanced Host Controller Interface Specification for
        // Universal Serial Bus, 3.4 Split Transaction Isochronous
        // Transfer Descriptor (siTD), 3.4.3 siTD Transfer State, Total
        // Bytes To Transfer:  "Maximum value is 1023 (3FFh)"
        //
        // See also Universal Host Controller Interface (UHCI) Design
        // Guide, 3.2 Transfer Descriptor (TD), 3.2.3 TD TOKEN (DWORD 2:
        // 08-0Bh), Maximum Length (MaxLen): "Note that the maximum
        // length allowed by the USB specification is 1023 bytes."
        //
        MaxPacketSize = 1023;

        USBPORT_ASSERT(period == 1);
        for (i = 0; i< Urb->u.Isoch.NumberOfPackets; i++) {
            if (GREATER_THAN_EQUAL_TO(cf, (i + Urb->u.Isoch.StartFrame))) {
                usbdPak = &Urb->u.Isoch.IsoPacket[i];

                if (usbdPak->Status == USBD_STATUS_NOT_SET) {
                    usbdPak->Status = USBD_STATUS_ISO_NA_LATE_USBPORT;

                    LOGENTRY(FdoDeviceObject, LOG_ISO, 'late', 
                             cf, i + Urb->u.Isoch.StartFrame, Transfer);
                }
            }
        }
    }
    // initialize the packets

    for (p=0; p< isoTransfer->PacketCount; p++) {

        ULONG n;

        usbdPak = &Urb->u.Isoch.IsoPacket[p];
        mpPak = &isoTransfer->Packets[p];

        //Preserve error status for packets that are already set
        if (usbdPak->Status != USBD_STATUS_NOT_SET) {
            mpPak->UsbdStatus = usbdPak->Status;
        }
        
        // first Zero the mp packet
        RtlZeroMemory(mpPak, sizeof(*mpPak));
        
        // each packet has an 'offset' from the start
        // of the client buffer we need to find the sg
        // entry associated with this packet based on
        // this 'offset' and get the physical address
        // for the satrt of the packet

        b0 = USBPORT_GetPhysicalAddressFromOffset(sgList,
                                                   usbdPak->Offset,
                                                   &b0Idx);

        LOGENTRY(FdoDeviceObject, LOG_ISO, 'ib0=',
            usbdPak->Offset, b0Idx, p);

        // length is implied by the offset specified in the
        // usbd packet, the length is the difference between the
        // current packet start offset and the next packet start
        // offset.

        if (p == isoTransfer->PacketCount - 1) {
            n = Transfer->Tp.TransferBufferLength;
        } else {
            n = Urb->u.Isoch.IsoPacket[p+1].Offset;
        }
        mpPak->Length = n - usbdPak->Offset;

        // Don't allow packet lengths to exceed the specification
        // maximum limits.  The UHCI miniport packet double buffers
        // assume packets are not illegally larger than allowed in the
        // USB specification.
        //
        if (mpPak->Length > MaxPacketSize) {
            mpPak->Length = MaxPacketSize;
        }

        if (highSpeed) {
            mpPak->FrameNumber = Urb->u.Isoch.StartFrame+(p/(8/period));
            mpPak->MicroFrameNumber = p%(8/period);
        } else {
            mpPak->FrameNumber = Urb->u.Isoch.StartFrame+p;
        }

        // get the sg entry associated with the last byte of the packet
        b1 = USBPORT_GetPhysicalAddressFromOffset(sgList,
                                                   usbdPak->Offset +
                                                     mpPak->Length -1,
                                                   &b1Idx);

        LOGENTRY(FdoDeviceObject, LOG_ISO, 'ib1=',
            usbdPak->Offset, b1Idx, usbdPak->Offset + mpPak->Length);

        USBPORT_ASSERT(b1Idx - b0Idx < 2);

        if (b0Idx == b1Idx) {
            // this packet is contained by a single sg entry
            mpPak->BufferPointer0 = b0;
            mpPak->BufferPointer0Length = mpPak->Length;
            mpPak->BufferPointerCount = 1;

        } else {
            PTRANSFER_SG_ENTRY sg;

            // this packet crosses an sg entry...
            mpPak->BufferPointer0 = b0;
            // since this packet bumps in to the end
            // of a page the length is page_size minus
            // phys offset

            mpPak->BufferPointer0Length = 0x1000;
            mpPak->BufferPointer0Length -= (b0.Hw32 & 0xFFF);

            // since we crossed an sg entry on this packet
            // the start address will be the phys address
            // of the sg entry
            sg = &sgList->SgEntry[b1Idx];
            mpPak->BufferPointer1 = sg->LogicalAddress;
            mpPak->BufferPointer1Length = mpPak->Length -
                mpPak->BufferPointer0Length;

            mpPak->BufferPointerCount = 2;

            USBPORT_ASSERT(mpPak->BufferPointer1.Hw64.QuadPart != mpPak->BufferPointer0.Hw64.QuadPart);
        }

        if (mpPak->BufferPointerCount == 2) {
            DMA_KdPrint(("Packet %d, B0 0x%llx (0%x bytes), B1 0x%llx (0%x bytes) \n", p , mpPak->BufferPointer0.Hw64.QuadPart, mpPak->BufferPointer0Length, mpPak->BufferPointer1.Hw64.QuadPart, mpPak->BufferPointer1Length ));
        } else {
            DMA_KdPrint(("Packet %d, B0 0x%llx (0%x bytes) \n", p , mpPak->BufferPointer0.Hw64.QuadPart, mpPak->BufferPointer0Length ));
        }
        
        USBPORT_ASSERT(mpPak->BufferPointerCount != 0);

    }

    return USBD_STATUS_SUCCESS;
}

#if 0
VOID
USBPORT_ErrorCompleteIsoTransfer(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoint,
    PHCD_TRANSFER_CONTEXT Transfer,
    PEP_LOCK_HANDLE EpLock
    )
/*++

Routine Description:

Arguments:

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION devExt;
    PTRANSFER_URB urb;
    USBD_STATUS usbdStatus;
    PMINIPORT_ISO_TRANSFER isoTransfer;
    ULONG bytesTransferred, p;
    
    ASSERT_TRANSFER(Transfer);
    isoTransfer = Transfer->IsoTransfer;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ASSERT_ENDPOINT(Endpoint);

    usbdStatus = USBD_STATUS_ISOCH_REQUEST_FAILED;
    urb = Transfer->Urb;
    LOGENTRY(FdoDeviceObject, LOG_ISO, 'cpLi', 0,
             Transfer, urb);
    ASSERT_TRANSFER_URB(urb);

    DbgTrace((UsbpDebugTrace, "  ISO (USBD_STATUS_ISOCH_REQUEST_FAILED) - packets %d\n",
        isoTransfer->PacketCount));
    // do some conversion on the isoTransfer data
    bytesTransferred = 0;
    urb->u.Isoch.ErrorCount = isoTransfer->PacketCount;

    for (p=0; p<isoTransfer->PacketCount; p++) {

        urb->u.Isoch.IsoPacket[p].Status =
            isoTransfer->Packets[p].UsbdStatus;


         DbgTrace((UrbIsoTrace,
         "ErrorCompleteIsoTransfer.1(urb%p.adr%d.ep%x.pkt%d.frame%d.mf%d.Len%d.usbst%x)\n", 
                urb, 
                Endpoint->Parameters.DeviceAddress,
                Endpoint->Parameters.EndpointAddress,
                p,
                isoTransfer->Packets[p].FrameNumber,
                isoTransfer->Packets[p].MicroFrameNumber,
                isoTransfer->Packets[p].LengthTransferred,
                isoTransfer->Packets[p].UsbdStatus
                )); 


    }

    urb->TransferBufferLength = bytesTransferred;
   
    DbgTrace((UrbIsoTrace,
        "ErrorCompleteIsoTransfer.2(urb%p.adr%d.ep%x.bytes%d.errc%d.usbst%x)\n", 
                urb, 
                Endpoint->Parameters.DeviceAddress,
                Endpoint->Parameters.EndpointAddress,
                bytesTransferred,
                urb->u.Isoch.ErrorCount,
                usbdStatus
                )); 

    // insert the transfer on to our 'done list', this routine initaites
    // a DPC to complete the transfers

//usbperf
    USBPORT_QueueDoneTransfer(FdoDeviceObject,
                              Transfer,
                              usbdStatus,
                              EpLock);

}
#endif

USBD_STATUS
USBPORT_FlushIsoTransfer(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoint,
    PTRANSFER_PARAMETERS TransferParameters,
    PMINIPORT_ISO_TRANSFER IsoTransfer
    )
/*++

Routine Description:

    called to complete a transfer.

    ** Must be called in the context of PollEndpoint

Arguments:

    FdoDeviceObject - USB host controller

    Endpoint - referebced endpoint

Return Value:

    USBD_STATUS error code

--*/
{
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer;
    PTRANSFER_URB urb;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    ULONG bytesTransferred, p;
       
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'cpTi', 0,
        TransferParameters->FrameCompleted,
        TransferParameters);

    TRANSFER_FROM_TPARAMETERS(transfer, TransferParameters);
    ASSERT_TRANSFER(transfer);

    urb = transfer->Urb;
    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'cpUi', 0,
        transfer, urb);
    ASSERT_TRANSFER_URB(urb);

    transfer->MiniportFrameCompleted =
        TransferParameters->FrameCompleted;

    // do some conversion on the isoTransfer data
    bytesTransferred = 0;

    urb->u.Isoch.ErrorCount = 0;

    for (p=0; p<IsoTransfer->PacketCount; p++) {
        
        DbgTrace((UrbIsoTrace,
        "FlushIsoTransfer1(urb%p.adr%d.ep%x.pkt%d.frame%d.mf%d.Len%d.usbst%x)\n", 
                urb, 
                Endpoint->Parameters.DeviceAddress,
                Endpoint->Parameters.EndpointAddress,
                p,
                IsoTransfer->Packets[p].FrameNumber,
                IsoTransfer->Packets[p].MicroFrameNumber,
                IsoTransfer->Packets[p].LengthTransferred,
                IsoTransfer->Packets[p].UsbdStatus)); 
     
        bytesTransferred += IsoTransfer->Packets[p].LengthTransferred;

        urb->u.Isoch.IsoPacket[p].Status =
            IsoTransfer->Packets[p].UsbdStatus;

        // note:
        // in an effort to create some consistency we handle the buffer
        // underrun case here.
        // That is:
        //      if the SHORT_XFER_OK flag is set AND
        //      the error is USBD_STATUS_DATA_UNDERRUN
        //      then
        //      ignore the error

        // NOTE: The OHCI controllers report USBD_STATUS_DATA_UNDERRUN
        // for short iso packets

        if (/*urb->TransferFlags & USBD_SHORT_TRANSFER_OK && */
            urb->u.Isoch.IsoPacket[p].Status == USBD_STATUS_DATA_UNDERRUN) {
            urb->u.Isoch.IsoPacket[p].Status = USBD_STATUS_SUCCESS;
            LOGENTRY(FdoDeviceObject, LOG_ISO, 'igER',
                    urb->u.Isoch.IsoPacket[p].Status,
                    transfer,
                    urb);
        }

        if (urb->u.Isoch.IsoPacket[p].Status != USBD_STATUS_SUCCESS) {
            urb->u.Isoch.ErrorCount++;
        }

        if (transfer->Direction == ReadData) {
            urb->u.Isoch.IsoPacket[p].Length =
                IsoTransfer->Packets[p].LengthTransferred;
        }

        LOGENTRY(FdoDeviceObject, LOG_ISO, 'isoP',
            urb->u.Isoch.IsoPacket[p].Length,
            urb->u.Isoch.IsoPacket[p].Status,
            bytesTransferred);

    }

    if (urb->u.Isoch.ErrorCount ==
        IsoTransfer->PacketCount) {
        // all errors set global status in urb
        usbdStatus = USBD_STATUS_ISOCH_REQUEST_FAILED;
    }
    DbgTrace((UrbIsoTrace,
        "FlushIsoTransfer2(urb%p.adr%d.ep%x.bytes%d.errc%d.usbst%x)\n", 
                urb, 
                Endpoint->Parameters.DeviceAddress,
                Endpoint->Parameters.EndpointAddress,
                bytesTransferred,
                urb->u.Isoch.ErrorCount,
                usbdStatus)); 

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'isoD', 0,
        bytesTransferred, urb->u.Isoch.ErrorCount);

    transfer->MiniportBytesTransferred = bytesTransferred;

    return usbdStatus;
}


VOID
USBPORTSVC_CompleteIsoTransfer(
    PDEVICE_DATA DeviceData,
    PENDPOINT_DATA EndpointData,
    PTRANSFER_PARAMETERS TransferParameters,
    PMINIPORT_ISO_TRANSFER IsoTransfer
    )
/*++

Routine Description:

    isoch analog to USBPORTSVC_CompleteTransfer, called by miniport to complete an isochronous transfer.

    ** Must be called in the context of PollEndpoint

--*/
{
    PHCD_ENDPOINT endpoint;
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer;
    PDEVICE_OBJECT hcFdo;
    USBD_STATUS usbdStatus;
    PXDPC_CONTEXT xdpc;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);

    // sould be in the context of the usbx dpc which polls the endpoint
    
    xdpc = &devExt->Fdo.UsbHcIntDpc;
    hcFdo = devExt->HcFdoDeviceObject;

    TRANSFER_FROM_TPARAMETERS(transfer, TransferParameters);
    ASSERT_TRANSFER(transfer);

    LOGENTRY(hcFdo, LOG_ISO, 'isod', 0, transfer, devExt->CurrentAbortTransfer);
    if (devExt->CurrentAbortTransfer != transfer && 
        devExt->CurrentAbortTransfer != NULL) {
        // not current means we are being called in the context of AbortTransfer
        // we will allow this
        xdpc = NULL;
    }        

    endpoint = USBPORT_EndpointFromHandle(transfer->EpRefHandle);
    ASSERT_ENDPOINT(endpoint);
    
    DbgTrace((UrbIsoTrace,
        "SvcCompleteIsoTransfer(urb%p.adr%d.ep%x)\n", transfer->Urb, endpoint->Parameters.DeviceAddress,
                endpoint->Parameters.EndpointAddress)); 
       
    SET_FLAG(transfer->Flags, USBPORT_TXFLAG_MPCOMPLETED);

    usbdStatus = USBPORT_FlushIsoTransfer(hcFdo,
                                          endpoint,
                                          TransferParameters,
                                          IsoTransfer);


    USBPORT_Core_iCompleteTransfer(hcFdo, 
                                  xdpc,
                                  transfer,
                                  usbdStatus,
                                  Ev_TX_MiniportComplete,
                                  TX_HwDone);


}
