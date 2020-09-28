//////////////////////////////////////////////////////////////////////////
//  Viewports.cpp
//  created:	2005/06/08
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////
#include "Viewports.h"
#include "MultiView.h"
#include "Overlapping.h"
#include "SplitStrip.h"
#include "BadIndex.h"
#include "NoIndex.h"
#include "MultiScissor.h"
#include "ScaleView.h"
#include "ScaleScissor.h"
#include "ScissorPosition.h"
#include "SubScissor.h"

ViewportsTestApp g_App;

color MultiColors[16] = 
{
	color(1.0f,		0.0f,	0.0f),
	color(0.875f,	0.125f,	0.0f),
	color(0.75f,	0.25f,	0.0f),
	
	color(0.625f,	0.375f,	0.0f),
	color(0.5f,		0.5f,	0.0f),
	color(0.375f,	0.625f,	0.0f),
	
	color(0.25f,	0.75f,	0.0f),
	color(0.125f,	0.875f,	0.0f),
	color(0.0f,		1.0f,	0.0f),
	color(0.0f,		0.858f,	0.143f),
	
	color(0.0f,		0.715f,	0.286f),
	color(0.0f,		0.572f,	0.429f),
	color(0.0f,		0.429f,	0.572f),
	
	color(0.0f,		0.286f,	0.715f),
	color(0.0f,		0.143f,	0.858f),
	color(0.0f,		0.0f,	1.0f)
};

float ViewportShifts[16][2] =
{
	0.5f,	0.5f,
	-0.1f,	0.5f,
	0.1f,	0.5f,
	0.0f,	0.5f,
	0.5f,	-0.1f,
	-0.1f,	-0.1f,
	0.1f,	-0.1f,
	0.0f,	-0.1f,
	0.5f,	0.1f,
	-0.1f,	0.1f,
	0.1f,	0.1f,
	0.0f,	0.1f,
	0.5f,	0.0f,
	-0.1f,	0.0f,
	0.1f,	0.0f,
	0.0f,	0.0f,
};

BEGIN_NAMED_VALUES( OUTPUT_TYPE )
	NAMED_VALUE( "RTV", OUTPUT_TYPE_RTV )
	NAMED_VALUE( "UAV", OUTPUT_TYPE_UAV )
END_NAMED_VALUES( OUTPUT_TYPE )

tstring ToString( D3D11_VIEWPORT v )
{
	static TCHAR convert_buffer[256];

	_sntprintf( convert_buffer, sizeof( convert_buffer ) - 1, 
		_T( "Height: %f\tWidth: %f\tTopLeftX: %f\tTopLeftY: %f\tMinDepth: %f\tMaxDepth: %f" ), 
		v.Height, v.Width, v.TopLeftX, v.TopLeftY, v.MinDepth, v.MaxDepth );
	convert_buffer[255] = 0; // Force zero termination of convert_buffer
	return tstring( convert_buffer );	
}
	
void ViewportsTestApp::InitOptionVariables()
{
	int n = AddOptionCategory( _T( "Debug" ) );

	CD3D11TestFramework::InitOptionVariables();
    if (GetShell()->IsConsole())
    {
        m_bPresent = true; // Present() will dump to file if "-viewresources" is used
    }
    else
    {
	    RegisterOptionVariable( _T( "Present" ), _T( "Causes the tests to actually Present the render target." ), &m_bPresent, false, 0, n );
    }

	UnRegisterOptionVariable("srconly");

	m_D3DOptions.SrcOnly = true;
}

