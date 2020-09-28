#include "d3dx11async.h"
#include "D3DMSHelper.h"

#include "UAV.h"

//----------------------------------------------------------------------------------------
//
// UAVBufferTypedCS.cpp - CUAVBufferTypedCSTest
//
//
//
// Concrete implementation of CUAVBufferTypedTest for testing on Compute Shader.
// 


//----------------------------------------------------------------------------------------

void CUAVBufferTypedCSTest::InitTestParameters()
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
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R32G32B32A32_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R16G16B16A16_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R32G32_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R10G10B10A2_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R8G8B8A8_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R16G16_UINT );
	pUAVFormatUIntValueSet->AddValue( DXGI_FORMAT_R32_UINT );
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

	// setup thread / thread group count 
	CUserValueSet<UINT> *pThreadCountValues = new CUserValueSet<UINT>();
	pThreadCountValues->AddValue(1);
	pThreadCountValues->AddValue(8);

	testfactor::RFactor rfThreadCountX = AddParameter<UINT>( _T( "ThreadCountX" ), &m_threadCountX, pThreadCountValues );
	testfactor::RFactor rfThreadCountY = AddParameter<UINT>( _T( "ThreadCountY" ), &m_threadCountY, pThreadCountValues );
	testfactor::RFactor rfThreadCountZ = AddParameter<UINT>( _T( "ThreadCountZ" ), &m_threadCountZ, pThreadCountValues );

	CUserValueSet<UINT> *pDispatchCountValues = new CUserValueSet<UINT>();
	pDispatchCountValues->AddValue( 1 );
	pDispatchCountValues->AddValue( 3 );

	testfactor::RFactor rfDispatchCountX = AddParameter<UINT>( _T( "DispatchCountX" ), &m_dispatchCountX, pDispatchCountValues );
	testfactor::RFactor rfDispatchCountY = AddParameter<UINT>( _T( "DispatchCountY" ), &m_dispatchCountY, pDispatchCountValues );
	testfactor::RFactor rfDispatchCountZ = AddParameter<UINT>( _T( "DispatchCountZ" ), &m_dispatchCountZ, pDispatchCountValues );
	SetRootTestFactor(
		( ( rfUAVResourceTypeUIntBuffer * rfUAVFormatUInt ) + ( rfUAVResourceTypeSIntBuffer * rfUAVFormatSInt ) + ( rfUAVResourceTypeFloatBuffer * rfUAVFormatFloat ) ) * ( rfThreadCountX % rfThreadCountY % rfThreadCountZ ) * ( rfDispatchCountX % rfDispatchCountY ) * rfDispatchCountZ
	);

		// Adding priority filters
	CFilterHandle filterNumThreads = FilterEqual<UINT>(_T("DispatchCountX"), 1) &&
		FilterEqual<UINT>(_T("DispatchCountZ"), 1) &&
		FilterEqual<UINT>(_T("ThreadCountX"), 8);

	AddPriorityPatternFilter(filterNumThreads, WeightFactorLarge);
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
}

//----------------------------------------------------------------------------------------

