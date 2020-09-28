/*++
Copyright (c) Microsoft Corporation

Module Name:

    endpoint.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

#define USB_ENDPOINT_NUMBER_MASK                0x0F
#define USB_ENDPOINT_MAXPACKETSIZE_MASK         0x7FF
#define USB_ENDPOINT_TRANSACTIONS_PER_MF_MASK   0x1800

#define DEFAULT_ENDPOINT_CONTEXT_INDEX          1
#define CONTROL_TRANSFER_TIMEOUT_MS             5000

//
// The time to wait, in ms, before reissuing an endpoint stop command
//
#define ENDPOINT_STOP_REISSUE_DELAY             200
#define ENDPOINT_STOP_REISSUE_MAX_COUNT         20

//
// The time to wait, in ms, before the initial issue of an endpoint
// stop command. Note that this is only used if the device flag
// requesting this behavior is set.
//
#define ENDPOINT_STOP_INITIAL_DELAY             500

#define EndpointAddressToDCI(e)                         \
    ((((e) & (~USB_ENDPOINT_DIRECTION_MASK)) * 2) +     \
     (USB_ENDPOINT_DIRECTION_IN(e) ? 1 : 0))            \

//
// These values are taken from Table 57 in rev 0.96 of xHCI specification
//
typedef enum _ENDPOINT_TYPE {
    EndpointType_NotValid       = 0,
    EndpointType_IsochOut       = 1,
    EndpointType_BulkOut        = 2,
    EndpointType_InterruptOut   = 3,
    EndpointType_Control        = 4,
    EndpointType_IsochIn        = 5,
    EndpointType_BulkIn         = 6,
    EndpointType_InterruptIn    = 7
} ENDPOINT_TYPE;

typedef enum _ENDPOINT_DIRECTION {
    EndpointDirection_Out       = 0,
    EndpointDirection_In        = 1
} ENDPOINT_DIRECTION;

typedef struct _STATIC_STREAMS_DATA *PSTATIC_STREAMS_DATA;
typedef struct _ENDPOINT_RESET_DATA *PENDPOINT_RESET_DATA;

//
// This structure contains everything needed to implement the 
// ClearFeature(EndpointStall) command for controllers that need
// ConfigureEndpoint instead of SetTRDQPointer after StopEndpoint.
// 
typedef struct _CLEAR_STALL_CONTEXT {
    PIRP                            Irp;
    struct _URB_CONTROL_TRANSFER_EX Urb;
    WDFREQUEST                      Request;
} CLEAR_STALL_CONTEXT, *PCLEAR_STALL_CONTEXT;

#define CLEAR_STALL_IRP_STACK_SIZE  4

typedef union _ENDPOINT_FLAGS {

    ULONG AsUlong32;

    struct {
        ULONG CancelReportedSinceMappingStart:1;
        ULONG CommandCompletionFailureReported:1;
        ULONG PurgeTransferRingQueuesAfterHalt:1;
        ULONG OkToReportFSE:1;
        ULONG FSEReceived:1;
        ULONG HaltedCompletionCodeReceived:1;
        ULONG DropFSE:1;
        ULONG ControllerResetCompleteAcknowledged:1;
        ULONG OkToCompleteEndpointResetRequest:1;
    };

} ENDPOINT_FLAGS;

typedef struct _ENDPOINT_DATA {

    HANDLE                              ControllerHandle;
    UCXUSBDEVICE                        UcxUsbDevice;
    HANDLE                              UsbDeviceHandle;
    UCXENDPOINT                         UcxEndpoint;

    //
    // N.B: These flags get cleared in one-shot when mapping starts. So only use
    //      these flags for bits that are Ok to clear the next time mapping start.
    //      Any writes to individual bits should be done with Interlocked operations.
    //      Plain reads are Ok.
    //
    ENDPOINT_FLAGS                      Flags;

    BOOLEAN                             ForwardProgressEnabled;
    BOOLEAN                             StreamsCapable;

    KEVENT                              ControllerResetCompleteAcknowledgeEvent;

    //
    // Links this endpoint into the list of all endpoints that exist for
    // this device.
    //
    LIST_ENTRY                          EndpointListEntry;

    RECORDER_LOG                        RecorderLog;

    HANDLE                              TransferRingHandle;

    USB_ENDPOINT_DESCRIPTOR             UsbEndpointDescriptor;

    USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR
                                        UsbSuperSpeedEndpointCompanionDescriptor;

    USB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR
                                        UsbSuperSpeedPlusIsochEndpointCompanionDescriptor;

    ENDPOINT_TYPE                       Type;

    PSTATIC_STREAMS_DATA                DefaultStaticStreamsData;
    PSTATIC_STREAMS_DATA                StaticStreamsData;

    ULONG                               ContextIndex;

    ULONG                               MaxBurstSize;

    ULONG                               MaxPayloadPerServiceOpportunity;

    ULONG                               StopReissueCount;

    CRB                                 Crb;
    PBUFFER_DATA                        InputBufferData;

    PCLEAR_STALL_CONTEXT                ClearStallContext;

    //
    // Endpoint Reset request from client is stashed here because it has
    // to go through State Machine.
    //
    WDFREQUEST                          EndpointResetRequest;
    NTSTATUS                            EndpointResetCompletionStatus;

    SM_CONTEXT                          SmContext;

} ENDPOINT_DATA, *PENDPOINT_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(ENDPOINT_DATA, GetEndpointData)

typedef struct _STATIC_STREAM_DATA {
    HANDLE  TransferRingHandle;
    CRB     Crb;
} STATIC_STREAM_DATA, *PSTATIC_STREAM_DATA;

typedef struct _STATIC_STREAMS_DATA {
    PENDPOINT_DATA      EndpointData;
    ULONG               NumberOfStreams;
    ULONG               MaxPrimaryStreams;
    ULONG               PurgeCount;

    //
    // Used to keep track of how many streams the Set Dequeue Pointer
    // Command has completed on.
    //
    LONG                UpdateCount;

    ULONG               HaltedCompletionCode;
    PBUFFER_DATA        StreamContextArray;
    STATIC_STREAM_DATA  StaticStreamData[1];
} STATIC_STREAMS_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(STATIC_STREAMS_DATA, GetStaticStreamsData);

typedef struct _ENDPOINT_UPDATE_DATA {

    PENDPOINT_DATA              EndpointData;
    PBUFFER_DATA                InputBufferData;
    ULONG                       MaxPacketSize;
    CRB                         Crb;

} ENDPOINT_UPDATE_DATA, *PENDPOINT_UPDATE_DATA;

typedef struct _ENDPOINT_STATIC_STREAMS_CONFIGURE {

    PSTATIC_STREAMS_DATA        StaticStreamsData;

} ENDPOINT_STATIC_STREAMS_CONFIGURE, *PENDPOINT_STATIC_STREAMS_CONFIGURE;

typedef struct _ENDPOINT_RESET_DATA {

    PENDPOINT_DATA              EndpointData;
    PBUFFER_DATA                InputBufferData;
    CRB                         Crb;

} ENDPOINT_RESET_DATA, *PENDPOINT_RESET_DATA;

//
// Public functions.
//
FORCEINLINE
HANDLE
Endpoint_GetEndpointHandle(
    __in
        UCXENDPOINT UcxEndpoint
    )
{
    return GetEndpointData(UcxEndpoint);
}

FORCEINLINE
UCXENDPOINT
Endpoint_GetUcxEndpoint(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->UcxEndpoint;
}

FORCEINLINE
UCXUSBDEVICE
Endpoint_GetUcxUsbDevice(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->UcxUsbDevice;
}

FORCEINLINE
HANDLE
Endpoint_GetUsbDeviceHandle(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->UsbDeviceHandle;
}

FORCEINLINE
RECORDER_LOG
Endpoint_GetRecorderLog(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->RecorderLog;
}

FORCEINLINE
USHORT
Endpoint_GetMaxPacketSize(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->UsbEndpointDescriptor.wMaxPacketSize & USB_ENDPOINT_MAXPACKETSIZE_MASK;
}

FORCEINLINE
USHORT
Endpoint_GetTransactionsPerMicroframe(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return (EndpointData->UsbEndpointDescriptor.wMaxPacketSize & USB_ENDPOINT_TRANSACTIONS_PER_MF_MASK) >> 11;
}

FORCEINLINE
ENDPOINT_DIRECTION
Endpoint_GetDirection(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return (EndpointData->UsbEndpointDescriptor.bEndpointAddress & USB_ENDPOINT_DIRECTION_MASK) >> 7;
}

FORCEINLINE
VOID
Endpoint_SetBurstSize(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           MaxBurstSize
    )
{
    EndpointData->MaxBurstSize = MaxBurstSize;
}

FORCEINLINE
ULONG
Endpoint_GetMaxBurstSize(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->MaxBurstSize;
}

FORCEINLINE
UCHAR
Endpoint_GetInterval(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->UsbEndpointDescriptor.bInterval;
}

FORCEINLINE
VOID
Endpoint_SetMaxPayloadPerServiceOpportunity(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           MaxPayloadPerServiceOpportunity
    )
{
    EndpointData->MaxPayloadPerServiceOpportunity = MaxPayloadPerServiceOpportunity;
}

FORCEINLINE
ULONG
Endpoint_GetMaxPayloadPerServiceOpportunity(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->MaxPayloadPerServiceOpportunity;
}

FORCEINLINE
UCHAR
Endpoint_GetEndpointNumber(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->UsbEndpointDescriptor.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
}

FORCEINLINE
UCHAR
Endpoint_GetAddress(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->UsbEndpointDescriptor.bEndpointAddress;
}

FORCEINLINE
ENDPOINT_TYPE
Endpoint_GetType(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->Type;
}

FORCEINLINE
ULONG
Endpoint_GetTransferType(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->UsbEndpointDescriptor.bmAttributes & USB_ENDPOINT_TYPE_MASK;
}

FORCEINLINE
ULONG
Endpoint_GetErrorCount(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    ULONG errorCount;
    KSE_XHCI_CONTROLLER_FLAGS   deviceFlags;

    deviceFlags = Controller_GetDeviceFlags(EndpointData->ControllerHandle);

    //
    // There are no retries on Isoch, hence no error count.
    //
    if (Endpoint_GetTransferType(EndpointData) == USB_ENDPOINT_TYPE_ISOCHRONOUS) {
        errorCount = 0;
    } else if (deviceFlags.SetUnlimitedErrorRetryCountForEndpoint &&
               (Endpoint_GetTransferType(EndpointData) == USB_ENDPOINT_TYPE_INTERRUPT) &&
               (Endpoint_GetDirection(EndpointData) == EndpointDirection_In) &&
               (UsbDevice_GetVendorId(EndpointData->UsbDeviceHandle) == USB_VENDOR_ID_PSC_SCANNER) &&
               (UsbDevice_GetProductId(EndpointData->UsbDeviceHandle) == USB_PRODUCT_ID_PSC_SCANNER) &&
               (UsbDevice_GetPortPathDepth(EndpointData->UsbDeviceHandle) > 1)) {
        //
        // Interrupt IN endpoints for PSC Scanners connected to a non-root hub port
        // require configuring unlimited retries (CErr = errorCount = 0) due to a
        // H/W bug in Intel host controllers.
        //
        errorCount = 0;
    } else {
        errorCount = 3;
    }

    return errorCount;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_SetLogIdentifier(
    __in
        PENDPOINT_DATA  EndpointData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_StreamsSetEndpointHaltedAndPendingReset(
    __in
        PENDPOINT_DATA  EndpointData
    );

FORCEINLINE
ULONG
Endpoint_StreamsGetCompletionCode(
    __in
        PENDPOINT_DATA  EndpointData
    )
/*
Routine Description:

    This function returns the last error completion code on a streams
    enabled endpoint.

*/
{
    return EndpointData->StaticStreamsData->HaltedCompletionCode;
}

