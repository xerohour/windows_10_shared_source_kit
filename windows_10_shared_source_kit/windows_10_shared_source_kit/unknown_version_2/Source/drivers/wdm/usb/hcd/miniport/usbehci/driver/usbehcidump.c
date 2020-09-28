/*++

Copyright (c) 2010  Microsoft Corporation

Module Name:

    USBEHCIDump.c

Abstract:

    USB 2.0 EHCI dump driver

Environment:

    kernel mode only

Notes:

Revision History:

    6-8-10

--*/

#include "common.h"

NTSTATUS
__stdcall
EHCIDumpInitializeInterface(
    __inout PUSB_DUMP_CONTROLLER_INFO ControllerData
    )
/*++

Routine Description:

    Get the dump data structures here

Arguments:

Return Value:

    STATUS_SUCCESS

--*/
{
    ControllerData->ControllerInitialize = EHCIDumpControllerInitialize;
    ControllerData->ControllerSendURB = EHCIDumpUrbSend;
    ControllerData->ControllerCleanUp = EHCIDumpControllerCleanUp;
    ControllerData->ControllerResetDevice = EHCIDumpResetDevice;
    ControllerData->ControllerSendURBAsync = NULL;
    ControllerData->ControllerPollForCompletion = NULL;
    return STATUS_SUCCESS;
}


NTSTATUS
__stdcall
EHCIDumpCleanUpInterface(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext
    )
/*++

Routine Description:

    Get the dump data structures here

Arguments:

Return Value:

    STATUS_SUCCESS

--*/
{
    return STATUS_SUCCESS;
}



NTSTATUS
__stdcall
EHCIDumpControllerInitialize(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext
    )
{

    PUCHAR                      base;
    HCLENGTHVERSION             hcLengthVersion;
    USBSTS                      sts;
    USBINTR                     intReg;
    PHC_OPERATIONAL_REGISTER    hcOp              = NULL;
    PHC_CAPABILITIES_REGISTER   hcCap             = NULL;
    NTSTATUS                    ntStatus          = STATUS_SUCCESS;

    // Get the capabilities register and read the length
    base = DumpContext->BaseRegisterPointer;
    hcCap = (PHC_CAPABILITIES_REGISTER)base;
    hcLengthVersion.ul = READ_REGISTER_ULONG(&hcCap->HcLengthVersion.ul);

    // First check if the device is still there. If not fail
    if (hcLengthVersion.ul == EHCI_HARDWARE_GONE) {
        ntStatus = STATUS_DEVICE_ENUMERATION_ERROR;
        goto EHCIControllerInitializeEnd;
    }

    // Lets get the operational register now
    hcOp = (PHC_OPERATIONAL_REGISTER) (base + hcLengthVersion.HcCapLength);

    // Check usb status register USBSTS
    // eHC should be functional
    // Host system error field
    // usb error interrupt field
    // if things are bad we will need to reset the h/w
    sts.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);
    if (sts.HostSystemError == 1) {
        ntStatus = STATUS_DEVICE_ENUMERATION_ERROR;
        goto EHCIControllerInitializeEnd;
    }

    //Disable interrupts by setting USBINTR.USBInterruptEnable to 0
    // clear the port change interrupt
    // TODO: Check if we should be clearing all the bits
    intReg.ul = READ_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul);
    intReg.UsbInterrupt = 0;
    WRITE_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul,
                         intReg.ul);

    // Stop current async and any periodic sync schedule
    ntStatus = EHCIDumpDisableSchedule(hcOp);
    if (!NT_SUCCESS(ntStatus)) {
        goto EHCIControllerInitializeEnd;
    }

    // Setup the QH and TD for control and bulk endpoints
    // Setup the new aync schedule based on ASYNCLISTADDR
    EHCIDumpInitializeQH(hcOp, DumpContext);

    ntStatus = EHCIDumpEnableAsyncSchedule(hcOp);

EHCIControllerInitializeEnd:

    return ntStatus;
}


NTSTATUS
__stdcall
EHCIDumpControllerCleanUp(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext
    )
/*++

Routine Description:

    This routine clears the host controller Run/Stop bit and halts the
    host controller.

Arguments:

Return Value:

    STATUS_SUCCESS or STATUS_DEVICE_ENUMERATION_ERROR

--*/
{
    PUCHAR                      base;
    HCLENGTHVERSION             hcLengthVersion;
    USBCMD                      cmd;
    PHC_OPERATIONAL_REGISTER    hcOp              = NULL;
    PHC_CAPABILITIES_REGISTER   hcCap             = NULL;
    NTSTATUS                    ntStatus          = STATUS_SUCCESS;
    USBSTS                      status;
    ULONG                       i;

    // Get the capabilities register and read the length
    base = DumpContext->BaseRegisterPointer;
    hcCap = (PHC_CAPABILITIES_REGISTER)base;
    hcLengthVersion.ul = READ_REGISTER_ULONG(&hcCap->HcLengthVersion.ul);

    // First check if the device is still there. If not fail
    if (hcLengthVersion.ul == EHCI_HARDWARE_GONE) {
        ntStatus = STATUS_DEVICE_ENUMERATION_ERROR;
        goto EHCIDumpControllerCleanUpEnd;
    }

    // Lets get the operational register now
    hcOp = (PHC_OPERATIONAL_REGISTER) (base + hcLengthVersion.HcCapLength);

    // clear the run bit
    cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
    cmd.HostControllerRun = 0;
    cmd.IntOnAsyncAdvanceDoorbell = 0;
    WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

    // The EHCI specification says "The Host Controller must halt within
    // 16 micro-frames after software clears the Run bit. The HC Halted
    // bit in the status register indicates when the Host Controller has
    // finished its pending pipelined transactions and has entered the
    // stopped state."
    //
    for (i=0; i<18; i++) {
        status.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);
        if (status.HcHalted) {
            break;
        }
        KeStallExecutionProcessor(125);
    }

    if (status.HcHalted != 1) {
        // hardware is messed up
        ntStatus = STATUS_DEVICE_ENUMERATION_ERROR;
        goto EHCIDumpControllerCleanUpEnd;
    }

EHCIDumpControllerCleanUpEnd:

    return ntStatus;
}


// This structure is here for debugging purpose.
// Eventually it would be moved into the function.
typedef struct _EHCI_DUMP_ENDPOINT_INFO{
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    PHCD_TRANSFER_DESCRIPTOR td[DUMP_MAX_TD_SUPPORTED];
}EHCI_DUMP_ENDPOINT_INFO,*PEHCI_DUMP_ENDPOINT_INFO;

EHCI_DUMP_ENDPOINT_INFO DumpEndpoint[DumpEndpointTypeCount];

