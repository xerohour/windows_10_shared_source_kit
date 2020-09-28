// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  WaveTestTaef.cpp
//
// Abstract:
//
//  Implementation file for WaveTestTaef
//



// -------------------------------------------------------------------------------
#include "PreComp.h"
#include "WaveTestTaef.h"
#include "tests.h"

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

#define RUN_TEST_CASE(testfn)\
{\
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);\
    DWORD dwRes = testfn(); \
    VERIFY_ARE_NOT_EQUAL(dwRes, (DWORD)FNS_FAIL); \
    if(dwRes != FNS_PASS)\
    {\
        if(dwRes == FNS_SKIPPED){Log::Result(TestResults::Result::Skipped);} \
        else if(dwRes == FNS_ABORTED){Log::Result(TestResults::Result::Blocked);} \
        else{Log::Result(TestResults::Result::Unknown);} \
    }\
}

#define SPEAKER_MASK_FROM_CHANNELS(channels) \
    ((channels == 1) ? KSAUDIO_SPEAKER_MONO : \
    (channels == 2) ? KSAUDIO_SPEAKER_STEREO : \
    (channels == 4) ? KSAUDIO_SPEAKER_QUAD : \
    (channels == 6) ? KSAUDIO_SPEAKER_5POINT1 : \
    (channels == 8) ? KSAUDIO_SPEAKER_7POINT1 : 0)

IBasicLog * g_pBasicLog = NULL;
WDMAudio::WaveTest * g_pWaveTest = NULL;

// If the module is declared in a header
namespace WDMAudio
{
    BEGIN_MODULE()
        MODULE_PROPERTY(L"Feature", L"PortCls HCK Tests")
        MODULE_PROPERTY(L"TestResourceDependent", L"true")
    END_MODULE()

    // Create WEXBasicLog, Coinitialize Etc
    MODULE_SETUP(WDMAudioSetup)

    //Release stuff acquired in WDMAudioSetup
    MODULE_CLEANUP(WDMAudioCleanup)
};


bool WDMAudio::WDMAudioSetup()
{
    if(NULL == g_pBasicLog)
    {
        if(!(VERIFY_SUCCEEDED(CreateWexBasicLog(&g_pBasicLog))))
        {
            return false;
        }
    }

    if(!VERIFY_SUCCEEDED(::CoInitializeEx(NULL, COINIT_MULTITHREADED)))
    {
        return false;
    }

    return true;
}

bool WDMAudio::WDMAudioCleanup()
{
    if(g_pBasicLog)
    {
        g_pBasicLog->Release();
    }

    return true;
}

/////////////////////////
bool WDMAudio::WaveTest::WaveTestSetup()
{
    // initialize member data;
    m_pHalf = NULL;
    m_fLoop = FALSE;
    //m_pTimer = &g_Timer;
    m_fLogHistograms = FALSE;
    g_pWaveTest = this;
    m_pDataFormat = NULL;
    m_DataFormatSize = 0;

    return true;
}

bool WDMAudio::WaveTest::WaveTestCleanUp()
{
    g_pWaveTest = NULL;
    // m_pDataFormat TODO free?
    return true;
}

