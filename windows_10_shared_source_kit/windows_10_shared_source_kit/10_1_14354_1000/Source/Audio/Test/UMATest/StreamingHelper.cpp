// Copyright(C) Microsoft. All rights reserved.

//
// StreamingHelper.cpp
//  
//
// Description:
//
//  Helper functions to simplify functional streaming tests
//

#include <stdafx.h>
#include <UMATestStreaming.h>
#include <soundanalyze.h>


#include "StreamingHelper.h"
#include "DevTrans.h"

using namespace DevTrans;

// never allow render streaming to continue for more than 10 seconds
const UINT32 MAX_RENDER_DURATION_MS = 10000;

// analyze 1 second of loopback data by default
const UINT32 LOOPBACK_CAPTURE_DURATION_MS = 1000;

// error tolerance for loopback analysis (in hertz)
const FLOAT LOOPBACK_ERROR_TOLER_HZ = 10.0f; 

// half-width of analyzed signal in the freq domain
const FLOAT SIGNAL_HALF_WIDTH_HZ = 20.0f;

HRESULT TestRenderTone(AUDCLNT_SHAREMODE nShareMode,
                       DWORD dwStreamFlags,
                       PWAVEFORMATEX pwfxStreamingFormat,
                       TEST_STREAMING_ALGORITHM eStreamingAlgorithm,
                       FLOAT fToneAmplitude,
                       FLOAT fToneFrequency,
                       UINT32 nDuration,
                       bool bEnableLoopbackVerification)
{
    CComPtr<IBasicLog> pobjLogger = NULL;
    HRESULT hr = g_IShell->GetBasicLog(&pobjLogger);
    
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        return hr;
    }

    // Ensure we have a valid endpoint
    if((DEVICE_TYPE_RENDER != GetDeviceTypeUnderTest()&&
       (DEVICE_TYPE_RENDEROFFLOAD != GetDeviceTypeUnderTest())))
    {
        XLOG(XFAIL, eError, "FAIL: Endpoint under test does not support render!");
        return E_FAIL;
    }

    // Enable offload, if appropriate
    AudioClientProperties clientProperties = {0};
    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);
    if(DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest())
    {
        if (AUDCLNT_SHAREMODE_SHARED == nShareMode) 
            clientProperties.bIsOffload = TRUE;
        clientProperties.eCategory = AudioCategory_Media;
    }

    // Get the endpoint ID
    LPCWSTR pwszEndpointUnderTest = GetEndpointIDUnderTest();
    if(pwszEndpointUnderTest == NULL)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain current endpoint ID");
        return E_FAIL;
    }

    // If the client is requesting loopback verification, ensure that all the necessary prerequisites
    // have been met.
    if(bEnableLoopbackVerification)
    {
        if(0 != nDuration)
        {
            XLOG(XFAIL, eError, "FAIL: Loopback verification requires that the client specify an infinite streaming duration!");
            hr = E_FAIL;
        }

        if(AUDCLNT_SHAREMODE_SHARED != nShareMode)
        {
            XLOG(XFAIL, eError, "FAIL: Loopback verification is only possible in shared streaming mode!");
            hr = E_FAIL;
        }

        if(FAILED(hr))
        {
            return hr;
        }
    }


    // If we're in exclusive mode, check to see if the streaming format is supported
    // before doing anything else.
    if(AUDCLNT_SHAREMODE_EXCLUSIVE == nShareMode)
    {
        hr = CheckFormatSupported(nShareMode, pwfxStreamingFormat);
        if(S_OK != hr)
        {
            SLOG(eError, "<Format not supported by endpoint - Skipping...>");
            return S_FALSE;
        }                                      
    }


    CHandle shStreamingCompletionEvent(CreateEvent(NULL, FALSE, FALSE, NULL));
    if(NULL == (HANDLE)shStreamingCompletionEvent)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        if(SUCCEEDED(hr))
        {
            hr = E_FAIL;
        }

        XLOG(XFAIL, eError, "FAIL: Unable to create a waitable event handle (0x%08X)", hr);
        return hr;        
    }

    // Create the streaming engine
    // this takes a weak reference on the shStreamingCompletionEvent
    CComPtr<IRenderTestStreamingEngine> spRenderEngine;
    METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId",
                      hr,
                      CreateTestStreamingEngineOnEndpointId(IID_IRenderTestStreamingEngine,
                                                pwszEndpointUnderTest,
                                                eStreamingAlgorithm,
                                                pobjLogger,
                                                (void**)&spRenderEngine));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create test streaming engine!");
        return hr;
    }

    // Get the ITestStreamingEngine interface                                                
    CComPtr<ITestStreamingEngine> spStreamingEngine;
    METHODCHECKRESULT("IRenderTestStreamingEngine::QueryInterface",
                      hr,
                      spRenderEngine->QueryInterface(IID_ITestStreamingEngine,
                                                     (void**)&spStreamingEngine));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to QI for ITestStreamingEngine");   
        return hr;
    }   

    if(EVENT_PULL_RENDER == eStreamingAlgorithm)
    {
        dwStreamFlags |= AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
    }

    METHODCHECKRESULT("ITestStreamingEngine::SetRequestedStreamingFlags",
                      hr,
                      spStreamingEngine->SetRequestedStreamingFlags(dwStreamFlags));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to set requested streaming flags");
        return hr;
    }

    METHODCHECKRESULT("ITestStreamingEngine::SetRequestedShareMode",
                      hr,
                      spStreamingEngine->SetRequestedShareMode(nShareMode));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to set requested share mode");
        return hr;
    }

    METHODCHECKRESULT("ITestStreamingEngine::SetRequestedStreamingFormat",
                      hr,
                      spStreamingEngine->SetRequestedStreamingFormat(pwfxStreamingFormat));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to set requested streaming format");
        return hr;
    }

    // enable stream checking
    METHODCHECKRESULT("ITestStreamingEngine::SetStreamingVerificationState",
                      hr,
                      spStreamingEngine->SetStreamingVerificationState(TRUE));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to enable streaming verification");
        return hr;
    }   

    // Initialize the engine
    METHODCHECKRESULT("ITestStreamingEngine::Initialize()",
                      hr,
                      spStreamingEngine->Initialize(&clientProperties));
    if(FAILED(hr))
    {
        // If the hardware says it cannot do a specific format that should not
        // be a test failure
        if(clientProperties.bIsOffload && (AUDCLNT_E_UNSUPPORTED_FORMAT == hr))
        {
            SLOG(eWarn1, "WARNING: Unsupported audio format");
            return S_OK;
        }
        else if(hr == UTS_E_OFFLOAD_NOT_SUPPORTED)
        {
            XLOG(XSKIP, eError, "SKIP: Render Endpoint does not support offload");
            goto SKIPPED;
        }
        else
        {   
            SLOG(eError, "ERROR: Unable to initialize streaming engine");
            return hr;      
        }
    }     

    ////
    // Stream Tone
    ////
    METHODCHECKRESULT("RenderTestStreamingEngine::RenderSineTone",
                      hr,                      
                      spRenderEngine->RenderSineTone(fToneFrequency,
                                                     fToneAmplitude,
                                                     nDuration,
                                                     DITHER_TYPE_TPD,
                                                     shStreamingCompletionEvent));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error streaming sine tone");
        return hr;
    }

    // Perform loopback verification, if necessary
    if(bEnableLoopbackVerification)
    {
        CComPtr<ISimpleAudioVolume> spSimpleVol = NULL;
        CComPtr<IChannelAudioVolume> spChanVol = NULL;
        CComPtr<IAudioStreamVolume> spStreamVol = NULL;

        UINT32 nCapturedChannels = 0;
        CComHeapPtr<DOUBLE> srdCapturedToneFreqs;
        CComHeapPtr<DOUBLE> srdCapturedToneSNR;
        UINT32 dwStreamChannelCount = 0, dwSessionChannelCount = 0;

        // set the session, channel and stream volumes to unity, as this will
        // affect loopback capture
        METHODCHECKRESULT("ITestStreamingEngine::GetSimpleAudioVolume",
                          hr,
                          spStreamingEngine->GetSimpleAudioVolume(&spSimpleVol))
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume");
            return hr;
        }

        METHODCHECKRESULT("ITestStreamingEngine::GetStreamAudioVolume",
                          hr,
                          spStreamingEngine->GetStreamAudioVolume(&spStreamVol))
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioStreamVolume");
            return hr;
        }

        METHODCHECKRESULT("ITestStreamingEngine::GetChannelAudioVolume",
                          hr,
                          spStreamingEngine->GetChannelAudioVolume(&spChanVol))
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to obtain IChannelAudioVolume");
            return hr;
        }

        // session master volume
        METHODCHECKRESULT("ISimpleAudioVolume::SetMasterVolume",
                          hr,
                          spSimpleVol->SetMasterVolume(1.0f, NULL));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "Unable to set master session volume to unity!");
            return hr;
        }

        // session channel volumes
        METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                          hr,
                          spChanVol->GetChannelCount(&dwSessionChannelCount));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to obtain session channel count!");
            return hr;
        }

        for(UINT32 i=0; i<dwSessionChannelCount; i++)
        {
            METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                              hr,
                              spChanVol->SetChannelVolume(i, 1.0f, NULL));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Unable to set session channel volume!");
                return hr;
            }
        }        

        // stream channel volumes
        METHODCHECKRESULT("IAudioStreamVolume::GetChannelCount",
                          hr,
                          spStreamVol->GetChannelCount(&dwStreamChannelCount));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to obtain stream channel count!");
            return hr;
        }

        for(UINT32 i=0; i<dwStreamChannelCount; i++)
        {
            METHODCHECKRESULT("IAudioStreamVolume::SetChannelVolume",
                              hr,
                              spStreamVol->SetChannelVolume(i, 1.0f));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Unable to stream channel volume!");
                return hr;
            }
        }

        hr = LoopbackVerify(pwszEndpointUnderTest,
                            LOOPBACK_CAPTURE_DURATION_MS,
                            &nCapturedChannels,
                            &srdCapturedToneFreqs,
                            &srdCapturedToneSNR);
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Error performing loopback verification (0x%08X)", hr);
            return hr;
        }

        SLOG(eInfo1, "  Captured and verified loopback data for %d channels.", nCapturedChannels);

        for(UINT32 i=0; i<nCapturedChannels; i++)
        {
            FLOAT fError = abs((FLOAT)srdCapturedToneFreqs[i]-fToneFrequency);
            SLOG(eInfo1,
                 "  Channel [%d]    Observed Frequency: %.2f Hz    Expected Frequency: %.2f Hz   (Error = %.2f Hz)",
                 i,
                 srdCapturedToneFreqs[i],
                 fToneFrequency,
                 fError);
            SLOG(eInfo1,
                 "  Channel [%d]    Observed SNR: %.2f dB",
                 i,
                 srdCapturedToneSNR[i]);

            if(LOOPBACK_ERROR_TOLER_HZ < fError)
            {
                XLOG(XFAIL, eError, "FAIL:  Error (%.2f Hz exceeds specified tolerance of +/- %.2f Hz", fError, LOOPBACK_ERROR_TOLER_HZ);
                // keep going
            }
            else
            {
                SLOG(eInfo1, "    [OK]");
            }
        }

        // Stop the engine
        METHODCHECKRESULT("ITestStreamingEngine::StopStreaming (capture)",
                          hr,                      
                          spRenderEngine->StopStreaming());
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Error stopping render streaming engine!");
            return hr;
        }
    }
    else
    {
        // The loopback verification code will call StopStreaming explicitly
        // and so there will be no need to call wait and stop again if
        // loopback verification is done
        // wait for streaming to complete
        if(WAIT_OBJECT_0 != WaitForSingleObject(shStreamingCompletionEvent,
                                            max(nDuration, MAX_RENDER_DURATION_MS)))
        {
            XLOG(XFAIL, 
                 eError, 
                 "FAIL: TIMEOUT (expected no longer than %d ms) waiting for async render streaming to complete", 
                 max(nDuration,MAX_RENDER_DURATION_MS));
            return E_FAIL;
        }

        // Check for any errors from the streaming engine
        // If streaming has stopped this will be no op
        // If there were any errors during streaming StopStreaming will return it
        METHODCHECKRESULT("ITestStreamingEngine::StopStreaming (spRenderEngine)",
                            hr,                      
                            spRenderEngine->StopStreaming());
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Error occurred during streaming!");
            return hr;
        }
    }

    return hr;

