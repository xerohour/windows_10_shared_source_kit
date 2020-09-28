//////////////////////////////////////////////////////////////////////////
//  depth.cpp
//  created:	2006/11/10
//
//  purpose:    base class for all depth tests
//////////////////////////////////////////////////////////////////////////

#include "depth.h"
#include "testapp.h"

// string converstions for custom types
tstring __stdcall ToString( PSType type );
bool __stdcall FromString( PSType *type, const tstring &str );

// data for table value sets
UINT Widths[] = { 128, 5 };
UINT Heights[] = { 2, 256 };
UINT ArraySizes[] = { 2 };
PSType PSTypes[] = { DEFAULT, ODEPTH, NONE };

extern CDepthStencilTestApp g_TestApp;

CDepthTest::CDepthTest()
: m_bCube(false),
  m_uMipSlice(0),
  m_uArraySlice(0),
  m_pDSTex(NULL),
  m_pDSTexStaging(NULL),
  m_pRTTex(NULL),
  m_pDSV(NULL),
  m_pDSVWriteable(NULL),
  m_pRTV(NULL),
  m_pDSS(NULL),
  m_pVB(NULL),
  m_pIL(NULL),
  m_pCB(NULL),
  m_pRS(NULL),
  m_pVS(NULL),
  m_pVSODepth(NULL),
  m_pGSTri(NULL),
  m_pGSPoint(NULL),
  m_pPS(NULL),
  m_pPSODepth(NULL)
{
	//
	// Depth Desc
	//
	m_DSTex2DDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
	m_DSTex2DDesc.ArraySize			= 1;
	m_DSTex2DDesc.CPUAccessFlags	= 0;
	m_DSTex2DDesc.Format			= DXGI_FORMAT_D32_FLOAT;
	m_DSTex2DDesc.MipLevels			= 1;
	m_DSTex2DDesc.MiscFlags			= 0;
	m_DSTex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
	m_DSTex2DDesc.SampleDesc.Count	= 1;
	m_DSTex2DDesc.SampleDesc.Quality = 0;
	m_DSTex2DDesc.Width				= 1;
	m_DSTex2DDesc.Height			= 1;

	//
	// Depth State Desc
	// 
	m_DSDesc.DepthEnable	= true;
	m_DSDesc.DepthFunc		= D3D11_COMPARISON_ALWAYS;
	m_DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	m_DSDesc.StencilEnable  = false;

	//
	// Depth View Description
	//
	m_DSVDesc.Flags = 0;

	//
	// Framework Settings
	//

    // Cache framework settings changed by this test
    m_origRenderTargetOptions.NoDepthStencils       = g_TestApp.m_RenderTargetOptions.NoDepthStencils;
    m_origRenderTargetOptions.NoRenderTargets       = g_TestApp.m_RenderTargetOptions.NoRenderTargets;
    m_origRenderTargetOptions.NoSwapChains          = g_TestApp.m_RenderTargetOptions.NoSwapChains;

	g_TestApp.m_RenderTargetOptions.NoDepthStencils = true;
	g_TestApp.m_RenderTargetOptions.NoRenderTargets = true;
	g_TestApp.m_RenderTargetOptions.NoSwapChains = true;
}

CDepthTest::~CDepthTest()
{
    g_TestApp.m_RenderTargetOptions.NoDepthStencils       = m_origRenderTargetOptions.NoDepthStencils;
    g_TestApp.m_RenderTargetOptions.NoRenderTargets       = m_origRenderTargetOptions.NoRenderTargets;
    g_TestApp.m_RenderTargetOptions.NoSwapChains          = m_origRenderTargetOptions.NoSwapChains;
}

