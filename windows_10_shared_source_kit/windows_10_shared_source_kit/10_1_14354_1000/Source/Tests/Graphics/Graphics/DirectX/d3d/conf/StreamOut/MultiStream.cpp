#include "MultiStream.h"

static MULTI_STREAM_DECL_MODE DeclModeTable[] = 
{
	ONE_STREAM_FOUR_BUFFER,
	FOUR_STREAM_FOUR_BUFFER,
	TWO_STREAM_TWO_BUFFER,
};

BEGIN_NAMED_VALUES( MULTI_STREAM_DECL_MODE )
	NAMED_VALUE( _T( "OneStreamFourBuffer" ),   ONE_STREAM_FOUR_BUFFER )
	NAMED_VALUE( _T( "TwoStreamTwoBuffer" ),    TWO_STREAM_TWO_BUFFER )
	NAMED_VALUE( _T( "ThreeStreamFourBuffer" ), THREE_STREAM_FOUR_BUFFER )
	NAMED_VALUE( _T( "FourStreamFourBuffer" ),  FOUR_STREAM_FOUR_BUFFER )
END_NAMED_VALUES( MULTI_STREAM_DECL_MODE )

static const float g_clearRTVColor = 999;

static D3D11_SO_DECLARATION_ENTRY s_soDeclOneStreamFourBuffer[] =
{
	{ 0, "SV_Position", 0, 0, 4, 0 },
	{ 0, "data", 0, 0, 1, 1 }, // split
	{ 0, "data", 0, 1, 1, 2 }, // split
	{ 0, "data", 1, 0, 1, 3 }, // split
	{ 0,  NULL,  0, 0, 4, 3 }, // gap
	{ 0, "data", 1, 1, 1, 3 }, // split
	{ 0, "data", 2, 0, 4, 0 },
	{ 0, "data", 3, 0, 4, 1 },
	{ 0, "data", 4, 0, 4, 2 },
	{ 0, "data", 5, 0, 4, 3 },
	{ 0, "data", 6, 0, 4, 0 },
	{ 0, "data", 7, 0, 4, 1 },
	{ 0, "data", 8, 0, 4, 2 },
	{ 0, "data", 9, 0, 4, 3 },
	{ 0, "data", 10, 0, 3, 0 }, // split
	{ 0, "data", 10, 3, 1, 1 }, // split
	{ 0, "data", 11, 0, 4, 1 },
	{ 0, "data", 12, 0, 4, 2 },
	{ 0, "data", 13, 0, 4, 3 },
	{ 0, "data", 14, 0, 4, 3 },
	{ 0, "data", 15, 0, 4, 2 },
	{ 0, "data", 16, 0, 4, 1 },
	{ 0, "data", 17, 0, 4, 0 },
	{ 0, "data", 18, 0, 4, 0 },
	{ 0,  NULL,  0, 0,  4, 0 }, // gap
	{ 0, "data", 20, 0, 4, 0 },
	{ 0, "data", 21, 0, 1, 0 }, // split
	{ 0, "data", 21, 1, 3, 1 }, // split
	{ 0, "data", 22, 0, 4, 1 },
	{ 0, "data", 23, 0, 4, 1 },
	{ 0, "data", 24, 0, 4, 1 },
	{ 0,  NULL,  0, 0,  4, 2 }, // gap
	{ 0, "data", 25, 0, 4, 2 },
	{ 0, "data", 26, 0, 4, 2 },
	{ 0, "data", 27, 0, 4, 2 },
	{ 0, "data", 28, 0, 4, 3 },
	{ 0, "data", 29, 0, 4, 3 },
	{ 0, "data", 30, 0, 4, 3 },
};

static D3D11_SO_DECLARATION_ENTRY s_soDeclTwoStreamTwoBuffer[] =
{
	{ 0, "SV_Position", 0, 0, 4, 0 },
	{ 0, "data", 0, 0, 1, 0 }, // split
	{ 0, "data", 0, 1, 1, 0 }, // split
	{ 0, "data", 1, 0, 1, 0 }, // split
	{ 0,  NULL,  0, 0, 4, 0 }, // gap
	{ 0, "data", 1, 1, 1, 0 }, // split
	{ 0, "data", 2, 0, 4, 0 },
	{ 0, "data", 3, 0, 4, 0 },
	{ 0, "data", 4, 0, 4, 0 },
	{ 0, "data", 5, 0, 4, 0 },
	{ 0, "data", 6, 0, 4, 0 },
	{ 0, "data", 7, 0, 4, 0 },
	{ 0, "data", 8, 0, 4, 0 },
	{ 0, "data", 9, 0, 4, 0 },
	{ 0, "data", 10, 0, 4, 0 },
	{ 0, "data", 11, 0, 4, 0 },
	{ 1, "data", 12, 0, 4, 1 },
	{ 1, "data", 13, 0, 4, 1 },
	{ 1, "data", 14, 0, 4, 1 },
	{ 1, "data", 15, 0, 4, 1 },
	{ 1,  NULL,  0,  0, 4, 1 }, // gap
	{ 1, "data", 17, 0, 4, 1 },
	{ 1, "data", 18, 0, 4, 1 },
	{ 1, "data", 19, 0, 1, 1 }, // split
	{ 1, "data", 19, 1, 1, 1 }, // split
	{ 1, "data", 19, 2, 1, 1 }, // split
	{ 1, "data", 19, 3, 1, 1 }, // split
	{ 1, "data", 20, 0, 4, 1 },
	{ 1, "data", 21, 0, 4, 1 },
	{ 1, "data", 22, 0, 4, 1 },
	{ 1, "data", 23, 0, 4, 1 },
	{ 1, "data", 24, 0, 4, 1 },
	{ 1,  NULL,  0,  0, 1, 1 }, // gap
	{ 1, "data", 25, 0, 4, 1 },
	{ 1, "data", 26, 0, 4, 1 },
	{ 1, "data", 27, 0, 4, 1 },
	{ 1, "data", 28, 0, 4, 1 },
	{ 1, "data", 29, 0, 4, 1 },
	{ 1, "data", 30, 0, 4, 1 },
};

static D3D11_SO_DECLARATION_ENTRY s_soDeclThreeStreamFourBuffer[] =
{
	{ 0, "SV_Position", 0, 0, 4, 0 },
	{ 1, "data", 0, 0, 1, 1 },
	{ 1,  NULL,  0, 0, 4, 1 },
	{ 2, "data", 0, 1, 1, 2 },
	{ 2, "data", 1, 0, 1, 3 },
	{ 2,  NULL,  0, 0, 3, 3 },
	{ 2, "data", 1, 1, 1, 3 },

	{ 2, "data", 2, 0, 4, 2 },
	{ 2, "data", 3, 0, 4, 3 },
	{ 2, "data", 4, 0, 4, 2 },
	{ 2, "data", 5, 0, 4, 3 },
	{ 2, "data", 6, 0, 4, 2 },
	{ 2, "data", 7, 0, 4, 2 },
	{ 2, "data", 8, 0, 4, 3 },
	{ 2, "data", 9, 0, 4, 2 },
	{ 2, "data", 10, 0, 4, 3 },
	{ 2, "data", 11, 0, 4, 2 },
	{ 2, "data", 12, 0, 4, 3 },
	{ 2, "data", 13, 0, 4, 2 },
	{ 2, "data", 14, 0, 4, 3 },
	{ 2, "data", 15, 0, 4, 2 },
	{ 2, "data", 16, 0, 4, 3 },
	{ 2, "data", 17, 0, 4, 2 },
	{ 2, "data", 18, 0, 4, 3 },
	{ 2, "data", 19, 0, 4, 2 },
	{ 2, "data", 20, 0, 2, 3 }, // split 
	{ 2, "data", 20, 2, 2, 2 }, // split
	{ 2, "data", 21, 0, 4, 2 },
	{ 2, "data", 22, 0, 4, 3 },
	{ 2, "data", 23, 0, 4, 2 },
	{ 2, "data", 24, 0, 4, 3 },
	{ 2, "data", 25, 0, 4, 2 },
	{ 2, "data", 26, 0, 4, 3 },
	{ 2,  NULL,  0,  0, 2, 3 }, // gap
	{ 2, "data", 27, 0, 4, 2 },
	{ 2, "data", 28, 0, 4, 3 },
	{ 2, "data", 29, 0, 4, 2 },
	{ 2, "data", 30, 0, 4, 3 },
};

