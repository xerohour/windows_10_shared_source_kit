
/*++

Copyright (c) Microsoft Corporation

Module Name:

    Control.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

#define MAX_CONTROL_TRANSFER_LENGTH         (64 * 1024)     // 64 KB.

typedef struct _CONTROL_DATA*               PCONTROL_DATA;
typedef struct _CONTROL_TRANSFER_DATA*      PCONTROL_TRANSFER_DATA;

TR_GETCONFIGURATION                         Control_GetConfiguration;
TR_INITIALIZE                               Control_Initialize;
TR_CLEANUP                                  Control_Cleanup;

TR_ENABLE                                   Control_EP_Enable;
TR_DISABLE                                  Control_EP_Disable;
TR_ENABLEFORWARDPROGRESS                    Control_EP_EnableForwardProgress;
TR_ARETRANSFERSPENDING                      Control_EP_AreTransfersPending;
TR_OKTORECLAIMTRANSFERSONCANCEL             Control_EP_OkToReclaimTransfersOnCancel;
TR_RECLAIMTRANSFERS                         Control_EP_ReclaimTransfers;
TR_TRANSFEREVENTHANDLER                     Control_EP_TransferEventHandler;
TR_STARTMAPPING                             Control_EP_StartMapping;
TR_STOPMAPPING                              Control_EP_StopMapping;
TR_ISMAPPINGSTOPPEDORPAUSED                 Control_EP_IsMappingStoppedOrPaused;
TR_PROCESSOUTSTANDINGEVENTTRBS              Control_EP_ProcessExpectedEventTRBs;
TR_WASDOORBELLRUNGSINCEMAPPINGSTART         Control_EP_WasDoorbellRungSinceMappingStart;

EVT_WDF_IO_QUEUE_IO_DEFAULT                 Control_WdfEvtIoDefault;
EVT_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE       Control_WdfEvtIoCanceledOnQueue;
EVT_WDF_TIMER                               Control_WdfEvtTimerForTransferTimeout;
EVT_WDF_REQUEST_CANCEL                      Control_WdfEvtRequestCancel;
EVT_WDF_DPC                                 Control_WdfEvtDpcForTransferCompletion;
EVT_WDF_DPC                                 Control_WdfDpcForCanceledOnQueueTransferCompletion;

DRIVER_LIST_CONTROL                         Control_EvtDmaCallback;

typedef union _CONTROL_FLAGS {

    ULONG AsUlong32;

    struct {
        ULONG DoorbellRungSinceMappingStart:1;
        ULONG ReclaimTransfersAndAcknowledge:1;
        ULONG AcknowledgeExpectedEventTRBsProcessed:1;
        ULONG OkToReclaimTransfersOnCancel:1;
    };

} CONTROL_FLAGS;
C_ASSERT(sizeof(CONTROL_FLAGS) == sizeof(ULONG));

typedef struct _CONTROL_DATA {

    TR_DATA;

    BOOLEAN                     ImmediateDataEnabled;

    WDFDPC                      WdfDpcForCanceledOnQueueTransferCompletion;

    WDFTIMER                    WdfTimerForTransferTimeout;
    PCONTROL_TRANSFER_DATA      TransferData;

    //
    // N.B: These flags get cleared in one-shot when mapping starts. So only use
    //      these flags for bits that are Ok to clear the next time mapping start.
    //      All reads/writes should be done under the Lock, since this is a struct
    //      bit-field.
    //
    CONTROL_FLAGS               Flags;

    ULONG                       OutstandingEventCountAfterStop;

    LIST_ENTRY                  CanceledOnQueueTransferList;

    BOOLEAN                     DpcRunning;

} CONTROL_DATA, *PCONTROL_DATA;

typedef struct _CONTROL_TRANSFER_DATA {

    LIST_ENTRY                  ListEntry;
    BOOLEAN                     Initialized;

    WDFREQUEST                  WdfRequest;
    PTRANSFER_URB               TransferUrb;
    PCONTROL_DATA               ControlData;

    TRANSFER_MECHANISM          TransferMechanism;
    PMDL                        Mdl;
    PUCHAR                      Buffer;
    PBUFFER_DATA                DoubleBufferData;
    PSCATTER_GATHER_LIST        ScatterGatherList;

    ULONG                       BytesTotal;
    ULONG                       BytesTransferred;

    TRANSFER_CANCEL_STATE       CancelState;
    TRANSFER_TIMEOUT_STATE      TimeoutState;

    NTSTATUS                    Status;
    ULONG                       TrbCompletionCode;

    ULONG                       TDCount;
    ULONG                       ReceivedEventCount;

    TRB_RANGE                   TrbRange;

} CONTROL_TRANSFER_DATA, *PCONTROL_TRANSFER_DATA;

//
// Private functions
//

_Requires_lock_held_(ControlData->Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Control_Transfer_CompleteCancelable(
    __in
        PCONTROL_DATA ControlData
    );

_Requires_lock_held_(ControlData->Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Control_Transfer_Complete(
    __in
        PCONTROL_DATA           ControlData,
    __in
        PCONTROL_TRANSFER_DATA  TransferData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Control_MapTransfer(
    __in
        PCONTROL_DATA ControlData
    );