SKIPPED:
    return FNS_SKIPPED;
}

HRESULT TestCapture(AUDCLNT_SHAREMODE nShareMode,
                    DWORD dwStreamFlags,
                    PWAVEFORMATEX pwfxStreamingFormat,
                    TEST_STREAMING_ALGORITHM eStreamingAlgorithm,
                    UINT32 nDuration)
{
    // Engine for primary testing    
    CComPtr<ITestStreamingEngine> spStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine> spCaptureEngine = NULL;

    // Secondary engine for loopback testing
    CComPtr<ITestStreamingEngine> spSecondaryEngine = NULL;
    CComPtr<IRenderTestStreamingEngine> spSecondaryRenderEngine = NULL;

    CComPtr<IBasicLog> pobjLogger;
    HRESULT hr = g_IShell->GetBasicLog(&pobjLogger);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        return hr;
    }
    
    if(EVENT_PULL_CAPTURE == eStreamingAlgorithm)
    {
        dwStreamFlags |= AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
    }

    // Ensure we have a valid endpoint
    if( (DEVICE_TYPE_CAPTURE != GetDeviceTypeUnderTest()) && (0 == (dwStreamFlags & AUDCLNT_STREAMFLAGS_LOOPBACK)) )
    {
        XLOG(XFAIL, eError, "FAIL: Endpoint under test does not support capture!");
        return E_FAIL;
    }

    // Get the endpoint ID
    LPCWSTR pwszEndpointUnderTest = GetEndpointIDUnderTest();
    if(pwszEndpointUnderTest == NULL)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain current endpoint ID");
        return E_FAIL;
    }

    // Get the form-factor of the endpoint under test
    CComPtr<IMMDevice> spEndpoint;
    hr = GetEndpointUnderTest(&spEndpoint);
    if(FAILED(hr))
    {
         XLOG( XFAIL, eFatalError, "FAIL: Unable to obtain IMMDevice interface for the endpoint under test (0x%8x)", hr);
        return FNS_PASS;
    }

    EndpointFormFactor eFormFactor;
    hr = GetEndpointFormFactor(&eFormFactor, spEndpoint);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to determine the EndpointFormFactor of the endpoint under test (0x%8x)", hr);
        return hr;
    }

    // If we're in exclusive mode, check to see if the streaming format is supported
    // before doing anything else.
    if(AUDCLNT_SHAREMODE_EXCLUSIVE == nShareMode)
    {
        hr = CheckFormatSupported(nShareMode, pwfxStreamingFormat);
        if(S_OK != hr)
        {
            SLOG(eError, "<Format not supported by endpoint - Skipping...>");
            return S_FALSE;
        }                                      
    }

    AudioClientProperties clientProperties = {0};
    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    // Create the secondary engine, if necessary
    if(0 != (AUDCLNT_STREAMFLAGS_LOOPBACK & dwStreamFlags))
    {
        auto eRenderAlgorithm = (EVENT_PULL_CAPTURE == eStreamingAlgorithm || DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest()) ? EVENT_PULL_RENDER : TIMER_PUSH_RENDER;

        // Create the render engine
        METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId (Render)",
                          hr,
                          CreateTestStreamingEngineOnEndpointId(IID_IRenderTestStreamingEngine,
                                                    pwszEndpointUnderTest,
                                                    eRenderAlgorithm,
                                                    pobjLogger,
                                                    (void**)&spSecondaryRenderEngine));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to create test render streaming engine");
            return hr;
        }

        // Get the IRenderTestStreamingEngine interface                                                
        METHODCHECKRESULT("IRenderTestStreamingEngine::QueryInterface",
                          hr,
                          spSecondaryRenderEngine->QueryInterface(IID_ITestStreamingEngine,
                                                                  (void**)&spSecondaryEngine));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to QI for ITestStreamingEngine");   
            return hr;
        }   

        // enable stream checking
        METHODCHECKRESULT("ITestStreamingEngine::SetStreamingVerificationState",
                          hr,
                          spSecondaryEngine->SetStreamingVerificationState(TRUE));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unable to enable streaming verification for render engine");
            return hr;
        }

        // check to see if render should be offloaded
        if(DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest())
        {
            clientProperties.bIsOffload = TRUE;
            clientProperties.eCategory = AudioCategory_Media;
        }

        // Initialize the engine
        METHODCHECKRESULT("ITestStreamingEngine::Initialize()",
                          hr,
                          spSecondaryEngine->Initialize(&clientProperties));

        if(hr == UTS_E_OFFLOAD_NOT_SUPPORTED)
        {
            XLOG(XSKIP, eError, "SKIP: Render Endpoint does not support offload");
            goto SKIPPED;
        }
        else if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Unable to initialize render streaming engine");
            return hr;
        }     

        ////
        // Stream Tone
        // The frequency and amplitude doesn't matter, so just use 1kHz, 0.01-amplitude
        ////
        METHODCHECKRESULT("RenderTestStreamingEngine::RenderSineTone",
                      hr,                      
                      spSecondaryRenderEngine->RenderSineTone(1000.0f,
                                                              0.01,
                                                              0,
                                                              DITHER_TYPE_NONE,
                                                              NULL));
        if(FAILED(hr))
        {
            return hr;
        }
    }

    // Create the streaming engine
    METHODCHECKRESULT("CreateTestStreamingEngineOnEndpointId (Capture)",
                      hr,
                      CreateTestStreamingEngineOnEndpointId(IID_ICaptureTestStreamingEngine,
                                                pwszEndpointUnderTest,
                                                eStreamingAlgorithm,
                                                pobjLogger,
                                                (void**)&spCaptureEngine));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create test capture streaming engine!");
        return hr;
    }

    // Get the ITestStreamingEngine interface                                                
    METHODCHECKRESULT("IRenderTestStreamingEngine::QueryInterface",
                      hr,
                      spCaptureEngine->QueryInterface(IID_ITestStreamingEngine,
                                                     (void**)&spStreamingEngine));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to QI for ITestStreamingEngine");   
        return hr;
    }   

    METHODCHECKRESULT("ITestStreamingEngine::SetRequestedStreamingFlags",
                      hr,
                      spStreamingEngine->SetRequestedStreamingFlags(dwStreamFlags));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to set requested streaming flags");
        return hr;
    }

    METHODCHECKRESULT("ITestStreamingEngine::SetRequestedShareMode",
                      hr,
                      spStreamingEngine->SetRequestedShareMode(nShareMode));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to set requested share mode");
        return hr;
    }

    METHODCHECKRESULT("ITestStreamingEngine::SetRequestedStreamingFormat",
                      hr,
                      spStreamingEngine->SetRequestedStreamingFormat(pwfxStreamingFormat));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to set requested streaming format");
        return hr;
    }

    // enable stream checking
    METHODCHECKRESULT("ITestStreamingEngine::SetStreamingVerificationState",
                      hr,
                      spStreamingEngine->SetStreamingVerificationState(TRUE));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to enable streaming verification");
        return hr;
    }

    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;

    // Initialize the engine
    METHODCHECKRESULT("ITestStreamingEngine::Initialize()",
                      hr,
                      spStreamingEngine->Initialize(&clientProperties));

    if(hr == UTS_E_OFFLOAD_NOT_SUPPORTED)
    {
        XLOG(XSKIP, eError, "SKIP: Render Endpoint does not support offload");
        goto SKIPPED;
    }
    else if(FAILED(hr))
    {
        SLOG(eError, "ERROR: Unable to initialize streaming engine");
        return hr;
    }     

    ////
    // Capture data
    ////
    METHODCHECKRESULT("ICaptureTestStreamingEngine::NullCapture",
                      hr,                      
                      spCaptureEngine->NullCapture(nDuration, NULL));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error encountered while capturing");
        return hr;
    }

    Sleep(nDuration);

    // stop the primary engine
    METHODCHECKRESULT("ITestStreamingEngine::StopStreaming (capture)",
                      hr,                      
                      spCaptureEngine->StopStreaming());
    if(UTS_E_BAD_DEVICEPOSITION_DETECTED == hr)
    {
        // if this is a SPDIF endpoint, log a warning, but do not fail the test
        // the position does not move on some devices when a SPDIF source is not connected
        if((SPDIF == eFormFactor)||(HDMI == eFormFactor))
        {
            if (g_IShell->GetMetric(FNSMETRIC_BVT))
            {
                hr = S_OK;
                SLOG(eFatalError, "WARNING: DEVICE POSITION ERROR DETECTED (ignoring error for digital input)");
                SLOG(eFatalError, "WARNING: You may need to connect an input source to your audio device");
            }
            else
            {
                hr = E_ABORT;
                SLOG(eFatalError, "WARNING: DEVICE POSITION ERROR DETECTED (ignoring error for digital input)");
                SLOG(eFatalError, "WARNING: You may need to connect an input source to your audio device");
                
                //  Note:  Failure will be reported on FAILED() clause...
            }
        }
    }    
    else if(UTS_E_STREAMING_EVENT_TIMOUT == hr)
    {
        // if this is a SPDIF endpoint, log a warning, but do not fail the test
        // events don't fire on some devices when a SPDIF source is not connected
        if((SPDIF == eFormFactor)||(HDMI == eFormFactor))
        {
            if (g_IShell->GetMetric(FNSMETRIC_BVT))
            {
                hr = S_OK;
                SLOG(eFatalError, "WARNING: STREAMING EVENT TIMEOUT DETECTED (ignoring error for digital input)");
                SLOG(eFatalError, "WARNING: You may need to connect an input source to your audio device");
            }
            else
            {
                hr = E_ABORT;
                SLOG(eFatalError, "WARNING: DEVICE POSITION ERROR DETECTED (ignoring error for digital input)");
                SLOG(eFatalError, "WARNING: You may need to connect an input source to your audio device");
                
                //  Note:  Failure will be reported on FAILED() clause...
            }
        }
    }

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error encountered while capturing");
        return hr;
    }

    // stop the secondary engine
    if(0 != (AUDCLNT_STREAMFLAGS_LOOPBACK & dwStreamFlags))
    {
        METHODCHECKRESULT("ITestStreamingEngine::StopStreaming (render)",
                          hr,                      
                          spSecondaryRenderEngine->StopStreaming());
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Error encountered while rendering");
            return hr;
        }
    }

    return hr;

