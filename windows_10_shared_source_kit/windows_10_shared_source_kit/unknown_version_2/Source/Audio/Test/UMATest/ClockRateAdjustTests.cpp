// Copyright(C) Microsoft. All rights reserved.

//
// ClockRateAdjustTests.cpp
//  
//
// Description:
//
//  WASAPI IAudioClockAdjustment API and Functional Tests

#include <stdafx.h>
#include <UMATestStreaming.h>

#include "TestUtilities.h"

#include <SoundAnalyze.h>

const static DOUBLE TEST_TONE_FREQ = 500.0;
const static DOUBLE TEST_TONE_AMP = 0.01; // nice and quiet

// Allow up to one octave shift in frequency
const static FLOAT PERCENT_SRC_VARIANCE_ALLOWED_UPPER = 100.0f;
const static FLOAT PERCENT_SRC_VARIANCE_ALLOWED_LOWER = 50.0f;

const static FLOAT LOOPBACK_FREQ_TOLER_HZ = 1.0f;  // allow an error of +/ 1 hz
                                                   // for loopback analysis
                                                   
//------------------------------------------------------------------------------
// Name: GetRenderEngine
// Desc: Helper function for creating a render streaming engine
//
// Returns:
//              S_OK on success, the appropriate HRESULT on failure.
//------------------------------------------------------------------------------
HRESULT GetRenderEngine(LPCWSTR wszEndpoint,
                        ITestStreamingEngine** ppTestStreamingEngine,
                        IRenderTestStreamingEngine** ppRenderTestStreamingEngine)
{
    HRESULT hr = S_OK;

    if((NULL == wszEndpoint)||
       (NULL == ppTestStreamingEngine)||
       (NULL == ppRenderTestStreamingEngine))
    {
        hr = E_POINTER;
        return hr;
    }

    CComPtr<IBasicLog> pobjLogger;
    CComPtr<ITestStreamingEngine> spStreamingEngine;
    CComPtr<IRenderTestStreamingEngine> spRenderStreamingEngine;

    hr = g_IShell->GetBasicLog(&pobjLogger);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        return hr;
    }

    hr = CreateTestStreamingEngineOnEndpointId(IID_IRenderTestStreamingEngine,
                                   wszEndpoint,
                                   TIMER_PUSH_RENDER,
                                   pobjLogger,
                                   (void**)(&spRenderStreamingEngine));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to create IRenderTestStreamingEngine (0x%8x)",
             hr);
        return hr;
    }

    hr = spRenderStreamingEngine->QueryInterface(IID_ITestStreamingEngine,
                                                 (void**)(&spStreamingEngine));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to QI IRenderTestStreamingEngine for ITestStreamingEngine (0x%8x)",
             hr);
        return hr;
    }

    hr = spRenderStreamingEngine.CopyTo(ppRenderTestStreamingEngine);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Error copying IRenderTestStreamingEngine interface (0x%8x)",
             hr);
        return hr;
    }

    hr = spStreamingEngine.CopyTo(ppTestStreamingEngine);
    if(FAILED(hr))
    {
        *ppRenderTestStreamingEngine = NULL;
        
        XLOG(XFAIL,
             eError,
             "FAIL: Error copying ITestStreamingEngine interface (0x%8x)",
             hr);
        return hr;
    }            

    return hr;
}

//------------------------------------------------------------------------------
// Name: GetCaptureEngine
// Desc: Helper function for creating a capture streaming engine
//
// Returns:
//              S_OK on success, the appropriate HRESULT on failure.
//------------------------------------------------------------------------------
HRESULT GetCaptureEngine(LPCWSTR wszEndpoint,
                         ITestStreamingEngine** ppTestStreamingEngine,
                         ICaptureTestStreamingEngine** ppCaptureTestStreamingEngine)
{
    HRESULT hr = S_OK;

    if((NULL == wszEndpoint)||
       (NULL == ppTestStreamingEngine)||
       (NULL == ppCaptureTestStreamingEngine))
    {
        return E_POINTER;
    }
    
    CComPtr<IBasicLog> pobjLogger = NULL;
    CComPtr<ITestStreamingEngine> spStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine> spCaptureStreamingEngine = NULL;

    hr = g_IShell->GetBasicLog(&pobjLogger);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        return hr;
    }

    hr = CreateTestStreamingEngineOnEndpointId(IID_ICaptureTestStreamingEngine,
                                   wszEndpoint,
                                   TIMER_PUSH_CAPTURE,
                                   pobjLogger,
                                   (void**)(&spCaptureStreamingEngine));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to create ICaptureTestStreamingEngine (0x%8x)",
             hr);
        return hr;
    }

    hr = spCaptureStreamingEngine->QueryInterface(IID_ITestStreamingEngine,
                                                  (void**)(&spStreamingEngine));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to QI ICaptureTestStreamingEngine for ITestStreamingEngine (0x%8x)",
             hr);
        return hr;
    }

    hr = spCaptureStreamingEngine.CopyTo(ppCaptureTestStreamingEngine);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Error copying ICaptureTestStreamingEngine interface (0x%8x)",
             hr);
        return hr;
    }

    hr = spStreamingEngine.CopyTo(ppTestStreamingEngine);
    if(FAILED(hr))
    {
        *ppCaptureTestStreamingEngine = NULL;
        
        XLOG(XFAIL,
             eError,
             "FAIL: Error copying ITestStreamingEngine interface (0x%8x)",
             hr);
        return hr;
    }            

    return hr;
}

