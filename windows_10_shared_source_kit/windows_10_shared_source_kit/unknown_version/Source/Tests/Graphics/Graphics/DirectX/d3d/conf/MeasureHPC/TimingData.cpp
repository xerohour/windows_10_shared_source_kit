////////////////////////////////////////////////////////////////////////////////
// TimingData.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "TimingData.h"
#include "d3d11sdklayers.h"

//#include "EndOfPipeline.h"
#include "Frequency.h"
//#include "MultipleEngine.h"
//#include "MarkerValidation.h"
//#include "PState.h"
//#include "CustomEvent.h"

#include "provider.h"

#define MAX_WAIT_TIME 10000

CTimingDataApp g_App;

string g_szPassthroughVS = 
"	struct VSIn \n" 
"	{ \n"
"		float4 pos : POSITION; \n"
"		float2 tex : TEXCOORD; \n"
"	}; \n"
" \n"
"	struct VSOut \n"
"	{ \n"
"		float4 pos : SV_Position; \n"
"		float2 tex : TEXCOORD; \n"
"	}; \n"
" \n"
"	VSOut VSMain(VSIn input) \n"
"	{ \n"
"		VSOut output; \n"
"		output.pos.xyz = input.pos.xyz; \n"
"		output.pos.w = 1; \n"
"		output.tex = input.tex; \n"
"		\n"
"		return output; \n"
"	} \n";

string g_szPassthroughPS = 
"	struct PSIn \n"
"	{ \n"
"		float4 pos : SV_Position; \n"
"		float2 tex : TEXCOORD; \n"
"	}; \n"
"   \n"
"	float4 PSMain(PSIn input) : SV_TARGET \n"
"	{    \n"
"		float4 temp = float4(1.0, 1.0, 1.0, 1.0); \n"
"		return (temp); \n"
"	} \n";

string g_szPassthroughPS_Alt = 
"	struct PSIn \n"
"	{ \n"
"		float4 pos : SV_Position; \n"
"		float2 tex : TEXCOORD; \n"
"	}; \n"
"   \n"
"	float4 PSMain(PSIn input) : SV_TARGET \n"
"	{    \n"
"		float4 temp = float4(0.5, 0.5, 0.5, 1.0); \n"
"		return (temp); \n"
"	} \n";

string g_szBasicCS = 
"	RWTexture2D<float> texUAV; \n"
"	\n"
"	struct CSIn \n"
"	{	\n"
"		uint3 threadID : SV_GroupThreadID; \n"
"		uint3 groupID : SV_GroupID; \n"
"		uint threadIndex : SV_GroupIndex; \n"
"	};	\n"
"	\n"
"	[numthreads(1,1,1)] \n"
"	void CSMain(CSIn input) \n"
"	{	\n"
"		texUAV[input.threadID.xy] = (float)input.threadIndex; \n"
"	}	\n";

string g_szBasicCS_Alt = 
"	RWTexture2D<float> texUAV; \n"
"	\n"
"	struct CSIn \n"
"	{	\n"
"		uint3 threadID : SV_GroupThreadID; \n"
"		uint3 groupID : SV_GroupID; \n"
"		uint threadIndex : SV_GroupIndex; \n"
"	};	\n"
"	\n"
"	[numthreads(1,1,1)] \n"
"	void CSMain(CSIn input) \n"
"	{	\n"
"		texUAV[input.threadID.xy] = (float)input.threadIndex * 2.0f; \n"
"	}	\n";

////////////////////////////////////////////////////////////////////////////////
//
// CTimingDataApp
//
////////////////////////////////////////////////////////////////////////////////

CTimingDataApp::CTimingDataApp()
{
    m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;
}

