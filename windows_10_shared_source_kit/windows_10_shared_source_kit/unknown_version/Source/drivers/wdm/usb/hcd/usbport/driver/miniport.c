/*
Module: MINIPORT.C


name:USB miniport interface (HCDI)

    Copyright (c) Microsoft Corporation. All rights reserved



Abstract:


    Functions used to interface with USB miniport drivers. This file contains both USBX (extended) and standard
    documentation.

    naming convention:

    MINIPORT_Usbx<FuncName> - USBX miniport extensions.
    MINIPORT_<FuncName> - standard and USB2.



<pre>

<USBX miniport extensions>
This is a general name given extensions to the standard USB miniport interface in the Longhorn codebase.  These
extensions will be used in USBEHCI.SYS to move the USB2 bus specific budget code into the miniport.  They provide
a better method for synchronizing with the hardware on more sophisticated controllers.  All extended miniport
functions begin with the preamble (Usbx) and are only called in the miniport sets the OPT_USBX flag on registration
with USBPORT.SYS.  The standard USB 1.1 controller miniports do not use these extensions.  These extension do not
contain any USB2 specific features -- these are isolated with a separate flag OPT_USB2.

Modifications to the miniport interface for WHCI (wireless USB) should occur in this interface.

</pre>


Notes:

    Margin = 120

--*/

#include "common.h"

#ifdef USB2_INTERNAL
// globals
extern ULONG USB2LIB_HcContextSize;
extern ULONG USB2LIB_TtContextSize;
extern ULONG USB2LIB_EndpointContextSize;
#endif

VOID
MPh_AcquireMpSuspendResumeLock(
    PDEVICE_EXTENSION HcFdoExt
    )
{
    ASSERT_PASSIVE();

    KeWaitForSingleObject(&HcFdoExt->Fdo.MpSuspendResumeLock,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);
    LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_PNP, 'MPL+',
        HcFdoExt->Fdo.MpResumeThread, HcFdoExt->Fdo.MpSuspendThread, 0);

}

VOID
MPh_ReleaseMpSuspendResumeLock(
    PDEVICE_EXTENSION HcFdoExt
    )
{
    ASSERT_PASSIVE();

    LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_PNP, 'MPL-',
        HcFdoExt->Fdo.MpResumeThread, HcFdoExt->Fdo.MpSuspendThread, 0);

    KeReleaseSemaphore(&HcFdoExt->Fdo.MpSuspendResumeLock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);
}


/*

Routine: MINIPORT_CheckHwSync

ULONG MINIPORT_CheckHwSync(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext
    );


Description:

    This routine allows the miniport to manage the hardware synchronization for the removal of endpoints (queue heads)
    internally.  If the OPT_HW_SYNC flag is indicated by the miniport then the endpoint will be polled on each hardware
    interrupt until the miniport indicates the endpoint staructures have been removed from the HC schedule.


HCI_Revison - 3+.

Core_Lock - yes.

Irql - DPC_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Endpoint Context.


Returns:

    non-zero if the endpoint has finished synchronizing with the host adapter.  0 if it is still busy.

*/
ULONG
MPf_CheckHwSync(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    ULONG hc_done = 0;
    KIRQL irql;

    USBPORT_ASSERT(USBPORT_HW_SYNC(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_PollHwSync != NULL);

        if (MPF(HcFdoExt).MINIPORT_PollHwSync == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            hc_done = 1;
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        hc_done = MPF(HcFdoExt).MINIPORT_PollHwSync(HcFdoExt->Fdo.MiniportDeviceData,
                                                    &Endpoint->MiniportEndpointData[0]);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

    return hc_done;
}

/*

Routine: MINIPORT_UsbxOpenEndpoint

USB_MINIPORT_STATUS MINIPORT_UsbxOpenEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_PARAMETERS EpParameters,
    PENDPOINT_DATA EpContext,
    PHSB_HC_DATA HsbHcContext,
    PHSB_EP_DATA HsbEpContext,
    PHSB_TT_DATA HsbTtContext
    );


Description:

    Extended version to support USB 2.0 and wireless USB. This version is called at passive level, the miniport may
    block.  This opertion is serialized thru the miniport so it is not reentrant.

HCI_Revison - 3+.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

EpParameters(Epp) - general USB parmeters. This structure is identical to OpenEndpoint (classic version), however,
some fields are no longer used.

EpContext - Endpoint Context.

HsbHcContext, HsbEpContext, HsbTtContext - (optional) high speed bus context structures.

<pre>
------------------------------------------------------------------------------------------------------------------------
Epp.DeviceAddress - input, from endpoint descriptor.
Epp.EndpointAddress - input, from endpoint descriptor.
Epp.MaxPacketSize - input, from endpoint descriptor.
Epp.MaxPeriod - input, max allowable period from endpoint descriptor.
Epp.DeviceSpeed- input, device operating speed.
Epp.TransferType - input, from endpoint descriptor.
Epp.TransferDirection input, from endpoint descriptor.
Epp.CommonBufferVa - input, .
Epp.CommonBufferPhys - input, .
Epp.CommonBufferBytes - input, .
Epp.EndpointFlags - input, endpoint option flags (EP_PARM_FLAG_*).
Epp.TtDeviceAddress - input, .
Epp.TtPortNumber - input, port number (really TT number) of the TT for this device.
Epp.TransactionsPerMicroframe - input, from endpoint descriptor.
Epp.MuxPacketSize - input, from endpoint descriptor.

Epp.MaxTransferSize - output, maximum size in bytes of any single transfer.
Epp.Period -  output, adjusted interrupt period will be one of : 128, 64, 32, 16, 8, 4, 2, 1. miniport must select the
period.
Epp.Bandwidth -  output, relative bandwidth consumed in bits/ms. If the device is on a TT this is the relative bandwdth
consumed on the TT.

Epp.ScheduleOffset - reserved, (do not use).
Epp.Ordinal - reserved,(do not use).
Epp.Pad - reserved,(do not use).
Epp.InterruptScheduleMask - reserved, (do not use).
Epp.SplitCompletionMask - reserved, (do not use).
-----------------------------------------------------------------------------------------------------------------------
</pre>

Returns:

    MP_STATUS_SUCCESS if the endpoint can be initialized. Endpoint should be initially set to the PAUSE state if
    success is returned.

*/
USB_MINIPORT_STATUS
MPf_UsbxOpenEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR Tt,
    __inout_opt USBD_STATUS *UsbdStatus
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;
    PHSB_TT_DATA hsbTt = NULL;

    if (Tt) {
        USBPORT_ASSERT(Endpoint->Tt == Tt);
        hsbTt = Tt->HsbTtContextPtr;

    }

    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxOpenEndpoint!= NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxOpenEndpoint == NULL) {
            break;
        }

        mps = MPF(HcFdoExt).MINIPORT_UsbxOpenEndpoint(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->Parameters,
                                                &Endpoint->MiniportEndpointData[0],
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                Endpoint->HsbEpContextPtr,
                                                hsbTt);

    } WHILE (0);

    return mps;
}

/*

Routine: MINIPORT_UsbxPokeEndpoint

USB_MINIPORT_STATUS MINIPORT_UsbxPokeEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_PARAMETERS EpParameters,
    PENDPOINT_DATA EpContext,
    PHSB_HC_DATA HsbHcContext,
    PHSB_EP_DATA HsbEpContext,
    PHSB_TT_DATA HsbTtContext
    );


Description:

    Use to change the parmeters of an endpoint without removing it from USBPORT internal queues.  This call is only
    valid if the endpoint is in the PAUSE state.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Endpoint Context.

EpParameters - general USB endpoint parmeters.

HsbHcContext, HsbEpContext, HsbTtContext - (optional) high speed bus context structures.

Returns:

    MP_STATUS_SUCCESS if the endpoint can be re-initialized.


*/
USB_MINIPORT_STATUS
MPf_UsbxPokeEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR Tt
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;
    PHSB_TT_DATA hsbTt = NULL;

    if (Tt) {
        USBPORT_ASSERT(Endpoint->Tt == Tt);
        hsbTt = Tt->HsbTtContextPtr;

    }

    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxPokeEndpoint!= NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxPokeEndpoint == NULL) {
            break;
        }

        mps = MPF(HcFdoExt).MINIPORT_UsbxPokeEndpoint(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->Parameters,
                                                &Endpoint->MiniportEndpointData[0],
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                Endpoint->HsbEpContextPtr,
                                                hsbTt);

    } WHILE (0);

    return mps;
}


/*

Routine: MINIPORT_UsbxInitTransactionTranslator

VOID MINIPORT_UsbxInitTransactionTranslator(
    PDEVICE_DATA HcContext,
    PHSB_HC_DATA HsbHcContext,
    PHSB_TT_DATA HsbTtContext
    );


Description:

    Initializes an instance of a tt in the miniport.  This routine is only called if the OPT_USB20 flag is set.

HCI_Revison - 3+.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

TtContext - Transaction Translator Context.

Returns:

    none.


*/
USB_MINIPORT_STATUS
MPf_UsbxInitHsbTransactionTranslator(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in PTRANSACTION_TRANSLATOR Tt
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;

    // this routine is the analog to USB2LIB_InitTt
    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxInitHsbTransactionTranslator != NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxInitHsbTransactionTranslator == NULL) {
            break;
        }

        mps = MPF(HcFdoExt).MINIPORT_UsbxInitHsbTransactionTranslator(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                Tt->HsbTtContextPtr);

    } WHILE (0);

    return mps;
}


/*

Routine: MINIPORT_UsbxInitHsbController

VOID MINIPORT_UsbxInitHsbController(
    PDEVICE_DATA HcContext,
    PHSB_HC_DATA HsbHcContext
    );


Description:

    Initializes an instance of a high speen controller thru the miniport.  This routine is only called if the
    OPT_USB20 flag is set.

HCI_Revison - 3+.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.


Returns:

    none.


*/
USB_MINIPORT_STATUS
MPf_UsbxInitHsbController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;

    // this routine is the analog to USB2LIB_InitTt
    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxInitHsbController != NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxInitHsbController == NULL) {
            break;
        }

        mps = MPF(HcFdoExt).MINIPORT_UsbxInitHsbController(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                HcFdoExt->Fdo.HsbControllerContextPtr);

    } WHILE (0);

    return mps;
}


/*

Routine: MINIPORT_UsbxInitHsbEndpoint

VOID MINIPORT_UsbxInitHsbEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext,
    PHSB_HC_DATA HsbHcContext,
    PHSB_EP_DATA HsbEpContext
    PHSB_TT_DATA HsbTtContext
    );


Description:

    Initializes an instance of high speed endpoint in the miniport.  This routine is only called if the
    OPT_USB20 flag is set.

HCI_Revison - 3+.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.


Returns:

    none.


*/
USB_MINIPORT_STATUS
MPf_UsbxInitHsbEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __in PTRANSACTION_TRANSLATOR Tt
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;

    // this routine is the analog to USB2LIB_InitTt
    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxInitHsbEndpoint!= NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxInitHsbEndpoint == NULL) {
            break;
        }

        mps = MPF(HcFdoExt).MINIPORT_UsbxInitHsbEndpoint(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0],
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                Endpoint->HsbEpContextPtr,
                                                Tt->HsbTtContextPtr);

    } WHILE (0);

    return mps;
}


