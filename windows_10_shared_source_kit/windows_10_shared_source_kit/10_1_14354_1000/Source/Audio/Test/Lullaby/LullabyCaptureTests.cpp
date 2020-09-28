//
// Copyright(C) Microsoft.All rights reserved.
//
// LullabyCaptureTests.cpp
//
// Description:
//
//  Lullaby Capture Test Implementation
//


#include "StdAfx.h"
#include "AudioOffload.h"
#include "QueryMachineInfo.h"


const DWORD    SYNC_CAPTURE_DURATION    = 3000;  // millisecond
const DWORD    ASYNC_CAPTURE_DURATION   = 5000;  // millisecond //bump this up for sleep state time

const LPCTSTR  TEST_FILENAME  = L"CaptureTestFile.wav";


//
//  Simple helper class for handling incoming streaming completion notifications
//
class CCallbackHandler : public IAudioStreamingAsyncCallback 
{
public:    
    explicit CCallbackHandler(IBasicLog* pLog, HANDLE hCompletionEvent, HRESULT& hr)
        : m_pLogger(pLog)
        , m_cRef(1)
        , m_heCompletionEvent(NULL)
        , m_hrLastResult(S_OK)
    { 
        if(FAILED(hr))
        {
            return;
        }
        hr = S_OK;

        if (NULL != m_pLogger)
        {
            m_pLogger->AddRef();
        }
        else
        {
            hr = E_POINTER;
            return;
        }
        if(!DuplicateHandle(GetCurrentProcess(),
            hCompletionEvent,
            GetCurrentProcess(),
            &m_heCompletionEvent,
            0,
            FALSE,
            DUPLICATE_SAME_ACCESS))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            m_heCompletionEvent = NULL;
            return;
        }

    }

    // IAudioStreamingAsyncCallback
    HRESULT STDMETHODCALLTYPE Callback(HRESULT hrRet) 
    {
        m_hrLastResult = hrRet;

        if(FAILED(hrRet))
        {
            if (g_pLullabyDeviceTestModule && g_pLullabyDeviceTestModule->GetDevice()->IsBluetoothDevice())
            {
                // Don't register an error yet for Bluetooth endpoints
                XLOG(XMSG, eInfo2, L"Streaming Callback fired with hr = %s", GetHRString(hrRet));
            }
            else
            {
                XLOG(XFAIL, eError, L"Streaming Callback fired with hr = %s", GetHRString(hrRet));
            }
        }
        

        if (NULL != m_heCompletionEvent)
        {
            // ignore any errors since there's nothing we can do
            SetEvent(m_heCompletionEvent);
        }

        return S_OK;
    }

    // IUnknown
    ULONG STDMETHODCALLTYPE AddRef() { return InterlockedIncrement(&m_cRef); }

    ULONG STDMETHODCALLTYPE Release()
    {
        LONG retval = InterlockedDecrement(&m_cRef);

        if (0 >= retval) 
        {
            delete this;
        }
        return retval;
    }

    STDMETHOD(QueryInterface)(REFIID riid, void** pp)
    {
        if (NULL == pp) return E_POINTER;
        *pp = NULL;
        if (__uuidof(IUnknown) == riid || __uuidof(IAudioStreamingAsyncCallback) == riid)
        {
            *pp = (IAudioStreamingAsyncCallback*)this;
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    HRESULT GetLastStreamingResult() { return m_hrLastResult; }

private:
    ~CCallbackHandler() 
    {
        SAFE_RELEASE(m_pLogger);
        SAFE_CLOSE_HANDLE(m_heCompletionEvent);
    }

    LONG                m_cRef;
    IBasicLog*          m_pLogger;
    HANDLE              m_heCompletionEvent;

    volatile HRESULT    m_hrLastResult;
};

//******************************************************************************
//  CCaptureTest class implementation methods
//******************************************************************************
CCaptureTest::CCaptureTest(int nSynchronous, SYSTEM_POWER_STATE spsState, UINT nSleep, UINT nSnooze, ApiEnum api)
: CTestCase(nSynchronous, TEST_CAPTURE, spsState, nSleep, nSnooze, api),
  m_pCapture(NULL), m_pAudioStreamingControl(NULL), m_pCallback(NULL), m_heStreamingCompletionEvent(NULL),
  m_pbBuffer(NULL)
{
    StringCchPrintf(m_szTestName, 40, L"CWASAPICapture() Test (%s)", 
        (m_nSynchronous == SYNC) ? L"Synchronous" : L"Asynchronous");
}

CCaptureTest::CCaptureTest(int nSynchronous, ApiEnum api)
: CTestCase(nSynchronous, TEST_CAPTURE, api), 
  m_pCapture(NULL), m_pAudioStreamingControl(NULL), m_pCallback(NULL), m_heStreamingCompletionEvent(NULL),
  m_pbBuffer(NULL)
{
    StringCchPrintf(m_szTestName, 40, L"CWASAPICapture() CS Test (%s)", 
        (m_nSynchronous == SYNC) ? L"Synchronous" : L"Asynchronous");
}

// ----------------------------------------------------------
CCaptureTest::~CCaptureTest( void )
{
}

// ----------------------------------------------------------
BOOL CCaptureTest::Initialize( void )
{
    HRESULT hr = S_OK;

    // For Bluetooth devices, check to make sure the device is ready after the last test
    // This is necessary because the last test may have been run on a different endpoint on the same device
    if (g_pLullabyDeviceTestModule && g_pLullabyDeviceTestModule->GetDevice()->IsBluetoothDevice())
    {
        if (!CheckForDeviceAvailable())
            return BoolToFns(FALSE);
        XLOG(XMSG, eInfo1, L"Checked Bluetooth device for reconnect - device reports ready.");
    }

    hr = CreateCapturerForDUT();
    if (FAILED(hr)) 
    {
        XLOG(XFAIL, eError, L"Failed Creating capturer for device under test :%s", GetHRString(hr));
        return FALSE;
    }

    return CTestCase::Initialize();
}

HRESULT CCaptureTest::CreateCapturerForDUT()
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

    hr = CreateAudioStreamingCapture(m_api, spDeviceUnderTest, spLogger, &m_pCapture);
    if (FAILED(hr)) 
    {
        XLOG(XFAIL, eError, L"CreateAudioStreamingCapture (%s)", GetHRString(hr));
        return hr;
    }

    hr = m_pCapture->Initialize(NULL);
    if (FAILED(hr)) 
    {
        XLOG(XFAIL, eError, L"Failed IAudioStreamingCapture::Initialize():%s", GetHRString(hr));
        return hr;
    }

    return hr;
}
// ----------------------------------------------------------
void CCaptureTest::CleanUp( void )
{
    SAFE_RELEASE(m_pCapture);
    SAFE_RELEASE(m_pAudioStreamingControl);
    SAFE_RELEASE(m_pCallback);    
    SAFE_CLOSE_HANDLE(m_heStreamingCompletionEvent);
    SAFE_DELETE_ARRAY(m_pbBuffer);
    
    CTestCase::CleanUp();
}