// inits common test factors
void CDepthTest::InitDepthTestParameters()
{
	//
	// Depth Formats
	//

	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T("DepthFormat"), &m_DSTex2DDesc.Format );
	CUserValueSet<DXGI_FORMAT> *pFormatSet = new CUserValueSet<DXGI_FORMAT>();
	pFormatSet->AddValue(DXGI_FORMAT_D24_UNORM_S8_UINT);
	pFormatSet->AddValue(DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
	testfactor::RFactor rfFormatWithStencil = AddParameterValueSet( pFormatParam, pFormatSet );

	pFormatSet->AddValue(DXGI_FORMAT_D32_FLOAT);
	pFormatSet->AddValue(DXGI_FORMAT_D16_UNORM);
	testfactor::RFactor rfFormat = AddParameterValueSet( pFormatParam, pFormatSet );

	//
	// Pixel Shader
	//
	CTestCaseParameter<PSType> *pPSParam = AddParameter( _T( "PixelShader" ), &m_PSType );
	CUserValueSet<PSType> *pPSSet = new CUserValueSet<PSType>();
	pPSSet->AddValue(NONE);
	pPSSet->AddValue(DEFAULT);
	pPSSet->AddValue(ODEPTH);
	testfactor::RFactor rfPS = AddParameterValueSet( pPSParam, pPSSet );
	testfactor::RFactor rfPSNone = AddParameterValue( pPSParam, NONE );

	//
	// Texture dimension
	//

	CTestCaseParameter<D3D11_DSV_DIMENSION> *pDimParam = AddParameter( _T( "Dimension" ), &m_DSVDesc.ViewDimension );
	testfactor::RFactor rfTex1D = AddParameterValue( pDimParam, D3D11_DSV_DIMENSION_TEXTURE1D );
	testfactor::RFactor rfTex1DArray = AddParameterValue( pDimParam, D3D11_DSV_DIMENSION_TEXTURE1DARRAY );
	testfactor::RFactor rfTex2D = AddParameterValue( pDimParam, D3D11_DSV_DIMENSION_TEXTURE2D );
	testfactor::RFactor rfTex2DArray = AddParameterValue( pDimParam, D3D11_DSV_DIMENSION_TEXTURE2DARRAY );

	// only use on tex2d array with array size of 6
	CTestCaseParameter<bool> *pCubeParam = AddParameter( _T( "Cube" ), &m_bCube );
	testfactor::RFactor rfCube = AddParameterValue( pCubeParam, true );
	pCubeParam->SetDefaultValue(false);

	//
	// Width and Height
	//

	// width

	CTestCaseParameter<UINT> *pWidthParam = AddParameter( _T( "Width" ), &m_DSTex2DDesc.Width );
	CTableValueSet<UINT> *pWidthValues = new CTableValueSet< UINT >( Widths, sizeof(UINT), ARRAY_SIZE( Widths ) );
	testfactor::RFactor rfWidth = AddParameterValueSet( pWidthParam, pWidthValues );

	// height
	CTestCaseParameter<UINT> *pHeightParam = AddParameter( _T( "Height" ), &m_DSTex2DDesc.Height );
	CTableValueSet<UINT> *pHeightValues = new CTableValueSet< UINT >( Heights, sizeof(UINT), ARRAY_SIZE( Heights ) );
	testfactor::RFactor rfHeight = AddParameterValueSet( pHeightParam, pHeightValues );
	testfactor::RFactor rfHeightOne = AddParameterValue( pHeightParam, (UINT)1 );
	pHeightParam->SetDefaultValue(1);

	// factors that include width, height, miplevels, and mipslice
	testfactor::RFactor rfAll2DSizes = (rfWidth % rfHeight);
	testfactor::RFactor rfAll1DSizes = (rfWidth * rfHeightOne);


	//
	// Mips
	//

	// optional not included in factor - defaults to 0 miplevels and 0 mipslice

	// mips - combination of mip levels and mip slice
	CTestCaseParameter<UINT> *pMipLevelsParam = AddParameter( _T( "MipLevels" ), &m_DSTex2DDesc.MipLevels );
	CTestCaseParameter<UINT> *pMipSliceParam = AddParameter( _T( "MipSlice" ), &m_uMipSlice );
	testfactor::RFactor rfAll2DSizesAllMips = AddParameterValue( pWidthParam, (UINT)1 ) * AddParameterValue( pHeightParam, (UINT)1 );
	testfactor::RFactor rfAll1DSizesAllMips = AddParameterValue( pWidthParam, (UINT)1 );
	testfactor::RFactor rfAll2DCubeSizesAllMips = AddParameterValue( pWidthParam, (UINT)1 ) * AddParameterValue( pHeightParam, (UINT)1 );
	testfactor::RFactor rfDefault2DSizeDefaultMips = AddParameterValue( pWidthParam, (UINT)16 ) * AddParameterValue( pHeightParam, (UINT)2 );
	testfactor::RFactor rfDefault1DSizeDefaultMips = AddParameterValue( pWidthParam, (UINT)16 );
	testfactor::RFactor rfDefault2DCubeSizeDefaultMips = AddParameterValue( pWidthParam, (UINT)16 ) * AddParameterValue( pHeightParam, (UINT)16 );
	pMipLevelsParam->SetDefaultValue(1);
	pMipSliceParam->SetDefaultValue(0);
	for (UINT i = 0; i < ARRAY_SIZE( Widths ); ++i)
	{
		// calculate max # of mips for 1d and 2d
		UINT uDim2D = min(Widths[i], Heights[i]);
		UINT uDim1D = Widths[i];
		UINT uMaxMipLevels2D = 1;
		UINT uMaxMipLevels1D = 1;
		while (uDim2D = uDim2D >> 1) uMaxMipLevels2D++;
		while (uDim1D = uDim1D >> 1) uMaxMipLevels1D++;
		
		// texture2d
		testfactor::RFactor rfWidth = AddParameterValue( pWidthParam, Widths[i] );
		testfactor::RFactor rfHeight = AddParameterValue( pHeightParam, Heights[i] );
		testfactor::RFactor rfMips = AddParameterValue( pMipLevelsParam, (UINT)1 ) * AddParameterValue( pMipSliceParam, (UINT)0 );
		for (UINT j = 2; j < uMaxMipLevels2D; ++j)
		{
			// !Speedup
			// only keep max, min, or middle mip values
			UINT uMax = uMaxMipLevels2D - 1;
			UINT uMid = (uMaxMipLevels2D - 1)/2;
			if ((j != uMax) && (j != uMid)) continue;   // skip the rest
			if ((j == uMid) && (uMid < 2)) continue;	// only keep mid if not equal to min

			rfMips = rfMips + AddParameterValue( pMipLevelsParam, j ) *
							( AddParameterValue( pMipSliceParam, (UINT)0 ) + AddParameterValue( pMipSliceParam, j-1 ) );
			if (j > 2)
				rfMips = rfMips + AddParameterValue( pMipLevelsParam, j ) * 
								  AddParameterValue( pMipSliceParam, j/(UINT)2 );
		}
		rfAll2DSizesAllMips = rfAll2DSizesAllMips + (rfMips * rfWidth * rfHeight);
		if (Widths[i] == Heights[i])
			rfAll2DCubeSizesAllMips = rfAll2DCubeSizesAllMips + (rfWidth * rfHeight * rfMips);

		// texture1d
		rfMips = AddParameterValue( pMipLevelsParam, (UINT)1 ) * AddParameterValue( pMipSliceParam, (UINT)0 );
		for (UINT j = 2; j < uMaxMipLevels1D; ++j)
		{
			// !Speedup
			// only keep max, min, or middle mip values
			UINT uMax = uMaxMipLevels2D - 1;
			UINT uMid = (uMaxMipLevels2D - 1)/2;
			if ((j != uMax) && (j != uMid)) continue;   // skip the rest
			if ((j == uMid) && (uMid < 2)) continue;	// only keep mid if not equal to min

			rfMips = rfMips + AddParameterValue( pMipLevelsParam, j ) *
							( AddParameterValue( pMipSliceParam, (UINT)0 ) + AddParameterValue( pMipSliceParam, j-1 ) );
			if (j > 2)
				rfMips = rfMips + AddParameterValue( pMipLevelsParam, j ) * 
								  AddParameterValue( pMipSliceParam, j/(UINT)2 );
		}
		rfAll1DSizesAllMips = rfAll1DSizesAllMips + (rfWidth * rfHeightOne * rfMips);
	}

	//
	// Arrays
	//
	
	// optional not included in factor - defaults to 0 size and 0 slice

	// combination of array slice and array size
	CTestCaseParameter<UINT> *pArraySizeParam = AddParameter( _T( "ArraySize" ), &m_DSTex2DDesc.ArraySize );
	CTestCaseParameter<UINT> *pArraySliceParam = AddParameter( _T( "ArraySlice" ), &m_uArraySlice );
	testfactor::RFactor rfArraySizeFour = AddParameterValue( pArraySizeParam, (UINT)4 );
	testfactor::RFactor rfArraySliceTwo = AddParameterValue( pArraySliceParam, (UINT)2 );
	testfactor::RFactor rfArraySingle = rfArraySizeFour * rfArraySliceTwo;
	pArraySizeParam->SetDefaultValue(1);
	pArraySliceParam->SetDefaultValue(0);
		 
	testfactor::RFactor rfArrayDefault = AddParameterValue( pArraySizeParam, (UINT)1 ) * AddParameterValue( pArraySliceParam, (UINT)0 );
	testfactor::RFactor rfArrayAll = AddParameterValue( pArraySizeParam, (UINT)1 ) * AddParameterValue( pArraySliceParam, (UINT)0 );
	for (UINT i = 0; i < ARRAY_SIZE( ArraySizes ); ++i)
	{
		testfactor::RFactor rfArrays = AddParameterValue( pArraySizeParam, ArraySizes[i] ) *
									   AddParameterValueSet( pArraySliceParam, new CRangeValueSet<UINT>( 0, ArraySizes[i]-1, 1 ) );
		rfArrayAll = rfArrayAll + rfArrays;
	}
	testfactor::RFactor rfArrayCubeDefault = AddParameterValue( pArraySizeParam, (UINT)6 ) * AddParameterValue( pArraySliceParam, (UINT)0 );
	testfactor::RFactor rfArrayCube = AddParameterValue( pArraySizeParam, (UINT)6 ) *
									  AddParameterValueSet( pArraySliceParam, new CRangeValueSet<UINT>( 0, 5, 5 ) );

	//
	// Factor groups
	//
	m_rfAll = 
	  ( rfPSNone * rfTex1D * rfAll1DSizesAllMips +
		rfPSNone * rfTex1DArray * rfAll1DSizesAllMips * rfArrayAll +
		rfPS     * rfTex2D * rfAll2DSizesAllMips +
		rfPSNone * rfTex2DArray * rfAll2DSizesAllMips * rfArrayAll +
		rfPSNone * rfCube * rfTex2DArray * rfAll2DCubeSizesAllMips * rfArrayCube ) * rfFormat;

	m_rfReduced = 
	  ( rfPSNone * rfTex1D * rfDefault1DSizeDefaultMips +
		rfPSNone * rfTex1DArray * rfDefault1DSizeDefaultMips * rfArrayDefault +
		rfPS     * rfTex2D * rfDefault2DSizeDefaultMips +
		rfPSNone * rfTex2DArray * rfDefault2DSizeDefaultMips * rfArrayDefault +
		rfPSNone * rfCube * rfTex2DArray * rfDefault2DCubeSizeDefaultMips * rfArrayCubeDefault ) * rfFormat;

	m_rfReducedWithStencil = 
	  ( rfPSNone * rfTex1D * rfDefault1DSizeDefaultMips +
		rfPSNone * rfTex1DArray * rfDefault1DSizeDefaultMips * rfArrayDefault +
		rfPS     * rfTex2D * rfDefault2DSizeDefaultMips +
		rfPSNone * rfTex2DArray * rfDefault2DSizeDefaultMips * rfArrayDefault +
		rfPSNone * rfCube * rfTex2DArray * rfDefault2DCubeSizeDefaultMips * rfArrayCubeDefault ) * rfFormatWithStencil;
}