bool WDMAudio::WaveTest::TestCaseSetup()
{
    CComPtr<ITestResource> spResource;
    CComQIPtr<IHalfAppContainer> spHalfContainer;
    CComBSTR szStreaming;
    BOOL bUseDeviceFormat = FALSE;
    unsigned int uSampleRate;
    unsigned int uSampleSize;
    unsigned int uChannels;
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    Log::Comment(L"");
    Log::Comment(L"------------------------------------------------------");
    Log::Comment(L"Running test case setup");
    
    // 1. Assign m_fLoop
    size_t count = Resources::Count();
    // expect every test case to have a resource
    VERIFY_ARE_EQUAL(count, (size_t)1); //TODO greater or equal?

    VERIFY_SUCCEEDED(Resources::Item(0, &spResource));

    VERIFY_SUCCEEDED(spResource->GetValue(
        CComBSTR(TestResourceProperty::c_szStreaming), &szStreaming));

    if(0 == wcscmp(szStreaming, L"Standard Looped"))
    {
        m_fLoop = TRUE;
    }
    else
    {
        VERIFY_IS_TRUE(0 == wcscmp(szStreaming, L"Standard"));
    }

    Log::Comment(String().Format(L"Pin supports %s streaming", (PCWSTR)szStreaming));

    // 2. Assign m_pHalf
    spHalfContainer = spResource;
    VERIFY_IS_NOT_NULL(spHalfContainer);

    VERIFY_SUCCEEDED(spHalfContainer->GetHalfApp(&m_pHalf));

    // 3. if has data, check format for compatibility
    if (!IsEqualGUID(m_pHalf->m_Mode, GUID_NULL))
    {
        bUseDeviceFormat = m_pHalf->m_pPin->SetViablePCMWfxt(NULL, m_pHalf->m_Mode, TRUE);
        if (bUseDeviceFormat)
        {
            PWAVEFORMATEX pWfxProposed = m_pHalf->m_pPin->GetFormat();
            PWAVEFORMATEXTENSIBLE pWfxEx;
            LPWAVEFORMATEX pWfx;

            memset(m_pHalf->m_pFmtCurrent, 0, sizeof(FORMAT_ENTRY));
            pWfxEx = &m_pHalf->m_pFmtCurrent->wfxFormat;
            pWfx = &pWfxEx->Format;

            if (WAVE_FORMAT_EXTENSIBLE == pWfxProposed->wFormatTag)
            {
                memcpy(pWfxEx, pWfxProposed, sizeof(*pWfxEx));
            }
            else
            {
                memcpy(pWfx, pWfxProposed, sizeof(*pWfx));
            }

            Log::Comment(L"Specific Audio Processing Mode specified, test will run on driver-proposed data format:");
            LogWaveFormat(g_pBasicLog, pWfxProposed);
        }
    }
    if (!bUseDeviceFormat && SUCCEEDED(TestData::TryGetValue(L"SampleRate", uSampleRate)))
    {
        bool fRequired;
        PWAVEFORMATEXTENSIBLE pWfxEx;
        LPWAVEFORMATEX pWfx;
        // we got format data
        VERIFY_SUCCEEDED(TestData::TryGetValue(L"SampleSize", uSampleSize));
        VERIFY_SUCCEEDED(TestData::TryGetValue(L"Channels", uChannels));
        VERIFY_SUCCEEDED(TestData::TryGetValue(L"Required", fRequired));

        memset(m_pHalf->m_pFmtCurrent, 0, sizeof(FORMAT_ENTRY));
        pWfxEx = &m_pHalf->m_pFmtCurrent->wfxFormat;
        pWfx = &pWfxEx->Format;

        // intersect and set as the current format
        pWfx->wFormatTag = WAVE_FORMAT_EXTENSIBLE;
        pWfx->cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
        pWfx->wBitsPerSample = (uSampleSize > 16) ? 32 : (WORD)uSampleSize;
        pWfx->nChannels = (WORD)uChannels;
        pWfx->nSamplesPerSec = (DWORD)uSampleRate;
        pWfx->nBlockAlign = pWfx->wBitsPerSample * pWfx->nChannels / 8;
        pWfx->nAvgBytesPerSec = pWfx->nBlockAlign * pWfx->nSamplesPerSec;

        pWfxEx->Samples.wValidBitsPerSample = (WORD)uSampleSize;
        pWfxEx->dwChannelMask = SPEAKER_MASK_FROM_CHANNELS(uChannels);
        pWfxEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

        // double check with the pin to see if it can support this format.  
        // This check is to circumvent the deficiencies in KSDATARANGE_AUDIO,
        // namely the lack of nMinChannels
        if (!m_pHalf->m_pPin->DoesFormatIntersect(pWfx, TRUE))
        {
            // If the WAVEFORMATEXTENSIBLE version is not supported, then 
            // fill and check WAVEFORMATEX version of this format
            Log::Comment(L"WAVEFORMATEXTENSIBLE does not intersect. Trying WAVEFORMATEX");

            pWfx->wFormatTag = WAVE_FORMAT_PCM;
            pWfx->cbSize = 0;
                    
            if (!m_pHalf->m_pPin->DoesFormatIntersect(pWfx, FALSE))
            {
                if(fRequired)
                {
                    Log::Error(L"Requested format is required but does not intersect with what the pin supports.");
                    return true;
                }
                else
                {
                    Log::Comment(L"Requested format does not intersect with what the pin supports.");
                    Log::Result(TestResults::Skipped);
                    return true;
                }
            }
        }

        Log::Comment(L"Test case will be run on the following format:");
        LogWaveFormat(g_pBasicLog, pWfx);
    }

    return true;
}

