/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    usbport.c

Abstract:

    Port driver for USB host controllers

Environment:

    kernel mode only

Notes:

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs

--*/

#include "common.h"

#ifdef USBPORT_WPP
#include "usbport.tmh"
#endif


NTSTATUS
DriverEntry(
    __in struct _DRIVER_OBJECT *DriverObject,
    __in PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    Installable driver initialization entry point.
    This entry point is called directly by the I/O system.

Arguments:

    DriverObject - pointer to the driver object

    RegistryPath - pointer to a unicode string representing the path
                   to driver-specific key in the registry

Return Value:

    NT status code

--*/
{
    // This function is never called
    return STATUS_SUCCESS;
}


NTSTATUS
USBPORT_StopDevice(
     PDEVICE_OBJECT FdoDeviceObject
     )
/*++

Routine Description:

    Stop the port and miniport in the context of a IRP_MN_STOP_DEVICE, IRP_MN_SURPRISE_REMOVAL,
    IRP_MN_REMOVE_DEVICE

    Stop and remove treated the same in the port driver. However, during a stop we must remain capable
    of being started again.  All function drivers should have received the STOP or S_REMOVE message
    so we can block on outstanding requests.  All device handles should be released at this point.

    All hardware and memory resources are freed on a stop.


Arguments:

    FdoDeviceObject - DeviceObject of the controller to stop

Return Value:

    NT status code.

--*/
{
    PDEVICE_EXTENSION devExt = NULL;
    KIRQL irql;
    PDEVICE_OBJECT rhPdo;
    LONG ioc;
    ULONG contextIdx;

    UNREFERENCED_PARAMETER(ioc);

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(FdoDeviceObject, LOG_PNP, 'stpD', 0, 0, 0);

#ifdef EN_TEST_CODE
    // bug #1645530
    USBPORT_StopDevice_Delay(FdoDeviceObject);
#endif //EN_TEST_CODE

    USBPORT_FlushCachedRegistryKeys(FdoDeviceObject);

    rhPdo = USBPORT_RefRootHubPdo(FdoDeviceObject, REF_RhPDO_Stop);

    if (rhPdo != NULL) {
        USBPORT_RootHub_RemoveDevice(FdoDeviceObject, rhPdo);
        USBPORT_DerefRootHubPdo(FdoDeviceObject, rhPdo, REF_RhPDO_Stop);
    }


    // If we still have active transfers pending we cannot yet disable
    // the interrupt, the DPCs, and the DM timer.  Transfer completion
    // processing relies on the execution of the interrupt, the DPCs,
    // and the DM timer.
    //
    // Endpoint state transistion processing also relies on the
    // execution of the interrupt, the DPCs, and the DM timer.
    //
    // Synchronously abort all transfers on all devices and close all
    // pipes on all devices before continuing to disable the interrupt,
    // the DPCs, and the DM timer.
    //
    USBPORT_AbortAllTransfersAndCloseAllPipes(FdoDeviceObject,
                                              Ev_RemoveControllerStopping);


    // dh deleted list should be empty by now, this routine will deal with
    // the leaks.
    USBPORT_ScanDeviceHandleDeletedList(FdoDeviceObject);

    // make sure all lists are empty and all endpoints
    // are freed before terminating the worker thread

    USBPORT_Core_WaitPendingTransfers(FdoDeviceObject);

    // all device handle activity should be complete
    // all of our lists should be empty

    USBPORT_ASSERT(IsListEmpty(&devExt->Fdo.EpNeoStateChangeList) == TRUE);

    //
    // Added for Wireless USB
    // On shutdown, there may be some operations that the miniport needs to do
    // while interrupts are still enabled.
    //
    if (!devExt->Fdo.Flags.ControllerGone && (devExt->Fdo.MpStateFlags & MP_STATE_STARTED)) {

        MPf_StopControllerWithInterrupts(devExt);

    }

    //disable transfer Dpcs
    USBPORT_XdpcDisableAll(FdoDeviceObject);

    if (devExt->Fdo.MpStateFlags & MP_STATE_STARTED) {
        BOOLEAN hwPresent = TRUE;
        // stop the miniport, disable interrupts
        if ((devExt->Fdo.HcResources.Flags & HCR_IRQ) &&
            devExt->Fdo.Flags.IrqConnected) {
            MP_DisableInterrupts(FdoDeviceObject);
        }
        devExt->Fdo.MpStateFlags &= ~MP_STATE_STARTED;

        USBPORT_ACQUIRE_DM_LOCK(devExt, irql);

        if (devExt->Fdo.Flags.ControllerGone) {
            hwPresent = FALSE;
            devExt->Fdo.DmTimerFlags.DecodesOff = 1;
        }

        USBPORT_RELEASE_DM_LOCK(devExt, irql);

        MPf_StopController(devExt, hwPresent);
    }

    // stop our deadman timer
    USBPORT_StopDM_Timer(FdoDeviceObject);

    // see if we have an interrupt
    // if so disconnect it


    if (devExt->Fdo.Flags.IrqConnected) {
        LONG pnpIrqState;

        IoDisconnectInterrupt(devExt->Fdo.InterruptObject);
        LOGENTRY(FdoDeviceObject, LOG_MISC, 'IOCd', 0, 0, 0);
        devExt->Fdo.InterruptObject = NULL;
        devExt->Fdo.Flags.IrqConnected = 0;
        devExt->Fdo.HcResources.InterruptObject = NULL;

         // state = disconnected (-1);
        pnpIrqState = InterlockedDecrement(&devExt->Fdo.IoConnectInterruptState);
        if (pnpIrqState == -1) {
            // dpc not queued, running
            USBPORT_Core_DecIoRequestCount(FdoDeviceObject, &devExt->Fdo.UsbIoIsrDpc, TRUE);
        }
    }

    if (devExt->Fdo.Flags.PnpIoStartRef) {
        // Ref From _MN_START, UsbIoTag_PnpStart
        devExt->Fdo.Flags.PnpIoStartRef = 0;
        USBPORT_Core_DecIoRequestCount(FdoDeviceObject, &devExt->UsbIoPnpStop, TRUE);
        KeWaitForSingleObject(&devExt->IoRequestStopEvent,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);
    }
#if DBG
    ioc = USBPORT_Core_GetIoRequestCount(FdoDeviceObject);
    USBPORT_ASSERT(ioc == 0);
#endif

    if ((NULL != devExt->Fdo.HcResources.DeviceRegisters) &&
        ((devExt->Fdo.HcResources.Flags & HCR_MAP_IO_REGS) ||
         (devExt->Fdo.HcResources.Flags & HCR_MEM_REGS))) {
        MmUnmapIoSpace(devExt->Fdo.HcResources.DeviceRegisters,
                       devExt->Fdo.HcResources.DeviceRegistersLength);
        devExt->Fdo.HcResources.DeviceRegisters = NULL;
        devExt->Fdo.HcResources.DeviceRegistersLength = 0;
    }

    // free any common buffer we allocated for the miniport
    USBPORT_FreeControllerCommonBuffers(FdoDeviceObject);

    if (devExt->Fdo.AdapterObject) {
        if (devExt->Fdo.AdapterObject != devExt->Fdo.CommonBufferAdapterObject) {
            (devExt->Fdo.AdapterObject->DmaOperations->PutDmaAdapter)
                    (devExt->Fdo.AdapterObject);
        }
        devExt->Fdo.AdapterObject = NULL;
    }

    if (devExt->Fdo.CommonBufferAdapterObject) {
        (devExt->Fdo.CommonBufferAdapterObject->DmaOperations->PutDmaAdapter)
            (devExt->Fdo.CommonBufferAdapterObject);
        devExt->Fdo.CommonBufferAdapterObject = NULL;
    }

    // delete the legacy HCD symbolic link
    if (devExt->Fdo.Flags.LegacySymbolicLink == 1) {
        IoDeleteSymbolicLink(&devExt->Fdo.LegacyLinkUnicodeString);
        RtlFreeUnicodeString(&devExt->Fdo.LegacyLinkUnicodeString);
        devExt->Fdo.Flags.LegacySymbolicLink = 0;
    }

    // free address table
    if (devExt->Fdo.Flags.UsbAddressInit == 1) {
        UsbFreePool(devExt->Fdo.AddressList);
        devExt->Fdo.AddressList = NULL;
        devExt->Fdo.Flags.UsbAddressInit = 0;
    }

    // disable the HCD symbolic link
    USBPORT_DisableDeviceInterface(devExt);
    //if (TEST_FLAG(devExt->Flags, USBPORT_FLAG_IFACE_ENABLE)) {
    //    // return value ignored!
    //    IoSetDeviceInterfaceState(&devExt->SymbolicLinkName, FALSE);
    //     CLEAR_FLAG(devExt->Flags, USBPORT_FLAG_IFACE_ENABLE);
    //}

    if (devExt->Fdo.Flags.FdoRegistered) {
        if (USBPORT_IS_USB20(devExt)) {
            USBPORT_DeregisterUSB2fdo(FdoDeviceObject);
        } else {
            USBPORT_DeregisterUSB1fdo(FdoDeviceObject);
        }
    }

    if (devExt->Fdo.PciBusInterface.Size ==
        sizeof(devExt->Fdo.PciBusInterface)) {
        devExt->Fdo.PciBusInterface.InterfaceDereference(
            devExt->Fdo.PciBusInterface.Context);
        RtlZeroMemory(&devExt->Fdo.PciBusInterface,
                      sizeof(devExt->Fdo.PciBusInterface));
    }

    for (contextIdx = 0; contextIdx < DUMP_MAX_CONTEXT_COUNT; contextIdx++) {
        if (devExt->DumpContext[contextIdx] != NULL) {

            ExFreePool(devExt->DumpContext[contextIdx]);
            devExt->DumpContext[contextIdx] = NULL;
        }
    }

    //
    // Unregister with WMI
    //
    USBPORT_WmiUnRegisterLog(FdoDeviceObject);

    if (devExt->Fdo.AcpiDeviceInformation) {
        UsbFreePool(devExt->Fdo.AcpiDeviceInformation);
        devExt->Fdo.AcpiDeviceInformation = NULL;
    }

    return STATUS_SUCCESS;
}


NTSTATUS
USBPORT_DeferIrpCompletion(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PVOID Context
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PKEVENT event = Context;
    KeSetEvent(event, 1, FALSE);
    return STATUS_MORE_PROCESSING_REQUIRED;
}


VOID
USBPORT_FreeControllerCommonBuffers(
    PDEVICE_OBJECT HcFdo
    )
/*++

Routine Description:

    Frees all controller common buffers and re-inits the device extension.

Arguments:

Return Value:



--*/
{
    PDEVICE_EXTENSION devExt = NULL;
    ULONG idx;
    PLIST_ENTRY cbListEntry = NULL;
    PUSBPORT_COMMON_BUFFER cb = NULL;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    if (NULL != devExt->Fdo.ControllerCommonBuffer) {
        USBPORT_HalFreeCommonBuffer(HcFdo, devExt->Fdo.ControllerCommonBuffer);
        devExt->Fdo.ControllerCommonBuffer = NULL;
    }

    if (NULL != devExt->Fdo.ScratchCommonBuffer) {
        USBPORT_HalFreeCommonBuffer(HcFdo, devExt->Fdo.ScratchCommonBuffer);
        devExt->Fdo.ScratchCommonBuffer = NULL;
    }

    for (idx=0; idx<USB_MAX_COMMON_BUFFER_BLOCKS; idx++) {
        if (NULL != devExt->Fdo.MpCommonBufferBlock[idx]) {
            USBPORT_HalFreeCommonBuffer(HcFdo, devExt->Fdo.MpCommonBufferBlock[idx]);
            devExt->Fdo.MpCommonBufferBlock[idx] = NULL;
        }
    }

    while (!IsListEmpty(&devExt->Fdo.FreeSmallCommonBufferList)) {

        cbListEntry = RemoveHeadList(&devExt->Fdo.FreeSmallCommonBufferList);

        cb = CONTAINING_RECORD(cbListEntry,
                               USBPORT_COMMON_BUFFER,
                               FreeListEntry);

        USBPORT_HalFreeCommonBuffer(HcFdo, cb);
    }

    while (!IsListEmpty(&devExt->Fdo.FreeLargeCommonBufferList)) {

        cbListEntry = RemoveHeadList(&devExt->Fdo.FreeLargeCommonBufferList);

        cb = CONTAINING_RECORD(cbListEntry,
                               USBPORT_COMMON_BUFFER,
                               FreeListEntry);

        USBPORT_HalFreeCommonBuffer(HcFdo, cb);
    }
}


NTSTATUS
USBPORT_AllocateControllerCommonBuffers(
    PDEVICE_OBJECT HcFdo,
    PHC_RESOURCES HcResources
    )
/*++

Routine Description:

    Allocate a set of common buffer blocks for the USB host controller.  This routine will allocate the following blocks:

    buffer                      size
    _______
    ControllerCommonBuffer      (regpacket)CommonBufferBytes
    ScratchCommonBuffer         USB_PAGE_SIZE - sizeof(USBPORT_COMMON_BUFFER)
    CommonBufferBlocks[]        (regpacket)CommonBufferBytes

Arguments:

Return Value:

    Returns STATUS_SUCCESS if all blocks are allocated successfuly

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION devExt = NULL;
    ULONG idx;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    // initialize FDO
    devExt->Fdo.ScratchCommonBuffer = NULL;
    devExt->Fdo.ControllerCommonBuffer = NULL;
    for (idx=0; idx<USB_MAX_COMMON_BUFFER_BLOCKS; idx++) {
        devExt->Fdo.MpCommonBufferBlock[idx] = NULL;
    }

    // allocation loop, break on error
    nts = STATUS_SUCCESS;

    do {
        ULONG cBytes;
        PUSBPORT_COMMON_BUFFER cb;

        //ControllerCommonBuffer
        cBytes = REGISTRATION_PACKET(devExt).CommonBufferBytes;
        // skip request for zero bytes
        if (cBytes) {
            cb = USBPORT_HalAllocateCommonBuffer(HcFdo, cBytes);
            if (cb == NULL) {
                nts = STATUS_INSUFFICIENT_RESOURCES;
                break;
            }
            devExt->Fdo.ControllerCommonBuffer = cb;
            HcResources->CommonBufferVa = cb->MiniportVa;
            HcResources->CommonBufferPhys = cb->MiniportPhys;
        }

        //ScratchCommonBuffer
        cBytes = USB_PAGE_SIZE - sizeof(USBPORT_COMMON_BUFFER);
        cb = USBPORT_HalAllocateCommonBuffer(HcFdo, cBytes);
        if (cb == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }
        devExt->Fdo.ScratchCommonBuffer = cb;


        //CommonBufferBlocks
        if (MPx_MpRevision(devExt) >= USB_MINIPORT_HCI_VERSION_3 ) {

            ULONG nBlocks = REGISTRATION_PACKET(devExt).CommonBufferBlockCount;

            USBPORT_ASSERT(nBlocks <= USB_MAX_COMMON_BUFFER_BLOCKS);

            if (nBlocks >USB_MAX_COMMON_BUFFER_BLOCKS) {
                nBlocks = USB_MAX_COMMON_BUFFER_BLOCKS;
            }

            HcResources->CommonBufferBlockCount = nBlocks;

            for (idx=0; idx < nBlocks; idx++) {

                cBytes = REGISTRATION_PACKET(devExt).CommonBufferBlockBytes[idx];
                // skip request for zero bytes
                if (cBytes) {
                    cb = USBPORT_HalAllocateCommonBuffer(HcFdo, cBytes);

                    if (cb == NULL) {
                        nts = STATUS_INSUFFICIENT_RESOURCES;
                        break;
                    }
                    devExt->Fdo.MpCommonBufferBlock[idx] = cb;
                    HcResources->CommonBufferBlocksVa[idx] = cb->MiniportVa;
                    HcResources->CommonBufferBlocksPhys[idx] = cb->MiniportPhys;
                }
            }

        }

    } WHILE (0);

    // cleanup on error
    if (NT_ERROR(nts)) {
        USBPORT_FreeControllerCommonBuffers(HcFdo);
    }

    return nts;

}


NTSTATUS
USBPORT_QueryCapabilities(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_CAPABILITIES DeviceCapabilities
    )
/*++

Routine Description:

Arguments:

Return Value:

    ntstatus

--*/

{
    PIRP irp = NULL;
    KEVENT event;
    NTSTATUS ntStatus = STATUS_BOGUS;
    PDEVICE_EXTENSION devExt = NULL;
    PIO_STACK_LOCATION nextStack = NULL;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // init the caps structure before calldown
    RtlZeroMemory(DeviceCapabilities, sizeof(DEVICE_CAPABILITIES));
    DeviceCapabilities->Size = sizeof(DEVICE_CAPABILITIES);
    DeviceCapabilities->Version = 1;
    DeviceCapabilities->Address = (ULONG)-1;
    DeviceCapabilities->UINumber = (ULONG)-1;

    irp = IoAllocateIrp(devExt->Fdo.TopOfStackDeviceObject->StackSize + 1, FALSE);
    if (!irp) {
        DbgTrace((UsbpDebugTrace, "failed to allocate Irp\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    } else {
        // All PnP IRP's need the Status field initialized
        // to STATUS_NOT_SUPPORTED before calldown
        irp->IoStatus.Status = STATUS_NOT_SUPPORTED;

        nextStack = IoGetNextIrpStackLocation(irp);
        USBPORT_ASSERT(nextStack != NULL);
        nextStack->MajorFunction = IRP_MJ_PNP;
        nextStack->MinorFunction = IRP_MN_QUERY_CAPABILITIES;

        KeInitializeEvent(&event, NotificationEvent, USBD_EVENT_STATE_NOT_SIGNALED);

        IoSetCompletionRoutine(irp,
                               USBPORT_DeferIrpCompletion,
                               &event,
                               TRUE,
                               TRUE,
                               TRUE);

        nextStack->Parameters.DeviceCapabilities.Capabilities = DeviceCapabilities;
        ntStatus = IoCallDriver(devExt->Fdo.TopOfStackDeviceObject, irp);
        if (ntStatus == STATUS_PENDING) {
           // wait for irp to complete
           KeWaitForSingleObject(
                &event,
                Suspended,
                KernelMode,
                FALSE,
                NULL);
           ntStatus = irp->IoStatus.Status;
        }
        IoFreeIrp(irp);
    }

    return ntStatus;
}

NTSTATUS
USBPORT_GetUrsDeviceInfo(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    This routine sends the IOCTL_INTERNAL_URS_GET_DEVICE_INFORMATION which can be used to get
    details of the underlying bus when the host controller stack is enumerated by the USB Role
    Switch driver.

--*/
{
    PURS_DEVICE_INFORMATION ursDeviceInfo;
    PACPI_DEVICE_INFORMATION_OUTPUT_BUFFER acpiDeviceInfo;
    PURS_PCI_DEVICE_INFORMATION pciDeviceInfo;
    ULONG outputSize;
    IO_STATUS_BLOCK ioBlock = {0};
    KEVENT event;
    NTSTATUS status;
    PIRP irp;
    ULONG numErrors = 0;
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    outputSize = sizeof(URS_DEVICE_INFORMATION);

    for (numErrors = 0; numErrors < 2; ++numErrors) {

        ALLOC_POOL_Z(ursDeviceInfo, NonPagedPoolNx, outputSize);

        if (ursDeviceInfo == NULL) {
            LOGENTRY(FdoDeviceObject, LOG_PNP, 'URS1', FdoDeviceObject, devExt, 0);
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto USBPORT_GetUrsDeviceInfo_Done;
        }

        KeInitializeEvent(&event, SynchronizationEvent, FALSE);

        irp = IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_URS_GET_DEVICE_INFORMATION,
                                            devExt->Fdo.TopOfStackDeviceObject,
                                            NULL,
                                            0,
                                            ursDeviceInfo,
                                            outputSize,
                                            TRUE,
                                            &event,
                                            &ioBlock);

        if (irp == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LOGENTRY(FdoDeviceObject, LOG_PNP, 'URS2', FdoDeviceObject, status, 0);
            goto USBPORT_GetUrsDeviceInfo_Done;
        }

        status = IoCallDriver(devExt->Fdo.TopOfStackDeviceObject, irp);
        if (status == STATUS_PENDING) {
            KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
            status = ioBlock.Status;
        }

        if (status == STATUS_BUFFER_OVERFLOW) {

            LOGENTRY(FdoDeviceObject, LOG_PNP, 'URS3', FdoDeviceObject, ioBlock.Information, numErrors+1);
            outputSize = ursDeviceInfo->Size;
            UsbFreePool(ursDeviceInfo);
            ursDeviceInfo = NULL;
            continue;
        }

        break;
    }

    if (!NT_SUCCESS(status)) {
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'URS4', FdoDeviceObject, status, 0);
        goto USBPORT_GetUrsDeviceInfo_Done;
    }

    //
    // Populate information about the underlying bus. Most of the rest of the code can
    // operate under the assumption that the controller is either PCI or ACPI enumerated,
    // and ignore the fact that it is URS enumerated.
    //

    switch(ursDeviceInfo->BusType) {

    case UrsBusTypePci:
        pciDeviceInfo = &(ursDeviceInfo->PciDeviceInformation);
        devExt->Fdo.BusNumber = pciDeviceInfo->BusNumber;
        devExt->Fdo.BusDevice = pciDeviceInfo->DeviceNumber;
        devExt->Fdo.BusFunction = pciDeviceInfo->FunctionNumber;
        RtlCopyMemory(&(devExt->Fdo.PciBusInterface),
                      &(pciDeviceInfo->BusInterfaceStandard),
                      sizeof(BUS_INTERFACE_STANDARD));
        break;

    case UrsBusTypeAcpi:
        acpiDeviceInfo = &(ursDeviceInfo->AcpiDeviceInformation.AcpiDeviceInformationOutput);
        ALLOC_POOL_Z(devExt->Fdo.AcpiDeviceInformation,
                     NonPagedPoolNx,
                     acpiDeviceInfo->Size);

        if (devExt->Fdo.AcpiDeviceInformation == NULL) {
            LOGENTRY(FdoDeviceObject, LOG_PNP, 'URS5', FdoDeviceObject, devExt, 0);
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto USBPORT_GetUrsDeviceInfo_Done;
        }

        RtlCopyMemory(devExt->Fdo.AcpiDeviceInformation,
                      acpiDeviceInfo,
                      acpiDeviceInfo->Size);
        devExt->Fdo.AcpiDeviceInformationSize = acpiDeviceInfo->Size;

        devExt->Fdo.DisablePciConfigSpace = 1;
        devExt->Fdo.HcResources.DisablePciConfigSpace = 1;

        USBPORT_SetSpecialAcpiDevFunc(devExt);
        break;

    default:

        NT_ASSERT(FALSE);
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'URS6', FdoDeviceObject, ursDeviceInfo->BusType, 0);
        status = STATUS_NOT_IMPLEMENTED;
        goto USBPORT_GetUrsDeviceInfo_Done;
    }

USBPORT_GetUrsDeviceInfo_Done:

    if (ursDeviceInfo) {
        UsbFreePool(ursDeviceInfo);
    }

    if (!NT_SUCCESS(status) && devExt->Fdo.AcpiDeviceInformation) {
        UsbFreePool(devExt->Fdo.AcpiDeviceInformation);
        devExt->Fdo.AcpiDeviceInformation = NULL;
        devExt->Fdo.AcpiDeviceInformationSize = 0;
    }

    return status;
}

#define EXPECTED_IOCTL_ACPI_GET_DEVICE_INFORMATION_STRING_LENGTH 100
NTSTATUS
USBPORT_GetAcpiDeviceInfo(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    This routine sends the IOCTL_ACPI_GET_DEVICE_INFORMATION

Arguments:


Return Value:

    ntstatus

--*/

{
    PACPI_DEVICE_INFORMATION_OUTPUT_BUFFER output;
    ULONG outputSize;
    IO_STATUS_BLOCK ioBlock = {0};
    KEVENT event;
    NTSTATUS status;
    PIRP irp;
    ULONG numErrors = 0;
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    outputSize = sizeof(ACPI_DEVICE_INFORMATION_OUTPUT_BUFFER)
                 + EXPECTED_IOCTL_ACPI_GET_DEVICE_INFORMATION_STRING_LENGTH;

    for (numErrors = 0; numErrors < 2; numErrors++) {

        ALLOC_POOL_Z(output, NonPagedPoolNx, outputSize);

        if (output == NULL) {
            LOGENTRY(FdoDeviceObject, LOG_PNP, 'ACP1', FdoDeviceObject, devExt, 0);
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        //
        // Build the request for ACPI to provide the device information
        //

        KeInitializeEvent(&event, SynchronizationEvent, FALSE);

        irp = IoBuildDeviceIoControlRequest(IOCTL_ACPI_GET_DEVICE_INFORMATION,
                                            devExt->Fdo.TopOfStackDeviceObject,
                                            NULL,
                                            0,
                                            output,
                                            outputSize,
                                            FALSE,
                                            &event,
                                            &ioBlock);

        if (irp == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LOGENTRY(FdoDeviceObject, LOG_PNP, 'ACP2', FdoDeviceObject, status, 0);
            break;
        }

        //
        // Pass request to the underlying (ACPI) bus driver and wait for the IRP
        // to be completed.
        //

        status = IoCallDriver(devExt->Fdo.TopOfStackDeviceObject, irp);
        if (status == STATUS_PENDING) {
            KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
            status = ioBlock.Status;
        }

        if (status == STATUS_BUFFER_OVERFLOW) {

            LOGENTRY(FdoDeviceObject, LOG_PNP, 'ACP3', FdoDeviceObject, ioBlock.Information, numErrors+1);

            UsbFreePool(output);
            output = NULL;
            outputSize = (ULONG) ioBlock.Information;
            continue;
        }

        break;
    }

    if (NT_SUCCESS(status)) {
        devExt->Fdo.AcpiDeviceInformation = output;
        devExt->Fdo.AcpiDeviceInformationSize = (ULONG) ioBlock.Information;
    } else {
        if (output) {
            UsbFreePool(output);
        }
    }

    return status;
}

NTSTATUS
USBPORT_StartDevice(
     PDEVICE_OBJECT FdoDeviceObject,
     PHC_RESOURCES HcResources
     )
/*++

Routine Description:

    Start the port and miniport

Arguments:

    FdoDeviceObject - DeviceObject of the controller to start
    HcResources - Alloted to us

Return Value:

    NT status code.

--*/
{
    ULONG i = 0;
    ULONG legsup = 0;
    ULONG tmpLength = 0;
    ULONG keyLenBytes = 0;
    // default to enabled
    ULONG gDisableSS = 0;
    ULONG classCodeRev = 0;
    ULONG busBandwidth = 0;
    ULONG mpOptionFlags = 0;
    ULONG gDisableCCDetect = 0;
    ULONG gForceHcD3NoWakeArm = 0;
    ULONG commonBuffer2GBLimit = 0;
    ULONG forceHCResetOnResume = 0;
    ULONG fastResumeEnable = 0;

    BOOLEAN isCC = FALSE;
    NTSTATUS ntStatus = STATUS_BOGUS;
    POWER_STATE newState;
    PDEVICE_EXTENSION devExt = NULL;
    DEVICE_DESCRIPTION devDesc;
    USB_MINIPORT_STATUS mpStatus = USBMP_STATUS_SUCCESS;

    LOGENTRY(FdoDeviceObject, LOG_PNP, 'SRT>', FdoDeviceObject, 0, 0);

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // re-init some state vars
    devExt->Fdo.Flags.ul = 0;

    if (devExt->Fdo.PowerFlags.RhS3mappedToS4) {
        //
        // Preserve the RhS3mappedToS4 value in the case root hub PDO
        // already exists and USBPORT_ComputeRootHubDeviceCaps() is not
        // called again.  This happens in the START_DEVICE, STOP_DEVICE,
        // START_DEVICE rebalance case.
        //
        devExt->Fdo.PowerFlags.ul = 0;
        devExt->Fdo.PowerFlags.RhS3mappedToS4 = 1;
    } else {
        devExt->Fdo.PowerFlags.ul = 0;
    }
    devExt->Fdo.DmTimerFlags.ul = 0;
    devExt->Fdo.NextTransferSequenceNumber = 0;

    devExt->Fdo.MpSuspendThread = NULL;
    devExt->Fdo.MpResumeThread = NULL;

    // update power vars
    devExt->Fdo.LastSystemSleepState = PowerSystemUnspecified;
    devExt->Fdo.CurrentSystemPowerState = PowerSystemWorking;

    devExt->Dx_State = newState.DeviceState = PowerDeviceD0;
    PoSetPowerState(FdoDeviceObject, DevicePowerState, newState);

    // fetch the global keys from the registry
    // ignore return value!
    USBPORT_GetGlobalHacks(FdoDeviceObject,
                           &gDisableSS,
                           &gDisableCCDetect,
                           &devExt->Fdo.PMDebug,
                           &gForceHcD3NoWakeArm
               );

    //
    // Add ForceHcD3NoWakeArm to the FDO Extension
    //
    devExt->Fdo.PowerFlags.ForceHcD3NoWakeArm = gForceHcD3NoWakeArm;

    if (gDisableSS) {
        devExt->Fdo.SSP_Support = SSPsupport_GlobalDisabled;
    }
    // check reg for SS flag, note that miniport can still override it
    if (devExt->Fdo.SSP_Support == SSPsupport_RegDefined) {
        if (USBPORT_SelectiveSuspendRegistry(FdoDeviceObject)) {
            devExt->Fdo.SSP_Support = SSPsupport_RegEnabled;
        } else {
            devExt->Fdo.SSP_Support = SSPsupport_RegDisabled;
        }
    }

    // check for common buffer allocation 2GB limit
    keyLenBytes = sizeof(PDOKEY_COMMON_BUFFER_2GB_LIMIT);
    USBPORT_GetRegistryKeyValueForPdo(devExt->HcFdoDeviceObject,
                                      devExt->Fdo.PhysicalDeviceObject,
                                      USBPORT_SW_BRANCH,
                                      PDOKEY_COMMON_BUFFER_2GB_LIMIT,
                                      keyLenBytes,
                                      &commonBuffer2GBLimit,
                                      sizeof(commonBuffer2GBLimit));
    devExt->Fdo.CommonBuffer2GBLimit = commonBuffer2GBLimit ? 1 : 0;
    DbgTrace((UsbpDebugTrace, "common buffer 2GB limit %d\n", commonBuffer2GBLimit));

    // check for forced reset of HC on resume
    keyLenBytes = sizeof(PDOKEY_FORCE_HC_RESET_ON_RESUME);
    USBPORT_GetRegistryKeyValueForPdo(devExt->HcFdoDeviceObject,
                                      devExt->Fdo.PhysicalDeviceObject,
                                      USBPORT_SW_BRANCH,
                                      PDOKEY_FORCE_HC_RESET_ON_RESUME,
                                      keyLenBytes,
                                      &forceHCResetOnResume,
                                      sizeof(forceHCResetOnResume));
    devExt->Fdo.ForceHCResetOnResume = forceHCResetOnResume ? 1 : 0;
    DbgTrace((UsbpDebugTrace, "force HC reset on resume %d\n", forceHCResetOnResume));

    // Check for Fast S0 Resume Enable
    //
    keyLenBytes = sizeof(PDOKEY_FAST_RESUME_ENABLE);
    USBPORT_GetRegistryKeyValueForPdo(devExt->HcFdoDeviceObject,
                                      devExt->Fdo.PhysicalDeviceObject,
                                      USBPORT_SW_BRANCH,
                                      PDOKEY_FAST_RESUME_ENABLE,
                                      keyLenBytes,
                                      &fastResumeEnable,
                                      sizeof(fastResumeEnable));
    devExt->Fdo.FastResumeEnable = fastResumeEnable ? 1 : 0;

    if (HcResources->DetectedLegacyBIOS) {
        devExt->Fdo.Flags.LegacyBIOS = 1;
        legsup = 1;
    } else {
        legsup = 0;
    }
    keyLenBytes = sizeof(SYM_LEGSUP_KEY);
    USBPORT_SetRegistryKeyValueForPdo(devExt->Fdo.PhysicalDeviceObject,
                                      USBPORT_HW_BRANCH,
                                      REG_DWORD,
                                      SYM_LEGSUP_KEY,
                                      keyLenBytes,
                                      &legsup,
                                      sizeof(legsup));

    mpOptionFlags = devExt->Fdo.HcOptionFlags;
    LOGENTRY(FdoDeviceObject, LOG_PNP, 'mpOP',
        mpOptionFlags, 0, HcResources->Flags);

    if (USBPORT_IsEnumeratorEqual(devExt, URS_ENUMERATOR_NAME)) {

        devExt->Fdo.IsUrsEnumerated = TRUE;
        ntStatus = USBPORT_GetUrsDeviceInfo(FdoDeviceObject);
        if (!NT_SUCCESS(ntStatus)) {
            goto Exit;
        }
    }

    if (TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_NO_PCI_CONFIG)){
        devExt->Fdo.HcResources.DisablePciConfigSpace = 1;
    }

    if(0 == devExt->Fdo.HcResources.DisablePciConfigSpace){

        // Query the PCI Bus Interface used by USBPORT_ReadWriteConfigSpace()
        // If we are URS enumerated, we have already obtained this information.
        if (!(devExt->Fdo.IsUrsEnumerated)) {
            ntStatus = USBPORT_QueryPciBusInterface(FdoDeviceObject);
            if (!NT_SUCCESS(ntStatus)) {
                goto Exit;
            }
        }

        // fetch the Dev Prod and Rev IDs from config space
        ntStatus = USBPORT_ReadConfigSpace(FdoDeviceObject,
                                           &devExt->Fdo.PciVendorId,
                                           0,
                                           sizeof(devExt->Fdo.PciVendorId));
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'Gvid',
            devExt->Fdo.PciVendorId, 0, ntStatus);
        if (!NT_SUCCESS(ntStatus)) {
            goto Exit;
        }

        ntStatus = USBPORT_ReadConfigSpace(FdoDeviceObject,
                                           &devExt->Fdo.PciDeviceId,
                                           2,
                                           sizeof(devExt->Fdo.PciDeviceId));
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'Gdid',
            devExt->Fdo.PciDeviceId, 0, ntStatus);
        if (!NT_SUCCESS(ntStatus)) {
            goto Exit;
        }

        ntStatus = USBPORT_ReadConfigSpace(FdoDeviceObject,
                                           &classCodeRev,
                                           8,
                                           sizeof(ULONG));
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'Grev',
            classCodeRev, 0, ntStatus);
        if (!NT_SUCCESS(ntStatus)) {
            goto Exit;
        }

        devExt->Fdo.PciRevisionId = (UCHAR)classCodeRev;
        devExt->Fdo.PciClass      = (UCHAR)(classCodeRev >> 24);
        devExt->Fdo.PciSubClass   = (UCHAR)(classCodeRev >> 16);
        devExt->Fdo.PciProgIf     = (UCHAR)(classCodeRev >>  8);

        if (devExt->Fdo.PciClass == PCI_CLASS_SERIAL_BUS_CTLR &&
            devExt->Fdo.PciSubClass == PCI_SUBCLASS_SB_USB) {
            // attempt to detect companion controllers if class and subclass match USB

            //USBPORT_ASSERT(devExt->Fdo.PciClass == PCI_CLASS_SERIAL_BUS_CTLR);
            //USBPORT_ASSERT(devExt->Fdo.PciSubClass == PCI_SUBCLASS_SB_USB);
            USBPORT_ASSERT(devExt->Fdo.PciProgIf == 0x00 ||
                           devExt->Fdo.PciProgIf == 0x10 ||
                           devExt->Fdo.PciProgIf == 0x20);

            DbgTrace((UsbpDebugTrace, "USB Controller VID %x DEV %x REV %x\n",
                      devExt->Fdo.PciVendorId, devExt->Fdo.PciDeviceId,
                      devExt->Fdo.PciRevisionId));

            // set the HW flavor so that the miniports and possibly
            // the port know what 'special things' to do to make the controller
            // be good.
            HcResources->ControllerFlavor =
                devExt->Fdo.HcFlavor =
                USBPORT_GetHcFlavor(FdoDeviceObject,
                                    devExt->Fdo.PciVendorId,
                                    devExt->Fdo.PciDeviceId,
                                    devExt->Fdo.PciRevisionId);

            // If this is an OHCI or UHCI controller check if it is a CC
            if ((devExt->Fdo.PciClass == PCI_CLASS_SERIAL_BUS_CTLR) &&
                (devExt->Fdo.PciSubClass == PCI_SUBCLASS_SB_USB) &&
                (devExt->Fdo.PciProgIf < 0x20) &&
                NT_SUCCESS(USBPORT_IsCompanionController(FdoDeviceObject, &isCC))) {

                if (isCC) {
                    devExt->Fdo.Flags.IsCompanionController = 1;
                } else {
                    devExt->Fdo.Flags.IsCompanionController = 0;
                }
            }

            // global flag overrides registry settings from inf and
            // any hard coded detection
            if (gDisableCCDetect) {
                devExt->Fdo.Flags.IsCompanionController = 0;
            }

            if (devExt->Fdo.Flags.IsCompanionController) {
                DbgTrace((UsbpDebugTrace,"Is CC %04X %04X %02X  %02X %02X %02X\n",
                          devExt->Fdo.PciVendorId, devExt->Fdo.PciDeviceId,
                          devExt->Fdo.PciRevisionId, devExt->Fdo.PciClass,
                          devExt->Fdo.PciSubClass, devExt->Fdo.PciProgIf));
            } else {
                DbgTrace((UsbpDebugTrace,"Is not CC %04X %04X %02X  %02X %02X %02X\n",
                          devExt->Fdo.PciVendorId, devExt->Fdo.PciDeviceId,
                          devExt->Fdo.PciRevisionId, devExt->Fdo.PciClass,
                    devExt->Fdo.PciSubClass, devExt->Fdo.PciProgIf));
            }



































































        } else {

            // classify as generic
            devExt->Fdo.HcFlavor = USB_HcGeneric;

            DbgTrace((UsbpDebugTrace,"Is not USB controller %04X %04X %02X  %02X %02X %02X\n",
                      devExt->Fdo.PciVendorId, devExt->Fdo.PciDeviceId,
                      devExt->Fdo.PciRevisionId, devExt->Fdo.PciClass,
                      devExt->Fdo.PciSubClass, devExt->Fdo.PciProgIf));

            // if it not USB it follows that it is not a companion
            devExt->Fdo.Flags.IsCompanionController = 0;
        }
    }
    else{
        //
        // No PCI bus, classify as generic EHCI for now
        //
        RtlZeroMemory(&devExt->Fdo.PciBusInterface,
                             sizeof(devExt->Fdo.PciBusInterface));

        if (devExt->Fdo.IsUrsEnumerated) {
            //
            // We have already obtained the ACPI device information.
            //
            ntStatus = STATUS_SUCCESS;

        } else {
            ntStatus = USBPORT_GetAcpiDeviceInfo(FdoDeviceObject);
        }

        if (!NT_SUCCESS(ntStatus)) {
            goto Exit;
        } else {
            HcResources->AcpiDeviceInformation = devExt->Fdo.AcpiDeviceInformation;
            HcResources->AcpiDeviceInformationSize = devExt->Fdo.AcpiDeviceInformationSize;
        }

        HcResources->ControllerFlavor =
            devExt->Fdo.HcFlavor =
            USBPORT_GetHcFlavor_FromACPI(FdoDeviceObject,
                                         devExt->Fdo.AcpiDeviceInformation);










        devExt->Fdo.Flags.IsCompanionController = 0;

        devExt->Fdo.PciVendorId = 0xffff;
        devExt->Fdo.PciDeviceId = 0xffff;
        devExt->Fdo.PciRevisionId = (UCHAR)0xff;
        devExt->Fdo.PciClass      = (UCHAR)0xff;
        devExt->Fdo.PciSubClass   = (UCHAR)0xff;
        devExt->Fdo.PciProgIf     = (UCHAR)0xff;

    }

    // bw
    devExt->Fdo.TotalBusBandwidth = REGISTRATION_PACKET(devExt).BusBandwidth;

    if(USBPORT_IS_USB20(devExt)){
        // Update the 2.0 bandwidth to reflect the amount used by the budgeter
        devExt->Fdo.TotalBusBandwidth = USBPORT_GetTotal20Bandwidth(FdoDeviceObject);
    }

    // enable transfer DPCs
    USBPORT_XdpcEnableAll(FdoDeviceObject);

    // allow a registry override of the total BW for this bus
    busBandwidth = devExt->Fdo.TotalBusBandwidth;
    keyLenBytes = sizeof(BW_KEY);
    USBPORT_GetRegistryKeyValueForPdo(devExt->HcFdoDeviceObject,
                                      devExt->Fdo.PhysicalDeviceObject,
                                      USBPORT_SW_BRANCH,
                                      BW_KEY,
                                      keyLenBytes,
                                      &busBandwidth,
                                      sizeof(busBandwidth));
    if (busBandwidth != devExt->Fdo.TotalBusBandwidth) {
        USBPORT_KdPrint((1, "Warning: Registry Override of bus bandwidth\n"));
        devExt->Fdo.TotalBusBandwidth = busBandwidth;
    }

    if(!USBPORT_IS_USB20(devExt)){
        // init the bandwidth table
        for (i=0; i<USBPORT_MAX_INTEP_POLLING_INTERVAL; i++) {
            devExt->Fdo.BusBandwidthTable[i] = devExt->Fdo.TotalBusBandwidth -
                                            devExt->Fdo.TotalBusBandwidth/10;
        }
    }


    if (devExt->Fdo.HcResources.DisablePciConfigSpace) {
        // arbitrarily pick a bus number
        devExt->Fdo.BusNumber = 0;

    } else if (!(devExt->Fdo.IsUrsEnumerated)) {

        //
        // If we are URS enumerated, this information has already been populated.
        //

        ntStatus = IoGetDeviceProperty(devExt->Fdo.PhysicalDeviceObject,
                                       DevicePropertyBusNumber,
                                       sizeof(devExt->Fdo.BusNumber),
                                       &devExt->Fdo.BusNumber,
                                       &tmpLength);
        if (!NT_SUCCESS(ntStatus)) {
            goto Exit;
        }
    }

    // query our device caps from the PDO and save them
    ntStatus = USBPORT_QueryCapabilities(FdoDeviceObject,
                                         &devExt->DeviceCapabilities);
    if (!NT_SUCCESS(ntStatus)) {
        goto Exit;
    }

    // extract device and function number from the caps. If we are URS enumerated, this
    // information has already been populated.
    if((0 == devExt->Fdo.HcResources.DisablePciConfigSpace) &&
       (!(devExt->Fdo.IsUrsEnumerated))) {
        devExt->Fdo.BusDevice = devExt->DeviceCapabilities.Address>>16;
        devExt->Fdo.BusFunction = devExt->DeviceCapabilities.Address & 0x0000ffff;
    }
    DbgTrace((UsbpDebugTrace, "'BUS %x, device %x, function %x\n",
        devExt->Fdo.BusNumber, devExt->Fdo.BusDevice, devExt->Fdo.BusFunction));


    // Host Controller identification data for EtwEvents
    //
    devExt->Fdo.EtwHcDevice.DeviceObject =  FdoDeviceObject;
    devExt->Fdo.EtwHcDevice.PciBus =        devExt->Fdo.BusNumber;
    devExt->Fdo.EtwHcDevice.PciDevice =     (USHORT)devExt->Fdo.BusDevice;
    devExt->Fdo.EtwHcDevice.PciFunction =   (USHORT)devExt->Fdo.BusFunction;
    devExt->Fdo.EtwHcDevice.PciVendorId =   devExt->Fdo.PciVendorId;
    devExt->Fdo.EtwHcDevice.PciDeviceId =   devExt->Fdo.PciDeviceId;

    // got the capabilities, compute the power state table
    USBPORT_ComputeHcPowerStates(FdoDeviceObject,
                                 &devExt->DeviceCapabilities,
                                 &devExt->Fdo.HcPowerStateTbl);

    // make sure we got all the resources we need
    // note that we check here becuase problems may occurr
    // if we attempt to connect the interrupt without all
    // the necessary resources

    if (TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_POLL_CONTROLLER)) {
        devExt->Fdo.Flags.PollController = 1 ;
    }

    if (TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_NO_SS)) {
        // host controller driver does not do global suspend
        devExt->Fdo.SSP_Support = SSPsupport_HcDisabled;
    }

    if ((mpOptionFlags & USB_MINIPORT_OPT_NEED_IOPORT) &&
        !(HcResources->Flags & HCR_UNMAP_IO_REGS) &&
        !(HcResources->Flags & HCR_MAP_IO_REGS)) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'noRG', 0, 0, 0);
        goto Exit;
    }

    if ((mpOptionFlags & USB_MINIPORT_OPT_NEED_MEMORY) &&
        !(HcResources->Flags & HCR_MEM_REGS)) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'noMM', 0, 0, 0);
        goto Exit;
    }

    // make sure we got an IRQ
    if ((mpOptionFlags & USB_MINIPORT_OPT_NEED_IRQ) &&
        !(HcResources->Flags & HCR_IRQ)) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'noIQ', 0, 0, 0);
        goto Exit;
    }

    // simulates a fairly common scenario where PnP gives us
    // the wrong resources.
    //TEST_PATH(ntStatus, FAILED_NEED_RESOURCE);
    if (!NT_SUCCESS(ntStatus)) {
        goto Exit;
    }

    // Create our adapter object for a standard USB PCI adapter
    if (TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_NO_PNP_RESOURCES)) {
        devExt->Fdo.CommonBufferAdapterObject = NULL;
        devExt->Fdo.AdapterObject = NULL;
        REGISTRATION_PACKET(devExt).CommonBufferBytes = 0;
        goto Skip1;
    }

    // Attempt to query hardware capabilities through the pre start controller
    // callback if one is registered by the miniport.
    //

    mpStatus = MPf_PreStartController(devExt, HcResources, MPF_ON_PNP_THREAD);

    ntStatus = MPSTATUS_TO_NTSTATUS(mpStatus);
    LOGENTRY(FdoDeviceObject, LOG_PNP, 'mpPS', mpStatus, ntStatus, 0);

    if ((!NT_SUCCESS(ntStatus)) && (mpStatus != STATUS_NOT_IMPLEMENTED)) {
        DbgTrace((UsbpDebugTrace, "MP_PreStartController failed\n"));
        goto Exit;
    }

    // Query the capabilities of the host controller which may vary
    // between different instances of host controllers which share the
    // same miniport.
    //

    MPf_GetHcCapabilities(devExt);

    // Attempt to get the DMA adapter to be used for Scatter Gather operations
    // first based on the host controller addressing capabilities.
    //
    // If the DMA Adapter received is V3 which has the AllocateCommonBufferEx
    // API present, use this single DMA adapter for the purposes of both
    // allocating common buffers (within 4GB limit) and for scatter gather
    // operations.
    //

    // set up adapter object
    RtlZeroMemory(&devDesc, sizeof(DEVICE_DESCRIPTION));

    //
    // Try DEVICE_DESCRIPTION_VERSION3 first.
    //
    devExt->Fdo.AdapterObjectDeviceDescriptionVersion = DEVICE_DESCRIPTION_VERSION3;

    devDesc.Version = devExt->Fdo.AdapterObjectDeviceDescriptionVersion;
    devDesc.Master = TRUE;

    devDesc.ScatterGather = TRUE;

    devDesc.InterfaceType = PCIBus;
    devDesc.DmaWidth = Width32Bits;
    devDesc.DmaSpeed = Compatible;

    devDesc.MaximumLength = (ULONG)-1;

    devExt->Fdo.NumberOfMapRegisters = (ULONG)-1;

    if (devExt->Fdo.Bit64AddressingCapability) {

        // Allocate a 64-bit AdapterObject used for GetScatterGatherList().
        //
        devDesc.Dma64BitAddresses = TRUE;
        devDesc.DmaAddressWidth = 64;

    } else {

        // Allocate a 32-bit AdapterObject used for GetScatterGatherList().
        //
        devDesc.Dma32BitAddresses = TRUE;
        devDesc.DmaAddressWidth = 32;
    }

    devExt->Fdo.AdapterObject =
        IoGetDmaAdapter(devExt->Fdo.PhysicalDeviceObject,
                        &devDesc,
                        &devExt->Fdo.NumberOfMapRegisters);

    if (NULL == devExt->Fdo.AdapterObject) {






















        if (NULL == devExt->Fdo.AdapterObject) {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit;
        }
    }

    if (devExt->Fdo.AdapterObjectDeviceDescriptionVersion >= DEVICE_DESCRIPTION_VERSION3) {

        devExt->Fdo.UseSingleDmaAdapter = TRUE;
        devExt->Fdo.CommonBufferAdapterObject = NULL;
        devExt->Fdo.CommonBufferNumberOfMapRegisters = (ULONG)-1;

    } else {

        devExt->Fdo.UseSingleDmaAdapter = FALSE;

        RtlZeroMemory(&devDesc, sizeof(DEVICE_DESCRIPTION));

        devDesc.Version = devExt->Fdo.AdapterObjectDeviceDescriptionVersion;
        devDesc.Master = TRUE;

        devDesc.ScatterGather = TRUE;

        devDesc.Dma32BitAddresses = TRUE;
        devDesc.DmaAddressWidth = 32;

        devDesc.InterfaceType = PCIBus;
        devDesc.DmaWidth = Width32Bits;
        devDesc.DmaSpeed = Compatible;

        // This AdapterObject is only used for AllocateCommonBuffer().
        // It will never be used for GetScatterGatherList().
        // Set MaximumLength to zero so that no map registers are ever
        // allocated for this AdapterObject.
        //
        devDesc.MaximumLength = 0;

        devExt->Fdo.CommonBufferNumberOfMapRegisters = (ULONG)-1;

        // Allocate the AdapterObject used for CommonBuffer allocations.
        //
        devExt->Fdo.CommonBufferAdapterObject =
            IoGetDmaAdapter(devExt->Fdo.PhysicalDeviceObject,
            &devDesc,
            &devExt->Fdo.CommonBufferNumberOfMapRegisters);

        if (NULL == devExt->Fdo.CommonBufferAdapterObject) {

            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit;
        }
    }

    // see if we have an interrupt
    if (HcResources->Flags & HCR_IRQ) {
        IO_CONNECT_INTERRUPT_PARAMETERS interruptParameters;
        RtlZeroMemory(&interruptParameters, sizeof(IO_CONNECT_INTERRUPT_PARAMETERS));
        devExt->Fdo.Flags.IrqConnected = 1;
        // state = connected
        devExt->Fdo.IoConnectInterruptState = 0;
        // pending request for the ISR dpc
        USBPORT_Core_IncIoRequestCount(FdoDeviceObject, &devExt->Fdo.UsbIoIsrDpc, devExt, UsbIoTag_IsrDpc, FALSE);

        //
        // We need to use IoConnectInteruptEx instead of IoConnectInterrupt so that we
        // can call IoReportInterruptActive() and IoReportInterruptInactive() APIs
        //
        interruptParameters.Version                                 = CONNECT_FULLY_SPECIFIED;
        interruptParameters.FullySpecified.PhysicalDeviceObject     = devExt->Fdo.PhysicalDeviceObject;
        interruptParameters.FullySpecified.InterruptObject          = &devExt->Fdo.InterruptObject;
        interruptParameters.FullySpecified.ServiceRoutine           = USBPORT_InterruptService;
        interruptParameters.FullySpecified.ServiceContext           = (PVOID) FdoDeviceObject;
        interruptParameters.FullySpecified.FloatingSave             = FALSE;
        interruptParameters.FullySpecified.SpinLock                 = NULL;
        interruptParameters.FullySpecified.SynchronizeIrql          = HcResources->InterruptLevel;
        interruptParameters.FullySpecified.ShareVector              = HcResources->ShareIRQ;
        interruptParameters.FullySpecified.Vector                   = HcResources->InterruptVector;
        interruptParameters.FullySpecified.Irql                     = HcResources->InterruptLevel;
        interruptParameters.FullySpecified.InterruptMode            = HcResources->InterruptMode;
        interruptParameters.FullySpecified.ProcessorEnableMask      = HcResources->Affinity;

        ntStatus = IoConnectInterruptEx(&interruptParameters);

		LOGENTRY(FdoDeviceObject, LOG_PNP, 'IOCi', 0, 0, ntStatus);
        if (NT_ERROR(ntStatus)) {
            devExt->Fdo.Flags.IrqConnected = 0;
            HcResources->InterruptObject = NULL;

            USBPORT_Core_DecIoRequestCount(FdoDeviceObject, &devExt->Fdo.UsbIoIsrDpc, TRUE);

            goto Exit;
        }
        HcResources->InterruptObject = devExt->Fdo.InterruptObject;
    }

    // miniport common buffers
    ntStatus = USBPORT_AllocateControllerCommonBuffers(FdoDeviceObject, HcResources);
    if (NT_ERROR(ntStatus)) {
        goto Exit;
    }

