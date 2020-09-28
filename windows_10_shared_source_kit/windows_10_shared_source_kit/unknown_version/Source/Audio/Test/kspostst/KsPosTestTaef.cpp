// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  KsPosTestTaef.cpp
//
// Abstract:
//
//  TAEF Class Implementation
//



// -------------------------------------------------------------------------------

#include "PreComp.h"
#include "halfdup.h"
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
}\
(void)0


// hopefully no one does pre/post increments on the parameter :)
#define SPEAKER_MASK_FROM_CHANNELS(channels) \
    ((channels == 1) ? KSAUDIO_SPEAKER_MONO : \
    (channels == 2) ? KSAUDIO_SPEAKER_STEREO : \
    (channels == 4) ? KSAUDIO_SPEAKER_QUAD : \
    (channels == 6) ? KSAUDIO_SPEAKER_5POINT1 : \
    (channels == 8) ? KSAUDIO_SPEAKER_7POINT1 : 0)

IBasicLog * g_pBasicLog = NULL;
WDMAudio::KsPosTest * g_pKsPosTst = NULL;

TIMER_MECHANISM g_Timer =
{
    tpQPC, "QueryPerformanceCounter"
};

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

    if (!VERIFY_SUCCEEDED(::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
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

    CoUninitialize();

    return true;
}

bool WDMAudio::KsPosTest::KsPosTestSetup()
{
    // initialize member data;
    m_pHalf = NULL;
    m_fLoop = FALSE;
    m_pTimer = &g_Timer;
    m_fLogHistograms = FALSE;
    g_pKsPosTst = this;
    return true;
}

bool WDMAudio::KsPosTest::KsPosTestCleanup()
{
    g_pKsPosTst = NULL;
    return true;
}

bool WDMAudio::KsPosTest::TestCaseSetup()
{
    CComPtr<ITestResource> spResource;
    CComQIPtr<IHalfAppContainer> spHalfContainer;
    CComBSTR szName;
    CComBSTR szStreaming;
    unsigned int uSampleRate;
    unsigned int uSampleSize;
    unsigned int uChannels;
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    Log::Comment(L"");
    Log::Comment(L"------------------------------------------------------");
    Log::Comment(L"Running test case setup");
    
    // 1. Assign m_fLoop and m_uTestDataIndex
    size_t count = Resources::Count();
    // expect every test case to have a resource
    VERIFY_ARE_EQUAL(count, (size_t)1);

    VERIFY_SUCCEEDED(Resources::Item(0, &spResource));

    VERIFY_SUCCEEDED(spResource->GetValue(
        CComBSTR(TestResourceProperty::c_szName), &szName));

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

    VERIFY_SUCCEEDED(TestData::TryGetValue(L"Index", m_uTestDataIndex));

    Log::Comment(String().Format(L"Testing %s", (PCWSTR)szName));

    Log::Comment(String().Format(L"Pin supports %s streaming", (PCWSTR)szStreaming));

    // 2. Assign m_pHalf
    spHalfContainer = spResource;
    VERIFY_IS_NOT_NULL(spHalfContainer);

    VERIFY_SUCCEEDED(spHalfContainer->GetHalfApp(&m_pHalf));

    // 3. if has data, check format for compatibility
    if (SUCCEEDED(TestData::TryGetValue(L"SampleRate", uSampleRate)))
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

bool WDMAudio::KsPosTest::TestCaseCleanup()
{
    memset(m_pHalf->m_pFmtCurrent, 0, sizeof(FORMAT_ENTRY));
    if (nullptr != m_pHalf->m_pPin)
    {
        m_pHalf->m_pPin->ClosePin();
    }

    m_pHalf = NULL;
    m_fLoop = FALSE;
    return true;
}

void WDMAudio::KsPosTest::TAEF_Capture_Latency()
{
    RUN_TEST_CASE(Test_Latency);
}

void WDMAudio::KsPosTest::TAEF_Render_Latency()
{
    RUN_TEST_CASE(Test_Latency);
}

void WDMAudio::KsPosTest::TAEF_Capture_DurationLength()
{
    RUN_TEST_CASE(Test_DurationLength);
}

void WDMAudio::KsPosTest::TAEF_Render_DurationLength()
{
    RUN_TEST_CASE(Test_DurationLength);
}

void WDMAudio::KsPosTest::TAEF_Capture_Progression()
{
    RUN_TEST_CASE(Test_Progression);
}

void WDMAudio::KsPosTest::TAEF_Render_Progression()
{
    RUN_TEST_CASE(Test_Progression);
}

void WDMAudio::KsPosTest::TAEF_Capture_ReadPacket()
{
    RUN_TEST_CASE(Test_ReadPacket);
}

void WDMAudio::KsPosTest::TAEF_Capture_KeywordBurstRead()
{
    RUN_TEST_CASE(Test_KeywordBurstRead);
}

void WDMAudio::KsPosTest::TAEF_Capture_DriftAndJitter()
{
    RUN_TEST_CASE(Test_DriftAndJitter);
}

void WDMAudio::KsPosTest::TAEF_Render_DriftAndJitter()
{
    RUN_TEST_CASE(Test_DriftAndJitter);
}

void WDMAudio::KsPosTest::TAEF_Render_GetPosition()
{
    RUN_TEST_CASE(Test_GetPosition);
}

void WDMAudio::KsPosTest::TAEF_Render_GetPositionMult()
{
    RUN_TEST_CASE(Test_GetPositionMult);
}

void WDMAudio::KsPosTest::TAEF_Render_WritePacket()
{
    RUN_TEST_CASE(Test_WritePacket);
}

void WDMAudio::KsPosTest::TAEF_Capture_DriftAndJitter_ForAEC()
{
    RUN_TEST_CASE(Test_DriftAndJitter_ForAEC);
}

void WDMAudio::KsPosTest::TAEF_Render_DriftAndJitter_ForAEC()
{
    RUN_TEST_CASE(Test_DriftAndJitter_ForAEC);
}

void WDMAudio::KsPosTest::TAEF_Render_ERT_PCMStartLatency()
{
    RUN_TEST_CASE(Test_ERT_PCMStartLatency);
}

void WDMAudio::KsPosTest::TAEF_Render_ERT_PauseLatency()
{
    RUN_TEST_CASE(Test_ERT_PauseLatency);
}

void WDMAudio::KsPosTest::TAEF_Render_ERT_StopLatency()
{
    RUN_TEST_CASE(Test_ERT_StopLatency);
}

OACR_WARNING_POP
