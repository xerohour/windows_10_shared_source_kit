/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    device.c

Abstract:

    This module creates "Devices" on the bus for
    bus emuerators like the hub driver

Environment:

    kernel mode only

Notes:

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs

--*/

#include "common.h"

#ifdef USBPORT_WPP
#include "device.tmh"
#endif


/*
    Handle validation routines, we keep a list
    of valid handles and match the ones passed
    in with our list.

    Access to the device handle list for

    //USBPORT_CreateDevice
    //USBPORT_RemoveDevice
    //USBPORT_InitializeDevice

*/

NTSTATUS
USBPORT_AllocateEndpointForwardProgressContext(
    __in PDEVICE_OBJECT HcFdo,
    __in PHCD_ENDPOINT Endpoint,
    __in ULONG MaxForwardProgressTransferSize
    )
/*++

Routine Description:

    Allocate forward progress support structure for an endpoint

Arguments:

    HcFdo - Host controller Fdo

    Endpoint - endpoint whose FP context to be allocated

    MaxForwardProgressTransferSize

Return Value:

    STATUS_SUCCESS
    STATUS_INSUFFICIENT_RESOURCES

--*/
{
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION devExt;
    PDMA_ADAPTER adapterObject;
    PCALCULATE_SCATTER_GATHER_LIST_SIZE  calcSGListSize;
    ULONG sgBufferLen;
    ULONG transferContextSize;
    ULONG fpAllocSize;
    PHCD_ENDPOINT_FORWARD_PROGRESS_CONTEXT epFp;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    epFp = NULL;
    adapterObject = devExt->Fdo.AdapterObject;
    calcSGListSize = adapterObject->DmaOperations->CalculateScatterGatherList;

    //
    // Calculate the resource size
    //

    // Account for boundary conditions
    fpAllocSize = MaxForwardProgressTransferSize+PAGE_SIZE;

    // Calculate the SG list size
    ntStatus = (*calcSGListSize)(adapterObject,
                                 NULL,
                                 NULL,
                                 fpAllocSize,
                                 &sgBufferLen,
                                 NULL);
    if (!NT_SUCCESS(ntStatus)) {
        goto USBPORT_AllocateEndpointForwardProgressContext_Done;
    }

    // Calculate the transfer size
    transferContextSize = sizeof(HCD_TRANSFER_CONTEXT) +
                          sizeof(TRANSFER_SG_ENTRY)*
                            USBPORT_ADDRESS_AND_SIZE_TO_SPAN_PAGES_4K(
                               0, fpAllocSize) +
                          sizeof(SCATTER_GATHER_ENTRY)*
                            (fpAllocSize/(devExt->Fdo.NumberOfMapRegisters<<PAGE_SHIFT)+1)+
                          REGISTRATION_PACKET(devExt).TransferContextSize;

    // Allocate a forward progress context
    ALLOC_POOL_Z(epFp,
                 NonPagedPoolNx,
                 sizeof(HCD_ENDPOINT_FORWARD_PROGRESS_CONTEXT));

    if (!epFp) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto USBPORT_AllocateEndpointForwardProgressContext_Done;
    }

    // Allocated an Mdl
    epFp->Mdl = IoAllocateMdl(NULL,
                              fpAllocSize,
                              FALSE,
                              FALSE,
                              NULL);
    if (!epFp->Mdl) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto USBPORT_AllocateEndpointForwardProgressContext_Done;
    }

    // Allocate a transfer
    epFp->TransferContextSize = transferContextSize;
    ALLOC_POOL_Z(epFp->TransferContext,
                 NonPagedPoolNx,
                 epFp->TransferContextSize);
    if (!epFp->TransferContext) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto USBPORT_AllocateEndpointForwardProgressContext_Done;
    }

    // Allocate a SG buffer
    epFp->SGBufferLength = sgBufferLen;
    ALLOC_POOL_Z(epFp->SGBuffer,
                 NonPagedPoolNx,
                 epFp->SGBufferLength);

    if (!epFp->SGBuffer) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto USBPORT_AllocateEndpointForwardProgressContext_Done;
    }

    epFp->MaxForwardProgressTransferSize = MaxForwardProgressTransferSize;
    Endpoint->FpContext = epFp;
    epFp = NULL;

USBPORT_AllocateEndpointForwardProgressContext_Done:

    if (epFp) {
        if (epFp->Mdl) {
            IoFreeMdl(epFp->Mdl);
        }

        if (epFp->TransferContext) {
            UsbFreePool(epFp->TransferContext);
        }

        if (epFp->SGBuffer) {
            UsbFreePool(epFp->SGBuffer);
        }

        UsbFreePool(epFp);
    }

    return ntStatus;
}


VOID
USBPORT_FreeEndpointForwardProgressContext(
    __in PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Allocate forward progress support structure for an endpoint

Arguments:

    Endpoint - endpoint whose FP context to be freed

Return Value:

    None

--*/
{
    PHCD_ENDPOINT_FORWARD_PROGRESS_CONTEXT epFp;

    epFp = Endpoint->FpContext;

    if (epFp) {
        if (epFp->Mdl) {
            IoFreeMdl(epFp->Mdl);
        }

        if (epFp->TransferContext) {
            UsbFreePool(epFp->TransferContext);
        }

        if (epFp->SGBuffer) {
            UsbFreePool(epFp->SGBuffer);
        }

        UsbFreePool(epFp);

        Endpoint->FpContext = NULL;
    }
}


BOOLEAN
USBPORT_NeoValidDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    ULONG ValidMask,
    PVOID ReferenceObj,
    PIRP Irp,
    ULONG Tag,
    BOOLEAN PendingFlag
    )
/*++

Routine Description:

    returns true if a device handle is valid

Arguments:

    DeviceHandle - device handle to validate

    ReferenceObj (optional) - a refernce is taken on the devicehandle using this
        object. This is used by the API path to track client driver
        requests, it allows us to stall removal until all requests complete.

    PendingFlag - requires a ref object, if TRUE this ref will block a remove otherwise
            only the delete of the handle is blocked.

    Irp - (optional) - irp associated with the request.

    ValidMask - mask representing the valid states for the handle, these rae defined as
        DEVH_VALID_XXX in usbport.h

Return Value:

    TRUE if the handle is valid, the state&mask != 0

--*/
{
    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    PLIST_ENTRY le;
    PUSBD_DEVICE_HANDLE foundHandle = NULL;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.DevHandleListSpin, &irql);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'vld>', DeviceHandle, ValidMask,
        ReferenceObj);

    le = devExt->Fdo.DeviceHandleList.Flink;

    while (le != &devExt->Fdo.DeviceHandleList) {

        PUSBD_DEVICE_HANDLE nextHandle;

        if (DeviceHandle == NULL) {
            // NULL is obviously not valid
            break;
        }

        nextHandle = (PUSBD_DEVICE_HANDLE) CONTAINING_RECORD(
                    le,
                    struct _USBD_DEVICE_HANDLE,
                    DevhLink);

        le = nextHandle->DevhLink.Flink;

        if (nextHandle == DeviceHandle) {
            foundHandle = DeviceHandle;
            break;
        }
    }


    while (foundHandle) {

        PDEVH_REF_OBJ refObj = NULL;

        if (!(ValidMask & foundHandle->DhState)) {
            foundHandle = NULL;
            break;
        }

        if (ReferenceObj == NULL) {
            break;
        }

        if (PendingFlag) {
            InterlockedIncrement(&DeviceHandle->PendingCount);
        }

#ifndef USB_FORCE_MEMORY_STRESS
        ALLOC_POOL_Z(refObj, NonPagedPoolNx, sizeof(struct _DEVH_REF_OBJ));
#endif

        if (refObj) {
            refObj->DeviceHandle = foundHandle;
            refObj->Sig = SIG_DEVICE_HANDLE_REF;
            refObj->Object = ReferenceObj;
            refObj->Tag = Tag;
            refObj->PendingFlag = PendingFlag;
            InsertTailList(&foundHandle->RefListHead,  &refObj->RefLink);
        } else {
            // Allocation fails, use the resource-less ref count
            foundHandle->RefCountNoRes++;
        }

        break;
    }

    KeReleaseSpinLock(&devExt->Fdo.DevHandleListSpin, irql);

    if (foundHandle == NULL) {

        // this can happen, one example is the hid driver attemting to
        // access the device while it is stil being reset.
        LOGENTRY(FdoDeviceObject, LOG_MISC, 'vld!', DeviceHandle, 0, 0);

        DbgTrace((UsbpDebugTrace, "'bad device handle %p\n", DeviceHandle));

        return FALSE;
    } else {
        return TRUE;
    }
}


BOOLEAN
USBPORT_iValidDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle
    )
/*++

Routine Description:

    returns true if a device handle is valid. This is used by internal routines
    to validate the handle against the standard mask.


Arguments:

    DeviceHandle - device handle to validate

Return Value:

    TRUE if the handle is valid

--*/
{

    return USBPORT_NeoValidDeviceHandle(FdoDeviceObject, DeviceHandle, Devh_Valid, NULL, NULL, 0,
        FALSE);

}

BOOLEAN
USBPORT_UrbValidDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PVOID Urb,
    PIRP Irp
    )
/*++

Routine Description:

    returns true if a device handle is valid. This is in the urb api path to track
    pending requests on the handle.

    The Urb is added to the device handle ref list and will block removal.

Arguments:

    DeviceHandle - device handle to validate

    Urb - the urb to link in

    Irp - the irp associated with this urb

Return Value:

    TRUE if the handle is valid

--*/
{
    USBPORT_ASSERT(Urb != NULL);

    return USBPORT_NeoValidDeviceHandle(FdoDeviceObject, DeviceHandle, Devh_Valid,
        Urb, Irp, DEVH_URB_TAG, TRUE);

}


VOID
USBPORT_RemoveDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    USBD_DEVH_EVENT Event,
    BOOLEAN LockDeviceConfigs,
    BOOLEAN DeviceGone
    )
/*++

Routine Description:

    Internal version:

    This routine transitions the handle to the 'disabled' or 'removed' state.  All pending
    transfers completed.  Once in the removed/disabled state the handle cannot be referenced
    and transfers cannot be queued.

    Configuartaion and endpoints remain intact.

    This version does not block on outstanding requests.  The wait must be done by the caller.

Arguments:

    DeviceHandle - a referenced device handle.

    Event triggering the condition

Return Value:

   none

--*/
{
    PDEVICE_EXTENSION devExt = NULL;
    KIRQL irql;

    ASSERT_DEVICE_HANDLE(DeviceHandle);
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'remD', DeviceHandle, 0, 0);

    if (LockDeviceConfigs) {

        // Synchronize with any URB_FUNCTION_SELECT_CONFIGURATION or
        // URB_FUNCTION_SELECT_INTERFACE request that may be in progress
        // when the device is removed.
        //
        LOCK_DEVICE_CONFIGS(DeviceHandle, FdoDeviceObject);
    }

    KeAcquireSpinLock(&devExt->Fdo.DevHandleListSpin, &irql);
    USBPORT_SetDeviceHandleState(FdoDeviceObject, DeviceHandle, Devh_Removed, Event);
    KeReleaseSpinLock(&devExt->Fdo.DevHandleListSpin, irql);

    // complete outstanding transfers
    USBPORT_AbortAllTransfers(FdoDeviceObject, DeviceHandle, DeviceGone);

    if (LockDeviceConfigs) {

        // End synchronization with any URB_FUNCTION_SELECT_CONFIGURATION
        // or URB_FUNCTION_SELECT_INTERFACE request that may be in progress
        // when the device is removed.
        //
        UNLOCK_DEVICE_CONFIGS(DeviceHandle, FdoDeviceObject);
    }

    // wait for any refs from transfer/API refs to drain
    // transition to -1 signals the event
    if (InterlockedDecrement(&DeviceHandle->PendingCount) < 0) {
        KeSetEvent(&DeviceHandle->RemoveDevhEvent, IO_NO_INCREMENT, FALSE);
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'SDH1', DeviceHandle, 0, 0);
    }

    // WAIT
    KeWaitForSingleObject(&DeviceHandle->RemoveDevhEvent,
                            Executive,
                            KernelMode,
                            FALSE,
                            NULL);
}



ULONG
USBPORT_GetDeviceCount(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    counts devices on the BUS. This routiine can aslo be used to check
    for handle leaks.

Arguments:

Return Value:

    number of devices (including the root hub)

--*/
{
    PLIST_ENTRY listEntry;
    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    ULONG deviceCount = 0;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.DevHandleListSpin, &irql);

    listEntry = &devExt->Fdo.DeviceHandleList;

    if (!IsListEmpty(listEntry)) {
        listEntry = devExt->Fdo.DeviceHandleList.Flink;
    }

    while (listEntry != &devExt->Fdo.DeviceHandleList) {

        PUSBD_DEVICE_HANDLE nextHandle;

        nextHandle = (PUSBD_DEVICE_HANDLE) CONTAINING_RECORD(
                    listEntry,
                    struct _USBD_DEVICE_HANDLE,
                    DevhLink);

        deviceCount++;
        listEntry = nextHandle->DevhLink.Flink;

    }

    KeReleaseSpinLock(&devExt->Fdo.DevHandleListSpin, irql);

    return deviceCount;
}


VOID
USBPORT_RemovePipeHandle(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle
    )
/*++

Routine Description:

    Removes a pipe handle from our list of 'valid handles'

Arguments:

Return Value:

    none

--*/
{
    USBPORT_ASSERT(PipeHandle->ListEntry.Flink != NULL &&
                   PipeHandle->ListEntry.Blink != NULL);

    RemoveEntryList(&PipeHandle->ListEntry);
    PipeHandle->ListEntry.Flink = NULL;
    PipeHandle->ListEntry.Blink = NULL;
}


VOID
USBPORT_AddPipeHandle(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle
    )
/*++

Routine Description:

    adds a pipe handle to our internal list

Arguments:

Return Value:

    TRUE is handle is valid

--*/
{
    ASSERT_DEVICE_HANDLE(DeviceHandle);

    USBPORT_ASSERT(PipeHandle->ListEntry.Flink == NULL &&
                   PipeHandle->ListEntry.Blink == NULL);

    if (!TEST_FLAG(PipeHandle->PipeStateFlags, USBPORT_PIPE_ZERO_BW) &&
        PipeHandle->EpHandle != USB_ZERO_BW_ENDP_HANDLE) {

        PHCD_ENDPOINT   endpoint;

        endpoint = USBPORT_EndpointFromHandle(PipeHandle->EpHandle);

        // Store the ETW endpoint info.  This happens initially at the
        // time of USBPORT_OpenEndpoint(), then again if an endpoint is
        // migrated from an old device handle to a new device handle by
        // USBPORT_CloneDevice().
        //
        endpoint->EtwUsbEndpoint.Endpoint = endpoint;
        endpoint->EtwUsbEndpoint.PipeHandle = PipeHandle;
        endpoint->EtwUsbEndpoint.DeviceHandle = DeviceHandle;
    }

    InsertTailList(&DeviceHandle->PipeHandleList,
        &PipeHandle->ListEntry);
}


BOOLEAN
USBPORT_ValidatePipeHandle(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle
    )
/*++

Routine Description:

    returns true if a device handle is valid

Arguments:

Return Value:

    TRUE is handle is valid

--*/
{
    BOOLEAN found = FALSE;
    PLIST_ENTRY listEntry;

    ASSERT_DEVICE_HANDLE(DeviceHandle);

    listEntry = &DeviceHandle->PipeHandleList;

    if (!IsListEmpty(listEntry)) {
        listEntry = DeviceHandle->PipeHandleList.Flink;
    }

    while (listEntry != &DeviceHandle->PipeHandleList) {

        PUSBD_PIPE_HANDLE_I nextHandle;

        nextHandle = (PUSBD_PIPE_HANDLE_I) CONTAINING_RECORD(
                    listEntry,
                    struct _USBD_PIPE_HANDLE_I,
                    ListEntry);


        listEntry = nextHandle->ListEntry.Flink;

        if (nextHandle == PipeHandle) {
            found = TRUE;
            break;
        }
    }

#if DBG
    if (!found) {
        DbgTrace((UsbpDebugTrace, "'bad pipe handle %p\n", PipeHandle));
        TEST_TRAP();
    }
#endif

    return found;
}


NTSTATUS
USBPORT_SendCommandEx(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PVOID Buffer,
    ULONG BufferLength,
    PURB_CALLBACK_ROUTINE UrbCallbackFunc,
    PVOID UrbCallbackContext,
    USBD_STATUS *UsbdStatus
    )
/*++

Routine Description:

    Send a standard USB command on the default pipe.

    Essentially what we do here is build a control transfer and queue it directly.  This version is async
    so the urb are allocated from the heap is not freed.

Arguments:

    DeviceHandle - ptr to USBPORT device structure the command will be sent to

    FdoDeviceObject -

    RequestCode -

    WValue - wValue for setup packet

    WIndex - wIndex for setup packet

    WLength - wLength for setup packet

    Buffer - Input/Output Buffer for command

    BufferLength - Length of Input/Output buffer.

    UsbStatus - USBPORT status code returned in the URB.

Return Value:

    USBD_STATUS_PENDING if the transfer is successfully queued

--*/

{
    NTSTATUS ntStatus;
    PTRANSFER_URB urb = NULL;
    PUSBD_PIPE_HANDLE_I defaultPipe = NULL;
    PDEVICE_EXTENSION devExt;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    KEVENT event;
    PHCD_ENDPOINT endpoint = NULL;

    //ASSERT_PASSIVE();
    DbgTrace((UsbpDebugTrace, "'enter USBPORT_SendCommandEx\n"));

    ASSERT_DEVICE_HANDLE(DeviceHandle);
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT((USHORT)BufferLength == SetupPacket->wLength);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'SENc', DeviceHandle, 0, 1);

    KeInitializeEvent(&event,
                      NotificationEvent,
                      USBD_EVENT_STATE_NOT_SIGNALED);

    ALLOC_POOL_Z(urb, NonPagedPoolNx,
                 sizeof(struct _TRANSFER_URB));

    if (urb) {
        ntStatus = USBPORT_rReferenceDeviceHandle(FdoDeviceObject,
                                                  DeviceHandle,
                                                  urb,
                                                  NULL, // no irp
                                                  DEVH_SEND_CMDEX_TAG);
        //Tag VISTA #1399053
        if (NT_ERROR(ntStatus)) {

            UsbFreePool(urb);
            urb = NULL;
            usbdStatus = USBPORT_NtStatus_TO_UsbdStatus(ntStatus);
            USBPORT_ASSERT(usbdStatus != USBD_STATUS_SUCCESS);

            if (UsbdStatus) {
                *UsbdStatus = usbdStatus;
            }

        } else {

            defaultPipe = &(DeviceHandle->DefaultPipe);

            endpoint = USBPORT_EndpointFromHandle(defaultPipe->EpHandle);
            ASSERT_ENDPOINT(endpoint);

            //InterlockedIncrement(&DeviceHandle->PendingUrbs);

            usbdStatus = USBD_STATUS_SUCCESS;

            urb->Hdr.Length = sizeof(struct _TRANSFER_URB);
            urb->Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER;

            RtlCopyMemory(urb->u.SetupPacket,
                          SetupPacket,
                          8);

            urb->TransferFlags = USBD_SHORT_TRANSFER_OK;
            urb->UsbdPipeHandle = defaultPipe;
            urb->Hdr.UsbdDeviceHandle = DeviceHandle;
            urb->Hdr.UsbdFlags = 0;

            // USBPORT is responsible for setting the transfer direction
            //
            // TRANSFER direction is implied in the command

            if (SetupPacket->bmRequestType.Dir ==  BMREQUEST_DEVICE_TO_HOST) {
                USBPORT_SET_TRANSFER_DIRECTION_IN(urb->TransferFlags);
            } else {
                USBPORT_SET_TRANSFER_DIRECTION_OUT(urb->TransferFlags);
            }

            urb->TransferBufferLength = BufferLength;
            urb->TransferBuffer = Buffer;
            urb->TransferBufferMDL = NULL;

            if (urb->TransferBufferLength != 0) {

                if ((urb->TransferBufferMDL =
                     IoAllocateMdl(urb->TransferBuffer,
                                   urb->TransferBufferLength,
                                   FALSE,
                                   FALSE,
                                   NULL)) == NULL) {
                    usbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;
                    // map the error
                    ntStatus = USBPORT_SetUSBDError(NULL, usbdStatus);
                } else {
                    SET_FLAG(urb->Hdr.UsbdFlags, USBPORT_REQUEST_MDL_ALLOCATED);
                    MmBuildMdlForNonPagedPool(
                        urb->TransferBufferMDL);
                }

            }

            LOGENTRY(FdoDeviceObject,
                        LOG_MISC, 'sndC',
                            urb->TransferBufferLength,
                            SetupPacket->bmRequestType.B,
                            SetupPacket->bRequest);
            DbgTrace((UsbpDebugTrace,
                "'SendCommand cmd = 0x%x  0x%x buffer = 0%p length = 0x%x direction = 0x%x\n",
                             SetupPacket->bmRequestType.B,
                             SetupPacket->bRequest,
                             urb->TransferBuffer,
                             urb->TransferBufferLength,
                             urb->TransferFlags));

            // queue the transfer
            if (NT_SUCCESS(ntStatus)) {

                usbdStatus = USBPORT_Core_AllocTransferEx(FdoDeviceObject,
                                                   urb,
                                                   DeviceHandle,
                                                   NULL,
                                                   NULL,
                                                   5000,
                                                   UrbCallbackFunc,
                                                   UrbCallbackContext);

                if (USBD_SUCCESS(usbdStatus)) {

                    USBPORT_EtwWriteHcDeviceEndpointData2(
                        FdoDeviceObject,
                        DeviceHandle,
                        endpoint,
                        &USBPORT_ETW_EVENT_INTERNAL_URB_FUNCTION_CONTROL_TRANSFER,
                        NULL,

                        &urb,
                        sizeof(PURB),

                        urb,
                        sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

                    // do the transfer, 5 second timeout
                    USBPORT_Core_QueueTransferUrb(urb);

                    // the urb may be gone from this point on, set the loacl pointer to NULL
                    urb = NULL;

                    usbdStatus = USBD_STATUS_PENDING;
                    ntStatus = STATUS_PENDING;
                }

                if (UsbdStatus) {
                    *UsbdStatus = usbdStatus;
                }
            }else {

                usbdStatus = USBPORT_NtStatus_TO_UsbdStatus(ntStatus);
                USBPORT_ASSERT(usbdStatus != USBD_STATUS_SUCCESS);

                if (UsbdStatus) {
                    *UsbdStatus = usbdStatus;
                }
            }

        }

    } else {
        if (UsbdStatus) {
            *UsbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;
            ntStatus = USBPORT_SetUSBDError(NULL, *UsbdStatus);
        } else {
            ntStatus = USBPORT_SetUSBDError(NULL, USBD_STATUS_INSUFFICIENT_RESOURCES);
        }
    }

    //LOGENTRY(defaultPipe->Endpoint,
    //    FdoDeviceObject, LOG_MISC, 'SENd', 0, ntStatus, usbdStatus);

    DbgTrace((UsbpDebugTrace, "'exit USBPORT_SendCommandEx 0x%x\n", ntStatus));

    return ntStatus;

}