Skip1:

    // attempt to start the miniport

    mpStatus = MPf_StartController(devExt, HcResources, MPF_ON_PNP_THREAD);

    ntStatus = MPSTATUS_TO_NTSTATUS(mpStatus);
    LOGENTRY(FdoDeviceObject, LOG_PNP, 'mpST', mpStatus, ntStatus, 0);

    if (!NT_SUCCESS(ntStatus)) {
        DbgTrace((UsbpDebugTrace,"MP_StartController failed\n"));
        goto Exit;
    }

    //
    // Device in D0 (called PoSetPowerState earlier)
    // Controller has successfully started,
    // we should be able to register with runtime PM
    //

    ntStatus = USBPORT_RuntimePMRegister(FdoDeviceObject);

    if (!NT_SUCCESS(ntStatus)) {
        goto Exit;
    }

    // controller started, set flag and begin passing
    // interrupts to the miniport
    SET_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_STARTED);
    LOGENTRY(FdoDeviceObject, LOG_PNP, 'eIRQ', mpStatus, 0, 0);
    if (HcResources->Flags & HCR_IRQ) {
        MP_EnableInterrupts(FdoDeviceObject);
    }

    // placing the faiure here simulates a failure from some point above
    //TEST_PATH(ntStatus, FAILED_USBPORT_START);

    if (!NT_SUCCESS(ntStatus)) {
        goto Exit;
    }

    // Start up the 'DEADMAN' timer
    //
    USBPORT_StartDM_Timer(FdoDeviceObject);

    USBPORT_ASSERT(NT_SUCCESS(ntStatus));

    // Ref From _MN_START, UsbIoTag_PnpStart
    if (NT_SUCCESS(ntStatus)) {
        LONG ioc;

        devExt->Fdo.Flags.PnpIoStartRef = 1;
        ioc = USBPORT_Core_IncIoRequestCount(FdoDeviceObject, &devExt->UsbIoPnpStop, devExt, UsbIoTag_PnpStart, FALSE);
        KeResetEvent(&devExt->IoRequestStopEvent);
        USBPORT_ASSERT(ioc >= 1);
    }
