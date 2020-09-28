#include "GroupSharedMemory.h"

///////////////////////////////////////////////////////////////////////////////
//	Base class for Thread Group Shared Memory test
///////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------
TEST_RESULT CTGSMTest::Setup()
{
	if ( !CComputeTest::Setup() )
	{
		WriteToLog(_T("CComputeTest::Setup() failed"));
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

//------------------------------------------------------------------------------------------------
TEST_RESULT CTGSMTest::SetupTestCase()
{
	if ( SkipSlowOnRefWithMessage( ( m_dispatchX * m_dispatchY * m_dispatchZ > 512 ) ||
		(m_dispatchX * m_dispatchY * m_dispatchZ * m_numThreadsX * m_numThreadsY * m_numThreadsZ > 8192 ) ) )
		return RESULT_SKIP;
	
	if ( SetupOutputUAV() != RESULT_PASS )
	{
		WriteToLog(_T("CTGSMTest::SetupTestCase: SetupOutputUAV failed "));
		return RESULT_FAIL;
	}

	if ( SetupShader() != RESULT_PASS )
	{
		WriteToLog(_T("CTGSMTest::SetupTestCase: SetupShader failed "));
		return RESULT_FAIL;
	}

	if ( GenerateCBdata() != RESULT_PASS )
	{
		WriteToLog(_T("CTGSMTest::SetupTestCase: GenerateCBdata failed "));
		return RESULT_FAIL;
	}

	return RESULT_PASS;

}

//------------------------------------------------------------------------------------------------
TEST_RESULT CTGSMTest::SetupOutputUAV()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.ByteWidth = m_dispatchX * m_dispatchY * m_dispatchZ * 4;
    bufDesc.Usage = D3D11_USAGE_DEFAULT;
    bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    bufDesc.CPUAccessFlags = 0;
    bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufDesc.StructureByteStride = 4;
	hr = GetFramework()->CreateBufferTiledOptional( &bufDesc, NULL, &m_pUABuffer );
	if (FAILED(hr))
	{
		WriteToLog(_T("CTGSMTest::SetupOutputUAV: CreateBuffer failed for m_pUABuffer, HR = %s"), 
			D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto error_end;
	}

	bufDesc.Usage = D3D11_USAGE_STAGING;
    bufDesc.BindFlags = 0;
    bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	hr = GetFramework()->CreateBufferTiledOptional( &bufDesc, NULL, &m_pCopyUABuffer );
	if (FAILED(hr))
	{
		WriteToLog(_T("CTGSMTest::SetupOutputUAV: CreateBuffer failed for m_pCopyUATex1D, HR = %s"), 
			D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto error_end;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC  outUAVDesc;
	outUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	outUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	outUAVDesc.Buffer.FirstElement = 0;
	outUAVDesc.Buffer.NumElements = bufDesc.ByteWidth / 4;
	outUAVDesc.Buffer.Flags = 0;
	hr = m_pDevice->CreateUnorderedAccessView( m_pUABuffer, &outUAVDesc, &m_pOutUAV );
	if (FAILED(hr))
	{
		WriteToLog(_T("CTGSMTest::SetupOutputUAV: CreateUnorderedAccessView failed for m_pOutUAV, HR = %s"), 
			D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto error_end;
	}

	return tr;

error_end:
	SAFE_RELEASE(m_pUABuffer);
	SAFE_RELEASE(m_pCopyUABuffer);
	SAFE_RELEASE(m_pOutUAV);

	return tr;
	
}

//------------------------------------------------------------------------------------------------
const CBSIZE = 128;
float g_floatDataPat[CBSIZE/16] = { 4.1f, 2.2f, 3.3f, 1.4f, 
			                         5.5f, 2.7f, 4.9f, 1.8f }; 
UINT g_uintDataPat[CBSIZE/16] = { 4, 2, 3, 1, 
		                            5, 3, 4, 1, }; 
double g_doubleDataPat[CBSIZE/16] = { 4.1f, 2.2f, 3.3f, 1.4f, 
			                         5.5f, 2.7f, 4.9f, 1.8f }; 
TEST_RESULT CTGSMTest::GenerateCBdata( )
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.ByteWidth = CBSIZE;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	GetFramework()->CreateBufferTiledOptional( &bufDesc, NULL, &m_pCBufferData );
		
	// fill the data
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	hr = GetImmediateContext()->Map( m_pCBufferData, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
	if (FAILED (hr))
	{
		WriteToLog( _T("CTGSMTest::GenerateCBdata: Failed to map constant buffer.") );
		return RESULT_FAIL;
	}

	float* pFloatData = NULL;
	UINT* pUintData = NULL;
	double* pDoubleData = NULL;

	if ( m_dataType == _T("float") )
	{	
		pFloatData = (float*) mappedRes.pData;
		for ( UINT i = 0; i < CBSIZE/16; i++ )
		{
			pFloatData[i * 4] = g_floatDataPat[i];
			pFloatData[i * 4 + 1] = 0.0f;
			pFloatData[i * 4 + 2] = 0.0f;
			pFloatData[i * 4 + 3] = 0.0f;
		}
	}
	else if ( m_dataType == _T("uint") )
	{
		pUintData = (UINT*) mappedRes.pData;
		for ( UINT i = 0; i < CBSIZE/16; i++ )
		{
			pUintData[i * 4] = g_uintDataPat[i];
			pUintData[i * 4 + 1] = 0;
			pUintData[i * 4 + 2] = 0;
			pUintData[i * 4 + 3] = 0;
		}
	}
	else if ( m_dataType == _T("double") )
	{
		if ( m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 )
		{
			WriteToLog( _T("CTGSMTest::GenerateCBdata: \
						   Doubles are not supported on feature levels lower than 11.") );
			return RESULT_FAIL;
		}
		pDoubleData = (double*) mappedRes.pData;
		for ( UINT i = 0; i < CBSIZE/16; i++ )
		{
			pDoubleData[i * 4] = g_doubleDataPat[i];
			pDoubleData[i * 4 + 1] = 0;
		}
	}
	
	GetImmediateContext()->Unmap(m_pCBufferData,0);
	
	return RESULT_PASS;
}

//------------------------------------------------------------------------------------------------
TEST_RESULT CTGSMTest::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	const UINT clearVals[4] = {0, 0, 0, 0};
	
	m_pEffectiveContext->ClearUnorderedAccessViewUint(m_pOutUAV, clearVals);
	m_pEffectiveContext->CSSetUnorderedAccessViews(0, 1, &m_pOutUAV, NULL);
	m_pEffectiveContext->CSSetConstantBuffers( 0, 1, &m_pCBufferData );
	m_pEffectiveContext->CSSetShader(m_pCS, NULL, NULL);
	m_pEffectiveContext->Dispatch(m_dispatchX, m_dispatchY, m_dispatchZ);
	m_pEffectiveContext->CopyResource(m_pCopyUABuffer, m_pUABuffer);
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		return RESULT_FAIL;
	}

	D3D11_MAPPED_SUBRESOURCE mappedRes;
	hr = GetImmediateContext()->Map( m_pCopyUABuffer, 0,  D3D11_MAP_READ, 0, &mappedRes );
	if (FAILED (hr))
	{
		WriteToLog( _T("CTGSM10Test::ExecuteTestCase: Failed to map m_pCopyUABuffer.") );
		return RESULT_FAIL;
	}

	float floatExpRes = 0;
	UINT uintExpRes = 0;
	double doubleExpRes = 0;
	float* pFloatData = NULL;
	UINT* pUintData = NULL;
	double* pDoubleData = NULL;

	if ( m_dataType == _T("float") )
	{
		if ( !GetExpectedResult(NULL, &floatExpRes) )
		{
			WriteToLog( _T("CTGSMTest::ExecuteTestCase: GetExpectedResult failed for float data.") );
			tr = RESULT_FAIL;
			goto DONE;
		}
		pFloatData = (float*)mappedRes.pData;
		for( UINT i = 0; i < m_dispatchX * m_dispatchY * m_dispatchZ; i++)
		{
			float RelativeEpsilon = 0.02f;
			if ( (abs(pFloatData[i] - floatExpRes)) / floatExpRes  > RelativeEpsilon )
			{
				WriteToLog( _T("CTGSMTest::ExecuteTestCase: Expected result: %f, Result found: %f \
							   in thread group %d."), 
							   floatExpRes, pFloatData[i], i );
				tr = RESULT_FAIL;
				break;
			}
		}
	}
	else if ( m_dataType == _T("uint") )
	{
		if ( !GetExpectedResult(&uintExpRes, NULL) )
		{
			WriteToLog( _T("CTGSMTest::ExecuteTestCase: GetExpectedResult failed for uint data.") );
			tr = RESULT_FAIL;
			goto DONE;
		}
		pUintData = (UINT*)mappedRes.pData;
		for( UINT i = 0; i < m_dispatchX * m_dispatchY * m_dispatchZ; i++)
		{
			if ( pUintData[i] != uintExpRes )
			{
				WriteToLog( _T("CTGSMTest::ExecuteTestCase: Expected result: %d, Result found: %d \
							   in thread group %d."), 
							   uintExpRes, pUintData[i], i );
				tr = RESULT_FAIL;
				break;
			}
		}
	}
	else if ( m_dataType == _T("double") )
	{
		if ( !GetExpectedResult(NULL, NULL, &doubleExpRes) )
		{
			WriteToLog( _T("CTGSMTest::ExecuteTestCase: GetExpectedResult failed for double data.") );
			tr = RESULT_FAIL;
			goto DONE;
		}
		pDoubleData = (double*)mappedRes.pData;
		for( UINT i = 0; i < m_dispatchX * m_dispatchY * m_dispatchZ; i++)
		{
			if ( pDoubleData[i] != doubleExpRes )
			{
				WriteToLog( _T("CTGSMTest::ExecuteTestCase: Expected result: %d, Result found: %d \
							   in thread group %d."), 
							   doubleExpRes, pDoubleData[i], i );
				tr = RESULT_FAIL;
				break;
			}
		}
	}

DONE:
	GetImmediateContext()->Unmap( m_pCopyUABuffer, 0);
	return tr;

}



//------------------------------------------------------------------------------------------------
void CTGSMTest::CleanupTestCase()
{
	SAFE_RELEASE(m_pUABuffer);
	SAFE_RELEASE(m_pCopyUABuffer);
	SAFE_RELEASE(m_pOutUAV);
	SAFE_RELEASE(m_pCBufferData);
	SAFE_RELEASE(m_pCS);
}


///////////////////////////////////////////////////////////////////////////////
//  Class for TGSM of Compute on 10.x
//	class CTGSM10Test : public CTGSMTest
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
	UINT numThreads;
	UINT numX;
	UINT numY;
	UINT numZ;
	UINT TGSMSize;
}TGSMLimit;

typedef struct
{
	UINT XDimension;
	UINT YDimension;
}XYDimensions;

TGSMLimit g_TGSMSizeLimits[] = 
{
// Num of threads per group	(x, y, z)	writable per thread
//	{ 1,	1, 1, 1,					4}, // compiler bug for 1, 1, 1 as thread number per group
	{ 16,	4, 4, 1,					4},
	{ 64,	4, 8, 2,					256},
	{ 68,	17, 1, 4,					240},
	{ 72,	2, 4, 9,					224},
	{ 76,	2, 19, 2,					208},
	{ 84,	1, 14, 6,					192},
	{ 92,	4, 9, 2,					176},
	{ 100,	5, 5, 4,					160},
	{ 112,	7, 8, 2,					144},
	{ 128,	64,2, 1,					128},
	{ 144,	12,6, 2,					112},
	{ 168,	14, 6, 2,					96},
	{ 204,	3, 17, 4,					80},
	{ 256,	16, 16, 1,					64},
	{ 340,	17, 10, 2,					48},
	{ 512,	8, 8, 8,					32},
	{ 768,	12, 8, 8,					16}
	
};

XYDimensions g_DispatchXY[] = 
{
						// num of groups
	{1,		1},			// 1
	{2,		3},			// 6
	{512,		64},	// 16K
//	{65535,		2}		// 128K  // requires too large amount of memory that casues E_OUTOFMEMORY 
};

tstring g_shaderStr10x = 
"#ifndef MACRO_SET \n"
"#define NUM_TGSM_DATA 16\n"
"#define NUM_THREADS_X 4\n"
"#define NUM_THREADS_Y 4\n"
"#define NUM_THREADS_Z 1\n"
"#define DISPATCH_X 64\n"
"#define DISPATCH_Y 64\n"
"#define DISPATCH_Z 1\n"
"#define TYPE uint\n"
"#define CBUFFERSIZE 8\n"
"#endif\n"
"#define groupthreads ( NUM_THREADS_X * NUM_THREADS_Y * NUM_THREADS_Z )\n"
"struct output \n"
"{ \n"
"    TYPE data; \n"
"}; \n"
"struct TGSMstruct \n"
"{ \n"
"	TYPE data[NUM_TGSM_DATA]; \n"
"}; \n"
"cbuffer cbCS : register( b0 ) \n"
"{ \n"
"    TYPE Cbuf[CBUFFERSIZE];   \n"
"}; \n"
"RWStructuredBuffer<output> Result : register( u0 ); \n"
"groupshared TGSMstruct gData[groupthreads]; \n"
"[numthreads( NUM_THREADS_X, NUM_THREADS_Y, NUM_THREADS_Z )] \n"
"void CSMain( uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID,  uint GFid: SV_GroupIndex) \n"
"{ \n"
"	uint gidx = Gid.z * ( DISPATCH_X * DISPATCH_Y ) + Gid.y * DISPATCH_X + Gid.x;	\n"
"	uint DataStartIndex = ( GFid * 17 ) % (CBUFFERSIZE); \n"
"   // fill data to the shared memory \n"
"	for ( uint k = 0; k < NUM_TGSM_DATA ; k++) \n "
"	{ \n"
"		uint DataIndex = (DataStartIndex + k) % (CBUFFERSIZE); \n"
"		gData[GFid].data[k] = Cbuf[DataIndex]; \n"
"	} \n"
"   GroupMemoryBarrierWithGroupSync(); \n"
"    // only choose at most three data to add: first, last and the one at the 1/4 position  \n"
"    uint quarter = (NUM_TGSM_DATA - 1) >> 2; \n"
"    uint j = 0;   \n"
"    for ( uint i = groupthreads; i > 1 ; i = j) \n"
"    { \n"
"		j = ( i + 1 ) >> 1; // valid for both even and odd number of threads \n"
"		if ( ( GFid < j ) && ( GFid + j < i ) ) \n"
"		{"
"			gData[GFid].data[0] += gData[GFid + j].data[NUM_TGSM_DATA - 1];"
"			if ( NUM_TGSM_DATA - 1 != 0 )"
"				gData[GFid].data[NUM_TGSM_DATA - 1] += gData[GFid + j].data[0];"
"			if ( quarter != 0 )"
"				gData[GFid].data[quarter] += gData[GFid + j].data[quarter];"
"		}		"
		"GroupMemoryBarrierWithGroupSync();"
"    }   "
"    if ( GFid == 0)"
"   {"
"		struct output Op;"
"		Op.data = 0;"
"		Op.data += gData[GFid].data[0];"
"		if ( NUM_TGSM_DATA - 1 != 0 )"
"				Op.data += gData[GFid].data[NUM_TGSM_DATA - 1];"
"		if ( quarter != 0 )"
"				Op.data += gData[GFid].data[quarter];"
"		Result[gidx] = Op;"
"   }"
"}";


//------------------------------------------------------------------------------------------------
TEST_RESULT CTGSM10Test::Setup()
{

	if ( !CTGSMTest::Setup() )
	{
		return RESULT_FAIL;
	}

	// First check if compute on 10.x is supported
	D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS compOn10Support;
    m_pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &compOn10Support, 
		sizeof(compOn10Support) );
	if ( !compOn10Support.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x )
	{
		WriteToLog(_T("CTGSMTest::Setup(): Compute on 10.x is not supported by the driver/hardware."));
		return RESULT_SKIP;
	}

	return RESULT_PASS;
}

//------------------------------------------------------------------------------------------------
TEST_RESULT CTGSM10Test::SetupTestCase()
{
	return CTGSMTest::SetupTestCase();
}

//------------------------------------------------------------------------------------------------
void CTGSM10Test::InitTestParameters()
{
	m_dispatchZ = 1;

	// dispatch groups
	testfactor::RFactor rfDispatchX = AddParameter<UINT>( _T("DispatchDimensionX"), &m_dispatchX, 
		new CTableValueSet<UINT>(&g_DispatchXY[0].XDimension, sizeof(g_DispatchXY[0]), ARRAY_SIZE(g_DispatchXY) ) );

	testfactor::RFactor rfDispatchY = AddParameter<UINT>( _T("DispatchDimensionY"), &m_dispatchY, 
		new CTableValueSet<UINT>(&g_DispatchXY[0].YDimension, sizeof(g_DispatchXY[0]), ARRAY_SIZE(g_DispatchXY) ) );

	CTestCaseParameter<UINT> *pDispatchZ = AddParameter<UINT>( _T( "DispatchDimensionZ" ), &m_dispatchZ );
	testfactor::RFactor rfDispatchZ = AddParameterValue<UINT>(pDispatchZ, 1);

	// threads per group
	testfactor::RFactor rfThreadNumX = AddParameter<UINT>( _T("NumThreadPerGroupX"), &m_numThreadsX, 
		new CTableValueSet<UINT>(&g_TGSMSizeLimits[0].numX, sizeof(g_TGSMSizeLimits[0]), ARRAY_SIZE(g_TGSMSizeLimits) ) );

	testfactor::RFactor rfThreadNumY = AddParameter<UINT>( _T("NumThreadPerGroupY"), &m_numThreadsY, 
		new CTableValueSet<UINT>(&g_TGSMSizeLimits[0].numY, sizeof(g_TGSMSizeLimits[0]), ARRAY_SIZE(g_TGSMSizeLimits) ) );

	testfactor::RFactor rfThreadNumZ = AddParameter<UINT>( _T("NumThreadPerGroupZ"), &m_numThreadsZ, 
		new CTableValueSet<UINT>(&g_TGSMSizeLimits[0].numZ, sizeof(g_TGSMSizeLimits[0]), ARRAY_SIZE(g_TGSMSizeLimits) ) );

	testfactor::RFactor rfTGSMSize = AddParameter<UINT>( _T("ThreadWritableTGSM"), &m_TGSMSize, 
		new CTableValueSet<UINT>(&g_TGSMSizeLimits[0].TGSMSize, sizeof(g_TGSMSizeLimits[0]), ARRAY_SIZE(g_TGSMSizeLimits) ) );

	// TGSM data type
	CUserValueSet<tstring> *pTGSMDataType = new CUserValueSet<tstring>;
	pTGSMDataType->AddValue(_T("uint"));
	pTGSMDataType->AddValue(_T("float"));
	testfactor::RFactor rfDataType = AddParameter<tstring>(_T("TGSMDataType"), &m_dataType, pTGSMDataType);

	testfactor::RFactor rfRoot = rfDataType * ( rfDispatchX % rfDispatchY ) * rfDispatchZ 
		* ( rfThreadNumX % rfThreadNumY % rfThreadNumZ % rfTGSMSize );

	SetRootTestFactor(rfRoot);	

	// Adding priority filters
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumThreadPerGroupX"), 4), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumThreadPerGroupX"), 12), WeightFactorLarge);
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
}