bool ViewportsTestApp::InitTestGroups()
{		
	//combined	
	if( !ADD_TEST( "NoIndex", NoIndex ) ) return false;
	if( !ADD_TEST( "BadIndex", BadIndex ) ) return false;

	//viewport tests
	if( !ADD_TEST( "MultiViewport", MultiView ) ) return false;
	if( !ADD_TEST( "ArbitraryViewport", ArbitraryView ) ) return false;
	if( !ADD_TEST( "ScaleView", ScaleView ) ) return false;
	if( !ADD_TEST( "OverlappingViewport", OverlappingView ) ) return false;
	if( !ADD_TEST( "SplitStrip", SplitStrip ) ) return false;

	//scissor tests
	if( !ADD_TEST( "MultiScissor", MultiScissor ) ) return false;
	if( !ADD_TEST( "ScaleScissor", ScaleScissor ) ) return false;
	if( !ADD_TEST( "ScissorPosition", ScissorPosition ) ) return false;
	if( !ADD_TEST( "SubScissor", SubScissor ) ) return false;
	return true;
}
	
testfactor::RFactor	ViewportsTest::GetViewportsBaseRFactor()
{
	CRangeValueSet< UINT > *pVSNumViewports = new CRangeValueSet< UINT >( 1, 16, 1 );
	CTestCaseParameter< UINT > *pNumViewportsParam = AddParameter( _T("NumViewports"), &m_NumViewports );
	return AddParameterValueSet( pNumViewportsParam, pVSNumViewports );
}

testfactor::RFactor	ViewportsTest::GetOutputTypeBaseRFactor()
{
    CUserValueSet<OUTPUT_TYPE>* pOutputTypeSet = new CUserValueSet<OUTPUT_TYPE>();
    pOutputTypeSet->AddValue( OUTPUT_TYPE_RTV );
    pOutputTypeSet->AddValue( OUTPUT_TYPE_UAV );
	CTestCaseParameter<OUTPUT_TYPE>* pOutputTypeParam = AddParameter( _T("OutputType"), &m_OutputType );
	return AddParameterValueSet( pOutputTypeParam, pOutputTypeSet );
}

