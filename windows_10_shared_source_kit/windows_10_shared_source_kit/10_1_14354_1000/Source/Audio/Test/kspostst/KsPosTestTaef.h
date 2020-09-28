// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  KsPosTestTaef.h
//
// Abstract:
//
//  TAEF Class Declaration
//



// -------------------------------------------------------------------------------

#ifndef _KSPOSTESTTAEF_H_
#define _KSPOSTESTTAEF_H_

extern IBasicLog * g_pBasicLog;

// define custom test resource properties
namespace WEX { namespace TestExecution { namespace TestResourceProperty
{
    static const wchar_t c_szStreaming[]    = L"Streaming";
    static const wchar_t c_szPinId[]        = L"PinId";
} } }

namespace WDMAudio
{
    class KsPosTest
    {
        BEGIN_TEST_CLASS(KsPosTest)
            TEST_CLASS_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client x86")
            TEST_CLASS_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client x64")
            TEST_CLASS_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Server x64")
            TEST_CLASS_PROPERTY(L"Kits.MinRelease", L"TH1")
            TEST_CLASS_PROPERTY(L"Kits.CorePackageComposition", L"Full")
            TEST_CLASS_PROPERTY(L"Kits.IsInProc", L"True")
            TEST_CLASS_PROPERTY(L"Kits.Parameter", L"DeviceID")
            TEST_CLASS_PROPERTY(L"Kits.Parameter.DeviceID.Description", L"Device id of device under test")
            TEST_CLASS_PROPERTY(L"Kits.PublishingOrganization", L"Microsoft Corporation")
            TEST_CLASS_PROPERTY(L"Kits.TestType", L"Development")
            TEST_CLASS_PROPERTY(L"Kits.DevelopmentPhase", L"Development and Integration")
            TEST_CLASS_PROPERTY(L"Subfeature", L"KS Position Tests")
        END_TEST_CLASS()

        TEST_CLASS_SETUP(KsPosTestSetup)

        TEST_CLASS_CLEANUP(KsPosTestCleanup)

        TEST_METHOD_SETUP(TestCaseSetup)

        TEST_METHOD_CLEANUP(TestCaseCleanup)

        BEGIN_TEST_METHOD(TAEF_Capture_Latency)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#CaptureFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Render_Latency)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#RenderFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Capture_DurationLength)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#CaptureFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Render_DurationLength)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#RenderFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Capture_Progression)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#CaptureFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Render_Progression)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#RenderFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Capture_ReadPacket)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#CaptureFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Capture_KeywordBurstRead)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#CaptureFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Capture_DriftAndJitter)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#CaptureFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Render_DriftAndJitter)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#RenderFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Render_GetPosition)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#RenderFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Render_GetPositionMult)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#RenderFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
        END_TEST_METHOD()
        
        BEGIN_TEST_METHOD(TAEF_Render_WritePacket)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#RenderFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Capture_DriftAndJitter_ForAEC)
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Render_DriftAndJitter_ForAEC)
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Render_ERT_PCMStartLatency)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#RenderFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Render_ERT_PauseLatency)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#RenderFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
        END_TEST_METHOD()

        BEGIN_TEST_METHOD(TAEF_Render_ERT_StopLatency)
            TEST_METHOD_PROPERTY(L"DataSource", L"Table:KsPosTestDataSource.xml#RenderFormats")
            TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
        END_TEST_METHOD()

    public:
        unsigned            m_uTestDataIndex;
        CHalfApp *          m_pHalf;
        BOOL                m_fLoop;
        PTIMER_MECHANISM    m_pTimer;
        BOOL                m_fLogHistograms;
    };
}

extern WDMAudio::KsPosTest * g_pKsPosTst;

#endif
