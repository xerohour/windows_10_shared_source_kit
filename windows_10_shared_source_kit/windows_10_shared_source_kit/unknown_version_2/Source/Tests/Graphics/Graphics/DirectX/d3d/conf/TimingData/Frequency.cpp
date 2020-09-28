////////////////////////////////////////////////////////////////////////////////
// Frequency.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "Frequency.h"
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
using namespace DirectX;

#define REQUIRED_FREQUENCY  12500000
#define REQUIRED_RESOLUTION 8 // 80 ns

////////////////////////////////////////////////////////////////////////////////
// CFrequencyTest
////////////////////////////////////////////////////////////////////////////////

CFrequencyTest::CFrequencyTest() 
{
}

TEST_RESULT CFrequencyTest::SetupTestCase()
{
    TEST_RESULT tr = CTimingDataTest::SetupTestCase();
    HRESULT hr = S_OK;

    if(tr != RESULT_PASS)
    {
        return tr;
    }

    tr = SetupPipeline();

    return tr;
}

void CFrequencyTest::CleanupTestCase()
{
    CTimingDataTest::CleanupTestCase();

    CleanupPipeline();
}


//
// ValidateFrequency() measures accuracy and precision, or at least some aspects of them
//
// 1. Is the CalibrateGpuClock Frequency accurate enough, i.e., has a resolution of 80ns or better?
// 2. If we Average the CalibrateGpuClock timestamps, and use the Cpu QPF as golden, does that have a resolution of 80ns or better?
// 3. Precision. If we perform several identical operations, do they take the same time (HistoryBuffers), within some tolerance?
// The third case is tricky because Optimizations will kill us.
//

