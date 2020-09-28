//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once 

#define MAX_STREAMS 255

typedef struct _STREAM_CONTEXT {
    UCX_SIG                           Sig;
    ULONG                             Index;
    PUCXSSTREAMS_PRIVCONTEXT          UcxSStreamsContext;
    UCXPIPE_INFO                      StreamPipeInfo;
    ULONG                             StreamID;
} STREAM_CONTEXT, *PSTREAM_CONTEXT;

typedef struct _UCXSSTREAMS_INIT *PUCXSSTREAMS_INIT;

typedef struct _UCXSSTREAMS_PRIVCONTEXT {
    UCX_SIG                  Sig;
    ULONG                    NumberOfStreams;               
    UCXSSTREAMS              UcxSStreams;
    PUCXENDPOINT_PRIVCONTEXT UcxEndpointContext; 
    PUCXSSTREAMS_INIT        Init;
    //
    // Adding a variable sized StreamInfo Field.
    //
    PSTREAM_CONTEXT          StreamContexts;
    // Please do not add any fileds here.
} UCXSSTREAMS_PRIVCONTEXT, *PUCXSSTREAMS_PRIVCONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(UCXSSTREAMS_PRIVCONTEXT, 
                                   UcxStaticStreamsGetContext);

typedef struct _UCXSSTREAMS_INIT {
    UCX_SIG                 Sig;
    UCXSSTREAMS             UcxSStreams;
    UCXENDPOINT             UcxEndpoint;
    ULONG                   NumberOfStreams;    
    ULONG                   StreamInfoArrayIndex;
    BOOLEAN                 FailCreate;
} UCXSSTREAMS_INIT, *PUCXSSTREAMS_INIT;

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
    );

EVT_WDF_OBJECT_CONTEXT_DESTROY StaticStreams_EvtDestroyCallback;
