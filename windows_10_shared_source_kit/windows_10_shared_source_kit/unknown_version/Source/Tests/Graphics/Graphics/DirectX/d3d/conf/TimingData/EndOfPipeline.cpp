////////////////////////////////////////////////////////////////////////////////
// EndOfPipeline.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "EndOfPipeline.h"

////////////////////////////////////////////////////////////////////////////////
//
// CEndOfPipelineTest
//
////////////////////////////////////////////////////////////////////////////////

#define LARGE_RT_SIZE 1024
#define SOURCE_TEX_SIZE 512
#define DELTA_FAILURE_TOLERANCE 10.0f

string g_szBlurPS = 
"   Texture2D t; \n"
"   sampler s; \n"
"   \n"
"   struct PSIn \n"
"   { \n"
"       float4 pos : SV_Position; \n"
"       float2 tex : TEXCOORD; \n"
"   }; \n"
"   \n"
"   float4 PSMain(PSIn input) : SV_TARGET \n"
"   {    \n"
"       float step = 1.0f / 512.0f; \n"
"       float4 color = ( \n"
"           t.Sample(s, float2(input.tex.x - step, input.tex.y - step)) + \n"
"           t.Sample(s, float2(input.tex.x, input.tex.y - step)) + \n"
"           t.Sample(s, float2(input.tex.x + step, input.tex.y - step)) + \n"
"           t.Sample(s, float2(input.tex.x - step, input.tex.y)) + \n"
"           t.Sample(s, float2(input.tex.x, input.tex.y)) + \n"
"           t.Sample(s, float2(input.tex.x + step, input.tex.y)) + \n"
"           t.Sample(s, float2(input.tex.x - step, input.tex.y + step)) + \n"
"           t.Sample(s, float2(input.tex.x, input.tex.y + step)) + \n"
"           t.Sample(s, float2(input.tex.x + step, input.tex.y + step)) \n"
"           ) / 9.0f; \n"
"       return (color); \n"
"   } \n";

CEndOfPipelineTest::CEndOfPipelineTest() :
    m_pRTTex_Large(nullptr),
    m_pRTView_Large(nullptr)
{
}