//------------------------------------------------------------------------------------------------
TEST_RESULT CTGSM10Test::SetupShader()
{
	TEST_RESULT tr= RESULT_PASS;
	HRESULT hr;

	tstring TGSMSizeStr = FormatString( "%i", m_TGSMSize/4 );
	tstring numThreadsXStr = FormatString( "%i", m_numThreadsX );
	tstring numThreadsYStr = FormatString( "%i", m_numThreadsY );
	tstring numThreadsZStr = FormatString( "%i", m_numThreadsZ );
	tstring dispatchXStr = FormatString( "%i", m_dispatchX );
	tstring dispatchYStr = FormatString( "%i", m_dispatchY );
	tstring dispatchZStr = FormatString( "%i", m_dispatchZ );
	tstring cBufSizeStr = FormatString( "%i", (CBSIZE/16) );

	D3D10_SHADER_MACRO macroSet[] =
	{
		{ "MACRO_SET", "true" },
		{ "NUM_TGSM_DATA", TGSMSizeStr.c_str() },
		{ "NUM_THREADS_X", numThreadsXStr.c_str() },
		{ "NUM_THREADS_Y", numThreadsYStr.c_str() },
		{ "NUM_THREADS_Z", numThreadsZStr.c_str() },
		{ "DISPATCH_X", dispatchXStr.c_str() },
		{ "DISPATCH_Y", dispatchYStr.c_str() },
		{ "DISPATCH_Z", dispatchZStr.c_str() },
		{ "TYPE", m_dataType.c_str() },
		{ "CBUFFERSIZE", cBufSizeStr.c_str() },
		NULL
	};

	hr = CompileAndCreateComputeShaderFromMemory( 
		g_shaderStr10x.c_str(), 
		strlen( g_shaderStr10x.c_str()), 
		_T("CSMain"), 
		m_pDevice->GetFeatureLevel(), 
		&m_pCS,
		macroSet
		);


	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CTGSM10Test::SetupShader(): CompileAndCreateComputeShaderFromMemory() failed. hr=%s\n" ), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto shader_done;
	}

