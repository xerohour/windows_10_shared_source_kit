//////////////////////////////////////////////////////////////////////////
//  WGFCompute.cpp
//  created:	2009/03/23
//
//   Base class and the application class for WGFCompute test
//////////////////////////////////////////////////////////////////////////

// Includes
#include "WGFCompute.h"
#include "UAV.h"
#include "MemoryBarrierSync.h"
#include "GroupSharedMemory.h"
#include "Atomicity.h"
#include "Dispatch.h"
#include "AppendConsume.h"
#include "CounterUAV.h"

// Globals
CComputeTestApp  g_TestApp;

BEGIN_NAMED_VALUES(SHARED_MEMORY_TYPE)
    NAMED_VALUE("TGSM", SHARED_MEMORY_TGSM)
    NAMED_VALUE("UAV", SHARED_MEMORY_UAV)
END_NAMED_VALUES(SHARED_MEMORY_TYPE)

///////////////////////////////////////////////////////////////////////////////
//	Base class and the application class for WGFCompute test
///////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------------
// CComputeTest
//------------------------------------------------------------------------------------------------

TEST_RESULT CComputeTest::Setup()
{
    TEST_RESULT tRes = RESULT_PASS;

    SAFE_RELEASE(m_pCS);

    m_pDevice = g_TestApp.GetDevice();
    m_pEffectiveContext = g_TestApp.GetEffectiveContext();

    if (g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0)
    {
        D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwOptions;
        ZeroStructMem(hwOptions);

        HRESULT hr = m_pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwOptions, sizeof(hwOptions) );
        if (FAILED(hr))
        {
            WriteToLog( _T("CheckFeatureSupport(D3D10_X_HARDWARE_OPTIONS) failed, hr=%s"), D3DHResultToString(hr).c_str() );
            tRes = RESULT_FAIL;
        }
        else
        {
            if (!hwOptions.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
            {
                WriteToLog( _T("Skipping, as device does not support SM 4.x compute features") );
                tRes = RESULT_SKIP;
            }
        }
    }

    return tRes;
}

//------------------------------------------------------------------------------------------------
void CComputeTest::Cleanup()
{
    SAFE_RELEASE(m_pCS);

    m_pDevice = NULL;
    m_pEffectiveContext = NULL;
}

