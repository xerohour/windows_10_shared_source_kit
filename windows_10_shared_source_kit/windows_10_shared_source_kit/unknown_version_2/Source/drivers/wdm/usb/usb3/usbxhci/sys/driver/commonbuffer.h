/*++

Copyright (c) Microsoft Corporation

Module Name:

    commonbuffer.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

//
// This is set to the maximum bytes that can be specified for event data. This
// value must be evenly divisible by max packet size.
//
#define MAX_DMA_TRANSFER_SIZE                       0xFFFE00

//
// Defaults for large segment policy.
//
#define LARGE_SEGMENTS_PER_PAGE                     1
#define LARGE_SEGMENT_SIZE                          (PAGE_SIZE / LARGE_SEGMENTS_PER_PAGE)

#define LARGE_SEGMENT_INITIAL_COUNT                 (4 * LARGE_SEGMENTS_PER_PAGE)
#define LARGE_SEGMENT_PREALLOCATION_THRESHOLD       (1 * LARGE_SEGMENTS_PER_PAGE)
#define LARGE_SEGMENT_PREALLOCATION_COUNT           (1 * LARGE_SEGMENTS_PER_PAGE)
#define LARGE_SEGMENT_RECLAMATION_THRESHOLD         (2 * LARGE_SEGMENTS_PER_PAGE)

//
// Defaults for small segment policy.
//
#define SMALL_SEGMENTS_PER_PAGE                     8
#define SMALL_SEGMENT_SIZE                          (PAGE_SIZE / SMALL_SEGMENTS_PER_PAGE)

#define SMALL_SEGMENT_INITIAL_COUNT                 (1 * SMALL_SEGMENTS_PER_PAGE)
#define SMALL_SEGMENT_PREALLOCATION_THRESHOLD       4
#define SMALL_SEGMENT_PREALLOCATION_COUNT           (1 * SMALL_SEGMENTS_PER_PAGE)
#define SMALL_SEGMENT_RECLAMATION_THRESHOLD         (2 * SMALL_SEGMENTS_PER_PAGE)

typedef struct _COMMON_BUFFER_POLICY {

    ULONG LargeSegmentInitialCount;
    ULONG LargeSegmentPreallocationThreshold;
    ULONG LargeSegmentPreallocationCount;
    ULONG LargeSegmentReclamationThreshold;

    ULONG SmallSegmentInitialCount;
    ULONG SmallSegmentPreallocationThreshold;
    ULONG SmallSegmentPreallocationCount;
    ULONG SmallSegmentReclamationThreshold;

} COMMON_BUFFER_POLICY, *PCOMMON_BUFFER_POLICY;

typedef struct _COMMON_BUFFER_CALLBACK_DATA *PCOMMON_BUFFER_CALLBACK_DATA;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(COMMON_BUFFER_CALLBACK) (
    __in
        PCOMMON_BUFFER_CALLBACK_DATA    CommonBufferCallbackData,
    __in
        NTSTATUS                        Status,
    __in
        PLIST_ENTRY                     BufferList
    );

typedef COMMON_BUFFER_CALLBACK *PCOMMON_BUFFER_CALLBACK;

typedef struct _TRACKING_DATA {

    LIST_ENTRY              ListEntry;

    ULONG                   Tag;

    HANDLE                  Owner;

} TRACKING_DATA, *PTRACKING_DATA;

typedef struct _SEGMENT_HEADER {

    LIST_ENTRY              ListEntry;

    BOOLEAN                 PendingFree;

} SEGMENT_HEADER, *PSEGMENT_HEADER;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(SEGMENT_HEADER, GetSegmentHeader)

typedef struct _BUFFER_DATA {

    LIST_ENTRY              ListEntry;

    PUCHAR                  VirtualAddress;

    PHYSICAL_ADDRESS        LogicalAddress;

    WDFCOMMONBUFFER         WdfCommonBuffer;

    ULONG                   Size;

    TRACKING_DATA           TrackingData;

} BUFFER_DATA, *PBUFFER_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(BUFFER_DATA, GetBufferData)

typedef struct _COMMON_BUFFER_CALLBACK_DATA {

    LIST_ENTRY                  ListEntry;

    HANDLE                      Owner;

    ULONG                       NumberToAcquire;

    ULONG                       BufferSize;

    ULONG                       Tag;

    PCOMMON_BUFFER_CALLBACK     Callback;

} COMMON_BUFFER_CALLBACK_DATA;

typedef struct _COMMON_BUFFER_COUNTER {

    ULONG                       PagesTotal;

    ULONG                       PagesInUse;

    LONG                        AllocationCount;

    ULONG                       FreeCount;

    PPCW_INSTANCE               PcwInstanceHandle;

} COMMON_BUFFER_COUNTER, *PCOMMON_BUFFER_COUNTER;

typedef struct _COMMON_BUFFER_DATA {

    WDFDMAENABLER               WdfDmaEnabler;
    HANDLE                      ControllerHandle;

    COMMON_BUFFER_COUNTER       Counter;

    COMMON_BUFFER_POLICY        Policy;

    ULONG                       MaxDmaSize;

    ULONG                       MapRegisterCount;

    PDMA_ADAPTER                DmaAdapter;

    KEVENT                      WorkItemEvent;

    ULONG                       NumberOfLargeListEntries;

    ULONG                       AvailableLargeListEntries;

    KSPIN_LOCK                  CommonBufferLock;

    LIST_ENTRY                  LargeList;

    LIST_ENTRY                  LargeSegmentHeaderList;

    ULONG                       NumberOfSmallListEntries;

    ULONG                       AvailableSmallListEntries;

    LIST_ENTRY                  SmallList;

    LIST_ENTRY                  SmallSegmentHeaderList;

    LIST_ENTRY                  BufferDataTrackingList;

    LIST_ENTRY                  CallbackList;

    //
    // Work Item used to rebalance common buffer resources at PASSIVE_LEVEL
    //
    WDFWORKITEM                 RebalanceResourcesWorkItem;

    ULONG                       PendingFreePageCount;

    LIST_ENTRY                  RebalanceResourcesList;

} COMMON_BUFFER_DATA, *PCOMMON_BUFFER_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(COMMON_BUFFER_DATA, GetCommonBufferData)

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
PDMA_ADAPTER
CommonBuffer_GetDmaAdapter(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    )
{
    return CommonBufferData->DmaAdapter;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG
CommonBuffer_GetMaxDmaSize(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    )
{
    return CommonBufferData->MaxDmaSize;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG
CommonBuffer_GetMapRegisterCount(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    )
{
    return CommonBufferData->MapRegisterCount;
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
    );

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
    );

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
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
CommonBuffer_ReleaseBuffer(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData,
    __in
        PBUFFER_DATA        BufferData
    );

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
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
CommonBuffer_ReleaseBuffers(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData,
    __in
        PLIST_ENTRY         BufferList
    );

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
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
CommonBuffer_D0Exit(
    __in
        PCOMMON_BUFFER_DATA     CommonBufferData,
    __in
        WDF_POWER_DEVICE_STATE  WdfTargetState
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
CommonBuffer_CalculateMaxDmaSize(
    __in
        HANDLE          ControllerHandle,
    __in
        WDFDEVICE       WdfDevice,
    __in
        WDF_DMA_PROFILE WdfDmaProfile,
    __out
        PULONG          MaxDmaSize,
    __out
        PULONG          MapRegisterCount
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
CommonBuffer_InitializePolicy(
    __in
        PCOMMON_BUFFER_DATA     CommonBufferData
    );

IO_WORKITEM_ROUTINE_EX  CommonBuffer_WorkItem;

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
    );

EVT_WDF_OBJECT_CONTEXT_CLEANUP  CommonBuffer_WdfEvtCleanupCallback;

EVT_WDF_WORKITEM CommonBuffer_RebalanceResourcesWorkItem;

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
CommonBuffer_RebalanceResources(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
CommonBuffer_ControllerResetPostReset(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
CommonBuffer_ControllerGone(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
CommonBuffer_FlushWorkItems(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    );