TEST_RESULT CFrequencyTest::ValidateFrequency()
{
    //
    // Since this method will be called by several tests, maintain a flag that says we've 
    // already reported certain failures, so we don't clutter the logs.
    //
    static bool fFailedGpuFrequency = false;
    //
    // This flag allows us to keep track of whether we've failed this pass, while allowing testing to continue.
    //
    bool fFailure = false;          //this time

    vector<CEvent_CalibrateGpuClock*> calibrateEvents;
    TEST_RESULT tr = FilterCalibrateEvents(&calibrateEvents);
    if (tr != RESULT_PASS || calibrateEvents.size() == 0)
    {
        LogError(__FILEW__, __LINE__, L"No calibration events found",
            false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
        return RESULT_FAIL;
    }

    // Might be more accurate, testwise, to the the QPF from the ETW event.
    LARGE_INTEGER cpuFrequency;
    if (QueryPerformanceFrequency(&cpuFrequency) == FALSE)
    {
        LogError(__FILEW__, __LINE__, L"QueryPerformanceFrequency() failed", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCQueryPerformanceCounter);
        return RESULT_FAIL;
    }
    const UINT64 minimumFrequency = 12500000;                     //12.5mhz
    const UINT64 minimumFrequencyTolerance = 300000; 
    //
    // Verify Event Frequency has a resolution of at least 80ns
    //
    const UINT64 ReportedGpuFrequency = calibrateEvents.front()->GpuFrequency();
    WriteToLog(_T("Event Reported Gpu Frequency is %lld\n"), ReportedGpuFrequency);

    const UINT64 cpuDeltaOverall = calibrateEvents.back()->CpuClock() - calibrateEvents.front()->CpuClock();
    const UINT64 gpuDeltaOverall = calibrateEvents.back()->GpuClock() - calibrateEvents.front()->GpuClock();
    const double cpuSecElapsed = (double)cpuDeltaOverall / (double)cpuFrequency.QuadPart;
    const UINT64 CalculatedGpuFrequency = (UINT64)((double)gpuDeltaOverall / cpuSecElapsed);
    WriteToLog(_T("Calculated Gpu Frequency is %lld\n"), CalculatedGpuFrequency);

    //
    // Verify Reported and Calculated Frequencies (Accuracy)
    //
    if (!fFailedGpuFrequency)
    {
        if (ReportedGpuFrequency < minimumFrequency)
        {
            WriteToLog(_T("Event Reported Gpu Frequency too small\n"));

            if (ReportedGpuFrequency + minimumFrequencyTolerance < minimumFrequency)
            {
                LogError(__FILEW__, __LINE__, L"Event Reported Gpu Frequency too small (12.5mhz required)",
                    false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
                fFailedGpuFrequency = true;
                fFailure = true;
            }
        }
        //
        // Verify Calculated Frequency has a resolution of at least 80ns
        //
        if (CalculatedGpuFrequency < minimumFrequency)
        {
            WriteToLog(_T("Calculated Gpu Frequency too small\n"));
            if (CalculatedGpuFrequency + minimumFrequencyTolerance < minimumFrequency)
            {
                LogError(__FILEW__, __LINE__, L"Calculated Gpu Frequency too small  (12.5mhz required)",
                    false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
                fFailedGpuFrequency = true;
                fFailure = true;
            }
        }
    }
    if (fFailure)
    {
        return RESULT_FAIL;
    }
    return RESULT_PASS;
}

////////////////////////////////////////////////////////////////////////////////
// CFrequencyTest_Draw
////////////////////////////////////////////////////////////////////////////////
//
// This test will draw non-overlapping single pixels. We can use the results for postprocessing as well
// as contemporaneous failures.
//
TEST_RESULT CFrequencyTest_Draw::SetupTestCase()
{
    TEST_RESULT tr = CTimingDataTest::SetupTestCase();
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    struct VertexPos
    {
        XMFLOAT3 Pos;
    };
    m_width = VIDEO_WIDTH;
    m_height = VIDEO_HEIGHT;
    const int vertex_width = 16;
    const int vertex_height = 16;

    static VertexPos vertices[vertex_width * vertex_height * 3] =
    {
        XMFLOAT3(-1.0f, -1.0f, 0.0f),
        XMFLOAT3(-1.0f + 1.0f / m_width, -1.0f + 2.0f / m_height, 0.0f),
        XMFLOAT3(-1.0f + 2.0f / m_width, -1.0f, 0.0f),
    };

    // Prepare vertex buffer
    for (int i = 3; i < _countof(vertices); i++)
    {
        int k = i % 3;
        float x = (float)((i / 3) % vertex_width) * 2.0f / m_width;
        float y = (float)((i / 3) / vertex_width) * 2.0f / m_width;
        vertices[i].Pos = XMFLOAT3(vertices[k].Pos.x + 4 * x, vertices[k].Pos.y + 4 * y, vertices[k].Pos.z);
    }

    m_vertices = (void *)vertices;
    m_vertices_size = sizeof(vertices);
    m_vertices_count = _countof(vertices);
    m_stride = sizeof(VertexPos);
    m_offset = 0;

    // Define the input layout
    static D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    static string my_VS = "float4 VSMain(float4 Pos : POSITION) : SV_POSITION \n{\n return Pos; \n}\n";
    static string my_PS = "float4 PSMain(float4 Pos : SV_POSITION) : SV_Target \n{\n return float4(1.0f, 1.0f, 0.0f, 1.0f);\n}\n";

    //override Vertex and Pixel Shaders
    m_display = true;
    m_szVS = my_VS;
    m_szPS = my_PS;
    m_szPS_Alt = my_PS;
    m_layout = layout;
    m_layout_count = _countof(layout);
    m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    tr = SetupPipeline();
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    return RESULT_PASS;
}

TEST_RESULT CFrequencyTest_Draw::ExecuteTestCase()
{
    TEST_RESULT tr;
    HRESULT hr = E_FAIL;

    tr = StartETWTracing();
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    for (UINT u = 0; u < m_vertices_count; u += 3)
    {
        m_pDeviceContext->Draw(3, u);
    }
    m_pDeviceContext->Flush();
    tr = StopETWTracing();
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    if (FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
            hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute);
        return RESULT_FAIL;
    }
    tr = ValidateFrequency();
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    return RESULT_PASS;
}


TEST_RESULT CFrequencyTest_Draw::ValidateFrequency()
{
    vector<CEvent_HistoryBuffer*> historyBufferList;
    vector<UINT64> timestamps;
    vector<UINT64> ElapsedTime;

    TEST_RESULT tr = CFrequencyTest::ValidateFrequency();
    if (tr != RESULT_PASS)
    {
        return tr;
    }

    tr = FilterHistoryBufferEvents(&historyBufferList);
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    //
    // Verify the history buffer timestamps are arriving in the correct order.
    //
    return CorrelateTimestamps(&historyBufferList, &timestamps, nullptr, &ElapsedTime);
}

////////////////////////////////////////////////////////////////////////////////
// CFrequencyTest_Copy
////////////////////////////////////////////////////////////////////////////////

CFrequencyTest_Copy::CFrequencyTest_Copy() :
    m_pDestTex(nullptr)
{

}

TEST_RESULT CFrequencyTest_Copy::SetupTestCase()
{
    TEST_RESULT tr = CFrequencyTest::SetupTestCase();
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

TEST_RESULT CFrequencyTest_Copy::ExecuteTestCase()
{
    HRESULT hr = E_FAIL;
    TEST_RESULT tr = StartETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    for(UINT32 i = 0; i < TEST_CASE_ITERATIONS; ++i)
    {
        m_pDeviceContext->UpdateSubresource(m_pDestTex, 0, nullptr, (void*)&i, sizeof(UINT32), 0);
    }

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
    tr = ValidateFrequency();
    if(tr != RESULT_PASS)
    {
        // ValidateFrequency() will log cause of failure
        return tr;
    }

    return RESULT_PASS;
}

void CFrequencyTest_Copy::CleanupTestCase()
{
    CFrequencyTest::CleanupTestCase();

    SAFE_RELEASE(m_pDestTex);
}

////////////////////////////////////////////////////////////////////////////////
// CFrequencyTest_Compute
////////////////////////////////////////////////////////////////////////////////

TEST_RESULT CFrequencyTest_Compute::Setup()
{
    if(GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0)
    {
        WriteToLog(_T("Skipping test group due to lack of compute shader support on feature level 9"));
        return RESULT_SKIP;
    }

    return CFrequencyTest::Setup();
}

TEST_RESULT CFrequencyTest_Compute::ExecuteTestCase()
{
    HRESULT hr = E_FAIL;
    TEST_RESULT tr = StartETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }
    
    for(UINT32 i = 0; i < TEST_CASE_ITERATIONS; ++i)
    {
        ToggleCS();

        m_pDeviceContext->Dispatch(1, 1, 1);
    }

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported); 
        return RESULT_FAIL;
    }

    tr = StopETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    tr = ValidateFrequency();
    if(tr != RESULT_PASS)
    {
        // ValidateFrequency() will log cause of failure
        return tr;
    }

    return RESULT_PASS;
}