shader_done:
	return tr;
}



//------------------------------------------------------------------------------------------------
bool CTGSM10Test::GetExpectedResult( UINT* pUintResult, float* pFloatResult, double* pDoubleResult)
{
	if ( m_dataType == _T("float") && !pFloatResult)
		return false;
	if ( m_dataType == _T("uint") && !pUintResult)
		return false;
	if ( m_dataType == _T("double") && !pDoubleResult)
		return false;


	UINT numGroupThreads = m_numThreadsX * m_numThreadsY * m_numThreadsZ;
	UINT numTGSMData = m_TGSMSize/4;
	if ( m_dataType == _T("float"))
	{
		float *TGSMData = new float[numGroupThreads * numTGSMData];
		if (!TGSMData)
			return false;
		for (UINT i = 0; i < numGroupThreads; i++)
		{
			UINT DataStartIndex = ( i * 17 ) % ( CBSIZE/16);
			for ( UINT j = 0; j < numTGSMData ; j++)
			{
				UINT DataIndex = (DataStartIndex + j) % ( CBSIZE/16);
				TGSMData[i * numTGSMData + j] = g_floatDataPat[DataIndex];
			}
		}
		float resFloat = 0.0;
		UINT quarter = (numTGSMData - 1) >> 2;
		for (UINT i = 0; i < numGroupThreads; i++)
		{
			resFloat += TGSMData[i * numTGSMData + 0];
			if ( numTGSMData - 1 != 0)
				resFloat += TGSMData[i * numTGSMData + numTGSMData - 1];
			if ( quarter != 0 )
				resFloat += TGSMData[i * numTGSMData + quarter];
		} 
		*pFloatResult = resFloat;
		delete[] TGSMData;
		return true;
	}
	else if ( m_dataType == _T("uint") )
	{
		UINT *TGSMData = new UINT[numGroupThreads * numTGSMData];
		if (!TGSMData)
			return false;
		for (UINT i = 0; i < numGroupThreads; i++)
		{
			UINT DataStartIndex = ( i * 17 ) % ( CBSIZE/16);
			for ( UINT j = 0; j < numTGSMData ; j++)
			{
				UINT DataIndex = (DataStartIndex + j) % ( CBSIZE/16);
				TGSMData[i * numTGSMData + j] = g_uintDataPat[DataIndex];
			}
		}
		UINT resUint = 0;
		UINT quarter = (numTGSMData - 1) >> 2;
		for (UINT i = 0; i < numGroupThreads; i++)
		{
			resUint += TGSMData[i * numTGSMData + 0];
			if ( numTGSMData - 1 != 0)
				resUint += TGSMData[i * numTGSMData + numTGSMData - 1];
			if ( quarter != 0 )
				resUint += TGSMData[i * numTGSMData + quarter];
		} 
		*pUintResult = resUint;
		delete[] TGSMData;
		return true;
	}

	return false;
}





