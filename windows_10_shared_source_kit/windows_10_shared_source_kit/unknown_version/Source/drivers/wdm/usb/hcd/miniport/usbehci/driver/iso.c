/*++

Copyright (c) 1999, 2000 Microsoft Corporation

Module Name:

   iso.c

Abstract:

   miniport transfer code for interrupt endpoints

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1999, 2000 Microsoft Corporation.  All Rights Reserved.


Revision History:

    1-1-01 : created, jdunn

--*/

#include "common.h"

/*
    We build a table of 32 TDs for iso endpoints and insert them in the
    schedule, these TDs are static -- we only change the buffer pointers.

    The TD 'table' represents a 32ms snapshot of time.

    We end up with each iso endpoint siTD list as a column in the table



frame  dummyQH iso1  iso2  iso3  staticQH
  1             |     |     |       |---> (periodic lists)
  2             |     |     |       |
  3             |     |     |       |
  4             |     |     |       |
...             |     |     |       |
                |     |     |       |
1024            |     |     |       |
*/


static
PHCD_QUEUEHEAD_DESCRIPTOR
__inline
EHCI_GetRealDummyQueueHeadForFrame(
    __inout PDEVICE_DATA    DeviceData,
    ULONG                   Frame
    )
/*++

Routine Description:

    This routine returns the address of the Dummy QH for the specified
    frame numer in the periodic schedule.

    This routine is only called by EHCI_InterruptNextSOF().

Arguments:

Return Value:

    queue head

--*/
{
    return &DeviceData->DummyQueueHeads[Frame];
}

static
PHCD_QUEUEHEAD_DESCRIPTOR
__inline
EHCI_GetDummyQueueHeadForFrame(
    __inout PDEVICE_DATA    DeviceData,
    ULONG                   Frame
    )
/*++

Routine Description:

    This routine returns the address of the Dummy QH for the specified
    frame number in the periodic schedule.  If there is currently an HS
    Iso SOF TD linked into the periodic schedule the address of the HS
    Iso SOF TD will be returned instead of the address of the Dummy QH.

    This routine is called by EHCI_InsertIsoTdsInSchedule() and
    EHCI_InsertHsIsoTdsInSchedule() when full-speed and high-speed
    Isochronous endpoints are opened, and called by EHCI_RemoveIsoTdsFromSchedule()
    and EHCI_RemoveHsIsoTdsFromSchedule() when full-speed and high-speed
    Isochronous endpoints are closed.

Arguments:

Return Value:

    queue head

--*/
{
    PHCD_QUEUEHEAD_DESCRIPTOR   dummyQh;

    dummyQh = &DeviceData->DummyQueueHeads[Frame];

    return dummyQh;
}


#define     ISO_SCHEDULE_SIZE       32
#define     ISO_SCHEDULE_MASK       0x1f

#define HIGHSPEED(ed) ((ed)->Parameters.DeviceSpeed == HighSpeed ? TRUE : FALSE)

