// Copyright(C) Microsoft. All rights reserved.

//
// SecurityTests.cpp
//  
//
// Description:
//
//  WASAPI / Audio Engine Security Tests

#include <stdafx.h>
#include <signal-source.h>
#include <dithering.h>
#include <sine-signal-source.h>
#include <nan.h>
#include <UMATestStreaming.h>


using namespace NotANumber;

const static UINT32         DEFAULT_RENDER_DURATION_MS = 3000;
const static FLOAT          DEFAULT_RENDER_AMPLITUDE = 0.01f; // render nice and quiet-like
const static FLOAT          DEFAULT_RENDER_FREQUENCY = 1000.0f;

//------------------------------------------------------------------------------
// Name: TC_Security_InvalidFloatStreaming
// Desc: Stream various invalid FLOAT values using WASAPI shared mode
//
// Arguments:
//      N/A
//         
// Return value:
//      FNS_PASS.
//      Test failure is signalled via XLOG.
////
//------------------------------------------------------------------------------
DWORD TC_Security_InvalidFloatStreaming()
{
    HRESULT hr = S_OK;
    AudioClientProperties clientProperties = {0};
    CComPtr<ITestStreamingEngine> spStreamingEngine;
    CComPtr<IRenderTestStreamingEngine> spRenderStreamingEngine;

    LPCWSTR pwszEndpointUnderTest = NULL;
    CComPtr<IBasicLog> pobjLogger = NULL;

    WAVEFORMATEX* pwfxStreamingFormat = NULL;
    size_t cbRenderBufferSize = 0;
    PBYTE pbRenderBuffer = NULL;
    FLOAT32* pfRenderBuffer = NULL; // points to the same buffer
    UINT32 nRenderBufferFrames = 0;
    
    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    hr = g_IShell->GetBasicLog(&pobjLogger);
    
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        goto END;
    }

    // Ensure we have a valid endpoint
    if(DEVICE_TYPE_RENDER != GetDeviceTypeUnderTest())
    {
        XLOG(XFAIL, eError, "FAIL: Endpoint under test does not support render!");
        goto END;
    }

    // Get the endpoint ID
    pwszEndpointUnderTest = GetEndpointIDUnderTest();
    if(NULL == pwszEndpointUnderTest)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain current endpoint ID");
        goto END;
    }

    // Create the streaming engine
    METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId",
                      hr,
                      CreateTestStreamingEngineOnEndpointId(IID_IRenderTestStreamingEngine,
                                                pwszEndpointUnderTest,
                                                EVENT_PULL_RENDER,
                                                pobjLogger,
                                                (void**)&spRenderStreamingEngine));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create test streaming engine!");
        goto END;
    }

    // Get the ITestStreamingEngine interface                                                
    METHODCHECKRESULT("IRenderTestStreamingEngine::QueryInterface",
                      hr,
                      spRenderStreamingEngine->QueryInterface(IID_ITestStreamingEngine,
                                                              (void**)&spStreamingEngine));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to QI for ITestStreamingEngine");   
        goto END;
    }   

    METHODCHECKRESULT("ITestStreamingEngine::SetRequestedShareMode",
                      hr,
                      spStreamingEngine->SetRequestedShareMode(AUDCLNT_SHAREMODE_SHARED));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to set requested share mode");
        goto END;
    }

    // enable stream checking
    METHODCHECKRESULT("ITestStreamingEngine::SetStreamingVerificationState",
                      hr,
                      spStreamingEngine->SetStreamingVerificationState(TRUE));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to enable streaming verification");
        goto END;
    }   

    // Initialize the engine
    METHODCHECKRESULT("ITestStreamingEngine::Initialize()",
                      hr,
                      spStreamingEngine->Initialize(&clientProperties));

    if(hr == UTS_E_OFFLOAD_NOT_SUPPORTED)
    {
        XLOG(XSKIP, eError, "SKIP: Render Endpoint does not support offload");
        goto SKIPPED;
    }

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "ERROR: Unable to initialize streaming engine");
        goto END;
    }     

    // Get the streaming format
    METHODCHECKRESULT("ITestStreamingEngine::GetRequestedStreamingFormat",
                      hr,
                      spStreamingEngine->GetRequestedStreamingFormat(&pwfxStreamingFormat));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain mix format");
        goto END;
    }    
    
    // Allocate a buffer and populate it with a sine tone
    nRenderBufferFrames = (UINT32)DEFAULT_RENDER_DURATION_MS *
                          pwfxStreamingFormat->nSamplesPerSec / 1000;
    cbRenderBufferSize = pwfxStreamingFormat->nBlockAlign * nRenderBufferFrames;

    pbRenderBuffer = new BYTE[cbRenderBufferSize];
    if(NULL == pbRenderBuffer)
    {
        XLOG(XFAIL, eError, "FAIL: Out of memory!");
        goto END;
    }

    pfRenderBuffer = reinterpret_cast<FLOAT32*>(pbRenderBuffer);

    SLOG(eInfo1, 
         "INFO: Allocated %d ms (%Id byte) buffer",
         DEFAULT_RENDER_DURATION_MS,
         cbRenderBufferSize);

    METHODCHECKRESULT("FillBufferWithSineSignal",
                      hr,
                      FillBufferWithSineSignal(pobjLogger,
                                               XFAIL,
                                               pwfxStreamingFormat,
                                               DEFAULT_RENDER_AMPLITUDE, // amplitude
                                               DEFAULT_RENDER_FREQUENCY, // frequency
                                               0.0, // initial phase,
                                               0.0, // dc
                                               Method_NoDithering,
                                               pbRenderBuffer,
                                               nRenderBufferFrames,
                                               static_cast<UINT32>(cbRenderBufferSize)));

    if (FAILED(hr)) {
        XLOG(XFAIL,
             eError,
             "FAIL: Error generating sine tone");
        goto END;
    }

    //
    // Inject some bad data
    //
    SLOG(eInfo1, 
         "INFO: Using random seed %u to generate bad data",
         g_IShell->GetRandomSeed());
         

    // very large negative finite numbers
    for(UINT32 i=0;i<100;i++)
    {
        FLOAT fVal = 0.0f;
        
        // tweak a valid random sample
        UINT32 nSampleOffset = (g_IShell->Random()) % ((UINT32)cbRenderBufferSize / sizeof(FLOAT32));        

        fVal = -9999999.0f * static_cast<float>(g_IShell->Random()/sizeof(DWORD));

        pfRenderBuffer[nSampleOffset] = fVal;
    }

    // very large finite numbers
    for(UINT32 i=0;i<100;i++)
    {
        FLOAT fVal = 0.0f;
        
        // tweak a valid random sample
        UINT32 nSampleOffset = (g_IShell->Random()) % ((UINT32)cbRenderBufferSize / sizeof(FLOAT32));

        fVal = 9999999.0f * static_cast<float>(g_IShell->Random()/sizeof(DWORD));

        pfRenderBuffer[nSampleOffset] = fVal;   
    }

    // plus infinity
    for(UINT32 i=0;i<100;i++)
    {
        FLOAT fVal = 0.0f;

        // tweak a valid random sample
        UINT32 nSampleOffset = (g_IShell->Random()) % ((UINT32)cbRenderBufferSize / sizeof(FLOAT32));

        GetFloat32Infinity(&fVal);
            
        pfRenderBuffer[nSampleOffset] = fVal;
    }

    // minus infinity
    for(UINT32 i=0;i<100;i++)
    {
        FLOAT fVal = 0.0f;

        // tweak a valid random sample
        UINT32 nSampleOffset = (g_IShell->Random()) % ((UINT32)cbRenderBufferSize / sizeof(FLOAT32));

        GetFloat32NegInfinity(&fVal);
            
        pfRenderBuffer[nSampleOffset] = fVal;
    }

    // negative 0
    for(UINT32 i=0;i<100;i++)
    {
        FLOAT fVal = 0.0f;

        // tweak a valid random sample
        UINT32 nSampleOffset = (g_IShell->Random()) % ((UINT32)cbRenderBufferSize / sizeof(FLOAT32));

        GetFloat32NegZero(&fVal);
            
        pfRenderBuffer[nSampleOffset] = fVal;    
    }

    // signaling NAN
    for(UINT32 i=0;i<100;i++)
    {
        FLOAT fVal = 0.0f;

        // tweak a valid random sample
        UINT32 nSampleOffset = (g_IShell->Random()) % ((UINT32)cbRenderBufferSize / sizeof(FLOAT32));

        GetFloat32SignalingNaN(&fVal);
            
        pfRenderBuffer[nSampleOffset] = fVal;
    }

    // Quiet NAN
    for(UINT32 i=0;i<100;i++)
    {
        FLOAT fVal = 0.0f;
        
        // tweak a valid random sample
        UINT32 nSampleOffset = (g_IShell->Random()) % ((UINT32)cbRenderBufferSize / sizeof(FLOAT32));

        GetFloat32QuietNaN(&fVal);
            
        pfRenderBuffer[nSampleOffset] = fVal;    
    }

    
    // Stream the buffer
    METHODCHECKRESULT("RenderTestStreamingEngine::RenderBuffer",
                      hr,                      
                      spRenderStreamingEngine->RenderBuffer(pbRenderBuffer,
                                                            cbRenderBufferSize,
                                                            nRenderBufferFrames,
                                                            pwfxStreamingFormat,
                                                            FALSE,
                                                            0));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error streaming sine tone");
        goto END;
    }
    
END:
    // engine stops streaming automatically if something went wrong
    SAFE_COTASKMEMFREE(pwfxStreamingFormat);
    SAFE_DELETE_ARRAY(pbRenderBuffer);
    return FNS_PASS;

SKIPPED:
    return FNS_SKIPPED;
}