FORCEINLINE
ULONG
Endpoint_GetMaxBurst(
    __in
        PENDPOINT_DATA  EndpointData
    )
/*++

Routine Description:

Return Value:

    The maximum number of packets the endpoint can send or receive as
    part of a burst. Valid values are from 0 to 15. A value of 0
    indicates that the endpoint can only burst one packet at a time and
    a value of 15 indicates that the endpoint can burst up to 16 packets
    at a time.

    For endpoints of type control this shall be set to 0.

    See USB 3.0: 9.6.7 SuperSpeed Endpoint Companion

--*/
{
    return EndpointData->UsbSuperSpeedEndpointCompanionDescriptor.bMaxBurst;
}

FORCEINLINE
ULONG
Endpoint_GetBytesPerIntervalForSS(
    __in
        PENDPOINT_DATA  EndpointData
    )
/*++

Routine Description:

    Returns the wBytesPerInterval value from the SuperSpeed Endpoint Companion
    Descriptor.

    (d)wBytesPerInterval is the total number of bytes this endpoint will transfer
    every service interval. This field is only valid for periodic endpoints.
    For isochronous endpoints, this value is used to reserve the bus time in
    the schedule, required for the frame data payloads per 125 μs. The pipe may,
    on an ongoing basis, actually use less bandwidth than that reserved. The device
    reports, if necessary, the actual bandwidth used via its normal, non-USB defined
    mechanisms.  (d)wBytesPerInterval is reserved and must be set to zero for control
    and bulk endpoints.

    See USB 3.0: 9.6.7 SuperSpeed Endpoint Companion

--*/
{
    return EndpointData->UsbSuperSpeedEndpointCompanionDescriptor.wBytesPerInterval;
}