static D3D11_SO_DECLARATION_ENTRY s_soDeclFourStreamFourBuffer[] =
{
	{ 0, "SV_Position", 0, 0, 4, 3 },
	{ 0, "data", 0, 0, 1, 3 }, // split
	{ 1, "data", 0, 1, 1, 2 }, // split
	{ 2, "data", 1, 0, 1, 1 }, // split
	{ 2,  NULL,  0, 0, 4, 1 }, // gap
	{ 2, "data", 1, 1, 1, 1 }, // split

	{ 2, "data", 2, 0, 4, 1 },
	{ 2, "data", 3, 0, 4, 1 },
	{ 2, "data", 4, 0, 4, 1 },
	{ 2, "data", 5, 0, 4, 1 },
	{ 2, "data", 6, 0, 4, 1 },
	{ 2, "data", 7, 0, 4, 1 },
	{ 2, "data", 8, 0, 4, 1 },
	{ 2, "data", 9, 0, 4, 1 },
	{ 2, "data", 10, 0, 4, 1 },
	{ 2, "data", 11, 0, 4, 1 },
	{ 2, "data", 12, 0, 4, 1 },
	{ 3, "data", 13, 0, 4, 0 },
	{ 3, "data", 14, 0, 4, 0 },
	{ 3, "data", 15, 0, 4, 0 },
	{ 3, "data", 16, 0, 4, 0 },
	{ 3, "data", 17, 0, 4, 0 },
	{ 3, "data", 18, 0, 4, 0 },
	{ 3, "data", 19, 0, 4, 0 },
	{ 3, "data", 20, 0, 4, 0 },
	{ 3, "data", 21, 0, 4, 0 },
	{ 3, "data", 22, 0, 4, 0 },
	{ 3, "data", 23, 0, 4, 0 },
	{ 3, "data", 24, 0, 4, 0 },
	{ 3, "data", 25, 0, 4, 0 },
	{ 3,  NULL,  0,  0, 4, 0 }, // gap
	{ 3, "data", 26, 0, 4, 0 },
	{ 3, "data", 27, 0, 4, 0 },
	{ 3, "data", 28, 0, 4, 0 },
	{ 3, "data", 29, 0, 1, 0 }, // split
	{ 3, "data", 29, 1, 3, 0 }, // split
	{ 3, "data", 30, 0, 4, 0 },
};

//////////////////////////////////////////////////////////////////////////
// CStreamOutMultiStream
//////////////////////////////////////////////////////////////////////////
CStreamOutMultiStream::CStreamOutMultiStream() :
	m_numInputVerts( 4 ),
	m_numShaderLoopIterations( 0 ),
	m_numDrawCalls( 0 ),
	m_declMode( ONE_STREAM_FOUR_BUFFER ),
	m_pRTTexture2D( NULL ),
	m_pRTTexture2DStaging( NULL ),
	m_pRTV( NULL ),
	m_numExpectedPixelsWritten( 0 ),
	m_maxNumPixelsWritten( 0 ),
	m_pPixelShouldBeOutput( NULL ),
	m_pExpectedPixelResult( NULL )
{
	m_sm = SHADER_MODEL_5_0;
	m_useNullGS = false;
	m_gsOutputPrimitiveType = "Points";
	m_inputTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	ZeroMemory( m_stridePadding, sizeof(m_stridePadding) );
	ZeroMemory( m_soVertexStride, sizeof( m_soVertexStride ) );
}

//////////////////////////////////////////////////////////////////////////
static UINT s_maxVertexCountTable[] =
{
	24,
	6
};

// Ensure that GSOUT128 really has 128 components
C_ASSERT( (sizeof(UINT) * 128) == sizeof( CStreamOutMultiStream::GSOUT128 ) );