Exit:

    LOGENTRY(FdoDeviceObject, LOG_PNP, 'SRT<', ntStatus, 0, 0);
    return ntStatus;
}


NTSTATUS
USBPORT_PassIrp(
    __inout PDEVICE_OBJECT DeviceObject,
    __in_opt PIO_COMPLETION_ROUTINE CompletionRoutine,
    __in_opt PVOID Context,
    __in BOOLEAN InvokeOnSuccess,
    __in BOOLEAN InvokeOnError,
    __in BOOLEAN InvokeOnCancel,
    __in PIRP Irp,
    __inout_opt PUSB_IOREQUEST_CONTEXT UsbIoRequest
    )
/*++

Routine Description:

    Passes an irp to the next lower driver, this is done by the FDO for all PnP Irps.

Arguments:

    Args to IoSetCompletionRoutine

    UsbIoRequest - (optional) released before the call to IoCallDriver unless a completion routine is set

Return Value:

    Return code from IoCallDriver

--*/
{
    NTSTATUS ntStatus = STATUS_BOGUS;
    PDEVICE_EXTENSION devExt = NULL;

    GET_DEVICE_EXT(devExt, DeviceObject);
    ASSERT_FDOEXT(devExt);

    // note that we do not dec the pending request count
    // if we set a completion routine.
    // We do not want to 'go away' before the completion
    // routine is called.

    if (CompletionRoutine) {
        IoCopyCurrentIrpStackLocationToNext(Irp);
        IoSetCompletionRoutine(Irp,
                               CompletionRoutine,
                               Context,
                               InvokeOnSuccess,
                               InvokeOnError,
                               InvokeOnCancel);
    } else {
        IoSkipCurrentIrpStackLocation(Irp);
        if (UsbIoRequest) {

            USBPORT_Core_UnlinkAndFreeIoRequest(UsbIoRequest, Irp);
        }
    }

    ntStatus = IoCallDriver(devExt->Fdo.TopOfStackDeviceObject, Irp);

    return ntStatus;
}






