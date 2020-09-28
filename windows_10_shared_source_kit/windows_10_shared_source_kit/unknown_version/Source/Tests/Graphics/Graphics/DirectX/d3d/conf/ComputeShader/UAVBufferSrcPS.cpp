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
// UAVBufferSrcPS.cpp - CUAVBufferSrcPSTest
//
//
//
// Concrete implementation of CUAVBufferSrcTest for testing on Compute Shader.
// 

//----------------------------------------------------------------------------------------

void CUAVBufferSrcPSTest::InitTestParameters()
{
	// setup source resource type
	CTestCaseParameter<RESOURCE_TYPE> *pSrcResourceTypeParam = AddParameter( _T( "SrcResourceType" ), &m_SrcResourceType );
	CUserValueSet<RESOURCE_TYPE> *pSrcResourceStructuredBufferValueSet = new CUserValueSet<RESOURCE_TYPE>();
	pSrcResourceStructuredBufferValueSet->AddValue( RESOURCE_STRUCTUREDBUFFER_MIX );
	pSrcResourceStructuredBufferValueSet->AddValue( RESOURCE_STRUCTUREDBUFFER_1FLOAT );
	pSrcResourceStructuredBufferValueSet->AddValue( RESOURCE_STRUCTUREDBUFFER_2SINT );
	pSrcResourceStructuredBufferValueSet->AddValue( RESOURCE_STRUCTUREDBUFFER_4UINT );
	testfactor::RFactor rfSrcResourceTypeStructured = AddParameterValueSet( pSrcResourceTypeParam, pSrcResourceStructuredBufferValueSet );

	CUserValueSet<RESOURCE_TYPE> *pSrcResourceByteAddrBufferValueSet = new CUserValueSet<RESOURCE_TYPE>();
	pSrcResourceByteAddrBufferValueSet->AddValue( RESOURCE_BYTEADDRESSBUFFER );
	testfactor::RFactor rfSrcResourceTypeByteAddr = AddParameterValueSet( pSrcResourceTypeParam, pSrcResourceByteAddrBufferValueSet );

	// setup dxgi format
	CTestCaseParameter<DXGI_FORMAT> *pSrcFormatParam = AddParameter( _T( "SrcFormat" ), &m_srcFormat );
	CUserValueSet<DXGI_FORMAT> *pSrcFormaStructuredValueSet = new CUserValueSet<DXGI_FORMAT>();
	pSrcFormaStructuredValueSet->AddValue( DXGI_FORMAT_UNKNOWN );
	testfactor::RFactor rfSrcFormatStructured = AddParameterValueSet( pSrcFormatParam, pSrcFormaStructuredValueSet );

	CUserValueSet<DXGI_FORMAT> *pSrcFormatByteAddrValueSet = new CUserValueSet<DXGI_FORMAT>();
	pSrcFormatByteAddrValueSet->AddValue( DXGI_FORMAT_R32_TYPELESS );
	testfactor::RFactor rfSrcFormatByteAddr = AddParameterValueSet( pSrcFormatParam, pSrcFormatByteAddrValueSet );

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
		( ( rfSrcResourceTypeStructured * rfSrcFormatStructured )
			+ ( rfSrcResourceTypeByteAddr * rfSrcFormatByteAddr ) )
		* ( rfViewportWidth * rfViewportHeight ) % rfIsUAVOnly
	);

	// Adding priority filters
	CFilterHandle filterNumThreads = FilterEqual<UINT>(_T("ViewportWidth"), 24) &&
		FilterEqual<UINT>(_T("ViewportHeight"), 80);
		
	AddPriorityPatternFilter(filterNumThreads, WeightFactorLarge);
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
}

//----------------------------------------------------------------------------------------