///////////////////////////////////////////////////////////////////////////////
//  Class for TGSM of Compute FL 11
//	class CTGSM11Test : public CTGSMTest
///////////////////////////////////////////////////////////////////////////////
typedef struct
{
	UINT NumUnits;// the number of data of 32bits or 64bits(double)
	UINT NumDcls;
}RawTGSMDcl;

RawTGSMDcl g_RawTGSMDcls[] =
{
	{ 1,	1 },
	{ 4,	2 },
	{ 3,	4 },
	{ 8,	16 },
	{ 32,	5 },
};

RawTGSMDcl g_RawTGSMDclsLimit[] =
{
	{ 512,	8 },
	{ 512,	8 },
	{ 1024,	8 }
};

typedef struct
{
	UINT NumUnits;// the number of data of 32bits 
	UINT NumDcls;
	UINT ArraySize;
}StructTGSMDcl;

StructTGSMDcl g_StructTGSMDcls[] =
{
	{ 1,	1,		1 },
	{ 3,	2,		4 },
	{ 8,	4,		3 },
	{ 2,	16,		8 },
	{ 2,	5,		32 },
};


StructTGSMDcl g_StructTGSMDclsLimit[] =
{
	{ 1,	8,		512 },
	{ 1,	8,		512 },
	{ 1,	8,		1024 },
};


typedef struct
{
	UINT numX;
	UINT numY;
	UINT numZ;
}NumThreadsXYZ;

NumThreadsXYZ g_NumThreads[] =
{
//	{1, 1, 1},		// 1
	{8, 8, 2},		// 128 
	{16, 4, 16},	// 1024
};

// Needs 3 sets to pair with g_StructTGSMDclsLimit and g_RawTGSMDclsLimit
NumThreadsXYZ g_NumThreadsLimit[] =
{
	{1, 512, 1},	// 512
	{1, 1024, 1},	// 1024
	{1, 1024, 1},	// 1024
};



