//
// Copyright(C) Microsoft.All rights reserved.
//
// LullabyRenderTests.cpp
//
// Description:
//
//  Lullaby Render Test Implementation
//
#include "StdAfx.h"
#include "AudioOffload.h"
#include "QueryMachineInfo.h"

const float    TEST_TONE_FREQ           = 500.0f;
const float    TEST_TONE_AMP            = 0.708f;
const DWORD    TEST_DURATION            = 3000;  // millisecond
const DWORD    CS_TEST_RENDER_DURATION  = 30000; // millisecond

//******************************************************************************
//  CRenderTest class implementation methods
//******************************************************************************
CRenderTest::CRenderTest(int nSynchronous, SYSTEM_POWER_STATE spsState, UINT nSleep, UINT nSnooze, ApiEnum api, BOOL bOffload)
: CTestCase(nSynchronous, TEST_PLAYBACK, spsState, nSleep, nSnooze, api),
m_pStreaming(NULL), m_bOffload(bOffload)
{
    StringCchPrintf(m_szTestName, 40, L"CWASAPIRender() Test (%s)", 
        (m_nSynchronous == SYNC) ? L"Synchronous" : L"Asynchronous");
}

CRenderTest::CRenderTest(int nSynchronous, ApiEnum api, BOOL bOffload)
: CTestCase(nSynchronous, TEST_PLAYBACK, api), m_pStreaming(NULL), m_bOffload(bOffload)
{
    StringCchPrintf(m_szTestName, 40, L"CWASAPIRender() CS Test (%s)", 
        (m_nSynchronous == SYNC) ? L"Synchronous" : L"Asynchronous");
}
// ----------------------------------------------------------
CRenderTest::~CRenderTest( void )
{
}

// ----------------------------------------------------------
BOOL CRenderTest::Initialize( void )
{
    HRESULT hr = S_OK;

    // For Bluetooth devices, check to make sure the device is ready after the last test
    // This is necessary because the last test may have been run on a different endpoint on the same device
    if (g_pLullabyDeviceTestModule && g_pLullabyDeviceTestModule->GetDevice()->IsBluetoothDevice())
    {
        if (!CheckForDeviceAvailable())
        {
            return BoolToFns(FALSE);
        }
        XLOG(XMSG, eInfo1, L"Checked Bluetooth device for reconnect - device reports ready.");
    }

    hr = CreateRendererForDUT();
    if (FAILED(hr)) 
    {
        XLOG(XFAIL, eError, L"Failed Create Rendering Device (hr = %s)", GetHRString(hr));
        return FALSE;
    }

    if (FNS_SKIPPED == m_dwResPre)
    {
        return FALSE;
    }

    return CTestCase::Initialize();
}