bool CTimingDataApp::InitTestGroups()
{
    //ADD_TEST_AND_DESC(_T("MultipleEngine/Draw"), CMultipleEngineTest_Draw, _T("Test cases for ensuring the GPU timestamp appears on the rasterization engine"));
    //ADD_TEST_AND_DESC(_T("MultipleEngine/Copy"), CMultipleEngineTest_Copy, _T("Test cases for ensuring the GPU timestamp appears on the memory op engine"));
    //ADD_TEST_AND_DESC(_T("MultipleEngine/Compute"), CMultipleEngineTest_Compute, _T("Test cases for ensuring the GPU timestamp appears on the compute engine"));
    //ADD_TEST_AND_DESC(_T("PState"), CPStateTest, _T("Ensures that the timestamp is invariant during P-State transitions"));
    //ADD_TEST_AND_DESC(_T("CustomEvent"), CCustomEventTest, _T("Verifies that custom driver events are not logged when the marker mode is NONE"));
    //ADD_TEST_AND_DESC(_T("Frequency/Copy"), CFrequencyTest_Copy, _T("Verifies the required frequency of the GPU timestamp on the memory op engine"));
    //ADD_TEST_AND_DESC(_T("Frequency/Compute"), CFrequencyTest_Compute, _T("Verifies the required frequency of the GPU timestamp on the compute engine"));
    //ADD_TEST_AND_DESC(_T("Frequency/Video"), CFrequencyTest_Video, _T("Verifies the required frequency of the GPU timestamp on the video engine"));
    //ADD_TEST_AND_DESC(_T("Frequency/Draw"), CFrequencyTest_Draw, _T("Verifies the required frequency of the GPU timestamp on the rasterization engine"));
    ADD_TEST_AND_DESC(_T("Frequency/Teapot"), CFrequencyTest_Teapot, _T("Verifies the required frequency of the GPU timestamp on the rasterization engine"));
    //ADD_TEST_AND_DESC(_T("MarkerValidation/CommandList"), CMarkerValidationTest_CommandList, _T("Verifies DDI rules around the command list event"));
    //ADD_TEST_AND_DESC(_T("MarkerValidation/Optimized"), CMarkerValidationTest_Optimized, _T("Verifies DDI rules regarding optimized and dropped API calls"));
    //ADD_TEST_AND_DESC(_T("MarkerValidation/Flush"), CMarkerValidationTest_Flush, _T("Ensures that DDI rules around flushing the pipeline are honored"));
    //ADD_TEST_AND_DESC(_T("MarkerValidation/CommandBufferLimit"), CMarkerValidationTest_CommandBufferLimit, _T("Verifies that the driver doesn't pass command buffer limit in the event"));
    //ADD_TEST_AND_DESC(_T("MarkerValidation/Range"), CMarkerValidationTest_Range, _T("Ensures that GPU timestamps fall between the range defined by the begin and end timestamps of a DMA Buffer"));
    //ADD_TEST_AND_DESC(_T("MarkerValidation/APISequence"), CMarkerValidationTest_APISequence, _T("Ensures general DDI rules regarding APISequenceNumbers are honored"));
    return true;
}

void CTimingDataApp::InitOptionVariables()
{
    CD3D11TestFramework::InitOptionVariables();

}

////////////////////////////////////////////////////////////////////////////////
//
// CTimingDataTest
//
////////////////////////////////////////////////////////////////////////////////

CTimingDataTest::CTimingDataTest() :
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
    m_RTWidth(1),
    m_RTHeight(1),
    m_SRWidth(1),
    m_SRHeight(1),
    m_UAWidth(1),
    m_UAHeight(1),
    m_pTimingDataHelper(NULL),
    m_pXperfHelper(NULL),
    m_bTracing(false),
    m_ETL_file(ETL_FILE)
{
    DWORD pid = GetCurrentProcessId();
    memset(m_wszProcessOption, 0, sizeof(m_wszProcessOption));
    StringCchPrintfW(m_wszProcessOption, STR_SIZE, L"-process %d", pid);

}