//------------------------------------------------------------------------------
// Name: GetLoopbackEngine
// Desc: Helper function for creating a capture streaming engine
//
// Note: The client must call CoTaskMemFree on ppwfxLoopbackFormat
//
// Returns:
//              S_OK on success, the appropriate HRESULT on failure.
//------------------------------------------------------------------------------
HRESULT GetLoopbackEngine(LPCWSTR wszEndpoint,
                          ITestStreamingEngine** ppLoopbackEngine,
                          ICaptureTestStreamingEngine** ppLoopbackCaptureEngine,
                          WAVEFORMATEX** ppwfxLoopbackFormat)
{
    HRESULT hr = S_OK;
    AudioClientProperties clientProperties = {0};
    CComPtr<IBasicLog> pobjLogger = NULL;
    CComPtr<ITestStreamingEngine> spStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine> spCaptureStreamingEngine = NULL;
    PWAVEFORMATEX pwfxFormat = NULL;

    if((NULL == wszEndpoint)||
       (NULL == ppLoopbackEngine)||
       (NULL == ppLoopbackCaptureEngine)||
       (NULL == ppwfxLoopbackFormat))
    {
        hr = E_POINTER;
        goto Exit;
    }

    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    hr = g_IShell->GetBasicLog(&pobjLogger);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to create IBasicLog!");
        goto Exit;
    }

    hr = CreateTestStreamingEngineOnEndpointId(IID_ICaptureTestStreamingEngine,
                                   wszEndpoint,
                                   TIMER_PUSH_CAPTURE,
                                   pobjLogger,
                                   (void**)(&spCaptureStreamingEngine));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to create ICaptureTestStreamingEngine for loopback (0x%8x)",
             hr);
        goto Exit;
    }

    hr = spCaptureStreamingEngine->QueryInterface(IID_ITestStreamingEngine,
                                                  (void**)(&spStreamingEngine));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to QI ICaptureTestStreamingEngine for ITestStreamingEngine (0x%8x)",
             hr);
        goto Exit;
    }

    // set the loopback streaming flag
    hr = spStreamingEngine->SetRequestedStreamingFlags(AUDCLNT_STREAMFLAGS_LOOPBACK);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to set requested streamflags (0x%8x)",
             hr);
        goto Exit;
    }

    // initialize the engine
    hr = spStreamingEngine->Initialize(&clientProperties);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Error initializing the loopback streaming engine (0x%8x)",
             hr);
        goto Exit;
    }

    // get the loopback format
    hr = spStreamingEngine->GetRequestedStreamingFormat(&pwfxFormat);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Error obtaining the loopback streaming format (0x%8x)",
             hr);  
        goto Exit;
    }

    if(NULL == pwfxFormat)
    {
        XLOG(XFAIL,
             eError,
             "FAIL: ITestStreamingEngine::GetRequestedStreamingFormat returned NULL format");
        goto Exit;
    }

    hr = spCaptureStreamingEngine.CopyTo(ppLoopbackCaptureEngine);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Error copying ICaptureTestStreamingEngine interface (0x%8x)",
             hr);
        goto Exit;
    }

    hr = spStreamingEngine.CopyTo(ppLoopbackEngine);
    if(FAILED(hr))
    {
        *ppLoopbackCaptureEngine = NULL;
        
        XLOG(XFAIL,
             eError,
             "FAIL: Error copying ITestStreamingEngine interface (0x%8x)",
             hr);
        goto Exit;
    }            

    *ppwfxLoopbackFormat = pwfxFormat;

Exit:
    if(FAILED(hr))
    {
        SAFE_COTASKMEMFREE(pwfxFormat);
    }

    return hr;
}

////
// Routine Description:
//      Given a buffer, return the fundamental frequency for each channel
// Arguments:
//      pBuffer             [in] -              Pointer to the buffer to be analyzed
//      pwfxBufferFormat    [in] -              Format of the buffer to be analyzed
//      nBufferFrames       [in] -              Number of frames in the buffer to be analyzed
//      pnChannelFrequencies [out] -            Number of output channel frequencies
//      padChannelFrequencies [out] -           Array containing the fundamental frequency for each channel
//
//      Note: The caller must call CoTaskMemFree on padChannelFrequencies.
//         
// Return value:
//      S_OK on success, the appropriate HRESULT on failure
////
HRESULT GetBufferFundamentalFrequency(PBYTE pBuffer,
                                      PWAVEFORMATEX pwfxBufferFormat,
                                      UINT32 nBufferFrames,
                                      UINT32* pnChannelFrequencies,
                                      DOUBLE** padChannelFrequencies)
{
    HRESULT                                         hr = S_OK;

    CComPtr<ISoundAnalyze>                          spSndAnlyz = NULL;

    DOUBLE*                                         adFrequencies = NULL;
    UINT32                                          nFreqs = 0;

    if((NULL == pBuffer)||
       (NULL == pwfxBufferFormat)||
       (NULL == pnChannelFrequencies)||
       (NULL == padChannelFrequencies))
    {
        hr = E_POINTER;
        goto Exit;
    }

    // get the sound analyzer interface
    hr = CreateSoundAnalyze(&spSndAnlyz);
    if(FAILED(hr))
    {
        XLOG(
            XFAIL,
            eError,
            "FAIL: Unable to obtain SoundAnalyzer interface! (0x%8x)",
            hr);
        goto Exit;
    }

    // Initialize the Sound Analyzer
    hr = spSndAnlyz->InitializeSoundAnalyzer((LPCWAVEFORMATEX)(pwfxBufferFormat),
                                             pBuffer,
                                             nBufferFrames,
                                             RECTANGLE,
                                             FALSE);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
            eError,
            "FAIL: Unable to initialize Sound Analyzer (0x%8x)",
            hr);

        if(HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND) == hr) // some custom SndAnlyz error code
        {
            // give the user a hint
            SLOG(eError, "--->Is SndAnlyz.dll in your path?");
        }
    }

    // allocate an array of doubles to store the fundamental frequency for each channel
    nFreqs = pwfxBufferFormat->nChannels;

    if(NULL == (adFrequencies = (DOUBLE*)(CoTaskMemAlloc(sizeof(DOUBLE)*nFreqs))) )
    {
        hr = E_OUTOFMEMORY;
        XLOG(XFAIL,
            eError,
            "FAIL: Out of memory!");
        goto Exit;
    }

    for(UINT32 i=0;i<nFreqs;i++)
    {            
        // Get the base frequency
        // channels are 1-based in SoundAnalyze
        hr = spSndAnlyz->GetBaseFrequency(i+1, adFrequencies[i]);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: ISoundAnalyze failed with code 0x%8x",
                 hr);
            goto Exit;
        }
    }

