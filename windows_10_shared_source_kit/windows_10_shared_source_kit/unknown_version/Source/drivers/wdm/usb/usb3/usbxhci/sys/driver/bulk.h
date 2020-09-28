
/*++

Copyright (c) Microsoft Corporation

Module Name:

    Bulk.h

Abstract:

    Bulk and Interrupt transfer implementation.

--*/

#pragma once

#define BULK_MAX_PENDING_STAGES_PER_TRANSFER        1
#define BULK_MAX_PENDING_STAGES_PER_TRANSFERRING    5

#define BULK_MAX_COMMONBUFFER_ACQUIRE_ATTEMPTS      3
//
// This is the minimum size of a stage. The code expects a control
// transfer can be mapped into a single stage.
//
#define MIN_STAGE_SIZE                              65536

typedef struct _BULK_TRANSFER_DATA*                 PBULK_TRANSFER_DATA;
typedef struct _BULK_STAGE_DATA*                    PBULK_STAGE_DATA;

TR_GETCONFIGURATION                                 Bulk_GetConfiguration;
TR_INITIALIZE                                       Bulk_Initialize;
TR_CLEANUP                                          Bulk_Cleanup;
TR_COMMONBUFFERCALLBACK                             Bulk_CommonBufferCallback;

TR_ENABLE                                           Bulk_EP_Enable;
TR_DISABLE                                          Bulk_EP_Disable;
TR_ENABLEFORWARDPROGRESS                            Bulk_EP_EnableForwardProgress;
TR_ARETRANSFERSPENDING                              Bulk_EP_AreTransfersPending;
TR_OKTORECLAIMTRANSFERSONCANCEL                     Bulk_EP_OkToReclaimTransfersOnCancel;
TR_RECLAIMTRANSFERS                                 Bulk_EP_ReclaimTransfers;
TR_ISTRANSFEREVENTLIKELYDUPLICATE                   Bulk_EP_IsTransferEventLikelyDuplicate;
TR_TRANSFEREVENTHANDLER                             Bulk_EP_TransferEventHandler;
TR_STARTMAPPING                                     Bulk_EP_StartMapping;
TR_STOPMAPPING                                      Bulk_EP_StopMapping;
TR_ISMAPPINGSTOPPEDORPAUSED                         Bulk_EP_IsMappingStoppedOrPaused;
TR_ENDPOINTHALTED                                   Bulk_EP_EndpointHalted;
TR_PROCESSOUTSTANDINGEVENTTRBS                      Bulk_EP_ProcessExpectedEventTRBs;
TR_FSERECEIVED                                      Bulk_EP_FSEReceived;
TR_WASDOORBELLRUNGSINCEMAPPINGSTART                 Bulk_EP_WasDoorbellRungSinceMappingStart;

EVT_WDF_IO_QUEUE_STATE                              Bulk_WdfEvtIoQueueReadyNotification;
EVT_WDF_DPC                                         Bulk_WdfEvtDpcForTransferCompletion;
EVT_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE               Bulk_WdfEvtIoCanceledOnQueue;
EVT_WDF_REQUEST_CANCEL                              Bulk_WdfEvtRequestCancel;

DRIVER_LIST_CONTROL                                 Bulk_EvtDmaCallback;

typedef union _BULK_DATA_FLAGS {

    ULONG AsUlong32;

    struct {
        ULONG DoorbellRungSinceMappingStart:1;
        ULONG AcknowledgeExpectedEventTRBsProcessed:1;
        ULONG EndpointHalted:1;
        ULONG FSEReceived:1;
        ULONG AtleastOneTransferCanceled:1;
        ULONG OkToReclaimTransfersOnCancel:1;
        ULONG ReclaimTransfersAndAcknowledge:1;
    };

} BULK_DATA_FLAGS;

typedef struct _BULK_DATA {

    TR_DATA;

    BOOLEAN                     ImmediateDataEnabled;                   // Synchronization not needed.

    //
    // N.B: These flags get cleared in one-shot when mapping starts. So only use
    //      these flags for bits that are Ok to clear the next time mapping start.
    //      Any writes to individual bits should be done with Interlocked operations.
    //      Plain reads are Ok.
    //
    BULK_DATA_FLAGS             Flags;                                  // Under lock.

    BOOLEAN                     DpcRunning;                             // Under lock.
    ULONG                       CommonBufferAcquireAttemptCount;

    LONG                        ContinueMapping;                        // Interlocked.
    LONG                        AttemptMapping;                         // Interlocked.

    ULONG                       MaxPendingStages;                       // Synchronization not needed.
    ULONG                       PendingStageCount;                      // Mapping and Completion.
    ULONG                       OutstandingEventCountAfterStop;

    PBULK_STAGE_DATA            CurrentStage;                           // Mapping only.

    LIST_ENTRY                  PendingTransferList;                    // Mapping and Completion.
    LIST_ENTRY                  WaitingForCancelCallbackTransferList;
    LIST_ENTRY                  CompletionTransferList;

} BULK_DATA, *PBULK_DATA;

C_ASSERT(BULK_MAX_PENDING_STAGES_PER_TRANSFERRING < UCHAR_MAX);