TEST_RESULT ViewportsTest::Setup()
{
	TEST_RESULT tr = RESULT_PASS;

	ID3D10Blob *pVSBuf = NULL;
	ID3D10Blob *pGSBuf = NULL;
	ID3D10Blob *pPSRTVBuf = NULL;
	ID3D10Blob *pPSUAVBuf = NULL;
	D3D11_BUFFER_DESC bufferDesc;

	m_pRTView = g_App.GetRTView();
	m_pDSView = g_App.GetDSView();

    const char* PixelShaderProfile = GetFramework()->GetShaderProfile( D3D_SHADER_STAGE_PIXEL, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0);
    char *szVS = m_bUseIndex? "vs_wIndex.vsh": "vs.vsh";
	char *szGS = m_bUseIndex? "gs_wIndex.gsh": "gs.gsh";
	char *szPS = m_bUseIndex? "ps_wIndex.psh": "ps.psh";
	ID3D10Blob *pErrorBuf = NULL;

    //
    // Perform UAV-rasterization specific initialization only on FL 11+
    //

    if( GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 )
    {
        HRESULT hr = E_FAIL;

        // Create a depth stencil state that disables depth testing

	    D3D11_DEPTH_STENCILOP_DESC DepthOpDesc;
	    DepthOpDesc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	    DepthOpDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	    DepthOpDesc.StencilFunc = D3D11_COMPARISON_ALWAYS;
	    DepthOpDesc.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	    D3D11_DEPTH_STENCIL_DESC DepthDesc;
	    DepthDesc.BackFace = DepthOpDesc;
	    DepthDesc.DepthEnable = false;
	    DepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	    DepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	    DepthDesc.FrontFace = DepthOpDesc;
	    DepthDesc.StencilEnable = false;
	    DepthDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	    DepthDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	    if( FAILED( hr = GetDevice()->CreateDepthStencilState( &DepthDesc, &m_pDepthTestingDisabledState ) ) )
	    {
            LogError( __FILEW__, __LINE__, L"CreateDepthStencilState() for UAV-only rasterization failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateDepthStencilState );
            tr = RESULT_FAIL;
		    goto FAILED;
	    }

        // Create the unordered output texture buffer
        DXGI_FORMAT UAVFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
        D3D11_TEXTURE2D_DESC UnorderedAccessTexDesc;
	    UnorderedAccessTexDesc.ArraySize = 1;
	    UnorderedAccessTexDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	    UnorderedAccessTexDesc.CPUAccessFlags = 0;
	    UnorderedAccessTexDesc.Format = UAVFormat;
	    UnorderedAccessTexDesc.Height = m_RTHeight;
	    UnorderedAccessTexDesc.MipLevels = 1;
	    UnorderedAccessTexDesc.MiscFlags = 0;
	    UnorderedAccessTexDesc.SampleDesc.Count = 1;
	    UnorderedAccessTexDesc.SampleDesc.Quality = 0;
	    UnorderedAccessTexDesc.Usage = D3D11_USAGE_DEFAULT;
	    UnorderedAccessTexDesc.Width = m_RTWidth;
	    if( FAILED( hr = GetDevice()->CreateTexture2D( &UnorderedAccessTexDesc, NULL, &m_pUnorderedAccessTexture ) ) )
	    {
            LogError( __FILEW__, __LINE__, L"CreateTexture2D() for UAV-only texture failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
		    tr = RESULT_FAIL;
		    goto FAILED;
	    }

        // Create the unordered overdraw buffer for UAV-only rasterization
        UnorderedAccessTexDesc.Format = DXGI_FORMAT_R32_UINT;
	    if( FAILED( hr = GetDevice()->CreateTexture2D( &UnorderedAccessTexDesc, NULL, &m_pUnorderedAccessOverdrawTexture ) ) )
	    {
            LogError( __FILEW__, __LINE__, L"CreateTexture2D() for overdraw texture failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
		    tr = RESULT_FAIL;
		    goto FAILED;
	    }

        // Create the UAV for the unordered texture buffer
        if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pUnorderedAccessTexture, NULL, &m_pUAV ) ) )
	    {
            LogError( __FILEW__, __LINE__, L"CreateUnorderedAccessView() for UAV-only texture failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateView );
		    tr = RESULT_FAIL;
		    goto FAILED;
	    }

        // Create the UAV for the unordered overdraw texture buffer
        if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pUnorderedAccessOverdrawTexture, NULL, &m_pOverdrawUAV ) ) )
	    {
            LogError( __FILEW__, __LINE__, L"CreateUnorderedAccessView() for overdraw texture failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateView );
		    tr = RESULT_FAIL;
		    goto FAILED;
	    }

        // Create the UAV pixel shader
        if ( FAILED( hr = D3DX10CompileFromResource(NULL, szPS, NULL, NULL, NULL, "mainUAV", PixelShaderProfile, 0, 0, NULL, &pPSUAVBuf, NULL, NULL) ) )
		{
            LogError( __FILEW__, __LINE__, L"D3DX10CompileFromResource() for UAV-only rasterization failed (Pixel Shader).", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3DX::gFCCompileShader );
			tr = RESULT_FAIL;
			goto FAILED;
		}

		if ( FAILED( hr = GetDevice()->CreatePixelShader( (DWORD*) pPSUAVBuf->GetBufferPointer(), pPSUAVBuf->GetBufferSize(), NULL, &m_pUAVPS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreatePixelShader() for UAV-only rasterization failed (Pixel Shader).", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader );
			tr = RESULT_FAIL;
			goto FAILED;
		}

		UnorderedAccessTexDesc.BindFlags = 0;
		UnorderedAccessTexDesc.Usage = D3D11_USAGE_STAGING;
		UnorderedAccessTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		
		if ( FAILED( hr = GetDevice()->CreateTexture2D( &UnorderedAccessTexDesc, NULL, &m_pUAVOverdrawTexCopy ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateTexture2D() for overdraw texture copy failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
			tr = RESULT_FAIL;
			goto FAILED;
		}
    }

    //
    // Perform RTV-rasterization specific initialization
    //

	//Create rasterizer states- one for viewport tests with scissors disabled
	//the other for scissor tests.
	//Each test will set their desired state

	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = true;
	rastDesc.DepthBias = false;
	rastDesc.DepthBiasClamp = 0;
	rastDesc.SlopeScaledDepthBias = 0;
	rastDesc.DepthClipEnable = true;
	rastDesc.ScissorEnable = false;
	rastDesc.MultisampleEnable = false;
	rastDesc.AntialiasedLineEnable = false;
	rastDesc.FillMode = D3D11_FILL_SOLID;
		
	if( FAILED( GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSView ) ) )
	{
		WriteToLog("CreateRasterizerState failed.");
		tr = RESULT_FAIL;
		goto FAILED;
	}

	rastDesc.ScissorEnable = true;
	if( FAILED( GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSScissor ) ) )
	{
		WriteToLog("CreateRasterizerState failed.");
		tr = RESULT_FAIL;
		goto FAILED;
	}

	GetEffectiveContext()->RSSetState( m_pRSView );
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	//
	// Set Blend state
	//
	D3D11_BLEND_DESC blendDesc;	
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;  // TODO add alpha param
	blendDesc.RenderTarget[1].BlendEnable = FALSE;
	blendDesc.RenderTarget[2].BlendEnable = FALSE;
	blendDesc.RenderTarget[3].BlendEnable = FALSE;
	blendDesc.RenderTarget[4].BlendEnable = FALSE;
	blendDesc.RenderTarget[5].BlendEnable = FALSE;
	blendDesc.RenderTarget[6].BlendEnable = FALSE;
	blendDesc.RenderTarget[7].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0xf;
	blendDesc.RenderTarget[1].RenderTargetWriteMask = 0xf;
	blendDesc.RenderTarget[2].RenderTargetWriteMask = 0xf;
	blendDesc.RenderTarget[3].RenderTargetWriteMask = 0xf;
	blendDesc.RenderTarget[4].RenderTargetWriteMask = 0xf;
	blendDesc.RenderTarget[5].RenderTargetWriteMask = 0xf;
	blendDesc.RenderTarget[6].RenderTargetWriteMask = 0xf;
	blendDesc.RenderTarget[7].RenderTargetWriteMask = 0xf;
	if( FAILED( GetDevice()->CreateBlendState( &blendDesc, &m_pBS ) ) )
	{
		WriteToLog("CreateBlendState failed.");
		tr = RESULT_FAIL;
		goto FAILED;
	}

	GetEffectiveContext()->OMSetBlendState( m_pBS, 0, D3D10_DEFAULT_SAMPLE_MASK );

	//
	// Create the constant buffer
	//
	D3D11_BUFFER_DESC bufdesc;
	bufdesc.ByteWidth = 16;
	bufdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufdesc.MiscFlags = 0;
	if( FAILED( GetDevice()->CreateBuffer( &bufdesc, NULL, &m_pCBuffer ) ) )
	{
		WriteToLog("CreateBuffer for constant buffer failed.");
		tr = RESULT_FAIL;
		goto FAILED;
	}

	GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pCBuffer );
	GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pCBuffer );
	GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pCBuffer );

	//
	// Create Shaders
	//
	if ( FAILED(D3DX10CompileFromResource(NULL, szVS, NULL, NULL, NULL, "main", "vs_4_0", 0, 0, NULL, &pVSBuf, &pErrorBuf, NULL) ) )
	{
		WriteToLog("CompileShaderFromResource for vertex shader failed.");
		if (pErrorBuf)
		{
			LPVOID pBuf = pErrorBuf->GetBufferPointer();
			size_t bufSize = pErrorBuf->GetBufferSize();
			char* szError = new char[bufSize+1];
			memcpy(szError, pBuf, bufSize);
			szError[bufSize] = 0;
			WriteToLog( _T( "Compiler errors: %s\n" ), szError );
			delete[] szError;
		}
		tr = RESULT_FAIL;
		goto FAILED;
	}

	if ( FAILED( GetDevice()->CreateVertexShader( (DWORD*) pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), NULL, &m_pVS ) ) )
	{
		WriteToLog("CreateVertexShader Failed.");
		tr = RESULT_FAIL;
		goto FAILED;
	}

	if ( FAILED( D3DX10CompileFromResource(NULL, szGS, NULL, NULL, NULL, "main", "gs_4_0", 0, 0, NULL, &pGSBuf, NULL, NULL ) ) )
	{
		WriteToLog("CompileShaderFromResource for geometry shader failed.");
		tr = RESULT_FAIL;
		goto FAILED;
	}

	if ( FAILED( GetDevice()->CreateGeometryShader( (DWORD*) pGSBuf->GetBufferPointer(), pGSBuf->GetBufferSize(), NULL, &m_pGS ) ) )
	{
		WriteToLog("CreateGeometryShader Failed.");
		tr = RESULT_FAIL;
		goto FAILED;
	}

	if ( FAILED( D3DX10CompileFromResource(NULL, szPS, NULL, NULL, NULL, "mainRTV", PixelShaderProfile, 0, 0, NULL, &pPSRTVBuf, NULL, NULL) ) )
	{
		WriteToLog("CompileShaderFromResource for pixel shader failed.");
		tr = RESULT_FAIL;
		goto FAILED;
	}

	if ( FAILED( GetDevice()->CreatePixelShader( (DWORD*) pPSRTVBuf->GetBufferPointer(), pPSRTVBuf->GetBufferSize(), NULL, &m_pRTVPS ) ) )
	{
		WriteToLog("CreatePixelShader failed.");
		tr = RESULT_FAIL;
		goto FAILED;
	}

	GetEffectiveContext()->VSSetShader(m_pVS, NULL, 0);
	GetEffectiveContext()->GSSetShader(m_pGS, NULL, 0);

	// Define our vertex data layout
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "pos", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "color", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VPi", 0, DXGI_FORMAT_R8_UINT,			1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if(	FAILED(	GetDevice()->CreateInputLayout( layout, 3, pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), &m_pVLayout ) ) )
	{
		WriteToLog("CreateInputLayout failed.");
		tr = RESULT_FAIL;
		goto FAILED;
	}

	GetEffectiveContext()->IASetInputLayout( m_pVLayout );

	//
	// Create vertex buffer
	//
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = m_numVertices * sizeof( VERTS );
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				
	if( FAILED( GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pVBuffer ) ) )
	{
		WriteToLog("CreateBuffer for vertex buffer failed.");
		tr = RESULT_FAIL;
		goto FAILED;
	}
	if ( m_bUseIndex )
	{
		bufferDesc.ByteWidth = m_numVertices * sizeof(UINT);
		if( FAILED( GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pVIBuffer ) ) )
		{
			WriteToLog("CreateBuffer for vertex buffer failed.");
			tr = RESULT_FAIL;
			goto FAILED;
		}
	}

	UINT strides[] = { sizeof( VERTS ), sizeof( UINT ) };
	UINT offsets[] = { 0, 0 };
	ID3D11Buffer *pBuffers[2] = {
		m_pVBuffer,
		m_pVIBuffer,
	};

	GetEffectiveContext()->IASetVertexBuffers( 0, m_bUseIndex?2:1, pBuffers, strides, offsets );

	//
	// setup const data
	//
	FLOAT *bufData;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if( FAILED( GetEffectiveContext()->Map( m_pCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
	{
		WriteToLog("Map on constant buffer failed");
		tr = RESULT_FAIL;
		goto FAILED;
	}

	bufData = (float*) mappedRes.pData;

	// Inverse viewport scale.
	bufData[2] = 2;
	bufData[3] = -2;
	// Prescaled inverse viewport translation.
	bufData[0] = -1.0f;
	bufData[1] = 1.0f;

	GetEffectiveContext()->Unmap(m_pCBuffer,0);

	SAFE_RELEASE( pVSBuf );
	SAFE_RELEASE( pGSBuf );
	SAFE_RELEASE( pPSRTVBuf );
	SAFE_RELEASE( pPSUAVBuf );

	D3D11_TEXTURE2D_DESC tex2ddesc;
	tex2ddesc.ArraySize = 1;
	tex2ddesc.BindFlags = 0;
	tex2ddesc.Usage = D3D11_USAGE_STAGING;
	tex2ddesc.SampleDesc.Count = 1;
	tex2ddesc.SampleDesc.Quality = 0;
	tex2ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	tex2ddesc.Format = g_App.m_RenderTargetOptions.SwapChainDesc.Format;
	tex2ddesc.Height = m_RTHeight;
	tex2ddesc.Width = m_RTWidth;
	tex2ddesc.MipLevels = 1;
	tex2ddesc.MiscFlags = 0;
		
	if ( FAILED( GetDevice()->CreateTexture2D( &tex2ddesc, NULL, &m_pTex2DCopy ) ) )
	{
		WriteToLog( _T("CreateTexture2D failed for m_pRTTexture2DCopy.") );
		tr = RESULT_FAIL;
		goto FAILED;
	}

	HRESULT hr;
	if( FAILED(hr = ExecuteEffectiveContext()) )
	{
		WriteToLog( _T("Failed to execute deferred context: %s"), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto FAILED;
	}

	return tr;

FAILED:
	Cleanup();
	return tr;
}

TEST_RESULT ViewportsTest::SetupTestCase()
{
    //
    // First, make sure we're not executing UAV-only rasterization test cases
    // on unsupported feature levels
    //

    if( m_OutputType == OUTPUT_TYPE_UAV && GetDevice()->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 )
    {
        WriteToLog( _T( "Skipping test case because UAV-only rasterization is only supported on FL11.0+ HW." ) );
        return RESULT_SKIP;
    }

    //
    // Now, perform the actual set-up
    //

    if( m_OutputType == OUTPUT_TYPE_RTV )
    {
        // Bind the RTV and DSV to the pipeline, unbind UAVs
        GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, m_pDSView );

	    // Set RTV shaders
	    GetEffectiveContext()->PSSetShader( m_pRTVPS, NULL, 0 );

        // Set the depth stencil state to default
        GetEffectiveContext()->OMSetDepthStencilState( NULL, 0 );
    }
    else // m_OutputType == OUTPUT_TYPE_UAV
    {
        // Unbind the RTV and DSV, bind UAVs
        ID3D11UnorderedAccessView* pUAVArray[2] = { m_pUAV, m_pOverdrawUAV };
        GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 0, NULL, NULL, 0, 2, pUAVArray, NULL );

        // Set RTV shaders
	    GetEffectiveContext()->PSSetShader( m_pUAVPS, NULL, 0 );

        // Disable depth testing
        GetEffectiveContext()->OMSetDepthStencilState( m_pDepthTestingDisabledState, 0 );
    }

    ClearOutputTextures();

    return RESULT_PASS;
}

void ViewportsTest::ClearOutputTextures()
{
    FLOAT clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    UINT clearColorUint[4] = {0, 0, 0, 0};

    if( m_OutputType == OUTPUT_TYPE_RTV )
    {
        // Clear the RTV
        GetEffectiveContext()->ClearRenderTargetView( m_pRTView, clearColor );

        // Clear the DSV
        GetEffectiveContext()->ClearDepthStencilView( m_pDSView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
    }
    else // m_OutputType == OUTPUT_TYPE_UAV
    {
        // Clear the UAVs
        GetEffectiveContext()->ClearUnorderedAccessViewFloat( m_pUAV, clearColor );
        GetEffectiveContext()->ClearUnorderedAccessViewUint( m_pOverdrawUAV, clearColorUint );
    }
}

void ViewportsTest::Cleanup()
{
	ID3D11RenderTargetView* pRTNull[] = { NULL, NULL, NULL, NULL };
	ID3D11Buffer* pBufferNull[] = { NULL };
	ID3D11Buffer *pNullBuffers[2] = { NULL, NULL };
	UINT offsets[] = { 0 };


	if ( GetDevice() )
	{
		GetEffectiveContext()->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );
		//GetEffectiveContext()->OMSetRenderTargets( 4, pRTNull, NULL );
		GetEffectiveContext()->IASetInputLayout( NULL );
		GetEffectiveContext()->IASetVertexBuffers( 0, m_bUseIndex?2:1, pNullBuffers, offsets, offsets );
		GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->VSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->GSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->RSSetState( NULL );
		GetEffectiveContext()->SOSetTargets( 1, pNullBuffers, offsets );
		//GetEffectiveContext()->OMSetDepthStencilState( NULL, 0 );

        //
        // Release UAV-rasterization specific resources
        //

        if( GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 )
        {
            SAFE_RELEASE( m_pUAVPS );
            SAFE_RELEASE( m_pUAVOverdrawTexCopy );

            SAFE_RELEASE( m_pDepthTestingDisabledState );
            SAFE_RELEASE( m_pUAV );
            SAFE_RELEASE( m_pOverdrawUAV );
            SAFE_RELEASE( m_pUnorderedAccessTexture );
            SAFE_RELEASE( m_pUnorderedAccessOverdrawTexture );
        }

		HRESULT hr;
		if( FAILED(hr = ExecuteEffectiveContext()) )
		{
			WriteToLog( _T("Failed to execute deferred context: %s"), D3DHResultToString(hr).c_str() );
		}
	}

    //
    // Release RTV-rasterization specific resources
    //

	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pRTVPS );

	SAFE_RELEASE( m_pTex2DCopy );
	SAFE_RELEASE( m_pCBuffer );

	SAFE_RELEASE( m_pRSView );
	SAFE_RELEASE( m_pRSScissor );

	SAFE_RELEASE( m_pBS );

	SAFE_RELEASE( m_pVLayout );
	SAFE_RELEASE( m_pVBuffer );
	SAFE_RELEASE( m_pVIBuffer );
}

