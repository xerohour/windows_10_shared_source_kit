#include "d3dx11async.h"

#include "UAV.h"


//----------------------------------------------------------------------------------------
//
// UAVBufferRWCS.cpp - CUAVBufferRWCSTest
//
//
//
// Concrete implementation of CUAVBufferRWTest for testing on Compute Shader.
//

//----------------------------------------------------------------------------------------

void CUAVBufferRWCSTest::InitTestParameters()
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

	// setup thread / threadgroup count

	CUserValueSet<UINT> *pThreadCountValues = new CUserValueSet<UINT>();
	pThreadCountValues->AddValue( 1 );
	pThreadCountValues->AddValue( 8 );

	testfactor::RFactor rfThreadCountX = AddParameter<UINT>( _T( "ThreadCountX" ), &m_threadCountX, pThreadCountValues );
	testfactor::RFactor rfThreadCountY = AddParameter<UINT>( _T( "ThreadCountY" ), &m_threadCountY, pThreadCountValues );
	testfactor::RFactor rfThreadCountZ = AddParameter<UINT>( _T( "ThreadCountZ" ), &m_threadCountZ, pThreadCountValues );

	CUserValueSet<UINT> *pDispatchCountValues = new CUserValueSet<UINT>();
	pDispatchCountValues->AddValue( 1 );
	pDispatchCountValues->AddValue( 3 );

	CUserValueSet<UINT> *pDispatchZCountValues = new CUserValueSet<UINT>();
	pDispatchZCountValues->AddValue( 1 );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ) // not available for 10_*
	{
		pDispatchZCountValues->AddValue( 3 );
	}

	testfactor::RFactor rfDispatchCountX = AddParameter<UINT>( _T( "DispatchCountX" ), &m_dispatchCountX, pDispatchCountValues );
	testfactor::RFactor rfDispatchCountY = AddParameter<UINT>( _T( "DispatchCountY" ), &m_dispatchCountY, pDispatchCountValues );
	testfactor::RFactor rfDispatchCountZ = AddParameter<UINT>( _T( "DispatchCountZ" ), &m_dispatchCountZ, pDispatchZCountValues );
	SetRootTestFactor(
		( ( rfUAVResourceTypeStructured * rfUAVFormatStructured )
			+ ( rfUAVResourceTypeByteAddr * rfUAVFormatByteAddr ) 
			+ ( rfUAVResourceTypeBuffer % rfUAVFormatBuffer ) ) 
		* ( rfThreadCountX % rfThreadCountY % rfThreadCountZ ) 
		* ( rfDispatchCountX % rfDispatchCountY ) * rfDispatchCountZ
	);

	// Adding priority filters
	CFilterHandle filterNumThreads = FilterEqual<UINT>(_T("DispatchCountX"), 1) &&
		FilterEqual<UINT>(_T("DispatchCountZ"), 1) &&
		FilterEqual<UINT>(_T("ThreadCountX"), 8);

	AddPriorityPatternFilter(filterNumThreads, WeightFactorLarge);
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
}

//----------------------------------------------------------------------------------------

void CUAVBufferRWCSTest::CreateInitialData()
{
	m_uByteWidth = m_uThreadByteWidth * m_threadCountX * m_threadCountY * m_threadCountZ
		* m_dispatchCountX * m_dispatchCountY * m_dispatchCountZ;

	CUAVBufferRWTest::CreateInitialData();
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferRWCSTest::CreateShader()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;

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

	tstring threadCountX = ToString( m_threadCountX );
	tstring threadCountY = ToString( m_threadCountY );
	tstring threadCountZ = ToString( m_threadCountZ );
	tstring dispatchCountX = ToString( m_dispatchCountX );
	tstring dispatchCountY = ToString( m_dispatchCountY );
	tstring dispatchCountZ = ToString( m_dispatchCountZ );
	tstring inputBufferSize = ToString( m_uBufferSize );

	D3D10_SHADER_MACRO const macros[] = 
	{
		{ _T( "THREAD_COUNT_X" ), threadCountX.c_str() },
		{ _T( "THREAD_COUNT_Y" ), threadCountY.c_str() },
		{ _T( "THREAD_COUNT_Z" ), threadCountZ.c_str() },
		{ _T( "DISPATCH_COUNT_X" ), dispatchCountX.c_str() },
		{ _T( "DISPATCH_COUNT_Y" ), dispatchCountY.c_str() },
		{ _T( "DISPATCH_COUNT_Z" ), dispatchCountZ.c_str() },
		{ _T( "BUFFER_BYTE_WIDTH_PER_THREAD" ), _T( UAV_BUFFER_RW_TEST_BUFFER_SIZE_STRING ) },
		{ _T( "DEFAULT_BUFFER_SIZE" ), inputBufferSize.c_str() },
		{ _T( "DEFINE_SET" ), _T( "1" ) },
		{ _T( "COMPUTE_SHADER_TEST" ), _T( "1" ) },
		{ resourceType.c_str(), _T( "1" ) },
		NULL
	};

	hr = CComputeTest::CompileAndCreateComputeShaderFromResource( 
		_T("RC_UAV_BUFFER_RW_FX"),  _T("CSMain"), g_TestApp.m_D3DOptions.FeatureLevel, 
		&m_pComputeShader, macros); 
	if ( FAILED(hr) ) 
	{ 
		tr = RESULT_FAIL; 
		WriteToLog( _T( "CUAVBufferRWCSTest::CreateShader: CompileAndCreateComputeShaderFromResource failed. hr = %s."), 
			D3DHResultToString(hr).c_str() ); 
	}

	return tr;
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferRWCSTest::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;

	UINT ifaces = 1;	
	GetEffectiveContext()->CSSetShaderResources( 0, 1, &m_pSRV );
	GetEffectiveContext()->CSSetUnorderedAccessViews( 0, 1, &m_pUAV, &ifaces );
	GetEffectiveContext()->CSSetShader( m_pComputeShader, NULL, 0 );

	GetEffectiveContext()->Dispatch( m_dispatchCountX, m_dispatchCountY, m_dispatchCountZ );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T( "CUAVBufferRWCSTest::ExecuteTestCase() - Failed to execute deferred context" ) );
		return RESULT_FAIL;
	}

	GetImmediateContext()->CopyResource( m_pUAVResourceStaging, m_pUAVResource );
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = GetImmediateContext()->Map( m_pUAVResourceStaging, 0, D3D11_MAP_READ, 0, &mappedResource );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CUAVBufferRWCSTest::ExecuteTestCase() - Map failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
		
	tr = CompareResults( mappedResource.pData ); 

	GetImmediateContext()->Unmap( m_pUAVResourceStaging, 0 );

	return tr;
}