/*

Routine: MINIPORT_UsbxAllocateBandwidth

USB_MINIPORT_STATUS MINIPORT_UsbxAllocateBandwidth(
    PDEVICE_DATA HcContext,
    TT_DATA TtContext,
    PENDPOINT_PARAMETERS EpParameters,
    PENDPOINT_DATA EpContext
    );

Description:

To simplify backward compatibility bandwidth is allocated thru a separate function call into the miniport.  Bandwidth
is allocated in the context of UsbxOpenendpoint and serialized in the same way.  USBPORT will use  'Legacy' USB 11
bandwidth calculations on behalf of the miniport if the OPT_USB11 flag is set.

<pre>
The following table indicates supported values for combinations of USBX, USB11, USB20 miniport flags.

USB_MINIPORT_OPT_USB11                0x00000008
USB_MINIPORT_OPT_USB20                0x00000010
USB_MINIPORT_OPT_USBX                 0x00000400


mask        OPT_USB11       OPT_USB20       OPT_USBX
0x00000000  0               0               0               - Legacy USB 1.1 bw allocation
0x00000400  0               0               1               - Legacy USB 1.1 bw allocation
0x00000010  0               1               0               - not supported (a)
0x00000410  0               1               1               - Allocate thru miniport
0x00000008  1               0               0               - Legacy USB 1.1 bw allocation
0x00000408  1               0               1               - Allocate thru miniport
0x00000018  1               1               0               - not supported, invalid
0x00000418  1               1               1               - not supported, invalid

(a) not supported since the only high speed ehci miniport is usbehci.



</pre>

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

TtContext - Transaction Translator Context (optional).

EpParameters(Epp) - general USB parmeters. This structure is identical to OpenEndpoint (classic version), however,
some fields are no longer used.

<pre>
------------------------------------------------------------------------------------------------------------------------
The following fields are filled in by the port driver:

Epp.DeviceAddress - input, from endpoint descriptor.
Epp.EndpointAddress - input, from endpoint descriptor.
Epp.MaxPacketSize - input, from endpoint descriptor.
Epp.MaxPeriod - input, max allowable period from endpoint descriptor.
Epp.DeviceSpeed- input, device operating speed.
Epp.TransferType - input, from endpoint descriptor.
Epp.TransferDirection input, from endpoint descriptor.
Epp.CommonBufferVa - input, .
Epp.CommonBufferPhys - input, .
Epp.CommonBufferBytes - input, .
Epp.EndpointFlags - input, endpoint option flags (EP_PARM_FLAG_*).
Epp.TtDeviceAddress - input, .
Epp.TtPortNumber - input, port number (really TT number) of the TT for this device.
Epp.TransactionsPerMicroframe - input, from endpoint descriptor.
Epp.MuxPacketSize - input, from endpoint descriptor.

The following fields are filled in by the miniport driver:
Epp.MaxTransferSize - output, maximum size in bytes of any single transfer.
Epp.Period -  output, adjusted interrupt period will be one of : 128, 64, 32, 16, 8, 4, 2, 1. miniport must select the
period.
Epp.Bandwidth -  output, relative bandwidth consumed in bits/ms. If the device is on a TT this is the relative bandwdth
consumed on the TT.

Epp.ScheduleOffset - reserved, (do not use).
Epp.Ordinal - reserved,(do not use).
Epp.Pad - reserved,(do not use).
Epp.InterruptScheduleMask - reserved, (do not use).
Epp.SplitCompletionMask - reserved, (do not use).
-----------------------------------------------------------------------------------------------------------------------
</pre>

EpContext - Endpoint Context.

Returns:

    STATUS_SUCCESS if the bandwidth is allocated.


*/
USB_MINIPORT_STATUS
MPf_UsbxAllocateBandwidth(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR Tt
    )
{
    USB_MINIPORT_STATUS mps;
    PHSB_TT_DATA hsbTt = NULL;

    mps = USBMP_STATUS_FAILURE;

    if (Tt) {
        hsbTt = Tt->HsbTtContextPtr;
    }


    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxAllocateBandwidth != NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxAllocateBandwidth == NULL) {
            break;
        }

        mps = MPF(HcFdoExt).MINIPORT_UsbxAllocateBandwidth(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->Parameters,
                                                &Endpoint->MiniportEndpointData[0],
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                hsbTt,
                                                Endpoint->HsbEpContextPtr);
    } WHILE (0);

    return mps;
}


/*

Routine: MINIPORT_UsbxFreeBandwidth

VOID MINIPORT_UsbxFreeBandwidth(
    PDEVICE_DATA HcContext,
    TT_DATA TtContext,
    PENDPOINT_DATA EpContext
    );


Description:

    Miniport should release any bandwidth allocated by MINIPORT_UsbxAllocateBandwidth for this endpoint.  This routine
    is only called if the miniport successfuly allocates bandwidth for an endpoint.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

TtContext - Transaction Translator Context (optional).

EpContext - Endpoint Context.

Returns:

    none.


*/
VOID
MPf_UsbxFreeBandwidth(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR Tt
    )
{
    PHSB_TT_DATA hsbTt = NULL;

    USBPORT_ASSERT(Endpoint->EpFlags.USBX_Bandwidth);

    if (Tt) {
        hsbTt = Tt->HsbTtContextPtr;
    }

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxFreeBandwidth != NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxFreeBandwidth == NULL) {
            break;
        }

        // call the engine and attempt to allocate the necessary
        // bus time for this endoint

        MPF(HcFdoExt).MINIPORT_UsbxFreeBandwidth(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0],
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                hsbTt,
                                                Endpoint->HsbEpContextPtr);
    } WHILE (0);

    return ;
}

/*

Routine: MINIPORT_UsbxQueryBandwidthData

ULONG MINIPORT_UsbxQueryBandwidthData(
    PDEVICE_DATA HcContext,
    PHSB_HC_DATA HsbHcContext,
    ULONG Frame,
    ULONG Microframe
    );


Description:

    Queries internal bandwidth data from the miniport.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Frame - USB frame number

Microframe - USB microframe number

Returns:

    used bandwidth for the frame,microframe.


*/
ULONG
MPf_UsbxQueryBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in ULONG Frame,
    __in ULONG Microframe
    )
{
    ULONG bw = 0;

    // this routine is the analog to USB2LIB_InitTt
    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxQueryBandwidthData!= NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxQueryBandwidthData == NULL) {
            break;
        }

        bw = MPF(HcFdoExt).MINIPORT_UsbxQueryBandwidthData(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                Frame,
                                                Microframe);

    } WHILE (0);

    return bw;
}


/*

Routine: MINIPORT_UsbxQueryTtBandwidthData

ULONG MINIPORT_UsbxQueryTtBandwidthData(
    PDEVICE_DATA HcContext,
    PHSB_HC_DATA HsbHcContext,
    PHSB_TT_DATA HsbTtContext,
    ULONG Frame
    );


Description:

    Queries internal bandwidth data from the miniport for a given TT.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

TtContext - context for tt.

Returns:

     used bandwidth for the frame.


*/
ULONG
MPf_UsbxQueryTtBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PTRANSACTION_TRANSLATOR Tt,
    __in ULONG Frame
    )
{
   ULONG bw = 0;

    // this routine is the analog to USB2LIB_InitTt
    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxQueryTtBandwidthData!= NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxQueryTtBandwidthData == NULL) {
            break;
        }

        bw = MPF(HcFdoExt).MINIPORT_UsbxQueryTtBandwidthData(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                Tt->HsbTtContextPtr,
                                                Frame);

    } WHILE (0);


    return bw;
}


/*

Routine: MINIPORT_UsbxQueryEpBandwidthData

ULONG MINIPORT_UsbxQueryEpBandwidthData(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EndpointContext,
    PHSB_HC_DATA HsbHcContext,
    PHSB_EP_DATA HsbEpContext
    PHSB_TT_DATA HsbTtContext
    );


Description:

    Queries internal bandwidth data from the miniport for a given endpoint.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EndpointContext - context for endpoint.

Returns:

     used bandwidth for the endpoint.


*/
ULONG
MPf_UsbxQueryEpBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout_opt PTRANSACTION_TRANSLATOR Tt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    PHSB_TT_DATA hsbTt = NULL;
    ULONG bw = 0;

    if (Tt) {
        hsbTt = Tt->HsbTtContextPtr;
    }

    // this routine is the analog to USB2LIB_InitTt
    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxQueryEpBandwidthData!= NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxQueryEpBandwidthData == NULL) {
            break;
        }

        bw = MPF(HcFdoExt).MINIPORT_UsbxQueryEpBandwidthData(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0],
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                Endpoint->HsbEpContextPtr,
                                                hsbTt);

    } WHILE (0);


    return bw;

}


/*

Routine: MINIPORT_NotifyTransferQueueState

VOID
MINIPORT_NotifyTransferQueueState(
    PDEVICE_DATA HcContext,
    ULONG AsyncTransferCount,
    ULONG PeriodicTransferCount
    );


Description:

    Notifies the miniport of the state of the transfer queue. This is used by the ehci miniport to disble the periodic
    or async lists when not in use.

Core_Lock - yes.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

AsyncTransferCount - current count of async (bulk, control) transfers in the queue.

PeriodicTransferCount - current count of perodic (isoch, interrupt) in the queue.

Returns:

    none.

*/
VOID
MPf_NotifyTransferQueueState(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in ULONG AsyncTransferCount,
    __in ULONG PeriodicTransferCount
    )
{
    KIRQL irql;

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_NotifyTransferQueueState == NULL) {
            // treat this one as optional
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_NotifyTransferQueueState(
                               HcFdoExt->Fdo.MiniportDeviceData,
                               AsyncTransferCount,
                               PeriodicTransferCount);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}



/*

Routine: MINIPORT_TakePortControl

VOID MINIPORT_TakePortControl(
    PDEVICE_DATA HcContext
    );


Description:

    This routine sets the owner of a shared USB port. 'Take port control' and chirp_ports are version 3+ specific we
    need them for power management to work properly with EHCI host controllers.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     none


*/
VOID
MPf_TakePortControl(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_TakePortControl != NULL);

        if (MPF(HcFdoExt).MINIPORT_TakePortControl == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_TakePortControl(HcFdoExt->Fdo.MiniportDeviceData);

    }  WHILE (0);

}


/*

Routine: MINIPORT_ReleasePortControl

VOID MINIPORT_ReleasePortControl(
    PDEVICE_DATA HcContext
    );


Description:

    Release control of USB 2 ports to CCs. This routine sets the owner of a shared USB ports. This routine is only
    supported if miniport indicates version 3 or higher HCI.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     none


*/
VOID
MPf_ReleasePortControl(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }
        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_ReleasePortControl != NULL);

        if (MPF(HcFdoExt).MINIPORT_ReleasePortControl == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        MPF(HcFdoExt).MINIPORT_ReleasePortControl(HcFdoExt->Fdo.MiniportDeviceData);

    } WHILE (0);

}


/*

Routine: MINIPORT_SuspendController

VOID MINIPORT_SuspendController(
    PDEVICE_DATA HcContext
    );


Description:

    Put the host controller in global suspend state.  This routine should only be called once while the
    before MP_ResumeController.

HCI_Revison - all.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     none

*/

