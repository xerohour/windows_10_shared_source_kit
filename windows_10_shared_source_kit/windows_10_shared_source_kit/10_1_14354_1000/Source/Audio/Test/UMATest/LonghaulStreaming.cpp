// Copyright(C) Microsoft. All rights reserved.

//
// LonghaulStreaming.cpp
//  
//
// Description:
//
//  WASAPI Longhaul Streaming Tests

#include <stdafx.h>
#include <UMATestStreaming.h>


const DOUBLE TEST_TONE_FREQ = 500.0;    //  Use a 500Hz test tone
const DOUBLE TEST_TONE_AMP  = 0.01;      //  Play nice and quiet-like

//------------------------------------------------------------------------------
// Name: Tc_RenderLonghaul
// Desc: Stream indefinitely until the test is aborted, reporting any errors.
//
// Returns:
//              FNS_PASS; pass status determined via XLOG
//------------------------------------------------------------------------------
DWORD Tc_RenderLonghaul()
{
    HRESULT hr = S_OK;
    AudioClientProperties               clientProperties = {0};
    CComPtr<IBasicLog>                  spLogger = NULL;

    LPCWSTR                             pwszEndpointUnderTest;

    CComPtr<ITestStreamingEngine>       spStreamingEngine = NULL;
    CComPtr<IRenderTestStreamingEngine> spRenderEngine = NULL;

    CHandle                             shCompletionEvent;

    clientProperties.cbSize = sizeof(clientProperties);

    //  Get ourselves a logger.
    hr = g_IShell->GetBasicLog(&spLogger);    
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        return FNS_PASS;
    }

    //  Ensure we have a render endpoint
    if ((DEVICE_TYPE_RENDER != GetDeviceTypeUnderTest())&&(DEVICE_TYPE_RENDEROFFLOAD != GetDeviceTypeUnderTest()))
    {
        XLOG(XFAIL, eError, "FAIL: Endpoint under test does not support render!");
        return FNS_PASS;
    }

    // enable offload, if appropriate
    clientProperties.bIsOffload = ( DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest() ? TRUE : FALSE );
    clientProperties.eCategory = ( DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest() ? AudioCategory_Media : AudioCategory_Other );

    //  Get the endpoint ID
    pwszEndpointUnderTest = GetEndpointIDUnderTest();
    if (pwszEndpointUnderTest == NULL)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain current endpoint ID");
        return FNS_PASS;
    }

    HANDLE hCompletionEvent = NULL;
    //  Create the event used to signal streaming completion
    if (NULL == (hCompletionEvent = CreateEvent(NULL,       //  Event Attribs
                                                 TRUE,       //  Manual reset event
                                                 FALSE,      //  Default to non-signalled
                                                 NULL        //  Name
                                                 )))
    {
        XLOG(XFAIL, eError, "FAIL: CreateEvent returned NULL (Error Code: %d)", GetLastError());
        return FNS_PASS;
    }
    else
    {
        shCompletionEvent.Attach(hCompletionEvent);
    }

    //  Instantiate a render streaming engine
    METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId",
                      hr,
                      CreateTestStreamingEngineOnEndpointId(__uuidof(IRenderTestStreamingEngine),
                                                pwszEndpointUnderTest,
                                                TIMER_PUSH_RENDER,
                                                spLogger,
                                                reinterpret_cast<void**>(&spRenderEngine)));
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create test streaming engine!");
        return FNS_PASS;
    }

    //  Get an ITestStreamingEngine interface
    METHODCHECKRESULT("IRenderTestStreamingEngine::QueryInterface",
                      hr,
                      spRenderEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                     reinterpret_cast<void**>(&spStreamingEngine)));
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to QI for ITestStreamingEngine");
        return FNS_PASS;
    }

    //  Initialize the streaming engine
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
        SLOG(eError, "ERROR: Unable to initialize streaming engine");
        return FNS_PASS;
    }     

    // Start streaming the tone.
    METHODCHECKRESULT("IRenderTestStreamingEngine::RenderSineTone (async)",
                      hr,
                      spRenderEngine->RenderSineTone(TEST_TONE_FREQ,
                                                     TEST_TONE_AMP,
                                                     0,
                                                     DITHER_TYPE_TPD,
                                                     shCompletionEvent));
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error streaming sine tone!");
        return FNS_PASS;
    }

    while (true)
    {
        DWORD   dwWaitResult = 0;

        //  Check to see if termination of the application was requested.
        if (g_IShell->RequestAbort())
        {
            SLOG(eInfo1, "Test termination requested, shutting down");

            METHODCHECKRESULT("IRenderTestStreamingEngine::StopStreaming",
                              hr,
                              spRenderEngine->StopStreaming());
            if (FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Error reported while stopping streaming");
                return FNS_PASS;
            }

            //  all done
            break;
        }

        //  Periodically check to see if the streaming completion event was signalled
        if (WAIT_TIMEOUT != (dwWaitResult = WaitForSingleObject(shCompletionEvent, 100)))
        {
            XLOG(XFAIL, eError, "FAIL: Stream completed prematurely (wait result: %d)", dwWaitResult);

            //  log the streaming result for diagnostic purposes
            METHODCHECKRESULT("IRenderTestStreamingEngine::GetLastStreamingError",
                              hr,
                              spRenderEngine->GetLastStreamingError());
            return FNS_PASS;
        }

        //  wait awhile then try again...
        Sleep(1000);
    }

    return FNS_PASS;

