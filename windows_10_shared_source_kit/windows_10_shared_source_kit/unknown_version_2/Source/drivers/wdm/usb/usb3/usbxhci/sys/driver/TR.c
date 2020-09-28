
/*++

Copyright (c) Microsoft Corporation

Module Name:

    TR.c

Abstract:

    USB XHCI Driver

--*/

#include "pch.h"
#include "TR.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, TR_Create)
#endif

const CHAR TransferRingTag[] = "Transfer Ring Tag";

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
TR_Create(
    __in
        HANDLE    ControllerHandle,
    __in
        HANDLE    EndpointHandle,
    __in
        WDFOBJECT ParentWdfObject,
    __in
        ULONG     StreamId,
    __out
        PHANDLE   TransferRingHandle
    )
{
    NTSTATUS                status;
    WDF_DPC_CONFIG          wdfDpcConfig;
    WDF_OBJECT_ATTRIBUTES   wdfAttributes;
    WDF_IO_QUEUE_CONFIG     wdfIoQueueConfig;
    WDFQUEUE                wdfQueue;
    PTR_DATA                trData;
    TR_CONFIG               trConfig;

    PAGED_CODE();

    TRY {

        RtlZeroMemory(&trConfig, sizeof(TR_CONFIG));

        trConfig.MaxStageSize =
            CommonBuffer_GetMaxDmaSize(Controller_GetCommonBufferHandle(ControllerHandle)) - PAGE_SIZE;

        trConfig.MapRegisterCount =
            CommonBuffer_GetMapRegisterCount(Controller_GetCommonBufferHandle(ControllerHandle));

        trConfig.TransferType = Endpoint_GetTransferType(EndpointHandle);

        switch (trConfig.TransferType) {

        case USB_ENDPOINT_TYPE_CONTROL:
            Control_GetConfiguration(ControllerHandle, &trConfig);
            break;

        case USB_ENDPOINT_TYPE_BULK:
        case USB_ENDPOINT_TYPE_INTERRUPT:
            Bulk_GetConfiguration(ControllerHandle, &trConfig);
            break;

        case USB_ENDPOINT_TYPE_ISOCHRONOUS:
            Isoch_GetConfiguration(ControllerHandle, &trConfig);
            break;

        default:
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        WDF_IO_QUEUE_CONFIG_INIT(&wdfIoQueueConfig, trConfig.QueueDispatchType);
        wdfIoQueueConfig.EvtIoDefault         = trConfig.FunctionTable->WDF_EvtIoDefault;
        wdfIoQueueConfig.EvtIoCanceledOnQueue = trConfig.FunctionTable->WDF_EvtIoCanceledOnQueue;

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, TR_DATA);
        wdfAttributes.ContextSizeOverride = trConfig.ContextSize;
        wdfAttributes.EvtCleanupCallback  = TR_WdfEvtCleanupCallback;
        wdfAttributes.ParentObject        = ParentWdfObject;

        //
        // This queue handles USB bus traffic of downstream USB devices. Upon USB devices exiting
        // D0, Usbhub3 and UCX guarantee cancellation of this traffic, therefore the queue doesn't
        // need an EvtIoStop callback.
        //
        status = WdfIoQueueCreate(Controller_GetWdfDeviceObject(ControllerHandle),
                                  &wdfIoQueueConfig,
                                  &wdfAttributes,
                                  &wdfQueue);

        if (!NT_SUCCESS(status)) {
            LogError(Controller_GetRecorderLog(ControllerHandle), TransferRing,
                     "%u.%u.%u: WdfIoQueueCreate Failed %!STATUS!",
                     UsbDevice_GetSlotId(Endpoint_GetUsbDeviceHandle(EndpointHandle)),
                     Endpoint_GetContextIndex(EndpointHandle), StreamId, status);
            LEAVE;
        }

        trData = GetTRData(wdfQueue);

        RtlCopyMemory(trData, &trConfig, sizeof(TR_CONFIG));

        trData->ControllerHandle  = ControllerHandle;
        trData->UsbDeviceHandle   = Endpoint_GetUsbDeviceHandle(EndpointHandle);
        trData->EndpointHandle    = EndpointHandle;
        trData->StreamId          = StreamId;
        trData->WdfQueue          = wdfQueue;

        KeInitializeSpinLock(&trData->Lock);

        InitializeListHead(&trData->SegmentList);
        InitializeListHead(&trData->DoubleBufferDataList);

        if (trConfig.FunctionTable->WDF_EvtIoQueueReadyNotification) {

            status = WdfIoQueueReadyNotify(wdfQueue,
                                           trConfig.FunctionTable->WDF_EvtIoQueueReadyNotification,
                                           (WDFCONTEXT)trData);

            if (!NT_SUCCESS(status)) {
                LogError(Controller_GetRecorderLog(ControllerHandle), TransferRing,
                         "%u.%u.%u: WdfIoQueueReadyNotify Failed %!STATUS!",
                         UsbDevice_GetSlotId(Endpoint_GetUsbDeviceHandle(EndpointHandle)),
                         Endpoint_GetContextIndex(EndpointHandle), StreamId, status);
                LEAVE;
            }
        }

        //
        // DPC used for completion of cancelled transfers.
        //
        WDF_DPC_CONFIG_INIT(&wdfDpcConfig, trConfig.FunctionTable->WDF_EvtDpcForTransferCompletion);

        WDF_OBJECT_ATTRIBUTES_INIT(&wdfAttributes);
        wdfAttributes.ParentObject = wdfQueue;

        status = WdfDpcCreate(&wdfDpcConfig, &wdfAttributes, &trData->WdfDpcForTransferCompletion);

        if (!NT_SUCCESS(status)) {

            LogError(Controller_GetRecorderLog(ControllerHandle), TransferRing,
                     "%u.%u.%u: WdfDpcCreate Failed %!STATUS!",
                     UsbDevice_GetSlotId(Endpoint_GetUsbDeviceHandle(EndpointHandle)),
                     Endpoint_GetContextIndex(EndpointHandle), StreamId, status);
            LEAVE;
        }

        //
        // All TransferRing types start out with 1 segment. Atleast one segment
        // needs to be setup since sometime after this point, a DequeuePointer value
        // of a segment needs to be provided to the controller in a configure endpoint
        // command.
        //
        // N.B: Allocation of this segment is done in Create callback and not in the
        //      Enable callback because Enable callback can come in at Dispatch and
        //      needs to complete synchronously. If it does come in at Dispatch, and
        //      if the CommonBuffer object does not have enough pre-allocated segments,
        //      the Enable call would have to be failed synchronously, even though the
        //      CommonBuffer object is going to allocate a work item to allocate more
        //      CommonBuffer. By attempting the segment allocation here at PASSIVE, we
        //      reduce the chance of Enable failing.
        //
        status = TR_EnsureSegments(trData, 1, FALSE);

        if (!NT_SUCCESS(status)) {
            LogError(Controller_GetRecorderLog(ControllerHandle), TransferRing,
                     "%u.%u.%u: TR_EnsureSegments Failed %!STATUS!",
                     UsbDevice_GetSlotId(trData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(EndpointHandle), StreamId, status);
            LEAVE;
        }

        trData->CurrentSegmentBufferData = TR_AcquireSegment(trData);

        status = trConfig.FunctionTable->TR_Initialize(trData);

        if (!NT_SUCCESS(status)) {
            LogError(Controller_GetRecorderLog(ControllerHandle), TransferRing,
                     "%u.%u.%u: TR_Initialize Failed %!STATUS!",
                     UsbDevice_GetSlotId(Endpoint_GetUsbDeviceHandle(EndpointHandle)),
                     Endpoint_GetContextIndex(EndpointHandle), StreamId, status);
            LEAVE;
        }

        Counter_CreateTransferRingInstance(trData->EndpointHandle,
                                           trData->StreamId,
                                           &trData->Counter);

        *TransferRingHandle = trData;

    } FINALLY {

    }

    return status;
}

VOID
TR_WdfEvtCleanupCallback(
    WDFOBJECT WdfQueue
    )
{
    PTR_DATA trData;

    trData = GetTRData(WdfQueue);

    NT_ASSERT(IsListEmpty(&trData->SegmentList));
    NT_ASSERT(IsListEmpty(&trData->DoubleBufferDataList));

    if (trData->CurrentSegmentBufferData != NULL) {

        CommonBuffer_ReleaseBuffer(Controller_GetCommonBufferHandle(trData->ControllerHandle),
                                   trData->CurrentSegmentBufferData);

        trData->CurrentSegmentBufferData = NULL;
    }

    Counter_CloseTransferRingInstance(&trData->Counter);

    trData->FunctionTable->TR_Cleanup(trData);
}

__drv_maxIRQL(DISPATCH_LEVEL)
HANDLE
TR_GetContext(
    __in
        WDFOBJECT WdfQueue
    )
{
    return GetTRData(WdfQueue);
}

#pragma prefast(push)
#pragma prefast(disable:28167, "Changes the IRQL and does not restore the IRQL before it exits.")

_Requires_lock_not_held_(((PTR_DATA)Context)->Lock)
_Acquires_lock_(((PTR_DATA)Context)->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_Lock(
    __in
        HANDLE Context
    )
{
    KIRQL       irql;
    PTR_DATA    trData;

    trData = (PTR_DATA)Context;

    KeAcquireSpinLock(&trData->Lock, &irql);

    trData->SavedIrql = irql;
}

_Requires_lock_held_(((PTR_DATA)Context)->Lock)
_Releases_lock_(((PTR_DATA)Context)->Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
TR_Unlock(
    __in
        HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    KeReleaseSpinLock(&trData->Lock, trData->SavedIrql);
}

_Requires_lock_not_held_(((PTR_DATA)Context)->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
TR_EnsureSegments(
    __in
        HANDLE                  Context,
    __in
        ULONG                   SegmentsRequired,
    __in
        BOOLEAN                 Async
    )
{
    PBUFFER_DATA                    bufferData;
    PCOMMON_BUFFER_CALLBACK_DATA    callbackData;
    HANDLE                          commonBufferHandle;
    ULONG                           segmentsAvailable;
    LIST_ENTRY                      localSegmentList;
    NTSTATUS                        status;
    PTR_DATA                        trData;

    TRY {

        trData             = (PTR_DATA)Context;
        segmentsAvailable  = 0;
        commonBufferHandle = Controller_GetCommonBufferHandle(trData->ControllerHandle);

        TR_Lock(trData);

        FOR_ALL_IN_LIST(BUFFER_DATA, &trData->SegmentList, ListEntry, bufferData) {

            segmentsAvailable += 1;
        }

        TR_Unlock(trData);

        LogVerbose(Controller_GetRecorderLog(trData->ControllerHandle), TransferRing,
                   "%u.%u.%u: Segments available %u, Segments required %u",
                   UsbDevice_GetSlotId(trData->UsbDeviceHandle),
                   Endpoint_GetContextIndex(trData->EndpointHandle),
                   trData->StreamId, segmentsAvailable, SegmentsRequired);

        if (SegmentsRequired <= segmentsAvailable) {
            status = STATUS_SUCCESS;
            LEAVE;
        }

        SegmentsRequired -= segmentsAvailable;

        InitializeListHead(&localSegmentList);

        if (Async) {

            callbackData = &trData->CommonBufferCallbackData;

            callbackData->Owner           = trData;
            callbackData->Callback        = TR_CommonBufferCallback;
            callbackData->NumberToAcquire = SegmentsRequired;
            callbackData->BufferSize      = trData->SegmentSize;
            callbackData->Tag             = TRANSFERRING_ALLOC_TAG_TRANSFER_SEGMENT;

        } else {

            callbackData = NULL;
        }

        status = CommonBuffer_AcquireBuffers(commonBufferHandle,
                                             SegmentsRequired,
                                             trData->SegmentSize,
                                             &localSegmentList,
                                             trData,
                                             TRANSFERRING_ALLOC_TAG_TRANSFER_SEGMENT,
                                             callbackData);

        if (!NT_SUCCESS(status)) {
            LogError(Controller_GetRecorderLog(trData->ControllerHandle), TransferRing,
                     "%u.%u.%u: Failed to acquire %u segment buffers",
                     UsbDevice_GetSlotId(trData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(trData->EndpointHandle),
                     trData->StreamId, SegmentsRequired);
            LEAVE;
        }

        TR_Lock(trData);

        APPEND_LIST(&trData->SegmentList, &localSegmentList);

        TR_Unlock(trData);

    } FINALLY {

    }

    return status;
}

_Requires_lock_not_held_(((PTR_DATA)Context)->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
PBUFFER_DATA
TR_AcquireSegment(
    __in
        HANDLE Context
    )
{
    PBUFFER_DATA    segment;
    PTR_DATA        trData;

    trData = (PTR_DATA)Context;

    TR_Lock(trData);

    if (IsListEmpty(&trData->SegmentList)) {
        segment = NULL;
    } else {
        segment = (PBUFFER_DATA)RemoveHeadList(&trData->SegmentList);
        InitializeListHead(&segment->ListEntry);
    }

    TR_Unlock(trData);

    return segment;
}

_Requires_lock_not_held_(((PTR_DATA)Context)->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_ReleaseSegment(
    __in
        HANDLE          Context,
    __in
        PBUFFER_DATA    Segment
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    TR_Lock(trData);

    InsertTailList(&trData->SegmentList, &Segment->ListEntry);

    TR_Unlock(trData);

    return;
}

_Requires_lock_not_held_(((PTR_DATA)Context)->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
TR_AcquireSegments(
    __in
        HANDLE      Context,
    __in
        ULONG       SegmentCount,
    __inout
        PLIST_ENTRY SegmentList
    )
{
    ULONG           i;
    PBUFFER_DATA    segment;
    NTSTATUS        status;
    PTR_DATA        trData;

    TRY {

        trData = (PTR_DATA)Context;

        status = TR_EnsureSegments(trData, SegmentCount, TRUE);

        if ((status == STATUS_PENDING) || !NT_SUCCESS(status)) {
            LEAVE;
        }

        TR_Lock(trData);

        //
        // TODO: Optimize. Grab a bunch instead of popping one by one.
        //
        for (i = 0; i < SegmentCount; i += 1) {
            segment = (PBUFFER_DATA)RemoveHeadList(&trData->SegmentList);
            InsertHeadList(SegmentList, &segment->ListEntry);
        }

        TR_Unlock(trData);

    } FINALLY {

    }

    return status;
}

_Requires_lock_held_(((PTR_DATA)Context)->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_ReleaseSegments(
    __in
        HANDLE          Context,
    __inout
        PLIST_ENTRY     SegmentList,
    __in
        BOOLEAN         ReleaseToHeadOfList
    )
{
    PBUFFER_DATA    segment;
    PTR_DATA        trData;

    TRY {

        trData = (PTR_DATA)Context;

        if (IsListEmpty(SegmentList)) {
            LEAVE;
        }

        if (Controller_GetDeviceFlags(trData->ControllerHandle).ZeroTRBSegments) {

            FOR_ALL_IN_LIST(BUFFER_DATA, SegmentList, ListEntry, segment) {

                RtlZeroMemory(segment->VirtualAddress, segment->Size);
            }
        }

        if (ReleaseToHeadOfList) {
            PREPEND_LIST(&trData->SegmentList, SegmentList);
        } else {
            APPEND_LIST(&trData->SegmentList, SegmentList);
        }

    } FINALLY {

    }

    return;
}

_Requires_lock_not_held_(((PTR_DATA)Context)->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
PBUFFER_DATA
TR_AcquireDoubleBuffer(
    __in
        HANDLE Context
    )
{
    HANDLE          commonBufferHandle;
    PBUFFER_DATA    segment;
    PTR_DATA        trData;

    trData = (PTR_DATA)Context;

    TR_Lock(trData);

    if (IsListEmpty(&trData->DoubleBufferDataList)) {

        commonBufferHandle = Controller_GetCommonBufferHandle(trData->ControllerHandle);

        segment = CommonBuffer_AcquireBuffer(commonBufferHandle,
                                             trData->DoubleBufferSize,
                                             trData,
                                             TRANSFERRING_ALLOC_TAG_DOUBLEBUFFER);

    } else {

        segment = (PBUFFER_DATA)RemoveHeadList(&trData->DoubleBufferDataList);
        InitializeListHead(&segment->ListEntry);
    }

    TR_Unlock(trData);

    return segment;
}

_Requires_lock_not_held_(((PTR_DATA)Context)->Lock)
VOID
TR_ReleaseDoubleBuffer(
    __in
        HANDLE          Context,
    __in
        PBUFFER_DATA    Segment
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    TR_Lock(trData);

    InsertTailList(&trData->DoubleBufferDataList, &Segment->ListEntry);

    TR_Unlock(trData);

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_CommonBufferCallback(
    __in
        PCOMMON_BUFFER_CALLBACK_DATA    CommonBufferCallbackData,
    __in
        NTSTATUS                        Status,
    __in
        PLIST_ENTRY                     BufferList
    )
{
    PTR_DATA trData;

    trData = CommonBufferCallbackData->Owner;

    if (NT_SUCCESS(Status)) {

        TR_Lock(trData);

        PREPEND_LIST(&trData->SegmentList, BufferList);

        TR_Unlock(trData);
    }

    trData->FunctionTable->TR_CommonBufferCallback(trData, Status);
}

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG
TR_GetPacketCount(
    __in
        HANDLE Context,
    __in
        ULONG  BufferSize
    )
/*++

Routine Description:

    Returns the number of packets spanned by a buffer of the specified size.

--*/
{
    ULONG       packetCount;
    PTR_DATA    trData;

    trData = (PTR_DATA)Context;

    if (Controller_GetXhciMajorVersion(trData->ControllerHandle) >= XHCI_MAJOR_VERSION_1) {

        packetCount = (BufferSize + (Endpoint_GetMaxPacketSize(trData->EndpointHandle) - 1)) /
                      Endpoint_GetMaxPacketSize(trData->EndpointHandle);

    } else {

        packetCount = 0;
    }

    return packetCount;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_InitializeLinkTrb(
    __in
        HANDLE      Context,
    __in
        TRB_STAGE   TrbStage,
    __out
        PTRB        LinkTrb,
    __in
        BOOLEAN     FirstTrb
    )
/*++

Routine Description:

    This function initializes certain fields of the Link TRB for all TransferRing types:

    Caller is responsible for initializing these fields:
        - RingSegmentPointer
        - ToggleCycle

--*/
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    RtlZeroMemory(LinkTrb, sizeof(TRB));

    LinkTrb->Link.Type = TRB_TYPE_LINK;

    //
    // Explicitly set the CycleBit to valid or invalid depending on whether this
    // is the first TRB of the transfer. CycleBit for the first TRB must be kept
    // invalid and toggled after programming the rest of the TRBs, to keep the
    // controller from seeing TRBs in a transient state. This also satisfies the
    // assumption of some controllers that the Link TRB always points to a valid TRB.
    //
    if (FirstTrb) {
        LinkTrb->Link.CycleBit = INVERT(trData->CycleState);
    } else {
        LinkTrb->Link.CycleBit = trData->CycleState;
    }

    //
    // The chain bit needs to be set if the Link TRB falls within a TD.
    //
    if (Controller_GetDeviceFlags(trData->ControllerHandle).SetLinkTRBChainBit) {

        LinkTrb->Link.ChainBit = 1;

    } else if (FirstTrb) {

        LinkTrb->Link.ChainBit = 0;

    } else {

        switch (TrbStage) {

            case TrbStage_Normal:
            case TrbStage_EventData:
            case TrbStage_FinalEventData:

                LinkTrb->Link.ChainBit = 1;
                break;

            default:
                LinkTrb->Link.ChainBit = 0;
                break;
        }
    }

    LinkTrb->Link.InterrupterTarget = trData->InterrupterTarget;

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG
TR_GetCycleState(
    __in
        HANDLE Context
    )
{
    return ((PTR_DATA)Context)->CycleState;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_ToggleCycleState(
    __in
        HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    trData->CycleState = INVERT(trData->CycleState);
}

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG
TR_CalculateTDSize(
    __in
        HANDLE  Context,
    __in
        ULONG   TdPacketCount,
    __in
        ULONG   LengthMapped,
    __in
        BOOLEAN FinalTrb
    )
/*++

Routine Description:

    Calculate the TD Size based on the logic in XHCI 1.0 section 4.11.2.4.
    This is only done for xHCI 1.0 and greater. For 0.96, the TD size
    is not calculated and is set to zero.

--*/
{
    ULONG       packetsTransferred;
    ULONG       tdSize;
    PTR_DATA    trData;

    trData = (PTR_DATA)Context;

    if (Controller_GetXhciMajorVersion(trData->ControllerHandle) >= XHCI_MAJOR_VERSION_1) {

        if (FinalTrb) {

            tdSize = 0;

        } else {

            packetsTransferred = LengthMapped / Endpoint_GetMaxPacketSize(trData->EndpointHandle);

            if ((TdPacketCount - packetsTransferred) > 31) {
                tdSize = 31;
            } else {
                tdSize = TdPacketCount - packetsTransferred;
            }
       }

    } else {

        tdSize = 0;
    }

    return tdSize;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_QueueDpcForTransferCompletion(
    __in
        HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    WdfDpcEnqueue(trData->WdfDpcForTransferCompletion);
}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
TR_Enable_Internal(
    __in
        PTR_DATA TRData
    )
{
    NTSTATUS status;

    TRY {

        NT_ASSERT(TRData->CurrentSegmentBufferData != NULL);
        NT_ASSERT(TRData->MappingState == MS_Stopped);

        TR_InitializeTransferRing(TRData);

        status = TRData->FunctionTable->EP_Enable(TRData);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

    } FINALLY {

        LogInfo(Controller_GetRecorderLog(TRData->ControllerHandle), TransferRing,
                "%u.%u.0: UcxEndpoint 0x%p %!STATUS!",
                UsbDevice_GetSlotId(TRData->UsbDeviceHandle),
                Endpoint_GetContextIndex(TRData->EndpointHandle),
                Endpoint_GetUcxEndpoint(TRData->EndpointHandle), status);
    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_Disable_Internal(
    __in
        PTR_DATA    TRData,
    __in
        BOOLEAN     FreeResources
    )
{
    HANDLE          commonBufferHandle;
    MAPPING_STATE   previousState;

    TRY {

        commonBufferHandle = Controller_GetCommonBufferHandle(TRData->ControllerHandle);

        TRData->FunctionTable->EP_Disable(TRData, FreeResources);

        //
        // Maintain the invariant that TransferRing state is Stopped untile
        // the TransferRing is enabled.
        //
        previousState =
            (MAPPING_STATE)InterlockedExchange((volatile LONG*)&TRData->MappingState,
                                               MS_Stopped);

        NT_ASSERT_ASSUME((previousState == MS_Stopped) || (previousState == MS_Paused));

        if (FreeResources) {

            if (IsListEmpty(&TRData->SegmentList) == FALSE) {
                CommonBuffer_ReleaseBuffers(commonBufferHandle, &TRData->SegmentList);
            }

            if (IsListEmpty(&TRData->DoubleBufferDataList) == FALSE) {
                CommonBuffer_ReleaseBuffers(commonBufferHandle, &TRData->DoubleBufferDataList);
            }

            if (TRData->ForwardProgressMdl != NULL) {
                IoFreeMdl(TRData->ForwardProgressMdl);
                TRData->ForwardProgressMdl = NULL;
            }
        }

    } FINALLY {

    }
}

WDF_IO_FORWARD_PROGRESS_ACTION
TR_WdfEvtIoExamineIrpForForwardProgress(
    WDFQUEUE    WdfQueue,
    PIRP        Irp
    )
/*++

Routine Description:

    Notify WDF if the reserved request needs to be used for this irp. This is
    done by checking if the forward progress flag is set within the URB.

--*/
{
    PIO_STACK_LOCATION              irpStack;
    PTRANSFER_URB                   transferUrb;
    WDF_IO_FORWARD_PROGRESS_ACTION  wdfIoForwardProgressAction;

    UNREFERENCED_PARAMETER(WdfQueue);

    irpStack    = IoGetCurrentIrpStackLocation(Irp);
    transferUrb = irpStack->Parameters.Others.Argument1;

    if (transferUrb->TransferFlags & USBD_FORWARD_PROGRESS_TRANSFER) {
        wdfIoForwardProgressAction = WdfIoForwardProgressActionUseReservedRequest;
    } else {
        wdfIoForwardProgressAction = WdfIoForwardProgressActionFailRequest;
    }

    return wdfIoForwardProgressAction;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
TR_EnableForwardProgress_Internal(
    __in
        PTR_DATA    trData,
    __in
        ULONG       MaxTransferSize
    )
{
    NTSTATUS                                status;
    PMDL                                    mdl;
    WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY    wdfQueueForwardProgressPolicy;

    TRY {

        mdl = NULL;

        status = trData->FunctionTable->EP_EnableForwardProgress(trData, MaxTransferSize);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // Pre-allocate an MDL big enough to handle one transfer.
        //
        mdl = IoAllocateMdl(NULL, MaxTransferSize, FALSE, FALSE, NULL);

        if (mdl == NULL) {

            LogError(Controller_GetRecorderLog(trData->ControllerHandle), TransferRing,
                     "%u.%u.0: Failed to allocate ForwardProgressMdl",
                     UsbDevice_GetSlotId(trData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(trData->EndpointHandle));

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        //
        // Enable forward progress on the queue.
        //
        WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY_EXAMINE_INIT(
            &wdfQueueForwardProgressPolicy,
            1,
            TR_WdfEvtIoExamineIrpForForwardProgress);

        status = WdfIoQueueAssignForwardProgressPolicy(trData->WdfQueue,
                                                       &wdfQueueForwardProgressPolicy);

        if (!NT_SUCCESS(status)) {
            LogError(Controller_GetRecorderLog(trData->ControllerHandle), TransferRing,
                     "%u.%u.0: WdfIoQueueAssignForwardProgressPolicy Failed %!STATUS!",
                     UsbDevice_GetSlotId(trData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(trData->EndpointHandle), status);
            LEAVE;
        }

    } FINALLY {

        if (NT_SUCCESS(status)) {

            //
            // This function can get called multiple times if the MaxTransferSize changes.
            // So free the previously allocated MDL, if one exists.
            //
            if (trData->ForwardProgressMdl != NULL) {
                IoFreeMdl(trData->ForwardProgressMdl);
            }
            trData->ForwardProgressMdl = mdl;

            LogInfo(Controller_GetRecorderLog(trData->ControllerHandle), TransferRing,
                    "%u.%u.0: Forward progress enabled for MaxTransferSize %u",
                    UsbDevice_GetSlotId(trData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(trData->EndpointHandle), MaxTransferSize);

        } else {

            if (mdl) {
                IoFreeMdl(mdl);
            }
        }
    }

    return status;
}

//
// Trampoline functions
//

_Use_decl_annotations_
NTSTATUS
TR_Enable(
    HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    return TR_Enable_Internal(trData);
}

_Use_decl_annotations_
VOID
TR_Disable(
    HANDLE  Context,
    BOOLEAN FreeResources
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    TR_Disable_Internal(trData, FreeResources);
}

_Use_decl_annotations_
NTSTATUS
TR_EnableForwardProgress(
    HANDLE Context,
    ULONG  MaxTransferSize
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    return TR_EnableForwardProgress_Internal(trData, MaxTransferSize);
}

_Use_decl_annotations_
BOOLEAN
TR_AreTransfersPending(
    HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    return trData->FunctionTable->EP_AreTransfersPending(trData);
}

_Use_decl_annotations_
VOID
TR_OkToReclaimTransfersOnCancel(
    HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    trData->FunctionTable->EP_OkToReclaimTransfersOnCancel(trData);
}

_Use_decl_annotations_
VOID
TR_ReclaimTransfers(
    HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    //
    // We are required to call into the Endpoint object after reclaiming transfers.
    // But as soon as we complete the last outstanding transfer, the Endpoint object
    // might get deleted. Take a reference to ensure that the Endpoint object sticks
    // around until we acknowledge the completion of outstanding transfers.
    //
    WdfObjectReferenceWithTag(WdfObjectContextGetObject(trData->EndpointHandle),
                              (PVOID)TransferRingTag);

    trData->FunctionTable->EP_ReclaimTransfers(trData);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_TransfersReclaimed(
    __in
        HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    Endpoint_TR_TransfersReclaimed(trData->EndpointHandle);

    WdfObjectDereferenceWithTag(WdfObjectContextGetObject(trData->EndpointHandle),
                              (PVOID)TransferRingTag);
}

_Use_decl_annotations_
BOOLEAN
TR_IsTransferEventLikelyDuplicate(
    HANDLE              Context,
    PHYSICAL_ADDRESS    DequeuePointer,
    PBOOLEAN            PointsToNoOpTRB
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    return trData->FunctionTable->EP_IsTransferEventLikelyDuplicate(trData, DequeuePointer, PointsToNoOpTRB);
}

_Use_decl_annotations_
BOOLEAN
TR_TransferEventHandler(
    PTRANSFER_EVENT_TRB Trb,
    HANDLE              Context
    )
{
    PTR_DATA            trData;
    PTD_TRACKING_DATA   tdTrackingData;

    trData = (PTR_DATA)Context;

    if (trData != NULL) {

        return trData->FunctionTable->EP_TransferEventHandler(Trb, trData);

    } else {

        tdTrackingData = (PTD_TRACKING_DATA)&Trb->Pointer.QuadPart;

        switch (tdTrackingData->EndpointType) {

        case USB_ENDPOINT_TYPE_CONTROL:
            return Control_EP_TransferEventHandler(Trb, NULL);
            break;

        case USB_ENDPOINT_TYPE_BULK:
        case USB_ENDPOINT_TYPE_INTERRUPT:
            return Bulk_EP_TransferEventHandler(Trb, NULL);
            break;

        case USB_ENDPOINT_TYPE_ISOCHRONOUS:
        default:
            return FALSE;
            break;
        }
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
WDFQUEUE
TR_GetWdfQueue(
    __in
        PTR_DATA TRData
    )
{
    return TRData->WdfQueue;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_InitializeTransferRing(
    __in
        PTR_DATA TRData
    )
{
    RtlZeroMemory(TRData->CurrentSegmentBufferData->VirtualAddress,
                  TRData->CurrentSegmentBufferData->Size);

    TRData->CurrentSegment      = (PTRB)TRData->CurrentSegmentBufferData->VirtualAddress;
    TRData->CurrentSegmentIndex = 0;
    TRData->MaxSegmentIndex     = ((TRData->CurrentSegmentBufferData->Size / sizeof(TRB)) - 1);
    TRData->CycleState          = DEFAULT_TRANSFERRING_CYCLE_STATE;
}

__drv_maxIRQL(DISPATCH_LEVEL)
LONG64
TR_GetCurrentSegmentLA(
    __in
        PTR_DATA TRData
    )
{
    return TRData->CurrentSegmentBufferData->LogicalAddress.QuadPart;
}

__drv_maxIRQL(DISPATCH_LEVEL)
LONG64
TR_GetEnqueuePointerLA(
    __in
        PTR_DATA TRData
    )
{
    return (TRData->CurrentSegmentBufferData->LogicalAddress.QuadPart +
            (TRData->CurrentSegmentIndex * sizeof(TRB)));
}

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG64
TR_GetDequeuePointer(
    __in
        PTR_DATA TRData
    )
{
    TRANSFERRING_DEQUEUE_POINTER trDequeuePointer;

    trDequeuePointer.DequeuePointer    = TR_GetEnqueuePointerLA(TRData);
    trDequeuePointer.DequeueCycleState = TRData->CycleState;

    if (TRData->StreamId != 0) {
        trDequeuePointer.StreamContextType = STREAM_CONTEXT_TYPE_PRIMARY_TR;
    }

    return trDequeuePointer.DequeuePointer;
}

__drv_maxIRQL(DISPATCH_LEVEL)
USBD_STATUS
TR_GetUsbdStatusFromTrbCompletionCode(
    __in
        ULONG       TrbCompletionCode,
    __in
        USBD_STATUS UsbdStatus
    )
{
    USBD_STATUS usbdStatus;

    switch (TrbCompletionCode) {

    case TRB_COMPLETION_CODE_INVALID:
        usbdStatus = UsbdStatus;
        break;

    case TRB_COMPLETION_CODE_SUCCESS:
    case TRB_COMPLETION_CODE_SHORT_PACKET:
    case TRB_COMPLETION_CODE_STOPPED:
    case TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET:

        usbdStatus = USBD_STATUS_SUCCESS;
        break;

    case TRB_COMPLETION_CODE_DATA_BUFFER_ERROR:

        usbdStatus = USBD_STATUS_DATA_BUFFER_ERROR;
        break;

    case TRB_COMPLETION_CODE_ISOCH_BUFFER_OVERRUN:
    case TRB_COMPLETION_CODE_BABBLE_DETECTED_ERROR:

        usbdStatus = USBD_STATUS_BABBLE_DETECTED;
        break;

    case TRB_COMPLETION_CODE_STALL_ERROR:

        usbdStatus = USBD_STATUS_STALL_PID;
        break;

    case TRB_COMPLETION_CODE_USB_TRANSACTION_ERROR:
    case TRB_COMPLETION_CODE_SPLIT_TRANSACTION_ERROR:

        usbdStatus = USBD_STATUS_XACT_ERROR;
        break;

    case TRB_COMPLETION_CODE_MISSED_SERVICE_ERROR:

        usbdStatus = USBD_STATUS_ISO_TD_ERROR;
        break;

    case TRB_COMPLETION_CODE_NO_PING_RESPONSE_ERROR:

        usbdStatus = USBD_STATUS_NO_PING_RESPONSE;
        break;

    case TRB_COMPLETION_CODE_STOPPED_LENGTH_INVALID:
        usbdStatus = USBD_STATUS_ISO_NOT_ACCESSED_BY_HW;
        break;

    case TRB_COMPLETION_CODE_INVALID_STREAM_TYPE_ERROR:

        usbdStatus = USBD_STATUS_INVALID_STREAM_TYPE;
        break;

    case TRB_COMPLETION_CODE_INVALID_STREAM_ID_ERROR:

        usbdStatus = USBD_STATUS_INVALID_STREAM_ID;
        break;

    default:

        //
        // WIN8 behavior: USBD_STATUS_XACT_ERROR for unrecognized TRB completion
        // codes.
        //
        usbdStatus = USBD_STATUS_XACT_ERROR;
        NT_ASSERTMSG("Unsupported CompletionCode", 0);
        break;
    }

    return usbdStatus;
}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
TR_GetNtStatusFromUsbdStatus(
    __in
        USBD_STATUS UsbdStatus
    )
{
    NTSTATUS status;

    status = USBD_STATUS_To_NTSTATUS(UsbdStatus);

    if (status == STATUS_UNSUCCESSFUL) {

        //
        // STATUS_UNSUCCESSFUL is a catchall. Make sure we have set an expected USBD_STATUS (listed
        // below).
        //
        NT_ASSERTMSG("Unrecognized USBD_STATUS",
                     (UsbdStatus == USBD_STATUS_DATA_BUFFER_ERROR ||
                      UsbdStatus == USBD_STATUS_BABBLE_DETECTED ||
                      UsbdStatus == USBD_STATUS_STALL_PID ||
                      UsbdStatus == USBD_STATUS_XACT_ERROR ||
                      UsbdStatus == USBD_STATUS_ISO_TD_ERROR ||
                      UsbdStatus == USBD_STATUS_INVALID_STREAM_TYPE ||
                      UsbdStatus == USBD_STATUS_INVALID_STREAM_ID ||
                      UsbdStatus == USBD_STATUS_ISOCH_REQUEST_FAILED));
    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
TR_IsUrbUsingChainedMdl(
    __in
        PTRANSFER_URB Urb
    )
{
    if ((Urb->TransferBufferMDL != NULL) && (Urb->TransferBufferMDL->Next != NULL)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
TR_DoesSegmentContainDequeuePointer(
    __in
        PBUFFER_DATA    Segment,
    __in
        ULONG           StartIndex,
    __in
        ULONG           OnePastEndIndex,
    __in
        LONGLONG        DequeuePointer
    )
{
    ULONG firstTrb;
    ULONG onePastLastTrb;

    firstTrb = StartIndex * sizeof(TRB);

    if (OnePastEndIndex * sizeof(TRB) > Segment->Size) {
        onePastLastTrb = Segment->Size;
    } else {
        onePastLastTrb = OnePastEndIndex * sizeof(TRB);
    }

    return ((DequeuePointer >= (Segment->LogicalAddress.QuadPart + firstTrb)) &&
            (DequeuePointer <  (Segment->LogicalAddress.QuadPart + onePastLastTrb)));
}

MAPPING_STATE
TR_AttemptStateChange(
    __in
        HANDLE          Context,
    __in
        MAPPING_STATE   FromState,
    __in
        MAPPING_STATE   ToState
    )
/*++

Routine Description:

    Atomically transition the TransferRing MappingState using Compare And Swap (CAS)
    instructions.

    If FromState is MS_Any, unconditionally transition to ToState.

    If FromState is MS_Mapping, and transition fails, attempt transition to MS_Stopped
    and acknowledge the stop to the Endpoint object.

    If ToState is MS_Stopped, acknowledge the stop to the Endpoint object on a successful
    transition.

--*/
{
    MAPPING_STATE   previousState;
    PTR_DATA        trData;

    trData = (PTR_DATA)Context;

    if (FromState == MS_Any) {

        previousState =
            (MAPPING_STATE)InterlockedExchange((volatile LONG*)&trData->MappingState,
                                               ToState);

    } else {

        previousState =
            (MAPPING_STATE)InterlockedCompareExchange((volatile LONG*)&trData->MappingState,
                                                      ToState,
                                                      FromState);
    }

    if ((FromState == previousState) || (FromState == MS_Any)) {

        //
        // Successfully changed state.
        //

        // LogVerbose(Endpoint_GetRecorderLog(trData->EndpointHandle), TransferRing,
        //            "%u.%u.%u: %!MappingState! => %!MappingState!",
        //            UsbDevice_GetSlotId(trData->UsbDeviceHandle),
        //            Endpoint_GetContextIndex(trData->EndpointHandle),
        //            trData->StreamId, FromState, ToState);

        if (ToState == MS_Stopped) {
            Endpoint_TR_MappingStopped(trData->EndpointHandle);
        }

    } else {
        
        //
        // Failed to change state.
        //
        if (FromState == MS_Mapping) {

            //
            // MS_Mapping got changed to MS_Stopping. So go to MS_Stopped now.
            //
            NT_ASSERT(previousState == MS_Stopping);

            if ((MAPPING_STATE)InterlockedCompareExchange((volatile LONG*)&trData->MappingState,
                                                          MS_Stopped,
                                                          MS_Stopping)
                == MS_Stopping) {

                // LogVerbose(Endpoint_GetRecorderLog(trData->EndpointHandle), TransferRing,
                //            "%u.%u.%u: %!MappingState! => %!MappingState!",
                //            UsbDevice_GetSlotId(trData->UsbDeviceHandle),
                //            Endpoint_GetContextIndex(trData->EndpointHandle),
                //            trData->StreamId, MS_Stopping, MS_Stopped);

                Endpoint_TR_MappingStopped(trData->EndpointHandle);
            }
        }
    }

    return previousState;
}

_Use_decl_annotations_
VOID
TR_EmptyFunction(
    HANDLE Context
    )
{
    UNREFERENCED_PARAMETER(Context);
}

_Use_decl_annotations_
VOID
TR_StartMapping(
    HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    trData->FunctionTable->EP_StartMapping(trData);
}

_Use_decl_annotations_
VOID
TR_StopMapping(
    HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    trData->FunctionTable->EP_StopMapping(trData);
}

_Use_decl_annotations_
BOOLEAN
TR_IsMappingStoppedOrPaused(
    HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    return trData->FunctionTable->EP_IsMappingPaused(trData);
}

_Use_decl_annotations_
VOID
TR_EndpointHalted(
    HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    trData->FunctionTable->EP_EndpointHalted(trData);
}

_Use_decl_annotations_
VOID
TR_EndpointResetFromClient(
    HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    trData->FunctionTable->EP_EndpointResetFromClient(trData);
}

_Use_decl_annotations_
VOID
TR_ProcessExpectedEventTRBs(
    HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    trData->FunctionTable->EP_ProcessExpectedEventTRBs(trData);
}

_Use_decl_annotations_
VOID
TR_FSEReceived(
    HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    trData->FunctionTable->EP_FSEReceived(trData);
}

_Use_decl_annotations_
BOOLEAN
TR_WasDoorbellRungSinceMappingStart(
    HANDLE Context
    )
{
    PTR_DATA trData;

    trData = (PTR_DATA)Context;

    return trData->FunctionTable->EP_WasDoorbellRungSinceMappingStart(trData);
}
