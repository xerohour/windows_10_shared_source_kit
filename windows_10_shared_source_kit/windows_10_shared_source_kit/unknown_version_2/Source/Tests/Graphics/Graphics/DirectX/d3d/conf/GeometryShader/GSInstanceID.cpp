// ***************************************************************
//  GSInstanceID   version:  1.0   ·  date: 03/05/2009
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

// Includes
#include "GSInstanceID.h"

// Set the number of instances to run per primitive
D3D10_SHADER_MACRO g_macros[] =
{
	{ "NUM_INSTANCES_PER_PRIMITIVE", "1" },
	NULL
};

struct VS_IN
{
	UINT data[4];
};

static const VS_IN g_inputDataTriList[] = 
{
	{ 0x1000, 0x1001, 0x1002, 0x1003 },
	{ 0x2000, 0x2001, 0x2002, 0x2003 },
	{ 0x3000, 0x3001, 0x3002, 0x3003 },
									 
	{ 0x1100, 0x1101, 0x1102, 0x1103 },
	{ 0x2100, 0x2101, 0x2102, 0x2103 },
	{ 0x3100, 0x3101, 0x3102, 0x3103 },
									 
	{ 0x1200, 0x1201, 0x1202, 0x1203 },
	{ 0x2200, 0x2201, 0x2202, 0x2203 },
	{ 0x3200, 0x3201, 0x3202, 0x3203 },
									 
	{ 0x1300, 0x1301, 0x1302, 0x1303 },
	{ 0x2300, 0x2301, 0x2302, 0x2303 },
	{ 0x3300, 0x3301, 0x3302, 0x3303 },
									 
	{ 0x1400, 0x1401, 0x1402, 0x1403 },
	{ 0x2400, 0x2401, 0x2402, 0x2403 },
	{ 0x3400, 0x3401, 0x3402, 0x3403 },
									 
	{ 0x1500, 0x1501, 0x1502, 0x1503 },
	{ 0x2500, 0x2501, 0x2502, 0x2503 },
	{ 0x3500, 0x3501, 0x3502, 0x3503 },
									 
	{ 0x1600, 0x1601, 0x1602, 0x1603 },
	{ 0x2600, 0x2601, 0x2602, 0x2603 },
	{ 0x3600, 0x3601, 0x3602, 0x3603 },
									 
	{ 0x1700, 0x1701, 0x1702, 0x1703 },
	{ 0x2700, 0x2701, 0x2702, 0x2703 },
	{ 0x3700, 0x3701, 0x3702, 0x3703 },
									 
	{ 0x1800, 0x1801, 0x1802, 0x1803 },
	{ 0x2800, 0x2801, 0x2802, 0x2803 },
	{ 0x3800, 0x3801, 0x3802, 0x3803 },
									 
	{ 0x1900, 0x1901, 0x1902, 0x1903 },
	{ 0x2900, 0x2901, 0x2902, 0x2903 },
	{ 0x3900, 0x3901, 0x3902, 0x3903 },
								 	
	{ 0x1a00, 0x1a01, 0x1a02, 0x1a03 },
	{ 0x2a00, 0x2a01, 0x2a02, 0x2a03 },
	{ 0x3a00, 0x3a01, 0x3a02, 0x3a03 },
								 	
	{ 0x1b00, 0x1b01, 0x1b02, 0x1b03 },
	{ 0x2b00, 0x2b01, 0x2b02, 0x2b03 },
	{ 0x3b00, 0x3b01, 0x3b02, 0x3b03 },
								 	
	{ 0x1c00, 0x1c01, 0x1c02, 0x1c03 },
	{ 0x2c00, 0x2c01, 0x2c02, 0x2c03 },
	{ 0x3c00, 0x3c01, 0x3c02, 0x3c03 },
								 	
	{ 0x1d00, 0x1d01, 0x1d02, 0x1d03 },
	{ 0x2d00, 0x2d01, 0x2d02, 0x2d03 },
	{ 0x3d00, 0x3d01, 0x3d02, 0x3d03 },
								 	
	{ 0x1e00, 0x1e01, 0x1e02, 0x1e03 },
	{ 0x2e00, 0x2e01, 0x2e02, 0x2e03 },
	{ 0x3e00, 0x3e01, 0x3e02, 0x3e03 },
								 	
	{ 0x1f00, 0x1f01, 0x1f02, 0x1f03 },
	{ 0x2f00, 0x2f01, 0x2f02, 0x2f03 },
	{ 0x3f00, 0x3f01, 0x3f02, 0x3f03 },
};