VOID
MPf_SuspendController(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in BOOLEAN EnableRH_IRQ
    )
{
    KIRQL           irql;
    PDEVICE_OBJECT  hcFdo;
    ULONG           mpOptionFlags;

    ASSERT_PASSIVE();

    hcFdo = HcFdoExt->HcFdoDeviceObject;

    mpOptionFlags = REGISTRATION_PACKET(HcFdoExt).OptionFlags;

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_SuspendController != NULL);

        if (MPF(HcFdoExt).MINIPORT_SuspendController == NULL) {
            break;
        }

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, irql);

        if (!TEST_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_STARTED)) {
            // only suspend if started
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, irql);
            break;
        }


        if (TEST_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED)) {
            // already suspended, bail
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, irql);
            break;
        }

        SET_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED);

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, irql);

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        MPh_AcquireMpSuspendResumeLock(HcFdoExt);

        LOGENTRY(hcFdo, LOG_TRANSFER, 'mpS+', 0, 0, 0);
        USBPORT_ASSERT(HcFdoExt->Fdo.MpResumeThread == NULL);
        HcFdoExt->Fdo.MpSuspendThread = KeGetCurrentThread();

        // introduce a 10ms wait here to allow any port suspends to finish -- according to spec
        USBPORT_Wait(HcFdoExt->HcFdoDeviceObject, 10);

        if (EnableRH_IRQ) {
            MPRH_EnableIrq(HcFdoExt, ROOTHUB_LOCK_NOT_ACQUIRED);
        }

        if (TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_SUSPEND_DISABLE_IRQ)) {
            //
            // Prevent the race condition where the miniport ISR or DPC
            // runs while in the process of suspending the controller.

            // Disable the host controller interrupts.  The miniport
            // will take care of reenabling the RH port status change
            // interrupt when it has finished suspending the controller.
            //
            KeAcquireSpinLock(&HcFdoExt->Fdo.IsrDpcSpin, &irql);

            MPF(HcFdoExt).MINIPORT_DisableInterrupts(HcFdoExt->Fdo.MiniportDeviceData);

            KeReleaseSpinLock(&HcFdoExt->Fdo.IsrDpcSpin, irql);

            // Flush DPCs to flush the ISR DPC
            //
            KeFlushQueuedDpcs();
        }

        // Wait for any outstanding AsyncCallbacks to complete before
        // calling MINIPORT_SuspendController().
        //
        KeWaitForSingleObject(&HcFdoExt->IoRequestAsyncCallbackEvent,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);

        LOGENTRY(hcFdo, LOG_TRANSFER, 'mpS1', 0, 0, 0);
        MPF(HcFdoExt).MINIPORT_SuspendController(HcFdoExt->Fdo.MiniportDeviceData);
        LOGENTRY(hcFdo, LOG_TRANSFER, 'mpS2', 0, 0, 0);

        USBPORT_ASSERT(HcFdoExt->Fdo.MpSuspendThread != NULL);
        HcFdoExt->Fdo.MpSuspendThread = NULL;

        MPh_ReleaseMpSuspendResumeLock(HcFdoExt);

    } WHILE (0);

}


/*

Routine: MINIPORT_ResumeController

USB_MINIPORT_STATUS MINIPORT_ResumeController(
    PDEVICE_DATA HcContext
    );


Description:

    Attempts to resume HC HW from the suspend state.  The miniport may fail this in the event the controller has been
    hosed by the BIOS.  In that even the port driver may attempt to power cycle.

HCI_Revison - all.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     miniport status code (success or error)


*/

USB_MINIPORT_STATUS
MPf_ResumeController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_FAILURE;
    KIRQL dm_irql;
    PDEVICE_OBJECT hcFdo;

    hcFdo = HcFdoExt->HcFdoDeviceObject;

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_ResumeController != NULL);

        if (MPF(HcFdoExt).MINIPORT_ResumeController == NULL) {
            break;
        }

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dm_irql);

        if (TEST_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED)) {
            CLEAR_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED);
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dm_irql);

        } else {
            // not suspended, assume we are resumed
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dm_irql);
            mps = USBMP_STATUS_SUCCESS;

            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        //
        // MINIPORT_ResumeController would touch the controller hardware registers to resume the
        // controller. At this time, we must ensure that the controller devnode is not in D3.
        // If the controller is in D3 it is possible that the power to the controller and
        // Phys is off. Thus it is not Ok to touch the hardware registers unless the controller
        // is in D0.
        // Thus verify that at the time we are calling the resume controller the
        // Run time PM state should be active a.k.a the controller would be in D0.
        //
        USBPORT_RuntimePMStateActiveAssert(hcFdo);

        MPh_AcquireMpSuspendResumeLock(HcFdoExt);

        LOGENTRY(hcFdo, LOG_TRANSFER, 'mpR+', 0, 0, 0);
        USBPORT_ASSERT(HcFdoExt->Fdo.MpSuspendThread == NULL);
        HcFdoExt->Fdo.MpResumeThread = KeGetCurrentThread();

        mps = MPF(HcFdoExt).MINIPORT_ResumeController(HcFdoExt->Fdo.MiniportDeviceData);

        LOGENTRY(hcFdo, LOG_TRANSFER, 'mpR-', 0, 0, mps);
        USBPORT_ASSERT(HcFdoExt->Fdo.MpResumeThread != NULL);
        HcFdoExt->Fdo.MpResumeThread = NULL;

        MPh_ReleaseMpSuspendResumeLock(HcFdoExt);

    } WHILE (0);


    return mps;
}

/*

Routine: MINIPORT_ReadCfgFlag

ULONG MINIPORT_ReadCfgFlag(
    PDEVICE_DATA HcContext
    );


Description:

    Return the value of the USB2 config flag register.

Core_Lock - no.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     Current value of config flag register. Zero if config flag not supported by this hardware.


*/

ULONG
MPf_ReadCfgFlag(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    ULONG cf = 0;

    do {
        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_ReadCfgFlag == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        // only read if the USB20 (ehci) flag is set.
        if (USBPORT_IS_USB20(HcFdoExt)) {
            USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);
            cf = MPF(HcFdoExt).MINIPORT_ReadCfgFlag(HcFdoExt->Fdo.MiniportDeviceData);
        }

    } WHILE (0);

    return cf;
}


/*

Routine: MINIPORT_PollController

VOID MINIPORT_PollController(
    PDEVICE_DATA HcContext
    );


Description:

     Optional Poll routine for miniport this function will be called at the MiniportPollInterval specficeid in
     the registration packet.

     Specifying a value of zero in the registration packet disables polling of the controller.

Core_Lock - no.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     none.


*/

VOID
MPf_PollController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    KIRQL irql;

    do {

        if (MPF(HcFdoExt).MINIPORT_PollController == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        // The Fdo.MpRootHubFunctionSpin spinlock must always be held when
        // USBPORTSVC_InvalidateRootHub() is called and
        // MINIPORT_PollController() can call USBPORTSVC_InvalidateRootHub().
        //
        //
        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject,
                                &HcFdoExt->Fdo.MpRootHubFunctionSpin, &irql);

        MPF(HcFdoExt).MINIPORT_PollController(HcFdoExt->Fdo.MiniportDeviceData);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject,
                                &HcFdoExt->Fdo.MpRootHubFunctionSpin, irql);

    } WHILE (0);

}



/*

Routine: MINIPORT_Get32BitFrameNumber

ULONG MINIPORT_Get32BitFrameNumber(
    PDEVICE_DATA HcContext
    );


Description:

    Return the value of the USB 32 bit frame counter.

Core_Lock - yes.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     Current frame number.


*/

ULONG
MPf_Get32BitFrameNumber(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    ULONG current_frame = 0;
    KIRQL irql, dmirql;

    do {

        if (MPF(HcFdoExt).MINIPORT_Get32BitFrameNumber == NULL) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            current_frame = 0xFFFFFFFF;
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.PowerFlags.Suspended ||
            HcFdoExt->Fdo.PowerFlags.Off ||
            HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {

            current_frame = 0xFFFFFFFF;
        } else {
            current_frame = MPF(HcFdoExt).MINIPORT_Get32BitFrameNumber(
                                                       HcFdoExt->Fdo.MiniportDeviceData);
        }

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);


    } WHILE (0);

    return current_frame;
}

//#ifdef VISTA_RTM

/*

Routine: MINIPORT_Get32BitMicroFrameNumber

ULONG MINIPORT_Get32BitMicroFrameNumber(
    PDEVICE_DATA HcContext
    );


Description:

    Return the value of the USB 32 bit frame counter and microframe counter.

Core_Lock - yes.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

HsFrameCounter - high speed frame counter with microframes on the end (not shifted off)

Returns:

     high speed frame counter with microframes on the end (not shifted off).


*/

ULONG
MPf_Get32BitMicroFrameNumber(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    ULONG current_hs_frame = 0;
    KIRQL irql, dmirql;

    do {


        if (MPF(HcFdoExt).MINIPORT_Get32BitMicroFrameNumber == NULL) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            current_hs_frame = 0xFFFFFFFF;
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.PowerFlags.Suspended ||
            HcFdoExt->Fdo.PowerFlags.Off ||
            HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {

            current_hs_frame = 0xFFFFFFFF;
        } else {
            current_hs_frame = MPF(HcFdoExt).MINIPORT_Get32BitMicroFrameNumber(
                                                       HcFdoExt->Fdo.MiniportDeviceData);
        }

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);


    } WHILE (0);

    return current_hs_frame;
}

//#endif

/*

Routine: MINIPORT_InterruptService

BOOLEAN
MINIPORT_InterruptService (
    PDEVICE_DATA DeviceData
    );

Description:


    Interrupt service routine for HC hardware interrupt.  If TRUE is returned the ISR DPC is queued to handle the
    interrupt.

Core_Lock - no.

HCI_Revison - all.

Irql - IRQ_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     TRUE if controller generated the interrupt.


*/

BOOLEAN
MPf_InterruptService(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    BOOLEAN usb_int = FALSE;

    do {


        if (MPF(HcFdoExt).MINIPORT_InterruptService == NULL) {
            break;
        }

        usb_int = MPF(HcFdoExt).MINIPORT_InterruptService( HcFdoExt->Fdo.MiniportDeviceData);


    } WHILE (0);

    return usb_int;
}

/*

Routine: MINIPORT_InterruptDpcEx

ULONG
MINIPORT_InterruptDpcEx(
    PDEVICE_DATA DeviceData,
    BOOLEAN EnableInterrupts
    );

Description:

    Called by port in response to an interrupt generated by the HW.

    This function is not serialize with other core functions since which allow it to call services in USBPORT that
    result in immediate calls to core functions.

    The port driver passes a flag to indicate if interrupts need to be re-enabled on completion of this routine.
    This is to handle the rare case where interrupts have been disabled just after the ISR has queued a DPC.

Core_Lock - no.

HCI_Revison - 3+.

Irql - DPC_LEVEL

Parameters:

HcContext - Host Controller Context.

EnableInterrupts -  TRUE if interrupts should be enabled on return.

Returns:

     flags indicating source of irq.


*/

ULONG
MPf_InterruptDpcEx(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in BOOLEAN EnableInterrupts
    )
{
    ULONG flags = 0;

    do {


        if (MPF(HcFdoExt).MINIPORT_InterruptDpcEx == NULL) {
            break;
        }

        flags = MPF(HcFdoExt).MINIPORT_InterruptDpcEx(HcFdoExt->Fdo.MiniportDeviceData,
                                                      EnableInterrupts);


    } WHILE (0);

    return flags;
}

/*

Routine: MINIPORT_CheckController

VOID
MINIPORT_CheckController(
    PDEVICE_DATA DeviceData
    );

Description:

    Entry point called by usbport periodically to check the hardware state, this function is not serialized.
    Typically this is used to detect surprise removal of the hardware.

Core_Lock - no.

HCI_Revison - All.

Irql - DPC_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     none.


*/

VOID
MPf_CheckController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    KIRQL irql;

    do {

        if (MPF(HcFdoExt).MINIPORT_CheckController == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, irql);

        if (!HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {

            USBPORT_RELEASE_DM_LOCK(HcFdoExt, irql);

            MPF(HcFdoExt).MINIPORT_CheckController(HcFdoExt->Fdo.MiniportDeviceData);

        } else {
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, irql);
        }

    } WHILE (0);

}


/*

Routine: MINIPORT_InterruptNextSOF

VOID
MINIPORT_InterruptNextSOF(
    PDEVICE_DATA DeviceData
    );

Description:

    Requests a hardware interrupt at the next SOF interval.

Core_Lock - yes.

HCI_Revison - All.

Irql - DPC_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     none.


*/

VOID
MPf_InterruptNextSOF(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    KIRQL irql, dmirql;

    do {


        if (MPF(HcFdoExt).MINIPORT_InterruptNextSOF == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);


        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        //
        // If the controller is suspended or otherwise not running, then there won't be a next SOF,
        // and thus there won't be another interrupt
        //
        if ((!TEST_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_STARTED)) ||
             (TEST_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED)) ||
             (TEST_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_IN_RESET_RECOVERY)) ||
             HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

            //
            // This means that the interrupt won't fire, so only the DM Timer
            // can trigger the next operation, which happens to only be an
            // endpoint state change.  However, if the controller is in a
            // low power state, the DM Timer may have been disabled.
            // Instead, we'll just force the DM Timer DPC routine to run now
            //

            KeInsertQueueDpc(&HcFdoExt->Fdo.DM_SimulateTimerDpc,0,0);

            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        MPF(HcFdoExt).MINIPORT_InterruptNextSOF(HcFdoExt->Fdo.MiniportDeviceData);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}