VOID
EHCI_RebalanceIsoEndpoint(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_PARAMETERS EndpointParameters,
    __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    compute how much common buffer we will need
    for this endpoint

Arguments:

Return Value:

--*/
{
    PHCD_SI_TRANSFER_DESCRIPTOR siTd;
    ULONG i, f;
    ULONG currentFrame;

    // Don't mess with the SiTD for the current frame.  Start with SiTD
    // for next frame.  This is an attempt to avoid a transfer glitch on
    // an Isoch endpoint which has active transfers when it is
    // rebalanced.
    //
    // The SiTD for the current frame will be fixed up by
    // EHCI_MapIsoPacketToTd().
    //
    currentFrame = EHCI_Get32BitFrameNumber(DeviceData) + 1;

    // should only have to deal with s-mask and c-mask changes

    EHCI_ASSERT(DeviceData, !HIGHSPEED(EndpointData));

    //NOTE: irql should be raised for us

    // update internal copy of parameters
    EndpointData->Parameters = *EndpointParameters;

    f = currentFrame & ISO_SCHEDULE_MASK;

    for (i = 0; i < EndpointData->TdCount - 1; i++) {

        siTd = &EndpointData->SiTdList->Td[f];

        siTd->HwTD.Control.cMask =
            EndpointParameters->SplitCompletionMask;
        siTd->HwTD.Control.sMask =
            EndpointParameters->InterruptScheduleMask;

        f++;
        f &= ISO_SCHEDULE_MASK;
    }
}


VOID
EHCI_InitializeSiTD(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    __in PENDPOINT_PARAMETERS EndpointParameters,
    __inout PHCD_SI_TRANSFER_DESCRIPTOR SiTd,
    __in PHCD_SI_TRANSFER_DESCRIPTOR PrevSiTd,
    HW_32BIT_PHYSICAL_ADDRESS PhysicalAddress
    )
/*++

Routine Description:

    Initailze a static SiTD for an endpoint

Arguments:

Return Value:

    none
--*/
{
    HC_SITD_STATE state;

    SiTd->Sig = SIG_HCD_SITD;
    SiTd->PhysicalAddress = PhysicalAddress;
    ISO_PACKET_PTR(SiTd->Packet) = NULL;

    SiTd->HwTD.Caps.ul = 0;
    SiTd->HwTD.Caps.DeviceAddress =
        EndpointParameters->DeviceAddress;
    SiTd->HwTD.Caps.EndpointNumber =
        EndpointParameters->EndpointAddress;
    SiTd->HwTD.Caps.HubAddress =
        EndpointParameters->TtDeviceAddress;
    SiTd->HwTD.Caps.PortNumber =
        EndpointParameters->TtPortNumber;
    // 1 = IN 0 = OUT
    SiTd->HwTD.Caps.Direction =
        (EndpointParameters->TransferDirection == In) ? 1 : 0;

    SiTd->HwTD.Control.ul = 0;
    SiTd->HwTD.Control.cMask =
        EndpointParameters->SplitCompletionMask;
    SiTd->HwTD.Control.sMask =
        EndpointParameters->InterruptScheduleMask;

#if 0 // not in sp2
    if(Out == EndpointParameters->TransferDirection) {
        SiTd->HwTD.BackPointer.HwAddress = 1;
    }
    else {
        // transfer direction is IN
        // HC state machine will access BackPointer
        // only when
        // a. CMask is 1 and
        // b. CMask is 2 and SMask[0] is 0
        SiTd->HwTD.BackPointer.HwAddress =
            PrevSiTd->PhysicalAddress;
    }
#endif

    // from sp2
    SiTd->HwTD.BackPointer.HwAddress =
                PrevSiTd->PhysicalAddress;

    state.ul = 0;
    SiTd->HwTD.State = state;
}


VOID
EHCI_InsertIsoTdsInSchedule(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    __inout_opt PENDPOINT_DATA PrevEndpointData,
    __inout_opt PENDPOINT_DATA NextEndpointData
    )
/*++

Routine Description:

   Insert ISO Tds into the HW list

   schedule should look like:

   DUMMYQH->ISOQH-ISOQH->INTQH (NEC)
   FRAMELIST->ISOQH-ISOQH->INTQH (Others)

Arguments:


--*/
{
    PHW_32BIT_PHYSICAL_ADDRESS frameBase = NULL;
    ULONG i;

    LOGENTRY(DeviceData, G, '_iAD', PrevEndpointData,
        NextEndpointData, EndpointData);

    frameBase = DeviceData->FrameListBaseAddress;

    for (i=0; i<USBEHCI_MAX_FRAME; i++) {

        PHCD_SI_TRANSFER_DESCRIPTOR siTd = NULL, nextSiTd = NULL;
        PHCD_QUEUEHEAD_DESCRIPTOR dQh = NULL;
        PHCD_QUEUEHEAD_DESCRIPTOR stQh = NULL;
        HW_32BIT_PHYSICAL_ADDRESS qhPhys = 0;
        ULONG phys = 0;

        siTd = &EndpointData->SiTdList->Td[i&0x1f];

        // fixup next link
        if (NextEndpointData == NULL &&
            PrevEndpointData == NULL) {

            // list empty add to head
            if (i == 0) {
                EHCI_ASSERT(DeviceData, DeviceData->IsoEndpointListHead == NULL);
                DeviceData->IsoEndpointListHead = EndpointData;
                EndpointData->PrevEndpoint = NULL;
                EndpointData->NextEndpoint = NULL;
            }
            // list empty add to head

            // no iso endpoints, link to the interrupt
            // queue heads via the dummy qh
            //
            // point at the static perodic queue head pointed to
            // by the appropriate dummy
            // DUMMY->INTQH           FRAMELIST->INTQH
            //  to               OR    to
            // ISOTD->INTQH           ISOTD->INTQH
            if(DeviceData->UseDummyQheadsEnabled) {
                dQh = EHCI_GetDummyQueueHeadForFrame(DeviceData, i);
                siTd->HwTD.NextLink.HwAddress = dQh->HwQH.HLink.HwAddress;
                HW_PTR(siTd->NextLink) = HW_PTR(dQh->NextLink);
            }
            else {
                qhPhys = *frameBase;
                stQh = EHCI_GetQueueHeadForFrame(DeviceData, i);
                EHCI_ASSERT(DeviceData, (qhPhys & ~EHCI_DTYPE_Mask) == stQh->PhysicalAddress);
                siTd->HwTD.NextLink.HwAddress = qhPhys;
                HW_PTR(siTd->NextLink) = (PUCHAR)stQh;
            }

            phys = siTd->PhysicalAddress;
            SET_SITD(phys);
            if(DeviceData->UseDummyQheadsEnabled) {
                // appropriate dummy should point to these TDs
                // DUMMY->INTQH, ISOTD->INTQH
                //  to
                // DUMMY->ISOTD->INTQH
                dQh = EHCI_GetDummyQueueHeadForFrame(DeviceData, i);
                dQh->HwQH.HLink.HwAddress = phys;
                HW_PTR(dQh->NextLink) = (PUCHAR) siTd;
            }
            else { // EHCI_Generic
                *frameBase = phys;
            }
        } else { // not empty

            if (NextEndpointData == NULL) {
            // tail of list, list not empty
            // add to tail
                if (i == 0) {
                    EHCI_ASSERT(DeviceData, PrevEndpointData != NULL);
                    EHCI_ASSERT(DeviceData, DeviceData->IsoEndpointListHead != NULL);

                    PrevEndpointData->NextEndpoint = EndpointData;
                    EndpointData->PrevEndpoint = PrevEndpointData;
                    EndpointData->NextEndpoint = NULL;
                }

                LOGENTRY(DeviceData, G, '_iTL', PrevEndpointData,
                        NextEndpointData, EndpointData);

                // tail of list, link to qh
                // ISOTD->INTQH
                //  to
                // ISOTD->newISOTD->INTQH
                //
                if (HIGHSPEED(PrevEndpointData)) {

                    PHCD_HSISO_TRANSFER_DESCRIPTOR previTd;

                    PUCHAR next;

                    previTd = &PrevEndpointData->HsIsoTdList->Td[i];
                    ASSERT_ITD(DeviceData, previTd);

                    siTd = &EndpointData->SiTdList->Td[i&0x1f];
                    ASSERT_SITD(DeviceData, siTd);

                    // fixup current next ptr
                    phys = previTd->HwTD.NextLink.HwAddress;
                    next = HW_PTR(previTd->NextLink);
                    siTd->HwTD.NextLink.HwAddress = phys;
                    HW_PTR(siTd->NextLink) = next;

                    // fixup prev next ptr
                    HW_PTR(previTd->NextLink) = (PUCHAR) siTd;
                    phys = siTd->PhysicalAddress;
                    SET_SITD(phys);
                    previTd->HwTD.NextLink.HwAddress = phys;

                } else  {

                    PHCD_SI_TRANSFER_DESCRIPTOR prevSiTd;
                    PUCHAR next;

                    prevSiTd = &PrevEndpointData->SiTdList->Td[i&0x1f];
                    ASSERT_SITD(DeviceData, prevSiTd);

                    siTd = &EndpointData->SiTdList->Td[i&0x1f];
                    ASSERT_SITD(DeviceData, siTd);

                    if (i<32) {
                        //newISOTD->INTQH
                        phys = prevSiTd->HwTD.NextLink.HwAddress;
                        next = HW_PTR(prevSiTd->NextLink);
                        siTd->HwTD.NextLink.HwAddress = phys;
                        HW_PTR(siTd->NextLink) = next;
                        LOGENTRY(DeviceData, G, '_in1', phys, next, siTd);

                        //ISOTD->newISOTD
                        phys = siTd->PhysicalAddress;
                        SET_SITD(phys);
                        next = (PUCHAR) siTd;
                        prevSiTd->HwTD.NextLink.HwAddress = phys;
                        HW_PTR(prevSiTd->NextLink) = next;

                        LOGENTRY(DeviceData, G, '_in2', phys, next, siTd);
                    }
                }

            // add to tail
            } else {
            // list not empty, not tail
            // add to middle OR head
                //
                // link to the next iso endpoint
                // ISOTD->INTQH
                //  to
                // newISOTD->ISOTD->INTQH
                if (i == 0) {
                    EHCI_ASSERT(DeviceData, NextEndpointData != NULL);
                    EndpointData->NextEndpoint = NextEndpointData;
                    NextEndpointData->PrevEndpoint = EndpointData;
                }

                // link to next
                nextSiTd = &NextEndpointData->SiTdList->Td[i&0x1f];
                phys = nextSiTd->PhysicalAddress;
                SET_SITD(phys);

                // link to the next iso endpoint
                siTd->HwTD.NextLink.HwAddress = phys;
                HW_PTR(siTd->NextLink) = (PUCHAR) nextSiTd;

                // link to prev
                if (PrevEndpointData != NULL) {
                    // middle
                    // ISOTD->ISOTD->INTQH, newISOTD->ISOTD->INTQH
                    // to
                    // ISOTD->newISOTD->ISOTD->INTQH

                    if (i == 0) {
                        PrevEndpointData->NextEndpoint = EndpointData;
                        EndpointData->PrevEndpoint = PrevEndpointData;
                    }

                    if (HIGHSPEED(PrevEndpointData)) {

                        PHCD_HSISO_TRANSFER_DESCRIPTOR previTd;

                        previTd = &PrevEndpointData->HsIsoTdList->Td[i];
                        ASSERT_ITD(DeviceData, previTd);

                        siTd = &EndpointData->SiTdList->Td[i&0x1f];
                        ASSERT_SITD(DeviceData, siTd);

                        phys = siTd->PhysicalAddress;
                        SET_SITD(phys);
                        previTd->HwTD.NextLink.HwAddress = phys;
                        HW_PTR(previTd->NextLink) = (PUCHAR) siTd;
                    } else  {

                        PHCD_SI_TRANSFER_DESCRIPTOR prevSiTd;

                        prevSiTd = &PrevEndpointData->SiTdList->Td[i&0x1f];
                        ASSERT_SITD(DeviceData, prevSiTd);

                        siTd = &EndpointData->SiTdList->Td[i&0x1f];
                        ASSERT_SITD(DeviceData, siTd);

                        phys = siTd->PhysicalAddress;
                        SET_SITD(phys);
                        prevSiTd->HwTD.NextLink.HwAddress = phys;
                        HW_PTR(prevSiTd->NextLink) = (PUCHAR)siTd;
                    }
                } else {
                    // head of list, list not empty
                    if (i == 0) {
                        EHCI_ASSERT(DeviceData, NextEndpointData != NULL);
                        EHCI_ASSERT(DeviceData, NextEndpointData ==
                                        DeviceData->IsoEndpointListHead);

                        DeviceData->IsoEndpointListHead = EndpointData;
                        EndpointData->PrevEndpoint = NULL;
                    }

                    phys = siTd->PhysicalAddress;
                    SET_SITD(phys);
                    // head of list, link to Dummy QH
                    //
                    // appropriate dummy should point to these TDs
                    // DUMMY->ISOTD->INTQH, newISOTD->ISOTD->INTQH
                    //  to
                    // DUMMY->newISOTD->ISOTD->INTQH
                    if(DeviceData->UseDummyQheadsEnabled) {
                        dQh = EHCI_GetDummyQueueHeadForFrame(DeviceData, i);
                        dQh->HwQH.HLink.HwAddress = phys;
                        HW_PTR(dQh->NextLink) = (PUCHAR) siTd;
                    }
                    else {
                        *frameBase = phys;
                    }
                }

            }
        } // not empty
        frameBase++;
    } // for
}


VOID
EHCI_RemoveIsoTdsFromSchedule(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

   unlink the iso TDs from the schedule

Arguments:


--*/
{
    ULONG i;
    PENDPOINT_DATA prevEndpoint, nextEndpoint;
    PHCD_QUEUEHEAD_DESCRIPTOR dQh;

    prevEndpoint = EndpointData->PrevEndpoint;
    nextEndpoint = EndpointData->NextEndpoint;

    LOGENTRY(DeviceData, G, '_iRM', prevEndpoint,
        nextEndpoint, EndpointData);

    if (DeviceData->IsoEndpointListHead == EndpointData) {
        // this is the head
        PHW_32BIT_PHYSICAL_ADDRESS frameBase;
        frameBase = DeviceData->FrameListBaseAddress;

        for (i=0; i<USBEHCI_MAX_FRAME; i++) {

            PHCD_SI_TRANSFER_DESCRIPTOR siTd;
            ULONG phys;

            siTd = &EndpointData->SiTdList->Td[i&0x1f];
            phys = siTd->HwTD.NextLink.HwAddress;

            if(DeviceData->UseDummyQheadsEnabled) {
                dQh = EHCI_GetDummyQueueHeadForFrame(DeviceData, i);
                dQh->HwQH.HLink.HwAddress = phys;
                HW_PTR(dQh->NextLink) = HW_PTR(siTd->NextLink);
            }
            else {
                *frameBase = phys;
            }
            frameBase++;
        }

        DeviceData->IsoEndpointListHead =
            EndpointData->NextEndpoint;
        if (nextEndpoint != NULL) {
            EHCI_ASSERT(DeviceData,
                        nextEndpoint->PrevEndpoint == EndpointData);
            nextEndpoint->PrevEndpoint = NULL;
        }
    } else {
        // middle or tail
        EHCI_ASSERT(DeviceData, prevEndpoint != NULL);

        if (HIGHSPEED(prevEndpoint)) {

            for (i=0; i<USBEHCI_MAX_FRAME; i++) {
                PHCD_HSISO_TRANSFER_DESCRIPTOR previTd;
                PHCD_SI_TRANSFER_DESCRIPTOR siTd;
                ULONG phys;

                siTd = &EndpointData->SiTdList->Td[i&0x1f];
                previTd = &prevEndpoint->HsIsoTdList->Td[i];

                phys = siTd->HwTD.NextLink.HwAddress;
                previTd->HwTD.NextLink.HwAddress = phys;

                HW_PTR(previTd->NextLink) = HW_PTR(siTd->NextLink);
            }
            prevEndpoint->NextEndpoint =
                    EndpointData->NextEndpoint;
            if (nextEndpoint) {
                nextEndpoint->PrevEndpoint = prevEndpoint;
            }
        } else {

            for (i=0; i<ISO_SCHEDULE_SIZE; i++) {

                PHCD_SI_TRANSFER_DESCRIPTOR siTd, prevSiTd;
                ULONG phys;

                siTd = &EndpointData->SiTdList->Td[i];
                prevSiTd = &prevEndpoint->SiTdList->Td[i];

                phys = siTd->HwTD.NextLink.HwAddress;
                prevSiTd->HwTD.NextLink.HwAddress = phys;
                HW_PTR(prevSiTd->NextLink) = HW_PTR(siTd->NextLink);
            }
            prevEndpoint->NextEndpoint =
                    EndpointData->NextEndpoint;
            if (nextEndpoint) {
                nextEndpoint->PrevEndpoint = prevEndpoint;
            }
        }
    }
}


USB_MINIPORT_STATUS
EHCI_OpenIsochronousEndpoint(
    __inout PDEVICE_DATA DeviceData,
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
    HW_32BIT_PHYSICAL_ADDRESS phys;
    ULONG i;
    PENDPOINT_DATA prevEndpoint, nextEndpoint;

    LOGENTRY(DeviceData, G, '_opR', 0, 0, EndpointParameters);

    buffer = EndpointParameters->CommonBufferVa;
    phys = EndpointParameters->CommonBufferPhys;


    EndpointData->SiTdList = (PHCD_SITD_LIST) buffer;
    // bugbug use manifest
    EndpointData->TdCount = ISO_SCHEDULE_SIZE;
    EndpointData->LastFrame = 0;

    for (i=0; i<EndpointData->TdCount; i++) {

        EHCI_InitializeSiTD(DeviceData,
                            EndpointData,
                            EndpointParameters,
                            &EndpointData->SiTdList->Td[i],
                            i > 0 ?
                                &EndpointData->SiTdList->Td[i-1] :
                                &EndpointData->SiTdList->Td[EndpointData->TdCount-1],
                            phys);

        EHCI_AssertHwAlignment(DeviceData, phys,
                    sizeof(HW_SPLIT_ISOCHRONOUS_TD));
        phys += sizeof(HCD_SI_TRANSFER_DESCRIPTOR);

    }
    EndpointData->SiTdList->Td[0].HwTD.BackPointer.HwAddress =
        EndpointData->SiTdList->Td[EndpointData->TdCount-1].PhysicalAddress;

    // split iso eps are inserted after any high speed eps

    if (DeviceData->IsoEndpointListHead == NULL) {
        // empty list
        prevEndpoint = NULL;
        nextEndpoint = NULL;
    } else {

        prevEndpoint = NULL;
        nextEndpoint = DeviceData->IsoEndpointListHead;
        // walk the list to the first non HS ep or to
        // a NULL

        while (nextEndpoint != NULL &&
               HIGHSPEED(nextEndpoint)) {
            prevEndpoint = nextEndpoint;
            nextEndpoint = prevEndpoint->NextEndpoint;

        }
        //
        // nextEndpoint is first non high speed endpoint
        //

        while (nextEndpoint != NULL &&
               ((EndpointData->Parameters.Ordinal > nextEndpoint->Parameters.Ordinal) ||
                ((EndpointData->Parameters.Ordinal == nextEndpoint->Parameters.Ordinal) &&
                 (EndpointData->Parameters.InterruptScheduleMask >
                  nextEndpoint->Parameters.InterruptScheduleMask)))) {
            //
            // In order to enforce start/complete split rules,
            // we must add endpoints in the same order of their start-split masks.
            // Here are the ordinal values:
            //
            // 0: FATISO
            // 1: High-speed ISO or Full speed ISO s-split frame 1
            // 2:8 Full-speed ISO s-split frames 2-8
            //
            prevEndpoint = nextEndpoint;
            nextEndpoint = prevEndpoint->NextEndpoint;
        }
    }

    // insert this column of TDs thru the schedule
    EHCI_InsertIsoTdsInSchedule(DeviceData,
                                EndpointData,
                                prevEndpoint,
                                nextEndpoint);

    // init endpoint structures
    InitializeListHead(&EndpointData->IsoTransferListHead);

    EHCI_EnablePeriodicList(DeviceData);

    return USBMP_STATUS_SUCCESS;
}


VOID
EHCI_MapIsoPacketToTd(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    __in PMINIPORT_ISO_PACKET Packet,
    __inout PHCD_SI_TRANSFER_DESCRIPTOR SiTd
    )
/*++

Routine Description:



Arguments:

Returns:

--*/
{
    HC_SITD_STATE state;
    ULONG length;

    LOGENTRY(DeviceData, G, '_mpI', SiTd, 0, Packet);

    state.ul = 0;
    SiTd->HwTD.State = state;

    SiTd->HwTD.BufferPointer0.ul =
        Packet->BufferPointer0.Hw64.u.LowPart;

    SiTd->HwTD.BufferPointer64_0 =
        Packet->BufferPointer0.Hw64.u.HighPart;

    length = Packet->BufferPointer0Length;
    SiTd->StartOffset = SiTd->HwTD.BufferPointer0.CurrentOffset;

    SiTd->HwTD.BufferPointer1.ul = 0;
    SiTd->HwTD.BufferPointer64_1 = 0;

    if (Packet->BufferPointerCount > 1) {
        EHCI_ASSERT(DeviceData,
                    (Packet->BufferPointer1.Hw64.u.LowPart & 0xFFF) == 0);

        SiTd->HwTD.BufferPointer1.ul =
            Packet->BufferPointer1.Hw64.u.LowPart;

        SiTd->HwTD.BufferPointer64_1 =
            Packet->BufferPointer1.Hw64.u.HighPart;

        length += Packet->BufferPointer1Length;
    }

    // not sure if this is correct for IN
    SiTd->HwTD.BufferPointer1.Tposition = TPOS_ALL;

    if (EndpointData->Parameters.TransferDirection == Out) {

        if (length == 0) {
           SiTd->HwTD.BufferPointer1.Tcount = 1;
        } else {
           SiTd->HwTD.BufferPointer1.Tcount = ((length -1) / 188) +1;
        }

        if (SiTd->HwTD.BufferPointer1.Tcount == 1) {
            SiTd->HwTD.BufferPointer1.Tposition = TPOS_ALL;
        } else {
            SiTd->HwTD.BufferPointer1.Tposition = TPOS_BEGIN;
        }

        EHCI_ASSERT(DeviceData, SiTd->HwTD.BufferPointer1.Tcount <= 6);

    } else {
        SiTd->HwTD.BufferPointer1.Tcount = 0;
    }

    // Fix up cMask & sMask just in case this SiTD was skipped by
    // EHCI_RebalanceIsoEndpoint().
    //
    SiTd->HwTD.Control.cMask = EndpointData->Parameters.SplitCompletionMask;
    SiTd->HwTD.Control.sMask = EndpointData->Parameters.InterruptScheduleMask;

    state.BytesToTransfer = length;
    state.Active = 1;
    state.InterruptOnComplete = 1;
    SiTd->HwTD.State = state;

    EHCI_ASSERT(DeviceData, SiTd->HwTD.BackPointer.HwAddress != 0);
}


VOID
EHCI_CompleteIsoPacket(
    __inout PDEVICE_DATA DeviceData,
    __inout PMINIPORT_ISO_PACKET Packet,
    __inout PHCD_SI_TRANSFER_DESCRIPTOR SiTd
    )
/*++

Routine Description:



Arguments:

Returns:

--*/
{
    HC_SITD_STATE state;
    ULONG length;
    ULONG cf = EHCI_Get32BitFrameNumber(DeviceData);

    LOGENTRY(DeviceData, G, '_cpI', Packet, SiTd, cf);

    state = SiTd->HwTD.State;

    if (state.Active == 1) {
        // missed

        // Don't leave an Active SiTD in the schedule if it is being
        // completed.
        //
        state.Active = 0;
        SiTd->HwTD.State = state;

        Packet->LengthTransferred = 0;
        LOGENTRY(DeviceData, G, '_cms',
            Packet,
            0,
            Packet->FrameNumber);
        Packet->UsbdStatus = USBD_STATUS_ISO_NOT_ACCESSED_BY_HW;

    } else {

        //length = SiTd->HwTD.BufferPointer0.CurrentOffset -
        //    SiTd->StartOffset;
        //LOGENTRY(DeviceData, G, '_cp2',
        //    Packet->FrameNumber,
        //    SiTd->HwTD.BufferPointer0.CurrentOffset,
        //    SiTd->StartOffset);

        length = EHCI_TdBytesTransferred(Packet->Length, state.BytesToTransfer);

        LOGENTRY(DeviceData, G, '_cp3',
                 Packet->FrameNumber,
                 Packet->Length ,
                 state.BytesToTransfer);

        Packet->LengthTransferred = length;
        LOGENTRY(DeviceData, G, '_cpL', Packet, SiTd, length);


        // map status
        LOGENTRY(DeviceData, G, '_cpS', Packet, state.ul,
                 Packet->UsbdStatus);

        Packet->UsbdStatus = USBD_STATUS_SUCCESS;
        // warning:
        // This breaks camera+mouse on NEC(busted version) hub. The NEC hub is busted but by masking the missed
        // microframe errors the camera will appear to work. The fact that we did not report errors on XP was
        // really a bug in usbehci.sys
        // If we do not report errors then the if the device is disconnected the iso tarbsfer swill continue to
        // complete with success and no data.  As a result some cameras will hang pnp on surprise remove.

        // map status
        if (state.XactErr) {
            Packet->UsbdStatus = USBD_STATUS_XACT_ERROR;
        } else if (state.BabbleDetected) {
            Packet->UsbdStatus = USBD_STATUS_BABBLE_DETECTED;
        } else if (state.DataBufferError) {
            Packet->UsbdStatus = USBD_STATUS_DATA_BUFFER_ERROR;
        } else if (state.ERR) {
            Packet->UsbdStatus = USBD_STATUS_ISO_TD_ERROR;
        } else if (state.MissedMicroframe) {
            Packet->UsbdStatus = USBD_STATUS_ISO_TD_ERROR;
        }

        LOGENTRY(DeviceData, G, '_cpS', Packet, state.ul, Packet->UsbdStatus);

        // reset error fields
        state.BytesToTransfer = 0;
        state.XactErr = 0;
        state.BabbleDetected = 0;
        state.DataBufferError = 0;
        state.ERR = 0;
        state.MissedMicroframe = 0;
        SiTd->HwTD.State = state;
    }
}


PMINIPORT_ISO_PACKET
EHCI_GetPacketForFrame(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    _When_(return != NULL, _Out_) PTRANSFER_CONTEXT *Transfer,
    ULONG Frame
    )
/*++

Routine Description:

    fetch the iso packet associated with the given frame
    if we have one in our current transfer list

Arguments:

Returns:

--*/
{
    ULONG i;
    PLIST_ENTRY listEntry;

    listEntry = EndpointData->IsoTransferListHead.Flink;
    while (listEntry != &EndpointData->IsoTransferListHead) {

        PTRANSFER_CONTEXT transfer;

        transfer = (PTRANSFER_CONTEXT) CONTAINING_RECORD(
                     listEntry,
                     struct _TRANSFER_CONTEXT,
                     TransferLink);

        ASSERT_TRANSFER(DeviceData, transfer);

        if (GREATER_THAN_EQUAL_TO(transfer->FrameComplete, Frame)) {
            for(i=0; i<transfer->IsoTransfer->PacketCount; i++) {
                if (transfer->IsoTransfer->Packets[i].FrameNumber == Frame) {
                    *Transfer = transfer;
                    return &transfer->IsoTransfer->Packets[i];
                }
            }
        }

        listEntry = transfer->TransferLink.Flink;
    }

    *Transfer = NULL;
    return NULL;
}

ULONG IsoCount = 0;
ULONG pCount = 0;

VOID
EHCI_InternalPollIsoEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    BOOLEAN Complete
    )
/*++

Routine Description:

    Called when the endpoint 'needs attention'


    static iso TD table
    --------------------
    0                       < -- (lastFrame & 0x1f)
    1                       {completed}
    2                       {completed}
    3                       {limbo}
    4                       < -- (currentframe & 0x1f)
    ...

    31
    ---------------------


Arguments:

Return Value:

--*/

{
    ULONG x, i;
    ULONG currentFrame, lastFrame;
    PHCD_SI_TRANSFER_DESCRIPTOR siTd;
    PMINIPORT_ISO_PACKET packet;
    PLIST_ENTRY listEntry;
    ULONG transfersPending, fc = 0;

    currentFrame = EHCI_Get32BitFrameNumber(DeviceData);
    lastFrame = EndpointData->LastFrame;

    LOGENTRY(DeviceData, G, '_pis', lastFrame, currentFrame,
        EndpointData);

    if (GREATER_THAN(currentFrame, lastFrame + ISO_SCHEDULE_SIZE)) {
        PTRANSFER_CONTEXT transfer;
        // overrun
        lastFrame = currentFrame-1;
        LOGENTRY(DeviceData, G, '_ove', lastFrame, currentFrame, 0);

        // dump the current contents
        for (i = 0; i <ISO_SCHEDULE_SIZE; i++) {

            siTd = &EndpointData->SiTdList->Td[i];

            transfer = ISO_TRANSFER_PTR(siTd->Transfer);

            if (transfer != NULL) {
                ISO_PACKET_PTR(siTd->Packet) = NULL;
                ISO_TRANSFER_PTR(siTd->Transfer) = NULL;
                transfer->PendingPackets--;
            }
        }
    }

    if (lastFrame == currentFrame) {
        // too early to do anything
        LOGENTRY(DeviceData, G, '_ear', lastFrame, currentFrame, 0);
        return;
    }

    // TDs between lastframe and currentframe are complete,
    // complete the packets associated with them


//    f0
//    f1
//    f2  < ------- last frame   }
//    f3                         }  these are complete
//    f4                         <- backpointer may still be pointing here
//    f5  < ------- current frame
//    f6
//    f7
//    f8

    x = (lastFrame & (ISO_SCHEDULE_MASK));

    LOGENTRY(DeviceData, G, '_frm', lastFrame, x, currentFrame);
    while (x != ((currentFrame-1) & ISO_SCHEDULE_MASK)) {
        PTRANSFER_CONTEXT transfer;
        siTd = &EndpointData->SiTdList->Td[x];

        ASSERT_SITD(DeviceData, siTd);
        // complete this packet
        packet = ISO_PACKET_PTR(siTd->Packet);
        transfer = ISO_TRANSFER_PTR(siTd->Transfer);
        LOGENTRY(DeviceData, G, '_gpk', transfer, packet, x);

        if (packet != NULL) {
            transfer = ISO_TRANSFER_PTR(siTd->Transfer);
            ASSERT_TRANSFER(DeviceData, transfer);
            EHCI_CompleteIsoPacket(DeviceData, packet, siTd);
            ISO_PACKET_PTR(siTd->Packet) = NULL;
            ISO_TRANSFER_PTR(siTd->Transfer) = NULL;
            transfer->PendingPackets--;
        }

        lastFrame++;
        x++;
        x &= ISO_SCHEDULE_MASK;
    }

    // attempt to program what we can, if siTD is NULL
    // then we can program this frame
    // NOTE: No scheduling if paused!
    if (EndpointData->CurEpState != ENDPOINT_PAUSE) {
        PTRANSFER_CONTEXT transfer;
        LOGENTRY(DeviceData, G, '_psh', 0, 0, 0);

        // Start at currentFrame+1 instead of at currentFrame, that is
        // initialize i=1, not i=0.  Don't try to activate a siTD for
        // the current frame.  It might be too late to complete it in
        // the current frame and it might get left in the schedule with
        // the Active bit set.  Then it can be completed by the HC in
        // ISO_SCHEDULE_SIZE frames from the current frame after the
        // transfer has already been completed and freed by the '_gpk'
        // code path above.
        //
        for (i=1; i <ISO_SCHEDULE_SIZE; i++) {

            ULONG cf;

            x = ((currentFrame+i) & ISO_SCHEDULE_MASK);

            siTd = &EndpointData->SiTdList->Td[x];
            ASSERT_SITD(DeviceData, siTd);

            LOGENTRY(DeviceData, G, '_gpf', siTd, x, currentFrame+i);

            // open slot?
            if (ISO_PACKET_PTR(siTd->Packet) != NULL) {
                // no, bail
                continue;
            }

            // yes, see if we have a packet
            packet = EHCI_GetPacketForFrame(DeviceData,
                                            EndpointData,
                                            &transfer,
                                            currentFrame+i);

            if (packet != NULL) {
                EHCI_ASSERT(DeviceData, ISO_PACKET_PTR(siTd->Packet) == NULL);

                cf = EHCI_Get32BitFrameNumber(DeviceData);

                // Additional check to make sure a siTD is not
                // activated for the current frame.  Check with an
                // updated current frame now as it might have advanced
                // while this routine has been executing.
                //
                if (GREATER_THAN(currentFrame + i, cf)) {
                    EHCI_MapIsoPacketToTd(DeviceData, EndpointData,
                                          packet, siTd);
                    ISO_PACKET_PTR(siTd->Packet) = packet;
                    ASSERT_TRANSFER(DeviceData, transfer);
                    ISO_TRANSFER_PTR(siTd->Transfer) = transfer;
                    transfer->PendingPackets++;
                }
            }
        }
    }

    EHCI_ASSERT(DeviceData, lastFrame < currentFrame);
    EndpointData->LastFrame = lastFrame;

    // walk our list of active iso transfers and see
    // if any are complete

    listEntry = EndpointData->IsoTransferListHead.Flink;
    transfersPending = 0;

    while (listEntry != &EndpointData->IsoTransferListHead && Complete) {
        PTRANSFER_CONTEXT transfer;

        transfer = (PTRANSFER_CONTEXT) CONTAINING_RECORD(
                     listEntry,
                     struct _TRANSFER_CONTEXT,
                     TransferLink);

        LOGENTRY(DeviceData, G, '_ckt', transfer, transfer->FrameComplete+2
            , currentFrame);

        EHCI_ASSERT(DeviceData, transfer->Sig == SIG_EHCI_TRANSFER);
        if (GREATER_THAN_EQUAL_TO(currentFrame, transfer->FrameComplete + 2) &&
            transfer->PendingPackets == 0) {
// debug check for overrrun
//            if (currentFrame - transfer->FrameComplete > 5) {
//                TEST_TRAP(DeviceData);
//            }

            listEntry = transfer->TransferLink.Flink;

            LOGENTRY(DeviceData, G, '_cpi', transfer,
                    currentFrame, transfer->FrameComplete);

            //RemoveEntryList(&transfer->TransferLink);
            EHCI_DecPendingTransfer(DeviceData, EndpointData, transfer);

            USBPORT_COMPLETE_ISO_TRANSFER(DeviceData,
                                          EndpointData,
                                          transfer->TransferParameters,
                                          transfer->IsoTransfer);
        } else {
            transfersPending++;
            fc = transfer->FrameComplete;
            listEntry = transfer->TransferLink.Flink;
        }
    }

    if (transfersPending == 1) {

        currentFrame = EHCI_Get32BitFrameNumber(DeviceData);

        if (ABS((LONG)(fc - currentFrame)) < 2) {
            LOGENTRY(DeviceData, G, '_rei', fc, currentFrame, 0);
            EHCI_InterruptNextSOF(DeviceData);
        }
    }
}


VOID
EHCI_PollIsoEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    )
{
    ULONG cf;

    cf = EHCI_Get32BitFrameNumber(DeviceData);

    LOGENTRY(DeviceData, G, '_ipl', 0, 0, cf);

    if (!IsListEmpty(&EndpointData->IsoTransferListHead)) {
        LOGENTRY(DeviceData, G, '_III', 0, 0, cf);

        if (HIGHSPEED(EndpointData)) {
            EHCI_InternalPollHsIsoEndpoint(DeviceData, EndpointData, TRUE, FALSE);
        } else {
            EHCI_InternalPollIsoEndpoint(DeviceData, EndpointData, TRUE);
        }
    }
}


