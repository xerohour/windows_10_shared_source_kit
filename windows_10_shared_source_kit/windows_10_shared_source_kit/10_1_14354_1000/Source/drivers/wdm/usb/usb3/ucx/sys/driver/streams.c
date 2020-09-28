/*++
Copyright (c) Microsoft Corporation

Module Name:

    streams.c

Abstract:

    USB Controller Extension Driver

--*/
#include "pch.h"

#include "streams.tmh"

__drv_requiresIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
StaticStreams_Create(
    __in 
        PUCXENDPOINT_PRIVCONTEXT        EndpointContext,
    __in 
        ULONG                           NumberOfStreams,
    __out 
        UCXSSTREAMS                    *SStreams
    )
/*++

Routine Description:

    This function is called to create a Ucx Static Streams object
        
Arguments:

    Endpoint - The handle to the UCXENDPOINT for which the streams is being 
        created. 
        
    SStreams - Output - A pointer to a location that recieves a handle to the 
        created streams object

Return Value: 
    
    NT_SUCCESS status if a default object is successfully created
    
--*/
{
    NTSTATUS                  status;
    UCXSSTREAMS_INIT          ucxSStreamsInit = {0};
    PUCXSSTREAMS_PRIVCONTEXT  ucxSStreamsContext;
    ULONG                     arrayIndex;

    TRY {
        
        RtlZeroMemory(&ucxSStreamsInit, sizeof(ucxSStreamsInit));

        ucxSStreamsInit.Sig = SigUcxSStreamsInit;
        ucxSStreamsInit.UcxEndpoint = EndpointContext->UcxEndpoint;
        ucxSStreamsInit.NumberOfStreams = NumberOfStreams;

        status = EndpointContext->EvtCallbacks.EvtEndpointStaticStreamsAdd(
            EndpointContext->UcxEndpoint,
            NumberOfStreams,
            &ucxSStreamsInit);

        if (!NT_SUCCESS(status)) {
            LogError(FlagStreams, "EvtEndpointStreamsAdd Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // Ensure that the HCD has set StreamIDs and Queues for all streams. 
        //
        ucxSStreamsContext = UcxStaticStreamsGetContext(ucxSStreamsInit.UcxSStreams);

        //
        // We no longer need the Init Structure pointer in streams context
        //
        ucxSStreamsContext->Init = NULL;
        if (ucxSStreamsInit.FailCreate) {
            status = STATUS_INTERNAL_ERROR;
            LogError(FlagStreams, "Failing StaticStreams_Create since HCD did an error while calling UcxStaticStreamsSetStreamInfo, SStream Context %p", 
                     ucxSStreamsContext);
            Controller_VerifierBreak(EndpointContext->UcxControllerContext, "HCD did an error while calling UcxStaticStreamsSetStreamInfo");
            LEAVE;
        }

        for (arrayIndex = 0; arrayIndex < ucxSStreamsContext->NumberOfStreams; arrayIndex++){
            if (ucxSStreamsContext->StreamContexts[arrayIndex].Sig != SigStreamContext) {
                LogError(FlagStreams, "Failing StaticStreams_Create since HCD did not call UcxStaticStreamsSetStreamInfo enough number of times %p",
                         ucxSStreamsContext);
                status = STATUS_INTERNAL_ERROR;
                Controller_VerifierBreak(EndpointContext->UcxControllerContext, "HCD did not call UcxStaticStreamsSetStreamInfo enough number of times");
                LEAVE;
            }
        }

    } FINALLY {

        if (NT_SUCCESS(status)) {
            
            *SStreams = ucxSStreamsInit.UcxSStreams;
        
        } else {

            *SStreams = NULL;
            
            if (ucxSStreamsInit.UcxSStreams != NULL) {
                WdfObjectDelete(ucxSStreamsInit.UcxSStreams);
            }

        }

    }

    return status;
}

VOID
EXPORT(UcxStaticStreamsSetStreamInfo) (
    __in_opt 
        PUCX_GLOBALS            Globals,
    __in 
        UCXSSTREAMS             SStreams,
    __in    
        PSTREAM_INFO            StreamInfo
)
/*++

Routine Description:

    After the streams object has been created the HCD must set the necessarly 
    information for each streams. 
    This routine must be called from within EvtEndpointStreamsAdd. 
    This routine must be called for exactly the NumberOfStreams times. 
    
Arguments:

    Globals - Driver globals of the controller driver. This information is not
        being used. 

    SStreams - The handle to the Static Streams object just been created. 
    
    StreamInfo - Pointer to the stream info
        
Return Value:

    VOID    
--*/
{
    PUCXSSTREAMS_PRIVCONTEXT  ucxSStreamsContext;
    PUCXENDPOINT_PRIVCONTEXT  ucxEndpointContext;
    ULONG                     streamInfoArrayIndex;
    PUCXSSTREAMS_INIT         sStreamsInit;
    PSTREAM_CONTEXT           streamContext;

    UNREFERENCED_PARAMETER(Globals);

    TRY {

        ucxSStreamsContext = UcxStaticStreamsGetContext(SStreams);
        ucxEndpointContext = ucxSStreamsContext->UcxEndpointContext;
        
        if (ucxSStreamsContext->Init == NULL) {
            LogError(FlagStreams, "UcxStreamsSetStreamInfo can only be called from within EvtEndpointStreamsAdd");
            Controller_VerifierBreak(ucxEndpointContext->UcxControllerContext, "UcxStreamsSetStreamInfo can only be called from within EvtEndpointStreamsAdd");
            LEAVE;
        }
        sStreamsInit = ucxSStreamsContext->Init;

        //
        // Validate the size of the structure. For any future minor versions, 
        // this check should include checks for all previous sizes. 
        //
        if (StreamInfo->Size != sizeof(STREAM_INFO)) {
            sStreamsInit->FailCreate = TRUE;
            LogError(FlagStreams, "StreamInfo has invalid Size. Expected %d, Actual %d", sizeof(STREAM_INFO), StreamInfo->Size);
            Controller_VerifierBreak(ucxEndpointContext->UcxControllerContext, "Invalid Size of StreamInfo");
            LEAVE;
        }

        streamInfoArrayIndex = sStreamsInit->StreamInfoArrayIndex;
        sStreamsInit->StreamInfoArrayIndex++;
        
        
        if (sStreamsInit->StreamInfoArrayIndex > ucxSStreamsContext->NumberOfStreams) {
            sStreamsInit->FailCreate = TRUE;
            Controller_VerifierBreak(ucxEndpointContext->UcxControllerContext, "HCD Called UcxStreamsInitSetStreamInfo too many times");
            LEAVE;
        }

        if (streamInfoArrayIndex + 1 != StreamInfo->StreamId) {
            sStreamsInit->FailCreate = TRUE;
            LogError(FlagStreams, "StreamId should be sequential starting at 1");
            Controller_VerifierBreak(ucxEndpointContext->UcxControllerContext, "StreamId should be sequential starting at 1");
            LEAVE;
        }

        if (NULL == StreamInfo->WdfQueue) {
            sStreamsInit->FailCreate = TRUE;
            LogError(FlagStreams, "StreamInfo->WdfQueue cannot be NULL");
            Controller_VerifierBreak(ucxEndpointContext->UcxControllerContext, "StreamInfo->WdfQueue cannot be NULL");
            LEAVE;
        }
        
        streamContext = & ucxSStreamsContext->StreamContexts[streamInfoArrayIndex];
        
        streamContext->Sig = SigStreamContext;
        streamContext->Index = streamInfoArrayIndex;
        streamContext->UcxSStreamsContext = ucxSStreamsContext;
        streamContext->StreamPipeInfo.EndpointTransferType = EndpointTransferTypeBulk;
        streamContext->StreamPipeInfo.IsEndpointDirectionIn 
                  = ucxEndpointContext->UcxPipeInfo.IsEndpointDirectionIn;
        streamContext->StreamPipeInfo.IsStreamPipe = TRUE;
        streamContext->StreamPipeInfo.MaximumTransferSize 
            = ucxEndpointContext->UcxPipeInfo.MaximumTransferSize;
        WdfObjectReferenceWithTag(StreamInfo->WdfQueue, TAG(EXPORT(UcxStaticStreamsSetStreamInfo)));
        streamContext->StreamPipeInfo.Queue = StreamInfo->WdfQueue;            
        streamContext->StreamPipeInfo.Self = &streamContext->StreamPipeInfo;
        streamContext->StreamID = StreamInfo->StreamId;

    } FINALLY {

    }
}

VOID
StaticStreams_EvtDestroyCallback(
    __in 
        WDFOBJECT UcxSStreams
   )
/*++

Routine Description:

    This is an Event Callback function called when the endpoint object's 
    reference count go to 0
    
Arguments:

    UcxEndpoint - Handle to the endpoint object

--*/
{
    PUCXSSTREAMS_PRIVCONTEXT  ucxSStreamsContext;
    ULONG                     arrayIndex;

    TRY {
        ucxSStreamsContext = UcxStaticStreamsGetContext(UcxSStreams);
        
        for (arrayIndex = 0; arrayIndex <  ucxSStreamsContext->NumberOfStreams; arrayIndex++) {

            //
            // Mark stream as invalid.
            //
            ucxSStreamsContext->StreamContexts[arrayIndex].StreamPipeInfo.Self = NULL;

            if (ucxSStreamsContext->StreamContexts[arrayIndex].StreamPipeInfo.Queue != NULL) {
                WdfObjectDereferenceWithTag(
                    ucxSStreamsContext->StreamContexts[arrayIndex].StreamPipeInfo.Queue,
                    TAG(EXPORT(UcxStaticStreamsSetStreamInfo)));
                ucxSStreamsContext->StreamContexts[arrayIndex].StreamPipeInfo.Queue = NULL;
            }
        }
    
    } FINALLY {

    }

}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
EXPORT(UcxStaticStreamsCreate) (
    __in_opt 
        PUCX_GLOBALS               Globals,
    __in 
        UCXENDPOINT                Endpoint,
    __deref_inout_opt 
        PUCXSSTREAMS_INIT         *SStreamsInit,
    __in_opt
        PWDF_OBJECT_ATTRIBUTES     Attributes,
    __out
        UCXSSTREAMS               *SStreams
   )
/*++

Routine Description:

    This export function creates an streams object 
    
Arguments:

    Globals - Driver globals of the controller driver. This information is not
        being used. 
    
    Endpoint - The handle to the Endpoint for which the streams is being
        created
    
    SStreamsInit - Pointer to an init structure describing various
        information for the streams creation.
    
    Attributes - Pointer to the object attributes structure 
    
    SStreams - Output - Pointer to a location that will recieve a handle to the 
        streams object

Return Value:

    NT_SUCCESS status if a streams object is  successfully created
    
--*/
{
    NTSTATUS                          status;
    UCXSSTREAMS                       ucxSStreams = NULL;
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;
    PUCXSSTREAMS_PRIVCONTEXT          ucxSStreamsContext;
    WDF_OBJECT_ATTRIBUTES             objectAttributes;

    UNREFERENCED_PARAMETER(Globals);

    TRY {
        *SStreams = NULL;

        //
        // Parameter Validation
        //
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&objectAttributes,
                                                UCXSSTREAMS_PRIVCONTEXT);
        //
        // Since the UCXSTREAMS_PRIVCONTEXT has a variable size array at the end, 
        // we need to adjust the size of the context by setting the ContextSizeOverride 
        // parameter
        //
        objectAttributes.ContextSizeOverride = 
            sizeof(UCXSSTREAMS_PRIVCONTEXT) 
            + ((*SStreamsInit)->NumberOfStreams * sizeof(STREAM_CONTEXT));

        objectAttributes.ParentObject = (*SStreamsInit)->UcxEndpoint;
        objectAttributes.EvtDestroyCallback = StaticStreams_EvtDestroyCallback; 
        
        status = Ucx_WdfObjectCreateWith2Attributes(&objectAttributes, 
                                                    Attributes,
                                                    &ucxSStreams);
        
        if (!NT_SUCCESS(status)) { 
            LogError(FlagStreams, "WdfObjectCreate Failed %!STATUS!", status);
            LEAVE;
        }

        ucxSStreamsContext = UcxStaticStreamsGetContext(ucxSStreams);

        ucxEndpointContext = UcxEndpointGetContext(Endpoint);

        LogInfo(FlagStreams, "UcxEndpoint 0x%p, UcxStreams 0x%p, UcxStreamsContext 0x%p", 
                Endpoint, ucxSStreams, ucxSStreamsContext);

        ucxSStreamsContext->Sig = SigUcxStreamsContext;
        ucxSStreamsContext->NumberOfStreams = (*SStreamsInit)->NumberOfStreams;
        ucxSStreamsContext->StreamContexts = (PSTREAM_CONTEXT)(ucxSStreamsContext+1);

        ucxSStreamsContext->UcxSStreams = ucxSStreams;
        ucxSStreamsContext->UcxEndpointContext = ucxEndpointContext;
        ucxSStreamsContext->Init = (*SStreamsInit);
        
    } FINALLY {

        if (!NT_SUCCESS(status)) {
            
            if (ucxSStreams != NULL) {
                WdfObjectDereferenceWithTag(ucxSStreams, TAG(EXPORT(UcxStaticStreamsCreate)));
                WdfObjectDelete(ucxSStreams);
            }

        } else {
            *SStreams = ucxSStreams;
            (*SStreamsInit)->UcxSStreams = ucxSStreams;
            *SStreamsInit = NULL;
        }
        
        LogVerbose(FlagStreams, "UcxStreamsCreate 0x%x", status);

    }

    return status;
}