VOID
EHCIDumpInitializeQH(
    __inout PHC_OPERATIONAL_REGISTER OperationalRegister,
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext
)
{
    PHCD_QUEUEHEAD_DESCRIPTOR AsyncQueueHead;
    HC_QTD_TOKEN token;
    HW_LINK_POINTER AsyncQueueHeadHwLink;
    HW_32BIT_PHYSICAL_ADDRESS endp;
    PUCHAR buffer;
    HW_32BIT_PHYSICAL_ADDRESS phys;
    ULONG i, j;
    ULONG bytes;
    PHCD_TRANSFER_DESCRIPTOR setupTd;

    RtlZeroMemory(&DumpEndpoint[0], DumpEndpointTypeCount*sizeof(EHCI_DUMP_ENDPOINT_INFO));
    RtlZeroMemory(DumpContext->CommonBufferPool,DumpContext->CommonBufferSize);

    buffer = DumpContext->CommonBufferPool;
    phys = DumpContext->CommonBufferPA.LowPart;
    bytes = DumpContext->CommonBufferSize;

    // Allocate the Asynchronous List Persistent Queue Head.
    //
    AsyncQueueHead = (PHCD_QUEUEHEAD_DESCRIPTOR) buffer;
    AsyncQueueHead->PhysicalAddress = phys;

    phys += sizeof(HCD_QUEUEHEAD_DESCRIPTOR);
    buffer += sizeof(HCD_QUEUEHEAD_DESCRIPTOR);
    bytes -= sizeof(HCD_QUEUEHEAD_DESCRIPTOR);

    // Verify if address is aligned
    endp = AsyncQueueHead->PhysicalAddress + sizeof(HW_QUEUEHEAD_DESCRIPTOR)-1;
    if ((endp & ~(EHCI_PAGE_SIZE-1)) !=
        (AsyncQueueHead->PhysicalAddress & ~(EHCI_PAGE_SIZE-1))) {
        // TODO: EHCI_AssertHwAlignment would fire
    }

    // Head of Reclamation List Flag = 1
    // CurrentTD = NULL
    // Next_qTD = EHCI_TERMINATE_BIT
    // AltNext_qTD = NULL
    // Active = 0, Halted = 1
    //
    SET_T_BIT(AsyncQueueHead->HwQH.Overlay.qTD.Next_qTD.HwAddress);
    token.ul = 0;
    token.Halted = 1;
    AsyncQueueHead->HwQH.Overlay.qTD.Token = token;
    AsyncQueueHead->HwQH.EpChars.HeadOfReclimationList = 1;
    AsyncQueueHead->HwQH.EpCaps.HighBWPipeMultiplier = 1;
    AsyncQueueHead->Sig = SIG_HCD_AQH;

    AsyncQueueHeadHwLink.HwAddress = AsyncQueueHead->PhysicalAddress;
    SET_QH(AsyncQueueHeadHwLink.HwAddress);

    // Get the QH and TD for the endpoints
    for(i=0; i<DumpEndpointTypeCount;i++) {
        // 256 byte block used to check for overlay sync
        // problems
        // use descriptor pad value to maintain alignment on a 4k boundary

        DumpEndpoint[i].qh = (PHCD_QUEUEHEAD_DESCRIPTOR) buffer;

        DumpEndpoint[i].qh->PhysicalAddress = phys;

        DumpEndpoint[i].qh->Sig = SIG_HCD_QH;

        phys += sizeof(HCD_QUEUEHEAD_DESCRIPTOR);
        buffer += sizeof(HCD_QUEUEHEAD_DESCRIPTOR);
        bytes -= sizeof(HCD_QUEUEHEAD_DESCRIPTOR);

        // Initialize all TD's here
        for (j=0; j<DUMP_MAX_TD_SUPPORTED ; j++) {
            DumpEndpoint[i].td[j] = (PHCD_TRANSFER_DESCRIPTOR) buffer;
            DumpEndpoint[i].td[j]->PhysicalAddress = phys;
            DumpEndpoint[i].td[j]->Sig = SIG_HCD_TD;
            TRANSFER_CONTEXT_PTR(DumpEndpoint[i].td[j]->TransferContext) = FREE_TD_CONTEXT;

            phys += sizeof(HCD_TRANSFER_DESCRIPTOR);
            buffer += sizeof(HCD_TRANSFER_DESCRIPTOR);
            bytes -= sizeof(HCD_TRANSFER_DESCRIPTOR);
        }

        DumpEndpoint[i].qh->HwQH.EpChars.DeviceAddress = DumpContext->DeviceAddress;

        DumpEndpoint[i].qh->HwQH.EpChars.EndpointSpeed = HcEPCHAR_HighSpeed;

        // TODO: Double check
        // BUGBUG: need to put int specific ep's maxpktsize.  Note this field is 11 bits

        DumpEndpoint[i].qh->HwQH.EpCaps.HighBWPipeMultiplier = 1;

        // init the overlay are such that we are in the 'advance queue'
        // state with the next queue Tds pointing to terminate links
        DumpEndpoint[i].qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = EHCI_TERMINATE_BIT;
        DumpEndpoint[i].qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = EHCI_TERMINATE_BIT;
        token.ul = 0; // Active = 0, Halted = 0
        DumpEndpoint[i].qh->HwQH.Overlay.qTD.Token = token;

        // Check is this the control pipe
        if (i == DumpControl) {
            // setupTD is the first TD in the control transfer
            setupTd = DumpEndpoint[i].td[0];

            // use data toggle in the TDs for control
            DumpEndpoint[i].qh->HwQH.EpChars.DataToggleControl = HcEPCHAR_Toggle_From_qTD;
            DumpEndpoint[i].qh->HwQH.EpChars.MaximumPacketLength = 0x40;

            DumpEndpoint[i].qh->HwQH.CurrentTD.HwAddress = setupTd->PhysicalAddress;
            DumpEndpoint[i].qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = EHCI_TERMINATE_BIT;
            DumpEndpoint[i].qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = EHCI_TERMINATE_BIT;
            token.ul = 0; // Active = 0, BytesToTransfer = 0
            DumpEndpoint[i].qh->HwQH.Overlay.qTD.Token = token;

            setupTd->HwTD.Next_qTD.HwAddress = EHCI_TERMINATE_BIT;
            TRANSFER_DESCRIPTOR_PTR(setupTd->NextHcdTD) = NULL;
            setupTd->HwTD.AltNext_qTD.HwAddress = EHCI_TERMINATE_BIT;
            TRANSFER_DESCRIPTOR_PTR(setupTd->AltNextHcdTD) = NULL;
            token.ul = 0; // Active = 0, Halted = 0
            setupTd->HwTD.Token = token;
            for (j=0; j<5; j++) {
                setupTd->HwTD.BufferPage[j].ul = setupTd->PhysicalAddress;
            }
        } else {

            DumpEndpoint[i].qh->HwQH.EpChars.DataToggleControl = HcEPCHAR_Ignore_Toggle;
            DumpEndpoint[i].qh->HwQH.EpChars.MaximumPacketLength = 0x200;

            DumpEndpoint[i].qh->HwQH.CurrentTD.HwAddress = DumpEndpoint[i].td[0]->PhysicalAddress;
            DumpEndpoint[i].qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = DumpEndpoint[i].td[0]->PhysicalAddress;
            DumpEndpoint[i].qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = DumpEndpoint[i].td[0]->PhysicalAddress;

            token.ul = 0; // Active = 0, BytesToTransfer = 0
            DumpEndpoint[i].qh->HwQH.Overlay.qTD.Token = token;
        }
    }

    //
    // Initializing the endpoint addresses upfront.
    // DumpEndpoint[0] is the control endpoint
    //
    for (i=0, j=1; i<DUMP_MAX_ENDPOINT_COUNT-1; i++, j++) {

        if (DumpContext->EndpointInfo[i].Address != 0) {
            DumpEndpoint[j].qh->HwQH.EpChars.EndpointNumber = DumpContext->EndpointInfo[i].Address;
        }
    }

    //
    // Now finally lets link all the QH's up
    //
    AsyncQueueHead->HwQH.HLink.HwAddress = DumpEndpoint[DumpControl].qh->PhysicalAddress;
    SET_QH(AsyncQueueHead->HwQH.HLink.HwAddress);

    for (i=0; i<DUMP_MAX_ENDPOINT_COUNT-1 ; i++) {

        DumpEndpoint[i].qh->HwQH.HLink.HwAddress = DumpEndpoint[i+1].qh->PhysicalAddress;
        SET_QH(DumpEndpoint[i].qh->HwQH.HLink.HwAddress);
    }

    DumpEndpoint[DUMP_MAX_ENDPOINT_COUNT-1].qh->HwQH.HLink.HwAddress = AsyncQueueHead->PhysicalAddress;
    SET_QH(DumpEndpoint[DUMP_MAX_ENDPOINT_COUNT-1].qh->HwQH.HLink.HwAddress);

    // write the async qh to the controller
    WRITE_REGISTER_ULONG(&OperationalRegister->AsyncListAddr,
            AsyncQueueHeadHwLink.HwAddress);

}

