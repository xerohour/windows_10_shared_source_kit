////////////////////////////////////////////////////////////////////////////////
// CustomEvent.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "CustomEvent.h"

////////////////////////////////////////////////////////////////////////////////
// CCustomEventTest
////////////////////////////////////////////////////////////////////////////////

CCustomEventTest::CCustomEventTest() 
{
}

TEST_RESULT CCustomEventTest::Setup()
{
    TEST_RESULT tr = CD3D11Test::Setup();
    HRESULT hr = S_OK;

    if(tr != RESULT_PASS)
    {
        return tr;
    }

    m_pDevice = GetDevice2();
    m_pDeviceContext = GetEffectiveContext2();

    // Ensure device doesn't support markers
    D3D11_FEATURE_DATA_MARKER_SUPPORT MarkerData;
    if(FAILED(hr = m_pDevice->CheckFeatureSupport( D3D11_FEATURE_MARKER_SUPPORT, &MarkerData, sizeof( MarkerData ) ) ))
    {
        LogError(__FILEW__, __LINE__, L"Call to CheckFeatureSupport() failed", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCheckFeatureSupport); 
        return RESULT_FAIL;
    }

    if(MarkerData.Profile == TRUE)
    {
        WriteToLog(_T("Skipping due to marker data support, this is a feature leak test"));
        return RESULT_SKIP;
    }

    return tr;
}

TEST_RESULT CCustomEventTest::ExecuteTestCase()
{
    HRESULT hr = E_FAIL;
    TEST_RESULT tr = StartETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }
    
    // texture creation with init data and draw with SRV bound most likely to trigger non-required custom events
    tr = SetupPipeline();
    if(tr != RESULT_PASS)
    {
        goto Fail;
    }

    m_pDeviceContext->Draw(4, 0);

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute); 
        goto Fail;
    }

    tr = StopETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    // verify that no custom driver events were fired
    vector<CEvent_CustomDriverMarker*> *pCustomEventList;
    if(GetFramework()->Is11On12())
    {
        m_Results.Task = Direct3D11EtwProviderGuid_TASK_CustomDriverMarker;
        GatherSpecifiedEvents();
        pCustomEventList = &m_Results.CustomVector;
    }
    else if(GetFramework()->IsCompat())
    {
        m_Results.Task = Direct3D11EtwProviderGuid_TASK_CustomDriverMarker;
        GatherSpecifiedEvents();
        pCustomEventList = &m_Results.CustomVector;
    }
    else
    {
        pCustomEventList = m_pTimingDataHelper->GatherCustomDriverMarkerList(RESULTS_FILE, m_wszProcessOption);
    }
    if(pCustomEventList != nullptr && pCustomEventList->size() > 0)
    {
        LogError(__FILEW__, __LINE__, L"Custom driver events were fired when the marker mode is set to 0", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
        goto Fail;
    }   

    goto Done;

Fail:
    tr = RESULT_FAIL;
    StopETWTracing();

Done:
    CleanupPipeline();

    return tr;
}