TEST_RESULT CComputeTest::SetupTestCase()
{
    HRESULT hr;
    TEST_RESULT tRes = RESULT_PASS;

    switch( m_TargetUAVDimension )
    {
        case D3D11_UAV_DIMENSION_BUFFER:
            {
                D3D11_BUFFER_DESC desc;
                ZeroStructMem(desc);

                desc.ByteWidth = m_TargetUAVElementByteStride * m_TargetUAVSize[0];
                desc.Usage = D3D11_USAGE_DEFAULT;
                desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
                desc.CPUAccessFlags = 0;
                desc.MiscFlags = m_TargetUAVMiscFlags;
                desc.StructureByteStride = m_TargetUAVElementByteStride;

                ID3D11Buffer* pBuffer(NULL);
                hr = GetFramework()->CreateBufferTiledOptional( &desc, NULL, &pBuffer );
                if( FAILED(hr) || pBuffer == NULL )
                {
                    WriteToLog( _T("CreateBuffer(target) failed, hr=%s"), D3DHResultToString(hr).c_str() );
                    return RESULT_FAIL;
                }

                SAFE_RELEASE(m_pTargetResource);
                m_pTargetResource = pBuffer;

                {
                    CD3D11_UNORDERED_ACCESS_VIEW_DESC const uavDesc(
                        pBuffer,
                        m_TargetUAVFormat,
                        0,
                        m_TargetUAVSize[0],
                        m_TargetUAVBufferFlag
                    );

                    SAFE_RELEASE(m_pTargetUAV);
                    hr = m_pDevice->CreateUnorderedAccessView( m_pTargetResource, &uavDesc, &m_pTargetUAV );
                    if (FAILED(hr) || NULL == m_pTargetUAV)
                    {
                        WriteToLog( "CreateUnorderedAccessView(Buffer) failed. hr=%s", D3DHResultToString(hr).c_str() );
                        return RESULT_FAIL;
                    }
                }

                pBuffer = NULL;
                desc.Usage = D3D11_USAGE_STAGING;
                desc.BindFlags = 0;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                desc.MiscFlags = 0;
                hr = GetFramework()->CreateBufferTiledOptional( &desc, NULL, &pBuffer );
                if( FAILED(hr) || pBuffer == NULL )
                {
                    WriteToLog( _T("CreateBuffer(target copy) failed, hr=%s"), D3DHResultToString(hr).c_str() );
                    return RESULT_FAIL;
                }

                SAFE_RELEASE(m_pTargetResourceCopy);
                m_pTargetResourceCopy = pBuffer;
            }
            break;
        case D3D11_UAV_DIMENSION_TEXTURE1D:
        case D3D11_UAV_DIMENSION_TEXTURE1DARRAY:
            {
                UINT const arraySize = m_TargetUAVDimension == D3D11_UAV_DIMENSION_TEXTURE1DARRAY ? m_TargetUAVArraySize : 1;

                D3D11_TEXTURE1D_DESC desc1d;
                ZeroStructMem(desc1d);
                desc1d.Width = m_TargetUAVSize[0];
                desc1d.MipLevels = 1;
                desc1d.ArraySize = arraySize;
                desc1d.Format = m_TargetUAVFormat;
                desc1d.Usage = D3D11_USAGE_DEFAULT;
                desc1d.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
                desc1d.CPUAccessFlags = 0;
                desc1d.MiscFlags = 0;

                ID3D11Texture1D* pTarget1D(NULL);
                hr = GetDevice()->CreateTexture1D( &desc1d, NULL, &pTarget1D );
                if( FAILED(hr) || pTarget1D == NULL )
                {
                    WriteToLog( _T("CreateTexture1D(target) failed, hr=%s"), D3DHResultToString(hr).c_str() );
                    return RESULT_FAIL;
                }

                SAFE_RELEASE(m_pTargetResource);
                m_pTargetResource = pTarget1D;

                {
                    CD3D11_UNORDERED_ACCESS_VIEW_DESC const uavDesc(
                        pTarget1D,
                        m_TargetUAVDimension,
                        m_TargetUAVFormat,
                        0,
                        0,
                        arraySize
                    );

                    SAFE_RELEASE(m_pTargetUAV);
                    hr = m_pDevice->CreateUnorderedAccessView( m_pTargetResource, &uavDesc, &m_pTargetUAV );
                    if (FAILED(hr) || NULL == m_pTargetUAV)
                    {
                        WriteToLog( "CreateUnorderedAccessView(1DArray) failed. hr=%s", D3DHResultToString(hr).c_str() );
                        return RESULT_FAIL;
                    }
                }

                pTarget1D = NULL;
                desc1d.Usage = D3D11_USAGE_STAGING;
                desc1d.BindFlags = 0;
                desc1d.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                hr = GetDevice()->CreateTexture1D( &desc1d, NULL, &pTarget1D );
                if( FAILED(hr) || pTarget1D == NULL )
                {
                    WriteToLog( _T("CreateTexture1D(target copy) failed, hr=%s"), D3DHResultToString(hr).c_str() );
                    return RESULT_FAIL;
                }

                SAFE_RELEASE(m_pTargetResourceCopy);
                m_pTargetResourceCopy = pTarget1D;
            }
            break;
        case D3D11_UAV_DIMENSION_TEXTURE2D:
        case D3D11_UAV_DIMENSION_TEXTURE2DARRAY:
            {
                UINT const arraySize = m_TargetUAVDimension == D3D11_UAV_DIMENSION_TEXTURE2DARRAY ? m_TargetUAVArraySize : 1;

                ID3D11Texture2D *pTarget2D = NULL;
                D3D11_TEXTURE2D_DESC desc2d;
                ZeroStructMem(desc2d);
                desc2d.Width = m_TargetUAVSize[0];
                desc2d.Height = m_TargetUAVSize[1];
                desc2d.ArraySize = arraySize;
                desc2d.MipLevels = 1;
                desc2d.Format = m_TargetUAVFormat;
                desc2d.Usage = D3D11_USAGE_DEFAULT;
                desc2d.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
                desc2d.CPUAccessFlags = 0;
                desc2d.MiscFlags = 0;
                desc2d.SampleDesc.Count = 1;
                desc2d.SampleDesc.Quality = 0;
                hr = GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional( &desc2d, NULL, &pTarget2D);
                if (FAILED(hr) || NULL == pTarget2D)
                {
                    WriteToLog( "CreateTexture2D(target) failed. hr=%s", D3DHResultToString(hr).c_str() );
                    return RESULT_FAIL;
                }
                else
                {
                    SAFE_RELEASE(m_pTargetResource);
                    m_pTargetResource = pTarget2D;

                    CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc( 
                        pTarget2D, 
                        m_TargetUAVDimension,
                        m_TargetUAVFormat,
                        0,
                        0,
                        arraySize
                    );
                    SAFE_RELEASE(m_pTargetUAV);
                    hr = m_pDevice->CreateUnorderedAccessView( m_pTargetResource, &uavDesc, &m_pTargetUAV );
                    if (FAILED(hr) || NULL == m_pTargetUAV)
                    {
                        WriteToLog( "CreateUnorderedAccessView(2DArray) failed. hr=%s", D3DHResultToString(hr).c_str() );
                        return RESULT_FAIL;
                    }

                    pTarget2D = NULL;
                    desc2d.Usage = D3D11_USAGE_STAGING;
                    desc2d.BindFlags = 0;
                    desc2d.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                    hr = GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional( &desc2d, NULL, &pTarget2D);
                    if (FAILED(hr) || NULL == pTarget2D)
                    {
                        WriteToLog( "CreateTexture2D(target copy) failed. hr=%s", D3DHResultToString(hr).c_str() );
                        return RESULT_FAIL;
                    }

                    SAFE_RELEASE(m_pTargetResourceCopy);
                    m_pTargetResourceCopy = pTarget2D;
                }
            }
            break;
        case D3D11_UAV_DIMENSION_TEXTURE3D:
            {
                ID3D11Texture3D *pTarget3D = NULL;
                D3D11_TEXTURE3D_DESC desc3d;
                ZeroStructMem(desc3d);
                desc3d.Width = m_TargetUAVSize[0];
                desc3d.Height = m_TargetUAVSize[1];
                desc3d.Depth = m_TargetUAVSize[2];
                desc3d.MipLevels = 1;
                desc3d.Format = m_TargetUAVFormat;
                desc3d.Usage = D3D11_USAGE_DEFAULT;
                desc3d.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
                desc3d.CPUAccessFlags = 0;
                desc3d.MiscFlags = 0;
                hr = GetFramework()->CreateTexture3DMapDefaultOptional( &desc3d, NULL, &pTarget3D);
                if (FAILED(hr) || NULL == pTarget3D)
                {
                    WriteToLog( "CreateTexture3D failed. hr=%s", D3DHResultToString(hr).c_str() );
                    return RESULT_FAIL;
                }
                else
                {
                    SAFE_RELEASE(m_pTargetResource);
                    m_pTargetResource = pTarget3D;

                    CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc(pTarget3D);
                    SAFE_RELEASE(m_pTargetUAV);
                    hr = m_pDevice->CreateUnorderedAccessView( m_pTargetResource, &uavDesc, &m_pTargetUAV );
                    if (FAILED(hr) || NULL == m_pTargetUAV)
                    {
                        WriteToLog( "CreateUnorderedAccessView(3D) failed. hr=%s", D3DHResultToString(hr).c_str() );
                        return RESULT_FAIL;
                    }


                    pTarget3D = NULL;
                    desc3d.Usage = D3D11_USAGE_STAGING;
                    desc3d.BindFlags = 0;
                    desc3d.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                    hr = GetFramework()->CreateTexture3DMapDefaultOptional( &desc3d, NULL, &pTarget3D);
                    if (FAILED(hr) || NULL == pTarget3D)
                    {
                        WriteToLog( "CreateTexture3D(target copy) failed. hr=%s", D3DHResultToString(hr).c_str() );
                        return RESULT_FAIL;
                    }

                    SAFE_RELEASE(m_pTargetResourceCopy);
                    m_pTargetResourceCopy = pTarget3D;
                }
            }
            break;
        default:
            {
                tstring const name = ToString( m_TargetUAVDimension );
                WriteToLog( _T("Unknown target resource type: %s"), name.c_str() );
                return RESULT_FAIL;
            }
    }
    return RESULT_PASS;
}