// initialization for features that don't change between test cases
void CDepthTest::SetupCommon()
{
	TEST_RESULT tRes = RESULT_PASS;

	try
	{
		//
		// DepthStencil state
		//
		if( FAILED( GetDevice()->CreateDepthStencilState( &m_DSDesc, &m_pDSS ) ) )
			throw TRException("CreateDepthStencilState() failed.");
		GetEffectiveContext()->OMSetDepthStencilState( m_pDSS, 0 );

		//
		// Rasterizer State
		//
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
		if( FAILED( GetDevice()->CreateRasterizerState( &rastDesc, &m_pRS ) ) )
			throw TRException("CreateRasterizerState for m_pRS failed.");
		GetEffectiveContext()->RSSetState( m_pRS );
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		//
		// Blend State
		//
		GetEffectiveContext()->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );

		//
		// Create the constant buffer
		//
		D3D11_BUFFER_DESC bufdesc;
		bufdesc.ByteWidth = 512;  // extreme over estimate
		bufdesc.Usage = D3D11_USAGE_DYNAMIC;
		bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufdesc.MiscFlags = 0;
		if( FAILED( GetDevice()->CreateBuffer( &bufdesc, NULL, &m_pCB ) ) )
			throw TRException("CreateBuffer for m_pCB failed.");
		FillConstantBuffer();
		GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pCB );
		GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pCB );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pCB );

		HRESULT hr;
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context.") );
		}

		//
		// Shaders
		//
		SetupShaders();
	}
	catch(TRException &exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CDepthTest::SetupCommon()] ");
		throw exc;  // pass it up the stack
	}
}