// ----------------------------------------------------------
DWORD CCaptureTest::RunPreTest()
{
    BOOL    fRes = TRUE;
    HRESULT hr = S_OK;
    CComPtr<IBasicLog>          spLogger = NULL;
    //  Get an IBasicLog
    hr = g_IShell->GetBasicLog(&spLogger);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Unable to create IBasicLog!");
        return FALSE;
    }
    
    //movin on up to eliminate C2362
    CComPtr<IWaveFileSink> pWfs = NULL;
    WAVEFORMATEX* pwfxTmp;
    hr = m_pCapture->GetWaveFormat(&pwfxTmp);
    if (FAILED(hr)) 
    {
        XLOG(XFAIL, eError, L"IAudioStreamingCapture::GetWaveFormat(): %s", GetHRString(hr));
        return BoolToFns(FALSE);
    }

    CAutoWaveFormatEx wfx(pwfxTmp);
    CoTaskMemFree(pwfxTmp);

    DWORD duration;
    if(SYNC == m_nSynchronous)
    {
        duration = SYNC_CAPTURE_DURATION;
    }
    else
    {
        duration = ASYNC_CAPTURE_DURATION;
    }

    XLOG(XMSG, eInfo2, L"Capturing %u milliseconds", duration);

    // If this isn't PCM we may have problems here
    DWORD nBytes = duration * wfx->nAvgBytesPerSec / 1000;
    m_pbBuffer = new BYTE[nBytes];
    if (NULL == m_pbBuffer) 
    {
        XLOG(XFAIL, eError, L"Could not allocate %u bytes", nBytes);
        return BoolToFns(FALSE);
    }

    if(SYNC == m_nSynchronous)
    {
        hr = m_pCapture->CaptureBuffer(m_pbBuffer, nBytes);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, L"IAudioStreamingCapture::CaptureBuffer(*, %d): %s", nBytes, GetHRString(hr));
            return BoolToFns(FALSE);
        }
    }
    else
    {
        //create
        m_heStreamingCompletionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if(NULL == m_heStreamingCompletionEvent)
        {
            XLOG(XFAIL, eError, L"Could not create event.");
            return BoolToFns(FALSE);
        }
        m_pCallback = 
            reinterpret_cast<IAudioStreamingAsyncCallback*>(new CCallbackHandler(spLogger, m_heStreamingCompletionEvent, hr));
        if (m_pCallback == NULL)
        {
            XLOG(XFAIL, eError, L"Could not create CallbackHandler. HR = %s", GetHRString(hr));
            return BoolToFns(FALSE);
        }
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, L"Failed initializing the Callback Handler HR = %s", GetHRString(hr));
            return BoolToFns(FALSE);
        }

        hr = m_pCapture->CaptureBufferAsync(m_pbBuffer, (LPBUFFER_SIZE)&nBytes, m_pCallback);
        if (FAILED(hr))
        {
            return BoolToFns(FALSE);
        }    
    }

    m_dwResPre = BoolToFns(fRes);
    return m_dwResPre;
}