NTSTATUS
USBPORT_SendCommand(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PVOID Buffer,
    ULONG BufferLength,
    PULONG BytesReturned,
    USBD_STATUS *UsbdStatus
    )
/*++

Routine Description:

    Send a standard USB command on the default pipe.

    essentially what we do here is build a control
    transfer and queue it directly

Arguments:

    DeviceHandle - ptr to USBPORT device structure the command will be sent to

    FdoDeviceObject -

    RequestCode -

    WValue - wValue for setup packet

    WIndex - wIndex for setup packet

    WLength - wLength for setup packet

    Buffer - Input/Output Buffer for command

    BufferLength - Length of Input/Output buffer.

    BytesReturned - pointer to ulong to copy number of bytes
                    returned (optional)

    UsbStatus - USBPORT status code returned in the URB.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS ntStatus;
    PTRANSFER_URB urb = NULL;
    PUSBD_PIPE_HANDLE_I defaultPipe = NULL;
    PDEVICE_EXTENSION devExt;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    KEVENT event;
    PHCD_ENDPOINT endpoint;

    ASSERT_PASSIVE();
    DbgTrace((UsbpDebugTrace, "'enter USBPORT_SendCommand\n"));

    ASSERT_DEVICE_HANDLE(DeviceHandle);
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT((USHORT)BufferLength == SetupPacket->wLength);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'SENc', 0, 0, 0);

    KeInitializeEvent(&event,
                      NotificationEvent,
                      USBD_EVENT_STATE_NOT_SIGNALED);

    ALLOC_POOL_Z(urb, NonPagedPoolNx,
                 sizeof(struct _TRANSFER_URB));

    if (urb) {
        ntStatus = USBPORT_rReferenceDeviceHandle(FdoDeviceObject,
                                                  DeviceHandle,
                                                  urb,
                                                  NULL, // no irp
                                                  DEVH_SEND_CMD_TAG);
        //Tag VISTA #1399053
        if (NT_ERROR(ntStatus)) {

            usbdStatus = USBPORT_NtStatus_TO_UsbdStatus(ntStatus);
            USBPORT_ASSERT(usbdStatus != USBD_STATUS_SUCCESS);

            if (UsbdStatus) {
                *UsbdStatus = usbdStatus;
            }

        } else {

            defaultPipe = &(DeviceHandle->DefaultPipe);

            endpoint = USBPORT_EndpointFromHandle(defaultPipe->EpHandle);
            ASSERT_ENDPOINT(endpoint);

            //InterlockedIncrement(&DeviceHandle->PendingUrbs);

            usbdStatus = USBD_STATUS_SUCCESS;

            urb->Hdr.Length = sizeof(struct _TRANSFER_URB);
            urb->Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER;

            RtlCopyMemory(urb->u.SetupPacket,
                          SetupPacket,
                          8);

            urb->TransferFlags = USBD_SHORT_TRANSFER_OK;
            urb->UsbdPipeHandle = defaultPipe;
            urb->Hdr.UsbdDeviceHandle = DeviceHandle;
            urb->Hdr.UsbdFlags = 0;

            // USBPORT is responsible for setting the transfer direction
            //
            // TRANSFER direction is implied in the command

            if (SetupPacket->bmRequestType.Dir ==  BMREQUEST_DEVICE_TO_HOST) {
                USBPORT_SET_TRANSFER_DIRECTION_IN(urb->TransferFlags);
            } else {
                USBPORT_SET_TRANSFER_DIRECTION_OUT(urb->TransferFlags);
            }

            urb->TransferBufferLength = BufferLength;
            urb->TransferBuffer = Buffer;
            urb->TransferBufferMDL = NULL;

            if (urb->TransferBufferLength != 0) {

                if ((urb->TransferBufferMDL =
                     IoAllocateMdl(urb->TransferBuffer,
                                   urb->TransferBufferLength,
                                   FALSE,
                                   FALSE,
                                   NULL)) == NULL) {
                    usbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;
                    // map the error
                    ntStatus = USBPORT_SetUSBDError(NULL, usbdStatus);
                } else {
                    SET_FLAG(urb->Hdr.UsbdFlags, USBPORT_REQUEST_MDL_ALLOCATED);
                    MmBuildMdlForNonPagedPool(
                        urb->TransferBufferMDL);
                }

            }

            LOGENTRY(FdoDeviceObject,
                        LOG_MISC, 'sndC',
                            urb->TransferBufferLength,
                            SetupPacket->bmRequestType.B,
                            SetupPacket->bRequest);
            DbgTrace((UsbpDebugTrace,
                "'SendCommand cmd = 0x%x  0x%x buffer = 0%p length = 0x%x direction = 0x%x\n",
                             SetupPacket->bmRequestType.B,
                             SetupPacket->bRequest,
                             urb->TransferBuffer,
                             urb->TransferBufferLength,
                             urb->TransferFlags));

            // queue the transfer
            if (NT_SUCCESS(ntStatus)) {

                usbdStatus = USBPORT_Core_AllocTransfer(FdoDeviceObject,
                                                   urb,
                                                   DeviceHandle,
                                                   NULL,
                                                   &event,
                                                   5000);

                if (USBD_SUCCESS(usbdStatus)) {

                    USBPORT_EtwWriteHcDeviceEndpointData2(
                        FdoDeviceObject,
                        DeviceHandle,
                        endpoint,
                        &USBPORT_ETW_EVENT_INTERNAL_URB_FUNCTION_CONTROL_TRANSFER,
                        NULL,

                        &urb,
                        sizeof(PURB),

                        urb,
                        sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

                    // do the transfer, 5 second timeout
                    USBPORT_Core_QueueTransferUrb(urb);

                    LOGENTRY(FdoDeviceObject, LOG_MISC, 'sWTt', 0, 0, urb);

                    // wait for completion
                    KeWaitForSingleObject(&event,
                                          Suspended,
                                          KernelMode,
                                          FALSE,
                                          NULL);

                    LOGENTRY(FdoDeviceObject,  LOG_MISC, 'sWTd', 0, 0, urb);

                    usbdStatus = urb->Hdr.Status;

                }

                // map the error
                ntStatus =
                    SET_USBD_ERROR(urb, usbdStatus);

                if (BytesReturned) {
                    *BytesReturned = urb->TransferBufferLength;
                }

                if (UsbdStatus) {
                    *UsbdStatus = usbdStatus;
                }
            }else {

                usbdStatus = USBPORT_NtStatus_TO_UsbdStatus(ntStatus);
                USBPORT_ASSERT(usbdStatus != USBD_STATUS_SUCCESS);

                if (UsbdStatus) {
                    *UsbdStatus = usbdStatus;
                }
            }

            USBPORT_DereferenceDeviceHandle(FdoDeviceObject,  DeviceHandle, urb, DEVH_SEND_CMD_TAG, TRUE);

        }
        // free the transfer URB
        UsbFreePool(urb);

    } else {
        if (UsbdStatus) {
            *UsbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;
            ntStatus = USBPORT_SetUSBDError(NULL, *UsbdStatus);
        } else {
            ntStatus = USBPORT_SetUSBDError(NULL, USBD_STATUS_INSUFFICIENT_RESOURCES);
        }
    }

    //LOGENTRY(defaultPipe->Endpoint,
    //    FdoDeviceObject, LOG_MISC, 'SENd', 0, ntStatus, usbdStatus);

    DbgTrace((UsbpDebugTrace, "'exit USBPORT_SendCommand 0x%x\n", ntStatus));

    return ntStatus;
}


NTSTATUS
USBPORT_PokeEndpoint(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    This function closes an existing endpoint in the miniport,
    frees the common buffer the reopens it with new requirements
    and parameters.

    This function is synchronous and assumes no active transfers
    are pending for the endpoint.

    This function is currently used to grow the transfer parameters
    on interrupt and control endpoints and to change the address of
    the default control endpoint

    NOTES:
        1. for now we assume no changes to bw allocation
        2. new parameters are set before the call in the endpoint
            structure

Arguments:

Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus;
    ENDPOINT_REQUIREMENTS requirements = {0};
    USB_MINIPORT_STATUS mpStatus;
    PDEVICE_EXTENSION devExt;
    PUSBPORT_COMMON_BUFFER commonBuffer;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // close the endpoint in the miniport

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'poke', Endpoint, 0, 0);

    // perform sync remove from hardware

    // This will unlink the endpoint and prevent anyone else from referencing it
    USBPORT_SetGlobalEndpointStateAndWait(FdoDeviceObject, Endpoint, Ev_gEp_ReqRemove, ENDPOINT_REMOVE);

    // re-init the endpoint by routing poke thru the ep state machine.
    //
    // in order to safely re-open the endpoint and re-initialize we must first close the endpoint to ensure
    // that it is not referenced by the controller
    USBPORT_SetGlobalEndpointState(FdoDeviceObject, Endpoint, Ev_gEp_Poke);

    // free the old miniport common buffer (this must happen at passive level)
    if (Endpoint->CommonBuffer) {
        USBPORT_HalFreeCommonBufferToFreeList(FdoDeviceObject,
                                              Endpoint->CommonBuffer);
        Endpoint->CommonBuffer = NULL;
    }

    MPx_QueryEndpointRequirements(devExt, Endpoint, &requirements);

    // alloc new common buffer
    // save the requirements

    USBPORT_ASSERT(Endpoint->Parameters.TransferType != Bulk);
    USBPORT_ASSERT(Endpoint->Parameters.TransferType != Isochronous);

    DbgTrace((UsbpDebugTrace, "'(POKE) miniport requesting %d bytes\n",
        requirements.MinCommonBufferBytes));

    // allocate common buffer for this endpoint

    if (requirements.MinCommonBufferBytes) {
        commonBuffer =
            USBPORT_HalAllocateCommonBuffer(FdoDeviceObject,
                                            requirements.MinCommonBufferBytes);
    } else {
        commonBuffer = NULL;
    }

    if (commonBuffer == NULL &&
        requirements.MinCommonBufferBytes) {

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        Endpoint->CommonBuffer = NULL;

        //Endpoint->EpFlags.Zapped = 1;
        //Endpoint->EpFlags.HcSusupended = 0;
        // this will close the endpoint
        USBPORT_SetGlobalEndpointState(FdoDeviceObject, Endpoint, Ev_gEp_Zap);

    } else {
        Endpoint->CommonBuffer = commonBuffer;
        ntStatus = STATUS_SUCCESS;
    }

    // this check is redundant but it keeps
    // prefast happy
    if (Endpoint->CommonBuffer &&
        commonBuffer) {

        Endpoint->Parameters.CommonBufferVa =
             commonBuffer->MiniportVa;
        Endpoint->Parameters.CommonBufferPhys =
             commonBuffer->MiniportPhys;
        Endpoint->Parameters.CommonBufferBytes =
             commonBuffer->TotalLength;
    }

    if (NT_SUCCESS(ntStatus)) {

        mpStatus = MPx_OpenEndpoint(devExt, Endpoint, NULL);

        // in this UNIQUE situation this API is not allowed
        // (and should not) fail
        USBPORT_ASSERT(mpStatus == USBMP_STATUS_SUCCESS);
        USBPORT_ASSERT(Endpoint->MpOpenRef);
        // we need to sync the endpoint state with the miniport, when first opened the miniport
        // puts the endpoint in status:HALT, state:PAUSE.

        // re-open
        USBPORT_SetGlobalEndpointState(FdoDeviceObject, Endpoint, Ev_gEp_Reopen);

        USBPORT_SetGlobalEndpointState(FdoDeviceObject, Endpoint, Ev_gEp_ReqActive);

    }

    return ntStatus;

}


UCHAR
USBPORT_NormalizeHighSpeedInterval(
    UCHAR HsInterval
    )
{
    UCHAR tmp;
    // normalize the high speed period to microframes
    // for USB 20 the period specifies a power of 2
    // ie period = 2^(hsInterval-1)
    if (HsInterval) {
        HsInterval--;
    }
    // hsInterval must be 0..5
    if (HsInterval > 5) {
        HsInterval = 5;
    }
    tmp = 1<<HsInterval;

    DbgTrace((UsbpDebugTrace, "'Hs Interval %d  normalize %d\n",
            HsInterval, tmp));

    return tmp;
}


BOOLEAN
USBPORT_DoesEndpointRequireClearTtBufferRequestOnTransferCancel(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    This routine returns TRUE if a CLEAR_TT_BUFFER request should be
    issued to the upstream TT hub of a device if a transfer on the
    specified device endpoint is canceled or aborted, else it returns
    FALSE.


    The following HCD_ENDPOINT fields must already be initialized:

    Endpoint->EpFlags.RootHub
    Endpoint->Parameters.TransferType
    Endpoint->Tt

Arguments:

    FdoDeviceObject

    DeviceHandle

    Endpoint

Return Value:

    TRUE if a CLEAR_TT_BUFFER request should be issued to the upstream
    TT hub of a device if a transfer on the specified device endpoint is
    canceled or aborted, else it returns FALSE.

--*/
{
    PDEVICE_EXTENSION devExt;

    devExt = GetHcFdoExt(FdoDeviceObject);

    //
    // If the endpoint is a root hub endpoint then a CLEAR_TT_BUFFER
    // request is not necessary.
    //
    if (Endpoint->EpFlags.RootHub) {
        return FALSE;
    }

    //
    // If controller is not a High-Speed controller then a
    // CLEAR_TT_BUFFER request is not necessary.
    //
    if (!USBPORT_IS_USB20(devExt)) {
        return FALSE;
    }

    //
    // If the endpoint is not an asychronous endpoint (Control or Bulk)
    // then a CLEAR_TT_BUFFER request is not necessary.
    //
    if ((Endpoint->Parameters.TransferType == Isochronous) ||
        (Endpoint->Parameters.TransferType == Interrupt)) {
        return FALSE;
    }

    //
    // If there is no TT associated with this endpoint then a
    // CLEAR_TT_BUFFER request is not necessary.
    //
    if (Endpoint->Tt == NULL) {

        // If the endpoint is on a Low-Speed or Full-Speed device
        // downstream of a High-Speed controller then there should be a
        // TT associated with this endpoint.
        //
        USBPORT_ASSERT(DeviceHandle->DeviceSpeed == HighSpeed);

        return FALSE;
    }

    //
    // If the TTFLAGS_NO_CLEAR_TT_BUFFER_ON_CANCEL flag is set for the
    // TT associated with this endpoint then do not send a
    // CLEAR_TT_BUFFER request.
    //
    if (TEST_FLAG(Endpoint->Tt->TtFlags, TTFLAGS_NO_CLEAR_TT_BUFFER_ON_CANCEL)) {
        return FALSE;
    }












    return TRUE;
}


NTSTATUS
USBPORT_OpenEndpoint(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    OUT USBD_STATUS *ReturnUsbdStatus,
    BOOLEAN IsDefaultPipe
    )