static const VS_IN g_inputDataLineList[] = 
{
	{ 0x1000, 0x1001, 0x1002, 0x1003 },
	{ 0x2000, 0x2001, 0x2002, 0x2003 },
									 
	{ 0x1100, 0x1101, 0x1102, 0x1103 },
	{ 0x2100, 0x2101, 0x2102, 0x2103 },
									 
	{ 0x1200, 0x1201, 0x1202, 0x1203 },
	{ 0x2200, 0x2201, 0x2202, 0x2203 },
									 
	{ 0x1300, 0x1301, 0x1302, 0x1303 },
	{ 0x2300, 0x2301, 0x2302, 0x2303 },
									 
	{ 0x1400, 0x1401, 0x1402, 0x1403 },
	{ 0x2400, 0x2401, 0x2402, 0x2403 },
									 
	{ 0x1500, 0x1501, 0x1502, 0x1503 },
	{ 0x2500, 0x2501, 0x2502, 0x2503 },
									 
	{ 0x1600, 0x1601, 0x1602, 0x1603 },
	{ 0x2600, 0x2601, 0x2602, 0x2603 },
									 
	{ 0x1700, 0x1701, 0x1702, 0x1703 },
	{ 0x2700, 0x2701, 0x2702, 0x2703 },
									 
	{ 0x1800, 0x1801, 0x1802, 0x1803 },
	{ 0x2800, 0x2801, 0x2802, 0x2803 },
									 
	{ 0x1900, 0x1901, 0x1902, 0x1903 },
	{ 0x2900, 0x2901, 0x2902, 0x2903 },
								 	
	{ 0x1a00, 0x1a01, 0x1a02, 0x1a03 },
	{ 0x2a00, 0x2a01, 0x2a02, 0x2a03 },
								 	
	{ 0x1b00, 0x1b01, 0x1b02, 0x1b03 },
	{ 0x2b00, 0x2b01, 0x2b02, 0x2b03 },
								 	
	{ 0x1c00, 0x1c01, 0x1c02, 0x1c03 },
	{ 0x2c00, 0x2c01, 0x2c02, 0x2c03 },
								 	
	{ 0x1d00, 0x1d01, 0x1d02, 0x1d03 },
	{ 0x2d00, 0x2d01, 0x2d02, 0x2d03 },
								 	
	{ 0x1e00, 0x1e01, 0x1e02, 0x1e03 },
	{ 0x2e00, 0x2e01, 0x2e02, 0x2e03 },
								 	
	{ 0x1f00, 0x1f01, 0x1f02, 0x1f03 },
	{ 0x2f00, 0x2f01, 0x2f02, 0x2f03 },
};

static const VS_IN g_inputDataPointList[] = 
{
	{ 0x1000, 0x1001, 0x1002, 0x1003 },
	{ 0x1100, 0x1101, 0x1102, 0x1103 },
	{ 0x1200, 0x1201, 0x1202, 0x1203 },
	{ 0x1300, 0x1301, 0x1302, 0x1303 },
	{ 0x1400, 0x1401, 0x1402, 0x1403 },
	{ 0x1500, 0x1501, 0x1502, 0x1503 },
	{ 0x1600, 0x1601, 0x1602, 0x1603 },
	{ 0x1700, 0x1701, 0x1702, 0x1703 },
	{ 0x1800, 0x1801, 0x1802, 0x1803 },
	{ 0x1900, 0x1901, 0x1902, 0x1903 },
	{ 0x1a00, 0x1a01, 0x1a02, 0x1a03 },
	{ 0x1b00, 0x1b01, 0x1b02, 0x1b03 },
	{ 0x1c00, 0x1c01, 0x1c02, 0x1c03 },
	{ 0x1d00, 0x1d01, 0x1d02, 0x1d03 },
	{ 0x1e00, 0x1e01, 0x1e02, 0x1e03 },
	{ 0x1f00, 0x1f01, 0x1f02, 0x1f03 },
};

