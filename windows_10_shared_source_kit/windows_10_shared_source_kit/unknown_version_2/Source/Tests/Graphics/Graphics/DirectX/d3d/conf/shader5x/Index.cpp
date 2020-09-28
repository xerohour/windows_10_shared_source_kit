("Format"), DXGI_FORMAT_R32G32B32A32_TYPELESS), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R10G10B10A2_TYPELESS), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), 0), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), uMaxQualityLevel - 1), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), D3D11_STANDARD_MULTISAMPLE_PATTERN), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), D3D11_CENTER_MULTISAMPLE_PATTERN), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Width"), 128), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Height"), 17), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ArraySize"), 2), WeightFactorLarge);

	// Not adding the DeclarePriorityLevelByPriorityValue() calls here since this is a base class
}

TEST_RESULT CMultisampleTest::Setup()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	try
	{
		//
		//Set Rasterizer state
		//
		D3D11_RASTERIZER_DESC rastDesc;
		rastDesc.CullMode = D3D11_CULL_NONE;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.MultisampleEnable = true;
		rastDesc.FrontCounterClockwise = true;
		rastDesc.DepthBias = false;
		rastDesc.DepthBiasClamp = 0;
		rastDesc.SlopeScaledDepthBias = 0;
		rastDesc.DepthClipEnable = true;
		rastDesc.ScissorEnable = false;
		rastDesc.AntialiasedLineEnable = false;
		if( FAILED( hr = GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSEnabled ) ) )
			throw TRException("CreateRasterizerState for m_pRSEnabled failed.", hr, RESULT_FAIL);

		GetEffectiveContext()->RSSetState( m_pRSEnabled );
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

		//
		//Set DepthStencil state
		//
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		dsDesc.DepthEnable = FALSE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.StencilEnable = FALSE;
		if( FAILED( hr = GetDevice()->CreateDepthStencilState ( &dsDesc, &m_pDSDisabled ) ) )
			throw TRException("CreateDepthStencilState() failed.", hr, RESULT_FAIL);
		GetEffectiveContext()->OMSetDepthStencilState( m_pDSDisabled, 0 );

		//
		// Blend State
		//
		GetEffectiveContext()->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );

		//
		// Create the constant buffer
		//
		D3D11_BUFFER_DESC bufdesc;
		bufdesc.ByteWidth = CONST_BUFFER_SIZE;
		bufdesc.Usage = D3D11_USAGE_DYNAMIC;
		bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufdesc.MiscFlags = 0;
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufdesc, NULL, &m_pConstBuffer ) ) )
			throw TRException("CreateBuffer for m_pConstBuffer failed.", hr, RESULT_FAIL);
		GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );

		//
		// Create vertex buffer
		//

		// vertex buffer initial data
		float pVBData[] = {-1.f, -1.f, -1.f, 1.f, 1.f, -1.f, 1.f, 1.f};  // full screen quad
		D3D11_SUBRESOURCE_DATA srVBData;
		srVBData.pSysMem = &pVBData[0];
		srVBData.SysMemPitch = sizeof(pVBData);
		srVBData.SysMemSlicePitch = sizeof(pVBData);

		// vertex buffer desc
		D3D11_BUFFER_DESC bufDesc;
		bufDesc.ByteWidth = 4 * 2 * sizeof(float);
		bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.MiscFlags = 0;
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufDesc, &srVBData, &m_pVBQuad ) ) )
			throw TRException("CreateBuffer failed for vertex buffer.", hr, RESULT_FAIL);
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

		// backup render target
		D3D11_TEXTURE2D_DESC tex2DDesc;
		tex2DDesc.ArraySize	   = 1;
		tex2DDesc.Width		   = 4;
		tex2DDesc.Height	   = 4;
		tex2DDesc.SampleDesc.Count = 1;
		tex2DDesc.SampleDesc.Quality = 0;
		tex2DDesc.Format	   = DXGI_FORMAT_R32G32_FLOAT;
		tex2DDesc.BindFlags	   = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tex2DDesc.CPUAccessFlags = 0;
		tex2DDesc.MipLevels	   = 1;
		tex2DDesc.MiscFlags	   = 0;
		tex2DDesc.Usage		   = D3D11_USAGE_DEFAULT;
		if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, &m_pTex2DBackup ) ) )
			throw TRException("CreateTexture2D failed for render target texture.", hr, RESULT_FAIL);

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Format = tex2DDesc.Format;
		rtvDesc.Texture2D.MipSlice = 0;
		if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pTex2DBackup, &rtvDesc, &m_pRTVBackup ) ) )
			throw TRException("CreateRenderTargetView failed for render target view. (hr = %s)", hr, RESULT_FAIL);

		if (FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest::Setup()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}

TEST_RESULT CMultisampleTest::SetupTestCase()
{
	//
	// Skip the current sample count if this format doesn't support it (ie: when quality levels returned is 0)
	//
	UINT uQualityLevels;
	GetDevice()->CheckMultisampleQualityLevels( m_Tex2DDesc.Format, m_Tex2DDesc.SampleDesc.Count, &uQualityLevels );
	const bool bIsValidQualityRange = m_Tex2DDesc.SampleDesc.Quality < uQualityLevels;
	const bool bIsD3D10_1SamplePattern = ( GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_1 ) &&
		( m_Tex2DDesc.SampleDesc.Quality == D3D11_STANDARD_MULTISAMPLE_PATTERN || m_Tex2DDesc.SampleDesc.Quality == D3D11_CENTER_MULTISAMPLE_PATTERN );

	if( uQualityLevels == 0 ||									// If this method returns 0, the format and sample count combination is not supported for the installed adapter
		( !bIsValidQualityRange && !bIsD3D10_1SamplePattern ) )	// The valid range is between zero and one less than the level returned by ID3D11Device::CheckMultiSampleQualityLevels, or one of the sample patterns
	{
		WriteToLog( "Skipping unsupported sample count for this format" );
		return RESULT_SKIP;
	}

	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	try
	{
		//
		// Textures
		//
		// multisample render target
		m_Tex2DDesc.BindFlags	   = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		m_Tex2DDesc.CPUAccessFlags = 0;
		m_Tex2DDesc.MipLevels	   = 1;
		m_Tex2DDesc.MiscFlags	   = 0;
		m_Tex2DDesc.Usage		   = D3D11_USAGE_DEFAULT;
		if( FAILED( hr = GetDevice()->CreateTexture2D( &m_Tex2DDesc, NULL, &m_pRTTexture2D ) ) )
			throw TRException("CreateTexture2D failed for MS render target texture.", hr, RESULT_FAIL);

		// rtv
		D3D11_RENDER_TARGET_VIEW_DESC rtv;
		rtv.Format = m_Tex2DDesc.Format;
		rtv.ViewDimension = m_RTVDesc.ViewDimension;

		// srv
		D3D11_SHADER_RESOURCE_VIEW_DESC srv;
		srv.Format = m_Tex2DDesc.Format;

		switch( m_RTVDesc.ViewDimension )
		{
		case D3D11_RTV_DIMENSION_TEXTURE2DMS:
			// srv
			srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			break;
		case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
			// srv
			srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
			srv.Texture2DMSArray.ArraySize = m_Tex2DDesc.ArraySize - m_uArraySlice;
			srv.Texture2DMSArray.FirstArraySlice = m_uArraySlice;
			// rtv
			rtv.Texture2DMSArray.ArraySize = m_Tex2DDesc.ArraySize;
			rtv.Texture2DMSArray.FirstArraySlice = 0;
			break;
		}

		if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRTTexture2D, &rtv, &m_pRTView ) ) )
			throw TRException("CreateRenderTargetView failed.", hr, RESULT_FAIL);

		if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pRTTexture2D, &srv, &m_pSRView ) ) )
			throw TRException("CreateShaderResourceView failed.", hr, RESULT_FAIL);

		//
		// Shaders
		//
		SetupShaders();

		//
		// Setup the default viewport
		//
		D3D11_VIEWPORT vp;
		vp.Height = (float)m_Tex2DDesc.Height;
		vp.Width = (float)m_Tex2DDesc.Width;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		GetEffectiveContext()->RSSetViewports( 1, &vp );

		//
		// Const buffer
		//
		UINT bufData[1];
		bufData[0] = m_uArraySlice;
		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, bufData ) ) )
			throw TRException("Map on m_pConstBuffer Failed", hr, RESULT_FAIL);

		// state
		FLOAT color[] = { 0, 0, 0, 0 };
		UINT strides[] = { 8 };
		UINT offsets[] = { 0 };
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVBQuad, strides, offsets );
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, NULL );
		GetEffectiveContext()->ClearRenderTargetView( m_pRTView, color );

		if (FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest::SetupTestCase()] ");
		if (tRes != RESULT_PASS)
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}