// ----------------------------------------------------------
DWORD CCaptureTest::RunPostTest()
{
    BOOL fRes = TRUE;
    HRESULT hr = S_OK;

    if(SYNC == m_nSynchronous)
    {
       //capture again
        CComPtr<IWaveFileSink> pWfs = NULL;
        WAVEFORMATEX* pwfxTmp;
        hr = m_pCapture->GetWaveFormat(&pwfxTmp);
        if (FAILED(hr)) 
        {
            XLOG(XFAIL, eError, L"IAudioStreamingCapture::GetWaveFormat(): %s", GetHRString(hr));
            return BoolToFns(FALSE);
        }

        CAutoWaveFormatEx wfx(pwfxTmp);
        CoTaskMemFree(pwfxTmp);

        XLOG(XMSG, eInfo2, L"Capturing %u milliseconds", SYNC_CAPTURE_DURATION);

        // If this isn't PCM we may have problems here.
        DWORD nBytes = SYNC_CAPTURE_DURATION * wfx->nAvgBytesPerSec / 1000;
        
        //clean up old buffer before allocating the new one
        SAFE_DELETE_ARRAY(m_pbBuffer);
        m_pbBuffer = new BYTE[nBytes];
        if (NULL == m_pbBuffer) 
        {
            XLOG(XFAIL, eError, L"Could not allocate %u bytes", nBytes);
            return BoolToFns(FALSE);
        }

        //Check to see if the device is available
        if (!CheckForDeviceAvailable())
            return BoolToFns(FALSE);

        //device is ready now
        //release the old capturer and make a new one
        SAFE_RELEASE(m_pCapture);
        hr = CreateCapturerForDUT();
        if (FAILED(hr)) 
        {
            XLOG(XFAIL, eError, L"Failed Creating capturer for device under test :%s", GetHRString(hr));
            return BoolToFns(FALSE);
        }

        hr = m_pCapture->CaptureBuffer(m_pbBuffer, nBytes);
        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, L"IAudioStreamingCapture::CaptureBuffer(*, %d): %s", nBytes, GetHRString(hr));
            return BoolToFns(FALSE);
        }
    }
    else
    {   
        //wait for the initial capture to finish
        XLOG(XMSG, eInfo2, L"Waiting for capture completion...");
        if (WAIT_OBJECT_0 != WaitForSingleObject(m_heStreamingCompletionEvent,WAIT_MULTIPLIER * ASYNC_CAPTURE_DURATION))
        {
            // stop streaming so we can release the capture buffer
            hr = m_pCapture->Stop();
            XLOG(XMSG, eInfo2, L"Had to stop manually. hr = %s", GetHRString(hr));
            if(FAILED(hr))
            {
                if (g_pLullabyDeviceTestModule && g_pLullabyDeviceTestModule->GetDevice()->IsBluetoothDevice())
                {
                    // Don't register an error yet for Bluetooth endpoints
                    XLOG(XMSG, eInfo2, L"IAudTestCapture::Stop failed with hr = %s", GetHRString(hr));
                }
                else
                {
                    XLOG(XFAIL, eError, L"IAudTestCapture::Stop failed with hr = %s", GetHRString(hr));
                    fRes = FALSE;
                }
            }
            XLOG(XMSG, eInfo2, L"Capture complete.");
        }
        // For Bluetooth devices, if an error was detected, make sure the device reconnects
        if (g_pLullabyDeviceTestModule && g_pLullabyDeviceTestModule->GetDevice()->IsBluetoothDevice())
        {
            if (m_pCallback && (FAILED(reinterpret_cast<CCallbackHandler*>(m_pCallback)->GetLastStreamingResult())))
            {
                XLOG(XMSG, eInfo2, L"Error reported in callback. hr = %s", GetHRString(reinterpret_cast<CCallbackHandler*>(m_pCallback)->GetLastStreamingResult()));
            }
            if (FAILED(hr) || (m_pCallback && (FAILED(reinterpret_cast<CCallbackHandler*>(m_pCallback)->GetLastStreamingResult()))))
            {
                XLOG(XMSG, eInfo2, L"Checking to make sure device reconnects.");
                //Check to see if the device is available
                if (!CheckForDeviceAvailable())
                    return BoolToFns(FALSE);
                XLOG(XMSG, eInfo2, L"Device successfully reconnected.");
            }
        }
    }
    m_dwResPost = BoolToFns(fRes);
    return m_dwResPost;
}