void CUAVBufferSrcPSTest::CreateInitialData()
{
	m_uTotalThreadCount = m_viewportWidth * m_viewportHeight;
	CUAVBufferSrcTest::CreateInitialData();
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferSrcPSTest::SetupTestCase()
{
	if ( g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
	{
		return RESULT_SKIP;
	}
	return CUAVBufferTest::SetupTestCase();
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferSrcPSTest::CreateShader()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	ID3D10Blob *pPixelShaderBlob = NULL;
	ID3D10Blob *pPixelErrorBlob = NULL;
	ID3D10Blob *pVertexShaderBlob = NULL;
	ID3D10Blob *pVertexErrorBlob = NULL;

	tstring resourceType;
	tstring expectedPattern;
	
	switch( m_SrcResourceType )
	{
	case RESOURCE_BYTEADDRESSBUFFER:
		resourceType = _T( "SRC_BYTEADDRESSBUFFER" );
		expectedPattern = _T( UAV_BUFFER_SRC_TEST_PATTERN_UINT_STRING );
		break;
	case RESOURCE_STRUCTUREDBUFFER_1FLOAT:
		resourceType = _T( "SRC_STRUCTUREDBUFFER_1FLOAT" );
		expectedPattern = _T( UAV_BUFFER_SRC_TEST_PATTERN_FLOATGROUP_STRING );
		break;
	case RESOURCE_STRUCTUREDBUFFER_4UINT:
		resourceType = _T( "SRC_STRUCTUREDBUFFER_4UINT" );
		expectedPattern = _T( UAV_BUFFER_SRC_TEST_PATTERN_UINTGROUP_STRING );
		break;
	case RESOURCE_STRUCTUREDBUFFER_2SINT:
		resourceType = _T( "SRC_STRUCTUREDBUFFER_2SINT" );
		expectedPattern = _T( UAV_BUFFER_SRC_TEST_PATTERN_SINTGROUP_STRING );
		break;
	case RESOURCE_STRUCTUREDBUFFER_MIX:
		resourceType = _T( "SRC_STRUCTUREDBUFFER_MIX" );
		expectedPattern = _T( UAV_BUFFER_SRC_TEST_PATTERN_MIXGROUP_STRING );
		break;
	}

	tstring viewportWidth = ToString( m_viewportWidth );
	tstring viewportHeight = ToString( m_viewportHeight );
	UINT blankBytes = UAV_BUFFER_SRC_TEST_BLANK_BUFFER_BYTE_COUNT;
	tstring blankBytesStr = ToString( blankBytes );

	tstring isUAVOnly;
	if (m_bUAVOnly)
		isUAVOnly = _T("1");
	else
		isUAVOnly = _T("0");

	D3D10_SHADER_MACRO const macros[] = 
	{
		{ _T( "VIEWPORT_WIDTH" ), viewportWidth.c_str() },
		{ _T( "VIEWPORT_HEIGHT" ), viewportHeight.c_str() },
		{ _T( "BUFFER_BYTE_WIDTH" ), _T( DEFAULT_BUFFER_SIZE_STRING ) },
		{ _T( "BLANK_BYTES" ), blankBytesStr.c_str() },
		{ _T( "DEFINE_SET" ), _T( "1" ) },
		{ _T( "PIXEL_SHADER_TEST" ), _T( "1" ) },
		{ resourceType.c_str(), _T( "1" ) },
		{ _T( "RESULT_PATTERN" ), expectedPattern.c_str() },
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
		WriteToLog( _T( "CUAVBufferSrcPSTest::CreateShader() - Failed to compile vertex shader, hr=%s, Errors=%s" ),
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
		WriteToLog( _T( "CUAVBufferSrcPSTest::CreateShader() - Failed to create vertex shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
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
		WriteToLog( _T( "CUAVBufferSrcPSTest::ExecuteTestCase() - CreateInputLayout() failed" ) );
		tr = RESULT_FAIL;
		goto Done;
	}

	// Create pixel shader

	hr = D3DX11CompileFromResource(
		NULL,
		_T( "RC_UAV_BUFFER_SRC_FX" ),
		_T( "UAVBufferSrc.fx" ),
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
		WriteToLog( _T( "CUAVBufferSrcPSTest::CreateShader() - Failed to compile pixel shader, hr=%s, Errors=%s" ),
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
		WriteToLog( _T( "CUAVBufferSrcPSTest::CreateShader() - Failed to create pixel shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
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

TEST_RESULT CUAVBufferSrcPSTest::ExecuteTestCase()
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
		WriteToLog( _T( "CUAVBufferSrcPSTest::ExecuteTestCase() - CreateBuffer failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	// Set Input Assembler
	GetEffectiveContext()->IASetInputLayout( m_pInputLayout );

	// Set vertex buffer
	UINT stride = sizeof( float ) * 3;
	UINT offset = 0;
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// clear UAV

	UINT clearVals[4] = { 0, 0, 0, 0 };
	GetEffectiveContext()->ClearUnorderedAccessViewUint( m_pUAV, clearVals );

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
			WriteToLog(_T("CUAVBufferSrcPSTest::ExecuteTestCase: CComputeTest::SetupRTV() failed "));
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
	
	// Setup the SRV
	ID3D11ShaderResourceView * srvArray[2];
	srvArray[0] = m_pSRV;
	srvArray[1] = m_pSRVBlank;
	GetEffectiveContext()->PSSetShaderResources( 0, 2, srvArray );
	GetEffectiveContext()->VSSetShader( m_pVertexShader, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPixelShader, NULL, 0 );

	GetEffectiveContext()->Draw( 6, 0 );
	
	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T( "CUAVBufferSrcPSTest::ExecuteTestCase() - Failed to execute deferred context") );
		return RESULT_FAIL;
	}

	GetImmediateContext()->CopyResource( m_pUAVResourceStaging, m_pUAVResource );
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = GetImmediateContext()->Map( m_pUAVResourceStaging, 0, D3D11_MAP_READ, 0, &mappedResource );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CUAVBufferSrcPSTest::ExecuteTestCase() - Map failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
		
	tr = CompareResults( mappedResource.pData ); 

	GetImmediateContext()->Unmap( m_pUAVResourceStaging, 0 );

	return tr;
}
