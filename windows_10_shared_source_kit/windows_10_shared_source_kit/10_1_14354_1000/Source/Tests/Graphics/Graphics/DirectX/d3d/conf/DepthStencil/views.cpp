#include <WGFTestCore.h>

#include "views.h"

void CDepthStencilTest::CreateFormatMap()
{
	m_FormatMap.clear();

	DSFormatInfo format;
	format.Format			= DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	format.HasStencil		= true;
	format.DepthBits		= 32;
	format.StencilBits		= 8;
	format.TotalBits		= 64;
	format.DepthCompType	= COMP_FLOAT;
	format.StencilCompType	= COMP_UINT;
	m_FormatMap[ DXGI_FORMAT_D32_FLOAT_S8X24_UINT ] = format;

	format.Format			= DXGI_FORMAT_D32_FLOAT;
	format.HasStencil		= false;
	format.DepthBits		= 32;
	format.StencilBits		= 0;
	format.TotalBits		= 32;
	format.DepthCompType	= COMP_FLOAT;
	m_FormatMap[ DXGI_FORMAT_D32_FLOAT ] = format;

	format.Format			= DXGI_FORMAT_D16_UNORM;
	format.HasStencil		= false;
	format.DepthBits		= 16;
	format.StencilBits		= 0;
	format.TotalBits		= 16;
	format.DepthCompType	= COMP_UNORM;
	m_FormatMap[ DXGI_FORMAT_D16_UNORM ] = format;

	format.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	format.HasStencil		= true;
	format.DepthBits		= 24;
	format.StencilBits		= 8;
	format.TotalBits		= 32;
	format.DepthCompType	= COMP_UNORM;
	format.StencilCompType	= COMP_UINT;
	m_FormatMap[ DXGI_FORMAT_D24_UNORM_S8_UINT ] = format;
}

void CDepthStencilTest::InitTestParameters()
{
	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T("Format"), &m_Format );
	testfactor::RFactor rfFormat = AddParameterValueSet( pFormatParam, FormatValueSet( FF_DS, 0 ) );

	CTestCaseParameter<D3D11_DSV_DIMENSION> *pDimParam = AddParameter( _T( "Dimension" ), &m_ResourceDimension );
	testfactor::RFactor rfTex1D = AddParameterValue( pDimParam, D3D11_DSV_DIMENSION_TEXTURE1D );
	testfactor::RFactor rfTex1DArray = AddParameterValue( pDimParam, D3D11_DSV_DIMENSION_TEXTURE1DARRAY );
	testfactor::RFactor rfTex2D = AddParameterValue( pDimParam, D3D11_DSV_DIMENSION_TEXTURE2D );
	testfactor::RFactor rfTex2DArray = AddParameterValue( pDimParam, D3D11_DSV_DIMENSION_TEXTURE2DARRAY );
	

	CTestCaseParameter<UINT> *pMipParam = AddParameter( _T( "MipSlice" ), &m_uMipSlice );
	CUserValueSet<UINT> *pMipValues = new CUserValueSet<UINT>( );
	pMipValues->AddValue( 0 );
	pMipValues->AddValue( 1 );
	pMipValues->AddValue( 2 );
	testfactor::RFactor rfMipSlice = AddParameterValueSet( pMipParam, pMipValues );

	CTestCaseParameter<UINT> *pArrayParam = AddParameter( _T( "ArraySlice" ), &m_uArraySlice );
	CUserValueSet<UINT> *pArrayValues = new CUserValueSet<UINT>( );
	pArrayValues->AddValue( 0 );
	pArrayValues->AddValue( 1 );
	pArrayValues->AddValue( 2 );
	pArrayValues->AddValue( 3 );
	testfactor::RFactor rfArraySlice = AddParameterValueSet( pArrayParam, pArrayValues );

	CTestCaseParameter<UINT32> *pWidthParam = AddParameter( _T( "Width" ), &m_uWidth );
	CUserValueSet<UINT32> *pWidthValues = new CUserValueSet<UINT32>( );
	pWidthValues->AddValue( 256 );
	pWidthValues->AddValue( 90 );
	pWidthValues->AddValue( 1 );
	testfactor::RFactor rfWidth = AddParameterValueSet( pWidthParam, pWidthValues );

	CTestCaseParameter<UINT32> *pHeightParam = AddParameter( _T( "Height" ), &m_uHeight );
	CUserValueSet<UINT32> *pHeightValues = new CUserValueSet<UINT32>( );
	pHeightValues->AddValue( 128 );
	pHeightValues->AddValue( 70 );
	pHeightValues->AddValue( 1 );
	testfactor::RFactor rfHeight = AddParameterValueSet( pHeightParam, pHeightValues );

	CTestCaseParameter<float> *pDepthParam = AddParameter( _T( "PrimitiveDepth" ), &m_fDepth );
	CUserValueSet<float> *pDepthValues = new CUserValueSet<float>( );
	pDepthValues->AddValue( 0.f );
	pDepthValues->AddValue( 0.25f );
	pDepthValues->AddValue( 0.75f );
	testfactor::RFactor rfDepth = AddParameterValueSet( pDepthParam, pDepthValues );

	SetRootTestFactor( 
		( rfTex1D * rfFormat * rfMipSlice * rfWidth * rfDepth ) +
		( rfTex1DArray * rfFormat * rfMipSlice * rfArraySlice * rfWidth * rfDepth ) +
		( rfTex2D * rfFormat * rfMipSlice * ( rfWidth % rfHeight ) * rfDepth ) +
		( rfTex2DArray * rfFormat * rfMipSlice * rfArraySlice * ( rfWidth % rfHeight ) * rfDepth )
	);
}

