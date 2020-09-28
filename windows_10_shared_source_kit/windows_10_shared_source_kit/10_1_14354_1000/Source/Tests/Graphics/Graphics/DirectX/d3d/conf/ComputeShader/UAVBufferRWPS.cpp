#include "d3dx11async.h"

#include "UAV.h"

const char szVSPassthrough[] = 
	"float4 main( float4 input : POSITION ): SV_POSITION \n"
	"{\n"
	"	return input; \n"
	"}\n"
	;

//----------------------------------------------------------------------------------------
//
// UAVBufferRWPS.cpp - CUAVBufferRWPSTest
//
//
//
// Concrete implementation of CUAVBufferRWTest for testing on Pixel Shader.
//

//----------------------------------------------------------------------------------------

void CUAVBufferRWPSTest::InitTestParameters()
{
	// setup UAV type

	CTestCaseParameter<RESOURCE_TYPE> *pUAVResourceTypeParam = AddParameter( _T( "UAVResourceType" ), &m_UAVResourceType );
	CUserValueSet<RESOURCE_TYPE> *pUAVResourceStructuredBufferValueSet = new CUserValueSet<RESOURCE_TYPE>( );
	pUAVResourceStructuredBufferValueSet->AddValue( RESOURCE_RWSTRUCTUREDBUFFER_MIX );
	pUAVResourceStructuredBufferValueSet->AddValue( RESOURCE_RWSTRUCTUREDBUFFER_1FLOAT );
	pUAVResourceStructuredBufferValueSet->AddValue( RESOURCE_RWSTRUCTUREDBUFFER_2SINT );
	pUAVResourceStructuredBufferValueSet->AddValue( RESOURCE_RWSTRUCTUREDBUFFER_4UINT );
	testfactor::RFactor rfUAVResourceTypeStructured = AddParameterValueSet( pUAVResourceTypeParam, pUAVResourceStructuredBufferValueSet );

	CUserValueSet<RESOURCE_TYPE> *pUAVResourceByteAddrBufferValueSet = new CUserValueSet<RESOURCE_TYPE>( );
	pUAVResourceByteAddrBufferValueSet->AddValue( RESOURCE_RWBYTEADDRESSBUFFER );
	testfactor::RFactor rfUAVResourceTypeByteAddr = AddParameterValueSet( pUAVResourceTypeParam, pUAVResourceByteAddrBufferValueSet );

	CUserValueSet<RESOURCE_TYPE> *pUAVResourceTypeBufferValueSet = new CUserValueSet<RESOURCE_TYPE>( );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ) // not available for 10_*
	{
		pUAVResourceTypeBufferValueSet->AddValue( RESOURCE_RWBUFFER_UINT2 );
		pUAVResourceTypeBufferValueSet->AddValue( RESOURCE_RWBUFFER_FLOAT4 );
		pUAVResourceTypeBufferValueSet->AddValue( RESOURCE_RWBUFFER_UINT );
		pUAVResourceTypeBufferValueSet->AddValue( RESOURCE_RWBUFFER_SINT );
		pUAVResourceTypeBufferValueSet->AddValue( RESOURCE_RWBUFFER_FLOAT );
	}
	testfactor::RFactor rfUAVResourceTypeBuffer = AddParameterValueSet( pUAVResourceTypeParam, pUAVResourceTypeBufferValueSet );

	// setup UAV dxgi format

	CTestCaseParameter<DXGI_FORMAT> *pUAVFormatParam = AddParameter( _T( "UAVFormat" ), &m_UAVFormat );
	CUserValueSet<DXGI_FORMAT> *pUAVFormatStructuredBufferValueSet = new CUserValueSet<DXGI_FORMAT>( );
	pUAVFormatStructuredBufferValueSet->AddValue( DXGI_FORMAT_UNKNOWN );
	testfactor::RFactor rfUAVFormatStructured = AddParameterValueSet( pUAVFormatParam, pUAVFormatStructuredBufferValueSet );

	CUserValueSet<DXGI_FORMAT> *pUAVFormatByteAddrBufferValueSet = new CUserValueSet<DXGI_FORMAT>( );
	pUAVFormatByteAddrBufferValueSet->AddValue( DXGI_FORMAT_R32_TYPELESS );
	testfactor::RFactor rfUAVFormatByteAddr = AddParameterValueSet( pUAVFormatParam, pUAVFormatByteAddrBufferValueSet );

	CUserValueSet<DXGI_FORMAT> *pUAVFormatBufferValueSet = new CUserValueSet<DXGI_FORMAT>( );
	pUAVFormatBufferValueSet->AddValue( DXGI_FORMAT_R32_UINT );
	pUAVFormatBufferValueSet->AddValue( DXGI_FORMAT_R32_FLOAT );
	pUAVFormatBufferValueSet->AddValue( DXGI_FORMAT_R32_UINT );
	pUAVFormatBufferValueSet->AddValue( DXGI_FORMAT_R32_SINT );
	pUAVFormatBufferValueSet->AddValue( DXGI_FORMAT_R32_FLOAT );
	testfactor::RFactor rfUAVFormatBuffer = AddParameterValueSet( pUAVFormatParam, pUAVFormatBufferValueSet );

	// setup viewport size
	CUserValueSet<UINT> *pWidthValues = new CUserValueSet<UINT>();
	pWidthValues->AddValue(1);
	pWidthValues->AddValue(24);
	pWidthValues->AddValue(100);

	CUserValueSet<UINT> *pHeightValues = new CUserValueSet<UINT>();
	pHeightValues->AddValue(1);
	pHeightValues->AddValue(80);

	testfactor::RFactor rfViewportWidth= AddParameter<UINT>( _T( "ViewportWidth" ), &m_viewportWidth, pWidthValues );
	testfactor::RFactor rfViewportHeight = AddParameter<UINT>( _T( "ViewportHeight" ), &m_viewportHeight, pHeightValues );

	// testing both "UAV with RTV" and "UAV only" cases
	testfactor::RFactor rfIsUAVOnly = AddParameter<bool>( _T( "IsUAVOnly" ), &m_bUAVOnly, new CBoolValueSet() );

	SetRootTestFactor(
		( ( rfUAVResourceTypeStructured * rfUAVFormatStructured )
			+ ( rfUAVResourceTypeByteAddr * rfUAVFormatByteAddr ) 
			+ ( rfUAVResourceTypeBuffer % rfUAVFormatBuffer ) ) 
		* ( rfViewportWidth * rfViewportHeight ) % rfIsUAVOnly
	);

	
	// Adding priority filters
	CFilterHandle filterNumThreads = FilterEqual<UINT>(_T("ViewportWidth"), 24) &&
		FilterEqual<UINT>(_T("ViewportHeight"), 80);
		
	AddPriorityPatternFilter(filterNumThreads, WeightFactorLarge);
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
}