/*

Routine: MINIPORT_SetWakeOnConnect

USB_MINIPORT_STATUS MINIPORT_SetWakeOnConnect(
    PDEVICE_DATA HcContext,
    ULONG EnableFlag
    );


Description:

    This routine will enable or disble wake on connect/disconnect events.

Core_Lock - no.

HCI_Revison - 3+.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

EnableFlag - 1 turns ON wake on connect change. 0 turns OFF wake on connect change.

Returns:

    success if feature is supported by hw.

*/

RHSTATUS
MPf_SetWakeOnConnect(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in ULONG EnableFlag
    )
{
    NTSTATUS nts = STATUS_NOT_SUPPORTED;
    RHSTATUS rhs;

    do {
        USB_MINIPORT_STATUS mps;

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_SetWakeOnConnect == NULL) {
            break;
        }

        mps = MPF(HcFdoExt).MINIPORT_SetWakeOnConnect(HcFdoExt->Fdo.MiniportDeviceData, EnableFlag);

        nts = USBPORT_MiniportStatus_TO_NtStatus(mps);

    } WHILE (0);

    if (NT_ERROR(nts)) {
        rhs = RH_STALL;
    } else {
        rhs = RH_SUCCESS;
    }

    return rhs;
}

/*

Routine: MINIPORT_PreStartController

USB_MINIPORT_STATUS MINIPORT_PreStartController(
    PDEVICE_DATA HcContext,
    PHC_RESOURCES HcResources
    );

Description:

    Pre start callback called by USBPORT in order for the miniport to query
    hardware capabilities (related to DMA, for example), with only the memory
    mapped resources being available at the time of the call.

    This is called once during the start process and not during restart.

Core_Lock - no.

HCI_Revison - all.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

HcResources - Pnp allocated memory resources only.

StartFlags - may be one of: MPF_RESTART, MPF_ON_PNP_THREAD

Returns:

    success if capabilities are queried successfully by the miniport.

*/

USB_MINIPORT_STATUS
MPf_PreStartController(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHC_RESOURCES HcResources,
    __in ULONG StartFlags
    )
{
    USB_MINIPORT_STATUS mps;

    ASSERT_PASSIVE();

    if ((MPx_MpRevision(HcFdoExt) >= USB_MINIPORT_HCI_VERSION_5) &&
        (MPF(HcFdoExt).MINIPORT_PreStartController != NULL)) {

        mps = MPF(HcFdoExt).MINIPORT_PreStartController(HcFdoExt->Fdo.MiniportDeviceData, HcResources);

    } else {

        mps = STATUS_NOT_IMPLEMENTED;
    }

    LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_PNP, 'mpPS', mps, 0, 0);
    return mps;
}

/*

Routine: MINIPORT_StartController

USB_MINIPORT_STATUS MINIPORT_StartController(
    PDEVICE_DATA HcContext,
    PHC_RESOURCES HcResources
    );

Description:

    Allow miniport to initialize hardware, allocate memory etc. On return (STATUS_SUCCESS) the device is considered
    started and powered and must handle ALL CORE functions. The miniport should disable any BIOS if present. The last
    thing the driver should do before returning is enable interrupt generation by the controller.

Core_Lock - no.

HCI_Revison - all.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

HcResources - Pnp allocated hardware and memory resources.

StartFlags - may be one of: MPF_RESTART, MPF_ON_PNP_THREAD

Returns:

    success if controller is started.

*/

USB_MINIPORT_STATUS
MPf_StartController(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHC_RESOURCES HcResources,
    __in ULONG StartFlags
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_FAILURE;
    ULONG i;

    ASSERT_PASSIVE();

    do {

        if (MPF(HcFdoExt).MINIPORT_StartController == NULL) {
            break;
        }

        HcResources->Restart = FALSE;
        if (StartFlags & MPF_RESTART) {
            // indicate to miniport this is a restart
            HcResources->Restart = TRUE;
        }

        if (StartFlags & MPF_ON_PNP_THREAD) {
            // indicate to miniport this is a restart
            HcFdoExt->Fdo.Flags.OnPnpThread = 1;
        }

        // Zero the miniport common buffer blocks.
        //
        if (HcResources->CommonBufferVa != NULL) {
            RtlSecureZeroMemory(HcResources->CommonBufferVa,
                                REGISTRATION_PACKET(HcFdoExt).CommonBufferBytes);
        }

        for (i = 0; i < HcResources->CommonBufferBlockCount; i++) {
            if (HcResources->CommonBufferBlocksVa[i] != NULL) {
                RtlSecureZeroMemory(HcResources->CommonBufferBlocksVa[i],
                                    REGISTRATION_PACKET(HcFdoExt).CommonBufferBlockBytes[i]);
            }
        }

        // Now call the miniport to start the host controller.
        //
        mps = MPF(HcFdoExt).MINIPORT_StartController(HcFdoExt->Fdo.MiniportDeviceData, HcResources);

    } WHILE (0);

    HcFdoExt->Fdo.Flags.OnPnpThread = 0;
    HcResources->Restart = FALSE;

    LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_PNP, 'mpST', mps, 0, 0);

    return mps;
}


/*

Routine: MINIPORT_StopController

VOID MINIPORT_StopController(
    PDEVICE_DATA HcContext,
    BOOLEAN HardwarePresent
    );

Description:

    Allow miniport to disconnect interrupt, free memory etc. On return
    (non Failable) the device is considered stopped and powered down and
    will no longer receive calls to CORE functions.

<pre>
    NOTES:

    - The miniport will only receive a stop request if it was
      successfuly started.

    Currently Called From:              At Level:
    +-----------------------------------+-------------------------------+
    | USBPORT_StopDevice()              | PASSIVE_LEVEL                 |
    +-----------------------------------+-------------------------------+
    | USBPORT_ResumeUsbController()     | PASSIVE_LEVEL                 |
    +-----------------------------------+-------------------------------+
    | USBPORT_TurnUsbControllerOff()    | DISPATCH_LEVEL (on chk krnl)  |
    +-----------------------------------+-------------------------------+

    MP_DisableInterrupts() is called before MPf_StopController() is
    called.

</pre>

Core_Lock - no.

HCI_Revison - all.

Parameters:

HcContext - Host Controller Context.

HardwarePresent - If Hardware Present is FALSE the miniport should not
                  access memory registers or ioports.

Returns:

    none.

*/

VOID
MPf_StopController(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in BOOLEAN HardwarePresent
    )
{
    do {

        if (MPF(HcFdoExt).MINIPORT_StopController == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        // Cancel all outstanding AsyncCallbacks before calling
        // MINIPORT_StopController().
        //
        USBPORT_CancelAllAsyncTimerCallbacks(HcFdoExt->HcFdoDeviceObject);

        MPF(HcFdoExt).MINIPORT_StopController(HcFdoExt->Fdo.MiniportDeviceData, HardwarePresent);

    } WHILE (0);

}


/*

Routine: MINIPORT_StopControllerWithInterrupts

VOID MINIPORT_StopControllerWithInterrupts(
    PDEVICE_DATA HcContext
    );

Description:

    Allow miniport to do whatever operations are needed to stop the controller
    while interrupts are enabled.  If the hardware isn't present, this function
    will not be called

    This was added for Wireless USB.

<pre>
    NOTES:
        - The miniport will only receive a stop request if it was successfuly started.
        - Miniport should disable all interrupts from the hardware.
</pre>

Core_Lock - no.

HCI_Revison - all.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.


Returns:

    none.

*/

VOID
MPf_StopControllerWithInterrupts(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_310) {
            break;
        }
        if (MPF(HcFdoExt).MINIPORT_StopControllerWithInterrupts == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        MPF(HcFdoExt).MINIPORT_StopControllerWithInterrupts(HcFdoExt->Fdo.MiniportDeviceData);

    } WHILE (0);

}

/*

Routine: MINIPORT_FlushInterrupts

VOID MINIPORT_FlushInterrupts(
    PDEVICE_DATA HcContext
    );


Description:

    Miniport should acknowlege any outstanding interrupts on the hardware.  This routine is called just prior to
    disconnecting the irq on unload or suspend.

Core_Lock - no. (serialized thru pnp)

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

    None

*/
VOID
MPf_FlushInterrupts(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_FlushInterrupts != NULL);

    if (MPF(HcFdoExt).MINIPORT_FlushInterrupts) {
        MPF(HcFdoExt).MINIPORT_FlushInterrupts(HcFdoExt->Fdo.MiniportDeviceData);
    }
}



/*

Routine: MINIPORT_QueryEndpointRequirements

VOID MINIPORT_QueryEndpointRequirements(
    PDEVICE_DATA DeviceData,
    PENDPOINT_PARAMETERS EndpointParameters,
    PENDPOINT_REQUIREMENTS EndpointRequirements
    );

Description:
Description:

    Allows miniport to indicate how much memory it will need to support a given set of transfers.

<pre>
The following applies if USBX flag is set:

Notes on parameters:

    EndpointParameters.MAXTRANSFERSIZE is largest single transfer this endpoint can handle. If a function driver passes
    down a larger transfer usbport will fail the transfer request.
    default =

    EndpointParameters.MINCOMMONBUFFERBYTES is the minimum size of the common buffer block allocated for this endpoint.

Control:
    This value must be 64k for control endpoints.
    default =

    EndpointParameters.MAXTRANSFERSIZE can never be larger than 64k by spec. The default value is 64k.

Interrupt:
Bulk:
    EndpointParameters.MAXTRANSFERSIZE can be any value. No transfer will be passed in larger than this value.
    default =

Isoch:
    EndpointParameters.MAXTRANSFERSIZE is the largest size of the buffer that encompasses all of the isochronous
    packets.
    default =

----------------------------------------------------------------------------------------------------------------------
The following applies if USBX flag is NOT set (for backward compatibilty with older miniports):

Notes on parameters:

    EndpointParameters.MAXTRANSFERSIZE is largest single transfer this endpoint can handle. If a function driver passes
    down a larger transfer usbport will fail the transfer request.

    EndpointParameters.MINCOMMONBUFFERBYTES is the minimum size of the common buffer block allocated for this endpoint.

Control:
    This value must be 64k for control endpoints.

    EndpointParameters.MAXTRANSFERSIZE can never be larger than 64k by spec. The default value is 64k.

Interrupt:
Bulk:
    EndpointParameters.MAXTRANSFERSIZE can be any value. No transfer will be passed in larger than this value.


Isoch:
    EndpointParameters.MAXTRANSFERSIZE is the largest size of the buffer that encompasses all of the isochronous
    packets.

</pre>

Core_Lock - no.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EndpointParameters - Describes the endpoints.

EndpointRequirements - filled in by miniport with the limits of what the miniport can handle.

Returns:

    none.

*/

VOID
MPf_QueryEndpointRequirements(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PENDPOINT_REQUIREMENTS Requirements
    )
{
    KIRQL irql;

    USBPORT_ASSERT(!USBPORT_IS_USBX(HcFdoExt));
    USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_QueryEndpointRequirements != NULL);

    USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
    USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);
    MPF(HcFdoExt).MINIPORT_QueryEndpointRequirements(HcFdoExt->Fdo.MiniportDeviceData,
                                                                                        &Endpoint->Parameters,
                                                                                        Requirements);
    USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
}