SKIPPED:
    return FNS_SKIPPED;
}

//------------------------------------------------------------------------------
// Name: Tc_CaptureLonghaul
// Desc: Stream indefinitely until the test is aborted, reporting any errors.
//
// Returns:
//              FNS_PASS; pass status determined via XLOG
//------------------------------------------------------------------------------
DWORD Tc_CaptureLonghaul()
{
    HRESULT hr = S_OK;
    AudioClientProperties                   clientProperties = {0};
    CComPtr<IBasicLog>                      spLogger = NULL;

    LPCWSTR                                 pwszEndpointUnderTest;

    CComPtr<ITestStreamingEngine>           spStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine>    spCaptureEngine = NULL;

    CHandle                                 shCompletionEvent;

    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    //  Get ourselves a logger.
    hr = g_IShell->GetBasicLog(&spLogger);    
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        return FNS_PASS;
    }

    //  Ensure we have a capture endpoint
    if (DEVICE_TYPE_CAPTURE != GetDeviceTypeUnderTest())
    {
        XLOG(XFAIL, eError, "FAIL: Endpoint under test does not support render!");
        return FNS_PASS;
    }

    //  Get the endpoint ID
    pwszEndpointUnderTest = GetEndpointIDUnderTest();
    if (pwszEndpointUnderTest == NULL)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain current endpoint ID");
        return FNS_PASS;
    }

    HANDLE hCompletionEvent = NULL;
    //  Create the event used to signal streaming completion
    if (NULL == (hCompletionEvent = CreateEvent(NULL,       //  Event Attribs
                                                TRUE,       //  Manual reset event
                                                FALSE,      //  Default to non-signalled
                                                NULL        //  Name
                                               )))
    {
        XLOG(XFAIL, eError, "FAIL: CreateEvent returned NULL (Error Code: %d)", GetLastError());
        return FNS_PASS;
    }
    else
    {
        shCompletionEvent.Attach(hCompletionEvent);
    }

    //  Instantiate a capture streaming engine
    METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId",
                      hr,
                      CreateTestStreamingEngineOnEndpointId(__uuidof(ICaptureTestStreamingEngine),
                                                pwszEndpointUnderTest,
                                                TIMER_PUSH_CAPTURE,
                                                spLogger,
                                                reinterpret_cast<void**>(&spCaptureEngine)));
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create test streaming engine!");
        return FNS_PASS;
    }

    //  Get an ITestStreamingEngine interface
    METHODCHECKRESULT("ICaptureTestStreamingEngine::QueryInterface",
                      hr,
                      spCaptureEngine->QueryInterface(__uuidof(ITestStreamingEngine),
                                                      reinterpret_cast<void**>(&spStreamingEngine)));
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to QI for ITestStreamingEngine");
        return FNS_PASS;
    }

    //  Initialize the streaming engine
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
        SLOG(eError, "ERROR: Unable to initialize streaming engine");
        return FNS_PASS;
    }     

    // Start capturing...
    METHODCHECKRESULT("ICaptureTestStreamingEngine::NullCapture (async)",
                      hr,
                      spCaptureEngine->NullCapture(0,
                                                   shCompletionEvent));
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error initiating capture!");
        return FNS_PASS;
    }

    while (true)
    {
        DWORD   dwWaitResult = 0;   

        //  Check to see if termination of the application was requested.
        if (g_IShell->RequestAbort())
        {
            SLOG(eInfo1, "Test termination requested, shutting down");

            METHODCHECKRESULT("ICaptureTestStreamingEngine::StopStreaming",
                              hr,
                              spCaptureEngine->StopStreaming());
            if (FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Error reported while stopping streaming");
                return FNS_PASS;
            }

            //  all done
            break;
        }

        //  Periodically check to see if the streaming completion event was signalled
        if (WAIT_TIMEOUT != (dwWaitResult = WaitForSingleObject(shCompletionEvent, 100)))
        {
            XLOG(XFAIL, eError, "FAIL: Stream completed prematurely (wait result: %d)", dwWaitResult);

            //  log the streaming result for diagnostic purposes
            METHODCHECKRESULT("ICaptureTestStreamingEngine::GetLastStreamingError",
                              hr,
                              spCaptureEngine->GetLastStreamingError());
            return FNS_PASS;
        }

        //  wait awhile then try again...
        Sleep(1000);
    }

    return FNS_PASS;

SKIPPED:
    return FNS_SKIPPED;     
}