//----------------------------------------------------------------------------------------

void CUAVBufferRWPSTest::CreateInitialData()
{
	m_uByteWidth = m_uThreadByteWidth * m_viewportWidth * m_viewportHeight;

	CUAVBufferRWTest::CreateInitialData();
}


//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferRWPSTest::SetupTestCase()
{
	if ( g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
	{
		return RESULT_SKIP;
	}
	return CUAVBufferTest::SetupTestCase();
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferRWPSTest::CreateShader()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	ID3D10Blob *pPixelShaderBlob = NULL;
	ID3D10Blob *pPixelErrorBlob = NULL;
	ID3D10Blob *pVertexShaderBlob = NULL;
	ID3D10Blob *pVertexErrorBlob = NULL;

	tstring resourceType;
	
	switch( m_UAVResourceType )
	{
	case RESOURCE_RWBYTEADDRESSBUFFER:
		resourceType = _T( "UAV_RWBYTEADDRESSBUFFER" );
		break;
	case RESOURCE_RWSTRUCTUREDBUFFER_1FLOAT:
		resourceType = _T( "UAV_RWSTRUCTUREDBUFFER_1FLOAT" );
		break;
	case RESOURCE_RWSTRUCTUREDBUFFER_4UINT:
		resourceType = _T( "UAV_RWSTRUCTUREDBUFFER_4UINT" );
		break;
	case RESOURCE_RWSTRUCTUREDBUFFER_2SINT:
		resourceType = _T( "UAV_RWSTRUCTUREDBUFFER_2SINT" );
		break;
	case RESOURCE_RWSTRUCTUREDBUFFER_MIX:
		resourceType = _T( "UAV_RWSTRUCTUREDBUFFER_MIX" );
		break;
	case RESOURCE_RWBUFFER_SINT:
		resourceType = _T( "UAV_RWBUFFER_SINT" );
		break;
	case RESOURCE_RWBUFFER_UINT:
		resourceType = _T( "UAV_RWBUFFER_UINT" );
		break;
	case RESOURCE_RWBUFFER_UINT2:
		resourceType = _T( "UAV_RWBUFFER_UINT2" );
		break;
	case RESOURCE_RWBUFFER_FLOAT:
		resourceType = _T( "UAV_RWBUFFER_FLOAT" );
		break;
	case RESOURCE_RWBUFFER_FLOAT4:
		resourceType = _T( "UAV_RWBUFFER_FLOAT4" );
		break;
	}

	tstring viewportWidth = ToString( m_viewportWidth );
	tstring viewportHeight = ToString( m_viewportHeight );
	tstring inputBufferSize = ToString( m_uBufferSize );

	tstring isUAVOnly;
	if (m_bUAVOnly)
		isUAVOnly = _T("1");
	else
		isUAVOnly = _T("0");


	D3D10_SHADER_MACRO const macros[] = 
	{
		{ _T( "VIEWPORT_WIDTH" ), viewportWidth.c_str() },
		{ _T( "VIEWPORT_HEIGHT" ), viewportHeight.c_str() },
		{ _T( "BUFFER_BYTE_WIDTH_PER_THREAD" ), _T( UAV_BUFFER_RW_TEST_BUFFER_SIZE_STRING ) },
		{ _T( "DEFINE_SET" ), _T( "1" ) },
		{ _T( "PIXEL_SHADER_TEST" ), _T( "1" ) },
		{ _T( "DEFAULT_BUFFER_SIZE" ), inputBufferSize.c_str() },
		{ resourceType.c_str(), _T( "1" ) },
		{ _T( "PIXEL_SHADER_UAV_ONLY" ), isUAVOnly.c_str() },
		NULL
	};

	// Create passthrough VS
	
	hr = D3DX11CompileFromMemory( 
		szVSPassthrough, 
		strlen( szVSPassthrough ), 
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

	if( FAILED( hr ) || pVertexShaderBlob == NULL )
	{
		WriteToLog( _T( "CUAVBufferRWPSTest::CreateShader() - Failed to compile vertex shader, hr=%s, Errors=%s" ),
			D3DHResultToString( hr ).c_str(), pVertexErrorBlob ? pVertexErrorBlob->GetBufferPointer() : "" );
		tr = RESULT_FAIL;
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
		WriteToLog( _T( "CUAVBufferRWPSTest::CreateShader() - Failed to create vertex shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto Done;
	}


	//Create input layout
	
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
		WriteToLog( _T( "CUAVBufferRWPSTest::ExecuteTestCase() - CreateInputLayout() failed" ) );
		tr = RESULT_FAIL;
		goto Done;
	}

	// Create pixel shader

	hr = D3DX11CompileFromResource(
		NULL,
		_T( "RC_UAV_BUFFER_RW_FX" ),
		_T( "UAVBufferRW.fx" ),
		macros,
		NULL,
		_T( "PSMain" ),
		_T("ps_5_0"),
		D3D10_SHADER_DEBUG, 
		D3D10_SHADER_DEBUG, 
		NULL,
		&pPixelShaderBlob,
		&pPixelErrorBlob,
		NULL
	);

	if( FAILED( hr ) || pPixelShaderBlob == NULL )
	{
		WriteToLog( _T( "CUAVBufferRWPSTest::CreateShader() - Failed to compile pixel shader, hr=%s, Errors=%s" ),
			D3DHResultToString( hr ).c_str(), pPixelErrorBlob ? pPixelErrorBlob->GetBufferPointer() : "" );
		tr = RESULT_FAIL;
		goto Done;
	}

	hr = GetDevice()->CreatePixelShader( 
		pPixelShaderBlob->GetBufferPointer(), 
		pPixelShaderBlob->GetBufferSize(), 
		NULL, 
		&m_pPixelShader
	);

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CUAVBufferRWPSTest::CreateShader() - Failed to create pixel shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
	}

Done:
	SAFE_RELEASE( pVertexShaderBlob );
	SAFE_RELEASE( pVertexErrorBlob );
	SAFE_RELEASE( pPixelShaderBlob );
	SAFE_RELEASE( pPixelErrorBlob );

	return tr;
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferRWPSTest::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;

	// Create input buffers
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
		WriteToLog( _T( "CUAVBufferTypedPSTest::ExecuteTestCase() - CreateBuffer failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	// Set Input Assembler
	GetEffectiveContext()->IASetInputLayout( m_pInputLayout );

	// Set vertex buffer
	UINT stride = sizeof( float ) * 3;
	UINT offset = 0;
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Setup the default viewport

	D3D11_VIEWPORT vp;
	vp.Height = (float)m_viewportHeight;
	vp.Width = (float)m_viewportWidth;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	GetEffectiveContext()->RSSetViewports( 1, &vp );

	
	// Setup the UAV
	UINT ifaces = 1;
	if (m_bUAVOnly)
	{
		GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 
			0, 
			NULL, 
			NULL,
			0, 
			1,
			&m_pUAV, 
			&ifaces);
	}
	else
	{
		// create a dummy rendertarget
		if ( CComputeTest::SetupRTV(m_viewportWidth, m_viewportHeight) != RESULT_PASS )
		{
			WriteToLog(_T("CUAVBufferRWPSTest::ExecuteTestCase: CComputeTest::SetupRTV() failed "));
			return RESULT_FAIL;
		}

		GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 
			1, 
			&m_pRTView, 
			NULL,
			1, 
			1,
			&m_pUAV, 
			&ifaces);
	}

	GetEffectiveContext()->PSSetShaderResources( 0, 1, &m_pSRV );
	GetEffectiveContext()->VSSetShader( m_pVertexShader, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPixelShader, NULL, 0 );

	GetEffectiveContext()->Draw( 6, 0 );
	
	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T( "CUAVBufferRWPSTest::ExecuteTestCase() - Failed to execute deferred context" ) );
		return RESULT_FAIL;
	}

	GetImmediateContext()->CopyResource( m_pUAVResourceStaging, m_pUAVResource );
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = GetImmediateContext()->Map( m_pUAVResourceStaging, 0, D3D11_MAP_READ, 0, &mappedResource );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CUAVBufferRWPSTest::ExecuteTestCase() - Map failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
		
	tr = CompareResults( mappedResource.pData ); 

	GetImmediateContext()->Unmap( m_pUAVResourceStaging, 0 );

	return tr;
}