Exit:
    if(FAILED(hr))
    {
        SAFE_COTASKMEMFREE(adFrequencies);
    }
    else
    {
        // only update the out-params when everything succeeded
        *pnChannelFrequencies = nFreqs;
        *padChannelFrequencies = adFrequencies;
    }

    return hr;
}


void LogStreamFlags(LPCSTR pszFunctionName, DWORD dwStreamFlags)
{
    XLOG(XMSG, 3,
         "-------------------------------------------------------------\n"
         "-------------------------------------------------------------\n"
         "INFO: %s called with StreamFlags:\n%s%s%s",
         pszFunctionName,
         (((dwStreamFlags & AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM) == 0) ?
                "" : "\n      AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM"),
         (((dwStreamFlags & AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY) == 0) ?
                "" : "\n      AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY"),
         (((dwStreamFlags & AUDCLNT_STREAMFLAGS_RATEADJUST) == 0) ?
                "" : "\n      AUDCLNT_STREAMFLAGS_RATEADJUST"));
}

//------------------------------------------------------------------------------
// Name: FunctionalVerifyClockRateAdjust
// Desc: Verify dynamic clock rate changes using loopback
//
// Arguments:
//      wszEndpoint              [in] -  (Render) endpoint to be used for testing
//      dwStreamFlags            [in] -  Streaming flags used for rendering
//      dTestRateMultiplier      [in] -  Factor by which the native SR should be multipied for testing
//         
// Return value:
//      S_OK on success, the appropriate HRESULT on error conditions
//      Test failure is signalled via XLOG.
////
//------------------------------------------------------------------------------
HRESULT FunctionalVerifyClockRateAdjust(LPCWSTR wszEndpoint,
                                        DWORD dwStreamFlags,
                                        FLOAT fTestRateMultiplier)
{
    HRESULT hr = S_OK;

    if(NULL == wszEndpoint)
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Test given NULL endpoint ID");
        return E_INVALIDARG;        
    }

    AudioClientProperties clientProperties = {0};

    CComPtr<ITestStreamingEngine> spStreamingEngine = NULL;
    CComPtr<IRenderTestStreamingEngine> spRenderStreamingEngine = NULL;
    CComPtr<IAudioClockAdjustment> spClockAdjust = NULL;

    CComPtr<ITestStreamingEngine> spLoopbackEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine> spLoopbackCaptureEngine = NULL;

    PWAVEFORMATEX pwfxCurrentStreamingFormat = NULL;
    PWAVEFORMATEX pwfxLoopbackFormat = NULL;

    HANDLE hLoopbackCompletionEvent = INVALID_HANDLE_VALUE;
    size_t cbLoopbackBufferSize = 0;
    LPBYTE pLoopbackBuffer = NULL;
    UINT32 nLoopbackFrames = 0;

    UINT32 nLoopbackChannelFrequencies = 0;
    DOUBLE* adLoopbackFrequencies = NULL;

    FLOAT fStreamingSampleRate = 0.0f;
    FLOAT fTestSampleRate = 0.0f;
    FLOAT fExpectedFreq = 0.0f;

    clientProperties.bIsOffload = (DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest() ? TRUE : FALSE );
    clientProperties.eCategory = (DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest() ? AudioCategory_Media : AudioCategory_Other );
    clientProperties.cbSize = sizeof(clientProperties);

    LogStreamFlags("FunctionalVerifyClockRateAdjust", dwStreamFlags);

    // Create an event for monitoring loopback completion
    hLoopbackCompletionEvent = CreateEvent(NULL,
                                           FALSE,
                                           FALSE,
                                           NULL);
    if((INVALID_HANDLE_VALUE == hLoopbackCompletionEvent) || (NULL == hLoopbackCompletionEvent))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Error creating event handle (LastError: 0x%8x)",
             GetLastError());

        hr = E_FAIL;
        goto Exit;
    }


    // Create a test streaming engine to do the actual rendering    
    hr = GetRenderEngine(wszEndpoint,
                         &spStreamingEngine,
                         &spRenderStreamingEngine);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "ERROR: Unable to create render streaming engine (0x%8x)",
             hr);
        
        goto Exit;
    }

    // set the rateadjust flag
    hr = spStreamingEngine->SetRequestedStreamingFlags(dwStreamFlags);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Error setting requested streamflags (hr: 0x%8x)",
             hr);
        goto Exit;
    }

    // initialize
    hr = spStreamingEngine->Initialize(&clientProperties);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Error initializing streaming engine!");
        goto Exit;
    }        

    // Create a test streaming engine to do the loopback capture
    hr = GetLoopbackEngine(wszEndpoint,
                           &spLoopbackEngine,
                           &spLoopbackCaptureEngine,
                           &pwfxLoopbackFormat);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "ERROR: Unable to create loopback streaming engine (0x%8x)",
             hr);
        
        goto Exit;
    }

    // get the current mix format
    hr = spStreamingEngine->GetRequestedStreamingFormat(&pwfxCurrentStreamingFormat);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to obtain the current streaming format (0x%8x)",
             hr);
        goto Exit;
    }
    else if(NULL == pwfxCurrentStreamingFormat)
    {
        XLOG(XFAIL,
             eError,
             "FAIL: UMATestStreaming returned NULL current streaming format!");
        goto Exit;
    }

    fStreamingSampleRate = (FLOAT)(pwfxCurrentStreamingFormat->nSamplesPerSec);

    SLOG(eInfo1, "INFO: Current base streaming sample rate: %f", fStreamingSampleRate);

    // start streaming a sine tone
    hr = spRenderStreamingEngine->RenderSineTone(TEST_TONE_FREQ,
                                                 TEST_TONE_AMP,
                                                 0,                 // stream infinitely
                                                 DITHER_TYPE_TPD,
                                                 NULL);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Error calling IRenderTestStreamingEngine::RenderSineTone");
        goto Exit;
    }

    hr = spStreamingEngine->GetAudioClockAdjustment(&spClockAdjust);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to obtain IAudioClockAdjustment interface (0x%8x)",
             hr);    
        goto Exit;
    }        

    fTestSampleRate = fTestRateMultiplier * fStreamingSampleRate; 
    
    // set the input sample rate to its test value
    SLOG(eInfo1, "INFO: Dynamically adjusting sample rate to %.2f Hz", fTestSampleRate);
    hr = spClockAdjust->SetSampleRate(fTestSampleRate);
    if(S_OK != hr)
    {
        XLOG(XFAIL,
             eError,
             "FAIL: IAudioClockAdjustment::SetSampleRate failed (0x%8x)",
             hr);
    }

    // wait for the new sample rate to take effect
    Sleep(100);
  
    // capture 1 second worth of loopback data
    nLoopbackFrames = pwfxLoopbackFormat->nSamplesPerSec;
    cbLoopbackBufferSize = nLoopbackFrames * pwfxLoopbackFormat->nBlockAlign;

    if(NULL == (pLoopbackBuffer = new BYTE[cbLoopbackBufferSize]))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Out of memory!");
        goto Exit;
    }

    
    hr = spLoopbackCaptureEngine->CaptureBuffer(pLoopbackBuffer,
                                                cbLoopbackBufferSize,
                                                nLoopbackFrames,
                                                (const WAVEFORMATEX*)pwfxLoopbackFormat,
                                                FALSE,
                                                0,
                                                hLoopbackCompletionEvent);
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: IRenderTestStreamingEngine::CaptureBuffer failed (0x%8x)",
             hr); 
        goto Exit;
    }

    // wait for the loopback to complete
    if(WAIT_OBJECT_0 != WaitForSingleObject(hLoopbackCompletionEvent, 5000))
    {
        // timeout after 5 seconds
        XLOG(XFAIL,
             eError,
             "FAIL: TIMEOUT waiting for loopback to signal completion!");
        goto Exit;
    }
    else
    {
        SLOG(eInfo1, "Loopback completed");
    }

    hr = spRenderStreamingEngine->StopStreaming();
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Error stopping streaming (0x%8x)",
             hr);
        goto Exit;
    }

    // analysis time
    SLOG(eInfo1, "Analyzing loopback results...");
    
    hr = GetBufferFundamentalFrequency(pLoopbackBuffer,
                                       pwfxLoopbackFormat,
                                       nLoopbackFrames,
                                       &nLoopbackChannelFrequencies,
                                       &adLoopbackFrequencies);
    if(FAILED(hr))
    {
        // analysis failed; function calls XLOG appropriately
        goto Exit;
    }

    // calculate the expected fundamental frequency
    fExpectedFreq = fTestRateMultiplier * (FLOAT)TEST_TONE_FREQ;
    
    for(UINT32 i=0;i<nLoopbackChannelFrequencies;i++)
    {
        if(LOOPBACK_FREQ_TOLER_HZ < abs(adLoopbackFrequencies[i] - fExpectedFreq))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Channel %d: Observed fundamental frequency (%.2f Hz) outside of expected range (%.2f Hz +/ %.2f Hz)",
                 i,
                 adLoopbackFrequencies[i],
                 fExpectedFreq,
                 LOOPBACK_FREQ_TOLER_HZ);
        }
        else
        {
            SLOG(eInfo1, "Channel %d: %.2f Hz [OK]", i, adLoopbackFrequencies[i]);
        }
    }