NTSTATUS
USBPORT_DispatchPnP(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

Arguments:

    DeviceObject - pointer to a device object.
    Irp - pointer to an I/O Request Packet.

Return Value:

    NT status value

--*/
{
    NTSTATUS ntStatus = STATUS_BOGUS;
    PDEVICE_EXTENSION devExt = NULL;

    ntStatus = STATUS_BOGUS;
    GET_DEVICE_EXT(devExt, DeviceObject);

    if (USBPORT_DEVICE_EXT_SIG == devExt->Sig) {
        ntStatus = USBPORT_FdoPnP(DeviceObject, Irp);
    }
    else
    if (ROOTHUB_DEVICE_EXT_SIG == devExt->Sig) {
        ntStatus = USBPORT_PdoPnP(DeviceObject, Irp);
    }

    return ntStatus;
}

NTSTATUS
USBPORT_Dispatch(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    Process the IRPs sent to this device.

Arguments:

    DeviceObject - pointer to a device object

    Irp          - pointer to an I/O Request Packet

Return Value:

    NT status code

--*/
{

    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION devExt;
    BOOLEAN forRootHubPdo = FALSE;
    KIRQL irql;
    BOOLEAN pnpRemoved;
    BOOLEAN logPower = FALSE;
    PUSB_IOREQUEST_CONTEXT usbIoRequest = NULL;
    UCHAR power_func;
    ULONG power_data;
    ULONG idx;

    DbgTrace((UsbpDebugTrace, "'enter USBPORT_Dispatch\n"));

    GET_DEVICE_EXT(devExt, DeviceObject);
    irpStack = IoGetCurrentIrpStackLocation (Irp);


    // figure out if this is the FDO for the HC or the PDO
    // for the root hub
    if (devExt->Sig == ROOTHUB_DEVICE_EXT_SIG) {
        forRootHubPdo = TRUE;
        DbgTrace((UsbpDebugTrace, "'USBPORT_Dispatch[IRP->PDO] IRP = %p, stack = %p (func) %x %x\n",
            Irp, irpStack, irpStack->MajorFunction, irpStack->MinorFunction));
        logPower = TRUE;
    } else if (devExt->Sig == USBPORT_DEVICE_EXT_SIG) {
        forRootHubPdo = FALSE;
        DbgTrace((UsbpDebugTrace, "'USBPORT_Dispatch[IRP->FDO] IRP = %p, stack = %p (func) %x %x\n",
            Irp, irpStack, irpStack->MajorFunction, irpStack->MinorFunction));
        logPower = TRUE;
    } else {
        // this is a bug, bugcheck
        DbgTrace((UsbpDebugTrace, "'USBPORT_Dispatch[unknown] IRP = %p, stack = %p (func) %x %x\n",
            Irp, irpStack, irpStack->MajorFunction, irpStack->MinorFunction));
        USBPORT_ASSERTMSG("Invalid devExt->Sig",FALSE);
    }

    //* Log Power Irps for FDO, PDO here
    if (irpStack->MajorFunction == IRP_MJ_POWER && logPower) {
        power_func = irpStack->MinorFunction;
        devExt->CurrentPowerFunc = (ULONG) power_func;

        idx = InterlockedIncrement(&devExt->PowerFuncHistoryIdx);
        idx = idx % USB_PP_HISTORY_LEN;
        devExt->PowerFuncHistory[idx] = (UCHAR) power_func;
        power_data = 0;

        switch (power_func) {
        case IRP_MN_WAIT_WAKE:
            power_data |= 0x80000000;
            power_data |= irpStack->Parameters.WaitWake.PowerState;
            break;
        case IRP_MN_SET_POWER:
        case IRP_MN_QUERY_POWER:
            if (irpStack->Parameters.Power.Type == SystemPowerState) {
                power_data |= 0x80000000;
                power_data |= irpStack->Parameters.Power.State.SystemState;
            } else {
                power_data |= irpStack->Parameters.Power.State.DeviceState;
            }

            break;
        default:
            power_data = 0xffffffff;
        }
        devExt->PowerFuncHistoryData[idx] = power_data;
    }

    // *BEGIN SPECIAL CASE
    // Before doing anything see if this devobj is 'removed'
    // if so do some 'special' handling

    KeAcquireSpinLock(&devExt->IoRequestSpin, &irql);

    switch(USBPORT_GetPnpState(devExt)) {
    case PnpRemoved:
    case PnpSurpriseRemoved:
        pnpRemoved = TRUE;
        break;
    default:
        pnpRemoved = FALSE;
    }
//    if (TEST_FLAG(devExt->PnpStateFlags, USBPORT_PNP_SURPRISE_REMOVED) ||
//        TEST_FLAG(devExt->PnpStateFlags, USBPORT_PNP_REMOVED)) {
    if (pnpRemoved) {

        // someone has managed to call us after the remove

        DbgTrace((UsbpDebugTrace, "'(irp after remove) IRP = %p, DO %p MJx%x MNx%x\n",
            Irp, DeviceObject, irpStack->MajorFunction,
                irpStack->MinorFunction));

        KeReleaseSpinLock(&devExt->IoRequestSpin, irql);

        if (forRootHubPdo) {

            switch(irpStack->MajorFunction) {

            // in the removed state complete all power irps with
            // success, this happens if you suspend with the root
            // hub disbaled
            case IRP_MJ_POWER:
                Irp->IoStatus.Status = ntStatus = STATUS_SUCCESS;
                PoStartNextPowerIrp(Irp);
                IoCompleteRequest(Irp,
                                  IO_NO_INCREMENT);
                goto USBPORT_Dispatch_Done;

            // since the root hub pdo exists even when the device
            // is removed by PnP we still allow irps thru
            default:
                break;
            }

        } else {

            switch(irpStack->MajorFunction) {

            // allow PnP irps even though we are removed
            case IRP_MJ_PNP:
                break;

            case IRP_MJ_POWER:


                Irp->IoStatus.Status = ntStatus = STATUS_DEVICE_REMOVED;
                PoStartNextPowerIrp(Irp);
                IoCompleteRequest(Irp,
                                  IO_NO_INCREMENT);
                goto USBPORT_Dispatch_Done;

            default:
                Irp->IoStatus.Status = ntStatus = STATUS_DEVICE_REMOVED;
                IoCompleteRequest(Irp,
                                  IO_NO_INCREMENT);
                goto USBPORT_Dispatch_Done;
            }
        }

    } else {

        KeReleaseSpinLock(&devExt->IoRequestSpin, irql);

    }
    // *END SPECIAL CASE

    usbIoRequest = USBPORT_Core_AllocIoRequest(DeviceObject, Irp, UsbIoTag_IoIrp);
    if (usbIoRequest == NULL) {
        Irp->IoStatus.Status = ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        IoCompleteRequest(Irp,
                          IO_NO_INCREMENT);
        goto USBPORT_Dispatch_Done;
    }

    switch (irpStack->MajorFunction) {

    case IRP_MJ_CREATE:

        DbgTrace((UsbpDebugTrace, "'IRP_MJ_CREATE\n"));
        USBPORT_Core_IoCompleteIrp(DeviceObject, Irp, usbIoRequest, ntStatus, 0);

        break;

    case IRP_MJ_CLOSE:

        DbgTrace((UsbpDebugTrace, "'IRP_MJ_CLOSE\n"));
        USBPORT_Core_IoCompleteIrp(DeviceObject, Irp, usbIoRequest, ntStatus, 0);

        break;

    case IRP_MJ_INTERNAL_DEVICE_CONTROL:

        DbgTrace((UsbpDebugTrace, "'IRP_MJ_NTERNAL_DEVICE_CONTROL\n"));
        if (forRootHubPdo) {
            ntStatus = USBPORT_PdoInternalDeviceControlIrp(DeviceObject, Irp, usbIoRequest);
        } else {
            ntStatus = USBPORT_FdoInternalDeviceControlIrp(DeviceObject, Irp, usbIoRequest);
        }
        break;

    case IRP_MJ_DEVICE_CONTROL:
        DbgTrace((UsbpDebugTrace, "'IRP_MJ_DEVICE_CONTROL\n"));
        if (forRootHubPdo) {
            ntStatus = USBPORT_PdoDeviceControlIrp(DeviceObject, Irp, usbIoRequest);
        } else {
            ntStatus = USBPORT_FdoDeviceControlIrp(DeviceObject, Irp, usbIoRequest);
        }
        break;

    case IRP_MJ_POWER:
        if (forRootHubPdo) {
            ntStatus = USBPORT_PdoPowerIrp(DeviceObject, Irp, usbIoRequest);
        } else {
            ntStatus = USBPORT_FdoPowerIrp(DeviceObject, Irp, usbIoRequest);
        }
        break;

    case IRP_MJ_SYSTEM_CONTROL:
        if (forRootHubPdo) {
            DbgTrace((UsbpDebugTrace, "'IRP_MJ_SYSTEM_CONTROL\n"));
            ntStatus = Irp->IoStatus.Status;
            USBPORT_Core_IoCompleteIrp(DeviceObject, Irp, usbIoRequest, ntStatus, 0);
        } else {
            DbgTrace((UsbpDebugTrace, "'IRP_MJ_SYSTEM_CONTROL\n"));

            ntStatus = USBPORT_DispatchSystemControl(DeviceObject, Irp, usbIoRequest);
        }
        break;

    default:
        DbgTrace((UsbpDebugTrace, "'unrecognized IRP_MJ_ function (%x)\n", irpStack->MajorFunction));
        ntStatus = STATUS_INVALID_DEVICE_REQUEST;
        USBPORT_Core_IoCompleteIrp(DeviceObject, Irp, usbIoRequest, ntStatus, 0);
    } /* case MJ_FUNCTION */

    DbgTrace((UsbpDebugTrace, "'exit USBPORT_Dispatch 0x%x\n", ntStatus));

USBPORT_Dispatch_Done:

    return ntStatus;
}


NTSTATUS
USBPORT_GetConfigValue(
    __in_z PWSTR ValueName,
    __in ULONG ValueType,
    __in_bcount_opt(ValueLength) PVOID ValueData,
    __in ULONG ValueLength,
    __in_opt PVOID Context,
    __in_opt PVOID EntryContext
    )
/*++

Routine Description:

    This routine is a callback routine for RtlQueryRegistryValues
    It is called for each entry in the Parameters
    node to set the config values. The table is set up
    so that this function will be called with correct default
    values for keys that are not present.

Arguments:

    ValueName - The name of the value (ignored).
    ValueType - The type of the value
    ValueData - The data for the value.
    ValueLength - The length of ValueData.
    Context - A pointer to the CONFIG structure.
    EntryContext - The index in Config->Parameters to save the value.

Return Value:

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    // Until WPP is fixed we cannot trace here since this can be called before DriverEntry
    //DbgTrace((UsbpDebugTrace, "'Type 0x%x, Length 0x%x\n", ValueType, ValueLength));

    switch (ValueType) {
    case REG_DWORD:
        if (EntryContext && ValueData && (EntryContext != ValueData)) {
            RtlCopyMemory(EntryContext, ValueData, sizeof(ULONG));
        }
        break;
    case REG_BINARY:
        // BUGBUG we are only set up to read a byte
        if (EntryContext && ValueData && (EntryContext != ValueData)) {
            RtlCopyMemory(EntryContext, ValueData, 1);
        }
        break;
    default:
        ntStatus = STATUS_INVALID_PARAMETER;
    }
    return ntStatus;
}


VOID
USBPORT_Wait(
    PDEVICE_OBJECT FdoDeviceObject,
    ULONG Milliseconds
    )
/*++

Routine Description:

    Synchrounously wait the specified number of miliseconds

Return Value:

    None

--*/
{
    LARGE_INTEGER waitTime, currentTime, endTime, frequency;

    if (Milliseconds == 0) {
        return;
    }

    if (USBPORT_HighResWait(Milliseconds)) {
        return;
    }

    //
    // Compute end of wait
    //
    currentTime = KeQueryPerformanceCounter(&frequency);
    endTime.QuadPart = frequency.QuadPart * Milliseconds;
    endTime.QuadPart = (endTime.QuadPart / 1000) + currentTime.QuadPart;

    //
    // Convert wait time from milliseconds to 100-nanoseconds relative time
    //
    waitTime.QuadPart = Milliseconds * -10000i64;


    do {
        KeDelayExecutionThread(KernelMode, FALSE, &waitTime);

        //
        // See if the end of wait has elapsed
        //
        currentTime = KeQueryPerformanceCounter(&frequency);

        if (currentTime.QuadPart > endTime.QuadPart) {
            break;
        }

        //
        // KeDelayExecutionThread returned early.  Compute remaining time in the wait and
        // delay execution again.
        //
        waitTime.QuadPart = endTime.QuadPart - currentTime.QuadPart;
        frequency.QuadPart = frequency.QuadPart/1000;
        waitTime.QuadPart = waitTime.QuadPart/frequency.QuadPart;

        if (waitTime.QuadPart == 0) {
            //
            // Less than 1ms left in wait; wait at least 1 more millsecond
            //
            waitTime = RtlConvertLongToLargeInteger(-10000);
        } else {
            //
            // Convert remaining time to 100-nanosecond relative time
            //
            waitTime.QuadPart = waitTime.QuadPart * -10000i64;
        }

    } WHILE (1);
}


// 4 byte sync, 4 byte split token, 1 byte EOP, 11 byte ipg, plus
// 4 byte sync, 3 byte regular token, 1 byte EOP, 11 byte ipg
#define HS_SPLIT_SAME_OVERHEAD 39
// 4 byte sync, 4 byte split token, 1 byte EOP, 11 byte ipg, plus
// 4 byte sync, 3 byte regular token, 1 byte EOP, 1 byte bus turn
#define HS_SPLIT_TURN_OVERHEAD 29
// 4 byte sync, 1 byte PID, 2 bytes CRC16, 1 byte EOP, 11 byte ipg
#define HS_DATA_SAME_OVERHEAD 19
// 4 byte sync, 1 byte PID, 2 bytes CRC16, 1 byte EOP, 1 byte bus turn
#define HS_DATA_TURN_OVERHEAD 9
// 4 byte sync, 1 byte PID, 1 byte EOP, 1 byte bus turn
#define HS_HANDSHAKE_OVERHEAD 7

// This could actually be a variable based on an HC implementation
// some measurements have shown 3?us between transactions or about 3% of a
// microframe
// which is about 200+ byte times.  We'll use about half that for budgeting
// purposes.
#define HS_HC_THINK_TIME 100

// 4 byte sync, 3 byte regular token, 1 byte EOP, 11 byte ipg
#define HS_TOKEN_SAME_OVERHEAD 19
// 4 byte sync, 3 byte regular token, 1 byte EOP, 1 byte bus turn
#define HS_TOKEN_TURN_OVERHEAD 9


ULONG
USBPORT_CalculateUsbBandwidth(
    PDEVICE_OBJECT FdoDeviceObject,
    ULONG MaxPacketSize,
    DEVICE_SPEED DeviceSpeed,
    ENDPOINT_TRANSFER_TYPE TransferType,
    ENDPOINT_TRANSFER_DIRECTION Direction
    )
/*++

Routine Description:

    Computes the bandwidth that must be reserved for a given pipe.  This is an
    exact value for USB 1.1 controllers but an approximation for USB 2.0.
    This function is used to return the bandwidth that WOULD be consumed by a
    pipe.

Arguments:

Return Value:

    banwidth required in bits/ms, returns 0 for bulk and control endpoints.

    For high speed devices and for devices thru a TT this calculation is
    approximate since the bandwidth depends on the budget and where the
    endpoint is actually inserted.  The bandwidth value returned by this
    function for a TT is used to report out-of-bandwidth errors thru the UI.

--*/
{
    ULONG bw = 0;
    ULONG overhead = 0;
#define USB_ISO_OVERHEAD_BYTES              9
#define USB_INTERRUPT_OVERHEAD_BYTES        13

    ASSERT_PASSIVE();

    //
    // control, iso, bulk, interrupt
    //

    switch(TransferType) {
    case Bulk:
    case Control:
        overhead = 0;
        break;

    case Isochronous:
        if (DeviceSpeed == HighSpeed) {
            if (Direction == Out) {
                overhead = HS_TOKEN_SAME_OVERHEAD +
                           HS_DATA_SAME_OVERHEAD + HS_HC_THINK_TIME;
            } else {
                overhead = HS_TOKEN_TURN_OVERHEAD +
                           HS_DATA_TURN_OVERHEAD + HS_HC_THINK_TIME;
            }
        } else {
            overhead = USB_ISO_OVERHEAD_BYTES;
        }
        break;

    case Interrupt:
        if (DeviceSpeed == HighSpeed) {
            if (Direction == Out) {
                overhead = HS_TOKEN_SAME_OVERHEAD + HS_DATA_SAME_OVERHEAD +
                           HS_HANDSHAKE_OVERHEAD + HS_HC_THINK_TIME;
            } else {
                overhead = HS_TOKEN_TURN_OVERHEAD + HS_DATA_TURN_OVERHEAD +
                           HS_HANDSHAKE_OVERHEAD + HS_HC_THINK_TIME;
            }
        } else {
            overhead = USB_INTERRUPT_OVERHEAD_BYTES;
        }
        break;
    }


    // Calculate bandwidth based on speed of endpoint,maxpacket

    if (overhead) {
        switch (DeviceSpeed) {

        case FullSpeed:
            //
            // Use this calculation for full and low speed -- this is classic
            // USB 1.1
            //
            // Calculate bandwidth for endpoint.  We will use the
            // approximation: (overhead bytes plus MaxPacket bytes)
            // times 8 bits/byte times worst case bitstuffing overhead.
            // This gives bit times, for low speed endpoints we multiply
            // by 8 again to convert to full speed bits.
            //

            //
            // Figure out how many bits are required for the transfer.
            // (multiply by 7/6 because, in the worst case you might
            // have a bit-stuff every six bits requiring 7 bit times to
            // transmit 6 bits of data.)
            //

            // overhead(bytes) * maxpacket(bytes/ms) * 8
            //      (bits/byte) * bitstuff(7/6) = bits/ms
            bw = ((overhead+MaxPacketSize) * 8 * 7) / 6;

            break;

        case LowSpeed:

            bw = ((overhead+MaxPacketSize) * 8 * 7) / 6;
            // adjust for low speed
            bw *= 8;
            break;

        case HighSpeed:
            bw = MaxPacketSize;

            // Bit stuffing is 16.6666% extra.
            // But we'll calculate bitstuffing as 16% extra with an add of a 4bit
            // shift (i.e.  value + value/16) to avoid floats.
            bw += (bw>>4);

            bw += overhead;
            break;
        }
    }

    return bw;
}


ULONG
USBPORT_CalculateUsbBandwidthEp(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Computes the bandwidth that must be reserved for a
    given endpoint

Arguments:

Return Value:

    banwidth required in bits/ms, returns 0 for bulk
    and control endpoints

--*/
{
    ASSERT_PASSIVE();

    return USBPORT_CalculateUsbBandwidth(FdoDeviceObject,
                                        Endpoint->Parameters.MaxPacketSize,
                                        Endpoint->Parameters.DeviceSpeed,
                                        Endpoint->Parameters.TransferType,
                                        Endpoint->Parameters.TransferDirection);


}


VOID
USBPORT_UpdateAllocatedBw(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    ULONG alloced_bw, i, m;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    m = 0;

    for (i=0; i<USBPORT_MAX_INTEP_POLLING_INTERVAL; i++) {
        alloced_bw = devExt->Fdo.TotalBusBandwidth -
                devExt->Fdo.BusBandwidthTable[i];

        if (alloced_bw > m) {
            m = alloced_bw;
        }
    }

    devExt->Fdo.MaxAllocedBw = m;

    m = devExt->Fdo.TotalBusBandwidth;

    for (i=0; i<USBPORT_MAX_INTEP_POLLING_INTERVAL; i++) {
        alloced_bw = devExt->Fdo.TotalBusBandwidth -
                devExt->Fdo.BusBandwidthTable[i];

        if (alloced_bw < m) {
            m = alloced_bw;
        }
    }

    devExt->Fdo.MinAllocedBw = m;
    if (m == devExt->Fdo.TotalBusBandwidth) {
        devExt->Fdo.MinAllocedBw = 0;
    }
}


BOOLEAN
USBPORT_AllocateBandwidthUSB11(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Computes the best schedule offset for this endpoint
    and allocates the bandwidth.

    Offsets in the bw allocation table orrespond to the values
    in the following tree structure.

    The schedule offset is the best possible posoition for the
    endpoint to reside in the schedule based on its period.

    for 32 ms or mf ep there are 32 possible positions
    for 16 there are 16
    for 8 there are 8
    ...
fr <32>  <16>  <08>   <04>      <02>              <01>


0   ( 0) -\
          ( 0)-\
16  ( 1) -/     \
                ( 0)-\
8   ( 2) -\     /     \
          ( 1)-/       \
24  ( 3) -/             \
                        (0)-\
4   ( 4) -\             /    \
          ( 2)-\       /      \
20  ( 5) -/     \     /        \
                ( 1)-/          \
12  ( 6) -\     /                \
          ( 3)-/                  \
28  ( 7) -/                        \
                                   (0)-\
2   ( 8) -\                        /    \
          ( 4)-\                  /      \
18  ( 9) -/     \                /        \
                ( 2)-\          /          \
10  (10) -\     /     \        /            \
          ( 5)-/       \      /              \
26  (11) -/             \    /                \
                        (1)-/                  \
6   (12) -\             /                       \
          ( 6)-\       /                         \
22  (13) -/     \     /                           \
                ( 3)-/                             \
14  (14) -\     /                                   \
          ( 7)-/                                     \
30  (15) -/                                           \
                                                      (0)
1   (16) -\                                           /
          ( 8)-\                                     /
17  (17) -/     \                                   /
                ( 4)-\                             /
9   (18) -\     /     \                           /
          ( 9)-/       \                         /
25  (19) -/             \                       /
                        (2)-\                  /
5   (20) -\             /    \                /
          (10)-\       /      \              /
21  (21) -/     \     /        \            /
                ( 5)-/          \          /
13  (22) -\     /                \        /
          (11)-/                  \      /
29  (23) -/                        \    /
                                   (1)-/
3   (24) -\                        /
          (12)-\                  /
19  (25) -/     \                /
                ( 6)-\          /
11  (26) -\     /     \        /
          (13)-/       \      /
27  (27) -/             \    /
                        (3)-/
7   (28) -\             /
          (14)-\       /
23  (29) -/     \     /
                ( 7)-/
15  (30) -\     /
          (15)-/
31  (32) -/

Allocations:
    period.offset           table entries
      1                    0, 1, 2.........31

      2.0                  0, 1, 2.........15
      2.1                 16,17,18.........31

      4.0                  0, 1, 2..........7
      4.1                  8, 9,10.........15
      4.2                 16,17,18.........23
      4.3                 24,25,26.........31

      8.0                  0, 1, 2, 3
      8.1                  4, 5, 6, 7
      8.2                  8, 9,10,11
      8.3                 12,13,14,15
      8.4                 16,17,18,19
      8.5                 20,21,22,23
      8.6                 24,25,26,27
      8.7                 28,29,30,31

      ...


frame     nodes visited (period.offset)
    0   32.0   16.0    8.0    4.0    2.0
    1   32.16  16.8    8.4    4.2    2.1
    2   32.8   16.4    8.2    4.1    2.0
    3   32.24  16.12   8.6    4.3    2.1
    4   32.4   16.2    8.1    4.0    2.0
    5   32.20  16.10   8.5    4.2    2.1
    6   32.12  16.6    8.3    4.1    2.0
    7   32.28  16.14   8.7    4.3    2.1

    ...

    all miniports should maintain a 32 entry table for
    interrupt endpoints, the lower 5 bits of the current
    frame are used as an index in to this table. see the
    above graph for index to frame mappings



Arguments:

Return Value:

    FALSE if no bandwidth availble

--*/
{
    PDEVICE_EXTENSION devExt;
    ULONG period, bandwidth, scheduleOffset, i;
    ULONG bestFitBW, min, n;
    LONG bestScheduleOffset;
    BOOLEAN willFit;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ASSERT_ENDPOINT(Endpoint);

    if (Endpoint->Parameters.TransferType == Bulk ||
        Endpoint->Parameters.TransferType == Control ||
        Endpoint->EpFlags.RootHub) {

        // control/bulk come out of our standard 10%
        // and root hub endpoints consume no BW
        Endpoint->Parameters.ScheduleOffset = 0;
        return TRUE;
    }

    // Iso and interrupt -- iso is just like interrupt with
    // a period of 1

    // see if we can fit it

    scheduleOffset = 0;
    period = Endpoint->Parameters.Period;
    USBPORT_ASSERT(period != 0);
    bandwidth = Endpoint->Parameters.Bandwidth;

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'rqBW', scheduleOffset,
            bandwidth, Endpoint);

    // The bandwidth table conatins the bw avaialble for all
    // possible schedule offsets up to the max polling interval
    // we support.

    bestFitBW = 0;
    bestScheduleOffset = -1;

    // scan all possible offsets and select the 'best fit'
    // our goal here is to position the ep in the location
    // with the most free bw

    do {
        // assume it will fit
        willFit = TRUE;
        min = devExt->Fdo.TotalBusBandwidth;
        n = USBPORT_MAX_INTEP_POLLING_INTERVAL/period;

        for (i=0; i<n; i++) {

            if (devExt->Fdo.BusBandwidthTable[n*scheduleOffset+i] < bandwidth) {
                willFit = FALSE;
                break;
            }
            // set min to the lowest free entry for this
            // offset
            if (min > devExt->Fdo.BusBandwidthTable[n*scheduleOffset+i]) {
                min = devExt->Fdo.BusBandwidthTable[n*scheduleOffset+i];
            }
        }

        if (willFit && min > bestFitBW) {
            // it will fit compare this to the we have found
            bestFitBW = min;
            bestScheduleOffset = scheduleOffset;
        }

        scheduleOffset++;

    } while (scheduleOffset < period);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'ckBW', bestScheduleOffset,
            bandwidth, period);

    if (bestScheduleOffset != -1) {

        scheduleOffset = bestScheduleOffset;

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'alBW', scheduleOffset,
            bandwidth, period);

        // we found an offset that will work for the
        // given period, alloc the bw and return the
        // offset

        Endpoint->Parameters.ScheduleOffset =
            scheduleOffset;

        n = USBPORT_MAX_INTEP_POLLING_INTERVAL/period;

        for (i=0; i<n; i++) {

            USBPORT_ASSERT(n*scheduleOffset+i < USBPORT_MAX_INTEP_POLLING_INTERVAL);
            USBPORT_ASSERT(devExt->Fdo.BusBandwidthTable[n*scheduleOffset+i] >= bandwidth);
            devExt->Fdo.BusBandwidthTable[n*scheduleOffset+i] -= bandwidth;

        }

        // update our bw tracking info
        if (Endpoint->Parameters.TransferType == Isochronous) {
            devExt->Fdo.AllocedIsoBW += bandwidth;
        } else {
            USBPORT_GET_BIT_SET(period, n);
            USBPORT_ASSERT(n<6);
            devExt->Fdo.AllocedInterruptBW[n] += bandwidth;
        }
    }

    USBPORT_UpdateAllocatedBw(FdoDeviceObject);

    return bestScheduleOffset == -1 ? FALSE : TRUE;
}


VOID
USBPORT_FreeBandwidthUSB11(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoint
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
    ULONG period, bandwidth, scheduleOffset, i, n;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ASSERT_ENDPOINT(Endpoint);

    if (Endpoint->Parameters.TransferType == Bulk ||
        Endpoint->Parameters.TransferType == Control ||
        Endpoint->EpFlags.RootHub) {
        // these come out of our standard 10%
        return;
    }

    scheduleOffset = Endpoint->Parameters.ScheduleOffset;
    bandwidth = Endpoint->Parameters.Bandwidth;
    period = Endpoint->Parameters.Period;

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'frBW', scheduleOffset, bandwidth, period);

    n = USBPORT_MAX_INTEP_POLLING_INTERVAL/period;

    for (i=0; i<n; i++) {

        USBPORT_ASSERT(n*scheduleOffset+i < USBPORT_MAX_INTEP_POLLING_INTERVAL);
        devExt->Fdo.BusBandwidthTable[n*scheduleOffset+i] += bandwidth;

    }
    //for (i=sheduleOffset; i<USBPORT_MAX_INTEP_POLLING_INTERVAL; i+=period) {
    //    USBPORT_ASSERT(i < USBPORT_MAX_INTEP_POLLING_INTERVAL);
    //    devExt->Fdo.BandwidthTable[i] += bandwidth;
    //}

    // update our bw tracking info
    if (Endpoint->Parameters.TransferType == Isochronous) {
        devExt->Fdo.AllocedIsoBW -= bandwidth;
    } else {
        USBPORT_GET_BIT_SET(period, n);
        USBPORT_ASSERT(n<6);
        devExt->Fdo.AllocedInterruptBW[n] -= bandwidth;
    }

    // update max allocated BW
    USBPORT_UpdateAllocatedBw(FdoDeviceObject);

    return;
}