/*++

Routine Description:

    open an endpoint on a USB device.

    This function creates (initializes) endpoints and
    hooks it to a pipehandle

Arguments:

    DeviceHandle - data describes the device this endpoint is on.
    DeviceObject - USBPORT device object.
    ReturnUsbdStatus - OPTIONAL

Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION devExt;
    PHCD_ENDPOINT endpoint;
    USBD_STATUS usbdStatus;
    BOOLEAN gotBw = FALSE;
    USB_HIGH_SPEED_MAXPACKET muxPacket;
    extern ULONG USB2LIB_EndpointContextSize;

    // this function is not pagable because we raise irql

    // we should be at passive level
    ASSERT_PASSIVE();

    // devhandle should have been validated
    // before we get here
    ASSERT_DEVICE_HANDLE(DeviceHandle);

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(FdoDeviceObject,
             LOG_PNP, 'opE+', PipeHandle, devExt->Fdo.HcdEndpointSize,
             REGISTRATION_PACKET(devExt).EndpointDataSize);


    // allocate the endoint

    // * begin special case
    // check for a no bandwidth endpoint ie max_packet = 0
    // if so return success and set the endpoint pointer
    // in the pipe handle to a dummy value

    if (PipeHandle->EndpointDescriptor.wMaxPacketSize == 0) {

        ntStatus = STATUS_SUCCESS;
        SET_FLAG(PipeHandle->PipeStateFlags, USBPORT_PIPE_ZERO_BW);
        CLEAR_FLAG(PipeHandle->PipeStateFlags, USBPORT_PIPE_STATE_CLOSED);

        PipeHandle->EpHandle = USB_ZERO_BW_ENDP_HANDLE;

        USBPORT_AddPipeHandle(DeviceHandle,
                              PipeHandle);

        goto USBPORT_OpenEndpoint_Done;
    }

    // * end special case

    ALLOC_POOL_Z(endpoint, NonPagedPoolNx, devExt->Fdo.HcdEndpointSize);

    do {

        if (endpoint == NULL) {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            if (ReturnUsbdStatus != NULL) {
                *ReturnUsbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;
            }
            break;
        }

        endpoint->Sig = SIG_ENDPOINT;
        endpoint->EpFlags.ul = 0;
        endpoint->GepState = GEp_Init;
        endpoint->MiniportEndpointDataSize = REGISTRATION_PACKET(devExt).EndpointDataSize;

        endpoint->UsbHcIntDpcGenerationCount = devExt->Fdo.UsbHcIntDpcGenerationCount;

        endpoint->EpRefFP.Endpoint = endpoint;
        endpoint->EpRefFP.Sig = SIG_EPREF;
        endpoint->EpRefFP.Tag = EPREF_FORWARD_PROGRESS_TAG;

        if (IS_ROOT_HUB(DeviceHandle)) {
           endpoint->EpFlags.RootHub =1;
        }

        ntStatus = USBPORT_OpenEndpointRefList(FdoDeviceObject, endpoint);
        if (NT_ERROR(ntStatus)) {
            UNSIG(endpoint);
            UsbFreePool(endpoint);
            endpoint = NULL;
            break;
        }

        // At this point if (endpoint != NULL) then the endpoint has
        // been referenced.

        endpoint->FdoDeviceObject = FdoDeviceObject;

        if (DeviceHandle->TtHandle) {

            ntStatus = USBPORT_ReferenceTt(FdoDeviceObject,
                                            DeviceHandle->TtHandle,
                                            endpoint,
                                            TT_ENDPOINT_TAG); //epTT
            if (NT_ERROR(ntStatus)) {
                break;
            }

            endpoint->Tt = DeviceHandle->TtHandle;
            ASSERT_TT(endpoint->Tt);
        }

        // At this point if (endpoint->Tt != NULL) then the TT (if any)
        // associated with this device endpoint has been referenced.

        ntStatus = MPx_InitializeHsbEndpoint(devExt, endpoint, endpoint->Tt);
        if (NT_ERROR(ntStatus)) {
            break;
        }



















        LOGENTRY(FdoDeviceObject,
             LOG_PNP, 'ope+', PipeHandle, devExt->Fdo.HcdEndpointSize,
             REGISTRATION_PACKET(devExt).EndpointDataSize);

        // initialize the endpoint
        InitializeListHead(&endpoint->HwPendingListHead);
        InitializeListHead(&endpoint->TxMappedNotInMiniportListHead);
        InitializeListHead(&endpoint->TxCanceledPendingClearTTCompletionListHead);
        InitializeListHead(&endpoint->TxDonePendingClearTTCompletionListHead);

        KeInitializeEvent(&endpoint->GepEventPause, NotificationEvent, USBD_EVENT_STATE_SIGNALED);
        KeInitializeEvent(&endpoint->GepEventRemove, NotificationEvent, USBD_EVENT_STATE_SIGNALED);


        // extract some information from the
        // descriptor
        endpoint->Parameters.DeviceAddress =
            DeviceHandle->DeviceAddress;

        if (endpoint->Tt != NULL) {
            ASSERT_TT(endpoint->Tt);
            endpoint->Parameters.TtDeviceAddress =
                endpoint->Tt->DeviceAddress;
        } else {
            endpoint->Parameters.TtDeviceAddress = 0xFFFF;
        }

        endpoint->Parameters.TtPortNumber =
            DeviceHandle->TtPortNumber;

        if (DeviceHandle->DeviceDescriptor.bcdUSB < 0x250) {

            muxPacket.us = PipeHandle->EndpointDescriptor.wMaxPacketSize;
            endpoint->Parameters.MuxPacketSize =
                muxPacket.MaxPacket;
            endpoint->Parameters.TransactionsPerMicroframe =
                (UCHAR)muxPacket.HSmux+1;
            endpoint->Parameters.MaxPacketSize =
                muxPacket.MaxPacket * (muxPacket.HSmux+1);

        } else {

            endpoint->Parameters.MaxPacketSize =
                PipeHandle->EndpointDescriptor.wMaxPacketSize;

        }

        endpoint->Parameters.EndpointAddress =
            PipeHandle->EndpointDescriptor.bEndpointAddress;

        if ((PipeHandle->EndpointDescriptor.bmAttributes &
              USB_ENDPOINT_TYPE_MASK) == USB_ENDPOINT_TYPE_ISOCHRONOUS) {
            endpoint->Parameters.TransferType = Isochronous;
        } else if ((PipeHandle->EndpointDescriptor.bmAttributes &
              USB_ENDPOINT_TYPE_MASK) == USB_ENDPOINT_TYPE_BULK) {
            endpoint->Parameters.TransferType = Bulk;
        } else if ((PipeHandle->EndpointDescriptor.bmAttributes &
              USB_ENDPOINT_TYPE_MASK) == USB_ENDPOINT_TYPE_INTERRUPT) {
            endpoint->Parameters.TransferType = Interrupt;
        } else {
            USBPORT_ASSERT((PipeHandle->EndpointDescriptor.bmAttributes &
              USB_ENDPOINT_TYPE_MASK) == USB_ENDPOINT_TYPE_CONTROL);
            endpoint->Parameters.TransferType = Control;
        }

        // check for low speed
        endpoint->Parameters.DeviceSpeed = DeviceHandle->DeviceSpeed;

        // Set max transfer size based on transfer type
        //
        // Note: that the MaximumTransferSize set by the
        // client driver in the pipe information structure
        // is no longer used.
        switch(endpoint->Parameters.TransferType) {
        case Interrupt:
            // this allows clients to put down larger
            // interrupt buffers if they want without
            // taking a perf hit. For some reason
            // printers do this.

            // bugbug research the praticality of splitting
            // interrupt transfers for the miniports this may
            // significantly reduce the memory allocated by
            // he uhci miniport
            endpoint->Parameters.MaxTransferSize = 1024;

            //
            // For Wireless USB, some additional endpoint attributes have been
            // defined for interrupt endpoints.
            //
            if (PipeHandle->EndpointDescriptor.bmAttributes & 0x4) {

                endpoint->Parameters.EndpointFlags |= EP_PARM_FLAG_LOPWR;

            }

            if (PipeHandle->EndpointDescriptor.bmAttributes & 0x80) {

                endpoint->Parameters.EndpointFlags |= EP_PARM_FLAG_ADJ_MAXPKT;

            }

            break;
        case Control:
            // 4k
            // node that the old win2k 4k usb stack does not actually
            // handle transfers larger than this correctly.
            endpoint->Parameters.MaxTransferSize = 1024*4;

            // set the default to 64k if this is not the control endpoint
            if (endpoint->Parameters.EndpointAddress != 0) {
                // the COMPAQ guys test this

                endpoint->Parameters.MaxTransferSize = 1024*64;
            }
            break;
        case Bulk:
            // 64k default
            endpoint->Parameters.MaxTransferSize = 1024*64;

            if (PipeHandle->EndpointDescriptor.bmAttributes & 0x8) {

                endpoint->Parameters.EndpointFlags |= EP_PARM_FLAG_ADJ_MAXPKT;

            }

            break;
        case Isochronous:
            // there is no reason to have a limit here
            // choose a really large default
            endpoint->Parameters.MaxTransferSize = 0x01000000;
            break;
        }

        endpoint->Parameters.Period = 0;

        // compute period required
        if (endpoint->Parameters.TransferType == Interrupt) {

            UCHAR tmp;

            if (endpoint->Parameters.DeviceSpeed == HighSpeed) {
                // normalize the high speed period to microframes
                // for USB 20 the period specifies a power of 2
                // ie period = 2^(hsInterval-1)
                tmp = USBPORT_NormalizeHighSpeedInterval(
                         PipeHandle->EndpointDescriptor.bInterval);
            } else {
                tmp = PipeHandle->EndpointDescriptor.bInterval;
            }
            // this code finds the first interval
            // <= USBPORT_MAX_INTEP_POLLING_INTERVAL
            // valid intervals are:
            // 1, 2, 4, 8, 16, 32(USBPORT_MAX_INTEP_POLLING_INTERVAL)

            // Initialize Period, may be adjusted down

            endpoint->Parameters.Period = USBPORT_MAX_INTEP_POLLING_INTERVAL;

            if ((tmp != 0) && (tmp < USBPORT_MAX_INTEP_POLLING_INTERVAL)) {

                // bInterval is in range.  Adjust Period down if necessary.

                if ((endpoint->Parameters.DeviceSpeed == LowSpeed) &&
                    (tmp < 8)) {

                    // bInterval is not valid for LowSpeed, cap Period at 8

                    endpoint->Parameters.Period = 8;

                } else {

                    // Adjust Period down to greatest power of 2 less than or
                    // equal to bInterval.

                    while ((endpoint->Parameters.Period & tmp) == 0) {
                        endpoint->Parameters.Period >>= 1;
                    }
                }
            }

            endpoint->Parameters.MaxPeriod =
                endpoint->Parameters.Period;
        }

        if (endpoint->Parameters.TransferType == Isochronous) {

            if (endpoint->Parameters.DeviceSpeed == HighSpeed) {
                // by definition the 20 interval must always be a power
                // of 2 so we don't have to convert
                endpoint->Parameters.Period =
                    USBPORT_NormalizeHighSpeedInterval(
                        PipeHandle->EndpointDescriptor.bInterval);
            } else {
                // otherwise set period to one for non-hs endpoints
                endpoint->Parameters.Period = 1;
            }
        }

        if (USB_ENDPOINT_DIRECTION_IN(
            PipeHandle->EndpointDescriptor.bEndpointAddress)) {
            endpoint->Parameters.TransferDirection = In;
        } else {
            endpoint->Parameters.TransferDirection = Out;
        }

        if (USBPORT_DoesEndpointRequireClearTtBufferRequestOnTransferCancel(FdoDeviceObject,
                                                                            DeviceHandle,
                                                                            endpoint)) {
            endpoint->EpFlags.ClearTtBufferOnCancel = 1;
        }

        gotBw = MPx_AllocateBandwidth(devExt, endpoint);

        if (gotBw) {

            if (IsDefaultPipe ||
                endpoint->Parameters.TransferType == Isochronous) {
                // iso and default pipes do not halt on errors
                // ie they do not require a resetpipe
                endpoint->Parameters.EndpointFlags |= EP_PARM_FLAG_NOHALT;
            }

            ntStatus = STATUS_SUCCESS;
        } else {
            LOGENTRY(FdoDeviceObject, LOG_PNP, 'noBW', endpoint, 0, 0);

            // no bandwidth error
            ntStatus = USBPORT_SetUSBDError(NULL, USBD_STATUS_NO_BANDWIDTH);
            if (ReturnUsbdStatus != NULL) {
                *ReturnUsbdStatus = USBD_STATUS_NO_BANDWIDTH;
            }
        }
    }WHILE (0);


    if (NT_SUCCESS(ntStatus)) {

        // now do the open

        if (endpoint->EpFlags.RootHub) {

            // opens for the root hub device are not passed to the miniport
            usbdStatus = USBD_STATUS_SUCCESS;

            // track the hub interrupt endpoint
            if (endpoint->Parameters.TransferType == Interrupt) {

                ntStatus = USBPORT_Ev_Rh_IntrEp_Open(FdoDeviceObject, endpoint);

                if (NT_ERROR(ntStatus)) {
                    usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                }
            }

        } else {

            USB_MINIPORT_STATUS mpStatus;
            PUSBPORT_COMMON_BUFFER commonBuffer;
            ENDPOINT_REQUIREMENTS requirements;

            // find out what we will need from the miniport for this endpoint

            MPx_QueryEndpointRequirements(devExt, endpoint, &requirements);

            // adjust maxtransfer based on miniport feedback.
            switch (endpoint->Parameters.TransferType) {
            case Bulk:
            case Control:
            case Interrupt:
                LOGENTRY(FdoDeviceObject, LOG_MISC, 'MaxT', endpoint, requirements.MaximumTransferSize, 0);

                EP_MAX_TRANSFER(endpoint) = requirements.MaximumTransferSize;
                break;
            }

            DbgTrace((UsbpDebugTrace, "'miniport requesting %d bytes\n",
                requirements.MinCommonBufferBytes));

            // allocate common buffer for this endpoint
            if (requirements.MinCommonBufferBytes) {
                commonBuffer =
                   USBPORT_HalAllocateCommonBuffer(FdoDeviceObject,
                           requirements.MinCommonBufferBytes);
            } else {
                commonBuffer = NULL;
            }


            if (commonBuffer == NULL &&
                requirements.MinCommonBufferBytes) {

                mpStatus = USBMP_STATUS_NO_RESOURCES;
                endpoint->CommonBuffer = NULL;

            } else {

                ULONG mpOptionFlags;

                mpOptionFlags = devExt->Fdo.HcOptionFlags;

                endpoint->CommonBuffer = commonBuffer;
                if (commonBuffer != NULL) {
                    endpoint->Parameters.CommonBufferVa =
                        commonBuffer->MiniportVa;
                    endpoint->Parameters.CommonBufferPhys =
                        commonBuffer->MiniportPhys;
                    endpoint->Parameters.CommonBufferBytes =
                        commonBuffer->TotalLength;
                }

                // initial state is closed, successful open assumes it is in the paused state
                endpoint->CurrentEpState = endpoint->PendingEpState = ENDPOINT_CLOSED;
                // call open request to minport
                mpStatus = MPx_OpenEndpoint(devExt, endpoint, NULL);
                if (mpStatus == USBMP_STATUS_SUCCESS) {
                    USBPORT_ASSERT(endpoint->MpOpenRef);
                }

                // set our internal flags based on what the miniport passed back
                endpoint->EpFlags.MapTransfers = 1;
                endpoint->EpFlags.HwSyncOnSOF = 1;
                if (USBPORT_HW_SYNC(devExt)) {
                    endpoint->EpFlags.HwSyncOnSOF = 0;
                    endpoint->EpFlags.PollForHwSync = 1;
                }

                if (TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_NO_PNP_RESOURCES)) {
                    // no mapping for the virtual bus
                    endpoint->EpFlags.MapTransfers = 0;
                    endpoint->EpFlags.VirtualBus = 1;
                }
                endpoint->EpFlags.Pure = 1;

            }

            // endpoint will go active in USBPORT_SetGlobalEndpointState
            usbdStatus = MPSTATUS_TO_USBSTATUS(mpStatus);
        }

        // convert usb status to nt status
        ntStatus = USBPORT_SetUSBDError(NULL, usbdStatus);
        if (ReturnUsbdStatus != NULL) {
            *ReturnUsbdStatus = usbdStatus;
        }
    }

    do {

        if (NT_SUCCESS(ntStatus) && endpoint) {
            ASSERT_ENDPOINT(endpoint);
            //
            // attach a reference for the pointer in the pipehandle

            ntStatus = USBPORT_ReferenceEndpoint(FdoDeviceObject, endpoint,
                &PipeHandle->EpHandle, PipeHandle, EPREF_PIPE_TAG);

            if (NT_SUCCESS(ntStatus)) {

                USBPORT_AddPipeHandle(DeviceHandle,
                                      PipeHandle);

                // track the endpoint, this will set it to open (from init)
                USBPORT_SetGlobalEndpointState(FdoDeviceObject, endpoint, Ev_gEp_Open);

                CLEAR_FLAG(PipeHandle->PipeStateFlags, USBPORT_PIPE_STATE_CLOSED);

                USBPORT_SetGlobalEndpointState(FdoDeviceObject, endpoint, Ev_gEp_ReqActive);

                RtlCopyMemory(&endpoint->EndpointDescriptor,
                              &PipeHandle->EndpointDescriptor,
                              sizeof(USB_ENDPOINT_DESCRIPTOR));

                // Write a USBPORT_ETW_EVENT_ENDPOINT_OPEN event
                //
                USBPORT_EtwWriteHcDeviceEndpoint(FdoDeviceObject,
                    DeviceHandle,
                    endpoint,
                    &USBPORT_ETW_EVENT_ENDPOINT_OPEN,
                    NULL);

                break;
            }
        }

        if (endpoint) {

            // Failure path resource allocation cleanup.

            // Free resources allocated by USBPORT_HalAllocateCommonBuffer().
            //
            if (endpoint->CommonBuffer) {
                USBPORT_HalFreeCommonBufferToFreeList(FdoDeviceObject,
                    endpoint->CommonBuffer);
                endpoint->CommonBuffer = NULL;
            }

            // Free the bandwidth allocation, if that was successful.
            // Need to do this before freeing the endpoint->HsbEpContextPtr
            // and the endpoint->Tt.
            //
            if (gotBw) {
                MPx_FreeBandwidth(devExt, endpoint);
            }

            // Free resources allocated by MPx_InitializeHsbEndpoint().
            //
            if (endpoint->HsbEpContextPtr) {
                UsbFreePool(endpoint->HsbEpContextPtr);
                endpoint->HsbEpContextPtr = NULL;
            }

            if (endpoint->TegraExtension) {
                UsbFreePool(endpoint->TegraExtension);
                endpoint->TegraExtension = NULL;
            }

            if (endpoint->Tt != NULL) {
                ASSERT_TT(endpoint->Tt);
                USBPORT_DereferenceTt(FdoDeviceObject,
                                      endpoint->Tt,
                                      endpoint,
                                      TT_ENDPOINT_TAG); //epTT
                endpoint->Tt = NULL;
            }
            MPf_DbgFreeEndpoint(devExt, endpoint);

            USBPORT_CloseEndpointRefList(FdoDeviceObject, endpoint);

            UNSIG(endpoint);
            UsbFreePool(endpoint);
        }

    } WHILE (0);

USBPORT_OpenEndpoint_Done:

    return ntStatus;
}

VOID
USBPORT_CloseEndpoint(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Close an Endpoint

Arguments:

    DeviceHandle - ptr to USBPORT device data structure.

    DeviceObject - USBPORT device object.

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION devExt;
    HC_GLOBAL_EP_STATE geps;
    KIRQL irql;
    PUSBPORT_DB_HANDLE dbHandle;

    // should have been validated before we
    // get here
    ASSERT_DEVICE_HANDLE(DeviceHandle);

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    //endpoint(common buffer) can only be freed at passive (
    ASSERT_PASSIVE();

    // Write a USBPORT_ETW_EVENT_ENDPOINT_CLOSE event
    //
    USBPORT_EtwWriteHcDeviceEndpoint(FdoDeviceObject,
                                     DeviceHandle,
                                     Endpoint,
                                     &USBPORT_ETW_EVENT_ENDPOINT_CLOSE,
                                     NULL);

    // we should have no requests queued to the
    // endpoint
    LOGENTRY(FdoDeviceObject,
                LOG_MISC, 'clEP', Endpoint, 0, DeviceHandle);

    if (Endpoint->EpFlags.RootHub &&
        Endpoint->Parameters.TransferType == Interrupt) {

        // this is also done when the root hub device handle is removed
        USBPORT_Ev_Rh_IntrEp_Close(FdoDeviceObject);
    }

    // this will override any pause initiated by IoCancelRequest and allow the endpoint to finish the trasnition to
    // PAUSE
    USBPORT_SetGlobalEndpointStateAndWait(FdoDeviceObject, Endpoint, Ev_gEp_ReqPause, ENDPOINT_PAUSE);

    USBPORT_ClearTtBufferAndWait(FdoDeviceObject, DeviceHandle, Endpoint);

    // This will unlink the endpoint and prevent anyone else from referencing it
    USBPORT_SetGlobalEndpointStateAndWait(FdoDeviceObject, Endpoint, Ev_gEp_ReqRemove, ENDPOINT_REMOVE);

    // The client is locked out at this point ie he can't access the pipe tied to the endpoint.  We need to wait
    // for any outstanding stuff to complete -- including any state changes, after which we can ask the coreworker
    // to remove the endpoint.


    // lock the endpoint & set the state to remove and
    // free the bw
    MPx_FreeBandwidth(devExt, Endpoint);
    if (Endpoint->Tt) {
        USBPORT_DereferenceTt(FdoDeviceObject,
                              Endpoint->Tt,
                              Endpoint,
                              TT_ENDPOINT_TAG); //epTT
        Endpoint->Tt = NULL;
    }

    //
    // This will block here until USBPORT_iDereferenceEndpoint() is
    // called the last time to release all of the outstanding references
    // on the Endpoint->EpRefListHead.
    //
    USBPORT_KdPrint((1, " >**WAIT CloseEndpoint %p\n", Endpoint));
    USBPORT_CloseEndpointRefList(FdoDeviceObject, Endpoint);
    USBPORT_KdPrint((1, " >**WAIT CloseEndpoint %p (DONE)\n", Endpoint));

    // The endpoint cannot be accessed. Perform a synchronous close opertion thru the
    // miniport before freeing the data structures.
    // this routine will deal with zapped and root hub endpoints as well

    // free the endpoint
    geps = USBPORT_SetGlobalEndpointState(FdoDeviceObject, Endpoint, Ev_gEp_Close);

    // free endpoint memory
    USBPORT_ASSERT(geps == GEp_Free);
    MPf_DbgFreeEndpoint(devExt, Endpoint);
    if (Endpoint->CommonBuffer) {

        USBPORT_HalFreeCommonBufferToFreeList(FdoDeviceObject,
                                              Endpoint->CommonBuffer);
    }

    if (Endpoint->HsbEpContextPtr) {
        UsbFreePool(Endpoint->HsbEpContextPtr);
        Endpoint->HsbEpContextPtr = NULL;
    }

    USBPORT_FreeEndpointForwardProgressContext(Endpoint);

    //
    // Fee the double buffer handles
    //
    if (Endpoint->TegraExtension != NULL) {
        KeAcquireSpinLock(&Endpoint->TegraExtension->IsoDbHandleListLock, &irql);
        while (!IsListEmpty(&Endpoint->TegraExtension->IsoDbHandlesForTegra)) {
            dbHandle = CONTAINING_RECORD(RemoveHeadList(&Endpoint->TegraExtension->IsoDbHandlesForTegra),
                                         USBPORT_DB_HANDLE,
                                         DbLink);
            USBPORT_FreeDbHandleForIsoOnTegraFinal(dbHandle);
        }
        while (!IsListEmpty(&Endpoint->TegraExtension->PhysicallyContiguousIsoDbHandlesForTegra)) {
            dbHandle = CONTAINING_RECORD(RemoveHeadList(&Endpoint->TegraExtension->PhysicallyContiguousIsoDbHandlesForTegra),
                                         USBPORT_DB_HANDLE,
                                         DbLink);
            USBPORT_FreeDbHandleForIsoOnTegraFinal(dbHandle);
        }
        KeReleaseSpinLock(&Endpoint->TegraExtension->IsoDbHandleListLock, irql);

        UsbFreePool(Endpoint->TegraExtension);
        Endpoint->TegraExtension = NULL;
    }

    UNSIG(Endpoint);
    UsbFreePool(Endpoint);

    return;
}


VOID
USBPORT_ClosePipe(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_PIPE_HANDLE_I PipeHandle
    )
/*++

Routine Description:

    Close a USB pipe and the endpoint associated with it

    This is a synchronous operation that waits for all
    transfers associated with the pipe to be completed.

Arguments:

    DeviceHandle - ptr to USBPORT device data structure
    DeviceObject - USBPORT device object
    PipeHandle - USBPORT pipe handle associated with the endpoint

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    // should be valid at time of call
    ASSERT_DEVICE_HANDLE(DeviceHandle);
    ASSERT_PIPE_HANDLE(PipeHandle);

    LOGENTRY(FdoDeviceObject,
        LOG_MISC, 'clPI', PipeHandle, 0, 0);

    if (PipeHandle->PipeStateFlags & USBPORT_PIPE_STATE_CLOSED) {
        // already closed
        // generally when a partially open interface needs to
        // be closed due to an error
        USBPORT_ASSERT(PipeHandle->ListEntry.Flink == NULL &&
                       PipeHandle->ListEntry.Blink == NULL);

        return;
    }

    // Invalidate the pipe pipe handle.
    //
    USBPORT_RemovePipeHandle(DeviceHandle,
                             PipeHandle);

    SET_FLAG(PipeHandle->PipeStateFlags, USBPORT_PIPE_STATE_CLOSED);

    // At this point the client driver will be unable to queue any
    // additional transfers to this pipe or endpoint.

    LOGENTRY(FdoDeviceObject,
        LOG_MISC, 'pipW', PipeHandle, 0, 0);

    // now 'close' the endpoint
    if (TEST_FLAG(PipeHandle->PipeStateFlags, USBPORT_PIPE_ZERO_BW) ||
        PipeHandle->EpHandle == USB_ZERO_BW_ENDP_HANDLE) {
        CLEAR_FLAG(PipeHandle->PipeStateFlags, USBPORT_PIPE_ZERO_BW);
    } else {
        PHCD_ENDPOINT endpoint;
        KIRQL irql;

        endpoint = USBPORT_EndpointFromHandle(PipeHandle->EpHandle);
        ASSERT_ENDPOINT(endpoint);

        irql = USBPORT_TxAcquireLock(FdoDeviceObject, NULL);
        endpoint->EpFlags.PipeClosed = 1;
        USBPORT_TxReleaseLock(FdoDeviceObject, NULL, irql);

        // If there are any outstanding transfers on the endpoint then
        // abort them now.
        //
        if (USBPORT_Core_GetEndpointTransferCount(FdoDeviceObject, endpoint)) {
            USBPORT_Core_SyncAbortEndpoint(FdoDeviceObject, endpoint);
        }

        // drop the pipe reference here, this routine will delete the endpoint
        // if this is the last ref
        USBPORT_DereferenceEndpoint(FdoDeviceObject, PipeHandle->EpHandle);
        PipeHandle->EpHandle = NULL;

        USBPORT_CloseEndpoint(DeviceHandle,
                              FdoDeviceObject,
                              endpoint);
    }
}

#define USB_MAX_ADDRESS_COUNT 128

VOID
USBPORT_FreeUsbAddress(
    PDEVICE_OBJECT FdoDeviceObject,
    USHORT DeviceAddress
    )
/*++

    Frees a previously allocated USB address

--*/
{
    PDEVICE_EXTENSION devExt;
    ULONG bit;
    PUSB_ADDRESS_LIST usbAddressList;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    usbAddressList = devExt->Fdo.AddressList;

    do {

        // we should never see a free to device address 0
        USBPORT_ASSERT(DeviceAddress != 0);
        if (DeviceAddress == 0) {
            break;
        }

        USBPORT_ASSERT(DeviceAddress < USB_MAX_ADDRESS_COUNT);
        if (DeviceAddress >= USB_MAX_ADDRESS_COUNT) {
            break;
        }

        // If the host controller is surprise removed it is possible
        // that the AddressList bitmap has already been freed by
        // USBPORT_StopDevice() before all of the devices have been
        // removed.  In this case freeing the device address is a NOP.
        //
        if (usbAddressList == NULL) {
            break;
        }

        bit = DeviceAddress;
        USBPORT_ASSERT(RtlTestBit(&usbAddressList->Bitmap, bit));
        RtlClearBit(&usbAddressList->Bitmap, bit);

        DbgTrace((UsbpDebugTrace, "'USBPORT free Address %d\n", DeviceAddress));

   } WHILE (0);

}


NTSTATUS
USBPORT_AllocateUsbAddress(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSHORT AssignedAddress
    )
