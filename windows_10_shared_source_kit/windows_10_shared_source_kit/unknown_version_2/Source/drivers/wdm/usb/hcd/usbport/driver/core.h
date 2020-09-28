//
//    Copyright (C) Microsoft.  All rights reserved.
//

//
// Since the DMA Tegra Fix is a new and risky change, keeping this DMA_KdPrint around.
// This can be removed in future.
//
extern BOOLEAN g_DMA_KdPrint;
#define DMA_KdPrint(_x_) {if (g_DMA_KdPrint) { KdPrint (_x_); }}

VOID
USBPORT_AssertListEntry(PLIST_ENTRY Le);

VOID
USBPORT_AssertListEntryLog(PDEVICE_OBJECT HcFdo, PLIST_ENTRY Le);

BOOLEAN
USBPORT_IsLinked(PLIST_ENTRY Le);

#define USB_ASSERT_UNLINKED(x) USBPORT_ASSERT(USBPORT_IsLinked((x)) == FALSE);

#if DBG

BOOLEAN
USBPORT_IsListEntryOnListHead(
    PLIST_ENTRY ListHead,
    PLIST_ENTRY ListEntry
    );

#define USBPORT_DBG_ASSERT_ISONLIST(head, entry) USBPORT_ASSERT(USBPORT_IsListEntryOnListHead((head),(entry)))

#else

#define USBPORT_DBG_ASSERT_ISONLIST(head, entry)

#endif

VOID
USB_ZAP_LINK(PLIST_ENTRY Le);

PDEVICE_EXTENSION
GetHcFdoExt(
    __inout PDEVICE_OBJECT DevObj
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_UsbIocDpc_Worker(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT UsbDoneDpc,
    __drv_restoresIRQL __in KIRQL Irql
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_iClearTTBufferComplete(
    __inout PDEVICE_OBJECT  HcFdo,
    __inout PHCD_ENDPOINT   Endpoint,
    __in    BOOLEAN         SignalIocDpc
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_UsbDoneDpc_Worker(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT UsbDoneDpc,
    __drv_restoresIRQL __in KIRQL Irql
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_UsbMapDpc_Worker(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT UsbMapDpc,
    __drv_restoresIRQL __in KIRQL Irql
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_UsbHcIntDpc_Worker(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT UsbHcIntDpc,
    __drv_restoresIRQL __in KIRQL Irql
    );

PHCD_TRANSFER_CONTEXT
USBPORT_Core_UnlinkTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PTRANSFER_URB Urb,
    __inout PHCD_TRANSFER_CONTEXT CompletedTransfer
    );

PHCD_TRANSFER_CONTEXT
USBPORT_Core_CheckTransferQueues(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT FindTransfer
    );

VOID
USBPORT_Core_QueueTransferUrb(
    __inout PTRANSFER_URB Urb
    );

VOID
USBPORT_Core_iInsertPriorityTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    );

VOID
USBPORT_Core_LockInsertPriorityTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    );

VOID
USBPORT_Core_iCompleteTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __in USBD_STATUS UsbdCompleteCode,
    __in TX_EVENT TxEvent,
    __in TX_STATE TxState
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_iIrpCsqCompleteDoneTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __drv_restoresIRQL __in KIRQL Irql
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_iCompleteDoneTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PIRP Irp,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __drv_restoresIRQL __in KIRQL Irql
    );

DRIVER_LIST_CONTROL USBPORT_Core_ProcessScatterGatherList;

ULONG
USBPORT_Core_GetEndpointTransferCount(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
USBPORT_Core_iMapTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT UsbMapDpc,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __inout PHCD_ENDPOINT Endpoint,
    __drv_restoresIRQL __in KIRQL Irql
    );

USB_MINIPORT_STATUS
USBPORT_Core_iSubmitTransferToMiniport(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __in BOOLEAN CompleteOnError
    );

PHCD_TRANSFER_CONTEXT
USBPORT_Core_iFindPriorityTransferIrp(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PIRP Irp
    );

VOID
USBPORT_Core_iCancelPriorityTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    );

PHCD_TRANSFER_CONTEXT
USBPORT_Core_iGetNextPriorityTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PXDPC_CONTEXT Xdpc
    );

VOID
USBPORTSVC_InvalidateEndpoint(
    PDEVICE_DATA DeviceData,
    PENDPOINT_DATA EndpointData
    );

VOID
USBPORTSVC_CompleteTransfer(
    PDEVICE_DATA DeviceData,
    PDEVICE_DATA EndpointData,
    PTRANSFER_PARAMETERS TransferParameters,
    USBD_STATUS UsbdStatus,
    ULONG BytesTransferred
    );