NTSTATUS
USBPORT_QueryD3ColdInterface(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

Arguments:

    FdoDeviceObject - USB Host Controller FDO for which the PCI Bus
                      Interface is queried.

Return Value:

    Success or failure of the query.

Notes:

--*/
{
    PIRP irp = NULL;
    KEVENT irpCompleted;
    NTSTATUS status = STATUS_BOGUS;
    PDEVICE_OBJECT targetDevice = NULL;
    IO_STATUS_BLOCK statusBlock;
    PDEVICE_EXTENSION devExt = NULL;
    PIO_STACK_LOCATION irpStack = NULL;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    RtlZeroMemory(&devExt->Fdo.D3ColdInterface,
                  sizeof(devExt->Fdo.D3ColdInterface));

    // Initialize the event we'll wait on
    KeInitializeEvent(&irpCompleted, NotificationEvent, USBD_EVENT_STATE_NOT_SIGNALED);

    // Find out where we are sending the irp
    targetDevice = IoGetAttachedDeviceReference(FdoDeviceObject);

    // Get an IRP
    irp = IoBuildSynchronousFsdRequest(IRP_MJ_PNP,
                                       targetDevice,
                                       NULL,            // Buffer
                                       0,               // Length
                                       0,               // StartingOffset
                                       &irpCompleted,
                                       &statusBlock);

    if (!irp) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto USBPORT_QueryD3ColdInterfaceDone;
    }

    irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
    irp->IoStatus.Information = 0;

    // Initialize the stack location
    irpStack = IoGetNextIrpStackLocation(irp);
    irpStack->MinorFunction = IRP_MN_QUERY_INTERFACE;
    irpStack->Parameters.QueryInterface.InterfaceType =
        (LPGUID) &GUID_D3COLD_SUPPORT_INTERFACE;
    irpStack->Parameters.QueryInterface.Size = sizeof(devExt->Fdo.D3ColdInterface);
    irpStack->Parameters.QueryInterface.Version = D3COLD_SUPPORT_INTERFACE_VERSION;
    irpStack->Parameters.QueryInterface.Interface =
        (PINTERFACE)&devExt->Fdo.D3ColdInterface;
    irpStack->Parameters.QueryInterface.InterfaceSpecificData = NULL;

    // Call the driver and wait for completion
    status = IoCallDriver(targetDevice, irp);

    if (status == STATUS_PENDING) {
        KeWaitForSingleObject(&irpCompleted, Executive, KernelMode, FALSE, NULL);
        status = statusBlock.Status;
    }

    if (!NT_SUCCESS(status)) {
        DbgTrace((UsbpDebugTrace,"BUS_INTERFACE_STANDARD query interface failed\n"));
    } else {
        DbgTrace((UsbpDebugTrace,"BUS_INTERFACE_STANDARD query interface succeeded\n"));
    }

USBPORT_QueryD3ColdInterfaceDone:

    // We're done with this device stack
    ObDereferenceObject(targetDevice);

    return status;
}