NTSTATUS
EHCIDumpPollForCompletion(
    __inout PHC_OPERATIONAL_REGISTER OperationalRegister,
    __in PEHCI_DUMP_ENDPOINT_INFO DumpEndpoint)
{
    int i;
    HC_QTD_TOKEN token;
    USBSTS sts;
    NTSTATUS result = USBD_STATUS_SUCCESS;
    ULONG timeoutMicrosec;

    for (i = 0; i < DUMP_MAX_TD_SUPPORTED; i++) {

        //
        // 5 second timeout in microseconds
        //
        timeoutMicrosec = 5*1000*1000;

        do {

            token = DumpEndpoint->td[i]->HwTD.Token;

            if (OperationalRegister != NULL) {

                sts.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbStatus.ul);
                if (sts.HcHalted || sts.HostSystemError == 1 || sts.UsbError == 1){

                    //
                    // Need to log this error
                    //
                }

                if (token.Halted) {

                    sts.UsbError = 0;
                    WRITE_REGISTER_ULONG(&OperationalRegister->UsbStatus.ul,sts.ul);
                    return USBD_STATUS_STALL_PID;
                }
            }

            if (token.XactErr) {
                result = USBD_STATUS_XACT_ERROR;
                break;
            }

            if (token.BabbleDetected) {
                result = USBD_STATUS_BABBLE_DETECTED;
                break;
            }

            if (token.DataBufferError) {
                result = USBD_STATUS_DATA_BUFFER_ERROR;
                break;
            }

            if (token.MissedMicroFrame) {
                result = USBD_STATUS_XACT_ERROR;
                break;
            }

            KeStallExecutionProcessor(50);
            timeoutMicrosec -= 50;

        }while (token.Active && (timeoutMicrosec > 0));

        if (timeoutMicrosec <= 0) {

            result = USBD_STATUS_TIMEOUT;
            break;
        }

        if (result != USBD_STATUS_SUCCESS) {

            break;
        }

    }

    return result;
}


VOID
EHCIDumpInitTD(
    __inout PHCD_TRANSFER_DESCRIPTOR TD)
{
    HC_QTD_TOKEN token;
    ULONG i;
    TD->Flags |= TD_FLAG_XFER;
    for (i=0; i<5; i++) {
        TD->HwTD.BufferPage[i].ul = 0x0bad0000;
        TD->HwTD.BufferPage64[i]  = 0x00000000;
    }
    TD->HwTD.Next_qTD.HwAddress = EHCI_TERMINATE_BIT;
    TD->HwTD.AltNext_qTD.HwAddress = EHCI_TERMINATE_BIT;
    token.ul = 0;
    token.ErrorCounter = 3;
    TD->HwTD.Token = token;
    TRANSFER_DESCRIPTOR_PTR((TD)->NextHcdTD) = NULL;
}