void CMultisampleTest::SetupShaders()
{
	HRESULT hr;
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	tstring szVS, szGS, szPS;

	//
	// Default shaders
	//

	szVS = g_TestApp.GetShaderProfile(
		D3D_SHADER_STAGE_VERTEX,
		g_TestApp.m_D3DOptions.FeatureLevel,
		g_TestApp.m_D3DOptions.FeatureLevel);

	szVS += " \n";

	// default VS
	const char VS[] =
	"dcl_input v[0].xy"										"\n"
	"dcl_output_sv  o[0].xyzw, position"					"\n"
	"mov o[0].xy, v[0].xyxx"								"\n"
	"mov o[0].zw, vec4( 0.0, 0.0, 0.0, 1.0 )"				"\n";

	szVS += VS;

	szGS = g_TestApp.GetShaderProfile(
		D3D_SHADER_STAGE_GEOMETRY,
		g_TestApp.m_D3DOptions.FeatureLevel,
		g_TestApp.m_D3DOptions.FeatureLevel);

	szGS += " \n";

	// default GS
	const char GS[] =
	"dcl_inputPrimitive triangle"		"\n"
	"dcl_outputTopology trianglestrip"	"\n"
	"dcl_maxOutputVertexCount 3"	"\n"
	"dcl_constantBuffer c0[1], immediateIndexed"		"\n"
	"dcl_temps 6"					"\n"
	"dcl_input_sv v[3][0].xyzw, position"	"\n" // Result from previous stage
	"dcl_output_sv o[0].xyzw, position"			"\n"
	"dcl_output_sv o[1].x, renderTargetArrayIndex"	"\n" // render target array index
	"mov o[0].xyzw, v[0][0].xyzw"	"\n" // copy pos
	"mov o[1].x, c0[0].x"			"\n" // get array index from constant data
	"emit"							"\n"
	"mov o[0].xyzw, v[1][0].xyzw"	"\n" // copy pos
	"emit"							"\n"
	"mov o[0].xyzw, v[2][0].xyzw"	"\n" // copy pos
	"emit"							"\n";

	szGS += GS;

	// fill in shader strings
	SetupShaderStrings(szVS, szGS, szPS);

	// vertex shader
	if( !AssembleShader( szVS.c_str(), &pShaderBuf, &pErrorBuf ) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T("[CMultisampleTest::SetupShaders] AssembleShader failed assembling VS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		SAFE_RELEASE( pErrorBuf );
		throw TRException("");
	}
	if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pVS ) ) )
	{
		SAFE_RELEASE( pShaderBuf );
		throw TRException("[CMultisampleTest::SetupShaders] CreateVertexShader() failed creating default VS", hr, RESULT_FAIL);
	}
	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );

	// Set the input layout
	D3D11_INPUT_ELEMENT_DESC DeclTL[] =
	{
		{"0_", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},  //position
	};
	if(	FAILED(	hr = GetDevice()->CreateInputLayout( DeclTL, sizeof(DeclTL) / sizeof(DeclTL[ 0 ]), pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pLayout ) ) )
		throw TRException("[CMultisampleTest::SetupShaders] CreateInputLayout() failed.", hr, RESULT_FAIL);

	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );
	GetEffectiveContext()->IASetInputLayout( m_pLayout );

	// Geometry Shader
	if( !AssembleShader( szGS.c_str(), &pShaderBuf, &pErrorBuf ) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T("[CMultisampleTest::SetupShaders] AssembleShader failed assembling GS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		SAFE_RELEASE( pErrorBuf );
		throw TRException("");
	}
	if( FAILED( hr = GetDevice()->CreateGeometryShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pGS ) ) )
	{
		SAFE_RELEASE( pShaderBuf );
		throw TRException("[CMultisampleTest::SetupShaders] CreateGeometryShader() failed creating default GS", hr, RESULT_FAIL);
	}
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	//	Create pixel shader
	if ( !AssembleShader(szPS.c_str(), &pShaderBuf, &pErrorBuf) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T("[CMultisampleTest::SetupShaders] AssembleShader failed assembling PS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		SAFE_RELEASE( pErrorBuf );
		throw TRException("");
	}
	if (FAILED( hr = GetDevice()->CreatePixelShader((DWORD*)pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pPS)))
	{
		SAFE_RELEASE( pShaderBuf );
		throw TRException("[CMultisampleTest::SetupShaders] CreateVertexShader() failed creating default PS", hr, RESULT_FAIL);
	}
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );
}

void CMultisampleTest::CleanupTestCase()
{
	// unbind
	ID3D11RenderTargetView* pRTNull[] = { NULL, NULL, NULL, NULL };
	ID3D11ShaderResourceView *pSRVNull[] = { NULL, NULL };
	GetEffectiveContext()->OMSetRenderTargets( 1, pRTNull, NULL );
	GetEffectiveContext()->PSSetShaderResources( 0, 1, pSRVNull );
	GetEffectiveContext()->IASetInputLayout( NULL );
	GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
	}

	// release
	SAFE_RELEASE( m_pRTTexture2D );
	SAFE_RELEASE( m_pRTView );
	SAFE_RELEASE( m_pSRView );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pLayout );
}

void CMultisampleTest::Cleanup()
{
	// cleanup VB
	if (GetEffectiveContext())
	{
		ID3D11Buffer *pNullBuffers[] = { NULL };
		UINT offsets[] = { 0 };
		GetEffectiveContext()->IASetVertexBuffers(0, 1, pNullBuffers, offsets, offsets);
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
	}

	SAFE_RELEASE( m_pVBQuad );

	SAFE_RELEASE( m_pTex2DBackup );
	SAFE_RELEASE( m_pRTVBackup );
	// cleanup base
	CShader5xTest::Cleanup();
}


//////////////////////////////////////////////////////////////////////////
// SampleInfo
//////////////////////////////////////////////////////////////////////////

CSampleInfoTest::CSampleInfoTest()
: CMultisampleTest()
{

}

void CSampleInfoTest::InitTestParameters()
{
	CMultisampleTest::InitTestParameters();

	// sample quality
	CTestCaseParameter< bool > *pInfoIntParam = AddParameter( _T("_uint"), &m_bInfoInt );
	testfactor::RFactor rfInfoInt = AddParameterValueSet< bool >( pInfoIntParam, new CBoolValueSet() );

	// rasterizer
	CTestCaseParameter< bool > *pRasterizerParam = AddParameter( _T("rasterizer"), &m_bRasterizer );
	testfactor::RFactor rfRasterizer = AddParameterValueSet< bool >( pRasterizerParam, new CBoolValueSet() );

	// bind resource
	CTestCaseParameter< bool > *pBindParam = AddParameter( _T("bindResource"), &m_bBindResource );
	testfactor::RFactor rfBind = AddParameterValueSet< bool >( pBindParam, new CBoolValueSet() );

	SetRootTestFactor( GetRootTestFactor() * rfInfoInt * rfRasterizer * rfBind );

	// Sample frequency interpolation are undefined
	// when using the center sample patterns (D3D10_CENTER_MULTISAMPLE_PATTERN)
	// that have more than one sample overlapping at the center of the pixel.
	CFilterHandle filter;
	filter = FilterEqual<bool>( _T("rasterizer"), true ) &&
		     FilterEqual<UINT>( _T("SampleQuality"), D3D11_CENTER_MULTISAMPLE_PATTERN ) &&
		     FilterGreater<UINT>( _T("SampleCount"), 1 );

	SkipMatching( filter );

	AddPriorityPatternFilter(FilterEqual<bool>(_T("bindResource"), true), WeightFactorVeryLarge);

	DeclarePriorityLevelByPriorityValue( 1, 5.0f );
	DeclarePriorityLevelByPriorityValue( 2, 3.0f );
}

void CSampleInfoTest::SetupShaderStrings(tstring &szVS, tstring &szGS, tstring &szPS)
{

	szPS = g_TestApp.GetShaderProfile(
		D3D_SHADER_STAGE_PIXEL,
		g_TestApp.m_D3DOptions.FeatureLevel,
		g_TestApp.m_D3DOptions.FeatureLevel);

	szPS += " \n";

	const char PS[] =
	"dcl_input_ps_sv v[0].xyzw, linear_noperspective, position"  "\n"
	"dcl_temps 1"												 "\n"
	"dcl_output o[0].xyzw"										 "\n";

	szPS += PS;

	// resource info
	D3D11_TEXTURE2D_DESC desc;
	m_pRTTexture2D->GetDesc(&desc);
	UINT uFlags = GetFlags(desc.Format);

	// declare resource
	szPS += "dcl_resource t0, Texture2DMS";
	if (m_RTVDesc.ViewDimension == D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY)
		szPS += "Array";

	if (uFlags & FF_UINT)
		szPS += ", uint \n";
	else if (uFlags & FF_SINT)
		szPS += ", sint \n";
	else
		szPS += ", float \n";

	// return type info
	ID3D11Texture2D *pTex2D = m_bRasterizer ? m_pRTTexture2D : m_pTex2DBackup;
	pTex2D->GetDesc(&desc);
	uFlags = GetFlags(desc.Format);

	// construct end of PS
	// part 1: call sampleinfo
	if (m_bInfoInt)
		szPS += tstring("sampleinfo_uint r0.xyzw, ");
	else
		szPS += tstring("sampleinfo r0.xyzw, ");

	// read from the rasterizer or a bound resource
	if (m_bRasterizer)
		szPS += tstring("rasterizer \n");
	else
		szPS += tstring("t0 \n");


	// part 2: convert to render target format
	if (m_bInfoInt)
	{
		if (uFlags & (FF_FLOAT | FF_UNORM | FF_SNORM))
		{
			szPS += tstring("utof r0, r0 \n");
			if (uFlags & (FF_UNORM | FF_SNORM))
				szPS += tstring("mul r0, r0, vec4(0.031250f, 0, 0, 0) \n");
		}
	}
	else
	{
		if (uFlags & (FF_UNORM | FF_SNORM))
			szPS += tstring("mul r0, r0, vec4(0.031250f, 0, 0, 0) \n");
		else if (uFlags & (FF_UINT | FF_SINT))
			szPS += tstring("ftou r0, r0 \n");
	}
	szPS += tstring("mov o[0], r0");

}

TEST_RESULT CSampleInfoTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	D3D11_TEXTURE2D_DESC desc;
	UINT uArraySlice = m_uArraySlice;
	ID3D11Texture2D *pTex2D = m_pRTTexture2D;
	if (!m_bRasterizer)
	{
		pTex2D = m_pTex2DBackup;
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTVBackup, NULL );

		if(m_bBindResource)
			GetEffectiveContext()->PSSetShaderResources( 0, 1, &m_pSRView );
		else
		{
			ID3D11ShaderResourceView *pNullView = NULL ;
			GetEffectiveContext()->PSSetShaderResources( 0, 1, &pNullView );
		}

		uArraySlice = 0;
	}
	pTex2D->GetDesc(&desc);

	// viewport
	D3D11_VIEWPORT vp;
	vp.Height = (float)desc.Height;
	vp.Width = (float)desc.Width;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	GetEffectiveContext()->RSSetViewports( 1, &vp );

	// draw
	GetEffectiveContext()->Draw(4, 0);

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("[CSampleInfoTest::ExecuteTestCase] ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	// verify
	std::vector<D3DXVECTOR4> vBuffer(desc.Width * desc.Height * desc.SampleDesc.Count * desc.ArraySize);
	if( !MSHelper::Read(GetFramework(), pTex2D, (FLOAT*)&vBuffer[0], desc.Format) )
	{
		WriteToLog("[CSampleInfoTest::ExecuteTestCase] MSHelper::Read failed. ");
		return RESULT_FAIL;
	}

	D3DXVECTOR4 *pData = &vBuffer[desc.Width * desc.Height * desc.SampleDesc.Count * uArraySlice];
	for (UINT uSample = 0; uSample < desc.SampleDesc.Count; ++uSample)
		for (UINT uY = 0; uY < desc.Height; ++uY)
			for (UINT uX = 0; uX < desc.Width; ++uX)
			{
				UINT uData;
				// convert unorms and snorms back to ints
				if (GetFlags(desc.Format) & (FF_UNORM | FF_SNORM))
					uData = (UINT)(pData->x*32 + 0.5);
				else
					uData = (UINT)pData->x;

				// compare reported sample count to supplied sample count
				// Note: The first check is making ensuring two things:
				// 1) The shader uses the sampleinfo instruction with a texture as an input (not the rasterizer)
				// 2) The test did NOT bind the texture as an input to the shader
				if( !m_bRasterizer && !m_bBindResource )
				{
					// Regression coverage for DirectX Bug: 107710
					if( uData != 0 )
					{
						WriteToLog("Pixel [%d,%d], Sample[%d]", uX, uY, uSample);
						WriteToLog("sampleinfo returned %d", uData);
						WriteToLog("[CSampleInfoTest::ExecuteTestCase()] sampleinfo did NOT return 0 for the sampleCount.");
						return RESULT_FAIL;
					}
				}
				else if (uData != m_Tex2DDesc.SampleDesc.Count)
				{
					WriteToLog("Pixel [%d,%d], Sample[%d]", uX, uY, uSample);
					WriteToLog("sampleinfo returned %d", uData);
					WriteToLog("[CSampleInfoTest::ExecuteTestCase()] Resource sample count != sampleinfo.");
					return RESULT_FAIL;
				}
				++pData;
			}

	return RESULT_PASS;
}