HRESULT CRenderTest::CreateRendererForDUT()
{
    HRESULT hr = S_OK;
    CLullabyDevice* pCurrentDevice = NULL;
    CComPtr<IMMDevice> spDeviceUnderTest = NULL;
    CComPtr<IBasicLog>          spLogger = NULL;

    //  Get an IBasicLog
    hr = g_IShell->GetBasicLog(&spLogger);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Unable to create IBasicLog!");
        return hr;
    }

    //  Get the endpoint under test
    if (NULL == (pCurrentDevice = g_pLullabyDeviceTestModule->GetDevice()))
    {
        XLOG(XFAIL, eError, L"FAIL: IMMDevice under test was null!");
        return E_FAIL;
    }

    hr = pCurrentDevice->GetEndpoint(&spDeviceUnderTest);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: GetEndpoint encountered error (hr = %s).", GetHRString(hr));
        return hr;
    }

    hr = CreateAudioStreamingRender(m_api, spDeviceUnderTest, spLogger, &m_pStreaming);
    if (FAILED(hr)) 
    {
        XLOG(XFAIL, eError, L"Failed CreateAudioStreamingRenderOnEndpointId() (hr = %s)", GetHRString(hr));
        return hr;
    }

    if(TRUE == m_bOffload)
    {
        //verify the device supports offload 
        bool bSupportsOffload = FALSE;
        hr = AudioOffload::DoesDeviceSupportOffload(spLogger, spDeviceUnderTest, AudioOffload::EApiLevel::MMDEV_API, bSupportsOffload);
        if (FAILED(hr)) 
        {
            XLOG(XFAIL, eError, L"Failed: AudioOffload::DoesDeviceSupportOffload (hr = %s)", GetHRString(hr));
            return hr;
        }

        if (false == bSupportsOffload)
        {
            XLOG(XMSG, eInfo1, L"Device does not support offload, Skipping test.");
            m_dwResPre = FNS_SKIPPED;
            return S_OK;
        }

        //force offload
        CComPtr<IAudioStreamingControl> pAudioStreamingControl;
        hr =  m_pStreaming->QueryInterface(__uuidof(IAudioStreamingControl),
            reinterpret_cast<void**>(&pAudioStreamingControl));
        if (FAILED(hr)) 
        {
            XLOG(XFAIL, eError, L"Failed QI for IAudioStreamingControl (hr = %s)", GetHRString(hr));
            return hr;
        }

        CComPtr<IAudioStreamingWasapiControl> pAudioStreamingWasapiControl;
        hr =  pAudioStreamingControl->GetService(__uuidof(IAudioStreamingWasapiControl),
            reinterpret_cast<void**>(&pAudioStreamingWasapiControl));
        if (FAILED(hr)) 
        {
            XLOG(XFAIL, eError, L"Failed GetService for IAudioStreamingWasapiControl() (hr = %s)", GetHRString(hr));
            return hr;
        }

        AudioClientProperties clientProperties = {0};
        clientProperties.bIsOffload = TRUE;
        clientProperties.eCategory = AudioCategory_Media;
        clientProperties.cbSize = sizeof(clientProperties);
        hr = pAudioStreamingWasapiControl->SetClientProperties(&clientProperties);
        if (FAILED(hr)) 
        {
            XLOG(XFAIL, eError, L"IAudioStreamingWasapiControl::SetClientProperties (hr = %s)", GetHRString(hr));
            return hr;
        }

        hr = pAudioStreamingWasapiControl->SetAudioClientInitializeParameters(
            AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
            200000,
            200000,
            GUID_NULL
        );
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, L"IAudioStreamingWasapiControl::SetAudioClientInitializeParameters (hr = %s)", GetHRString(hr));
            return hr;
        }

        //now it can initialize like normal
    }

    hr = m_pStreaming->Initialize(NULL);
    if (FAILED(hr)) 
    {
        XLOG(XFAIL, eError, L"Failed IAudioStreamingRender::Initialize() (hr = %s)", GetHRString(hr));
        return hr;
    }

    return hr;
}
// ----------------------------------------------------------
void CRenderTest::CleanUp( void )
{
    if(NULL != m_pStreaming)
    {
        m_pStreaming->Release();
    }

    CTestCase::CleanUp();
}

// ----------------------------------------------------------
DWORD CRenderTest::RunPreTest()
{
    BOOL    fRes = TRUE;
    HRESULT hr = S_OK;

    // Synchronous test
    if(m_nSynchronous == SYNC)
    {
        XLOG(XMSG, eInfo1, L"This test makes sure that a sine wave can be played before and after suspending using AudioStreaming.");
        XLOG(XMSG, eInfo1, L"TestTone: Using %gHz tone (amplitude %g)", TEST_TONE_FREQ, TEST_TONE_AMP);
        hr = m_pStreaming->PlaySineWave(TEST_TONE_FREQ, TEST_TONE_AMP, Method_NoDithering, TEST_DURATION);
        if (FAILED(hr)) 
        {
            XLOG(XFAIL, eError, L"TestTone: Failed IAudioStreamingRender::PlaySineWave() (hr = %s)", GetHRString(hr));
            fRes = FALSE;
        }
    }
    else // Asynchronous test        
    {
        XLOG(XMSG, eInfo2, L"This test makes sure that the machine can suspend while playing a sinewave using AudioStreaming.");
        XLOG(XMSG, eInfo2, L"The sound should resume when the machine resumes.");
        XLOG(XMSG, eInfo2, L"TestTone: Using %gHz tone (amplitude %g)", TEST_TONE_FREQ, TEST_TONE_AMP);
        hr = m_pStreaming->PlaySineWaveAsync(TEST_TONE_FREQ, TEST_TONE_AMP, Method_NoDithering);
        if (FAILED(hr)) 
        {
            XLOG(XFAIL, eError, L"TestTone: Failed IAudioStreamingRender::PlaySineWaveAsync() (hr = %s)", GetHRString(hr));
            fRes = FALSE;
        }
    }

    m_dwResPre = BoolToFns(fRes);
    return m_dwResPre;
}

