//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once 

EVT_WDF_OBJECT_CONTEXT_DESTROY Endpoint_EvtDestroyCallback;

typedef struct _UCX_ENDPOINT_ALL_EVENT_CALLBACKS{
    
    PEVT_UCX_DEFAULT_ENDPOINT_UPDATE            EvtDefaultEndpointUpdate;
    PEVT_UCX_ENDPOINT_PURGE                     EvtEndpointPurge;
    PEVT_UCX_ENDPOINT_START                     EvtEndpointStart;
    PEVT_UCX_ENDPOINT_ABORT                     EvtEndpointAbort;
    PEVT_UCX_ENDPOINT_RESET                     EvtEndpointReset;
    PEVT_UCX_ENDPOINT_OK_TO_CANCEL_TRANSFERS    EvtEndpointOkToCancelTransfers;
    PEVT_UCX_ENDPOINT_STATIC_STREAMS_ADD        EvtEndpointStaticStreamsAdd;
    PEVT_UCX_ENDPOINT_STATIC_STREAMS_ENABLE     EvtEndpointStaticStreamsEnable;
    PEVT_UCX_ENDPOINT_STATIC_STREAMS_DISABLE    EvtEndpointStaticStreamsDisable;
    PEVT_UCX_ENDPOINT_ENABLE_FORWARD_PROGRESS   EvtEndpointEnableForwardProgress;
} UCX_ENDPOINT_ALL_EVENT_CALLBACKS;

typedef enum _UCXENDPOINT_TYPE {
    UcxEndpointTypeGeneric=0,
    UcxEndpointTypeZeroBw,
    UcxEndpointTypeDefault
} UCXENDPOINT_TYPE;

//
// If ever updating this enum, you must update the g_EndpointMaximumTransferSizeTable
//
typedef enum _ENDPOINT_TRANSFER_TYPE {
    EndpointTransferTypeUnknown=0,
    EndpointTransferTypeControl,
    EndpointTransferTypeBulk,
    EndpointTransferTypeIsochronous,
    EndpointTransferTypeInterrupt
} ENDPOINT_TRANSFER_TYPE;

typedef struct _UCXPIPE_INFO *PUCXPIPE_INFO;

typedef struct _UCXPIPE_INFO {
    WDFQUEUE                Queue;
    BOOLEAN                 IsStreamPipe;
    BOOLEAN                 IsEndpointDirectionIn;
    ENDPOINT_TRANSFER_TYPE  EndpointTransferType;
    UCXENDPOINT_TYPE        EndpointType;
    ULONG                   MaximumTransferSize;
    ULONG                   MaximumIsochPacketCount;

    //
    // IsochPeriod in MicroFrames
    //
    ULONG                   IsochPeriodInMicroFrames;
    //
    // Pointer to Self, would be useful for validation of an invalid handle
    //
    PUCXPIPE_INFO           Self;

    //
    // Forward progress provisioning info
    //
    BOOLEAN                 ForwardProgressEnabled;
    BOOLEAN                 ForwardProgressResourcesAllocatedByHcd;
    ULONG                   ForwardProgressMaxTransferSize;

} UCXPIPE_INFO, *PUCXPIPE_INFO;

typedef struct _UCXSSTREAMS_PRIVCONTEXT *PUCXSSTREAMS_PRIVCONTEXT;

