// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"


DWORD Test_GraphPathology_SRC()
{
    AUDIO_FRAME_FORMAT frameFormatLocal;
    HRESULT hr;

    // local format
    BUILD_FORMAT(&frameFormatLocal, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 48000.00);

    // local graph apo's
    GRAPH_APO_DESCRIPTOR aApoDescriptors[] = { { APO_SRC, NULL } };

    // bindings between apo's and connections
    GRAPH_BINDING_DESCRIPTOR aBindingDescriptors[] = { { 0, 0, true  }, { 0, 1, false } };

    // connection descriptors
    GRAPH_CONNECTION_DESCRIPTOR aConnectionDescriptors[2]; 
    
	ZeroMemory(aConnectionDescriptors, sizeof(aConnectionDescriptors));
    CopyMemory(&aConnectionDescriptors[0].FrameFormat, &frameFormatLocal, sizeof(frameFormatLocal)); 
    CopyMemory(&aConnectionDescriptors[1].FrameFormat, &frameFormatLocal, sizeof(frameFormatLocal)); 

    aConnectionDescriptors[0].ConnectionDescriptor.u32MaxFrameCount =  800;   
    aConnectionDescriptors[1].ConnectionDescriptor.u32MaxFrameCount =  400;
    //aConnectionDescriptors[0].ConnectionDescriptor.u32ExtraFrameCount =  4;   
    //aConnectionDescriptors[1].ConnectionDescriptor.u32ExtraFrameCount =  0;


    //
    // now build the local graph
    CGraphRepresentation graph(aConnectionDescriptors,
                            sizeof(aConnectionDescriptors)/sizeof(aConnectionDescriptors[0]), 
                            aBindingDescriptors,
                            sizeof(aBindingDescriptors)/sizeof(aBindingDescriptors[0]), 
                            aApoDescriptors,
                            sizeof(aApoDescriptors)/sizeof(aApoDescriptors[0]),                                     
                            (UINT32)frameFormatLocal.f32FramesPerSecond*10,  
                            (UINT32)frameFormatLocal.f32FramesPerSecond*10);  

    graph.BuildGraph();

//    if (SUCCEEDED(hr = graph.BuildGraph()))
//    {
//        SLOG(eError, "ERROR: Should not be able to use SRC as the first APO in the graph");
//        return FNS_FAIL;
//    }

    return FNS_PASS;
}

DWORD Test_GraphPathology_Inplace()
{
    AUDIO_FRAME_FORMAT frameFormatLocal;
    HRESULT hr;

    // local format
    BUILD_FORMAT(&frameFormatLocal, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 48000.00);

    // local graph apo's
    GRAPH_APO_DESCRIPTOR aApoDescriptors[] = { { APO_VOLUME, NULL } };

    // bindings between apo's and connections
    GRAPH_BINDING_DESCRIPTOR aBindingDescriptors[] = { { 0, 0, true  }, { 0, 0, false } };

    // connection descriptors
    GRAPH_CONNECTION_DESCRIPTOR aConnectionDescriptors[1]; 
    
	ZeroMemory(aConnectionDescriptors, sizeof(aConnectionDescriptors));
    CopyMemory(&aConnectionDescriptors[0].FrameFormat, &frameFormatLocal, sizeof(frameFormatLocal)); 

    aConnectionDescriptors[0].ConnectionDescriptor.u32MaxFrameCount =  400;

    //
    // now build the local graph
    CGraphRepresentation graph(aConnectionDescriptors,
                            sizeof(aConnectionDescriptors)/sizeof(aConnectionDescriptors[0]), 
                            aBindingDescriptors,
                            sizeof(aBindingDescriptors)/sizeof(aBindingDescriptors[0]), 
                            aApoDescriptors,
                            sizeof(aApoDescriptors)/sizeof(aApoDescriptors[0]),                                     
                            (UINT32)frameFormatLocal.f32FramesPerSecond*10,  
                            (UINT32)frameFormatLocal.f32FramesPerSecond*10);  

    if (SUCCEEDED(hr = graph.BuildGraph()))
    {
        SLOG(eError, "ERROR: Should not be able to build a graph with only one in-place APO");
        return FNS_FAIL;
    }

    return FNS_PASS;
}