VOID
EHCI_PollResetIsoEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    )
/*
    Call USBPORT to complete any transfers that were queued on this
    Isochronous endpoint.  The endpoint has been removed from the host
    controller periodic schedule due to the host controller being reset.
*/
{
    PLIST_ENTRY listEntry;
    PTRANSFER_CONTEXT transfer;
    PMINIPORT_ISO_PACKET packet;
    ULONG p;

    listEntry = EndpointData->IsoTransferListHead.Flink;

    while (listEntry != &EndpointData->IsoTransferListHead) {

        transfer = (PTRANSFER_CONTEXT) CONTAINING_RECORD(
            listEntry,
            struct _TRANSFER_CONTEXT,
            TransferLink);

        packet = &transfer->IsoTransfer->Packets[0];

        for (p = 0; p < transfer->IsoTransfer->PacketCount; p++) {

            // Might want to try to account for any data that could
            // have possibly been transferred into the host data buffer
            // on an Isochronous IN transfer.  For now just return
            // zero.  DMA verifier could possibly complain about this
            // if any data was actually transferred into the host data
            // buffer.
            //
            packet->LengthTransferred = 0;
            packet->UsbdStatus = USBD_STATUS_XACT_ERROR;

            packet++;
        }

        listEntry = transfer->TransferLink.Flink;

        // This removes the transfer from the
        // EndpointData->IsoTransferListHead
        //
        EHCI_DecPendingTransfer(DeviceData, EndpointData, transfer);

        USBPORT_COMPLETE_ISO_TRANSFER(DeviceData,
                                      EndpointData,
                                      transfer->TransferParameters,
                                      transfer->IsoTransfer);
    }
}