typedef struct _UCXENDPOINT_PRIVCONTEXT {
    UCX_SIG                           Sig;
    UCXENDPOINT                       UcxEndpoint;
    UCXPIPE_INFO                      UcxPipeInfo;           
    PUCXCONTROLLER_PRIVCONTEXT        UcxControllerContext;
    PUCXUSBDEVICE_PRIVCONTEXT         UcxUsbDeviceContext;
    PUCXSSTREAMS_PRIVCONTEXT          UcxSStreamsContext;
    LONG                              StreamsEnabled;

    UCX_ENDPOINT_ALL_EVENT_CALLBACKS  EvtCallbacks;   
    //
    // This list is used to link all endpoints of the UsbDevice via
    // UcxUsbDeviceContext->EndpointListHead.
    //
    LIST_ENTRY                        EndpointListEntry;
    //
    // Following list entry is used to create a temporary list of endpoints on a 
    // UsbDeviceDisconnect operation.
    //
    LIST_ENTRY                        UsbDeviceDisconnectListEntryTemp;
    //
    // Following list entry is used to create a temporary list of endpoints on a 
    // Controller Reset operation.
    //
    LIST_ENTRY                        ControllerResetListEntryTemp;
    //
    // Following list entry is used to create a temporary list of endpoints on a 
    // TreePurge operation.
    //
    LIST_ENTRY                        UsbDeviceTreePurgeListEntryTemp;
    //
    // Following list entry is used to create a temporary list of endpoints 
    // during handling an UsbDevice level operation from hub that applies to 
    // all the endpoints of a UsbDevice such as PurgeIO, StartIO
    // NOTE: The reason we cannot use the UsbDeviceDisconnectListEntry for this
    // is that the UsbDeviceDisconnect operation from hub is not sychronized 
    // with another other UsbDevicexxx operations from hub.
    //
    LIST_ENTRY                        EndpointListEntryTemp;

    //
    // Following List Entry is used to create a list of endpoints 
    // if UcxPipeInfo->AllowIoOnInvalidPipeHandles is set for this pipe
    // 
    //
    LIST_ENTRY                        EndpointListEntryForInvalidHandleTraking;

    SM_ENGINE_CONTEXT                 SmEngineContext;

    //
    // Information to hold on to a reqeust temporarily while endpoint
    // state machine is running.
    //

    union {
        PIRP                          PendingStreamsEnableIrp;
        PIRP                          PendingStreamsDisableIrp;
        PIRP                          PendingResetIrp;
        WDFREQUEST                    PendingStreamsEnableRequest;
        WDFREQUEST                    PendingStreamsDisableRequest;
        WDFREQUEST                    PendingResetRequest;
    };

    ULONG                             OpenStaticStreamsFailedDueToControllerReset:1;

    //
    // To handle abort urb
    //
    IO_CSQ_IRP_CONTEXT                AbortPipeIrpCsqContext;

    //
    // A value to track if it is Ok to call EvtEndpointOkToCancelTransfers. 
    // EvtEndpointOkToCancelTransfers must be called only when OkToCancelTransfersSyncEvent
    // value reaches 0
    //
    LONG                              OkToCancelTransfersSyncEvent;

    USB_ENDPOINT_DESCRIPTOR           EndpointDescriptor;

    BOOLEAN                           SmEngineContextInitialized;
    
    BOOLEAN                           ExposedToClient;

    //
    // This flag indicates that a Clear TT Buffer request should be
    // issued to the upstream TT hub if a transfer is canceled this
    // endpoint.
    //
    BOOLEAN                           ClearTtBufferOnTransferCancel;

    //
    // This field keeps track of whether this endpoint is enabled in the hardware or not.
    // This is only supposed to be accessed from the endpoint state machine.
    // Initial value for this boolean is FALSE.
    // Note: This field does not track if the endpoint has been 
    // disabled due to a controller reset.
    //
    BOOLEAN                           Enabled;

} UCXENDPOINT_PRIVCONTEXT, *PUCXENDPOINT_PRIVCONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(UCXENDPOINT_PRIVCONTEXT, 
                                   UcxEndpointGetContext);

typedef struct _UCXENDPOINT_INIT {
    UCX_SIG                                                 Sig;
    UCXENDPOINT_TYPE                                        EndpointType;
    UCXUSBDEVICE                                            UcxUsbDevice;
    ENDPOINT_TRANSFER_TYPE                                  EndpointTransferType;    
    USB_ENDPOINT_DESCRIPTOR                                 EndpointDescriptor;
    USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR            SuperSpeedEndpointCompanionDescriptor;
    UCXENDPOINT                                             UcxEndpoint;
    UCX_ENDPOINT_ALL_EVENT_CALLBACKS                        EvtCallbacks;
    BOOLEAN                                                 CreateStateMachine;
    USB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR  SuperSpeedPlusIsochEndpointCompanionDescriptor;

} UCXENDPOINT_INIT, *PUCXENDPOINT_INIT;