TEST_RESULT CEndOfPipelineTest::SetupTestCase()
{
    TEST_RESULT tr = CTimingDataTest::SetupTestCase();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    m_szPS = g_szBlurPS;
    m_SRWidth = SOURCE_TEX_SIZE;
    m_SRHeight = SOURCE_TEX_SIZE;

    tr = SetupPipeline();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    HRESULT hr = E_FAIL;

    // create resources for pass on large render target
    D3D11_TEXTURE2D_DESC tex2DDesc;
    tex2DDesc.Width = LARGE_RT_SIZE;
    tex2DDesc.Height = LARGE_RT_SIZE;
    tex2DDesc.MipLevels = 1;
    tex2DDesc.ArraySize = 1;
    tex2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex2DDesc.SampleDesc.Count = 1;
    tex2DDesc.SampleDesc.Quality = 0;
    tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
    tex2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
    tex2DDesc.CPUAccessFlags = 0;
    tex2DDesc.MiscFlags = 0;

    if(FAILED(hr = m_pDevice->CreateTexture2D(&tex2DDesc, nullptr, &m_pRTTex_Large)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create texture for large render target", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource); 
        return RESULT_FAIL;
    }

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    if(FAILED(hr = m_pDevice->CreateRenderTargetView(m_pRTTex_Large, &rtvDesc, &m_pRTView_Large)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create view for large render target", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource); 
        return RESULT_FAIL;
    }

    return tr;
}

TEST_RESULT CEndOfPipelineTest::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = E_FAIL;

    if(GetFramework()->IsMobile())
    {
        // Force state to be set by initial draw
        tr = StartETWTracing(true);
    }
    else
    {
        tr = StartETWTracing();
    }
    
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    LARGE_INTEGER qpcTimestamps[2];
    LARGE_INTEGER cpuFrequency;
    UINT64 gpuTimestamps[2];
    vector<CEvent_HistoryBuffer*> timestampEvents;
    vector<UINT64> timestamps;
    vector<CEvent_CalibrateGpuClock*> calibrateEvents;

    // 1x1 RT pass
    if(QueryPerformanceCounter(&qpcTimestamps[0]) == FALSE)
    {
        LogError(__FILEW__, __LINE__, L"QueryPerformanceCounter() failed", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCQueryPerformanceCounter); 
        goto Fail;
    }

    m_pDeviceContext->Draw(4, 0);

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported); 
        return RESULT_FAIL;
    }

    m_pDeviceContext->Flush();

    // 1024x1024 RT pass
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRTView_Large, nullptr);

    m_pDeviceContext->Draw(4, 0);

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported); 
        return RESULT_FAIL;
    }

    if(QueryPerformanceCounter(&qpcTimestamps[1]) == FALSE)
    {
        LogError(__FILEW__, __LINE__, L"QueryPerformanceCounter() failed", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCQueryPerformanceCounter); 
        goto Fail;
    }

    tr = StopETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    CResourceViewerWindow *pWindow = ShowResource(L"1x1 render target",  m_pRTTex, 0, 0, 0, 5 );
    pWindow = ShowResource(L"large render target",  m_pRTTex_Large, 0, 0, 0, 5 );

    // validation
    if(QueryPerformanceFrequency(&cpuFrequency) == FALSE)
    {
        LogError(__FILEW__, __LINE__, L"QueryPerformanceFrequency() failed", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCQueryPerformanceFrequency); 
        goto Fail;
    }

    qpcTimestamps[0].QuadPart /= cpuFrequency.QuadPart;
    qpcTimestamps[1].QuadPart /= cpuFrequency.QuadPart;

    tr = FilterHistoryBufferEvents(&timestampEvents);
    if(tr != RESULT_PASS)
    {
        goto Fail;
    }

    tr = CorrelateTimestamps(&timestampEvents, &timestamps);
    if(tr != RESULT_PASS)
    {
        goto Fail;
    }

    if(timestamps.size() < 2)
    {
        WriteToLog(_T("Number of timestamps found: %d"), timestamps.size());
        LogError(__FILEW__, __LINE__, L"Expected at least two GPU timestamps", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCRequiredCapabilityNotSupported); 
        goto Fail;
    }

    tr = FilterCalibrateEvents(&calibrateEvents);
    if(tr != RESULT_PASS || calibrateEvents.size() == 0)
    {
        LogError(__FILEW__, __LINE__, L"No calibration events found", 
                    false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
        goto Fail;
    }

    UINT64 frequency = calibrateEvents[0]->GpuFrequency();

    gpuTimestamps[0] = timestamps[timestamps.size() - 1] / frequency;
    gpuTimestamps[1] = timestamps[timestamps.size() - 2] / frequency;

    UINT64 cpuDelta = qpcTimestamps[1].QuadPart - qpcTimestamps[0].QuadPart;
    UINT64 gpuDelta = gpuTimestamps[1] - gpuTimestamps[0];

    float error = (float)(abs((long)(cpuDelta - gpuDelta))) / (float)cpuDelta;

    if(error > DELTA_FAILURE_TOLERANCE)
    {
        LogError(__FILEW__, __LINE__, L"GPU timestamp is not logged at the end of the graphics pipeline", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure); 
        WriteToLog(L"Test case failed, there is a %f percent difference between the GPU and CPU timestamp deltas "
                    L"which implies that the GPU timestamp does not get logged at the end of the graphics pipeline. "
                    L"Expected difference to be <= %f percent.", error, DELTA_FAILURE_TOLERANCE);
        goto Fail;
    }

    goto Done;

Fail:
    tr = RESULT_FAIL;
    StopETWTracing();

Done:
    return tr;
}

void CEndOfPipelineTest::CleanupTestCase()
{
    CTimingDataTest::CleanupTestCase();

    CleanupPipeline();

    SAFE_RELEASE(m_pRTTex_Large);
    SAFE_RELEASE(m_pRTView_Large);
}