USB_MINIPORT_STATUS
EHCI_AbortIsoTransfer(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    __inout PTRANSFER_CONTEXT TransferContext
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    ULONG i;

    // iso TD table should be idle at this point all we
    // need to do is make sure we have no TDs still pointing
    // at this transfer and remove it frome any internal
    // queues

    if (HIGHSPEED(EndpointData)) {

        for (i = 0; i <USBEHCI_MAX_FRAME; i++) {

            PHCD_HSISO_TRANSFER_DESCRIPTOR hsIsoTd;
            ULONG period = EndpointData->Parameters.Period;

            LOGENTRY(DeviceData, G, '_ibh', TransferContext,
                    EndpointData, period);

            hsIsoTd = &EndpointData->HsIsoTdList->Td[i];

            if (ISO_TRANSFER_PTR(hsIsoTd->Transfer) == TransferContext) {
                ISO_TRANSFER_PTR(hsIsoTd->Transfer) = NULL;

                ISO_PACKET_PTR(hsIsoTd->FirstPacket) = NULL;
                TransferContext->PendingPackets -= (8/period);
            }
        }
    } else {

        for (i = 0; i <ISO_SCHEDULE_SIZE; i++) {
            PHCD_SI_TRANSFER_DESCRIPTOR siTd;

            LOGENTRY(DeviceData, G, '_ibi', TransferContext,
                    EndpointData, 1);

            siTd = &EndpointData->SiTdList->Td[i];

            if (ISO_TRANSFER_PTR(siTd->Transfer) == TransferContext) {
                ISO_TRANSFER_PTR(siTd->Transfer) = NULL;
                ISO_PACKET_PTR(siTd->Packet) = NULL;
                TransferContext->PendingPackets--;
            }
        }
    }

    EHCI_DecPendingTransfer(DeviceData, EndpointData, TransferContext);

    return USBMP_STATUS_SUCCESS;

}