/*++

Routine Description:

    Allocates a USB address from a bitmap of availble addresses. Valid USB address (1..127) to use for a device.
    returns 0 and STATUS_INSUFFICIENT_RESOURCES if no device address available.

--*/
{
    PDEVICE_EXTENSION devExt;
    USHORT address;
    NTSTATUS nts;
    PUSB_ADDRESS_LIST usbAddressList;
    ULONG bit;

    ASSERT_PASSIVE();
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    do {
        nts = STATUS_INSUFFICIENT_RESOURCES;
        address = 0;

        if (devExt->Fdo.Flags.UsbAddressInit == 0) {

            ALLOC_POOL_Z(usbAddressList, NonPagedPoolNx, sizeof(struct _USB_ADDRESS_LIST));

            if (usbAddressList) {
                RtlInitializeBitMap(&usbAddressList->Bitmap,
                                    &usbAddressList->Bits[0],
                                    USB_MAX_ADDRESS_COUNT);

                RtlClearAllBits(&usbAddressList->Bitmap);
                // reserve bit 0 (default address)
                RtlSetBit(&usbAddressList->Bitmap, 0);

                devExt->Fdo.Flags.UsbAddressInit = 1;
                USBPORT_ASSERT(devExt->Fdo.AddressList == NULL);
                devExt->Fdo.AddressList = usbAddressList;

            }

        } else {
            usbAddressList = devExt->Fdo.AddressList;
        }

        if (usbAddressList == NULL) {
            break;
        }

        bit = RtlFindClearBits(&usbAddressList->Bitmap, 1, 1);

        USBPORT_ASSERT(bit != 0);

        // if in range assign address
        if ((bit != 0) && (bit < USB_MAX_ADDRESS_COUNT)) {
            address = (USHORT) bit;
            RtlSetBit(&usbAddressList->Bitmap, bit);
            nts = STATUS_SUCCESS;
        }

   }WHILE (0);

#if DBG
    if (address == 0 && usbAddressList) {
        // no free addresses?
        NT_ASSERTMSG("No free addresses",FALSE);
    }
#endif

    DbgTrace((UsbpDebugTrace, "'USBPORT assigning Address %d\n", address));

    *AssignedAddress = address;

    return nts;
}


NTSTATUS
USBPORT_InitializeHsHub(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE HsHubDeviceHandle,
    ULONG TtCount
    )
/*++

Routine Description:

    Service exported for use by the hub driver

    This service initializes a high speed hub

Arguments:

    HubDeviceHandle - DeviceHandle for the creating USB Hub

Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus;
    ULONG i;
    PDEVICE_EXTENSION devExt;

    LOGENTRY(FdoDeviceObject,
        LOG_MISC, 'ihsb', 0, HsHubDeviceHandle, TtCount);

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // hub driver might pass us NULL if it could not
    // retrieve a device handle
    if (HsHubDeviceHandle == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    ASSERT_DEVICE_HANDLE(HsHubDeviceHandle);
    USBPORT_ASSERT(HsHubDeviceHandle->DeviceSpeed == HighSpeed);

    if (IS_ROOT_HUB(HsHubDeviceHandle)) {

        if (devExt->Fdo.RootHubTTCapable == FALSE) {

            // No TTs for root hubs unless they implement embedded TT
            // support.
            //
            return STATUS_SUCCESS;

        } else {

            // There is no interface for USBHUB to explicitly
            // uninitialize a Hs Hub and free the TT resources.  If the
            // root hub is disabled and then reenabled again USBHUB will
            // initialize the root hub again so free any previously
            // allocated TT resources before allocating them again.
            //
            USBPORT_RemoveTTs(FdoDeviceObject,
                              HsHubDeviceHandle);
        }

    } else {

        USBPORT_ASSERT(HsHubDeviceHandle->DeviceDescriptor.bDeviceClass == USB_DEVICE_CLASS_HUB);
        USBPORT_ASSERT(TEST_FLAG(HsHubDeviceHandle->DeviceFlags, USBPORT_DEVICEFLAG_HSHUB));
    }

    // If the Hs Hub device is a TT capable Root Hub then any previously
    // allocated TT resources should have been freed by the call above
    // to USBPORT_RemoveTTs().
    //
    // If the Hs Hub device is not a Root Hub then it should not yet
    // have any TT resources allocated.  However, if the Hs Hub device
    // is not a Root Hub then it is possible that the TTs from a
    // previous device handle instance were migrated to this new device
    // handle instance during USBPORT_CloneDevice().  There is really no
    // point of doing that in USBPORT_CloneDevice() and that TT
    // migration logic should probably be removed, but for now that
    // logic is being left in place and consequently this assert must be
    // disabled:
    //
    // USBPORT_ASSERT(IsListEmpty(&HsHubDeviceHandle->TtListHead));

    ntStatus = STATUS_SUCCESS;
    for (i=0; i< TtCount; i++) {
        ntStatus = USBPORT_InitializeTT(FdoDeviceObject,
                                        HsHubDeviceHandle,
                                        (USHORT)i+1);

        if(!NT_SUCCESS(ntStatus)) {
            break;
        }
    }

    HsHubDeviceHandle->TtCount = TtCount;

    return ntStatus;
}


NTSTATUS
SetCdPathError(
    PUSB_CD_ERROR_INFORMATION CdErrInfo,
    USBPORT_CREATEDEV_ERROR Path,
    LONG NtStatus
    )
{

    if (CdErrInfo) {
        CdErrInfo->PathError = Path;
        CdErrInfo->NtStatus = NtStatus;
    }

    return NtStatus;
}


NTSTATUS
USBPORT_NeoCreateDevice(
    PUSBD_DEVICE_HANDLE         *NewDeviceHandle,
    PDEVICE_OBJECT              FdoDeviceObject,
    PUSBD_DEVICE_HANDLE         HsHubDeviceHandle,
    USHORT                      PortStatus,
    PUSB_PORT_PATH              PortPath,
    PUSB_CD_ERROR_INFORMATION   CdErrInfo,
    USHORT                      TtPortNumber,
    PDEVICE_OBJECT              PdoDeviceObject,
    PUNICODE_STRING             PhysicalDeviceObjectName
    )
/*++

Routine Description:

    Service exported for use by the hub driver

    Called for each new device on the USB bus, this function sets
    up the internal data structures we need to keep track of the
    device and assigns it an address.

    This version create device takes additional parameters that identify the
    Tt associated with the device.  By passing these paraneters directly from
    the hub we can avoid the need to walk the device chain.

    Commands to a high speed device have three possible targets,
        1. The hub the device is attached
        2. The hub that owns the TT
        3. The device itself.

Arguments:

    DeviceHandle - ptr to return the ptr to the new device structure
                created by this routine.

    DeviceObject - USBPORT device object for the USB bus this device is on.

    HsHubDeviceHandle - DeviceHandle for the creating USB High speed Hub.
        This may be NULL if we are delaing with a USB 1.1 bus only.

    TtPortNumber - TtPortNumber to address the TT that is assigned to this hub

Return Value:

    NT status code.

--*/
{
    NTSTATUS nts;
    PUSBD_DEVICE_HANDLE deviceHandle;
    PUSBD_PIPE_HANDLE_I defaultPipe;
    PDEVICE_EXTENSION devExt;
    ULONG bytesReturned = 0;
    PUCHAR data = NULL;
    BOOLEAN open = FALSE;
    ULONG dataSize;
    KIRQL irql;

    ASSERT_PASSIVE();
    DbgTrace((UsbpDebugTrace, "'CreateDevice\n"));

    if (CdErrInfo) {
        RtlZeroMemory(CdErrInfo->XtraInfo, sizeof(CdErrInfo->XtraInfo));
        CdErrInfo->PathError = CreateDevErrNotSet;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    do {

        USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
        PUCHAR tmpDevDescBuf;

        if (NewDeviceHandle == NULL) {
            nts = SetCdPathError(CdErrInfo, CreateDevBadDevHandlePtr,
                STATUS_INVALID_PARAMETER);
            break;
        }

        *NewDeviceHandle = NULL;



        deviceHandle = USBPORT_AllocDeviceHandle(FdoDeviceObject);

        if (deviceHandle == NULL) {

             nts = SetCdPathError(CdErrInfo, CreateDevFailedAllocDevHandle,
                               STATUS_INSUFFICIENT_RESOURCES);

             break;
        }

        // Store a self-reference DeviceHandle in the ETW device info.
        //
        deviceHandle->EtwUsbDevice.DeviceHandle = deviceHandle;

        // Store the PortPath in the ETW device info.
        //
        if (PortPath != NULL) {
            RtlCopyMemory(&deviceHandle->EtwUsbDevice.PortPath,
                          PortPath,
                          sizeof(USB_PORT_PATH));
        }

        // Store the USBHUB driver enumerated PDO for this device
        // handle.  The PDO pointer is used only for diagnostic logging
        // and the pointer should not be dereferenced.
        //
        deviceHandle->DevicePdo_Unreferenced = PdoDeviceObject;

        // Copy the PDO name into the device handle.
        //
        if (PhysicalDeviceObjectName != NULL &&
            PhysicalDeviceObjectName->Buffer != NULL &&
            PhysicalDeviceObjectName->Length != 0) {

            // PhysicalDeviceObjectName->Length should not include the
            // terminating UNICODE_NULL.  Allocation size should
            // include the terminating UNICODE_NULL.
            //
            ALLOC_POOL_Z(deviceHandle->PhysicalDeviceObjectName.Buffer,
                         NonPagedPoolNx,
                         PhysicalDeviceObjectName->Length + sizeof(UNICODE_NULL));

            if (deviceHandle->PhysicalDeviceObjectName.Buffer != NULL) {

                RtlCopyMemory(deviceHandle->PhysicalDeviceObjectName.Buffer,
                              PhysicalDeviceObjectName->Buffer,
                              PhysicalDeviceObjectName->Length);

                deviceHandle->PhysicalDeviceObjectName.Length =
                    PhysicalDeviceObjectName->Length;

                deviceHandle->PhysicalDeviceObjectName.MaximumLength =
                    PhysicalDeviceObjectName->Length + sizeof(UNICODE_NULL);
            }
        }

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'CRED', 0, 0, deviceHandle);

        //
        // validate the deviceHandle for the creating hs hub, we need
        // this information for USB 1.1 devices behind a USB 2.0 hub.
        //

        if (HsHubDeviceHandle) {

            LOGENTRY(FdoDeviceObject, LOG_MISC, 'crD>', HsHubDeviceHandle,
                        TtPortNumber, PortStatus);

            USBPORT_ASSERT(USBPORT_IS_TT_ENABLED(devExt));

            USBPORT_ASSERT(!TEST_FLAG(PortStatus, PORT_STATUS_HIGH_SPEED));

            // reference the hs hub we are pointing to
            nts = USBPORT_iReferenceDeviceHandle(FdoDeviceObject,
                                                 HsHubDeviceHandle,
                                                 deviceHandle,
                                                 DEVH_HSHUB_TAG); //HShb

            if (NT_ERROR(nts)) {

                nts = SetCdPathError(CdErrInfo, CreateDevBadHubDevHandle,
                        STATUS_DEVICE_NOT_CONNECTED);

                USBPORT_DeleteDeviceHandle(FdoDeviceObject, deviceHandle, Ev_CreateDevice_Fail);
                // exit while
                break;
            }

            // reference the TT
            deviceHandle->TtHandle =
                USBPORT_GetTt(FdoDeviceObject, HsHubDeviceHandle, TtPortNumber, deviceHandle, TT_DEVH_TAG);

            if (deviceHandle->TtHandle == NULL) {
                nts = SetCdPathError(CdErrInfo, CreateDevBadHubDevHandle,
                        STATUS_INVALID_PARAMETER);

                USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                                HsHubDeviceHandle,
                                                deviceHandle,
                                                DEVH_HSHUB_TAG, //HShb
                                                FALSE);

                USBPORT_DeleteDeviceHandle(FdoDeviceObject, deviceHandle, Ev_CreateDevice_Fail);
                // exit while
                break;
            }

            // Hs port number is what maps to a specific tt but hub fw has to make sense of this.
            deviceHandle->TtPortNumber = TtPortNumber;

        }


        deviceHandle->HsHubDeviceHandle = HsHubDeviceHandle;
        deviceHandle->ConfigurationHandle = NULL;
        deviceHandle->DeviceAddress = USB_DEFAULT_DEVICE_ADDRESS;
        //deviceHandle->DeviceBandwidth = 0;

        if (PortStatus & PORT_STATUS_LOW_SPEED) {
            deviceHandle->DeviceSpeed = LowSpeed;
        } else if (PortStatus & PORT_STATUS_HIGH_SPEED) {
            deviceHandle->DeviceSpeed = HighSpeed;
            USBPORT_ASSERT(deviceHandle->HsHubDeviceHandle == NULL);
        } else {
            deviceHandle->DeviceSpeed = FullSpeed;
        }

        // Store the DeviceSpeed in the ETW device info.
        //
        deviceHandle->EtwUsbDevice.DeviceSpeed = deviceHandle->DeviceSpeed;

        deviceHandle->Sig = SIG_DEVICE_HANDLE;

        // buffer for our descriptor, one packet
        data = (PUCHAR) &deviceHandle->DeviceDescriptor;
        dataSize = sizeof(deviceHandle->DeviceDescriptor);

        // **
        // We need to talk to the device, first we open the default pipe
        // using the defined max packet size (defined by USB spec as 8
        // bytes until device receives the GET_DESCRIPTOR (device) command).
        // We set the address get the device descriptor then close the pipe
        // and re-open it with the correct max packet size.
        // **
#define USB_DEFAULT_LS_MAX_PACKET   8
        //
        // open the default pipe for the device
        //
        defaultPipe = &deviceHandle->DefaultPipe;
        if (deviceHandle->DeviceSpeed == LowSpeed) {
            INITIALIZE_DEFAULT_PIPE(*defaultPipe, USB_DEFAULT_LS_MAX_PACKET);
        } else {
            INITIALIZE_DEFAULT_PIPE(*defaultPipe, USB_DEFAULT_MAX_PACKET);
        }
        InitializeListHead(&deviceHandle->PipeHandleList);
        InitializeListHead(&deviceHandle->TtListHead);

        nts = USBPORT_OpenEndpoint(deviceHandle,
                                        FdoDeviceObject,
                                        defaultPipe,
                                        NULL,
                                        TRUE);
        open = NT_SUCCESS(nts);

        bytesReturned = 0;

        if (NT_ERROR(nts)) {
            // failed to open endpoint

            nts = SetCdPathError(CdErrInfo, CreateDevFailedOpenEndpoint, nts);

            if (HsHubDeviceHandle) {
                USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                                 HsHubDeviceHandle,
                                                 deviceHandle,
                                                 DEVH_HSHUB_TAG, //HShb
                                                 FALSE);
            }

            if (deviceHandle->TtHandle) {
                USBPORT_DereferenceTt(FdoDeviceObject,
                               deviceHandle->TtHandle,
                               deviceHandle,
                               TT_DEVH_TAG); //dvTT
            }

            USBPORT_DeleteDeviceHandle(FdoDeviceObject, deviceHandle, Ev_CreateDevice_Fail);
            // exit while
            break;

        }


        //
        // Configure the default pipe for this device and assign the
        // device an address
        //
        // NOTE: if this operation fails it means that we have a device
        // that will respond to the default endpoint and we can't change
        // it.
        // we have no choice but to disable the port on the hub this
        // device is attached to.
        //


        //
        // Get information about the device
        //

        // Would you believe that there exist some devices that get confused
        // if the very first Get Device Descriptor request does not have a
        // wLength value of 0x40 even though the device only has a 0x12 byte
        // Device Descriptor to return?  Any change to the way devices have
        // always been enumerated since the being of USB 1.0 time can cause
        // bizarre consequences.  Use a wLength value of 0x40 for the very
        // first Get Device Descriptor request.

        ALLOC_POOL_Z(tmpDevDescBuf, NonPagedPoolNx,
                     USB_DEFAULT_MAX_PACKET);

        if (tmpDevDescBuf == NULL) {

            nts = SetCdPathError(CdErrInfo, CreateDevFailedAllocDsBuff,
                STATUS_INSUFFICIENT_RESOURCES);

            if (HsHubDeviceHandle) {
                USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                                 HsHubDeviceHandle,
                                                 deviceHandle,
                                                 DEVH_HSHUB_TAG, //HShb
                                                 FALSE);
            }

            if (deviceHandle->TtHandle) {
                USBPORT_DereferenceTt(FdoDeviceObject,
                               deviceHandle->TtHandle,
                               deviceHandle,
                               TT_DEVH_TAG); //dvTT
            }

            USBPORT_DeleteDeviceHandle(FdoDeviceObject, deviceHandle, Ev_CreateDevice_Fail);
            // exit while
            break;

        }

        // setup packet for get device descriptor

        USBPORT_INIT_SETUP_PACKET(setupPacket,
                                  USB_REQUEST_GET_DESCRIPTOR, // bRequest
                                  BMREQUEST_DEVICE_TO_HOST, // Dir
                                  BMREQUEST_TO_DEVICE, // Recipient
                                  BMREQUEST_STANDARD, // Type
                                  USB_DESCRIPTOR_MAKE_TYPE_AND_INDEX(USB_DEVICE_DESCRIPTOR_TYPE, 0), //  wValue
                                  0, // wIndex
                                  USB_DEFAULT_MAX_PACKET); // wLength

        nts = USBPORT_SendCommand(deviceHandle,
                                       FdoDeviceObject,
                                       &setupPacket,
                                       tmpDevDescBuf,
                                       USB_DEFAULT_MAX_PACKET,
                                       &bytesReturned,
                                       NULL);

        // NOTE:
        // at this point we only have the first 8 bytes of the
        // device descriptor.

        RtlCopyMemory(data, tmpDevDescBuf, dataSize);

        UsbFreePool(tmpDevDescBuf);

        // some devices babble so we ignore the error
        // on this transaction if we got enough data
        if (bytesReturned == 8 && !NT_SUCCESS(nts)) {
            DbgTrace((UsbpDebugTrace,
                "'Error returned from get device descriptor -- ignored\n"));
            nts = STATUS_SUCCESS;
        }

        // validate the max packet value and descriptor
        // we need at least eight bytes a value of zero
        // in max packet is bogus

        if (NT_SUCCESS(nts) &&
            (bytesReturned >= 8) &&
            (deviceHandle->DeviceDescriptor.bLength >= sizeof(USB_DEVICE_DESCRIPTOR)) &&
            (deviceHandle->DeviceDescriptor.bDescriptorType == USB_DEVICE_DESCRIPTOR_TYPE) &&
            ((deviceHandle->DeviceDescriptor.bMaxPacketSize0 == 0x08) ||
             (deviceHandle->DeviceDescriptor.bMaxPacketSize0 == 0x10) ||
             (deviceHandle->DeviceDescriptor.bMaxPacketSize0 == 0x20) ||
             (deviceHandle->DeviceDescriptor.bMaxPacketSize0 == 0x40))) {

            // successful create
            KeAcquireSpinLock(&devExt->Fdo.DevHandleListSpin, &irql);
            USBPORT_SetDeviceHandleState(FdoDeviceObject, deviceHandle, Devh_Enabled_Addr0,
                Ev_CreateDevice_Success);
            KeReleaseSpinLock(&devExt->Fdo.DevHandleListSpin, irql);

            *NewDeviceHandle = deviceHandle;

            break;

        } else {

            PUCHAR p = (PUCHAR)&deviceHandle->DeviceDescriptor;

            if (CdErrInfo) {
                CdErrInfo->PathError = CreateDevFailedGetDs;
                CdErrInfo->NtStatus = nts;
                CdErrInfo->UlongArg1 = bytesReturned;
                RtlCopyMemory(CdErrInfo->XtraInfo, p,
                    sizeof(deviceHandle->DeviceDescriptor));
                USBPORT_KdPrint((1, "'CREATEDEVICE failed enumeration CDERR %p\n", CdErrInfo));
            }
            // print a big debug message
            USBPORT_KdPrint((1, "'CREATEDEVICE failed enumeration %p %02X\n",
                             nts, bytesReturned));

            USBPORT_KdPrint((1, "'%02X %02X %02X %02X %02X %02X %02X %02X"
                                " %02X %02X %02X %02X %02X %02X %02X %02X"
                                " %02X %02X\n",
                             p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],
                             p[8],p[9],p[10],p[11],p[12],p[13],p[14],p[15],
                             p[16],p[17]));

            TEST_TRAP();
            //
            // something went wrong, if we assigned any resources to
            // the default pipe then we free them before we get out.
            //

            // we need to signal to the parent hub that this
            // port is to be be disabled we will do this by
            // returning an error.
            nts = STATUS_DEVICE_DATA_ERROR;

            // if we opened a pipe close it
            if (open) {

                USBPORT_ClosePipe(deviceHandle,
                                  FdoDeviceObject,
                                  defaultPipe);
            }

            USBPORT_ASSERT(NT_ERROR(nts));

            if (HsHubDeviceHandle) {
                USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                                 HsHubDeviceHandle,
                                                 deviceHandle,
                                                 DEVH_HSHUB_TAG, //HShb
                                                 FALSE);
            }

            if (deviceHandle->TtHandle) {

                USBPORT_DereferenceTt(FdoDeviceObject,
                               deviceHandle->TtHandle,
                               deviceHandle,
                               TT_DEVH_TAG); //dvTT

            }

            USBPORT_DeleteDeviceHandle(FdoDeviceObject, deviceHandle, Ev_CreateDevice_Fail);
            // exit while
            break;

        }

    }WHILE (0);

    CATC_TRAP_ERROR(FdoDeviceObject, nts);

    if (*NewDeviceHandle) {
        LOGENTRY(FdoDeviceObject, LOG_MISC, 'cre1', *NewDeviceHandle, 0, nts);

        // Write a USBPORT_ETW_EVENT_DEVICE_CREATE event
        //
        USBPORT_EtwWriteHcDeviceData1(FdoDeviceObject,
                                      *NewDeviceHandle,
                                      &USBPORT_ETW_EVENT_DEVICE_CREATE,
                                      NULL,
                                      &((*NewDeviceHandle)->DeviceDescriptor),
                                      sizeof(USB_DEVICE_DESCRIPTOR));
    }

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'creD', 0, 0, nts);

    return nts;
}