void CDepthTest::SetupShaders()
{
	HRESULT hr;
	ID3D10Blob *pShader = NULL;

	//
	// Vertex Shaders
	//

	// pass through shader
	CompileShader(NULL, "depth.vsh", "main", "vs_4_0", D3D10_SHADER_DEBUG, &pShader);
	if ( FAILED( GetDevice()->CreateVertexShader( (DWORD*) pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, &m_pVS ) ) )
		throw TRException("CreateVertexShader Failed.");
	SAFE_RELEASE(pShader);

	// pass through odepth shader
	CompileShader(NULL, "odepth.vsh", "main", "vs_4_0", D3D10_SHADER_DEBUG, &pShader);
	if ( FAILED( GetDevice()->CreateVertexShader( (DWORD*) pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, &m_pVSODepth ) ) )
		throw TRException("CreateVertexShader Failed.");

 	//  vertex data layout
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "pos", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if(	FAILED(	GetDevice()->CreateInputLayout( layout, 1, pShader->GetBufferPointer(), pShader->GetBufferSize(), &m_pIL ) ) )
		throw TRException("CreateInputLayout for m_pIL Failed.");
	GetEffectiveContext()->IASetInputLayout( m_pIL );

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
	}

	SAFE_RELEASE(pShader);

	//
	// Geometry Shaders
	//

	// pass through shader
	CompileShader(NULL, "depth_tri.gsh", "main", "gs_4_0", D3D10_SHADER_DEBUG, &pShader);
	if ( FAILED( GetDevice()->CreateGeometryShader( (DWORD*) pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, &m_pGSTri ) ) )
		throw TRException("CreateVertexShader Failed.");
	SAFE_RELEASE(pShader);

	// pass through odepth shader
	CompileShader(NULL, "depth_point.gsh", "main", "gs_4_0", D3D10_SHADER_DEBUG, &pShader);
	if ( FAILED( GetDevice()->CreateGeometryShader( (DWORD*) pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, &m_pGSPoint ) ) )
		throw TRException("CreateVertexShader Failed.");
	SAFE_RELEASE(pShader);

	//
	// Pixel Shaders
	//

	// pass through shader
	CompileShader(NULL, "depth.psh", "main", "ps_4_0", D3D10_SHADER_DEBUG, &pShader);
	if ( FAILED( GetDevice()->CreatePixelShader( (DWORD*) pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, &m_pPS ) ) )
		throw TRException("CreateVertexShader Failed.");
	SAFE_RELEASE(pShader);

	// pass through odepth shader
	CompileShader(NULL, "odepth.psh", "main", "ps_4_0", D3D10_SHADER_DEBUG, &pShader);
	if ( FAILED( GetDevice()->CreatePixelShader( (DWORD*) pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, &m_pPSODepth ) ) )
		throw TRException("CreateVertexShader Failed.");
	SAFE_RELEASE(pShader);
}