/*

Routine: MINIPORT_OpenEndpoint

USB_MINIPORT_STATUS MINIPORT_OpenEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_PARAMETERS EpParameters,
    PENDPOINT_DATA EpContext
    );


Description:

    Legacy (old style) open endpoint request.

Core_Lock - no.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpParameters - New parameters for the endpoint.

EpContext - Hc endpoint Context.

Returns:

    miniport status code for the opertaion.

*/
USB_MINIPORT_STATUS
MPf_OpenEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt USBD_STATUS *UsbdStatus
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;
    KIRQL irql = PASSIVE_LEVEL;
    ULONG mpOptionFlags;
    BOOLEAN passive;

    USBPORT_ASSERT(!USBPORT_IS_USBX(HcFdoExt));
    USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_OpenEndpoint != NULL);

    mpOptionFlags = REGISTRATION_PACKET(HcFdoExt).OptionFlags;
    passive = TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_EP_OPENCLOSE_PASSIVE);

    if (MPF(HcFdoExt).MINIPORT_OpenEndpoint) {

        //
        // Wireless USB needs Open and Close to be able to block, which means
        // we don't want to call with the spin lock held.  Synchronization is
        // handled by the endpoint state machine and the miniport.
        //
        if (!passive) {

            USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        }

        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        mps = MPF(HcFdoExt).MINIPORT_OpenEndpoint(HcFdoExt->Fdo.MiniportDeviceData,
                                                  &Endpoint->Parameters,
                                                  &Endpoint->MiniportEndpointData[0]);

        if (!passive) {

            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
        }

    }

    if (UsbdStatus) {
        *UsbdStatus = USBPORT_MiniportStatus_TO_USBDStatus(mps);
    }

    return mps;
}


/*

Routine: MINIPORT_DbgFreeEndpoint

VOID MINIPORT_DbgFreeEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext
    );


Description:

    (Optional) Called just prior to releasing endpoint memory.  This is used to sanity check that the endpoint is not
    still active in the hardware.


Core_Lock - yes.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

Returns:

    none


*/
VOID
MPf_DbgFreeEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    KIRQL irql;

    do {
        if (Endpoint->MpOpenRef == FALSE) {
            break;
        }
        Endpoint->MpOpenRef = FALSE;

        if (MPF(HcFdoExt).MINIPORT_DbgFreeEndpoint == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        MPF(HcFdoExt).MINIPORT_DbgFreeEndpoint(HcFdoExt->Fdo.MiniportDeviceData,
                                               &Endpoint->MiniportEndpointData[0]);

#if DBG
        // this will flush out cases where the miniport may still be accessing the endpoint after close/free
        RtlFillMemory(&Endpoint->MiniportEndpointData[0], Endpoint->MiniportEndpointDataSize, 'e');
#endif
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}


/*

Routine: MINIPORT_HaltController

VOID MINIPORT_HaltController(
    PDEVICE_DATA HcContext
    );


Description:

    Halts the controller. This prevents race conditions where the ep structs are freed while the HC is still accessing
    them.

Core_Lock - yes.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

    none


*/
VOID
MPf_HaltController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    KIRQL irql;

    do {

        if (MPF(HcFdoExt).MINIPORT_HaltController == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);
        MPF(HcFdoExt).MINIPORT_HaltController(HcFdoExt->Fdo.MiniportDeviceData);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}



/*

Routine: MINIPORT_PokeEndpoint

USB_MINIPORT_STATUS MINIPORT_PokeEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_PARAMETERS EpParameters,
    PENDPOINT_DATA EpContext
    );


Description:

    Legacy (old style) poke endpoint request. Used to change the address of an endpoint without removing it from
    the schedule.

Core_Lock - yes.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpParameters - New parameters for the endpoint.

EpContext - Hc endpoint Context.

Returns:

    miniport status code.

*/
USB_MINIPORT_STATUS
MPf_PokeEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;
    KIRQL irql;

    USBPORT_ASSERT(!USBPORT_IS_USBX(HcFdoExt));
    USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_PokeEndpoint != NULL);

    if (MPF(HcFdoExt).MINIPORT_PokeEndpoint) {
        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);
        mps = MPF(HcFdoExt).MINIPORT_PokeEndpoint(HcFdoExt->Fdo.MiniportDeviceData,
                                                    &Endpoint->Parameters,
                                                    &Endpoint->MiniportEndpointData[0]);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    }

    return mps;
}


/*

Routine: MINIPORT_CloseEndpoint

VOID MINIPORT_CloseEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext
    );


Description:

    Indicates an endpoint is closed to a miniport.  This function has existed since v1 of the interface, however,
    it was never called.  This routine is only called if the miniport specifies miniport v3 interface or higher.

Core_Lock - yes.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

Returns:

    none.

*/
VOID
MPf_CloseEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    KIRQL irql = PASSIVE_LEVEL; // keep the compiler happy
    ULONG mpOptionFlags;
    BOOLEAN passive;

    mpOptionFlags = REGISTRATION_PACKET(HcFdoExt).OptionFlags;
    passive = TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_EP_OPENCLOSE_PASSIVE);

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_CloseEndpoint != NULL);

        if (MPF(HcFdoExt).MINIPORT_CloseEndpoint == NULL) {
            break;
        }

        if (Endpoint->MpOpenRef == FALSE) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        //
        // Wireless USB needs Open and Close to be able to block, which means
        // we don't want to call with the spin lock held.  Synchronization is
        // handled by the endpoint state machine and the miniport.
        //

        if (!passive) {

            USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        }

        MPF(HcFdoExt).MINIPORT_CloseEndpoint(HcFdoExt->Fdo.MiniportDeviceData,
                                             &Endpoint->MiniportEndpointData[0]);

        if (!passive) {

            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
        }

    } WHILE (0);

}


/*

Routine: MINIPORT_SetEndpointState

VOID MINIPORT_SetEndpointState(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext,
    MP_ENDPOINT_STATE NewEpState
    );


Description:

    Set an endpoint to one of our defined transfer states, the endpoint need not be in the state when the miniport
    returns. For USB11 there is an assumption here that the enpoint will reach the desired state on the next SOF, the
    port will keep track of this and assume that the state transition has occurred after one ms frame has passed.
    If the USBX flag is set tate is transition is left to the miniport and may be asynchronous. The port driver will
    poll the miniport via MP_UsbxCheckHwSync.

<pre>

    These software state changes can only be intiated by a  request thru the MINIPORT_SetEndpointState function.
    Endpoints cannot transition on their own.

    ENDPOINT_IDLE
        The endpoint has no active transfer, set this endpoint to a state that generates minimal activity on the
        contoller (ie remove it from the schedule,set skip bit etc).

    ENDPOINT_PAUSE
        Temporarily stop any bus activity associated with the endpoint, this is a prelude to receiving an AbortTransfer.

    ENDPOINT_ACTIVE
        Enable processing of the enpoint -- ie it is in the schedule and ready for action.

    ENDPOINT_REMOVE
        The endpoint has been removed from the HW schedule.


Not all endpoint changes require an interrupt (hardware sync), by only waiting on those that need it we are able to
decrease the latency associated with openaing an isoch endpoint.


+----------------------+---------------+----------------+----------------+----------------+---------------+
     current/Next      | ENDPOINT_IDLE |ENDPOINT_PAUSE  |ENDPOINT_ACTIVE |ENDPOINT_REMOVE |ENDPOINT_CLOSED|
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_IDLE      |      x        |                |                |                |               |
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_PAUSE     |      i        |       x        |        i       |      sync      |      x        |
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_ACTIVE    |      x        |      sync      |        x       |      sync      |      x        |
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_REMOVE    |      x        |       x        |                |       x        |      i        |
+----------------------+---------------+----------------+----------------+----------------+---------------+
   *ENDPOINT_CLOSED    |      x        |       x        |        i       |       i        |      x        |
+----------------------+---------------+----------------+----------------+----------------+---------------+

*ENDPOINT_CLOSED is initial state
x       = invalid
sync    = hw sync(ie next sof)
i       = immediate

</pre>

Core_Lock - yes.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

NewEpState - new state for the endpoint.

Returns:

    none.

*/
VOID
MPf_SetEndpointState(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __in MP_ENDPOINT_STATE NewEpState
    )
{
    KIRQL irql;

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_SetEndpointState != NULL);

        if (MPF(HcFdoExt).MINIPORT_SetEndpointState == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        MPF(HcFdoExt).MINIPORT_SetEndpointState(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0],
                                                NewEpState);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}


/*

Routine: MINIPORT_GetEndpointState

MPf_ENDPOINT_STATE MINIPORT_GetEndpointState(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext
    );

Description:

    Returns the current hardware (miniport) state for the endpoint.

Core_Lock - yes.

HCI_Revison - All.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

Returns:

    endpoint state.

*/
MP_ENDPOINT_STATE
MPf_GetEndpointState(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    KIRQL irql, dmirql;
    MP_ENDPOINT_STATE mp_state = ENDPOINT_ERROR;

    ASSERT_ENDPOINT(Endpoint);

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_GetEndpointState != NULL);

        if (MPF(HcFdoExt).MINIPORT_GetEndpointState == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            mp_state = ENDPOINT_ERROR;
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        mp_state = MPF(HcFdoExt).MINIPORT_GetEndpointState(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0]);
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

    return mp_state;
}

/*

Routine: MINIPORT_GetEndpointStatus

MP_ENDPOINT_STATUS MINIPORT_GetEndpointStatus(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext
    );

Description:

    Sets the status of an endpoint (HALTED or RUN) to a specific value. Typically the status of the endpoint is
    controlled by the HW.

Core_Lock - yes.

HCI_Revison - All.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

Returns:

    endpoint status.

    ENDPOINT_STATUS_RUN  - endpoint and controller are running and being accessed by the HC hardware.
    ENDPOINT_STATUS_HALT - endpoint is in the halted state, not being accessed by hw
    ENDPOINT_STATUS_ERROR - endpoint or controller is in an error state
    ENDPOINT_STATUS_ERROR_HC_HALTED - hc hardware is in the halted state.

*/
MP_ENDPOINT_STATUS
MPf_GetEndpointStatus(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    KIRQL irql, dmirql;
    MP_ENDPOINT_STATUS mp_status = ENDPOINT_STATUS_ERROR;

    ASSERT_ENDPOINT(Endpoint);

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_GetEndpointStatus != NULL);

        if (MPF(HcFdoExt).MINIPORT_GetEndpointStatus == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        mp_status = MPF(HcFdoExt).MINIPORT_GetEndpointStatus(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0]);

        Endpoint->CurrentStatus = mp_status;
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

    return mp_status;
}


/*

Routine: MINIPORT_SetEndpointStatus

VOID MINIPORT_SetEndpointStatus(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext,
    MP_ENDPOINT_STATUS EpStatus
    );

Description:

    Sets the status of an endpoint (HALTED or RUN) to a specific value. Typically the status of the endpoint is
    controlled by the HW.

Core_Lock - yes.

HCI_Revison - All.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

EpStatus - new endpoint status

Returns:

    none.

*/
VOID
MPf_SetEndpointStatus(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __in MP_ENDPOINT_STATUS EpStatus
    )
{
    KIRQL irql, dmirql;

    ASSERT_ENDPOINT(Endpoint);

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_SetEndpointStatus != NULL);

        if (MPF(HcFdoExt).MINIPORT_SetEndpointStatus == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        Endpoint->CurrentStatus = EpStatus;
        MPF(HcFdoExt).MINIPORT_SetEndpointStatus(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0],
                                                EpStatus);
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}


/*

Routine: MINIPORT_SetEndpointDataToggle

VOID MINIPORT_SetEndpointDataToggle(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext,
    ULONG ToggleValue
    );

Description:

    Sets the USB data toggle for a given endpoint to the specified value (0 or 1).

Core_Lock - yes.

HCI_Revison - All.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

ToggleValue - new data toggle value 0 or 1.

Returns:

    none.

*/
VOID
MPf_SetEndpointDataToggle(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __in ULONG ToggleValue
    )
{
    KIRQL irql, dmirql;

    ASSERT_ENDPOINT(Endpoint);

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_SetEndpointDataToggle != NULL);

        if (MPF(HcFdoExt).MINIPORT_SetEndpointDataToggle == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        if (ToggleValue > 1) {
            ToggleValue = 1;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        MPF(HcFdoExt).MINIPORT_SetEndpointDataToggle(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0],
                                                ToggleValue);
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}