VOID
USBPORT_NeoRemoveDevice(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Internal function

    Called for each device on the USB bus that needs to be removed.  This routine frees
    the device handle and the address assigned to the device handle.

    The routine is called with this pshyical instance of the device is removed from the bus.

Arguments:

    DeviceHandle - ptr to device data structure created by class driver in USBPORT_CreateDevice.

    FdoDeviceObject - USBPORT device object for the USB bus this device is on.


Return Value:

    NT status code.

--*/
{
    PUSBD_PIPE_HANDLE_I defaultPipe;

    LOGENTRY(FdoDeviceObject, LOG_PNP, 'REMV', DeviceHandle, 0, 0);

    // Write a USBPORT_ETW_EVENT_DEVICE_REMOVAL event
    //
    USBPORT_EtwWriteHcDevice(FdoDeviceObject,
                             DeviceHandle,
                             &USBPORT_ETW_EVENT_DEVICE_REMOVAL,
                             NULL);

    // Handle is no longer valid so all attempts to submit urbs by the
    // client driver should now be failed.
    //
    USBPORT_RemoveDeviceHandle(FdoDeviceObject, DeviceHandle, Ev_Remove, TRUE, TRUE);

    // Synchronize with any URB_FUNCTION_SELECT_CONFIGURATION or
    // URB_FUNCTION_SELECT_INTERFACE request that may be in progress
    // when the device is removed.
    //
    LOCK_DEVICE_CONFIGS(DeviceHandle, FdoDeviceObject);

    //
    // make sure and clean up any open pipe handles
    // the device may have
    //

    if (DeviceHandle->ConfigurationHandle) {

        USBPORT_InternalCloseConfiguration(DeviceHandle,
                                           FdoDeviceObject,
                                           0);

    }

    defaultPipe = &DeviceHandle->DefaultPipe;

    // we should aways have a default pipe, this will free
    // the endpoint
    USBPORT_ClosePipe(DeviceHandle,
                      FdoDeviceObject,
                      defaultPipe);

    if (DeviceHandle->DeviceAddress != USB_DEFAULT_DEVICE_ADDRESS) {
        USBPORT_FreeUsbAddress(FdoDeviceObject, DeviceHandle->DeviceAddress);
        DeviceHandle->DeviceAddress = USB_DEFAULT_DEVICE_ADDRESS;
    }

    // End synchronization with any URB_FUNCTION_SELECT_CONFIGURATION or
    // URB_FUNCTION_SELECT_INTERFACE request that may be in progress
    // when the device is removed.
    //
    UNLOCK_DEVICE_CONFIGS(DeviceHandle, FdoDeviceObject);

    //
    // free any Tt handles associated with this device handle, this code runs if the device handle
    // is for a high speed hub
    //
    USBPORT_RemoveTTs(FdoDeviceObject,
                      DeviceHandle);

    if (DeviceHandle->HsHubDeviceHandle) {
        USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                        DeviceHandle->HsHubDeviceHandle,
                                        DeviceHandle,
                                        DEVH_HSHUB_TAG, //HShb
                                        FALSE);
    }

    if (DeviceHandle->TtHandle) {
        USBPORT_DereferenceTt(FdoDeviceObject,
                       DeviceHandle->TtHandle,
                       DeviceHandle,
                       TT_DEVH_TAG); //dvTT
    }

    USBPORT_DeleteDeviceHandle(FdoDeviceObject, DeviceHandle, Ev_RemoveDone);

    return;
}



NTSTATUS
USBPORT_InitializeDevice(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_ID_ERROR_INFORMATION IdErrInfo
    )
/*++

Routine Description:

    Service exported for use by the hub driver

    Called for each device on the USB bus that needs to be initialized.
    This routine allocates an address and assigns it to the device.

    NOTE: on entry the the device descriptor in DeviceHandle is expected to
        contain at least the first 8 bytes of the device descriptor, this
        information is used to open the default pipe.

    On Error the DeviceHandle structure is freed.

Arguments:

    DeviceHandle - ptr to device data structure created by class driver
                from a call to USBPORT_CreateDevice.

    DeviceObject - USBPORT device object for the USB bus this device is on.

Return Value:

    NT status code.


--*/
{
    NTSTATUS ntStatus;
    PUSBD_PIPE_HANDLE_I defaultPipe;
    USHORT address;
    PDEVICE_EXTENSION devExt;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    PHCD_ENDPOINT endpoint;

    ASSERT_PASSIVE();

    DbgTrace((UsbpDebugTrace, "'InitializeDevice\n"));

    if (IdErrInfo) {
        RtlZeroMemory(IdErrInfo->XtraInfo, sizeof(IdErrInfo->XtraInfo));
        IdErrInfo->PathError = InitDevErrNotSet;
    }

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);
    USBPORT_ASSERT(DeviceHandle != NULL);

    defaultPipe = &DeviceHandle->DefaultPipe;
    endpoint = USBPORT_EndpointFromHandle(defaultPipe->EpHandle);
    ASSERT_ENDPOINT(endpoint);


    //
    // Assign Address to the device
    //

    ntStatus = USBPORT_AllocateUsbAddress(FdoDeviceObject, &address);

    if (NT_SUCCESS(ntStatus)) {

        DbgTrace((UsbpDebugTrace, "'SetAddress, assigning 0x%x address\n", address));
        LOGENTRY(FdoDeviceObject, LOG_MISC, 'ADRa', DeviceHandle, 0, address);

        USBPORT_ASSERT(DeviceHandle->DeviceAddress == USB_DEFAULT_DEVICE_ADDRESS);

        // setup packet for set_address
        USBPORT_INIT_SETUP_PACKET(setupPacket,
                USB_REQUEST_SET_ADDRESS, // bRequest
                BMREQUEST_HOST_TO_DEVICE, // Dir
                BMREQUEST_TO_DEVICE, // Recipient
                BMREQUEST_STANDARD, // Type
                address, // wValue
                0, // wIndex
                0); // wLength


        ntStatus = USBPORT_SendCommand(DeviceHandle,
                                       FdoDeviceObject,
                                       &setupPacket,
                                       NULL,
                                       0,
                                       NULL,
                                       NULL);

        DeviceHandle->DeviceAddress = address;

        // Store the DeviceAddress in the ETW device info.
        //
        DeviceHandle->EtwUsbDevice.DeviceAddress = address;

    } else {
        // indicates no address asigned
        DeviceHandle->DeviceAddress = USB_DEFAULT_DEVICE_ADDRESS;
    }

    if (NT_ERROR(ntStatus) && IdErrInfo) {
        // set address has failed
        IdErrInfo->PathError = InitDevFailedSetAddress;
        IdErrInfo->NtStatus = ntStatus;
        USBPORT_ASSERT(sizeof(IdErrInfo->XtraInfo) >= sizeof(setupPacket));
        RtlCopyMemory(IdErrInfo->XtraInfo, &setupPacket, sizeof(setupPacket));
    }

    if (NT_SUCCESS(ntStatus)) {
        //
        // done with addressing process...
        //
        // poke the endpoint zero to the new address and
        // the true max packet size for the default control.
        // endpoint.
        //
        endpoint->Parameters.MaxPacketSize =
            DeviceHandle->DeviceDescriptor.bMaxPacketSize0;
        endpoint->Parameters.DeviceAddress = address;

        //MP_PokeEndpoint(devExt, defaultPipe->Endpoint, mpStatus);
        //ntStatus = MPSTATUS_TO_NTSTATUS(mpStatus);
        ntStatus = USBPORT_PokeEndpoint(FdoDeviceObject, endpoint);

        if (NT_ERROR(ntStatus) && IdErrInfo) {
            // poke endpoint failed
            IdErrInfo->PathError = InitDevFailedPokeEndpoint;
            IdErrInfo->NtStatus = ntStatus;
        }
    }

    if (NT_SUCCESS(ntStatus)) {

        ULONG bytesReturned;
        USB_DEFAULT_PIPE_SETUP_PACKET setupPacket2;
        USBD_STATUS usbdStatus = 0xffffffff;

        // 10ms delay to allow devices to respond after
        // the setaddress command
        USBPORT_Wait(FdoDeviceObject, 10);

        //
        // Fetch the device descriptor again, this time
        // get the whole thing.
        //

        // setup packet for get device descriptor
        USBPORT_INIT_SETUP_PACKET(setupPacket2,
            USB_REQUEST_GET_DESCRIPTOR, // bRequest
            BMREQUEST_DEVICE_TO_HOST, // Dir
            BMREQUEST_TO_DEVICE, // Recipient
            BMREQUEST_STANDARD, // Type
            USB_DESCRIPTOR_MAKE_TYPE_AND_INDEX(USB_DEVICE_DESCRIPTOR_TYPE, 0), // wValue
            0, // wIndex
            sizeof(DeviceHandle->DeviceDescriptor)); // wLength

        ntStatus =
            USBPORT_SendCommand(DeviceHandle,
                            FdoDeviceObject,
                            &setupPacket2,
                            (PUCHAR) &DeviceHandle->DeviceDescriptor,
                            sizeof(DeviceHandle->DeviceDescriptor),
                            &bytesReturned,
                            &usbdStatus);

        if (NT_SUCCESS(ntStatus) &&
            (bytesReturned != sizeof(USB_DEVICE_DESCRIPTOR)) ||
            (DeviceHandle->DeviceDescriptor.bLength < sizeof(USB_DEVICE_DESCRIPTOR)) ||
            (DeviceHandle->DeviceDescriptor.bDescriptorType != USB_DEVICE_DESCRIPTOR_TYPE) ||
            ((DeviceHandle->DeviceDescriptor.bMaxPacketSize0 != 0x08) &&
             (DeviceHandle->DeviceDescriptor.bMaxPacketSize0 != 0x10) &&
             (DeviceHandle->DeviceDescriptor.bMaxPacketSize0 != 0x20) &&
             (DeviceHandle->DeviceDescriptor.bMaxPacketSize0 != 0x40))) {
            // print a big debug message
            USBPORT_KdPrint((1, "'InitializeDevice failed enumeration\n"));

            ntStatus = STATUS_DEVICE_DATA_ERROR;

        }

        if (IdErrInfo && NT_ERROR(ntStatus)) {
            // set address has failed
            IdErrInfo->PathError = InitDevBadDeviceDescriptor;
            IdErrInfo->Arg1 = bytesReturned;
            IdErrInfo->NtStatus = ntStatus;
            IdErrInfo->UsbdStatus = usbdStatus;
            IdErrInfo->UsbAddress = (ULONG) address;
            RtlCopyMemory(IdErrInfo->XtraInfo,
                          (PUCHAR) &DeviceHandle->DeviceDescriptor,
                          sizeof(DeviceHandle->DeviceDescriptor));



        }

    }

    if (NT_SUCCESS(ntStatus)) {
        KIRQL irql;

        if (DeviceHandle->DeviceSpeed == HighSpeed &&
            DeviceHandle->DeviceDescriptor.bDeviceClass == USB_DEVICE_CLASS_HUB) {
            // note that this is a hs hub, these require special
            // handling because of the TTs
            SET_FLAG(DeviceHandle->DeviceFlags, USBPORT_DEVICEFLAG_HSHUB);
        }

        // Store the Device Descriptor idVendor and idProduct in the ETW
        // device info.
        //
        DeviceHandle->EtwUsbDevice.idVendor  = DeviceHandle->DeviceDescriptor.idVendor;
        DeviceHandle->EtwUsbDevice.idProduct = DeviceHandle->DeviceDescriptor.idProduct;

        KeAcquireSpinLock(&devExt->Fdo.DevHandleListSpin, &irql);
        USBPORT_SetDeviceHandleState(FdoDeviceObject, DeviceHandle, Devh_Valid,
            Ev_InitializeDevice_Success);
        KeReleaseSpinLock(&devExt->Fdo.DevHandleListSpin, irql);

        // Write a USBPORT_ETW_EVENT_DEVICE_INITIALIZE event
        //
        USBPORT_EtwWriteHcDeviceData1(FdoDeviceObject,
                                      DeviceHandle,
                                      &USBPORT_ETW_EVENT_DEVICE_INITIALIZE,
                                      NULL,
                                      &DeviceHandle->DeviceDescriptor,
                                      sizeof(USB_DEVICE_DESCRIPTOR));
    } else {

        //
        // something went wrong, if we assigned any resources to
        // the default pipe then we free them before we get out.
        //

        // we need to signal to the parent hub that this
        // port is to be be disabled we will do this by
        // returning an error.

        // if we got here then we know the default
        // endpoint is open

        TEST_TRAP();
        if (IdErrInfo) {
            USBPORT_ASSERT(IdErrInfo->PathError != InitDevErrNotSet);
        }
        USBPORT_ClosePipe(DeviceHandle,
                          FdoDeviceObject,
                          defaultPipe);

        if (DeviceHandle->DeviceAddress != USB_DEFAULT_DEVICE_ADDRESS) {
            USBPORT_FreeUsbAddress(FdoDeviceObject, DeviceHandle->DeviceAddress);
            DeviceHandle->DeviceAddress = USB_DEFAULT_DEVICE_ADDRESS;
        }

        if (DeviceHandle->HsHubDeviceHandle) {
            USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                             DeviceHandle->HsHubDeviceHandle,
                                             DeviceHandle,
                                             DEVH_HSHUB_TAG, //HShb
                                             FALSE);
        }

        if (DeviceHandle->TtHandle) {
            USBPORT_DereferenceTt(FdoDeviceObject,
                           DeviceHandle->TtHandle,
                           DeviceHandle,
                           TT_DEVH_TAG); //dvTT
        }

        // delete it when empty
        USBPORT_DeleteDeviceHandle(FdoDeviceObject, DeviceHandle, Ev_InitializeDevice_Fail);
    }

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'iniD', DeviceHandle, 0, ntStatus);
    CATC_TRAP_ERROR(FdoDeviceObject, ntStatus);

    return ntStatus;
}


NTSTATUS
USBPORT_GetUsbDescriptor(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    UCHAR DescriptorType,
    PUCHAR DescriptorBuffer,
    PULONG DescriptorBufferLength
    )
/*++

Routine Description:

Arguments:

    DeviceHandle - ptr to device data structure created by class driver
                from a call to USBPORT_CreateDevice.

Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;

    USBPORT_INIT_SETUP_PACKET(setupPacket,
        USB_REQUEST_GET_DESCRIPTOR, // bRequest
        BMREQUEST_DEVICE_TO_HOST, // Dir
        BMREQUEST_TO_DEVICE, // Recipient
        BMREQUEST_STANDARD, // Type
        USB_DESCRIPTOR_MAKE_TYPE_AND_INDEX(DescriptorType, 0), // wValue
        0, // wIndex
        (USHORT)*DescriptorBufferLength); // wLength


    ntStatus =
        USBPORT_SendCommand(DeviceHandle,
                        FdoDeviceObject,
                        &setupPacket,
                        DescriptorBuffer,
                        *DescriptorBufferLength,
                        DescriptorBufferLength,
                        NULL);


    return ntStatus;
}


BOOLEAN
USBPORT_DeviceHasQueuedTransfers(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle
    )
/*++

Routine Description:

    Returns TRUE device has queued transfers.

--*/
{
    PDEVICE_EXTENSION devExt;
    BOOLEAN hasTransfers = FALSE;
    PLIST_ENTRY listEntry;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ASSERT_DEVICE_HANDLE(DeviceHandle);

    listEntry = &DeviceHandle->PipeHandleList;

    if (!IsListEmpty(listEntry)) {
        listEntry = DeviceHandle->PipeHandleList.Flink;
    }

    while (listEntry != &DeviceHandle->PipeHandleList) {

        PUSBD_PIPE_HANDLE_I nextHandle;
        PHCD_ENDPOINT endpoint = NULL;

        nextHandle = (PUSBD_PIPE_HANDLE_I) CONTAINING_RECORD(
                    listEntry,
                    struct _USBD_PIPE_HANDLE_I,
                    ListEntry);

        ASSERT_PIPE_HANDLE(nextHandle);

        listEntry = nextHandle->ListEntry.Flink;

        if (nextHandle->EpHandle != USB_ZERO_BW_ENDP_HANDLE){
            endpoint = USBPORT_EndpointFromHandle(nextHandle->EpHandle);

            if (USBPORT_Core_GetEndpointTransferCount(FdoDeviceObject, endpoint)) {
                hasTransfers = TRUE;
                break;
            }
        }
    }

    return hasTransfers;
}

VOID
USBPORT_AbortAllTransfers(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    BOOLEAN DeviceGone
    )
/*++

Routine Description:

    Abort all pending transfers associated with a device handle.

    This function is synchronous -- it is called after the device handle is
    removed from our tables so no new transfers can be posted.

    The idea here is to complete any transfers that may still be pending when
    the device is removed in case the client driver neglected to.

    On entry to this function the device is locked.

--*/
{
    PLIST_ENTRY listEntry;

    ASSERT_DEVICE_HANDLE(DeviceHandle);

    listEntry = DeviceHandle->PipeHandleList.Flink;

    while (listEntry != &DeviceHandle->PipeHandleList) {

        PUSBD_PIPE_HANDLE_I pipeHandle;
        PHCD_ENDPOINT endpoint = NULL;

        pipeHandle = (PUSBD_PIPE_HANDLE_I) CONTAINING_RECORD(
                    listEntry,
                    struct _USBD_PIPE_HANDLE_I,
                    ListEntry);

        ASSERT_PIPE_HANDLE(pipeHandle);

        if (pipeHandle->EpHandle != USB_ZERO_BW_ENDP_HANDLE) {
            endpoint = USBPORT_EndpointFromHandle(pipeHandle->EpHandle);

            if (DeviceGone) {
                KIRQL irql;
                irql = USBPORT_TxAcquireLock(FdoDeviceObject, NULL);
                endpoint->EpFlags.DeviceGone = 1;
                USBPORT_TxReleaseLock(FdoDeviceObject, NULL, irql);
            }

            // If there are any outstanding transfers on the
            // endpoint then synchronously abort them now.
            //
            if (USBPORT_Core_GetEndpointTransferCount(FdoDeviceObject, endpoint)) {
                USBPORT_Core_SyncAbortEndpoint(FdoDeviceObject, endpoint);
            }
        }

        listEntry = pipeHandle->ListEntry.Flink;
    }

    while (USBPORT_DeviceHasQueuedTransfers(FdoDeviceObject, DeviceHandle)) {

        // If everything is working correctly all transfers should have
        // been flushed out at this point.  But maybe other threads that
        // will complete the transfer cancelation are running slow and
        // and we need to wait for the outstanding transfer cancelation
        // to complete here.

        USBPORT_Wait(FdoDeviceObject, 100);
    }
}


VOID
USBPORT_AbortAllTransfersAndCloseAllPipes(
    PDEVICE_OBJECT  FdoDeviceObject,
    USBD_DEVH_EVENT Event
    )
/*++

Routine Description:

    All device handles are marked as Devh_Removed.  This will prevent
    any new transfers requests from being queued on the device handles.

    Synchronously abort all pending transfers associated with all device
    handles.

    The idea here is to complete any transfers that may still be pending
    when the host controller is being removed.

    Close all pipes on all devices.

    This routine must be called at PASSIVE_LEVEL.


    The Event parameter is only for diagnostic purposes to track why
    the device handles were marked as Devh_Removed.

--*/
{
    PDEVICE_EXTENSION   devExt;
    LIST_ENTRY          listHead;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // Build a referenced list of all device handles on the host
    // controller.
    //
    USBPORT_ReferenceAllDeviceHandles(FdoDeviceObject,
                                      &listHead,
                                      NULL,
                                      'DabA',   // AbaD
                                      TRUE,
                                      &listHead);

    while (!IsListEmpty(&listHead)) {

        PLIST_ENTRY         listEntry;
        PDEVH_REF_OBJ_EX    refObjEx;
        PUSBD_DEVICE_HANDLE deviceHandle;
        KIRQL               irql;

        listEntry = RemoveHeadList(&listHead);

        refObjEx = (PDEVH_REF_OBJ_EX) CONTAINING_RECORD(
            listEntry,
            struct _DEVH_REF_OBJ_EX,
            ExRefLink);

        deviceHandle = refObjEx->DeviceHandle;

        // Synchronize with any URB_FUNCTION_SELECT_CONFIGURATION or
        // URB_FUNCTION_SELECT_INTERFACE request that may be in progress
        // when the device is removed.
        //
        LOCK_DEVICE_CONFIGS(deviceHandle, FdoDeviceObject);

        // Setting the device handle state to Devh_Removed will cause
        // all subsequent calls to USBPORT_NeoReferenceDeviceHandle(),
        // USBPORT_iReferenceDeviceHandle(), and
        // USBPORT_rReferenceDeviceHandle() to fail.
        //
        // This will prevent any new transfer requests from being queued
        // on the device.
        //
        KeAcquireSpinLock(&devExt->Fdo.DevHandleListSpin, &irql);
        USBPORT_SetDeviceHandleState(FdoDeviceObject, deviceHandle, Devh_Removed, Event);
        KeReleaseSpinLock(&devExt->Fdo.DevHandleListSpin, irql);

        // Abort any outstanding transfers on the device.
        //
        USBPORT_AbortAllTransfers(FdoDeviceObject, deviceHandle, TRUE);

        // Close all configured pipes, if any, on the device.
        //
        if (deviceHandle->ConfigurationHandle) {

            USBPORT_InternalCloseConfiguration(deviceHandle,
                                               FdoDeviceObject,
                                               0);
        }

        // Close the default pipe on the device.
        //
        USBPORT_ClosePipe(deviceHandle,
                          FdoDeviceObject,
                          &deviceHandle->DefaultPipe);

        // End synchronization with any URB_FUNCTION_SELECT_CONFIGURATION
        // or URB_FUNCTION_SELECT_INTERFACE request that may be in progress
        // when the device is removed.
        //
        UNLOCK_DEVICE_CONFIGS(deviceHandle, FdoDeviceObject);

        // Release the reference on the device handle.  This frees the
        // refObjEx and the device handle is free to disappear at this
        // point.
        //
        USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                        deviceHandle,
                                        &listHead,
                                        'DabA',    // AbaD
                                        TRUE);
    }
}


VOID
USBPORT_ShiftBusContextDeviceHandleReference(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE OldDeviceHandle,
    PUSBD_DEVICE_HANDLE NewDeviceHandle
    )
/*++

Routine Description:

    This routine looks for a BusContext which references the
    OldDeviceHandle.  If one is found it is shifted to reference the
    NewDeviceHandle.

    It is assumed that there is at least one additional reference
    outstanding on the OldDeviceHandle and that removing the BusContext
    reference from the OldDeviceHandle does not remove its last
    outstanding reference.

    This routine is only intended to be called by USBPORT_CloneDevice().

--*/
{
    PDEVICE_EXTENSION   devExt;
    KIRQL               irql;
    PLIST_ENTRY         busContextListEntry;
    PBUS_CONTEXT        busContext;
    PLIST_ENTRY         refObjListEntry;
    PDEVH_REF_OBJ       refObj;
    BOOLEAN             foundRefObj;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.BusContextSpin, &irql);

    //
    // Walk the list of all BusContexts on this host controller
    // looking for one for the OldDeviceHandle.
    //
    for (busContextListEntry = devExt->Fdo.BusContextHead.Flink;
         busContextListEntry != &devExt->Fdo.BusContextHead;
         busContextListEntry = busContextListEntry->Flink) {

        busContext = CONTAINING_RECORD(busContextListEntry,
                                       BUS_CONTEXT,
                                       NextEntry);

        ASSERT_BUS_CONTEXT(busContext);

        if (busContext->DeviceHandle == OldDeviceHandle) {

            foundRefObj = FALSE;
            KeAcquireSpinLockAtDpcLevel(&devExt->Fdo.DevHandleListSpin);

            //
            // Walk the list of all references on the OldDeviceHandle
            // looking for the one for the BusContext.
            //
            for (refObjListEntry = OldDeviceHandle->RefListHead.Flink;
                 refObjListEntry != &OldDeviceHandle->RefListHead;
                 refObjListEntry = refObjListEntry->Flink) {

                refObj = CONTAINING_RECORD(refObjListEntry,
                                           DEVH_REF_OBJ,
                                           RefLink);

                if (refObj->Object == busContext &&
                    refObj->Tag == DEVH_BUS_CONTEXT_TAG) { //bsCT

                    //
                    // Shift the reference for the BusContext from the
                    // OldDeviceHandle to the NewDeviceHandle.
                    //
                    RemoveEntryList(refObjListEntry);

                    USBPORT_ASSERT(IsListEmpty(&OldDeviceHandle->RefListHead) == FALSE);

                    USBPORT_ASSERT(refObj->PendingFlag == FALSE);

                    InsertTailList(&NewDeviceHandle->RefListHead, refObjListEntry);

                    refObj->DeviceHandle = NewDeviceHandle;

                    foundRefObj = TRUE;
                    break;
                }
            }

            if (!foundRefObj) {
                // We might've hit low-res and failed the refObj allocation,
                // just transfer the refcount over
                USBPORT_ASSERT(OldDeviceHandle->RefCountNoRes);
                if (OldDeviceHandle->RefCountNoRes) {
                    NewDeviceHandle->RefCountNoRes ++;
                    OldDeviceHandle->RefCountNoRes --;
                }
            }

            //
            // Shift the BusContext from the OldDeviceHandle to the
            // NewDeviceHandle.
            //
            busContext->DeviceHandle = NewDeviceHandle;

            KeReleaseSpinLockFromDpcLevel(&devExt->Fdo.DevHandleListSpin);

            break;
        }
    }

    KeReleaseSpinLock(&devExt->Fdo.BusContextSpin, irql);
}


