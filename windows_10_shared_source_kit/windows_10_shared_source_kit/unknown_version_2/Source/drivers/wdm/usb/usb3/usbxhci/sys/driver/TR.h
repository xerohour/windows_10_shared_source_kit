
/*++

Copyright (c) Microsoft Corporation

Module Name:

    TR.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

#define INVERT(s) ((s) == 0) ? 1 : 0

#define DEFAULT_TRANSFERRING_CYCLE_STATE        1

//
// This is the amount of data, in bytes, that can put directly into the TRB.
//
#define IMMEDIATE_DATA_LENGTH                   8

//
// This is the maximum length that can be specified in a transfer TRB.
//
#define MAX_TRB_LENGTH                          65536

#define USBD_STATUS_NOT_SET                     0xFFFFFFFF

//
// The maximum number of TRBs used for a control transfer TD.
//
#define MAX_NUMBER_OF_TRBS_FOR_CONTROL_TD       21

//
// The isoch delay added when starting an asap stream.
//
#define ISOCH_TRANSFER_DELAY                    5

#define ISOCH_MAX_FRAME_MASK                    0x7FF

//
// This value is used to determine when to clear the BEI bit in order to generate an
// interrupt. The value's lower bits are set to determine how often to check. For
// example, if the lower seven bits are set, then an interrupt is generated every
// 128th isoch packet.
//
#define MAX_ISOCH_IOC_INTERVAL_MASK             0x7F

typedef struct _TR_CONFIG* PTR_CONFIG;

//
// Interface for inherited transferring objects.
//

typedef
__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
TR_GETCONFIGURATION(
    __in
        HANDLE      ControllerHandle,
    __out
        PTR_CONFIG  Config
    );

typedef TR_GETCONFIGURATION* PFN_TR_GETCONFIGURATION;

typedef
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
TR_INITIALIZE(
    __in
        HANDLE Context
    );

typedef TR_INITIALIZE* PFN_TR_INITIALIZE;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_CLEANUP(
    __in
        HANDLE Context
    );

typedef TR_CLEANUP* PFN_TR_CLEANUP;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_COMMONBUFFERCALLBACK(
    __in
        HANDLE      Context,
    __in
        NTSTATUS    Status
    );

typedef TR_COMMONBUFFERCALLBACK* PFN_TR_COMMONBUFFERCALLBACK;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
(TR_ENABLE)(
    __in
        HANDLE Context
    );

typedef TR_ENABLE* PFN_TR_ENABLE;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_DISABLE(
    __in
        HANDLE    Context,
    __in
        BOOLEAN   FreeResources
    );

typedef TR_DISABLE* PFN_TR_DISABLE;

typedef
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
TR_ENABLEFORWARDPROGRESS(
    __in
        HANDLE    Context,
    __in
        ULONG     MaxTransferSize
    );

typedef TR_ENABLEFORWARDPROGRESS* PFN_TR_ENABLEFORWARDPROGRESS;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
TR_ARETRANSFERSPENDING(
    __in
        HANDLE Context
    );

typedef TR_ARETRANSFERSPENDING* PFN_TR_ARETRANSFERSPENDING;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_OKTORECLAIMTRANSFERSONCANCEL(
    __in
        HANDLE Context
    );

typedef TR_OKTORECLAIMTRANSFERSONCANCEL* PFN_TR_OKTORECLAIMTRANSFERSONCANCEL;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_RECLAIMTRANSFERS(
    __in
        HANDLE Context
    );

typedef TR_RECLAIMTRANSFERS* PFN_TR_RECLAIMTRANSFERS;

typedef
__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
TR_ISTRANSFEREVENTLIKELYDUPLICATE(
    __in
        HANDLE              Context,
    __in
        PHYSICAL_ADDRESS    DequeuePointer,
    __out
        PBOOLEAN            PointsToNoOpTRB
    );

typedef TR_ISTRANSFEREVENTLIKELYDUPLICATE* PFN_TR_ISTRANSFEREVENTLIKELYDUPLICATE;

typedef
__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
TR_TRANSFEREVENTHANDLER(
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in_opt
        HANDLE              Context
    );

typedef TR_TRANSFEREVENTHANDLER* PFN_TR_TRANSFEREVENTHANDLER;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_STARTMAPPING(
    __in
        HANDLE Context
    );

typedef TR_STARTMAPPING* PFN_TR_STARTMAPPING;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_STOPMAPPING(
    __in
        HANDLE Context
    );

typedef TR_STOPMAPPING* PFN_TR_STOPMAPPING;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
TR_ISMAPPINGSTOPPEDORPAUSED(
    __in
        HANDLE Context
    );

typedef TR_ISMAPPINGSTOPPEDORPAUSED* PFN_TR_ISMAPPINGPAUSED;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_ENDPOINTHALTED(
    __in
        HANDLE Context
    );

typedef TR_ENDPOINTHALTED* PFN_TR_ENDPOINTHALTED;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_ENDPOINTRESETFROMCLIENT(
    __in
        HANDLE Context
    );

typedef TR_ENDPOINTRESETFROMCLIENT* PFN_TR_ENDPOINTRESETFROMCLIENT;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_PROCESSOUTSTANDINGEVENTTRBS(
    __in
        HANDLE Context
    );

typedef TR_PROCESSOUTSTANDINGEVENTTRBS* PFN_TR_PROCESSOUTSTANDINGEVENTTRBS;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_FSERECEIVED(
    __in
        HANDLE Context
    );

typedef TR_FSERECEIVED* PFN_TR_FSERECEIVED;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
TR_WASDOORBELLRUNGSINCEMAPPINGSTART(
    __in
        HANDLE Context
    );

typedef TR_WASDOORBELLRUNGSINCEMAPPINGSTART* PFN_TR_WASDOORBELLRUNGSINCEMAPPINGSTART;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_EMPTYFUNCTION(
    __in
        HANDLE Context
    );

typedef TR_EMPTYFUNCTION* PFN_TR_EMPTYFUNCTION;

typedef struct _TR_FUNCTION_TABLE {

    PFN_TR_INITIALIZE                       TR_Initialize;
    PFN_TR_CLEANUP                          TR_Cleanup;
    PFN_TR_COMMONBUFFERCALLBACK             TR_CommonBufferCallback;

    PFN_TR_ENABLE                           EP_Enable;
    PFN_TR_DISABLE                          EP_Disable;
    PFN_TR_ENABLEFORWARDPROGRESS            EP_EnableForwardProgress;               // !Isoch.
    PFN_TR_ARETRANSFERSPENDING              EP_AreTransfersPending;
    PFN_TR_OKTORECLAIMTRANSFERSONCANCEL     EP_OkToReclaimTransfersOnCancel;
    PFN_TR_RECLAIMTRANSFERS                 EP_ReclaimTransfers;
    PFN_TR_ISTRANSFEREVENTLIKELYDUPLICATE   EP_IsTransferEventLikelyDuplicate;
    PFN_TR_TRANSFEREVENTHANDLER             EP_TransferEventHandler;
    PFN_TR_STARTMAPPING                     EP_StartMapping;
    PFN_TR_STOPMAPPING                      EP_StopMapping;
    PFN_TR_ISMAPPINGPAUSED                  EP_IsMappingPaused;
    PFN_TR_ENDPOINTHALTED                   EP_EndpointHalted;
    PFN_TR_ENDPOINTRESETFROMCLIENT          EP_EndpointResetFromClient;
    PFN_TR_PROCESSOUTSTANDINGEVENTTRBS      EP_ProcessExpectedEventTRBs;
    PFN_TR_FSERECEIVED                      EP_FSEReceived;
    PFN_TR_WASDOORBELLRUNGSINCEMAPPINGSTART EP_WasDoorbellRungSinceMappingStart;

    PFN_WDF_IO_QUEUE_IO_DEFAULT             WDF_EvtIoDefault;                       // &Control.
    PFN_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE   WDF_EvtIoCanceledOnQueue;               // !Control.
    PFN_WDF_IO_QUEUE_STATE                  WDF_EvtIoQueueReadyNotification;        // !Control.
    PFN_WDF_DPC                             WDF_EvtDpcForTransferCompletion;        // !Control.

} TR_FUNCTION_TABLE, *PTR_FUNCTION_TABLE;

typedef CONST TR_FUNCTION_TABLE* PCTR_FUNCTION_TABLE;

typedef struct _TR_CONFIG {

    ULONG                                   TransferType;                           // For usb3kd.

    ULONG                                   MaxStageSize;
    ULONG                                   MapRegisterCount;
    WDF_IO_QUEUE_DISPATCH_TYPE              QueueDispatchType;
    ULONG                                   ContextSize;
    ULONG                                   SegmentSize;
    ULONG                                   DoubleBufferSize;                       // !Isoch.

    PCTR_FUNCTION_TABLE                     FunctionTable;

} TR_CONFIG, *PTR_CONFIG;

typedef enum _MAPPING_STATE {

    MS_Stopped                              = 0,
    MS_Stopping                             = 1,
    MS_Paused                               = 2,
    MS_Mapping                              = 3,
    MS_WaitingForTransferRingEmptyEvent     = 4,
    MS_Any                                  = 5

} MAPPING_STATE;

typedef struct _TRANSFERRING_COUNTER {

    ULONG                       TransferCount;
    ULONG                       FailedTransferCount;
    ULONGLONG                   BytesTransferred;

    ULONG                       IsochTDCount;
    ULONG                       IsochTDFailCount;
    ULONG                       MissedServiceErrorCount;
    ULONG                       UnderrunOverrunCount;

    PPCW_INSTANCE               PcwInstanceHandle;

} TRANSFERRING_COUNTER, *PTRANSFERRING_COUNTER;

typedef enum _TRB_STAGE {
    TrbStage_NotValid           = 0,
    TrbStage_Setup              = 1,
    TrbStage_Data               = 2,
    TrbStage_Status             = 3,
    TrbStage_Isoch              = 4,
    TrbStage_Normal             = 5,
    TrbStage_EventData          = 6,
    TrbStage_FinalEventData     = 7,
    TrbStage_PadTransferBuffer  = 8,
    TrbStage_Done               = 9,
    TrbStage_DummyNormal        = 10,
    TrbStage_FinalDummyNormal   = 11
} TRB_STAGE;

typedef struct _CONTROL_URB {
    UCHAR SetupPacket[8];
} CONTROL_URB, *PCONTROL_URB;

typedef struct _ISOCH_URB {
    ULONG                       StartFrame;
    ULONG                       NumberOfPackets;
    ULONG                       ErrorCount;
    USBD_ISO_PACKET_DESCRIPTOR  IsochPacket[1];
} ISOCH_URB, *PISOCH_URB;

struct _UCX_URB_DATA {
    ULONG ProcessorNumber;
#ifdef WIN64
    ULONG Padding;
#endif
    PVOID Reserved7[7];
};

typedef struct _TRANSFER_URB {

    struct _URB_HEADER      Header;

    PVOID                   UsbdPipeHandle;
    ULONG                   TransferFlags;
    ULONG                   TransferBufferLength;
    PUCHAR                  TransferBuffer;
    PMDL                    TransferBufferMDL;
    union {
        ULONG Timeout;              // no Linked Urbs
        PVOID ReservedMBNull;       // fields
    };
    struct _UCX_URB_DATA UrbData;   // fields for HCD use

    union {
        CONTROL_URB         Control;
        ISOCH_URB           Isoch;
    };

} TRANSFER_URB, *PTRANSFER_URB;

typedef struct _TRB_RANGE {

    PBUFFER_DATA                FirstSegment;
    ULONG                       FirstSegmentStartIndex;
    PBUFFER_DATA                LastSegment;
    ULONG                       LastSegmentEndIndex;

} TRB_RANGE, *PTRB_RANGE;

typedef struct _TRB_ITERATOR {

    PBUFFER_DATA                Segment;
    ULONG                       SegmentIndex;

} TRB_ITERATOR, *PTRB_ITERATOR;

typedef struct _ISOCH_TRACKING_DATA {

    ULONG64                      EndpointType:2;
    ULONG64                      Reserved:14;
    ULONG64                      IsochPacketIndex:16;

    ULONG64                      FrameNumber:32;

} ISOCH_TRACKING_DATA, *PISOCH_TRACKING_DATA;

C_ASSERT(sizeof(ISOCH_TRACKING_DATA) == sizeof(ULONG64));

typedef struct _CONTROL_TRACKING_DATA {

    ULONG64                      EndpointType:2;
    ULONG64                      Stage:1;
    ULONG64                      Reserved:61;

} CONTROL_TRACKING_DATA, *PCONTROL_TRACKING_DATA;

C_ASSERT(sizeof(CONTROL_TRACKING_DATA) == sizeof(ULONG64));

typedef union _TD_TRACKING_DATA {

    ULONG64                     EndpointType:2;
    ULONG64                     Reserved0:62;

    ISOCH_TRACKING_DATA         IsochTrackingData;
    CONTROL_TRACKING_DATA       ControlTrackingData;

} TD_TRACKING_DATA, *PTD_TRACKING_DATA;

C_ASSERT(sizeof(TD_TRACKING_DATA) == sizeof(ULONG64));

typedef struct _TRANSFERRING_DEQUEUE_POINTER {

    union {

        ULONG64     DequeuePointer;

        struct {
            ULONG64     DequeueCycleState:1;
            ULONG64     StreamContextType:3;
            ULONG64     DequeuePointerLo:28;
            ULONG64     DequeuePointerHi:32;
        };
    };

} TRANSFERRING_DEQUEUE_POINTER, *PTRANSFERRING_DEQUEUE_POINTER;

typedef struct _TR_DATA {

    TR_CONFIG;

    HANDLE                                  ControllerHandle;
    HANDLE                                  UsbDeviceHandle;
    HANDLE                                  EndpointHandle;
    ULONG                                   StreamId;

    WDFQUEUE                                WdfQueue;
    WDFDPC                                  WdfDpcForTransferCompletion;

    //
    // N.B: Keeping SavedIrql in this structure instead of on the stack provides the
    // convenience of writing wrapper routines TR_Lock/TR_Unlock to acquire/release
    // the lock and not having to keep the irql on the stack. This is safe to do
    // for regular spinlocks, but should not be done for queued spin locks.
    //
    KSPIN_LOCK                              Lock;
    KIRQL                                   SavedIrql;

    MAPPING_STATE                           MappingState;

    USHORT                                  InterrupterTarget;
    PMDL                                    ForwardProgressMdl;                     // !Isoch.
    COMMON_BUFFER_CALLBACK_DATA             CommonBufferCallbackData;

    PBUFFER_DATA                            CurrentSegmentBufferData;
    PTRB                                    CurrentSegment;
    ULONG                                   CurrentSegmentIndex;
    ULONG                                   MaxSegmentIndex;
    ULONG                                   CycleState;

    LIST_ENTRY                              SegmentList;
    LIST_ENTRY                              DoubleBufferDataList;                   // !Isoch.

    TRANSFERRING_COUNTER                    Counter;

} TR_DATA, *PTR_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(TR_DATA, GetTRData);

typedef enum _TRANSFER_MECHANISM {

    TransferMechanism_NoData                = 0,
    TransferMechanism_ImmediateData         = 1,
    TransferMechanism_DoubleBufferData      = 2,
    TransferMechanism_Dma                   = 3

} TRANSFER_MECHANISM; 

typedef enum _TRANSFER_CANCEL_STATE {

    TCS_NotCancelable                       = 0,
    TCS_Cancelable                          = 1,
    TCS_WaitingForCancelCallback            = 2,
    TCS_Canceled                            = 3

} TRANSFER_CANCEL_STATE;

typedef enum _TRANSFER_TIMEOUT_STATE {

    TTS_NoTimeout                           = 0,
    TTS_TimeoutSet                          = 1,
    TTS_WaitingForTimerCallback             = 2,
    TTS_TimedOut                            = 3

} TRANSFER_TIMEOUT_STATE;

typedef enum _PROCESS_STAGE_RESULT {

    PSR_Success                             = 0,
    PSR_TransferCompletedPrematurely        = 1,
    PSR_GapDetected                         = 2,
    PSR_WaitingOnCommonBuffer               = 3,
    PSR_ResourceAllocationFailed            = 4

} PROCESS_STAGE_RESULT;

typedef enum _MAP_STAGE_RESULT {

    MSR_Success                             = 0,
    MSR_GetScatterGatherListFailed          = 1,
    MSR_WaitingOnDmaCallback                = 2,
    MSR_TransferCompletedPrematurely        = 3,
    MSR_LateDetected                        = 4

} MAP_STAGE_RESULT;

//
// Public functions.
//

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
    );

COMMON_BUFFER_CALLBACK                  TR_CommonBufferCallback;

//
// WDF Callbacks.
//

EVT_WDF_OBJECT_CONTEXT_CLEANUP          TR_WdfEvtCleanupCallback;
EVT_WDF_IO_WDM_IRP_FOR_FORWARD_PROGRESS TR_WdfEvtIoExamineIrpForForwardProgress;

//
// Trampoline functions
//

TR_ENABLE                               TR_Enable;
TR_DISABLE                              TR_Disable;
TR_ENABLEFORWARDPROGRESS                TR_EnableForwardProgress;
TR_ARETRANSFERSPENDING                  TR_AreTransfersPending;
TR_ISTRANSFEREVENTLIKELYDUPLICATE       TR_IsTransferEventLikelyDuplicate;
TR_TRANSFEREVENTHANDLER                 TR_TransferEventHandler;
TR_OKTORECLAIMTRANSFERSONCANCEL         TR_OkToReclaimTransfersOnCancel;
TR_RECLAIMTRANSFERS                     TR_ReclaimTransfers;
TR_STARTMAPPING                         TR_StartMapping;
TR_STOPMAPPING                          TR_StopMapping;
TR_ISMAPPINGSTOPPEDORPAUSED             TR_IsMappingStoppedOrPaused;
TR_ENDPOINTHALTED                       TR_EndpointHalted;
TR_ENDPOINTRESETFROMCLIENT              TR_EndpointResetFromClient;
TR_PROCESSOUTSTANDINGEVENTTRBS          TR_ProcessExpectedEventTRBs;
TR_FSERECEIVED                          TR_FSEReceived;
TR_WASDOORBELLRUNGSINCEMAPPINGSTART     TR_WasDoorbellRungSinceMappingStart;

//
// Helper routines for objects from inherited class.
//

TR_EMPTYFUNCTION TR_EmptyFunction;

__drv_maxIRQL(DISPATCH_LEVEL)
HANDLE
TR_GetContext(
    __in
        HANDLE Context
    );

_Requires_lock_not_held_(((PTR_DATA)Context)->Lock)
_Acquires_lock_(((PTR_DATA)Context)->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_Lock(
    __in
        HANDLE Context
    );

_Requires_lock_held_(((PTR_DATA)Context)->Lock)
_Releases_lock_(((PTR_DATA)Context)->Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
TR_Unlock(
    __in
        HANDLE Context
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_QueueDpcForTransferCompletion(
    __in
        HANDLE Context
    );

_Requires_lock_not_held_(((PTR_DATA)Context)->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
TR_EnsureSegments(
    __in
        HANDLE  Context,
    __in
        ULONG   SegmentsRequired,
    __in
        BOOLEAN Async
    );

_Requires_lock_not_held_(((PTR_DATA)Context)->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
PBUFFER_DATA
TR_AcquireSegment(
    __in
        HANDLE Context
    );

_Requires_lock_not_held_(((PTR_DATA)Context)->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_ReleaseSegment(
    __in
        HANDLE          Context,
    __in
        PBUFFER_DATA    Segment
    );

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
    );

_Requires_lock_held_(((PTR_DATA)Context)->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_ReleaseSegments(
    __in
        HANDLE      Context,
    __inout
        PLIST_ENTRY SegmentList,
    __in
        BOOLEAN     ReleaseToHeadOfList
    );

_Requires_lock_not_held_(((PTR_DATA)Context)->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
PBUFFER_DATA
TR_AcquireDoubleBuffer(
    __in
        HANDLE Context
    );

_Requires_lock_not_held_(((PTR_DATA)Context)->Lock)
VOID
TR_ReleaseDoubleBuffer(
    __in
        HANDLE          Context,
    __in
        PBUFFER_DATA    Segment
    );

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG
TR_GetPacketCount(
    __in
        HANDLE Context,
    __in
        ULONG  BufferSize
    );

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
    );

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG
TR_GetCycleState(
    __in
        HANDLE Context
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_ToggleCycleState(
    __in
        HANDLE Context
    );

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
    );

__drv_maxIRQL(DISPATCH_LEVEL)
WDFQUEUE
TR_GetWdfQueue(
    __in
        PTR_DATA TRData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_InitializeTransferRing(
    __in
        PTR_DATA TRData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
LONG64
TR_GetCurrentSegmentLA(
    __in
        PTR_DATA TRData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
LONG64
TR_GetEnqueuePointerLA(
    __in
        PTR_DATA TRData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG64
TR_GetDequeuePointer(
    __in
        PTR_DATA TRData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
USBD_STATUS
TR_GetUsbdStatusFromTrbCompletionCode(
    __in
        ULONG       TrbCompletionCode,
    __in
        USBD_STATUS UsbdStatus
    );

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
TR_GetNtStatusFromUsbdStatus(
    __in
        USBD_STATUS UsbdStatus
    );

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
TR_IsUrbUsingChainedMdl(
    __in
        PTRANSFER_URB Urb
    );

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
TR_DoesSegmentContainDequeuePointer(
    __in
        PBUFFER_DATA    Segment,
    __in
        ULONG           StartIndex,
    __in
        ULONG           EndIndex,
    __in
        LONGLONG        DequeuePointer
    );

MAPPING_STATE
TR_AttemptStateChange(
    __in
        HANDLE          Context,
    __in
        MAPPING_STATE   FromState,
    __in
        MAPPING_STATE   ToState
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_TransfersReclaimed(
    __in
        HANDLE Context
    );

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
BOOLEAN
IsDequeuePointerInSegment(
    __in
        PHYSICAL_ADDRESS    DequeuePointer,
    __in
        PBUFFER_DATA        Segment
    )
{
    return ((DequeuePointer.QuadPart >= Segment->LogicalAddress.QuadPart) &&
            (DequeuePointer.QuadPart < (Segment->LogicalAddress.QuadPart + Segment->Size)));
}