/*

Routine: MINIPORT_PollEndpoint

VOID MINIPORT_PollEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext
    );

Description:

     Poll the endpoint for done transfers or other changes.

HCI_Revison - All.

Core_Lock - yes.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

Returns:

    none.

*/
VOID
MPf_PollEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    KIRQL irql, dmirql;

    ASSERT_ENDPOINT(Endpoint);

    do {

        if (Endpoint->EpFlags.RootHub) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_PollEndpoint != NULL);

        if (MPF(HcFdoExt).MINIPORT_PollEndpoint == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        MPF(HcFdoExt).MINIPORT_PollEndpoint(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0]);
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}


/*

Routine: MINIPORT_ResetController

VOID MINIPORT_ResetController(
    PDEVICE_DATA HcContext
    );

Description:

     Reset the host controller hardware (slegehammer reset).

     This function is serialized with the ISR and DPC and holds the core function lock in order to provide a
     safe environment for the miniport to reset the host hardware and rebuild the schedule.

Core_Lock - yes.

HCI_Revison - All.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

    none.

*/
VOID
MPf_ResetController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    KIRQL irql;

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_ResetController != NULL);

        if (MPF(HcFdoExt).MINIPORT_ResetController == NULL) {
            break;
        }

        LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_MISC, 'rset',
            HcFdoExt->HcFdoDeviceObject, HcFdoExt->Fdo.Flags.ul, 0);
        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_ResetController(
                                                HcFdoExt->Fdo.MiniportDeviceData);
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}

/*

Routine: MINIPORT_ResetControllerDontReinit

VOID MINIPORT_ResetControllerDontReinit(
    PDEVICE_DATA HcContext
    );

Description:

     Reset the host controller hardware (slegehammer reset), however dont reinitalize it.

     This function is serialized with the ISR and DPC and holds the core function lock in order to provide a
     safe environment for the miniport to reset the host hardware and rebuild the schedule.

Core_Lock - yes.

HCI_Revison - 3.10 +

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

    none.

*/
VOID
MPf_ResetControllerDontReinit(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    KIRQL irql;

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_ResetControllerDontReinit != NULL);

        if (MPF(HcFdoExt).MINIPORT_ResetControllerDontReinit == NULL) {
            break;
        }

        LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_MISC, 'Rset',
            HcFdoExt->HcFdoDeviceObject, HcFdoExt->Fdo.Flags.ul, 0);
        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_ResetControllerDontReinit(
                                                HcFdoExt->Fdo.MiniportDeviceData);
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}

/*

Routine: MINIPORT_PostResetController

VOID MINIPORT_ResetController(
    PDEVICE_DATA HcContext
    );

Description:

     A call into miniport at Passive Level after the controller has been
     reset (slegehammer reset) but before it has been reinialized.

Core_Lock - NO.

HCI_Revison - 3.10 +

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

    none.

*/
VOID
MPf_PostResetController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_PostResetController != NULL);

        if (MPF(HcFdoExt).MINIPORT_PostResetController == NULL) {
            break;
        }

        LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_MISC, 'Rpos',
            HcFdoExt->HcFdoDeviceObject, HcFdoExt->Fdo.Flags.ul, 0);

        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_PostResetController(
                                                HcFdoExt->Fdo.MiniportDeviceData);

    } WHILE (0);

}

/*

Routine: MINIPORT_ReinitController

VOID MINIPORT_ReinitController(
    PDEVICE_DATA HcContext
    );

Description:

     Reinitalize the controller after the host controller hardware has been reset (slegehammer reset)

     This function is serialized with the ISR and DPC and holds the core function lock in order to provide a
     safe environment for the miniport to reset the host hardware and rebuild the schedule.

Core_Lock - yes.

HCI_Revison - 3.10 +

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

    none.

*/
VOID
MPf_ReinitController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    KIRQL irql;

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_ReinitController != NULL);

        if (MPF(HcFdoExt).MINIPORT_ReinitController == NULL) {
            break;
        }

        LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_MISC, 'Rint',
            HcFdoExt->HcFdoDeviceObject, HcFdoExt->Fdo.Flags.ul, 0);
        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_ReinitController(
                                                HcFdoExt->Fdo.MiniportDeviceData);
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}


/*

Routine: MINIPORT_AbortTransfer

VOID
MINIPORT_AbortTransfer(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext,
    PTRANSFER_CONTEXT TransferContext,
    PULONG BytesTransferred
    );


Description:

    Abort a specific transfer that has been started, this will only be called if the endpoint is in the ENDPOINT_PAUSED
    state.  This call is NOT fail-able and the HW must have no reference to the transfer on return.  The miniport does
    not indicate completion on transfer by calling any USBPORT services.

    *There is one exception added near for Vista RTM. We will peremit a call to USBPORT_SVC_CompleteTransfer as long as
    its NOT the current transfer being aborted.

Core_Lock - yes.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

TxContext - Hc Transfer Context for a mpped transfer.

BytesTransferred - pointer filled in with the count of bytes successfully transferred before the endpoint was paused.

Returns:

    none.

*/
VOID
MPf_AbortTransfer(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __inout PULONG BytesTransferred
    )
{
    KIRQL irql;

    do {


        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_AbortTransfer != NULL);

        if (MPF(HcFdoExt).MINIPORT_AbortTransfer == NULL) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->CurrentAbortTransfer == NULL);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);
        HcFdoExt->CurrentAbortTransfer = Transfer;
        MPF(HcFdoExt).MINIPORT_AbortTransfer(
                               HcFdoExt->Fdo.MiniportDeviceData,
                               &Endpoint->MiniportEndpointData[0],
                               Transfer->MiniportContext,
                               BytesTransferred);

        HcFdoExt->CurrentAbortTransfer = NULL;
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}


/*

Routine: MINIPORT_SubmitTransfer

USB_MINIPORT_STATUS
MINIPORT_SubmitTransfer(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext,
    PTRANSFER_PARAMETERS TransferParameters,
    PTRANSFER_CONTEXT TransferContext,
    PTRANSFER_SG_LIST TransferSGList
    );


Description:

    Request the miniport to program a USB async transfer (bulk, interrupt or control) to the hardware. If no hw
    resources are avaiable to program the transfer then the miniport should return USBMP_STATUS_BUSY and the request
    will be tried later. If the transfer is successfully queued to the HW then the miniport returns
    USBMP_STATUS_SUCCESS.  At the time this routine is called the transfer buffer has been mapped
    (ie no need to call IoMapTransfer).

Core_Lock - yes.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

TransferParameters - USB parameters for this transfer.

TxContext - Hc Transfer Context for a mpped transfer.

TransferSGList - contains a list of physical addresses for transfer buffer.

Returns:

    USBMP_STATUS_BUSY or USBMP_STATUS_SUCCESS.

*/
USB_MINIPORT_STATUS
MPf_SubmitTransfer(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
{
    KIRQL irql;
    USB_MINIPORT_STATUS mps;

    do {

        mps = USBMP_STATUS_FAILURE;

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_SubmitTransfer != NULL);

        if (MPF(HcFdoExt).MINIPORT_SubmitTransfer == NULL) {
            break;
        }

        USBPORT_ASSERT(Endpoint->GepState != GEp_Zapped);
        if (Endpoint->GepState == GEp_Zapped) {
            // miniport cannot reference zapped enpoint
            mps = USBMP_STATUS_FAILURE;
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        mps = MPF(HcFdoExt).MINIPORT_SubmitTransfer(
                               HcFdoExt->Fdo.MiniportDeviceData,
                               &Endpoint->MiniportEndpointData[0],
                               &Transfer->Tp,
                               Transfer->MiniportContext,
                               &Transfer->SgList);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

    return mps;
}

/*

Routine: MINIPORT_SubmitIsoTransfer

USB_MINIPORT_STATUS
MINIPORT_SubmitIsoTransfer(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext,
    PTRANSFER_PARAMETERS TransferParameters,
    PTRANSFER_CONTEXT TransferContext,
    PMINIPORT_ISO_TRANSFER IsochTransfer
    );


Description:

    Request the miniport to program a USB isochronous transferto the hardware.

Core_Lock - yes.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

TransferParameters - USB parameters for this transfer.

TxContext - Hc Transfer Context for a mpped transfer.

IsochTransfer - contains a list of isoch packets and physical addresses for transfer.

Returns:

    USBMP_STATUS_BUSY or USBMP_STATUS_SUCCESS.

*/
USB_MINIPORT_STATUS
MPf_SubmitIsochTransfer(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
{
    KIRQL irql;
    USB_MINIPORT_STATUS mps;

    do {

        mps = USBMP_STATUS_FAILURE;

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_SubmitIsoTransfer != NULL);

        if (MPF(HcFdoExt).MINIPORT_SubmitIsoTransfer == NULL) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        mps = MPF(HcFdoExt).MINIPORT_SubmitIsoTransfer(
                               HcFdoExt->Fdo.MiniportDeviceData,
                               &Endpoint->MiniportEndpointData[0],
                               &Transfer->Tp,
                               Transfer->MiniportContext,
                               Transfer->IsoTransfer);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

    return mps;
}


/*

Routine: MINIPORT_CreateDeviceData

VOID
MINIPORT_CreateDeviceData(
    PDEVICE_DATA HcContext
    );


Description:

    Notify miniport of pnp ADD.

Core_Lock - no.

HCI_Revison - 3.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.


*/
VOID
MPf_CreateDeviceData(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    do {

        //USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_CreateDeviceData != NULL);

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_CreateDeviceData == NULL) {
            break;
        }
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_CreateDeviceData(HcFdoExt->Fdo.MiniportDeviceData);


    } WHILE (0);


}


/*
Routine: MINIPORT_DeleteDeviceData

VOID
MINIPORT_DeleteDeviceData(
    PDEVICE_DATA HcContext
    );


Description:

    Notify miniport of pnp REMOVE.

Core_Lock - no.

HCI_Revison - 3.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.


*/
VOID
MPf_DeleteDeviceData(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_DeleteDeviceData == NULL) {
            break;
        }


        MPF(HcFdoExt).MINIPORT_DeleteDeviceData(HcFdoExt->Fdo.MiniportDeviceData);


    } WHILE (0);


}


/*

Routine: MINIPORT_OpenDevHandle

VOID
MINIPORT_OpenDevHandle(
    PDEVICE_DATA HcContext,
    PDEVH_CONTEXT DevhContext
    );


Description:

    Notify miniport of USB function arrival.

Core_Lock - no.

HCI_Revison - 3.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

DevhContext - USB function driver context

*/
VOID
MPf_OpenDevHandle(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PUSBD_DEVICE_HANDLE DevH
    )
{
    do {


        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_OpenDevHandle == NULL) {
            break;
        }
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_OpenDevHandle(HcFdoExt->Fdo.MiniportDeviceData,
                                             DevH->MiniportContext);


    } WHILE (0);


}

/*

Routine: MINIPORT_CloseDevHandle

VOID
MINIPORT_CloseDevHandle(
    PDEVICE_DATA HcContext,
    PDEVH_CONTEXT DevhContext
    );


Description:

    Notify miniport of USB function removal.

Core_Lock - no.

HCI_Revison - 3.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

DevhContext - USB function driver context

*/
VOID
MPf_CloseDevHandle(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PUSBD_DEVICE_HANDLE DevH
    )
{
    do {


        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_CloseDevHandle == NULL) {
            break;
        }


        MPF(HcFdoExt).MINIPORT_CloseDevHandle(HcFdoExt->Fdo.MiniportDeviceData,
                                             DevH->MiniportContext);


    } WHILE (0);


}