NTSTATUS
USBPORT_QueryPciBusInterface(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

Arguments:

    FdoDeviceObject - USB Host Controller FDO for which the PCI Bus
                      Interface is queried.

Return Value:

    Success or failure of the query.

Notes:

--*/
{
    PIRP irp = NULL;
    KEVENT irpCompleted;
    NTSTATUS status = STATUS_BOGUS;
    PDEVICE_OBJECT targetDevice = NULL;
    IO_STATUS_BLOCK statusBlock;
    PDEVICE_EXTENSION devExt = NULL;
    PIO_STACK_LOCATION irpStack = NULL;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    RtlZeroMemory(&devExt->Fdo.PciBusInterface,
                  sizeof(devExt->Fdo.PciBusInterface));

    // Initialize the event we'll wait on
    KeInitializeEvent(&irpCompleted, NotificationEvent, USBD_EVENT_STATE_NOT_SIGNALED);

    // Find out where we are sending the irp
    targetDevice = IoGetAttachedDeviceReference(FdoDeviceObject);

    // Get an IRP
    irp = IoBuildSynchronousFsdRequest(IRP_MJ_PNP,
                                       targetDevice,
                                       NULL,            // Buffer
                                       0,               // Length
                                       0,               // StartingOffset
                                       &irpCompleted,
                                       &statusBlock);

    if (!irp) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto USBPORT_QueryPciBusInterfaceDone;
    }

    irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
    irp->IoStatus.Information = 0;

    // Initialize the stack location
    irpStack = IoGetNextIrpStackLocation(irp);
    irpStack->MinorFunction = IRP_MN_QUERY_INTERFACE;
    irpStack->Parameters.QueryInterface.InterfaceType =
        (LPGUID) &GUID_BUS_INTERFACE_STANDARD;
    irpStack->Parameters.QueryInterface.Size = sizeof(BUS_INTERFACE_STANDARD);
    irpStack->Parameters.QueryInterface.Version = 1;
    irpStack->Parameters.QueryInterface.Interface =
        (PINTERFACE)&devExt->Fdo.PciBusInterface;
    irpStack->Parameters.QueryInterface.InterfaceSpecificData = NULL;

    // Call the driver and wait for completion
    status = IoCallDriver(targetDevice, irp);

    if (status == STATUS_PENDING) {
        KeWaitForSingleObject(&irpCompleted, Executive, KernelMode, FALSE, NULL);
        status = statusBlock.Status;
    }

    if (!NT_SUCCESS(status)) {
        DbgTrace((UsbpDebugTrace,"BUS_INTERFACE_STANDARD query interface failed\n"));
    } else {
        DbgTrace((UsbpDebugTrace,"BUS_INTERFACE_STANDARD query interface succeeded\n"));
    }

USBPORT_QueryPciBusInterfaceDone:

    // We're done with this device stack
    ObDereferenceObject(targetDevice);

    return status;
}


NTSTATUS
USBPORT_ReadWriteConfigSpace(
    PDEVICE_OBJECT FdoDeviceObject,
    BOOLEAN Read,
    __in_bcount(Length)
    PVOID Buffer,
    ULONG Offset,
    ULONG Length
    )
/*++

Routine Description:

    This routine reads or writes config space.

    This routine calls GetBusData() or SetBusData() with the DataType
    value PCI_WHICHSPACE_CONFIG so this routine be called at IRQL <= DIRQL.

Arguments:

    FdoDeviceObject
    Read - TRUE if read, FALSE if write.
    Buffer - The info to read or write.
    Offset - The offset in config space to read or write.
    Length - The length to transfer.

Return Value:

    NTSTATUS

--*/
{
    ULONG bytesReadWritten = 0;
    PDEVICE_EXTENSION devExt = NULL;
    PGET_SET_DEVICE_DATA pciGetSetData = NULL;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);


    if(devExt->Fdo.PciBusInterface.Size != sizeof(devExt->Fdo.PciBusInterface)){
        //
        // No PCI config Info, just return
        //
        return STATUS_UNSUCCESSFUL;
    }

    //USBPORT_ASSERT(devExt->Fdo.PciBusInterface.Size ==
                   //sizeof(devExt->Fdo.PciBusInterface));

    if (Read) {
        RtlZeroMemory(Buffer, Length);
        pciGetSetData = devExt->Fdo.PciBusInterface.GetBusData;
    } else {
        pciGetSetData = devExt->Fdo.PciBusInterface.SetBusData;
    }

    bytesReadWritten = (*pciGetSetData)(devExt->Fdo.PciBusInterface.Context,
                                        PCI_WHICHSPACE_CONFIG,
                                        Buffer,
                                        Offset,
                                        Length);

    if (bytesReadWritten == Length) {
        return STATUS_SUCCESS;
    } else {
        return STATUS_UNSUCCESSFUL;
    }
}


VOID
USBPORTSVC_Wait(
    PDEVICE_DATA DeviceData,
    ULONG MillisecondsToWait
    )
/*++

Routine Description:

Arguments:

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT fdoDeviceObject;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);
    fdoDeviceObject = devExt->HcFdoDeviceObject;

    USBPORT_Wait(fdoDeviceObject, MillisecondsToWait);
}


VOID
USBPORT_InvalidateController(
    PDEVICE_OBJECT FdoDeviceObject,
    USB_CONTROLLER_STATE ControllerState
    )
/*++

Routine Description:

Arguments:

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION devExt;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    switch(ControllerState) {
    case UsbMpControllerPowerFault:
        USBPORT_PowerFault(FdoDeviceObject,
                           "Miniport Raised Exception");

        break;

    case UsbMpControllerNeedsHwReset:

        DbgTrace((UsbpDebugTrace,"'<UsbMpControllerNeedsHwReset>\n"));

        usbIoRequest = USBPORT_Core_AllocIoRequest(FdoDeviceObject, NULL, UsbIoTag_HardwareReset);

        if (usbIoRequest) {
            if (!KeInsertQueueDpc(&devExt->Fdo.HcResetDpc, usbIoRequest, 0)) {
                // already queued, drop this ref
                USBPORT_Core_UnlinkAndFreeIoRequest(usbIoRequest, usbIoRequest);
            }
        }
        break;

    case UsbMpControllerNeedsHwResetOnPassive:

        DbgTrace((UsbpDebugTrace,"'<UsbMpControllerNeedsHwReset>\n"));

        usbIoRequest = USBPORT_Core_AllocIoRequest(FdoDeviceObject, NULL, UsbIoTag_HardwareReset);

        if (usbIoRequest) {
            if (0 == InterlockedExchange(&devExt->Fdo.HcResetWIQueued, 1)) {
                USBPORT_QueueWorkItem(FdoDeviceObject,
                                      devExt->Fdo.HcResetWI,
                                      USBPORT_HcResetWI,
                                      DelayedWorkQueue,
                                      usbIoRequest,
                                      TRUE);
            } else {
                // already queued, drop this ref
                USBPORT_Core_UnlinkAndFreeIoRequest(usbIoRequest, usbIoRequest);

            }
        }
        break;

    case UsbMpControllerRemoved:

        // set our flag indicating the controller has been
        // removed and signal the worker thread.
        DbgTrace((UsbpDebugTrace,"'<UsbMpControllerRemoved>\n"));
        if (!devExt->Fdo.Flags.ControllerGone) {
            devExt->Fdo.Flags.ControllerGone = 1;

            usbIoRequest = USBPORT_Core_AllocIoRequest(FdoDeviceObject, NULL, UsbIoTag_SurpriseRemove);
            if (usbIoRequest) {
                if (!KeInsertQueueDpc(&devExt->Fdo.SurpriseRemoveDpc, usbIoRequest, 0)) {
                    // dpc alreay queued
                   USBPORT_Core_UnlinkAndFreeIoRequest(usbIoRequest, usbIoRequest);
                }
            }
        }
        break;

    case UsbMpSimulateInterrupt:
        USBPORT_SimulateInterrupt(FdoDeviceObject);
        break;

    case UsbMpSignalInterrupt:
        USBPORT_SignalInterrupt(FdoDeviceObject);
        break;

    default:
        TEST_TRAP();
    }
}


VOID
USBPORTSVC_InvalidateController(
    PDEVICE_DATA DeviceData,
    USB_CONTROLLER_STATE ControllerState
    )
/*++

Routine Description:

Arguments:

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT fdoDeviceObject;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);
    fdoDeviceObject = devExt->HcFdoDeviceObject;

    USBPORT_InvalidateController(fdoDeviceObject, ControllerState);
}


VOID
USBPORT_HcReset(
    PDEVICE_OBJECT         fdoDeviceObject,
    PUSB_IOREQUEST_CONTEXT usbIoRequest,
    BOOLEAN                DoStagedReset
    )

/*++

Routine Description:

    This routine runs at DISPATCH_LEVEL IRQL.

Arguments:

    Dpc - Pointer to the DPC object.

    DeferredContext - supplies FdoDeviceObject.

    SystemArgument1 - USB_IOREQUEST ref context.

    SystemArgument2 - not used.

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION devExt;
    PLIST_ENTRY listEntry;
    PHCD_ENDPOINT endpoint;
    KIRQL irql;
    ETW_EXECUTION_METRICS   execMetrics;
    LARGE_INTEGER delay;

    ICE_START_TRACE();
    ETW_MEASURE_TIME_START(execMetrics);

    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    devExt->Fdo.StatHardResetCount++;

    // Disconnect all root hub port attached devices.
    //
    USBPORT_HcDisconnectRootHubPorts(fdoDeviceObject);


    // Mark all endpoints as HcReset
    //
    USBPORT_AcquireEpListLock(fdoDeviceObject, &irql);

    listEntry = devExt->Fdo.GlobalEndpointList.Flink;

    while (listEntry != &devExt->Fdo.GlobalEndpointList) {

        endpoint = (PHCD_ENDPOINT) CONTAINING_RECORD(
            listEntry,
            struct _HCD_ENDPOINT,
            GlobalLink);

        if (!endpoint->EpFlags.RootHub) {
            endpoint->EpFlags.HcReset = 1;
        }

        listEntry = endpoint->GlobalLink.Flink;
    }

    USBPORT_ReleaseEpListLock(fdoDeviceObject, irql);


    // Disable controller interrupts, reset the controller, reenable
    // interrupts.
    //
    MP_DisableInterrupts(fdoDeviceObject);

    if (DoStagedReset) {

        //
        // To keep things simple, we are not synchronizing with the ISR using the
        // IoConnectInterruptState variable.
        // Adding 10 ms delay would be ample time to allow the ISR to run. Anyway we are
        // running at passive level right now.
        //
        delay.QuadPart = -10 * 10000; // 10 miliseconds
        SET_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_IN_RESET_RECOVERY);
        KeDelayExecutionThread(KernelMode, FALSE, &delay);
        KeFlushQueuedDpcs();
        MPf_ResetControllerDontReinit(devExt);
        MPf_PostResetController(devExt);
        MPf_ReinitController(devExt);
        CLEAR_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_IN_RESET_RECOVERY);

    } else {
        MPf_ResetController(devExt);
    }

    MP_EnableInterrupts(fdoDeviceObject);


    // Reconnect all root hub port attached devices.
    //
    USBPORT_HcReconnectRootHubPorts(fdoDeviceObject);

    USBPORT_Core_DecIoRequestCount(fdoDeviceObject, usbIoRequest, TRUE);

    ETW_STOP_AND_LOG_TIME(fdoDeviceObject, execMetrics, HCRESETDPC);

    ICE_STOP_TRACE();
}

VOID
USBPORT_HcResetDpc(
    PKDPC Dpc,
    PVOID DeferredContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2
    )

/*++

Routine Description:

    This routine runs at DISPATCH_LEVEL IRQL.

Arguments:

    Dpc - Pointer to the DPC object.

    DeferredContext - supplies FdoDeviceObject.

    SystemArgument1 - USB_IOREQUEST ref context.

    SystemArgument2 - not used.

Return Value:

    None.

--*/
{
    PDEVICE_OBJECT fdoDeviceObject = DeferredContext;
    PUSB_IOREQUEST_CONTEXT usbIoRequest = SystemArgument1;

    USBPORT_HcReset(fdoDeviceObject, usbIoRequest, FALSE);
}

VOID
USBPORT_HcResetWI(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSB_IOREQUEST_CONTEXT UsbIoRequest,
    __in PIO_WORKITEM           IoWorkItem
    )
{

    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    InterlockedExchange(&devExt->Fdo.HcResetWIQueued, 0);
    USBPORT_HcReset(FdoDeviceObject, UsbIoRequest, TRUE);
}

VOID
USBPORT_SurpriseRemoveDpc(
    PKDPC Dpc,
    PVOID DeferredContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2
    )

/*++

Routine Description:

    This routine runs at DISPATCH_LEVEL IRQL.

Arguments:

    Dpc - Pointer to the DPC object.
    DeferredContext - supplies FdoDeviceObject.
    SystemArgument1 - USB_IOREQUEST ref context.
    SystemArgument2 - not used.

Return Value:

    None.

--*/
{
    PDEVICE_OBJECT fdoDeviceObject = DeferredContext;
    PDEVICE_EXTENSION devExt = NULL;
    PUSB_IOREQUEST_CONTEXT usbIoRequest = SystemArgument1;
    ETW_EXECUTION_METRICS   execMetrics;

    ICE_START_TRACE();
    ETW_MEASURE_TIME_START(execMetrics);

    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    IoInvalidateDeviceState(devExt->Fdo.PhysicalDeviceObject);

    USBPORT_Core_UnlinkAndFreeIoRequest(usbIoRequest, usbIoRequest);

    ETW_STOP_AND_LOG_TIME(fdoDeviceObject, execMetrics, SURPRISEREMOVEDPC);
    ICE_STOP_TRACE();
}