FORCEINLINE
ULONG
Endpoint_IsochEndpointSSP(
    __in
        PENDPOINT_DATA  EndpointData
    )
/*++

Routine Description:

    Returns TRUE if SuperSpeedPlus Isochronous Endpoint Companion Descriptor is used.

    See USB 3.1: 9.6.8 SuperSpeedPlus Isochronous Endpoint Companion

--*/
{
    return (BOOLEAN)EndpointData->UsbSuperSpeedEndpointCompanionDescriptor.bmAttributes.Isochronous.SspCompanion;
}

FORCEINLINE
ULONG
Endpoint_GetBytesPerIntervalForSSP(
    __in
        PENDPOINT_DATA  EndpointData
    )
/*++

Routine Description:

    Returns the dwBytesPerInterval value from the SuperSpeedPlus Isochronous Endpoint Companion
    Descriptor.

    dwBytesPerInterval is the total number of bytes this endpoint will transfer
    every service interval.

    See USB 3.1: 9.6.8 SuperSpeedPlus Isochronous Endpoint Companion

--*/
{
    return EndpointData->UsbSuperSpeedPlusIsochEndpointCompanionDescriptor.dwBytesPerInterval;
}

FORCEINLINE
ULONG
Endpoint_GetMultForSS(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->UsbSuperSpeedEndpointCompanionDescriptor.bmAttributes.Isochronous.Mult;
}