void CStreamOutMultiStream::InitTestParameters()
{
	UINT bufferSize6Verts  = sizeof( CStreamOutMultiStream::GSOUT128 ) * 6;
	UINT bufferSize12Verts = sizeof( CStreamOutMultiStream::GSOUT128 ) * 12;

	AddParameter<UINT>                    ( "NumInputVerts",            &m_numInputVerts );         
	AddParameter<UINT>                    ( "ShaderLoopCount",          &m_numShaderLoopIterations );
	AddParameter<UINT>                    ( "GSMaxVertexCount",         &m_gsMaxVertexCount ); 
	AddParameter<UINT>                    ( "NumDrawCalls",             &m_numDrawCalls );        
	AddParameter<MULTI_STREAM_DECL_MODE>  ( "DeclMode",                 &m_declMode );
	AddParameter<UINT>                    ( "SOOffset0",                &m_soOffsets[0] );
	AddParameter<UINT>                    ( "SOBufferSize0",            &m_soBufferSize[0] );
	AddParameter<UINT>                    ( "StridePadding0",           &m_stridePadding[0] );
	AddParameter<UINT>                    ( "SOOffset1",                &m_soOffsets[1] );         
	AddParameter<UINT>                    ( "SOBufferSize1",            &m_soBufferSize[1] );
	AddParameter<UINT>                    ( "StridePadding1",           &m_stridePadding[1] );
	AddParameter<UINT>                    ( "SOOffset2",                &m_soOffsets[2] );         
	AddParameter<UINT>                    ( "SOBufferSize2",            &m_soBufferSize[2] );
	AddParameter<UINT>                    ( "StridePadding2",           &m_stridePadding[2] );
	AddParameter<UINT>                    ( "SOOffset3",                &m_soOffsets[3] );         
	AddParameter<UINT>                    ( "SOBufferSize3",            &m_soBufferSize[3] );
	AddParameter<UINT>                    ( "StridePadding3",           &m_stridePadding[3] );
	AddParameter<bool>                    ( "RuntimeCalculatesSOStrides", &m_haveRuntimeCalculateSOStrides );
	CTestCaseParameter<UINT> *pRasterizeStreamParam = AddParameter<UINT>( "RasterizeStream", &m_rasterizeStream );
	CTestCaseParameter<SHADER_MODEL> *pShaderModelParam = AddParameter<SHADER_MODEL>( "ShaderModel", &m_sm );
	
	
	SetParameterDefaultValue<UINT>                    ( "NumInputVerts",    s_pointsSize );
	SetParameterDefaultValue<UINT>                    ( "ShaderLoopCount",  2 );
	SetParameterDefaultValue<UINT>                    ( "GSMaxVertexCount", 2 );
	SetParameterDefaultValue<UINT>                    ( "NumDrawCalls",     1 );
	SetParameterDefaultValue<MULTI_STREAM_DECL_MODE>  ( "DeclMode",         ONE_STREAM_FOUR_BUFFER );
	SetParameterDefaultValue<UINT>                    ( "RasterizeStream",  0 );
	SetParameterDefaultValue<UINT>                    ( "SOOffset0",        0 );
	SetParameterDefaultValue<UINT>                    ( "SOBufferSize0",    bufferSize12Verts );
	SetParameterDefaultValue<UINT>                    ( "StridePadding0",   0 );
	SetParameterDefaultValue<UINT>                    ( "SOOffset1",        0 );
	SetParameterDefaultValue<UINT>                    ( "SOBufferSize1",    bufferSize12Verts );
	SetParameterDefaultValue<UINT>                    ( "StridePadding1",   0 );
	SetParameterDefaultValue<UINT>                    ( "SOOffset2",        0 );
	SetParameterDefaultValue<UINT>                    ( "SOBufferSize2",    bufferSize12Verts );
	SetParameterDefaultValue<UINT>                    ( "StridePadding2",   0 );
	SetParameterDefaultValue<UINT>                    ( "SOOffset3",        0 );
	SetParameterDefaultValue<UINT>                    ( "SOBufferSize3",    bufferSize12Verts );
	SetParameterDefaultValue<UINT>                    ( "StridePadding3",   0 );
	SetParameterDefaultValue<SHADER_MODEL>            ( "ShaderModel",		SHADER_MODEL_5_0 );
	SetParameterDefaultValue<bool>					  ( "RuntimeCalculatesSOStrides", false );
	
	testfactor::RFactor runtimeCalculatesSOStridesFalse = AddParameterValue<bool>( "RuntimeCalculatesSOStrides", false );
	testfactor::RFactor runtimeCalculatesSOStridesTrue = AddParameterValue<bool>( "RuntimeCalculatesSOStrides", true );

	testfactor::RFactor numInputVerts				= AddParameterValueSet<UINT>( "NumInputVerts", new CRangeValueSet<UINT>( 1, s_pointsSize, 1 ) );
	testfactor::RFactor numPrimsPerInputVertex		= AddParameterValueSet<UINT>( "ShaderLoopCount", new CRangeValueSet<UINT>( 1, 10, 3 ) );
	testfactor::RFactor gsMaxVertexCount			= AddParameterValue<UINT>( "GSMaxVertexCount", 8) + AddParameterValue<UINT>( "GSMaxVertexCount", 6);
	testfactor::RFactor numDrawCalls				= AddParameterValue<UINT>( "NumDrawCalls", 1 ) + AddParameterValue<UINT>( "NumDrawCalls", 3 );

	testfactor::RFactor buffer0SizeOffset = (AddParameterValue<UINT>( "SOOffset0", 0 ) + AddParameterValue<UINT>( "SOOffset0", 16 ) ) * 
											(AddParameterValue<UINT>( "SOBufferSize0", bufferSize6Verts ) + AddParameterValue<UINT>( "SOBufferSize0", bufferSize12Verts )) *
											(runtimeCalculatesSOStridesTrue + (runtimeCalculatesSOStridesFalse * (AddParameterValue<UINT>( "StridePadding0", 0) + AddParameterValue<UINT>( "StridePadding0", 4))));

	testfactor::RFactor buffer1SizeOffset = (AddParameterValue<UINT>( "SOOffset1", 0 ) + AddParameterValue<UINT>( "SOOffset1", 16 ) ) * 
											(AddParameterValue<UINT>( "SOBufferSize1", bufferSize6Verts ) + AddParameterValue<UINT>( "SOBufferSize1", bufferSize12Verts )) *
											(runtimeCalculatesSOStridesTrue + (runtimeCalculatesSOStridesFalse * (AddParameterValue<UINT>( "StridePadding1", 0) + AddParameterValue<UINT>( "StridePadding1", 12))));

	testfactor::RFactor buffer2SizeOffset = (AddParameterValue<UINT>( "SOOffset2", 0 ) + AddParameterValue<UINT>( "SOOffset2", 16 ) ) * 
											(AddParameterValue<UINT>( "SOBufferSize2", bufferSize6Verts ) + AddParameterValue<UINT>( "SOBufferSize2", bufferSize12Verts )) * 
											(runtimeCalculatesSOStridesTrue + (runtimeCalculatesSOStridesFalse * (AddParameterValue<UINT>( "StridePadding2", 0) + AddParameterValue<UINT>( "StridePadding2", 8))));

	testfactor::RFactor buffer3SizeOffset = (AddParameterValue<UINT>( "SOOffset3", 0 ) + AddParameterValue<UINT>( "SOOffset3", 16 ) ) * 
											(AddParameterValue<UINT>( "SOBufferSize3", bufferSize6Verts ) + AddParameterValue<UINT>( "SOBufferSize3", bufferSize12Verts )) * 
											(runtimeCalculatesSOStridesTrue + (runtimeCalculatesSOStridesFalse * (AddParameterValue<UINT>( "StridePadding3", 0) + AddParameterValue<UINT>( "StridePadding3", 16))));

	testfactor::RFactor oneStreamFourBuffer   = AddParameterValue<MULTI_STREAM_DECL_MODE>( "DeclMode", ONE_STREAM_FOUR_BUFFER )  * (buffer0SizeOffset + buffer1SizeOffset + buffer2SizeOffset + buffer3SizeOffset);
	testfactor::RFactor twoStreamTwoBuffer    = AddParameterValue<MULTI_STREAM_DECL_MODE>( "DeclMode", TWO_STREAM_TWO_BUFFER )   * (buffer0SizeOffset + buffer1SizeOffset);
	testfactor::RFactor threeStreamFourBuffer = AddParameterValue<MULTI_STREAM_DECL_MODE>( "DeclMode", THREE_STREAM_FOUR_BUFFER )* (buffer0SizeOffset + buffer1SizeOffset + buffer2SizeOffset + buffer3SizeOffset);
	testfactor::RFactor fourStreamFourBuffer  = AddParameterValue<MULTI_STREAM_DECL_MODE>( "DeclMode", FOUR_STREAM_FOUR_BUFFER ) * (buffer0SizeOffset + buffer1SizeOffset + buffer2SizeOffset + buffer3SizeOffset);

	// Ensure that the rasterize stream is always either D3D11_SO_NO_RASTERIZED_STREAM or a stream that is output by the GS
	testfactor::RFactor noRasterizeStream = AddParameterValue< UINT >( pRasterizeStreamParam, D3D11_SO_NO_RASTERIZED_STREAM );

	testfactor::RFactor rasterizeStream_0 = noRasterizeStream + 
											AddParameterValue< UINT >( pRasterizeStreamParam, 0 );

	testfactor::RFactor rasterizeStream_01 = noRasterizeStream + 
											 AddParameterValue< UINT >( pRasterizeStreamParam, 0 ) + 
											 AddParameterValue< UINT >( pRasterizeStreamParam, 1);

	testfactor::RFactor rasterizeStream_012 = noRasterizeStream + 
											 AddParameterValue< UINT >( pRasterizeStreamParam, 0 ) + 
											 AddParameterValue< UINT >( pRasterizeStreamParam, 1) +
											 AddParameterValue< UINT >( pRasterizeStreamParam, 2);

	testfactor::RFactor rasterizeStream_0123 = noRasterizeStream + 
											 AddParameterValue< UINT >( pRasterizeStreamParam, 0 ) + 
											 AddParameterValue< UINT >( pRasterizeStreamParam, 1) +
											 AddParameterValue< UINT >( pRasterizeStreamParam, 2) +
											 AddParameterValue< UINT >( pRasterizeStreamParam, 3);

	CUserValueSet<SHADER_MODEL> *pShaderModelValueSet = new CUserValueSet<SHADER_MODEL>();

	// Note: MultiStream is only run FL11
	pShaderModelValueSet->AddValue( SHADER_MODEL_4_0 );
	pShaderModelValueSet->AddValue( SHADER_MODEL_4_1 );
	pShaderModelValueSet->AddValue( SHADER_MODEL_5_0  );

	testfactor::RFactor shaderModel = AddParameterValueSet( pShaderModelParam, pShaderModelValueSet );

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{numInputVerts, 1, NULL, 0},
		{numPrimsPerInputVertex, 1, NULL, 0},
		{gsMaxVertexCount, 1, NULL, 0},
		{numDrawCalls, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	testfactor::RFactor rfPairwiseFactors = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), _T("Pairwise"), groupOrders, _countof(groupOrders));

	SetRootTestFactor
		(
			rfPairwiseFactors * 
			(( ((oneStreamFourBuffer * shaderModel) % rasterizeStream_0) + (twoStreamTwoBuffer % rasterizeStream_01) + (threeStreamFourBuffer % rasterizeStream_012) + (fourStreamFourBuffer % rasterizeStream_0123) ) )
		);

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumInputVerts"), s_pointsSize ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ShaderLoopCount"), 4 ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("GSMaxVertexCount"), 6 ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumDrawCalls"), 1 ), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<bool>(_T("RuntimeCalculatesSOStrides"), false ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SHADER_MODEL>(_T("ShaderModel"), SHADER_MODEL_4_1 ), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<MULTI_STREAM_DECL_MODE>(_T("DeclMode"), ONE_STREAM_FOUR_BUFFER ), WeightFactorSmall);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CStreamOutMultiStream::Setup()
{
	return SetupSOClearObjects();
}