VOID
USBPORTSVC_BugCheck(
    PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

Arguments:

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT fdoDeviceObject;
    ULONG vidDev;
    ULONG ilog;
    PLOG_ENTRY lelog;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);
    fdoDeviceObject = devExt->HcFdoDeviceObject;

    vidDev = devExt->Fdo.PciVendorId;
    vidDev <<=16;
    vidDev |= devExt->Fdo.PciDeviceId;

    ilog = devExt->Log.LogIdx;
    ilog &= devExt->Log.LogSizeMask;
    lelog = devExt->Log.LogStart+ilog;

    #pragma prefast(suppress: 28159, "Fatal miniport detected corruption bug requires a bugcheck")
    BUGCHECK_FDO(fdoDeviceObject,
                 USBBUGCODE_MINIPORT_ERROR, // BugCode
                 vidDev,                    // BugCheckParameter2
                 (ULONG_PTR)lelog,          // BugCheckParameter3
                 0,                         // BugCheckParameter4
                 0,                         // DataLength
                 NULL);                     // DataBuffer
}


USB_MINIPORT_STATUS
USBPORTSVC_ReadWriteConfigSpace(
    PDEVICE_DATA DeviceData,
    BOOLEAN Read,
    PVOID Buffer,
    ULONG Offset,
    ULONG Length
    )
/*++

Routine Description:

    Service Exported to miniports for accessing config
    space if needed.

    This service is synchronous and can be called at IRQL <= DIRQL.

    USBUHCI uses this service to clear set the PIRQD bit which
    enables/disables the controller.

Arguments:

Return Value:

    None.

--*/
{
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT fdoDeviceObject;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);
    fdoDeviceObject = devExt->HcFdoDeviceObject;

    ntStatus = USBPORT_ReadWriteConfigSpace(
        fdoDeviceObject,
        Read,
        Buffer,
        Offset,
        Length);

    return USBPORT_NtStatus_TO_MiniportStatus(ntStatus);
}


NTSTATUS
USBPORT_IsCompanionController(
    PDEVICE_OBJECT FdoDeviceObject,
    PBOOLEAN ReturnResult
    )
/*++

Routine Description:

Arguments:

    FdoDeviceObject - USB 1.0 OHCI or UHCI Host Controller FDO for which
                      an associated USB 2.0 EHCI Host Controller is searched.

    ReturnResult - TRUE if the query is successful and an associated USB 2.0
                   EHCI Host Controller is found, otherwise FALSE.

Return Value:

    Success or failure of the query, not the answer to the query.

Notes:

--*/
{
    PIRP irp = NULL;
    KEVENT irpCompleted;
    BOOLEAN result = FALSE;
    NTSTATUS status = STATUS_BOGUS;
    PDEVICE_OBJECT targetDevice = NULL;
    IO_STATUS_BLOCK statusBlock;
    PIO_STACK_LOCATION irpStack = NULL;
    PCI_DEVICE_PRESENT_INTERFACE dpInterface;
    PCI_DEVICE_PRESENCE_PARAMETERS dpParameters;

    ASSERT_PASSIVE();

    // Set the default return value;
    *ReturnResult = FALSE;

    // Initialize the event we'll wait on
    KeInitializeEvent(&irpCompleted, NotificationEvent, USBD_EVENT_STATE_NOT_SIGNALED);

    // Find out where we are sending the irp
    targetDevice = IoGetAttachedDeviceReference(FdoDeviceObject);

    // Get an IRP
    irp = IoBuildSynchronousFsdRequest(IRP_MJ_PNP,
                                       targetDevice,
                                       NULL,            // Buffer
                                       0,               // Length
                                       0,               // StartingOffset
                                       &irpCompleted,
                                       &statusBlock);
    if (!irp) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto USBPORT_IsCompanionControllerDone;
    }

    irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
    irp->IoStatus.Information = 0;

    // Initialize the stack location
    irpStack = IoGetNextIrpStackLocation(irp);
    irpStack->MinorFunction = IRP_MN_QUERY_INTERFACE;
    irpStack->Parameters.QueryInterface.InterfaceType = (LPGUID) &GUID_PCI_DEVICE_PRESENT_INTERFACE;
    irpStack->Parameters.QueryInterface.Version = PCI_DEVICE_PRESENT_INTERFACE_VERSION;
    irpStack->Parameters.QueryInterface.Size = sizeof(PCI_DEVICE_PRESENT_INTERFACE);
    irpStack->Parameters.QueryInterface.Interface = (PINTERFACE)&dpInterface;
    irpStack->Parameters.QueryInterface.InterfaceSpecificData = NULL;

    // Call the driver and wait for completion
    status = IoCallDriver(targetDevice, irp);

    if (status == STATUS_PENDING) {
        KeWaitForSingleObject(&irpCompleted, Executive, KernelMode, FALSE, NULL);
        status = statusBlock.Status;
    }

    if (!NT_SUCCESS(status)) {
        DbgTrace((UsbpDebugTrace,"PCI_DEVICE_PRESENT_INTERFACE query interface failed\n"));
        // Don't have the interface so don't free it.
        goto USBPORT_IsCompanionControllerDone;
    } else {
        DbgTrace((UsbpDebugTrace,"PCI_DEVICE_PRESENT_INTERFACE query interface succeeded\n"));
    }

    if (dpInterface.Size < sizeof(PCI_DEVICE_PRESENT_INTERFACE)) {
        DbgTrace((UsbpDebugTrace,"PCI_DEVICE_PRESENT_INTERFACE old version\n"));
        // Do have the interface so do free it.
        goto USBPORT_IsCompanionControllerFreeInterface;
    }

    // Initialize the Device Presence Parameters
    dpParameters.Size = sizeof(dpParameters);

    // We care about the Class / SubClass / Programming Interface
    // and the search target Function has to be on the same Bus and
    // Device.

    dpParameters.Flags = PCI_USE_CLASS_SUBCLASS | PCI_USE_PROGIF |
                         PCI_USE_LOCAL_BUS | PCI_USE_LOCAL_DEVICE;

    dpParameters.VendorID = 0;      // We don't care.
    dpParameters.DeviceID = 0;      // We don't care.
    dpParameters.RevisionID = 0;    // We don't care.
    dpParameters.SubVendorID = 0;   // We don't care.
    dpParameters.SubSystemID = 0;   // We don't care.
    dpParameters.BaseClass = PCI_CLASS_SERIAL_BUS_CTLR;
    dpParameters.SubClass = PCI_SUBCLASS_SB_USB;
    dpParameters.ProgIf = 0x20;     // USB 2.0 Programming Interface

    // Now call the Device Present Interface to see if the specified
    // device is present.
    result = dpInterface.IsDevicePresentEx(dpInterface.Context,
                                           &dpParameters);

    if (result) {
        DbgTrace((UsbpDebugTrace,"Found EHCI controller for FDO %p\n", FdoDeviceObject));
    } else {
        DbgTrace((UsbpDebugTrace,"Did not find EHCI controller for FDO %p\n", FdoDeviceObject));
    }

    // Set the return value
    *ReturnResult = result;
    status = STATUS_SUCCESS;

USBPORT_IsCompanionControllerFreeInterface:
    // We're done with this interface
    dpInterface.InterfaceDereference(dpInterface.Context);

USBPORT_IsCompanionControllerDone:
    // We're done with this device stack
    ObDereferenceObject(targetDevice);

    return status;
}


USB_CONTROLLER_FLAVOR
USBPORT_GetHcFlavor(
    PDEVICE_OBJECT FdoDeviceObject,
    USHORT PciVendorId,
    USHORT PciProductId,
    UCHAR PciRevision
    )
/*++

Routine Description:

    See if this is some known broken HW.

    We look at the VID,PID and rev in addition to
    reg keys.

Arguments:

Return Value:

    HC Flavor

--*/
{
    ULONG keyLenBytes = 0;
    PDEVICE_EXTENSION devExt = NULL;
    USB_CONTROLLER_FLAVOR flavor;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // set a base type from what the miniport told us
    switch (REGISTRATION_PACKET(devExt).HciType) {
    case USB_OHCI:
        flavor = OHCI_Generic;
        break;
    case USB_UHCI:
        flavor = UHCI_Generic;
        break;
    case USB_EHCI:
        flavor = EHCI_Generic;
        break;
    default:
        flavor = USB_HcGeneric;
    }

    do {
        // checks for flavor HERE

        if (PciVendorId == HC_VID_OPTI &&
            PciProductId == HC_PID_OPTI_HYDRA) {
            flavor = OHCI_Hydra;
            //exit
            break;
        }

        /*
        determine UHCI flavor (Ichxxx) if Intel UHCI based on PID. Currently we only identify PIIX4 thru
        ICH4 due mostly to issues with selective suspend.

        In the pass all unidentified chipsest have been treated as piix4
        */

        // unidentified UHCI
        if (flavor == UHCI_Generic) {

            if (PciVendorId == HC_VID_INTEL) {
                switch(PciProductId) {
                case HC_PID_PIIX3:
                    flavor = UHCI_Piix3;
                    break;

                case HC_PID_PIIX4:
                case HC_PID_PIIX5:
                    flavor = UHCI_Piix4;
                    break;

                case HC_PID_INTEL_ICH1_1:

                // previously not identified
                // treated as Ich1 for hack purposes it is also known as ICH0
                case HC_PID_INTEL_ICH1_2:
                    flavor = UHCI_Ich1;
                    break;

                case HC_PID_INTEL_ICH2_1:
                case HC_PID_INTEL_ICH2_2:
                    flavor = UHCI_Ich2;
                    break;

                case HC_PID_INTEL_ICH3M_1:
                case HC_PID_INTEL_ICH3M_2:
                case HC_PID_INTEL_ICH3M_3:
                    flavor = UHCI_Ich3m;
                    break;

                case HC_PID_INTEL_ICH4_1:
                case HC_PID_INTEL_ICH4_2:
                case HC_PID_INTEL_ICH4_3:
                    flavor = UHCI_Ich4;
                    break;

                default:

                    // treat all eraly revs as piix4
                    if ((PciProductId & 0xF000) == PIIXn_MASK) {
                        flavor = UHCI_Piix4;
                        break;
                    } else {
                        // everything else is 'Intel'
                        flavor = UHCI_Intel;
                    }
                    break;
                }
                //exit
                break;
            }

            // check for VIA UHCI
            if (PciVendorId == HC_VID_VIA &&
                PciProductId == HC_PID_VIA) {
                flavor = UHCI_VIA + PciRevision;
                //exit
                break;
            }
        }

        // unidentified EHCI
        if (flavor == EHCI_Generic) {
            // check for variations of EHCI controllers
            if (PciVendorId == 0x1033) {
                flavor = EHCI_NEC;
                break;
            }

            //if (PciVendorId == HC_VID_INTEL) {
            //    flavor = EHCI_Intel;
            //    break;
            //}

            //if (PciProductId == 0x1033) {
            //    flavor = EHCI_Lucent;
            //}
            break;
        }

    }WHILE (0);

    // hard coded VID PID checks for CCs HERE

    // identify NEC companion controllers by VID/PID
    if (PciVendorId == HC_VID_NEC_CC &&
        PciProductId == HC_PID_NEC_CC &&
        PciRevision == HC_REV_NEC_CC) {

        // this controller used func 2 for the USB 2 controller
        devExt->Fdo.Usb2BusFunction = 2;
        devExt->Fdo.Flags.IsCompanionController = 1;
    }

    // identify VIA companion controllers by VID/PID
    if (PciVendorId == HC_VID_VIA_CC &&
        PciProductId == HC_PID_VIA_CC &&
        PciRevision == HC_REV_VIA_CC) {

        // this controller used func 2 for the USB 2 controller
        devExt->Fdo.Usb2BusFunction = 2;
        devExt->Fdo.Flags.IsCompanionController = 1;
    }


    // identify INTEL companion controllers by VID/PID
    if (PciVendorId == HC_VID_INTEL_CC &&
        (PciProductId == HC_PID_INTEL_CC1 ||
         PciProductId == HC_PID_INTEL_CC2 ||
         PciProductId == HC_PID_INTEL_CC3)) {

        // this controller used func 7 for the USB 2 controller
        devExt->Fdo.Usb2BusFunction = 7;
        devExt->Fdo.Flags.IsCompanionController = 1;
    }
    // now check the registry
    // NOTE: checking the registry last allows this to override
    // any setting from the miniport or the port
    keyLenBytes = sizeof(FLAVOR_KEY);
    USBPORT_GetRegistryKeyValueForPdo(devExt->HcFdoDeviceObject,
                                          devExt->Fdo.PhysicalDeviceObject,
                                          USBPORT_SW_BRANCH,
                                          FLAVOR_KEY,
                                          keyLenBytes,
                                          &flavor,
                                          sizeof(flavor));
    return flavor;
}

USB_CONTROLLER_FLAVOR
USBPORT_GetHcFlavor_FromACPI(
    PDEVICE_OBJECT FdoDeviceObject,
    PACPI_DEVICE_INFORMATION_OUTPUT_BUFFER AcpiDeviceInformation
    )
/*++

Routine Description:





    We look at the ACPI Device information.

Arguments:

Return Value:

    HC Flavor

--*/
{
    ULONG keyLenBytes = 0;
    PDEVICE_EXTENSION devExt = NULL;
    USB_CONTROLLER_FLAVOR flavor;
    PUCHAR hardwareId;
    ULONG hardwareIdLen;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    hardwareId = ((PUCHAR)AcpiDeviceInformation) + AcpiDeviceInformation->VendorIdStringOffset;
    hardwareIdLen = AcpiDeviceInformation->VendorStringLength;





















    } else {

        flavor = EHCI_Generic;

    }


    // now check the registry
    // NOTE: checking the registry last allows this to override
    // any setting from the miniport or the port
    keyLenBytes = sizeof(FLAVOR_KEY);
    USBPORT_GetRegistryKeyValueForPdo(devExt->HcFdoDeviceObject,
                                          devExt->Fdo.PhysicalDeviceObject,
                                          USBPORT_SW_BRANCH,
                                          FLAVOR_KEY,
                                          keyLenBytes,
                                          &flavor,
                                          sizeof(flavor));

    return flavor;
}

ULONG
USBPORT_GetTotal20Bandwidth(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Computes the total 20 bw available in bits/ms.  This is the same value as used
    by the 2.0 scheduling algorithm

--*/
{
    // HS_MAX_PERIDIC_ALLOCATION is in bytes/microframe

    return HS_MAX_PERIODIC_ALLOCATION * 8 * 8;
}

ULONG
USBPORT_ComputeTotalBandwidth(
    PDEVICE_OBJECT FdoDeviceObject,
    PBUS_CONTEXT BusContext
    )
/*++

Routine Description:

    Compute total bandwidth for the physical bus.  All numbers are returned
    as bits/ms.  The numbers reported here must have the same units as the values
    returned by ComputeAllocatedBandwidth and the values reported by the hub
    when we are out of bandwidth.  These units are DIFFERENT from the values used
    by the WMI performance API(which uses bits/32ms).

Arguments:

    BusContext -

Return Value:

    bandwidth availble on this bus

--*/
{
    PDEVICE_EXTENSION devExt;
    ULONG bw;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);
    if (USBPORT_IS_USB20(devExt)) {

        PTRANSACTION_TRANSLATOR translator;
        NTSTATUS nts;

        nts = USBPORT_TtFromBusContext(FdoDeviceObject,
                                 BusContext,
                                 &translator);

        if (NT_ERROR(nts)) {
            bw = 0;
        } else if (translator) {
            // bus context will have a TT if the query is for a 1.1 device atached to a TT

            // return local tt bandwidth, this is in bits/ms
            ASSERT_TT(translator);
            bw = translator->TotalBusBandwidth;

            // release the tt;
            USBPORT_BusContextReleaseTt(FdoDeviceObject,
                                        BusContext,
                                        translator);
        } else {
            // return bw on or 2.0 bus.  This is stored in bits/ms.
            bw = devExt->Fdo.TotalBusBandwidth;
        }
    } else {
        bw = devExt->Fdo.TotalBusBandwidth;
    }

    DbgTrace((UsbpDebugTrace,"'Total bus BW %d\n", bw));

    return bw;
}

ULONG
USBPORT_ComputeAllocatedTtBandwidth(
    PDEVICE_OBJECT FdoDeviceObject,
    PTRANSACTION_TRANSLATOR Translator
    )
/*+++

Routine Description:

    Compute total BW allocated on a TT

Return Value:

    To maintain compatibility we will report the USB frame with the greatest allocated BW in
    the 32ms scheduling window

--*/
{
    PDEVICE_EXTENSION devExt;
    ULONG used = 0, i, bw;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    for(i=0; i<MAXFRAMES; i++){
        bw = MPx_QueryTtBandwidthData(devExt, Translator, i);
        if(bw > used){
            used = bw;
        }
    }

    return used;
}


ULONG
USBPORT_ComputeAllocatedBandwidth(
    PDEVICE_OBJECT FdoDeviceObject,
    PBUS_CONTEXT BusContext
    )
