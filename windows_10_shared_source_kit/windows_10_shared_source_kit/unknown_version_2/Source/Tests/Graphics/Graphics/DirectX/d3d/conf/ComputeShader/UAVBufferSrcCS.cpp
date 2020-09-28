#include "d3dx11async.h"

#include "UAV.h"


//----------------------------------------------------------------------------------------
//
// UAVBufferSrcCS.cpp - CUAVBufferSrcCSTest
//
//
//
// Concrete implementation of CUAVBufferSrcTest for testing on Compute Shader.
// 

//----------------------------------------------------------------------------------------

void CUAVBufferSrcCSTest::InitTestParameters()
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

	// setup thread / thread group counts
	CUserValueSet<UINT> *pThreadCountValues = new CUserValueSet<UINT>();
	pThreadCountValues->AddValue( 1 );
	pThreadCountValues->AddValue( 2 );
	pThreadCountValues->AddValue( 8 );

	testfactor::RFactor rfThreadCountX = AddParameter<UINT>( _T( "ThreadCountX" ), &m_threadCountX, pThreadCountValues );
	testfactor::RFactor rfThreadCountY = AddParameter<UINT>( _T( "ThreadCountY" ), &m_threadCountY, pThreadCountValues );
	testfactor::RFactor rfThreadCountZ = AddParameter<UINT>( _T( "ThreadCountZ" ), &m_threadCountZ, pThreadCountValues );
	
	CUserValueSet<UINT> *pDispatchCountValues = new CUserValueSet<UINT>();
	pDispatchCountValues->AddValue(1);
	pDispatchCountValues->AddValue(3);

	CUserValueSet<UINT> *pDispatchZCountValues = new CUserValueSet<UINT>();
	pDispatchZCountValues->AddValue( 1 );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ) // not available for 10_1
	{
		pDispatchZCountValues->AddValue( 3 );
	}

	testfactor::RFactor rfDispatchCountX = AddParameter<UINT>( _T( "DispatchCountX" ), &m_dispatchCountX, pDispatchCountValues );
	testfactor::RFactor rfDispatchCountY = AddParameter<UINT>( _T( "DispatchCountY" ), &m_dispatchCountY, pDispatchCountValues );
	testfactor::RFactor rfDispatchCountZ = AddParameter<UINT>( _T( "DispatchCountZ" ), &m_dispatchCountZ, pDispatchZCountValues );

	SetRootTestFactor(
		( ( rfSrcResourceTypeStructured * rfSrcFormatStructured )
			+ ( rfSrcResourceTypeByteAddr * rfSrcFormatByteAddr ) )
		* rfThreadCountX * ( rfThreadCountY % rfThreadCountZ )
		* ( rfDispatchCountX % rfDispatchCountY ) * rfDispatchCountZ
	);

	// Adding priority filters
	CFilterHandle filterNumThreads = FilterEqual<UINT>(_T("DispatchCountX"), 1) &&
		FilterEqual<UINT>(_T("DispatchCountZ"), 1) &&
		FilterEqual<UINT>(_T("ThreadCountX"), 8) &&
		FilterEqual<UINT>(_T("ThreadCountY"), 2) ;

	AddPriorityPatternFilter(filterNumThreads, WeightFactorLarge);
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
}

//----------------------------------------------------------------------------------------