VOID
USBPORT_Core_iEndpointPaused(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_Core_iEndpointActive(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_Core_iRemovePriorityTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    );

LONG
USBPORT_Core_IncIoRequestCount(
    __inout PDEVICE_OBJECT DevObj,
    __inout PUSB_IOREQUEST_CONTEXT UsbIoContext,
    __inout PVOID Object,
    __in ULONG Tag,
    __in BOOLEAN FreeContext
    );

LONG
USBPORT_Core_DecIoRequestCount(
    __inout PDEVICE_OBJECT DevObj,
    __inout PUSB_IOREQUEST_CONTEXT UsbIoContext,
    __in BOOLEAN InternalRequest
    );

NTSTATUS
USBPORT_Core_InitIoRequestCount(
    __inout PDEVICE_OBJECT DevObj
    );

FORCEINLINE
USBPORT_Core_RevertDoubleBuffer(
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __in    PHCD_ENDPOINT         Endpoint
    )
{
    NT_ASSERT(Transfer->DoubleBufferDueToTegraDmaBug);

    if ((Endpoint->Parameters.TransferDirection == In) ||
        (Endpoint->Parameters.TransferType == Control)) {

        if (Transfer->MiniportBytesTransferred > 0) {

            RtlCopyMemory(Transfer->OrigBuffer,
                          Transfer->DoubleBuffer,
                          Transfer->MiniportBytesTransferred);
        }
    }

    IoFreeMdl(Transfer->DoubleBufferMdl);
    ExFreePool(Transfer->DoubleBuffer);
    Transfer->TransferBufferMdl = Transfer->OrigBufferMdl;

    Transfer->DoubleBufferMdl = NULL;
    Transfer->OrigBuffer = NULL;
    Transfer->OrigBufferMdl = NULL;
    Transfer->DoubleBufferDueToTegraDmaBug = FALSE;
}

typedef struct _USBPORT_DB_HANDLE {
    ULONG Sig;
    LIST_ENTRY DbLink;
    PVOID DbSystemAddress;
    ULONG DbLength;
    ULONG DbLengthMax;
    PUCHAR DbData;
    BOOLEAN IsPhysicallyContiguous;
    PMDL DbMdl;
} USBPORT_DB_HANDLE, *PUSBPORT_DB_HANDLE;

FORCEINLINE
ULONG
USBPORT_NumberOfIsoPacketsPerTd(
    PHCD_ENDPOINT         Endpoint
    )
{
    ULONG numPacketsPerTd;

    if (Endpoint->Parameters.DeviceSpeed != HighSpeed) {
        //
        // For Full Speed Devices, only 1 Isoch Packet can be there per Td
        //
        numPacketsPerTd = 1;
    } else {
        //
        // For High Speed Device upto 8 packets can be there per
        // Td.
        // Period = 1, Number of Isoch Packets per iTd = 8
        // Period = 2, Number of Isoch Packets per iTd = 4
        // Period = 4, Number of Isoch Packets per iTd = 2
        // Period >= 8, Number of Isoch Packets per iTd = 1
        //
        numPacketsPerTd = 1 + (7/Endpoint->Parameters.Period);
    }
    return numPacketsPerTd;
}

FORCEINLINE
VOID
USBPORT_FreeDbHandleForIsoOnTegra(
    PHCD_ENDPOINT         Endpoint,
    PUSBPORT_DB_HANDLE    DbHandle)
{
    KIRQL irql;
    KeAcquireSpinLock(&Endpoint->TegraExtension->IsoDbHandleListLock, &irql);
    if (DbHandle->IsPhysicallyContiguous) {
        InsertTailList(&Endpoint->TegraExtension->PhysicallyContiguousIsoDbHandlesForTegra, &DbHandle->DbLink);
    } else {
        InsertTailList(&Endpoint->TegraExtension->IsoDbHandlesForTegra, &DbHandle->DbLink);
    }
    KeReleaseSpinLock(&Endpoint->TegraExtension->IsoDbHandleListLock, irql);
}

FORCEINLINE
VOID
USBPORT_FreeDbHandleForIsoOnTegraFinal(
    PUSBPORT_DB_HANDLE    DbHandle)
{
    if (DbHandle->DbMdl) {
        IoFreeMdl(DbHandle->DbMdl);
    }
    if (DbHandle->DbData) {
        if (DbHandle->IsPhysicallyContiguous) {
            MmFreeContiguousMemory(DbHandle->DbData);
        } else {
            ExFreePool(DbHandle->DbData);
        }
    }
    ExFreePool(DbHandle);
}

FORCEINLINE
PUSBPORT_DB_HANDLE
USBPORT_AllocateDbHandleForIsoOnTegra(
    PHCD_ENDPOINT         Endpoint,
    BOOLEAN               PhysicallyContiguous,
    PUCHAR                CurrentVa,
    ULONG                 Length)
{
    PUSBPORT_DB_HANDLE dbHandle = NULL;
    PMDL doubleIsoBufferMdl;
    PHYSICAL_ADDRESS minPhysicalAddress;
    PHYSICAL_ADDRESS maxPhysicalAddress;
    PHYSICAL_ADDRESS boundaryPhysicalAddress;
    PUCHAR doubleIsoBuffer;
    PLIST_ENTRY listHead;
    KIRQL irql;
    NTSTATUS status = STATUS_SUCCESS;
    ULONG maxDbBuffSize;

    do {

        maxDbBuffSize = Endpoint->Parameters.MaxPacketSize *
            USBPORT_NumberOfIsoPacketsPerTd(Endpoint);

        NT_ASSERT(Length <= maxDbBuffSize);
        if (Length > maxDbBuffSize) {
            // This is a bug in the client driver.
            // It is sending more data in an IsochPacket than that is allowed.
            Length = maxDbBuffSize;
        }

        if (PhysicallyContiguous) {
            listHead = &Endpoint->TegraExtension->PhysicallyContiguousIsoDbHandlesForTegra;
        } else {
            listHead = &Endpoint->TegraExtension->IsoDbHandlesForTegra;
        }

        KeAcquireSpinLock(&Endpoint->TegraExtension->IsoDbHandleListLock, &irql);

        if (!IsListEmpty(listHead)) {
            dbHandle = CONTAINING_RECORD(RemoveHeadList(listHead),
                                         USBPORT_DB_HANDLE,
                                         DbLink);
        }

        KeReleaseSpinLock(&Endpoint->TegraExtension->IsoDbHandleListLock, irql);

        if (dbHandle != NULL) {

            NT_ASSERT(maxDbBuffSize == dbHandle->DbLengthMax);

            dbHandle->DbLength = Length;
            dbHandle->DbSystemAddress = CurrentVa;

            doubleIsoBuffer = dbHandle->DbData;

            //
            // Note: It may seem that RtlCopyMemory is required only for Out Transfers. However
            // it is required for the In Transfers as well due to the following scenario:
            // * Device sent a short packet or some data was lost
            // * During completion of the request, we would copy the conents of the double buffer
            //   to the clients buffer.
            // * For the sections of the transfer buffer for which data was not recieved, we want
            //   to preserve what that there in the clients buffer in the first place.
            //
            RtlCopyMemory(doubleIsoBuffer, CurrentVa, Length);
            break;
        }

        dbHandle = ExAllocatePoolWithTag(NonPagedPoolNx,
                                     sizeof(*dbHandle),
                                     USBPORT_TAG);

        if (dbHandle == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        RtlZeroMemory(dbHandle, sizeof(*dbHandle));
        dbHandle->Sig = SIG_ISO_TEGRA_DB;

        if (PhysicallyContiguous) {
            //
            // Allocate a DoubleBuffer
            //
            NT_ASSERT(maxDbBuffSize > PAGE_SIZE);

            minPhysicalAddress.QuadPart = 0;
            maxPhysicalAddress.QuadPart = 0xFFFFFFFFFFFFFFFF;
            boundaryPhysicalAddress.QuadPart = 0;

            doubleIsoBuffer = MmAllocateContiguousNodeMemory(maxDbBuffSize,
                                                             minPhysicalAddress,
                                                             maxPhysicalAddress,
                                                             boundaryPhysicalAddress,
                                                             PAGE_READWRITE,
                                                             MM_ANY_NODE_OK);

        } else {

            doubleIsoBuffer = ExAllocatePoolWithTag(NonPagedPoolNx,
                                                    maxDbBuffSize,
                                                    USBPORT_TAG);
        }

        if (doubleIsoBuffer == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            NT_ASSERTMSG("Could not allocate Necessary Double Buffer", FALSE);
            break;
        }

        RtlZeroMemory(doubleIsoBuffer, maxDbBuffSize);

        dbHandle->DbData = doubleIsoBuffer;
        dbHandle->DbLength = Length;
        dbHandle->DbLengthMax = maxDbBuffSize;
        dbHandle->DbSystemAddress = CurrentVa;
        dbHandle->IsPhysicallyContiguous = PhysicallyContiguous;

        doubleIsoBufferMdl = IoAllocateMdl(doubleIsoBuffer,
                                           maxDbBuffSize,
                                           FALSE,
                                           FALSE,
                                           NULL);

        if (doubleIsoBufferMdl == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        MmBuildMdlForNonPagedPool(doubleIsoBufferMdl);

        dbHandle->DbMdl = doubleIsoBufferMdl;

        //
        // Note: It may seem that RtlCopyMemory is required only for Out Transfers. However
        // it is required for the In Transfers. <See previous comment in this function for more
        // details>
        //
        RtlCopyMemory(doubleIsoBuffer, CurrentVa, Length);

    } WHILE(0);

    if (!NT_SUCCESS(status)) {
        if (dbHandle != NULL) {
            USBPORT_FreeDbHandleForIsoOnTegraFinal(dbHandle);
            dbHandle = NULL;
        }
    }

    return dbHandle;

}

VOID
USBPORT_Core_RevertIsoTegraDoubleBuffers(
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __in    PHCD_ENDPOINT         Endpoint
    );

USBD_STATUS
USBPORT_Core_AllocTransferEx(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PTRANSFER_URB Urb,
    __inout PUSBD_DEVICE_HANDLE DeviceHandle,
    __inout_opt PIRP Irp,
    __inout_opt PKEVENT CompleteEvent,
    __in ULONG MillisecTimeout,
    __in_opt PURB_CALLBACK_ROUTINE UrbCallbackFunc,
    __in_opt PVOID UrbCallbackContext
    );

USBD_STATUS
USBPORT_Core_AllocTransfer(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PTRANSFER_URB Urb,
    __inout PUSBD_DEVICE_HANDLE DeviceHandle,
    __inout_opt PIRP Irp,
    __inout_opt PKEVENT CompleteEvent,
    __in ULONG MillisecTimeout
    );

VOID
USBPORT_Core_IoCompleteIrp(
    __inout PDEVICE_OBJECT DeviceObject,
    __inout PIRP Irp,
    __inout_opt PUSB_IOREQUEST_CONTEXT UsbIoRequest,
    __in LONG NtStatus,
    __in ULONG_PTR Information
    );

VOID
USBPORT_Core_FreeIoRequest(
    __inout_opt PUSB_IOREQUEST_CONTEXT UsbIoRequest
    );

PUSB_IOREQUEST_CONTEXT
USBPORT_Core_GetIoRequestFromObject(
    __inout PDEVICE_OBJECT DevObj,
    __inout PVOID UsbIoObject
    );

PUSB_IOREQUEST_CONTEXT
USBPORT_Core_AllocIoRequest(
    __inout PDEVICE_OBJECT DevObj,
    __in_opt PVOID Object,
    __inout ULONG Tag
    );

VOID
USBPORT_Core_WaitPendingTransfers(
    __inout PDEVICE_OBJECT HcFdo
    );

VOID
USBPORT_Core_iIncEpHwPendingTransferCount(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    );

VOID
USBPORT_Core_iDecEpHwPendingTransferCount(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    );

ULONG
USBPORT_Core_iGetEpHwPendingTransferCount(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_Core_CompleteAbortRequest(
    __inout PDEVICE_OBJECT HcFdo,
    __in    PUSB_ABORT_PIPE_CONTEXT AbortContext,
    __in    LONG NtStatus
    );

VOID
USBPORT_Core_FlushCompletedAborts(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc
    );

VOID
USBPORT_Core_iCheckAbortList(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PXDPC_CONTEXT Xdpc,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    );

BOOLEAN
USBPORT_Core_iAbortEndpoint(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PUSB_ABORT_PIPE_CONTEXT AbortContext
    );

PUSB_ABORT_PIPE_CONTEXT
USBPORT_Core_AllocateAbortRequest(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PUSBD_DEVICE_HANDLE DevHandle,
    __inout_opt PURB Urb,
    __inout_opt PIRP Irp,
    __inout_opt PKEVENT AbortEvent
    );

LONG
USBPORT_Core_GetIoRequestCount(
    __inout PDEVICE_OBJECT DevObj
    );

VOID
USBPORT_Core_SyncAbortEndpoint(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_Core_DetectActiveUrb(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PIRP Irp,
    __inout PURB Urb
    );

VOID
USBPORT_Core_FlushController(
    __inout PDEVICE_OBJECT HcFdo
    );

VOID
USBPORT_Core_TimeoutAllTransfers(
    PDEVICE_OBJECT HcFdo
    );

VOID
USBPORT_Core_RecordOcaTransferData(
    __inout PDEVICE_OBJECT HcFdo,
    __inout_opt PHCD_TRANSFER_CONTEXT Transfer
    );

VOID
USBPORT_Core_ZapAllEndpoints(
    __inout PDEVICE_OBJECT HcFdo
    );

VOID
USBPORT_Core_ResumeAllEndpoints(
    PDEVICE_OBJECT HcFdo
    );

PHCD_TRANSFER_CONTEXT
USBPORT_Core_iFindIoDoneTransferIrp(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PIRP Irp
    );

BOOLEAN
USBPORT_Core_CheckTimeout(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PUSB_TIMEOUT Timeout
    );

VOID
USBPORT_Core_InitTimeout(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PUSB_TIMEOUT Timeout,
    __in_opt PVOID OwnerObject,
    __in ULONG MillisecondTimeout
    );

VOID
USBPORT_Core_UnlinkAndFreeIoRequest(
    __inout PUSB_IOREQUEST_CONTEXT UsbIoRequest,
    __inout PVOID Object
    );