NTSTATUS
EHCIDumpControlTransfer(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext,
    __in_xcount(8) PUCHAR SetupPacket,
    __inout PHYSICAL_ADDRESS Buffer,
    __inout ULONG BufferLength
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
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    HC_QTD_TOKEN token;
    ULONG lengthMapped;
    ULONG nextToggle;
    PHC_OPERATIONAL_REGISTER    hcOp            = NULL;
    PHC_CAPABILITIES_REGISTER   hcCap           = NULL;
    NTSTATUS                    ntStatus        = STATUS_SUCCESS;
    PUCHAR                      base;
    HCLENGTHVERSION             hcLengthVersion;
    PUSB_DEFAULT_PIPE_SETUP_PACKET setuppkt = (PUSB_DEFAULT_PIPE_SETUP_PACKET)SetupPacket;
    ULONG                       tdIdx;
    //
    // Get the capabilities register and read the length
    //
    base = DumpContext->BaseRegisterPointer;
    hcCap = (PHC_CAPABILITIES_REGISTER)base;
    hcLengthVersion.ul = READ_REGISTER_ULONG(&hcCap->HcLengthVersion.ul);

    //
    // First check if the device is still there. If not fail
    //
    if (hcLengthVersion.ul == EHCI_HARDWARE_GONE) {
        ntStatus = STATUS_DEVICE_ENUMERATION_ERROR;
        return ntStatus;
    }

    //
    // Lets get the operational register now
    //
    hcOp = (PHC_OPERATIONAL_REGISTER) (base + hcLengthVersion.HcCapLength);

    nextToggle = HcTOK_Toggle1;

    //
    // first prepare a TD for the setup packet
    //
    //
    // We have a setup TD allocated already
    //
    setupTd = DumpEndpoint[DumpControl].td[0];

    //
    // Initialize the TD, including setting BufferPage64[0-4] to zero.
    //
    for (tdIdx = 0;tdIdx < DUMP_MAX_TD_SUPPORTED; tdIdx++) {
        EHCIDumpInitTD(DumpEndpoint[DumpControl].td[tdIdx]);
    }

    //
    // Move setup data into TD (8 chars long)
    //
    RtlCopyMemory(&setupTd->Packet[0],
                  &SetupPacket[0],
                  8);

    //
    // this will set the offset and phys address bits at
    // the same time
    setupTd->HwTD.BufferPage[0].ul = (ULONG)(((PCHAR) &setupTd->Packet[0])
                        - ((PCHAR) &setupTd->HwTD)) + setupTd->PhysicalAddress;

    token = setupTd->HwTD.Token;
    token.BytesToTransfer = 8;
    token.Pid = HcTOK_Setup;
    token.DataToggle = HcTOK_Toggle0;
    token.InterruptOnComplete = 0;
    //setupTd->HwTD.Token.Active = 1;
    setupTd->HwTD.Token = token;

    statusTd = DumpEndpoint[DumpControl].td[2];

    //
    // Initialize the TD, including setting BufferPage64[0-4] to zero.
    //
    EHCIDumpInitTD(statusTd);

    //
    // point setup to status
    //
    setupTd->HwTD.AltNext_qTD.HwAddress = statusTd->PhysicalAddress;
    TRANSFER_DESCRIPTOR_PTR(setupTd->AltNextHcdTD) = statusTd;

    //
    // now setup the data phase
    //
    td = prevTd = setupTd;
    lengthMapped = 0;

    while (lengthMapped < BufferLength) {
        //
        // fields for data TD
        //
        td =  DumpEndpoint[DumpControl].td[1];
        EHCIDumpInitTD(td);
        prevTd->HwTD.Next_qTD.HwAddress = td->PhysicalAddress;
        TRANSFER_DESCRIPTOR_PTR(prevTd->NextHcdTD) = td;

        token = td->HwTD.Token;

        // use direction specified in transfer
        if (setuppkt->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) {
            token.Pid = HcTOK_In;
        } else {
            token.Pid = HcTOK_Out;
        }

        token.DataToggle = nextToggle;
        nextToggle = (nextToggle == HcTOK_Toggle1) ? HcTOK_Toggle0 : HcTOK_Toggle1;

        lengthMapped = BufferLength;
        token.BytesToTransfer = lengthMapped;

        token.InterruptOnComplete = 0;
        token.Active = 1;
        td->HwTD.Token = token;

        td->HwTD.AltNext_qTD.HwAddress = statusTd->PhysicalAddress;
        TRANSFER_DESCRIPTOR_PTR(td->AltNextHcdTD) = statusTd;

        td->HwTD.BufferPage[0].ul = Buffer.u.LowPart;
        td->HwTD.BufferPage64[0] = Buffer.u.HighPart;

        //
        // calculate next data toggle
        // if number of packets is odd the nextToggle is 0
        // otherwise it is 1
        //
        prevTd = td;
    }

    //
    // last td prepared points to status
    //
    td->HwTD.Next_qTD.HwAddress = statusTd->PhysicalAddress;
    TRANSFER_DESCRIPTOR_PTR(td->NextHcdTD) = statusTd;

    //
    // now do the status phase
    //
    // no buffer
    //
    statusTd->HwTD.BufferPage[0].ul = 0;

    token = statusTd->HwTD.Token;

    token.BytesToTransfer = 0;
    statusTd->TransferLength = 0;
    //
    // status stage is always toggle 1
    //
    token.DataToggle = HcTOK_Toggle1;
    //statusTd->HwTD.Token.Active = 1;
    token.InterruptOnComplete = 0;

    //
    // status phase is opposite data dirrection
    //
    if (setuppkt->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) {
        token.Pid = HcTOK_Out;
    } else {
        token.Pid = HcTOK_In;
    }

    statusTd->HwTD.Token = token;

    //
    // td points to last TD in this transfer, point it at original setup (first TD)
    //
    statusTd->HwTD.Next_qTD.HwAddress = setupTd->PhysicalAddress;
    TRANSFER_DESCRIPTOR_PTR(statusTd->NextHcdTD) = setupTd;

    //
    // Make setupTD the alternate TD for the status TD
    //
    statusTd->HwTD.AltNext_qTD.HwAddress = setupTd->PhysicalAddress;
    TRANSFER_DESCRIPTOR_PTR(statusTd->AltNextHcdTD) = setupTd;

    //
    // set the active bit in the setup Phase TD, this will
    // activate the transfer
    //
    qh = DumpEndpoint[DumpControl].qh;

    qh->HwQH.CurrentTD.HwAddress = setupTd->PhysicalAddress;
    qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = setupTd->PhysicalAddress;
    qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = setupTd->PhysicalAddress;

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

    return EHCIDumpPollForCompletion(hcOp,&DumpEndpoint[DumpControl]);
}