SKIPPED:
    return FNS_SKIPPED;
}

HRESULT CheckFormatSupported(AUDCLNT_SHAREMODE nShareMode,
                             PWAVEFORMATEX pwfxStreamingFormat)
{
    CComPtr<IMMDevice> spEndpointUnderTest;
    HRESULT hr = GetEndpointUnderTest(&spEndpointUnderTest);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: GetEndpointUnderTest (0x%08X)", hr);
        return hr;
    }

    if(spEndpointUnderTest == NULL)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain endpoint under test");
        return hr;
    }


    CComPtr<IAudioClient> spAudClient;
    METHODCHECKRESULT("IMMDevice::Activate",
                      hr,
                      spEndpointUnderTest->Activate(__uuidof(IAudioClient),
                                                    CLSCTX_ALL,
                                                    NULL,
                                                    (void**)&spAudClient))
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioClient interface");
        return hr;
    }

    METHODCHECKRESULT("IAudioClient::IsFormatSupported",
                      hr,
                      spAudClient->IsFormatSupported(nShareMode,
                                                     pwfxStreamingFormat,
                                                     NULL));
    return hr;
}

HRESULT LoopbackVerify(LPCWSTR wszRenderEndpoint,
                       UINT32 nMillisCaptureDuration,
                       UINT32* pnCapturedChannels,
                       DOUBLE** ppdCapturedToneFreqs,
                       DOUBLE** ppdCapturedToneSNR)
{
    if((NULL == wszRenderEndpoint)||
       (NULL == ppdCapturedToneFreqs)||
       (NULL == pnCapturedChannels)||
       (NULL == ppdCapturedToneSNR))
    {
        return E_POINTER;
    }

    *pnCapturedChannels = 0;
    *ppdCapturedToneFreqs = NULL;
    *ppdCapturedToneSNR = NULL;

    CComPtr<IBasicLog> spLogger;
    HRESULT hr = g_IShell->GetBasicLog(&spLogger);
    if (FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to create IBasicLog (0x%08X)",
             hr);
        return hr;
    }

    CHandle shLoopbackCompletionEvent(CreateEvent(NULL, FALSE, FALSE, NULL));
    if(NULL == (HANDLE)shLoopbackCompletionEvent)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        if(SUCCEEDED(hr))
        {
            hr = E_FAIL;
        }

        XLOG(XFAIL, eError, "FAIL: Unable to create a waitable event handle (0x%08X)", hr);
        return hr;        
    }

    CComHeapPtr<BYTE> spLoopbackCaptureBuffer;
    
    // this takes a weak reference to:
    // the capture buffer (spLoopbackCaptureBuffer)
    // and the streaming completion event (shLoopbackCompletionEvent)
    CComPtr<ICaptureTestStreamingEngine> spLoopbackCaptureEngine;
    hr = CreateTestStreamingEngineOnEndpointId(IID_ICaptureTestStreamingEngine,
                                   wszRenderEndpoint,
                                   TIMER_PUSH_CAPTURE,
                                   spLogger,
                                   (void**)(&spLoopbackCaptureEngine));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Error obtaining ICaptureTestStreamingEngine (0x%8x)",
             hr);
        return hr;            
    }
                                   
    CComPtr<ITestStreamingEngine> spLoopbackEngine;
    hr = spLoopbackCaptureEngine->QueryInterface(IID_ITestStreamingEngine,
                                                 (void**)(&spLoopbackEngine));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to QI ICaptureTestStreamingEngine for ITestStreamingEngine (0x%08X)",
             hr);
        return hr;
    }
                                   
    // set the necessary loopback streamflags
    hr = spLoopbackEngine->SetRequestedStreamingFlags(AUDCLNT_STREAMFLAGS_LOOPBACK);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Error setting requested streaming flags for loopback engine: %d",
             hr);
        return hr;
    }

    AudioClientProperties clientProperties = {0};
    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);
    hr = spLoopbackEngine->Initialize(&clientProperties);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: ITestStreamingEngine::Initialize returned 0x%8x",
             hr);
        return hr;
    }

    // get the loopback streaming format
    CComHeapPtr<WAVEFORMATEX> spwfxLoopbackCaptureFormat;
    hr = spLoopbackEngine->GetRequestedStreamingFormat(&spwfxLoopbackCaptureFormat);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to obtain the loopback streaming format (0x%8x)",
             hr);
        return hr;
    }
    else if(NULL == spwfxLoopbackCaptureFormat)
    {

        XLOG(XFAIL,
             eError,
             "FAIL: NULL loopback streaming format encountered");
        return E_FAIL;
    }

    // calculate the necessary size for the loopback buffer
    UINT32 nLoopbackBufferFrames = (((spwfxLoopbackCaptureFormat->nSamplesPerSec)*nMillisCaptureDuration)/1000);
    size_t cbLoopbackBufferSize = nLoopbackBufferFrames * spwfxLoopbackCaptureFormat->nBlockAlign;

    // allocate the loopback buffer
    if(!spLoopbackCaptureBuffer.Allocate(cbLoopbackBufferSize))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Out of memory!");
        return E_OUTOFMEMORY;
    }
    
    SLOG(eInfo1, 
         "Attempting to capture %d loopback frames (%Id bytes)", 
         nLoopbackBufferFrames,
         cbLoopbackBufferSize);

    // capture the data
    hr = spLoopbackCaptureEngine->CaptureBuffer(spLoopbackCaptureBuffer,
                                                cbLoopbackBufferSize,
                                                nLoopbackBufferFrames,
                                                spwfxLoopbackCaptureFormat,
                                                FALSE,
                                                0,
                                                shLoopbackCompletionEvent);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Error initializing loopback buffer capture (0x%8x)",
             hr);
        return hr;
    }

    // wait for loopback to complete
    // timeout if capture takes longer than twice the expected duration
    if(WAIT_OBJECT_0 != WaitForSingleObject(shLoopbackCompletionEvent,
                                            2 * nMillisCaptureDuration))
    {
        SLOG(eInfo1,
             "ERROR: Timeout after %d ms waiting for loopback capture to signal completion",
             2 * nMillisCaptureDuration);
        return E_FAIL;
    }
    else
    {
        hr = spLoopbackCaptureEngine->GetLastStreamingError();
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Loopback streaming engine returned error 0x%08X", hr);
            return hr;
        }
        else
        {
            // success filling the buffer.
            SLOG(eInfo1,
                 "Captured %d frames.",
                 nLoopbackBufferFrames);
        }
    }

    hr = GetBufferFundamentalFrequencyAndSNR(spLoopbackCaptureBuffer,
                                             cbLoopbackBufferSize,
                                             spwfxLoopbackCaptureFormat,
                                             pnCapturedChannels,
                                             ppdCapturedToneFreqs,
                                             ppdCapturedToneSNR);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Error performing DSP analysis on captured loopback buffer (0x%08X)", hr);
        *pnCapturedChannels = 0;
        *ppdCapturedToneFreqs = NULL;
        *ppdCapturedToneSNR = NULL;
        return hr;
    }
    
    return hr;
}