//******************************************************************************
//  CCSCaptureTest class implementation methods
//******************************************************************************
CCSCaptureTest::CCSCaptureTest( int nSynchronous, ApiEnum api, BOOL bGlitchTest)
    : CCaptureTest(nSynchronous, api), CCSBaseTest(bGlitchTest)
{

}

// ----------------------------------------------------------
CCSCaptureTest::~CCSCaptureTest()
{
}

// ----------------------------------------------------------
BOOL CCSCaptureTest::Initialize()
{
    return (CCSBaseTest::Initialize() && CCaptureTest::Initialize());
}

// ----------------------------------------------------------
void CCSCaptureTest::CleanUp()
{
    CCSBaseTest::CleanUp();
    CCaptureTest::CleanUp();
}

// ----------------------------------------------------------
DWORD CCSCaptureTest::RunPreTest()
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
    dwRes = CCaptureTest::RunPreTest();
    if( FALSE == PASSED(dwRes) )
    {
        return dwRes;
    }

    return BoolToFns(EnterConnectedStandby());
}

// ----------------------------------------------------------
DWORD CCSCaptureTest::RunPostTest()
{
    DWORD dwRes = BoolToFns(ExitConnectedStandby());
    if( FALSE == PASSED(dwRes) )
    {
        return dwRes;
    }

    dwRes = CCaptureTest::RunPostTest();
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