Exit:
    SAFE_COTASKMEMFREE(pwfxCurrentStreamingFormat);
    SAFE_COTASKMEMFREE(pwfxLoopbackFormat);
    SAFE_COTASKMEMFREE(adLoopbackFrequencies);
    
    SAFE_DELETE_ARRAY(pLoopbackBuffer);
    SAFE_CLOSE_HANDLE(hLoopbackCompletionEvent);

    return hr;
}

//------------------------------------------------------------------------------
// Name: SetTestValidClockRates
// Desc: Set the adjustable clock rate to various valid values
//
// Arguments:
//      dwStreamFlags            [in] -  Streaming flags used for rendering
//
// Return value:
//      S_OK on success, the appropriate HRESULT on error conditions
//      Test failure is signalled via XLOG.
//------------------------------------------------------------------------------
HRESULT SetTestValidClockRates(DWORD dwStreamFlags)
{
    HRESULT hr = S_OK;
    LPCWSTR wszEndpointUnderTest = GetEndpointIDUnderTest();
    AudioClientProperties clientProperties = {0};

    CComPtr<ITestStreamingEngine> spStreamingEngine = NULL;
    CComPtr<IRenderTestStreamingEngine> spRenderStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine> spCaptureStreamingEngine = NULL;
    CComPtr<IAudioClockAdjustment> spClockAdjust = NULL;

    PWAVEFORMATEX pwfxCurrentStreamingFormat = NULL;
    FLOAT fStreamingSampleRate = 0.0f;
    FLOAT fMinValidSampleRate = 0, fMaxValidSampleRate = 0;
    FLOAT fTestSampleRate = 0.0f;

    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    LogStreamFlags("SetTestValidClockRates", dwStreamFlags);

    if((DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())||(DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest()))
    {
        clientProperties.bIsOffload = (DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest() ? TRUE : FALSE);
        clientProperties.eCategory = (DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest() ? AudioCategory_Media : AudioCategory_Other);

        hr = GetRenderEngine(wszEndpointUnderTest,
                             &spStreamingEngine,
                             &spRenderStreamingEngine);
        
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "ERROR: Unable to create render streaming engine (0x%8x)",
                 hr);
            
            goto Exit;
        }

        // set the rateadjust flag
        hr = spStreamingEngine->SetRequestedStreamingFlags(dwStreamFlags);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error setting requested streamflags (hr: 0x%8x)",
                 hr);
            goto Exit;
        }

        // initialize
        hr = spStreamingEngine->Initialize(&clientProperties);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error initializing streaming engine!");
            goto Exit;
        }        

        // get the current mix format
        hr = spStreamingEngine->GetRequestedStreamingFormat(&pwfxCurrentStreamingFormat);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Unable to obtain the current streaming format (0x%8x)",
                 hr);
            goto Exit;
        }
        else if(NULL == pwfxCurrentStreamingFormat)
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: UMATestStreaming returned NULL current streaming format!");
            goto Exit;
        }

        fStreamingSampleRate = (FLOAT)(pwfxCurrentStreamingFormat->nSamplesPerSec);

        SLOG(eInfo1, "INFO: Current base streaming sample rate: %f", fStreamingSampleRate);

        // start streaming a sine tone
        hr = spRenderStreamingEngine->RenderSineTone(TEST_TONE_FREQ,
                                                     TEST_TONE_AMP,
                                                     0,                 // stream infinitely
                                                     DITHER_TYPE_TPD,
                                                     NULL);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error calling IRenderTestStreamingEngine::RenderSineTone");
            goto Exit;
        }

        hr = spStreamingEngine->GetAudioClockAdjustment(&spClockAdjust);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Unable to obtain IAudioClockAdjustment interface (0x%8x)",
                 hr);    
            goto Exit;
        }        

        // calculate the min and max valid sample rates
        // for example, if the streaming sample rate is 44.1 kHz,
        // the min will be 22.05 kHz
        // and the max will be 88.2 kHz
        fMinValidSampleRate = fStreamingSampleRate * (1.0f - (PERCENT_SRC_VARIANCE_ALLOWED_LOWER/100.0f));
        fMaxValidSampleRate = fStreamingSampleRate * (1.0f + (PERCENT_SRC_VARIANCE_ALLOWED_UPPER/100.0f));        

        // Test 20 randomly selected sample rates in the range [min, max]
        for(int i = 0; i <= 20; i++)
        {
            fTestSampleRate = fMinValidSampleRate + rand_float() * (fMaxValidSampleRate - fMinValidSampleRate);

            SLOG(eInfo1, "Testing IAudioClockAdjustment::SetSampleRate for %f Hz", fTestSampleRate);
            hr = spClockAdjust->SetSampleRate(fTestSampleRate);
            if(S_OK != hr)
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: IAudioClockAdjustment::SetSampleRate failed for a valid sample rate! (0x%8x)",
                     hr);            
            }
            else
            {
                Sleep(100);
            }            
        }

        hr = spRenderStreamingEngine->StopStreaming();
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error stopping streaming (0x%8x)",
                 hr);
            goto Exit;
        }
        
    }
    else if(DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        hr = GetCaptureEngine(wszEndpointUnderTest,
                              &spStreamingEngine,
                              &spCaptureStreamingEngine);
        
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "ERROR: Unable to create render streaming engine (0x%8x)",
                 hr);
            
            goto Exit;
        }
        
        // set the rateadjust flag
        hr = spStreamingEngine->SetRequestedStreamingFlags(dwStreamFlags);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error setting requested streamflags (hr: 0x%8x)",
                 hr);
            goto Exit;
        }

        // initialize
        hr = spStreamingEngine->Initialize(&clientProperties);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error initializing streaming engine!");
            goto Exit;
        }  

        // get the current mix format
        hr = spStreamingEngine->GetRequestedStreamingFormat(&pwfxCurrentStreamingFormat);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Unable to obtain the current streaming format (0x%8x)",
                 hr);
            goto Exit;
        }
        else if(NULL == pwfxCurrentStreamingFormat)
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: UMATestStreaming returned NULL current streaming format!");
            goto Exit;
        }

        fStreamingSampleRate = (FLOAT)(pwfxCurrentStreamingFormat->nSamplesPerSec);

        SLOG(eInfo1, "INFO: Current base streaming sample rate: %f", fStreamingSampleRate);

        // start capturing; dump the samples on the floor
        hr = spCaptureStreamingEngine->NullCapture(0,       // stream indefinitely
                                                   NULL);   // async
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error calling ICaptureTestStreamingEngine::NullCapture");
            goto Exit;
        }

        hr = spStreamingEngine->GetAudioClockAdjustment(&spClockAdjust);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Unable to obtain IAudioClockAdjustment interface (0x%8x)",
                 hr);
            goto Exit;
        }        

        // calculate the min and max valid sample rates
        // for example, if the streaming sample rate is 44.1 kHz,
        // the min will be 22.05 kHz
        // and the max will be 88.2 kHz
        fMinValidSampleRate = fStreamingSampleRate * (1.0f - (PERCENT_SRC_VARIANCE_ALLOWED_LOWER/100.0f));
        fMaxValidSampleRate = fStreamingSampleRate * (1.0f + (PERCENT_SRC_VARIANCE_ALLOWED_UPPER/100.0f));        

        // Test 50 randomly selected sample rates in the range [min, max]
        for(int i = 0; i <= 50; i++)
        {
            fTestSampleRate = fMinValidSampleRate + rand_float() * (fMaxValidSampleRate - fMinValidSampleRate);

            SLOG(eInfo1, "Testing IAudioClockAdjustment::SetSampleRate for %.2f Hz", fTestSampleRate);
            hr = spClockAdjust->SetSampleRate(fTestSampleRate);
            if(S_OK != hr)
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: IAudioClockAdjustment::SetSampleRate failed for a valid sample rate! (0x%8x)",
                     hr);            
            }
        }

        hr = spCaptureStreamingEngine->StopStreaming();
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error stopping streaming (0x%8x)",
                 hr);
            goto Exit;
        }
        
    }
    else
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unknown device type under test!");
        goto Exit;
    }
    
