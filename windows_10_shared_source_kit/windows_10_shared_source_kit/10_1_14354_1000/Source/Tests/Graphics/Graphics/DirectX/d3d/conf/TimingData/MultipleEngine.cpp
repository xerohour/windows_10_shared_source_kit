////////////////////////////////////////////////////////////////////////////////
// MultipleEngine.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "MultipleEngine.h"

////////////////////////////////////////////////////////////////////////////////
//
// CMultipleEngineTest
//
////////////////////////////////////////////////////////////////////////////////

CMultipleEngineTest::CMultipleEngineTest() :
    m_pEngineHelper(nullptr)
{
}

TEST_RESULT CMultipleEngineTest::SetupTestCase()
{
    if(GetFramework()->IsCompat())
    {
        return RESULT_SKIP;
    }
    m_pEngineHelper = new CDxEtwHelper(ETL_FILE);

    TEST_RESULT tr = CTimingDataTest::SetupTestCase();
    HRESULT hr = S_OK;

    if(tr != RESULT_PASS)
    {
        return tr;
    }

    tr = SetupPipeline();

    return tr;
}

void CMultipleEngineTest::CleanupTestCase()
{
    SAFE_DELETE(m_pEngineHelper);
    
    CTimingDataTest::CleanupTestCase();

    CleanupPipeline();
}

TEST_RESULT CMultipleEngineTest::ValidateEngine()
{
    vector<CDMALatencyDetail*> *pEngineList = m_pEngineHelper->GatherDMALatencyDetails(ENGINE_RESULTS_FILE, L"", L"");  
    if(pEngineList == nullptr || pEngineList->size() == 0)
    {
        LogError(__FILEW__, __LINE__, L"Expected at least one kernel engine event",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCRequiredCapabilityNotSupported); 
        return RESULT_FAIL;
    }

    vector<CEvent_CalibrateGpuClock*> calibrateList;
    TEST_RESULT tr = FilterCalibrateEvents(&calibrateList); 
    if(tr != RESULT_PASS || calibrateList.size() == 0)
    {
        LogError(__FILEW__, __LINE__, L"Expected at least one Calibrate event",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCRequiredCapabilityNotSupported); 
        return RESULT_FAIL;
    }
    
    // engine ordinals must match in engine and Calibrate events
    const UINT32 dmaEngineOrdinal = (*pEngineList)[0]->m_ulEngineOrdinal;
    const UINT32 CalibrateEngineOrdinal = (UINT32)calibrateList[0]->EngineOrdinal();
    for(CDMALatencyDetail* DmaDetail : (*pEngineList) )
    {
        if(wcscmp(DmaDetail->m_wszQueuePacketName, L"Render") != 0)
        {
            continue;
        }
        bool bFoundOrdinal = false;
        for(CEvent_CalibrateGpuClock* CalibrateEvent : calibrateList )
        {
            if (CalibrateEvent->EngineOrdinal() == DmaDetail->m_ulEngineOrdinal)
            {
                bFoundOrdinal = true;
                break;
            }
        }
        if (!bFoundOrdinal)
        {
            WriteToLog(_T("Engine ordinal from DMA Engine event: %lu for Dma ID %lu."), DmaDetail->m_ulEngineOrdinal, DmaDetail->m_ullSubmissionId);
            LogError(__FILEW__, __LINE__, L"Engine ordinal values from DMA Engine ETW event and GPU Calibrate events do not match",
                false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
            return RESULT_FAIL;
        }
    }

    return RESULT_PASS;
}

////////////////////////////////////////////////////////////////////////////////
// CMultipleEngineTest_Draw
////////////////////////////////////////////////////////////////////////////////

TEST_RESULT CMultipleEngineTest_Draw::ExecuteTestCase()
{
    HRESULT hr = E_FAIL;
    TEST_RESULT tr = StartETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }
    
    m_pDeviceContext->Draw(4, 0);

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute); 
        return RESULT_FAIL;
    }

    tr = StopETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    // validation
    tr = ValidateEngine();
    if(tr != RESULT_PASS)
    {
        // ValidateEngine() will log cause of failure
        return tr;
    }

    return RESULT_PASS;
}

////////////////////////////////////////////////////////////////////////////////
// CMultipleEngineTest_Copy
////////////////////////////////////////////////////////////////////////////////

CMultipleEngineTest_Copy::CMultipleEngineTest_Copy() :
    m_pDestTex(nullptr)
{

}

TEST_RESULT CMultipleEngineTest_Copy::SetupTestCase()
{
    TEST_RESULT tr = CMultipleEngineTest::SetupTestCase();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    HRESULT hr = E_FAIL;

    D3D11_TEXTURE2D_DESC tex2DDesc;
    tex2DDesc.Width = 1;
    tex2DDesc.Height = 1;
    tex2DDesc.MipLevels = 1;
    tex2DDesc.ArraySize = 1;
    tex2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex2DDesc.SampleDesc.Count = 1;
    tex2DDesc.SampleDesc.Quality = 0;
    tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
    tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    tex2DDesc.CPUAccessFlags = 0;
    tex2DDesc.MiscFlags = 0;

    if(FAILED(hr = m_pDevice->CreateTexture2D(&tex2DDesc, nullptr, &m_pDestTex)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create update destination texture", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource); 
        return RESULT_FAIL;
    }

    return RESULT_PASS;
}

TEST_RESULT CMultipleEngineTest_Copy::ExecuteTestCase()
{
    HRESULT hr = E_FAIL;
    TEST_RESULT tr = StartETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }
    
    UINT32 i = 0xff00ff00;
    m_pDeviceContext->UpdateSubresource(m_pDestTex, 0, nullptr, (void*)&i, sizeof(UINT32), 0);

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute); 
        return RESULT_FAIL;
    }

    tr = StopETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    // validation
    tr = ValidateEngine();
    if(tr != RESULT_PASS)
    {
        // ValidateEngine() will log cause of failure
        return tr;
    }

    return RESULT_PASS;
}

void CMultipleEngineTest_Copy::CleanupTestCase()
{
    CMultipleEngineTest::CleanupTestCase();

    SAFE_RELEASE(m_pDestTex);
}

////////////////////////////////////////////////////////////////////////////////
// CMultipleEngineTest_Compute
////////////////////////////////////////////////////////////////////////////////

TEST_RESULT CMultipleEngineTest_Compute::Setup()
{
    if(GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0)
    {
        WriteToLog(_T("Skipping test group due to lack of compute shader support on feature level 9"));
        return RESULT_SKIP;
    }

    return CMultipleEngineTest::Setup();
}

TEST_RESULT CMultipleEngineTest_Compute::ExecuteTestCase()
{
    HRESULT hr = E_FAIL;
    TEST_RESULT tr = StartETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }
    
    m_pDeviceContext->Dispatch(1, 1, 1);

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute); 
        return RESULT_FAIL;
    }

    tr = StopETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    // validation
    tr = ValidateEngine();
    if(tr != RESULT_PASS)
    {
        // ValidateEngine() will log cause of failure
        return tr;
    }

    return RESULT_PASS;
}