//////////////////////////////////////////////////////////////////////////
// SamplePos
//////////////////////////////////////////////////////////////////////////

CSamplePosTest::CSamplePosTest()
: CMultisampleTest()
{

}

void CSamplePosTest::InitTestParameters()
{
	CMultisampleTest::InitTestParameters();

	CUserValueSet<bool> *pTrueValueSet = new CUserValueSet< bool >();
	pTrueValueSet->AddValue( true );

	CUserValueSet<bool> *pFalseValueSet = new CUserValueSet< bool >();
	pFalseValueSet->AddValue( false );

	// rasterizer
	CTestCaseParameter< bool > *pRasterizerParam = AddParameter( _T("rasterizer"), &m_bRasterizer );
	testfactor::RFactor rfRasterizer      = AddParameterValueSet< bool >( pRasterizerParam, new CBoolValueSet() );
	testfactor::RFactor rfRasterizerTrue  = AddParameterValueSet< bool >( pRasterizerParam, pTrueValueSet );
	testfactor::RFactor rfRasterizerFalse = AddParameterValueSet< bool >( pRasterizerParam, pFalseValueSet );

	CTestCaseParameter< bool > *pSRVBound = AddParameter( _T("SRV_Bound"), &m_bSRVBound );
	testfactor::RFactor rfSRVBound        = AddParameterValueSet< bool >( pSRVBound, new CBoolValueSet() );
	testfactor::RFactor rfSRVBoundFalse   = AddParameterValueSet< bool >( pSRVBound, pFalseValueSet );


	// sample index (explicitly test out-of-bounds sample index)
	CTestCaseParameter< UINT > *pSampleIndexParam = AddParameter( _T("SampleIndex"), &m_uSampleIndex );
	testfactor::RFactor rfSampleIndex = AddParameterValueSet< UINT >( pSampleIndexParam, new CRangeValueSet<UINT>(0, m_Tex2DDesc.SampleDesc.Count+1, 1) );

	// The SRVBound test case only matters when m_bRasterizer = false
	SetRootTestFactor( (GetRootTestFactor() * rfRasterizerTrue  * rfSRVBoundFalse * rfSampleIndex) +
					   (GetRootTestFactor() * rfRasterizerFalse * rfSRVBound      * rfSampleIndex) );

	// Sample frequency interpolation are undefined
	// when using the center sample patterns (D3D10_CENTER_MULTISAMPLE_PATTERN)
	// that have more than one sample overlapping at the center of the pixel.
	CFilterHandle filter;
	filter = FilterEqual<bool>( _T("rasterizer"), true ) &&
		     FilterEqual<UINT>( _T("SampleQuality"), D3D11_CENTER_MULTISAMPLE_PATTERN ) &&
		     FilterGreater<UINT>( _T("SampleCount"), 1 );

	SkipMatching( filter );

	AddPriorityPatternFilter(FilterEqual<bool>(_T("SRV_Bound"), true), WeightFactorVeryLarge);

	DeclarePriorityLevelByPriorityValue( 1, 10.125f );
	DeclarePriorityLevelByPriorityValue( 2, 6.0f );
}

void CSamplePosTest::SetupShaderStrings(tstring &szVS, tstring &szGS, tstring &szPS)
{
	szPS = g_TestApp.GetShaderProfile(
		D3D_SHADER_STAGE_PIXEL,
		g_TestApp.m_D3DOptions.FeatureLevel,
		g_TestApp.m_D3DOptions.FeatureLevel);

	szPS += " \n";

	const char PS[] =
	"dcl_input_ps_sv v[0].xyzw, linear_noperspective, position"  "\n"
	"dcl_temps 1"												 "\n"
	"dcl_output o[0].xyzw"										 "\n";

	szPS += PS;


	// resource info
	D3D11_TEXTURE2D_DESC desc;
	m_pRTTexture2D->GetDesc(&desc);
	UINT uFlags = GetFlags(desc.Format);

	// declare resource
	szPS += "dcl_resource t0, Texture2DMS";
	if (m_RTVDesc.ViewDimension == D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY)
		szPS += "Array";

	if (uFlags & FF_UINT)
		szPS += ", uint \n";
	else if (uFlags & FF_SINT)
		szPS += ", sint \n";
	else
		szPS += ", float \n";

	// return type info
	ID3D11Texture2D *pTex2D = m_bRasterizer ? m_pRTTexture2D : m_pTex2DBackup;
	pTex2D->GetDesc(&desc);
	uFlags = GetFlags(desc.Format);

	// construct end of PS
	// part 1: call samplepos
	// read from the rasterizer or a bound resource
	if (m_bRasterizer)
		szPS += tstring("samplepos r0, rasterizer, ") + ToString(m_uSampleIndex)+ tstring("\n");
	else
		szPS += tstring("samplepos r0, t0, ") + ToString(m_uSampleIndex) + tstring("\n");

	// part 2: convert to render target format
	szPS += tstring("add r0, r0, vec4(0.5f, 0.5f, 0, 0) \n");
	if (uFlags & (FF_SINT | FF_UINT))
	{
		szPS += tstring("mul r0, r0, vec4(32.0f, 32.0f, 0, 0) \n");
		szPS += tstring("ftou r0, r0 \n");
	}
	szPS += tstring("mov o[0], r0 \n");
}

TEST_RESULT CSamplePosTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("[CSamplePosTest::ExecuteTestCase] ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	// determine sample locations
	if(!SampleFinder::Search(GetFramework(), m_pRTView, m_pRTTexture2D, m_vSamplePos))
		return RESULT_FAIL;


	// pick render target and shader resource based on test parameters
	D3D11_TEXTURE2D_DESC desc;
	UINT uArraySlice = m_uArraySlice;
	ID3D11Texture2D *pTex2D = m_pRTTexture2D;
	if (!m_bRasterizer)
	{
		pTex2D = m_pTex2DBackup;
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTVBackup, NULL );

		ID3D11ShaderResourceView* SRVArray[1];

		if (m_bSRVBound)
		{
			SRVArray[0] = m_pSRView;
		}
		else
		{
			SRVArray[0] = NULL;
		}

		GetEffectiveContext()->PSSetShaderResources( 0, 1, SRVArray );
		uArraySlice = 0;
	}
	pTex2D->GetDesc(&desc);

	// viewport
	D3D11_VIEWPORT vp;
	vp.Height = (float)desc.Height;
	vp.Width = (float)desc.Width;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	GetEffectiveContext()->RSSetViewports( 1, &vp );

	// draw
	GetEffectiveContext()->Draw(4, 0);

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("[CSamplePosTest::ExecuteTestCase] ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	// verify
	std::vector<D3DXVECTOR4> vBuffer(desc.Width * desc.Height * desc.SampleDesc.Count * desc.ArraySize);
	if( !MSHelper::Read(GetFramework(), pTex2D, (FLOAT*)&vBuffer[0], desc.Format) )
	{
		WriteToLog("[CSampleInfoTest::ExecuteTestCase] MSHelper::Read failed. ");
		return RESULT_FAIL;
	}

	// compute expected value
	// remap pExpected from [0, 1<<SubPixelPrecision] to [-16, 16]
	const int SubPixelPrecision = 8;

	POINT pExpected;

	if(!m_bRasterizer && !m_bSRVBound)
	{
		pExpected.x = 0;
		pExpected.y = 0;
	}
	else if(m_uSampleIndex < m_Tex2DDesc.SampleDesc.Count)
	{
		pExpected = m_vSamplePos[m_uSampleIndex];

		if(4 == SubPixelPrecision)
		{
			pExpected.x = pExpected.x*2 - 16;
			pExpected.y = pExpected.y*2 - 16;
		}
		else
		{
			assert(8 == SubPixelPrecision);
			pExpected.x = pExpected.x/8 - 16;
			pExpected.y = pExpected.y/8 - 16;
		}
	}
	else
	{
		pExpected.x = 0;
		pExpected.y = 0;
	}


	// verify
	D3DXVECTOR4 *pData = &vBuffer[desc.Width * desc.Height * desc.SampleDesc.Count * uArraySlice];
	for (UINT uSample = 0; uSample < desc.SampleDesc.Count; ++uSample)
		for (UINT uY = 0; uY < desc.Height; ++uY)
			for (UINT uX = 0; uX < desc.Width; ++uX)
			{
				POINT pActual;
				UINT uFlags = GetFlags(desc.Format);

				// convert coords to ints
				if (uFlags & (FF_FLOAT | FF_SNORM | FF_UNORM))
				{
					pActual.x = (int)(pData->x*32 + 0.5) - 16;
					pActual.y = (int)(pData->y*32 + 0.5) - 16;
				}
				else
				{
					pActual.x = (int)pData->x - 16;
					pActual.y = (int)pData->y - 16;
				}

				// is no G component, can't test y
				if (GetBitsPerComponent(1, desc.Format) == 0)
					pActual.y = pExpected.y;


				// compare to expected
				if ((pActual.x != pExpected.x) ||
					(pActual.y != pExpected.y))
				{
					WriteToLog("Pixel [%d,%d], Sample[%d]", uX, uY, uSample);
					WriteToLog("samplepos returned (%d, %d)", pActual.x, pActual.y);
					WriteToLog("actual sample position = (%d, %d)", pExpected.x, pExpected.y);
					WriteToLog("[CSampleInfoTest::ExecuteTestCase()] samplepos returned incorrect sample location.");
					return RESULT_FAIL;
				}
				++pData;
			}

	return RESULT_PASS;
}



