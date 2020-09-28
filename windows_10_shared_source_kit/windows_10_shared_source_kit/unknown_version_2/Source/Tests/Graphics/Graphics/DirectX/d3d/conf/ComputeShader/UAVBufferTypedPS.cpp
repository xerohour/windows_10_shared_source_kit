#include "d3dx11async.h"
#include "D3DMSHelper.h"

#include "UAV.h"

const char szVSPassthrough[] = 
	"float4 main( float4 input : POSITION ): SV_POSITION \n"
	"{\n"
	"	return input; \n"
	"}\n"
	;

//----------------------------------------------------------------------------------------
//
// UAVBufferTypedPS.cpp - CUAVBufferTypedPSTest
//
//
//
// Concrete implementation of CUAVBufferTypedTest for testing on Pixel Shader.
// 


//----------------------------------------------------------------------------------------

void CUAVBufferTypedPSTest::InitTestParameters()
{
	// setup UAV type
	CTestCaseParameter<RESOURCE_TYPE> *pUAVResourceTypeParam = AddParameter( _T( "UAVResourceType" ), &m_UAVResourceType );

	CUserValueSet<RESOURCE_TYPE> *pUAVResourceTypeUIntValueSet = new CUserValueSet<RESOURCE_TYPE>( );
	pUAVResourceTypeUIntValueSet->AddValue( RESOURCE_RWBUFFER_UINT );
	testfactor::RFactor rfUAVResourceTypeUIntBuffer = AddParameterValueSet( pUAVResourceTypeParam, pUAVResourceTypeUIntValueSet );

	CUserValueSet<RESOURCE_TYPE> *pUAVResourceTypeSIntValueSet = new CUserValueSet<RESOURCE_TYPE>( );
	pUAVResourceTypeSIntValueSet->AddValue( RESOURCE_RWBUFFER_SINT );
	testfactor::RFactor rfUAVResourceTypeSIntBuffer = AddParameterValueSet( pUAVResourceTypeParam, pUAVResourceTypeSIntValueSet );

	CUserValueSet<RESOURCE_TYPE> *pUAVResourceTypeFloatValueSet = new CUserValueSet<RESOURCE_TYPE>( );
	pUAVResourceTypeFloatValueSet->AddValue( RESOURCE_RWBUFFER_FLOAT );
	testfactor::RFactor rfUAVResourceTypeFloatBuffer = AddParameterValueSet( pUAVResourceTypeParam, pUAVResourceTypeFloatValueSet );

	// setup dxgi format
	CTestCaseParameter<DXGI_FORMAT> *pUAVFormatParam = AddParameter( _T( "UAVFormat" ), &m_UAVFormat );

	CUserValueSet<DXGI_FORMAT> *pUAVFormatUIntValueSet = new CUserValueSet<DXGI_FORMAT>( );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R32_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R32G32B32A32_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R16G16B16A16_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R32G32_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R10G10B10A2_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R8G8B8A8_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R16G16_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R8G8_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R16_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R8_UINT );
	testfactor::RFactor rfUAVFormatUInt = AddParameterValueSet( pUAVFormatParam, pUAVFormatUIntValueSet );

	CUserValueSet<DXGI_FORMAT> *pUAVFormatSIntValueSet = new CUserValueSet<DXGI_FORMAT>( );
	pUAVFormatSIntValueSet->AddValue( DXGI_FORMAT_R32G32B32A32_SINT );
	pUAVFormatSIntValueSet->AddValue( DXGI_FORMAT_R16G16B16A16_SINT );
	pUAVFormatSIntValueSet->AddValue( DXGI_FORMAT_R32G32_SINT );
	pUAVFormatSIntValueSet->AddValue( DXGI_FORMAT_R8G8B8A8_SINT );
	pUAVFormatSIntValueSet->AddValue( DXGI_FORMAT_R16G16_SINT );
	pUAVFormatSIntValueSet->AddValue( DXGI_FORMAT_R32_SINT );
	pUAVFormatSIntValueSet->AddValue( DXGI_FORMAT_R8G8_SINT );
	pUAVFormatSIntValueSet->AddValue( DXGI_FORMAT_R16_SINT );
	pUAVFormatSIntValueSet->AddValue( DXGI_FORMAT_R8_SINT );
	testfactor::RFactor rfUAVFormatSInt = AddParameterValueSet( pUAVFormatParam, pUAVFormatSIntValueSet );

	CUserValueSet<DXGI_FORMAT> *pUAVFormatFloatValueSet = new CUserValueSet<DXGI_FORMAT>( );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R32G32B32A32_FLOAT );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R16G16B16A16_FLOAT );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R16G16B16A16_UNORM );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R16G16B16A16_SNORM );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R32G32_FLOAT );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R11G11B10_FLOAT );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R10G10B10A2_UNORM );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R8G8B8A8_UNORM );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R8G8B8A8_SNORM );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R16G16_UNORM );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R16G16_FLOAT );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R16G16_SNORM );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R32_FLOAT );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R8G8_UNORM );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R8G8_SNORM );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R16_FLOAT );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R16_UNORM );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R16_SNORM );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R8_UNORM );
	pUAVFormatFloatValueSet->AddValue( DXGI_FORMAT_R8_SNORM );
	testfactor::RFactor rfUAVFormatFloat = AddParameterValueSet( pUAVFormatParam, pUAVFormatFloatValueSet );

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
		( ( rfUAVResourceTypeUIntBuffer * rfUAVFormatUInt ) + ( rfUAVResourceTypeSIntBuffer * rfUAVFormatSInt ) + ( rfUAVResourceTypeFloatBuffer * rfUAVFormatFloat ) ) 
		* (rfViewportWidth * rfViewportHeight) % rfIsUAVOnly
	);
}