TEST_RESULT CDepthStencilTest::Setup()
{
	TEST_RESULT tr = CD3D11Test::Setup( );
	if( tr != RESULT_PASS )
		return tr;

	HRESULT hr;

	// create and set rasterizer state
	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = false;
	rastDesc.DepthBias = false;
	rastDesc.DepthBiasClamp = 0;
	rastDesc.SlopeScaledDepthBias = 0;
	rastDesc.DepthClipEnable = false;
	rastDesc.ScissorEnable = false;
	rastDesc.MultisampleEnable = false;
	rastDesc.AntialiasedLineEnable = false;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSEnabled );

	GetEffectiveContext()->RSSetState( m_pRSEnabled );

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		return RESULT_FAIL;
	}

	// create and set sampler
	m_SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	m_SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.MipLODBias = 0;
	m_SamplerDesc.MaxAnisotropy = 1;
	m_SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	m_SamplerDesc.BorderColor[0] = 0.5f;
	m_SamplerDesc.BorderColor[1] = 0.5f;
	m_SamplerDesc.BorderColor[2] = 0.5f;
	m_SamplerDesc.BorderColor[3] = 1.0f;
	m_SamplerDesc.MinLOD = 0;
	m_SamplerDesc.MaxLOD = 5;

	if( FAILED( hr = GetDevice()->CreateSamplerState(&m_SamplerDesc, &m_pSampler ) ))
	{
		WriteToLog( _T("CRenderTargetsTest::Setup() - CreateSamplerState() failed. hr = 0x%x"), hr );
		return RESULT_FAIL;
	}

	if( (tr = SetupGeometry( )) != RESULT_PASS )
		return tr;

	if( (tr = SetupShaders( )) != RESULT_PASS )
		return tr;


	return RESULT_PASS;
}

TEST_RESULT CDepthStencilTest::SetupGeometry()
{
	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.ByteWidth      = sizeof( TLVERTEX );
	buffer_desc.Usage          = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags      = 0;

	HRESULT hr;
	if( FAILED( hr = GetDevice()->CreateBuffer( &buffer_desc, NULL, &m_pVertexBuffer) ) )
	{
		WriteToLog( _T("CDepthStencilTest::SetupGeometry() - CreateBuffer() for Stream Input Buffer failed. hr = 0x%x"), hr );
		return RESULT_FAIL;
	}

	UINT StridesTL = sizeof( TLVERTEX );
	UINT Offset = 0;

	TLVERTEX vertices [ ] = 
	{
		{ 0.f, 0.f, 0.f, 0.f, 0.f, 0.f }
	};

	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if( FAILED( hr = GetEffectiveContext()->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes ) ) )
	{
		WriteToLog( _T("CDepthStencilTest::SetupGeometry() - Map() failed. hr = 0x%x"), hr );
		return RESULT_FAIL;
	}

	memcpy( mappedRes.pData, vertices, sizeof( vertices ) );
	GetEffectiveContext( )->Unmap( m_pVertexBuffer, 0 );

	GetEffectiveContext( )->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	GetEffectiveContext( )->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &StridesTL, &Offset);

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