Exit:
    SAFE_COTASKMEMFREE(pwfxCurrentStreamingFormat);
    return hr;        
}

//------------------------------------------------------------------------------
// Name: SetTestInvalidClockRates
// Desc: Set the clock rate to various values outside the valid range.
//
// Arguments:
//      dwStreamFlags            [in] -  Streaming flags used for rendering
//
// Return value:
//      S_OK on success, the appropriate HRESULT on error conditions
//      Test failure is signalled via XLOG.
//------------------------------------------------------------------------------
HRESULT SetTestInvalidClockRates(DWORD dwStreamFlags)
{
    HRESULT hr = S_OK;
    LPCWSTR wszEndpointUnderTest = GetEndpointIDUnderTest();
    AudioClientProperties clientProperties = {0};

    CComPtr<ITestStreamingEngine> spStreamingEngine = NULL;
    CComPtr<IRenderTestStreamingEngine> spRenderStreamingEngine = NULL;
    CComPtr<ICaptureTestStreamingEngine> spCaptureStreamingEngine = NULL;
    CComPtr<IAudioClockAdjustment> spClockAdjust = NULL;

    PWAVEFORMATEX pwfxCurrentStreamingFormat = NULL;
    FLOAT fStreamingSampleRate = 0.0f;
    FLOAT fTestSampleRate = 0.0f;

    clientProperties.bIsOffload = FALSE;
    clientProperties.eCategory = AudioCategory_Other;
    clientProperties.cbSize = sizeof(clientProperties);

    LogStreamFlags("SetTestInvalidClockRates", dwStreamFlags);

    if((DEVICE_TYPE_RENDER == GetDeviceTypeUnderTest())||(DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest()))
    {
        clientProperties.bIsOffload = (DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest() ? TRUE : FALSE);
        clientProperties.eCategory = (DEVICE_TYPE_RENDEROFFLOAD == GetDeviceTypeUnderTest() ? AudioCategory_Media : AudioCategory_Other);
        
        hr = GetRenderEngine(wszEndpointUnderTest,
                             &spStreamingEngine,
                             &spRenderStreamingEngine);
        
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "ERROR: Unable to create render streaming engine (0x%8x)",
                 hr);
            
            goto Exit;
        }

        // set the rateadjust flag
        hr = spStreamingEngine->SetRequestedStreamingFlags(dwStreamFlags);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error setting requested streamflags (hr: 0x%8x)",
                 hr);
            goto Exit;
        }

        // initialize
        hr = spStreamingEngine->Initialize(&clientProperties);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error initializing streaming engine!");
            goto Exit;
        }        

        // get the current mix format
        hr = spStreamingEngine->GetRequestedStreamingFormat(&pwfxCurrentStreamingFormat);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Unable to obtain the current streaming format (0x%8x)",
                 hr);
            goto Exit;
        }
        else if(NULL == pwfxCurrentStreamingFormat)
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: UMATestStreaming returned NULL current streaming format!");
            goto Exit;
        }

        fStreamingSampleRate = (FLOAT)(pwfxCurrentStreamingFormat->nSamplesPerSec);

        SLOG(eInfo1, "INFO: Current base streaming sample rate: %f", fStreamingSampleRate);

        // start streaming a sine tone
        hr = spRenderStreamingEngine->RenderSineTone(TEST_TONE_FREQ,
                                                     TEST_TONE_AMP,
                                                     0,                 // stream infinitely
                                                     DITHER_TYPE_TPD,
                                                     NULL);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error calling IRenderTestStreamingEngine::RenderSineTone");
            goto Exit;
        }

        hr = spStreamingEngine->GetAudioClockAdjustment(&spClockAdjust);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Unable to obtain IAudioClockAdjustment interface (0x%8x)",
                 hr);            
            goto Exit;
        }        

        // BUGBUG: need to put the exact error code here
        SLOG(eInfo1, "Testing IAudioClockAdjustment::SetSampleRate with a negative value");
        hr = spClockAdjust->SetSampleRate(-1.0f);
        if(!FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: IAudioClockAdjustment::SetSampleRate should not succeed for a negative rate!");            
        }

        // out of bounds on the plus side
        // BUGBUG: need to put exact error code here
        SLOG(eInfo1, "Testing IAudioClockAdjustment::SetSampleRate 1%% above valid range");

        fTestSampleRate = fStreamingSampleRate*(1.0f+((PERCENT_SRC_VARIANCE_ALLOWED_UPPER+1.0f)/100.0f));
        SLOG(eInfo1, 
             "Standard SR: %f Hz Test SR: %f Hz",
             fStreamingSampleRate,
             fTestSampleRate);
        
        hr = spClockAdjust->SetSampleRate(fTestSampleRate);
        if(!FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: IAudioClockAdjustment::SetSampleRate should not have succeeded out of range!");
        }

        // out of bounds on the minus side
        // BUGBUG: need to put exact error code here
        SLOG(eInfo1, "Testing IAudioClockAdjustment::SetSampleRate 1%% below valid range");

        fTestSampleRate = fStreamingSampleRate*(1.0f-((PERCENT_SRC_VARIANCE_ALLOWED_LOWER+1.0f)/100.0f));
        SLOG(eInfo1, 
             "Standard SR: %f Hz Test SR: %f Hz",
             fStreamingSampleRate,
             fTestSampleRate);    

        hr = spClockAdjust->SetSampleRate(fTestSampleRate);
        if(!FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: IAudioClockAdjustment::SetSampleRate should not have succeeded out of range!");
        }

        hr = spRenderStreamingEngine->StopStreaming();
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error stopping streaming (0x%8x)",
                 hr);
            goto Exit;
        }
        
    }
    else if(DEVICE_TYPE_CAPTURE == GetDeviceTypeUnderTest())
    {
        hr = GetCaptureEngine(wszEndpointUnderTest,
                              &spStreamingEngine,
                              &spCaptureStreamingEngine);
        
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "ERROR: Unable to create render streaming engine (0x%8x)",
                 hr);
            
            goto Exit;
        }
        
        // set the rateadjust flag
        hr = spStreamingEngine->SetRequestedStreamingFlags(dwStreamFlags);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error setting requested streamflags (hr: 0x%8x)",
                 hr);
            goto Exit;
        }

        // initialize
        hr = spStreamingEngine->Initialize(&clientProperties);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error initializing streaming engine!");
            goto Exit;
        }  

        // get the current mix format
        hr = spStreamingEngine->GetRequestedStreamingFormat(&pwfxCurrentStreamingFormat);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Unable to obtain the current streaming format (0x%8x)",
                 hr);
            goto Exit;
        }
        else if(NULL == pwfxCurrentStreamingFormat)
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: UMATestStreaming returned NULL current streaming format!");
            goto Exit;
        }

        fStreamingSampleRate = (FLOAT)(pwfxCurrentStreamingFormat->nSamplesPerSec);

        SLOG(eInfo1, "INFO: Current base streaming sample rate: %f", fStreamingSampleRate);

        // start capturing; dump the samples on the floor
        hr = spCaptureStreamingEngine->NullCapture(0,       // stream indefinitely
                                                   NULL);   // async
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error calling ICaptureTestStreamingEngine::NullCapture");
            goto Exit;
        }

        hr = spStreamingEngine->GetAudioClockAdjustment(&spClockAdjust);
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Unable to obtain IAudioClockAdjustment interface (0x%8x)",
                 hr);
            goto Exit;
        }        

        // BUGBUG: need to put the exact error code here
        SLOG(eInfo1, "Testing IAudioClockAdjustment::SetSampleRate with a negative value");
        hr = spClockAdjust->SetSampleRate(-1.0f);
        if(!FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: IAudioClockAdjustment::SetSampleRate should not succeed for a negative rate!");            
        }

        // out of bounds on the plus side
        // BUGBUG: need to put exact error code here
        SLOG(eInfo1, "Testing IAudioClockAdjustment::SetSampleRate 1%% above valid range");

        fTestSampleRate = fStreamingSampleRate*(1.0f+((PERCENT_SRC_VARIANCE_ALLOWED_UPPER+1.0f)/100.0f));
        SLOG(eInfo1, 
             "Standard SR: %f Hz Test SR: %f Hz",
             fStreamingSampleRate,
             fTestSampleRate);
        
        hr = spClockAdjust->SetSampleRate(fTestSampleRate);
        if(!FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: IAudioClockAdjustment::SetSampleRate should not have succeeded out of range!");
        }

        // out of bounds on the minus side
        // BUGBUG: need to put exact error code here
        SLOG(eInfo1, "Testing IAudioClockAdjustment::SetSampleRate 1%% below valid range");

        fTestSampleRate = fStreamingSampleRate*(1.0f-((PERCENT_SRC_VARIANCE_ALLOWED_LOWER+1.0f)/100.0f));
        SLOG(eInfo1, 
             "Standard SR: %f Hz Test SR: %f Hz",
             fStreamingSampleRate,
             fTestSampleRate);    

        hr = spClockAdjust->SetSampleRate(fTestSampleRate);
        if(!FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: IAudioClockAdjustment::SetSampleRate should not have succeeded out of range!");
        }

        hr = spCaptureStreamingEngine->StopStreaming();
        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Error stopping streaming (0x%8x)",
                 hr);
            goto Exit;
        }
        
    }
    else
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unknown device type under test!");
        goto Exit;
    }
    