void CDepthTest::CompileShader(HMODULE hModule, LPCSTR pResource, LPCSTR pFunctionName, LPCSTR pProfile, UINT uFlags, ID3D10Blob **ppShader)
{
	assert(ppShader);

	ID3D10Blob *pError;
	HRESULT hr = D3DX10CompileFromResource( NULL, pResource, NULL, NULL, NULL, pFunctionName, pProfile, uFlags, 0, NULL, ppShader, &pError, NULL );

	if( pError )
		WriteToLog( _T( "%s" ), pError->GetBufferPointer() );

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CompileShaderFromResource - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		WriteToLog( _T( "Shader file '%s'" ), pResource );
		SAFE_RELEASE( (*ppShader) );
		SAFE_RELEASE( pError );

		throw TRException("[CDepthTest::SetupShaders] CompileShaderFromResource Failed.");
	}
}

void CDepthTest::SetupTestCaseCommon()
{
	TEST_RESULT tRes = RESULT_PASS;

	try
	{
		// calculate subresource width and height
		m_uSubResWidth = m_DSTex2DDesc.Width >> m_uMipSlice;
		if ( (m_DSVDesc.ViewDimension == D3D11_DSV_DIMENSION_TEXTURE2D) || 
			 (m_DSVDesc.ViewDimension == D3D11_DSV_DIMENSION_TEXTURE2DARRAY) )
			m_uSubResHeight = m_DSTex2DDesc.Height >> m_uMipSlice;
		else
			m_uSubResHeight = 1;

		// cube
		m_DSTex2DDesc.MiscFlags = (m_bCube? D3D11_RESOURCE_MISC_TEXTURECUBE : 0);

		//
		// Setup the default viewport
		//
		D3D11_VIEWPORT vp;
		vp.Height = (float) m_uSubResHeight;
		vp.Width = (float) m_uSubResWidth;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		GetEffectiveContext()->RSSetViewports( 1, &vp );

		HRESULT hr;
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context.") );
		}

		SetupTextures();
		SetupVB();
		FillConstantBuffer();

		//
		// Select shaders
		//
		switch (m_PSType)
		{
		case NONE:
			GetEffectiveContext()->VSSetShader(m_pVS, NULL, 0);
			GetEffectiveContext()->GSSetShader(m_pGSTri, NULL, 0);
			GetEffectiveContext()->PSSetShader(NULL, NULL, 0);
			break;
		case DEFAULT:
			GetEffectiveContext()->VSSetShader(m_pVS, NULL, 0);
			GetEffectiveContext()->GSSetShader(m_pGSTri, NULL, 0);
			GetEffectiveContext()->PSSetShader(m_pPS, NULL, 0);
			break;
		case ODEPTH:
			GetEffectiveContext()->VSSetShader(m_pVSODepth, NULL, 0);
			GetEffectiveContext()->GSSetShader(m_pGSTri, NULL, 0);
			GetEffectiveContext()->PSSetShader(m_pPSODepth, NULL, 0);
			break;
		}
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context.") );
		}

	}
	catch(TRException &exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CDepthTest::SetupCommon()] ");
		throw exc;  // pass it up the stack
	}
}