USB_MINIPORT_STATUS
EHCI_SubmitIsoTransfer(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PTRANSFER_PARAMETERS TransferParameters,
     __inout PTRANSFER_CONTEXT TransferContext,
     __inout PMINIPORT_ISO_TRANSFER IsoTransfer
    )
/*++

Routine Description:

    This MINIPORT_SubmitIsoTransfer() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.

Arguments:

Return Value:

--*/
{
    ULONG period, startFrame;
    BOOLEAN alwaysSchedule = FALSE;

    if (TEST_FLAG(EndpointData->Flags, EHCI_EDFLAG_HC_RESET)) {
        return USBMP_STATUS_BUSY;
    }

    period = EndpointData->Parameters.Period;

    // init the structures and queue the endpoint
    LOGENTRY(DeviceData, G, '_ISO', TransferContext, IsoTransfer, period);

    RtlZeroMemory(TransferContext, sizeof(TRANSFER_CONTEXT));
    TransferContext->Sig = SIG_EHCI_TRANSFER;
    TransferContext->IsoTransfer = IsoTransfer;
    TransferContext->EndpointData = EndpointData;
    TransferContext->TransferParameters = TransferParameters;
    TransferContext->PostedFrame = EHCI_Get32BitFrameNumber(DeviceData);

#if DBG
    IsoCount++;
    LOGENTRY(DeviceData, G, '_Ict', 0, IsoCount, 0);
#endif

    if (HIGHSPEED(EndpointData)) {
        ULONG cf = EHCI_Get32BitFrameNumber(DeviceData);

        LOGENTRY(DeviceData, G, '_sHS', 0, cf,
                 IsoTransfer->Packets[0].FrameNumber);

        TransferContext->FrameComplete =
            IsoTransfer->Packets[0].FrameNumber +
                (IsoTransfer->PacketCount/(8/period));

        startFrame = IsoTransfer->Packets[0].FrameNumber;

        if (GREATER_THAN(startFrame, cf + 8)) {
            //
            // If this transfer is scheduled to start more than 8ms in the future,
            // we must tell PollHsIsoEndpoint to always schedule this transfer.
            // We should watch this carefully for perf degredation
            //
            alwaysSchedule = TRUE;
        }

    } else {
        TransferContext->FrameComplete =
            IsoTransfer->Packets[0].FrameNumber + IsoTransfer->PacketCount;
    }
    TransferContext->PendingPackets = 0;

    // if queues are empty the go ahead and reset the table
    // so we can fill now
    if (IsListEmpty(&EndpointData->IsoTransferListHead)) {
        EndpointData->LastFrame = 0;
        LOGENTRY(DeviceData, G, '_rsi', 0, 0, 0);
    }

    EHCI_IncPendingTransfer(DeviceData, EndpointData, TransferContext);

    // scehdule the initial part of the transfer if
    // possible
    if (HIGHSPEED(EndpointData)) {
        EHCI_InternalPollHsIsoEndpoint(DeviceData,
                                       EndpointData,
                                       FALSE,
                                       alwaysSchedule);
    } else {
        EHCI_InternalPollIsoEndpoint(DeviceData,
                                     EndpointData,
                                     FALSE);
    }

    return USBMP_STATUS_SUCCESS;
}


VOID
EHCI_SetIsoEndpointState(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     MP_ENDPOINT_STATE State
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    ENDPOINT_TRANSFER_TYPE epType;
    ULONG i, j;

    epType = EndpointData->Parameters.TransferType;
    EHCI_ASSERT(DeviceData, epType == Isochronous);

    switch(State) {
    case ENDPOINT_ACTIVE:
        EndpointData->LastFrame =  EHCI_Get32BitFrameNumber(DeviceData);
        break;

    case ENDPOINT_PAUSE:
        // clear the active bit on all TDs
        if (HIGHSPEED(EndpointData)) {
            HC_ITD_TRANSACTION transaction;
            for (i=0; i<EndpointData->TdCount; i++) {
                for(j=0; j<8; j++) {
                    transaction = EndpointData->HsIsoTdList->Td[i].HwTD.Transaction[j];
                    transaction.Active = 0;
                    EndpointData->HsIsoTdList->Td[i].HwTD.Transaction[j] = transaction;
                }
            }
        } else {
            HC_SITD_STATE state;
            for (i=0; i<EndpointData->TdCount; i++) {
                state = EndpointData->SiTdList->Td[i].HwTD.State;
                state.Active = 0;
                EndpointData->SiTdList->Td[i].HwTD.State = state;
            }
        }
        break;

    case ENDPOINT_REMOVE:
        if (HIGHSPEED(EndpointData)) {
            EHCI_RemoveHsIsoTdsFromSchedule(DeviceData,
                                          EndpointData);
        } else {
            EHCI_RemoveIsoTdsFromSchedule(DeviceData,
                                          EndpointData);
        }
        break;

    default:
        // invalid state, memory corrupt of parameters invalid
        // this will likely lead to some kind of crash
        EHCI_ASSERTMSG(DeviceData, "Invalid Endpoint State", FALSE);
    }

    //EndpointData->State = State;
}

/*
    High Speed Iso code


    We use a variation of the split Iso code here. We allocate 1024
    static TDs and insert them in the schedule.  These TDs are then
    updated with the current transfers instead of inserted or removed.


*/

VOID
EHCI_Initialize_iTD(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    __in PENDPOINT_PARAMETERS EndpointParameters,
    __inout PHCD_HSISO_TRANSFER_DESCRIPTOR IsoTd,
    HW_32BIT_PHYSICAL_ADDRESS PhysicalAddress,
    ULONG Frame
    )
/*++

Routine Description:

    Initailze a static SiTD for an endpoint

Arguments:

Return Value:

    none
--*/
{
    ULONG i;
    HC_ITD_TRANSACTION transaction;

    IsoTd->Sig = SIG_HCD_ITD;
    IsoTd->PhysicalAddress = PhysicalAddress;
    ISO_PACKET_PTR(IsoTd->FirstPacket) = NULL;
    IsoTd->HostFrame = Frame;

    for (i=0; i< 8; i++) {
        transaction.ul = 0;
        IsoTd->HwTD.Transaction[i] = transaction;
    }

    IsoTd->HwTD.BufferPointer0.DeviceAddress =
        EndpointParameters->DeviceAddress;
    IsoTd->HwTD.BufferPointer0.EndpointNumber =
        EndpointParameters->EndpointAddress;

    IsoTd->HwTD.BufferPointer1.MaxPacketSize =
        EndpointParameters->MuxPacketSize;
    // 1 = IN 0 = OUT
    IsoTd->HwTD.BufferPointer1.Direction =
        (EndpointParameters->TransferDirection == In) ? 1 : 0;

    IsoTd->HwTD.BufferPointer2.Multi =
        EndpointParameters->TransactionsPerMicroframe;
}

#define EHCI_OFFSET_MASK    0x00000FFF
#define EHCI_PAGE_SHIFT     12

VOID
EHCI_MapHsIsoPacketsToTd(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    __in PMINIPORT_ISO_PACKET FirstPacket,
    __inout PHCD_HSISO_TRANSFER_DESCRIPTOR IsoTd,
    BOOLEAN InterruptOnComplete
    )