NTSTATUS
USBPORT_CloneDevice(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE OldDeviceHandle,
    PUSBD_DEVICE_HANDLE NewDeviceHandle
    )
/*++

Routine Description:

    Internal function.

    This routine clones the configuration from the 'old' device handle onto
    the new device handle/

Arguments:

    NewDeviceHandle - ptr to device data structure created by class driver
                in USBPORT_CreateDevice.

    OldDeviceHandle - ptr to device data structure created by class driver
                in USBPORT_CreateDevice.

    FdoDeviceObject - USBPORT device object for the USB bus this device is on.

Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION devExt;
    USBD_STATUS usbdStatus;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // assume success
    ntStatus = STATUS_SUCCESS;

    LOGENTRY(FdoDeviceObject, LOG_PNP, 'Cln>',
        OldDeviceHandle, NewDeviceHandle, 0);

    DbgTrace((UsbpDebugTrace,"'Cloning Device\n"));
    TEST_TRAP();

    // make sure we have two valid device handles

    if (!USBPORT_iValidDeviceHandle(FdoDeviceObject, OldDeviceHandle)) {
        // this is most likely a bug in the hub
        // driver
        //TEST_TRAP();
        USBPORT_ASSERTMSG("Invalid Device Handle",FALSE);

        // chances are that the device handle is bad becuse the
        // device is gone.
        return STATUS_DEVICE_NOT_CONNECTED;
    }

    if (!USBPORT_iValidDeviceHandle(FdoDeviceObject, NewDeviceHandle)) {
        // this is most likely a bug in the hub
        // driver
        TEST_TRAP();
        USBPORT_ASSERTMSG("Invalid Device Handle",FALSE);

        // chances are that the device handle is bad becuse the
        // device is gone.
        return STATUS_DEVICE_NOT_CONNECTED;
    }

    LOGENTRY(FdoDeviceObject, LOG_PNP, 'Cln+',
        OldDeviceHandle, NewDeviceHandle, 0);


    // There are two cases where this API is called:

    // case 1 - the device driver has requested a reset of the device.
    // In this event the device has returned to the unconfigured state
    // and has been re-addressed with the 'NewDeviceHandle'
    //
    // case 2 - the controller has been shut off -- thanks to power
    // management.  In this case the device is also in the unconfigured
    // state and associated with the 'NewDeviceHandle' device handle


    // make sure we are dealing with the same device
    if (TEST_FLAG(OldDeviceHandle->DeviceFlags, USBPORT_DEVICEFLAG_SUPPORTS_MSOS20_DESCRIPTOR)) {
        if (RtlCompareMemory(&NewDeviceHandle->DeviceDescriptor,
                             &OldDeviceHandle->OriginalDeviceDescriptor,
                             sizeof(OldDeviceHandle->DeviceDescriptor)) !=
                             sizeof(OldDeviceHandle->DeviceDescriptor)) {

            return STATUS_UNSUCCESSFUL;
        }
    } else if (RtlCompareMemory(&NewDeviceHandle->DeviceDescriptor,
                                &OldDeviceHandle->DeviceDescriptor,
                                sizeof(OldDeviceHandle->DeviceDescriptor)) !=
                                sizeof(OldDeviceHandle->DeviceDescriptor)) {

        return STATUS_UNSUCCESSFUL;
    }

    // Clone the driver name
    //
    RtlCopyMemory(NewDeviceHandle->DriverName,
                  OldDeviceHandle->DriverName,
                  sizeof(NewDeviceHandle->DriverName));

    // make sure the 'new device' is unconfigured
    USBPORT_ASSERT(NewDeviceHandle->ConfigurationHandle == NULL);

    // before performing the clone operation remove the device handle
    // and wait for any pending URBs to drain
    USBPORT_RemoveDeviceHandle(FdoDeviceObject, OldDeviceHandle, Ev_Clone, FALSE, FALSE);

    // clone the config
    NewDeviceHandle->ConfigurationHandle =
        OldDeviceHandle->ConfigurationHandle;

    if (OldDeviceHandle->ConfigurationHandle != NULL) {

        // set the device to the previous configuration,
        // Send the 'set configuration' command.

        USBPORT_INIT_SETUP_PACKET(setupPacket,
                USB_REQUEST_SET_CONFIGURATION, // bRequest
                BMREQUEST_HOST_TO_DEVICE, // Dir
                BMREQUEST_TO_DEVICE, // Recipient
                BMREQUEST_STANDARD, // Type
                NewDeviceHandle->ConfigurationHandle->\
                    ConfigurationDescriptor->bConfigurationValue, // wValue
                0, // wIndex
                0); // wLength


        USBPORT_SendCommand(NewDeviceHandle,
                            FdoDeviceObject,
                            &setupPacket,
                            NULL,
                            0,
                            NULL,
                            &usbdStatus);

        DbgTrace((UsbpDebugTrace,"' SendCommand, SetConfiguration returned 0x%x\n", usbdStatus));

        if (USBD_ERROR(usbdStatus)) {

            DbgTrace((UsbpDebugTrace, "failed to 'set' the configuration on a clone\n"));

            //
            // the set_config failed, this can happen if the device has been
            // removed or if the device has lost its brains.
            // We continue with the cloning process for the endpoints so they
            // will be properly freed when the 'new' device handle is
            // eventually removed.
            //

            ntStatus = SET_USBD_ERROR(NULL, usbdStatus);

        }
    }

    // clone any alternate interface settings, since we restore the pipes to
    // the state at the time of hibernate they may be associated with
    // particular alternate interfaces

    // walk the interface chain
    if (OldDeviceHandle->ConfigurationHandle != NULL &&
        NT_SUCCESS(ntStatus)) {

        PUSBD_CONFIG_HANDLE cfgHandle;
        PLIST_ENTRY listEntry;
        PUSBD_INTERFACE_HANDLE_I iHandle;

        cfgHandle = NewDeviceHandle->ConfigurationHandle;
        GET_HEAD_LIST(cfgHandle->InterfaceHandleList, listEntry);

        while (listEntry &&
               listEntry != &cfgHandle->InterfaceHandleList) {

            // extract the handle from this entry
            iHandle = (PUSBD_INTERFACE_HANDLE_I) CONTAINING_RECORD(
                        listEntry,
                        struct _USBD_INTERFACE_HANDLE_I,
                        InterfaceLink);

            ASSERT_INTERFACE(iHandle);

            // see if we currently have an alt setting selected
            if (iHandle->HasAlternateSettings) {

                NTSTATUS status;
                //
                // If we have alternate settings we need
                // to send the set interface command.
                //

                USBPORT_INIT_SETUP_PACKET(setupPacket,
                    USB_REQUEST_SET_INTERFACE, // bRequest
                    BMREQUEST_HOST_TO_DEVICE, // Dir
                    BMREQUEST_TO_INTERFACE, // Recipient
                    BMREQUEST_STANDARD, // Type
                    iHandle->InterfaceDescriptor.bAlternateSetting, // wValue
                    iHandle->InterfaceDescriptor.bInterfaceNumber, // wIndex
                    0); // wLength

                status = USBPORT_SendCommand(NewDeviceHandle,
                                             FdoDeviceObject,
                                             &setupPacket,
                                             NULL,
                                             0,
                                             NULL,
                                             &usbdStatus);

                LOGENTRY(FdoDeviceObject, LOG_PNP, 'sIF2',
                         status,
                         iHandle->InterfaceDescriptor.bAlternateSetting,
                         iHandle->InterfaceDescriptor.bInterfaceNumber);

            }

            listEntry = iHandle->InterfaceLink.Flink;
        }
    }

    // clone the TT and TT related data
    if (TEST_FLAG(NewDeviceHandle->DeviceFlags, USBPORT_DEVICEFLAG_HSHUB)) {

        // Note: There does not appear to be any need for the TT
        // migration logic inside this if statement and it leads to an
        // accumulation of TT resource allocations each time a high
        // speed hub is reset or disabled/enabled.  All of the code in
        // this if statement should probably be replaced by a call to
        // USBPORT_RemoveTTs() but since there is non-zero risk of
        // introducing regressions from making that change it is being
        // left as-is for now.

        // remove the TT entries from the old handle and add them
        // to the new handle

        while (!IsListEmpty(&OldDeviceHandle->TtListHead)) {
            PTRANSACTION_TRANSLATOR tt;
            PLIST_ENTRY listEntry;

            listEntry = RemoveTailList(&OldDeviceHandle->TtListHead);
            USBPORT_ASSERT(listEntry != NULL);

            tt = (PTRANSACTION_TRANSLATOR) CONTAINING_RECORD(
                        listEntry,
                        struct _TRANSACTION_TRANSLATOR,
                        TtLink);
            ASSERT_TT(tt);

            tt->DeviceAddress = NewDeviceHandle->DeviceAddress;
            InsertHeadList(&NewDeviceHandle->TtListHead, &tt->TtLink);

            // shift the rferences on the TT to the new handle
            USBPORT_iShiftReferenceTt(FdoDeviceObject, tt,  OldDeviceHandle, NewDeviceHandle, TT_INIT_TAG);

        }

        NewDeviceHandle->TtCount = OldDeviceHandle->TtCount;
    }

    // copy the pipe handle list, for each pipe we  will need to re-open
    // the endpoint or re-init the endpoint.
    //
    // if the device did not lose its brains then all we need to do
    // is update the host controllers idea of what the endpoint address is.
    // this has the added advantage of allowing a reset even when transfers
    // are queued to the HW although we don't allow that.

    while (!IsListEmpty(&OldDeviceHandle->PipeHandleList)) {

        PHCD_ENDPOINT endpoint = NULL;
        PLIST_ENTRY listEntry = OldDeviceHandle->PipeHandleList.Flink;
        PUSBD_PIPE_HANDLE_I pipeHandle;
        PTRANSACTION_TRANSLATOR transactionTranslator = NULL;

        // see if we are dealing with a TT
        if (NewDeviceHandle->TtHandle != NULL) {
            transactionTranslator = NewDeviceHandle->TtHandle;
            ASSERT_TT(transactionTranslator);
        }

        pipeHandle = (PUSBD_PIPE_HANDLE_I) CONTAINING_RECORD(
                    listEntry,
                    struct _USBD_PIPE_HANDLE_I,
                    ListEntry);

        LOGENTRY(FdoDeviceObject, LOG_PNP, 'CLNE', pipeHandle, 0, 0);
        ASSERT_PIPE_HANDLE(pipeHandle);

        USBPORT_RemovePipeHandle(OldDeviceHandle,
                                 pipeHandle);

        // we need to special case the default pipe because it
        // is embedded in the DeviceHandle.
        //
        // Since NewDeviceHandle is a newly created device
        // the endpoint associated with it is valid, so is
        // the one for the 'OldDeviceHandle'

        if (pipeHandle != &OldDeviceHandle->DefaultPipe) {

            USB_MINIPORT_STATUS mpStatus;

            USBPORT_AddPipeHandle(NewDeviceHandle, pipeHandle);

            // skip re-init for zero bw endpoints becuase we have
            // no endpoint structure -- these are ghost endpoints
            if (!TEST_FLAG(pipeHandle->PipeStateFlags, USBPORT_PIPE_ZERO_BW) &&
                pipeHandle->EpHandle != USB_ZERO_BW_ENDP_HANDLE) {

                HC_GLOBAL_EP_STATE geps;
                ULONG contextIdx;

                // note endpoint is still pinned to this pipe
                endpoint = USBPORT_EndpointFromHandle(pipeHandle->EpHandle);
                ASSERT_ENDPOINT(endpoint);

                endpoint->Parameters.DeviceAddress =
                        NewDeviceHandle->DeviceAddress;

                //
                // If device is PW boot device then update its device address
                //
                for (contextIdx = 0; contextIdx < DUMP_MAX_CONTEXT_COUNT; contextIdx++) {
                    if (devExt->DumpContext[contextIdx] != NULL) {

                        if (OldDeviceHandle->DeviceAddress ==
                            devExt->DumpContext[contextIdx]->DeviceAddress) {

                            devExt->DumpContext[contextIdx]->DeviceAddress =
                                NewDeviceHandle->DeviceAddress;
                        }

                        if (OldDeviceHandle->DeviceAddress ==
                            devExt->DumpContext[contextIdx]->ParentHubDeviceAddress) {

                            devExt->DumpContext[contextIdx]->ParentHubDeviceAddress =
                                NewDeviceHandle->DeviceAddress;
                        }
                    }
                }

                LOGENTRY(FdoDeviceObject, LOG_PNP, 'CLN1', endpoint, 0, 0);
                // this just returns the current state, it allows us to
                // detected 'Zapped'
                geps =  USBPORT_SetGlobalEndpointState(FdoDeviceObject, endpoint, Ev_gEp_Clone);

                if (geps == GEp_Zapped) {
                    // re-open
                    ENDPOINT_REQUIREMENTS requirements;

                    if (transactionTranslator != NULL) {
                        endpoint->Parameters.TtDeviceAddress =
                            transactionTranslator->DeviceAddress;
                    }


                    // call open request to minport, all the endpoint structures are still valid we just need
                    // to re-add it to the schedule.

                    USBPORT_ASSERT(endpoint->MiniportEndpointDataSize);
                    RtlZeroMemory(&endpoint->MiniportEndpointData[0], endpoint->MiniportEndpointDataSize);
                    RtlSecureZeroMemory(endpoint->Parameters.CommonBufferVa, endpoint->Parameters.CommonBufferBytes);

                    LOGENTRY(FdoDeviceObject, LOG_MISC, 'clRO', pipeHandle,
                        endpoint, 0);
                    USBPORT_KdPrint((1, " Clone Endpoint %p (ZAPPED)\n", endpoint));

                    // query requirements (although they should not change)
                    // just in case the miniport does some initialization here
                    MPx_QueryEndpointRequirements(devExt, endpoint, &requirements);

                    mpStatus = MPx_OpenEndpoint(devExt, endpoint, NULL);
                    // in this UNIQUE situation this API is should not,
                    // if it does we leave it in the pause,'ZAPPED'
                    // state
                    if (mpStatus == USBMP_STATUS_SUCCESS) {

                        // reset the endpoint state so we can re-initialize this clears
                        // this clears the 'ZAPPED' flag such that the endpoint can be polled in the miniport
                        USBPORT_SetGlobalEndpointState(FdoDeviceObject, endpoint, Ev_gEp_Reset);

                        // we need to sync the endpoint state with the miniport, when first opened the miniport
                        // puts the endpoint in status HALT state PAUSE.

                        USBPORT_SetGlobalEndpointState(FdoDeviceObject, endpoint, Ev_gEp_ReqActive);
                    }

                } else {

                    LOGENTRY(FdoDeviceObject, LOG_MISC, 'clR2', pipeHandle,
                        endpoint, 0);
                    // if this device has an associated TT then
                    // we will need to do a little more here
                    if (transactionTranslator != NULL) {
                        endpoint->Parameters.TtDeviceAddress =
                            transactionTranslator->DeviceAddress;
                    }

                    // this endpoint is already in the schedule,
                    // poke-it with the new address.

                    mpStatus = MPx_PokeEndpoint(devExt, endpoint, endpoint->Tt);

                    // in this UNIQUE situation this API is not allowed
                    // (and should not) fail
                    USBPORT_ASSERT(mpStatus == USBMP_STATUS_SUCCESS);

                    // The endpoint on the device should have had its data
                    // toggle reset back to Data0 so reset the data toggle
                    // on the host endpoint to match.
                    //
                    MPf_SetEndpointDataToggle(devExt, endpoint, 0);

                    // clear halt status
                    MPf_SetEndpointStatus(devExt, endpoint, ENDPOINT_STATUS_RUN);

                }
            }
        }
    }

    // the pipes and config have been cloned, the final step is to free the
    // 'OldDeviceData' ie the old handle.

    // put the old 'default' pipe back on the list before
    // we close it
    USBPORT_AddPipeHandle(OldDeviceHandle,
                          &OldDeviceHandle->DefaultPipe);

    // we should aways have a default pipe, this will free
    // the endpoint
    USBPORT_ClosePipe(OldDeviceHandle,
                      FdoDeviceObject,
                      &OldDeviceHandle->DefaultPipe);

    if (OldDeviceHandle->DeviceAddress != USB_DEFAULT_DEVICE_ADDRESS) {
        USBPORT_FreeUsbAddress(FdoDeviceObject, OldDeviceHandle->DeviceAddress);
        OldDeviceHandle->DeviceAddress = USB_DEFAULT_DEVICE_ADDRESS;
    }

    //
    // Shift the BusContext from the OldDeviceHandle to the
    // NewDeviceHandle.
    //
    USBPORT_ShiftBusContextDeviceHandleReference(
        FdoDeviceObject,
        OldDeviceHandle,
        NewDeviceHandle);

    // release the refs from the old handld
    if (OldDeviceHandle->HsHubDeviceHandle) {
        USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                        OldDeviceHandle->HsHubDeviceHandle,
                                        OldDeviceHandle,
                                        DEVH_HSHUB_TAG, //HShb
                                        FALSE);
    }

    if (OldDeviceHandle->TtHandle) {
        USBPORT_DereferenceTt(FdoDeviceObject,
                       OldDeviceHandle->TtHandle,
                       OldDeviceHandle,
                       TT_DEVH_TAG); //dvTT
    }

    USBPORT_DeleteDeviceHandle(FdoDeviceObject, OldDeviceHandle,
        Ev_RestoreDevice_Success);

    // STATUS_SUCCESS indicates that OldDeviceHandle
    // is NOT valid and NewDeviceHandle is valid
    ntStatus = STATUS_SUCCESS;

    return ntStatus;
}


NTSTATUS
USBPORT_ReferenceTt(
    PDEVICE_OBJECT FdoDeviceObject,
    PTRANSACTION_TRANSLATOR Tt,
    PVOID Object,
    ULONG Tag
    )
/*++

Routine Description:

    This routine puts a reference on a TT object. It is used in a simlr fashion to device handle
    references, however the references are olnly internal.

Arguments:

    Tt - Transaction Translator to add the ref

    Object - object to associate with this ref

    Tag - signature that ids this ref

Return Value:

    error if the handle can not be referenced.

--*/
{

    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    NTSTATUS nts;
    PTT_REF_OBJ refObj;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.GlobalTtSpin, &irql);

    ASSERT_TT(Tt);

    DbgTrace((UsbpDebugTrace, "'USBPORT_ReferenceTt %p\n", Tt));

    do {

        USBPORT_ASSERT(Tt->TtState != TT_Deleted);

        if (Tt->TtState == TT_Removed) {
            nts = STATUS_NO_SUCH_DEVICE;
            break;
        }

        ALLOC_POOL_Z(refObj, NonPagedPoolNx, sizeof(struct _TT_REF_OBJ));

        if (!refObj) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        refObj->Tt = Tt;
        refObj->Sig = SIG_TT_REF;
        refObj->Object = Object;
        refObj->Tag = Tag;
        LOGENTRY(FdoDeviceObject, LOG_MISC, 'trf-', Tt, Object, Tag);
        LOGENTRY(FdoDeviceObject, LOG_MISC, 'trf1', 0, refObj, 0);

        InsertTailList(&Tt->RefListHead,  &refObj->RefLink);
        nts = STATUS_SUCCESS;

    } WHILE (0);

    KeReleaseSpinLock(&devExt->Fdo.GlobalTtSpin, irql);

    return nts;
}


VOID
USBPORT_DereferenceTt(
    PDEVICE_OBJECT FdoDeviceObject,
    PTRANSACTION_TRANSLATOR Tt,
    PVOID Object,
    ULONG Tag
    )
/*++

Routine Description:

    This routine removes a reference on a TT object.

Arguments:

    Tt - Transaction Translator to remove the ref

    Object - object to associate with this ref

    Tag - signature that ids this ref


--*/
{
    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    PTT_REF_OBJ refObj = NULL;
    PLIST_ENTRY le;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.GlobalTtSpin, &irql);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'Trf-', Tt, Object, Tag);
    ASSERT_TT(Tt);

    le = Tt->RefListHead.Flink;

    // locate the ref and remove it
    while (le != &Tt->RefListHead) {

        PTT_REF_OBJ nxtObj;

        nxtObj = (PTT_REF_OBJ) CONTAINING_RECORD(
                    le,
                    struct _TT_REF_OBJ,
                    RefLink);

        le = nxtObj->RefLink.Flink;

        if (nxtObj->Object == Object &&
            nxtObj->Tag == Tag) {
            refObj = nxtObj;
            break;
        }
    }

    // we should have found the ref
    //
    if (refObj != NULL) {
        RemoveEntryList(&refObj->RefLink);

        UsbFreePool(refObj);
        refObj = NULL;
    } else {
        USBPORT_ASSERTMSG("Object ref should always be found",FALSE);
    }

    if (IsListEmpty(&Tt->RefListHead)) {

        Tt->TtState = TT_Deleted;

        ASSERT_TT(Tt);

        RemoveEntryList(&Tt->TtGlobalLink);

        UNSIG(Tt);
        if (Tt->HsbTtContextPtr) {
            UsbFreePool(Tt->HsbTtContextPtr);
            Tt->HsbTtContextPtr = NULL;
        }
        UsbFreePool(Tt);
    }

    KeReleaseSpinLock(&devExt->Fdo.GlobalTtSpin, irql);
}