Exit:
    SAFE_COTASKMEMFREE(pwfxCurrentStreamingFormat);
    return hr;        
}


//------------------------------------------------------------------------------
// Name: TC_ClockRateAdjust_Valid1
// Desc: Set the adjustable clock rate to various valid values
//
// Returns:
//              FNS_PASS (failure is signalled by XLOG
//------------------------------------------------------------------------------
DWORD TC_ClockRateAdjust_Valid1()
{
    // HRESULT values from all call are being ingnored since failures
    // role up in XLOG

    // Verify Clock Rate Adjust for shared mode
#pragma prefast(suppress:28931, "Ignore hr")
    SetTestValidClockRates(AUDCLNT_STREAMFLAGS_RATEADJUST);
    // Verify Clock Rate Adjust for compat mode
#pragma prefast(suppress:28931, "Ignore hr")
    SetTestValidClockRates(AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                           AUDCLNT_STREAMFLAGS_RATEADJUST);
    // Verify Clock Rate Adjust for compat (SRC Default Quality) mode
#pragma prefast(suppress:28931, "Ignore hr")
    SetTestValidClockRates(AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                           AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY | 
                           AUDCLNT_STREAMFLAGS_RATEADJUST);

    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Name: TC_ClockRateAdjust_Valid2
// Desc: Test that sample are not perturbed for no rate change
//
// Returns:
//              FNS_PASS (failure is signalled by XLOG)
//------------------------------------------------------------------------------
DWORD TC_ClockRateAdjust_Valid2()
{
    // HRESULT values from all call are being ingnored since failures
    // role up in XLOG

    // Verify Clock Rate Adjust for shared mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    1.0f); // no change
    // Verify Clock Rate Adjust for compat mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    1.0f); // no change
    // Verify Clock Rate Adjust for compat (SRC Default Quality) mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                    AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY | 
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    1.0f); // no change
    
    return (FNS_PASS);
}

