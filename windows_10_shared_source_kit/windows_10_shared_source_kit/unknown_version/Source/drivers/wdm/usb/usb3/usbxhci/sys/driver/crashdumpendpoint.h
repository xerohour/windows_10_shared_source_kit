/*++
Copyright (c) Microsoft Corporation

Module Name:

    crashdumpendpoint.h

Abstract:

    USB XHCI Crashdump write module.

--*/

//
// The Primary Stream Array contains 2**(MaxPStreams+1) entries so the minimum
// array size is 4 entries.
//
#define CRASHDUMP_MIN_STREAM_CONTEXT_ARRAY_SIZE     (4 * sizeof(STREAM_CONTEXT))

typedef struct _CRASHDUMP_ENDPOINT_DATA {

    HANDLE                  DumperHandle;
    HANDLE                  RegisterHandle;
    HANDLE                  CommandHandle;
    HANDLE                  UsbDeviceHandle;

    ULONG                   EndpointDCI;
    PENDPOINT_CONTEXT64     OutputEndpointContext;
    ENDPOINT_CONTEXT64      EndpointContext;

    CRASHDUMP_BUFFER_DATA   TrbSegment[2];

    BOOLEAN                 StreamsCapable;
    CRASHDUMP_BUFFER_DATA   StreamContextArray;

    ULONG                   CurrentSegmentIndex;

    LONGLONG                TransferTag;
    PURB                    LastUrb;
    TRANSFER_EVENT_TRB      LastTransferEvent;

} CRASHDUMP_ENDPOINT_DATA, *PCRASHDUMP_ENDPOINT_DATA;

//
// Public functions.
//
_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Crashdump_Endpoint_Initialize(
    __inout
        PCRASHDUMP_ENDPOINT_DATA    EndpointData,
    __in
        HANDLE                      DumperHandle,
    __in
        HANDLE                      UsbDeviceHandle,
    __in
        ULONG                       EndpointDCI,
    __in
        ULONG                       MaxTransferSize,
    __in
        PVOID                       DeviceContext
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_PrepareForHibernate(
    __in
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_SendControlTransfer(
    __in
        PCRASHDUMP_ENDPOINT_DATA        EndpointData,
    __in
        PUSB_DEFAULT_PIPE_SETUP_PACKET  SetupPacket,
    __in
        ULONG                           DataBufferLength,
    __in_opt
        PPHYSICAL_ADDRESS               DataBuffer
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_SendBulkTransfer(
    __in
        PCRASHDUMP_ENDPOINT_DATA        EndpointData,
    __in
        PURB_BULK_OR_INTERRUPT_TRANSFER BulkUrb,
    __in
        PPHYSICAL_ADDRESS               TransferPAArray,
    __in
        BOOLEAN                         IsTransferAsync
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_Cleanup(
    __in
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
PENDPOINT_CONTEXT64
Crashdump_Endpoint_GetEndpointContext(
    __in
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    )
{
    return &EndpointData->EndpointContext;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG
Crashdump_Endpoint_GetDequeueCycleState(
    __in
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    )
{
    UNREFERENCED_PARAMETER(EndpointData);

    return 1;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG64
Crashdump_Endpoint_GetStreamOrTransferRingLA(
    __in
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    )
{
    if (EndpointData->StreamsCapable) {
        return EndpointData->StreamContextArray.LogicalAddress.QuadPart;
    } else {
        return EndpointData->TrbSegment[EndpointData->CurrentSegmentIndex].LogicalAddress.QuadPart;
    }
}

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG
Crashdump_Endpoint_GetStreamId(
    __in
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    )
{
    if (EndpointData->StreamsCapable) {
        return 1;
    } else {
        return 0;
    }
}

//
// Private functions.
//
_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_StopEndpoint(
    __in
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_SetDequeuePointer(
    __in
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_SendTransferAndPollForCompletion(
    __inout
        PCRASHDUMP_ENDPOINT_DATA    EndpointData,
    __in
        ULONG                       RetryCount
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG
Crashdump_Endpoint_GetEndpointState(
    __in
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    )
{
    return EndpointData->OutputEndpointContext->EndpointState;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Endpoint_EventRingCallback(
    __inout
        PCRASHDUMP_ENDPOINT_DATA    EndpointData,
    __in
        PTRB                        Trb,
    __out
        BOOLEAN                     *Continue
    );

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Crashdump_Endpoint_SetOutputEndpointContext(
    __inout
        PCRASHDUMP_ENDPOINT_DATA    EndpointData,
    __in
        PVOID                       DeviceContext
    )
{
    if (Crashdump_Register_GetContextSize(EndpointData->RegisterHandle) == REGISTER_CONTEXTSIZE64) {

        EndpointData->OutputEndpointContext = 
            UsbDevice_DeviceContext64GetEndpoint((PDEVICE_CONTEXT64)DeviceContext, EndpointData->EndpointDCI);

    } else {

        EndpointData->OutputEndpointContext = 
            (PENDPOINT_CONTEXT64)
                UsbDevice_DeviceContext32GetEndpoint((PDEVICE_CONTEXT32)DeviceContext, EndpointData->EndpointDCI);
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
VOID
Crashdump_Endpoint_SetEndpointContext(
    __inout
        PCRASHDUMP_ENDPOINT_DATA    EndpointData,
    __in
        PVOID                       DeviceContext
    )
{
    if (Crashdump_Register_GetContextSize(EndpointData->RegisterHandle) == REGISTER_CONTEXTSIZE64) {

        RtlCopyMemory(&EndpointData->EndpointContext,
                      UsbDevice_DeviceContext64GetEndpoint((PDEVICE_CONTEXT64)DeviceContext, EndpointData->EndpointDCI),
                      sizeof(ENDPOINT_CONTEXT64));

    } else {

        RtlCopyMemory(&EndpointData->EndpointContext,
                      UsbDevice_DeviceContext32GetEndpoint((PDEVICE_CONTEXT32)DeviceContext, EndpointData->EndpointDCI),
                      sizeof(ENDPOINT_CONTEXT32));
    }
}

FORCEINLINE
VOID
Crashdump_Endpoint_ReInitializeForMultiStageResume(
    __inout
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    )
{
    EndpointData->TransferTag = 1;
    EndpointData->CurrentSegmentIndex = 0;
}

FORCEINLINE
PURB
Crashdump_Endpoint_GetLastURB(
    __inout
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    )
{
    return EndpointData->LastUrb;
}

FORCEINLINE
VOID
Crashdump_Endpoint_DisableLastTransfer(
    __inout
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    )
{
    PTRB                trbArray;

    //
    // The segment index was toggled at the end of the transfer
    //
    if (EndpointData->CurrentSegmentIndex == 0) {
        trbArray = (PTRB)EndpointData->TrbSegment[1].VirtualAddress;
    } else {
        trbArray = (PTRB)EndpointData->TrbSegment[0].VirtualAddress;
    }
    
    trbArray[0].TransferEvent.CycleBit = 0;
}

FORCEINLINE
VOID
Crashdump_Endpoint_IncrementTransferTag(
    __inout
        PCRASHDUMP_ENDPOINT_DATA    EndpointData
    )
{
    EndpointData->TransferTag++;
}