VOID
USBPORT_iShiftReferenceTt(
    PDEVICE_OBJECT FdoDeviceObject,
    PTRANSACTION_TRANSLATOR Tt,
    PVOID OldObject,
    PVOID NewObject,
    ULONG Tag
    )
/*++

Routine Description:

   This routine shifts the tt ref from one object to another. We use it as part
   of the handle cloning opertaion.  The ref itself remians on the TTs list.

Arguments:

    Tt - Transaction Translator to add the ref

    Object - object to associate with this ref

    Tag - signature that ids this ref

Return Value:

    error if the handle can not be referenced.

--*/
{

    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    PLIST_ENTRY le;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.GlobalTtSpin, &irql);

    ASSERT_TT(Tt);

    le = Tt->RefListHead.Flink;

    DbgTrace((UsbpDebugTrace, "'USBPORT_iShiftReferenceTt %p\n", Tt));

    // locate the ref and modify it
    while (le != &Tt->RefListHead) {

        PTT_REF_OBJ nxtObj;

        nxtObj = (PTT_REF_OBJ) CONTAINING_RECORD(
                    le,
                    struct _TT_REF_OBJ,
                    RefLink);

        le = nxtObj->RefLink.Flink;

        if (nxtObj->Object == OldObject &&
            nxtObj->Tag == Tag) {
            nxtObj->Object = NewObject;
            break;
        }
    }


    KeReleaseSpinLock(&devExt->Fdo.GlobalTtSpin, irql);

    return;
}



PTRANSACTION_TRANSLATOR
USBPORT_GetTt(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE HsHubDeviceHandle,
    USHORT TtPortNumber,
    PVOID Object,
    ULONG Tag
    )
/*++

Routine Description:

    This routine returns the TT for a particular device.  It extracts the TT from the
    High Speed hub list using the device handle for the HS hub and the ttport number.

    The tt is returned with a reference the caller must release.

Arguments:

    HsHubDeviceHandle - Hs Hub Device handle referenced by

Return Value:

    tt associated with this port number

--*/
{

    PDEVICE_EXTENSION devExt;
    PTRANSACTION_TRANSLATOR tt = NULL;
    PLIST_ENTRY listEntry;
    NTSTATUS nts;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    do {

        USBPORT_ASSERT(HsHubDeviceHandle->DeviceSpeed == UsbHighSpeed);

        USBPORT_ASSERT(HsHubDeviceHandle->TtCount &&
                           !IsListEmpty(&HsHubDeviceHandle->TtListHead));

        if (!HsHubDeviceHandle->TtCount ||
            IsListEmpty(&HsHubDeviceHandle->TtListHead)) {
            // This case should not normally happen.  If it does,
            // an error has occurred somewhere.
            break;
        }

        if (HsHubDeviceHandle->TtCount > 1) {

            GET_HEAD_LIST(HsHubDeviceHandle->TtListHead, listEntry);

            while (listEntry != NULL &&
                   listEntry != &HsHubDeviceHandle->TtListHead) {

                tt = (PTRANSACTION_TRANSLATOR) CONTAINING_RECORD(
                        listEntry,
                        struct _TRANSACTION_TRANSLATOR,
                        TtLink);
                ASSERT_TT(tt);

                if (tt->TT_Port == TtPortNumber) {
                    break;
                }

                listEntry = tt->TtLink.Flink;
                tt = NULL;
            }

        } else {
            // single TT, use the one tt structure regardless of port
            GET_HEAD_LIST(HsHubDeviceHandle->TtListHead, listEntry);
            tt = (PTRANSACTION_TRANSLATOR) CONTAINING_RECORD(
                    listEntry,
                    struct _TRANSACTION_TRANSLATOR,
                    TtLink);
            ASSERT_TT(tt);
        }

        // we should have selected a tt
        USBPORT_ASSERT(tt != NULL);

    } WHILE (0);

    if (tt) {
        // ref it
        nts = USBPORT_ReferenceTt(FdoDeviceObject,
                                  tt,
                                  Object,
                                  Tag); //epTT

        if (NT_ERROR(nts)) {
            // don't return tt if we cannot ref it.
            tt = NULL;
        }
    }

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'gTTa', HsHubDeviceHandle, TtPortNumber, tt);

    return tt;
}


NTSTATUS
USBPORT_InitializeTT(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE HsHubDeviceHandle,
    USHORT Port
    )
/*++

Routine Description:

    Initialze the TT list used to track this hub.  This routine allocates a tt structure and attaches
    to the hubs list of TTs.  The initial ref is created here for the TT.

Arguments:

    HubDeviceHandle - referenced by the hub driver.

    Port - port number associated withe this TT on the Hosgh speed hub.

Return Value:

    nt status code

--*/
{
    PDEVICE_EXTENSION devExt;
    PTRANSACTION_TRANSLATOR transactionTranslator;
    USHORT siz;
    extern ULONG USB2LIB_TtContextSize;
    NTSTATUS ntStatus;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(USBPORT_IS_TT_ENABLED(devExt));

    siz = sizeof(struct _TRANSACTION_TRANSLATOR);

    do {
        KIRQL irql;

        ALLOC_POOL_Z(transactionTranslator, NonPagedPoolNx, siz);

        if (transactionTranslator == NULL) {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        transactionTranslator->TtState = TT_Init;
        transactionTranslator->Sig = SIG_TT;
        InitializeListHead(&transactionTranslator->RefListHead);

        // reference the tt with the devh structure so it will not be prematurely
        // freed up. These refs are released whe the TTs are cleaned up along with the hub
        // dev handle.
        ntStatus = USBPORT_ReferenceTt(FdoDeviceObject,
                                       transactionTranslator,
                                       HsHubDeviceHandle,
                                       TT_INIT_TAG); //inTT

        if (NT_ERROR(ntStatus)) {
            UsbFreePool(transactionTranslator);
            break;
        }

        KeAcquireSpinLock(&devExt->Fdo.GlobalTtSpin, &irql);
        InsertTailList(&devExt->Fdo.GlobalTtListHead,
                       &transactionTranslator->TtGlobalLink);
        KeReleaseSpinLock(&devExt->Fdo.GlobalTtSpin, irql);

        transactionTranslator->DeviceAddress = HsHubDeviceHandle->DeviceAddress;
        transactionTranslator->TT_Port = Port;

        if (TEST_FLAG(HsHubDeviceHandle->DeviceFlags, USBPORT_DEVICEFLAG_HSHUB_RESET_TT_ON_CANCEL)) {
            SET_FLAG(transactionTranslator->TtFlags, TTFLAGS_RESET_TT_ON_CANCEL);
        }

        if (TEST_FLAG(HsHubDeviceHandle->DeviceFlags, USBPORT_DEVICEFLAG_HSHUB_NO_CLEAR_TT_BUFFER_ON_CANCEL)) {
            SET_FLAG(transactionTranslator->TtFlags, TTFLAGS_NO_CLEAR_TT_BUFFER_ON_CANCEL);
        }

        // transactionTranslator->PdoDeviceObject = devExt->Fdo.RhPdoPtr;
        // each translator is a virtual 1.1 bus
        transactionTranslator->TotalBusBandwidth =
            USB_11_BUS_BANDWIDTH;

        ntStatus = MPx_InitializeHsbTt(devExt, transactionTranslator);

        if (NT_ERROR(ntStatus)) {

            // Error cleanup.  The only reference that should exist on
            // the TT at this point is the TT_INIT_TAG reference taken
            // above.  Releasing this reference could cause the
            // TT->HsbTtContextPtr allocation to be freed (if allocated)
            // along with the TT allocation itself, in addition to
            // removing the TT from the Fdo.GlobalTtListHead.
            //
            USBPORT_DereferenceTt(FdoDeviceObject,
                                  transactionTranslator,
                                  HsHubDeviceHandle,
                                  TT_INIT_TAG); //inTT
            break;
        }

        InsertTailList(&HsHubDeviceHandle->TtListHead,
                       &transactionTranslator->TtLink);

        ntStatus = STATUS_SUCCESS;
    } WHILE (0);

    return ntStatus;
}


VOID
USBPORT_RemoveTT(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE HsHubDeviceHandle,
    PTRANSACTION_TRANSLATOR Tt
    )
/*++

Routine Description:

    Releases a previously allocated Tt.  This routine releases any bandwidth allocated to the TT

Arguments:

    HsHubDeviceHandle - device handle of owning hub.

    Tt - TT to release

Return Value:

    None

--*/
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    Tt->TtState = TT_Removed;

    USBPORT_DereferenceTt(FdoDeviceObject,
                          Tt,
                          HsHubDeviceHandle,
                          TT_INIT_TAG); //inTT
}


VOID
USBPORT_RemoveTTs(
    PDEVICE_OBJECT      FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle
    )
/*++

Routine Description:

    Release all previously allocated Tts for a Hs Hub.  No op if the
    device is not a Hs Hub with allocated Tts.

Arguments:

    DeviceHandle - Device handle, which may or may not be a Hs Hub.

Return Value:

    None

--*/
{
    PTRANSACTION_TRANSLATOR tt;
    PLIST_ENTRY             listEntry;

    //
    // Free any Tt handles associated with this device handle.  Only Hs
    // Hubs should have Tt handles, otherwise the TtListHead should be
    // empty.
    //
    while (!IsListEmpty(&DeviceHandle->TtListHead)) {

        listEntry = RemoveHeadList(&DeviceHandle->TtListHead);
        tt = (PTRANSACTION_TRANSLATOR) CONTAINING_RECORD(
                        listEntry,
                        struct _TRANSACTION_TRANSLATOR,
                        TtLink);
        ASSERT_TT(tt);

        USBPORT_RemoveTT(FdoDeviceObject, DeviceHandle, tt);
    }
}


/*
    These routines will merge and/or replace the LOCK and ADD functions for device handles
*/

PUSBD_DEVICE_HANDLE
USBPORT_AllocDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Allocates and intializes a device handle. Also creates the initial reference, the
    handle is only freed when all references are removed.


Arguments:

    FdoDeviceObject - Fdo we want to associate this device handle with

Return Value:

    NULL or Device handle

--*/
{
    PDEVICE_EXTENSION devExt;
    PUSBD_DEVICE_HANDLE devh;
    KIRQL irql;
    NTSTATUS nts;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ALLOC_POOL_Z(devh, NonPagedPoolNx,
                        sizeof(USBD_DEVICE_HANDLE));

    while (devh) {

        devh->Sig = SIG_DEVICE_HANDLE;
        devh->DhState = Devh_Created;
        InitializeListHead(&devh->RefListHead);
        devh->PendingCount = 0;
        KeInitializeEvent(&devh->RemoveDevhEvent,
                          NotificationEvent,
                          USBD_EVENT_STATE_NOT_SIGNALED);

        devh->FdoDevObj = FdoDeviceObject;
        // initialize the ref list

        nts = USBPORT_iReferenceDeviceHandle(FdoDeviceObject,
                                             devh,
                                             devh,
                                             DEVH_INIT_TAG); //dvh+

        if (NT_ERROR(nts)) {
            UsbFreePool(devh);
            devh = NULL;
            break;
        }

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'addD', devh, 0, 0);

        KeAcquireSpinLock(&devExt->Fdo.DevHandleListSpin, &irql);

        InsertTailList(&devExt->Fdo.DeviceHandleList,  &devh->DevhLink);

        KeReleaseSpinLock(&devExt->Fdo.DevHandleListSpin, irql);
        break;
    }

    return devh;
}


VOID
USBPORT_DeleteDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE Devh,
    USBD_DEVH_EVENT Event
    )
/*++

Routine Description:

    Releases the intial ref on the device handle so that it may be closed and freed.

Arguments:

    FdoDeviceObject - Fdo the device handle is associated with

Return Value:

   none

--*/
{

    PDEVICE_EXTENSION devExt;
    KIRQL irql;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.DevHandleListSpin, &irql);

    // removes from DeviceHandleList
    RemoveEntryList(&Devh->DevhLink);

    // Check if a true device removal and if this is a root-attached device
    if ((Event == Ev_RemoveDone) && (Devh->EtwUsbDevice.PortPath.PortPathDepth == 1)) {
        PLIST_ENTRY le;
        PUSBD_DEVICE_HANDLE nextHandle;
        ULONG rootPortNumber;

        // If a tree of devices is unplugged from a root port, the root-attached hub
        // will be the first device handle to be deleted.  This would allow the hub driver
        // to selectively suspend the root hub while the downstream devices are
        // pending removal.  The expectation of the endpoint state machine is that endpoints
        // will be removed/deleted while the host controller is still
        // active.  To prevent the root hub from suspending we will identify
        // orphaned downstream devices here and fail suspend requests until their device handles
        // are deleted.

        rootPortNumber = Devh->EtwUsbDevice.PortPath.PortPath[0];
        le = devExt->Fdo.DeviceHandleList.Flink;

        // scan for devices downstream of the device we are removing
        while (le != &devExt->Fdo.DeviceHandleList) {
            nextHandle = (PUSBD_DEVICE_HANDLE) CONTAINING_RECORD(
                          le,
                          struct _USBD_DEVICE_HANDLE,
                          DevhLink);

            le = nextHandle->DevhLink.Flink;

            if (nextHandle->EtwUsbDevice.PortPath.PortPath[0] == rootPortNumber) {
                // Found one, mark it as orphaned
                SET_DEVICE_FLAG(nextHandle, USBPORT_DEVICEFLAG_ORPHANED);

                // increment the orphaned device count to keep the root hub from suspending
                // until all orphans are removed.
                devExt->Fdo.OrphanedDeviceCount++;
            }
        }
    } else if (TEST_DEVICE_FLAG(Devh, USBPORT_DEVICEFLAG_ORPHANED)) {
        USBPORT_ASSERT(devExt->Fdo.OrphanedDeviceCount != 0);
        // Device was an orphan.  Decrement the orphaned device count.  When it goes to 0 the
        // root hub will be allowed to suspend
        devExt->Fdo.OrphanedDeviceCount--;
    }

    USBPORT_SetDeviceHandleState(FdoDeviceObject, Devh,
        Devh_DeletePending, Event);

    InsertTailList(&devExt->Fdo.DeviceHandleDeletedList,  &Devh->DevhLink);

    KeReleaseSpinLock(&devExt->Fdo.DevHandleListSpin, irql);

    // tag bug 1367210
    // release the device address no so that it may be recycled
    if (Devh->DeviceAddress != USB_DEFAULT_DEVICE_ADDRESS) {
        USBPORT_FreeUsbAddress(FdoDeviceObject, Devh->DeviceAddress);
        Devh->DeviceAddress = USB_DEFAULT_DEVICE_ADDRESS;
    }

    USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                    Devh,
                                    Devh,
                                    DEVH_INIT_TAG, //dvh+
                                    FALSE);
}


NTSTATUS
USBPORT_NeoReferenceDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PVOID Object,
    PIRP Irp,
    ULONG Tag,
    BOOLEAN PendingFlag
    )
/*++

Routine Description:

    This routine puts a reference on an internal device handle object so that it does
    not get deleted out from under someone.

    These routines replace the original lock and validate functions but require
    participation from the hub driver and any other driver that utilizes the device handles.

    This is the primary worker routine that referbces the device handle, it is called by
    several helper functions.

Arguments:

    DeviceHandle - device handle to add the ref

    Object - object to associate with this ref

    Tag - signature that ids this ref

    Irp -(optional) stored in ref entry

    PendingFlag - if set this ref will block remove

Return Value:

    error if the handle can not be referenced. Caller should not access the handle if it
    cannot be referenced.  (This is used internally when a ref is already held to keep
    from adding refs to a devhandle that is on its way out).

--*/
{

    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    NTSTATUS nts;
    PDEVH_REF_OBJ refObj;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    if (DeviceHandle == NULL) {
        // if NULL passed in fail the request
        return STATUS_INVALID_PARAMETER;
    }

    KeAcquireSpinLock(&devExt->Fdo.DevHandleListSpin, &irql);

    ASSERT_DEVICE_HANDLE(DeviceHandle);

    DbgTrace((UsbpDebugTrace, "'USBPORT_ReferenceDeviceHandle %p\n", DeviceHandle));

    do {

        USBPORT_ASSERT(DeviceHandle->DhState != Devh_Deleted);

        if (DeviceHandle->DhState == Devh_Removed) {
            nts = STATUS_NO_SUCH_DEVICE;
            break;
        }

        if (PendingFlag) {
            InterlockedIncrement(&DeviceHandle->PendingCount);
        }

#ifndef USB_FORCE_MEMORY_STRESS
        ALLOC_POOL_Z(refObj, NonPagedPoolNx, sizeof(struct _DEVH_REF_OBJ));
#else
        refObj = NULL;
#endif
        if (refObj) {
            refObj->DeviceHandle = DeviceHandle;
            refObj->Sig = SIG_DEVICE_HANDLE_REF;
            refObj->Object = Object;
            refObj->Tag = Tag;
            refObj->Irp = Irp;
            refObj->PendingFlag = PendingFlag;
            InsertTailList(&DeviceHandle->RefListHead,  &refObj->RefLink);
        } else {
            // Allocation fails use the resource-less ref count
            DeviceHandle->RefCountNoRes++;
        }

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'drf-', DeviceHandle, Object, Tag);
        LOGENTRY(FdoDeviceObject, LOG_MISC, 'drf1', Irp, refObj, PendingFlag);

        nts = STATUS_SUCCESS;

    } WHILE (0);

    KeReleaseSpinLock(&devExt->Fdo.DevHandleListSpin, irql);

    return nts;
}


NTSTATUS
USBPORT_ReferenceAllDeviceHandles(
    PDEVICE_OBJECT  FdoDeviceObject,
    PVOID           Object,
    PIRP            Irp,
    ULONG           Tag,
    BOOLEAN         PendingFlag,
    PLIST_ENTRY     ListHead
    )
/*++

Routine Description:

    This routine take a reference on each internal Device Handle object
    for a host controller so that the Device Handle objects do not get
    deleted while the references are outstanding.

Arguments:

    FdoDeviceObject - Host controller FDO for which all Device Handle
    objects should be referenced.

    Object - Object to associate with these references.  This object is
    token which must be used to release the reference for each Device
    Handle which is referenenced.

    Tag - Signature that IDs these references.

    Irp - (optional) Irp to associate with these references.

    PendingFlag - If set these references will block the removal of each
    Device Handle object until the corresponding reference for that
    Device Handle object is released.

    ListHead - The list of the DEVH_REF_OBJ_EX references is returned in
    this caller supplied list head.

Return Value:

    Error if the Device Handle objects cannot all be referenced.
    ListHead is not necessarily empty in that case.

--*/
{

    PDEVICE_EXTENSION   devExt;
    KIRQL               irql;
    PLIST_ENTRY         listEntry;
    PUSBD_DEVICE_HANDLE deviceHandle;
    PDEVH_REF_OBJ_EX    refObjEx;
    NTSTATUS            nts;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    InitializeListHead(ListHead);

    // Acquire the host controller Device Handle list spinlock while
    // walking the list of Device Handles.
    //
    KeAcquireSpinLock(&devExt->Fdo.DevHandleListSpin, &irql);

    listEntry = devExt->Fdo.DeviceHandleList.Flink;

    nts = STATUS_SUCCESS;

    while (listEntry != &devExt->Fdo.DeviceHandleList) {

        deviceHandle = (PUSBD_DEVICE_HANDLE) CONTAINING_RECORD(
            listEntry,
            struct _USBD_DEVICE_HANDLE,
            DevhLink);

        // Allocate a DEVH_REF_OBJ_EX to contain the reference
        //
        ALLOC_POOL_Z(refObjEx,
                     NonPagedPoolNx,
                     sizeof(struct _DEVH_REF_OBJ_EX));

        if (!refObjEx) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        refObjEx->DeviceHandle = deviceHandle;
        refObjEx->Sig = SIG_DEVICE_HANDLE_REF;
        refObjEx->Object = Object;
        refObjEx->Tag = Tag;
        refObjEx->Irp = Irp;
        refObjEx->PendingFlag = PendingFlag;

        if (PendingFlag) {
            InterlockedIncrement(&deviceHandle->PendingCount);
        }

        // Link the reference onto the Device Handle reference list.
        //
        InsertTailList(&deviceHandle->RefListHead,  &refObjEx->RefLink);

        // Link the reference onto the callers list.
        //
        InsertTailList(ListHead, &refObjEx->ExRefLink);

        listEntry = deviceHandle->DevhLink.Flink;
    }

    KeReleaseSpinLock(&devExt->Fdo.DevHandleListSpin, irql);

    return nts;
}


NTSTATUS
USBPORT_iReferenceDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PVOID Object,
    ULONG Tag
    )
/*++

Routine Description:

    Refernces devices handles for non-urb functions. These refs will NOT block
    a remove.

Arguments:

Return Value:

    see USBPORT_NeoReferenceDeviceHandle
--*/
{

    return USBPORT_NeoReferenceDeviceHandle(FdoDeviceObject,
                                            DeviceHandle,
                                            Object,
                                            NULL,
                                            Tag,
                                            FALSE);

}


NTSTATUS
USBPORT_rReferenceDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PVOID Object,
    PIRP Irp,
    ULONG Tag
    )
/*++

Routine Description:

    Refernces devices handles for non-urb functions. These refs will block
    a remove.

Arguments:

Return Value:

    see USBPORT_NeoReferenceDeviceHandle
--*/
{

    USBPORT_ASSERT(Tag != DEVH_URB_TAG);
    return USBPORT_NeoReferenceDeviceHandle(FdoDeviceObject,
                                            DeviceHandle,
                                            Object,
                                            NULL,
                                            Tag,
                                            TRUE);
}



VOID
USBPORT_DereferenceDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PVOID Object,
    ULONG Tag,
    BOOLEAN PendingFlag
    )