void CStreamOutMultiStream::Cleanup()
{
	CleanupSOClearObjects();
}

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CStreamOutMultiStream::SetupTestCase()
{
	assert( !m_useNullGS ); // Multi-stream is only supported when there is a GS

	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;
	D3D11_TEXTURE2D_DESC textureDesc;

	//
	// Setup Output Streams
	//	
	memset( &m_soBufferDesc, 0, sizeof( m_soBufferDesc ) );
	memset( &m_soBuffers, 0, sizeof( m_soBuffers ) );

	for( UINT i = 0; i < 4; i++ )
	{
		m_soBufferDesc[i].CPUAccessFlags = 0;
		m_soBufferDesc[i].ByteWidth = m_soBufferSize[i];
		m_soBufferDesc[i].Usage = D3D11_USAGE_DEFAULT;
		m_soBufferDesc[i].MiscFlags = 0;
		m_soBufferDesc[i].BindFlags = D3D11_BIND_STREAM_OUTPUT;
	}

	tr = CreateSOBuffers( m_soBuffers, m_soBufferDesc );
	if( tr != RESULT_PASS )
		return tr;

	//
	// Setup Shaders
	//
	std::string vsName;
	std::string gsName;

	std::vector< D3D11_SO_DECLARATION_ENTRY > vSODecl;
	GetCurSODecl( &vSODecl );

	D3D11_SO_DECLARATION_ENTRY* soDecl = &(vSODecl[0]);
	UINT numEntries = vSODecl.size();

	WriteDeclToLog( soDecl, numEntries );

	FromString( &vsName, tstring( _T( "VSChangeData" ) ) );
	tr = SetupVSFromResource( "ShaderSource.sh", vsName.c_str(), NULL, 0, NULL, NULL, 0, &m_pVS, &m_pInputLayout );
		
	if( tr != RESULT_PASS )
		return tr;

	// Compute stride of each buffer
	ZeroMemory( m_soVertexStride, sizeof(m_soVertexStride) );
	for( UINT i = 0; i < numEntries; i++ )
	{
		m_soVertexStride[ soDecl[i].OutputSlot ] += (soDecl[i].ComponentCount * sizeof(UINT));
	}

	// Pad stride by a few bytes
	for( UINT i = 0; i < 4; i++ )
	{
		m_soVertexStride[i] += m_stridePadding[i];
	}

	FromString( &gsName, ComposeGSName() + tstring( _T( "x" ) ) + ToString( m_declMode ) );
	const bool multiStream = GetNumStreams() == 1 ? false : true;
	tr = SetupGSFromResource( "ShaderSource.sh", gsName.c_str(), soDecl, numEntries, NULL, m_haveRuntimeCalculateSOStrides ? NULL : m_soVertexStride, m_haveRuntimeCalculateSOStrides ? 0 : 4, multiStream, &m_pGS );
	if( tr != RESULT_PASS )
		return tr;


	// Setup the pixel shader
	tr = SetupPSFromResource( "ShaderSource.sh", "PSMain", &m_pPS );
	if( tr != RESULT_PASS )
		return tr;

	//
	// Setup render targets
	//

	// Calculate max number of pixel that could stored (including gaps from SO etc...)
	m_maxNumPixelsWritten = m_numDrawCalls * m_numInputVerts * m_numShaderLoopIterations;

	m_pPixelShouldBeOutput = new bool[m_maxNumPixelsWritten];
	m_pExpectedPixelResult = new PIXELRESULT[m_maxNumPixelsWritten];

	// Init the data to false
	for( UINT i = 0; i < m_maxNumPixelsWritten; ++i )
		m_pPixelShouldBeOutput[i] = false;

	// Calculate the number of pixels written and the expected results
	const UINT numStreams = GetNumStreams();
	if( m_rasterizeStream == D3D11_SO_NO_RASTERIZED_STREAM || m_rasterizeStream >= numStreams )
	{
		m_numExpectedPixelsWritten = 0;
	}
	else
	{
		// Reset the count
		m_numExpectedPixelsWritten = 0;
		UINT yIndex = 0;

		// Calculate the number of expected pixels to output
		for( UINT drawIndex = 0; drawIndex < m_numDrawCalls; ++drawIndex )
		{
			for( UINT vertexIndex = 0; vertexIndex < m_numInputVerts; ++vertexIndex )
			{
				// Max vertex count applies per vertex in a draw call
				UINT countVerticesAppendedToStreams = 0;

				for( UINT shaderLoopIndex = 0; shaderLoopIndex < m_numShaderLoopIterations; ++shaderLoopIndex )
				{
					// This is from the shader ("uint n = drawIndex.x + 2 * i + input[0].data")
					UINT vertexShaderOutputData = s_vbdataPoints[vertexIndex].data + 1;	// The vertex shader does an "encode"

					UINT n = drawIndex + 2 * shaderLoopIndex + vertexShaderOutputData;

					// The shader does an "Encode" 4 times
					++n;
					m_pExpectedPixelResult[yIndex].data[0] = n;
					++n;
					m_pExpectedPixelResult[yIndex].data[1] = n;
					++n;
					m_pExpectedPixelResult[yIndex].data[2] = n;
					++n;
					m_pExpectedPixelResult[yIndex].data[3] = n;

					// The shader does this check before appending to any stream
					if( 1 != (n % 5) )
					{
						for( UINT streamIndex = 0; streamIndex < numStreams; ++streamIndex )
						{
							if( countVerticesAppendedToStreams < m_gsMaxVertexCount )
							{
								// Note: We dont need to handle D3D11_SO_NO_RASTERIZED_STREAM here because it will never == streamIndex
								if( m_rasterizeStream == streamIndex )
								{
									// Increment the number of pixels we expect to be written
									m_numExpectedPixelsWritten++;

									m_pPixelShouldBeOutput[yIndex] = true;

									// Go through all expected data components and change their value based on the stream being rasterized
									for( UINT i = 0; i < 4; ++i )
										m_pExpectedPixelResult[yIndex].data[i] += streamIndex * (i + 1);
								}

								// Increment the count for the number of vertices that have been appended by the GS
								countVerticesAppendedToStreams++;
							}							
						}
					}
					else
					{
						m_pPixelShouldBeOutput[yIndex] = false;

						// Go through all expected data components and change their value to the clear color
						for( UINT i = 0; i < 4; ++i )
							m_pExpectedPixelResult[yIndex].data[i] = (UINT)g_clearRTVColor;
					}

					// Increment the count for where we are rendering in the render target
					++yIndex;
				}
			}
		}	
	}

	// Texture2D description
	textureDesc.Width = 1;
	textureDesc.Height = m_maxNumPixelsWritten;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the texture2D
	if( FAILED( hr = GetDevice()->CreateTexture2D( &textureDesc, NULL, &m_pRTTexture2D ) ) )
	{
		WriteToLog( _T("CStreamOutMultiStream::SetupTestCase() - Failed to create the render target texture (m_pRTTexture2D). HR = %s"), D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		return tr;
	}

	// Create the staging texure2D
	textureDesc.Usage = D3D11_USAGE_STAGING;
	textureDesc.BindFlags = 0;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	if( FAILED( hr = GetDevice()->CreateTexture2D( &textureDesc, NULL, &m_pRTTexture2DStaging ) ) )
	{
		WriteToLog( _T("CStreamOutMultiStream::SetupTestCase() - Failed to create the render target texture (m_pRTTexture2DStaging). HR = %s"), D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		return tr;
	}

	//
	// Create the render target view
	//

	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRTTexture2D, NULL, &m_pRTV ) ) )
	{
		WriteToLog( _T("CStreamOutMultiStream::SetupTestCase() - Failed to create the render target view (m_pRTV). HR = %s"), D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		return tr;
	}

	// Clear the render target view
	const float clearColor[] = { g_clearRTVColor, g_clearRTVColor, g_clearRTVColor, g_clearRTVColor };
	GetEffectiveContext()->ClearRenderTargetView( m_pRTV, clearColor );

	//
	// Setup vertex buffer
	//
	tr = SetupVertexBuffer( s_pointsSize, s_vbdataPoints );
	if( tr != RESULT_PASS )
		return tr;

	//
	// Create constant buffer
	//
	D3D11_BUFFER_DESC cbdesc =
	{
		1 * sizeof( CONSTDATA ),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		0
	};

	hr = GetDevice()->CreateBuffer( &cbdesc, NULL, &m_pCB );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateBuffer() unexpectedly failed creating constant buffer (m_pCB) - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Setup rasterizer and depth/stencil state
	//
	tr = SetupRasterizer();
	if( tr != RESULT_PASS )
		return tr;
	
	//
	// Setup SO statistics query
	//
	tr = SetupQueryObjects();
	if( tr != RESULT_PASS )
		return tr;

	return tr;
}