bool CComputeTest::SkipSlowOnRefWithMessage( bool isSlow)
{
	const bool skipForRef = ( g_TestApp.m_D3DOptions.DriverType == D3D_DRIVER_TYPE_REFERENCE )
		&& (!g_TestApp.m_bRunSlowOnRef);
    if ( isSlow
        && ( skipForRef  
		|| g_TestApp.m_bSkipSlowOnHardware )
        && ( g_TestApp.GetCurrentTestCase() > 0 ) )
    {
		if( g_TestApp.m_bSkipSlowOnHardware )
		{
			WriteToLog(_T("Test case forced to skip by cmd flag SkipSlowTestCasesOnHardware"));
		}
		else
		{
			WriteToLog(_T("Test cases running very slow on REF are skipped for REF"));
		}
        return true;
    }
    else
        return false;
}

//------------------------------------------------------------------------------------------------

void CComputeTest::CleanupTestCase()
{
    SAFE_RELEASE(m_pTargetResource);
    SAFE_RELEASE(m_pTargetResourceCopy);
    SAFE_RELEASE(m_pTargetUAV);
}


//------------------------------------------------------------------------------------------------
HRESULT CComputeTest::CompileAndCreateComputeShaderFromMemory(
    LPCSTR shaderStr, size_t shaderLen, LPCSTR entryPt, 
    D3D_FEATURE_LEVEL minFL, 
    ID3D11ComputeShader** ppCS, 
    CONST D3D10_SHADER_MACRO *pDefines)
{
    HRESULT hr;
    ID3D10Blob* pShaderBlob = NULL;
    ID3D10Blob* pErrorBlob = NULL;
    ID3D10Blob* pShaderText = NULL;

    hr = g_TestApp.CompileShaderFromMemory(shaderStr, shaderLen, entryPt, 
        D3D_SHADER_STAGE_COMPUTE, minFL, D3D_FEATURE_LEVEL_11_0, 0,
        &pShaderBlob, &pErrorBlob,
        pDefines, &pShaderText);
    if (FAILED(hr) || g_TestApp.m_bLogShaderText)
    {
        if (pShaderText != NULL)
            WriteToLog( "Shader code: \n%s", pShaderText->GetBufferPointer() );
        else
            WriteToLog( "Shader code: \n%s", shaderStr );
    }
    if (FAILED(hr) || NULL == pShaderBlob)
    {
        TEST_RESULT tRes = RESULT_FAIL;
        WriteToLog( "CompileShaderFromMemory failed, hr=%s", D3DHResultToString(hr).c_str() );
        if (pErrorBlob)
        {
            WriteToLog( "Compiler errors: %s", pErrorBlob->GetBufferPointer() );
        }
        goto createDone;
    }

    SAFE_RELEASE((*ppCS));
    hr = m_pDevice->CreateComputeShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, ppCS );
    if (FAILED(hr) || NULL == *ppCS)
    {
        TEST_RESULT tRes = RESULT_FAIL;
        WriteToLog( "CreateComputeShader failed, hr=%s", D3DHResultToString(hr).c_str() );
        goto createDone;
    }