NTSTATUS
EHCIDumpBulkTransfer(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext,
    __in    PURB TransferURB,
    __in PPHYSICAL_ADDRESS TransferPAArray
)
{
    PHC_OPERATIONAL_REGISTER    hcOp            = NULL;
    PHC_CAPABILITIES_REGISTER   hcCap           = NULL;
    NTSTATUS                    ntStatus        = STATUS_SUCCESS;
    PUCHAR                      base;
    HCLENGTHVERSION             hcLengthVersion;
    PEHCI_DUMP_ENDPOINT_INFO    dumpEndpoint;
    PPHYSICAL_ADDRESS           BufferPA;
    PHCD_QUEUEHEAD_DESCRIPTOR   qh;
    PHCD_TRANSFER_DESCRIPTOR    td;
    HC_QTD_TOKEN                token;
    ULONG                       nextToggle;
    ULONG                       lengthMapped;
    ULONG                       lengthMappedInTD;
    ULONG                       i;
    ULONG                       tdIdx;
    ULONG                       bp;
    ULONG                       endpointAddress;
    ULONG                       bufferLength;
    PVOID                       scratch         = NULL;
    ULONG                       physIdx;
    BOOLEAN                     doubleBufferCopyBack = FALSE;
    PHYSICAL_ADDRESS            phys[DUMP_MAX_TRANSFER_PAGES];
    struct _URB_BULK_OR_INTERRUPT_TRANSFER *bulkUrb;

    //
    // Get the capabilities register and read the length
    //
    base = DumpContext->BaseRegisterPointer;
    hcCap = (PHC_CAPABILITIES_REGISTER)base;
    hcLengthVersion.ul = READ_REGISTER_ULONG(&hcCap->HcLengthVersion.ul);

    //
    // First check if the device is still there. If not fail
    //
    if (hcLengthVersion.ul == EHCI_HARDWARE_GONE) {
        ntStatus = STATUS_DEVICE_ENUMERATION_ERROR;
        return ntStatus;
    }

    //
    // Lets get the operational register now
    //
    hcOp = (PHC_OPERATIONAL_REGISTER) (base + hcLengthVersion.HcCapLength);

    bulkUrb = (struct _URB_BULK_OR_INTERRUPT_TRANSFER *)TransferURB;

    endpointAddress = ((PUSBD_PIPE_INFORMATION)bulkUrb->PipeHandle)->EndpointAddress;

    dumpEndpoint = NULL;


    for (i = 1; i < DUMP_MAX_ENDPOINT_COUNT ; i++) {
        if ((endpointAddress&USB_ENDPOINT_ADDRESS_MASK) == DumpEndpoint[i].qh->HwQH.EpChars.EndpointNumber) {
            dumpEndpoint = &DumpEndpoint[i];
            break;
        }
    }

    //
    // If we did not find an endpoint something is seriously wrong
    //
    if (dumpEndpoint == NULL) {
        ntStatus = STATUS_INVALID_DEVICE_STATE;
        return ntStatus;
    }

    //
    // We should never have a case where the endpoint address
    // of one of the qh is zero
    //
    if (dumpEndpoint->qh->HwQH.EpChars.EndpointNumber == 0) {
        NT_ASSERT(FALSE);
        return STATUS_INVALID_DEVICE_STATE;
    }

    bufferLength = bulkUrb->TransferBufferLength;

    //
    // Lets clear out all the TD's in this endpoint
    //
    for (tdIdx = 0;tdIdx < DUMP_MAX_TD_SUPPORTED; tdIdx++) {
        EHCIDumpInitTD(dumpEndpoint->td[tdIdx]);
    }

    //
    // Transfer PA is an array of Physical Addresses
    // Its length is DUMP_MAX_TRANSFER_PAGES
    // All the unused entries are initilaized to 0
    // Cound of used entries is bound by the BufferLength
    //
    BufferPA = TransferPAArray;

    //
    // There are some controllers that can not accept physical addresses > 4GB
    // We need to double buffer for them for such physical addresses
    // This is a perf hit
    //
    if (!DumpContext->Bit64AddressingCapability && BufferPA->HighPart != 0) {

        scratch = (PUCHAR)DumpContext->CommonBufferPool + DUMP_BUFFER_HEADER_SIZE + DUMP_BUFFER_SCRATCH_SIZE;

        //
        // For In we need to copy back after the transfer
        // For out we need to copy before the transfer
        //
        if (endpointAddress & USB_ENDPOINT_DIRECTION_MASK) {
            doubleBufferCopyBack = TRUE;
        } else {
            RtlCopyMemory(scratch, bulkUrb->TransferBuffer, bulkUrb->TransferBufferLength);
        }

        //
        // Zero out the contents of physical page pointers
        //
        for (physIdx = 0; physIdx < DUMP_MAX_TRANSFER_PAGES; physIdx++) {
            phys[physIdx].QuadPart = 0;
        }

        phys[0] = DumpContext->CommonBufferPA;
        phys[0].QuadPart += DUMP_BUFFER_HEADER_SIZE + DUMP_BUFFER_SCRATCH_SIZE;
        for (physIdx = 1;
             physIdx <= (bulkUrb->TransferBufferLength/PAGE_SIZE) &&
             physIdx < DUMP_MAX_TRANSFER_PAGES;
             physIdx++) {

            phys[physIdx].QuadPart = phys[physIdx-1].QuadPart + PAGE_SIZE;
        }
        BufferPA = phys;
    }

    qh = dumpEndpoint->qh;
    lengthMapped = 0;
    td = NULL;

    tdIdx = 0;
    nextToggle = HcTOK_Toggle0;

    while (lengthMapped < bufferLength && tdIdx < DUMP_MAX_TD_SUPPORTED) {

        td = dumpEndpoint->td[tdIdx];
        lengthMappedInTD = 0;

        CLEAR_FLAG(td->Flags, TD_FLAG_SLOT_RESET);

        token = td->HwTD.Token;
        token.InterruptOnComplete = 0;

        if (endpointAddress & USB_ENDPOINT_DIRECTION_MASK) {
            token.Pid = HcTOK_In;
        } else {
            token.Pid = HcTOK_Out;
        }

        // DataToggle is not used for TD's for Bulk transfers
        // We disable the toggle by setting a field in the EPChars in QH
        // to HcEPCHAR_Ignore_Toggle
        // The following code is a nop
        //
        token.DataToggle = nextToggle;
        nextToggle = (nextToggle == HcTOK_Toggle1) ? HcTOK_Toggle0 : HcTOK_Toggle1;

        td->HwTD.BufferPage[0].ul = BufferPA->LowPart;
        td->HwTD.BufferPage64[0] = BufferPA->HighPart;
        BufferPA++;

        //
        // If the buffer is less than a page the first BufferPage can transfer it
        // If the buffer is more than one page, the first page may start after
        // the page boundary
        //
        lengthMappedInTD += bufferLength < PAGE_SIZE ?
                                bufferLength :
                                (PAGE_SIZE - td->HwTD.BufferPage[0].CurrentOffset);

        for (bp = 1; bp < 5 && (lengthMappedInTD + lengthMapped) < bufferLength ; bp++) {

            td->HwTD.BufferPage[bp].ul = BufferPA->LowPart;
            td->HwTD.BufferPage64[bp] = BufferPA->HighPart;
            if ((lengthMappedInTD + lengthMapped + PAGE_SIZE) <= bufferLength) {
                lengthMappedInTD += PAGE_SIZE;
            }
            else {
                lengthMappedInTD += (bufferLength - (lengthMappedInTD + lengthMapped));
            }
            BufferPA++;
        }

        token.BytesToTransfer = lengthMappedInTD;
        td->HwTD.Token = token;

        lengthMapped += lengthMappedInTD;
        tdIdx++;
    }

    //
    // This mean we have more data than what our TD's can handle
    //
    if (lengthMapped < bufferLength) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // special case the zero length transfer
    //
    if (bufferLength == 0) {

        td = dumpEndpoint->td[0];

        CLEAR_FLAG(td->Flags, TD_FLAG_SLOT_RESET);

        //
        // Initialize the TD, including setting BufferPage64[0-4] to zero.
        //
        EHCIDumpInitTD(td);

        token = td->HwTD.Token;
        token.InterruptOnComplete = 0;

        //
        // use direction specified in transfer
        //
        if (endpointAddress & USB_ENDPOINT_DIRECTION_MASK) {
            token.Pid = HcTOK_In;
        } else {
            token.Pid = HcTOK_Out;
        }

        token.DataToggle = HcTOK_Toggle0;

        //
        // point to a dummy buffer
        //
        td->HwTD.BufferPage[0].ul = td->PhysicalAddress;

        token.BytesToTransfer = 0;
        td->HwTD.Token = token;

        td->TransferLength = 0;
    }

    for (i=0;i < tdIdx && tdIdx > 1; i++) {
        dumpEndpoint->td[i]->HwTD.Next_qTD.HwAddress = dumpEndpoint->td[(i+1)%tdIdx]->PhysicalAddress;
        TRANSFER_DESCRIPTOR_PTR(dumpEndpoint->td[i]->NextHcdTD) = dumpEndpoint->td[(i+1)%tdIdx];
    }

    qh->HwQH.CurrentTD.HwAddress = dumpEndpoint->td[0]->PhysicalAddress;
    qh->HwQH.Overlay.qTD.AltNext_qTD.HwAddress = dumpEndpoint->td[0]->PhysicalAddress;
    qh->HwQH.Overlay.qTD.Next_qTD.HwAddress = dumpEndpoint->td[0]->PhysicalAddress;

    //
    // no transfers current, reinitialze the slot
    //
    token = qh->HwQH.Overlay.qTD.Token;
    token.BytesToTransfer = 0;
    token.Active = 0;
    token.ErrorCounter = 3;
    token.SplitXstate = 0;
    token.C_Page = 0;
    qh->HwQH.Overlay.qTD.Token = token;

    for (i=0; i< tdIdx; i++) {
        token = dumpEndpoint->td[i]->HwTD.Token;
        token.Active = 1;
        dumpEndpoint->td[i]->HwTD.Token = token;
    }

    ntStatus = EHCIDumpPollForCompletion(hcOp,dumpEndpoint);

    if (NT_SUCCESS(ntStatus)){
        //
        // If this was a double buffered IN transaction
        // we need to copy back the results
        //
        if (doubleBufferCopyBack) {
            RtlCopyMemory(bulkUrb->TransferBuffer, scratch, bulkUrb->TransferBufferLength);
        }
    }

    return ntStatus;

}