/*

Routine: MINIPORT_RebalanceEndpoint

VOID
MINIPORT_RebalanceEndpoint(
    PDEVICE_DATA HcContext,
    PHCD_ENDPOINT EpContext
    );


Description:

    Rebalance a USB 2.0 endpoint. This routine is used to notify the host controller that tha an endpoints
    schedule parameters have changed

Core_Lock - yes.

HCI_Revison - 3.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - miniport context for endpoint

*/
VOID
MPf_RebalanceEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    KIRQL irql;

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_RebalanceEndpoint == NULL) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        MPF(HcFdoExt).MINIPORT_RebalanceEndpoint(HcFdoExt->Fdo.MiniportDeviceData,
                                                 &Endpoint->Parameters,
                                                 &Endpoint->MiniportEndpointData[0]);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);



    } WHILE (0);


}


/**********************************************************************************************************************
    These routines dispatches to the appropriate function (USBX or legacy) based on miniport flags
***********************************************************************************************************************/

VOID
MPx_QueryEndpointRequirements(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __out   PENDPOINT_REQUIREMENTS Requirements
    )
{
    if (USBPORT_IS_USBX(HcFdoExt)) {
        TEST_TRAP();
        // newer version with perf tweaks
        //MP_UsbxQueryEndpointRequirements(HcFdoExt, Endpoint, Requirements);
    } else {
        MPf_QueryEndpointRequirements(HcFdoExt, Endpoint, Requirements);
    }
}


USB_MINIPORT_STATUS
MPx_OpenEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt USBD_STATUS *UsbdStatus
    )
{
    USB_MINIPORT_STATUS mps;

    if (USBPORT_IS_USBX(HcFdoExt)) {
        TEST_TRAP();
        mps = MPf_UsbxOpenEndpoint(HcFdoExt, Endpoint, Endpoint->Tt, UsbdStatus);
    } else {
        mps = MPf_OpenEndpoint(HcFdoExt, Endpoint, UsbdStatus);
    }

    // if endpoint open succeeds the miniport has a reference to the endpoint that must be closed/freed
    // we keep track of this with a single flag.

    if (mps == USBMP_STATUS_SUCCESS) {
        Endpoint->MpOpenRef = TRUE;
    }

    return mps;
}


USB_MINIPORT_STATUS
MPx_RebalanceEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_SUCCESS;

    if (USBPORT_IS_USBX(HcFdoExt)) {
        TEST_TRAP();
        //mps = MPf_UsbxRebalanceEndpoint(HcFdoExt, Endpoint, Endpoint->Tt, UsbdStatus);
        mps = USBMP_STATUS_NOT_SUPPORTED;
    } else {
        MPf_RebalanceEndpoint(HcFdoExt, Endpoint);
    }

    return mps;
}


USB_MINIPORT_STATUS
MPx_PokeEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR Tt
    )
{
    USB_MINIPORT_STATUS mps;

    if (USBPORT_IS_USBX(HcFdoExt)) {
        TEST_TRAP();
        mps = MPf_UsbxPokeEndpoint(HcFdoExt, Endpoint, Tt);
    } else {
        mps = MPf_PokeEndpoint(HcFdoExt, Endpoint);
    }

    return mps;
}


VOID
MPx_InitializeHsb(
    VOID
    )
{
#ifdef USB2_INTERNAL
    USB2LIB_InitializeLibEx(&USB2LIB_HcContextSize,
                            &USB2LIB_EndpointContextSize,
                            &USB2LIB_TtContextSize);
#endif
}


ULONG
MPx_MpRevision(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{

    ULONG rev;

    rev = HcFdoExt->Fdo.MiniportDriver->HciVersion;

    return rev;
}


ULONG
MPx_HsbTtContextSize(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    ULONG n = 0;

    if (USBPORT_IS_USBX(HcFdoExt)) {
        n = MPF(HcFdoExt).HsbTtContextSize;
    } else {
#ifdef USB2_INTERNAL
        n = USB2LIB_TtContextSize;
#endif
    }

    return n;
}

ULONG
MPx_HsbControllerContextSize(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    ULONG n = 0;

    if (USBPORT_IS_USBX(HcFdoExt)) {
        n = MPF(HcFdoExt).HsbContextSize;
    } else {
#ifdef USB2_INTERNAL
        n = USB2LIB_HcContextSize;
#endif
    }

    return n;
}


ULONG
MPx_HsbEndpointContextSize(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    ULONG n = 0;

    if (USBPORT_IS_USBX(HcFdoExt)) {
        n = MPF(HcFdoExt).HsbEpContextSize;
    } else {
#ifdef USB2_INTERNAL
        n = USB2LIB_EndpointContextSize;
#endif
    }

    return n;

}



USB_MINIPORT_STATUS
MPx_SubmitTransfer(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
{
    USB_MINIPORT_STATUS mps;

    if (USBPORT_IS_USBX(HcFdoExt)) {
        TEST_TRAP();
//        mps = MPf_UsbxSubmitTransfer(HcFdoExt, Endpoint, Transfer);
        mps = USBMP_STATUS_NOT_SUPPORTED;
    } else {

        if (Endpoint->LastTransferSequenceNumberAccepted != 0) {
            //
            // Assert that the sequence number of the transfer that is
            // currently being submitted to the miniport is greater than
            // the sequence number of the last transfer accepted by the
            // miniport.
            //
            USBPORT_ASSERT(Transfer->Tp.SequenceNumber -
                           Endpoint->LastTransferSequenceNumberAccepted <
                           MAXLONG);
        }

        if (TEST_FLAG(Transfer->Flags, USBPORT_TXFLAG_ISO)) {
            mps = MPf_SubmitIsochTransfer(HcFdoExt, Endpoint, Transfer);
        } else {
            mps = MPf_SubmitTransfer(HcFdoExt, Endpoint, Transfer);
        }

        if (mps == USBMP_STATUS_SUCCESS) {
            //
            // Remember the sequence number of the last transfer
            // accepted by the miniport.
            //
            Endpoint->LastTransferSequenceNumberAccepted =
                Transfer->Tp.SequenceNumber;
        }
    }

    return mps;
}

/*
    Initializes an instance of a high speed endpoint for HS bugdeter.
*/
NTSTATUS
MPx_InitializeHsbEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR TransactionTranslator
    )
{
    NTSTATUS nts;
    ULONG nBytes;

    Endpoint->HsbEpContextPtr = NULL;

    if (!USBPORT_IS_USB20(HcFdoExt)) {
        return STATUS_SUCCESS;
    }

    nBytes = MPx_HsbEndpointContextSize(HcFdoExt);

    if (nBytes) {
        nts = UsbAllocPool_Z(&Endpoint->HsbEpContextPtr, NonPagedPoolNx, nBytes);
    } else {
        nts = STATUS_INVALID_PARAMETER;
    }

    if (NT_SUCCESS(nts)) {

        if (USBPORT_IS_USBX(HcFdoExt)) {

            USB_MINIPORT_STATUS mps;

            TEST_TRAP();
            // callout to miniport
            if (NT_SUCCESS(nts)) {
                mps = MPf_UsbxInitHsbEndpoint(HcFdoExt, Endpoint, TransactionTranslator);

                nts = USBPORT_MiniportStatus_TO_NtStatus(mps);
            }
        } else {

#ifdef USB2_INTERNAL

            nts = STATUS_SUCCESS;
#else
            nts = STATUS_NOT_SUPPORTED;
#endif
        }
    }
    return nts;
}



/*
    Initializes an instance of a high speed TT for HS bugdeter.
*/
NTSTATUS
MPx_InitializeHsbTt(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PTRANSACTION_TRANSLATOR TransactionTranslator
    )
{
    NTSTATUS nts;
    ULONG nBytes;

    TransactionTranslator->HsbTtContextPtr = NULL;

    if (!USBPORT_IS_USB20(HcFdoExt)) {
        return STATUS_SUCCESS;
    }

    nBytes = MPx_HsbTtContextSize(HcFdoExt);

    if (nBytes) {
        nts = UsbAllocPool_Z(&TransactionTranslator->HsbTtContextPtr, NonPagedPoolNx, nBytes);
    } else {
        nts = STATUS_INVALID_PARAMETER;
    }

    if (NT_SUCCESS(nts)) {
        if (USBPORT_IS_USBX(HcFdoExt)) {
            USB_MINIPORT_STATUS mps;

            TEST_TRAP();
            mps = MPf_UsbxInitHsbTransactionTranslator(HcFdoExt, TransactionTranslator);

            nts = USBPORT_MiniportStatus_TO_NtStatus(mps);
        } else {

#ifdef USB2_INTERNAL
            USB2LIB_InitTt(HcFdoExt->Fdo.HsbControllerContextPtr,  TransactionTranslator->HsbTtContextPtr);

            nts = STATUS_SUCCESS;
#else
            nts = STATUS_NOT_SUPPORTED;
#endif

        }
    }
    return nts;
}

/*
    Initializes an instance of a high speed USB controller from Add Device.
*/
NTSTATUS
MPx_InitializeHsbController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    NTSTATUS nts;
    ULONG flags;
    USB_MINIPORT_STATUS mps;
    ULONG nBytes;

//#define USB_MINIPORT_OPT_USB20                0x00000010
//#define USB_MINIPORT_OPT_USBX                 0x00000400

    HcFdoExt->Fdo.HsbControllerContextPtr = NULL;

    if (!USBPORT_IS_USB20(HcFdoExt)) {
        // only allowed for USB20
        return STATUS_SUCCESS;
    }

    flags = MPF(HcFdoExt).OptionFlags & 0x00000410;

    // allocate this block dynamically
    nBytes = MPx_HsbControllerContextSize(HcFdoExt);
    if (nBytes) {
        nts = UsbAllocPool_Z(&HcFdoExt->Fdo.HsbControllerContextPtr, NonPagedPoolNx, nBytes);
    } else {
        nts = STATUS_INVALID_PARAMETER;
    }

    if (NT_SUCCESS(nts)) {
        switch(flags) {
        // USB2 only
        case 0x00000010:
#ifdef USB2_INTERNAL

            USB2LIB_InitController(HcFdoExt->Fdo.HsbControllerContextPtr);

            nts = STATUS_SUCCESS;
#else
            nts = STATUS_NOT_SUPPORTED;
#endif
            break;

        // USBX + USB2
        case 0x00000410:

            mps = MPf_UsbxInitHsbController(HcFdoExt);

            nts = USBPORT_MiniportStatus_TO_NtStatus(mps);
            break;

        default:
            nts = STATUS_INVALID_PARAMETER;

        }
    }
    return nts;
}

ULONG
MPx_QueryTtBandwidthData(
    PDEVICE_EXTENSION HcFdoExt,
    PTRANSACTION_TRANSLATOR Tt,
    ULONG Frame
    )
{
/*
    Returns consumed bandwidth for frame on a TT
*/

    ULONG bw = 0;

    if (!USBPORT_IS_USB20(HcFdoExt)) {
        return 0;
    }

    if (USBPORT_IS_USBX(HcFdoExt)) {
        bw = MPf_UsbxQueryTtBandwidthData(HcFdoExt, Tt, Frame);
    } else {
#ifdef USB2_INTERNAL
        bw = USB2LIB_GetTt_time_used(Tt->HsbTtContextPtr, Frame);
#endif
    }


    return bw;
}


ULONG
MPx_QueryBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in ULONG Frame,
    __in ULONG Microframe
    )
{
/*
    Returns consumed bandwidth for frame, microframe on a high speed controller
*/

    ULONG bw = 0;

    if (!USBPORT_IS_USB20(HcFdoExt)) {
        return 0;
    }

   if (USBPORT_IS_USBX(HcFdoExt)) {
        bw = MPf_UsbxQueryBandwidthData(HcFdoExt, Frame, Microframe);
    } else {
#ifdef USB2_INTERNAL
        bw = USB2LIB_Get_bus_time(HcFdoExt->Fdo.HsbControllerContextPtr, Frame, Microframe);
#endif
    }


    return bw;
}