createDone:
    SAFE_RELEASE(pShaderBlob);
    SAFE_RELEASE(pErrorBlob);
    SAFE_RELEASE(pShaderText);

    return hr;
}

//------------------------------------------------------------------------------------------------
HRESULT CComputeTest::CompileAndCreateComputeShaderFromResource(
    LPCSTR resourceStr, LPCSTR entryPt, D3D_FEATURE_LEVEL minFL, 
    ID3D11ComputeShader** ppCS, CONST D3D10_SHADER_MACRO *pDefines )
{
    HRESULT hr;
    ID3D10Blob* pShaderBlob = NULL;
    ID3D10Blob* pErrorBlob = NULL;
    ID3D10Blob* pShaderText = NULL;

    hr = g_TestApp.CompileShaderFromResource( NULL, resourceStr, entryPt, 
        D3D_SHADER_STAGE_COMPUTE, minFL, D3D_FEATURE_LEVEL_11_0, 0,
        &pShaderBlob, &pErrorBlob,
        pDefines, &pShaderText);
    if (FAILED(hr) || g_TestApp.m_bLogShaderText)
    {
        if (pShaderText != NULL)
            WriteToLog( "Shader code: \n%s", pShaderText->GetBufferPointer() );
        else
            WriteToLog( "Shader code: \n%s", resourceStr );
    }
    if (FAILED(hr) || NULL == pShaderBlob)
    {
        TEST_RESULT tRes = RESULT_FAIL;
        WriteToLog( "CompileShaderFromResource failed, hr=%s", 
            D3DHResultToString(hr).c_str() );
        if (pErrorBlob)
        {
            WriteToLog( "Compiler errors: %s", pErrorBlob->GetBufferPointer() );
        }
        goto createDone;
    }

    SAFE_RELEASE((*ppCS));
    hr = m_pDevice->CreateComputeShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, ppCS );
    if (FAILED(hr) || NULL == *ppCS)
    {
        TEST_RESULT tRes = RESULT_FAIL;
        WriteToLog( "CreateComputeShader failed, hr=%s", D3DHResultToString(hr).c_str() );
        goto createDone;
    }

