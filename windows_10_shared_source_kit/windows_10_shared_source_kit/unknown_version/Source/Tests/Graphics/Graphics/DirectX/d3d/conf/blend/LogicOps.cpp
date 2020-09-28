// Includes
#include "Blend.h"
#include "FormatHelper.h"
#include "FormatDesc.hpp"

using namespace testfactor;

DXGI_FORMAT g_pRenderTargetFormatData[][D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = 
{		//RT0							//RT1							//RT2							//RT3							//RT4							//RT5							//RT6							//RT7
	{	DXGI_FORMAT_R32G32B32A32_UINT,	DXGI_FORMAT_R32G32B32A32_UINT,	DXGI_FORMAT_R32G32B32A32_UINT,	DXGI_FORMAT_R32G32B32A32_UINT,	DXGI_FORMAT_R32G32B32A32_UINT,	DXGI_FORMAT_R32G32B32A32_UINT,	DXGI_FORMAT_R32G32B32A32_UINT,	DXGI_FORMAT_R32G32B32A32_UINT	},
	{	DXGI_FORMAT_R8_UINT,			DXGI_FORMAT_R8_UINT,			DXGI_FORMAT_R8_UINT,			DXGI_FORMAT_R8_UINT,			DXGI_FORMAT_R8_UINT,			DXGI_FORMAT_R8_UINT,			DXGI_FORMAT_R8_UINT,			DXGI_FORMAT_R8_UINT	},
	{	DXGI_FORMAT_R32G32B32A32_UINT,	DXGI_FORMAT_R32G32B32A32_UINT,	DXGI_FORMAT_R10G10B10A2_UINT,	DXGI_FORMAT_R10G10B10A2_UINT,	DXGI_FORMAT_R32G32_UINT,		DXGI_FORMAT_R32G32_UINT,		DXGI_FORMAT_R32_UINT,			DXGI_FORMAT_R32_UINT	},
	{	DXGI_FORMAT_R16G16B16A16_UINT,	DXGI_FORMAT_R16G16B16A16_UINT,	DXGI_FORMAT_R16G16_UINT,		DXGI_FORMAT_R16G16_UINT,		DXGI_FORMAT_R16_UINT,			DXGI_FORMAT_R16_UINT,			DXGI_FORMAT_R10G10B10A2_UINT,	DXGI_FORMAT_R10G10B10A2_UINT	},
	{	DXGI_FORMAT_R8G8B8A8_UINT,		DXGI_FORMAT_R8G8B8A8_UINT,		DXGI_FORMAT_R8G8_UINT,			DXGI_FORMAT_R8G8_UINT,			DXGI_FORMAT_R8_UINT,			DXGI_FORMAT_R8_UINT, 			DXGI_FORMAT_R10G10B10A2_UINT,	DXGI_FORMAT_R32G32B32A32_UINT	},
};
const UINT g_pRenderTargetFormatDataNumEntries = sizeof( g_pRenderTargetFormatData ) / sizeof( g_pRenderTargetFormatData[0] );

CLogicOpTest::CLogicOpTest()
	: CBlendConfTest()
	, m_pSrcTexture2D( NULL )
	, m_pSrcTexture2DSRV( NULL )
	, m_pPSInitRT( NULL )
	, m_pPipelineQuery( NULL )
	, m_pSrcTextureData( NULL )
	, m_pLogicORBlendState( NULL )
{
	// Setup some defaults
	m_NumRTs = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
	m_nDimension = 2;	// The the parent class to create Texture2D rendertargets
	m_nPrimitiveW = 23;
	m_nPrimitiveH = m_nPrimitiveW;	
	m_sPixelShaderName = m_sVertexShaderName = _T("logic.sh");
	m_sPSEntryPt = _T("PSMain");
	m_sPSInitRTEntryPt = _T("PSMainInitRT");

	ZeroMemory(&m_BlendDesc1, sizeof(m_BlendDesc1));
	m_BlendDesc1.AlphaToCoverageEnable = FALSE;
	m_BlendDesc1.IndependentBlendEnable = FALSE;
	for (int n = 0; n < 8; ++n)
	{
		m_BlendDesc1.RenderTarget[n].LogicOpEnable = FALSE;
		m_BlendDesc1.RenderTarget[n].BlendEnable = FALSE;
		m_BlendDesc1.RenderTarget[n].BlendOp = D3D11_BLEND_OP_ADD;
		m_BlendDesc1.RenderTarget[n].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		m_BlendDesc1.RenderTarget[n].SrcBlend = D3D11_BLEND_ONE;
		m_BlendDesc1.RenderTarget[n].SrcBlendAlpha = D3D11_BLEND_ONE;
		m_BlendDesc1.RenderTarget[n].DestBlend = D3D11_BLEND_ZERO;
		m_BlendDesc1.RenderTarget[n].DestBlendAlpha = D3D11_BLEND_ZERO;
		m_BlendDesc1.RenderTarget[n].LogicOp = D3D11_LOGIC_OP_CLEAR;
		m_BlendDesc1.RenderTarget[n].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
}

void CLogicOpTest::InitTestParameters()
{
	// Local variables
	const tstring logicOpStr		= "LogicOp";
	tstring rtFormatStr[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];	// The string names are set below

	// Logic Op
	// Operation: (s == PS output, d = RTV contents)
	CUserValueSet<D3D11_LOGIC_OP> *pLogicOpValues = new CUserValueSet<D3D11_LOGIC_OP>();								
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_CLEAR );		   // 0
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_SET );            // 1
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_COPY );           // s
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_COPY_INVERTED );  // ~s
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_NOOP );           // d
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_INVERT );         // ~d
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_AND );            // s & d
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_NAND );           // ~(s & d)
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_OR );             // s | d
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_NOR );            // ~(s | d)
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_XOR );            // s ^ d
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_EQUIV );          // ~(s ^ d)
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_AND_REVERSE );    // s & ~d
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_AND_INVERTED );   // ~s & d
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_OR_REVERSE );     // s | ~d
    pLogicOpValues->AddValue( D3D11_LOGIC_OP_OR_INVERTED );    // ~s | d
	RFactor rLogicOps = AddParameter<D3D11_LOGIC_OP>( _T( logicOpStr ), &m_logicOp, pLogicOpValues );

	// Render Target Formats
	RFactor rRTFormat[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	for( int i = 0; i < m_NumRTs; i++ )
	{
		// The name of the parameter contains the RT index
		rtFormatStr[i] = "RenderTargetFormat" + ToString(i);

		// Add the parameter based on the name
		rRTFormat[i] = AddParameter( rtFormatStr[i], &m_eRTFormats[i], 
			new CTableValueSet<DXGI_FORMAT>( 
			&(g_pRenderTargetFormatData[0][i]),			// First instance of the D3D11_LOGIC_OP
			sizeof( g_pRenderTargetFormatData[0] ),		// Stride
			g_pRenderTargetFormatDataNumEntries ) );

		SetParameterDefaultValue( rtFormatStr[i], DXGI_FORMAT_R32G32B32A32_UINT );
	}

	RFactor rRTFormats = rRTFormat[0];
	for( int i = 1; i < m_NumRTs; i++ )
	{
		// These are %'d because they are table value sets. 
		// Each parameter value is associated with the other factors based on the data tables.
		// See g_pRenderTargetFormatData.
		rRTFormats	= rRTFormats % rRTFormat[i];
	}

	SetRootTestFactor( rLogicOps * rRTFormats );

	// Priority patten filters
	AddPriorityPatternFilter(FilterEqual<D3D11_LOGIC_OP>( _T( logicOpStr ), D3D11_LOGIC_OP_AND), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<D3D11_LOGIC_OP>( _T( logicOpStr ), D3D11_LOGIC_OP_OR), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<D3D11_LOGIC_OP>( _T( logicOpStr ), D3D11_LOGIC_OP_XOR), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<D3D11_LOGIC_OP>( _T( logicOpStr ), D3D11_LOGIC_OP_INVERT), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

////

TEST_RESULT CLogicOpTest::Setup()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool bSuccess = false;
	std::vector<UINT> bitPatterns;
	D3D11_TEXTURE2D_DESC tex2DDesc;
	D3D11_SUBRESOURCE_DATA tex2DSubresourceData;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_VIEWPORT vp;
	D3D11_RASTERIZER_DESC rastDesc;
	D3D11_QUERY_DESC queryDesc;
	D3D11_BLEND_DESC1 blendDesc1;
	D3D11_FEATURE_DATA_D3D11_OPTIONS dataD3D11Options;
	
	bitPatterns.clear();
	ZeroMemory( &tex2DDesc, sizeof( tex2DDesc ) );
	ZeroMemory( &tex2DSubresourceData, sizeof( tex2DSubresourceData ) );
	ZeroMemory( &srvDesc, sizeof( srvDesc ) );
	ZeroMemory( &vp, sizeof( vp ) );
	ZeroMemory( &rastDesc, sizeof( rastDesc ) );
	ZeroMemory( &queryDesc, sizeof( queryDesc ) );
	ZeroMemory( &blendDesc1, sizeof( blendDesc1 ) );
	ZeroMemory( &dataD3D11Options, sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS) );

	//
	// Get the device and context
	//
	m_pDevice1 = GetFramework()->GetDevice1();
	m_pDeviceContext1 = GetFramework()->GetEffectiveContext1();

	//
	// This test group doesn't run feature level 9.X
	//
	if( m_pDevice1->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_0 )
	{
		WriteToLog( _T("This test group doesn't run feature level 9.X. Skipping the group.") );
		return RESULT_SKIP;
	}

	// LogicOps are optional for feature levels < 11.1
	if( m_pDevice1->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_1 )
	{
		// Check for support
		D3D11_FEATURE_DATA_D3D11_OPTIONS d3d11OptionsData;
		ZeroMemory( &d3d11OptionsData, sizeof(d3d11OptionsData) );
		if( FAILED( hr = m_pDevice1->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS, &d3d11OptionsData, sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS) ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CheckFeatureSupport() failed.", false, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCheckFeatureSupport );
			goto done;
		}

		if( !d3d11OptionsData.OutputMergerLogicOp )
		{
			WriteToLog(_T("The driver doesn't support LogicOps. Skipping the group.") );
			return RESULT_SKIP;
		}
	}

	//
	// Create our set of interesting bit patterns
	//

	// The textures should be square
	WriteToLog( _T("Texture width = %d"), m_nPrimitiveW );
	WriteToLog( _T("Texture height = %d"), m_nPrimitiveH );
	if( m_nPrimitiveW != m_nPrimitiveH )
	{
		LogError( __FILEW__, __LINE__, L"The src texture is not square, this will cause an issue with the test.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConflictingParameters );
		goto done;
	}

	// Add our interesting patterns
	// We should only have enough to fill one dimension of the src texture
	// Note: 8, 10, 16, and 32 bit boundaries
	
	// Doesn't clamp for any output format >= 2-bit per component
	bitPatterns.push_back( 0x00000000 );
	bitPatterns.push_back( 0x00000001 );
	bitPatterns.push_back( 0x00000002 );
	bitPatterns.push_back( 0x00000003 );

	// Doesn't clamp for any output format >= 8-bit per component
	bitPatterns.push_back( 0x000000FF );
	bitPatterns.push_back( 0x000000AA );
	bitPatterns.push_back( 0x00000055 );
	bitPatterns.push_back( 0x000000CC );
	bitPatterns.push_back( 0x0000000D );
	bitPatterns.push_back( 0x00000005 );
	
	// Doesn't clamp for any output format >= 10-bit per component
	bitPatterns.push_back( 0x000003FF );
	bitPatterns.push_back( 0x000002AA );
	bitPatterns.push_back( 0x0000010F );

	// Doesn't clamp for any output format >= 16-bit per component
	bitPatterns.push_back( 0x0000FFFF );
	bitPatterns.push_back( 0x0000ACAC );
	bitPatterns.push_back( 0x0000A35C );

	// Doesn't clamp for any output format >= 32-bit per component
	bitPatterns.push_back( 0xFFFFFFFF );
	bitPatterns.push_back( 0x00FFFFFF );
	bitPatterns.push_back( 0x00FF00FF );
	bitPatterns.push_back( 0x0F0F0F0F );
	bitPatterns.push_back( 0xF0F0F0F0 );
	bitPatterns.push_back( 0xFFCAAFFC );
	bitPatterns.push_back( 0x3333AAAA );

	WriteToLog( _T("%u unique bit patterns will be added to the src texture."), bitPatterns.size() );
	if( bitPatterns.size() != m_nPrimitiveW )
	{
		LogError( __FILEW__, __LINE__, L"The src texture dimensions don't match the number of bit patterns specified.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConflictingParameters );
		goto done;
	}

	// Create a reference texture that will serve as the "known" data that the PS will output
	// This 2D texture is just a collection of interesting 32-bit patterns
	assert( m_pSrcTexture2D == NULL );
	tex2DDesc.ArraySize = 1;
	tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex2DDesc.CPUAccessFlags = 0;
	tex2DDesc.Format = DXGI_FORMAT_R32_UINT;
	tex2DDesc.Height = m_nPrimitiveH;
	tex2DDesc.MipLevels = 1;
	tex2DDesc.MiscFlags = 0;
	tex2DDesc.SampleDesc.Count = 1;
	tex2DDesc.SampleDesc.Quality = 0;
	tex2DDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tex2DDesc.Width = m_nPrimitiveW;

	// Create our reference texture initial data
	// The reference texture has the same data in each row
	// Eg:
	// 0x00000000	0xFFFFFFFF	0x00FFFFFF	0x00FF00FF ...
	// 0x00000000	0xFFFFFFFF	0x00FFFFFF	0x00FF00FF ...
	// 0x00000000	0xFFFFFFFF	0x00FFFFFF	0x00FF00FF ...
	//	...
	assert( tex2DDesc.Format == DXGI_FORMAT_R32_UINT );
	assert( m_pSrcTextureData == NULL );
	m_pSrcTextureData = new UINT[tex2DDesc.Width * tex2DDesc.Height];
	for( UINT r = 0; r < tex2DDesc.Height; r++ )
	{
		for( UINT c = 0; c < tex2DDesc.Width; c++ )
		{
			const UINT srcTextureIndex = (r * tex2DDesc.Width) + c;
			m_pSrcTextureData[srcTextureIndex] = bitPatterns[c];
		}
	}

	tex2DSubresourceData.pSysMem = m_pSrcTextureData;
	tex2DSubresourceData.SysMemPitch = sizeof( UINT ) * tex2DDesc.Width;
	tex2DSubresourceData.SysMemSlicePitch = tex2DSubresourceData.SysMemPitch * tex2DDesc.Height;

	if( FAILED( hr = m_pDevice1->CreateTexture2D( &tex2DDesc, &tex2DSubresourceData, &m_pSrcTexture2D ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateTexture2D() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
		goto done;
	}

	// Create the SRV for the reference texture
	srvDesc.Format = tex2DDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	if( FAILED( hr = m_pDevice1->CreateShaderResourceView( m_pSrcTexture2D, &srvDesc, &m_pSrcTexture2DSRV ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateShaderResourceView() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateView );
		goto done;
	}

	// Create and set the viewport
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float) tex2DDesc.Width;
	vp.Height   = (float) tex2DDesc.Height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	m_pDeviceContext1->RSSetViewports(1, &vp);

	// Create the rasterizer state
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = true;
	rastDesc.DepthBias = false;
	rastDesc.DepthBiasClamp = 0.0f;
	rastDesc.SlopeScaledDepthBias = 0.0f;
	rastDesc.DepthClipEnable = true;
	rastDesc.ScissorEnable = false;
	rastDesc.MultisampleEnable = false;
	rastDesc.AntialiasedLineEnable = false;
	if( FAILED( hr = m_pDevice1->CreateRasterizerState( &rastDesc, &m_pRSEnabled ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateRasterizerState() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateState );
		goto done;
	}

	// Create the default
	blendDesc1.AlphaToCoverageEnable = false;
	blendDesc1.IndependentBlendEnable = false;	// Logic ops must have this false
	blendDesc1.RenderTarget[0].BlendEnable = false;
	blendDesc1.RenderTarget[0].LogicOpEnable = true;
	blendDesc1.RenderTarget[0].LogicOp = D3D11_LOGIC_OP_OR;
	blendDesc1.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if( FAILED( hr = m_pDevice1->CreateBlendState1( &blendDesc1, &m_pLogicORBlendState ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateBlendState1() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateState );
		goto done;
	}

	//
	// Create the shaders
	//

	if( !CreateShaders() )
	{
		WriteToLog( _T("CreateShaders() failed.") );
		goto done;
	}

	//
	// Create vertex buffer and index buffer
	//

	if( !CreateBuffers() )
	{
		WriteToLog( _T("CreateBuffers() failed.") );
		goto done;
	}

	//
	// Create the queries
	//

	queryDesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
	queryDesc.MiscFlags = 0;

	if( FAILED( hr = m_pDevice1->CreateQuery( &queryDesc, &m_pPipelineQuery ) ) ) 
	{
		LogError( __FILEW__, __LINE__, L"CreateQuery() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateQuery );
		EndSection();
		goto done;
	}

	// Execute the effective context
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
		goto done;
	}

	// If we got here, we passed
	bSuccess = true;

done:
	//delete [] pSrcTextureData;
	//delete [] pSrcRTTextureData;
	return bSuccess ? RESULT_PASS : RESULT_FAIL;
}

////

TEST_RESULT CLogicOpTest::SetupTestCase()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool bSuccess = false;
	D3D11_TEXTURE2D_DESC tex2DDesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;

	ZeroMemory( &tex2DDesc, sizeof( tex2DDesc ) );
	ZeroMemory( &rtvDesc, sizeof( rtvDesc ) );
	
	//
	// Create our Render Target Textures
	//

	tex2DDesc.ArraySize = 1;
	// Bind flag is filled out below
	// Cpu access flag filled out below
	// Format is filled out below
	tex2DDesc.Height = m_nPrimitiveH;
	tex2DDesc.MipLevels = 1;
	tex2DDesc.MiscFlags = 0;
	tex2DDesc.SampleDesc.Count = 1;
	tex2DDesc.SampleDesc.Quality = 0;
	// Usage is filled out below
	tex2DDesc.Width = m_nPrimitiveW;

	for( int i = 0; i < m_NumRTs; ++i )
	{
		assert( m_pTextureRTs[i] == NULL );

		// Change the RT Format based on the test case
		tex2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
		tex2DDesc.CPUAccessFlags = 0;
		tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
		tex2DDesc.Format = m_eRTFormats[i];
		assert( m_pTextureRTs[i] == NULL );
		if( FAILED( hr = m_pDevice1->CreateTexture2D( &tex2DDesc, NULL, &(m_pTextureRTs[i]) ) ) )
		{
			WriteToLog( _T("Creating Render Target %d with format %s."), i, ToString(tex2DDesc.Format).c_str() );
			LogError( __FILEW__, __LINE__, L"CreateTexture2D() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
			goto done;
		}

		// Create a staging copy of the RT
		tex2DDesc.BindFlags = 0;
		tex2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tex2DDesc.Usage = D3D11_USAGE_STAGING;
		assert( m_pTextureRTsStaging[i] == NULL );
		if( FAILED( hr = m_pDevice1->CreateTexture2D( &tex2DDesc, NULL, &(m_pTextureRTsStaging[i]) ) ) )
		{
			WriteToLog( _T("Creating Staging Resource for Render Target %d with format %s."), i, ToString(tex2DDesc.Format).c_str() );
			LogError( __FILEW__, __LINE__, L"CreateTexture2D() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
			goto done;
		}

		// Create the view for the render target
		rtvDesc.Format = tex2DDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		assert( m_pRTViews[i] == NULL );
		if( FAILED( hr = m_pDevice1->CreateRenderTargetView( m_pTextureRTs[i], &rtvDesc, &(m_pRTViews[i]) ) ) )
		{
			WriteToLog( _T("Creating Render Target View %d with format %s."), i, ToString(rtvDesc.Format).c_str() );
			LogError( __FILEW__, __LINE__, L"CreateRenderTargetView() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateView );
			goto done;
		}
	}

	//
	// Initialize the render targets with know data using our src texture
	// We use a Draw() here to have the HW perform the format conversion for us
	//

	BeginSection( L"Setting up the pipeline to Draw() and initalize the Render Target data." );

	// Setup input buffers
	const UINT stride = sizeof(LOGICOPVERTEX);
	const UINT offset = 0;
	m_pDeviceContext1->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_pDeviceContext1->IASetInputLayout( m_pLayout );
	m_pDeviceContext1->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pDeviceContext1->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Setup shaders
	m_pDeviceContext1->PSSetShaderResources( 0, 1, &m_pSrcTexture2DSRV );
	m_pDeviceContext1->VSSetShader( m_pVS, NULL, 0 );
	m_pDeviceContext1->GSSetShader( NULL, NULL, 0 );
	m_pDeviceContext1->PSSetShader( m_pPSInitRT, NULL, 0 );

	// Setup rasterizer
	m_pDeviceContext1->RSSetState( m_pRSEnabled );

	// Set blend state
	// IMPORTANT: We OR with a Zero'd RTV (See ClearRenderTargetView below) to get our known bit pattern in the RT
	// If this doesn't work then the test will fail because the bitpattern is assume to be in the RT and loaded from m_pSrcTextureData
	FLOAT blendFactor[4] = { 1, 1, 1, 1 };
	m_pDeviceContext1->OMSetBlendState( m_pLogicORBlendState, blendFactor, 0xffffffff );

	// Setup output render targets & clear them
	WriteToLog( _T("Binding and clearing %d Render Target Views."), m_NumRTs );
	ID3D11RenderTargetView* pViews[8];
	ZeroMemory( pViews, sizeof( pViews ) );
	memcpy( pViews, m_pRTViews, min( 8, m_NumRTs ) * sizeof( ID3D11RenderTargetView* ) );
	m_pDeviceContext1->OMSetRenderTargets( m_NumRTs, pViews, NULL );
	for (int rtNum = 0; rtNum < m_NumRTs; ++rtNum)
	{
		FLOAT colors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		if (m_pRTViews[rtNum] != NULL)
		{
			m_pDeviceContext1->ClearRenderTargetView( m_pRTViews[rtNum], colors );
		}
	}

	// Draw
	WriteToLog( _T("DrawIndexed with count = %d."), m_nIndexCount );
	m_pDeviceContext1->Begin( m_pPipelineQuery );
	m_pDeviceContext1->DrawIndexed(m_nIndexCount, 0, 0);
	m_pDeviceContext1->End( m_pPipelineQuery );

	// Execute the effective context
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
		EndSection();
		goto done;
	}

	// Check query data
	// We have to call get data on the immediate context
	// Note we don't check the HR - since it can fail
	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineStats;
	GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pPipelineQuery, &pipelineStats, sizeof( pipelineStats ), 0 );

	// TODO: Check pipeline stats
	
	// Discard the log output
	EndSection( false ); 

	//////
	////// Verify the render targets got the correct data
	//////

	//for( int i = 0; i < m_NumRTs; i++ )
	//{
	//	// Copy each RT to its staging copy
	//	m_pDeviceContext1->CopyResource( m_pTextureRTsStaging[i], m_pTextureRTs[i] );

	//	// Execute the effective context
	//	if( FAILED( hr = ExecuteEffectiveContext() ) )
	//	{
	//		LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceContextExecute );
	//		EndSection();
	//		goto done;
	//	}

	//	// Map the staging copy
	//	D3D11_MAPPED_SUBRESOURCE mappedSubres;
	//	if( FAILED( hr = m_pDeviceContext1->Map( m_pTextureRTsStaging[i], 0, D3D11_MAP_READ, 0, &mappedSubres ) ) )
	//	{
	//		LogError( __FILEW__, __LINE__, L"Map() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceContextMap );
	//		goto done;
	//	}

	//	// Perform check on the RT data
	//	UINT *pData = (UINT*)mappedSubres.pData;

	//	// Unmap the staging copy
	//	m_pDeviceContext1->Unmap( m_pTextureRTsStaging[i], 0 );
	//}

	// If we got here, we passed
	bSuccess = true;

done:
	return bSuccess ? RESULT_PASS : RESULT_FAIL;
}

////

TEST_RESULT CLogicOpTest::ExecuteTestCase()
{
	// At this point our src texture and render targets are filled with known data
	// Perform the logic op based on the testcase

	// Local variables
	HRESULT hr = E_FAIL;
	bool bSuccess = false;
	D3D11_BLEND_DESC1 blendDesc1;
	ID3D11BlendState1 *pBlendState = NULL;
	const bool bLogVerbose = GetFramework()->LogVerbose();

	ZeroMemory( &blendDesc1, sizeof( blendDesc1 ) );

	// Create Blend State based on the test case
	blendDesc1.AlphaToCoverageEnable = false;
	blendDesc1.IndependentBlendEnable = false;	// Logic ops must have this false
	blendDesc1.RenderTarget[0].BlendEnable = false;
	blendDesc1.RenderTarget[0].LogicOpEnable = true;
	blendDesc1.RenderTarget[0].LogicOp = m_logicOp;
	blendDesc1.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if( FAILED( hr = m_pDevice1->CreateBlendState1( &blendDesc1, &pBlendState ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateBlendState1() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateState );
		goto done;
	}

	// Set the new PS and blend state
	WriteToLog( _T("Setting new PS and BlendState with LogicOp") );
	FLOAT blendFactor[4] = { 1, 1, 1, 1 };
	m_pDeviceContext1->PSSetShaderResources( 0, 1, &m_pSrcTexture2DSRV );
	m_pDeviceContext1->PSSetShader( m_pPS, NULL, 0 );
	m_pDeviceContext1->OMSetBlendState( pBlendState, blendFactor, 0xffffffff );

	// Draw
	WriteToLog( _T("DrawIndexed with count = %d."), m_nIndexCount );
	m_pDeviceContext1->Begin( m_pPipelineQuery );
	m_pDeviceContext1->DrawIndexed(m_nIndexCount, 0, 0);
	m_pDeviceContext1->End( m_pPipelineQuery );

	// Check query data
	// We have to call get data on the immediate context
	// Note we don't check the HR - since it can fail
	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineStats;
	GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pPipelineQuery, &pipelineStats, sizeof( pipelineStats ), 0 );

	// TODO: Check pipeline stats


	//
	// Verify the render targets got the correct data
	//

	// Go through each RT...
	for( int i = 0; i < m_NumRTs; i++ )
	{
		WriteToLog( _T("Testing RT %d."), i );
		BeginSection( L"RT Validation" );

		// Reset the flag to indicate a failure in the current RT
		bSuccess = false;

		// Copy each RT to a staging resource
		m_pDeviceContext1->CopyResource( m_pTextureRTsStaging[i], m_pTextureRTs[i] );

		// Execute the effective context
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
			EndSection();
			goto done;
		}

		// Map the staging copy
		D3D11_MAPPED_SUBRESOURCE mappedSubres;
		if( FAILED( hr = GetFramework()->GetImmediateContext1()->Map( m_pTextureRTsStaging[i], 0, D3D11_MAP_READ, 0, &mappedSubres ) ) )
		{
			LogError( __FILEW__, __LINE__, L"Map() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextMap );
			EndSection();
			goto done;
		}

		// Get the current texture's description so we can get dimensions and format etc...
		D3D11_TEXTURE2D_DESC tex2DDesc;
		m_pTextureRTsStaging[i]->GetDesc( &tex2DDesc );
		assert( tex2DDesc.Format == m_eRTFormats[i] );
		const UINT elementByteWidth = max( GetBitsPerElement( tex2DDesc.Format ) / 8, 1 );
		const UINT numComponentsPerElement = CD3D10FormatHelper_Test::GetNumComponentsInFormat( tex2DDesc.Format );

		//
		// Check the converted RT against an expected value
		//

		WriteToLog( _T("RT %d mapped. pData = %#x. Row pitch = %d. DepthPitch = %d."), i, mappedSubres.pData, mappedSubres.RowPitch, mappedSubres.DepthPitch ); 
		WriteToLog( _T("Converting RT %d from %s to DXGI_FORMAT_R32_UINT."), i, ToString(tex2DDesc.Format).c_str() );
		WriteToLog( _T("%s element byte width = %u. Number of components = %u."), ToString(tex2DDesc.Format).c_str(), elementByteWidth, numComponentsPerElement );
		WriteToLog( _T("Checking contents of converted RT...") );

		UINT pComponentBits[4];
		for( UINT iCmp = 0; iCmp < 4; iCmp++ )
		{
			pComponentBits[iCmp] = CD3D10FormatHelper_Test::GetBitsPerComponent( tex2DDesc.Format, iCmp );
		}

		// Go through each row...
		for( UINT r = 0; r < tex2DDesc.Height; ++r )
		{
			// Go through each element...
			for( UINT c = 0; c < tex2DDesc.Width; ++c )
			{
				// Get a pointer to the start of the element and track what UINT32 block we are looking at in the element
				UINT uCurrentBit = 0;
				UINT32 *pStartOfElement = (UINT32*)((BYTE*)mappedSubres.pData + (r * mappedSubres.RowPitch) + (c * elementByteWidth));
				UINT32 *pCurrentDWORDInElement = (UINT32*) pStartOfElement;
				UINT32 zero = 0;

				if( elementByteWidth < 4 )
				{
					pCurrentDWORDInElement = &zero;
					memcpy( pCurrentDWORDInElement, pStartOfElement, elementByteWidth );
				}

				if( bLogVerbose )
				{
					WriteToLog( _T("Element [%u][%u] starts at %#x."), r, c, pStartOfElement );
				}

				// Go through each component...
				for( UINT iCmp = 0; iCmp < numComponentsPerElement; ++iCmp )
				{
					// Set a flag to indicated if the component matched the expected value
					bool bComponentFailed = true;

					// Check to see if the component is valid
					if( pComponentBits[iCmp] == 0 )
					{
						continue;
					}
					assert( (pComponentBits[iCmp] > 0) && (pComponentBits[iCmp] <= 32) );	// Every component should be less than 32 bits

					// See what bit in the element we are at next. Increment the UINT32 block if necessary
					if( uCurrentBit >= 32 )
					{
						if( bLogVerbose )
						{
							WriteToLog( _T("\tIncrementing current DWORD pointer within the element.") );
						}

						pCurrentDWORDInElement++;
						uCurrentBit = uCurrentBit % 32;
					}

					// Create mask for current component
					const UINT uNumCompBits = pComponentBits[iCmp];
					UINT32 uCompMask = 0;
					UINT32 uCompShift = uCurrentBit;
					for( UINT i = 0; i < uNumCompBits; ++i )
					{
						uCompMask = (uCompMask << 1) + 1;
					}
					uCompMask <<= uCompShift;

					// Grab the value
					UINT32 uCompData = (*pStartOfElement & uCompMask) >> uCompShift;

					//
					// Calculate the expected value
					//

					// Load the known src texture value that the PS loaded from the SRV
					// Load the known RTV value. Note if the Logical OR in SetupTestCase() didn't work, the real contents of the RTV wont match the expected content.
					const UINT32 srcTextureIndex = (r * tex2DDesc.Width) + c;
					const UINT32 srcRTTextureIndex = (c * tex2DDesc.Width) + r;	// Note: Reversed x, y. See PS.
					UINT psOutput = m_pSrcTextureData[srcTextureIndex];
					UINT rtvContents = m_pSrcTextureData[srcRTTextureIndex];
					UINT psOutputClamped = psOutput;
					UINT rtvContentsClamped = rtvContents;

					// Clamp the bits if needed
					if( uNumCompBits < 32 )
					{
						psOutputClamped = ClampUINT( psOutput, (UINT8)uNumCompBits );		// HW can either truncate or clamp the bits before the logic op
						rtvContentsClamped = ClampUINT( rtvContents, (UINT8)uNumCompBits );	// HW can either truncate or clamp the bits before the logic op
					}

					// Perform the logic operation using the PS output and the RT contents assuming the HW truncates
					// Note: uCompMask is the mask for the current DWORD, we need to shift it back to 0
					UINT32 expectedRTData = 0;
					if( !LogicOp( m_logicOp, psOutput, rtvContents, uCompMask >> uCompShift, expectedRTData ) )
					{
						WriteToLog( _T("Trying to perform LogicOp %s failed. Make sure the test is correct."), ToString(m_logicOp).c_str() );
						LogError( __FILEW__, __LINE__, L"LogicOp() failed.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
						EndSection();
						goto done;
					}

					// Perform the logic operation using the PS output and the RT contents assuming the HW clamps
					// Note: uCompMask is the mask for the current DWORD, we need to shift it back to 0
					UINT32 expectedRTDataClamped = 0;
					if( !LogicOp( m_logicOp, psOutputClamped, rtvContentsClamped, uCompMask >> uCompShift, expectedRTDataClamped ) )
					{
						WriteToLog( _T("Trying to perform LogicOp %s failed. Make sure the test is correct."), ToString(m_logicOp).c_str() );
						LogError( __FILEW__, __LINE__, L"LogicOp() failed.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
						EndSection();
						goto done;
					}

					// Check the expected result with the actual result
					if( uCompData == expectedRTData )
					{
						if( bLogVerbose )
						{
							WriteToLog( _T("\t[%u][%u], Component %u passed by truncating!"), r, c, iCmp );
						}
						bComponentFailed = false;
					}
					else if( uCompData == expectedRTDataClamped )
					{
						if( bLogVerbose )
						{
							WriteToLog( _T("\t[%u][%u], Component %u passed by clamping!"), r, c, iCmp );
						}
						bComponentFailed = false;
					}
					else
					{
						WriteToLog( _T("\t[%u][%u], Component %u failed!"), r, c, iCmp );
						bComponentFailed = true;
					}

					// Display detailed component info if request or the value didn't match the expected value
					if( bLogVerbose || bComponentFailed )
					{
						WriteToLog( _T("\t\tActual RT Content:") );
						WriteToLog( _T("\t\t\tCurrent bit in DWORD = %u."), uCurrentBit );
						WriteToLog( _T("\t\t\tCurrent component has %u bits. Component DWORD Mask = %#x. Component Shift = %u."), uNumCompBits, uCompMask, uCompShift );
						WriteToLog( _T("\t\t\t[%u][%u], Component %u = %u (%#010x)."), r, c, iCmp, uCompData, uCompData );

						// Print what should happen if the HW truncates to the RT format before the logic op (ideal)
						WriteToLog( _T("") );
						WriteToLog( _T("\t\tIf the HW truncates before the logic op (ideal):") );
						WriteToLog( _T("\t\tExpected RT Content before Draw():") );
						WriteToLog( _T("\t\t\t[%u][%u], Pixel Shader output = %u (%#010x)."), r, c, psOutput, psOutput );
						WriteToLog( _T("\t\t\t[%u][%u], Component %u RT Content = %u (%#010x)."), r, c, iCmp, rtvContents, rtvContents );
						WriteToLog( _T("\t\tExpected RT Content after Draw() with logic op (%s) applied:"), ToString(m_logicOp).c_str() );
						WriteToLog( _T("\t\t\t[%u][%u], Component %u = %u (%#010x)."), r, c, iCmp, expectedRTData, expectedRTData );

						// Print what should happen if the HW clamps to the RT format before the logic op (not ideal)
						WriteToLog( _T("") );
						WriteToLog( _T("\t\tIf the HW clamped to the RT format before the logic op (not ideal):") );
						WriteToLog( _T("\t\tExpected RT Content before Draw():") );
						WriteToLog( _T("\t\t\t[%u][%u], Pixel Shader output = %u (%#010x)."), r, c, psOutputClamped, psOutputClamped );
						WriteToLog( _T("\t\t\t[%u][%u], Component %u RT Content = %u (%#010x)."), r, c, iCmp, rtvContentsClamped, rtvContentsClamped );
						WriteToLog( _T("\t\tExpected RT Content after Draw() with logic op (%s) applied:"), ToString(m_logicOp).c_str() );
						WriteToLog( _T("\t\t\t[%u][%u], Component %u = %u (%#010x)."), r, c, iCmp, expectedRTDataClamped, expectedRTDataClamped );
					}

					// If we failed, stop checking results
					if( bComponentFailed )
					{
						// If not running LogVerbose, print a message
						if( !bLogVerbose )
						{
							WriteToLog( _T("You are not running the test with \"-logverbose\". This command line option will provide more detail about the RT. WARNING: This will produce massive log files.") );
						}

						// Display the expected result
						WriteToLog( _T("\t[%u][%u], Component %u (%#010x) failed to match one of the expected values (%#010x or %#010x). Exiting..."), r, c, iCmp, uCompData, expectedRTData, expectedRTDataClamped );

						goto doneWithCurrentStagingResource;
					}

					uCurrentBit += pComponentBits[ iCmp ];
				}
			}
		}

		// If we got here, the RT passed the checks
		bSuccess = true;

doneWithCurrentStagingResource:

		// Unmap the staging copy
		if( bLogVerbose )
		{
			WriteToLog( _T("Unmapping RT %d."), i );
		}

		GetFramework()->GetImmediateContext1()->Unmap( m_pTextureRTsStaging[i], 0 );
		
		if( !bSuccess )
		{
			WriteToLog( _T("RT %d failed. Exiting..."), i );
			EndSection();
			goto done;
		}

		// Discard log output
		EndSection(false);
		WriteToLog( _T("RT %d passed."), i );
	}

done:
	SAFE_RELEASE(pBlendState);

	return bSuccess ? RESULT_PASS : RESULT_FAIL;
}

////

void CLogicOpTest::CleanupTestCase()
{
	// Cleanup the Render Targets
	for( int i = 0; i < m_NumRTs; i++ )
	{
		SAFE_RELEASE( m_pTextureRTs[i] );
		SAFE_RELEASE( m_pTextureRTsStaging[i] );
		SAFE_RELEASE( m_pRTViews[i] );
	}

	// Call the parent cleanup
	CBlendConfTest::CleanupTestCase();
}

////

void CLogicOpTest::Cleanup()
{
	SAFE_RELEASE( m_pSrcTexture2D );
	SAFE_RELEASE( m_pSrcTexture2DSRV );
	SAFE_RELEASE( m_pPSInitRT );
	SAFE_RELEASE( m_pPipelineQuery );
	SAFE_RELEASE( m_pLogicORBlendState );

	delete [] m_pSrcTextureData;

	// Call the parent cleanup
	CBlendConfTest::Cleanup();
}

////

bool CLogicOpTest::CreateShaders()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool bSuccess = false;
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	
	// Cleanup existing shaders and input layout
	SAFE_RELEASE(m_pLayout);
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pPSInitRT);

	if (m_sPixelShaderName.size() > 0)
	{
		// Compile the pixel shader
		WriteToLog( _T("Compiling PS from resource: %s"), m_sPixelShaderName.c_str() );
		WriteToLog( _T("Entry point: %s"), m_sPSEntryPt.c_str() );
		hr = D3DX10CompileFromResource(NULL, m_sPixelShaderName.c_str(), NULL, NULL, NULL, m_sPSEntryPt.c_str(), "ps_4_0", D3D10_SHADER_DEBUG, 0, NULL, &pShaderBuf, &pErrorBuf, NULL);
		if( FAILED( hr ) )
		{
			LogError( __FILEW__, __LINE__, L"D3DX10CompileFromResource() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3DX::gFCCompileShader );

			// Print out the compiler error if available
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
			goto done;
		}

		// Create the pixel shader
		if( FAILED( hr = m_pDevice1->CreatePixelShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, &m_pPS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreatePixelShader() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader );
			goto done;
		}

		assert( m_pPS );
		SAFE_RELEASE(pShaderBuf);
		SAFE_RELEASE(pErrorBuf);

		// Compile the pixel shader to init the render targets
		WriteToLog( _T("Compiling PS from resource: %s"), m_sPixelShaderName.c_str() );
		WriteToLog( _T("Entry point: %s"), m_sPSInitRTEntryPt.c_str() );
		hr = D3DX10CompileFromResource(NULL, m_sPixelShaderName.c_str(), NULL, NULL, NULL, m_sPSInitRTEntryPt.c_str(), "ps_4_0", D3D10_SHADER_DEBUG, 0, NULL, &pShaderBuf, &pErrorBuf, NULL);
		if( FAILED( hr ) )
		{
			LogError( __FILEW__, __LINE__, L"D3DX10CompileFromResource() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3DX::gFCCompileShader );

			// Print out the compiler error if available
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
			goto done;
		}

		// Create the pixel shader
		if( FAILED( hr = m_pDevice1->CreatePixelShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, &m_pPSInitRT ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreatePixelShader() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader );
			goto done;
		}

		assert( m_pPSInitRT );
		SAFE_RELEASE(pShaderBuf);
		SAFE_RELEASE(pErrorBuf);
	}
	else
	{
		WriteToLog( _T("No Pixel Shader was specified to be created.") );
	}

	if (m_sVertexShaderName.size() > 0)
	{
		// Compile the vertex shader
		WriteToLog( _T("Compiling VS from resource: %s"), m_sVertexShaderName.c_str() );
		hr = D3DX10CompileFromResource(NULL, m_sVertexShaderName.c_str(), NULL, NULL, NULL, "VSMain", "vs_4_0", D3D10_SHADER_DEBUG, 0, NULL, &pShaderBuf, &pErrorBuf, NULL);
		if( FAILED( hr ) )
		{
			LogError( __FILEW__, __LINE__, L"D3DX10CompileFromResource() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3DX::gFCCompileShader );

			// Print out the compiler error if available
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
			goto done;
		}

		// Create the vertex shader
		if( FAILED( hr = m_pDevice1->CreateVertexShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, &m_pVS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateVertexShader() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader );
			goto done;
		}

		// Create the input layout
		D3D11_INPUT_ELEMENT_DESC pInputElementDesc[] =
		{
			{"pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},			//position
			{"texCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},	//texture coord to load from the src texture
		};
		const UINT numElements = sizeof(pInputElementDesc) / sizeof(*pInputElementDesc);

		if(	FAILED(	hr = m_pDevice1->CreateInputLayout( pInputElementDesc, numElements, pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pLayout ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateInputLayout() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateInputLayout );
			goto done;
		}

		assert( m_pVS );
		assert( m_pLayout );
		SAFE_RELEASE(pShaderBuf);
		SAFE_RELEASE(pErrorBuf);
	}
	else
	{
		WriteToLog( _T("No Vertex Shader was specified to be created.") );
	}

	// If we got here, we passed
	bSuccess = true;

done:
	SAFE_RELEASE( pShaderBuf );
	SAFE_RELEASE( pErrorBuf );
	return bSuccess;
}

////


bool CLogicOpTest::CreateBuffers()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool bSuccess = false;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA subresData;

	// Cleanup the existing buffers
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	ZeroStructMem(bufferDesc);
	ZeroStructMem(subresData);

	// Create the vertex buffer
	// 0--1
	// |\ |
	// | \|
	// 3--2

	LOGICOPVERTEX vertices[] = 
	{
		// PosX	PosY	TexCoordX	TexCoordY
		{-1,	1,		0,			0},	// 0
		{1,		1,		1,			0},	// 1
		{1,		-1,		1,			1},	// 2
		{-1,	-1,		0,			1},	// 3
	};
	
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subresData.pSysMem = vertices;
	subresData.SysMemPitch = 0;
	subresData.SysMemSlicePitch = 0;
	if( FAILED( hr = m_pDevice1->CreateBuffer(&bufferDesc, &subresData, &m_pVertexBuffer) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateBuffer() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
		goto done;
	}

	// Create the index buffer
	UINT16 indices[] = 
	{	
		0,1,2, 2,3,0,
	};
	m_nIndexCount = sizeof(indices) / sizeof(*indices);
	ZeroStructMem(bufferDesc);
	ZeroStructMem(subresData);

	bufferDesc.ByteWidth = sizeof(indices);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subresData.pSysMem = indices;
	subresData.SysMemPitch = 0;
	subresData.SysMemSlicePitch = 0;
	if( FAILED( hr = m_pDevice1->CreateBuffer(&bufferDesc, &subresData, &m_pIndexBuffer) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateBuffer() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
		goto done;
	}

	// If we got here, we passed
	bSuccess = true;
	assert( m_pVertexBuffer );
	assert( m_pIndexBuffer );

done:
	return bSuccess;
}

////

bool CLogicOpTest::LogicOp( const D3D11_LOGIC_OP &logicOp, const UINT32 &psOutput, const UINT32 &rtvContents, const UINT32 &uCompMask, UINT32 &expectedRTData ) const
{
	// Local variables
	bool bSuccess = false;

	// Operation: (s == PS output, d = RTV contents)
	switch( m_logicOp )
	{
	default:
		{
			assert( !"Invalid LogicOP" );
			goto done;
			break;
		}

	case D3D11_LOGIC_OP_CLEAR:			// 0
		{
			expectedRTData = 0;
			break;
		}

    case D3D11_LOGIC_OP_SET:            // 1
		{
			expectedRTData = 0xFFFFFFFF;
			break;
		}

    case D3D11_LOGIC_OP_COPY:           // s
		{
			expectedRTData = psOutput;
			break;
		}
    case D3D11_LOGIC_OP_COPY_INVERTED:  // ~s
		{
			expectedRTData = ~(psOutput);
			break;
		}
    case D3D11_LOGIC_OP_NOOP:           // d
		{
			expectedRTData = rtvContents;
			break;
		}
    case D3D11_LOGIC_OP_INVERT:         // ~d
		{
			expectedRTData = ~(rtvContents);
			break;
		}
    case D3D11_LOGIC_OP_AND:            // s & d
		{
			expectedRTData = psOutput & rtvContents;
			break;
		}
    case D3D11_LOGIC_OP_NAND:           // ~(s & d)
		{
			expectedRTData = ~(psOutput & rtvContents);
			break;
		}
    case D3D11_LOGIC_OP_OR:             // s | d
		{
			expectedRTData = psOutput | rtvContents;
			break;
		}
    case D3D11_LOGIC_OP_NOR:            // ~(s | d)
		{
			expectedRTData = ~(psOutput | rtvContents);
			break;
		}
    case D3D11_LOGIC_OP_XOR:            // s ^ d
		{
			expectedRTData = psOutput ^ rtvContents;
			break;
		}
    case D3D11_LOGIC_OP_EQUIV:          // ~(s ^ d)
		{
			expectedRTData = ~(psOutput ^ rtvContents);
			break;
		}
    case D3D11_LOGIC_OP_AND_REVERSE:    // s & ~d
		{
			expectedRTData = psOutput & (~rtvContents);
			break;
		}
    case D3D11_LOGIC_OP_AND_INVERTED:   // ~s & d
		{
			expectedRTData = ~(psOutput) & rtvContents;
			break;
		}
	case D3D11_LOGIC_OP_OR_REVERSE:     // s | ~d
		{
			expectedRTData = psOutput | ~(rtvContents);
			break;
		}
    case D3D11_LOGIC_OP_OR_INVERTED:    // ~s | d
		{
			expectedRTData = ~(psOutput) | rtvContents;
			break;
		}
	};

	// If we got here, we passed
	bSuccess = true;

	// Mask off the bits of the expected result
	expectedRTData = expectedRTData & uCompMask;

done:
	return bSuccess;
}
