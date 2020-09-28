
/*++

Copyright (c) Microsoft Corporation

Module Name:

    Isoch.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

#define ISOCH_MAX_PENDING_STAGES_PER_TRANSFER       3
#define ISOCH_MAX_PENDING_STAGES_PER_TRANSFERRING   5

#define ISOCH_MAX_COMMONBUFFER_ACQUIRE_ATTEMPTS     3

typedef struct _ISOCH_DATA*                         PISOCH_DATA;
typedef struct _ISOCH_TRANSFER_DATA*                PISOCH_TRANSFER_DATA;
typedef struct _ISOCH_STAGE_DATA*                   PISOCH_STAGE_DATA;

TR_GETCONFIGURATION                                 Isoch_GetConfiguration;
TR_INITIALIZE                                       Isoch_Initialize;
TR_CLEANUP                                          Isoch_Cleanup;
TR_COMMONBUFFERCALLBACK                             Isoch_CommonBufferCallback;

TR_ENABLE                                           Isoch_EP_Enable;
TR_DISABLE                                          Isoch_EP_Disable;

TR_ARETRANSFERSPENDING                              Isoch_EP_AreTransfersPending;
TR_OKTORECLAIMTRANSFERSONCANCEL                     Isoch_EP_OkToReclaimTransfersOnCancel;
TR_RECLAIMTRANSFERS                                 Isoch_EP_ReclaimTransfers;
TR_TRANSFEREVENTHANDLER                             Isoch_EP_TransferEventHandler;
TR_STARTMAPPING                                     Isoch_EP_StartMapping;
TR_STOPMAPPING                                      Isoch_EP_StopMapping;
TR_ISMAPPINGSTOPPEDORPAUSED                         Isoch_EP_IsMappingStoppedOrPaused;
TR_PROCESSOUTSTANDINGEVENTTRBS                      Isoch_EP_ProcessExpectedEventTRBs;
TR_WASDOORBELLRUNGSINCEMAPPINGSTART                 Isoch_EP_WasDoorbellRungSinceMappingStart;
TR_ENDPOINTRESETFROMCLIENT                          Isoch_EP_EndpointResetFromClient;

EVT_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE               Isoch_WdfEvtIoCanceledOnQueue;
EVT_WDF_IO_QUEUE_STATE                              Isoch_WdfEvtIoQueueReadyNotification;
EVT_WDF_DPC                                         Isoch_WdfEvtDpcForTransferCompletion;
EVT_WDF_REQUEST_CANCEL                              Isoch_WdfEvtRequestCancel;

#ifndef DISABLE_RING_EMPTY_TIMER
EVT_WDF_TIMER                                       Isoch_WdfEvtRingEmptyTimer;
#define ISOCH_RING_EMPTY_TIMEOUT                    WDF_REL_TIMEOUT_IN_SEC(10)
#endif

DRIVER_LIST_CONTROL                                 Isoch_EvtDmaCallback;

typedef union _ISOCH_DATA_FLAGS {

    ULONG AsUlong32;

    struct {
        ULONG DoorbellRungSinceMappingStart:1;
        ULONG OkToReclaimTransfersOnCancel:1;
        ULONG AtleastOneTransferCanceled:1;
        ULONG ReclaimTransfersAndAcknowledge:1;
        ULONG TransferRingEmptyEventExpected:1;
        ULONG FrameNumberOfLastTDInScheduleValid:1;
        ULONG FSEReceived:1;
    };

} ISOCH_DATA_FLAGS;
C_ASSERT(sizeof(ISOCH_DATA_FLAGS) == sizeof(ULONG));

typedef struct _ISOCH_DATA {

    TR_DATA;

#ifndef DISABLE_RING_EMPTY_TIMER
    WDFTIMER                    WdfRingEmptyTimer;
#endif

    //
    // N.B: These flags get cleared in one-shot when mapping starts. So only use
    //      these flags for bits that are Ok to clear the next time mapping start.
    //      Any writes to individual bits should be done with Interlocked operations.
    //      Plain reads are Ok.
    //
    ISOCH_DATA_FLAGS            Flags;

    BOOLEAN                     DpcRunning;                         // Under Lock.
    BOOLEAN                     RingDoorbellOnFirstTD;              // Mapping path only.
    BOOLEAN                     IsochPure;                          // Mapping path only.

    ULONG                       CommonBufferAcquireAttemptCount;

    LONG                        ContinueMapping;
    LONG                        AttemptMapping;
    LONG                        TransferRingEmptyEventReceived;
    LONG                        LateDetected;

    ULONG                       PacketsPerFrame;

    ULONG                       NextTransferStartFrame;
    ULONG                       FrameNumberOfLastTDInSchedule;

    PISOCH_STAGE_DATA           CurrentStage;
    ULONG                       PendingStageCount;

    LIST_ENTRY                  PendingTransferList;
    LIST_ENTRY                  WaitingForCancelCallbackTransferList;
    LIST_ENTRY                  CompletionTransferList;

    ULONG                       EstimatedTrbsPerTD;

} ISOCH_DATA, *PISOCH_DATA;

typedef struct _ISOCH_STAGE_DATA {

    PISOCH_TRANSFER_DATA        TransferData;

    LIST_ENTRY                  FreeSegmentList;                // Synchronization not needed.
    LIST_ENTRY                  UsedSegmentList;                // ??

    ULONG                       FirstPacket;                    // Sticky.
    ULONG                       LastPacket;                     // Mapping & Completion.
    ULONG                       OnePastLastCompletedPacket;     // completion path only.

    ULONG                       TransferSize;                   // mapping path only.
    BOOLEAN                     FreeMdl;                        // Synchronization not needed.
    PMDL                        Mdl;                            // Synchronization not needed.
    PSCATTER_GATHER_LIST        ScatterGatherList;              // Synchronization not needed.

    TRB_RANGE                   TrbRange;                       // Mapping & Completion.
    TRB_ITERATOR                TrbIterator;                    // completion path only.

} ISOCH_STAGE_DATA, *PISOCH_STAGE_DATA;

typedef struct _ISOCH_STAGE_QUEUE {

    STAGE_QUEUE_HEADER          Header;

    DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT)
    ISOCH_STAGE_DATA            StageArray[ISOCH_MAX_PENDING_STAGES_PER_TRANSFER];

} ISOCH_STAGE_QUEUE, *PISOCH_STAGE_QUEUE;

C_ASSERT(ISOCH_MAX_PENDING_STAGES_PER_TRANSFER < UCHAR_MAX);

typedef struct _ISOCH_TRANSFER_DATA {

    LIST_ENTRY                  ListEntry;
    BOOLEAN                     Initialized;

    WDFREQUEST                  WdfRequest;
    PTRANSFER_URB               TransferUrb;
    PISOCH_DATA                 IsochData;

    TRANSFER_CANCEL_STATE       CancelState;
    NTSTATUS                    WdfRequestStatus;

    PMDL                        Mdl;

    ULONG                       BytesTotal;                     // Synchronization not needed.
    ULONG                       BytesTransferred;               // Completion path only.
    ULONG                       BytesMapped;                    // mapping path only.
    ULONG                       BytesSkipped;                   // mapping path only.

    ULONG                       PacketsTotal;                   // Synchronization not needed.
    ULONG                       PacketsCompleted;               // Completion path only. Includes packets that were not mapped.
    ULONG                       PacketsMapped;                  // mapping path only.
    ULONG                       PacketsSkipped;                 // mapping path only.

    ULONG                       StagesCompleted;                // Mapping & Completion.
    ULONG                       StagesMapped;                   // Mapping path only.

    ISOCH_STAGE_QUEUE           StageQueue;

} ISOCH_TRANSFER_DATA, *PISOCH_TRANSFER_DATA;

//
// Private functions.
//

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Isoch_MapTransfers(
    __in
        PISOCH_DATA IsochData
    );

ULONG
Isoch_CalculatePacketsPerFrame(
    __in
        PISOCH_DATA IsochData
    );

ULONG
Isoch_GetFrameCount(
    __in
        PISOCH_DATA IsochData,
    __in
        ULONG       PacketCount
    );

ULONG
Iosch_GetPacketFrameNumber(
    __in
        PISOCH_DATA IsochData,
    __in
        ULONG       StartFrame,
    __in
        ULONG       PacketIndex
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
ULONG
Isoch_GetTDPC(
    __in
        PISOCH_DATA IsochData,
    __in
        ULONG       TDLength
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
ULONG
Isoch_GetTBC(
    __in
        PISOCH_DATA         IsochData,
    __in
        ULONG               MaxTrbLength
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
ULONG
Isoch_GetTLBPC(
    __in
        PISOCH_DATA         IsochData,
    __in
        ULONG               TDLength
    );

__drv_maxIRQL(DISPATCH_LEVEL)
USBD_STATUS
Isoch_GetUsbdStatusFromTrbCompletionCode(
    __in
        ULONG CompletionCode
    );

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Isoch_GetNtStatusFromUsbdStatus(
    __in
        USBD_STATUS UsbdStatus
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
ULONG
Isoch_GetPacketLength(
    __in
        PISOCH_TRANSFER_DATA    TransferData,
    __in
        ULONG                   PacketIndex
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
ULONG
Isoch_GetPacketSpanLength(
    __in
        PISOCH_TRANSFER_DATA    TransferData,
    __in
        ULONG                   FirstPacket,
    __in
        ULONG                   LastPacket
    );

_Requires_lock_held_(IsochData->Lock)
VOID
Isoch_Transfer_CompleteCancelable(
    __in
        PISOCH_DATA             IsochData,
    __in
        PISOCH_TRANSFER_DATA    TransferData,
    __in
        USBD_STATUS             UrbPacketStatus,
    __in
        USBD_STATUS             UrbHeaderStatus,
    __in
        BOOLEAN                 CompleteIfCanceled,
    __in
        BOOLEAN                 CompleteAsync
    );

_Requires_lock_held_(IsochData->Lock)
BOOLEAN
Isoch_DoesDriverOwnRequests(
    __in
        PISOCH_DATA IsochData
    );