////////////////////////

CGSInstanceIDTest::CGSInstanceIDTest(void) :
		 m_pStreamOutputBuffer(NULL)
		,m_pStreamOutputBufferCopy(NULL)
{
}

CGSInstanceIDTest::~CGSInstanceIDTest(void)
{
}

////////////////////////

void CGSInstanceIDTest::InitTestParameters()
{
	CUserValueSet<UINT> *pNumInputPrimitivesValueSet = new CUserValueSet<UINT>();
	pNumInputPrimitivesValueSet->AddValue( 1 );
	pNumInputPrimitivesValueSet->AddValue( 9 );
	pNumInputPrimitivesValueSet->AddValue( 16 );
	
	CUserValueSet< UINT > *pNumGSInstanceCountValueSet = new CUserValueSet< UINT >();
	pNumGSInstanceCountValueSet->AddValue(3);
	pNumGSInstanceCountValueSet->AddValue(4);
	pNumGSInstanceCountValueSet->AddValue(17);
	pNumGSInstanceCountValueSet->AddValue(32);
	pNumGSInstanceCountValueSet->AddValue(1);
	
	CUserValueSet<GS_INPUT_PRIM_TYPE> *pInputPrimTypeValueSet = new CUserValueSet<GS_INPUT_PRIM_TYPE>();
	pInputPrimTypeValueSet->AddValue( PRIM_TYPE_POINT );
	pInputPrimTypeValueSet->AddValue( PRIM_TYPE_LINE );
	pInputPrimTypeValueSet->AddValue( PRIM_TYPE_TRIANGLE );
	
	CTestCaseParameter<UINT> *pNumInputPrimitivesParam = AddParameter( _T( "NumInputPrimitives" ), &m_numInputPrimitives );
	CTestCaseParameter<UINT> *pNumInstancesParam = AddParameter( _T( "NumGSInstancesPerPrimitive" ), &m_numInstances );
	CTestCaseParameter<GS_INPUT_PRIM_TYPE> *pInputPrimTypeParam = AddParameter( _T( "InputPrimType" ), &m_inputPrimType );
	
	testfactor::RFactor numInputPrimitives = AddParameterValueSet( pNumInputPrimitivesParam, pNumInputPrimitivesValueSet );
	testfactor::RFactor numInstances = AddParameterValueSet( pNumInstancesParam, pNumGSInstanceCountValueSet );
	testfactor::RFactor inputPrimType = AddParameterValueSet( pInputPrimTypeParam, pInputPrimTypeValueSet );
	
	SetRootTestFactor( numInputPrimitives * numInstances * inputPrimType );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T( "NumInputPrimitives" ), 1), 0.5f);

	AddPriorityPatternFilter(
		FilterNotEqual<UINT>(_T( "NumGSInstancesPerPrimitive" ), 1 ) && 
		FilterNotEqual<UINT>(_T( "NumGSInstancesPerPrimitive" ), 17 ) && 
		FilterNotEqual<UINT>(_T( "NumGSInstancesPerPrimitive" ), 32 ), 
		0.5f);

	AddPriorityPatternFilter(FilterEqual<UINT>(_T( "NumGSInstancesPerPrimitive" ), 1 ), 0.5f);

	AddPriorityPatternFilter(
		FilterNotEqual<GS_INPUT_PRIM_TYPE>(_T( "InputPrimType" ), PRIM_TYPE_TRIANGLE) &&
		FilterNotEqual<GS_INPUT_PRIM_TYPE>(_T( "InputPrimType" ), PRIM_TYPE_POINT), 
		0.5f);

	DeclarePriorityLevelByPriorityValue( 1, 1.0f );
}