// ----------------------------------------------------------
DWORD CRenderTest::RunPostTest()
{
    BOOL fRes = TRUE;
    HRESULT hr = S_OK;
    // Synchronous test
    if(m_nSynchronous == SYNC)
    {
        //Check to see if the device is available
        if (!CheckForDeviceAvailable())
            return BoolToFns(FALSE);

        //we know the device is ready now
        // release the old streaming renderer and make a new one
        SAFE_RELEASE(m_pStreaming);
        hr = CreateRendererForDUT();
        if (FAILED(hr)) 
        {
            XLOG(XFAIL, eError, L"Failed Create Rendering Device (hr = %s)", GetHRString(hr));
            return BoolToFns(FALSE);
        }

        XLOG(XMSG, eInfo2, L"Making sure that we can still play a sinewave using AudioStreaming...");
        hr = m_pStreaming->PlaySineWave(TEST_TONE_FREQ, TEST_TONE_AMP, Method_NoDithering, TEST_DURATION);
        if (FAILED(hr)) 
        {
            XLOG(XFAIL, eError, L"TestTone: Failed IAudioStreamingRender::PlaySineWave() (hr = %s)", GetHRString(hr));
            fRes = FALSE;
        }    
    }
    // Asynchronous test
    else 
    {
        hr = m_pStreaming->Stop();
        if (S_OK != hr)
        {
            // For Bluetooth devices, if an error was detected, make sure the device reconnects
            if (g_pLullabyDeviceTestModule && g_pLullabyDeviceTestModule->GetDevice()->IsBluetoothDevice())
            {
                XLOG(XMSG, eInfo2, L"TestTone: Failed IAudioStreamingRender::PlaySineWaveAsync() (hr = %s)", GetHRString(hr));
                XLOG(XMSG, eInfo2, L"Checking to make sure device reconnects.");
                //Check to see if the device is available
                if (!CheckForDeviceAvailable())
                    return BoolToFns(FALSE);
                XLOG(XMSG, eInfo2, L"Device successfully reconnected.");
            }
            else
            {
                XLOG(XFAIL, eError, L"TestTone: Failed IAudioStreamingRender::PlaySineWaveAsync() (hr = %s)", GetHRString(hr));
                fRes = FALSE;
            }
        }
    }

    m_dwResPost = BoolToFns(fRes);
    return m_dwResPost;
}


//******************************************************************************
//  CCSRenderTest class implementation methods
//******************************************************************************
CCSRenderTest::CCSRenderTest( int nSynchronous, ApiEnum api, BOOL bOffload, BOOL bGlitchTest )
    :  CRenderTest( nSynchronous, api, bOffload), CCSBaseTest(bGlitchTest)
{

}

// ----------------------------------------------------------
CCSRenderTest::~CCSRenderTest()
{
}

// ----------------------------------------------------------
BOOL CCSRenderTest::Initialize()
{
    return (CCSBaseTest::Initialize() && CRenderTest::Initialize());
}

// ----------------------------------------------------------
void CCSRenderTest::CleanUp()
{
    CCSBaseTest::CleanUp();
    CRenderTest::CleanUp();
}

// ----------------------------------------------------------
DWORD CCSRenderTest::RunPreTest()
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_PASS;
    if (m_bGlitchTest)
    {
        hr = SetupAndStartTracing(&m_pwszGlitchFileName);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, L"Glitch Tracing: Failed Setting up tracing (hr = %s)", GetHRString(hr));
            return FNS_FAIL;
        }
    }
    
    dwRes = CRenderTest::RunPreTest();
    if( FALSE == PASSED(dwRes) )
    {
        XLOG(XFAIL, eError, L"Failed Running CRenderPreTest");
        return dwRes;
    }

    return BoolToFns(EnterConnectedStandby());
}

// ----------------------------------------------------------
DWORD CCSRenderTest::RunPostTest()
{
    DWORD dwRes = BoolToFns(ExitConnectedStandby());
    if( FALSE == PASSED(dwRes) )
    {
        return dwRes;
    }
    
    dwRes = CRenderTest::RunPostTest();
    // Don't exit out early

    if(m_bGlitchTest)
    {
        HRESULT hr = StopProcessAndCleanupTracing(m_pwszGlitchFileName);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, L"Glitch Tracing: Failed Setting up tracing (hr = %s)", GetHRString(hr));
            return FNS_FAIL;
        }
    }

    return dwRes;
}
