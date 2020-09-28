/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    cmnbuf.c

Abstract:

    Code to manage common buffer -- hardware addressable memory

    a common buffer block looks like this

start    ------------ <- address returned from the hal
         padding
         ------------ <- address returned to the miniport
         mp data
         ------------ <- ptr to header
         header
end      ------------

Environment:

    kernel mode only

Notes:

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs

--*/

#include "common.h"

#ifdef USBPORT_WPP
#include "cmnbuf.tmh"
#endif


PUSBPORT_COMMON_BUFFER
USBPORT_HalAllocateCommonBuffer(
    PDEVICE_OBJECT FdoDeviceObject,
    ULONG NumberOfBytes
    )

/*++

Routine Description:

    Allocates common buffer directly from the hal.
    The common buffer is passed to the miniport, we
    always allocate a multiple of PAGE_SIZE so the
    always starts on a page_boundry. This ensures
    proper alignement of the TDs used by the miniport

Arguments:

    DeviceObject - DeviceObject of the controller to stop

Return Value:

    returns Virtual Address of common buffer or NULL if
    unsuccessful.

--*/

{
    ULONG pageCount = 0;
    PUCHAR virtualAddress = NULL;
    PDMA_OPERATIONS dmaOp = NULL;
    PHYSICAL_ADDRESS logicalAddress = {0};
    PDEVICE_EXTENSION devExt = NULL;
    PUSBPORT_COMMON_BUFFER commonBuffer = NULL;
    PLIST_ENTRY cbListHead;
    PLIST_ENTRY cbListEntry;
    PULONG cbListFreeCount;
    KIRQL irql;
    BOOLEAN useCachedMemory;

    ASSERT_PASSIVE();

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'acm>', 0, 0, NumberOfBytes);

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

#if defined(_ARM_) || defined(_ARM64_)
    // ARM has caches without bus-snooping so we will request
    // our common buffers to have no caching
    //
    useCachedMemory = FALSE;
#else
    useCachedMemory = TRUE;