/////

TEST_RESULT CGSInstanceIDTest::Setup()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;

	//
	// Setup pass through vertex shader
	//
	D3D11_INPUT_ELEMENT_DESC iedesc[] = 
	{
		{ "data", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	tr = SetupVSFromResource( "InstanceID.sh", "VSPassThrough", iedesc, 1, &m_pVS, &m_pInputLayout, g_macros );
	if( tr != RESULT_PASS )
		return tr;

	// Set the pipeline
	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );

	return RESULT_PASS;
}

////////////////////////

TEST_RESULT CGSInstanceIDTest::SetupTestCase()
{
	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;

	//
	// Create the stream output buffer
	//
	D3D11_BUFFER_DESC bufferDesc =
	{
		sizeof(UINT) * m_numInputPrimitives * m_numInstances,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_STREAM_OUTPUT,
		0,
		0
	};
	
	if( FAILED( GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pStreamOutputBuffer ) ) )
	{
		WriteToLog( _T("CGSInstanceIDTest::Setup() Stream Output Buffer failed.\n") );
		tr = RESULT_FAIL;
		goto exit;
	}

	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufferDesc.Usage = D3D11_USAGE_STAGING;
	bufferDesc.BindFlags = 0;
	bufferDesc.MiscFlags = 0;
	
	if( FAILED( GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pStreamOutputBufferCopy ) ) )
	{
		WriteToLog( _T("CGSInstanceIDTest::Setup() Stream Output Buffer copy failed.\n") );
		tr = RESULT_FAIL;
		goto exit;
	}

	// Set the stream output buffer
	ID3D11Buffer *buffers[4] = { m_pStreamOutputBuffer, NULL, NULL, NULL };

	UINT offsets[] = { 0, 0, 0, 0 };
	GetEffectiveContext()->SOSetTargets( 1, buffers, offsets );

	//
	// Setup vertex input
	//
	D3D11_BUFFER_DESC bufDesc;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA srData;
	srData.pSysMem = NULL;
	srData.SysMemPitch = 0;
	srData.SysMemSlicePitch = 0;
	switch( m_inputPrimType )
	{
		case PRIM_TYPE_TRIANGLE:
			bufDesc.ByteWidth = m_numInputPrimitives * 3 * sizeof( VS_IN );
			srData.pSysMem = g_inputDataTriList;
			break;
			
		case PRIM_TYPE_LINE:
			bufDesc.ByteWidth = m_numInputPrimitives * 2 * sizeof( VS_IN );
			srData.pSysMem = g_inputDataLineList;
			break;
		
		case PRIM_TYPE_POINT:
			bufDesc.ByteWidth = m_numInputPrimitives * sizeof( VS_IN );
			srData.pSysMem = g_inputDataPointList;
			break;
	}
	
	GetDevice()->CreateBuffer( &bufDesc, &srData, &m_pVB );
	
	//
	// Setup geometry shader and input layout
	//
	
	LPCSTR gsEntry = NULL;
	
	switch( m_inputPrimType )
	{
		case PRIM_TYPE_POINT:
			gsEntry = "GSInstanceIDPointIn";
			break;
			
		case PRIM_TYPE_LINE:
			gsEntry = "GSInstanceIDLineIn";
			break;
			
		case PRIM_TYPE_TRIANGLE:
			gsEntry = "GSInstanceIDTriangleIn";
			break;
		default:
			WriteToLog("CGSInstanceIDTest::SetupTestCase() - Input primitive type not found!");
			tr = RESULT_FAIL;
			goto exit;
	};

	// Set the number of instances to run per primitive
	char instanceCStr[50];
	sprintf(instanceCStr, "%u", m_numInstances);
	D3D10_SHADER_MACRO macros[] =
	{
		{ "NUM_INSTANCES_PER_PRIMITIVE", instanceCStr},
		NULL
	};


	// Setup the SO decls
	D3D11_SO_DECLARATION_ENTRY soDecls[] =
	{
		{ 0, "gsInstanceID", 0, 0, 1, 0 }
	};

	// requires at least feature level 11.0
	const char* shaderProfile = g_App.GetShaderProfile(D3D_SHADER_STAGE_GEOMETRY, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_0);
	// Compile the GS from the resource
	hr = D3DX10CompileFromResource(
		  NULL,
		  "InstanceID.sh",
		  NULL,
		  macros,
		  NULL,
		  gsEntry,
		  shaderProfile,
		  0,
		  0,
		  NULL,
		  &pShader,
		  &pError,
		  &hr
		);
	
	// Check to see if the compile had any errors
	if( FAILED( hr ) )
	{
		tstring errStr = "CompileShaderFromResource() unexpectedly failed - result code: " + D3DHResultToString( hr );
		WriteToLog( errStr.c_str() );
		tr = RESULT_FAIL;
		goto exit;
	}

	const UINT pBufferStrides[] = 
	{ 
		sizeof( UINT32 ), 
		0, 
		0, 
		0 
	};

	UINT outputStreamStride = sizeof(UINT);
	hr = GetDevice()->CreateGeometryShaderWithStreamOutput( pShader->GetBufferPointer(), pShader->GetBufferSize(), soDecls, 1, pBufferStrides, 1, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &m_pGS );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateGeometryShaderWithStreamOutput() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto exit;
	}

	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	