TEST_RESULT CDepthStencilTest::SetupShaders()
{
	TEST_RESULT bResult = RESULT_PASS;
	HRESULT hr;
	D3DXMATRIX MatWorld;

	m_pVS = NULL; 
	m_pGS = NULL;
	m_pPS = NULL;

	GetEffectiveContext()->VSSetSamplers(0, 1, &m_pSampler);
	GetEffectiveContext()->GSSetSamplers(0, 1, &m_pSampler);
	GetEffectiveContext()->PSSetSamplers(0, 1, &m_pSampler);

	// create shaders and layout
	ID3D10Blob *pGShaderBlob = NULL;
	ID3D10Blob *pGErrorBlob = NULL;
	ID3D10Blob *pPShaderBlob = NULL;
	ID3D10Blob *pPErrorBlob = NULL;
	ID3D10Blob *pVShaderBlob = NULL;
	ID3D10Blob *pVErrorBlob = NULL;

	if( FAILED( hr = D3DX10CompileFromResource(	
		NULL, 
		"RC_VIEWS_GSH",
		NULL,
		NULL,
		NULL,
		"GShaderMain", 
		"gs_4_0", 
		D3D10_SHADER_DEBUG, 
		0,
		NULL,
		&pGShaderBlob, 
		&pGErrorBlob,
		NULL ) ) )
	{
		WriteToLog( _T( "CDepthStencilTest::Setup() - D3DXCompileShaderFromResource() failed with geometry shader, hr = %s, errors: \n%s" ), D3DHResultToString(hr).c_str(), pGErrorBlob->GetBufferPointer( ) );
		goto Fail;
	}

	if( FAILED( hr = GetDevice()->CreateGeometryShader( pGShaderBlob->GetBufferPointer(), pGShaderBlob->GetBufferSize( ), NULL, &m_pGS ) ) )
	{
		WriteToLog( _T( "CDepthStencilTest::Setup() - CreateGeometryShader failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		goto Fail;
	}

	if( FAILED( hr = D3DX10CompileFromResource(	
		NULL, 
		"RC_VIEWS_PSH",
		NULL,
		NULL,
		NULL,
		"PShaderMain", 
		"ps_4_0", 
		D3D10_SHADER_DEBUG, 
		0,
		NULL,
		&pPShaderBlob, 
		&pPErrorBlob,
		NULL ) ) )
	{
		WriteToLog( _T( "CDepthStencilTest::Setup() - D3DXCompileShaderFromResource() failed with pixel shader, hr = %s, errors: \n%s" ), D3DHResultToString(hr).c_str(), pPErrorBlob->GetBufferPointer( ) );
		goto Fail;
	}

	if( FAILED( hr = GetDevice()->CreatePixelShader( pPShaderBlob->GetBufferPointer(), pPShaderBlob->GetBufferSize( ), NULL, &m_pPS ) ) )
	{
		WriteToLog( _T( "CDepthStencilTest::Setup() - CreatePixelShader failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		goto Fail;
	}

	if( FAILED( hr = D3DX10CompileFromResource(	
		NULL, 
		"RC_VIEWS_VSH", 
		NULL,
		NULL,
		NULL,
		"VShaderMain", 
		"vs_4_0", 
		D3D10_SHADER_DEBUG,
		0,
		NULL,
		&pVShaderBlob, 
		&pVErrorBlob,
		NULL ) ) )
	{
		WriteToLog( _T( "CDepthStencilTest::Setup() - D3DXCompileShaderFromResource() failed with vertex shader, hr = %s, errors: \n%s" ), D3DHResultToString(hr).c_str(), pVErrorBlob->GetBufferPointer( ) );
		goto Fail;
	}

	if( FAILED( hr = GetDevice()->CreateVertexShader( pVShaderBlob->GetBufferPointer(), pVShaderBlob->GetBufferSize( ), NULL, &m_pVS ) ) )
	{
		WriteToLog( _T( "CDepthStencilTest::Setup() - CreateVertexShader failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		goto Fail;
	}

	// setup input layout
	D3D11_INPUT_ELEMENT_DESC inputDesc [ ] =
	{
		{"pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},  //position
		{"texCoord", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},  //texture coordinates
	};

	if( FAILED( hr = GetDevice()->CreateInputLayout( inputDesc, sizeof( inputDesc ) / sizeof( *inputDesc ), pVShaderBlob->GetBufferPointer(), pVShaderBlob->GetBufferSize( ), &m_pLayout ) ) )
	{
		WriteToLog( _T( "CDepthStencilTest::Setup() - CreateInputLayout failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		goto Fail;
	}

	GetEffectiveContext()->IASetInputLayout( m_pLayout );

	// setup constant buffer
	D3D11_BUFFER_DESC bufdesc;
	bufdesc.ByteWidth = 512;
	bufdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufdesc.MiscFlags = 0;
	GetDevice()->CreateBuffer( &bufdesc, NULL, &m_pConstBuffer );

	GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		goto Fail;
	}

	float* pConstData = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	GetEffectiveContext()->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
	pConstData = (float*) mappedRes.pData;

	D3DXMatrixIdentity( &MatWorld );
	float transx = (float) m_uWidth / 2.f;
	float transy = (float) m_uHeight / 2.f;

	// Inverse viewport scale.
	MatWorld._13 = 1 / transx;
	MatWorld._14 = -1 / transy;

	// Prescaled inverse viewport translation.
	MatWorld._11 = -transx * MatWorld._13;
	MatWorld._12 = -transy * MatWorld._14;

	m_mxTrans = MatWorld;

	memcpy( pConstData, MatWorld, sizeof( MatWorld ) );
	GetEffectiveContext()->Unmap(m_pConstBuffer,0);

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		goto Fail;
	}

	goto Pass;
Fail:
	bResult = RESULT_FAIL;
Pass:
	SAFE_RELEASE( pGShaderBlob );
	SAFE_RELEASE( pGErrorBlob );
	SAFE_RELEASE( pPShaderBlob );
	SAFE_RELEASE( pPErrorBlob );
	SAFE_RELEASE( pVShaderBlob );
	SAFE_RELEASE( pVErrorBlob );

	return bResult;
}

TEST_RESULT CDepthStencilTest::SetupTestCase()
{
	D3D11_TEXTURE1D_DESC tex1DDesc;
	D3D11_TEXTURE2D_DESC tex2DDesc;
	HRESULT hr;

	switch( m_ResourceDimension )
	{
	case D3D11_DSV_DIMENSION_TEXTURE1D:
	case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
		
		tex1DDesc.ArraySize			= m_uArraySize;
		tex1DDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
		tex1DDesc.CPUAccessFlags	= 0;
		tex1DDesc.Format			= m_Format;
		tex1DDesc.MipLevels			= 0;
		tex1DDesc.MiscFlags			= 0;
		tex1DDesc.Usage				= D3D11_USAGE_DEFAULT;
		tex1DDesc.Width				= m_uWidth;

		if( FAILED( hr = GetDevice()->CreateTexture1D( &tex1DDesc, NULL, (ID3D11Texture1D**) &m_pResourceDS ) ) )
		{
			WriteToLog( _T( "CDepthStencilTest::SetupTestCase() - CreateTexture1D() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		tex1DDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;
		tex1DDesc.BindFlags			= 0;
		tex1DDesc.Usage				= D3D11_USAGE_STAGING;

		if( FAILED( hr = GetDevice()->CreateTexture1D( &tex1DDesc, NULL, (ID3D11Texture1D**) &m_pResourceDSCopy ) ) )
		{
			WriteToLog( _T( "CDepthStencilTest::SetupTestCase() - CreateTexture1D() failed for copy texture, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		( reinterpret_cast<ID3D11Texture1D*>( m_pResourceDS ) )->GetDesc( &tex1DDesc );
		m_uMipLevels = tex1DDesc.MipLevels;

		break;
	case D3D11_DSV_DIMENSION_TEXTURE2D:
	case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
		
		tex2DDesc.ArraySize			= m_uArraySize;
		tex2DDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
		tex2DDesc.CPUAccessFlags	= 0;
		tex2DDesc.Format			= m_Format;
		tex2DDesc.MipLevels			= 0;
		tex2DDesc.MiscFlags			= 0;
		tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
		tex2DDesc.Width				= m_uWidth;
		tex2DDesc.Height			= m_uHeight;
		tex2DDesc.SampleDesc.Count	= 1;
		tex2DDesc.SampleDesc.Quality = 0;

		if( FAILED( hr = GetFramework()->CreateTexture2DTiledOptional( &tex2DDesc, NULL, (ID3D11Texture2D**) &m_pResourceDS ) ) )
		{
			WriteToLog( _T( "CDepthStencilTest::SetupTestCase() - CreateTexture2D() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		tex2DDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;
		tex2DDesc.BindFlags			= 0;
		tex2DDesc.Usage				= D3D11_USAGE_STAGING;

		if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, (ID3D11Texture2D**) &m_pResourceDSCopy ) ) )
		{
			WriteToLog( _T( "CDepthStencilTest::SetupTestCase() - CreateTexture2D() failed for copy texture, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		( reinterpret_cast<ID3D11Texture2D*>( m_pResourceDS ) )->GetDesc( &tex2DDesc );
		m_uMipLevels = tex2DDesc.MipLevels;

		break;
	}

	if( m_uMipLevels <= m_uMipSlice )
		m_uMipSlice = m_uMipLevels - 1;
	
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = m_Format;
	dsvDesc.ViewDimension = m_ResourceDimension;
	dsvDesc.Flags = 0;
	
	switch( m_ResourceDimension )
	{
	case D3D11_DSV_DIMENSION_TEXTURE1D:
		dsvDesc.Texture1D.MipSlice = m_uMipSlice;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
		dsvDesc.Texture1DArray.ArraySize = 1;
		dsvDesc.Texture1DArray.FirstArraySlice = m_uArraySlice;
		dsvDesc.Texture1DArray.MipSlice = m_uMipSlice;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE2D:
		dsvDesc.Texture2D.MipSlice = m_uMipSlice;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
		dsvDesc.Texture2DArray.ArraySize = 1;
		dsvDesc.Texture2DArray.FirstArraySlice = m_uArraySlice;
		dsvDesc.Texture2DArray.MipSlice = m_uMipSlice;
		break;
	}

	if( FAILED( hr = GetDevice()->CreateDepthStencilView( m_pResourceDS, &dsvDesc, &m_pDSV ) ) )
	{
		WriteToLog( _T( "CDepthStencilTest::SetupTestCase() - CreateDepthStencilView() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	// Using default values for D3D11_DEPTH_STENCIL_DESC struct.
	m_DSDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
	m_DSDesc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_ZERO;
	m_DSDesc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_DECR;
	m_DSDesc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;
	
	m_DSDesc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
	m_DSDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_ZERO;
	m_DSDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_INCR;
	m_DSDesc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

	m_DSDesc.DepthEnable		= true;
	m_DSDesc.StencilEnable		= true;
	m_DSDesc.DepthFunc			= D3D11_COMPARISON_LESS;
	m_DSDesc.DepthWriteMask		= D3D11_DEPTH_WRITE_MASK_ALL;
	m_DSDesc.StencilReadMask	= 0xff;
	m_DSDesc.StencilWriteMask	= 0xff;


	if( FAILED( hr = GetDevice()->CreateDepthStencilState( &m_DSDesc, &m_pDSState ) ) )
	{
		WriteToLog( _T( "CDepthStencilTest::SetupTestCase() - CreateDepthStencilState() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	// Using zero as default StencilRef.
	GetEffectiveContext()->OMSetDepthStencilState( m_pDSState, m_uStencilRef );

	ID3D11RenderTargetView *pRTV = GetRTView();
	GetEffectiveContext()->OMSetRenderTargets( 1, &pRTV, m_pDSV );

	float height = (float) m_uHeight;
	if( m_ResourceDimension == D3D11_DSV_DIMENSION_TEXTURE1D || m_ResourceDimension == D3D11_DSV_DIMENSION_TEXTURE1DARRAY )
		height = 1.f;

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float) m_uWidth;
	vp.Height   = (float) (UINT)height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	GetEffectiveContext()->RSSetViewports(1, &vp);

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		return RESULT_FAIL;
	}

	// pass current width and height to shader
	void *pData = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if( FAILED( hr = GetEffectiveContext()->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes ) ) )
	{
		WriteToLog( _T( "CDepthStencilTest::SetupTestCase() - failed to map const buffer, hr = %s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}
	pData = mappedRes.pData;

	D3DXMatrixIdentity( &m_mxTrans );
	float transx = (float) m_uWidth / 2.f;
	float transy = height / 2.f;

	// Inverse viewport scale.
	m_mxTrans._13 = 1.f / transx;
	m_mxTrans._14 = -1.f / transy;

	// Prescaled inverse viewport translation.
	m_mxTrans._11 = -transx * m_mxTrans._13;
	m_mxTrans._12 = -transy * m_mxTrans._14;

	SIZE_T offset = sizeof( float ) * 4;
	memcpy( pData, &m_mxTrans, offset );

	float dims[ 3 ] = { (float) m_uWidth, height, m_fDepth };
	memcpy( ((BYTE*)pData) + offset, dims, sizeof(float) * 3 );
	offset += sizeof(float) * 3;

	GetEffectiveContext()->Unmap( m_pConstBuffer, 0 );
		
	return RESULT_PASS;
}

TEST_RESULT CDepthStencilTest::ExecuteTestCase()
{
	GetEffectiveContext()->ClearDepthStencilView( m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, m_fDepthClear, m_uStencilClear );
	
	if( !m_bClearTest )
		GetEffectiveContext()->Draw( 1, 0 );

	GetEffectiveContext()->CopyResource( m_pResourceDSCopy, m_pResourceDS );

	HRESULT hr;
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		return RESULT_FAIL;
	}

	UINT uPitch;
	D3D11_MAPPED_SUBRESOURCE texMap;
	void *pResult = NULL;

	UINT uSubRes = m_uArraySlice * m_uMipLevels + m_uMipSlice;
	
	switch( m_ResourceDimension )
	{
	case D3D11_DSV_DIMENSION_TEXTURE1D:
		uSubRes = m_uMipSlice;
	case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
		if( FAILED( hr = GetImmediateContext()->Map( m_pResourceDSCopy, uSubRes, D3D11_MAP_READ, 0, &texMap ) ) )
		{
			WriteToLog( _T( "CDepthStencilTest::ExecuteTestCase() - failed to map depth stencil texture, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		pResult = texMap.pData;
		uPitch = m_uWidth;

		if( !CompareResults( pResult, uPitch, m_uMipSlice ) )
			return RESULT_FAIL;
		
		GetImmediateContext()->Unmap( m_pResourceDSCopy, uSubRes );

		break;
	case D3D11_DSV_DIMENSION_TEXTURE2D:
		uSubRes = m_uMipSlice;
	case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
		if( FAILED( hr = GetImmediateContext()->Map( m_pResourceDSCopy, uSubRes, D3D11_MAP_READ, 0, &texMap ) ) )
		{
			WriteToLog( _T( "CDepthStencilTest::ExecuteTestCase() - failed to map depth stencil texture, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		pResult = texMap.pData;
		uPitch = texMap.RowPitch;

		if( !CompareResults( pResult, uPitch, m_uMipSlice ) )
			return RESULT_FAIL;

		GetImmediateContext()->Unmap( m_pResourceDSCopy, uSubRes );

		break;
	}

	return RESULT_PASS;
}

bool CDepthStencilTest::CompareResults( void *pResult, UINT uPitch, UINT uMipSlice )
{
	bool bPass = true;
	bool bOneDim = m_ResourceDimension == D3D11_DSV_DIMENSION_TEXTURE1D || m_ResourceDimension == D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
	
	UINT uWidth = m_uWidth;
	UINT uHeight = bOneDim ? 1 : m_uHeight;
	if( !GetMipDimensions( uMipSlice, uWidth, uHeight ) )
		return false;
	
	float fExpectedDepth, fOutputDepth;
	UINT uExpectedStencil, uOutputStencil;
	UINT32 *pCurrentRes = (UINT32*) pResult;

	if( m_bClearTest )
	{
		UINT32 uDepth = *( (UINT32*) &m_fDepthClear );
		if( IsDenorm( m_fDepthClear ) || IsNaN( m_fDepthClear ) || uDepth == 0xff800000 )
			fExpectedDepth = 0.f;
		else if( uDepth == 0x7f800000 )
			fExpectedDepth = 1.f;
		else
			fExpectedDepth = m_fDepthClear;

		uExpectedStencil = m_uStencilClear;
	}
	else
	{
		// assuming D3D11_COMPARISON_LESS
		fExpectedDepth = min( DS_DEPTH, m_fDepth );
		uExpectedStencil = (fExpectedDepth == DS_DEPTH ? DS_STENCIL : DS_STENCIL + 1 );
	}

	DSFormatInfo format = m_FormatMap[ m_Format ];
	for( UINT y = 0; y < uHeight; ++y )
	{
		for( UINT x = 0; x < uWidth; ++x )
		{
			if( format.DepthBits == 24 )
				fOutputDepth = ToFloat32( (*pCurrentRes & MASK_24), 24, format.DepthCompType );
			else if( format.DepthBits == 16 )
				fOutputDepth = ToFloat32( (*pCurrentRes & MASK_16), 16, format.DepthCompType );
			else if( format.DepthBits == 8 )
				fOutputDepth = ToFloat32( (*pCurrentRes & MASK_8), 8, format.DepthCompType );
			else if( format.DepthCompType == COMP_FLOAT )
				fOutputDepth = *( (float*) pCurrentRes );

			if( format.HasStencil )
			{
				if( format.DepthBits >= 32 )
					uOutputStencil = *( pCurrentRes + 1 ) & MASK_8;
				else
					uOutputStencil = (*pCurrentRes & 0xff000000) >> 24;
			}

			// check depth values
			if( bPass && fabs( fExpectedDepth - fOutputDepth ) > 0.0001f )
			{
				WriteToLog( _T( "CDepthStencilTest::CompareResults() - Test case failed, depth value does not match in pixel (%d,%d)\n\t\t\tExpected: %f\n\t\t\tOutput: %f" ), x, y, fExpectedDepth, fOutputDepth );
				bPass = false;
			}
						
			// check stencil values
			if( bPass && format.HasStencil && uOutputStencil != uExpectedStencil )
			{
				WriteToLog( _T( "CDepthStencilTest::CompareResults() - Test case failed, stencil value does not match in pixel (%d,%d)\n\t\t\tExpected: %d\n\t\t\tOutput: %d" ), x, y, uExpectedStencil, uOutputStencil );
				bPass = false;
			}

			pCurrentRes += ( format.TotalBits / 32 );
		}
		pCurrentRes = (UINT32*) (((BYTE*)pResult) + (uPitch * (y + 1)));
	}
	
	return bPass;
}

float CDepthStencilTest::ToFloat32( UINT32 uData, UINT uBits, ComponentType CompType )
{
	float fResult = 0.f;

	// for current formats this is the only conversion necessary
	if( CompType == COMP_UNORM )
	{
		UINT32 uMax = ( 1 << uBits ) - 1;
		fResult = (float) uData / (float) uMax;
	}
	
	return fResult;
}

bool CDepthStencilTest::IsDenorm( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;
	
	return ((uValue & exp) == 0) && ((uValue & frac) != 0);
}

bool CDepthStencilTest::IsNaN( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;

	return ((uValue & exp) == 0x7f800000) && ((uValue & frac) != 0);
}

bool CDepthStencilTest::GetMipDimensions( UINT uMipSlice, UINT &uWidth, UINT &uHeight )
{
	if( uWidth == 0 || uHeight == 0 )
		return false;

	UINT uCurrMip = 0;
	while( uCurrMip < uMipSlice )
	{
		if( uWidth > 1 ) uWidth /= 2;
		if( uHeight > 1 ) uHeight /= 2;
		uCurrMip++;
	}

	return true;
}

bool CDepthStencilTest::WithinOneULP( float fReference, float fOutput )
{
	// WARNING:
	// This function does not correctly handle edge cases involving negative floats, zeros
	// and NaNs. If future test result discrepancies are discovered to have been masked by this function,
	// please start using CompareFloatULP() function in Framework\Core\FloatUtils.cpp which
	// does not suffer from these problems.

	if( fReference == fOutput )
		return true;

	UINT32 uRef = *( (UINT32*) &fReference );
	UINT32 uOut = *( (UINT32*) &fOutput );
	if( (int)max( uRef, uOut ) - (int)min( uRef, uOut ) <= 1 )
		return true;

	return false;
}

void CDepthStencilTest::CleanupTestCase()
{
	if( GetDevice() )
	{
		ID3D11DepthStencilState *pNullDS = NULL;
		GetEffectiveContext()->OMSetDepthStencilState( pNullDS, 0 );

		ID3D11DepthStencilView *pNullDSV = NULL;
		ID3D11RenderTargetView *pNullRTV	= NULL;
		GetEffectiveContext()->OMSetRenderTargets( 1, &pNullRTV, pNullDSV );
		
		HRESULT hr;
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context.") );
		}

	}

	SAFE_RELEASE( m_pResourceDS );
	SAFE_RELEASE( m_pResourceDSCopy );
	SAFE_RELEASE( m_pDSV );
	SAFE_RELEASE( m_pRTV );
	SAFE_RELEASE( m_pDSState );
}

void CDepthStencilTest::Cleanup()
{
	CD3D11Test::Cleanup();

	if( GetDevice() )
	{
		ID3D11Buffer *pNullBuffers [ ] = { NULL, NULL };
		UINT uZero = 0;
		GetEffectiveContext()->VSSetConstantBuffers( 0, 2, pNullBuffers );
		GetEffectiveContext()->GSSetConstantBuffers( 0, 2, pNullBuffers );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 2, pNullBuffers );
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, pNullBuffers, &uZero, &uZero );

		ID3D11VertexShader *pVSNull = NULL;
		ID3D11GeometryShader *pGSNull = NULL;
		ID3D11PixelShader *pPSNull = NULL;
		GetEffectiveContext()->VSSetShader( pVSNull, NULL, 0 );
		GetEffectiveContext()->GSSetShader( pGSNull, NULL, 0 );
		GetEffectiveContext()->PSSetShader( pPSNull, NULL, 0 );

		ID3D11InputLayout *pILNull = NULL;
		ID3D11RasterizerState *pRSNull = NULL;
		GetEffectiveContext()->IASetInputLayout( pILNull );
		GetEffectiveContext()->RSSetState( pRSNull );

		ID3D11SamplerState *pNullSampler = NULL;
		GetEffectiveContext()->VSSetSamplers( 0, 1, &pNullSampler );
		GetEffectiveContext()->GSSetSamplers( 0, 1, &pNullSampler );
		GetEffectiveContext()->PSSetSamplers( 0, 1, &pNullSampler );

		HRESULT hr;
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context.") );
		}
	}

	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pSampler );
	SAFE_RELEASE( m_pRSEnabled );
	SAFE_RELEASE( m_pLayout );
	SAFE_RELEASE( m_pConstBuffer );
}

//---------------------------------------------------------------

/******************************
* CDepthStencilClearTest methods
******************************/

void CDepthStencilClearTest::InitTestParameters()
{
	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T("Format"), &m_Format );
	CFilterHandleCommonSource<DXGI_FORMAT> formatFilter =
		FilterRegExp<DXGI_FORMAT>( _T( "D32_FLOAT" ) );
	testfactor::RFactor rfFormat = AddParameterValueSet( pFormatParam, FormatValueSet( formatFilter ) );

	CTestCaseParameter<D3D11_DSV_DIMENSION> *pDimParam = AddParameter( _T( "Dimension" ), &m_ResourceDimension );
	testfactor::RFactor rfTex1D = AddParameterValue( pDimParam, D3D11_DSV_DIMENSION_TEXTURE1D );
	testfactor::RFactor rfTex1DArray = AddParameterValue( pDimParam, D3D11_DSV_DIMENSION_TEXTURE1DARRAY );
	testfactor::RFactor rfTex2D = AddParameterValue( pDimParam, D3D11_DSV_DIMENSION_TEXTURE2D );
	testfactor::RFactor rfTex2DArray = AddParameterValue( pDimParam, D3D11_DSV_DIMENSION_TEXTURE2DARRAY );


	CTestCaseParameter<UINT> *pMipParam = AddParameter( _T( "MipSlice" ), &m_uMipSlice );
	CUserValueSet<UINT> *pMipValues = new CUserValueSet<UINT>( );
	pMipValues->AddValue( 0 );
	pMipValues->AddValue( 1 );
	pMipValues->AddValue( 2 );
	testfactor::RFactor rfMipSlice = AddParameterValueSet( pMipParam, pMipValues );

	CTestCaseParameter<UINT> *pArrayParam = AddParameter( _T( "ArraySlice" ), &m_uArraySlice );
	CUserValueSet<UINT> *pArrayValues = new CUserValueSet<UINT>( );
	pArrayValues->AddValue( 0 );
	pArrayValues->AddValue( 1 );
	pArrayValues->AddValue( 2 );
	pArrayValues->AddValue( 3 );
	testfactor::RFactor rfArraySlice = AddParameterValueSet( pArrayParam, pArrayValues );

	CTestCaseParameter<UINT32> *pWidthParam = AddParameter( _T( "Width" ), &m_uWidth );
	CUserValueSet<UINT32> *pWidthValues = new CUserValueSet<UINT32>( );
	pWidthValues->AddValue( 128 );
	pWidthValues->AddValue( 70 );
	pWidthValues->AddValue( 1 );
	testfactor::RFactor rfWidth = AddParameterValueSet( pWidthParam, pWidthValues );

	CTestCaseParameter<UINT32> *pHeightParam = AddParameter( _T( "Height" ), &m_uHeight );
	CUserValueSet<UINT32> *pHeightValues = new CUserValueSet<UINT32>( );
	pHeightValues->AddValue( 256 );
	pHeightValues->AddValue( 90 );
	pHeightValues->AddValue( 1 );
	testfactor::RFactor rfHeight = AddParameterValueSet( pHeightParam, pHeightValues );

	CTestCaseParameter<UINT8> *pStencilClearParam = AddParameter( _T( "StencilClear" ), &m_uStencilClear );
	CUserValueSet<UINT8> *pStencilClearValues = new CUserValueSet<UINT8>( );
	pStencilClearValues->AddValue( 0 );
	pStencilClearValues->AddValue( 0xff );
	pStencilClearValues->AddValue( 0xdb );
	testfactor::RFactor rfStencilClear = AddParameterValueSet( pStencilClearParam, pStencilClearValues );

	DWORD dwInfP = 0x7f800000;
	DWORD dwInfN = 0xff800000;
	DWORD dwNaN = 0x7f800001;
	DWORD dwDenorm = 0x00000001;

	CTestCaseParameter<float> *pDepthClearParam = AddParameter( _T( "DepthClear" ), &m_fDepthClear );
	CUserValueSet<float> *pDepthClearValues = new CUserValueSet<float>( );
	pDepthClearValues->AddValue( 0.f );
//	pDepthClearValues->AddValue( *( (float*) &dwNaN ) );
	pDepthClearValues->AddValue( *( (float*) &dwDenorm ) );
	testfactor::RFactor rfDepthClear = AddParameterValueSet( pDepthClearParam, pDepthClearValues );

	SetRootTestFactor( 
		( rfTex1D * rfFormat * rfMipSlice * rfWidth * rfDepthClear * rfStencilClear ) +
		( rfTex1DArray * rfFormat * rfMipSlice * rfArraySlice * rfWidth * rfDepthClear * rfStencilClear ) +
		( rfTex2D * rfFormat * rfMipSlice * ( rfWidth % rfHeight ) * rfDepthClear * rfStencilClear ) +
		( rfTex2DArray * rfFormat * rfMipSlice * rfArraySlice * ( rfWidth % rfHeight ) * rfDepthClear * rfStencilClear )
		);
}

//---------------------------------------------------------------