//------------------------------------------------------------------------------------------------
TEST_RESULT CTGSM11Test::Setup()
{
	TEST_RESULT tr = CTGSMTest::Setup();
	if ( tr != RESULT_PASS)
	{
		WriteToLog(_T("CTGSMTest::Setup() failed"));
		return tr;
	}

	if (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
	{
		WriteToLog(_T("Skip this group on feature levels less than 11_0."));
		return RESULT_SKIP;
	}
	return RESULT_PASS;
}

//------------------------------------------------------------------------------------------------
TEST_RESULT CTGSM11Test::SetupTestCase()
{
	return CTGSMTest::SetupTestCase();
}

//------------------------------------------------------------------------------------------------
void CTGSM11Test::InitTestParameters()
{
	m_dispatchZ = 1;

	// dispatch groups
	testfactor::RFactor rfDispatchX = AddParameter<UINT>( _T("DispatchDimensionX"), &m_dispatchX, 
		new CTableValueSet<UINT>(&g_DispatchXY[0].XDimension, sizeof(g_DispatchXY[0]), ARRAY_SIZE(g_DispatchXY) ) );

	testfactor::RFactor rfDispatchY = AddParameter<UINT>( _T("DispatchDimensionY"), &m_dispatchY, 
		new CTableValueSet<UINT>(&g_DispatchXY[0].YDimension, sizeof(g_DispatchXY[0]), ARRAY_SIZE(g_DispatchXY) ) );

	CTestCaseParameter<UINT> *pDispatchZ = AddParameter<UINT>( _T( "DispatchDimensionZ" ), &m_dispatchZ );
	testfactor::RFactor rfDispatchZ = AddParameterValue<UINT>(pDispatchZ, 1);

	// threads per group
	testfactor::RFactor rfThreadNumX = AddParameter<UINT>( _T("NumThreadPerGroupX"), &m_numThreadsX, 
		new CTableValueSet<UINT>(&g_NumThreads[0].numX, sizeof(g_NumThreads[0]), ARRAY_SIZE(g_NumThreads) ) );

	testfactor::RFactor rfThreadNumY = AddParameter<UINT>( _T("NumThreadPerGroupY"), &m_numThreadsY, 
		new CTableValueSet<UINT>(&g_NumThreads[0].numY, sizeof(g_NumThreads[0]), ARRAY_SIZE(g_NumThreads) ) );

	testfactor::RFactor rfThreadNumZ = AddParameter<UINT>( _T("NumThreadPerGroupZ"), &m_numThreadsZ, 
		new CTableValueSet<UINT>(&g_NumThreads[0].numZ, sizeof(g_NumThreads[0]), ARRAY_SIZE(g_NumThreads) ) );

	testfactor::RFactor rfThreadNumXLimit = AddParameterValueSet<UINT>( _T("NumThreadPerGroupX"),  
		new CTableValueSet<UINT>(&g_NumThreadsLimit[0].numX, sizeof(g_NumThreadsLimit[0]), ARRAY_SIZE(g_NumThreadsLimit) ) );

	testfactor::RFactor rfThreadNumYLimit = AddParameterValueSet<UINT>( _T("NumThreadPerGroupY"),
		new CTableValueSet<UINT>(&g_NumThreadsLimit[0].numY, sizeof(g_NumThreadsLimit[0]), ARRAY_SIZE(g_NumThreadsLimit) ) );

	testfactor::RFactor rfThreadNumZLimit = AddParameterValueSet<UINT>( _T("NumThreadPerGroupZ"),  
		new CTableValueSet<UINT>(&g_NumThreadsLimit[0].numZ, sizeof(g_NumThreadsLimit[0]), ARRAY_SIZE(g_NumThreadsLimit) ) );

	// array size for structured TGSM
	testfactor::RFactor rfStructArraySize = AddParameter<UINT>( _T("StructuredTGSMArraySize"), &m_TGSMArraySize, 
		new CTableValueSet<UINT>(&g_StructTGSMDcls[0].ArraySize, sizeof(g_StructTGSMDcls[0]), 
		ARRAY_SIZE(g_StructTGSMDcls) ) );
	testfactor::RFactor rfStructArraySizeLimit = AddParameterValueSet<UINT>( _T("StructuredTGSMArraySize"), 
		new CTableValueSet<UINT>(&g_StructTGSMDclsLimit[0].ArraySize, sizeof(g_StructTGSMDclsLimit[0]), 
		ARRAY_SIZE(g_StructTGSMDclsLimit) ) );

	// number of data per data struct
	testfactor::RFactor rfStructUnitSize = AddParameter<UINT>( _T("SizePerTGSMDataUnit"), &m_TGSMUnitSize, 
		new CTableValueSet<UINT>(&g_StructTGSMDcls[0].NumUnits, sizeof(g_StructTGSMDcls[0]), 
		ARRAY_SIZE(g_StructTGSMDcls) ) );
	testfactor::RFactor rfStructUnitSizeLimit = AddParameterValueSet<UINT>( _T("SizePerTGSMDataUnit"),  
		new CTableValueSet<UINT>(&g_StructTGSMDclsLimit[0].NumUnits, sizeof(g_StructTGSMDclsLimit[0]), 
		ARRAY_SIZE(g_StructTGSMDclsLimit) ) );
	testfactor::RFactor rfRawUnitSize = AddParameterValueSet<UINT>( _T("SizePerTGSMDataUnit"), 
		new CTableValueSet<UINT>(&g_RawTGSMDcls[0].NumUnits, sizeof(g_RawTGSMDcls[0]), ARRAY_SIZE(g_RawTGSMDcls) ) );
	testfactor::RFactor rfRawUnitSizeLimit = AddParameterValueSet<UINT>( _T("SizePerTGSMDataUnit"), 
		new CTableValueSet<UINT>(&g_RawTGSMDclsLimit[0].NumUnits, sizeof(g_RawTGSMDclsLimit[0]), 
		ARRAY_SIZE(g_RawTGSMDclsLimit) ) );
	

	// number of dcls
	testfactor::RFactor rfStructNumDcls = AddParameter<UINT>( _T("NumOfTGSMDcls"), &m_numTGSMDcls, 
		new CTableValueSet<UINT>(&g_StructTGSMDcls[0].NumDcls, sizeof(g_StructTGSMDcls[0]), 
		ARRAY_SIZE(g_StructTGSMDcls) ) );
	testfactor::RFactor rfStructNumDclsLimit = AddParameterValueSet<UINT>( _T("NumOfTGSMDcls"),  
		new CTableValueSet<UINT>(&g_StructTGSMDclsLimit[0].NumDcls, sizeof(g_StructTGSMDclsLimit[0]), 
		ARRAY_SIZE(g_StructTGSMDclsLimit) ) );
	testfactor::RFactor rfRawNumDcls = AddParameterValueSet<UINT>( _T("NumOfTGSMDcls"),
		new CTableValueSet<UINT>(&g_RawTGSMDcls[0].NumDcls, sizeof(g_RawTGSMDcls[0]), ARRAY_SIZE(g_RawTGSMDcls) ) );
	testfactor::RFactor rfRawNumDclsLimit = AddParameterValueSet<UINT>( _T("NumOfTGSMDcls"),
		new CTableValueSet<UINT>(&g_RawTGSMDclsLimit[0].NumDcls, sizeof(g_RawTGSMDclsLimit[0]), 
		ARRAY_SIZE(g_RawTGSMDclsLimit) ) );


	// TGSM decl type
	testfactor::RFactor rfTGSMDclUniformRaw = AddParameter<tstring>(_T("TGSMDclScenarios"), &m_TGSMDclScenarios, _T("UniformRawTGSMs"));
	testfactor::RFactor rfTGSMDclUniformStruct = AddParameterValue<tstring>(_T("TGSMDclScenarios"),  _T("UniformStructTGSMs"));
	testfactor::RFactor rfTGSMDclMixed = AddParameterValue<tstring>(_T("TGSMDclScenarios"),  _T("MixedTGSMs"));

	CUserValueSet<tstring> *pTGSMDataType = new CUserValueSet<tstring>;
	pTGSMDataType->AddValue(_T("uint"));
	pTGSMDataType->AddValue(_T("float"));
//	pTGSMDataType->AddValue(_T("double"));  // disabled for a compiler bug: directx 109373
	testfactor::RFactor rfDataType = AddParameter<tstring>(_T("TGSMDataType"), &m_dataType, pTGSMDataType);
	testfactor::RFactor rfDataTypeFloat = AddParameterValue<tstring>(_T("TGSMDataType"), _T("float"));

	testfactor::RFactor rfRoot =  ( rfDispatchX % rfDispatchY ) * rfDispatchZ 
		* ( 
		// Raw TGSM
		    rfDataType * rfTGSMDclUniformRaw * 
				(  (rfRawNumDcls % rfRawUnitSize) * ( rfThreadNumX % rfThreadNumY % rfThreadNumZ ) + 
		// Raw TGSM with Max size allowed
				(rfRawNumDclsLimit % rfRawUnitSizeLimit) % ( rfThreadNumXLimit % rfThreadNumYLimit % rfThreadNumZLimit ) )
		// structured TGSM
		    + rfDataType * rfTGSMDclUniformStruct * 
				(  (rfStructNumDcls % rfStructUnitSize % rfStructArraySize) * ( rfThreadNumX % rfThreadNumY % rfThreadNumZ ) + 
		// structured TGSM with Max size allowed
				  (rfStructNumDclsLimit % rfStructUnitSizeLimit % rfStructArraySizeLimit) 
				  % ( rfThreadNumXLimit % rfThreadNumYLimit % rfThreadNumZLimit ) )
	   // Mixed TGSM dcls
			+ rfDataTypeFloat * rfTGSMDclMixed * ( rfThreadNumX % rfThreadNumY % rfThreadNumZ )
		) ;

	SetRootTestFactor(rfRoot);	

	// Adding priority filters
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumThreadPerGroupY"), 8), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumThreadPerGroupY"), 512), WeightFactorLarge);
	AddPriorityPatternFilter(FilterLess<UINT>(_T("DispatchDimensionX"), 4), WeightFactorLarge);
	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
}