//////////////////////////////////////////////////////////////////////////
// SampleIndex
//////////////////////////////////////////////////////////////////////////

CSampleIndexTest::CSampleIndexTest()
: CMultisampleTest()
{

}

void CSampleIndexTest::InitTestParameters()
{
	CMultisampleTest::InitTestParameters();

	// Sample frequency interpolation are undefined
	// when using the center sample patterns (D3D10_CENTER_MULTISAMPLE_PATTERN)
	// that have more than one sample overlapping at the center of the pixel.
	CFilterHandle filter;
	filter = FilterEqual<UINT>( _T("SampleQuality"), D3D11_CENTER_MULTISAMPLE_PATTERN ) &&
		     FilterGreater<UINT>( _T("SampleCount"), 1 );

	SkipMatching( filter );

	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 2.0f );
}

void CSampleIndexTest::SetupShaderStrings(tstring &szVS, tstring &szGS, tstring &szPS)
{

	// shader profile, like vs_5_0
	szPS = g_TestApp.GetShaderProfile(
		D3D_SHADER_STAGE_PIXEL,
		g_TestApp.m_D3DOptions.FeatureLevel,
		g_TestApp.m_D3DOptions.FeatureLevel);

	szPS += " \n";

	const char PS[] =
	"dcl_input_ps_sv v[0].xyzw, linear_noperspective, position"  "\n"
	"dcl_input_ps_sv v[1].x, constant, sampleIndex"						 "\n"
	"dcl_temps 1"												 "\n"
	"dcl_output o[0].xyzw"										 "\n";

	szPS += PS;

	// resource info
	D3D11_TEXTURE2D_DESC desc;
	m_pRTTexture2D->GetDesc(&desc);
	UINT uFlags = GetFlags(desc.Format);

	// construct end of PS
	// convert to render target format
	szPS += tstring("mov r0.yzww, vec4(0,0,0,0)");
	szPS += tstring("mov r0.x, v[1].x \n");
	if (uFlags & (FF_FLOAT | FF_UNORM | FF_SNORM))
	{
		szPS += tstring("utof r0, r0.x \n");
		if (uFlags & (FF_UNORM | FF_SNORM))
			szPS += tstring("mul r0, r0, vec4(0.031250f, 0, 0, 0) \n");
	}
	szPS += tstring("mov o[0], r0 \n");
}

TEST_RESULT CSampleIndexTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	// pick render target and shader resource based on test parameters
	D3D11_TEXTURE2D_DESC desc;
	UINT uArraySlice = m_uArraySlice;
	m_pRTTexture2D->GetDesc(&desc);

	// draw
	GetEffectiveContext()->Draw(4, 0);

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("[CSampleIndexTest::ExecuteTestCase] ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	// verify
	std::vector<D3DXVECTOR4> vBuffer(desc.Width * desc.Height * desc.SampleDesc.Count * desc.ArraySize);
	if( !MSHelper::Read(GetFramework(), m_pRTTexture2D, (FLOAT*)&vBuffer[0], desc.Format) )
	{
		WriteToLog("[CSamplePosTest::ExecuteTestCase] MSHelper::Read failed. ");
		return RESULT_FAIL;
	}

	// verify
	D3DXVECTOR4 *pData = &vBuffer[desc.Width * desc.Height * desc.SampleDesc.Count * uArraySlice];
	for (UINT uSample = 0; uSample < desc.SampleDesc.Count; ++uSample)
		for (UINT uY = 0; uY < desc.Height; ++uY)
			for (UINT uX = 0; uX < desc.Width; ++uX)
			{
				UINT uActual;
				// convert unorms and snorms back to ints
				if (GetFlags(desc.Format) & (FF_UNORM | FF_SNORM))
					uActual = (UINT)(pData->x*32 + 0.5);
				else
					uActual = (UINT)pData->x;

				// compare to expected
				if (uActual != uSample)
				{
					WriteToLog("Pixel [%d,%d]", uX, uY);
					WriteToLog("actual sample index = %d", uActual);
					WriteToLog("expected sample index = %d", uSample);
					WriteToLog("[CSamplePosTest::ExecuteTestCase()] sv_sampleindex is incorrect.");
					return RESULT_FAIL;
				}
				++pData;
			}

	return RESULT_PASS;
}


//////////////////////////////////////////////////////////////////////////
//  Index.cpp
//  created:	2005/05/11
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "Index.h"

#define SZ_ARRAY_SIZE "4096"
#define EPSILON 0.0001f

BEGIN_NAMED_VALUES( CShader5xTest_Index::INDEXING_SCENARIO )
	NAMED_VALUE( _T( "Temp" ), CShader5xTest_Index::INDEXING_SCENARIO_TEMP )
	NAMED_VALUE( _T( "TempNested" ), CShader5xTest_Index::INDEXING_SCENARIO_TEMP_NESTED )
	NAMED_VALUE( _T( "ConstBuffer" ), CShader5xTest_Index::INDEXING_SCENARIO_CONSTANT_BUFFER )
	NAMED_VALUE( _T( "ConstBufferNested" ), CShader5xTest_Index::INDEXING_SCENARIO_CONSTANT_BUFFER_NESTED )
	NAMED_VALUE( _T( "InputOutput" ), CShader5xTest_Index::INDEXING_SCENARIO_INPUT_OUTPUT )
	NAMED_VALUE( _T( "InputOutputNested" ), CShader5xTest_Index::INDEXING_SCENARIO_INPUT_OUTPUT_NESTED )
	NAMED_VALUE( _T( "ConstBufferUniform" ), CShader5xTest_Index::INDEXING_SCENARIO_CONSTANT_BUFFER_UNIFORM )
	NAMED_VALUE( _T( "Buffer" ), CShader5xTest_Index::INDEXING_SCENARIO_BUFFER )
	NAMED_VALUE( _T( "Texture2D" ), CShader5xTest_Index::INDEXING_SCENARIO_TEXTURE2D )
	NAMED_VALUE( _T( "Texture3D" ), CShader5xTest_Index::INDEXING_SCENARIO_TEXTURE3D )
	NAMED_VALUE( _T( "Sampler" ), CShader5xTest_Index::INDEXING_SCENARIO_SAMPLER )
END_NAMED_VALUES( CShader5xTest_Index::INDEXING_SCENARIO )

BEGIN_NAMED_VALUES( CShader5xTest_Index::INDEXING_INSTRUCTION )
	NAMED_VALUE( _T( "mov" ), CShader5xTest_Index::INDEXING_INSTRUCTION_MOV )
	NAMED_VALUE( _T( "ld" ), CShader5xTest_Index::INDEXING_INSTRUCTION_LD )
	NAMED_VALUE( _T( "sample" ), CShader5xTest_Index::INDEXING_INSTRUCTION_SAMPLE )
END_NAMED_VALUES( CShader5xTest_Index::INDEXING_INSTRUCTION )

BEGIN_NAMED_VALUES( CShader5xTest_Index::INDEXING_COMPUTATION_METHOD )
	NAMED_VALUE( _T( "TempRegister" ), CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_TEMP_REG )
	NAMED_VALUE( _T( "StaticIndexableRegister" ), CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_STATIC_INDEX_REG )
	NAMED_VALUE( _T( "ImmediateConstant" ), CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST )

	NAMED_VALUE( _T( "TempRegister_Plus_ImmediateConstant" ), CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_TEMP_PLUS_IMMEDIATE )
	NAMED_VALUE( _T( "StaticIndexableRegister_Plus_ImmediateConstant" ), CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_INDEX_PLUS_IMMEDIATE )
END_NAMED_VALUES( CShader5xTest_Index::INDEXING_COMPUTATION_METHOD )

typedef UINT32 VertexElement[4];
typedef FLOAT32 VertexFloatElement[4];


// temp indexing
const char szTempIndexSrc[] =
	// Write to the temp array using gray code of index
	"mov r0, vec4( 4096, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"ushr r0.y, r0.x, 1\n"
		"xor r0.y, r0.y, r0.x\n" // r0.y = gray( r0.x )
		"iadd r0.y, r0.y, -4\n" // Invent a bias so we can add an offset to the index.
		"mov x0[r0.y + 3], r0.xxxx\n"
	"endloop\n"
	// Read from the temp array and validate values
	"mov r0.x, 4096\n"
	"loop\n"
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r0.y, r0.x, -4\n"
		"mov r0.z, x0[r0.y + 3]\n" // r0.z = revgray( r0.x )
		"ushr r0.y, r0.z, 1\n"
		"xor r0.y, r0.y, r0.z\n" 
		"ieq r0.y, r0.y, r0.x\n"
		"and r0.w, r0.w, r0.y\n" // Accumulate results in r0.w
	"endloop\n"
	"mov r0.x, r0.w\n"	// Move results from .w to .x
	;
	
const char szTempIndexNestedSrc[] =
	// Write to the temp array using gray code of index
	"mov x0[0], vec4( 4096, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd x0[0].x, x0[0].x, -1\n"
		"breakc_z x0[0].x\n"
		"ushr x0[0].y, x0[0].x, 1\n"
		"xor x0[0].y, x0[0].y, x0[0].x\n" 
		"iadd x0[0].y, x0[0].y, -4\n" // Invent a bias so we can add an offset to the index.
		"mov x0[x0[0].y + 4], x0[0].xxxx\n"
	"endloop\n"
	// Read from the temp array and validate values
	"mov x0[0].x, 4096\n"
	"loop\n"
		"iadd x0[0].x, x0[0].x, -1\n"
		"breakc_z x0[0].x\n"
		"iadd x0[0].y, x0[0].x, -4\n"
		"mov x0[0].z, x0[x0[0].y + 4]\n" // x0[0].z = revgray( x0[0].x )
		"ushr x0[0].y, x0[0].z, 1\n"
		"xor x0[0].y, x0[0].y, x0[0].z\n" 
		"ieq x0[0].y, x0[0].y, x0[0].x\n"
		"and x0[0].w, x0[0].w, x0[0].y\n" // Accumulate results in x0[0].w
	"endloop\n"
	"mov x0[0].x, x0[0].w\n"	// Move results from .w to .x
	;
	