createDone:
    SAFE_RELEASE(pShaderBlob);
    SAFE_RELEASE(pErrorBlob);
    SAFE_RELEASE(pShaderText);

    return hr;
}

//------------------------------------------------------------------------------------------------
HRESULT CComputeTest::CompileAndCreatePixelShaderFromResource(
    LPCSTR resourceStr, LPCSTR entryPt, D3D_FEATURE_LEVEL minFL, 
    ID3D11PixelShader** ppPS, CONST D3D10_SHADER_MACRO *pDefines )
{
    HRESULT hr;
    ID3D10Blob* pShaderBlob = NULL;
    ID3D10Blob* pErrorBlob = NULL;
    ID3D10Blob* pShaderText = NULL;

    hr = g_TestApp.CompileShaderFromResource( NULL, resourceStr, entryPt, 
        D3D_SHADER_STAGE_PIXEL, minFL, D3D_FEATURE_LEVEL_11_0, 0,
        &pShaderBlob, &pErrorBlob,
        pDefines, &pShaderText);
    if (FAILED(hr) || g_TestApp.m_bLogShaderText)
    {
        if (pShaderText != NULL)
            WriteToLog( "Shader code: \n%s", pShaderText->GetBufferPointer() );
        else
            WriteToLog( "Shader code: \n%s", resourceStr );
    }
    if (FAILED(hr) || NULL == pShaderBlob)
    {
        TEST_RESULT tRes = RESULT_FAIL;
        WriteToLog( "CompileShaderFromResource failed, hr=%s", 
            D3DHResultToString(hr).c_str() );
        if (pErrorBlob)
        {
            WriteToLog( "Compiler errors: %s", pErrorBlob->GetBufferPointer() );
        }
        goto createDone;
    }

    SAFE_RELEASE((*ppPS));
    hr = m_pDevice->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, ppPS );
    if (FAILED(hr) || NULL == *ppPS)
    {
        TEST_RESULT tRes = RESULT_FAIL;
        WriteToLog( "CreatePixelShader failed, hr=%s", D3DHResultToString(hr).c_str() );
        goto createDone;
    }