//------------------------------------------------------------------------------------------------
/* When NumThreads > totalNumData ( num of data in the structure * number of TGSM decls) 
/* The first totalNumData of threads participate in writing data from constant buffer to one data location 
/* in the group shared memory.
/* When NumThreads < totalNumData, each thread writes to possibly more than one data location ( NumThreads * 0 + FGid, NumThreads * 1 + FGid ...) 
/* from the constant buffer to the group shared memory.
/*
/* Then all the data in the TGSM are added up in parallel by the threads. If NUM_TGSM_DATA > NUM_TGSM_DCLS,
/* threads run in parallel in NUM_TGSM_DATA dimension. Each thread adds the data with index FGid from all TGSM dcls.
/*
/* If NUM_TGSM_DATA < NUM_TGSM_DCLS, threads run in parallel in NUM_TGSM_DCLS dimension. Each thread adds all the 
/* data from the TGSM with dcl index GFid
/*
/* In order to add up all data in all TGSM dcls (for the simplicity of verification), 
/* NumThreads has to be greater than Max(NUM_TGSM_DATA, NUM_TGSM_DCLS).
*/
tstring g_shaderHeadStr1 = 
"#ifndef MACRO_SET \n"
"#define NUM_TGSM_DATA 2 \n"
"#define NUM_THREADS_X 2 \n"
"#define NUM_THREADS_Y 2 \n"
"#define NUM_THREADS_Z 1 \n"
"#define DISPATCH_X 1 \n"
"#define DISPATCH_Y 1 \n"
"#define DISPATCH_Z 1 \n"
"#define TYPE uint \n"
"#define ARRAY_SIZE 8 \n"
"#define CBUFFERSIZE 8 \n"
"#define NUM_TGSM_DCLS 4 \n"
"#endif \n"
"struct output \n"
"{ \n"
"    TYPE data; \n"
"}; \n"
"struct TGSMstruct \n"
"{ \n"
"	TYPE data[NUM_TGSM_DATA]; \n"
"}; \n"
"cbuffer cbCS : register( b0 ) \n"
"{ \n"
"    TYPE Cbuf[CBUFFERSIZE];   \n"
"}; \n"  
"RWStructuredBuffer<output> Result : register( u0 ); \n"
"#define groupthreads ( NUM_THREADS_X * NUM_THREADS_Y * NUM_THREADS_Z ) \n";


tstring g_shaderHeadStr2 = 
"[numthreads( NUM_THREADS_X, NUM_THREADS_Y, NUM_THREADS_Z )] \n"
"void CSMain( uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID,  uint GFid: SV_GroupIndex) \n"
"{ \n"
"	uint gidx = Gid.z * ( DISPATCH_X * DISPATCH_Y ) + Gid.y * DISPATCH_X + Gid.x;	 \n";

tstring g_shaderBodyPart1 =
"	// fill data to the shared memory \n"
"	uint numDataPerThread = ((NUM_TGSM_DCLS * numDataBlocks )%groupthreads)? ((NUM_TGSM_DCLS * numDataBlocks )/groupthreads + 1): \n"
"	                                                                          ((NUM_TGSM_DCLS * numDataBlocks )/groupthreads);	 \n"
"   [loop] \n"
"	for(uint k = 0; k < numDataPerThread; k++) \n"
"	{ \n"
"		uint TGSMIndex = k * groupthreads + GFid; \n"
"		if ( TGSMIndex < NUM_TGSM_DCLS * numDataBlocks ) \n"
"		{ \n"
"			uint j = TGSMIndex / numDataBlocks; \n"
"			uint i = TGSMIndex % numDataBlocks; \n"
"			uint DataIndex = TGSMIndex % (CBUFFERSIZE ); \n"
"			[branch]"
"			switch(j) \n"
"			{  \n";

tstring g_shaderBodyPart2 = 
"			} \n"
"		} \n"
"	}	\n"
"	GroupMemoryBarrierWithGroupSync(); \n";

tstring g_biggerNumDataPart1 = 
"	for(uint i = 0; ( i < numDataBlocks ) && ( i < groupthreads ); i++)  \n"
"	{  \n"
"		if ( GFid == i)  \n"
"		{  \n"
"			uint sumIndex = i;    \n";

tstring g_biggerNumDataPart2 = 
"		} \n"
"	} \n"
"	GroupMemoryBarrierWithGroupSync(); \n"
"	if (GFid == 0) \n"
"	{ \n"
"		struct output Op; \n"
"		Op.data = 0; \n"
"		for(uint i = 0; ( i < numDataBlocks ) && ( i < groupthreads ); i++) \n" ;


tstring g_biggerNumDclsPart1 = 
"	if ( numDataBlocks > 1) \n"
"	{ \n"
"		switch(GFid) \n"
"			{ \n";

tstring g_biggerNumDclsPart2 = 
"			}	\n"
"	} \n"
"	GroupMemoryBarrierWithGroupSync(); \n"
"	if (GFid == 0) \n"
"	{ \n"
"		struct output Op; \n"
"		Op.data = 0; \n";


tstring g_shaderEndStr = 
"		Result[gidx] = Op; \n"
"	} \n"
"} \n" ;