ULONG
MPx_QueryEpBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
/*
    Returns consumed bandwidth for a given endpoint
*/

    ULONG bw = 0;
    ULONG flags;
//USB_MINIPORT_OPT_USB11                0x00000008
//USB_MINIPORT_OPT_USB20                0x00000010
//USB_MINIPORT_OPT_USBX                 0x00000400

    flags = (REGISTRATION_PACKET(HcFdoExt).OptionFlags & 0x00000418);

    switch(flags) {
    // USB11
    case 0x00000008:
        // internal (legacy mode)
        bw = USBPORT_CalculateUsbBandwidthEp(HcFdoExt->HcFdoDeviceObject, Endpoint);
        break;
    // USBX
    case 0x00000400:
    // USBX, USB20
    case 0x00000410:
    // USBX, USB11
    case 0x00000408:
        // query thru miniport
        bw = MPf_UsbxQueryEpBandwidthData(HcFdoExt, Endpoint->Tt, Endpoint);
        break;

    // USB20
    case 0x00000010:

#ifdef USB2_INTERNAL
        // query thru old budgeter code
        bw = USB2LIB_Get_Ep_bus_time(Endpoint->HsbEpContextPtr);
#endif
        break;

    default:
        bw = 0;
    }


    return bw;
}

BOOLEAN
MPx_AllocateBandwidth(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    BOOLEAN gotBw;
    ULONG flags;
//USB_MINIPORT_OPT_USB11                0x00000008
//USB_MINIPORT_OPT_USB20                0x00000010
//USB_MINIPORT_OPT_USBX                 0x00000400

    flags = (REGISTRATION_PACKET(HcFdoExt).OptionFlags & 0x00000418);

    switch(flags) {
    case 0x00000000:

    //USB11
    case 0x00000008:

        // Legacy USB 1.1 bw allocation
        Endpoint->Parameters.Bandwidth =
            USBPORT_CalculateUsbBandwidthEp(HcFdoExt->HcFdoDeviceObject, Endpoint);

        // caclualte the best schedule position
        gotBw = USBPORT_AllocateBandwidthUSB11(HcFdoExt->HcFdoDeviceObject, Endpoint);
        if (gotBw) {
            Endpoint->EpFlags.USB11_Bandwidth = 1;
        }
        break;

    // USBX (wireless case?)
    case 0x00000400:
    // USB11 USBX (wireless case?)
    case 0x00000408:
    // USBX USB20
    case 0x00000410:
        {
        // USBX interface allocates thru miniport
        USB_MINIPORT_STATUS mpStatus;
        PTRANSACTION_TRANSLATOR tt;

        tt = Endpoint->Tt;
        if (tt) {
            ASSERT_TT(tt);
        }
        // call the miniport and attempt to allocate the necessary bus time for this endoint
        mpStatus = MPf_UsbxAllocateBandwidth(HcFdoExt, Endpoint, tt);

        if (mpStatus == USBMP_STATUS_SUCCESS) {
            gotBw = TRUE;
            Endpoint->EpFlags.USBX_Bandwidth = 1;
        } else {
            gotBw = FALSE;
        }
        }
        break;

    //USB20 - use old budgeter code
    case  0x00000010:
        gotBw = FALSE;
#ifdef USB2_INTERNAL
        {
        NTSTATUS nts;

        nts = USBPORT_AllocateBandwidthUSB20(HcFdoExt->HcFdoDeviceObject, Endpoint);
        if (NT_SUCCESS(nts)) {

            gotBw = TRUE;
        }

        }
#endif
        break;

    default:
        gotBw = FALSE;

    }

    return gotBw;
}


VOID
MPx_FreeBandwidth(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    ULONG flags;
//USB_MINIPORT_OPT_USB11                0x00000008
//USB_MINIPORT_OPT_USB20                0x00000010
//USB_MINIPORT_OPT_USBX                 0x00000400

    flags = (REGISTRATION_PACKET(HcFdoExt).OptionFlags & 0x00000418);

    switch(flags) {
    case 0x00000000:

    //USB11
    case 0x00000008:
        if (Endpoint->EpFlags.USB11_Bandwidth) {

            // caclualte the best schedule position
            USBPORT_FreeBandwidthUSB11(HcFdoExt->HcFdoDeviceObject, Endpoint);
        }

        break;

     //USBX (wireless case?)
    case 0x00000400:
    // USB11 USBX (wireless case?)
    case 0x00000408:
    // USBX USB20
    case 0x00000410:
        {
        // USBX interface frees thru miniport
        PTRANSACTION_TRANSLATOR tt;

        tt = Endpoint->Tt;
        if (tt) {
            ASSERT_TT(tt);
        }

        MPf_UsbxFreeBandwidth(HcFdoExt, Endpoint, tt);
        }
        break;

    //USB20 - use old budgeter code
    case  0x00000010:

#ifdef USB2_INTERNAL
        USBPORT_FreeBandwidthUSB20(HcFdoExt->HcFdoDeviceObject, Endpoint);
#endif
        break;

    default:
        break;
    }

    return ;
}

NTSTATUS
MPf_GetContainerIdForPort(
    __in PDEVICE_EXTENSION HcFdoExt,
    __in USHORT PortNumber,
    __inout LPGUID ContainerId
    )
{
    USB_MINIPORT_STATUS usbMpStatus;


    if ( (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_310) ||
         (MPF(HcFdoExt).MINIPORT_RH_GetContainerIdForPort == NULL) ){
            return STATUS_NOT_IMPLEMENTED;
    }

    usbMpStatus = MPF(HcFdoExt).MINIPORT_RH_GetContainerIdForPort(HcFdoExt->Fdo.MiniportDeviceData,
                                                    PortNumber,
                                                    ContainerId);

    return ((usbMpStatus == USBMP_STATUS_SUCCESS) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL);
}

VOID
MPf_SetContainerIdForPort(
    __in PDEVICE_EXTENSION HcFdoExt,
    __in USHORT PortNumber,
    __in LPGUID ContainerId
    )
{
    if ( (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_310) ||
         (MPF(HcFdoExt).MINIPORT_RH_SetContainerIdForPort == NULL) ){
            return;
    }

    MPF(HcFdoExt).MINIPORT_RH_SetContainerIdForPort(HcFdoExt->Fdo.MiniportDeviceData,
                                                    PortNumber,
                                                    ContainerId);
}


NTSTATUS
MPf_GetHcCapabilities(
    __inout PDEVICE_EXTENSION   HcFdoExt
    )
{
    MINIPORT_HC_CAPABILITIES_V2 hcCapabilitiesV2;
    USB_MINIPORT_STATUS         usbMpStatus;

    if ( (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_310) ||
         (MPF(HcFdoExt).MINIPORT_GetHcCapabilities == NULL) ){
            return STATUS_NOT_IMPLEMENTED;
    }

    hcCapabilitiesV2.Size = sizeof(MINIPORT_HC_CAPABILITIES_V2);

    usbMpStatus = MPF(HcFdoExt).MINIPORT_GetHcCapabilities(
        HcFdoExt->Fdo.MiniportDeviceData,
        (PMINIPORT_HC_CAPABILITIES)&hcCapabilitiesV2);

    if (usbMpStatus == USBMP_STATUS_SUCCESS) {
        if (hcCapabilitiesV2.Size >= sizeof(MINIPORT_HC_CAPABILITIES_V1) &&
            hcCapabilitiesV2.Version >= MINIPORT_HC_CAPABILITIES_VERSION1) {
            HcFdoExt->Fdo.Bit64AddressingCapability =
                hcCapabilitiesV2.Bit64AddressingCapability ? 1 : 0;
        }

        if (hcCapabilitiesV2.Size >= sizeof(MINIPORT_HC_CAPABILITIES_V2) &&
            hcCapabilitiesV2.Version >= MINIPORT_HC_CAPABILITIES_VERSION2) {
            HcFdoExt->Fdo.RootHubTTCapable =
                hcCapabilitiesV2.IntegratedTTSupported;
        } else {
            HcFdoExt->Fdo.RootHubTTCapable = FALSE;
        }



        return STATUS_SUCCESS;

    } else {

        HcFdoExt->Fdo.Bit64AddressingCapability = 0;
        HcFdoExt->Fdo.RootHubTTCapable = FALSE;

        return STATUS_UNSUCCESSFUL;
    }
}


USHORT
MPf_GetDebugPortNumber(
    __inout PDEVICE_EXTENSION   HcFdoExt
    )
{

    if ( (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_310) ||
         (MPF(HcFdoExt).MINIPORT_GetDebugPortNumber == NULL) ){
            return 0;
    }

    return MPF(HcFdoExt).MINIPORT_GetDebugPortNumber(
        HcFdoExt->Fdo.MiniportDeviceData);
}


BOOLEAN
MPf_CheckInvalidateRootHub(
    __inout PDEVICE_EXTENSION   HcFdoExt
    )
{

    if ( (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_310) ||
         (MPF(HcFdoExt).MINIPORT_CheckInvalidateRootHub == NULL) ){
            return FALSE;
    }

    return MPF(HcFdoExt).MINIPORT_CheckInvalidateRootHub(
        HcFdoExt->Fdo.MiniportDeviceData);
}


VOID
MPf_SetClearPortResumeTime(
    __inout PDEVICE_EXTENSION   HcFdoExt,
    __in USHORT PortNumber,
    __in BOOLEAN SetTime,
    __in UCHAR ResumeTime
    )
{
    KIRQL irql;

    if ( (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_310) ||
         (MPF(HcFdoExt).MINIPORT_SetPortResumeTime == NULL) ){
            return;
    }

    USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

    MPF(HcFdoExt).MINIPORT_SetPortResumeTime(HcFdoExt->Fdo.MiniportDeviceData,
                                             PortNumber,
                                             SetTime,
                                             ResumeTime);

    USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
}

/*********************************************************************************************************************/
/* USBPORT service documentation                                                                                     */
/*********************************************************************************************************************/

/*

Routine: (service) USBPORTSVC_RequestAsyncCallbackEx

NTSTATUS
USBPORTSVC_RequestAsyncCallbackEx(
    PDEVICE_DATA DeviceData,
    ULONG MilliSecondInterval,
    PVOID Context,
    ULONG ContextLength,
    PMPFN_MINIPORT_CALLBACK Callback,
    PVOID *CbHandle,
    BOOLEAN RootHubPortFunction
    )

Description:

    Allows miniport to request an async callback when the millisecond interval specified has elapsed.

    The context field is copied to an allocated structure and the miniport is called back with the copy so it
    is safe for the miniport to use a stack variable as context.

    The callback is serialized with the core function lock.

Irql - DISPATCH_LEVEL

Parameters:

MilliSecondInterval - minimum (ms) time that must elapse before callback is invoked.

Context - miniport specified context for callback.

ContextLength - length in bytes of miniport context.

CbHandle - optional, pointer filled in with handle that can be used to cancel the callback dpc

RootHubPortFunction - miniport must set this value to true if the request is in the context of a root hub port function,
                        MINIPORT_RH_* or an async timer callback


Returns:

     NT error code.


*/


/*

Routine: (service) USBPORTSVC_CancelAsyncCallback

BOOLEAN
USBPORTSVC_CancelAsyncCallback(
    PDEVICE_DATA DeviceData,
    PVOID CbHandle,
    BOOLEAN RootHubPortFunction
    )

Description:

    Allows miniport to cancel a previously requested callback.


Irql - DISPATCH_LEVEL

Parameters:

CbHandle - handle returned from a call to USBPORTSVC_RequestAsyncCallbackEx

RootHubPortFunction - miniport must set this value to true if the request is in the context of a root hub port function,
                        MINIPORT_RH_* or an async timer callback


Returns:

     TRUE if callback was canceled.


*/