/*++

Routine Description:



Arguments:

Returns:

--*/
{
    PHC_ITD_BUFFER_POINTER  currentBp;
    PULONG                  currentBp64;
    PMINIPORT_ISO_PACKET pkt = FirstPacket;
    ULONG page, offset, bpCount, i;
    ULONG frame = FirstPacket->FrameNumber;
    ULONG period;
    HC_ITD_TRANSACTION transaction;

    UNREFERENCED_PARAMETER(frame);

    period = EndpointData->Parameters.Period;
    LOGENTRY(DeviceData, G, '_HHS', IsoTd, period, FirstPacket);
    ASSERT_ITD(DeviceData, IsoTd);

    bpCount = 0;
    currentBp = (PHC_ITD_BUFFER_POINTER) &IsoTd->HwTD.BufferPointer0;
    currentBp64 = &IsoTd->HwTD.BufferPointer64[0];

    // map the first packet
    page = (pkt->BufferPointer0.Hw64.u.LowPart >> EHCI_PAGE_SHIFT);
    currentBp->BufferPointer = page;
    *currentBp64 = pkt->BufferPointer0.Hw64.u.HighPart;

    // This Td will represent up to 8 packets

    for (i=0; i<8; i++) {

        if (i % period == 0) {

            EHCI_ASSERT(DeviceData, pkt->FrameNumber == frame);

            page = (pkt->BufferPointer0.Hw64.u.LowPart >> EHCI_PAGE_SHIFT);
            offset = pkt->BufferPointer0.Hw64.u.LowPart & EHCI_OFFSET_MASK;

            // Advance to the next TD buffer pointer if the current
            // isoch transfer packet page is not equal to the current TD
            // buffer pointer page.
            //
            if (page != currentBp->BufferPointer ||
                pkt->BufferPointer0.Hw64.u.HighPart != (LONG)*currentBp64) {
                currentBp++;
                currentBp64++;
                bpCount++;
                currentBp->BufferPointer = page;
                *currentBp64 = pkt->BufferPointer0.Hw64.u.HighPart;
            }















            transaction = IsoTd->HwTD.Transaction[i];
            transaction.Offset = offset;
            transaction.Length = pkt->Length;
            transaction.PageSelect = bpCount;
            if (InterruptOnComplete && i==(8-period)) {
                transaction.InterruptOnComplete = 1;
            } else {
                transaction.InterruptOnComplete = 0;
            }
            transaction.Active = 1;
            IsoTd->HwTD.Transaction[i] = transaction;

            if (pkt->BufferPointerCount > 1) {
                page = (pkt->BufferPointer1.Hw64.u.LowPart >> EHCI_PAGE_SHIFT);
                currentBp++;
                currentBp64++;
                bpCount++;
                currentBp->BufferPointer = page;
                *currentBp64 = pkt->BufferPointer1.Hw64.u.HighPart;
                EHCI_ASSERT(DeviceData, bpCount <= 6)
            }

            pkt++;

        } else {
            transaction = IsoTd->HwTD.Transaction[i];
            transaction.Active = 0;
            IsoTd->HwTD.Transaction[i] = transaction;
        }
    }

    LOGENTRY(DeviceData, G, '_mhs', IsoTd, 0, bpCount);
}


VOID
EHCI_CompleteHsIsoPackets(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    __inout PMINIPORT_ISO_PACKET FirstPacket,
    __inout PHCD_HSISO_TRANSFER_DESCRIPTOR IsoTd
    )
/*++

Routine Description:

    Complete the eight high speed packets associated with this
    TD

Arguments:

Returns:

--*/
{
    ULONG length, i;
    ULONG cf = EHCI_Get32BitFrameNumber(DeviceData);
    PMINIPORT_ISO_PACKET pkt = FirstPacket;
    ULONG period;
    HC_ITD_TRANSACTION transaction;

    period = EndpointData->Parameters.Period;
    LOGENTRY(DeviceData, G, '_cpI', pkt, IsoTd, cf);

    // see vista bug #1349350
    //for (i=0; i<(8/period); i++) {
    for (i=0; i<8; i+=period) {
        transaction = IsoTd->HwTD.Transaction[i];
        if (transaction.Active == 1) {
            // missed

            // Don't leave an Active TD in the schedule if it is being
            // completed.
            //
            transaction.Active = 0;
            IsoTd->HwTD.Transaction[i] = transaction;

            pkt->LengthTransferred = 0;
            LOGENTRY(DeviceData, G, '_cms',
                pkt,
                i,
                pkt->FrameNumber);
            pkt->UsbdStatus = USBD_STATUS_ISO_NOT_ACCESSED_BY_HW;
        } else {
            // if this is an out assume all data transferred
            if (IsoTd->HwTD.BufferPointer1.Direction == 0) {
                // out
                length = pkt->Length;
                LOGENTRY(DeviceData, G, '_cp3',
                    pkt->FrameNumber,
                    pkt->Length ,
                    pkt);
            } else {
                // in
                length = transaction.Length;
                LOGENTRY(DeviceData, G, '_cp4',
                    pkt->FrameNumber,
                    pkt->Length ,
                    pkt);
            }

            pkt->LengthTransferred = length;

            // check the errubit

            if (transaction.XactError) {
                pkt->UsbdStatus = USBD_STATUS_XACT_ERROR;
            } else if (transaction.BabbleDetect) {
                pkt->UsbdStatus = USBD_STATUS_BABBLE_DETECTED;
            } else if (transaction.DataBufferError) {
                pkt->UsbdStatus = USBD_STATUS_DATA_BUFFER_ERROR;
            } else {
                pkt->UsbdStatus = USBD_STATUS_SUCCESS;
            }
            LOGENTRY(DeviceData, G, '_cpL', pkt, IsoTd, length);
            // ALI
            transaction.Length = 0;
            transaction.XactError = 0;
            transaction.BabbleDetect = 0;
            transaction.DataBufferError = 0;
            IsoTd->HwTD.Transaction[i] = transaction;
        }
        pkt++;
    }
}


USB_MINIPORT_STATUS
EHCI_OpenHsIsochronousEndpoint(
    __inout PDEVICE_DATA DeviceData,
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
    HW_32BIT_PHYSICAL_ADDRESS phys;
    ULONG i;
    PENDPOINT_DATA prevEndpoint, nextEndpoint;

    LOGENTRY(DeviceData, G, '_opS', 0, 0, EndpointParameters);

    buffer = EndpointParameters->CommonBufferVa;
    phys = EndpointParameters->CommonBufferPhys;

    // how much did we get


    EndpointData->HsIsoTdList = (PHCD_HSISOTD_LIST) buffer;
    // bugbug use manifest
    EndpointData->TdCount = USBEHCI_MAX_FRAME;
    EndpointData->LastFrame = 0;

    for (i=0; i<EndpointData->TdCount; i++) {

        EHCI_Initialize_iTD(DeviceData,
                            EndpointData,
                            EndpointParameters,
                            &EndpointData->HsIsoTdList->Td[i],
                            phys,
                            i);

        EHCI_AssertHwAlignment(DeviceData, phys, sizeof(HW_ISOCHRONOUS_TD));
        phys += sizeof(HCD_HSISO_TRANSFER_DESCRIPTOR);

    }

    //
    if (DeviceData->IsoEndpointListHead == NULL) {
        // empty list, no iso endpoints
        prevEndpoint = NULL;
        nextEndpoint = NULL;
    } else {
        // currently we insert HS endpoints in front of split
        // iso endpoints, so for high speed we just stick them
        // on the head of the list

        prevEndpoint = NULL;
        nextEndpoint = DeviceData->IsoEndpointListHead;
    }

    // insert this column of TDs thru the schedule
    EHCI_InsertHsIsoTdsInSchedule(DeviceData,
                                  EndpointData,
                                  prevEndpoint,
                                  nextEndpoint);

    // init endpoint structures
    InitializeListHead(&EndpointData->IsoTransferListHead);

    EHCI_EnablePeriodicList(DeviceData);

    return USBMP_STATUS_SUCCESS;
}