tstring g_shaderMixedDcls = 
"#ifndef MACRO_SET \n"
"#define NUM_THREADS_X 2 \n"
"#define NUM_THREADS_Y 2 \n"
"#define NUM_THREADS_Z 1 \n"
"#define DISPATCH_X 1 \n"
"#define DISPATCH_Y 1 \n"
"#define DISPATCH_Z 1 \n"
"#endif \n"
"struct output \n"
"{ \n"
"    float data; \n"
"}; \n"
"struct TGSMstruct \n"
"{ \n"
"	int intData1; \n"
"	int2 intData2; \n"
"	float floatData3[3]; \n"
"	float4 floatData4; \n"
"}; \n"
"RWStructuredBuffer<output> Result : register( u0 ); \n"
"#define groupthreads ( NUM_THREADS_X * NUM_THREADS_Y * NUM_THREADS_Z ) \n"
"#define BIAS 1.72537\n"
"groupshared int gData0; \n"
"groupshared int2 gData1; \n"
"groupshared float gData2; \n"
"groupshared float4 gData3; \n"
"groupshared TGSMstruct gData4; \n"
"groupshared int gData5[2]; \n"
"groupshared int2 gData6[2]; \n"
"groupshared float gData7[2]; \n"
"groupshared float4 gData8[2]; \n"
"groupshared TGSMstruct gData9[2]; \n"
"[numthreads( NUM_THREADS_X, NUM_THREADS_Y, NUM_THREADS_Z )] \n"
"void CSMain( uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID,  uint GFid: SV_GroupIndex) \n"
"{ \n"
"	uint gidx = Gid.z * ( DISPATCH_X * DISPATCH_Y ) + Gid.y * DISPATCH_X + Gid.x;	\n"	
"	if ( GFid == 0)\n"
"	{\n"
"		gData0 = asuint(0 + BIAS); \n"
"	}		\n"
"	if ( GFid == 1)\n"
"	{\n"
"		gData1.x = asint(1 + BIAS); \n"
"		gData1.y = asint(1 + BIAS * 2); \n"
"	} 		\n"
"	if ( GFid == 2)\n"
"	{\n"
"		gData2 = asfloat(2 + BIAS); \n"
"	}		\n"
"	if ( GFid == 3)\n"
"	{\n"
"		gData3.x = asfloat(3 + BIAS); \n"
"		gData3.y = asfloat(3 + BIAS * 2); \n"
"		gData3.z = asfloat(3 + BIAS * 3); \n"
"		gData3.w = asfloat(3 + BIAS * 4); \n"
"	}		\n"
"	if ( GFid == 4)\n"
"	{\n"
"		gData4.intData1 = asint(4 + BIAS); \n"
"		gData4.intData2.x = asint(4 + BIAS * 2); \n"
"		gData4.intData2.y = asint(4 + BIAS * 3); \n"
"		gData4.floatData3[0] = asfloat(4 + BIAS * 4); \n"
"		gData4.floatData3[1] = asfloat(4 + BIAS * 5); \n"
"		gData4.floatData3[2] = asfloat(4 + BIAS * 6); \n"
"		gData4.floatData4.x = asfloat(4 + BIAS * 7); \n"
"		gData4.floatData4.y = asfloat(4 + BIAS * 8); \n"
"		gData4.floatData4.z = asfloat(4 + BIAS * 9); \n"
"		gData4.floatData4.w = asfloat(4 + BIAS * 10); \n"
"	}	\n"		
"	GroupMemoryBarrierWithGroupSync(); \n"
"	if ( (groupthreads -1 - GFid) == 0)\n"
"	{ \n"
"		gData5[0] = gData0; \n"
"		gData5[1] = gData0;\n"
"	}		\n"
"	if ( (groupthreads -1 - GFid) == 1)\n"
"	{\n"
"		gData6[0] = gData1; \n"
"		gData6[1] = gData1;\n"
"	} 		\n"
"	if ( (groupthreads -1 - GFid) == 2)\n"
"	{\n"
"		gData7[0] = gData2; \n"
"		gData7[1] = gData2; \n"
"	}		\n"
"	if ( (groupthreads -1 - GFid) == 3)\n"
"	{\n"
"		gData8[0] = gData3; \n"
"		gData8[1] = gData3;\n"
"	}\n"
"	if ( (groupthreads -1 - GFid) == 4)\n"
"	{\n"
"		gData9[0] = gData4; \n"
"		gData9[1] = gData4;\n"
"	}		 	\n"
"	GroupMemoryBarrierWithGroupSync(); 	\n"
"	if (GFid == 0) \n"
"	{ \n"
"		struct output Op; \n"
"		Op.data = 0.0; \n"
"		Op.data += asfloat(gData5[0]) + \n"
"		asfloat(gData6[0].x) + \n"
"		asfloat(gData6[1].y) + \n"
"		gData7[0] + \n"
"		gData8[1].x + \n"
"		gData8[0].y + \n"
"		gData8[1].z + \n"
"		gData8[0].w + \n"
"		asfloat(gData9[0].intData1) + \n"
"		asfloat((gData9[0].intData2).x) + \n"
"		asfloat((gData9[0].intData2).y) + \n"
"		gData9[1].floatData3[0] + \n"
"		gData9[1].floatData3[1] + \n"
"		gData9[1].floatData3[2] + \n"
"		gData9[0].floatData4.x + \n"
"		gData9[0].floatData4.y + \n"
"		gData9[0].floatData4.z + \n"
"		gData9[0].floatData4.w; \n"	
"		Result[gidx] = Op; 		\n"
"	} \n"
"} \n";
//------------------------------------------------------------------------------------------------
TEST_RESULT CTGSM11Test::SetupShader()
{
	TEST_RESULT tr= RESULT_PASS;
	HRESULT hr;

	tstring TGSMSizeStr = FormatString( "%i", m_TGSMUnitSize );
	tstring numThreadsXStr = FormatString( "%i", m_numThreadsX );
	tstring numThreadsYStr = FormatString( "%i", m_numThreadsY );
	tstring numThreadsZStr = FormatString( "%i", m_numThreadsZ );
	tstring dispatchXStr = FormatString( "%i", m_dispatchX );
	tstring dispatchYStr = FormatString( "%i", m_dispatchY );
	tstring dispatchZStr = FormatString( "%i", m_dispatchZ );
	tstring cBufSizeStr = FormatString( "%i", CBSIZE/16 );
	tstring numTGSMDclsStr = FormatString( "%i", m_numTGSMDcls );
	tstring TGSMArraySizeStr = FormatString( "%i", m_TGSMArraySize );

	D3D10_SHADER_MACRO macroSet[] =
	{
		{ "MACRO_SET", "true" },
		{ "NUM_TGSM_DATA", TGSMSizeStr.c_str() },
		{ "NUM_THREADS_X", numThreadsXStr.c_str() },
		{ "NUM_THREADS_Y", numThreadsYStr.c_str() },
		{ "NUM_THREADS_Z", numThreadsZStr.c_str() },
		{ "DISPATCH_X", dispatchXStr.c_str() },
		{ "DISPATCH_Y", dispatchYStr.c_str() },
		{ "DISPATCH_Z", dispatchZStr.c_str() },
		{ "TYPE", m_dataType.c_str() },
		{ "CBUFFERSIZE", cBufSizeStr.c_str() },
		{ "NUM_TGSM_DCLS",  numTGSMDclsStr.c_str()},
		{ "ARRAY_SIZE",  TGSMArraySizeStr.c_str() },
		NULL
	};

	tstring shaderStr;
	// special scenario for mixed decls of TGSM, including raw and structured TGSM decls and different data types
	if (m_TGSMDclScenarios == _T("MixedTGSMs"))
	{
		shaderStr = g_shaderMixedDcls;
	}
	else
	{
		shaderStr = g_shaderHeadStr1;
		tstring TGSMDcl;
		// generating multiple TGSM Dcls
		for (UINT i = 0; i < m_numTGSMDcls; i++)
		{
			if ( m_TGSMDclScenarios == _T("UniformRawTGSMs"))
			{
				TGSMDcl += FormatString(_T("groupshared TGSMstruct gData%i; \n"), i);
			}
			else if (m_TGSMDclScenarios == _T("UniformStructTGSMs") )
			{
				TGSMDcl += FormatString(_T("groupshared TGSMstruct gData%i[%i]; \n"), i, m_TGSMArraySize);
			}
		}
		shaderStr += TGSMDcl;
		shaderStr += g_shaderHeadStr2;

		tstring dataBlockStr;
		UINT numDataBlocks = 0;
		if ( m_TGSMDclScenarios == _T("UniformRawTGSMs") )
		{
				dataBlockStr += FormatString(_T("uint numDataBlocks = NUM_TGSM_DATA; \n"));
				numDataBlocks = m_TGSMUnitSize;
		}
		else if ( m_TGSMDclScenarios == _T("UniformStructTGSMs"))
		{
				dataBlockStr += FormatString(_T("uint numDataBlocks = ARRAY_SIZE; \n"));
				numDataBlocks = m_TGSMArraySize;
		}
		shaderStr += dataBlockStr;

		if (  m_TGSMDclScenarios == _T("UniformRawTGSMs") || m_TGSMDclScenarios == _T("UniformStructTGSMs") )
		{
			shaderStr += g_shaderBodyPart1;
			tstring fillDataStr;
			for (UINT i = 0; i < m_numTGSMDcls; i++)
			{
				fillDataStr += FormatString(_T("case %d: \n"), i);
				if ( m_TGSMDclScenarios == _T("UniformRawTGSMs") )
					fillDataStr += FormatString(_T("gData%d.data[i] = Cbuf[DataIndex]; \n"), i );
				else if ( m_TGSMDclScenarios == _T("UniformStructTGSMs") )
					fillDataStr += FormatString(_T("gData%d[i].data[NUM_TGSM_DATA - 1] = Cbuf[DataIndex]; \n"), i );
				fillDataStr += _T("break; \n");
			}
			shaderStr += fillDataStr;
			shaderStr += g_shaderBodyPart2;
			
			if ( numDataBlocks >= m_numTGSMDcls)
			{
				shaderStr += g_biggerNumDataPart1;
				tstring dataAccessStr1;
				if ( m_numTGSMDcls > 1)
				{
					for (UINT i = 1; i < m_numTGSMDcls; i++)
					{
						if ( m_TGSMDclScenarios == _T("UniformRawTGSMs") )
							dataAccessStr1 += FormatString(_T("gData0.data[sumIndex] += gData%d.data[sumIndex]; \n"), i);
						else if ( m_TGSMDclScenarios == _T("UniformStructTGSMs") )
							dataAccessStr1 += FormatString(
							_T("gData0[sumIndex].data[NUM_TGSM_DATA - 1] += gData%d[sumIndex].data[NUM_TGSM_DATA - 1]; \n"), i);
					}
				}
				else
					dataAccessStr1 += FormatString(_T("; \n"));

				shaderStr += dataAccessStr1;
				shaderStr += g_biggerNumDataPart2;
				tstring dataAccessStr2;

				if ( m_TGSMDclScenarios == _T("UniformRawTGSMs") )
					dataAccessStr2 += _T("Op.data += gData0.data[i]; \n");
				else if ( m_TGSMDclScenarios == _T("UniformStructTGSMs") )
					dataAccessStr2 += _T("Op.data += gData0[i].data[NUM_TGSM_DATA - 1]; \n");

				shaderStr += dataAccessStr2;

			}
			else 
			{
				shaderStr += g_biggerNumDclsPart1;
				tstring dataAccessStr1;
				for (UINT i = 0; i < m_numTGSMDcls; i++)
				{
					dataAccessStr1 += FormatString(_T("case %d: \n"), i);
					dataAccessStr1 += FormatString(_T("for(uint j = 1; j < numDataBlocks; j++) \n"));
					if ( m_TGSMDclScenarios == _T("UniformRawTGSMs") )
						dataAccessStr1 += FormatString(_T("gData%d.data[0] += gData%d.data[j]; \n"), i, i );
					else if ( m_TGSMDclScenarios == _T("UniformStructTGSMs") )
						dataAccessStr1 += FormatString(_T("gData%d[0].data[NUM_TGSM_DATA - 1] += gData%d[j].data[NUM_TGSM_DATA - 1]; \n"), i, i );
					dataAccessStr1 += _T("break; \n");
				}
				shaderStr += dataAccessStr1;
				shaderStr += g_biggerNumDclsPart2;

				tstring dataAccessStr2;
				for (UINT i = 0; i < m_numTGSMDcls; i++)
				{
					if ( m_TGSMDclScenarios == _T("UniformRawTGSMs") )
						dataAccessStr2 += FormatString(_T("Op.data += gData%d.data[0]; \n"), i);
					else if ( m_TGSMDclScenarios == _T("UniformStructTGSMs") )
						dataAccessStr2 += FormatString(_T("Op.data += gData%d[0].data[NUM_TGSM_DATA - 1]; \n"), i);
				}
				shaderStr += dataAccessStr2;
			}
		}
		
		shaderStr += g_shaderEndStr;
	}

	hr = CompileAndCreateComputeShaderFromMemory( 
		shaderStr.c_str(), 
		strlen(shaderStr.c_str()), 
		_T("CSMain"), 
		D3D_FEATURE_LEVEL_11_0, 
		&m_pCS,
		macroSet
		);

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CTGSM11Test::SetupShader(): CompileAndCreateComputeShaderFromMemory() failed. hr=%s\n" ), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto shader_done;
	}