NTSTATUS
__stdcall
EHCIDumpUrbSend(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext,
    __in    PURB TransferURB,
    __in    PPHYSICAL_ADDRESS TransferPAArray
    )
{

    NTSTATUS                        ntStatus;
    struct _URB_PIPE_REQUEST*       resetPipeUrb;
    UCHAR                           endpointAddress;
    struct _URB_CONTROL_TRANSFER*   controlTransferUrb;
    PHYSICAL_ADDRESS                physcratch;

    //
    // Assume success for the completion of the Irp and URB.
    //
    ntStatus = STATUS_SUCCESS;

    //
    // Initialize the URB status to USBD_STATUS_SUCCESS.
    // Some drivers do not initialize the URB status.
    //
    TransferURB->UrbHeader.Status = USBD_STATUS_SUCCESS;

    switch (TransferURB->UrbHeader.Function) {

        case URB_FUNCTION_RESET_PIPE:

            //
            // Get endpoint number
            //
            resetPipeUrb = (struct _URB_PIPE_REQUEST *)TransferURB;
            endpointAddress = ((PUSBD_PIPE_INFORMATION)resetPipeUrb->PipeHandle)->EndpointAddress;

            ntStatus = EHCIDumpResetPipe(DumpContext,
                                         endpointAddress);

            if (!NT_SUCCESS(ntStatus)) {
                //
                // Fail the bulk reset pipe
                //
            }


            break;

        case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:

            ntStatus = EHCIDumpBulkTransfer(DumpContext,
                                            TransferURB,
                                            TransferPAArray);
            if (!NT_SUCCESS(ntStatus)) {
                //
                // Fail the bulk transfer
                //
            }
            break;

        case URB_FUNCTION_CONTROL_TRANSFER:

            controlTransferUrb = (struct _URB_CONTROL_TRANSFER *)TransferURB;

            physcratch.QuadPart = 0;
            //
            // A control transfer in dump scenario's could at max contain
            // only one Physical Address.
            //
            if (TransferPAArray != NULL) {
                physcratch = TransferPAArray[0];
            }

            ntStatus = EHCIDumpControlTransfer(DumpContext,
                                               controlTransferUrb->SetupPacket,
                                               physcratch,
                                               controlTransferUrb->TransferBufferLength);
        default:
            break;
    }

    return ntStatus;
}

NTSTATUS
EHCIDumpResetExternalHubPort(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext,
    __in ULONG PortNum
    )
/*++

Routine Description:

    Reset an external hub port


    NOTES:


--*/
{
    NTSTATUS                    ntStatus = STATUS_SUCCESS;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    PUCHAR                      base;
    HCLENGTHVERSION             hcLengthVersion;
    PHC_CAPABILITIES_REGISTER   hcCap    = NULL;
    PHYSICAL_ADDRESS            physcratch;
    PUCHAR                      scratch;
    PUSB_PORT_STATUS_AND_CHANGE portStatusAndChange;
    ULONG                       retries;

    //
    // Get the capabilities register and read the length
    //
    base = DumpContext->BaseRegisterPointer;
    hcCap = (PHC_CAPABILITIES_REGISTER)base;
    hcLengthVersion.ul = READ_REGISTER_ULONG(&hcCap->HcLengthVersion.ul);

    //
    // First check if the device is still there. If not fail
    //
    if (hcLengthVersion.ul == EHCI_HARDWARE_GONE) {
        ntStatus = STATUS_DEVICE_ENUMERATION_ERROR;
        goto EHCIResetDeviceEnd;
    }

    //
    // Reuse the device's control qh/ep by just changing the qh's DeviceAddress to
    // the parent external hub's DeviceAddress.
    //
    DumpEndpoint[DumpControl].qh->HwQH.EpChars.DeviceAddress = DumpContext->ParentHubDeviceAddress;

    //
    // Reset the parent hub's port that the storage device is currently on
    //
    EHCIDUMP_INIT_SETUP_PACKET(setupPacket,
                               USB_REQUEST_SET_FEATURE,     // bRequest
                               BMREQUEST_HOST_TO_DEVICE,    // Dir
                               BMREQUEST_TO_OTHER,          // Recipient
                               BMREQUEST_CLASS,             // Type
                               0x4,                         // wValue
                               (USHORT)PortNum,             // wIndex
                               0);                          // wLength

    physcratch.QuadPart = 0;

    ntStatus = EHCIDumpControlTransfer(DumpContext,
                                       (PUCHAR)&setupPacket,
                                       physcratch,
                                       0);

    if (!NT_SUCCESS(ntStatus)) {
        goto EHCIResetDeviceEnd;
    }

    //
    // Wait for sometime before we send out the next transfer to the
    // device. Spec says 50ms is worst case
    //
    KeStallExecutionProcessor(50000);

    retries = 10;

    do {
        //
        // Check if port reset completed
        //
        EHCIDUMP_INIT_SETUP_PACKET(setupPacket,
                                   USB_REQUEST_GET_STATUS,      // bRequest
                                   BMREQUEST_DEVICE_TO_HOST,    // Dir
                                   BMREQUEST_TO_OTHER,          // Recipient
                                   BMREQUEST_CLASS,             // Type
                                   0,                           // wValue
                                   (USHORT)PortNum,             // wIndex
                                   4);                          // wLength

        scratch = (PUCHAR)DumpContext->CommonBufferPool + DUMP_BUFFER_HEADER_SIZE;
        physcratch.QuadPart = DumpContext->CommonBufferPA.QuadPart;
        physcratch.QuadPart += DUMP_BUFFER_HEADER_SIZE;
        RtlZeroMemory(scratch, sizeof(USB_PORT_STATUS_AND_CHANGE));

        ntStatus = EHCIDumpControlTransfer(DumpContext,
                                           (PUCHAR)&setupPacket,
                                           physcratch,
                                           sizeof(USB_PORT_STATUS_AND_CHANGE));

        if (!NT_SUCCESS(ntStatus)) {
            goto EHCIResetDeviceEnd;
        }

        //
        // Check to see if reset is complete
        //
        portStatusAndChange = (PUSB_PORT_STATUS_AND_CHANGE)scratch;

        if ((portStatusAndChange->PortStatus.Usb20PortStatus.Reset != 0) ||
           (portStatusAndChange->PortChange.Usb20PortChange.ResetChange != 1)) {
            //
            // Reset not completed
            //
            KeStallExecutionProcessor(50000);

        } else {
            //
            // Reset completed
            //
            break;
        }
    } while (retries--);

    //
    // Total loop duration of 550ms was not enough to reset the device
    //
    if (retries == 0) {
        ntStatus = STATUS_TIMEOUT;
    }

EHCIResetDeviceEnd:

    //
    // Reset the control ep to the storage device's DeviceAddress
    //
    DumpEndpoint[DumpControl].qh->HwQH.EpChars.DeviceAddress = DumpContext->DeviceAddress;

    return ntStatus;

}