// constant buffer indexing
const char szConstBufferIndexSrc[] =
	"mov r0, vec4( 4096, 0, 0, 0xffffffff )\n"
	// Read from the const buffer array and verify data
	// using indexed addressing.
	"loop\n"
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r0.y, r0.x, -4\n"
		"mov r0.z, c0[r0.y + 4]\n" // r0.z = revgray( r0.x )
		"ushr r0.y, r0.z, 1\n"
		"xor r0.y, r0.y, r0.z\n" 
		"ieq r0.y, r0.y, r0.x\n"
		"and r0.w, r0.w, r0.y\n" // Accumulate results in r0.w
	"endloop\n"
	"mov r0.x, r0.w\n"	// Move results from .w to .x
	;

const char szConstBufferIndexNestedSrc[] =
	// Write to the temp array using gray code of index
	"mov x0[0], vec4( 4096, 0, 0, 0xffffffff )\n"
	// Read from the const buffer array and verify data
	// using indexed addressing.
	"loop\n"
		"iadd x0[0].x, x0[0].x, -1\n"
		"breakc_z x0[0].x\n"
		"iadd x0[0].y, x0[0].x, -4\n"
		"mov x0[0].z, c0[x0[0].y + 4]\n" // x0[0].z = revgray( x0[0].x )
		"ushr x0[0].y, x0[0].z, 1\n"
		"xor x0[0].y, x0[0].y, x0[0].z\n" 
		"ieq x0[0].y, x0[0].y, x0[0].x\n"
		"and x0[0].w, x0[0].w, x0[0].y\n" // Accumulate results in x0[0].w
	"endloop\n"
	"mov x0[0].x, x0[0].w\n"	// Move results from .w to .x
	;


// input/output indexing
const char szVSInputIndexSrc[] =
	"mov r0, vec4( 16, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r0.y, r0.x, -4\n"
		"mov r0.z, v[r0.y + 4]\n"		// r0.z = revgray( r0.x )
		"mov o[r0.y + 4], r0.zzzz\n"	// replicate r0.z to o[r0.x]
		"%s\n"							//"mov o[r0.y + 20], r0.zzzz\n" // replicate r0.z to o[r0.x + 16]
		"ine r0.y, r0.z, 0\n"  // Make sure the value is non-zero
		"and r0.w, r0.w, r0.y\n"
		"ushr r0.y, r0.z, 1\n"
		"xor r0.y, r0.y, r0.z\n" 
		"ieq r0.y, r0.y, r0.x\n"
		"and r0.w, r0.w, r0.y\n" // Accumulate results in r0.w
	"endloop\n"
	"mov r0.x, r0.w\n"	// Move results from .w to .x
	;
	
const char szGSInputIndexSrc[] =
	"mov r0, vec4( 16, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r0.y, r0.x, -4\n"
		"mov r0.z, v[0][r0.y + 4]\n"	// r0.z = revgray( r0.x )
		"mov o[r0.y + 4], r0.zzzz\n"	// replicate r0.z to o[r0.x]
		"%s\n"							//"mov o[r0.y + 20], r0.zzzz\n" // replicate r0.z to o[r0.x + 16]
		"ine r0.y, r0.z, 0\n"  // Make sure the value is non-zero
		"and r0.w, r0.w, r0.y\n"
		"ushr r0.y, r0.z, 1\n"
		"xor r0.y, r0.y, r0.z\n" 
		"ieq r0.y, r0.y, r0.x\n"
		"and r0.w, r0.w, r0.y\n" // Accumulate results in r0.w
	"endloop\n"
	"mov r0.x, r0.w\n"	// Move results from .w to .x
	;

const char szDSInputIndexSrc[] =
	"mov r0, vec4( 16, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r0.y, r0.x, -4\n"
		"mov r0.z, vcp[0][r0.y + 4]\n"  // r0.z = revgray( r0.x )
		"mov o[r0.y + 4], r0.zzzz\n"    // replicate r0.z to o[r0.x]
		"%s\n"							//"mov o[r0.y + 20], r0.zzzz\n"   // replicate r0.z to o[r0.x + 16]
		"ine r0.y, r0.z, 0\n"  // Make sure the value is non-zero
		"and r0.w, r0.w, r0.y\n"
		"ushr r0.y, r0.z, 1\n"
		"xor r0.y, r0.y, r0.z\n" 
		"ieq r0.y, r0.y, r0.x\n"
		"and r0.w, r0.w, r0.y\n" // Accumulate results in r0.w
	"endloop\n"
	"mov r0.x, r0.w\n"	// Move results from .w to .x
	;

const char szPSInputIndexSrc[] =
	"mov r0, vec4( 15, 0, 0, 0xffffffff )\n"
	"loop\n"
		"breakc_z r0.x\n"
		"iadd r0.y, r0.x, -4\n"
		"mov r0.z, v[r0.y + 4]\n" // r0.z = revgray( r0.x )
		"ine r0.y, r0.z, 0\n"  // Make sure the value is non-zero
		"and r0.w, r0.w, r0.y\n"
		"ushr r0.y, r0.z, 1\n"
		"xor r0.y, r0.y, r0.z\n" 
		"ieq r0.y, r0.y, r0.x\n"
		"and r0.w, r0.w, r0.y\n" // Accumulate results in r0.w
		"iadd r0.x, r0.x, -1\n"
		"ieq r0.y, r0.z, v[r0.x + 17]\n" // check that v[r0.x + 16] == v[r0.x]
		"and r0.w, r0.w, r0.y\n" // Accumulate results in r0.w
	"endloop\n"
	"mov r0.x, r0.w\n"	// Move results from .w to .x
	;

	
const char szVSInputIndexNestedSrc[] =
	"mov x0[0], vec4( 16, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd x0[0].x, x0[0].x, -1\n"
		"breakc_z x0[0].x\n"
		"iadd x0[0].y, x0[0].x, -4\n"
		"mov x0[0].z, v[x0[0].y + 4]\n" // x0[0].z = revgray( x0[0].x )
		"mov o[x0[0].y + 4], x0[0].zzzz\n" // replicate x0[0].z to o[x0[0].x]
		"%s\n"							//"mov o[x0[0].y + 20], x0[0].zzzz\n" // replicate x0[0].z to o[x0[0].x + 16]
		"ine x0[0].y, x0[0].z, 0\n"  // Make sure the value is non-zero
		"and x0[0].w, x0[0].w, x0[0].y\n"
		"ushr x0[0].y, x0[0].z, 1\n"
		"xor x0[0].y, x0[0].y, x0[0].z\n" 
		"ieq x0[0].y, x0[0].y, x0[0].x\n"
		"and x0[0].w, x0[0].w, x0[0].y\n" // Accumulate results in x0[0].w
	"endloop\n"
	"mov x0[0].x, x0[0].w\n"	// Move results from .w to .x
	;
	
const char szGSInputIndexNestedSrc[] =
	"mov x0[0], vec4( 16, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd x0[0].x, x0[0].x, -1\n"
		"breakc_z x0[0].x\n"
		"iadd x0[0].y, x0[0].x, -4\n"
		"mov x0[0].z, v[0][x0[0].y + 4]\n"  // x0[0].z = revgray( x0[0].x )
		"mov o[x0[0].y + 4], x0[0].zzzz\n"  // replicate x0[0].z to o[x0[0].x]
		"%s\n"							//"mov o[x0[0].y + 20], x0[0].zzzz\n" // replicate x0[0].z to o[x0[0].x + 16]
		"ine x0[0].y, x0[0].z, 0\n"  // Make sure the value is non-zero
		"and x0[0].w, x0[0].w, x0[0].y\n"
		"ushr x0[0].y, x0[0].z, 1\n"
		"xor x0[0].y, x0[0].y, x0[0].z\n" 
		"ieq x0[0].y, x0[0].y, x0[0].x\n"
		"and x0[0].w, x0[0].w, x0[0].y\n" // Accumulate results in x0[0].w
	"endloop\n"
	"mov x0[0].x, x0[0].w\n"	// Move results from .w to .x
	;

const char szDSInputIndexNestedSrc[] =
	"mov x0[0], vec4( 16, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd x0[0].x, x0[0].x, -1\n"
		"breakc_z x0[0].x\n"
		"iadd x0[0].y, x0[0].x, -4\n"
		"mov x0[0].z, vcp[0][x0[0].y + 4]\n"  // x0[0].z = revgray( x0[0].x )
		"mov o[x0[0].y + 4], x0[0].zzzz\n"    // replicate x0[0].z to o[x0[0].x]
		"%s\n"							//"mov o[x0[0].y + 20], x0[0].zzzz\n"   // replicate x0[0].z to o[x0[0].x + 16]
		"ine x0[0].y, x0[0].z, 0\n"  // Make sure the value is non-zero
		"and x0[0].w, x0[0].w, x0[0].y\n"
		"ushr x0[0].y, x0[0].z, 1\n"
		"xor x0[0].y, x0[0].y, x0[0].z\n" 
		"ieq x0[0].y, x0[0].y, x0[0].x\n"
		"and x0[0].w, x0[0].w, x0[0].y\n" // Accumulate results in x0[0].w
	"endloop\n"
	"mov x0[0].x, x0[0].w\n"	// Move results from .w to .x
	;

const char szPSInputIndexNestedSrc[] =
	"mov x0[0], vec4( 15, 0, 0, 0xffffffff )\n"
	"loop\n"
		"breakc_z x0[0].x\n"
		"iadd x0[0].y, x0[0].x, -4\n"
		"mov x0[0].z, v[x0[0].y + 4]\n" // x0[0].z = revgray( x0[0].x )
		"ine x0[0].y, x0[0].z, 0\n"  // Make sure the value is non-zero
		"and x0[0].w, x0[0].w, x0[0].y\n"
		"ushr x0[0].y, x0[0].z, 1\n"
		"xor x0[0].y, x0[0].y, x0[0].z\n" 
		"ieq x0[0].y, x0[0].y, x0[0].x\n"
		"and x0[0].w, x0[0].w, x0[0].y\n" // Accumulate results in x0[0].w
		"iadd x0[0].x, x0[0].x, -1\n"
		"ieq x0[0].y, x0[0].z, v[x0[0].x + 17]\n" // check that v[r0.x + 16] == v[r0.x]
		"and x0[0].w, x0[0].w, x0[0].y\n" // Accumulate results in x0[0].w
	"endloop\n"
	"mov x0[0].x, x0[0].w\n"	// Move results from .w to .x
	;

const char g_szResourceIndexSampleSrc[] = 
	"mov r2.x, 0\n"
	"mov x0[0].x, 1\n"
	"mov r1, vec4(%ff,%ff,%ff,%ff)\n"
	"sample_indexable(%s)(float,float,float,float) r0, r1, t[%s], s[%s]\n"
;