shader_done:

	return tr;
}

//------------------------------------------------------------------------------------------------
bool CTGSM11Test::GetExpectedResult( UINT* pUintResult, float* pFloatResult, double* pDoubleResult)
{
	if ( m_dataType == _T("float") && !pFloatResult)
		return false;
	if ( m_dataType == _T("uint") && !pUintResult)
		return false;
	if ( m_dataType == _T("double") && !pDoubleResult)
		return false;
	if ( m_TGSMDclScenarios == _T("MixedTGSMs") && !pFloatResult)
		return false;

	UINT numDataBlocks = 0;
	if ( m_TGSMDclScenarios == _T("UniformRawTGSMs") )
		numDataBlocks = m_TGSMUnitSize;
	else if ( m_TGSMDclScenarios == _T("UniformStructTGSMs") )
		numDataBlocks = m_TGSMArraySize;

	UINT groupThreads = m_numThreadsX * m_numThreadsY * m_numThreadsZ;
	UINT numDataPerThread = (m_numTGSMDcls * numDataBlocks )/groupThreads;
	if (m_TGSMDclScenarios == _T("MixedTGSMs"))
	{
		float Bias = (float)1.72537; // defined in the shader string
		int intData = (int)Bias + (int)(1 + Bias) + (int)(1 + Bias * 2) 
			+ (int)(4 + Bias) + (int)(4 + Bias * 2) + (int)(4 + Bias * 3);

		float floatData = (float)(2 + Bias) + (float)(3 + Bias) + (float)(3 + Bias * 2)
			+ (float)(3 + Bias * 3) + (float)(3 + Bias * 4) + (float)(4 + Bias * 4) 
			+ (float)(4 + Bias * 5) + (float)(4 + Bias * 6) + (float)(4 + Bias * 7)
			+ (float)(4 + Bias * 8) + (float)(4 + Bias * 9) + (float)(4 + Bias * 10);

		*pFloatResult = (float)( intData + floatData);
		return true;
	}
	else
	{
		if ( m_dataType == _T("float"))
		{
			float *TGSMData = new float[m_numTGSMDcls * numDataBlocks];
			if (!TGSMData)
				return false;
			for (UINT j = 0; j < m_numTGSMDcls; j++)
			{
				for (UINT i = 0; i < numDataBlocks; i++)
				{
					UINT DataIndex = ( numDataBlocks * j + i ) % ( CBSIZE/16);
					TGSMData[j * numDataBlocks + i] = g_floatDataPat[DataIndex];
				}
			}
			float resFloat = 0.0;
			
			if ( m_TGSMDclScenarios == _T("UniformRawTGSMs") || m_TGSMDclScenarios == _T("UniformStructTGSMs"))
			{
				for (UINT j = 0; j < m_numTGSMDcls; j++)
				{
					for (UINT i = 0; i < numDataBlocks; i++)
					{
						resFloat += TGSMData[j * numDataBlocks + i];
					}
				}
			}
			*pFloatResult = resFloat;
			delete[] TGSMData;
			return true;
		}
		else if ( m_dataType == _T("uint") )
		{
			UINT *TGSMData = new UINT[m_numTGSMDcls * numDataBlocks];
			if (!TGSMData)
				return false;
			for (UINT j = 0; j < m_numTGSMDcls; j++)
			{
				for (UINT i = 0; i < numDataBlocks; i++)
				{
					UINT DataIndex = ( numDataBlocks * j + i ) % ( CBSIZE/16);
					TGSMData[j * numDataBlocks + i] = g_uintDataPat[DataIndex];
				}
			}
			UINT resUint = 0;
			if (m_TGSMDclScenarios == _T("UniformRawTGSMs") || m_TGSMDclScenarios == _T("UniformStructTGSMs"))
			{
				for (UINT j = 0; j < m_numTGSMDcls; j++)
				{
					for (UINT i = 0; i < numDataBlocks; i++)
					{
						resUint += TGSMData[j * numDataBlocks + i];
					}
				}
			}
			*pUintResult = resUint;
			delete[] TGSMData;
			return true;
		}
		else if ( m_dataType == _T("double") )
		{
			double *TGSMData = new double[m_numTGSMDcls * numDataBlocks];
			if (!TGSMData)
				return false;
			for (UINT j = 0; j < m_numTGSMDcls; j++)
			{
				for (UINT i = 0; i < numDataBlocks; i++)
				{
					UINT DataIndex = ( numDataBlocks * j + i ) % ( CBSIZE/16);
					TGSMData[j * numDataBlocks + i] = g_uintDataPat[DataIndex];
				}
			}
			double resDouble = 0.0;
			if (m_TGSMDclScenarios == _T("UniformRawTGSMs") || m_TGSMDclScenarios == _T("UniformStructTGSMs"))
			{
				for (UINT j = 0; j < m_numTGSMDcls; j++)
				{
					for (UINT i = 0; i < numDataBlocks; i++)
					{
						resDouble += TGSMData[j * numDataBlocks + i];
					}
				}
			}

			*pDoubleResult = resDouble;
			delete[] TGSMData;
			return true;
		}
	}

	return false;

}