bool ViewportsTest::MapRT( D3D11_MAPPED_SUBRESOURCE* TexMap, D3D11_MAPPED_SUBRESOURCE* OverdrawTexMap )
{
	bool tRes = true;

    if( m_OutputType == OUTPUT_TYPE_RTV )
    {
        // Copy the output texture to a staging copy
	    GetEffectiveContext()->CopySubresourceRegion( m_pTex2DCopy, 0, 0, 0, 0, g_App.GetRTTexture(), 0, 0 );
    }
    else // m_OutputType == OUTPUT_TYPE_UAV
    {
        // Copy the output texture to a staging copy
	    GetEffectiveContext()->CopySubresourceRegion( m_pTex2DCopy, 0, 0, 0, 0, m_pUnorderedAccessTexture, 0, 0 );

        // If needed, also copy the UAV overdraw texture to a staging copy
        if( OverdrawTexMap != NULL )
        {
            GetEffectiveContext()->CopySubresourceRegion( m_pUAVOverdrawTexCopy, 0, 0, 0, 0, m_pUnorderedAccessOverdrawTexture, 0, 0 );
        }
    }

    HRESULT hr;
	if( FAILED(hr = ExecuteEffectiveContext()) )
	{
		WriteToLog( _T("Failed to execute deferred context: %s"), D3DHResultToString(hr).c_str() );
		tRes = false;
		goto done;
	}

	if( FAILED( hr = GetImmediateContext()->Map( m_pTex2DCopy, 0, D3D11_MAP_READ, 0, TexMap )) )
	{
        LogError( __FILEW__, __LINE__, L"Map() for output texture failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextMap );
		tRes = false;
        goto done;
	}

    if( m_OutputType == OUTPUT_TYPE_UAV && OverdrawTexMap != NULL )
    {
        if( FAILED( hr = GetImmediateContext()->Map( m_pUAVOverdrawTexCopy, 0, D3D11_MAP_READ, 0, OverdrawTexMap )) )
	    {
		    LogError( __FILEW__, __LINE__, L"Map() for overdraw texture failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextMap );
		    tRes = false;
            goto done;
	    }
    }

done:

	return tRes;
}

RECT ViewportToRect( const D3D11_VIEWPORT& Viewport, D3D_FEATURE_LEVEL FeatureLevel )
{
	RECT ViewRect;

	// FL 11 and 10 behavior is [1.0, 2.0) => 1
	// C cast behavior is floor as well
	// Win7:686495
	// According to the D3D11 spec: The implicit scissor to the viewport rounds the viewport X/Y extents to negative infinity.
	// The viewport width and height cannot be rounded; they must be added to unrounded TopLeftX and TopLeftY to determine the right and bottom extents, 
	// which then get rounded to determine the scissor extents. Round to negative infinity is just a floor.
	ViewRect.left = (LONG) floor( (double)Viewport.TopLeftX );
	ViewRect.top = (LONG) floor( (double)Viewport.TopLeftY );
	ViewRect.right = (LONG) floor( (double)Viewport.TopLeftX + (double)Viewport.Width );
	ViewRect.bottom = (LONG) floor( (double)Viewport.TopLeftY + (double)Viewport.Height );

	if ( g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		// The rect here is used to validate the RT
		// If fractional viewports are used there could possibly be a .5 pixel "dead zone" which is non-deterministic. Which we need to account for.

		// The spec says:
		// Observe that when the viewport location is fractional, which results in rounding to determine the implicit scissor, 
		// there is effectively a non-deterministic zone of up to 1/2 pixel wide along the left and top edges within the scissor area, 
		// not covered by the viewport.  Because it is optional for implementations to perform guard-band clipping to viewport extents, 
		// and even if they do, implementations of it could vary, this means that rendering results in the non-deterministic zone will be some 
		// undefined combination of background values and primitives that may or may not have been clipped off the zone.

		const float fractionalXComp = Viewport.TopLeftX - int(Viewport.TopLeftX);
		const float fractionalYComp = Viewport.TopLeftY - int(Viewport.TopLeftY);
		bool pixelCenterXIsCovered = false;
		bool pixelCenterYIsCovered = false;

		if( fractionalXComp <= 0.5f )
			pixelCenterXIsCovered = true;	// Then the scissor is covering the pixel center, so don't worry about modifying the rect

		if( fractionalYComp <= 0.5f )
			pixelCenterYIsCovered = true;	// Then the scissor is covering the pixel center, so don't worry about modifying the rect

		if( !pixelCenterXIsCovered )
			// Since the left most pixel center is not covered on the X, the left column is non-deterministic
			ViewRect.left++;

		if( !pixelCenterYIsCovered )
			// Since the top most pixel center is not covered on the Y, the top row is non-deterministic
			ViewRect.top++;
	}

	return ViewRect;
}
	
void CalcRelativeDiff( color Expected, color Compare, FLOAT Diff[3] )
{
	if ( Expected.r == Compare.r )
		Diff[0] = 0.0f;
	else
		Diff[0] = abs(Compare.r / Expected.r - 1.0f);

	if ( Expected.g == Compare.g )
		Diff[1] = 0.0f;
	else
		Diff[1] = abs(Compare.g / Expected.g - 1.0f);

	if ( Expected.b == Compare.b )
		Diff[2] = 0.0f;
	else
		Diff[2] = abs(Compare.b / Expected.b - 1.0f);
}