/*++

Routine Description:

    Releases reference on a device handle

Arguments:

Return Value:

   none

--*/
{
    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    BOOLEAN freeIt = FALSE;
    PDEVH_REF_OBJ refObj = NULL;
    BOOLEAN refObjFound = FALSE;
    PLIST_ENTRY le;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.DevHandleListSpin, &irql);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'drf+', DeviceHandle, Object, Tag);
    ASSERT_DEVICE_HANDLE(DeviceHandle);

    le = DeviceHandle->RefListHead.Flink;

    // locate the ref and remove it
    while (le != &DeviceHandle->RefListHead) {

        PDEVH_REF_OBJ nxtObj;

        nxtObj = (PDEVH_REF_OBJ) CONTAINING_RECORD(
                    le,
                    struct _DEVH_REF_OBJ,
                    RefLink);

        le = nxtObj->RefLink.Flink;

        if (nxtObj->Object == Object &&
            nxtObj->Tag == Tag) {
            refObj = nxtObj;
            refObjFound = TRUE;
            break;
        }
    }

    if (refObj != NULL) {
        RemoveEntryList(&refObj->RefLink);
        USBPORT_ASSERT(refObj->PendingFlag == PendingFlag);
        PendingFlag = refObj->PendingFlag;
        UsbFreePool(refObj);
        refObj = NULL;
    } else {
        // This is a resource-less deref
        USBPORT_ASSERT(DeviceHandle->RefCountNoRes);
        if (DeviceHandle->RefCountNoRes) {
            DeviceHandle->RefCountNoRes--;
        }
    }

    if (PendingFlag) {
        if (InterlockedDecrement(&DeviceHandle->PendingCount) < 0) {
            USBPORT_ASSERT(DeviceHandle->PendingCount == -1);
            KeSetEvent(&DeviceHandle->RemoveDevhEvent, IO_NO_INCREMENT, FALSE);
        }
    }

    if (IsListEmpty(&DeviceHandle->RefListHead) &&
        !DeviceHandle->RefCountNoRes) {

        freeIt = TRUE;
        USBPORT_SetDeviceHandleState(FdoDeviceObject, DeviceHandle,
               Devh_Deleted, Ev_Delete);

        USBPORT_ASSERT(DeviceHandle->Sig == SIG_DEVICE_HANDLE);

        // remove from DeviceHandleDeletedList
        RemoveEntryList(&DeviceHandle->DevhLink);
        InitializeListHead(&DeviceHandle->DevhLink);

        UNSIG(DeviceHandle);
        DeviceHandle->DhState = Devh_Deleted;
    }

    KeReleaseSpinLock(&devExt->Fdo.DevHandleListSpin, irql);

    if (freeIt && refObjFound) {
        //
        // Prefix fix:  DeviceHandle is only freed when RefListHead is
        // empty.  If this routine is called again after that then
        // RefListHead should still be empty and refObj cannot be
        // found.  Although at that point accessing DeviceHandle would
        // be touching freed memory which is just as bad as freeing it
        // again, but ASSERT_DEVICE_HANDLE(DeviceHandle) should
        // bugcheck in that case.
        //

        if (DeviceHandle->PhysicalDeviceObjectName.Buffer != NULL) {
            UsbFreePool(DeviceHandle->PhysicalDeviceObjectName.Buffer);
            DeviceHandle->PhysicalDeviceObjectName.Buffer = NULL;
        }

        USBPORT_ASSERT(IsListEmpty(&DeviceHandle->TtListHead));

        UsbFreePool(DeviceHandle);
    }
}


NTSTATUS
USBPORT_GetRootHubDeviceHandle(
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    Return the root hub device handle with a refernce

Arguments:

Return Value:

   status_success if we can set the handle.


--*/
{

    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION rhDevExt;
    PUSBD_DEVICE_HANDLE *devH;
    PVOID object;
    NTSTATUS nts;

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    DbgTrace((UsbpDebugTrace, "'USBPORT_GetRootHubDeviceHandle\n"));

    irpStack = IoGetCurrentIrpStackLocation(Irp);

    devH = irpStack->Parameters.Others.Argument1;
    if (devH == NULL) {
        nts = STATUS_INVALID_PARAMETER;

    } else {

        *devH = rhDevExt->Pdo.RootHubDeviceHandle;

        if (irpStack->Parameters.DeviceIoControl.IoControlCode ==
             IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_EX) {

            object = irpStack->Parameters.Others.Argument2;

            // we return with the handle referenced
            nts = USBPORT_rReferenceDeviceHandle(rhDevExt->HcFdoDeviceObject,
                                                *devH,
                                                 object,
                                                 Irp,
                                                 DEVH_RETURN_DH_TAG); //rtDH
        } else {
            nts = STATUS_SUCCESS;
        }

    }

    return nts;
}



USBD_DEVH_STATE
USBPORT_GetDeviceHandleState(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle
    )
/*++

Routine Description:

    returns the current state of the device handle.

Arguments:

Return Value:

    handle state

--*/
{

    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    USBD_DEVH_STATE state;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.DevHandleListSpin, &irql);

    ASSERT_DEVICE_HANDLE(DeviceHandle);

    state = DeviceHandle->DhState;

    KeReleaseSpinLock(&devExt->Fdo.DevHandleListSpin, irql);
    ASSERT_DEVICE_HANDLE(DeviceHandle);

    return state;
}


VOID
USBPORT_SetDeviceHandleState(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    USBD_DEVH_STATE NewState,
    USBD_DEVH_EVENT Event
    )
/*++

Routine Description:

    Sets the state of a device handle and updates the history.

    DevHandleListSpin must be acquired before calling this routine

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION devExt;
    DEVH_STATE_LOG_ENTRY *log;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ASSERT_DEVICE_HANDLE(DeviceHandle);

    DeviceHandle->LogIdx =(DeviceHandle->LogIdx+1) % NUM_DEVH_STATE_LOG_ENTRIES;
    log = &DeviceHandle->Log[DeviceHandle->LogIdx];

    log->Event = Event;
    log->CurrentState = DeviceHandle->DhState;
    log->NextState = DeviceHandle->DhState = NewState;

}


VOID
USBPORT_ScanDeviceHandleDeletedList(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

   This routine is called to check the deleted device handle list on Pnp stop and
   remove events for the FDO.

   Device handles on the list have been physically removed but still have outstanding
   references.  These references can block the deletion of the device object and cause
   code 10 pnp errors or bugchecks if the pointer is passed to the HC after unload.
   At this point the structures still contain enough information to id the offending
   function driver.

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    PLIST_ENTRY leNext;
    PUSBD_DEVICE_HANDLE dh;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.DevHandleListSpin, &irql);

    leNext = devExt->Fdo.DeviceHandleDeletedList.Flink;
    while (leNext != &devExt->Fdo.DeviceHandleDeletedList) {

        dh = (PUSBD_DEVICE_HANDLE) CONTAINING_RECORD(
                    leNext,
                    struct _USBD_DEVICE_HANDLE,
                    DevhLink);

        USBPORT_KdPrint((1, "Detected leaked device handle %p\n", dh));

        leNext = dh->DevhLink.Flink;
    }

    KeReleaseSpinLock(&devExt->Fdo.DevHandleListSpin, irql);

}

VOID
USBPORT_ClearTtBufferAndWait(
    PDEVICE_OBJECT HcFdo,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Synchronous version of USBPORT_ClearTtBuffer() that blocks until the
    CLEAR_TT_BUFFER request completes.

--*/
{
    KEVENT command_complete;
    NTSTATUS nts;

    if (Endpoint->EpFlags.ClearTtBufferOnCancel == 0) {
        return;
    }

    KeInitializeEvent(&command_complete,
                      NotificationEvent,
                      USBD_EVENT_STATE_NOT_SIGNALED);

    nts = USBPORT_ClearTtBuffer(HcFdo, DeviceHandle, Endpoint, &command_complete);

    if (nts == STATUS_PENDING) {
        LOGENTRY(HcFdo, LOG_MISC, 'ttW1', 0, 0, 0);

        // wait for completion
        KeWaitForSingleObject(&command_complete,
                              Suspended,
                              KernelMode,
                              FALSE,
                              NULL);

        LOGENTRY(HcFdo,  LOG_MISC, 'ttW2', 0, 0, 0);
    }
}


VOID
USBPORT_ClearTtCommandCallback(
    PDEVICE_OBJECT HcFdo,
    PTRANSFER_URB Urb,
    PVOID UrbCallbackContext,
    LONG NtStatus
    )
{
    PUSBPORT_URB_CB_CONTEXT ctx;
    PHCD_ENDPOINT           endpoint;
    PKEVENT                 commandCompleteEvent;
    KIRQL                   irql;

    ctx = (PUSBPORT_URB_CB_CONTEXT)UrbCallbackContext;

    endpoint = ctx->Endpoint;
    commandCompleteEvent = ctx->CommandCompleteEvent;

    // Release the references taken by USBPORT_ClearTtBuffer().
    //
    USBPORT_DereferenceTt(HcFdo, ctx->Tt, ctx->DeviceHandle, TT_CLEAR_TT_BUFFER_TAG);

    USBPORT_DereferenceDeviceHandle(HcFdo,
                                    ctx->HsHubDeviceHandle,
                                    endpoint,
                                    DEVH_HSHUB_CLEAR_TT_BUFFER_TAG,
                                    FALSE);

    USBPORT_DereferenceDeviceHandle(HcFdo,
                                    ctx->DeviceHandle,
                                    endpoint,
                                    DEVH_CLEAR_TT_BUFFER_TAG,
                                    FALSE);

    UsbFreePool(ctx);
    ctx = NULL;


    // Allow the final completion of transfer requests on this endpoint
    // that were blocked from completing until the CLEAR_TT_BUFFER
    // request completed.  Also allow the endpoint to transition back
    // to the active state, as appropriate.
    //
    irql = USBPORT_TxAcquireLock(HcFdo, NULL);

    USBPORT_Core_iClearTTBufferComplete(HcFdo, endpoint, TRUE);

    USBPORT_TxReleaseLock(HcFdo, NULL, irql);


    // Signal the optional command completion event used by
    // USBPORT_ClearTtBufferAndWait().
    //
    if (commandCompleteEvent != NULL) {
        KeSetEvent(commandCompleteEvent, IO_NO_INCREMENT, FALSE);
    }
}


NTSTATUS
USBPORT_ClearTtBuffer(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PHCD_ENDPOINT Endpoint,
    PKEVENT CommandCompleteEvent
    )
/*++

Routine Description:

    Transmit a clear tt buffer command to the hs hub associated with this device.

Arguments:

    DeviceHandle - ptr to device data structure created by class driver
                from a call to USBPORT_CreateDevice.

                NOTE: This is the handle for the device and not the hub

    Endpoint - LS/FS endpoint for the device associated with a TT.

    CommandCompleteEvent - optional ptr to an event signaled when the command completes

Return Value:

    STATUS_PENDING - If the command will complete asynchronously and
    the CommandCompleteEvent will be signaled on completion.  Else the
    CommandCompleteEvent will not be signaled.

--*/
{
    PUSBPORT_URB_CB_CONTEXT         ctx;
    PUSBD_DEVICE_HANDLE             hsHubDeviceHandle;
    PTRANSACTION_TRANSLATOR         tt;
    USB_HUB_CLEAR_TT_BUFFER_VALUE   v;
    UCHAR                           bRequest;
    BOOLEAN                         deviceReferenced;
    BOOLEAN                         hsHubReferenced;
    BOOLEAN                         ttReferenced;
    NTSTATUS                        nts;

    TRY {

        ctx = NULL;
        hsHubDeviceHandle = NULL;
        tt = NULL;
        deviceReferenced = FALSE;
        hsHubReferenced = FALSE;
        ttReferenced = FALSE;

        ASSERT_ENDPOINT(Endpoint);

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'ttc1', DeviceHandle, Endpoint, Endpoint->Parameters.TransferType);

        // Allocate a USBPORT_SendCommandEx() command completion
        // context.
        //
        ALLOC_POOL_Z(ctx, NonPagedPoolNx, sizeof(struct _USBPORT_URB_CB_CONTEXT));

        if (ctx == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        // Reference the device handle.
        //
        nts = USBPORT_RefValidDeviceHandle(FdoDeviceObject,
                                           DeviceHandle,
                                           Endpoint,  // obj
                                           NULL, // no irp
                                           DEVH_CLEAR_TT_BUFFER_TAG);

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'ttc2', DeviceHandle, Endpoint, nts);

        if (NT_ERROR(nts)) {
            LEAVE;
        } else {
            deviceReferenced = TRUE;
        }

        // Reference the high speed hub that we will receive the
        // CLEAR_TT_BUFFER request.
        //
        hsHubDeviceHandle = DeviceHandle->HsHubDeviceHandle;

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'ttc3', DeviceHandle, Endpoint, hsHubDeviceHandle);

        if (hsHubDeviceHandle == NULL) {
            LEAVE;
        }

        nts = USBPORT_RefValidDeviceHandle(FdoDeviceObject,
                                           hsHubDeviceHandle,
                                           Endpoint,
                                           NULL, //no irp
                                           DEVH_HSHUB_CLEAR_TT_BUFFER_TAG);

        if (NT_ERROR(nts)) {
            LEAVE;
        } else {
            hsHubReferenced = TRUE;
        }

        // Reference the TT.
        //
        tt = Endpoint->Tt;
        ASSERT_TT(tt);

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'ttc4', DeviceHandle, Endpoint, tt);

        nts = USBPORT_ReferenceTt(FdoDeviceObject,
                                  tt,
                                  DeviceHandle,
                                  TT_CLEAR_TT_BUFFER_TAG);

        if (NT_ERROR(nts)) {
            LEAVE;
        } else {
            ttReferenced = TRUE;
        }

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'ttc5', DeviceHandle, Endpoint, tt);

        // hs hub, tt and device all referenced

        // setup packet for clear tt buffer
        // 11.24.2.3

        // this parameter describes the endpoint
        v.us = 0;
        switch (Endpoint->Parameters.TransferType) {
        case Control:
            v.EndpointType = USB_ENDPOINT_TYPE_CONTROL;
            break;
        case Bulk:
            v.EndpointType = USB_ENDPOINT_TYPE_BULK;
            break;
        default:
            // we checked this earlier
            USBPORT_ASSERTMSG("Unexpected Endpoint->Parameters.TransferType",FALSE);
        }

        v.EndpointNumber = Endpoint->Parameters.EndpointAddress;
        v.DeviceAddress = Endpoint->Parameters.DeviceAddress;

        if (Endpoint->Parameters.TransferDirection == In) {
            v.Direction_1forIN = 1;
        }

        bRequest = USB_REQUEST_CLEAR_TT_BUFFER;

        // NOTE: In general issuing a RESET_TT request may do more harm
        // that good as it may upset the state of all split transactions
        // currently in progress for full and low speed devices
        // downstream of the target TT.  In contrast the CLEAR_TT_BUFFER
        // request should only effect the specified endpoint on the
        // specified device.
        //
        // Ignore the TTFLAGS_RESET_TT_ON_CANCEL flag and always issue
        // the CLEAR_TT_BUFFER request instead.
        //
        // If there are demonstrated issues with specific TT hubs and the
        // CLEAR_TT_BUFFER request the TTFLAGS_NO_CLEAR_TT_BUFFER_ON_CANCEL
        // flag (USBPORT_DEVICEFLAG_HSHUB_NO_CLEAR_TT_BUFFER_ON_CANCEL)
        // should be set instead of the TTFLAGS_RESET_TT_ON_CANCEL
        // (USBPORT_DEVICEFLAG_HSHUB_RESET_TT_ON_CANCEL) flag.
        //
        // if (TEST_FLAG(tt->TtFlags, TTFLAGS_RESET_TT_ON_CANCEL)) {
        //     bRequest = USB_REQUEST_RESET_TT;
        //     v.us = 0;   // The wValue value is zero for the RESET_TT request
        // }

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'ttc6', Endpoint, v.us, tt->TT_Port);

        USBPORT_INIT_SETUP_PACKET_EX(ctx->SetupPacket,
                                     bRequest,
                                     0x23, //00100011B bmRequest
                                     v.us, // wValue
                                     // port number of the tt
                                     tt->TT_Port, // wIndex
                                     0); // wLength

        ctx->Sig = SIG_URB_CB;
        ctx->HsHubDeviceHandle = hsHubDeviceHandle;
        ctx->DeviceHandle = DeviceHandle;
        ctx->Tt = tt;
        ctx->Endpoint = Endpoint;
        ctx->CommandCompleteEvent = CommandCompleteEvent;

        nts = USBPORT_SendCommandEx(hsHubDeviceHandle,
                                    FdoDeviceObject,
                                    &ctx->SetupPacket,
                                    NULL,
                                    0,
                                    USBPORT_ClearTtCommandCallback,
                                    ctx,
                                    NULL);

        if (nts != STATUS_PENDING) {
            LEAVE;
        }

    } FINALLY {

        if (nts != STATUS_PENDING) {

            if ((tt != NULL) && ttReferenced) {
                USBPORT_DereferenceTt(FdoDeviceObject,
                                      tt,
                                      DeviceHandle,
                                      TT_CLEAR_TT_BUFFER_TAG);
            }

            if ((hsHubDeviceHandle != NULL) && hsHubReferenced) {
                USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                                hsHubDeviceHandle,
                                                Endpoint,
                                                DEVH_HSHUB_CLEAR_TT_BUFFER_TAG,
                                                FALSE);
            }

            if (deviceReferenced) {
                USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                                DeviceHandle,
                                                Endpoint,
                                                DEVH_CLEAR_TT_BUFFER_TAG,
                                                FALSE);
            }

            if (ctx != NULL) {
                UsbFreePool(ctx);
                ctx = NULL;
            }
        }
    }

    return nts;
}


NTSTATUS
USBPORT_RefValidDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PVOID Object,
    PIRP Irp,
    ULONG Tag
    )
/*++

Routine Description:

    Used by the routines that queue an asynchronous reset_TT for devices on a USB 2.0 hub. This routine will reference
    the device handle (device or hub) as long as the device handle is valid.

Arguments:

    DeviceHandle - device handle to validate

    Urb - the urb to link in

    Irp - the irp associated with this urb

Return Value:

    nts = STATUS_NO_SUCH_DEVICE if the devicehandle is not in a state where we can transmit commands (removed).

--*/
{
    NTSTATUS nts = STATUS_NO_SUCH_DEVICE;
    BOOLEAN ref_success;

    // this ref will NOT block a remove and will fail if the device is in the process of being deleted.
    ref_success = USBPORT_NeoValidDeviceHandle(FdoDeviceObject, DeviceHandle, Devh_Valid,
        Object, Irp, Tag, FALSE);

    if (ref_success) {
        nts = STATUS_SUCCESS;
    }


    return nts;
}


NTSTATUS
USBPORT_AllocateForwardProgressContext(
    __in PDEVICE_OBJECT HcFdo,
    __in PUSB_FORWARD_PROGRESS_INFO FpInfo
    )
/*++

Routine Description:

    Allocate forward progress support structure

Arguments:

    HcFdo - Host controller Fdo

    FpInfo - forward progress info

Return Value:

    STATUS_SUCCESS
    STATUS_INSUFFICIENT_RESOURCES
    STATUS_INVALID_DEVICE_REQUEST

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION devExt;
    PUSBD_DEVICE_HANDLE deviceHandle;
    PHCD_ENDPOINT endpoint;
    ULONG i;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    deviceHandle = FpInfo->DeviceHandle;
    ASSERT_DEVICE_HANDLE(deviceHandle);

    // Allocate endpoint FP contexts
    LOCK_DEVICE_CONFIGS(deviceHandle, HcFdo);

    // Verify control pipe setting
    if (FpInfo->ControlPipeMaxForwardProgressTransferSize > USB_MAX_FP_XFER_SIZE) {
        ntStatus = STATUS_INVALID_DEVICE_REQUEST;
        goto USBPORT_AllocateForwardProgressContext_Done;
    }

    if (FpInfo->ControlPipeMaxForwardProgressTransferSize &&
        USBPORT_EndpointFromHandle(deviceHandle->DefaultPipe.EpHandle)->FpContext) {
        // Reprovisioning the control pipe is not supported.
        // Full FP support for multifunction devices is not planned for Win8.
        ntStatus = STATUS_INVALID_DEVICE_REQUEST;
        goto USBPORT_AllocateForwardProgressContext_Done;
    }

    // Verify non-control pipes settings
    for (i=0; i<FpInfo->NumberOfPipes; i++) {

        // Is pipe handle valid?
        if (!USBPORT_ValidatePipeHandle(deviceHandle, FpInfo->Pipes[i].PipeHandle)) {
            ntStatus = STATUS_INVALID_DEVICE_REQUEST;
            goto USBPORT_AllocateForwardProgressContext_Done;
        }

        // Check max FP transfer size
        if (FpInfo->Pipes[i].MaxForwardProgressTransferSize == 0 ||
            FpInfo->Pipes[i].MaxForwardProgressTransferSize > USB_MAX_FP_XFER_SIZE) {
            ntStatus = STATUS_INVALID_DEVICE_REQUEST;
            goto USBPORT_AllocateForwardProgressContext_Done;
        }

        // Has pipe handle been provisioned for FP already?
        endpoint = USBPORT_EndpointFromHandle(
                       ((PUSBD_PIPE_HANDLE_I)FpInfo->Pipes[i].PipeHandle)->EpHandle);
        if (endpoint->FpContext) {
            ntStatus = STATUS_INVALID_DEVICE_REQUEST;
            goto USBPORT_AllocateForwardProgressContext_Done;
        }
    }

    // Allocate FP context for control pipe
    if (FpInfo->ControlPipeMaxForwardProgressTransferSize) {
        ntStatus = USBPORT_AllocateEndpointForwardProgressContext(
                       HcFdo,
                       USBPORT_EndpointFromHandle(deviceHandle->DefaultPipe.EpHandle),
                       FpInfo->ControlPipeMaxForwardProgressTransferSize);
        if (!NT_SUCCESS(ntStatus)) {
            goto USBPORT_AllocateForwardProgressContext_Cleanup;
        }
    }

    // Allocate FP context for non-control pipes
    for (i=0; i < FpInfo->NumberOfPipes; i++) {
        endpoint = USBPORT_EndpointFromHandle(
                       ((PUSBD_PIPE_HANDLE_I)FpInfo->Pipes[i].PipeHandle)->EpHandle);
        ntStatus = USBPORT_AllocateEndpointForwardProgressContext(
                       HcFdo,
                       endpoint,
                       FpInfo->Pipes[i].MaxForwardProgressTransferSize);
        if (!NT_SUCCESS(ntStatus)) {
            goto USBPORT_AllocateForwardProgressContext_Cleanup;
        }
    }

USBPORT_AllocateForwardProgressContext_Cleanup:

    if (!NT_SUCCESS(ntStatus)) {
        if (FpInfo->ControlPipeMaxForwardProgressTransferSize) {
            USBPORT_FreeEndpointForwardProgressContext(
                USBPORT_EndpointFromHandle(deviceHandle->DefaultPipe.EpHandle));
        }

        for (i=0; i < FpInfo->NumberOfPipes; i++) {
            endpoint = USBPORT_EndpointFromHandle(
                           ((PUSBD_PIPE_HANDLE_I)FpInfo->Pipes[i].PipeHandle)->EpHandle);
            USBPORT_FreeEndpointForwardProgressContext(endpoint);
        }
    }

USBPORT_AllocateForwardProgressContext_Done:

    UNLOCK_DEVICE_CONFIGS(deviceHandle, HcFdo);

    return ntStatus;
}