/*++

Routine Description:

    Compute total bandwidth currently allocated on the bus. Note that this is TOTAL consumed not the amount
    consumed by this particular device.

    This routine will latch the TT if the buscontext is for a USB device downstream of a TT.


Arguments:

    BusContext - buscontext handle associated with the device, used only to locate the TT.

Return Value:

    Reports BW in bits/ms.  This value is intended to be compared with the value
    returned by USBPORT_ComputeTotalBandwidth.  The units should be the same accross
    a device.

    If the device has a TT, we will report the amount of BW
    consumed by the largest allocated frame on the TT.

    Otherwise we will report the amount of bw allocated for
    the frame/microframe with the most allocated bw on the controller.

--*/
{
    PDEVICE_EXTENSION devExt;
    ULONG totalBW, used, i, j;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    if (USBPORT_IS_USB20(devExt)) {
        //
        // High-speed controller
        //

        // tbd
        // allocated bandwidth is part of the budgeter code so we need to access the budgeter
        // tables to fetch this information.
        // For USB 2.0 we really need two values
        // 1. value 1 - bw consumed relative to the TT.
        // 2. value 2 - bw consumed relative to the high speed bus.
        //

//            for (i=0; i<USBPORT_MAX_INTEP_POLLING_INTERVAL; i++) {
//                if (totalBW - translator->BandwidthTable[i] > used) {
//                    used = totalBW - translator->BandwidthTable[i];
//                }
//            }

        PTRANSACTION_TRANSLATOR translator = NULL;
        NTSTATUS nts;

        used = 0;


        nts = USBPORT_TtFromBusContext(FdoDeviceObject,
                                       BusContext,
                                       &translator);

        if (NT_ERROR(nts)) {
            used = 0;

        } else if (translator) {
            // bus context will be a TT if the query is for a 1.1 device atached to a TT
            //
            // Report the TT allocated BW.
            //

            ASSERT_TT(translator);

            used = USBPORT_ComputeAllocatedTtBandwidth(FdoDeviceObject,
                                                       translator);

            //
            // Convert from bytes/frame to bits/ms
            //
            used = used * 8;

            // release the tt;

            USBPORT_BusContextReleaseTt(FdoDeviceObject,
                                        BusContext,
                                        translator);

        } else{
            //
            // Report the controller-allocated BW available
            //
            ULONG bw;

            for(i=0; i<MAXFRAMES; i++){
                for(j=0; j<MICROFRAMES_PER_FRAME; j++){
                    bw = MPx_QueryBandwidthData(devExt, i, j);
                    if(bw > used){
                        used = bw;
                    }
                }
            }
            //
            // Convert from bytes/microframe to bits/ms
            //
            used = used * 8 * 8;
        }
    }
    else{
        //
        // Full/Low speed bus
        //

        totalBW = devExt->Fdo.TotalBusBandwidth;
        used = 0;

        // table contains available bw, total-available = used
        //
        // TotalBusBandwidth and BusBandwidthTable are in bits/ms

        for (i=0; i<USBPORT_MAX_INTEP_POLLING_INTERVAL; i++) {
            if (totalBW - devExt->Fdo.BusBandwidthTable[i] > used) {
                used = totalBW - devExt->Fdo.BusBandwidthTable[i];
            }
        }
    }

    DbgTrace((UsbpDebugTrace,"'Bus BW used %d\n", used));

    return used;
}


BOOLEAN
USBPORT_SelectiveSuspendRegistry(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Return status of the selectivce suspend (gobal suspend) feature based on the
    registry settings.

    If no key is present the default is TRUE (enabled).

Arguments:

    FdoDeviceObject - host controler devobj

Return Value:

    TRUE if selective suspend Enabled.
    FALSE if it is Disabled.

--*/
{
    ULONG keyLenBytes = 0;
    ULONG disableSelectiveSuspend = HC_DISABLESSP_DEFAULT;
    PDEVICE_EXTENSION devExt = NULL;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    keyLenBytes = sizeof(DISABLE_SS_KEY);
    USBPORT_GetRegistryKeyValueForPdo(devExt->HcFdoDeviceObject,
                                      devExt->Fdo.PhysicalDeviceObject,
                                      USBPORT_SW_BRANCH,
                                      DISABLE_SS_KEY,
                                      keyLenBytes,
                                      &disableSelectiveSuspend,
                                      sizeof(disableSelectiveSuspend));

#if DBG
    if (disableSelectiveSuspend) {
        DbgTrace((UsbpDebugTrace,"'<Selective Suspend> Disabled in Registry for HC\n"));
    }
#endif

    return disableSelectiveSuspend ? FALSE : TRUE;
}


VOID
USBPORT_PowerFault(
    PDEVICE_OBJECT FdoDeviceObject,
    PUCHAR MessageText
    )
/*++

Routine Description:

    A power fault has occurred, dump information we will need
    to debug it.

    In the future we may take additional action such as event
    logging and disabling the controller.

Arguments:

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // take a power dump,
    // lets see the comment police find this one
    USBPORT_KdPrint((1, "*** USBPORT POWER FAULT ***\n"));

#if DBG
    // break if we are in debug mode, otherwise this ends up being a warning
    TEST_TRAP();
#endif

}

NTSTATUS
USBPORT_CreateLegacyFdoSymbolicLink(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Create the legacy symbolic name \\DosDevices\HCDn where
    n = 0...9,A...Z

    Many older applications detect the presence of USB by opening
    HCDn

Arguments:

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION devExt;
    WCHAR legacyLinkBuffer[17] = L"\\DosDevices\\HCD0";
    WCHAR *buffer;
    UNICODE_STRING deviceNameUnicodeString = {0};
    NTSTATUS ntStatus;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(devExt->Fdo.Flags.LegacySymbolicLink == 0);

    ntStatus = USBPORT_MakeHcdDeviceName(&deviceNameUnicodeString,
                                         devExt->Fdo.DeviceNameIdx);

    if (NT_SUCCESS(ntStatus)) {

        ALLOC_POOL_Z(buffer,
                     PagedPool,
                     sizeof(legacyLinkBuffer));

        if (buffer != NULL) {

            RtlCopyMemory(buffer,
                          legacyLinkBuffer,
                          sizeof(legacyLinkBuffer));

            USBPORT_ASSERT(devExt->Fdo.DeviceNameIdx < 256);

            //
            // The count begins with ASCII 0-9 and continues with ASCII A-Z
            //

            if(devExt->Fdo.DeviceNameIdx < 10){
                buffer[15] = (WCHAR)('0'+ devExt->Fdo.DeviceNameIdx);
            }
            else{
                buffer[15] = (WCHAR)('A' + (devExt->Fdo.DeviceNameIdx - 10));
            }

            // Null terminate the buffer.
            buffer[16] = (WCHAR)'\0';

            RtlInitUnicodeString(&devExt->Fdo.LegacyLinkUnicodeString,
                                 buffer);

            #pragma prefast(push)
            #pragma prefast(disable: __WARNING_BUFFER_OVERFLOW, "Esp:751")
            ntStatus =
                IoCreateSymbolicLink(&devExt->Fdo.LegacyLinkUnicodeString,
                                     &deviceNameUnicodeString);

            if (!NT_SUCCESS(ntStatus)) {
                // free now since we won't be setting our flag
                RtlFreeUnicodeString(&devExt->Fdo.LegacyLinkUnicodeString);
            }
            #pragma prefast(pop)
        } else {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }

    }

    if (deviceNameUnicodeString.Buffer != NULL)
    {
        RtlFreeUnicodeString(&deviceNameUnicodeString);
    }

    if (NT_SUCCESS(ntStatus)) {
         devExt->Fdo.Flags.LegacySymbolicLink = 1;
    }

    return ntStatus;
}


NTSTATUS
USBPORT_GetGlobalHacks(
     PDEVICE_OBJECT FdoDeviceObject,
     PULONG GlobalDisableSS,
     PULONG GlobalDisableCCDetect,
     PULONG EnPMDebug,
     PULONG ForceHcD3NoWakeArm
     )

/*++

Routine Description:

    Read the regkeys defining BIOS specific 'special' behavior, these hacks are
    applied globaly to all controllers in the system and usualy
    involve power management.

Arguments:

    DeviceObject - DeviceObject of the controller

Return Value:

    NT status code.

--*/

{
#define MAX_GLOBAL_KEYS    5
    ULONG k = 0;
    PWCHAR usb = L"usb";
    NTSTATUS ntStatus = STATUS_BOGUS;
    RTL_QUERY_REGISTRY_TABLE QueryTable[MAX_GLOBAL_KEYS];

    ASSERT_PASSIVE();

    // Set up QueryTable to do the following:
    QueryTable[k].QueryRoutine = USBPORT_GetConfigValue;
    QueryTable[k].Flags = 0;
    QueryTable[k].Name = G_DISABLE_SS_KEY;
    QueryTable[k].EntryContext = GlobalDisableSS;
    QueryTable[k].DefaultType = REG_DWORD;
    QueryTable[k].DefaultData = GlobalDisableSS;
    QueryTable[k].DefaultLength = sizeof(*GlobalDisableSS);
    k++;

    QueryTable[k].QueryRoutine = USBPORT_GetConfigValue;
    QueryTable[k].Flags = 0;
    QueryTable[k].Name = G_DISABLE_CC_DETECT_KEY;
    QueryTable[k].EntryContext = GlobalDisableCCDetect;
    QueryTable[k].DefaultType = REG_DWORD;
    QueryTable[k].DefaultData = GlobalDisableCCDetect;
    QueryTable[k].DefaultLength = sizeof(*GlobalDisableCCDetect);
    k++;

    QueryTable[k].QueryRoutine = USBPORT_GetConfigValue;
    QueryTable[k].Flags = 0;
    QueryTable[k].Name = G_ENABLE_PM_DEBUG_KEY;
    QueryTable[k].EntryContext = EnPMDebug;
    QueryTable[k].DefaultType = REG_DWORD;
    QueryTable[k].DefaultData = EnPMDebug;
    QueryTable[k].DefaultLength = sizeof(*EnPMDebug);
    k++;

    QueryTable[k].QueryRoutine = USBPORT_GetConfigValue;
    QueryTable[k].Flags = 0;
    QueryTable[k].Name = G_FORCE_HC_D3_NO_WAKE_ARM;
    QueryTable[k].EntryContext = ForceHcD3NoWakeArm;
    QueryTable[k].DefaultType = REG_DWORD;
    QueryTable[k].DefaultData = ForceHcD3NoWakeArm;
    QueryTable[k].DefaultLength = sizeof(*ForceHcD3NoWakeArm);
    k++;


    USBPORT_ASSERT(k < MAX_GLOBAL_KEYS);

    // stop
    QueryTable[k].QueryRoutine = NULL;
    QueryTable[k].Flags = 0;
    QueryTable[k].Name = NULL;

    ntStatus = RtlQueryRegistryValues(
                RTL_REGISTRY_SERVICES,
                usb,
                QueryTable,     // QueryTable
                NULL,           // Context
                NULL);          // Environment

#undef MAX_GLOBAL_KEYS

    return ntStatus;
}

#ifdef EN_TEST_CODE

ULONG USBPORT_StopDevice_Delay_ms = 0;

/*
TEST CODE
*/
typedef struct _USB_TEST_WORK {
    ULONG Sig;
    PIO_WORKITEM QueueItem;
    PDEVICE_OBJECT HcFdo;
    PKTHREAD WkThread; //NULL until worker is dispatched
    USB_IOREQUEST_CONTEXT UsbIoRequest;
} USB_TEST_WORK, *PUSB_TEST_WORK;


VOID
USBPORT_UsbTestWorker(
    PDEVICE_OBJECT HcFdo,
    PVOID Context
    );

NTSTATUS
USBPORT_SendSynchronousPnpRequestToRootHub(
    PDEVICE_OBJECT HcFdo
    );

NTSTATUS
USBPORT_QueueTestWorkerItem(
    PDEVICE_OBJECT HcFdo
    )
/*++


--*/
{

    PUSB_TEST_WORK testWork;
    NTSTATUS nts;
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    ALLOC_POOL_Z(testWork, NonPagedPoolNx, sizeof(struct _USB_TEST_WORK));

    if (testWork != NULL) {
        testWork->QueueItem = IoAllocateWorkItem(HcFdo);
        if (testWork->QueueItem != NULL) {
            testWork->HcFdo = HcFdo;


            USBPORT_Core_IncIoRequestCount(HcFdo, &testWork->UsbIoRequest, testWork, UsbIoTag_TESTworker, FALSE);

            IoQueueWorkItem(testWork->QueueItem,
                            USBPORT_UsbTestWorker,
                            DelayedWorkQueue,
                            testWork);


            nts = STATUS_SUCCESS;
        } else {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            UsbFreePool(testWork);
        }
    } else {
        nts = STATUS_INSUFFICIENT_RESOURCES;
    }

    return nts;
}


VOID
USBPORT_UsbTestWorker(
    PDEVICE_OBJECT HcFdo,
    PVOID Context
    )
/*++


--*/
{
    PUSB_TEST_WORK testWork = Context;
    PDEVICE_OBJECT hcFdo;
    NTSTATUS nts;
    PDEVICE_EXTENSION devExt;
    ULONG c;
    hcFdo = testWork->HcFdo;

    GET_DEVICE_EXT(devExt, hcFdo);
    ASSERT_FDOEXT(devExt);

    testWork->WkThread = KeGetCurrentThread();

    // do something
    for (c=0; c<10; c++) {
        USBPORT_SendSynchronousPnpRequestToRootHub(hcFdo);
    }
    if (devExt->TestWorkWait) {
        USBPORT_Wait(hcFdo, devExt->TestWorkWait);
    }
    IoFreeWorkItem(testWork->QueueItem);

    USBPORT_ASSERT(!testWork->UsbIoRequest.FreeContext);
    testWork->UsbIoRequest.FreeContext = FALSE; // prefix 1313870


    // run again?
    if (devExt->TestWorkState == t_work_run_again) {
        USBPORT_QueueTestWorkerItem(HcFdo);
    }

    USBPORT_Core_DecIoRequestCount(HcFdo, &testWork->UsbIoRequest, TRUE);

    testWork->WkThread = NULL;
    UsbFreePool(testWork);

    return;
}

NTSTATUS
USBPORT_SendSynchronousPnpRequestToRootHub(
    PDEVICE_OBJECT HcFdo
    )
/*++


--*/
{
    NTSTATUS nts;
    KEVENT event;
    PIRP irp;
    IO_STATUS_BLOCK ioStatus;
    PDEVICE_EXTENSION devExt;
    PIO_STACK_LOCATION nextStk;
    PDEVICE_OBJECT topOfDeviceStack;
    PDEVICE_OBJECT rhPdo = NULL;
    DEVICE_CAPABILITIES deviceCapabilities;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    RtlZeroMemory(&deviceCapabilities, sizeof(DEVICE_CAPABILITIES));
    deviceCapabilities.Size = sizeof(DEVICE_CAPABILITIES);
    deviceCapabilities.Version = 1;
    deviceCapabilities.Address = -1;
    deviceCapabilities.UINumber = -1;


    do {
        rhPdo = USBPORT_RefRootHubPdo(HcFdo, REF_RhPDO_TEST);
        if (rhPdo == NULL) {
            nts = STATUS_NO_SUCH_DEVICE;
            break;
        }

        topOfDeviceStack = IoGetAttachedDeviceReference(rhPdo);

        if (topOfDeviceStack == NULL) {
            nts = STATUS_NO_SUCH_DEVICE;
            break;
        }


        USBPORT_ASSERT(topOfDeviceStack);

        irp = IoAllocateIrp(topOfDeviceStack->StackSize + 1, FALSE);
        if (!irp) {

            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        // All PnP IRP's need the Status field initialized
        // to STATUS_NOT_SUPPORTED before calldown
        irp->IoStatus.Status = STATUS_NOT_SUPPORTED;

        nextStk = IoGetNextIrpStackLocation(irp);
        USBPORT_ASSERT(nextStk != NULL);
        //IRP_MN_QUERY_CAPABILITIES
        //nextStk->MajorFunction = IRP_MJ_PNP;
        //nextStk->MinorFunction = IRP_MN_QUERY_CAPABILITIES;
        //nextStk->Parameters.DeviceCapabilities.Capabilities = &deviceCapabilities;

        // IRP_MN_QUERY_DEVICE_RELATIONS
        nextStk->MajorFunction = IRP_MJ_PNP;
        nextStk->MinorFunction = IRP_MN_QUERY_DEVICE_RELATIONS;
        nextStk->Parameters.QueryDeviceRelations.Type = TargetDeviceRelation;

        KeInitializeEvent(&event, NotificationEvent, USBD_EVENT_STATE_NOT_SIGNALED);

        IoSetCompletionRoutine(irp,
                               USBPORT_DeferIrpCompletion,
                               &event,
                               TRUE,
                               TRUE,
                               TRUE);


        nts = IoCallDriver(topOfDeviceStack, irp);
        if (nts == STATUS_PENDING) {
           // wait for irp to complete
           KeWaitForSingleObject(
                &event,
                Suspended,
                KernelMode,
                FALSE,
                NULL);
           nts = irp->IoStatus.Status;
        }
        IoFreeIrp(irp);

        //
        ObDereferenceObject(topOfDeviceStack);

    } WHILE (0);

    if (rhPdo) {
        USBPORT_DerefRootHubPdo(HcFdo, rhPdo, REF_RhPDO_TEST);
    }

    return nts;
}


VOID
USBPORT_StopDevice_Delay(
    PDEVICE_OBJECT HcFdo
    )
/*++


--*/
{
    if (USBPORT_StopDevice_Delay_ms) {
        USBPORT_Wait(HcFdo, USBPORT_StopDevice_Delay_ms);
    }
}


#endif //EN_TEST_CODE

