/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    dma.c

Abstract:

    special functions for processing enpoints that use DMA to process transfers or otherwise need hardware synchronization.

Environment:

    kernel mode only

Notes:

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs
    07-14-05 : modified for SFIO/perf

--*/

#include "common.h"

VOID
USBPORT_Core_RevertIsoTegraDoubleBuffers(
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __in    PHCD_ENDPOINT         Endpoint
    )
{
    PUSBPORT_DB_HANDLE curr, next;

    UNREFERENCED_PARAMETER(Endpoint);

    FOR_ALL_IN_LIST_SAFE(USBPORT_DB_HANDLE,
                         &Transfer->IsoTegraDoubleBufferListHead,
                         DbLink,
                         curr,
                         next) {
        if (Endpoint->Parameters.TransferDirection == In) {
            if (Transfer->MiniportBytesTransferred > 0) {
                RtlCopyMemory(curr->DbSystemAddress,
                              curr->DbData,
                              curr->DbLength);
            }
        }
        RemoveEntryList(&curr->DbLink);

        USBPORT_FreeDbHandleForIsoOnTegra(Endpoint, curr);

    }
}

VOID
USBPORTSVC_NotifyDoubleBuffer(
    PDEVICE_DATA DeviceData,
    PTRANSFER_PARAMETERS TransferParameters,
    PVOID DbSystemAddress,
    ULONG DbLength
    )

/*++

Routine Description:

    Notify the port driver that double buffering has occured,
    port driver will create a node for use during a subsequent
    adapter flush.

Arguments:

Return Value:

    none

--*/

{
    PDEVICE_EXTENSION devExt;
    PHCD_TRANSFER_CONTEXT transfer;
    PDEVICE_OBJECT fdoDeviceObject;
    PUSBPORT_DB_HANDLE dbHandle;
    ULONG cbDbList = 0;
    BOOLEAN write;
    NTSTATUS nts;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);

    fdoDeviceObject = devExt->HcFdoDeviceObject;

    LOGENTRY(fdoDeviceObject, LOG_TRANSFER, 'NOdb', 0,
        0, TransferParameters);

    TRANSFER_FROM_TPARAMETERS(transfer, TransferParameters);
    ASSERT_TRANSFER(transfer);

    NT_ASSERT(!TEST_FLAG(devExt->Fdo.HcOptionFlags, USB_MINIPORT_OPT_NO_DOUBLE_BUFFER));

    write = transfer->Direction == WriteData ? TRUE : FALSE;

    // allocate a node and add it to the list, we don't care if it is a
    // write

    if (!write) {
        PUCHAR pch = NULL;

        //cbDbList = sizeof(USBPORT_DB_HANDLE) + DbLength;
        nts = RtlULongAdd(sizeof(USBPORT_DB_HANDLE), DbLength, &cbDbList);

        if (NT_SUCCESS(nts)) {
            nts = UsbAllocPool_Z(&pch, NonPagedPoolNx, cbDbList);
            LOGENTRY(fdoDeviceObject, LOG_TRANSFER, 'db+A', nts, 0, 0);
        }

        LOGENTRY(fdoDeviceObject, LOG_TRANSFER, 'db++', DbSystemAddress, DbLength, transfer);


        if (NT_SUCCESS(nts) && pch) {
            dbHandle = (PUSBPORT_DB_HANDLE) pch;
            pch += sizeof(USBPORT_DB_HANDLE);
            dbHandle->Sig = SIG_DB;
            dbHandle->DbSystemAddress = DbSystemAddress;
            dbHandle->DbLength = DbLength;
            dbHandle->DbData = pch;

            RtlCopyMemory(pch,
                          DbSystemAddress,
                          DbLength);

            if (transfer->TxType == Tx_SPLIT_CHILD) {
                ASSERT_TRANSFER(transfer->ParentTransfer);
                InsertTailList(&transfer->ParentTransfer->DoubleBufferListHead,
                               &dbHandle->DbLink);
            } else {
                InsertTailList(&transfer->DoubleBufferListHead,
                               &dbHandle->DbLink);
            }
        }
    }
}


VOID
USBPORT_FlushAdapterDBs(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_TRANSFER_CONTEXT Transfer
    )

/*++

Routine Description:

Arguments:

Return Value:

    none

--*/

{
    PDEVICE_EXTENSION devExt;
    PLIST_ENTRY listEntry;
    PUSBPORT_DB_HANDLE dbHandle;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ASSERT_TRANSFER(Transfer);

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'flDB', Transfer,
            0, 0);
// dump the 4 dwords of the transfer buffer
//{
//    PULONG p;
//
//    p = (PULONG) Transfer->SgList.MdlVirtualAddress;
//    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'dmp1', *(p),
//            *(p+1), *(p+2));
//}

    while (!IsListEmpty(&Transfer->DoubleBufferListHead)) {

        listEntry = RemoveHeadList(&Transfer->DoubleBufferListHead);

        LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'flle', Transfer,
            listEntry, 0);

        dbHandle = (PUSBPORT_DB_HANDLE) CONTAINING_RECORD(
                   listEntry,
                   struct _USBPORT_DB_HANDLE,
                   DbLink);

        LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'DBHf', Transfer,
            dbHandle, 0);
        ASSERT_DB_HANDLE(dbHandle);

        // flush to the system address
        RtlCopyMemory(dbHandle->DbSystemAddress,
                      dbHandle->DbData,
                      dbHandle->DbLength);

        UsbFreePool(dbHandle);

    }
}