void CDepthTest::SetupTextures()
{
	D3D11_TEXTURE1D_DESC tex1DDesc;
	D3D11_TEXTURE2D_DESC tex2DDesc;
	HRESULT hr;

	//
	//	Depth/Stencil Texture
	//
	switch( m_DSVDesc.ViewDimension )
	{
	case D3D11_DSV_DIMENSION_TEXTURE1D:
	case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
		
		tex1DDesc.ArraySize			= m_DSTex2DDesc.ArraySize;
		tex1DDesc.BindFlags			= m_DSTex2DDesc.BindFlags;
		tex1DDesc.CPUAccessFlags	= m_DSTex2DDesc.CPUAccessFlags;
		tex1DDesc.Format			= m_DSTex2DDesc.Format;
		tex1DDesc.MipLevels			= m_DSTex2DDesc.MipLevels;
		tex1DDesc.MiscFlags			= m_DSTex2DDesc.MiscFlags;
		tex1DDesc.Usage				= m_DSTex2DDesc.Usage;
		tex1DDesc.Width				= m_DSTex2DDesc.Width;
		if( FAILED( hr = GetDevice()->CreateTexture1D( &tex1DDesc, NULL, (ID3D11Texture1D**) &m_pDSTex ) ) )
		{
			WriteToLog( _T( "CreateTexture1D() result code '%s'" ), D3DHResultToString( hr ).c_str() );
			throw TRException("[CDepthTest::SetupTextures] CreateTexture1D failed for depth texture.");
		}
		
		tex1DDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;
		tex1DDesc.BindFlags			= 0;
		tex1DDesc.Usage				= D3D11_USAGE_STAGING;
		if( FAILED( hr = GetDevice()->CreateTexture1D( &tex1DDesc, NULL, (ID3D11Texture1D**) &m_pDSTexStaging ) ) )
		{
			WriteToLog( _T( "CreateTexture1D() result code '%s'" ), D3DHResultToString( hr ).c_str() );
			throw TRException("[CDepthTest::SetupTextures] CreateTexture1D failed for depth staging texture.");
		}

		// render target
		tex1DDesc.CPUAccessFlags	= 0;
		tex1DDesc.BindFlags			= D3D11_BIND_RENDER_TARGET;
		tex1DDesc.Usage				= D3D11_USAGE_DEFAULT;
		tex1DDesc.Format			= DXGI_FORMAT_R8G8B8A8_UNORM;
		if( FAILED( hr = GetDevice()->CreateTexture1D( &tex1DDesc, NULL, (ID3D11Texture1D**) &m_pRTTex ) ) )
		{
			WriteToLog( _T( "CreateTexture1D() result code '%s'" ), D3DHResultToString( hr ).c_str() );
			throw TRException("[CDepthTest::SetupTextures] CreateTexture1D failed for render target.");
		}
		break;
	case D3D11_DSV_DIMENSION_TEXTURE2D:
	case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:

		tex2DDesc.ArraySize			 = m_DSTex2DDesc.ArraySize;
		tex2DDesc.BindFlags			 = m_DSTex2DDesc.BindFlags;
		tex2DDesc.CPUAccessFlags	 = m_DSTex2DDesc.CPUAccessFlags;
		tex2DDesc.Format			 = m_DSTex2DDesc.Format;
		tex2DDesc.MipLevels		     = m_DSTex2DDesc.MipLevels;
		tex2DDesc.MiscFlags			 = m_DSTex2DDesc.MiscFlags;
		tex2DDesc.Usage				 = m_DSTex2DDesc.Usage;
		tex2DDesc.Width				 = m_DSTex2DDesc.Width;
		tex2DDesc.Height			 = m_DSTex2DDesc.Height;
		tex2DDesc.SampleDesc.Count   = m_DSTex2DDesc.SampleDesc.Count;
		tex2DDesc.SampleDesc.Quality = m_DSTex2DDesc.SampleDesc.Quality;
		if( FAILED( hr = GetFramework()->CreateTexture2DTiledOptional( &tex2DDesc, NULL, (ID3D11Texture2D**) &m_pDSTex ) ) )
		{
			WriteToLog( _T( "CreateTexture2D() result code '%s'" ), D3DHResultToString( hr ).c_str() );
			throw TRException("[CDepthTest::SetupTextures] CreateTexture2D failed for depth texture.");
		}

		// staging resource desc
		tex2DDesc.BindFlags			= 0;
		tex2DDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;
		tex2DDesc.Usage				= D3D11_USAGE_STAGING;
		if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, (ID3D11Texture2D**) &m_pDSTexStaging ) ) )
		{
			WriteToLog( _T( "CreateTexture2D() result code '%s'" ), D3DHResultToString( hr ).c_str() );
			throw TRException("[CDepthTest::SetupTextures] CreateTexture2D failed for depth staging texture.");
		}

		// render target
		tex2DDesc.BindFlags			= D3D11_BIND_RENDER_TARGET;
		tex2DDesc.CPUAccessFlags	= 0;
		tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
		tex2DDesc.Format			= DXGI_FORMAT_R8G8B8A8_UNORM;
		if( FAILED( hr = GetFramework()->CreateTexture2DTiledOptional( &tex2DDesc, NULL, (ID3D11Texture2D**) &m_pRTTex ) ) )
		{
			WriteToLog( _T( "CreateTexture2D() result code '%s'" ), D3DHResultToString( hr ).c_str() );
			throw TRException("[CDepthTest::SetupTextures] CreateTexture2D failed for render target.");
		}
		break;
	}

	//
	//	Depth/Stencil View
	//
	D3D11_RENDER_TARGET_VIEW_DESC rtv;
	rtv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_DSVDesc.Format = m_DSTex2DDesc.Format;
	// don't set m_DSVDesc.Flags - set at constructor / by test factors
	switch( m_DSVDesc.ViewDimension )
	{
	case D3D11_DSV_DIMENSION_TEXTURE1D:
		m_DSVDesc.Texture1D.MipSlice = m_uMipSlice;
		rtv.Texture1D.MipSlice = m_uMipSlice;
		rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
		m_DSVDesc.Texture1DArray.ArraySize = m_DSTex2DDesc.ArraySize;
		m_DSVDesc.Texture1DArray.FirstArraySlice = 0;
		m_DSVDesc.Texture1DArray.MipSlice = m_uMipSlice;
		rtv.Texture1DArray.ArraySize = m_DSTex2DDesc.ArraySize;
		rtv.Texture1DArray.FirstArraySlice = 0;
		rtv.Texture1DArray.MipSlice = m_uMipSlice;
		rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE2D:
		m_DSVDesc.Texture2D.MipSlice = m_uMipSlice;
		rtv.Texture2D.MipSlice = m_uMipSlice;
		rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
		m_DSVDesc.Texture2DArray.ArraySize = m_DSTex2DDesc.ArraySize;
		m_DSVDesc.Texture2DArray.FirstArraySlice = 0;
		m_DSVDesc.Texture2DArray.MipSlice = m_uMipSlice;
		rtv.Texture2DArray.ArraySize = m_DSTex2DDesc.ArraySize;
		rtv.Texture2DArray.FirstArraySlice = 0;
		rtv.Texture2DArray.MipSlice = m_uMipSlice;
		rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		break;
	}

	if( FAILED( hr = GetDevice()->CreateDepthStencilView( m_pDSTex, &m_DSVDesc, &m_pDSV ) ) )
	{
		WriteToLog( _T( "CreateDepthStencilView() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CDepthTest::SetupTextures] CreateDepthStencilView failed.");
	}

	// create a view that's definitely writeable
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDescWritable;
	memcpy( &dsvDescWritable, &m_DSVDesc, sizeof( m_DSVDesc ) );
	dsvDescWritable.Flags = 0;
	if( FAILED( hr = GetDevice()->CreateDepthStencilView( m_pDSTex, &dsvDescWritable, &m_pDSVWriteable ) ) )
	{
		WriteToLog( _T( "CreateDepthStencilView() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CDepthTest::SetupTextures] CreateDepthStencilView failed.");
	}

	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRTTex, &rtv, &m_pRTV ) ) )
	{
		WriteToLog( _T( "CreateRenderTargetView() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CDepthTest::SetupTextures] CreateRenderTargetView failed.");
	}

}