//------------------------------------------------------------------------------
// Name: TC_ClockRateAdjust_Valid3
// Desc: Adjust the clock up by 1% and verify that it took effect using loopback
//
// Returns:
//              FNS_PASS (failure is signalled by XLOG)
//------------------------------------------------------------------------------
DWORD TC_ClockRateAdjust_Valid3()
{
    // HRESULT values from all call are being ingnored since failures
    // role up in XLOG

    // Verify Clock Rate Adjust for shared mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    1.01f); // +1%
    // Verify Clock Rate Adjust for compat mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    1.01f); // +1%
    // Verify Clock Rate Adjust for compat (SRC Default Quality) mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                    AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY | 
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    1.01f); // +1%

    return (FNS_PASS);
}

//------------------------------------------------------------------------------
// Name: TC_ClockRateAdjust_Valid4
// Desc: Adjust the clock up by 5% and verify that it took effect using loopback
//
// Returns:
//              FNS_PASS (failure is signalled by XLOG)
//------------------------------------------------------------------------------
DWORD TC_ClockRateAdjust_Valid4()
{
    // HRESULT values from all call are being ingnored since failures
    // role up in XLOG

    // Verify Clock Rate Adjust for shared mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    1.05f); // +5%
    // Verify Clock Rate Adjust for compat mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    1.05f); // +5%
    // Verify Clock Rate Adjust for compat (SRC Default Quality) mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                    AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY | 
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    1.05f); // +5%
    
    return (FNS_PASS);
}

