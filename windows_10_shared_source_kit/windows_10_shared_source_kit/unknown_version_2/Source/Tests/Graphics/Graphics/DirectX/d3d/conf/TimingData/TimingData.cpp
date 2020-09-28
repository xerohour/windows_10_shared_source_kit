////////////////////////////////////////////////////////////////////////////////
// TimingData.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "TimingData.h"
#include "d3d11sdklayers.h"
#define UNICODE
#undef OpenTrace
#define OpenTrace               OpenTraceW
#include "EtwController.h"
#include "EtwFileConsumer.h"
#undef OpenTrace
#undef UNICODE
#include "EndOfPipeline.h"
#include "Frequency.h"
#include "MultipleEngine.h"
#include "MarkerValidation.h"
#include "PState.h"
#include "CustomEvent.h"
#include "RedBlue.h"

#pragma comment(lib, "XperfHelper.lib")
#pragma comment(lib, "DxEtwHelper.lib")

#include "provider.h"

#define MAX_WAIT_TIME 10000
#define COPY_VALUE_FIXSIZE(X, pMEM) X = *reinterpret_cast<decltype(&X)>(pMEM); pMEM += sizeof(X);
#define COPY_VALUE_VARSIZE(X, pMEM, SIZE) memcpy(&X,pMEM, SIZE); pMEM += SIZE;

CTimingDataApp g_App;

string g_szPassthroughVS = 
"   struct VSIn \n" 
"   { \n"
"       float4 pos : POSITION; \n"
"       float2 tex : TEXCOORD; \n"
"   }; \n"
" \n"
"   struct VSOut \n"
"   { \n"
"       float4 pos : SV_Position; \n"
"       float2 tex : TEXCOORD; \n"
"   }; \n"
" \n"
"   VSOut VSMain(VSIn input) \n"
"   { \n"
"       VSOut output; \n"
"       output.pos.xyz = input.pos.xyz; \n"
"       output.pos.w = 1; \n"
"       output.tex = input.tex; \n"
"       \n"
"       return output; \n"
"   } \n";

string g_szPassthroughPS = 
"   struct PSIn \n"
"   { \n"
"       float4 pos : SV_Position; \n"
"       float2 tex : TEXCOORD; \n"
"   }; \n"
"   \n"
"   float4 PSMain(PSIn input) : SV_TARGET \n"
"   {    \n"
"       float4 temp = float4(1.0, 1.0, 1.0, 1.0); \n"
"       return (temp); \n"
"   } \n";

string g_szPassthroughPS_Alt = 
"   struct PSIn \n"
"   { \n"
"       float4 pos : SV_Position; \n"
"       float2 tex : TEXCOORD; \n"
"   }; \n"
"   \n"
"   float4 PSMain(PSIn input) : SV_TARGET \n"
"   {    \n"
"       float4 temp = float4(0.5, 0.5, 0.5, 1.0); \n"
"       return (temp); \n"
"   } \n";

string g_szBasicCS = 
"   RWTexture2D<float> texUAV; \n"
"   \n"
"   struct CSIn \n"
"   {   \n"
"       uint3 threadID : SV_GroupThreadID; \n"
"       uint3 groupID : SV_GroupID; \n"
"       uint threadIndex : SV_GroupIndex; \n"
"   };  \n"
"   \n"
"   [numthreads(1,1,1)] \n"
"   void CSMain(CSIn input) \n"
"   {   \n"
"       texUAV[input.threadID.xy] = (float)input.threadIndex; \n"
"   }   \n";

string g_szBasicCS_Alt = 
"   RWTexture2D<float> texUAV; \n"
"   \n"
"   struct CSIn \n"
"   {   \n"
"       uint3 threadID : SV_GroupThreadID; \n"
"       uint3 groupID : SV_GroupID; \n"
"       uint threadIndex : SV_GroupIndex; \n"
"   };  \n"
"   \n"
"   [numthreads(1,1,1)] \n"
"   void CSMain(CSIn input) \n"
"   {   \n"
"       texUAV[input.threadID.xy] = (float)input.threadIndex * 2.0f; \n"
"   }   \n";

////////////////////////////////////////////////////////////////////////////////
//
// CTimingDataApp
//
////////////////////////////////////////////////////////////////////////////////

CTimingDataApp::CTimingDataApp()
{
    m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;
    m_D3DOptions.DriverType = D3D_DRIVER_TYPE_HARDWARE;
}

bool CTimingDataApp::InitTestGroups()
{
    ADD_TEST_AND_DESC(_T("EndOfPipeline"), CEndOfPipelineTest, _T("Verifies that the timestamp appears at the end of the pipeline"));
    ADD_TEST_AND_DESC(_T("Frequency/Draw"), CFrequencyTest_Draw, _T("Verifies the required frequency of the GPU timestamp on the rasterization engine"));
    ADD_TEST_AND_DESC(_T("Frequency/Copy"), CFrequencyTest_Copy, _T("Verifies the required frequency of the GPU timestamp on the memory op engine"));
    ADD_TEST_AND_DESC(_T("Frequency/Compute"), CFrequencyTest_Compute, _T("Verifies the required frequency of the GPU timestamp on the compute engine"));
    ADD_TEST_AND_DESC(_T("Frequency/Video"), CFrequencyTest_Video, _T("Verifies the required frequency of the GPU timestamp on the video engine"));
    ADD_TEST_AND_DESC(_T("MarkerValidation/CommandBufferLimit"), CMarkerValidationTest_CommandBufferLimit, _T("Verifies that the driver doesn't pass command buffer limit in the event"));
    ADD_TEST_AND_DESC(_T("MarkerValidation/Range"), CMarkerValidationTest_Range, _T("Ensures that GPU timestamps fall between the range defined by the begin and end timestamps of a DMA Buffer"));
    ADD_TEST_AND_DESC(_T("MarkerValidation/CommandList"), CMarkerValidationTest_CommandList, _T("Verifies DDI rules around the command list event"));
    ADD_TEST_AND_DESC(_T("MarkerValidation/APISequence"), CMarkerValidationTest_APISequence, _T("Ensures general DDI rules regarding APISequenceNumbers are honored"));
    ADD_TEST_AND_DESC(_T("MarkerValidation/Optimized"), CMarkerValidationTest_Optimized, _T("Verifies DDI rules regarding optimized and dropped API calls"));
    ADD_TEST_AND_DESC(_T("MarkerValidation/Flush"), CMarkerValidationTest_Flush, _T("Ensures that DDI rules around flushing the pipeline are honored"));
    ADD_TEST_AND_DESC(_T("PState"), CPStateTest, _T("Ensures that the timestamp is invariant during P-State transitions"));
    ADD_TEST_AND_DESC(_T("CustomEvent"), CCustomEventTest, _T("Verifies that custom driver events are not logged when the marker mode is NONE"));
    ADD_TEST_AND_DESC(_T("RedBlue/Main"), CRedBlueTest, _T("Reconstructs pixel drawing based on the order of Gpu timestamps."));
    ADD_TEST_AND_DESC(_T("MultipleEngine/Draw"), CMultipleEngineTest_Draw, _T("Test cases for ensuring the GPU timestamp appears on the rasterization engine"));
    ADD_TEST_AND_DESC(_T("MultipleEngine/Copy"), CMultipleEngineTest_Copy, _T("Test cases for ensuring the GPU timestamp appears on the memory op engine"));
    ADD_TEST_AND_DESC(_T("MultipleEngine/Compute"), CMultipleEngineTest_Compute, _T("Test cases for ensuring the GPU timestamp appears on the compute engine"));
    ADD_TEST_AND_DESC(_T("Frequency/Teapot"), CFrequencyTest_Teapot, _T("Verifies precision"));
    ADD_TEST_AND_DESC(_T("Frequency/TeapotDC"), CFrequencyTest_TeapotDeferred, _T("Verifies deferred contexts"));
    ADD_TEST_AND_DESC(_T("Frequency/TeapotDCMarkers"), CFrequencyTest_TeapotDeferredMarkers, _T("Verifies deferred contexts"));
    return true;
}

void CTimingDataApp::InitOptionVariables()
{
    RegisterOptionVariable(_T("RedBlueCS"), _T("Wrap Api/Flush in Critical Section in Red/Blue, for testing"), &m_bRedBlueCS, false, 0, 0);
    RegisterOptionVariable(_T("MobileCompat"), _T("Mobile Compat Testing"), &m_MobileCompat, false, 0, 0);
    CD3D11TestFramework::InitOptionVariables();
}

////////////////////////////////////////////////////////////////////////////////
//
// CTimingDataTest
//
////////////////////////////////////////////////////////////////////////////////

CTimingDataTest::CTimingDataTest(bool fEnableETW, PWCHAR ETL_file) :
    m_pVB(NULL),
    m_pIB(NULL),
    m_pRS(NULL),
    m_pVS(NULL),
    m_pPS(NULL),
    m_pPS_0(NULL),
    m_pPS_1(NULL),
    m_pCS(NULL),
    m_pCS_0(NULL),
    m_pCS_1(NULL),
    m_pInputLayout(NULL),
    m_pRTTex(NULL),
    m_pRTView(NULL),
    m_pSRTex(NULL),
    m_pSRView(NULL),
    m_pUATex(NULL),
    m_pUAView(NULL),
    m_pDevice(NULL),
    m_pDeviceContext(NULL),
    m_pSampler(NULL),
    m_pStagingTex(NULL),
    m_width(1),
    m_height(1),
    m_pTimingDataHelper(NULL),
    m_pXperfHelper(NULL),
    m_bTracing(false)
{
    m_ETL_file = ETL_file;
    DWORD pid = GetCurrentProcessId();
    memset(m_wszProcessOption, 0, sizeof(m_wszProcessOption));
    m_Pid = GetCurrentProcessId();
    StringCchPrintfW(m_wszProcessOption, STR_SIZE, L"-process %d", pid);
    if (GetFramework()->Is11On12())
    {
        EnableETWTracing(ETL_file);
    }
}

TEST_RESULT CTimingDataTest::Setup()
{
    TEST_RESULT tr;
    if(GetFramework()->Is11On12())
    {
        tr = EnableETWTracing(ETLFileName());
        if(tr != RESULT_PASS)
        {
            return tr;
        }
    }
    HRESULT hr = S_OK;

    tr = CD3D11Test::Setup();
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    m_pDevice = GetDevice2();
    m_pDeviceContext = GetEffectiveContext2();

    // Ensure device supports markers
    D3D11_FEATURE_DATA_MARKER_SUPPORT MarkerData;
    if(FAILED(hr = m_pDevice->CheckFeatureSupport( D3D11_FEATURE_MARKER_SUPPORT, &MarkerData, sizeof( MarkerData ) ) ))
    {
        LogError(__FILEW__, __LINE__, L"Call to CheckFeatureSupport() failed", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCheckFeatureSupport); 
        return RESULT_FAIL;
    }

    if(MarkerData.Profile == FALSE && !GetFramework()->Is11On12())
    {
        // D3D10WARP.DLL does not have marker support, but is scheduled to be replaced by D3D12WARP.DLL.
        bool IsLegacyWarp10Dll;
        if (FAILED(GetFramework()->IsLegacyD3D10WarpDll(GetDXGIAdapter(), m_pDevice, IsLegacyWarp10Dll)))
        {
            LogError(__FILEW__, __LINE__, L"Call to CheckFeatureSupport() failed", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCheckFeatureSupport);
            return RESULT_FAIL;
        }

        if(GetFramework()->GetTestMode() == WGFTEST_MODE_WHQL && !IsLegacyWarp10Dll)
        {
            LogError(__FILEW__, __LINE__, L"Failing due to lack of marker data support on WDDM 1.3 hardware", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCRequiredCapabilityNotSupported); 
            return RESULT_FAIL;
        }
        else
        {
            WriteToLog(_T("Skipping due to lack of marker data support"));
            return RESULT_SKIP;
        }
    }

    return tr;
}