void CStreamOutMultiStream::GetCurSODecl( std::vector< D3D11_SO_DECLARATION_ENTRY > *pSODecl )
{
	D3D11_SO_DECLARATION_ENTRY* soDecl = NULL;
	UINT numEntries = 0;

	switch( m_declMode )
	{
	case ONE_STREAM_FOUR_BUFFER:
		soDecl = s_soDeclOneStreamFourBuffer;
		numEntries = ARRAY_SIZE(s_soDeclOneStreamFourBuffer);
		break;

	case TWO_STREAM_TWO_BUFFER:
		soDecl = s_soDeclTwoStreamTwoBuffer;
		numEntries = ARRAY_SIZE(s_soDeclTwoStreamTwoBuffer);
		break;

	case THREE_STREAM_FOUR_BUFFER:
		soDecl = s_soDeclThreeStreamFourBuffer;
		numEntries = ARRAY_SIZE(s_soDeclThreeStreamFourBuffer);
		break;

	case FOUR_STREAM_FOUR_BUFFER:
		soDecl = s_soDeclFourStreamFourBuffer;
		numEntries = ARRAY_SIZE(s_soDeclFourStreamFourBuffer);
		break;

	default:
		assert(false);
	}

	*pSODecl = std::vector< D3D11_SO_DECLARATION_ENTRY >( soDecl, soDecl + numEntries );
}

void CStreamOutMultiStream::CleanupTestCase()
{
	SAFE_RELEASE( m_pSOStatsQuery );
	SAFE_RELEASE( m_pSOOverflowPredicate );
	SAFE_RELEASE( m_pPipelineStatsQuery );
	SAFE_RELEASE( m_pDSDisable );
	SAFE_RELEASE( m_pRS );
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pCB );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_soBuffers[0] );
	SAFE_RELEASE( m_soBuffers[1] );
	SAFE_RELEASE( m_soBuffers[2] );
	SAFE_RELEASE( m_soBuffers[3] );
	SAFE_RELEASE( m_pRTTexture2D );
	SAFE_RELEASE( m_pRTTexture2DStaging );
	SAFE_RELEASE( m_pRTV );

	if( m_pPixelShouldBeOutput )
	{
		delete [] m_pPixelShouldBeOutput;
		m_pPixelShouldBeOutput = NULL;
	}

	if( m_pExpectedPixelResult )
	{
		delete [] m_pExpectedPixelResult;
		m_pExpectedPixelResult = NULL;
	}
}