void CUAVBufferSrcCSTest::CreateInitialData()
{
	m_uTotalThreadCount = m_threadCountX * m_threadCountY * m_threadCountZ 
		* m_dispatchCountX * m_dispatchCountY * m_dispatchCountZ;
	CUAVBufferSrcTest::CreateInitialData();
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferSrcCSTest::CreateShader()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	ID3D10Blob *pShaderBlob = NULL;
	ID3D10Blob *pErrorBlob = NULL;

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

	tstring threadCountX = ToString( m_threadCountX );
	tstring threadCountY = ToString( m_threadCountY );
	tstring threadCountZ = ToString( m_threadCountZ );
	tstring dispatchCountX = ToString( m_dispatchCountX );
	tstring dispatchCountY = ToString( m_dispatchCountY );
	tstring dispatchCountZ = ToString( m_dispatchCountZ );
	UINT blankBytes = UAV_BUFFER_SRC_TEST_BLANK_BUFFER_BYTE_COUNT;
	tstring blankBytesStr = ToString( blankBytes );

	D3D10_SHADER_MACRO const macros[] = 
	{
		{ _T( "THREAD_COUNT_X" ), threadCountX.c_str() },
		{ _T( "THREAD_COUNT_Y" ), threadCountY.c_str() },
		{ _T( "THREAD_COUNT_Z" ), threadCountZ.c_str() },
		{ _T( "DISPATCH_COUNT_X" ), dispatchCountX.c_str() },
		{ _T( "DISPATCH_COUNT_Y" ), dispatchCountY.c_str() },
		{ _T( "DISPATCH_COUNT_Z" ), dispatchCountZ.c_str() },
		{ _T( "BUFFER_BYTE_WIDTH" ), _T( DEFAULT_BUFFER_SIZE_STRING ) },
		{ _T( "BLANK_BYTES" ), blankBytesStr.c_str() },
		{ _T( "DEFINE_SET" ), _T( "1" ) },
		{ _T( "COMPUTE_SHADER_TEST" ), _T( "1" ) },
		{ resourceType.c_str(), _T( "1" ) },
		{ _T( "RESULT_PATTERN" ), expectedPattern.c_str() },
		NULL
	};

	tstring profileStr;
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		profileStr = _T("cs_5_0");
	}
	else if ( g_TestApp.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_1 )
	{
		profileStr = _T("cs_4_1");
	}
	else
	{	
		profileStr = _T("cs_4_0");
	}

	hr = D3DX11CompileFromResource(
		NULL,
		_T( "RC_UAV_BUFFER_SRC_FX" ),
		_T( "UAVBufferSrc.fx" ),
		macros,
		NULL,
		_T( "CSMain" ),
		profileStr.c_str(),
		D3D10_SHADER_DEBUG, 
		D3D10_SHADER_DEBUG, 
		NULL,
		&pShaderBlob,
		&pErrorBlob,
		NULL
	);

	if( FAILED( hr ) || pShaderBlob == NULL )
	{
		WriteToLog( _T( "CUAVBufferSrcCSTest::CreateShader() - Failed to compile shader, hr=%s, Errors=%s" ),
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
		WriteToLog( _T( "CUAVBufferSrcCSTest::CreateShader() - Failed to create shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
	}
	
Done:
	SAFE_RELEASE( pShaderBlob );
	SAFE_RELEASE( pErrorBlob );	

	return tr;
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferSrcCSTest::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;

	// clear target UAVs
	UINT clearVals[4] = { 0, 0, 0, 0 };
	GetEffectiveContext()->ClearUnorderedAccessViewUint( m_pUAV, clearVals );

	ID3D11ShaderResourceView * srvArray[2];
	srvArray[0] = m_pSRV;
	srvArray[1] = m_pSRVBlank;
	GetEffectiveContext()->CSSetShaderResources( 0, 2, srvArray );

	UINT ifaces = 1; 
	GetEffectiveContext()->CSSetUnorderedAccessViews( 0, 1, &m_pUAV, &ifaces );

	GetEffectiveContext()->CSSetShader( m_pComputeShader, NULL, 0 );

	GetEffectiveContext()->Dispatch( m_dispatchCountX, m_dispatchCountY, m_dispatchCountZ );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T( "CUAVBufferSrcCSTest::ExecuteTestCase() - Failed to execute deferred context" ) );
		return RESULT_FAIL;
	}

	GetImmediateContext()->CopyResource( m_pUAVResourceStaging, m_pUAVResource );
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = GetImmediateContext()->Map( m_pUAVResourceStaging, 0, D3D11_MAP_READ, 0, &mappedResource );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CUAVBufferSrcCSTest::ExecuteTestCase() - Map failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
		
	tr = CompareResults( mappedResource.pData ); 

	GetImmediateContext()->Unmap( m_pUAVResourceStaging, 0 );

	return tr;
}