//----------------------------------------------------------------------------------------

void CUAVBufferTypedPSTest::CreateInitialData()
{
	m_uThreadByteWidth = GetByteSizePerColor( m_UAVFormat ) * UAV_BUFFER_TYPED_TEST_NUMBER_OF_ELEMENT_PER_THREAD; // set XX elements per thread
	m_uByteWidth = m_uThreadByteWidth * m_viewportWidth * m_viewportHeight;

	m_SrcResourceType = RESOURCE_BYTEADDRESSBUFFER; // shader resource not needed - but don't want base class to blow up
	m_srcFormat = DXGI_FORMAT_R32_FLOAT;

}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferTypedPSTest::CreateShader()
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
	case RESOURCE_RWBUFFER_SINT:
		resourceType = _T( "UAV_RWBUFFER_SINT" );
		break;
	case RESOURCE_RWBUFFER_UINT:
		resourceType = _T( "UAV_RWBUFFER_UINT" );
		break;
	case RESOURCE_RWBUFFER_FLOAT:
		resourceType = _T( "UAV_RWBUFFER_FLOAT" );
		break;
	}

	tstring expectedResult1 = ToString( GetExpectedValue1( m_UAVFormat ) );
	tstring expectedResult2 = ToString( GetExpectedValue2( m_UAVFormat ) );

	tstring viewportWidth = ToString( m_viewportWidth );
	tstring viewportHeight = ToString( m_viewportHeight );
	tstring threadByteWidth = ToString( m_uThreadByteWidth );

	tstring isUAVOnly;
	if (m_bUAVOnly)
		isUAVOnly = _T("1");
	else
		isUAVOnly = _T("0");

	D3D10_SHADER_MACRO const macros[] = 
	{
		{ _T( "VIEWPORT_WIDTH" ), viewportWidth.c_str() },
		{ _T( "VIEWPORT_HEIGHT" ), viewportHeight.c_str() },
		{ _T( "EXPECTED_RESULT_1" ), expectedResult1.c_str() },
		{ _T( "EXPECTED_RESULT_2" ), expectedResult2.c_str() },
		{ _T( "BUFFER_BYTE_WIDTH_PER_THREAD" ), threadByteWidth.c_str() },
		{ _T( "NUMBER_OF_ELEMENT_PER_THREAD" ), _T( UAV_BUFFER_TYPED_TEST_NUMBER_OF_ELEMENT_PER_THREAD_STRING ) },
		{ _T( "DEFINE_SET" ), _T( "1" ) },
		{ _T( "PIXEL_SHADER_TEST" ), _T( "1" ) },
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
		WriteToLog( _T( "CUAVBufferTypedPSTest::CreateShader() - Failed to compile vertex shader, hr=%s, Errors=%s" ),
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
		WriteToLog( _T( "CUAVBufferTypedPSTest::CreateShader() - Failed to create vertex shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
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
		WriteToLog( _T( "CUAVBufferTypedPSTest::ExecuteTestCase() - CreateInputLayout() failed" ) );
		tr = RESULT_FAIL;
		goto Done;
	}

	// Create pixel shader

	hr = D3DX11CompileFromResource(
		NULL,
		_T( "RC_UAV_BUFFER_TYPED_FX" ),
		_T( "UAVBufferTyped.fx" ),
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
		WriteToLog( _T( "CUAVBufferTypedPSTest::CreateShader() - Failed to compile pixel shader, hr=%s, Errors=%s" ),
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
		WriteToLog( _T( "CUAVBufferTypedPSTest::CreateShader() - Failed to create pixel shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
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

TEST_RESULT CUAVBufferTypedPSTest::ExecuteTestCase()
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
			WriteToLog(_T("CUAVBufferTypedPSTest::ExecuteTestCase: CComputeTest::SetupRTV() failed "));
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

	GetEffectiveContext()->VSSetShader( m_pVertexShader, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPixelShader, NULL, 0 );

	GetEffectiveContext()->Draw( 6, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T( "CUAVBufferTypedPSTest::ExecuteTestCase() - Failed to execute deferred context" ) );
		return RESULT_FAIL;
	}
	
	GetImmediateContext()->CopyResource( m_pUAVResourceStaging, m_pUAVResource );
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = GetImmediateContext()->Map( m_pUAVResourceStaging, 0, D3D11_MAP_READ, 0, &mappedResource );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CUAVBufferTypedPSTest::ExecuteTestCase() - Map failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
		
	tr = CompareResults( mappedResource.pData ); 

	GetImmediateContext()->Unmap( m_pUAVResourceStaging, 0 );

	return tr;
}


