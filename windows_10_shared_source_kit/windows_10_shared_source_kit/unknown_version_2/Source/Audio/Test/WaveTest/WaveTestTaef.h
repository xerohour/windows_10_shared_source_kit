// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  WaveTestTaef.h
//
// Abstract:
//
//  Include file for WaveTestTaef
//



// -------------------------------------------------------------------------------
#pragma once

#include "halfdup.h"

extern IBasicLog * g_pBasicLog;

// define custom test resource properties
namespace WEX { namespace TestExecution { namespace TestResourceProperty
{
    static const wchar_t c_szStreaming[]    = L"Streaming";
    static const wchar_t c_szPinId[]        = L"PinId";
} } }

namespace WDMAudio
{

class WaveTest
{
    BEGIN_TEST_CLASS(WaveTest)
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
        TEST_CLASS_PROPERTY(L"Subfeature", L"Wave Driver Tests")
    END_TEST_CLASS()

    TEST_CLASS_SETUP(WaveTestSetup)
    TEST_CLASS_CLEANUP(WaveTestCleanUp)

    TEST_METHOD_SETUP(TestCaseSetup)
    TEST_METHOD_CLEANUP(TestCaseCleanUp)

    BEGIN_TEST_METHOD(TAEF_Render_ResetPin)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"DataSource", L"Table:WaveTestDataSource.xml#RenderFormats")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Capture_ResetPin)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"DataSource", L"Table:WaveTestDataSource.xml#CaptureFormats")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Render_ChangeAudioFormat)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"DataSource", L"Table:WaveTestDataSource.xml#RenderFormats")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Capture_ChangeAudioFormat)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"DataSource", L"Table:WaveTestDataSource.xml#CaptureFormats")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Render_PinStateChanges)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"DataSource", L"Table:WaveTestDataSource.xml#RenderFormats")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Capture_PinStateChanges)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"DataSource", L"Table:WaveTestDataSource.xml#CaptureFormats")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Render_CancelIO)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"DataSource", L"Table:WaveTestDataSource.xml#RenderFormats")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Render_StreamingMultiThread)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"DataSource", L"Table:WaveTestDataSource.xml#RenderFormats")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Render_StreamingSmallBuffer)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"DataSource", L"Table:WaveTestDataSource.xml#RenderFormats")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Render_GetPositionMultiThread)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"DataSource", L"Table:WaveTestDataSource.xml#RenderFormats")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Capture_GetPositionMultiThread)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"DataSource", L"Table:WaveTestDataSource.xml#CaptureFormats")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Render_StreamingMultiPin)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"DataSource", L"Table:WaveTestDataSource.xml#RenderFormats")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Render_StreamingMultiPinDifferentFormat)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"DataSource", L"Table:WaveTestDataSource.xml#RenderFormats")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Render_PinInstantiateCheck)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Capture_PinInstantiateCheck)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Render_DataIntersection)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Capture_DataIntersection)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Functional")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Render_EnumerateAudioEndpoint)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Capture_EnumerateAudioEndpoint)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Render_VerifyPinIsWaveRT)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Capture_VerifyPinIsWaveRT)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
    END_TEST_METHOD()
    
    BEGIN_TEST_METHOD(TAEF_Render_VerifyPinSupportsPullMode)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Capture_VerifyPinSupportsPullMode)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Render_VerifyRawOrDefaultProcessingSupported)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Render')")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TAEF_Capture_VerifyRawOrDefaultProcessingSupported)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"(@Type='Capture')")
    END_TEST_METHOD()
    
public:
    CHalfApp *          m_pHalf;
    BOOL                m_fLoop;
    PTIMER_MECHANISM    m_pTimer;
    BOOL                m_fLogHistograms;

    PKSDATAFORMAT   	m_pDataFormat;		// added to hold the dataformat returned by the FindIntersection method of the CWaveTest
    ULONG               m_DataFormatSize; 	// added to hold the lenght of the data hold by the m_pDataFormat
};

}

extern WDMAudio::WaveTest * g_pWaveTest;