createDone:
    SAFE_RELEASE(pShaderBlob);
    SAFE_RELEASE(pErrorBlob);
    SAFE_RELEASE(pShaderText);

    return hr;
}

//------------------------------------------------------------------------------------------------
// common code dealing with vertex shader for test groups of Pixel shader 
const char g_VSPassthrough[] = 
    "float4 main( float4 input : POSITION ): SV_POSITION \n"
    "{\n"
    "	return input; \n"
    "}\n"
    ;
TEST_RESULT CComputeTest::SetupVSStage()
{
    // same VS and inputlayout for all test cases in this group
    // Create passthrough VS
    TEST_RESULT tr = RESULT_PASS;
    ID3D10Blob *pVertexShaderBlob = NULL;
    ID3D10Blob *pVertexErrorBlob = NULL;

    HRESULT hr = D3DX11CompileFromMemory( 
        g_VSPassthrough, 
        strlen( g_VSPassthrough ), 
        NULL, 
        NULL, 
        NULL, 
        "main", 
        "vs_5_0", 
        0, 
        0, 
        NULL, 
        &pVertexShaderBlob, 
        &pVertexErrorBlob, 
        NULL 
    );

    if ( FAILED(hr) )
    {
        tr = RESULT_FAIL;
        WriteToLog( _T( "Compiler errors:"));
        if (pVertexErrorBlob)
        {
            LPVOID pBuf = pVertexErrorBlob->GetBufferPointer();
            size_t bufSize = pVertexErrorBlob->GetBufferSize();
            char* szError = new char[bufSize+1];
            memcpy(szError, pBuf, bufSize);
            szError[bufSize] = 0;
            WriteToLog( _T( "%s\n" ), szError );
            delete[] szError;
        }
        goto Done;
    }
    hr = GetDevice()->CreateVertexShader( 
        pVertexShaderBlob->GetBufferPointer(), 
        pVertexShaderBlob->GetBufferSize(), 
        NULL, 
        &m_pVertexShader
    );

    if( FAILED( hr ) )
    {
        WriteToLog( _T( "CMemoryBarrierPSTest::SetupVSStage() - Failed to create vertex shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
        tr = RESULT_FAIL;
        goto Done;
    }
    
    // create inputlayout
    D3D11_INPUT_ELEMENT_DESC pIEDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = GetDevice()->CreateInputLayout( 
        pIEDesc, 
        1, 
        pVertexShaderBlob->GetBufferPointer(), 
        pVertexShaderBlob->GetBufferSize(), 
        &m_pInputLayout );

    if( FAILED( hr ) )
    {
        WriteToLog( _T( "CMemoryBarrierPSTest::SetupVSStage() - CreateInputLayout() failed" ) );
        tr = RESULT_FAIL;
        goto Done;
    }


    // Create Vertex buffers
    float vertices[6][3];

    vertices[0][0] = -1.0f;	vertices[0][1] = -1.0f;	vertices[0][2] = 0.5f;
    vertices[1][0] = -1.0f;	vertices[1][1] = 1.0f;	vertices[1][2] = 0.5f;
    vertices[2][0] = 1.0f;	vertices[2][1] = -1.0f;	vertices[2][2] = 0.5f;
    vertices[3][0] = 1.0f;	vertices[3][1] = -1.0f;	vertices[3][2] = 0.5f;
    vertices[4][0] = -1.0f;	vertices[4][1] = 1.0f;	vertices[4][2] = 0.5f;
    vertices[5][0] = 1.0f;	vertices[5][1] = 1.0f;	vertices[5][2] = 0.5f;

    D3D11_BUFFER_DESC bd;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( float ) * 6 * 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = vertices;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;
    hr = GetFramework()->CreateBufferTiledOptional( &bd, &initData, &m_pVertexBuffer );
    if( FAILED( hr ) )
    {
        WriteToLog( _T( "CMemoryBarrierPSTest::SetupVSStage() - CreateBuffer failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
        tr = RESULT_FAIL;
        goto Done;
    }

Done:
    SAFE_RELEASE( pVertexShaderBlob );
    SAFE_RELEASE( pVertexErrorBlob );
    return tr;
}

// create a dummy rendertarget for pixel shader stage
TEST_RESULT CComputeTest::SetupRTV(UINT rtWidth, UINT rtHeight)
{
	D3D11_TEXTURE2D_DESC tex2dDesc;
	tex2dDesc.Width = rtWidth;
	tex2dDesc.Height = rtHeight;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.Format = DXGI_FORMAT_R32_FLOAT;
	tex2dDesc.SampleDesc.Count = 1;
	tex2dDesc.SampleDesc.Quality = 0;
    tex2dDesc.Usage = D3D11_USAGE_DEFAULT;
    tex2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
    tex2dDesc.CPUAccessFlags = 0;
    tex2dDesc.MiscFlags = 0;

	
	HRESULT hr = GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional( &tex2dDesc, NULL, &m_pRTTex2D );
	if (FAILED(hr) || NULL == m_pRTTex2D)
	{
		WriteToLog( "CComputeTest::SetupRTV: CreateTexture2D() failed for rendertarget texture2D. hr=%s", 
			D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}
	else
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtView;
		rtView.Format = DXGI_FORMAT_R32_FLOAT;
		rtView.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtView.Texture2D.MipSlice = 0;
						
		hr = m_pDevice->CreateRenderTargetView( m_pRTTex2D, &rtView, &m_pRTView );
		if (FAILED(hr) || NULL == m_pRTView)
		{
			WriteToLog( "CComputeTest::SetupRTV: CreateRenderTargetView failed. hr=%s", 
				D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}
	return RESULT_PASS;
}


//------------------------------------------------------------------------------------------------
// CComputeTestApp
//------------------------------------------------------------------------------------------------

bool CComputeTestApp::InitTestGroups()
{
    // Register test classes with the group manager.
    if( !ADD_TEST( _T( "Memory\\Sync" ), CMemorySyncTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Barrier\\CS" ), CMemoryBarrierTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Barrier\\PS" ), CMemoryBarrierPSTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\Add\\CS" ), CAtomicAddTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\Min\\CS" ), CAtomicMinTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\Max\\CS" ), CAtomicMaxTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\And\\CS" ), CAtomicAndTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\Or\\CS" ), CAtomicOrTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\Xor\\CS" ), CAtomicXorTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\Exchange\\CS" ), CAtomicExchangeTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\CompareExchange\\CS" ), CAtomicCompareExchangeTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\CompareStore\\CS" ), CAtomicCompareStoreTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\MultipleIntrinsics\\CompareExchange\\CS" ), CMultiAtomicCompareExchangeTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\MultipleIntrinsics\\CompareStore\\CS" ), CMultiAtomicCompareStoreTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\MultipleIntrinsics\\AllOthers\\CS" ), CAtomicMultipleIntrinsicTest ) ) return false;	
    if( !ADD_TEST( _T( "Memory\\Atomic\\Add\\PS" ), CAtomicAddPSTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\Min\\PS" ), CAtomicMinPSTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\Max\\PS" ), CAtomicMaxPSTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\And\\PS" ), CAtomicAndPSTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\Or\\PS" ), CAtomicOrPSTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\Xor\\PS" ), CAtomicXorPSTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\Exchange\\PS" ), CAtomicExchangePSTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\CompareExchange\\PS" ), CAtomicCompareExchangePSTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\CompareStore\\PS" ), CAtomicCompareStorePSTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\MultipleIntrinsics\\CompareExchange\\PS" ), CMultiAtomicCompareExchangePSTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\MultipleIntrinsics\\CompareStore\\PS" ), CMultiAtomicCompareStorePSTest ) ) return false;
    if( !ADD_TEST( _T( "Memory\\Atomic\\MultipleIntrinsics\\AllOthers\\PS" ), CAtomicMultipleIntrinsicPSTest ) ) return false;	
    if( !ADD_TEST( _T( "GroupSharedMemory\\FL10" ), CTGSM10Test ) ) return false;
    if( !ADD_TEST( _T( "GroupSharedMemory\\FL11" ), CTGSM11Test ) ) return false;
    if( !ADD_TEST( _T("UAV\\Buffer\\RW\\CS"), CUAVBufferRWCSTest ) ) return false;
    if( !ADD_TEST( _T("UAV\\Buffer\\RW\\PS"), CUAVBufferRWPSTest ) ) return false;
    if( !ADD_TEST( _T("UAV\\Buffer\\Src\\CS"), CUAVBufferSrcCSTest ) ) return false;
    if( !ADD_TEST( _T("UAV\\Buffer\\Src\\PS"), CUAVBufferSrcPSTest ) ) return false;
    if( !ADD_TEST( _T("UAV\\Buffer\\Typed\\CS"), CUAVBufferTypedCSTest ) ) return false;
    if( !ADD_TEST( _T("UAV\\Buffer\\Typed\\PS"), CUAVBufferTypedPSTest ) ) return false;
    if( !ADD_TEST( _T("UAV\\Typed"), CUAVTypedTest ) ) return false;
    if( !ADD_TEST( _T("UAV\\TypedPixelShader"), CUAVTypedPixelShaderTest ) ) return false;
    if( !ADD_TEST( _T("UAV\\CounterUAV\\CS"), CCounterUAVTest ) ) return false;
    if( !ADD_TEST( _T("UAV\\CounterUAV\\PS"), CCounterUAVPSTest ) ) return false;
    if( !ADD_TEST( _T( "Dispatch\\Call" ), CCallTest ) ) return false;
    if( !ADD_TEST( _T( "Dispatch\\SV" ), CSVTest ) ) return false;
    if( !ADD_TEST( _T( "AppendConsume\\CS" ), CAppendConsume ) ) return false;
    if( !ADD_TEST( _T( "AppendConsume\\PS" ), CAppendConsumePS ) ) return false;
    if( !ADD_TEST( _T( "Interaction" ),  CInteractionTest ) ) return false;
    if( !ADD_TEST( _T( "PresentedUAV" ),  CInteractionPresentTest ) ) return false;

    return true;
}

//------------------------------------------------------------------------------------------------

void CComputeTestApp::InitOptionVariables()
{
    CD3D11TestFramework::InitOptionVariables();
    UnRegisterOptionVariable("srconly");
    m_D3DOptions.SrcOnly = true;

    m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_10_0;

    RegisterOptionVariable("RunSlowCasesOnRef", "Run test cases that will be very slow on REF", &m_bRunSlowOnRef, false);
    RegisterOptionVariable("SkipSlowTestCasesOnHardware", "Force the test cases that run slow on ref to skip.", &m_bSkipSlowOnHardware, false);
    RegisterOptionVariable("LogShaderText", "Log the HLSL text of the shader for each test case", &m_bLogShaderText, false);
}