VOID
EHCI_RemoveHsIsoTdsFromSchedule(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

   unlink the iso TDs from the schedule

Arguments:


--*/
{
    ULONG i;
    PENDPOINT_DATA prevEndpoint, nextEndpoint;
    PHCD_QUEUEHEAD_DESCRIPTOR dQh = NULL;
    PHW_32BIT_PHYSICAL_ADDRESS frameBase;

    frameBase = DeviceData->FrameListBaseAddress;
    prevEndpoint = EndpointData->PrevEndpoint;
    nextEndpoint = EndpointData->NextEndpoint;

    LOGENTRY(DeviceData, G, '_iRM', prevEndpoint,
        nextEndpoint, EndpointData);

    if (DeviceData->IsoEndpointListHead == EndpointData) {
        // this is the head

        for (i=0; i<USBEHCI_MAX_FRAME; i++) {

            PHCD_HSISO_TRANSFER_DESCRIPTOR iTd;
            ULONG phys;

            iTd = &EndpointData->HsIsoTdList->Td[i];
            phys = iTd->HwTD.NextLink.HwAddress;

            if(DeviceData->UseDummyQheadsEnabled) {
                dQh = EHCI_GetDummyQueueHeadForFrame(DeviceData, i);
                dQh->HwQH.HLink.HwAddress = phys;

                dQh->NextLink = iTd->NextLink;
            }
            else {
                *frameBase = phys;
            }
            frameBase++;
        }

        DeviceData->IsoEndpointListHead =
            EndpointData->NextEndpoint;

        if (nextEndpoint != NULL) {
            EHCI_ASSERT(DeviceData,
                        nextEndpoint->PrevEndpoint == EndpointData);
            nextEndpoint->PrevEndpoint = NULL;
        }
    } else {
        // middle

        EHCI_ASSERT(DeviceData, HIGHSPEED(prevEndpoint));

        // link prev to next, prev will always be a HS ep
        prevEndpoint->NextEndpoint = nextEndpoint;
        if (nextEndpoint != NULL) {
            nextEndpoint->PrevEndpoint = prevEndpoint;
        }

        for (i=0; i<USBEHCI_MAX_FRAME; i++) {

            PHCD_HSISO_TRANSFER_DESCRIPTOR iTd, previTd;
            ULONG phys;

            iTd = &EndpointData->HsIsoTdList->Td[i];
            previTd = &prevEndpoint->HsIsoTdList->Td[i];

            phys = iTd->HwTD.NextLink.HwAddress;
            previTd->HwTD.NextLink.HwAddress = phys;
        }

    }
}


VOID
EHCI_InsertHsIsoTdsInSchedule(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    __inout_opt PENDPOINT_DATA PrevEndpointData,
    __inout_opt PENDPOINT_DATA NextEndpointData
    )
/*++

Routine Description:

   Insert HS Iso Tds into the HW list

Arguments:


--*/
{
    PHW_32BIT_PHYSICAL_ADDRESS frameBase = NULL;
    ULONG i;

    LOGENTRY(DeviceData, G, '_iAH', PrevEndpointData,
        NextEndpointData, EndpointData);

    // always insert to head
    EHCI_ASSERT(DeviceData, PrevEndpointData == NULL);

    DeviceData->IsoEndpointListHead = EndpointData;
    EndpointData->PrevEndpoint = NULL;

    EndpointData->NextEndpoint =
        NextEndpointData;
    if (NextEndpointData != NULL) {
        NextEndpointData->PrevEndpoint = EndpointData;
    }

    frameBase = DeviceData->FrameListBaseAddress;

    for (i=0; i<USBEHCI_MAX_FRAME; i++) {

        PHCD_HSISO_TRANSFER_DESCRIPTOR iTd = NULL;
        HW_32BIT_PHYSICAL_ADDRESS qhPhys = 0;
        PHCD_QUEUEHEAD_DESCRIPTOR dQh = NULL, stQh = NULL;
        ULONG phys = 0;

        iTd = &EndpointData->HsIsoTdList->Td[i];
        ASSERT_ITD(DeviceData, iTd);

        if(DeviceData->UseDummyQheadsEnabled) {
            dQh = EHCI_GetDummyQueueHeadForFrame(DeviceData, i);
        }

        // fixup next link
        if (NextEndpointData == NULL) {
            // no iso endpoints, link to the interrupt
            // queue heads via the dummy queue head

            if(DeviceData->UseDummyQheadsEnabled) {
                qhPhys = dQh->HwQH.HLink.HwAddress;
                iTd->HwTD.NextLink.HwAddress = qhPhys;
                iTd->NextLink = dQh->NextLink;
            } else { // EHCI_Generic
                qhPhys = *frameBase;
                iTd->HwTD.NextLink.HwAddress = qhPhys;
                stQh = EHCI_GetQueueHeadForFrame(DeviceData, i);
                EHCI_ASSERT(DeviceData, (qhPhys & ~EHCI_DTYPE_Mask) == stQh->PhysicalAddress);
                HW_PTR(iTd->NextLink) = (PUCHAR)stQh;
            }
        } else {
            // link to the next iso endpoint

            if (HIGHSPEED(NextEndpointData)) {
                PHCD_HSISO_TRANSFER_DESCRIPTOR tmp;

                tmp = &NextEndpointData->HsIsoTdList->Td[i];
                iTd->HwTD.NextLink.HwAddress =
                    tmp->PhysicalAddress;
                HW_PTR(iTd->NextLink) = (PUCHAR) tmp;
            } else {
                PHCD_SI_TRANSFER_DESCRIPTOR tmp;

                tmp = &NextEndpointData->SiTdList->Td[i%ISO_SCHEDULE_SIZE];
                phys = tmp->PhysicalAddress;
                SET_SITD(phys);

                iTd->HwTD.NextLink.HwAddress = phys;
                HW_PTR(iTd->NextLink) = (PUCHAR) tmp;
            }

        }

        // fixup prev link
        // since we always insert Hs iso on the head of the list
        // prev endpoint should always be NULL
        EHCI_ASSERT(DeviceData, PrevEndpointData == NULL);
        phys = iTd->PhysicalAddress;

        // link dummy QH to this TD
        if(DeviceData->UseDummyQheadsEnabled) { // NEC
            dQh->HwQH.HLink.HwAddress = phys;
            HW_PTR(dQh->NextLink) = (PUCHAR) iTd;
        }
        else { // EHCI_Generic
            *frameBase = phys;
        }
        frameBase++;
    }
}

#define     HSISO_SCHEDULE_MASK       0x3ff

VOID
EHCI_InternalPollHsIsoEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    BOOLEAN Complete,
    BOOLEAN AlwaysSchedule
    )
/*++

Routine Description:

    Called when the endpoint 'needs attention'


    static iso TD table
    --------------------
    0                       < -- (lastFrame & 0x3ff)
    1                       {completed}
    2                       {completed}
    3                       {completed}
    4                       < -- (currentframe & 0x3ff)
    ...

    1023
    ---------------------


Arguments:
    AlwaysSchedule:  Reduces perf optimizations in order to
        gaurantee that a transfer is scheduled.  This should only
        be used when first scheduling a transfer.

Return Value:

--*/

{
    ULONG x, i, loopC;
    ULONG currentFrame, lastFrame;
    PHCD_HSISO_TRANSFER_DESCRIPTOR iTd;
    PHCD_HSISO_TRANSFER_DESCRIPTOR lastiTd;
    HC_ITD_TRANSACTION transaction;
    PMINIPORT_ISO_PACKET packet;
    PLIST_ENTRY listEntry;
    ULONG period, cfNow, lookAhead;
    ULONG transfersPending, fc = 0;

    period = EndpointData->Parameters.Period;

    currentFrame = EHCI_Get32BitFrameNumber(DeviceData);
    lastFrame = EndpointData->LastFrame;

    LOGENTRY(DeviceData, G, '_pi>', lastFrame, currentFrame, 0);

    if (GREATER_THAN(currentFrame, lastFrame + USBEHCI_MAX_FRAME)) {
        PTRANSFER_CONTEXT transfer;
        // overrun
        lastFrame = currentFrame-1;
        LOGENTRY(DeviceData, G, '_ov1', lastFrame, currentFrame, 0);

        // dump the current contents
        for (i=0; i <USBEHCI_MAX_FRAME; i++) {

            iTd = &EndpointData->HsIsoTdList->Td[i];

            transfer = ISO_TRANSFER_PTR(iTd->Transfer);

            if (transfer != NULL) {
                ISO_PACKET_PTR(iTd->FirstPacket) = NULL;
                ISO_TRANSFER_PTR(iTd->Transfer) = NULL;
                EHCI_ASSERT(DeviceData, transfer->PendingPackets != 0);
                transfer->PendingPackets-=(8/period);
            }
        }
    }


    if (lastFrame == currentFrame) {
        // In our previous transfer we go up to currentFrame,
        // lastFrame can equal currentFrame and have valid TDs to schedule.
        // Keep the logging, but proceed as usual.
        LOGENTRY(DeviceData, G, '_ear', lastFrame, currentFrame, 0);
    }

    // TDs between lastframe and currentframe are complete,
    // complete the packets associated with them


//    f0
//    f1
//    f2  < ------- last frame   }
//    f3                         }  these are complete
//    f4                         }
//    f5  < ------- current frame
//    f6
//    f7
//    f8

    x = (lastFrame & (HSISO_SCHEDULE_MASK));

    lastiTd = NULL;

    LOGENTRY(DeviceData, G, '_frh', lastFrame, x, currentFrame);
    loopC = 0;
    // Unlike non-HS, all TDs up to the currentFrame are complete (no
    // limbo TD). So complete transfers up to currentFrame.
    while (x != ((currentFrame) & HSISO_SCHEDULE_MASK)) {
        PTRANSFER_CONTEXT transfer;

        iTd = &EndpointData->HsIsoTdList->Td[x];

        ASSERT_ITD(DeviceData, iTd);
        // complete this packet
        packet = ISO_PACKET_PTR(iTd->FirstPacket);
        loopC++;

        if (packet != NULL) {
            transfer = ISO_TRANSFER_PTR(iTd->Transfer);
            ASSERT_TRANSFER(DeviceData, transfer);
            EHCI_CompleteHsIsoPackets(DeviceData, EndpointData, packet, iTd);
            ISO_PACKET_PTR(iTd->FirstPacket) = NULL;
            ISO_TRANSFER_PTR(iTd->Transfer) = NULL;
            EHCI_ASSERT(DeviceData, transfer->PendingPackets != 0);
            transfer->PendingPackets-=(8/period);
        }

        lastFrame++;
        x++;
        x &= HSISO_SCHEDULE_MASK;
    }

    // attempt to program what we can, if iTD is NULL
    // then we can program this frame
    // NOTE: No scheduling if paused!
    cfNow = EHCI_Get32BitFrameNumber(DeviceData);
    LOGENTRY(DeviceData, G, '_gpk', 0, loopC, cfNow);

    lookAhead = 0;

    if (EndpointData->CurEpState != ENDPOINT_PAUSE) {
        PTRANSFER_CONTEXT transfer;

        LOGENTRY(DeviceData, G, '_psh', 0, cfNow, currentFrame);

        for (i=0; i <USBEHCI_MAX_FRAME; i++) {

            x = ((currentFrame+i) & HSISO_SCHEDULE_MASK);

            iTd = &EndpointData->HsIsoTdList->Td[x];
            ASSERT_ITD(DeviceData, iTd);

            LOGENTRY(DeviceData, G, '_gpf', iTd, x, currentFrame+i);

            // open slot?
            if (ISO_PACKET_PTR(iTd->FirstPacket) != NULL) {
                // no, bail
                LOGENTRY(DeviceData, G, '_gpC', 0, x, 0);
                continue;
            }

            // yes, see if we have a packet
            // this will fetch the first packet to transmit this frame
            packet = EHCI_GetPacketForFrame(DeviceData,
                                            EndpointData,
                                            &transfer,
                                            currentFrame+i);

            if (packet != NULL) {
                BOOLEAN ioc = FALSE;
                ULONG frameCount = transfer->IsoTransfer->PacketCount /
                    (8 / period);


                EHCI_ASSERT(DeviceData,
                            ISO_PACKET_PTR(iTd->FirstPacket) == NULL);

                // generate an interrupt on the first and last packet
                // of a transfer.  FrameComplete is the frame immediately
                // following the last frame of the transfer.  If we do
                // not set an interrupt on the last frame, the last transfer
                // in a stream may take a long time to complete.  We should
                // watch this change for any perf regressions.
                //
                if ((currentFrame+i) == (transfer->FrameComplete-1)) {
                    ioc = TRUE;
                }

                if ((currentFrame+i) == transfer->FrameComplete -
                                        frameCount) {
                    ioc = TRUE;
                }

//interrupt every frame
//ioc = TRUE;
                // map 8 microframes
                EHCI_MapHsIsoPacketsToTd(DeviceData, EndpointData,
                    packet, iTd, ioc);
                lastiTd = iTd;
                ISO_PACKET_PTR(iTd->FirstPacket) = packet;
                ASSERT_TRANSFER(DeviceData, transfer);
                ISO_TRANSFER_PTR(iTd->Transfer) = transfer;
                transfer->PendingPackets+=(8 / period);
            } else {
//                ULONG j;
                // no packets to program, bail out now
                LOGENTRY(DeviceData, G, '_gpB', 0, x, 0);

                lookAhead++;

                if (lookAhead>8 && !AlwaysSchedule) {
                    //
                    // Perf enhancement.  If we have more than 8 empty frames,
                    // break out.  However, if we are scheduling a new transfer,
                    // and the transfer begins more than 8ms in the future, it
                    // may never be scheduled.  In this case the AlwaysSchedule
                    // value should be set.
                    break;
                }
//perf
//                for (j=0; j<8; j++) {
//                    iTd->HwTD.Transaction[j].InterruptOnComplete = 0;
//                }
            }
        }

        // take a interrupt on the last TD programmed
        if (lastiTd != NULL) {
            if (8-period >= ARRAYSIZE(lastiTd->HwTD.Transaction)) {
                NT_ASSERTMSG("Not enough TDs for EP period",FALSE);
                return;
            }

            transaction = lastiTd->HwTD.Transaction[8-period];
            transaction.InterruptOnComplete = 1;
            lastiTd->HwTD.Transaction[8-period] = transaction;
        }
    }

    EndpointData->LastFrame = lastFrame;

    // walk our list of active iso transfers and see
    // if any are complete

    currentFrame = EHCI_Get32BitFrameNumber(DeviceData);

    listEntry = EndpointData->IsoTransferListHead.Flink;
    transfersPending = 0;

    while (listEntry != &EndpointData->IsoTransferListHead && Complete) {
        PTRANSFER_CONTEXT transfer;

        transfer = (PTRANSFER_CONTEXT) CONTAINING_RECORD(
                     listEntry,
                     struct _TRANSFER_CONTEXT,
                     TransferLink);

        LOGENTRY(DeviceData, G, '_ck1', transfer, transfer->FrameComplete+2
            , currentFrame);

        EHCI_ASSERT(DeviceData, transfer->Sig == SIG_EHCI_TRANSFER);
        if (GREATER_THAN_EQUAL_TO(currentFrame, transfer->FrameComplete) &&
            transfer->PendingPackets == 0) {

            ULONG p;

            listEntry = transfer->TransferLink.Flink;

            packet = &transfer->IsoTransfer->Packets[0];
            for (p = 0; p < transfer->IsoTransfer->PacketCount; p++) {
                LOGENTRY(DeviceData, G, '_ck2', transfer, 0, packet);

                if (packet->UsbdStatus == 0xffffffff) {
                    packet->UsbdStatus = USBD_STATUS_ISO_NOT_ACCESSED_BY_HW;
                }
                packet++;
            }

#if DBG
            cfNow = EHCI_Get32BitFrameNumber(DeviceData);
            LOGENTRY(DeviceData, G, '_cpi', transfer,
                transfer->FrameComplete, cfNow);
            LOGENTRY(DeviceData, G, '_cpP', transfer,
                transfer->PostedFrame, cfNow);

// for debug only
//            if (cfNow - transfer->FrameComplete > 3) {
//                TEST_TRAP(DeviceData);
//            }
#endif

            //RemoveEntryList(&transfer->TransferLink);
            EHCI_DecPendingTransfer(DeviceData, EndpointData, transfer);

            USBPORT_COMPLETE_ISO_TRANSFER(DeviceData,
                                          EndpointData,
                                          transfer->TransferParameters,
                                          transfer->IsoTransfer);
        } else {
            transfersPending++;
            fc = transfer->FrameComplete;
            listEntry = transfer->TransferLink.Flink;
        }
    }

    cfNow = EHCI_Get32BitFrameNumber(DeviceData);

    if (transfersPending == 1) {

        if (ABS((LONG)(fc - cfNow)) < 2) {
            LOGENTRY(DeviceData, G, '_reh', fc, cfNow, 0);
            EHCI_InterruptNextSOF(DeviceData);
        }
    }

    LOGENTRY(DeviceData, G, '_pi<', cfNow, 0, 0);
}