#endif

    // Compute the number of pages needed to satisfy the request
    //
    pageCount = ADDRESS_AND_SIZE_TO_SPAN_PAGES(0, NumberOfBytes);

    // Round up the allocation size to a multiple of the page size
    //
    NumberOfBytes = pageCount * PAGE_SIZE;

    // Look on the small free list or the large free list based on the
    // allocation size.
    //
    if (NumberOfBytes < devExt->Fdo.LargeCommonBufferThreshold) {
        cbListHead = &devExt->Fdo.FreeSmallCommonBufferList;
        cbListFreeCount = &devExt->Fdo.FreeSmallCommonBufferCount;
    } else {
        cbListHead = &devExt->Fdo.FreeLargeCommonBufferList;
        cbListFreeCount = &devExt->Fdo.FreeLargeCommonBufferCount;
    }

    // Check for an large enough existing common buffer allocation on
    // the free list
    //
    KeAcquireSpinLock(&devExt->Fdo.CommonBufferListSpin, &irql);

    cbListEntry = cbListHead->Flink;

    while (cbListEntry != cbListHead) {

        commonBuffer = CONTAINING_RECORD(cbListEntry,
                                         USBPORT_COMMON_BUFFER,
                                         FreeListEntry);

        if (NumberOfBytes <= commonBuffer->TotalLength) {

            // Found a large enough common buffer allocation.  Remove
            // it from the free list.
            //
            RemoveEntryList(cbListEntry);

            *cbListFreeCount -= 1;

            break;

        } else {

            // Common buffer allocation not large enough, keep looking.
            //
            cbListEntry = cbListEntry->Flink;
            commonBuffer = NULL;
        }
    }

    KeReleaseSpinLock(&devExt->Fdo.CommonBufferListSpin, irql);

    if (commonBuffer != NULL) {

        // Zero the common buffer from the existing allocation removed
        // from the free list
        //
        RtlSecureZeroMemory(commonBuffer->MiniportVa, commonBuffer->TotalLength);

    } else {

        // Allocate a structure to track a new common buffer allocation
        //
        ALLOC_POOL_Z(commonBuffer, NonPagedPoolNx, sizeof(USBPORT_COMMON_BUFFER));

        if (commonBuffer != NULL) {

            DbgTrace((UsbpDebugTrace,"'ALLOC(%d) bytes\n", NumberOfBytes));

            if (devExt->Fdo.CommonBuffer2GBLimit) {
                PHYSICAL_ADDRESS minPhysicalAddress;
                PHYSICAL_ADDRESS maxPhysicalAddress;
                PHYSICAL_ADDRESS boundaryPhysicalAddress;

                // Our private version of AllocateCommonBuffer()
                //
                minPhysicalAddress.QuadPart = 0;
                maxPhysicalAddress.QuadPart = 0x7FFFFFFF;   // (2GB - 1)
                boundaryPhysicalAddress.QuadPart = 0x0000000100000000;

                virtualAddress = MmAllocateContiguousNodeMemory(
                                     NumberOfBytes,
                                     minPhysicalAddress,
                                     maxPhysicalAddress,
                                     boundaryPhysicalAddress,
                                     useCachedMemory?(PAGE_READWRITE):(PAGE_READWRITE | PAGE_NOCACHE),
                                     MM_ANY_NODE_OK);

                if (virtualAddress != NULL) {
                    logicalAddress = MmGetPhysicalAddress(virtualAddress);
                }

            } else {

                if (devExt->Fdo.UseSingleDmaAdapter != FALSE) {

                    PHYSICAL_ADDRESS maxPhysicalAddress;

                    maxPhysicalAddress.QuadPart = 0xFFFFFFFF;   // (4GB - 1)
                    dmaOp = devExt->Fdo.AdapterObject->DmaOperations;
                    virtualAddress = dmaOp->AllocateCommonBufferEx(
                                         devExt->Fdo.AdapterObject,
                                         &maxPhysicalAddress,
                                         NumberOfBytes,
                                         &logicalAddress,
                                         useCachedMemory,
                                         MM_ANY_NODE_OK);

                } else {

                    dmaOp = devExt->Fdo.CommonBufferAdapterObject->DmaOperations;
                    virtualAddress = dmaOp->AllocateCommonBuffer(
                                         devExt->Fdo.CommonBufferAdapterObject,
                                         NumberOfBytes,
                                         &logicalAddress,
                                         useCachedMemory);
                }
            }

#if DBG
            if (virtualAddress == NULL) {
                USBPORT_KdPrint((1,"'HalAllocateCommonBuffer %d bytes failed\n", NumberOfBytes));
                TEST_TRAP();
            }
#endif

            if (virtualAddress != NULL) {

                devExt->Fdo.StatCommonBufferBytes += NumberOfBytes;

                // Initialize the common buffer allocation tracking
                // structure
                //
                commonBuffer->Sig = SIG_CMNBUF;
                commonBuffer->TotalLength = NumberOfBytes;
                commonBuffer->VirtualAddress = virtualAddress;
                commonBuffer->LogicalAddress = logicalAddress;

                // The AllocateCommonBuffer documentation does not
                // explicitly state that the returned addresses are page
                // aligned.  Make it so just in case.
                //
                commonBuffer->MiniportVa = PAGE_ALIGN(virtualAddress);
                commonBuffer->MiniportPhys = logicalAddress.LowPart & ~(PAGE_SIZE-1);;

                // USB controllers only support 32 bit phys addresses
                // for control structures
                USBPORT_ASSERT(logicalAddress.HighPart == 0);

                // zero the client part
                RtlSecureZeroMemory(commonBuffer->MiniportVa, commonBuffer->TotalLength);

            } else {

                // The common buffer allocation failed.  Free the
                // tracking structure and return failure.
                //
                UsbFreePool(commonBuffer);
                commonBuffer = NULL;
            }
        }
    }

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'acm<',
             (commonBuffer ? commonBuffer->MiniportVa : 0),
             (commonBuffer ? commonBuffer->MiniportPhys : 0),
             commonBuffer);

    return commonBuffer;
}


VOID
USBPORT_HalFreeCommonBuffer(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBPORT_COMMON_BUFFER CommonBuffer
    )

/*++

Routine Description:

    Free a common buffer for the miniport

Arguments:

Return Value:

    None.

--*/

{
    PDMA_OPERATIONS dmaOp = NULL;
    PDEVICE_EXTENSION devExt = NULL;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(CommonBuffer != NULL);
    ASSERT_COMMON_BUFFER(CommonBuffer);

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'hFCB',
            CommonBuffer, CommonBuffer->TotalLength,
            CommonBuffer->MiniportVa);