exit:
	SAFE_RELEASE( pShader );
	SAFE_RELEASE( pError );

	return tr;
}

////////////////////////

TEST_RESULT CGSInstanceIDTest::ExecuteTestCase()
{
	// Local variables
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;


	ID3D11Buffer *buffers[] = { m_pVB };
	UINT strides[] = { sizeof( VS_IN ) };
	UINT offsets[] = { 0 };
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, buffers, strides, offsets );
	GetEffectiveContext()->IASetInputLayout( m_pInputLayout );

	D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	UINT vertexCount = 0;
	
	switch( m_inputPrimType )
	{
		case PRIM_TYPE_POINT:
			topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
			vertexCount = m_numInputPrimitives;
			break;
			
		case PRIM_TYPE_LINE:
			topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			vertexCount = 2 * m_numInputPrimitives;
			break;
			
		case PRIM_TYPE_TRIANGLE:
			topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			vertexCount = 3 * m_numInputPrimitives;
			break;
	};
	
	GetEffectiveContext()->IASetPrimitiveTopology( topology );
	
	// Fill out queryDesc structure
	D3D11_QUERY_DESC querySODesc;
	querySODesc.Query = D3D11_QUERY_SO_STATISTICS;
	querySODesc.MiscFlags = 0;

	D3D11_QUERY_DESC queryPipelineDesc;
	queryPipelineDesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
	queryPipelineDesc.MiscFlags = 0;

	ID3D11Query * pSOStatsQuery = NULL;
	ID3D11Query * pPipelineStatsQuery = NULL;

	hr = GetDevice()->CreateQuery(&queryPipelineDesc, &pPipelineStatsQuery);

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateQuery() for Pipeline unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto exit;
	}

	hr = GetDevice()->CreateQuery(&querySODesc, &pSOStatsQuery);

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateQuery() for Streamout unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto exit;
	}

	// Draw
	GetEffectiveContext()->Begin(pPipelineStatsQuery);
	GetEffectiveContext()->Begin(pSOStatsQuery);

	GetEffectiveContext()->Draw( vertexCount, 0 );

	GetEffectiveContext()->End(pSOStatsQuery);
	GetEffectiveContext()->End(pPipelineStatsQuery);

	// Check the query to make sure we output as many IDs to the streamout buffer as we expected

	// execute any deferred operations
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		tr = RESULT_FAIL;
		WriteToLog("ExecuteEffectiveContext failed. hr = %s", D3DHResultToString(hr).c_str() );
		goto exit;
	}

	//
	// Check pipeline statistics
	//
	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineStats;
	bool bTestPipelineStats = GetFramework()->GetExecutionPriorityLevel() != 1;

	hr = GetFramework()->GetDataWithTimeout(GetImmediateContext(), pPipelineStatsQuery, ( void **) &pipelineStats, sizeof( pipelineStats ), 0 );
	if (hr != S_OK)
	{
		WriteToLog( "GetData failed or timed out for pPipelineStatsQuery, hr=%s", D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto exit;
	}

	if( pipelineStats.GSPrimitives != (m_numInstances * m_numInputPrimitives) && bTestPipelineStats )
	{
		WriteToLog( _T( "Number of GSPrimitives: %u, expected %i" ), pipelineStats.GSPrimitives , (m_numInstances * m_numInputPrimitives) );
		tr = RESULT_FAIL;
		goto exit;
	}
	
	if( pipelineStats.GSInvocations != (m_numInstances * m_numInputPrimitives) && bTestPipelineStats )
	{
		WriteToLog( "Number of GSInvocations: %u, expected %u", pipelineStats.GSInvocations, (m_numInstances * m_numInputPrimitives) );
		tr = RESULT_FAIL;
		goto exit;
	}

	//
	// Check stream out statistics
	//
	D3D11_QUERY_DATA_SO_STATISTICS soStats;
	hr = GetFramework()->GetDataWithTimeout(GetImmediateContext(), pSOStatsQuery, ( void **) &soStats, sizeof( soStats ), 0 );
	if (hr != S_OK)
	{
		WriteToLog( "GetData failed or timed out for pSOStatsQuery, hr=%s", D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto exit;
	}
	
	if( soStats.NumPrimitivesWritten != (m_numInstances * m_numInputPrimitives) && bTestPipelineStats )
	{
		WriteToLog( "Number of SO Primitives written: %u, expected %u", soStats.NumPrimitivesWritten, (m_numInstances * m_numInputPrimitives) );
		tr = RESULT_FAIL;
		goto exit;
	}

	// Examine the stream output
	UINT *pResults;

	GetImmediateContext()->CopySubresourceRegion( m_pStreamOutputBufferCopy, 0, 0, 0, 0, m_pStreamOutputBuffer, 0, NULL );
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;

	if (FAILED( hr = GetImmediateContext()->Map( m_pStreamOutputBufferCopy, 0, D3D11_MAP_READ, 0, &mappedBuffer ) ) )
	{
		WriteToLog( _T( "Failed to map m_pStreamOutputBufferCopy, hr=%s" ), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto exit;
	}

	pResults = (UINT*)(mappedBuffer.pData);

	/*
		While hardware may execute each GS instance in parallel,
		the output of all instances at the end is serialized as 
		if all the instanced GS invocations ran sequentially in a 
		loop iterating vGSInstanceID from 0 to instanceCount-1, with 
		implicit output topology cuts at the end of each instance
	*/

	for(UINT prim = 0; prim < m_numInputPrimitives; prim++)
	{
		for(UINT i = 0; i < m_numInstances; i++)
		{
			UINT index = (prim * m_numInstances) + i;
			UINT actual = pResults[index];
			UINT expected = i;
			if(actual != expected)
			{
				WriteToLog( _T( "Actual GSInstanceID for primitive (#%d): %d. Expected GSInsanceID: %d" ), prim, actual, expected );
				tr = RESULT_FAIL;
				goto exit;
			}
		}
	}

exit:
	SAFE_RELEASE( pSOStatsQuery );
	SAFE_RELEASE( pPipelineStatsQuery );

	// Unmap
	GetImmediateContext()->Unmap( m_pStreamOutputBufferCopy, 0 );
	return tr;
}

/////

void CGSInstanceIDTest::CleanupTestCase()
{
	SAFE_RELEASE( m_pStreamOutputBuffer );
	SAFE_RELEASE( m_pStreamOutputBufferCopy );

	static ID3D11Buffer *vbclear[] = { 0 };
	static UINT vbstridesclear[] = { 0 };
	static UINT vboffsetsclear[] = { 0 };
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, vbclear, vbstridesclear, vboffsetsclear );
	SAFE_RELEASE( m_pVB );
	
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	SAFE_RELEASE( m_pGS );
}