USB_MINIPORT_STATUS
EHCI_PokeIsoEndpoint(
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
    ULONG i;

    if (HIGHSPEED(EndpointData)) {

        PHCD_HSISO_TRANSFER_DESCRIPTOR iTd;

        EHCI_ASSERT(DeviceData,
                    EndpointData->TdCount == USBEHCI_MAX_FRAME);

        for (i=0; i<EndpointData->TdCount; i++) {

            iTd = &EndpointData->HsIsoTdList->Td[i];
            ASSERT_ITD(DeviceData, iTd);

            iTd->HwTD.BufferPointer0.DeviceAddress =
                 EndpointParameters->DeviceAddress;

            iTd->HwTD.BufferPointer0.EndpointNumber =
                EndpointParameters->EndpointAddress;

        }

    } else {
        PHCD_SI_TRANSFER_DESCRIPTOR siTd;

        for (i=0; i<EndpointData->TdCount; i++) {

            siTd = &EndpointData->SiTdList->Td[i];
            ASSERT_SITD(DeviceData, siTd);

            siTd->HwTD.Caps.DeviceAddress =
                EndpointParameters->DeviceAddress;
            siTd->HwTD.Caps.HubAddress =
                EndpointParameters->TtDeviceAddress;

        }
    }
    return USBMP_STATUS_SUCCESS;
}


ULONG
EHCI_AddDummyQueueHeads(
    __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

    NEC errata:

    Insert a table of 1024 dummy queue heads in the schedule for
    HW to access and point them at the interrupt queue heads.

    These queue heads must be before any iso TDs

    This is a workaround for a flaw in the NEC B0' stepping version
    of the controller.  We must put 'dummy' QH at the front of the
    peridoic list such that the first thing fetched is always a QH
    even when ISO TDs are in the schedule.

Arguments:

Return Value:

    memory consumed for the queue heads

--*/
{
    PHCD_QUEUEHEAD_DESCRIPTOR dQh, stqh;
    HC_QTD_TOKEN token;
    HW_32BIT_PHYSICAL_ADDRESS qhPhys;
    PHW_32BIT_PHYSICAL_ADDRESS frameBase;
    ULONG i;
    HW_32BIT_PHYSICAL_ADDRESS dqhPoolPhys;
    PUCHAR dqhPool;

    frameBase = DeviceData->FrameListBaseAddress;

    dqhPoolPhys = DeviceData->DummyQueueHeadsPhys;
    dqhPool = (PUCHAR)DeviceData->DummyQueueHeads;

    for (i = 0; i < USBEHCI_MAX_FRAME; i++) {
        PHCD_QUEUEHEAD_DESCRIPTOR dqhChk;

        // no iso endpoints should be in the schedule yet
        qhPhys = *frameBase;
        dqhChk = EHCI_GetDummyQueueHeadForFrame(DeviceData, i);

        // Note that we should never need to align since a
        // HCD_QUEUEHEAD_DESCRIPTOR is currently padded to 128 bytes.
        //
        dQh = (PHCD_QUEUEHEAD_DESCRIPTOR) dqhPool;
        EHCI_ASSERT(DeviceData, dqhChk == dQh);

        RtlZeroMemory(dQh, sizeof(HCD_QUEUEHEAD_DESCRIPTOR));
        dQh->PhysicalAddress = dqhPoolPhys;

        dqhPool     += sizeof(HCD_QUEUEHEAD_DESCRIPTOR);
        dqhPoolPhys += sizeof(HCD_QUEUEHEAD_DESCRIPTOR);

        // init the dummy queue head

        dQh->Sig = SIG_DUMMY_QH;

        dQh->HwQH.EpChars.DeviceAddress = 128;
        dQh->HwQH.EpChars.EndpointNumber = 0;
        dQh->HwQH.EpChars.EndpointSpeed = HcEPCHAR_FullSpeed;
        dQh->HwQH.EpChars.MaximumPacketLength = 64;

        dQh->HwQH.EpCaps.InterruptScheduleMask = 0;
        dQh->HwQH.EpCaps.SplitCompletionMask = 0;
        dQh->HwQH.EpCaps.HubAddress = 0;
        dQh->HwQH.EpCaps.PortNumber = 0;
        dQh->HwQH.EpCaps.HighBWPipeMultiplier = 1;

        dQh->HwQH.CurrentTD.HwAddress = 0;

        dQh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = EHCI_TERMINATE_BIT;
        dQh->HwQH.Overlay.qTD.Next_qTD.HwAddress = EHCI_TERMINATE_BIT;

        // Mark the HW_QUEUEHEAD_DESCRIPTOR as not Active in the Overlay
        // area.  This code is not strictly required here as the entire
        // dQh HCD_QUEUEHEAD_DESCRIPTOR containing the
        // HW_QUEUEHEAD_DESCRIPTOR was zero initialized above.
        //
        token.ul = 0; // Not Active
        dQh->HwQH.Overlay.qTD.Token = token;

        // link dummy to first interrupt queue head
        dQh->HwQH.HLink.HwAddress = qhPhys;
        stqh = EHCI_GetQueueHeadForFrame(DeviceData, i);
        EHCI_ASSERT(DeviceData, (qhPhys & ~EHCI_DTYPE_Mask) ==
            stqh->PhysicalAddress);

        HW_PTR(dQh->NextLink) = (PUCHAR)stqh;

        // add dummy queue head to frame list
        qhPhys = dQh->PhysicalAddress;
        EHCI_AssertHwAlignment(DeviceData, qhPhys, sizeof(HW_QUEUEHEAD_DESCRIPTOR));

        SET_QH(qhPhys);
        *frameBase = qhPhys;

        frameBase++;
    }

    // return how much memory we used
    return dqhPoolPhys - DeviceData->DummyQueueHeadsPhys;
}


VOID
EHCI_InterruptNextSOF(
    __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

    This routine is always called with the USBPORT Fdo.CoreFunctionSpin
    spinlock held.

Arguments:

Return Value:

--*/
{
    USBPORT_INVALIDATE_CONTROLLER(DeviceData, UsbMpSimulateInterrupt);
}