#if DBG
    // zap the miniport section
    RtlFillMemory(CommonBuffer->MiniportVa, CommonBuffer->TotalLength, 'U');
#endif

    devExt->Fdo.StatCommonBufferBytes -= CommonBuffer->TotalLength;

    if (devExt->Fdo.CommonBuffer2GBLimit) {

        // Our private version of FreeCommonBuffer()
        //
        MmFreeContiguousMemory(CommonBuffer->VirtualAddress);

    } else {

        if (devExt->Fdo.UseSingleDmaAdapter != FALSE) {

            dmaOp = devExt->Fdo.AdapterObject->DmaOperations;
            dmaOp->FreeCommonBuffer(devExt->Fdo.AdapterObject,
                                    CommonBuffer->TotalLength,
                                    CommonBuffer->LogicalAddress,
                                    CommonBuffer->VirtualAddress,
                                    TRUE);

        } else {

            dmaOp = devExt->Fdo.CommonBufferAdapterObject->DmaOperations;
            dmaOp->FreeCommonBuffer(devExt->Fdo.CommonBufferAdapterObject,
                                    CommonBuffer->TotalLength,
                                    CommonBuffer->LogicalAddress,
                                    CommonBuffer->VirtualAddress,
                                    TRUE);
        }
    }

    UsbFreePool(CommonBuffer);
}

VOID
USBPORT_HalFreeCommonBufferToFreeList(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBPORT_COMMON_BUFFER CommonBuffer
    )

/*++

Routine Description:

    Free a common buffer for the miniport

Arguments:

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION devExt = NULL;
    PUSBPORT_COMMON_BUFFER cb = NULL;
    PUSBPORT_COMMON_BUFFER cbToFree = NULL;
    PLIST_ENTRY cbListHead;
    PLIST_ENTRY cbListEntry;
    PULONG cbListFreeCount;
    PULONG cbListFreeMaxCount;
    KIRQL irql;

    ASSERT_PASSIVE();

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'fcm>',
            CommonBuffer, CommonBuffer->TotalLength,
            CommonBuffer->MiniportVa);

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

#if DBG
    // zap the miniport section
    RtlFillMemory(CommonBuffer->MiniportVa, CommonBuffer->TotalLength, 'U');
#endif

    // Place the allocation on the small or large allocation free list
    // using an insertion sort based on the size of the allocation.
    // Larger allocations are inserted after smaller allocations.
    //
    if (CommonBuffer->TotalLength < devExt->Fdo.LargeCommonBufferThreshold) {
        cbListHead = &devExt->Fdo.FreeSmallCommonBufferList;
        cbListFreeCount = &devExt->Fdo.FreeSmallCommonBufferCount;
        cbListFreeMaxCount = &devExt->Fdo.FreeSmallCommonBufferMaxCount;
    } else {
        cbListHead = &devExt->Fdo.FreeLargeCommonBufferList;
        cbListFreeCount = &devExt->Fdo.FreeLargeCommonBufferCount;
        cbListFreeMaxCount = &devExt->Fdo.FreeLargeCommonBufferMaxCount;
    }

    KeAcquireSpinLock(&devExt->Fdo.CommonBufferListSpin, &irql);

    cbListEntry = cbListHead->Flink;

    while (cbListEntry != cbListHead) {

        cb = CONTAINING_RECORD(cbListEntry,
                               USBPORT_COMMON_BUFFER,
                               FreeListEntry);

        if (CommonBuffer->TotalLength <= cb->TotalLength) {

            // The common buffer allocation to free is smaller than or
            // equal in size to the next one on the list.  Insert it
            // before the next one on the list.
            //
            InsertHeadList(cbListEntry->Blink, &CommonBuffer->FreeListEntry);

            break;

        } else {

            // The common buffer allocation to free is the largest so
            // far, keep looking.
            //
            cbListEntry = cbListEntry->Flink;
            cb = NULL;
        }
    }

    if (cb == NULL) {

        // The common buffer allocation to free is larger than all
        // allocations on the free list, or the free list is currently
        // empty.

        InsertTailList(cbListHead, &CommonBuffer->FreeListEntry);
    }

    // Limit the number of common buffer allocations on the free list.
    // If the free list is at the limit then discard the first
    // (smallest) common buffer allocation on the free list.
    //
    if (*cbListFreeCount >= *cbListFreeMaxCount) {

        cbListEntry = RemoveHeadList(cbListHead);

        cbToFree = CONTAINING_RECORD(cbListEntry,
                                     USBPORT_COMMON_BUFFER,
                                     FreeListEntry);
    } else {

        *cbListFreeCount += 1;
    }

    KeReleaseSpinLock(&devExt->Fdo.CommonBufferListSpin, irql);

    if (cbToFree != NULL) {
        USBPORT_HalFreeCommonBuffer(FdoDeviceObject, cbToFree);
    }
}

// This code is normally never compliled.
// set USER_C_FLAGS=-DUSBPORT_BUGCHECK_TEST
// and then build a private binary to enable this test code.
// Manually set MapHwPhysicalToVirtualBugcheck in the debugger to cause
// USBPORT to bugcheck.
//
#ifdef USBPORT_BUGCHECK_TEST
ULONG MapHwPhysicalToVirtualBugcheck = 0;
#endif

PUCHAR
USBPORTSVC_MapHwPhysicalToVirtual(
    HW_32BIT_PHYSICAL_ADDRESS HwPhysicalAddress,
    PDEVICE_DATA DeviceData,
    PENDPOINT_DATA EndpointData
    )

/*++

Routine Description:

    given a physical address return the corresponding
    virtual address.

Arguments:

    if the phys address is associated with an endpoint
    the endpoint is passed in.

Return Value:

    Virtual Address, NULL if not found

--*/