bool WDMAudio::WaveTest::TestCaseCleanUp()
{
    memset(m_pHalf->m_pFmtCurrent, 0, sizeof(FORMAT_ENTRY));
    m_pHalf = NULL;
    m_fLoop = FALSE;

    return true;
}

void WDMAudio::WaveTest::TAEF_Render_ResetPin()
{
    RUN_TEST_CASE(Test_ResetPin)
}

void WDMAudio::WaveTest::TAEF_Capture_ResetPin()
{
    RUN_TEST_CASE(Test_ResetPin)
}

void WDMAudio::WaveTest::TAEF_Render_PinStateChanges()
{
    RUN_TEST_CASE(Test_PinStateChanges)
}

void WDMAudio::WaveTest::TAEF_Capture_PinStateChanges()
{
    RUN_TEST_CASE(Test_PinStateChanges)
}

void WDMAudio::WaveTest::TAEF_Render_ChangeAudioFormat()
{
    RUN_TEST_CASE(Test_ChangeAudioFormat)
}

void WDMAudio::WaveTest::TAEF_Capture_ChangeAudioFormat()
{
    RUN_TEST_CASE(Test_ChangeAudioFormat)
}

void WDMAudio::WaveTest::TAEF_Render_CancelIO()
{
    RUN_TEST_CASE(Test_CancelIO)
}

void WDMAudio::WaveTest::TAEF_Render_StreamingMultiThread()
{
    RUN_TEST_CASE(Test_StreamingMultiThread)
}

void WDMAudio::WaveTest::TAEF_Render_StreamingSmallBuffer()
{
    RUN_TEST_CASE(Test_StreamingSmallBuffer)
}

void WDMAudio::WaveTest::TAEF_Render_GetPositionMultiThread()
{
    RUN_TEST_CASE(Test_GetPositionMultiThread)
}

void WDMAudio::WaveTest::TAEF_Capture_GetPositionMultiThread()
{
    RUN_TEST_CASE(Test_GetPositionMultiThread)
}

void WDMAudio::WaveTest::TAEF_Render_StreamingMultiPin()
{
    RUN_TEST_CASE(Test_StreamingMultiPin)
}

void WDMAudio::WaveTest::TAEF_Render_StreamingMultiPinDifferentFormat()
{
    RUN_TEST_CASE(Test_StreamingMultiPinDifferentFormat)
}
    
void WDMAudio::WaveTest::TAEF_Render_PinInstantiateCheck()
{
    RUN_TEST_CASE(Test_PinInstantiateCheck)
}

void WDMAudio::WaveTest::TAEF_Capture_PinInstantiateCheck()
{
    RUN_TEST_CASE(Test_PinInstantiateCheck)
}

void WDMAudio::WaveTest::TAEF_Render_DataIntersection()
{
    RUN_TEST_CASE(Test_RenderDataIntersection)
}

void WDMAudio::WaveTest::TAEF_Capture_DataIntersection()
{
    RUN_TEST_CASE(Test_CaptureDataIntersection)
}

void WDMAudio::WaveTest::TAEF_Render_EnumerateAudioEndpoint()
{
    RUN_TEST_CASE(Test_EnumerateAudioEndpoint)
}

void WDMAudio::WaveTest::TAEF_Capture_EnumerateAudioEndpoint()
{
    RUN_TEST_CASE(Test_EnumerateAudioEndpoint)
}

void WDMAudio::WaveTest::TAEF_Render_VerifyPinIsWaveRT()
{
    RUN_TEST_CASE(Test_VerifyPinIsWaveRT)
}

void WDMAudio::WaveTest::TAEF_Capture_VerifyPinIsWaveRT()
{
    RUN_TEST_CASE(Test_VerifyPinIsWaveRT)
}

void WDMAudio::WaveTest::TAEF_Render_VerifyPinSupportsPullMode()
{
    RUN_TEST_CASE(Test_VerifyPinSupportsPullMode)
}

void WDMAudio::WaveTest::TAEF_Capture_VerifyPinSupportsPullMode()
{
    RUN_TEST_CASE(Test_VerifyPinSupportsPullMode)
}

void WDMAudio::WaveTest::TAEF_Render_VerifyRawOrDefaultProcessingSupported()
{
    RUN_TEST_CASE(Test_VerifyRenderRawOrDefaultProcessingSupported)
}

void WDMAudio::WaveTest::TAEF_Capture_VerifyRawOrDefaultProcessingSupported()
{
    RUN_TEST_CASE(Test_VerifyCaptureRawOrDefaultProcessingSupported)
}