const char g_szResourceIndexLoadSrc[] = 
	"mov r2.x, 0\n"
	"mov x0[0].x, 1\n"
	"mov r1, vec4(%d,%d,%d,%d)\n"
	"ld_indexable(%s)(float,float,float,float) r0, r1, t[%s]\n"
;

const char g_szResourceIndexMovSrc[] = 
	"mov r2.x, 0\n"
	"mov x0[0].x, 1\n"
	"mov r0, cb[%s][0]\n"
;

const CShader5xTest_Index::INDEXING_COMPUTATION_METHOD g_pResourceIndexComputations[] =
{
	CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_TEMP_REG,				// #r
	CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_STATIC_INDEX_REG,		// #x
	CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST,		// e.g 2
	CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_TEMP_PLUS_IMMEDIATE,	// e.g #r + 2
	CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_INDEX_PLUS_IMMEDIATE	// e.g #x + 2
};

const UINT g_pResourceIndexValues[] = 
{
	0, // == INDEXING_COMPUTATION_METHOD_TEMP_REG			
	1, // == INDEXING_COMPUTATION_METHOD_STATIC_INDEX_REG	
	2, // == INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST	
	2, // == INDEXING_COMPUTATION_METHOD_TEMP_PLUS_IMMEDIATE
	3  // == INDEXING_COMPUTATION_METHOD_INDEX_PLUS_IMMEDIATE
};

const char* g_pResourceIndexExpr[] =
{
	"r2.x",
	"x0[0].x",
	"2",
	"r2.x + 2",
	"x0[0].x + 2"
};