{
    PDEVICE_EXTENSION devExt;
    PUCHAR virtualAddress;
    PHCD_ENDPOINT endpoint;
    ULONG offset;
    PDEVICE_OBJECT fdoDeviceObject;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);

    fdoDeviceObject = devExt->HcFdoDeviceObject;

#ifdef USBPORT_BUGCHECK_TEST
    if (MapHwPhysicalToVirtualBugcheck) {
        HwPhysicalAddress = 0xDEADF00D;
    }
#endif

    LOGENTRY(fdoDeviceObject, LOG_TRANSFER, 'mapP', HwPhysicalAddress, 0,
             EndpointData);

    if (EndpointData == NULL) {
        TEST_TRAP();
    } else {
        PUSBPORT_COMMON_BUFFER cb;

        ENDPOINT_FROM_EPDATA(endpoint, EndpointData);
        ASSERT_ENDPOINT(endpoint);

        cb = endpoint->CommonBuffer;

        // mask off base physical address
        offset = HwPhysicalAddress - cb->MiniportPhys;
        virtualAddress = (PUCHAR)cb->MiniportVa + offset;

        LOGENTRY(fdoDeviceObject, LOG_TRANSFER, 'mpPV', HwPhysicalAddress, offset,
                  cb->MiniportVa);

        USBPORT_ASSERT(HwPhysicalAddress >= cb->MiniportPhys &&
            HwPhysicalAddress < cb->MiniportPhys + cb->TotalLength);

        if (!(HwPhysicalAddress >= cb->MiniportPhys &&
              HwPhysicalAddress < cb->MiniportPhys + cb->TotalLength)) {

                ULONG vidDev;

                //address is out of bounds, this indicates a problem with the
                //controller, it is accessing physical addresses that it should
                //not be.

                vidDev = devExt->Fdo.PciVendorId;
                vidDev <<=16;
                vidDev |= devExt->Fdo.PciDeviceId;

                // Throw a miniport bugcheck and save the relevent information
                // for OCA.

                BUGCHECK_FDO(fdoDeviceObject,
                             USBBUGCODE_INVALID_PHYSICAL_ADDRESS,   // BugCode
                             (ULONG_PTR) devExt,                    // BugCheckParameter2
                             (ULONG_PTR) vidDev,                    // BugCheckParameter3
                             (ULONG_PTR) EndpointData,              // BugCheckParameter4
                             cb->TotalLength,                       // DataLength
                             cb->MiniportVa);                       // DataBuffer
        }

        LOGENTRY(fdoDeviceObject, LOG_TRANSFER, 'mapV', HwPhysicalAddress, 0,
                  virtualAddress);

        return virtualAddress;
    }


    // probably a bug in the miniport
    TEST_TRAP();

    return NULL;
}