TEST_RESULT CTimingDataTest::Setup()
{
    HRESULT hr = S_OK;

    TEST_RESULT tr = CD3D11Test::Setup();
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

    if(MarkerData.Profile == FALSE)
    {
        if(GetFramework()->GetTestMode() == WGFTEST_MODE_WHQL)
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
    m_width = m_RTWidth;
    m_height = m_RTHeight;
    m_display = false;   // do not show in a window
    m_pTimingDataHelper = new CTimingDataHelper(m_ETL_file);
    return RESULT_PASS;
}

void CTimingDataTest::CleanupTestCase()
{
    SAFE_DELETE(m_pTimingDataHelper);
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

void CTimingDataTest::SaveViewToBmp(ID3D11DeviceContext2 *m_pDeviceContext, ID3D11Texture2D *pStagingTex, ID3D11Texture2D *pRTTex, LPCTSTR bmpfile)
{
    D3D11_MAPPED_SUBRESOURCE mappedTexture = { 0 };
    m_pDeviceContext->CopyResource(pStagingTex, pRTTex);
    m_pDeviceContext->Map(pStagingTex, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_READ_WRITE, 0, &mappedTexture);
    //
    // Convert RGBA to BGR, invert y
    //
    int padding = m_width % 4;
    long s = m_height * (m_width * 3 + padding);
    BYTE* dest = new BYTE[s];
    BYTE *pDest = dest;
    for (int y = m_height - 1; y >= 0; y--)
    {
        BYTE *pSrc = y * m_width * 4 + (UCHAR*)mappedTexture.pData;  //y inversion
        for (UINT x = 0; x < m_width; x++)
        {
            *pDest++ = pSrc[2];
            *pDest++ = pSrc[1];
            *pDest++ = pSrc[0];
            pSrc += 4;
        }
        for (int x = 0; x < padding; x++)
        {
            *pDest++ = '\0';
        }
    }
    SaveBMP(dest, m_width, m_height, s, bmpfile);
    delete[] dest;
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
            bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits
            ok = WriteFile(file, &bmfh, sizeof (BITMAPFILEHEADER), &bwritten, NULL);   // write file header
        }
        if (ok)
        {
            BITMAPINFOHEADER info = {};
            info.biSize = sizeof(BITMAPINFOHEADER);
            info.biWidth = width;
            info.biHeight = height;
            info.biPlanes = 1;			// we only have one bitplane
            info.biBitCount = 24;		// RGB mode is 24 bits
            info.biCompression = BI_RGB;
            info.biSizeImage = 0;		// can be 0 for 24 bit images
            info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
            info.biYPelsPerMeter = 0x0ec4;
            info.biClrUsed = 0;			// we are in RGB mode and have no palette
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
    vp.Width = (float)m_width;
    vp.Height = (float)m_height;
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

    if (FAILED(hr = m_pDevice->CreateRasterizerState(&rastDesc, &m_pRS)))
    {
        LogError(__FILEW__, __LINE__, L"CreateRasterizerState() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateRasterizerState);
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

    if (FAILED(hr = m_pDevice->CreateBuffer(&bufferDesc, &data, &m_pVB)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create vertex buffer", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource);
        goto Fail;
    }

    bufferDesc.ByteWidth = sizeof(int);
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    const UINT32 indices[] = { 0, 1, 2, 3 };

    data.pSysMem = (void*)indices;
    data.SysMemPitch = sizeof(UINT32)* 4;

    if (FAILED(hr = m_pDevice->CreateBuffer(&bufferDesc, &data, &m_pIB)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create index buffer", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource);
        goto Fail;
    }

    // compile VS
    if (FAILED(hr = GetFramework()->CompileShaderFromMemory(
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
            char* szError = new char[bufSize + 1];
            memcpy(szError, pBuf, bufSize);
            szError[bufSize] = 0;
            WriteToLog(_T("%s\n"), szError);
            delete[] szError;
        }

        goto Fail;
    }

    if (FAILED(hr = m_pDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pVS)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create vertex shader", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader);
        goto Fail;
    }

    // input layout
    if (FAILED(hr = m_pDevice->CreateInputLayout(
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
    if (FAILED(hr = GetFramework()->CompileShaderFromMemory(
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
            char* szError = new char[bufSize + 1];
            memcpy(szError, pBuf, bufSize);
            szError[bufSize] = 0;
            WriteToLog(_T("%s\n"), szError);
            delete[] szError;
        }

        goto Fail;
    }

    if (FAILED(hr = m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pPS_0)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create pixel shader", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader);
        goto Fail;
    }

    SAFE_RELEASE(pShaderBlob);
    SAFE_RELEASE(pErrorBlob);

    // compile alt PS
    if (FAILED(hr = GetFramework()->CompileShaderFromMemory(
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
            char* szError = new char[bufSize + 1];
            memcpy(szError, pBuf, bufSize);
            szError[bufSize] = 0;
            WriteToLog(_T("%s\n"), szError);
            delete[] szError;
        }

        goto Fail;
    }

    if (FAILED(hr = m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pPS_1)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create alternate pixel shader", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader);
        goto Fail;
    }
    if (GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_0)
    {

        // compile CS
        if (FAILED(hr = GetFramework()->CompileShaderFromMemory(
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
                char* szError = new char[bufSize + 1];
                memcpy(szError, pBuf, bufSize);
                szError[bufSize] = 0;
                WriteToLog(_T("%s\n"), szError);
                delete[] szError;
            }

            goto Fail;
        }

        if (FAILED(hr = m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pCS_0)))
        {
            LogError(__FILEW__, __LINE__, L"Failed to create compute shader", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader);
            goto Fail;
        }

        // compile alt CS
        if (FAILED(hr = GetFramework()->CompileShaderFromMemory(
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
                char* szError = new char[bufSize + 1];
                memcpy(szError, pBuf, bufSize);
                szError[bufSize] = 0;
                WriteToLog(_T("%s\n"), szError);
                delete[] szError;
            }

            goto Fail;
        }

        if (FAILED(hr = m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pCS_1)))
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

    UINT32 *sourceData = new UINT32[m_SRWidth * m_SRHeight];
    for (UINT i = 0; i < m_SRWidth * m_SRHeight; ++i)
    {
        sourceData[i] = (rand() % 0xffffffff) | 0x000000ff;
    }

    data.pSysMem = (void*)sourceData;
    data.SysMemPitch = sizeof(UINT32)* m_SRWidth;
    data.SysMemSlicePitch = 0;

    if (FAILED(hr = m_pDevice->CreateTexture2D(&tex2DDesc, &data, &m_pSRTex)))
    {
        LogError(__FILEW__, __LINE__, L"Failed to create texture for shader resource", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource);
        delete[] sourceData;
        return RESULT_FAIL;
    }

    delete[] sourceData;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    if (FAILED(hr = m_pDevice->CreateShaderResourceView(m_pSRTex, &srvDesc, &m_pSRView)))
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

    m_pPS = m_pPS_0;
    m_pCS = m_pCS_0;

    m_pDeviceContext->IASetPrimitiveTopology(m_topology);
    m_pDeviceContext->IASetInputLayout(m_pInputLayout);
    m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &m_stride, &m_offset);
    m_pDeviceContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
    m_pDeviceContext->RSSetState(m_pRS);
    m_pDeviceContext->VSSetShader(m_pVS, NULL, 0);
    m_pDeviceContext->PSSetShader(m_pPS, NULL, 0);
    m_pDeviceContext->PSSetShaderResources(0, 1, &m_pSRView);
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRTView, NULL);

    if (FAILED(hr = ExecuteEffectiveContext()))
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
}

void CTimingDataTest::Cleanup()
{
    CD3D11Test::Cleanup();

    SAFE_DELETE(m_pXperfHelper);
}

TEST_RESULT CTimingDataTest::EnableETWTracing()
{
    HRESULT hr;

    if(m_bTracing == true)
    {
        return RESULT_PASS;
    }

    m_pXperfHelper = new XperfHelper(NORMAL, true, LARGE, FALSE);
    m_pXperfHelper->SetMergedFile(m_ETL_file);

    DxkernelSession* pDxkernelSession;
    OtherSession* pOtherSession;
    
    int bufferSize = 0;
    int minBuffers = 0;
    int maxBuffers = 0;

    m_pXperfHelper->GetDxkernelSessionBuffers(&bufferSize, &minBuffers, &maxBuffers);
    pDxkernelSession = new DxkernelSession(NORMAL, true, bufferSize, minBuffers, maxBuffers, true);
    pDxkernelSession->SetSessionProviders(L"802ec45a-1e99-4b83-9920-87c98277ba9d:0xffff:5");
    m_pXperfHelper->SetDxkernelSession(pDxkernelSession);

    pOtherSession = new OtherSession(NORMAL, true, 1024, 1024, 2048);
    pOtherSession->SetSessionProviders(L"db6f6ddb-ac77-4e88-8253-819df9bbf140:0xd000000000000fff:6");
    m_pXperfHelper->SetOtherSession(pOtherSession);

    CProvider etwProvider;
    hr = m_pXperfHelper->Start();
    if (hr == S_OK)
    {
        m_bTracing = true;
        return RESULT_PASS;
    }
    else
    {
        return RESULT_FAIL;
    }
}

TEST_RESULT CTimingDataTest::StartETWTracing()
{
    TEST_RESULT tr = EnableETWTracing();
    if (tr != RESULT_PASS)
    {
        throw TRException(_T("EnableETWTracing() failed."));
    }
    m_pDeviceContext->Flush();

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
    vector<CEvent_MarkerInt*> *pMarkerEvents = m_pTimingDataHelper->GatherMarkerIntList(RESULTS_FILE, m_wszProcessOption);
    if(pMarkerEvents == nullptr || pMarkerEvents->size() == 0)
    {
        WriteToLog(_T("WARNING: Did not find marker begin event"));
        return RESULT_PASS;
    }

    if((*pMarkerEvents)[0]->Label() != MARKER_NAME_START || (*pMarkerEvents)[0]->Data() != MARKER_UID)
    {
        WriteToLog(_T("WARNING: Marker begin event doesn't match expected label and id"));
        return RESULT_PASS;
    }

    *pMarkerBegin = (*pMarkerEvents)[0]->EventTime();	

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
    vector<CEvent_HistoryBuffer*> *pAllEvents = m_pTimingDataHelper->GatherHistoryBufferList(RESULTS_FILE, L"");
    if(pAllEvents == nullptr || pAllEvents->size() == 0)
    {
        return tr;
    }

    for(UINT i = 0; i < pAllEvents->size(); ++i)
    {
		if (fCheckOrder && (*pAllEvents)[i]->HistoryBuffer().size() >= 2)
        {
            const UINT64 MaxWRoll = 1000000;
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
//				fCheckOrder = false;
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
//					fCheckOrder = false;
					break;
				}
                diff = Last - Next;
                if (diff > MaxWRoll)
                {
					LogError(__FILEW__, __LINE__, L"History Buffer timestamp order violation, Completed Timestamps must not exceed Last timestamp, accounting for rollover", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
                    WriteToLog("@%llu, Next:%lld, Last:%lld\n", (*pAllEvents)[i]->EventTime(), Next, Last);
                    tr = RESULT_FAIL;
//					fCheckOrder = false;
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
    vector<CEvent_CalibrateGpuClock*> *pAllEvents = m_pTimingDataHelper->GatherCalibrateGpuClockList(RESULTS_FILE, L"");
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
    vector<CEvent_CommandBufferSubmission*> *pAllEvents = m_pTimingDataHelper->GatherCommandBufferSubmissionList(RESULTS_FILE, m_wszProcessOption);
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
    vector<CEvent_RuntimeDroppedCall*> *pAllEvents = m_pTimingDataHelper->GatherRuntimeDroppedCallList(RESULTS_FILE, m_wszProcessOption);
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
    vector<CEvent_CommandList*> *pAllEvents = m_pTimingDataHelper->GatherCommandListList(RESULTS_FILE, m_wszProcessOption);
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
                if (pFilteredEvents != nullptr)
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
        //was unreachable
        //if(itrCmdBuffer != apiSequenceNumbers.end())
        //{
        //	apiSequenceNumbers.erase(itrCmdBuffer);
        //}
        
        itrHist++;
    }

    return RESULT_PASS;
}