void CUAVBufferTypedCSTest::CreateInitialData()
{
	m_uThreadByteWidth = GetByteSizePerColor( m_UAVFormat ) * UAV_BUFFER_TYPED_TEST_NUMBER_OF_ELEMENT_PER_THREAD; // set XX elements per thread
	m_uByteWidth = m_uThreadByteWidth * m_threadCountX * m_threadCountY * m_threadCountZ * m_dispatchCountX * m_dispatchCountY * m_dispatchCountZ;

	m_SrcResourceType = RESOURCE_BYTEADDRESSBUFFER; // shader resource not needed - but don't want base class to blow up
	m_srcFormat = DXGI_FORMAT_R32_FLOAT;

}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferTypedCSTest::CreateShader()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	ID3D10Blob *pShaderBlob = NULL;
	ID3D10Blob *pErrorBlob = NULL;

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

	tstring threadCountX = ToString( m_threadCountX );
	tstring threadCountY = ToString( m_threadCountY );
	tstring threadCountZ = ToString( m_threadCountZ );
	tstring dispatchCountX = ToString( m_dispatchCountX );
	tstring dispatchCountY = ToString( m_dispatchCountY );
	tstring dispatchCountZ = ToString( m_dispatchCountZ );
	tstring threadByteWidth = ToString( m_uThreadByteWidth );

	D3D10_SHADER_MACRO const macros[] = 
	{
		{ _T( "THREAD_COUNT_X" ), threadCountX.c_str() },
		{ _T( "THREAD_COUNT_Y" ), threadCountY.c_str() },
		{ _T( "THREAD_COUNT_Z" ), threadCountZ.c_str() },
		{ _T( "DISPATCH_COUNT_X" ), dispatchCountX.c_str() },
		{ _T( "DISPATCH_COUNT_Y" ), dispatchCountY.c_str() },
		{ _T( "DISPATCH_COUNT_Z" ), dispatchCountZ.c_str() },
		{ _T( "EXPECTED_RESULT_1" ), expectedResult1.c_str() },
		{ _T( "EXPECTED_RESULT_2" ), expectedResult2.c_str() },
		{ _T( "BUFFER_BYTE_WIDTH_PER_THREAD" ), threadByteWidth.c_str() },
		{ _T( "NUMBER_OF_ELEMENT_PER_THREAD" ), _T( UAV_BUFFER_TYPED_TEST_NUMBER_OF_ELEMENT_PER_THREAD_STRING ) },
		{ _T( "COMPUTE_SHADER_TEST" ), _T( "1" ) },
		{ _T( "DEFINE_SET" ), _T( "1" ) },
		{ resourceType.c_str(), _T( "1" ) },
		NULL
	};

	hr = D3DX11CompileFromResource(
		NULL,
		_T( "RC_UAV_BUFFER_TYPED_FX" ),
		_T( "UAVBufferTyped.fx" ),
		macros,
		NULL,
		_T( "CSMain" ),
		_T("cs_5_0"),
		D3D10_SHADER_DEBUG, 
		D3D10_SHADER_DEBUG, 
		NULL,
		&pShaderBlob,
		&pErrorBlob,
		NULL
	);

	if( FAILED( hr ) || pShaderBlob == NULL )
	{
		WriteToLog( _T( "CUAVBufferTypedCSTest::CreateShader() - Failed to compile shader, hr=%s, Errors=%s" ),
			D3DHResultToString( hr ).c_str(), pErrorBlob ? pErrorBlob->GetBufferPointer() : "" );
		tr = RESULT_FAIL;
		goto Done;
	}

	hr = GetDevice()->CreateComputeShader(
		pShaderBlob->GetBufferPointer(), 
		pShaderBlob->GetBufferSize(),
		NULL,
		&m_pComputeShader
	);

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CUAVBufferTypedCSTest::CreateShader() - Failed to create shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
	}
	
Done:
	SAFE_RELEASE( pShaderBlob );
	SAFE_RELEASE( pErrorBlob );

	return tr;
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferTypedCSTest::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;

	UINT clearVals[4] = { 0, 0, 0, 0 };
	GetEffectiveContext()->ClearUnorderedAccessViewUint( m_pUAV, clearVals );

	UINT ifaces = 1;

	GetEffectiveContext()->CSSetShaderResources( 0, 1, &m_pSRV );
	GetEffectiveContext()->CSSetUnorderedAccessViews( 0, 1, &m_pUAV, &ifaces );
	GetEffectiveContext()->CSSetShader( m_pComputeShader, NULL, 0 );

	GetEffectiveContext()->Dispatch( m_dispatchCountX, m_dispatchCountY, m_dispatchCountZ );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T( "CUAVBufferTypedCSTest::ExecuteTestCase() - Failed to execute deferred context" ) );
		return RESULT_FAIL;
	}

	GetImmediateContext()->CopyResource( m_pUAVResourceStaging, m_pUAVResource );
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = GetImmediateContext()->Map( m_pUAVResourceStaging, 0, D3D11_MAP_READ, 0, &mappedResource );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CUAVBufferTypedCSTest::ExecuteTestCase() - Map failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
		
	tr = CompareResults( mappedResource.pData ); 

	GetImmediateContext()->Unmap( m_pUAVResourceStaging, 0 );

	return tr;
}