typedef struct _BULK_STAGE_DATA {

    PBULK_TRANSFER_DATA         TransferData;

    LIST_ENTRY                  FreeSegmentList;
    LIST_ENTRY                  UsedSegmentList;

    ULONG                       TransferSize;
    BOOLEAN                     FreeMdl;
    PMDL                        Mdl;
    PSCATTER_GATHER_LIST        ScatterGatherList;
    PUCHAR                      Buffer;
    PBUFFER_DATA                DoubleBufferData;

    ULONG                       TrbsPerBurst;
    ULONG                       RequiredTrbs;
    ULONG                       UsedTrbs;

    TRB_RANGE                   TrbRange;

} BULK_STAGE_DATA, *PBULK_STAGE_DATA;

typedef struct _BULK_STAGE_QUEUE {

    STAGE_QUEUE_HEADER          Header;

    DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT)
    BULK_STAGE_DATA             StageArray[BULK_MAX_PENDING_STAGES_PER_TRANSFER];

} BULK_STAGE_QUEUE, *PBULK_STAGE_QUEUE;

C_ASSERT(BULK_MAX_PENDING_STAGES_PER_TRANSFER < UCHAR_MAX);

typedef struct _BULK_TRANSFER_DATA {

    LIST_ENTRY                  ListEntry;
    BOOLEAN                     Initialized;

    WDFREQUEST                  WdfRequest;
    PTRANSFER_URB               TransferUrb;
    PBULK_DATA                  BulkData;

    TRANSFER_CANCEL_STATE       CancelState;
    ULONG                       TrbCompletionCode;
    NTSTATUS                    WdfRequestStatus;

    TRANSFER_MECHANISM          TransferMechanism;
    PMDL                        Mdl;
    PUCHAR                      Buffer;
    PBUFFER_DATA                DoubleBufferData;

    ULONG                       BytesTotal;
    ULONG                       BytesTransferred;
    ULONG                       BytesMapped;

    ULONG                       StagesCompleted;
    ULONG                       StagesMapped;

    BULK_STAGE_QUEUE            StageQueue;

} BULK_TRANSFER_DATA, *PBULK_TRANSFER_DATA;


//
// Private functions.
//

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_MapTransfers(
    __in
        PBULK_DATA BulkData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_Stage_Initialize(
    __in
        PBULK_TRANSFER_DATA    TransferData,
    __out
        PBULK_STAGE_DATA       StageData
    );

_Requires_lock_not_held_(BulkData->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_Stage_FreeScatterGatherList(
    __in
        PBULK_DATA          BulkData,
    __inout
        PBULK_STAGE_DATA    StageData
    );

_Requires_lock_held_(BulkData->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_Stage_Free(
    __in
        PBULK_DATA          BulkData,
    __inout
        PBULK_STAGE_DATA    StageData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
PBULK_STAGE_DATA
Bulk_Stage_Acquire(
    __inout
        PBULK_TRANSFER_DATA TransferData
    );

_Requires_lock_held_(BulkData->Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_Stage_Release(
    __in
        PBULK_DATA          BulkData,
    __inout
        PBULK_STAGE_DATA    StageData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_TransferData_Initialize(
    __in
        PBULK_DATA              BulkData,
    __in
        PBULK_TRANSFER_DATA     TransferData,
    __in
        WDFREQUEST              WdfRequest
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_TransferData_Free(
    __in
        PBULK_TRANSFER_DATA TransferData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Bulk_TransferData_DetermineTransferMechanism(
    __in
        PBULK_TRANSFER_DATA TransferData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Bulk_TransferData_ConfigureBuffer(
    __in
        PBULK_TRANSFER_DATA TransferData
    );

_Requires_lock_not_held_(BulkData->Lock)
VOID
Bulk_Transfer_Complete(
    __in
        PBULK_DATA             BulkData,
    __in
        PBULK_TRANSFER_DATA    TransferData,
    __in
        USBD_STATUS            UsbdStatus
    );

_Requires_lock_held_(BulkData->Lock)
VOID
Bulk_Transfer_CompleteCancelable(
    __in
        PBULK_DATA              BulkData,
    __in
        PBULK_TRANSFER_DATA     TransferData,
    __in
        USBD_STATUS             UsbdStatus,
    __in
        BOOLEAN                 CompleteIfCanceled
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_ProcessTransferEventWithED1(
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in_opt
        HANDLE              Context
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_ProcessTransferEventWithED0(
    __in
        PTRANSFER_EVENT_TRB Trb,
    __in_opt
        HANDLE              Context
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_FindStage(
    __in
        PBULK_DATA          BulkData,
    __in
        PHYSICAL_ADDRESS    TransferTrbLA,
    __out
        PBULK_STAGE_DATA*   StageData,
    __out
        PULONG              StagesSkipped
    );

_Requires_lock_held_(BulkData->Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Bulk_IsTransferEventLikelyDuplicate_Internal(
    __in
        PBULK_DATA          BulkData,
    __in
        PHYSICAL_ADDRESS    DequeuePointer,
    __out
        PBOOLEAN            PointsToNoOpTRB
    );

_Requires_lock_held_(BulkData->Lock)
BOOLEAN
Bulk_DoesDriverOwnRequests(
    __in
        PBULK_DATA BulkData
    );