FORCEINLINE
PUCXENDPOINT_PRIVCONTEXT
UcxEndpointGetContextFromEndpointPipeInfo(
    __in 
        PUCXPIPE_INFO EndpointPipeInfo
    )
{
    PUCXENDPOINT_PRIVCONTEXT ucxEndpointContext;
    
    TRY {
        ucxEndpointContext = CONTAINING_RECORD(EndpointPipeInfo, 
                                               UCXENDPOINT_PRIVCONTEXT,
                                               UcxPipeInfo);
        #pragma warning(suppress:26001)
        NT_ASSERT(ucxEndpointContext->Sig == SigUcxEndpointContext);

    } FINALLY {

    }
    return ucxEndpointContext;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
DefaultEndpoint_Create(
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        UCXUSBDEVICE       UsbDevice,
    __in 
        ULONG              MaxPacketSize,
    __out 
        UCXENDPOINT       *DefaultEndpoint
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
Endpoint_Create(
    __in 
        UCXUSBDEVICE                                  Hub,
    __in 
        UCXUSBDEVICE                                  UsbDevice,
    __in 
        PUSB_ENDPOINT_DESCRIPTOR                      EndpointDescriptor,
    __in 
        ULONG                                         EndpointDescriptorBufferLength,
    __in_opt
        PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR SuperSpeedEndpointCompanionDescriptor,
    __out 
        UCXENDPOINT                                  *Endpoint
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Endpoint_Delete(
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        UCXUSBDEVICE       UsbDevice, 
    __in 
        UCXENDPOINT        Endpoint
);

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_ExposedToClient(
    __in 
        UCXENDPOINT        Endpoint
);

USBD_PIPE_HANDLE
Endpoint_GetUsbdPipeHandle(
    __in 
        UCXENDPOINT        Endpoint
    );


BOOLEAN
Endpoint_EventAdd(
    __in
        PUCXENDPOINT_PRIVCONTEXT      UcxEndpointContext,
    __in
        SM_ENGINE_EVENT               Event               
    );

VOID
Endpoint_FindAndCompleteAbortUrbIfNotAlreadyCancelled(
    __in
        PUCXENDPOINT_PRIVCONTEXT        UcxEndpointContext
    );

FORCEINLINE
ENDPOINT_TRANSFER_TYPE
EndpointGetTransferTypeFromDescriptor(
    __in
        PUSB_ENDPOINT_DESCRIPTOR EndpointDescriptor
    ) 
{

    ENDPOINT_TRANSFER_TYPE transferType;

    TRY {
        
        if ((EndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) 
            == USB_ENDPOINT_TYPE_ISOCHRONOUS) {

            transferType = EndpointTransferTypeIsochronous;

        } else if ((EndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) 
                   == USB_ENDPOINT_TYPE_BULK) {

            transferType = EndpointTransferTypeBulk;

        } else if ((EndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK)
                   == USB_ENDPOINT_TYPE_INTERRUPT) {

            transferType = EndpointTransferTypeInterrupt;

        } else {

            NT_ASSERT((EndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) 
                      == USB_ENDPOINT_TYPE_CONTROL);

            transferType = EndpointTransferTypeControl;

        }

    } FINALLY {

    }
    return transferType;
}

VOID
Endpoint_ResetCompleteFromHcd(
    __in
        PIRP                    Irp, 
    __in 
        PENDPOINT_RESET         EndpointReset
);

typedef struct _XRB *PXRB;

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Endpoint_StaticStreamsEnableUrb(
    __in 
        PUCXENDPOINT_PRIVCONTEXT UcxEndpointContext,
    __in
        WDFDEVICE                RHPdo,
    __in 
        PXRB                     Xrb,
    __in 
        PIRP                     Irp    
);

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Endpoint_StaticStreamsDisableUrb(
    __in 
        PUCXENDPOINT_PRIVCONTEXT UcxEndpointContext,
    __in
        WDFDEVICE                RHPdo,
    __in 
        PXRB                     Xrb,
    __in 
        PIRP                     Irp    
);

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_StaticStreamsEnableUrbCompleteFromHcd(
    __in 
        PUCXENDPOINT_PRIVCONTEXT UcxEndpointContext,
    __in 
        PURB                     Urb,
    __in 
        PIRP                     Irp
);

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_StaticStreamsDisableUrbComplete(
    __in 
        PUCXENDPOINT_PRIVCONTEXT UcxEndpointContext,
    __in 
        PIRP                     Irp
);

extern PULONG g_EndpointMaximumTransferSizeTable[];

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_TryToCallEvtEndpointOkToCancelTransfers(
    __in 
        PUCXENDPOINT_PRIVCONTEXT UcxEndpointContext
);

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_ClearTTBufferComplete(
    __in
        UCXUSBDEVICE Hub,
    __in 
        UCXENDPOINT  Endpoint
);

ULONG
Endpoint_GetMaximumTransferSize(
    __in
        UCXENDPOINT         Endpoint
);