TEST_RESULT CTimingDataTest::SetupTestCase()
{
    TEST_RESULT tr = CD3D11Test::SetupTestCase();
    HRESULT hr = S_OK;

    if(tr != RESULT_PASS)
    {
        return tr;
    }

    m_szVS = g_szPassthroughVS;
    m_szPS = g_szPassthroughPS;
    m_szPS_Alt = g_szPassthroughPS_Alt;
    m_szCS = g_szBasicCS;
    m_szCS_Alt = g_szBasicCS_Alt;

    static const D3D11_INPUT_ELEMENT_DESC inputElements[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    m_layout = inputElements;
    m_layout_count = _countof(inputElements);

    const int vertex_float = 6;
    m_stride = sizeof(float)* vertex_float;
    m_offset = 0;
    static const float quad[vertex_float * 4] =
    {
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f
    };

    m_vertices = (void *)quad;
    m_vertices_size = sizeof(quad);
    m_vertices_count = sizeof(quad) / m_stride;
    m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    m_display = false;   // do not show in a window
    if(!GetFramework()->IsCompat() && !GetFramework()->Is11On12())
    {
        m_pTimingDataHelper = new CTimingDataHelper(ETLFileName());
    }
    return RESULT_PASS;
}

void CTimingDataTest::CleanupTestCase()
{
    if(!GetFramework()->IsCompat() && !GetFramework()->Is11On12())
    {
        SAFE_DELETE(m_pTimingDataHelper);
    }
}

void CTimingDataTest::ToggleCS()
{
    if(m_pCS == m_pCS_0)
    {
        m_pCS = m_pCS_1;
    }
    else
    {
        m_pCS = m_pCS_0;
    }

    m_pDeviceContext->CSSetShader(m_pCS, NULL, 0);
}

void CTimingDataTest::TogglePS()
{
    if(m_pPS == m_pPS_0)
    {
        m_pPS = m_pPS_1;
    }
    else
    {
        m_pPS = m_pPS_0;
    }

    m_pDeviceContext->PSSetShader(m_pPS, NULL, 0);
}

void CTimingDataTest::SaveMapToBmp(D3D11_MAPPED_SUBRESOURCE *pMapData, UINT width, UINT height, LPCTSTR bmpfile, bool invert)
{
    //
    // Convert RGBA to BGR, invert y
    //
    int padding = width % 4;
    long s = height * (width * 3 + padding);
    BYTE* dest = new BYTE[s];
    BYTE *pDest = dest;
    for (int y = height - 1; y >= 0; y--)
    {
        BYTE *pSrc = y * pMapData->RowPitch + static_cast<PBYTE>(pMapData->pData);  //y inversion
        for (UINT x = 0; x < width; x++)
        {
            if (invert)
            {
                *pDest++ = pSrc[2];
                *pDest++ = pSrc[1];
                *pDest++ = pSrc[0];
            }
            else
            {
                *pDest++ = pSrc[0];
                *pDest++ = pSrc[1];
                *pDest++ = pSrc[2];
            }
            pSrc += 4;
        }
        for (int x = 0; x < padding; x++)
        {
            *pDest++ = '\0';
        }
    }
    SaveBMP(dest, width, height, s, bmpfile);
    delete[] dest;
}

void CTimingDataTest::SaveViewToBmp(ID3D11DeviceContext2 *m_pDeviceContext, ID3D11Texture2D *pStagingTex, ID3D11Texture2D *pRTTex, LPCTSTR bmpfile)
{
    D3D11_MAPPED_SUBRESOURCE mappedTexture = { 0 };
    m_pDeviceContext->CopyResource(pStagingTex, pRTTex);
    HRESULT hr = m_pDeviceContext->Map(pStagingTex, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_READ_WRITE, 0, &mappedTexture);
    if (FAILED(hr))
    {
        return;                                     //not supported. Likely on a deferred context
    }
    SaveMapToBmp(&mappedTexture, m_width, m_height, bmpfile);
    m_pDeviceContext->Unmap(pStagingTex, D3D11CalcSubresource(0, 0, 1));
}

BOOL CTimingDataTest::SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile)
{
    HANDLE file = CreateFile(bmpfile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    BOOL ok = (file != NULL);
    if (ok)
    {
        unsigned long bwritten;
        {
            BITMAPFILEHEADER bmfh = {};
            bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
            bmfh.bfReserved1 = 0;
            bmfh.bfReserved2 = 0;
            bmfh.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+paddedsize;
            bmfh.bfOffBits = 0x36;      // number of bytes to start of bitmap bits
            ok = WriteFile(file, &bmfh, sizeof (BITMAPFILEHEADER), &bwritten, NULL);   // write file header
        }
        if (ok)
        {
            BITMAPINFOHEADER info = {};
            info.biSize = sizeof(BITMAPINFOHEADER);
            info.biWidth = width;
            info.biHeight = height;
            info.biPlanes = 1;          // we only have one bitplane
            info.biBitCount = 24;       // RGB mode is 24 bits
            info.biCompression = BI_RGB;
            info.biSizeImage = 0;       // can be 0 for 24 bit images
            info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
            info.biYPelsPerMeter = 0x0ec4;
            info.biClrUsed = 0;         // we are in RGB mode and have no palette
            info.biClrImportant = 0;    // all colors are important
            ok = WriteFile(file, &info, sizeof (BITMAPINFOHEADER), &bwritten, NULL);    // write infoheader
        }
        if (ok)
        {
            ok = WriteFile(file, Buffer, paddedsize, &bwritten, NULL);        // write image data
        }
        CloseHandle(file);
    }
    return ok;
}

TEST_RESULT CTimingDataTest::SetupPipeline()
{
    HRESULT hr = E_FAIL;
    TEST_RESULT tr = RESULT_PASS;

    ID3D10Blob *pShaderBlob = NULL;
    ID3D10Blob *pErrorBlob = NULL;

    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width    = (float)m_width;
    vp.Height   = (float)m_height;
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    m_pDeviceContext->RSSetViewports(1, &vp);

    D3D11_TEXTURE2D_DESC tex2DDesc_base;
    tex2DDesc_base.Width = m_width;
    tex2DDesc_base.Height = m_height;
    tex2DDesc_base.MipLevels = 1;
    tex2DDesc_base.ArraySize = 1;
    tex2DDesc_base.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex2DDesc_base.SampleDesc.Count = 1;
    tex2DDesc_base.SampleDesc.Quality = 0;
    tex2DDesc_base.Usage = D3D11_USAGE_DEFAULT;
    tex2DDesc_base.BindFlags = D3D11_BIND_RENDER_TARGET;
    tex2DDesc_base.CPUAccessFlags = 0;
    tex2DDesc_base.MiscFlags = 0;
    D3D11_TEXTURE2D_DESC tex2DDesc;

    // create and set rasterizer state
    D3D11_RASTERIZER_DESC rastDesc;
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthBias = false;
    rastDesc.DepthBiasClamp = 0;
    rastDesc.SlopeScaledDepthBias = 0;
    rastDesc.DepthClipEnable = true;
    rastDesc.ScissorEnable = false;
    rastDesc.MultisampleEnable = false;
    rastDesc.AntialiasedLineEnable = false;
    rastDesc.FillMode = D3D11_FILL_SOLID;

    if(FAILED(hr = m_pDevice->CreateRasterizerState(&rastDesc, &m_pRS)))
    {
        LogError( __FILEW__, __LINE__, L"CreateRasterizerState() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateRasterizerState );
        goto Fail;
    }

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = m_vertices_size;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = m_vertices;
    data.SysMemPitch = m_vertices_size;
    data.SysMemSlicePitch = 0;

    if(FAILED(hr = m_pDevice->CreateBuffer(&bufferDesc, &data, &m_pVB)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create vertex buffer", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource);
        goto Fail;
    }

    bufferDesc.ByteWidth = sizeof(int);
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    const UINT16 indices [] = { 0, 1, 2, 3 };

    data.pSysMem = (void*)indices;
    data.SysMemPitch = sizeof(UINT16) * 4;

    if(FAILED(hr = m_pDevice->CreateBuffer(&bufferDesc, &data, &m_pIB)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create index buffer", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource);
        goto Fail;
    }

    // compile VS
    if(FAILED(hr = GetFramework()->CompileShaderFromMemory(
        m_szVS.c_str(),
        m_szVS.length(),
        "VSMain",
        D3D_SHADER_STAGE_VERTEX,
        D3D_FEATURE_LEVEL_9_1,
        D3D_FEATURE_LEVEL_11_1,
        0,
        &pShaderBlob,
        &pErrorBlob)))
    {
        LogError(__FILEW__, __LINE__, L"CompileShaderFromMemory() failed for vertex shader", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3DX::gFCCompileShader);
        WriteToLog(_T("Compiler errors:\n"));

        if (pErrorBlob != NULL)
        {
            LPVOID pBuf = pErrorBlob->GetBufferPointer();
            size_t bufSize = pErrorBlob->GetBufferSize();
            char* szError = new char[bufSize+1];
            memcpy(szError, pBuf, bufSize);
            szError[bufSize] = 0;
            WriteToLog( _T( "%s\n" ), szError );
            delete[] szError;
        }

        goto Fail;
    }

    if(FAILED(hr = m_pDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pVS)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create vertex shader", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader);
        goto Fail;
    }

    // input layout
    if(FAILED(hr = m_pDevice->CreateInputLayout(
        m_layout,
        m_layout_count,
        pShaderBlob->GetBufferPointer(),
        pShaderBlob->GetBufferSize(),
        &m_pInputLayout
        )))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create input layout", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateInputLayout);
        goto Fail;
    }

    SAFE_RELEASE(pShaderBlob);
    SAFE_RELEASE(pErrorBlob);

    // compile PS
    if(FAILED(hr = GetFramework()->CompileShaderFromMemory(
        m_szPS.c_str(),
        m_szPS.length(),
        "PSMain",
        D3D_SHADER_STAGE_PIXEL,
        D3D_FEATURE_LEVEL_9_1,
        D3D_FEATURE_LEVEL_11_1,
        0,
        &pShaderBlob,
        &pErrorBlob)))
    {
        LogError(__FILEW__, __LINE__, L"CompileShaderFromMemory() failed for pixel shader", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3DX::gFCCompileShader);

        if (pErrorBlob != NULL)
        {
            LPVOID pBuf = pErrorBlob->GetBufferPointer();
            size_t bufSize = pErrorBlob->GetBufferSize();
            char* szError = new char[bufSize+1];
            memcpy(szError, pBuf, bufSize);
            szError[bufSize] = 0;
            WriteToLog( _T( "%s\n" ), szError );
            delete[] szError;
        }

        goto Fail;
    }

    if(FAILED(hr = m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pPS_0)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create pixel shader", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader);
        goto Fail;
    }

    SAFE_RELEASE(pShaderBlob);
    SAFE_RELEASE(pErrorBlob);

    // compile alt PS
    if(FAILED(hr = GetFramework()->CompileShaderFromMemory(
        m_szPS_Alt.c_str(),
        m_szPS_Alt.length(),
        "PSMain",
        D3D_SHADER_STAGE_PIXEL,
        D3D_FEATURE_LEVEL_9_1,
        D3D_FEATURE_LEVEL_11_1,
        0,
        &pShaderBlob,
        &pErrorBlob)))
    {
        LogError(__FILEW__, __LINE__, L"CompileShaderFromMemory() failed for alternate pixel shader", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3DX::gFCCompileShader);

        if (pErrorBlob != NULL)
        {
            LPVOID pBuf = pErrorBlob->GetBufferPointer();
            size_t bufSize = pErrorBlob->GetBufferSize();
            char* szError = new char[bufSize+1];
            memcpy(szError, pBuf, bufSize);
            szError[bufSize] = 0;
            WriteToLog( _T( "%s\n" ), szError );
            delete[] szError;
        }

        goto Fail;
    }

    if(FAILED(hr = m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pPS_1)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create alternate pixel shader", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader);
        goto Fail;
    }

    if(GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_0)
    {
        // compile CS
        if(FAILED(hr = GetFramework()->CompileShaderFromMemory(
            m_szCS.c_str(),
            m_szCS.length(),
            "CSMain",
            D3D_SHADER_STAGE_COMPUTE,
            D3D_FEATURE_LEVEL_9_1,
            D3D_FEATURE_LEVEL_11_1,
            0,
            &pShaderBlob,
            &pErrorBlob)))
        {
            LogError(__FILEW__, __LINE__, L"CompileShaderFromMemory() failed for compute shader", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3DX::gFCCompileShader);
            WriteToLog(_T("Compiler errors:\n"));

            if (pErrorBlob != NULL)
            {
                LPVOID pBuf = pErrorBlob->GetBufferPointer();
                size_t bufSize = pErrorBlob->GetBufferSize();
                char* szError = new char[bufSize+1];
                memcpy(szError, pBuf, bufSize);
                szError[bufSize] = 0;
                WriteToLog( _T( "%s\n" ), szError );
                delete[] szError;
            }

            goto Fail;
        }

        if(FAILED(hr = m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pCS_0)))
        {
            LogError(__FILEW__, __LINE__, L"Failed to create compute shader", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader);
            goto Fail;
        }

        // compile alt CS
        if(FAILED(hr = GetFramework()->CompileShaderFromMemory(
            m_szCS_Alt.c_str(),
            m_szCS_Alt.length(),
            "CSMain",
            D3D_SHADER_STAGE_COMPUTE,
            D3D_FEATURE_LEVEL_9_1,
            D3D_FEATURE_LEVEL_11_1,
            0,
            &pShaderBlob,
            &pErrorBlob)))
        {
            LogError(__FILEW__, __LINE__, L"CompileShaderFromMemory() failed for alternate compute shader", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3DX::gFCCompileShader);
            WriteToLog(_T("Compiler errors:\n"));

            if (pErrorBlob != NULL)
            {
                LPVOID pBuf = pErrorBlob->GetBufferPointer();
                size_t bufSize = pErrorBlob->GetBufferSize();
                char* szError = new char[bufSize+1];
                memcpy(szError, pBuf, bufSize);
                szError[bufSize] = 0;
                WriteToLog( _T( "%s\n" ), szError );
                delete[] szError;
            }

            goto Fail;
        }

        if(FAILED(hr = m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pCS_1)))
        {
            LogError(__FILEW__, __LINE__, L"Failed to create alternate compute shader", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader);
            goto Fail;
        }
        m_pDeviceContext->CSSetShader(m_pCS, NULL, 0);
        //
        // create resources for unordered access view
        //
        tex2DDesc = tex2DDesc_base;
        tex2DDesc.Format = DXGI_FORMAT_R32_FLOAT;
        tex2DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;

        if (FAILED(hr = m_pDevice->CreateTexture2D(&tex2DDesc, nullptr, &m_pUATex)))
        {
            LogError(__FILEW__, __LINE__, L"Failed to create texture for unordered access view", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource);
            return RESULT_FAIL;
        }

        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
        uavDesc.Texture2D.MipSlice = 0;

        if (FAILED(hr = m_pDevice->CreateUnorderedAccessView(m_pUATex, &uavDesc, &m_pUAView)))
        {
            LogError(__FILEW__, __LINE__, L"Failed to create unordered access view", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateView);
            return RESULT_FAIL;
        }

        m_pDeviceContext->CSSetUnorderedAccessViews(0, 1, &m_pUAView, NULL);
    }

    //
    // create resources for source texture, same as view.
    //
    tex2DDesc = tex2DDesc_base;
    tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    UINT32 *sourceData = new UINT32[m_width * m_height];
    for (UINT i = 0; i < m_width * m_height; ++i)
    {
        sourceData[i] = (rand() % 0xffffffff) | 0x000000ff;
    }

    data.pSysMem = (void*)sourceData;
    data.SysMemPitch = sizeof(UINT32)* m_width;
    data.SysMemSlicePitch = 0;

    if(FAILED(hr = m_pDevice->CreateTexture2D(&tex2DDesc, &data, &m_pSRTex)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create texture for shader resource", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource);
        delete [] sourceData;
        return RESULT_FAIL;
    }

    delete [] sourceData;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    if(FAILED(hr = m_pDevice->CreateShaderResourceView(m_pSRTex, &srvDesc, &m_pSRView)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create shader resource view", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateView);
        return RESULT_FAIL;
    }
    //
    // RTV
    //
    tex2DDesc = tex2DDesc_base;
    if (FAILED(hr = m_pDevice->CreateTexture2D(&tex2DDesc, NULL, &m_pRTTex)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create texture for render target", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource);
        goto Fail;
    }

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    if (FAILED(hr = m_pDevice->CreateRenderTargetView(m_pRTTex, &rtvDesc, &m_pRTView)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create render target view", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateView);
        goto Fail;
    }

    //
    // Create Staging Buffer
    //
    tex2DDesc = tex2DDesc_base;
    tex2DDesc.ArraySize = 1;
    tex2DDesc.MipLevels = 1;
    tex2DDesc.SampleDesc.Count = 1;
    tex2DDesc.SampleDesc.Quality = 0;
    tex2DDesc.Usage = D3D11_USAGE_STAGING;
    tex2DDesc.BindFlags = 0;
    tex2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    tex2DDesc.MiscFlags = 0;

    if (FAILED(hr = m_pDevice->CreateTexture2D(&tex2DDesc, nullptr, &m_pStagingTex)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create staging texture for render target view", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource);
        goto Fail;
    }

    D3D11_SAMPLER_DESC sampDesc;
    sampDesc = CD3D11_SAMPLER_DESC();
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    if (FAILED(hr = m_pDevice->CreateSamplerState(&sampDesc, &m_pSampler)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create sampler", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateState);
        goto Fail;
    }

    m_pPS = m_pPS_0;
    m_pCS = m_pCS_0;

    m_pDeviceContext->IASetPrimitiveTopology(m_topology);
    m_pDeviceContext->IASetInputLayout(m_pInputLayout);
    m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &m_stride, &m_offset);
    m_pDeviceContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R16_UINT, 0);
    m_pDeviceContext->RSSetState(m_pRS);
    m_pDeviceContext->VSSetShader(m_pVS, NULL, 0);
    m_pDeviceContext->PSSetShader(m_pPS, NULL, 0);
    m_pDeviceContext->PSSetSamplers(0, 1, &m_pSampler);
    m_pDeviceContext->PSSetShaderResources(0, 1, &m_pSRView);
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRTView, NULL);

    if(FAILED(hr = ExecuteEffectiveContext()))
    {
        LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
            hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported);
        return RESULT_FAIL;
    }

    goto Done;

Fail:
    tr = RESULT_FAIL;

Done:
    SAFE_RELEASE(pShaderBlob);
    SAFE_RELEASE(pErrorBlob);

    return tr;
}

void CTimingDataTest::CleanupPipeline()
{
    SAFE_RELEASE(m_pVB);
    SAFE_RELEASE(m_pIB);
    SAFE_RELEASE(m_pRS);
    SAFE_RELEASE(m_pVS);
    SAFE_RELEASE(m_pPS_0);
    SAFE_RELEASE(m_pPS_1);
    SAFE_RELEASE(m_pCS_0);
    SAFE_RELEASE(m_pCS_1);
    SAFE_RELEASE(m_pInputLayout);
    SAFE_RELEASE(m_pRTView);
    SAFE_RELEASE(m_pRTTex);
    SAFE_RELEASE(m_pSRView);
    SAFE_RELEASE(m_pSRTex);
    SAFE_RELEASE(m_pUAView);
    SAFE_RELEASE(m_pUATex);
    SAFE_RELEASE(m_pSampler);
}

void CTimingDataTest::Cleanup()
{
    CD3D11Test::Cleanup();

    SAFE_RELEASE(m_pStagingTex);

    if(!GetFramework()->IsCompat() && !GetFramework()->Is11On12())
    {
        SAFE_DELETE(m_pXperfHelper);
    }
}

TEST_RESULT CTimingDataTest::EnableETWTracing(PWCHAR EtlFile)
{
    HRESULT hr;

    if (m_bTracing == true)
    {
        return RESULT_PASS;
    }

    if(GetFramework()->Is11On12())
    {
        m_pXperfHelper = new XperfHelper(NORMAL, true, LARGE, FALSE);
        m_pXperfHelper->SetMergedFile(ETLFileName());

        DxkernelSession* pDxkernelSession;
        OtherSession* pOtherSession;

        int bufferSize = 0;
        int minBuffers = 0;
        int maxBuffers = 0;

        m_pXperfHelper->GetDxkernelSessionBuffers(&bufferSize, &minBuffers, &maxBuffers);
        pDxkernelSession = new DxkernelSession(NORMAL, true, bufferSize, minBuffers, maxBuffers, true);
        pDxkernelSession->SetSessionProviders(L"802ec45a-1e99-4b83-9920-87c98277ba9d:0xd843:5");
        m_pXperfHelper->SetDxkernelSession(pDxkernelSession);

        pOtherSession = new OtherSession(NORMAL, true, 1024, 1024, 2048);
        pOtherSession->SetSessionProviders(L"5d8087dd-3a9b-4f56-90df-49196cdc4f11:0xffffffffffffffff");
        m_pXperfHelper->SetOtherSession(pOtherSession);

        CProvider etwProvider;
        hr = m_pXperfHelper->Start();
    }
    else
    {
        m_pXperfHelper = new XperfHelper(NORMAL, true, LARGE, GetFramework()->IsMobile());  // Force max perf setting for Mobile
        m_pXperfHelper->SetMergedFile(EtlFile);

        DxkernelSession* pDxkernelSession;
        OtherSession* pOtherSession;

        int bufferSize = 0;
        int minBuffers = 0;
        int maxBuffers = 0;

        m_pXperfHelper->GetDxkernelSessionBuffers(&bufferSize, &minBuffers, &maxBuffers);
        pDxkernelSession = new DxkernelSession(NORMAL, true, bufferSize, minBuffers, maxBuffers, true);
        pDxkernelSession->SetSessionProviders(L"802ec45a-1e99-4b83-9920-87c98277ba9d:0xd843:5");
        m_pXperfHelper->SetDxkernelSession(pDxkernelSession);

        pOtherSession = new OtherSession(NORMAL, true, 1024, 1024, 2048);
        pOtherSession->SetSessionProviders(L"db6f6ddb-ac77-4e88-8253-819df9bbf140:0x8000000000000312");
        m_pXperfHelper->SetOtherSession(pOtherSession);

        m_pXperfHelper->SetAdditionalSession(NULL);
        hr = m_pXperfHelper->Start();

    }

    if (hr != S_OK)
    {
        LogError(__FILEW__, __LINE__, L"Enable ETW Trace failed with HRESULT:", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCLogicError); 
        return RESULT_FAIL;
    }
    m_bTracing = true;
    return RESULT_PASS;
}

TEST_RESULT CTimingDataTest::StartETWTracing(bool fDraw, PWCHAR EtlFile)
{

    TEST_RESULT tr;
    if(EtlFile == NULL || GetFramework()->Is11On12())
    {
        tr = EnableETWTracing(ETLFileName());
    }
    else
    {
        tr = EnableETWTracing(EtlFile);
    }
    
    if (tr != RESULT_PASS)
    {
        LogError(__FILEW__, __LINE__, L"ETW Tracing failed to begin.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError); 
        return RESULT_FAIL;
    }
    m_pDeviceContext->Flush();
    if (fDraw)
    {
        m_pDeviceContext->Draw(4, 0);
    }
    Sleep(2000);

    m_pDeviceContext->SetMarkerInt(MARKER_NAME_START, MARKER_UID);

    return RESULT_PASS;
}

TEST_RESULT CTimingDataTest::StopETWTracing()
{
    HRESULT hr;

    if(m_bTracing == true)
    {
        // flush and sleep to be sure we don't miss events
        m_pDeviceContext->Flush();
        Sleep(2000);
        m_pXperfHelper->Stop();
        m_bTracing = false;
    }

    return RESULT_PASS;
}

TEST_RESULT CTimingDataTest::FormSequenceNumbers(vector<UINT64> *pResults,
            const UINT32 &high,
            const vector<UINT32> &vLow0,
            const vector<UINT32> &vLow1)
{
    if(pResults == nullptr)
    {
        LogError(__FILEW__, __LINE__, L"Results vector pointer passed to FormSequenceNumbers is NULL", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError); 
        return RESULT_FAIL;
    }

    for(UINT i = 0; i < vLow0.size(); ++i)
    {
        UINT64 seqNum = (UINT64)vLow0[i] | ((UINT64)high << 32);
        pResults->push_back(seqNum);
    }

    for(UINT i = 0; i < vLow1.size(); ++i)
    {
        UINT64 seqNum = (UINT64)vLow1[i] | ((UINT64)high << 32);
        pResults->push_back(seqNum);
    }

    return RESULT_PASS;
}

TEST_RESULT CTimingDataTest::GetMarkerBegin(UINT64 *pMarkerBegin)
{
    if(pMarkerBegin == nullptr)
    {
        LogError(__FILEW__, __LINE__, L"Pointer to marker begin parameters is NULL", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError); 
        return RESULT_FAIL;
    }

    *pMarkerBegin = 0;
    // Verify that the first Marker Event is our special event.
    vector<CEvent_MarkerInt*> *pMarkerEvents;
    if(GetFramework()->Is11On12())
    {
        m_Results.Task = Direct3D12EtwProviderGuid_TASK_Marker;
        GatherSpecifiedEvents();
        pMarkerEvents = &m_Results.MarkerIntVector;    
    }
    else if(GetFramework()->IsCompat())
    {
        m_Results.Task = Direct3D11EtwProviderGuid_TASK_Marker;
        GatherSpecifiedEvents();
        pMarkerEvents = &m_Results.MarkerIntVector;    
    }
    else
    {
        pMarkerEvents = m_pTimingDataHelper->GatherMarkerIntList(RESULTS_FILE, m_wszProcessOption);
    }
    if(pMarkerEvents == nullptr || pMarkerEvents->size() == 0)
    {
        WriteToLog(_T("WARNING: Did not find marker begin event"));
        return RESULT_PASS;
    }
    CEvent_MarkerInt *BeginEvent = nullptr;
    for(CEvent_MarkerInt * markerEvent : *pMarkerEvents)
    {
        if(markerEvent->Label() == MARKER_NAME_START  && markerEvent->Data() == MARKER_UID)
        {
            BeginEvent = markerEvent;
        }
    }

    if(BeginEvent == nullptr)
    {
        WriteToLog(_T("WARNING: Marker begin event doesn't match expected label and id"));
        return RESULT_PASS;
    }

    *pMarkerBegin = BeginEvent->EventTime();   

    return RESULT_PASS;
}

TEST_RESULT CTimingDataTest::FilterHistoryBufferEvents(vector<CEvent_HistoryBuffer*> *pHistoryBufferEvents, bool fCheckOrder)
{
    UINT64 markerBegin = 0;

    if(pHistoryBufferEvents == nullptr)
    {
        LogError(__FILEW__, __LINE__, L"Pointer to event vector parameter is NULL", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError); 
        return RESULT_FAIL;
    }

    TEST_RESULT tr = GetMarkerBegin(&markerBegin);
    if(tr != RESULT_PASS)
    {
        return tr;
    }
    
    pHistoryBufferEvents->clear();
    vector<CEvent_HistoryBuffer*> *pAllEvents;
    if(GetFramework()->IsCompat() || GetFramework()->Is11On12())
    {
        m_Results.Task = DxgkControlGuid_TASK_HistoryBuffer;
        GatherSpecifiedEvents();
        pAllEvents  = &m_Results.HistoryVector;
    }
    else
    {
        pAllEvents = m_pTimingDataHelper->GatherHistoryBufferList(RESULTS_FILE, L"");
    }
    if(pAllEvents == nullptr || pAllEvents->size() == 0)
    {
        return tr;
    }

    for(UINT i = 0; i < pAllEvents->size(); ++i)
    {
        if (fCheckOrder && (*pAllEvents)[i]->HistoryBuffer().size() >= 2)
        {
            const UINT64 MaxWRoll = 5000000000ULL;  //5 seconds
            vector<UINT64> buffer = (*pAllEvents)[i]->HistoryBuffer();
            vector<UINT64>::iterator it = buffer.begin();
            UINT64 Prev = *it++;
            UINT64 Last = *it++;
            UINT64 diff = Last - Prev;
            if (diff > MaxWRoll)
            {
                LogError(__FILEW__, __LINE__, L"History Buffer timestamp order violation, Last must be larger than First, accounting for rollover", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
                WriteToLog("@%llu, First:%lld, Last:%lld\n", (*pAllEvents)[i]->EventTime(), Prev, Last);
                tr = RESULT_FAIL;
            }
            for (; it != buffer.end(); ++it)
            {
                UINT64 Next = *it;
                diff = Next - Prev;
                if (diff > MaxWRoll)
                {
                    LogError(__FILEW__, __LINE__, L"History Buffer timestamp order violation, Timestamps must increase, accounting for rollover", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
                    WriteToLog("@%llu, Prev:%lld, Next:%lld\n", (*pAllEvents)[i]->EventTime(), Prev, Next);
                    tr = RESULT_FAIL;
                    break;
                }
                diff = Last - Next;
                if (diff > MaxWRoll)
                {
                    LogError(__FILEW__, __LINE__, L"History Buffer timestamp order violation, Completed Timestamps must not exceed Last timestamp, accounting for rollover", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
                    WriteToLog("@%llu, Next:%lld, Last:%lld\n", (*pAllEvents)[i]->EventTime(), Next, Last);
                    tr = RESULT_FAIL;
                    break;
                }
                Prev = Next;
            }
        }

        if((*pAllEvents)[i]->EventTime() >= markerBegin)
        {
            pHistoryBufferEvents->push_back((*pAllEvents)[i]);
        }
    }
    return tr;
}

TEST_RESULT CTimingDataTest::FilterCalibrateEvents(vector<CEvent_CalibrateGpuClock*> *pCalibrateGpuClockEvents)
{
    UINT64 markerBegin = 0;

    if(pCalibrateGpuClockEvents == nullptr)
    {
        LogError(__FILEW__, __LINE__, L"Pointer to event vector parameter is NULL", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError); 
        return RESULT_FAIL;
    }

    TEST_RESULT tr = GetMarkerBegin(&markerBegin);
    if(tr != RESULT_PASS)
    {
        return tr;
    }
    
    pCalibrateGpuClockEvents->clear();
    vector<CEvent_CalibrateGpuClock*> *pAllEvents;
    {
        m_Results.Task = DxgkControlGuid_TASK_CalibrateGpuClockTask;
        GatherSpecifiedEvents();
        pAllEvents = &m_Results.CalibrateClockVector;
    }

    if(pAllEvents == nullptr || pAllEvents->size() == 0)
    {
        return RESULT_PASS;
    }

    for(UINT i = 0; i < pAllEvents->size(); ++i)
    {
        if((*pAllEvents)[i]->EventTime() >= markerBegin && (*pAllEvents)[i]->NodeOrdinal() == 0)
        {
            pCalibrateGpuClockEvents->push_back((*pAllEvents)[i]);
        }
    }

    return RESULT_PASS;
}

TEST_RESULT CTimingDataTest::FilterCommandBufferEvents(vector<CEvent_CommandBufferSubmission*> *pCommandBufferEvents)
{
    UINT64 markerBegin = 0;

    if(pCommandBufferEvents == nullptr)
    {
        LogError(__FILEW__, __LINE__, L"Pointer to event vector parameter is NULL", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError); 
        return RESULT_FAIL;
    }

    TEST_RESULT tr = GetMarkerBegin(&markerBegin);
    if(tr != RESULT_PASS)
    {
        return tr;
    }
    
    pCommandBufferEvents->clear();
    vector<CEvent_CommandBufferSubmission*> *pAllEvents;
    m_Results.Task = Direct3D12EtwProviderGuid_TASK_RuntimeMarker;
    GatherSpecifiedEvents();
    pAllEvents = &m_Results.CommandBufferVector;        

    if(pAllEvents == nullptr || pAllEvents->size() == 0)
    {
        return RESULT_PASS;
    }

    for(UINT i = 0; i < pAllEvents->size(); ++i)
    {
        if((*pAllEvents)[i]->EventTime() >= markerBegin)
        {
            pCommandBufferEvents->push_back((*pAllEvents)[i]);
        }
    }

    return RESULT_PASS;
}

TEST_RESULT CTimingDataTest::FilterDroppedCallEvents(vector<CEvent_RuntimeDroppedCall*> *pDroppedCallEvents)
{
    UINT64 markerBegin = 0;

    if(pDroppedCallEvents == nullptr)
    {
        LogError(__FILEW__, __LINE__, L"Pointer to event vector parameter is NULL", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError); 
        return RESULT_FAIL;
    }

    TEST_RESULT tr = GetMarkerBegin(&markerBegin);
    if(tr != RESULT_PASS)
    {
        return tr;
    }
    
    pDroppedCallEvents->clear();
    vector<CEvent_RuntimeDroppedCall*> *pAllEvents;
    if(GetFramework()->Is11On12())
    {
        m_Results.Task = Direct3D12EtwProviderGuid_TASK_RuntimeDroppedCall;
        GatherSpecifiedEvents();
        pAllEvents = &m_Results.DroppedVector;    
    }
    else if(GetFramework()->IsCompat())
    {
        m_Results.Task = Direct3D11EtwProviderGuid_TASK_RuntimeDroppedCall;
        GatherSpecifiedEvents();
        pAllEvents = &m_Results.DroppedVector;
    }
    else
    {
        pAllEvents = m_pTimingDataHelper->GatherRuntimeDroppedCallList(RESULTS_FILE, m_wszProcessOption);
    }
    if(pAllEvents == nullptr || pAllEvents->size() == 0)
    {
        return RESULT_PASS;
    }

    for(UINT i = 0; i < pAllEvents->size(); ++i)
    {
        if((*pAllEvents)[i]->EventTime() >= markerBegin)
        {
            pDroppedCallEvents->push_back((*pAllEvents)[i]);
        }
    }

    return RESULT_PASS;
}

TEST_RESULT CTimingDataTest::FilterCommandListEvents(vector<CEvent_CommandList*> *pCommandListEvents)
{
    UINT64 markerBegin = 0;

    if(pCommandListEvents == nullptr)
    {
        LogError(__FILEW__, __LINE__, L"Pointer to event vector parameter is NULL", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError); 
        return RESULT_FAIL;
    }

    TEST_RESULT tr = GetMarkerBegin(&markerBegin);
    if(tr != RESULT_PASS)
    {
        return tr;
    }
    
    pCommandListEvents->clear();
    vector<CEvent_CommandList*> *pAllEvents;
    if(GetFramework()->Is11On12())
    {
        m_Results.Task = Direct3D12EtwProviderGuid_TASK_CommandList;
        GatherSpecifiedEvents();
        pAllEvents = &m_Results.CommandListVector;
    }
    else if(GetFramework()->IsCompat())
    {
        m_Results.Task = Direct3D11EtwProviderGuid_TASK_CommandList;
        GatherSpecifiedEvents();
        pAllEvents = &m_Results.CommandListVector;
    }
    else
    {
        pAllEvents = m_pTimingDataHelper->GatherCommandListList(RESULTS_FILE, m_wszProcessOption);
    }
    if(pAllEvents == nullptr || pAllEvents->size() == 0)
    {
        return RESULT_PASS;
    }

    for(UINT i = 0; i < pAllEvents->size(); ++i)
    {
        if((*pAllEvents)[i]->EventTime() >= markerBegin)
        {
            pCommandListEvents->push_back((*pAllEvents)[i]);
        }
    }

    return RESULT_PASS;
}

TEST_RESULT CTimingDataTest::CorrelateTimestamps(vector<CEvent_HistoryBuffer*> *pHistoryBufferEvents, vector<UINT64> *pTimestamps, vector<CEvent_HistoryBuffer*> *pFilteredEvents, vector<UINT64> *pElapsedTime)
{
    if(pHistoryBufferEvents == nullptr || pTimestamps == nullptr)
    {
        LogError(__FILEW__, __LINE__, L"Pointers to vector parameters are NULL", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError); 
        return RESULT_FAIL;
    }

    if(pFilteredEvents == pHistoryBufferEvents)
    {
        LogError(__FILEW__, __LINE__, L"Pointers to filtered and non-filtered events must not be the same", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError); 
        return RESULT_FAIL;
    }

    pTimestamps->clear();

    vector<CEvent_CommandBufferSubmission*> commandBufferEvents;
    TEST_RESULT tr = FilterCommandBufferEvents(&commandBufferEvents);
    if(tr != RESULT_PASS)
    {
        return tr;
    }

    if(pFilteredEvents != nullptr)
    {
        pFilteredEvents->clear();
    }

    vector<CEvent_HistoryBuffer*>::iterator itrHist = pHistoryBufferEvents->begin();
    while(itrHist != pHistoryBufferEvents->end())
    {
        const UINT32 renderCBSeq = (*itrHist)->RenderCbSequence();
        vector<CEvent_CommandBufferSubmission*>::iterator itrCmdBuffer = commandBufferEvents.begin();
        while(itrCmdBuffer != commandBufferEvents.end())
        {
            if((*itrCmdBuffer)->RenderCBSequenceNumber() == renderCBSeq)
            {
                vector<UINT64> histBuffer = (*itrHist)->HistoryBuffer();
                auto itrTimestamp = histBuffer.begin();
                UINT64 Prev = *itrTimestamp;
                pTimestamps->push_back(Prev);
                itrTimestamp++;
                pTimestamps->push_back(*itrTimestamp++);
                for (; itrTimestamp != histBuffer.end(); itrTimestamp++)
                {
                    UINT64 Next = *itrTimestamp;
                    pTimestamps->push_back(Next);
                    if (pElapsedTime)
                    {
                        pElapsedTime->push_back(Next - Prev);
                    }
                    Prev = Next;
                }

                if(pFilteredEvents != nullptr)
                {
                    pFilteredEvents->push_back(*itrHist);
                }

                break;
            }

            itrCmdBuffer++;
        }

        if(itrCmdBuffer != commandBufferEvents.end())
        {
            commandBufferEvents.erase(itrCmdBuffer);
        }
        
        itrHist++;
    }

    return RESULT_PASS;
}

TEST_RESULT CTimingDataTest::CorrelateTimestampsToApiSequenceNumbers(
    vector<CEvent_HistoryBuffer*> *pHistoryBufferEvents,
    vector<UINT64> *pTimestamps,
    vector<UINT64> *pElapsedTime,
    vector<UINT64> apiSequenceNumbers,
    vector<CEvent_HistoryBuffer*> *pFilteredEvents
    )
{
    if(pHistoryBufferEvents == nullptr || pTimestamps == nullptr)
    {
        LogError(__FILEW__, __LINE__, L"Pointers to vector parameters are NULL", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError); 
        return RESULT_FAIL;
    }

    if(pFilteredEvents == pHistoryBufferEvents)
    {
        LogError(__FILEW__, __LINE__, L"Pointers to filtered and non-filtered events must not be the same", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError); 
        return RESULT_FAIL;
    }

    pTimestamps->clear();
    if(pFilteredEvents != nullptr)
    {
        pFilteredEvents->clear();
    }

    vector<CEvent_HistoryBuffer*>::iterator itrHist = pHistoryBufferEvents->begin();
    while(itrHist != pHistoryBufferEvents->end())
    {
        const UINT32 renderCBSeq = (*itrHist)->RenderCbSequence();
        for (auto itrCmdBuffer = apiSequenceNumbers.begin(); itrCmdBuffer != apiSequenceNumbers.end(); ++itrCmdBuffer)
        {
            if (((UINT32)(*itrCmdBuffer)) == renderCBSeq)
            {
                vector<UINT64> histBuffer = (*itrHist)->HistoryBuffer();
                auto itrTimestamp = histBuffer.begin();
                UINT64 Prev = *itrTimestamp;
                pTimestamps->push_back(Prev);
                itrTimestamp++;
                pTimestamps->push_back(*itrTimestamp++);
                for (; itrTimestamp != histBuffer.end(); itrTimestamp++)
                {
                    UINT64 Next = *itrTimestamp;
                    pTimestamps->push_back(Next);
                    if (pElapsedTime)
                    {
                        pElapsedTime->push_back(Next - Prev);
                    }
                    Prev = Next;
                }
                if (pFilteredEvents != nullptr)
                {
                    pFilteredEvents->push_back(*itrHist);
                }
            }
        }
        
        itrHist++;
    }

    return RESULT_PASS;
}

WCHAR Results_wszLine[4000];
//
// Read the Result file and generate an error if it is non-empty.
//
TEST_RESULT CTimingDataTest::ReadResultFile()
{
    TEST_RESULT tr = RESULT_PASS;
    FILE* stream;
    errno_t err = _wfopen_s(&stream, RESULTS_FILE, L"r");
    if (err)
    {
        WriteToLog(L"Could not open Results file");
        tr = RESULT_FAIL;
    }
    else
    {
        int Max = 50;
        while (fgetws(Results_wszLine, _countof(Results_wszLine), stream))
        {
            if (Max-- <= 0)
            {
                WriteToLog(L"...continued. Limiting Error Log for readability");
                break;
            }
            WriteToLog(Results_wszLine);
            tr = RESULT_FAIL;
        }
        fclose(stream);
    }
    return tr;
}

void *AllocateMemory(UINT Size,std::list<std::vector<BYTE>> *MemoryList)
{
    std::vector<BYTE> AllocatedMemory;
    AllocatedMemory.resize(Size);
    MemoryList->push_front(AllocatedMemory);
    return &MemoryList->front().front();
}

// Processes Events
void WINAPI EventRecordCallback(_In_ PEVENT_RECORD pEventRecord)
{
    EventResults *Results = reinterpret_cast<EventResults*>(pEventRecord->UserContext);
    auto pData = reinterpret_cast<BYTE*>(pEventRecord->UserData);
    const size_t PointerSize = sizeof(UINT*);
    GUID TaskGuid = Direct3D11EtwProviderGuid;
    USHORT MarkerTask = Direct3D11EtwProviderGuid_TASK_Marker;
    USHORT RuntimeMarkerTask = Direct3D11EtwProviderGuid_TASK_RuntimeMarker;
    USHORT RuntimeMarkerValue = EventD3D11RuntimeMarkerData_value;
    USHORT CommandBufferValue = EventD3D11CommandBufferSubmission_value;
    USHORT RuntimeDroppedCallTask = Direct3D11EtwProviderGuid_TASK_RuntimeDroppedCall;
    USHORT CustomDriverMarkerTask = Direct3D11EtwProviderGuid_TASK_CustomDriverMarker;
    if(Results->m_Is11On12)
    {
        TaskGuid = Direct3D12EtwProviderGuid;
        MarkerTask = Direct3D12EtwProviderGuid_TASK_Marker;
        RuntimeMarkerTask = Direct3D12EtwProviderGuid_TASK_RuntimeMarker;
        RuntimeMarkerValue = EventD3D12RuntimeMarkerData_value;
        CommandBufferValue = EventD3D12CommandBufferSubmission_value;
        RuntimeDroppedCallTask = Direct3D12EtwProviderGuid_TASK_RuntimeDroppedCall;
        CustomDriverMarkerTask = Direct3D12EtwProviderGuid_TASK_CustomDriverMarker;
    }
    if(IsEqualGUID(pEventRecord->EventHeader.ProviderId, TaskGuid))
    {
        if (Results->Task == pEventRecord->EventHeader.EventDescriptor.Task && (pEventRecord->EventHeader.EventDescriptor.Task == MarkerTask && IsEqualGUID(pEventRecord->EventHeader.ProviderId, TaskGuid)))
        {
            // Grabs SetMarker, BeginEvent, EndEvent for 11 and 12
            CEvent_MarkerInt *EventData = reinterpret_cast<CEvent_MarkerInt*>(AllocateMemory(sizeof(CEvent_MarkerInt), &Results->EventMemory));
            COPY_VALUE_VARSIZE(EventData->m_pID3D11DeviceContext, pData, PointerSize);
            COPY_VALUE_FIXSIZE(EventData->m_APISequenceNumber, pData);

            const size_t labelLength = (wcslen((WCHAR*)(pData)) + 1) * sizeof(WCHAR);
            EventData->m_szLabel = wstring(reinterpret_cast<WCHAR *>(pData));
            pData += (labelLength);
            COPY_VALUE_FIXSIZE(EventData->m_Data, pData);
            EventData->m_ProcessId = (UINT32)Results->m_Pid;
            EventData->m_EventTime = pEventRecord->EventHeader.TimeStamp.QuadPart;

            Results->MarkerIntVector.push_back(EventData);
        }
        else if (pEventRecord->EventHeader.EventDescriptor.Task == Direct3D11EtwProviderGuid_TASK_Context && pEventRecord->EventHeader.ProcessId == Results->m_Pid && !Results->m_Is11On12)
        {
            // 11 specific.  Used to connect history buffers and command bufferss
            UINT64 device;
            UINT64 context;
            COPY_VALUE_VARSIZE(device, pData, PointerSize);
            COPY_VALUE_VARSIZE(context, pData, PointerSize);
            vector<DxgkDeviceD3dContext>::iterator d = Results->EventDxgDevCon.begin();
            while(d != Results->EventDxgDevCon.end())
            {
                if(d->ContextHandle == 0 && d->D3dDevice == device)
                {
                    d->ContextHandle = context;
                    break;
                }
                else if(d->D3dDevice == device)
                {
                    DxgkDeviceD3dContext EventData = {};
                    EventData.D3dDevice = device;
                    EventData.DxgiDevice = d->DxgiDevice;
                    EventData.ContextHandle = context;
                    Results->EventDxgDevCon.push_back(EventData);
                    break;
                }
                d++;
            }
        }
        else if (pEventRecord->EventHeader.EventDescriptor.Task == Direct3D11EtwProviderGuid_TASK_Device && pEventRecord->EventHeader.ProcessId == Results->m_Pid && !Results->m_Is11On12)
        {
            DxgkDeviceD3dContext EventData = {};
            COPY_VALUE_VARSIZE(EventData.D3dDevice, pData, PointerSize);
            COPY_VALUE_VARSIZE(EventData.DxgiDevice, pData, PointerSize);
            Results->EventDxgDevCon.push_back(EventData);
        }
        else if(Results->m_Is11On12 && pEventRecord->EventHeader.EventDescriptor.Task == Direct3D12EtwProviderGuid_TASK_ExecuteCommandList && pEventRecord->EventHeader.ProcessId == Results->m_Pid) // 1 is the value for start
        {
            EventExecuteCommandList* EventData = reinterpret_cast<EventExecuteCommandList*>(AllocateMemory(sizeof(EventExecuteCommandList), &Results->EventMemory));;
            COPY_VALUE_VARSIZE(EventData->pCommandQueue, pData, PointerSize);
            COPY_VALUE_VARSIZE(EventData->pCommandList, pData, PointerSize);
            if(pEventRecord->EventHeader.EventDescriptor.Opcode == 1)
            {
                Results->ValidExecuteVector.push_back(EventData);
            }
            else if(pEventRecord->EventHeader.EventDescriptor.Opcode == 2)
            {
                for(vector<EventExecuteCommandList*>::iterator It = Results->ValidExecuteVector.begin(); It != Results->ValidExecuteVector.end(); It++)
                {
                    if((*It)->pCommandList == EventData->pCommandList && (*It)->pCommandQueue == EventData->pCommandQueue)
                    {
                        vector<EventRuntimeMarkerData*>::iterator RuntimeIt = Results->PrempRuntimeMarkerVector.begin();
                        for(; RuntimeIt != Results->PrempRuntimeMarkerVector.end(); )
                        {
                            if((*RuntimeIt)->m_Context == (*It)->pCommandList)
                            {
                                (*RuntimeIt)->m_DxgiDev = EventData->pCommandQueue;
                                Results->RuntimeMarkerVector.push_back(*RuntimeIt);
                                RuntimeIt = Results->PrempRuntimeMarkerVector.erase(RuntimeIt);
                                // RuntimeIt = Results->PrempRuntimeMarkerVector.begin(); //TODO: does not allow re submission of Command lists already recorded, at the moment detect resets and deal with those instead to delete
                            }
                            else
                            {
                                RuntimeIt++;
                            }
                        }
                        if(It == Results->ValidExecuteVector.end())
                        {
                            Results->ValidExecuteVector.pop_back();
                        }
                        else
                        {
                            Results->ValidExecuteVector.erase(It);
                        }
                        
                        return;
                    }
                }
            }
        }
        else if (Results->Task == pEventRecord->EventHeader.EventDescriptor.Task && (pEventRecord->EventHeader.EventDescriptor.Task == RuntimeMarkerTask && IsEqualGUID(pEventRecord->EventHeader.ProviderId, TaskGuid) && pEventRecord->EventHeader.EventDescriptor.Id == RuntimeMarkerValue) && pEventRecord->EventHeader.ProcessId == Results->m_Pid)
        {
            UINT32 *pThreadID;
            UINT32 DataSize;
            BYTE ThreadIds;
            UINT64 m_CPUFrequency;
            UINT64 m_FirstAPISequenceNumber;
            UINT64 m_pID3D11DeviceContext;
            UINT32  m_CPUTimeHigh;
            vector<UINT32> m_vThreadID;
            COPY_VALUE_FIXSIZE(m_CPUFrequency, pData);
            COPY_VALUE_FIXSIZE(m_FirstAPISequenceNumber, pData);
            COPY_VALUE_VARSIZE(m_pID3D11DeviceContext, pData, PointerSize); // This is a CommandList in 12
            COPY_VALUE_FIXSIZE(m_CPUTimeHigh, pData);
            COPY_VALUE_FIXSIZE(ThreadIds, pData);
            pThreadID = reinterpret_cast<UINT32 *>(pData);
            for(UINT32 i = 0; i < (UINT32) ThreadIds; i++)
            {
                m_vThreadID.push_back(*pThreadID);
                pData += sizeof(UINT32);
                pThreadID++;
            }
            COPY_VALUE_FIXSIZE(DataSize, pData);
            UINT64 dxgi = 0;
            if (!Results->m_Is11On12)
            {

                for (DxgkDeviceD3dContext d : Results->EventDxgDevCon)
                {
                    if (d.ContextHandle == m_pID3D11DeviceContext)
                    {
                        dxgi = d.DxgiDevice;
                        break;
                    }
                }
            }
            LARGE_INTEGER QPC = {};
            QPC.HighPart = m_CPUTimeHigh;
            EventRuntimeMarkerData *BeginMarker = nullptr;
            for (void *pDataEnd = pData + DataSize; pData < pDataEnd;)
            {
                EventBatch BatchData;
                if(Results->m_Is11On12)
                {
                    EventBatch12 Batch12Data = *reinterpret_cast<EventBatch12 *>(pData);
                    BatchData.MarkerAPI = Batch12Data.MarkerAPI;
                    BatchData.QPCLow = Batch12Data.QPCLow;
                    BatchData.ThreadIDOrdinal = Batch12Data.ThreadIDOrdinal;
                    if(BatchData.MarkerAPI == 3)
                    {
                        Batch12Data.MarkerAPI;
                        Results->Draws++;
                    }
                    else if(BatchData.MarkerAPI == (MSG_Map_D3D12_MARKER_API_CLEARRENDERTARGETVIEW - MSG_Map_D3D12_MARKER_API_SETMARKER))
                    {
                        // TODO: Investigate, Need to switch these to so that the validation checks the draw versus when the clear finished this allows the test to pass on smaller
                        // runs.
                        BeginMarker->m_MarkerApi = Batch12Data.MarkerAPI;
                        BatchData.MarkerAPI = MSG_Map_D3D12_MARKER_API_ENDSUBMISSION - MSG_Map_D3D12_MARKER_API_SETMARKER;
                    }
                    pData += sizeof(EventBatch12);
                }
                else
                {
                    BatchData = *reinterpret_cast<EventBatch *>(pData);
                    pData += sizeof(EventBatch);
                }
                QPC.LowPart = BatchData.QPCLow;
                EventRuntimeMarkerData *EventData = reinterpret_cast<EventRuntimeMarkerData*>(AllocateMemory(sizeof(EventRuntimeMarkerData), &Results->EventMemory));
                EventData->m_ApiSeqNum = m_FirstAPISequenceNumber;
                EventData->m_Context = m_pID3D11DeviceContext;
                EventData->m_MarkerApi = BatchData.MarkerAPI;
                EventData->m_DxgiDev = dxgi;
                EventData->m_ThreadId = m_vThreadID[BatchData.ThreadIDOrdinal];
                EventData->QPC = QPC.QuadPart;
                EventData->CpuFreq = m_CPUFrequency;
                m_FirstAPISequenceNumber++;
                if(Results->m_Is11On12 && (BatchData.MarkerAPI == (MSG_Map_D3D12_MARKER_API_ENDSUBMISSION - MSG_Map_D3D12_MARKER_API_SETMARKER) || BatchData.MarkerAPI == (MSG_Map_D3D12_MARKER_API_RESOURCEBARRIER - MSG_Map_D3D12_MARKER_API_SETMARKER)))
                {
                    BeginMarker = EventData;
                }
                if(Results->m_Is11On12)
                {
                    Results->PrempRuntimeMarkerVector.push_back(EventData);
                }
                else
                {
                    Results->RuntimeMarkerVector.push_back(EventData);
                }
            }
        
        }
        else if(Results->Task == pEventRecord->EventHeader.EventDescriptor.Task && (pEventRecord->EventHeader.EventDescriptor.Task == RuntimeMarkerTask && IsEqualGUID(pEventRecord->EventHeader.ProviderId, TaskGuid) && pEventRecord->EventHeader.EventDescriptor.Id == CommandBufferValue) && pEventRecord->EventHeader.ProcessId == Results->m_Pid)
        {
            UINT32 BroadcastContext = 0;
            UINT32 CompletedAPISequenceNumberLow0Size = 0;
            UINT32 CompletedAPISequenceNumberLow1Size = 0;
            UINT32 BegunAPISequenceNumberLow0Size = 0;
            UINT32 BegunAPISequenceNumberLow1Size = 0;
            CEvent_CommandBufferSubmission *EventData = reinterpret_cast<CEvent_CommandBufferSubmission*>(AllocateMemory(sizeof(CEvent_CommandBufferSubmission), &Results->EventMemory));
            COPY_VALUE_VARSIZE(EventData->m_pIDXGIDevice, pData, PointerSize);
            if(!Results->m_Is11On12)
            {
                BYTE BroadcastContextByte = 0;
                COPY_VALUE_FIXSIZE(EventData->m_hContext, pData);
                COPY_VALUE_FIXSIZE(BroadcastContextByte, pData);
                BroadcastContext = (UINT32) BroadcastContextByte;
            }
            else
            {
                COPY_VALUE_FIXSIZE(BroadcastContext, pData);
            }
            
            UINT32 *pBroadcastContexts = reinterpret_cast<UINT32*>(pData);
            for(UINT32 i = 0; i < (UINT32)BroadcastContext; i++)
            {
                EventData->m_vBroadcastContexts.push_back(*pBroadcastContexts);
                pBroadcastContexts++;
                pData += sizeof(UINT32);
            }
            if(Results->m_Is11On12)
            {
                EventData->m_hContext = EventData->m_vBroadcastContexts[0];
            }
            COPY_VALUE_FIXSIZE(EventData->m_MarkerLogType, pData); // treat this as loop iteration for 12
            COPY_VALUE_FIXSIZE(EventData->m_RenderCBSequenceNumber, pData);
            COPY_VALUE_FIXSIZE(EventData->m_FirstAPISequenceNumberHigh, pData);
            COPY_VALUE_FIXSIZE(CompletedAPISequenceNumberLow0Size, pData);
            EventData->m_MarkerLogType = pEventRecord->EventHeader.ThreadId; // For mobile this is used to identify thread ID
            if(!Results->m_Is11On12)
            {
                COPY_VALUE_FIXSIZE(CompletedAPISequenceNumberLow1Size, pData);
                COPY_VALUE_FIXSIZE(BegunAPISequenceNumberLow0Size, pData);
                COPY_VALUE_FIXSIZE(BegunAPISequenceNumberLow1Size, pData);
            }
            
            UINT32 seqNum0Size = CompletedAPISequenceNumberLow0Size * sizeof(UINT32);
            UINT32 *CompletedAPISequenceNumberLow0 = reinterpret_cast<UINT32*>(pData);
            pData += seqNum0Size;
            for(UINT32 i = 0; i < CompletedAPISequenceNumberLow0Size; i++)
            {
                EventData->m_vCompletedAPISequenceNumberLow0.push_back(*CompletedAPISequenceNumberLow0);
                CompletedAPISequenceNumberLow0++;
            }
            if(!Results->m_Is11On12)
            {
                UINT32 seqNum1Size = CompletedAPISequenceNumberLow1Size * sizeof(UINT32);
                UINT32 *CompletedAPISequenceNumberLow1 = reinterpret_cast<UINT32*>(pData);
                pData += seqNum1Size;
                for (UINT32 i = 0; i < CompletedAPISequenceNumberLow1Size; i++)
                {
                    EventData->m_vCompletedAPISequenceNumberLow1.push_back(*CompletedAPISequenceNumberLow1);
                    CompletedAPISequenceNumberLow1++;
                }

                seqNum0Size = BegunAPISequenceNumberLow0Size * sizeof(UINT32);
                UINT32 *BegunAPISequenceNumberLow0 = reinterpret_cast<UINT32*>(pData);
                pData += seqNum0Size;
                for (UINT32 i = 0; i < BegunAPISequenceNumberLow0Size; i++)
                {
                    EventData->m_vBegunAPISequenceNumberLow0.push_back(*BegunAPISequenceNumberLow0);
                    BegunAPISequenceNumberLow0++;
                }
                seqNum1Size = BegunAPISequenceNumberLow1Size * sizeof(UINT32);
                UINT32 *BegunAPISequenceNumberLow1 = reinterpret_cast<UINT32*>(pData);
                for (UINT32 i = 0; i < BegunAPISequenceNumberLow1Size; i++)
                {
                    EventData->m_vBegunAPISequenceNumberLow1.push_back(*BegunAPISequenceNumberLow1);
                    BegunAPISequenceNumberLow1++;
                }
            }

            EventData->m_ProcessId = pEventRecord->EventHeader.ProcessId;
            EventData->m_EventTime = pEventRecord->EventHeader.TimeStamp.QuadPart;
            Results->CommandBufferVector.push_back(EventData);
        }
        else if(Results->Task == pEventRecord->EventHeader.EventDescriptor.Task && (pEventRecord->EventHeader.EventDescriptor.Task == RuntimeDroppedCallTask && IsEqualGUID(pEventRecord->EventHeader.ProviderId, TaskGuid)) && pEventRecord->EventHeader.ProcessId == Results->m_Pid)
        {
            CEvent_RuntimeDroppedCall *EventData = reinterpret_cast<CEvent_RuntimeDroppedCall*>(AllocateMemory(sizeof(CEvent_RuntimeDroppedCall), &Results->EventMemory));
            COPY_VALUE_VARSIZE(EventData->m_pID3D11DeviceContext, pData, PointerSize);
            COPY_VALUE_FIXSIZE(EventData->m_Type, pData);

            EventData->m_ProcessId = pEventRecord->EventHeader.ProcessId;
            EventData->m_EventTime = pEventRecord->EventHeader.TimeStamp.QuadPart;

            Results->DroppedVector.push_back(EventData);        
        }
        else if(Results->Task == pEventRecord->EventHeader.EventDescriptor.Task && pEventRecord->EventHeader.EventDescriptor.Task == Direct3D11EtwProviderGuid_TASK_CommandList && pEventRecord->EventHeader.ProcessId == Results->m_Pid)
        {
            CEvent_CommandList *EventData = reinterpret_cast<CEvent_CommandList*>(AllocateMemory(sizeof(CEvent_CommandList), &Results->EventMemory));
            COPY_VALUE_VARSIZE(EventData->m_pID3D11DeviceContext, pData, PointerSize);
            COPY_VALUE_VARSIZE(EventData->m_pID3D11CommandList, pData, PointerSize);

            EventData->m_ProcessId = (UINT32)Results->m_Pid;
            EventData->m_EventTime = pEventRecord->EventHeader.TimeStamp.QuadPart;
            Results->CommandListVector.push_back(EventData);
        }
        else if(Results->Task == pEventRecord->EventHeader.EventDescriptor.Task && ((pEventRecord->EventHeader.EventDescriptor.Task == CustomDriverMarkerTask && IsEqualGUID(pEventRecord->EventHeader.ProviderId, TaskGuid))) && pEventRecord->EventHeader.ProcessId == Results->m_Pid)
        {
            // Gathers CustomDriver Markers for both 11 and 12
            CEvent_CustomDriverMarker *EventData = reinterpret_cast<CEvent_CustomDriverMarker*>(AllocateMemory(sizeof(CEvent_CustomDriverMarker), &Results->EventMemory));
            COPY_VALUE_FIXSIZE(EventData->m_InsertionAPISequenceNumber, pData);
            COPY_VALUE_VARSIZE(EventData->m_pID3D11DeviceContext, pData, PointerSize);
            if(!Results->m_Is11On12)
            {
                COPY_VALUE_FIXSIZE(EventData->m_hContext, pData);
            }
            COPY_VALUE_FIXSIZE(EventData->m_Index, pData);
            if(!Results->m_Is11On12)
            {
                COPY_VALUE_FIXSIZE(EventData->m_StringIndex, pData);
                EventData->m_szMarkerDescription = wstring(reinterpret_cast<WCHAR *>(pData));
            }
            EventData->m_ProcessId = pEventRecord->EventHeader.ProcessId;
            EventData->m_EventTime = pEventRecord->EventHeader.TimeStamp.QuadPart;

            Results->CustomVector.push_back(EventData);
        }
    }
    else if (IsEqualGUID(pEventRecord->EventHeader.ProviderId, DxgkControlGuid))
    {
        if (pEventRecord->EventHeader.EventDescriptor.Task == DxgkControlGuid_TASK_HistoryBuffer && Results->Task == pEventRecord->EventHeader.EventDescriptor.Task)
        {
            CEvent_HistoryBuffer *EventData = reinterpret_cast<CEvent_HistoryBuffer*>(AllocateMemory(sizeof(CEvent_HistoryBuffer), &Results->EventMemory));
            COPY_VALUE_VARSIZE(EventData->m_hContext, pData, PointerSize);
            COPY_VALUE_FIXSIZE(EventData->m_RenderCbSequence, pData);
            COPY_VALUE_FIXSIZE(EventData->m_DMASubmissionSequence, pData);
            COPY_VALUE_FIXSIZE(EventData->m_Precision, pData);
            COPY_VALUE_FIXSIZE(EventData->m_HistoryBufferSize, pData);
            UINT32 Stride = EventData->m_Precision <= 32 ? 4: 8;
            UINT64 PrecisionMask = (EventData->m_Precision == 64) ? 0 : (1ULL << EventData->m_Precision);
            UINT64 Mask = PrecisionMask - 1;
            UINT64 Val = 0;
            for(UINT i = 0; i < EventData->m_HistoryBufferSize; i += Stride)
            {
                Val = *reinterpret_cast<UINT64*>(pData);
                EventData->m_vHistoryBuffer.push_back((Mask & Val));
                pData += Stride;
            }
            EventData->m_ProcessId = (UINT32)pEventRecord->EventHeader.ProcessId;
            EventData->m_EventTime = pEventRecord->EventHeader.TimeStamp.QuadPart;
            for(vector<EventDxgContext>::iterator It = Results->EventDxgContext.begin(); It != Results->EventDxgContext.end(); It++)
                if(It->hContext == EventData->m_hContext)
                {
                    EventData->m_hContext = It->ContextHandle;
                    Results->HistoryVector.push_back(EventData);
                    
                    for(CEvent_CalibrateGpuClock* CalibrateEvent : Results->ValidCalibrateVector)
                    {
                        if(CalibrateEvent->m_EngineOrdinal == It->EngineAffinity && CalibrateEvent->m_NodeOrdinal == It->NodeOrdinal)
                        {
                            EventHistoryBuff *HistBuff = reinterpret_cast<EventHistoryBuff*>(AllocateMemory(sizeof(EventHistoryBuff), &Results->EventMemory));;
                            HistBuff->EventData = EventData;
                            HistBuff->m_CpuClock = CalibrateEvent->m_CpuClock;
                            HistBuff->m_GpuClock = CalibrateEvent->m_GpuClock;
                            HistBuff->m_GpuFrequency = CalibrateEvent->m_GpuFrequency;
                            Results->ValidHistoryVector.push_back(*HistBuff);
                            break;
                        }
                    }
                    break;
                }
                    
        }
        else if(pEventRecord->EventHeader.EventDescriptor.Task == DxgkControlGuid_TASK_CalibrateGpuClockTask)
        {
            CEvent_CalibrateGpuClock *EventData = reinterpret_cast<CEvent_CalibrateGpuClock*>(AllocateMemory(sizeof(CEvent_CalibrateGpuClock), &Results->EventMemory));
            COPY_VALUE_VARSIZE(EventData->m_hAdapter, pData, PointerSize);
            COPY_VALUE_FIXSIZE(EventData->m_NodeOrdinal, pData);
            COPY_VALUE_FIXSIZE(EventData->m_EngineOrdinal, pData);
            COPY_VALUE_FIXSIZE(EventData->m_GpuFrequency, pData);
            COPY_VALUE_FIXSIZE(EventData->m_GpuClock, pData);
            COPY_VALUE_FIXSIZE(EventData->m_CpuClock, pData);
            EventData->m_ProcessId = (UINT32)pEventRecord->EventHeader.ProcessId;
            EventData->m_EventTime = pEventRecord->EventHeader.TimeStamp.QuadPart;

            for (EventDxgDevice Device : Results->ValidDxgkDeviceVector)
            {
                if (Device.pDxgAdapter == EventData->Adapter())
                {
                    if (Results->Task == pEventRecord->EventHeader.EventDescriptor.Task)
                        Results->CalibrateClockVector.push_back(EventData);
                    else if (Results->Task == DxgkControlGuid_TASK_HistoryBuffer)
                    {
                        for (EventDxgDevice Device : Results->ValidDxgkDeviceVector)
                        {
                            if (Device.pDxgAdapter == EventData->Adapter())
                            {
                                Results->ValidCalibrateVector.push_back(EventData);
                                for (vector<CEvent_CalibrateGpuClock*>::iterator CalibrateIt = Results->ValidCalibrateVector.begin(); CalibrateIt != Results->ValidCalibrateVector.end(); CalibrateIt++)
                                {
                                    if ((*CalibrateIt)->m_NodeOrdinal == EventData->m_NodeOrdinal && (*CalibrateIt)->m_EngineOrdinal == EventData->m_EngineOrdinal)
                                    {
                                        (*CalibrateIt)->m_GpuFrequency = EventData->m_GpuFrequency;
                                        (*CalibrateIt)->m_CpuClock = EventData->m_CpuClock;
                                        (*CalibrateIt)->m_GpuClock = EventData->m_GpuClock;
                                        return;
                                    }
                                }
                                Results->ValidCalibrateVector.push_back(EventData);
                                break;
                            }
                        }
                    }
                }
            }
        }
        else if(pEventRecord->EventHeader.EventDescriptor.Task == DxgkControlGuid_TASK_Context)
        {
            // hContext and Context Handles
            EventDxgContext EventData = {};
            COPY_VALUE_VARSIZE(EventData.hDevice, pData, PointerSize);
            COPY_VALUE_FIXSIZE(EventData.NodeOrdinal, pData);
            COPY_VALUE_FIXSIZE(EventData.EngineAffinity, pData);
            COPY_VALUE_FIXSIZE(EventData.DmaBufferSize, pData);
            COPY_VALUE_FIXSIZE(EventData.DmaBufferSegmentSet, pData);
            COPY_VALUE_FIXSIZE(EventData.DmaBufferPrivateDataSize, pData);
            COPY_VALUE_FIXSIZE(EventData.AllocationListSize, pData);
            COPY_VALUE_FIXSIZE(EventData.PatchLocationListSize, pData);
            COPY_VALUE_FIXSIZE(EventData.ContextFlags, pData);
            COPY_VALUE_VARSIZE(EventData.hContext, pData, PointerSize);
            COPY_VALUE_VARSIZE(EventData.ContextHandle, pData, PointerSize);
            UINT32 EngineOrd = 0;
            if ((EventData.EngineAffinity & (EventData.EngineAffinity - 1)) == 0)
            {
                UINT32 Mask = EventData.EngineAffinity;
                while( Mask >>= 1)
                {
                    EngineOrd++;
                }
            }
            EventData.EngineAffinity = EngineOrd;
            for(EventDxgDevice Device : Results->ValidDxgkDeviceVector)
            {
                if(Device.hDevice == EventData.hDevice)
                {
                    Results->EventDxgContext.push_back(EventData);
                    break;
                }
            }

        }
        else if(pEventRecord->EventHeader.EventDescriptor.Task == DxgkControlGuid_TASK_Device)
        {
            EventDxgDevice DxgDeviceData = {};
            COPY_VALUE_VARSIZE(DxgDeviceData.hProcessId, pData, PointerSize);
            COPY_VALUE_VARSIZE(DxgDeviceData.pDxgAdapter, pData, PointerSize);
            COPY_VALUE_FIXSIZE(DxgDeviceData.ClientType, pData);
            COPY_VALUE_VARSIZE(DxgDeviceData.hDevice, pData, PointerSize);
            if(DxgDeviceData.hProcessId == Results->m_Pid)
            {
                Results->ValidDxgkDeviceVector.push_back(DxgDeviceData);
            }
        }
    }
}

void CTimingDataTest::GatherSpecifiedEvents()
{
    m_Results.m_Is11On12 = GetFramework()->Is11On12();
    m_Results.m_Pid = m_Pid;
    m_Results.EventDxgDevCon.clear();
    m_Results.ValidDxgkDeviceVector.clear();
    m_Results.ValidExecuteVector.clear();
    m_Results.Draws = 0;
    switch(m_Results.Task)
    {
    case DxgkControlGuid_TASK_CalibrateGpuClockTask:
        m_Results.CalibrateClockVector.clear();
    case DxgkControlGuid_TASK_HistoryBuffer:
        m_Results.HistoryVector.clear();
        m_Results.ValidHistoryVector.clear();
        m_Results.ValidCalibrateVector.clear();
        break;
    case Direct3D11EtwProviderGuid_TASK_CommandList:
        m_Results.CommandListVector.clear();
        break;
    case Direct3D11EtwProviderGuid_TASK_CustomDriverMarker:
        m_Results.CustomVector.clear();
        break;
    case Direct3D11EtwProviderGuid_TASK_Marker:
        m_Results.MarkerIntVector.clear();
        break;
    case Direct3D11EtwProviderGuid_TASK_RuntimeDroppedCall:
        m_Results.DroppedVector.clear();
        break;
    case Direct3D11EtwProviderGuid_TASK_RuntimeMarker:
        m_Results.CommandBufferVector.clear();
        m_Results.RuntimeMarkerVector.clear();
        break;
    default:
        break;
    }
    #define UNICODE
    Etw::Processor::EtwFileConsumer Consumer(ETLFileName());
    Consumer.SetEventRecordCallback(EventRecordCallback, &m_Results);
    Consumer.OpenTraceW();
    Consumer.ProcessTrace();
    Consumer.CloseTrace();
    #undef UNICODE    
}

TEST_RESULT CTimingDataTest::FilterRuntimeMarkers(vector<EventRuntimeMarkerData> *SortedRuntimeMarkers)
{
    m_Results.Task = Direct3D11EtwProviderGuid_TASK_RuntimeMarker;
    GatherSpecifiedEvents();
    SortedRuntimeMarkers->reserve(m_Results.RuntimeMarkerVector.size());
    for(EventRuntimeMarkerData *R : m_Results.RuntimeMarkerVector)
    {
        SortedRuntimeMarkers->push_back(*R);
    }
    return RESULT_PASS;
}

HRESULT CTimingDataTest::FilterAndSortRuntimeMarkers(vector<EventRuntimeMarkerData> *SortedRuntimeMarkers, UINT64 StartTime)
{
    // FilterHistoryBufferEvents
    m_Results.Task = DxgkControlGuid_TASK_HistoryBuffer;
    GatherSpecifiedEvents(); // get History Buffers
    vector<CEvent_HistoryBuffer *> HistoryBuffers = m_Results.HistoryVector;
    m_Results.Task = Direct3D11EtwProviderGuid_TASK_RuntimeMarker;
    GatherSpecifiedEvents(); // get CommandBufferSubmissions and RuntimeDetails
    vector<CEvent_CommandBufferSubmission *> CommandBuffers = m_Results.CommandBufferVector;
    vector<EventRuntimeMarkerData *> RuntimeMarkers = m_Results.RuntimeMarkerVector;
    SortedRuntimeMarkers->reserve(RuntimeMarkers.size());
    for (vector<EventHistoryBuff>::iterator HBuffer = m_Results.ValidHistoryVector.begin(); HBuffer != m_Results.ValidHistoryVector.end(); HBuffer++)
    {
        for (vector<CEvent_CommandBufferSubmission *>::iterator CBuffer = CommandBuffers.begin(); CBuffer != CommandBuffers.end(); CBuffer++)
        {
            if (HBuffer->EventData->RenderCbSequence() == (*CBuffer)->RenderCBSequenceNumber() && HBuffer->EventData->m_hContext == (*CBuffer)->Context())
            {
                UINT32 i = 2; // always start two in the first two history buffer values are the start and end time for the DMA packet
                if(GetFramework()->Is11On12())
                {
                    i = 0; // there are no extra timestamps in 12
                }
                LARGE_INTEGER ApiSequenceNumber;
                ApiSequenceNumber.HighPart = (*CBuffer)->m_FirstAPISequenceNumberHigh;

                if (!GetFramework()->Is11On12())
                {
                    if ((*CBuffer)->m_vCompletedAPISequenceNumberLow1.size() > 0)
                    {
                        ApiSequenceNumber.HighPart++;
                        for (UINT32 u : (*CBuffer)->m_vCompletedAPISequenceNumberLow1)
                        {
                            ApiSequenceNumber.LowPart = (UINT64)u;
                            for (vector<EventRuntimeMarkerData *>::iterator RMarker = RuntimeMarkers.begin(); RMarker != RuntimeMarkers.end(); )
                            {
                                if ((*RMarker)->m_ThreadId == (*CBuffer)->m_MarkerLogType && (UINT64)ApiSequenceNumber.QuadPart == (*RMarker)->m_ApiSeqNum && (*RMarker)->fSorted == false)
                                {
                                    EventRuntimeMarkerData Marker = {};
                                    Marker.m_MarkerApi = (*RMarker)->m_MarkerApi;
                                    if (i > HBuffer->EventData->m_HistoryBufferSize)
                                    {
                                        return E_FAIL;
                                    }
                                    UINT64 PrecisionMask = (HBuffer->EventData->m_Precision == 64) ? 0 : (1ULL << HBuffer->EventData->m_Precision);
                                    UINT64 Mask = PrecisionMask - 1;
                                    INT64 GpuTime = (INT64)HBuffer->EventData->m_vHistoryBuffer[i] - (INT64)(HBuffer->m_GpuClock & Mask);
                                    GpuTime *= (INT64)1000000000;
                                    GpuTime /= (INT64)HBuffer->m_GpuFrequency;
                                    GpuTime += ((((INT64)HBuffer->m_CpuClock - (INT64)StartTime) * 1000000000) / (*RMarker)->CpuFreq);
                                    Marker.m_HistoryBuff = GpuTime;
                                    SortedRuntimeMarkers->push_back(Marker);
                                    i++;
                                    (*RMarker)->fSorted = true;
                                    RMarker = RuntimeMarkers.erase(RMarker);
                                    break;

                                }
                                else
                                {
                                    RMarker++;
                                }
                            }
                        }
                    }
                    if ((*CBuffer)->m_vBegunAPISequenceNumberLow0.size() > 0 && RuntimeMarkers.size() != 0)
                    {
                        for (UINT32 u : (*CBuffer)->m_vBegunAPISequenceNumberLow0)
                        {
                            ApiSequenceNumber.LowPart = (UINT64)u;
                            for (vector<EventRuntimeMarkerData *>::iterator RMarker = RuntimeMarkers.begin(); RMarker != RuntimeMarkers.end();)
                            {
                                if ((*RMarker)->m_ThreadId == (*CBuffer)->m_MarkerLogType && (UINT64)ApiSequenceNumber.QuadPart == (*RMarker)->m_ApiSeqNum && (*RMarker)->fSorted == false)
                                {
                                    EventRuntimeMarkerData Marker = {};
                                    Marker.m_MarkerApi = (*RMarker)->m_MarkerApi;
                                    if (i > HBuffer->EventData->m_HistoryBufferSize)
                                    {
                                        return E_FAIL;
                                    }
                                    UINT64 PrecisionMask = (HBuffer->EventData->m_Precision == 64) ? 0 : (1ULL << HBuffer->EventData->m_Precision);
                                    UINT64 Mask = PrecisionMask - 1;
                                    INT64 GpuTime = (INT64)HBuffer->EventData->m_vHistoryBuffer[i] - (INT64)(HBuffer->m_GpuClock & Mask);
                                    GpuTime *= (INT64)1000000000;
                                    GpuTime /= (INT64)HBuffer->m_GpuFrequency;
                                    GpuTime += ((((INT64)HBuffer->m_CpuClock - (INT64)StartTime) * 1000000000) / (*RMarker)->CpuFreq);

                                    Marker.m_HistoryBuff = GpuTime;
                                    SortedRuntimeMarkers->push_back(Marker);
                                    i++;
                                    (*RMarker)->fSorted = true;
                                    RMarker = RuntimeMarkers.erase(RMarker);
                                    break;
                                }
                                else
                                {
                                    RMarker++;
                                }
                            }
                        }
                    }
                    if ((*CBuffer)->m_vBegunAPISequenceNumberLow1.size() > 0 && RuntimeMarkers.size() != 0)
                    {
                        ApiSequenceNumber.HighPart++;
                        for (UINT32 u : (*CBuffer)->m_vBegunAPISequenceNumberLow1)
                        {
                            ApiSequenceNumber.LowPart = (UINT64)u;
                            for (vector<EventRuntimeMarkerData *>::iterator RMarker = RuntimeMarkers.begin(); RMarker != RuntimeMarkers.end(); )
                            {
                                if ((*RMarker)->m_ThreadId == (*CBuffer)->m_MarkerLogType && (UINT64)ApiSequenceNumber.QuadPart == (*RMarker)->m_ApiSeqNum && (*RMarker)->fSorted == false)
                                {
                                    EventRuntimeMarkerData Marker = {};
                                    Marker.m_MarkerApi = (*RMarker)->m_MarkerApi;
                                    if (i > HBuffer->EventData->m_HistoryBufferSize)
                                    {
                                        return E_FAIL;
                                    }
                                    UINT64 PrecisionMask = (HBuffer->EventData->m_Precision == 64) ? 0 : (1ULL << HBuffer->EventData->m_Precision);
                                    UINT64 Mask = PrecisionMask - 1;
                                    INT64 GpuTime = (INT64)HBuffer->EventData->m_vHistoryBuffer[i] - (INT64)(HBuffer->m_GpuClock & Mask);
                                    GpuTime *= (INT64)1000000000;
                                    GpuTime /= (INT64)HBuffer->m_GpuFrequency;
                                    GpuTime += ((((INT64)HBuffer->m_CpuClock - (INT64)StartTime) * 1000000000) / (*RMarker)->CpuFreq);

                                    Marker.m_HistoryBuff = GpuTime;
                                    SortedRuntimeMarkers->push_back(Marker);
                                    i++;
                                    (*RMarker)->fSorted = true;
                                    RMarker = RuntimeMarkers.erase(RMarker);
                                    break;
                                }
                                else
                                {
                                    RMarker++;
                                }
                            }
                        }
                    }
                }
            }
            else
                continue;
        }
    }
    return S_OK;
}