void CDepthTest::SetupVB()
{
	HRESULT hr;
	//
	// Create vertex buffer
	//
	D3D11_BUFFER_DESC bufferDesc =
	{
		3 * m_uSubResWidth * m_uSubResHeight * sizeof( VERTEX ),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		0
	};
	if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pVB ) ) )
	{
		WriteToLog( _T( "CreateBuffer() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CDepthTest::SetupVB] CreateBuffer failed for vertex buffer.");
	}

	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
	}
}

void CDepthTest::FillVB(const std::vector<float> &data)
{
	assert(data.size() == m_uSubResHeight*m_uSubResWidth);

	HRESULT hr;
	VERTEX *pVert;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	// Fill vertex buffer
	if( FAILED( hr = GetEffectiveContext()->Map( m_pVB, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
	{
		WriteToLog( _T( "Map() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CDepthTest::FillDepth] Map on m_pVB failed.");
	}
	pVert = (VERTEX*) mappedRes.pData;

	for (UINT y = 0; y < m_uSubResHeight; ++y)
		for (UINT x = 0; x < m_uSubResWidth; ++x)
		{	
			// create triangle
			float depth = data[x + y * m_uSubResWidth];
			pVert->x = x + 0.5f;
			pVert->y = y + 0.5f;
			pVert->z = depth;
			pVert++;

			pVert->x = x + 0.6f;
			pVert->y = y + 0.5f;
			pVert->z = depth;
			pVert++;

			pVert->x = x + 0.5f;
			pVert->y = y + 0.6f;
			pVert->z = depth;
			pVert++;
		}

	GetEffectiveContext()->Unmap(m_pVB,0);

	// rebind
	UINT strides[] = { sizeof( VERTEX ) };
	UINT offsets[] = { 0 };
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVB, strides, offsets );

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
	}
}

void CDepthTest::FillConstantBuffer(float depth)
{
	HRESULT hr;
	float *bufData;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if( FAILED( hr = GetEffectiveContext()->Map( m_pCB, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
	{
		WriteToLog( _T( "Map() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CDepthTest::FillConstantBuffer] Map on m_pCB failed.");
	}
	bufData = (float*) mappedRes.pData;

	// Inverse viewport scale.
	bufData[2] = 2 / (float)m_uSubResWidth;
	bufData[3] = -2 / (float)m_uSubResHeight;
	// Prescaled inverse viewport translation.
	bufData[0] = -1.0f;
	bufData[1] = 1.0f;
	// depth - used only in odepth shader
	bufData[4] = depth;
	// rt_index (uint)
	bufData[5] = *((float*)&m_uArraySlice);

	GetEffectiveContext()->Unmap(m_pCB, 0);
}

void CDepthTest::GetDepth(std::vector<float> &vDepth)
{
	assert(vDepth.size() == m_uSubResWidth*m_uSubResHeight);
	HRESULT hr;
		
	//copy
	GetEffectiveContext()->CopyResource( m_pDSTexStaging, m_pDSTex );

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
	}

	// map
	void *pData;
	D3D11_MAPPED_SUBRESOURCE texmap;
	UINT uSubRes = m_uArraySlice * m_DSTex2DDesc.MipLevels + m_uMipSlice;
	switch( m_DSVDesc.ViewDimension )
	{
	case D3D11_DSV_DIMENSION_TEXTURE1D:
	case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
		if( FAILED( hr = GetImmediateContext()->Map( m_pDSTexStaging, uSubRes, D3D11_MAP_READ, 0, &texmap ) ) )
		{
			WriteToLog( _T( "Failed to map depth stencil texture, hr = %s" ), D3DHResultToString(hr).c_str() );
			throw TRException("[CDepthTest::GetDepth] Map failed on 1D depth staging texture.");
		}
		CopyDepth(texmap.pData, vDepth);
		GetImmediateContext()->Unmap( m_pDSTexStaging, uSubRes );
		break;
	case D3D11_DSV_DIMENSION_TEXTURE2D:
	case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
		if( FAILED( hr = GetImmediateContext()->Map( m_pDSTexStaging, uSubRes, D3D11_MAP_READ, 0, &texmap ) ) )
		{
			WriteToLog( _T( "Failed to map depth stencil texture, hr = %s" ), D3DHResultToString(hr).c_str() );
			throw TRException("[CDepthTest::GetDepth] Map failed on 2D depth staging texture.");
		}
		CopyDepth(texmap.pData, vDepth, texmap.RowPitch);
		GetImmediateContext()->Unmap( m_pDSTexStaging, uSubRes );
		break;
	}
}

void CDepthTest::CopyDepth(void *pSrc, std::vector<float>& vDest, UINT uRowPitch)
{
	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(m_DSTex2DDesc.Format);
	UINT uBPE = pTexFormatInfo->uBitsPerElement;
	UINT uBPC = pTexFormatInfo->pBitsPerComponent[0];
	for (UINT y = 0; y < m_uSubResHeight; ++y)
	{
		for (UINT x = 0; x < m_uSubResWidth; ++x)
		{
			// pointer to the element
			BYTE *pElement = (BYTE*)pSrc + x*(uBPE/8);

			// component data
			UINT uData;
			if (uBPE == 16)
				uData = ((UINT16*)pElement)[0];
			else
				uData = ((UINT32*)pElement)[0];
			uData &= UINT_MAX >> (32-uBPC);

			UINT index = x + y*m_uSubResWidth;
			if (pTexFormatInfo->dwFlags & FF_UNORM)
			{
				// convert unorm to float
				vDest[index] = uData/(float)(0xffffffff >> (32-uBPC));
			}
			else
			{
				// reinterpret bits as float (size is known at 32b)
				vDest[index] = *(float*)(&uData);
			}
		}
		// go to next row or src data
		pSrc = (BYTE*)pSrc + uRowPitch;
	}
}

void CDepthTest::CleanupTestCaseCommon()
{
	// unbind
	if (GetDevice())
	{
		ID3D11Buffer *pNullBuffers[] = { NULL };
		UINT offsets[] = { 0 };
		ID3D11RenderTargetView* pRTNull[] = { NULL, NULL, NULL, NULL };
		GetEffectiveContext()->OMSetRenderTargets( 1, pRTNull, NULL );
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, pNullBuffers, offsets, offsets );

		HRESULT hr;
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context.") );
		}
	}

	// cleanup
	SAFE_RELEASE( m_pDSTex );
	SAFE_RELEASE( m_pDSTexStaging );
	SAFE_RELEASE( m_pDSV );
	SAFE_RELEASE( m_pDSVWriteable );
	SAFE_RELEASE( m_pRTTex );
	SAFE_RELEASE( m_pRTV );	
	SAFE_RELEASE( m_pVB );
}

void CDepthTest::CleanupCommon()
{
	// unbind
	ID3D11Buffer* pBufferNull[] = { NULL };
	if (GetDevice())
	{
		GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->VSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->GSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->RSSetState( NULL );
		GetEffectiveContext()->OMSetDepthStencilState( NULL, 0 );

		HRESULT hr;
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context.") );
		}
	}

	// cleanup
	SAFE_RELEASE( m_pDSS );
	SAFE_RELEASE( m_pRS );

	SAFE_RELEASE( m_pCB );
	SAFE_RELEASE( m_pIL );

	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pVSODepth );
	SAFE_RELEASE( m_pGSTri );
	SAFE_RELEASE( m_pGSPoint );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pPSODepth );
}



tstring __stdcall ToString( PSType type )
{
	switch( type )
	{
		case DEFAULT:
			return tstring( _T( "DEFAULT" ) );
			break;
		case ODEPTH:
			return tstring( _T( "ODEPTH" ) );
			break;
		case NONE:
			return tstring( _T( "NONE" ) );
			break;
		default:
			return tstring( _T( "" ) );
			break;
	}
}

bool __stdcall FromString( PSType *type, const tstring &str )
{
	if( tcistring( str ) == _T( "DEFAULT" ) )
	{
		*type = DEFAULT;
		return true;
	}
	
	if( tcistring( str ) == _T( "ODEPTH" ) )
	{
		*type = ODEPTH;
		return true;
	}
	
	if( tcistring( str ) == _T( "NONE" ) )
	{
		*type = NONE;
		return true;
	}

	return false;
}