//------------------------------------------------------------------------------
// Name: TC_ClockRateAdjust_Valid5
// Desc: Adjust the clock up by 10% and verify that it took effect using loopback
//
// Returns:
//              FNS_PASS (failure is signalled by XLOG)
//------------------------------------------------------------------------------
DWORD TC_ClockRateAdjust_Valid5()
{
    // HRESULT values from all call are being ingnored since failures
    // role up in XLOG

    // Verify Clock Rate Adjust for shared mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    1.10f); // +10%
    // Verify Clock Rate Adjust for compat mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    1.10f); // +10%
    // Verify Clock Rate Adjust for compat (SRC Default Quality) mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                    AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY | 
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    1.10f); // +10%
    
    return (FNS_PASS);
}

//------------------------------------------------------------------------------
// Name: TC_ClockRateAdjust_Valid6
// Desc: Adjust the clock down by 1% and verify that it took effect using loopback
//
// Returns:
//              FNS_PASS (failure is signalled by XLOG)
//------------------------------------------------------------------------------
DWORD TC_ClockRateAdjust_Valid6()
{
    // HRESULT values from all call are being ingnored since failures
    // role up in XLOG

    // Verify Clock Rate Adjust for shared mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    .99f); // -1%
    // Verify Clock Rate Adjust for compat mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    .99f); // -1%
    // Verify Clock Rate Adjust for compat (SRC Default Quality) mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                    AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY | 
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    .99f); // -1%
    
    return (FNS_PASS);
}

//------------------------------------------------------------------------------
// Name: TC_ClockRateAdjust_Valid6
// Desc: Adjust the clock down by 5% and verify that it took effect using loopback
//
// Returns:
//              FNS_PASS (failure is signalled by XLOG)
//------------------------------------------------------------------------------
DWORD TC_ClockRateAdjust_Valid7()
{
    // HRESULT values from all call are being ingnored since failures
    // role up in XLOG

    // Verify Clock Rate Adjust for shared mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    .95f); // -5%
    // Verify Clock Rate Adjust for compat mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    .95f); // -5%
    // Verify Clock Rate Adjust for compat (SRC Default Quality) mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                    AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY | 
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    .95f); // -5%
    
    return (FNS_PASS);
}

//------------------------------------------------------------------------------
// Name: TC_ClockRateAdjust_Valid8
// Desc: Adjust the clock down by 10% and verify that it took effect using loopback
//
// Returns:
//              FNS_PASS (failure is signalled by XLOG)
//------------------------------------------------------------------------------
DWORD TC_ClockRateAdjust_Valid8()
{
    // HRESULT values from all call are being ingnored since failures
    // role up in XLOG

    // Verify Clock Rate Adjust for shared mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    .90f); // -10%
    // Verify Clock Rate Adjust for compat mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    .90f); // -10%
    // Verify Clock Rate Adjust for compat (SRC Default Quality) mode
#pragma prefast(suppress:28931, "Ignore hr")
    FunctionalVerifyClockRateAdjust(GetEndpointIDUnderTest(),
                                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                                    AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY | 
                                    AUDCLNT_STREAMFLAGS_RATEADJUST,
                                    .90f); // -10%
    
    return (FNS_PASS);
}


//------------------------------------------------------------------------------
// Name: TC_ClockRateAdjust_Invalid1
// Desc: Set the clock rate to various values outside the valid range.
//
// Returns:
//              FNS_PASS (failure is signalled by XLOG)
//------------------------------------------------------------------------------
DWORD TC_ClockRateAdjust_Invalid1()
{
    // HRESULT values from all call are being ingnored since failures
    // role up in XLOG

    // Verify Clock Rate Adjust for shared mode
#pragma prefast(suppress:28931, "Ignore hr")
    SetTestInvalidClockRates(AUDCLNT_STREAMFLAGS_RATEADJUST);
    // Verify Clock Rate Adjust for compat mode
#pragma prefast(suppress:28931, "Ignore hr")
    SetTestInvalidClockRates(AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                             AUDCLNT_STREAMFLAGS_RATEADJUST);
    // Verify Clock Rate Adjust for compat (SRC Default Quality) mode
#pragma prefast(suppress:28931, "Ignore hr")
    SetTestInvalidClockRates(AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
                             AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY | 
                             AUDCLNT_STREAMFLAGS_RATEADJUST);

    return FNS_PASS;
}
