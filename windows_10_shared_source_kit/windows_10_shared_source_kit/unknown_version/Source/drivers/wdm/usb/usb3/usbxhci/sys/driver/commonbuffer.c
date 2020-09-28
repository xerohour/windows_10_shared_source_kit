/*++
Copyright (c) Microsoft Corporation

Module Name:

    commonbuffer.c

Abstract:

    USB XHCI Driver

--*/

#include "pch.h"

#include "commonbuffer.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, CommonBuffer_InitializePolicy)
#pragma alloc_text(PAGE, CommonBuffer_Create)
#endif

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
CommonBuffer_InitializePolicy(
    __in
        PCOMMON_BUFFER_DATA     CommonBufferData
    )
{
    PAGED_CODE();

    if (Controller_GetTestConfiguration(CommonBufferData->ControllerHandle)->CommonBufferDisableCaching == 1) {

        LogInfo(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                "TestConfig: Disable caching in CommonBuffer object");

        CommonBufferData->Policy.LargeSegmentInitialCount           = 0;
        CommonBufferData->Policy.LargeSegmentPreallocationThreshold = 0;
        CommonBufferData->Policy.LargeSegmentPreallocationCount     = 0;
        CommonBufferData->Policy.LargeSegmentReclamationThreshold   = 0;

        CommonBufferData->Policy.SmallSegmentInitialCount           = 0;
        CommonBufferData->Policy.SmallSegmentPreallocationThreshold = 0;
        CommonBufferData->Policy.SmallSegmentPreallocationCount     = 0;
        CommonBufferData->Policy.SmallSegmentReclamationThreshold   = 0;

    } else {

        CommonBufferData->Policy.LargeSegmentInitialCount           = LARGE_SEGMENT_INITIAL_COUNT;
        CommonBufferData->Policy.LargeSegmentPreallocationThreshold = LARGE_SEGMENT_PREALLOCATION_THRESHOLD;
        CommonBufferData->Policy.LargeSegmentPreallocationCount     = LARGE_SEGMENT_PREALLOCATION_COUNT;
        CommonBufferData->Policy.LargeSegmentReclamationThreshold   = LARGE_SEGMENT_RECLAMATION_THRESHOLD;

        CommonBufferData->Policy.SmallSegmentInitialCount           = SMALL_SEGMENT_INITIAL_COUNT;
        CommonBufferData->Policy.SmallSegmentPreallocationThreshold = SMALL_SEGMENT_PREALLOCATION_THRESHOLD;
        CommonBufferData->Policy.SmallSegmentPreallocationCount     = SMALL_SEGMENT_PREALLOCATION_COUNT;
        CommonBufferData->Policy.SmallSegmentReclamationThreshold   = SMALL_SEGMENT_RECLAMATION_THRESHOLD;
    }
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
CommonBuffer_Create(
    __in
        WDFDEVICE   WdfDevice,
    __in
        HANDLE      ControllerHandle,
    __inout
        PHANDLE     ObjectHandle
    )
/*++

Routine Description:

    This function creates an common buffer object and initializes the data
    associated with common buffers.

--*/
{
    NTSTATUS                    status;
    WDFDMAENABLER               wdfDmaEnabler;
    WDF_DMA_PROFILE             wdfDmaProfile;
    WDF_DMA_ENABLER_CONFIG      wdfDmaConfig;
    WDF_OBJECT_ATTRIBUTES       wdfAttributes;
    PCOMMON_BUFFER_DATA         commonBufferData;
    PREGISTER_DATA              registerData;
    WDF_WORKITEM_CONFIG         wdfWorkItemConfig;
    size_t                      maxFragmentSize;
    ULONG                       maxDmaSize;
    ULONG                       mapRegisterCount;
    KSE_XHCI_CONTROLLER_FLAGS   deviceFlags;
    PTEST_CONFIGURATION         testConfiguration;

    PAGED_CODE();

    TRY {

        testConfiguration = Controller_GetTestConfiguration(ControllerHandle);

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, COMMON_BUFFER_DATA);
        wdfAttributes.EvtCleanupCallback = CommonBuffer_WdfEvtCleanupCallback;

        //
        // Determine if the controller is 32 or 64 bit.
        //
        registerData = Controller_GetRegisterHandle(ControllerHandle);

        if (Register_GetAddressingCapability(registerData) == 0) {

            deviceFlags.AsULong64 = 0;
            deviceFlags.Use32BitRegisterAccess = 1;

            Controller_SetDeviceFlags(ControllerHandle, deviceFlags);

            wdfDmaProfile = WdfDmaProfileScatterGatherDuplex;

        } else {

            wdfDmaProfile = WdfDmaProfileScatterGather64Duplex;
        }

        maxDmaSize = MAX_DMA_TRANSFER_SIZE;

        //
        // Create DMA enabler that is used for allocation of DMA resources.
        //
        WDF_DMA_ENABLER_CONFIG_INIT(&wdfDmaConfig, wdfDmaProfile, maxDmaSize);

        wdfDmaConfig.Flags = WDF_DMA_ENABLER_CONFIG_NO_SGLIST_PREALLOCATION;
        wdfDmaConfig.WdmDmaVersionOverride = DEVICE_DESCRIPTION_VERSION3;

        status = WdfDmaEnablerCreate(WdfDevice, &wdfDmaConfig, &wdfAttributes, &wdfDmaEnabler);

        if (!NT_SUCCESS(status)) {

            LogError(Controller_GetRecorderLog(ControllerHandle), CommonBuffer,
                     "WdfDmaEnablerCreate Failed %!STATUS!", status);
            LEAVE;
        }

        maxFragmentSize = WdfDmaEnablerGetFragmentLength(wdfDmaEnabler, WdfDmaDirectionReadFromDevice);
        if (maxFragmentSize > MAX_DMA_TRANSFER_SIZE) {

            maxDmaSize = MAX_DMA_TRANSFER_SIZE;

        } else {

            maxDmaSize = (ULONG) maxFragmentSize;
        }

        mapRegisterCount = BYTES_TO_PAGES(maxDmaSize) + 1;
 
        if ((testConfiguration->MaxDmaMapRegisters > 1) &&
            (testConfiguration->MaxDmaMapRegisters < mapRegisterCount)) {

            mapRegisterCount = testConfiguration->MaxDmaMapRegisters;
            maxDmaSize = (mapRegisterCount - 1) * PAGE_SIZE;

            LogInfo(Controller_GetRecorderLog(ControllerHandle), CommonBuffer,
                    "TestConfig: Reduce DMA Map Register count to %u, Max DMA Size: %u", 
                    mapRegisterCount, maxDmaSize);
        }

        commonBufferData = GetCommonBufferData(wdfDmaEnabler);

        commonBufferData->WdfDmaEnabler = wdfDmaEnabler;
        commonBufferData->ControllerHandle = ControllerHandle;

        LogInfo(Controller_GetRecorderLog(ControllerHandle), CommonBuffer,
                "wdfDmaEnabler 0x%p", wdfDmaEnabler);

        //
        // Allocate common buffer resources during AddDevice. These are freed
        // automatically by WDF when the device is removed.
        //
        KeInitializeSpinLock(&commonBufferData->CommonBufferLock);

        InitializeListHead(&commonBufferData->LargeList);
        InitializeListHead(&commonBufferData->LargeSegmentHeaderList);
        InitializeListHead(&commonBufferData->SmallList);
        InitializeListHead(&commonBufferData->SmallSegmentHeaderList);
        InitializeListHead(&commonBufferData->BufferDataTrackingList);
        InitializeListHead(&commonBufferData->CallbackList);
        InitializeListHead(&commonBufferData->RebalanceResourcesList);

        CommonBuffer_InitializePolicy(commonBufferData);

        CommonBuffer_AllocateBuffers(commonBufferData,
                                     LARGE_SEGMENT_SIZE,
                                     commonBufferData->Policy.LargeSegmentInitialCount,
                                     &commonBufferData->LargeList,
                                     &commonBufferData->NumberOfLargeListEntries,
                                     &commonBufferData->AvailableLargeListEntries,
                                     &commonBufferData->LargeSegmentHeaderList);

        CommonBuffer_AllocateBuffers(commonBufferData,
                                     SMALL_SEGMENT_SIZE,
                                     commonBufferData->Policy.SmallSegmentInitialCount,
                                     &commonBufferData->SmallList,
                                     &commonBufferData->NumberOfSmallListEntries,
                                     &commonBufferData->AvailableSmallListEntries,
                                     &commonBufferData->SmallSegmentHeaderList);

        //
        // Retrieve the WDM DMA Adapter object from WDF. The xHCI driver is forced to
        // specify the direction for the DMA by WDF, even though it can be used for both
        // read and write DMA operations. This is because the WDF DMA Enabler object was
        // created with a non-duplex profile. In this case, WDF gets only one WDM
        // DMA Adapter object, which it internally uses for both read/write DMA operations.
        //
        commonBufferData->DmaAdapter =
            WdfDmaEnablerWdmGetDmaAdapter(wdfDmaEnabler, WdfDmaDirectionReadFromDevice);

        commonBufferData->MaxDmaSize = maxDmaSize;
        commonBufferData->MapRegisterCount = mapRegisterCount;

        KeInitializeEvent(&commonBufferData->WorkItemEvent, NotificationEvent, TRUE);

        //
        // Create a WDF work item that is used to rebalance common buffer resources.
        //
        WDF_OBJECT_ATTRIBUTES_INIT(&wdfAttributes);
        wdfAttributes.ParentObject = wdfDmaEnabler;

        WDF_WORKITEM_CONFIG_INIT(&wdfWorkItemConfig, CommonBuffer_RebalanceResourcesWorkItem);

        status = WdfWorkItemCreate(&wdfWorkItemConfig,
                                   &wdfAttributes,
                                   &commonBufferData->RebalanceResourcesWorkItem);

        //
        // This is a non-fatal error. If this fails, then common buffer won't
        // be rebalanced.
        //
        if (!NT_SUCCESS(status)) {
            LogError(Controller_GetRecorderLog(ControllerHandle), CommonBuffer,
                     "WdfWorkItemCreate Failed %!STATUS!", status);
        }

        Counter_CreateCommonBufferInstance(commonBufferData);

        //
        // Return common buffer data as the handle to the common buffer object.
        //
        *ObjectHandle = (HANDLE)commonBufferData;

    } FINALLY {

    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
CommonBuffer_D0Exit(
    __in
        PCOMMON_BUFFER_DATA     CommonBufferData,
    __in
        WDF_POWER_DEVICE_STATE  WdfTargetState
    )
/*++

Routine Description:

    This function is called on an exit from d0 event.

--*/
{
    UNREFERENCED_PARAMETER(WdfTargetState);

    CommonBuffer_FlushWorkItems(CommonBufferData);
}

VOID
CommonBuffer_WdfEvtCleanupCallback(
    WDFOBJECT   WdfDmaEnabler
    )
/*++

Routine Description:

    The driver's EvtCleanupCallback event callback function removes the driver's
    references on an object so that the object can be deleted.

--*/
{
    PCOMMON_BUFFER_DATA commonBufferData;

    TRY {

        commonBufferData = GetCommonBufferData(WdfDmaEnabler);

        // Do not attempt to clean up if CommonBuffer_Create() was not
        // successful.
        //
        if (commonBufferData->WdfDmaEnabler != WdfDmaEnabler) {
            LEAVE;
        }

        LogVerbose(Controller_GetRecorderLog(commonBufferData->ControllerHandle), CommonBuffer,
                   "WdfDmaEnabler 0x%p", WdfDmaEnabler);

        Counter_CloseCommonBufferInstance(commonBufferData);

        //
        // Log and assert if we detect that common buffer wasn't released by other objects in
        // the stack.
        //
        if (commonBufferData->NumberOfLargeListEntries !=
            commonBufferData->AvailableLargeListEntries) {

            LogWarning(Controller_GetRecorderLog(commonBufferData->ControllerHandle), CommonBuffer,
                       "NumberOfLargeListEntries %d != AvailableLargeListEntries %d",
                       commonBufferData->NumberOfLargeListEntries,
                       commonBufferData->AvailableLargeListEntries);
        }

        if (commonBufferData->NumberOfSmallListEntries !=
            commonBufferData->AvailableSmallListEntries) {

            LogWarning(Controller_GetRecorderLog(commonBufferData->ControllerHandle), CommonBuffer,
                       "NumberOfSmallListEntries %d != AvailableSmallListEntries %d",
                       commonBufferData->NumberOfSmallListEntries,
                       commonBufferData->AvailableSmallListEntries);
        }

        NT_ASSERT(commonBufferData->NumberOfLargeListEntries ==
                  commonBufferData->AvailableLargeListEntries);

        NT_ASSERT(commonBufferData->NumberOfSmallListEntries ==
                  commonBufferData->AvailableSmallListEntries);

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
CommonBuffer_AllocateBuffers(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData,
    __in
        ULONG               Size,
    __in
        ULONG               NumberToAllocate,
    __in
        PLIST_ENTRY         List,
    __inout
        PULONG              NumberOfListEntries,
    __inout
        PULONG              AvailableListEntries,
    __in
        PLIST_ENTRY         SegmentHeaderList
    )
/*++

Routine Description:

    This function is called to allocate a number of common buffer segments
    and insert them onto the list of available small common buffer allocations.

    The size parameter must divide evenly into PAGE_SIZE. The number of allocated
    segments depends on PAGE_SIZE divided by size.

Arguments:

    Size - The size of each segment. This value is equal or less than PAGE_SIZE. If
           this value is smaller than PAGE_SIZE, then it must divide evenly into PAGE_SIZE.

    NumberToAllocate - The number of segments to allocate. This value times the size
                       must be evenly divisible by PAGE_SIZE. If not, common buffer
                       is wasted, since the system uses a full PAGE for each
                       common buffer allocation.

    List - This is the list to add created BUFFER_DATA structures.

    NumberOfListEntries
    AvailableListEntries - These are included here to track how many entries are
                           successfully added to the list. It is possible only some
                           of the allocations succeed, so the tracking needs to be
                           done here in order properly account for the number added
                           to the list.

    SegmentHeaderList - For all common buffer entries, this list tracks the
                        common buffer for each segment in order to simplify access
                        to each common buffer allocation.

Return Value:

    None. This function may or may not succeed in allocating common buffer
    segments. Based on system resources, it is possible this can initially fail and
    then on subsequent attempts succeed.

--*/
{
    NTSTATUS                    status;
    PSEGMENT_HEADER             segmentHeader;
    PBUFFER_DATA                bufferData;
    ULONG                       pagesToAllocate;
    ULONG                       numberOfSegments;
    ULONG                       i;
    ULONG                       n;
    WDF_OBJECT_ATTRIBUTES       wdfAttributes;
    WDF_COMMON_BUFFER_CONFIG    wdfCommonBufferConfig;
    WDFCOMMONBUFFER             wdfCommonBuffer;
    KIRQL                       irql;

    NT_ASSERT((PAGE_SIZE % Size) == 0);

    TRY {

        status = STATUS_SUCCESS;
        wdfCommonBuffer = NULL;

        LogVerbose(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                   "NumberOfListEntries 0x%p *NumberOfListEntries %d", NumberOfListEntries,
                   *NumberOfListEntries);
        LogVerbose(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                   "AvailableListEntries 0x%p *AvailableListEntries %d", AvailableListEntries,
                   *AvailableListEntries);

        InterlockedIncrement(&CommonBufferData->Counter.AllocationCount);

        pagesToAllocate  = BYTES_TO_PAGES(NumberToAllocate * Size);
        numberOfSegments = PAGE_SIZE / Size;

        LogVerbose(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                   "pagesToAllocate %d numberOfSegments %d", pagesToAllocate, numberOfSegments);

        for (i = 0; i < pagesToAllocate; i++) {

            //
            // Each context contains a segment header plus n number of buffer data.
            //
            WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, SEGMENT_HEADER);

            wdfAttributes.ContextSizeOverride = sizeof(SEGMENT_HEADER) +
                                                (sizeof(BUFFER_DATA) * numberOfSegments);

            //
            // There is a WDF bug which doesn't allow to us correctly specify the
            // alignment on WdfCommonBufferCreate. The workaround is to use
            // WdfCommonBufferCreateWithConfig. At some point, when this is fixed
            // in WDF, then this code can be changed to use WdfCommonBufferCreate.
            //
            WDF_COMMON_BUFFER_CONFIG_INIT(&wdfCommonBufferConfig, FILE_BYTE_ALIGNMENT);

            status = WdfCommonBufferCreateWithConfig(CommonBufferData->WdfDmaEnabler,
                                                     PAGE_SIZE,
                                                     &wdfCommonBufferConfig,
                                                     &wdfAttributes,
                                                     &wdfCommonBuffer);

            if (!NT_SUCCESS(status)) {

                LogWarning(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                           "WdfCommonBufferCreateWithConfig Failed %!STATUS!", status);
                LEAVE;
            }

            KeAcquireSpinLock(&CommonBufferData->CommonBufferLock, &irql);

            CommonBufferData->Counter.PagesTotal += 1;

            segmentHeader = GetSegmentHeader(wdfCommonBuffer);

            //
            // PendingFree means that this page can be freed back to the pool the
            // next time that we do a sweep. By default, a newly allocated page is
            // considered OK to be freed only because it makes counting PagesInUse
            // easier. It has a possible downside that a page might get freed if
            // a sweep happens right away.
            //
            segmentHeader->PendingFree = TRUE;

            InsertTailList(SegmentHeaderList, &segmentHeader->ListEntry);

            bufferData = (PBUFFER_DATA)(((PUCHAR)segmentHeader) + sizeof(SEGMENT_HEADER));

            for (n = 0; n < numberOfSegments; n++) {

                //
                // Retrieve the virtual address and calculate the offset into
                // the common buffer.
                //
                bufferData[n].VirtualAddress =
                    (PUCHAR)WdfCommonBufferGetAlignedVirtualAddress(wdfCommonBuffer);

                bufferData[n].VirtualAddress += (n * Size);

                //
                // Retrieve the logical address and calculate the offset into
                // the common buffer.
                //
                bufferData[n].LogicalAddress =
                    WdfCommonBufferGetAlignedLogicalAddress(wdfCommonBuffer);

                //
                // Validate that the common buffer is page aligned. If not,
                // delete this common buffer object and continue. This is
                // only done on the first segment.
                //
                if ((n==0) && ((bufferData[n].LogicalAddress.LowPart & 0xFFF) != 0)) {

                    LogWarning(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                               "Common buffer not page aligned!");
                    WdfObjectDelete(wdfCommonBuffer);
                    wdfCommonBuffer = NULL;
                    break;
                }

                bufferData[n].LogicalAddress.QuadPart += (n * Size);

                //
                // Cache the WDF common buffer object and add it to the list.
                //
                bufferData[n].WdfCommonBuffer = wdfCommonBuffer;

                //
                // Save the size of this segment in the buffer data.
                //
                bufferData[n].Size = Size;

                InsertTailList(List, &bufferData[n].ListEntry);

                InsertTailList(&CommonBufferData->BufferDataTrackingList,
                               &bufferData[n].TrackingData.ListEntry);

                //
                // Update the counters used to track allocations.
                //
                *NumberOfListEntries += 1;
                *AvailableListEntries += 1;

                LogVerbose(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                           "%d.%d: VirtualAddress 0x%p LogicalAddress 0x%08x Size %d NumberOfListEntries %d AvailableListEntries %d",
                           i, n, bufferData[n].VirtualAddress, bufferData[n].LogicalAddress.LowPart,
                           bufferData[n].Size, *NumberOfListEntries, *AvailableListEntries);
            }

            KeReleaseSpinLock(&CommonBufferData->CommonBufferLock, irql);
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            //
            // If wdfCommonBuffer is not NULL, then wdfCommonBuffer object failed on
            // an add to the WDF collection. When this happens, delete wdfCommonBuffer,
            // since there's no way to track wdfCommonBuffer.
            //
            if (wdfCommonBuffer != NULL) {
                WdfObjectDelete(wdfCommonBuffer);
            }
        }
    }

    return;
}

VOID
CommonBuffer_WorkItem(
    _In_
        PVOID           IoObject,
    _In_opt_
        PVOID           Context,
    _In_
        PIO_WORKITEM    IoWorkItem
    )
{
    KIRQL                           irql;
    ULONG                           smallSegmentCount;
    ULONG                           largeSegmentCount;
    LIST_ENTRY                      callbackList;
    PCOMMON_BUFFER_CALLBACK_DATA    commonBufferCallbackData;
    LIST_ENTRY                      bufferList;
    BOOLEAN                         allocateBuffers;
    NTSTATUS                        status;
    PCOMMON_BUFFER_DATA             commonBufferData;

    UNREFERENCED_PARAMETER(IoObject);

    TRY {

        smallSegmentCount = 0;
        largeSegmentCount = 0;
        commonBufferData  = (PCOMMON_BUFFER_DATA)Context;

        InitializeListHead(&callbackList);

        KeAcquireSpinLock(&commonBufferData->CommonBufferLock, &irql);

        //
        // Check the callback list to see if any objects need notification that
        // more common buffer may be available.
        //
        while (IsListEmpty(&commonBufferData->CallbackList) == FALSE) {

            commonBufferCallbackData =
                (PCOMMON_BUFFER_CALLBACK_DATA)RemoveHeadList(&commonBufferData->CallbackList);

            if (commonBufferCallbackData->BufferSize <= SMALL_SEGMENT_SIZE) {
                smallSegmentCount += commonBufferCallbackData->NumberToAcquire;
            } else {
                NT_ASSERT(commonBufferCallbackData->BufferSize <= LARGE_SEGMENT_SIZE);
                largeSegmentCount += commonBufferCallbackData->NumberToAcquire;
            }

            InsertTailList(&callbackList, &commonBufferCallbackData->ListEntry);
        }

        if (commonBufferData->AvailableSmallListEntries < commonBufferData->Policy.SmallSegmentPreallocationThreshold) {
            smallSegmentCount += commonBufferData->Policy.SmallSegmentPreallocationCount;
        }

        if (commonBufferData->AvailableLargeListEntries < commonBufferData->Policy.LargeSegmentPreallocationThreshold) {
            largeSegmentCount += commonBufferData->Policy.LargeSegmentPreallocationCount;
        }

        KeReleaseSpinLock(&commonBufferData->CommonBufferLock, irql);

        LogVerbose(Controller_GetRecorderLog(commonBufferData->ControllerHandle), CommonBuffer,
                   "Allocating common buffer: small segments %d large segments %d",
                   smallSegmentCount, largeSegmentCount);

        if (smallSegmentCount != 0) {

            CommonBuffer_AllocateBuffers(commonBufferData,
                                         SMALL_SEGMENT_SIZE,
                                         smallSegmentCount,
                                         &commonBufferData->SmallList,
                                         &commonBufferData->NumberOfSmallListEntries,
                                         &commonBufferData->AvailableSmallListEntries,
                                         &commonBufferData->SmallSegmentHeaderList);
        }

        if (largeSegmentCount != 0) {

            CommonBuffer_AllocateBuffers(commonBufferData,
                                         LARGE_SEGMENT_SIZE,
                                         largeSegmentCount,
                                         &commonBufferData->LargeList,
                                         &commonBufferData->NumberOfLargeListEntries,
                                         &commonBufferData->AvailableLargeListEntries,
                                         &commonBufferData->LargeSegmentHeaderList);
        }

        while (IsListEmpty(&callbackList) == FALSE) {

            commonBufferCallbackData = (PCOMMON_BUFFER_CALLBACK_DATA)RemoveHeadList(&callbackList);
            InitializeListHead(&commonBufferCallbackData->ListEntry);

            LogVerbose(Controller_GetRecorderLog(commonBufferData->ControllerHandle), CommonBuffer,
                       "commonBufferCallbackData 0x%p Owner 0x%p NumberToAcquire %d",
                       commonBufferCallbackData, commonBufferCallbackData->Owner,
                       commonBufferCallbackData->NumberToAcquire);

            InitializeListHead(&bufferList);

            if (commonBufferCallbackData->BufferSize <= SMALL_SEGMENT_SIZE) {

                status = CommonBuffer_AcquireSmallBuffers(commonBufferData,
                                                          commonBufferCallbackData->NumberToAcquire,
                                                          &bufferList,
                                                          commonBufferCallbackData->Owner,
                                                          commonBufferCallbackData->Tag,
                                                          &allocateBuffers);
            } else {

                status = CommonBuffer_AcquireLargeBuffers(commonBufferData,
                                                          commonBufferCallbackData->NumberToAcquire,
                                                          &bufferList,
                                                          commonBufferCallbackData->Owner,
                                                          commonBufferCallbackData->Tag,
                                                          &allocateBuffers);
            }

            (commonBufferCallbackData->Callback)(commonBufferCallbackData,
                                                 status,
                                                 &bufferList);
        }

    } FINALLY {

        IoFreeWorkItem(IoWorkItem);
        KeSetEvent(&commonBufferData->WorkItemEvent, IO_NO_INCREMENT, FALSE);
    }

    return;
}

#pragma prefast(push)
#pragma prefast(disable:6014, "(PFD) Leaking memory 'ioWorkItem'. It will be freed by CommonBuffer_WorkItem" )

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
CommonBuffer_QueueWorkItem(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    )
/*++

Routine Description:

    This function queues a work item that will try to allocate more commonbuffer.

    NOTE: PostWin8: investigate if this function can be changed to WDF.

--*/
{
    BOOLEAN         queued;
    PIO_WORKITEM    ioWorkItem;

    TRY {

        queued = FALSE;

        ioWorkItem =
            IoAllocateWorkItem(Controller_GetWdmDeviceObject(CommonBufferData->ControllerHandle));

        if (ioWorkItem == NULL) {

            LogWarning(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                       "Failed to allocate work item");
            LEAVE;
        }

        KeClearEvent(&CommonBufferData->WorkItemEvent);

        IoQueueWorkItemEx(ioWorkItem,
                          CommonBuffer_WorkItem,
                          CriticalWorkQueue,
                          CommonBufferData);

        queued = TRUE;

    } FINALLY {

    }

    return queued;
}

#pragma prefast(pop)

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
PBUFFER_DATA
CommonBuffer_AcquireSmallBuffer(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData,
    __in
        HANDLE              Owner,
    __in
        ULONG               Tag,
    __out
        PBOOLEAN            AllocateBuffers
    )
{
    PBUFFER_DATA    bufferData;
    KIRQL           irql;
    PSEGMENT_HEADER segmentHeader;

    KeAcquireSpinLock(&CommonBufferData->CommonBufferLock, &irql);

    if (IsListEmpty(&CommonBufferData->SmallList) == FALSE) {

        bufferData = (PBUFFER_DATA)RemoveHeadList(&CommonBufferData->SmallList);
        RtlZeroMemory(bufferData->VirtualAddress, SMALL_SEGMENT_SIZE);
        NT_ASSERT(CommonBufferData->AvailableSmallListEntries > 0);
        CommonBufferData->AvailableSmallListEntries--;

        bufferData->TrackingData.Owner = Owner;
        bufferData->TrackingData.Tag = Tag;

        segmentHeader = GetSegmentHeader(bufferData->WdfCommonBuffer);

        if (segmentHeader->PendingFree) {
            segmentHeader->PendingFree = FALSE;
            CommonBufferData->Counter.PagesInUse += 1;
        }

    } else {

        bufferData = NULL;

        LogWarning(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                   "Small entry list empty");
    }

    if (CommonBufferData->AvailableSmallListEntries < CommonBufferData->Policy.SmallSegmentPreallocationThreshold) {
        *AllocateBuffers = TRUE;
    } else {
        *AllocateBuffers = FALSE;
    }

    KeReleaseSpinLock(&CommonBufferData->CommonBufferLock, irql);

    return bufferData;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
PBUFFER_DATA
CommonBuffer_AcquireLargeBuffer(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData,
    __in
        HANDLE              Owner,
    __in
        ULONG               Tag,
    __out
        PBOOLEAN            AllocateBuffers
    )
{
    PBUFFER_DATA    bufferData;
    KIRQL           irql;
    PSEGMENT_HEADER segmentHeader;

    KeAcquireSpinLock(&CommonBufferData->CommonBufferLock, &irql);

    if (IsListEmpty(&CommonBufferData->LargeList) == FALSE) {

        bufferData = (PBUFFER_DATA)RemoveHeadList(&CommonBufferData->LargeList);
        RtlZeroMemory(bufferData->VirtualAddress, LARGE_SEGMENT_SIZE);
        NT_ASSERT(CommonBufferData->AvailableLargeListEntries > 0);
        CommonBufferData->AvailableLargeListEntries--;

        bufferData->TrackingData.Owner = Owner;
        bufferData->TrackingData.Tag = Tag;

        segmentHeader = GetSegmentHeader(bufferData->WdfCommonBuffer);

        if (segmentHeader->PendingFree) {
            segmentHeader->PendingFree = FALSE;
            CommonBufferData->Counter.PagesInUse += 1;
        }

    } else {

        bufferData = NULL;

        LogWarning(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                   "Large entry list empty");
    }

    if (CommonBufferData->AvailableLargeListEntries < CommonBufferData->Policy.LargeSegmentPreallocationThreshold) {
        *AllocateBuffers = TRUE;
    } else {
        *AllocateBuffers = FALSE;
    }

    KeReleaseSpinLock(&CommonBufferData->CommonBufferLock, irql);

    return bufferData;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
PBUFFER_DATA
CommonBuffer_AcquireBuffer(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData,
    __in
        ULONG               RequiredSize,
    __in
        HANDLE              Owner,
    __in
        ULONG               Tag
    )
/*++

Routine Description:

    This function is called to acquire a single common buffer segment.

--*/
{
    PBUFFER_DATA    bufferData;
    BOOLEAN         allocateBuffers;
    ULONG           numberToAllocate;

    NT_ASSERT(CommonBufferData != NULL);
    NT_ASSERT(RequiredSize != 0);
    NT_ASSERT(RequiredSize <= LARGE_SEGMENT_SIZE);

    TRY {

        //
        // There are two attempts made to acquire the BufferData. The first is successful
        // if there is an available allocation on the list. If there is not an available
        // BufferData, then an attempt is made to increase the list. This latter can only
        // be done at passive_level. If this is successful, then a BufferData is returned
        // to the calling function. If this is not called at passive, then a work item is
        // queued to grow the list and a NULL BufferData is returned.
        //
        if (RequiredSize <= SMALL_SEGMENT_SIZE) {

            bufferData = CommonBuffer_AcquireSmallBuffer(CommonBufferData,
                                                         Owner,
                                                         Tag,
                                                         &allocateBuffers);

            if ((bufferData == NULL) || (allocateBuffers)) {

                if (KeGetCurrentIrql() == PASSIVE_LEVEL) {

                    numberToAllocate = 0;

                    if (bufferData == NULL) {
                        numberToAllocate++;
                    }

                    if (allocateBuffers) {
                        numberToAllocate += CommonBufferData->Policy.SmallSegmentPreallocationCount;
                    }

                    CommonBuffer_AllocateBuffers(CommonBufferData,
                                                 SMALL_SEGMENT_SIZE,
                                                 numberToAllocate,
                                                 &CommonBufferData->SmallList,
                                                 &CommonBufferData->NumberOfSmallListEntries,
                                                 &CommonBufferData->AvailableSmallListEntries,
                                                 &CommonBufferData->SmallSegmentHeaderList);

                    //
                    // Only retry allocating additional buffers if the initial
                    // allocation failed.
                    //
                    if (bufferData == NULL) {

                        bufferData = CommonBuffer_AcquireSmallBuffer(CommonBufferData,
                                                                     Owner,
                                                                     Tag,
                                                                     &allocateBuffers);
                    }

                } else {

                    CommonBuffer_QueueWorkItem(CommonBufferData);
                }
            }

        } else if (RequiredSize <= LARGE_SEGMENT_SIZE) {

            bufferData = CommonBuffer_AcquireLargeBuffer(CommonBufferData,
                                                         Owner,
                                                         Tag,
                                                         &allocateBuffers);

            if ((bufferData == NULL) || (allocateBuffers)) {

                if (KeGetCurrentIrql() == PASSIVE_LEVEL) {

                    numberToAllocate = 0;

                    if (bufferData == NULL) {
                        numberToAllocate++;
                    }

                    if (allocateBuffers) {
                        numberToAllocate += CommonBufferData->Policy.LargeSegmentPreallocationCount;
                    }

                    CommonBuffer_AllocateBuffers(CommonBufferData,
                                                 LARGE_SEGMENT_SIZE,
                                                 numberToAllocate,
                                                 &CommonBufferData->LargeList,
                                                 &CommonBufferData->NumberOfLargeListEntries,
                                                 &CommonBufferData->AvailableLargeListEntries,
                                                 &CommonBufferData->LargeSegmentHeaderList);

                    //
                    // Only retry allocating additional buffers if the initial
                    // allocation failed.
                    //
                    if (bufferData == NULL) {

                        bufferData = CommonBuffer_AcquireLargeBuffer(CommonBufferData,
                                                                     Owner,
                                                                     Tag,
                                                                     &allocateBuffers);
                    }

                } else {

                    CommonBuffer_QueueWorkItem(CommonBufferData);
                }
            }

        } else {

            bufferData = NULL;

            LogWarning(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                       "RequiredSize too big %d", RequiredSize);
            LEAVE;
        }

    } FINALLY {

    }

    return bufferData;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
CommonBuffer_ReleaseBuffer(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData,
    __in
        PBUFFER_DATA        BufferData
    )
/*++

Routine Description:

    This function is called to release a single common buffer segment.

--*/
{
    KIRQL   irql;

    NT_ASSERT(CommonBufferData != NULL);

    TRY {

        KeAcquireSpinLock(&CommonBufferData->CommonBufferLock, &irql);

        BufferData->TrackingData.Owner = NULL;
        BufferData->TrackingData.Tag = 0;

        if (BufferData->Size == SMALL_SEGMENT_SIZE) {

            InsertHeadList(&CommonBufferData->SmallList, &BufferData->ListEntry);
            CommonBufferData->AvailableSmallListEntries++;

        } else if (BufferData->Size == LARGE_SEGMENT_SIZE) {

            InsertHeadList(&CommonBufferData->LargeList, &BufferData->ListEntry);
            CommonBufferData->AvailableLargeListEntries++;

        } else {

            LogError(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                     "BufferData->Size %d is corrupted", BufferData->Size);
        }

        KeReleaseSpinLock(&CommonBufferData->CommonBufferLock, irql);

    } FINALLY {

    }

    return;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
CommonBuffer_AcquireSmallBuffers(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData,
    __in
        ULONG               NumberToAcquire,
    __in
        PLIST_ENTRY         BufferList,
    __in
        HANDLE              Owner,
    __in
        ULONG               Tag,
    __out
        PBOOLEAN            AllocateBuffers
    )
{
    NTSTATUS        status;
    PBUFFER_DATA    bufferData;
    KIRQL           irql;
    ULONG           i;
    PSEGMENT_HEADER segmentHeader;

    KeAcquireSpinLock(&CommonBufferData->CommonBufferLock, &irql);

    LogVerbose(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
               "Small entries: requested %d available %d", NumberToAcquire,
               CommonBufferData->AvailableSmallListEntries);

    if (CommonBufferData->AvailableSmallListEntries < NumberToAcquire) {

        LogWarning(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                   "Insufficient small entries: requested %d available %d", NumberToAcquire,
                   CommonBufferData->AvailableSmallListEntries);

        status = STATUS_INSUFFICIENT_RESOURCES;

    } else {

        for (i=0; i<NumberToAcquire; i++) {

            bufferData = (PBUFFER_DATA)RemoveHeadList(&CommonBufferData->SmallList);
            RtlZeroMemory(bufferData->VirtualAddress, SMALL_SEGMENT_SIZE);

            bufferData->TrackingData.Owner = Owner;
            bufferData->TrackingData.Tag = Tag;

            segmentHeader = GetSegmentHeader(bufferData->WdfCommonBuffer);

            if (segmentHeader->PendingFree) {
                segmentHeader->PendingFree = FALSE;
                CommonBufferData->Counter.PagesInUse += 1;
            }

            InsertTailList(BufferList, &bufferData->ListEntry);
        }

        CommonBufferData->AvailableSmallListEntries -= NumberToAcquire;

        status = STATUS_SUCCESS;
    }

    if (CommonBufferData->AvailableSmallListEntries < CommonBufferData->Policy.SmallSegmentPreallocationThreshold) {
        *AllocateBuffers = TRUE;
    } else {
        *AllocateBuffers = FALSE;
    }

    KeReleaseSpinLock(&CommonBufferData->CommonBufferLock, irql);

    return status;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
CommonBuffer_AcquireLargeBuffers(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData,
    __in
        ULONG               NumberToAcquire,
    __in
        PLIST_ENTRY         BufferList,
    __in
        HANDLE              Owner,
    __in
        ULONG               Tag,
    __out
        PBOOLEAN            AllocateBuffers
    )
{
    NTSTATUS        status;
    PBUFFER_DATA    bufferData;
    KIRQL           irql;
    ULONG           i;
    PSEGMENT_HEADER segmentHeader;

    KeAcquireSpinLock(&CommonBufferData->CommonBufferLock, &irql);

    LogVerbose(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
               "Large entries: requested %d available %d", NumberToAcquire,
               CommonBufferData->AvailableLargeListEntries);

    if (CommonBufferData->AvailableLargeListEntries < NumberToAcquire) {

        LogWarning(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                   "Insufficient large entries: requested %d available %d", NumberToAcquire,
                   CommonBufferData->AvailableLargeListEntries);

        status = STATUS_INSUFFICIENT_RESOURCES;

    } else {

        for (i=0; i<NumberToAcquire; i++) {

            bufferData = (PBUFFER_DATA)RemoveHeadList(&CommonBufferData->LargeList);
            RtlZeroMemory(bufferData->VirtualAddress, LARGE_SEGMENT_SIZE);

            bufferData->TrackingData.Owner = Owner;
            bufferData->TrackingData.Tag = Tag;

            segmentHeader = GetSegmentHeader(bufferData->WdfCommonBuffer);

            if (segmentHeader->PendingFree) {
                segmentHeader->PendingFree = FALSE;
                CommonBufferData->Counter.PagesInUse += 1;
            }

            InsertTailList(BufferList, &bufferData->ListEntry);
        }

        CommonBufferData->AvailableLargeListEntries -= NumberToAcquire;

        status = STATUS_SUCCESS;
    }

    if (CommonBufferData->AvailableLargeListEntries < CommonBufferData->Policy.LargeSegmentPreallocationThreshold) {
        *AllocateBuffers = TRUE;
    } else {
        *AllocateBuffers = FALSE;
    }

    KeReleaseSpinLock(&CommonBufferData->CommonBufferLock, irql);

    return status;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
CommonBuffer_AcquireBuffers(
    __in
        PCOMMON_BUFFER_DATA             CommonBufferData,
    __in
        ULONG                           NumberToAcquire,
    __in
        ULONG                           RequiredSize,
    __inout
        PLIST_ENTRY                     BufferList,
    __in
        HANDLE                          Owner,
    __in
        ULONG                           Tag,
    __in_opt
        PCOMMON_BUFFER_CALLBACK_DATA    CommonBufferCallbackData
    )
/*++

Routine Description:

    This function is called to acquire a number of common buffer segments
    and insert them onto a list common buffer allocations returned to the caller.

--*/
{
    NTSTATUS    status;
    BOOLEAN     allocateBuffers;
    KIRQL       irql;
    ULONG       numberToAllocate;

    NT_ASSERT(CommonBufferData != NULL);
    NT_ASSERT(RequiredSize != 0);
    NT_ASSERT(RequiredSize <= LARGE_SEGMENT_SIZE);
    NT_ASSERT(BufferList != NULL);

    TRY {

        if (RequiredSize <= SMALL_SEGMENT_SIZE) {

            status = CommonBuffer_AcquireSmallBuffers(CommonBufferData,
                                                      NumberToAcquire,
                                                      BufferList,
                                                      Owner,
                                                      Tag,
                                                      &allocateBuffers);

            if ((!NT_SUCCESS(status)) || (allocateBuffers)) {

                if (KeGetCurrentIrql() == PASSIVE_LEVEL) {

                    numberToAllocate = 0;

                    if (!NT_SUCCESS(status)) {
                        numberToAllocate += NumberToAcquire;
                    }

                    if (allocateBuffers) {
                        numberToAllocate += CommonBufferData->Policy.SmallSegmentPreallocationCount;
                    }

                    CommonBuffer_AllocateBuffers(CommonBufferData,
                                                 SMALL_SEGMENT_SIZE,
                                                 numberToAllocate,
                                                 &CommonBufferData->SmallList,
                                                 &CommonBufferData->NumberOfSmallListEntries,
                                                 &CommonBufferData->AvailableSmallListEntries,
                                                 &CommonBufferData->SmallSegmentHeaderList);

                    //
                    // Only retry allocating additional buffers if the initial
                    // allocation failed.
                    //
                    if (!NT_SUCCESS(status)) {

                        status = CommonBuffer_AcquireSmallBuffers(CommonBufferData,
                                                                  NumberToAcquire,
                                                                  BufferList,
                                                                  Owner,
                                                                  Tag,
                                                                  &allocateBuffers);
                    }

                } else {

                    if ((!NT_SUCCESS(status)) && (CommonBufferCallbackData)) {

                        status = STATUS_PENDING;

                        KeAcquireSpinLock(&CommonBufferData->CommonBufferLock, &irql);

                        InsertTailList(&CommonBufferData->CallbackList,
                                       &CommonBufferCallbackData->ListEntry);

                        KeReleaseSpinLock(&CommonBufferData->CommonBufferLock, irql);
                    }

                    if ((CommonBuffer_QueueWorkItem(CommonBufferData) == FALSE) &&
                        (CommonBufferCallbackData != NULL)) {

                        PCOMMON_BUFFER_CALLBACK_DATA    callbackData;
                        BOOLEAN                         found;

                        //
                        // Either the work item failed to be queued or we don't know if it
                        // was queued. In this case, we have to assume that the work item
                        // may have been called. So walk the list callback data to see if
                        // we find the one we added. If we did, remove it and return a
                        // fail status. If it's not found, then the work item must have
                        // remove it from the list.
                        //
                        found = FALSE;

                        KeAcquireSpinLock(&CommonBufferData->CommonBufferLock, &irql);

                        FOR_ALL_IN_LIST(COMMON_BUFFER_CALLBACK_DATA,
                                        &CommonBufferData->CallbackList,
                                        ListEntry,
                                        callbackData) {

                            if (callbackData == CommonBufferCallbackData) {

                                RemoveEntryList(&CommonBufferCallbackData->ListEntry);
                                found = TRUE;
                                break;
                            }
                        }

                        KeReleaseSpinLock(&CommonBufferData->CommonBufferLock, irql);

                        if (found) {
                            status = STATUS_INSUFFICIENT_RESOURCES;
                        }
                    }
                }
            }

        } else if (RequiredSize <= LARGE_SEGMENT_SIZE) {

            status = CommonBuffer_AcquireLargeBuffers(CommonBufferData,
                                                      NumberToAcquire,
                                                      BufferList,
                                                      Owner,
                                                      Tag,
                                                      &allocateBuffers);

            if ((!NT_SUCCESS(status)) || (allocateBuffers)) {

                if (KeGetCurrentIrql() == PASSIVE_LEVEL) {

                    numberToAllocate = 0;

                    if (!NT_SUCCESS(status)) {
                        numberToAllocate += NumberToAcquire;
                    }

                    if (allocateBuffers) {
                        numberToAllocate += CommonBufferData->Policy.LargeSegmentPreallocationCount;
                    }

                    CommonBuffer_AllocateBuffers(CommonBufferData,
                                                 LARGE_SEGMENT_SIZE,
                                                 numberToAllocate,
                                                 &CommonBufferData->LargeList,
                                                 &CommonBufferData->NumberOfLargeListEntries,
                                                 &CommonBufferData->AvailableLargeListEntries,
                                                 &CommonBufferData->LargeSegmentHeaderList);

                    //
                    // Only retry allocating additional buffers if the initial
                    // allocation failed.
                    //
                    if (!NT_SUCCESS(status)) {

                        status = CommonBuffer_AcquireLargeBuffers(CommonBufferData,
                                                                  NumberToAcquire,
                                                                  BufferList,
                                                                  Owner,
                                                                  Tag,
                                                                  &allocateBuffers);
                    }

                } else {

                    if ((!NT_SUCCESS(status)) && (CommonBufferCallbackData)) {

                        status = STATUS_PENDING;

                        KeAcquireSpinLock(&CommonBufferData->CommonBufferLock, &irql);

                        InsertTailList(&CommonBufferData->CallbackList,
                                       &CommonBufferCallbackData->ListEntry);

                        KeReleaseSpinLock(&CommonBufferData->CommonBufferLock, irql);
                    }

                    if ((CommonBuffer_QueueWorkItem(CommonBufferData) == FALSE) &&
                        (CommonBufferCallbackData != NULL)) {

                        PCOMMON_BUFFER_CALLBACK_DATA    callbackData;
                        BOOLEAN                         found;

                        //
                        // Either the work item failed to be queued or we don't know if it
                        // was queued. In this case, we have to assume that the work item
                        // may have been called. So walk the list callback data to see if
                        // we find the one we added. If we did, remove it and return a
                        // fail status. If it's not found, then the work item must have
                        // remove it from the list.
                        //
                        found = FALSE;

                        KeAcquireSpinLock(&CommonBufferData->CommonBufferLock, &irql);

                        FOR_ALL_IN_LIST(COMMON_BUFFER_CALLBACK_DATA,
                                        &CommonBufferData->CallbackList,
                                        ListEntry,
                                        callbackData) {

                            if (callbackData == CommonBufferCallbackData) {

                                RemoveEntryList(&CommonBufferCallbackData->ListEntry);
                                found = TRUE;
                                break;
                            }
                        }

                        KeReleaseSpinLock(&CommonBufferData->CommonBufferLock, irql);

                        if (found) {
                            status = STATUS_INSUFFICIENT_RESOURCES;
                        }
                    }
                }
            }

        } else {

            LogWarning(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                       "RequiredSize too big %d", RequiredSize);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
CommonBuffer_ReleaseBuffers(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData,
    __in
        PLIST_ENTRY         BufferList
    )
/*++

Routine Description:

    This function is called to release a number of common buffer segments
    and insert them onto the list of available common buffer allocations.

--*/
{
    PBUFFER_DATA    bufferData;
    KIRQL           irql;

    NT_ASSERT(CommonBufferData != NULL);

    TRY {

        KeAcquireSpinLock(&CommonBufferData->CommonBufferLock, &irql);

        while (IsListEmpty(BufferList) == FALSE) {

            bufferData = (PBUFFER_DATA)RemoveHeadList(BufferList);

            bufferData->TrackingData.Owner = NULL;
            bufferData->TrackingData.Tag = 0;

            if (bufferData->Size == SMALL_SEGMENT_SIZE) {

                InsertHeadList(&CommonBufferData->SmallList, &bufferData->ListEntry);
                CommonBufferData->AvailableSmallListEntries++;

            } else if (bufferData->Size == LARGE_SEGMENT_SIZE) {

                InsertHeadList(&CommonBufferData->LargeList, &bufferData->ListEntry);
                CommonBufferData->AvailableLargeListEntries++;

            } else {

                LogWarning(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                           "Invalid Size %d", bufferData->Size);
            }
        }

        KeReleaseSpinLock(&CommonBufferData->CommonBufferLock, irql);

    } FINALLY {

    }

    return;
}

_Requires_lock_held_(CommonBufferData->CommonBufferLock)
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
CommonBuffer_RebalanceBuffers(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData,
    __in
        PLIST_ENTRY         SegmentHeaderList,
    __in
        PULONG              AvailableListEntries,
    __in
        PULONG              NumberOfListEntries,
    __in
        ULONG               ReclamationThreshold,
    __in
        ULONG               NumberOfSegments
    )
/*++

Routine Description:

    This function determines if buffers need to be freed, and if so, adds them to a
    rebalance list, which is later used to free the common buffer. The logic to
    determine if the buffer should be freed is if the number of available buffers
    exceeded a predefined maximum and the buffers have been on the available list
    for two iterations of rebalancing, then it is added to the rebalance list.

--*/
{
    PSEGMENT_HEADER segmentHeader;
    PSEGMENT_HEADER nextSegment;
    PBUFFER_DATA    bufferData;
    ULONG           n;

    //
    // Each segmented common buffer allocation has a single segment header. Walk the
    // list of segment headers to determine if the segment can be freed. We always
    // start at the beginning of the list and work to the end. In the case an entry
    // is removed, then the next entry is cached to prevent having to start at the
    // beginning of the list.
    //
    segmentHeader = (PSEGMENT_HEADER)SegmentHeaderList->Flink;

    while (segmentHeader != (PSEGMENT_HEADER)SegmentHeaderList) {

        if (*AvailableListEntries <= ReclamationThreshold) {
            break;
        }

        bufferData = (PBUFFER_DATA)(((PUCHAR)segmentHeader) + sizeof(SEGMENT_HEADER));

        //
        // If the buffer has been marked pending free, then remove it from the available
        // list and put it on a temp free list, where the entry is freed after the
        // lock is released.
        //
        if (segmentHeader->PendingFree) {

            LogVerbose(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                       "Free segment 0x%p", segmentHeader);

            for (n = 0; n < NumberOfSegments; n++) {

                RemoveEntryList(&bufferData[n].ListEntry);
                InitializeListHead(&bufferData[n].ListEntry);

                RemoveEntryList(&bufferData[n].TrackingData.ListEntry);
                InitializeListHead(&bufferData[n].TrackingData.ListEntry);
            }

            *AvailableListEntries -= NumberOfSegments;
            *NumberOfListEntries  -= NumberOfSegments;

            nextSegment = (PSEGMENT_HEADER)segmentHeader->ListEntry.Flink;
            RemoveEntryList(&segmentHeader->ListEntry);
            InsertTailList(&CommonBufferData->RebalanceResourcesList, &segmentHeader->ListEntry);
            CommonBufferData->PendingFreePageCount += 1;
            segmentHeader = nextSegment;

        } else {

            //
            // Check the owner tag in each buffer data to see if there is an owner. If
            // the owner is NULL, this means that the buffer data is on the available
            // list and is safe to free.
            //
            for (n = 0; n < NumberOfSegments; n++) {

                if (bufferData[n].TrackingData.Owner) {
                    break;
                }
            }

            //
            // Each buffer data in the segmented common buffer was on the available
            // list. This entry is safe to free. Mark it as pending free so in the
            // next iteration of rebalance, if the buffer isn't acquired, it will
            // be freed.
            //
            if (n == NumberOfSegments) {

               LogVerbose(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                          "Mark segment as pending free 0x%p", segmentHeader);

               segmentHeader->PendingFree = TRUE;

               CommonBufferData->Counter.PagesInUse -= 1;
            }

            segmentHeader = (PSEGMENT_HEADER)segmentHeader->ListEntry.Flink;
        }
    }

    return;
}

VOID
CommonBuffer_RebalanceResourcesWorkItem(
    _In_
        WDFWORKITEM WdfWorkItem
    )
/*++

Routine Description:

    This function frees all common buffer on the rebalance resource list.

--*/
{
    PCOMMON_BUFFER_DATA commonBufferData;
    KIRQL               irql;
    LIST_ENTRY          freeList;
    PSEGMENT_HEADER     segmentHeader;
    PBUFFER_DATA        bufferData;

    commonBufferData = GetCommonBufferData(WdfWorkItemGetParentObject(WdfWorkItem));

    InitializeListHead(&freeList);

    KeAcquireSpinLock(&commonBufferData->CommonBufferLock, &irql);

    //
    // Remove all the entries on the rebalance list to a local free list. The local
    // free list is used to free common buffer after the lock is released, since
    // common buffer must be deleted at PASSIVE.
    //
    APPEND_LIST(&freeList, &commonBufferData->RebalanceResourcesList);

    commonBufferData->Counter.FreeCount += 1;
    commonBufferData->Counter.PagesTotal -= commonBufferData->PendingFreePageCount;
    commonBufferData->PendingFreePageCount = 0;

    KeReleaseSpinLock(&commonBufferData->CommonBufferLock, irql);

    //
    // All the entries on the free list are safe to free. Use the WDFCOMMONBUFFER
    // object in the first buffer data to delete the object.
    //
    while (IsListEmpty(&freeList) == FALSE) {

        segmentHeader = (PSEGMENT_HEADER)RemoveHeadList(&freeList);
        InitializeListHead(&segmentHeader->ListEntry);

        bufferData = (PBUFFER_DATA)(((PUCHAR)segmentHeader) + sizeof(SEGMENT_HEADER));

        LogInfo(Controller_GetRecorderLog(commonBufferData->ControllerHandle), CommonBuffer,
                "Freeing WDFCOMMONBUFFER object 0x%p", bufferData->WdfCommonBuffer);

        WdfObjectDelete(bufferData->WdfCommonBuffer);
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
CommonBuffer_RebalanceResources(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    )
/*++

Routine Description:

    This function determines if common buffer resources need to be rebalanced. If
    there are resources to be rebalanced, then they are moved on a separate list
    and a work item is queued, since common buffer must be freed at PASSIVE.

--*/
{
    KIRQL   irql;
    BOOLEAN queueWorkItem;

    //
    // If the work item exists, then see if common buffer resources need to be rebalanced.
    // If the work item doesn't exist, then resources will never rebalance, but this
    // isn't a fatal error, just an inefficiency since the driver can have large amounts
    // of common buffer that won't be freed until the device unloads.
    //
    if (CommonBufferData->RebalanceResourcesWorkItem) {

        queueWorkItem = FALSE;

        KeAcquireSpinLock(&CommonBufferData->CommonBufferLock, &irql);

        LogVerbose(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                   "Rebalancing small buffers start: available %d",
                   CommonBufferData->AvailableSmallListEntries);

        CommonBuffer_RebalanceBuffers(CommonBufferData,
                                      &CommonBufferData->SmallSegmentHeaderList,
                                      &CommonBufferData->AvailableSmallListEntries,
                                      &CommonBufferData->NumberOfSmallListEntries,
                                      CommonBufferData->Policy.SmallSegmentReclamationThreshold,
                                      SMALL_SEGMENTS_PER_PAGE);

        LogVerbose(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                   "Rebalancing small buffers finish: available %d",
                   CommonBufferData->AvailableSmallListEntries);

        //
        // Rebalance the large buffer list.
        //
        LogVerbose(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                   "Rebalancing large buffers start: available %d",
                   CommonBufferData->AvailableLargeListEntries);

        CommonBuffer_RebalanceBuffers(CommonBufferData,
                                      &CommonBufferData->LargeSegmentHeaderList,
                                      &CommonBufferData->AvailableLargeListEntries,
                                      &CommonBufferData->NumberOfLargeListEntries,
                                      CommonBufferData->Policy.LargeSegmentReclamationThreshold,
                                      LARGE_SEGMENTS_PER_PAGE);

        LogVerbose(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                   "Rebalancing large buffers finish: available %d",
                   CommonBufferData->AvailableLargeListEntries);

        //
        // If there are resources on the rebalance list, then a work item needs to be
        // queued in order to free these resources.
        //
        if (IsListEmpty(&CommonBufferData->RebalanceResourcesList) == FALSE) {
            queueWorkItem = TRUE;
        }

        KeReleaseSpinLock(&CommonBufferData->CommonBufferLock, irql);

        if (queueWorkItem) {
            WdfWorkItemEnqueue(CommonBufferData->RebalanceResourcesWorkItem);
        }
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
CommonBuffer_ControllerResetPostReset(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    )
/*

Routine Description:

    The Controller object calls this routine after it has reset the controller
    due to an unrecoverable hardware error. This routine will ensure that any
    work items queued by this object have completed running.

*/
{
    CommonBuffer_FlushWorkItems(CommonBufferData);
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
CommonBuffer_ControllerGone(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    )
/*

Routine Description:

    The Controller object calls this routine after it has detected that the
    controller has been physically removed. This routine will ensure that any
    work items queued by this object have completed running.

*/
{
    CommonBuffer_FlushWorkItems(CommonBufferData);
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
CommonBuffer_FlushWorkItems(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    )
/*

Routine Description:

    This routine ensures that work items owned by the CommonBuffer are flushed
    before it returns.

*/
{
    LARGE_INTEGER   timeOut;
    NTSTATUS        status;

    TRY {

        //
        // Wait for the IOWORKITEM that allocates CommonBuffer, if one was queued.
        //
        timeOut.QuadPart = WDF_REL_TIMEOUT_IN_SEC(60);

        do {

            status = KeWaitForSingleObject(&CommonBufferData->WorkItemEvent,
                                           Executive,
                                           KernelMode,
                                           FALSE,
                                           &timeOut);

            if (status == STATUS_TIMEOUT) {

                LogInfo(Controller_GetRecorderLog(CommonBufferData->ControllerHandle), CommonBuffer,
                        "Waiting on WorkItem completion");

            } else  {

                NT_ASSERT(NT_SUCCESS(status));
                break;
            }

        } WHILE (TRUE);

        WdfWorkItemFlush(CommonBufferData->RebalanceResourcesWorkItem);

    } FINALLY {

    }

    return;
}