NTSTATUS
EHCIDumpResetRHPort(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext,
    __in ULONG PortNum
    )
/*++

Routine Description:

    Reset a root hub port


    NOTES:


--*/
{
    NTSTATUS                    ntStatus = STATUS_SUCCESS;
    PUCHAR                      base;
    HCLENGTHVERSION             hcLengthVersion;
    PHC_OPERATIONAL_REGISTER    hcOp            = NULL;
    PHC_CAPABILITIES_REGISTER   hcCap           = NULL;
    PORTSC                      port;

    //
    // Get the capabilities register and read the length
    //
    base = DumpContext->BaseRegisterPointer;
    hcCap = (PHC_CAPABILITIES_REGISTER)base;
    hcLengthVersion.ul = READ_REGISTER_ULONG(&hcCap->HcLengthVersion.ul);

    //
    // First check if the device is still there. If not fail
    //
    if (hcLengthVersion.ul == EHCI_HARDWARE_GONE) {
        ntStatus = STATUS_DEVICE_ENUMERATION_ERROR;
        goto EHCIResetDeviceEnd;
    }

    //
    // Lets get the operational register now
    //
    hcOp = (PHC_OPERATIONAL_REGISTER) (base + hcLengthVersion.HcCapLength);

    //
    // Reset the rh port
    //
    port.ul = 0;
    port.PortEnable = 0;
    port.PortReset = 1;
    WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNum-1].ul, port.ul);

    //
    // wait 50ms
    //
    KeStallExecutionProcessor(50000);

    port.ul = 0;
    port.PortReset = 0;
    WRITE_REGISTER_ULONG(&hcOp->PortRegister[PortNum-1].ul, port.ul);

    //
    // wait 50 ms
    //
    KeStallExecutionProcessor(50000);

    port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[PortNum-1].ul);

    if (port.PortReset){

        //
        // Device didn't reset if PortRest bit isn't clear
        //
        ntStatus = STATUS_DEVICE_ENUMERATION_ERROR;
        goto EHCIResetDeviceEnd;
    }

EHCIResetDeviceEnd:

    return ntStatus;

}

NTSTATUS
__stdcall
EHCIDumpResetPipe(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext,
    __in UCHAR EndpointAddress
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    USB_DEFAULT_PIPE_SETUP_PACKET   setupPacket;
    PHYSICAL_ADDRESS                physcratch;
    ULONG                           i;
    HC_QTD_TOKEN                    token;

    //
    // Initialize the setup packet for the Clear Feature Endpoint Stall
    // request.
    //
    EHCIDUMP_INIT_SETUP_PACKET(setupPacket,
                               USB_REQUEST_CLEAR_FEATURE,    // bRequest
                               BMREQUEST_HOST_TO_DEVICE,     // Dir
                               BMREQUEST_TO_ENDPOINT,        // Recipient
                               BMREQUEST_STANDARD,           // Type
                               USB_FEATURE_ENDPOINT_STALL,   // wValue
                               EndpointAddress,              // wIndex
                               0);                           // wLength

    physcratch.QuadPart = 0;

    ntStatus = EHCIDumpControlTransfer(DumpContext,
                                       (PUCHAR)&setupPacket,
                                       physcratch,
                                       0);

    //
    // The data toggle field needs to be updated when ever pipe reset happens
    //
    for (i=1; i<DUMP_MAX_ENDPOINT_COUNT ; i++) {
        if ((ULONG)(EndpointAddress&USB_ENDPOINT_ADDRESS_MASK) == DumpEndpoint[i].qh->HwQH.EpChars.EndpointNumber) {
            token = DumpEndpoint[i].qh->HwQH.Overlay.qTD.Token;
            token.DataToggle = 0;
            DumpEndpoint[i].qh->HwQH.Overlay.qTD.Token = token;
        }
    }

    return ntStatus;
}