TEST_RESULT CShader5xTest_Index::SetupTestCase()
{
	// Local variables
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	// Call the base SetupTestCase to determine if should skip based on FL and shader stage
	tRes = CShader5xTest::SetupTestCase();

	if( RESULT_PASS != tRes )
		return tRes;

	if( m_is >= INDEXING_SCENARIO_TEXTURE2D && m_ShaderStage == D3D_SHADER_STAGE_COMPUTE && g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
	{
		WriteToLog(_T("Non-buffer UAVs are not supported on FL < 11"));
		return RESULT_SKIP;
	}

	{
		D3D11_BUFFER_DESC descBuffer;
		m_pRTBuffer->GetDesc( &descBuffer );
		hr = GetDevice()->CreateBuffer( &descBuffer, NULL, &m_pRTBuffer_2 );

		if( FAILED( hr ) )
		{
			WriteToLog( _T("CreateBuffer() failed (m_pRTBuffer_2). HR = %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		D3D11_RENDER_TARGET_VIEW_DESC rtviewdesc;
		m_pRTBufferView->GetDesc( &rtviewdesc );
		hr = GetDevice()->CreateRenderTargetView( m_pRTBuffer_2, &rtviewdesc, &m_pRTBufferView_2 );

		if( FAILED( hr ) )
		{
			WriteToLog( _T("CreateRenderTargetView() failed (m_pRTBufferView_2). HR = %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		if( m_pRTUABufferView )
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC rtuaviewdesc;
			m_pRTUABufferView->GetDesc( &rtuaviewdesc );
			hr = GetDevice()->CreateUnorderedAccessView( m_pRTBuffer_2, &rtuaviewdesc, &m_pRTUABufferView_2 );

			if( FAILED( hr ) )
			{
				WriteToLog( _T("CreateUnorderedAccessView() failed (m_pRTUABufferView_2). HR = %s"), D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}
		}
	}

	switch( m_is )
	{
		default: 
			break;
		case INDEXING_SCENARIO_CONSTANT_BUFFER:
		case INDEXING_SCENARIO_CONSTANT_BUFFER_NESTED:
		{
			// Setup the constant buffer with gray code numbers.
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth = 4096 * 4 * sizeof( UINT32 );
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			VertexElement *pData = new VertexElement[4096];
			for( int i = 4096; i != 0; )
			{
				--i;
				UINT32 gray = ( i >> 1 ) ^ i;
				VertexElement &e = pData[gray];
				e[0] = i;
				e[1] = i;
				e[2] = i;
				e[3] = i;
			}
			
			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = pData;
			initData.SysMemPitch = 0;
			initData.SysMemSlicePitch = 0;
			if( FAILED( GetDevice()->CreateBuffer( &desc, &initData, &m_pCB ) ) )
			{
				WriteToLog( "CreateBuffer() failed" );
				return RESULT_FAIL;
			}
			
			SAFE_DELETE_ARRAY( pData );
			
			GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->HSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->DSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->CSSetConstantBuffers( 0, 1, &m_pCB );
		
			break;
		}
		
		case INDEXING_SCENARIO_INPUT_OUTPUT:
		case INDEXING_SCENARIO_INPUT_OUTPUT_NESTED:
		{
			// Setup the vertex buffer with gray code numbers.
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth = 32 * 4 * sizeof( UINT32 );
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			if( FAILED( GetDevice()->CreateBuffer( &desc, NULL, &m_pVB ) ) )
			{
				WriteToLog( "CreateBuffer() failed" );
				return RESULT_FAIL;
			}
			
			VertexElement *pData;
			D3D11_MAPPED_SUBRESOURCE mappedVertexBuffer;
			HRESULT hr;

			if( FAILED( hr = GetEffectiveContext()->Map( m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexBuffer ) ) )
			{
				WriteToLog( _T( "CShader5xTest_Index::SetupTestCase() - failed to map m_pVB, hr = %s" ), D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}

			pData = (VertexElement *)mappedVertexBuffer.pData;

			for( int i = 16; i != 0; )
			{
				--i;
				UINT32 gray = ( i >> 1 ) ^ i;
				VertexElement &e = pData[gray];
				e[0] = i;
				e[1] = i;
				e[2] = i;
				e[3] = i;
				if (m_ShaderStage == D3D_SHADER_STAGE_PIXEL)
				{
					// duplicate values to upper range, this is where our PS operates
					VertexElement &e2 = pData[gray + 16];
					memcpy(&e2, &e, sizeof(e));
				}
			}

			const float oneVal = 1.0f;
			VertexElement &posElem = pData[16];
			posElem[0] = 0;
			posElem[1] = 0;
			posElem[2] = 0;
			posElem[3] = *(DWORD*)(&oneVal);
			
			GetEffectiveContext()->Unmap( m_pVB, 0 );

			ID3D11Buffer *buffers[] = { m_pVB };
			UINT strides[] = { 32 * 4 * sizeof( UINT32 ) };
			UINT offsets[] = { 0 };
			
			GetEffectiveContext()->IASetVertexBuffers( 0, 1, buffers, strides, offsets );
			
			break;
		}

		case INDEXING_SCENARIO_CONSTANT_BUFFER_UNIFORM:
		{
			for( UINT i = 0; i < NUM_INDEXABLE_RESOURCES; ++i )
			{
				// Setup the constant buffer with gray code numbers.
				D3D11_BUFFER_DESC desc;
				desc.ByteWidth = 4 * sizeof( UINT32 );
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;
				desc.StructureByteStride = 0;
				VertexElement pData;

				pData[0] = i + 1;
				pData[1] = i + 1;
				pData[2] = i + 1;
				pData[3] = i + 1;
				
				D3D11_SUBRESOURCE_DATA initData;
				initData.pSysMem = &pData;
				initData.SysMemPitch = 0;
				initData.SysMemSlicePitch = 0;
				if( FAILED( GetDevice()->CreateBuffer( &desc, &initData, (ID3D11Buffer**)&m_pResources[i] ) ) )
				{
					WriteToLog( "CreateBuffer() failed" );
					return RESULT_FAIL;
				}
			}
			
			GetEffectiveContext()->VSSetConstantBuffers( 0, NUM_INDEXABLE_RESOURCES, (ID3D11Buffer**) m_pResources );
			GetEffectiveContext()->GSSetConstantBuffers( 0, NUM_INDEXABLE_RESOURCES, (ID3D11Buffer**) m_pResources );
			GetEffectiveContext()->PSSetConstantBuffers( 0, NUM_INDEXABLE_RESOURCES, (ID3D11Buffer**) m_pResources );
			GetEffectiveContext()->HSSetConstantBuffers( 0, NUM_INDEXABLE_RESOURCES, (ID3D11Buffer**) m_pResources );
			GetEffectiveContext()->DSSetConstantBuffers( 0, NUM_INDEXABLE_RESOURCES, (ID3D11Buffer**) m_pResources );
			GetEffectiveContext()->CSSetConstantBuffers( 0, NUM_INDEXABLE_RESOURCES, (ID3D11Buffer**) m_pResources );
		
			break;
		}
		
		case INDEXING_SCENARIO_SAMPLER:
		{
			for( UINT i = 0; i < NUM_INDEXABLE_RESOURCES; ++i )
			{
				D3D11_TEXTURE2D_DESC desc;
				desc.Width = 2;
				desc.Height = 2;
				desc.MipLevels = 1;
				desc.ArraySize = 1;
				desc.Format = DXGI_FORMAT_R32_TYPELESS;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;

				if( FAILED( GetDevice()->CreateTexture2D( &desc, NULL, (ID3D11Texture2D**)&m_pResources[i] ) ) )
				{
					WriteToLog( "CreateTexture2D() failed" );
					return RESULT_FAIL;
				}
				
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = 1;

				if( FAILED( GetDevice()->CreateShaderResourceView( m_pResources[i], &srvDesc, &m_pSRVs[i] ) ) )
				{
					WriteToLog( "CreateShaderResourceView() failed" );
					return RESULT_FAIL;
				}
				
				float borderColor = (float) (i + 1) / (float) NUM_INDEXABLE_RESOURCES;  

				D3D11_SAMPLER_DESC sampDesc;
				sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
				sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
				sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
				sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
				sampDesc.MipLODBias = 0.0f;
				sampDesc.MaxAnisotropy = 16;
				sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
				sampDesc.BorderColor[0] = borderColor;
				sampDesc.BorderColor[1] = borderColor;
				sampDesc.BorderColor[2] = borderColor;
				sampDesc.BorderColor[3] = borderColor;
				sampDesc.MinLOD = 0.0f;
				sampDesc.MaxLOD = FLT_MAX;

				if( FAILED( GetDevice()->CreateSamplerState( &sampDesc, &m_pSamplers[i] ) ) )
				{
					WriteToLog( "CreateSamplerState() failed" );
					return RESULT_FAIL;
				}
			}

			GetEffectiveContext()->VSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->GSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->PSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->HSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->DSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->CSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );

			GetEffectiveContext()->VSSetSamplers( 0, NUM_INDEXABLE_RESOURCES, m_pSamplers );
			GetEffectiveContext()->GSSetSamplers( 0, NUM_INDEXABLE_RESOURCES, m_pSamplers );
			GetEffectiveContext()->PSSetSamplers( 0, NUM_INDEXABLE_RESOURCES, m_pSamplers );
			GetEffectiveContext()->HSSetSamplers( 0, NUM_INDEXABLE_RESOURCES, m_pSamplers );
			GetEffectiveContext()->DSSetSamplers( 0, NUM_INDEXABLE_RESOURCES, m_pSamplers );
			GetEffectiveContext()->CSSetSamplers( 0, NUM_INDEXABLE_RESOURCES, m_pSamplers );
			
			break;
		}
		
		case INDEXING_SCENARIO_BUFFER:
		{
			for( UINT i = 0; i < NUM_INDEXABLE_RESOURCES; ++i )
			{
				// Setup the constant buffer with gray code numbers.
				D3D11_BUFFER_DESC desc;
				desc.ByteWidth = 4 * sizeof( FLOAT32 );
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;
				desc.StructureByteStride = 0;
				VertexFloatElement pData;

				pData[0] = FLOAT32(i + 1);
				pData[1] = FLOAT32(i + 1);
				pData[2] = FLOAT32(i + 1);
				pData[3] = FLOAT32(i + 1);
				
				D3D11_SUBRESOURCE_DATA initData;
				initData.pSysMem = &pData;
				initData.SysMemPitch = 0;
				initData.SysMemSlicePitch = 0;
				if( FAILED( GetDevice()->CreateBuffer( &desc, &initData, (ID3D11Buffer**)&m_pResources[i] ) ) )
				{
					WriteToLog( "CreateBuffer() failed" );
					return RESULT_FAIL;
				}
				
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
				srvDesc.Buffer.ElementOffset = 0;
				srvDesc.Buffer.ElementWidth = 4;

				if( FAILED( GetDevice()->CreateShaderResourceView( m_pResources[i], &srvDesc, &m_pSRVs[i] ) ) )
				{
					WriteToLog( "CreateShaderResourceView() failed" );
					return RESULT_FAIL;
				}
			}

			GetEffectiveContext()->VSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->GSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->PSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->HSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->DSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->CSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			
			break;
		}

		case INDEXING_SCENARIO_TEXTURE2D:
		{
			for( UINT i = 0; i < NUM_INDEXABLE_RESOURCES; ++i )
			{
				D3D11_TEXTURE2D_DESC desc;
				desc.Width = 2;
				desc.Height = 2;
				desc.MipLevels = 1;
				desc.ArraySize = 1;
				desc.Format = DXGI_FORMAT_R32_TYPELESS;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;

				VertexFloatElement pData;
				pData[0] = FLOAT32(( i * 4 ) + 1);
				pData[1] = FLOAT32(( i * 4 ) + 2);
				pData[2] = FLOAT32(( i * 4 ) + 3);
				pData[3] = FLOAT32(( i * 4 ) + 4);
				
				D3D11_SUBRESOURCE_DATA initData;
				initData.pSysMem = &pData;
				initData.SysMemPitch = sizeof(float) * 2;
				initData.SysMemSlicePitch = 0;
				if( FAILED( GetDevice()->CreateTexture2D( &desc, &initData, (ID3D11Texture2D**)&m_pResources[i] ) ) )
				{
					WriteToLog( "CreateTexture2D() failed" );
					return RESULT_FAIL;
				}
				
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = 1;

				if( FAILED( GetDevice()->CreateShaderResourceView( m_pResources[i], &srvDesc, &m_pSRVs[i] ) ) )
				{
					WriteToLog( "CreateShaderResourceView() failed" );
					return RESULT_FAIL;
				}
			}

			GetEffectiveContext()->VSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->GSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->PSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->HSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->DSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->CSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			
			break;
		}
		
		case INDEXING_SCENARIO_TEXTURE3D:
		{
			for( UINT i = 0; i < NUM_INDEXABLE_RESOURCES; ++i )
			{
				// Setup the constant buffer with gray code numbers.
				D3D11_TEXTURE3D_DESC desc;
				desc.Width = 2;
				desc.Height = 2;
				desc.Depth = 2;
				desc.MipLevels = 1;
				desc.Format = DXGI_FORMAT_R32_TYPELESS;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;

				VertexFloatElement pData[2];
				for( UINT j = 0; j < 2; ++j )
				{
					pData[j][0] = FLOAT32(( i * 8 ) + ( j * 4 ) + 1);
					pData[j][1] = FLOAT32(( i * 8 ) + ( j * 4 ) + 2);
					pData[j][2] = FLOAT32(( i * 8 ) + ( j * 4 ) + 3);
					pData[j][3] = FLOAT32(( i * 8 ) + ( j * 4 ) + 4);
				}
				
				D3D11_SUBRESOURCE_DATA initData;
				initData.pSysMem = &pData;
				initData.SysMemPitch = sizeof(float) * 2;
				initData.SysMemSlicePitch = sizeof(float) * 4;
				if( FAILED( GetDevice()->CreateTexture3D( &desc, &initData, (ID3D11Texture3D**)&m_pResources[i] ) ) )
				{
					WriteToLog( "CreateTexture3D() failed" );
					return RESULT_FAIL;
				}
				
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
				srvDesc.Texture3D.MostDetailedMip = 0;
				srvDesc.Texture3D.MipLevels = 1;

				if( FAILED( GetDevice()->CreateShaderResourceView( m_pResources[i], &srvDesc, &m_pSRVs[i] ) ) )
				{
					WriteToLog( "CreateShaderResourceView() failed" );
					return RESULT_FAIL;
				}
			}

			GetEffectiveContext()->VSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->GSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->PSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->HSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->DSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->CSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			
			break;
		}
	}
	
	tRes = BuildShaders();
	if( tRes != RESULT_PASS )
		return tRes;	

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		tRes = RESULT_FAIL;
	}

	return tRes;	
}


void CShader5xTest_Index::CleanupTestCase()
{
	ID3D11Buffer *pBufferNull[] = { NULL, NULL, NULL, NULL };
	UINT strides[] = { 0, 0, 0, 0 };
	UINT offsets[] = { 0, 0, 0, 0 };
	
	GetEffectiveContext()->VSSetConstantBuffers( 0, 1, pBufferNull );
	GetEffectiveContext()->GSSetConstantBuffers( 0, 1, pBufferNull );
	GetEffectiveContext()->PSSetConstantBuffers( 0, 1, pBufferNull );
	GetEffectiveContext()->HSSetConstantBuffers( 0, 1, pBufferNull );
	GetEffectiveContext()->DSSetConstantBuffers( 0, 1, pBufferNull );
	GetEffectiveContext()->CSSetConstantBuffers( 0, 1, pBufferNull );
	GetEffectiveContext()->IASetVertexBuffers( 0, 4, pBufferNull, strides, offsets );
	GetEffectiveContext()->IASetInputLayout( NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
	}

	SAFE_RELEASE( m_pVSBlob );
	
	SAFE_RELEASE( m_pCB );
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pLayout );
	SAFE_RELEASE( m_pRTBufferView_2 );
	SAFE_RELEASE( m_pRTUABufferView_2 );
	SAFE_RELEASE( m_pRTBuffer_2 );

	for( UINT i = 0; i < NUM_INDEXABLE_RESOURCES; ++i )
	{
		SAFE_RELEASE( m_pResources[i] );
		SAFE_RELEASE( m_pSamplers[i] );
		SAFE_RELEASE( m_pSRVs[i] );
		SAFE_RELEASE( m_pUAVs[i] );
	}
}


TEST_RESULT CShader5xTest_Index::ExecuteTestCase()
{
	if (m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	else
		GetEffectiveContext()->Draw( 1, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	// Lock the output stream and examine the results
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	bool bResult = true;

	DWORD pResult[2];
	ID3D11Buffer* pRenderTargets[2] =
	{
		m_pRTBuffer,
		m_pRTBuffer_2
	};

	// Lock the render target and examine the results
	for (int n = 0; n < ARRAYSIZE(pRenderTargets); ++n)
	{
		GetImmediateContext()->CopyResource( m_pRTBufferCopy, pRenderTargets[n] );
		if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, pResult[n]) ) )
		{
			WriteToLog( _T( "CShader5xTest_Index::ExecuteTestCase() - failed to map m_pRTBufferCopy[%d], hr = %s" ), n, D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	if( (tr = VerifyResult((void*)pResult)) == RESULT_FAIL )
	{
		WriteToLog( _T( "CShader5xTest_Index::ExecuteTestCase() - Test case failed." ) );
	}

	return tr;
}

TEST_RESULT CShader5xTest_Index::BuildShaders()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	const AssemblyHelper::Swizzle swizW = { AssemblyHelper::w, AssemblyHelper::noMask, AssemblyHelper::noMask, AssemblyHelper::noMask };

	bool bCustomVSPassThrough = false;
	bool bCustomPSPassThrough = false;
	bool bCustomGSPassThrough = false;
	const bool bPSPassThroughVerifies = true;

	tstring instructionCode;
	tstring pixelInstructionCode;

	const UINT STRING_SIZE = 256;
	char szResIndexSrc[STRING_SIZE];
	memset(szResIndexSrc, 0, STRING_SIZE);

	// Resource indexing test cases
	if( m_is >= INDEXING_SCENARIO_CONSTANT_BUFFER_UNIFORM )
	{
		int computationIndex = -1;
		for( UINT i = 0; i < (sizeof(g_pResourceIndexComputations) / sizeof(INDEXING_COMPUTATION_METHOD)); ++i )
			if( g_pResourceIndexComputations[i] == m_icm )
				computationIndex = i;

		if( computationIndex == -1 )
		{
			WriteToLog( "Skipping unhandled indexing computation" );
			return RESULT_SKIP;
		}

		const char* pResTypeStr[] = { "buffer", "Texture2D", "Texture3D" };
		const char *szResType = NULL;
		if( m_is == INDEXING_SCENARIO_BUFFER )
			szResType = pResTypeStr[0];
		else if( m_is == INDEXING_SCENARIO_TEXTURE2D || m_is == INDEXING_SCENARIO_SAMPLER )
			szResType = pResTypeStr[1];
		else if( m_is == INDEXING_SCENARIO_TEXTURE3D )
			szResType = pResTypeStr[2];

		if( m_is == INDEXING_SCENARIO_SAMPLER )
		{
			m_fTexCoordX = 2.0f;
			m_fTexCoordY = 2.0f;
			m_fTexCoordZ = 2.0f;
		}	

		if( m_ii == INDEXING_INSTRUCTION_MOV )
		{
			_snprintf(szResIndexSrc, STRING_SIZE, g_szResourceIndexMovSrc, g_pResourceIndexExpr[computationIndex]);
		}
		else if( m_ii == INDEXING_INSTRUCTION_LD )
		{
			// ld uses unsigned ints to index into the texture
			_snprintf(szResIndexSrc, STRING_SIZE, g_szResourceIndexLoadSrc, 
				UINT32(m_fTexCoordX), UINT32(m_fTexCoordY), UINT32(m_fTexCoordZ), 0,
				szResType,
				g_pResourceIndexExpr[computationIndex]);
		}
		else if( m_ii == INDEXING_INSTRUCTION_SAMPLE )
		{
			_snprintf(szResIndexSrc, STRING_SIZE, g_szResourceIndexSampleSrc, 
				m_fTexCoordX, m_fTexCoordY, m_fTexCoordZ, 0.0f,
				szResType,
				g_pResourceIndexExpr[computationIndex],
				g_pResourceIndexExpr[computationIndex]);
		}
		else
		{
			WriteToLog( "Skipping unhandled indexing instruction" );
			return RESULT_SKIP;
		}
	}

	const bool bFL10 = g_TestApp.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0;
	tstring extraMovStr = "";

	switch( m_is )
	{
		case INDEXING_SCENARIO_TEMP:
			instructionCode = szTempIndexSrc;
			break;
			
		case INDEXING_SCENARIO_TEMP_NESTED:
			instructionCode = szTempIndexNestedSrc;
			break;
			
		case INDEXING_SCENARIO_CONSTANT_BUFFER:
			instructionCode = szConstBufferIndexSrc;
			break;
			
		case INDEXING_SCENARIO_CONSTANT_BUFFER_NESTED:
			instructionCode = szConstBufferIndexNestedSrc;
			break;
			
		case INDEXING_SCENARIO_INPUT_OUTPUT:
			switch (m_ShaderStage)
			{
			case D3D_SHADER_STAGE_VERTEX:
				if( !bFL10 ) // VS and GS only have 16I/O registers on FL 10.0
					extraMovStr = "mov o[r0.y + 20], r0.zzzz"; // replicate r0.z to o[r0.x + 16]

				instructionCode = FormatString( szVSInputIndexSrc, extraMovStr.c_str() );
				break;
			case D3D_SHADER_STAGE_GEOMETRY:
			case D3D_SHADER_STAGE_HULL:
					extraMovStr = "mov o[r0.y + 20], r0.zzzz"; // replicate r0.z to o[r0.x + 16]

				instructionCode = FormatString( szGSInputIndexSrc, extraMovStr.c_str() );
				break;
			case D3D_SHADER_STAGE_DOMAIN:
				extraMovStr = "mov o[r0.y + 20], r0.zzzz"; // replicate r0.z to o[r0.x + 16]

				instructionCode = FormatString( szDSInputIndexSrc, extraMovStr.c_str() );
				break;
			case D3D_SHADER_STAGE_PIXEL:
				instructionCode = szPSInputIndexSrc;
				break;
			}
			pixelInstructionCode = szPSInputIndexSrc;
			break;
			
		case INDEXING_SCENARIO_INPUT_OUTPUT_NESTED:
			switch (m_ShaderStage)
			{
			case D3D_SHADER_STAGE_VERTEX:
				if( !bFL10 ) // VS and GS only have 16I/O registers on FL 10.0
					extraMovStr = "mov o[x0[0].y + 20], x0[0].zzzz"; // replicate x0[0].z to o[x0[0].x + 16]

				instructionCode = FormatString( szVSInputIndexNestedSrc, extraMovStr.c_str() );
				break;
			case D3D_SHADER_STAGE_GEOMETRY:
			case D3D_SHADER_STAGE_HULL:
					extraMovStr = "mov o[x0[0].y + 20], x0[0].zzzz"; //"mov o[x0[0].y + 20], x0[0].zzzz\n" // replicate x0[0].z to o[x0[0].x + 16]

				instructionCode = FormatString( szGSInputIndexNestedSrc, extraMovStr.c_str() );
				break;
			case D3D_SHADER_STAGE_DOMAIN:
					extraMovStr = "mov o[x0[0].y + 20], x0[0].zzzz\n";   // replicate x0[0].z to o[x0[0].x + 16]

				instructionCode = FormatString( szDSInputIndexNestedSrc, extraMovStr.c_str() );
				break;
			case D3D_SHADER_STAGE_PIXEL:
				instructionCode = szPSInputIndexNestedSrc;
				break;
			}
			pixelInstructionCode = szPSInputIndexSrc;
			break;
			
		case INDEXING_SCENARIO_CONSTANT_BUFFER_UNIFORM:
		case INDEXING_SCENARIO_SAMPLER:
		case INDEXING_SCENARIO_BUFFER:
		case INDEXING_SCENARIO_TEXTURE2D:
		case INDEXING_SCENARIO_TEXTURE3D:
			instructionCode = szResIndexSrc;
			break;

		default:
			WriteToLog( "Skipping unhandled indexing scenario" );
			return RESULT_SKIP;
	}

	AssemblyHelper asmHelper(0, 0);
	AssemblyHelper asmHelperGeometry(0, 1);
	AssemblyHelper asmHelperPixel(0, 2);
	const bool bIsFL10AndCompute = (!g_TestApp.IsFL11OrGreater() && m_ShaderStage == D3D_SHADER_STAGE_COMPUTE);

	// The number of input/output registers for 10 was 16. D3D10.1 increased that to 32.
	const UINT numVSGSInputRegisters = bFL10 ? 16 : 32;
	const UINT numIEdescs = numVSGSInputRegisters;
	const bool bVSOrGS = m_ShaderStage == D3D_SHADER_STAGE_VERTEX || m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY;
	const UINT numInputRegisters = ( bVSOrGS ) ? numVSGSInputRegisters : 32;
	const UINT numOutputRegisters = ( m_ShaderStage == D3D_SHADER_STAGE_VERTEX && bFL10 ) ? 16 : 32;
	const UINT positionRegister = 16;

	switch( m_is )
	{
		case INDEXING_SCENARIO_TEMP:
			// use implicit position at center of RT
			asmHelper = AssemblyHelper(0, 1);
			asmHelper.SetIndexableTempSize( 0, 4095 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );

			if( !bIsFL10AndCompute )
				asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;

		case INDEXING_SCENARIO_TEMP_NESTED:
			// use implicit position at center of RT
			asmHelper = AssemblyHelper(0, 0);
			asmHelper.SetIndexableTempSize( 0, 4096 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );
	
			if( !bIsFL10AndCompute )
				asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;
			
		case INDEXING_SCENARIO_CONSTANT_BUFFER:
			// use implicit position at center of RT
			asmHelper = AssemblyHelper(4096, 1);
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );

			if( !bIsFL10AndCompute )
				asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;
			
		case INDEXING_SCENARIO_CONSTANT_BUFFER_NESTED:
			// use implicit position at center of RT
			asmHelper = AssemblyHelper(4096, 0);
			asmHelper.SetIndexableTempSize( 0, 1 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );

			if( !bIsFL10AndCompute )
				asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;

		case INDEXING_SCENARIO_CONSTANT_BUFFER_UNIFORM:
			asmHelper = AssemblyHelper(1, 3);
			asmHelper.SetConstantBufferSize( 1, 1 );
			asmHelper.SetConstantBufferSize( 2, 1 );
			asmHelper.SetConstantBufferSize( 3, 1 );
			asmHelper.SetIndexableTempSize( 0, 1 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );
			asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;

		case INDEXING_SCENARIO_BUFFER:
			asmHelper = AssemblyHelper(0, 3);

			// Assembling the shader will fail if there is no decl and we are indexing using a immediate const
			// It’s expected because it's a case of using the indexable modifier without actually indexing anything. In this case, a decl is required.
			if( INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST == m_icm )
			{
				asmHelper.SetSamplerDecl(0, "mode_default");
				asmHelper.SetSamplerDecl(1, "mode_default");
				asmHelper.SetSamplerDecl(2, "mode_default");
				asmHelper.SetSamplerDecl(3, "mode_default");
				asmHelper.SetResourceDecl(0, "buffer, float");
				asmHelper.SetResourceDecl(1, "buffer, float");
				asmHelper.SetResourceDecl(2, "buffer, float");
				asmHelper.SetResourceDecl(3, "buffer, float");
			}

			asmHelper.SetIndexableTempSize( 0, 1 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );
			asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;

		case INDEXING_SCENARIO_TEXTURE2D:
			asmHelper = AssemblyHelper(0, 3);

			// Assembling the shader will fail if there is no decl and we are indexing using a immediate const
			// It’s expected because it's a case of using the indexable modifier without actually indexing anything. In this case, a decl is required.
			if( INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST == m_icm )
			{
				asmHelper.SetSamplerDecl(0, "mode_default");
				asmHelper.SetSamplerDecl(1, "mode_default");
				asmHelper.SetSamplerDecl(2, "mode_default");
				asmHelper.SetSamplerDecl(3, "mode_default");
				asmHelper.SetResourceDecl(0, "Texture2D, float");
				asmHelper.SetResourceDecl(1, "Texture2D, float");
				asmHelper.SetResourceDecl(2, "Texture2D, float");
				asmHelper.SetResourceDecl(3, "Texture2D, float");
			}

			asmHelper.SetIndexableTempSize( 0, 4 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );
			asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;

		case INDEXING_SCENARIO_TEXTURE3D:
			asmHelper = AssemblyHelper(0, 3);

			// Assembling the shader will fail if there is no decl and we are indexing using a immediate const
			// It’s expected because it's a case of using the indexable modifier without actually indexing anything. In this case, a decl is required.
			if( INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST == m_icm )
			{
				asmHelper.SetSamplerDecl(0, "mode_default");
				asmHelper.SetSamplerDecl(1, "mode_default");
				asmHelper.SetSamplerDecl(2, "mode_default");
				asmHelper.SetSamplerDecl(3, "mode_default");
				asmHelper.SetResourceDecl(0, "Texture3D, float");
				asmHelper.SetResourceDecl(1, "Texture3D, float");
				asmHelper.SetResourceDecl(2, "Texture3D, float");
				asmHelper.SetResourceDecl(3, "Texture3D, float");
			}

			asmHelper.SetIndexableTempSize( 0, 4 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );
			asmHelper.SetOutputRegMaskRaw( 1, Ass