TEST_RESULT CStreamOutMultiStream::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;
	ID3D11Buffer *pTempBuffer = NULL;

	ID3D11Query* pPerStreamSOStats[4] = { 0, 0, 0, 0 };
	ID3D11Query* pPerStreamSOOverflowPredicate[4] = { 0, 0, 0, 0 };

	const D3D11_QUERY SOStatsQueryType[4] = 
	{ 
		D3D11_QUERY_SO_STATISTICS_STREAM0, 
		D3D11_QUERY_SO_STATISTICS_STREAM1, 
		D3D11_QUERY_SO_STATISTICS_STREAM2, 
		D3D11_QUERY_SO_STATISTICS_STREAM3 
	};

	const D3D11_QUERY SOOverflowQueryType[4] = 
	{
		D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM0,
		D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM1,
		D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM2,
		D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM3
	};

	D3D11_QUERY_DATA_SO_STATISTICS expectedGlobalStats;
	BOOL expectedGlobalOverflow;

	//
	// Create queries
	//
	for( UINT i = 0; i < 4; i++ )
	{
		D3D11_QUERY_DESC QDSOStats = { SOStatsQueryType[i], 0 };
		hr = GetDevice()->CreateQuery( &QDSOStats, &pPerStreamSOStats[i] );
		if( FAILED(hr) )
		{
			WriteToLog( _T( "CreateQuery failed for a SOStats - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto testDone;
		}

		D3D11_QUERY_DESC QDSOOverflow = { SOOverflowQueryType[i], 0 };
		hr = GetDevice()->CreateQuery( &QDSOOverflow, &pPerStreamSOOverflowPredicate[i] );
		if( FAILED(hr) )
		{
			WriteToLog( _T( "CreateQuery failed for a SOOverflow - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto testDone;
		}
	}

	//
	// Clear streamout buffers
	//	
	for( UINT i = 0; i < ARRAY_SIZE(m_soBuffers); i++ )
	{
		tr = ClearSOBuffer( m_soBuffers[i], m_soBufferSize[i] );
		if( tr != RESULT_PASS )
			goto testDone;
	}

	//
	// Setup pipeline state
	//
	GetEffectiveContext()->IASetInputLayout( m_pInputLayout );
	ID3D11Buffer *vbuffers[] = { m_pVB };
	UINT strides[] = { sizeof( VSIN ) };
	UINT offsets[] = { 0 };
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, vbuffers, strides, offsets );
	GetEffectiveContext()->IASetPrimitiveTopology( m_inputTopology );
	GetEffectiveContext()->IASetIndexBuffer( NULL, DXGI_FORMAT_UNKNOWN, 0 );

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	ID3D11Buffer *cbuffers[] = { m_pCB };
	GetEffectiveContext()->GSSetConstantBuffers( 0, 1, cbuffers );
	GetEffectiveContext()->VSSetConstantBuffers( 0, 1, cbuffers );
	GetEffectiveContext()->RSSetState( m_pRS );

	// Enable pixel pipeline
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 ); 
	GetEffectiveContext()->OMSetDepthStencilState( m_pDSDisable, 0 );
	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTV, NULL );

	// Set viewport
	D3D11_VIEWPORT viewPort;

	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = 1;
	viewPort.Height = (float) m_maxNumPixelsWritten;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	GetEffectiveContext()->RSSetViewports( 1, &viewPort );

	// Setup stream out
	GetEffectiveContext()->SOSetTargets( 4, m_soBuffers, m_soOffsets );

	//
	// Draw
	//
	GetEffectiveContext()->Begin( m_pSOStatsQuery );
	GetEffectiveContext()->Begin( m_pSOOverflowPredicate );
	GetEffectiveContext()->Begin( m_pPipelineStatsQuery );

	for( UINT i = 0; i < 4; i++ )
	{
		GetEffectiveContext()->Begin( pPerStreamSOStats[i] );
		GetEffectiveContext()->Begin( pPerStreamSOOverflowPredicate[i] );
	}

	for( UINT drawIndex = 0; drawIndex < m_numDrawCalls; ++drawIndex )
	{
		CONSTDATA *pConstData;
		UINT *pConstViewportCorrectionData;
		D3D11_MAPPED_SUBRESOURCE mappedRes;

		// Setup constant data for draw call
		hr = GetEffectiveContext()->Map( m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
		if( FAILED( hr ) )
		{
			WriteToLog( _T( "Map() unexpectedly failed mapping dynamic constant buffer (m_pCB) - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto testDone;
		}
		pConstData = (CONSTDATA*) mappedRes.pData;

		pConstData->numPrimsPerInputVertex[0] = m_numShaderLoopIterations;
		pConstData->drawIndex[0] = drawIndex;
		pConstData->numDrawCalls[0] = m_numDrawCalls;
		pConstData->viewportHeight[0] = (UINT)viewPort.Height;

		GetEffectiveContext()->Unmap( m_pCB, 0 );

		// Draw
		GetEffectiveContext()->Draw( m_numInputVerts, 0 );
	}

	GetEffectiveContext()->End( m_pSOOverflowPredicate );
	GetEffectiveContext()->End( m_pSOStatsQuery );
	GetEffectiveContext()->End( m_pPipelineStatsQuery );

	for( UINT i = 0; i < 4; i++ )
	{
		GetEffectiveContext()->End( pPerStreamSOStats[i] );
		GetEffectiveContext()->End( pPerStreamSOOverflowPredicate[i] );
	}

	// Copy the RT texture
	GetEffectiveContext()->CopyResource( m_pRTTexture2DStaging, m_pRTTexture2D );

	// execute any deferred operations
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext failed. hr = %s", D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto testDone;
	}

	// Get query results
	D3D11_QUERY_DATA_SO_STATISTICS globalSOStats;
	D3D11_QUERY_DATA_SO_STATISTICS perStreamSOStats[4];

	struct
	{
		ID3D11Query* query;
		D3D11_QUERY_DATA_SO_STATISTICS* results;
	} soStatsQueries[] = 
	{
		{ m_pSOStatsQuery, &globalSOStats },
		{ pPerStreamSOStats[0], &perStreamSOStats[0] },
		{ pPerStreamSOStats[1], &perStreamSOStats[1] },
		{ pPerStreamSOStats[2], &perStreamSOStats[2] },
		{ pPerStreamSOStats[3], &perStreamSOStats[3] },
	};

	for( UINT i = 0; i < ARRAY_SIZE(soStatsQueries); i++ )
	{
		if (GetFramework()->GetDataWithTimeout( GetImmediateContext(), soStatsQueries[i].query, soStatsQueries[i].results, sizeof(D3D11_QUERY_DATA_SO_STATISTICS), 0 ) != S_OK)
		{
		    WriteToLog( _T( "GetData() failed or timed out." ) );
			tr = RESULT_FAIL;
		    goto testDone;
		}
	}

	BOOL globalOverflow;
	BOOL perStreamOverflow[4];

	struct
	{
		ID3D11Query* query;
		BOOL* results;
	} overflowQueries[] = 
	{
		{ m_pSOOverflowPredicate, &globalOverflow },
		{ pPerStreamSOOverflowPredicate[0], &perStreamOverflow[0] },
		{ pPerStreamSOOverflowPredicate[1], &perStreamOverflow[1] },
		{ pPerStreamSOOverflowPredicate[2], &perStreamOverflow[2] },
		{ pPerStreamSOOverflowPredicate[3], &perStreamOverflow[3] },
	};

	for( UINT i = 0; i < ARRAY_SIZE(overflowQueries); i++ )
	{
		if (GetFramework()->GetDataWithTimeout( GetImmediateContext(), overflowQueries[i].query, overflowQueries[i].results, sizeof(BOOL), 0 ) != S_OK)
		{
		    WriteToLog( _T( "GetData() failed or timed out." ) );
			tr = RESULT_FAIL;
		    goto testDone;
		}
	}

	//
	// Check pipeline statistics
	//

	UINT size = m_pPipelineStatsQuery->GetDataSize();
	if( size != sizeof( D3D11_QUERY_DATA_PIPELINE_STATISTICS ) )
	{
		WriteToLog( _T( "GetDataSize() return value incorrect - expected: %i, actual %i" ), sizeof( D3D11_QUERY_DATA_PIPELINE_STATISTICS ), size );
		tr = RESULT_FAIL;
		goto testDone;
	}

	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineStats;
	if (GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pPipelineStatsQuery, &pipelineStats, size, 0 ) != S_OK)
	{
		WriteToLog( _T( "GetData() failed or timed out." ) );
		tr = RESULT_FAIL;
		goto testDone;
	}

	// Verify data was passed to the rasterizer
	// Note: This is usually 4x because for Pixel shaders are executed in 2x2 grids, and we are dealing with points only.
	if( pipelineStats.PSInvocations < m_numExpectedPixelsWritten )
	{
		WriteToLog( _T( "pipelineStats.PSInvocations is invalid - expected at least %i, actual %i" ), m_numExpectedPixelsWritten, pipelineStats.PSInvocations );
		tr = RESULT_FAIL;
		goto testDone;
	}

	BOOL streamActive[4] = { FALSE, FALSE, FALSE, FALSE };

	// Verify the render target data
	tr = ValidateRenderTarget();

	if( RESULT_PASS != tr )
	{
		WriteToLog( _T( "ValidateRenderTarget() failed.") );
		tr = RESULT_FAIL;
		goto testDone;
	}

	// Verify data in stream-output buffers
	for( UINT i = 0; i < 4; i++ )
	{
		assert( NULL == pTempBuffer );

		D3D11_BUFFER_DESC tempdesc;
		tempdesc.ByteWidth = m_soBufferDesc[i].ByteWidth;
		tempdesc.BindFlags = 0;
		tempdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tempdesc.MiscFlags = 0;
		tempdesc.Usage = D3D11_USAGE_STAGING;

		hr = GetDevice()->CreateBuffer( &tempdesc, NULL, &pTempBuffer );
		if( FAILED( hr ) )
		{
			WriteToLog( _T( "CreateBuffer() unexpectedly failed creating temporary buffer - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto testDone;
		}

		GetImmediateContext()->CopyResource( pTempBuffer, m_soBuffers[i] );

		const BYTE *pSrcSOVerts;
		D3D11_MAPPED_SUBRESOURCE mappedRes;
		hr = GetImmediateContext()->Map( pTempBuffer, 0, D3D11_MAP_READ, 0, &mappedRes );
		if( FAILED( hr ) )
		{
			WriteToLog( _T( "ID3D11Buffer::Map() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto testDone;
		}
		pSrcSOVerts = (const BYTE*) mappedRes.pData;

		// Examine output data
		VSIN *pVertexIn;

		pVertexIn = s_vbdataPoints;
		
		// Verify the data in the so buffer is untouched where
		// "whitespace" is expected via offset, stride, decl gaps.
		for( UINT offset = 0; offset < (m_soOffsets[i]/sizeof(UINT)); ++offset )
		{
			UINT whitespace = ( (UINT *) pSrcSOVerts )[offset];
			if( whitespace != MAGIC_NUMBER )
			{
				WriteToLog( "SO buffer corruption within offset space" );
				tr = RESULT_FAIL;
				goto testDone;
			}
		}

		UINT associatedStream = 0;
		D3D11_QUERY_DATA_SO_STATISTICS expectedSOStats = { 0, 0 };
		BOOL expectedOverflow = FALSE;
		
		tr = TestPointList( i, pVertexIn, pSrcSOVerts + m_soOffsets[i], &associatedStream, &expectedSOStats, &expectedOverflow );

		if( tr != RESULT_PASS )
			goto testDone;		

		GetImmediateContext()->Unmap(pTempBuffer,0);
		SAFE_RELEASE( pTempBuffer );

		if( associatedStream != UINT_MAX )
		{
			streamActive[associatedStream] = TRUE;

			// Verify query results for this stream
			D3D11_QUERY_DATA_SO_STATISTICS actualSOStats = perStreamSOStats[associatedStream];
			BOOL actualOverflow = perStreamOverflow[associatedStream];

			if( expectedSOStats.NumPrimitivesWritten != actualSOStats.NumPrimitivesWritten )
			{
				WriteToLog( "NumPrimitivesWritten did not match expected value.  Stream = %u, Expected = %I64d, found = %I64d", associatedStream, expectedSOStats.NumPrimitivesWritten, actualSOStats.NumPrimitivesWritten );
				tr = RESULT_FAIL;
				goto testDone;
			}

			if( expectedSOStats.PrimitivesStorageNeeded != actualSOStats.PrimitivesStorageNeeded )
			{
				WriteToLog( "PrimitivesStorageNeeded did not match expected value.  Stream = %u, Expected = %I64d, found = %I64d", associatedStream, expectedSOStats.PrimitivesStorageNeeded, actualSOStats.PrimitivesStorageNeeded );
				tr = RESULT_FAIL;
				goto testDone;
			}

			if( expectedOverflow != actualOverflow )
			{
				WriteToLog( "Stream overflow did not match expected value.  Stream = %u, Expected = %s, Found = %s", associatedStream, expectedOverflow ? "TRUE" : "FALSE", actualOverflow ? "TRUE" : "FALSE" );
				tr = RESULT_FAIL;
				goto testDone;
			}
		}
	}

	// Verify query results for non active streams
	for( UINT i = 0; i < 4; i++ )
	{
		if( !streamActive[i] )
		{
			if( perStreamSOStats[i].NumPrimitivesWritten != 0 )
			{
				WriteToLog( "NumPrimitivesWritten for an inactive stream was non-zero.  Stream = %u, NumPrimitivesWritten = %I64d", i, perStreamSOStats[i].NumPrimitivesWritten );
				tr = RESULT_FAIL;
				goto testDone;
			}

			if( perStreamSOStats[i].PrimitivesStorageNeeded != 0 )
			{
				WriteToLog( "PrimitivesStorageNeeded for an inactive stream was non-zero.  Stream = %u, PrimitivesStorageNeeded = %I64d", i, perStreamSOStats[i].PrimitivesStorageNeeded );
				tr = RESULT_FAIL;
				goto testDone;
			}

			if( perStreamOverflow[i] )
			{
				WriteToLog( "Per-stream overflow was TRUE for an inactive stream.  Stream = %u", i );
				tr = RESULT_FAIL;
				goto testDone;
			}
		}
	}

	// Verify that global counters are correct
	// The global counter for SO stats is the same as stream 0
	// the global counter for the overflow predicate is the union of all overflow predicates
	expectedGlobalStats = perStreamSOStats[0];
	expectedGlobalOverflow = FALSE;

	for( UINT i = 0; i < 4; i++ )
	{
		expectedGlobalOverflow = expectedGlobalOverflow || perStreamOverflow[i];
	}

	if( expectedGlobalStats.NumPrimitivesWritten != globalSOStats.NumPrimitivesWritten )
	{
		WriteToLog( "NumPrimitivesWritten did not match expected value for D3D11_QUERY_SO_STATISTICS_STREAM counter.  This should by the same as D3D11_QUERY_SO_STATISTICS_STREAM0.  Expected = %I64d, found = %I64d", expectedGlobalStats.NumPrimitivesWritten, globalSOStats.NumPrimitivesWritten );
		tr = RESULT_FAIL;
		goto testDone;
	}

	if( expectedGlobalStats.PrimitivesStorageNeeded != globalSOStats.PrimitivesStorageNeeded )
	{
		WriteToLog( "PrimitivesStorageNeeded did not match expected value for D3D11_QUERY_SO_STATISTICS_STREAM counter.  This should by the same as D3D11_QUERY_SO_STATISTICS_STREAM0.  Expected = %I64d, found = %I64d", expectedGlobalStats.PrimitivesStorageNeeded, globalSOStats.PrimitivesStorageNeeded );
		tr = RESULT_FAIL;
		goto testDone;
	}

	if( expectedGlobalOverflow != globalOverflow )
	{
		WriteToLog( "Stream overflow did not match expected value for global counter.  Expected = %s, Found = %s", expectedGlobalOverflow ? "TRUE" : "FALSE", globalOverflow ? "TRUE" : "FALSE" );
		tr = RESULT_FAIL;
		goto testDone;
	}


testDone:
	SAFE_RELEASE( pTempBuffer );

	for( UINT i = 0; i < 4; i++ )
	{
		SAFE_RELEASE(pPerStreamSOStats[i]);
		SAFE_RELEASE(pPerStreamSOOverflowPredicate[i]);
	}

	return tr;
}


TEST_RESULT CStreamOutMultiStream::TestPointList( UINT bufferIndex, const VSIN *pVertexIn, const BYTE *pGSOutData, UINT* pAssociatedStreamOut, D3D11_QUERY_DATA_SO_STATISTICS* pSOStatsOut, BOOL* pOverflowOut )
{
	// Initialize outputs
	*pAssociatedStreamOut = UINT_MAX;
	pSOStatsOut->NumPrimitivesWritten = 0;
	pSOStatsOut->PrimitivesStorageNeeded = 0;
	*pOverflowOut = FALSE;

	TEST_RESULT tempResult;
	UINT outputVertexIndex[4] = {0, 0, 0, 0}; // index into each buffer of the current vertex.  There is one index per stream

	std::vector< D3D11_SO_DECLARATION_ENTRY > vSODecl;
	GetCurSODecl( &vSODecl );

	// Determine which stream this buffer is a part of
	UINT streamAssociatedWithBuffer = UINT_MAX;

	for( UINT i = 0; i < vSODecl.size(); i++ )
	{
		if( vSODecl[i].OutputSlot == bufferIndex )
		{
			streamAssociatedWithBuffer = vSODecl[i].Stream;
			break;
		}
	}

	*pAssociatedStreamOut = streamAssociatedWithBuffer;

	assert( m_soBufferSize[bufferIndex] >= m_soOffsets[bufferIndex] );
	UINT maxVertexCountInThisBuffer = m_soVertexStride[bufferIndex] ? ((m_soBufferSize[bufferIndex] - m_soOffsets[bufferIndex]) / m_soVertexStride[bufferIndex]) : 0;

	// Do nothing if the buffer at bufferIndex is not assocaited with any stream
	if( UINT_MAX != streamAssociatedWithBuffer )
	{
		UINT maxStreamVertices = UINT_MAX;

		// Writes to a stream stop when any buffer in the stream is full, so determine the maximum number of writes that can go to this stream
		for( UINT i = 0; i < vSODecl.size(); i++ )
		{
			if( vSODecl[i].Stream == streamAssociatedWithBuffer )
			{
				UINT otherBufferIndex = vSODecl[i].OutputSlot;

				assert( m_soBufferSize[bufferIndex] >= m_soOffsets[otherBufferIndex] );
				UINT maxVertexCountInBuffer = m_soVertexStride[otherBufferIndex] ? ((m_soBufferSize[otherBufferIndex] - m_soOffsets[otherBufferIndex]) / m_soVertexStride[otherBufferIndex]) : 0;
				maxStreamVertices = min(maxStreamVertices, maxVertexCountInBuffer);
			}
		}
		
		// For each Draw() call
		for( UINT i = 0; i < m_numDrawCalls; i++ )
		{
			// For each input vertex
			for( UINT j = 0; j < m_numInputVerts; ++j )
			{
				// simulate vs
				GSIN gsin;
				gsin.pos[0] = pVertexIn[j].pos[0];
				gsin.pos[1] = pVertexIn[j].pos[1];
				gsin.pos[2] = pVertexIn[j].pos[2];
				gsin.pos[3] = pVertexIn[j].pos[3];
				gsin.data = Encode( pVertexIn[j].data );

				UINT totalVerticesEmittedThisInvocation = 0; // total number of vertices that have been emitted to any stream.  The GS stops execution when this reaches m_gsMaxVertexCount

				// simulate GS
				// For each loop inside the GS
				for( UINT k = 0; k < m_numShaderLoopIterations; k++ )
				{
					// this code matches the code in GSPointInPointsOutOneStreamFourBuffer
					GSOUT128 refVert;

					UINT n = i + (2 * k) + gsin.data;

					for( UINT idx = 0; idx < 31; idx++ )
					{
						for( UINT cmp = 0; cmp < 4; cmp++ )
						{
							n = Encode(n);
							refVert.data[idx][cmp] = n;
						}
					}

					for( UINT stream = 0; stream < GetNumStreams(); stream++ )
					{
						// The GS will not emit when it has hit the max vertex count
						// or when 1 == n%5 (this is hard-coded in the shader code before each emit)
						if( (totalVerticesEmittedThisInvocation < m_gsMaxVertexCount) && (1 != (n % 5)) )
						{
							if( stream == streamAssociatedWithBuffer )
							{
								pSOStatsOut->PrimitivesStorageNeeded++;

								// If there is room in the stream, then increment the NumPrimitivesWritten counter
								// Otherwise, record that there was an overflow
								if( outputVertexIndex[stream] < maxStreamVertices )
								{
									pSOStatsOut->NumPrimitivesWritten++;
								}
								else
								{
									*pOverflowOut = TRUE;
								}
							

								// Don't call CompareSOVertex if outputVertexIndex points past the end of the buffer
								// Do call it however if outputVertexIndex points past the end of the STREAM
								// this tests to ensure that a stream is only written if all buffers associated with that stream have room
								if( outputVertexIndex[stream] < maxVertexCountInThisBuffer )
								{
									//
									// MAGIC_NUMBER should be in the buffer when:
									// the buffer itself has room for more vertices, but one of the other buffers in the stream does not
									//
									tempResult = CompareSOVertex( bufferIndex, stream, &refVert, pGSOutData, outputVertexIndex[stream], &(vSODecl[0]), vSODecl.size(), (outputVertexIndex[stream] < maxStreamVertices) );
									if( RESULT_PASS != tempResult )
										return tempResult;
								}
							}

							for( UINT idx = 0; idx < 31; idx++ )
							{
								for( UINT cmp = 0; cmp < 4; cmp++ )
								{
									refVert.data[idx][cmp] += (cmp+1);
								}
							}
							
							totalVerticesEmittedThisInvocation++;

							outputVertexIndex[stream]++;
						}
					}
				}
			}
		}
	}

	return RESULT_PASS;
}

TEST_RESULT CStreamOutMultiStream::CompareSOVertex( UINT bufferIndex, UINT streamIndex, const GSOUT128* refVert, const BYTE* pGSOutData, UINT outputVertexIndex, const D3D11_SO_DECLARATION_ENTRY* soDecl, UINT numEntries, bool inRange )
{
	// Find offset to the target vertex
	const BYTE* pGSOutputVertex = pGSOutData + (m_soVertexStride[bufferIndex] * outputVertexIndex);

	const BYTE* pCurr = pGSOutputVertex;

	// For each element in the SO decl referencing this buffer
	for( UINT i = 0; i < numEntries; i++ )
	{
		if( (soDecl[i].OutputSlot == bufferIndex) )
		{
			assert( soDecl[i].Stream == streamIndex );

			if( soDecl[i].SemanticName )
			{
				if( 0 == _stricmp( soDecl[i].SemanticName, "data" ) )
				{
					for( UINT j = 0; j < soDecl[i].ComponentCount; j++ )
					{
						UINT srcData = *((UINT*)pCurr);
						UINT refData = refVert->data[ soDecl[i].SemanticIndex ][ soDecl[i].StartComponent + j ];

						// If the vertex is out of the bounds defined by the stream, then expect the clear value
						if( !inRange )
						{
							refData = MAGIC_NUMBER;
						}

						if( srcData != refData )
						{
							WriteToLog( "GS output found bad data.  stream = %u, buffer = %u, expected = 0x%x, found = 0x%x, vertex = %u", streamIndex, bufferIndex, refData, srcData, outputVertexIndex );
							if( MAGIC_NUMBER == refData )
							{
								WriteToLog( "expected = 0x%8.8x means the GPU should not have written to that location in the buffer", MAGIC_NUMBER );
							}
							return RESULT_FAIL;
						}

						pCurr += sizeof(UINT);
					}
				}
				else
				{
					assert( 0 == _stricmp( soDecl[i].SemanticName, "sv_position" ) );   

					pCurr += (sizeof(UINT) * soDecl[i].ComponentCount);;
				}
			}
			else
			{
				 // GAP
				for( UINT j = 0; j < soDecl[i].ComponentCount; j++ )
				{
					UINT srcData = *((UINT*)pCurr);
					UINT refData = MAGIC_NUMBER;
					if(srcData != refData)
					{
						WriteToLog( "GS wrote into a gap.  stream = %u, buffer = %u, expected = 0x%x, found = 0x%x, vertex = %u", streamIndex, bufferIndex, refData, srcData, outputVertexIndex );
						return RESULT_FAIL;
					}
				}

				pCurr += (sizeof(UINT) * soDecl[i].ComponentCount);;
			}
		}
	}

	// Verify that GPU did not write into the padding at the end of the vertex
	assert( m_soVertexStride[bufferIndex] >= m_stridePadding[bufferIndex] );

	UINT paddingUINTs = m_stridePadding[bufferIndex] / 4;

	const UINT* paddingStart = (const UINT*)(pGSOutputVertex + m_soVertexStride[bufferIndex] - m_stridePadding[bufferIndex]);

	for( UINT i = 0; i < paddingUINTs; i++ )
	{
		UINT src = paddingStart[i];
		if( src != MAGIC_NUMBER )
		{
			WriteToLog( "GS wrote into the padding at the end of a vertex.  stream = %u, buffer = %u, expected = 0x%8.8x, found = 0x%x, vertex = %u", streamIndex, bufferIndex, MAGIC_NUMBER, src, outputVertexIndex );
			return RESULT_FAIL;
		}
	}

	return RESULT_PASS;
}

UINT CStreamOutMultiStream::GetNumStreams()
{
	switch( m_declMode )
	{
	case ONE_STREAM_FOUR_BUFFER:   return 1;
	case TWO_STREAM_TWO_BUFFER:    return 2;
	case THREE_STREAM_FOUR_BUFFER: return 3;
	case FOUR_STREAM_FOUR_BUFFER:  return 4;    
		
	default:
		assert(false);
		return 0;
	}
}

TEST_RESULT CStreamOutMultiStream::ValidateRenderTarget()
{
	// Local variables
	D3D11_TEXTURE2D_DESC tex2DDesc;
	m_pRTTexture2DStaging->GetDesc( &tex2DDesc );
	const UINT rtHeight = tex2DDesc.Height;
	const UINT rtWidth = tex2DDesc.Width;
	bool bFailedPixel = false;
	TEST_RESULT tr = RESULT_PASS;
	const UINT *pRTData;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	HRESULT hr = S_OK;

	// Map the staging texture
	hr = GetImmediateContext()->Map( m_pRTTexture2DStaging, 0, D3D11_MAP_READ, 0, &mappedRes );

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "ID3D11Buffer::Map() unexpectedly failed (m_pRTTexture2DStaging) - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto testDone;
	}

	pRTData = (const UINT*) mappedRes.pData;

	// Validate the results
	for( UINT r = 0; r < rtHeight; ++r )
	{
		for( UINT c = 0; c < rtWidth; ++c )
		{
			// Reset some flags
			bFailedPixel = false;

			// Get the current pixel
			const UINT *pCurrPixel = (const UINT*)(((const BYTE*)mappedRes.pData) + (r * mappedRes.RowPitch)) + (c * 4);

			if(PixelShouldHaveData(r, c))
			{
				// Get the expected pixel data
				// Note the expected result doesnt depend on c, because the width is 1.
				PIXELRESULT expectedPixelColor = m_pExpectedPixelResult[r];

				// Verify the data
				for( UINT colorComponent = 0; colorComponent < 4; ++colorComponent )
				{
					if( pCurrPixel[colorComponent] != expectedPixelColor.data[colorComponent] )
					{
						bFailedPixel = true;
					}
				}

				if( bFailedPixel )
				{
					WriteToLog( _T("Validation of the Render Target failed." ) );
					WriteToLog( _T(" Expected RT Pixel[%d][%d] = { %d, %d, %d, %d }"), r, c, expectedPixelColor.data[0], expectedPixelColor.data[1], expectedPixelColor.data[2], expectedPixelColor.data[3] );
					WriteToLog( _T(" Actual RT Pixel[%d][%d] = { %d, %d, %d, %d }"), r, c, pCurrPixel[0], pCurrPixel[1], pCurrPixel[2], pCurrPixel[3] );
					tr = RESULT_FAIL;
					goto testDone;
				}
			}
			else
			{
				// Verify the data is the clear color
				for( UINT colorComponent = 0; colorComponent < 4; ++colorComponent )
				{
					if( pCurrPixel[colorComponent] != g_clearRTVColor )
					{
						bFailedPixel = true;
					}
				}

				if( bFailedPixel )
				{
					WriteToLog( _T("Validation of the Render Target failed. Expected the clear color for the RT (%#x), but got something else."), g_clearRTVColor );
					WriteToLog( _T(" Actual RT Pixel[%d][%d] = { %d, %d, %d, %d }"), r, c, pCurrPixel[0], pCurrPixel[1], pCurrPixel[2], pCurrPixel[3] );
					tr = RESULT_FAIL;
					goto testDone;
				}
			}
		}
	}

testDone:
	// Unmap
	GetImmediateContext()->Unmap( m_pRTTexture2DStaging, 0 );

	return tr;
}

bool CStreamOutMultiStream::PixelShouldHaveData( const UINT &r, const UINT &c )
{
	// Note: Dont need c here, because width is always 1
	return m_pPixelShouldBeOutput[r];
}