FORCEINLINE
ULONG
Endpoint_GetMaxStreams(
    __in
        PENDPOINT_DATA  EndpointData
    )
/*++

Routine Description:

Return Value:

    The maximum number of streams this endpoint supports. Valid values
    are from 0 to 16, where a value of 0 indicates that the endpoint
    does not define streams. For the values 1 to 16, the number of
    streams supported equals 2**MaxStream.

    See USB 3.0: 9.6.7 SuperSpeed Endpoint Companion

--*/
{
    return EndpointData->UsbSuperSpeedEndpointCompanionDescriptor.bmAttributes.Bulk.MaxStreams;
}

FORCEINLINE
ULONG
Endpoint_GetContextIndex(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->ContextIndex;
}

EVT_UCX_USBDEVICE_DEFAULT_ENDPOINT_ADD      Endpoint_UcxEvtUsbDeviceDefaultEndpointAdd;

EVT_UCX_USBDEVICE_ENDPOINT_ADD              Endpoint_UcxEvtUsbDeviceEndpointAdd;

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Endpoint_SetType(
    __in
        PENDPOINT_DATA  EndpointData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Endpoint_SetContextIndex(
    __in
        PENDPOINT_DATA  EndpointData
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Endpoint_Create(
    __in
        UCXCONTROLLER               UcxController,
    __in
        UCXUSBDEVICE                UcxUsbDevice,
    __in
        PUCXENDPOINT_INIT           UcxEndpointInit,
    __in
        PUSB_ENDPOINT_DESCRIPTOR    UsbEndpointDescriptor,
    __in_opt
        PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR
                                    UsbSuperSpeedEndpointCompanionDescriptor,
    __in_opt
        PUSB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR
                                    UsbSuperSpeedPlusIsochEndpointCompanionDescriptor
    );

EVT_WDF_OBJECT_CONTEXT_CLEANUP              Endpoint_EvtEndpointCleanupCallback;

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Endpoint_Enable(
    __in
        PENDPOINT_DATA  EndpointData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_Disable(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        BOOLEAN         OnDeviceDisable
    );

COMMAND_CALLBACK                            Endpoint_EvaluateContextCompletion;

EVT_UCX_DEFAULT_ENDPOINT_UPDATE             Endpoint_UcxEvtDefaultEndpointUpdate;

EVT_WDF_IO_QUEUE_STATE                      Endpoint_WdfEvtPurgeComplete;

EVT_UCX_ENDPOINT_PURGE                      Endpoint_UcxEvtEndpointPurge;

EVT_UCX_ENDPOINT_START                      Endpoint_UcxEvtEndpointStart;

EVT_WDF_IO_QUEUE_STATE                      Endpoint_WdfEvtAbortComplete;

EVT_UCX_ENDPOINT_ABORT                      Endpoint_UcxEvtEndpointAbort;

EVT_UCX_ENDPOINT_RESET                      Endpoint_UcxEvtEndpointReset;

EVT_UCX_ENDPOINT_OK_TO_CANCEL_TRANSFERS     Endpoint_UcxEvtEndpointOkToCancelTransfers;

EVT_UCX_ENDPOINT_ENABLE_FORWARD_PROGRESS    Endpoint_UcxEvtEndpointEnableForwardProgress;

COMMAND_CALLBACK                            Endpoint_OnCancelSetDequeuePointerCompletion;

EVT_WDF_TIMER                               Endpoint_WdfEvtTimerForEndpointStopReissue;

EVT_WDF_TIMER                               Endpoint_WdfEvtStateMachineTimer;

typedef enum _ENDPOINT_CONFIGURE_OPERATION {

    EndpointConfigureOperation_Add = 0,
    EndpointConfigureOperation_Drop,
    EndpointConfigureOperation_DropAndAdd,

} ENDPOINT_CONFIGURE_OPERATION;

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_SetUpConfigureEndpointCommand(
    __inout
        PENDPOINT_DATA         EndpointData,
    __in
        ENDPOINT_CONFIGURE_OPERATION
                               ConfigureType,
    __in
        PCOMMAND_CALLBACK      CommandCallback,
    __in
        HANDLE                 CallbackContext,
    __in
        PBUFFER_DATA           InputContextCommonBuffer,
    __in_opt
        PENDPOINT_CONTEXT64    EndpointContextToCopy,
    __out_opt
        PENDPOINT_CONTEXT64  * EndpointContext,
    __out
        PCRB                   Crb
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_OnResetEndpointConfigure(
    __in
        PENDPOINT_DATA EndpointData
    );

COMMAND_CALLBACK                            Endpoint_OnResetEndpointConfigureCompletion_EpDropped;
COMMAND_CALLBACK                            Endpoint_OnResetEndpointConfigureCompletion;

VOID
Endpoint_OnCancelSetDequeuePointer(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           StreamId
    );

COMMAND_CALLBACK                            Endpoint_OnCancelStopCompletion;

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_OnCancelEndpointConfigure(
    __in
        PENDPOINT_DATA EndpointData
    );

COMMAND_CALLBACK                            Endpoint_OnCancelEndpointConfigureCompletion_EpDropped;
COMMAND_CALLBACK                            Endpoint_OnCancelEndpointConfigureCompletion;

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_StreamOnCancelStop(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        PBOOLEAN        StopStillPending
    );

typedef struct _TRANSFER_DATA *PTRANSFER_DATA;

COMMAND_CALLBACK                            Endpoint_OnStallSetDequeuePointerCompletion;

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_InitializeTransferRing(
    __in
        PENDPOINT_DATA  EndpointData,
    __in_opt
        ULONG           StreamId
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Endpoint_HaltedCompletionCode(
    __in
        ULONG   CompletionCode
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Endpoint_StoppedCompletionCode(
    __in
        ULONG   CompletionCode
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Endpoint_TransferEventHandler(
    __in
        PTRANSFER_EVENT_TRB TransferEventTrb,
    __in
        PENDPOINT_DATA      EndpointData
    );

//
// STREAM Additions
//

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Endpoint_Stream_IsTransferEventLikelyDuplicate(
    __in
        PTRANSFER_EVENT_TRB TransferEventTrb,
    __in
        PENDPOINT_DATA      EndpointData,
    __out
        PBOOLEAN            PointsToNoOpTRB
    );

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG64
Endpoint_GetDequeuePointer(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           StreamId
    );

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG
Endpoint_GetMaxPrimaryStreams(
    __in
        PENDPOINT_DATA  EndpointData
    );

EVT_UCX_ENDPOINT_STATIC_STREAMS_ADD Endpoint_UcxEvtEndpointStaticStreamsAdd;

EVT_WDF_OBJECT_CONTEXT_CLEANUP Endpoint_EvtStaticStreamsCleanupCallback;

EVT_UCX_ENDPOINT_STATIC_STREAMS_ENABLE Endpoint_UcxEvtEndpointStaticStreamsEnable;

EVT_UCX_ENDPOINT_STATIC_STREAMS_DISABLE Endpoint_UcxEvtEndpointStaticStreamsDisable;

FORCEINLINE
PSTREAM_CONTEXT
Endpoint_GetStreamContext(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           StreamId
    )
{
    PSTREAM_CONTEXT streamContextArray;

    streamContextArray = (PSTREAM_CONTEXT)EndpointData->StaticStreamsData->StreamContextArray->VirtualAddress;
    return &streamContextArray[StreamId];
}

FORCEINLINE
LONG64
Endpoint_GetStreamContextArrayLA(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    return EndpointData->StaticStreamsData->StreamContextArray->LogicalAddress.QuadPart;
}

FORCEINLINE
PHYSICAL_ADDRESS
Endpoint_GetStreamContextDequeuePointer(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           StreamId
    )
{
    PSTREAM_CONTEXT     streamContext;
    PHYSICAL_ADDRESS    dequeuePointer;

    streamContext           = Endpoint_GetStreamContext(EndpointData, StreamId);
    dequeuePointer.QuadPart = streamContext->DequeuePointer & 0xFFFFFFFFFFFFFFF0;

    return dequeuePointer;
}

FORCEINLINE
VOID
Endpoint_ResetStreamContextEdtla(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           StreamId
    )
{
    KSE_XHCI_CONTROLLER_FLAGS   deviceFlags;
    PSTREAM_CONTEXT             streamContext;

    deviceFlags   = Controller_GetDeviceFlags(EndpointData->ControllerHandle);
    streamContext = Endpoint_GetStreamContext(EndpointData, StreamId);

    if (deviceFlags.StreamEDTLAAlternateLocation1 || deviceFlags.StreamEDTLAAlternateLocation2) {

        if (deviceFlags.StreamEDTLAAlternateLocation2 == 0) {

            //
            // Location1 = 1, Location 2 = 0
            // EDTLA is in the Alt1 location
            // 
            streamContext->StreamEDTLA_Alt1 = 0;

        } else if (deviceFlags.StreamEDTLAAlternateLocation1 == 0) {

            //
            // Location1 = 0, Location 2 = 1
            // EDTLA is in the Alt2 location
            // 
            streamContext->StreamEDTLA_Alt2 = 0;

        } else {

            //
            // Location 1 = 1, Location 2 = 1
            // Unsupported location, assume EDTLA is 0
            // 
        }

    } else {    
        streamContext->StreamEDTLA = 0;
    }
}

FORCEINLINE
ULONG
Endpoint_GetStreamContextTransferLength(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           StreamId
    )
{
    KSE_XHCI_CONTROLLER_FLAGS   deviceFlags;
    PSTREAM_CONTEXT             streamContext;
    ULONG                       streamEDTLA;

    deviceFlags   = Controller_GetDeviceFlags(EndpointData->ControllerHandle);
    streamContext = Endpoint_GetStreamContext(EndpointData, StreamId);

    //
    // If either of the alternate EDTLA location bits are set, then need to look somewhere else for
    // the EDTLA
    // 
    if (deviceFlags.StreamEDTLAAlternateLocation1 || deviceFlags.StreamEDTLAAlternateLocation2) {

        if (deviceFlags.StreamEDTLAAlternateLocation2 == 0) {

            //
            // Location1 = 1, Location 2 = 0
            // EDTLA is in the Alt1 location
            // 
            streamEDTLA = streamContext->StreamEDTLA_Alt1;
            if (deviceFlags.UseEDTLAValidBit) {
                if (streamContext->EdtlaValid_Alt1 == 0) {
                    streamEDTLA = 0;
                }
            }

        } else if (deviceFlags.StreamEDTLAAlternateLocation1 == 0) {

            //
            // Location1 = 0, Location 2 = 1
            // EDTLA is in the Alt2 location
            // 
            streamEDTLA = streamContext->StreamEDTLA_Alt2;

        } else {

            //
            // Location 1 = 1, Location 2 = 1
            // Unsupported location, assume EDTLA is 0
            // 
            streamEDTLA = 0;
        }

    } else {    
        streamEDTLA = streamContext->StreamEDTLA;
    }

    if (deviceFlags.PartialStreamEDTLAValue) {

        NT_ASSERT(deviceFlags.StreamEDTLAAlternateLocation2 == 0);

        //
        // This controller's EDTLA value doesn't incude the bytes transferred for the partially completed
        // TRB pointed to by the dequeue pointer.  This is in a different field and needs to be added
        // to the partial EDTLA value to get the total EDTLA
        // 
        streamEDTLA += streamContext->PartialTRBLength;
    }

    return streamEDTLA;
}

FORCEINLINE
VOID
Endpoint_SetStreamHandle(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           StreamId,
    __in
        HANDLE          StreamHandle
    )
{
    EndpointData->StaticStreamsData->StaticStreamData[StreamId - 1].TransferRingHandle = StreamHandle;
}

FORCEINLINE
HANDLE
Endpoint_GetStreamHandle(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           StreamId
    )
{
    return EndpointData->StaticStreamsData->StaticStreamData[StreamId - 1].TransferRingHandle;
}

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
TR_NOTIFICATION(
    __in
        HANDLE Context
    );

typedef TR_NOTIFICATION* PFN_TR_NOTIFICATION;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
TR_QUERY(
    __in
        HANDLE Context
    );

typedef TR_QUERY* PFN_TR_QUERY;

FORCEINLINE
VOID
Endpoint_MUX_SendNotificationToTransferRings(
    __in
        PENDPOINT_DATA      EndpointData,
    __in
        PFN_TR_NOTIFICATION Notification
    )
{
    ULONG                   i;
    PSTATIC_STREAMS_DATA    staticStreamsData;

    if (EndpointData->StreamsCapable) {

        staticStreamsData = EndpointData->StaticStreamsData;

        staticStreamsData->UpdateCount = 0;

        for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

            Notification(Endpoint_GetStreamHandle(EndpointData, i));
        }

    } else {

        Notification(EndpointData->TransferRingHandle);
    }
}

FORCEINLINE
BOOLEAN
Endpoint_MUX_SendQueryToTransferRings(
    __in
        PENDPOINT_DATA      EndpointData,
    __in
        PFN_TR_QUERY        Query
    )
{
    BOOLEAN                 result;
    ULONG                   i;
    PSTATIC_STREAMS_DATA    staticStreamsData;

    result = FALSE;

    if (EndpointData->StreamsCapable) {

        staticStreamsData = EndpointData->StaticStreamsData;

        staticStreamsData->UpdateCount = 0;

        for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

            result = Query(Endpoint_GetStreamHandle(EndpointData, i));

            if (result) {
                break;
            }
        }

    } else {

        result = Query(EndpointData->TransferRingHandle);
    }

    return result;
}

FORCEINLINE
VOID
Endpoint_MUX_QueueEvent(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ESM_EVENT       Event
    )
{
    ULONG                   updateCount;
    PSTATIC_STREAMS_DATA    staticStreamsData;

    if (EndpointData->StreamsCapable) {

        staticStreamsData = EndpointData->StaticStreamsData;

        updateCount = (ULONG)InterlockedIncrement(&staticStreamsData->UpdateCount);

        if (updateCount == staticStreamsData->NumberOfStreams) {
            ESM_AddEsmEvent(EndpointData, Event);
        }

    } else {

        ESM_AddEsmEvent(EndpointData, Event);
    }
}

FORCEINLINE
VOID
Endpoint_ReportCommandFailure(
    __in
        PENDPOINT_DATA EndpointData
    )
/*

Routine Description:

    Queues EsmEventCommandCompletionFailure into the State Machine if one has not already
    been queued. More than one command completion failure can happen in case of streams,
    where multiple Set Dequeue Pointer commands can be outstanding.

*/
{
    ULONG commandFailureAlreadyReported;

    INTERLOCKED_GET_AND_SET_BIT_FIELD(ENDPOINT_FLAGS,
                                      &EndpointData->Flags,
                                      CommandCompletionFailureReported,
                                      &commandFailureAlreadyReported);

    if (commandFailureAlreadyReported == 0) {
        ESM_AddEsmEvent(EndpointData, EsmEventCommandCompletionFailure);
    }
}

//
// Callbacks from other objects that result in queueing of events into the
// Endpoint state machine.
//

FORCEINLINE
VOID
Endpoint_TR_TransferCanceled(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    ULONG cancelAlreadyReported;

    INTERLOCKED_GET_AND_SET_BIT_FIELD(ENDPOINT_FLAGS,
                                      &EndpointData->Flags,
                                      CancelReportedSinceMappingStart,
                                      &cancelAlreadyReported);

    if (cancelAlreadyReported == 0) {
        ESM_AddEsmEvent(EndpointData, EsmEventTransferCanceled);
    }
}

FORCEINLINE
VOID
Endpoint_TR_HaltedCompletionCodeReceived(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           CompletionCode,
    __in
        BOOLEAN         MatchingTransferFound
    )
/*

Routine Description:

    This routine queues HaltedCompletionCodeReceived event into the ESM. For
    Streams, a Transfer Event TRB with a halted completion code may not point to
    a specific transfer. In that case, all WDFQUEUEs are purged and outstanding
    transfers are completed with the halted completion code.

*/
{
    ULONG                   haltedCompletionCodeAlreadyReceived;
    PSTATIC_STREAMS_DATA    staticStreamsData;

    TRY {







        INTERLOCKED_GET_AND_SET_BIT_FIELD(ENDPOINT_FLAGS,
                                          &EndpointData->Flags,
                                          HaltedCompletionCodeReceived,
                                          &haltedCompletionCodeAlreadyReceived);

        if (haltedCompletionCodeAlreadyReceived != 0) {
            
            Controller_HwVerifierBreakIfEnabled(
                EndpointData->ControllerHandle,
                EndpointData->UcxUsbDevice,
                EndpointData->UcxEndpoint,
                ControllerHwVerifierDuplicateHaltedCompletionReceived,
                "Received duplicate Transfer Event TRB with Halted Completion Code",
                NULL,
                NULL);

            LEAVE;
        }

        if (MatchingTransferFound == FALSE) {

            NT_ASSERT(EndpointData->StreamsCapable);

            staticStreamsData = EndpointData->StaticStreamsData;

            staticStreamsData->HaltedCompletionCode = CompletionCode;

            INTERLOCKED_SET_BIT_FIELD(ENDPOINT_FLAGS, &EndpointData->Flags, PurgeTransferRingQueuesAfterHalt);
        }

        ESM_AddEsmEvent(EndpointData, EsmEventHaltedCompletionCodeReceived);

    } FINALLY {

    }
}

FORCEINLINE
BOOLEAN
Endpoint_TR_ShouldDropFSE(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    if (EndpointData->Flags.DropFSE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

FORCEINLINE
VOID
Endpoint_TR_FSEReceived(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    ULONG reportFSE;
    ULONG fseAlreadyReceived;

    TRY {

        //
        // Controller should not deliver FSE more than once, but this incorrect
        // behavior has been observed on some controller. Protect the State
        // Machine from seeing duplicate FSEReceived events.
        //
        INTERLOCKED_GET_AND_SET_BIT_FIELD(ENDPOINT_FLAGS,
                                          &EndpointData->Flags,
                                          FSEReceived,
                                          &fseAlreadyReceived);

        if (fseAlreadyReceived != 0) {
            
            Controller_HwVerifierBreakIfEnabled(
                EndpointData->ControllerHandle,
                EndpointData->UcxUsbDevice,
                EndpointData->UcxEndpoint,
                ControllerHwVerifierDuplicateHaltedCompletionReceived,
                "Received duplicate Stopped Transfer Events",
                NULL,
                NULL);

            LEAVE;
        }

        INTERLOCKED_GET_AND_TOGGLE_BIT_FIELD(ENDPOINT_FLAGS,
                                             &EndpointData->Flags,
                                             OkToReportFSE,
                                             &reportFSE);

        if (reportFSE) {
            ESM_AddEsmEvent(EndpointData, EsmEventFSEReceived);
        }

    } FINALLY {

    }
}

FORCEINLINE
VOID
Endpoint_TR_ExpectedEventTRBsProcessed(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    Endpoint_MUX_QueueEvent(EndpointData, EsmEventExpectedEventTRBsProcessed);
}

FORCEINLINE
VOID
Endpoint_TR_MappingStopped(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    Endpoint_MUX_QueueEvent(EndpointData, EsmEventMappingStopped);
}

FORCEINLINE
VOID
Endpoint_TR_TransfersReclaimed(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    Endpoint_MUX_QueueEvent(EndpointData, EsmEventTransfersReclaimed);
}

FORCEINLINE
VOID
Endpoint_ControllerResetStart(
    __in
        PENDPOINT_DATA EndpointData
    )
{
    ESM_AddEsmEvent(EndpointData, EsmEventControllerResetStart);
}

FORCEINLINE
VOID
Endpoint_ControllerResetComplete(
    __in
        PENDPOINT_DATA EndpointData
    )
{
    ULONG acknowledged;

    KeClearEvent(&EndpointData->ControllerResetCompleteAcknowledgeEvent);

    ESM_AddEsmEvent(EndpointData, EsmEventControllerResetComplete);

    INTERLOCKED_GET_AND_TOGGLE_BIT_FIELD(ENDPOINT_FLAGS,
                                         &EndpointData->Flags,
                                         ControllerResetCompleteAcknowledged,
                                         &acknowledged);

    if (acknowledged == 0) {
        KeWaitForSingleObject(&EndpointData->ControllerResetCompleteAcknowledgeEvent,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);
    }
}

FORCEINLINE
VOID
Endpoint_ControllerSurpriseRemoved(
    __in
        PENDPOINT_DATA EndpointData
    )
{
    ESM_AddEsmEvent(EndpointData, EsmEventControllerSurpriseRemoved);
}

//
// Private functions.
//

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_Disable_Internal(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        BOOLEAN         OnDeviceDisable
    );