////////////////////////////////////////////////////////////////////////////////
// CFrequencyTest_Video
////////////////////////////////////////////////////////////////////////////////

CFrequencyTest_Video::CFrequencyTest_Video() :
    m_pVideoDevice(nullptr),
    m_pVideoContext(nullptr),
    m_pVideoProcessorEnumerator(nullptr),
    m_pVideoProcessor(nullptr),
    m_pVideoInputTex(nullptr),
    m_pVideoOutputTex(nullptr),
    m_pVideoInputView(nullptr),
    m_pVideoOutputView(nullptr)
{
}

TEST_RESULT CFrequencyTest_Video::Setup()
{
    if(GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0 || GetFramework()->Is11On12())
    {
        WriteToLog(_T("Skipping video test group on feature level 9"));
        return RESULT_SKIP;
    }

    return CFrequencyTest::Setup();
}

TEST_RESULT CFrequencyTest_Video::SetupTestCase()
{
    TEST_RESULT tr = CFrequencyTest::SetupTestCase();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    HRESULT hr = E_FAIL;
    if(FAILED(hr = m_pDevice->QueryInterface(__uuidof(ID3D11VideoDevice), (void**)&m_pVideoDevice)))
    {
        LogError( __FILEW__, __LINE__, L"QueryInterface() for ID3D11VideoDevice failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported);
        return RESULT_FAIL;
    }

    if(FAILED(hr = m_pDeviceContext->QueryInterface(__uuidof(ID3D11VideoContext), (void**)&m_pVideoContext)))
    {
        LogError( __FILEW__, __LINE__, L"QueryInterface() for ID3D11VideoContext failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported);
        return RESULT_FAIL;
    }

    D3D11_VIDEO_PROCESSOR_CONTENT_DESC enumDesc;
    ZeroMemory(&enumDesc, sizeof(D3D11_VIDEO_PROCESSOR_CONTENT_DESC));

    enumDesc.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE;
    enumDesc.InputFrameRate.Numerator = VIDEO_FPS;
    enumDesc.InputFrameRate.Denominator = 1;
    enumDesc.InputWidth = VIDEO_WIDTH;
    enumDesc.InputHeight = VIDEO_HEIGHT;
    enumDesc.OutputFrameRate.Numerator = VIDEO_FPS;
    enumDesc.OutputFrameRate.Denominator = 1;
    enumDesc.OutputWidth = VIDEO_WIDTH;
    enumDesc.OutputHeight = VIDEO_HEIGHT;

    if(FAILED(hr = m_pVideoDevice->CreateVideoProcessorEnumerator(&enumDesc, &m_pVideoProcessorEnumerator)))
    {
        LogError( __FILEW__, __LINE__, L"CreateVideoProcessorEnumerator() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported);
        return RESULT_FAIL;
    }

    D3D11_VIDEO_PROCESSOR_CAPS caps;
    ZeroMemory(&caps, sizeof(caps));
    if(FAILED(hr = m_pVideoProcessorEnumerator->GetVideoProcessorCaps(&caps)))
    {
        LogError( __FILEW__, __LINE__, L"GetVideoProcessorCaps() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported);
        return RESULT_FAIL;
    }
    
    UINT flags = 0;
    if(FAILED(hr = m_pVideoProcessorEnumerator->CheckVideoProcessorFormat(DXGI_FORMAT_B8G8R8X8_UNORM, &flags)))
    {
        LogError( __FILEW__, __LINE__, L"CheckVideoProcessorFormat() failed for output format.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported);
        return RESULT_FAIL;
    }

    if((flags & D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_OUTPUT) == 0)
    {
        LogError(__FILEW__, __LINE__, L"Video output format required by test not supported (DXGI_FORMAT_B8G8R8X8_UNORM)", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCRequiredCapabilityNotSupported); 
        return RESULT_FAIL;
    }

    const DXGI_FORMAT videoFormats [] = 
    {
        DXGI_FORMAT_YUY2,
        DXGI_FORMAT_AYUV,
        DXGI_FORMAT_NV12,
        DXGI_FORMAT_420_OPAQUE      
    };

    DXGI_FORMAT chosenFormat = DXGI_FORMAT_UNKNOWN;

    for(UINT i = 0; i < ARRAY_SIZE(videoFormats); ++i)
    {
        if(FAILED(hr = m_pVideoProcessorEnumerator->CheckVideoProcessorFormat(videoFormats[i], &flags)))
        {
            LogError( __FILEW__, __LINE__, L"CheckVideoProcessorFormat() failed for input format.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported);
            return RESULT_FAIL;
        }
    
        UINT requiredFlags = (D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_INPUT | D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_OUTPUT);
        if((flags & requiredFlags) == requiredFlags)
        {
            chosenFormat = videoFormats[i];
            break;
        }
    }

    if(chosenFormat == DXGI_FORMAT_UNKNOWN)
    {
        LogError(__FILEW__, __LINE__, L"Failed to find a supported video processor format", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCRequiredCapabilityNotSupported); 
        return RESULT_FAIL;
    }

    if(FAILED(hr = m_pVideoDevice->CreateVideoProcessor(m_pVideoProcessorEnumerator, 0, &m_pVideoProcessor)))
    {
        LogError( __FILEW__, __LINE__, L"CreateVideoProcessor() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported);
        return RESULT_FAIL;
    }

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));

    texDesc.Width = VIDEO_WIDTH;
    texDesc.Height = VIDEO_HEIGHT;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = chosenFormat;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = 0;
    texDesc.MiscFlags = 0;

    if(FAILED(hr = m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pVideoInputTex)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create texture for video input", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource); 
        return RESULT_FAIL;
    }

    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;

    if(FAILED(hr = m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pVideoOutputTex)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create texture for video output", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource); 
        return RESULT_FAIL;
    }

    m_pVideoContext->VideoProcessorSetStreamFrameFormat(m_pVideoProcessor, 0, D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE);

    // Create Output View
    D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC OutputViewDesc;
    ZeroMemory(&OutputViewDesc, sizeof(D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC));

    OutputViewDesc.ViewDimension = D3D11_VPOV_DIMENSION_TEXTURE2D;
    OutputViewDesc.Texture2D.MipSlice = 0;
    OutputViewDesc.Texture2DArray.MipSlice = 0;
    OutputViewDesc.Texture2DArray.FirstArraySlice = 0;
    OutputViewDesc.Texture2DArray.ArraySize = 1;

    if(FAILED(hr = m_pVideoDevice->CreateVideoProcessorOutputView(
        m_pVideoOutputTex,
        m_pVideoProcessorEnumerator,
        &OutputViewDesc,
        &m_pVideoOutputView)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create view for video output", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateView); 
        return RESULT_FAIL;
    }

    // Create Input View
    D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC InputViewDesc;
    ZeroMemory(&InputViewDesc, sizeof(D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC));

    InputViewDesc.FourCC = 0;
    InputViewDesc.ViewDimension = D3D11_VPIV_DIMENSION_TEXTURE2D;
    InputViewDesc.Texture2D.MipSlice = 0;
    InputViewDesc.Texture2D.ArraySlice = 0;

    if(FAILED(hr = m_pVideoDevice->CreateVideoProcessorInputView( 
        m_pVideoInputTex,
        m_pVideoProcessorEnumerator,
        &InputViewDesc,
        &m_pVideoInputView)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create view for video input", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateView); 
        return RESULT_FAIL;
    }

    return tr;
}

TEST_RESULT CFrequencyTest_Video::ExecuteTestCase()
{
    HRESULT hr = E_FAIL;
    TEST_RESULT tr = StartETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }
    
    D3D11_VIDEO_PROCESSOR_STREAM stream_data;
    ZeroMemory(&stream_data, sizeof(D3D11_VIDEO_PROCESSOR_STREAM));

    stream_data.Enable = TRUE;
    stream_data.OutputIndex = 0;
    stream_data.InputFrameOrField = 0;
    stream_data.pInputSurface = m_pVideoInputView;
    stream_data.pInputSurfaceRight = NULL;
   
    for(UINT32 i = 0; i < TEST_CASE_ITERATIONS; ++i)
    {
        if(FAILED(hr = m_pVideoContext->VideoProcessorBlt(m_pVideoProcessor, m_pVideoOutputView, 0, 1, &stream_data)))
        {
            LogError(__FILEW__, __LINE__, L"VideoProcessorBlt() failed.", 
                    hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCConformanceFailure); 
            return RESULT_FAIL;
        }   
    }

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported); 
        return RESULT_FAIL;
    }

    tr = StopETWTracing();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    tr = ValidateFrequency();
    if(tr != RESULT_PASS)
    {
        // ValidateFrequency() will log cause of failure
        return tr;
    }

    return RESULT_PASS;
}

void CFrequencyTest_Video::CleanupTestCase()
{
    SAFE_RELEASE(m_pVideoDevice);
    SAFE_RELEASE(m_pVideoContext);
    SAFE_RELEASE(m_pVideoProcessorEnumerator);
    SAFE_RELEASE(m_pVideoProcessor);
    SAFE_RELEASE(m_pVideoInputTex);
    SAFE_RELEASE(m_pVideoOutputTex);
    SAFE_RELEASE(m_pVideoInputView);
    SAFE_RELEASE(m_pVideoOutputView);
}