HRESULT GetBufferFundamentalFrequencyAndSNR(LPBYTE pbBuffer,
                                            const size_t cbBuffer,
                                            LPCWAVEFORMATEX pwfxBufferFormat,
                                            PUINT32 pnFundamentalFrequencies,
                                            DOUBLE** ppdFundamentalFrequencies,
                                            DOUBLE** ppdSNRValues)
{
    HRESULT                                 hr = S_OK;

    CComPtr<ISoundAnalyze>                  spSoundAnalyze;
    UINT32                                  nBufferFrames = 0;

    UINT32                                  nChannels = 0;
    CComHeapPtr<DOUBLE>                     sp_dFundamentalFrequencies;
    CComHeapPtr<DOUBLE>                     sp_dSNRValues;
    SNR                                     sSNRValuesInternal = {0};

    if((NULL == pbBuffer)||
       (NULL == pwfxBufferFormat)||
       (NULL == pnFundamentalFrequencies)||
       (NULL == ppdFundamentalFrequencies)||
       (NULL == ppdSNRValues))
    {
        return E_POINTER;
    }

    //  calculate the number of frames (round down if necessary in the supplied data buffer
    nBufferFrames = (UINT32)cbBuffer / (pwfxBufferFormat->nBlockAlign);

    //  load up sound analyze
    hr = CreateSoundAnalyze(&spSoundAnalyze);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: CreateSoundAnalyze returned 0x%08X", hr); 
        return hr;
    }

    hr = spSoundAnalyze->InitializeSoundAnalyzer((LPCWAVEFORMATEX)(pwfxBufferFormat),
                                                 pbBuffer,
                                                 nBufferFrames,
                                                 RECTANGLE,
                                                 FALSE);
    if(HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND) == hr)
    {
        // give the user a hint
        SLOG(eInfo1, "--->Is SndAnlyz.dll in your path?");
    }
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: ISoundAnalyze::InitializeSoundAnalyzer returned 0x%08X", hr); 
        return hr;
    }

    //  allocate space to hold the fundamental frequency for each channel
    nChannels = pwfxBufferFormat->nChannels;

    //  soundanalyze currently only supports MAX_CHANNEL channels
    if(MAX_CHANNEL < nChannels)
    {
        XLOG(XFAIL, eError, "FAIL: soundanalyze currently only supports %d channels (requested %d)", MAX_CHANNEL, nChannels);
        return E_FAIL;
    }

    sp_dFundamentalFrequencies.Allocate(nChannels);
    sp_dSNRValues.Allocate(nChannels);
    if((NULL == sp_dFundamentalFrequencies)||(NULL == sp_dSNRValues))
    {
        XLOG(XFAIL, eError, "FAIL: Out of Memory!");
        return E_OUTOFMEMORY;
    }

    //  get the frequencies
    for (UINT32 i=0; i<nChannels; i++)
    {
        //  channels are 1-based in SoundAnalyze

        hr = spSoundAnalyze->GetBaseFrequency(i+1, sp_dFundamentalFrequencies[i]);
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: ISoundAnalyze::GetBaseFrequency returned 0x%08X", hr); 
            return hr;
        }
    }

    //  get the SNR value for each channel
    hr = spSoundAnalyze->GetSNR(SIGNAL_HALF_WIDTH_HZ, &sSNRValuesInternal);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: ISoundAnalyze::GetSNR returned 0x%08X", hr); 
        return hr;
    }

    // we only care about the SNR values for valid channels
    for(UINT32 i=0; i<nChannels; i++)
    {
        sp_dSNRValues[i] = sSNRValuesInternal.dSNR[i];
    }

    //  return the data to the caller
    *pnFundamentalFrequencies = nChannels;
    *ppdFundamentalFrequencies = sp_dFundamentalFrequencies;
    *ppdSNRValues = sp_dSNRValues;
    sp_dFundamentalFrequencies.Detach();
    sp_dSNRValues.Detach();
    
    return S_OK;
}