NTSTATUS
__stdcall
EHCIDumpResetDevice(
    __inout PEHCI_CRASHDUMP_CONTEXT DumpContext
    )
{
    NTSTATUS                        ntStatus;
    PUCHAR                          base;
    HCLENGTHVERSION                 hcLengthVersion;
    PHC_CAPABILITIES_REGISTER       hcCap;
    ULONG                           portNum;
    ULONG                           portDepth;

    ntStatus = STATUS_SUCCESS;
    //
    // Need to reset parent hub (root or external) port that the storage
    // device is plugged into.
    //
    // Get the capabilities register and read the length
    base = DumpContext->BaseRegisterPointer;
    hcCap = (PHC_CAPABILITIES_REGISTER)base;
    hcLengthVersion.ul = READ_REGISTER_ULONG(&hcCap->HcLengthVersion.ul);

    //
    // First check if the device is still there. If not fail
    //
    if (hcLengthVersion.ul == EHCI_HARDWARE_GONE) {
        ntStatus = STATUS_DEVICE_ENUMERATION_ERROR;
        goto EHCIResetDeviceEnd;
    }

    if (NT_SUCCESS(ntStatus)) {

        portDepth = DumpContext->PortPath.PortPathDepth;

        if (portDepth == 0) {
            //
            // No support for Root hub device embedded in host controller
            //
            ntStatus = STATUS_DEVICE_ENUMERATION_ERROR;
            goto EHCIResetDeviceEnd;

        }

        portNum = DumpContext->PortPath.PortPath[portDepth-1];
        if (portDepth == 1) {

            ntStatus = EHCIDumpResetRHPort(DumpContext,
                                           portNum);

            if (!NT_SUCCESS(ntStatus)) {
                //
                // At this point the only thing to do would be to reset the EHCI
                // controller and reenum the storage boot device
                //
                goto EHCIResetDeviceEnd;
            }

        } else {

            ntStatus = EHCIDumpResetExternalHubPort(DumpContext,
                                                    portNum);

            if (!NT_SUCCESS(ntStatus)) {
                //
                // At this point the only thing to do would be to reset the EHCI
                // controller's root hub port (or reset the EHCI controller) and reenum
                // the external hubs and storage boot device
                //
                goto EHCIResetDeviceEnd;
            }
        }

        //
        // Port reset doesn't reset the queuehead DeviceAddress
        //
        DumpEndpoint[DumpControl].qh->HwQH.EpChars.DeviceAddress = 0;

        //
        // Call into controller independent reset device function
        //
        ntStatus = UsbDumpResetDevice(DumpContext,
                                      EHCIDumpControlTransfer,
                                      &DumpEndpoint[DumpControl].qh->HwQH.EpChars.ul);

        if (!NT_SUCCESS(ntStatus)) {

            goto EHCIResetDeviceEnd;
        }

        //
        // We need to initialize the QH and TD's all over again
        // This would prevent us from using a stale toggle bit
        // and failing the dump.
        //
        EHCIDumpControllerInitialize(DumpContext);
    }

EHCIResetDeviceEnd:

    return ntStatus;
}

NTSTATUS
EHCIDumpEnableAsyncSchedule(
    PHC_OPERATIONAL_REGISTER    OperationalRegister
    )
/*++
Routine Description:

Arguments:

Return Value:

--*/
{
    USBCMD cmd;
    USBSTS sts;
    ULONG stallcount;

    cmd.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbCommand.ul);
    sts.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbStatus.ul);

    if (cmd.AsyncScheduleEnable == 0) {

        //
        // Enhanced Host Controller Interface Specification
        // Revision: 1.0, Section 4.8 Asynchronous Schedule:
        //
        // "Software must not modify the Asynchronous Schedule
        // Enable bit unless the value of the Asynchronous
        // Schedule Enable bit equals that of the Asynchronous
        // Schedule Status bit."
        //

        for (stallcount = 0;stallcount < DUMP_SCHEDULE_RETRY_COUNT; stallcount++) {
            if (cmd.AsyncScheduleEnable == sts.AsyncScheduleStatus) {
                break;
            } else {
                KeStallExecutionProcessor(25);

                cmd.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbCommand.ul);
                sts.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbStatus.ul);
            }
        }

        if (stallcount >= DUMP_SCHEDULE_RETRY_COUNT) {
            return STATUS_IO_DEVICE_ERROR;
        }
    }

    cmd.AsyncScheduleEnable = 1;
    cmd.IntOnAsyncAdvanceDoorbell = 1;

    WRITE_REGISTER_ULONG(&OperationalRegister->UsbCommand.ul, cmd.ul);

    sts.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbStatus.ul);
    for (stallcount = 0;stallcount < DUMP_SCHEDULE_RETRY_COUNT; stallcount++) {
        if (cmd.AsyncScheduleEnable == sts.AsyncScheduleStatus) {
            break;
        } else {
            KeStallExecutionProcessor(25);

            sts.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbStatus.ul);
        }
    }
    if (stallcount == DUMP_SCHEDULE_RETRY_COUNT) {
        return STATUS_IO_DEVICE_ERROR;
    }

    return STATUS_SUCCESS;
}


NTSTATUS
EHCIDumpDisableSchedule(
    PHC_OPERATIONAL_REGISTER    OperationalRegister
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    USBCMD cmd;
    USBSTS sts;
    ULONG stallcount;

    cmd.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbCommand.ul);
    sts.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbStatus.ul);


    if (cmd.PeriodicScheduleEnable == 1) {

        //
        // Enhanced Host Controller Interface Specification
        // Revision: 1.0, Section 4.6 Periodic Schedule
        //
        // "Software must not modify the Periodic Schedule Enable bit
        // unless the value of the Periodic Schedule Enable bit
        // equals that of the Periodic Schedule Status bit."
        //

        for (stallcount = 0; stallcount < DUMP_SCHEDULE_RETRY_COUNT; stallcount++) {
            if (cmd.PeriodicScheduleEnable == sts.PeriodicScheduleStatus) {
                break;
            } else {
                KeStallExecutionProcessor(25);
                cmd.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbCommand.ul);
                sts.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbStatus.ul);
            }
        }
        if (stallcount >= DUMP_SCHEDULE_RETRY_COUNT) {
            return STATUS_IO_DEVICE_ERROR;
        }
    }

    if (cmd.AsyncScheduleEnable == 1) {
        //
        // Enhanced Host Controller Interface Specification
        // Revision: 1.0, Section 4.8 Asynchronous Schedule:
        //
        // "Software must not modify the Asynchronous Schedule
        // Enable bit unless the value of the Asynchronous
        // Schedule Enable bit equals that of the Asynchronous
        // Schedule Status bit."
        //

        for (stallcount = 0; stallcount < DUMP_SCHEDULE_RETRY_COUNT; stallcount++) {
            if (cmd.AsyncScheduleEnable == sts.AsyncScheduleStatus) {
                break;
            } else {
                KeStallExecutionProcessor(25);
                cmd.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbCommand.ul);
                sts.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbStatus.ul);
            }
        }
        if (stallcount >= DUMP_SCHEDULE_RETRY_COUNT) {
            return STATUS_IO_DEVICE_ERROR;
        }
    }

    cmd.PeriodicScheduleEnable = 0;
    cmd.AsyncScheduleEnable = 0;
    cmd.IntOnAsyncAdvanceDoorbell = 0;

    WRITE_REGISTER_ULONG(&OperationalRegister->UsbCommand.ul, cmd.ul);

    sts.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbStatus.ul);
    for (stallcount = 0;stallcount < DUMP_SCHEDULE_RETRY_COUNT; stallcount++) {
        if (cmd.AsyncScheduleEnable == sts.AsyncScheduleStatus &&
            cmd.PeriodicScheduleEnable == sts.PeriodicScheduleStatus) {
            break;
        } else {
            KeStallExecutionProcessor(25);

            sts.ul = READ_REGISTER_ULONG(&OperationalRegister->UsbStatus.ul);
        }
    }
    if (stallcount == DUMP_SCHEDULE_RETRY_COUNT) {
        return STATUS_IO_DEVICE_ERROR;
    }

    return STATUS_SUCCESS;

}

