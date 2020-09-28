// ***************************************************************
//  MultiBuffer   version:  1.0   ·  date: 01/24/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#include "MultiBuffer.h"

typedef UINT SOElement4[4];
typedef UINT SOElement3[3];
typedef UINT SOElement2[2];

static D3D11_SO_DECLARATION_ENTRY s_soDeclSimpleMulti[] =
{
	{ 0, "data_a", 0, 0, 4, 0 },
	{ 0, "data_b", 0, 0, 4, 1 },
	{ 0, "data_c", 0, 0, 4, 2 },
	{ 0, "data_d", 0, 0, 4, 3 },
};

static D3D11_SO_DECLARATION_ENTRY s_soDeclPartialMulti[] =
{
	{ 0, "data_a", 0, 1, 2, 0 },
	{ 0, "data_b", 0, 0, 3, 1 },
	{ 0, "data_c", 0, 3, 1, 2 },
	{ 0, "data_d", 0, 1, 1, 3 },
};


static D3D11_SO_DECLARATION_ENTRY s_soDeclSplitElementsMulti[] =
{
	{ 0, "data_a", 0, 1, 2, 0 },
	{ 0, "data_a", 0, 3, 1, 1 },
};


static D3D11_SO_DECLARATION_ENTRY s_soDeclSkipSlotMulti[] =
{
	{ 0, "data_b", 0, 0, 4, 0 },
	{ 0, "data_c", 0, 0, 4, 2 },
	{ 0, "data_d", 0, 0, 4, 3 },
};

static D3D11_SO_DECLARATION_ENTRY s_soDeclSingleSlotMulti[] =
{
	{ 0, "data_c", 0, 0, 4, 2 },
};

static DECL_MODE s_declModeTable[] =
{
	DECL_MODE_SIMPLE,
	DECL_MODE_SKIP_SLOT,
	DECL_MODE_SPLIT_ELEMENTS,
	DECL_MODE_PARTIAL_ELEMENTS,
};



//////////////////////////////////////////////////////////////////////////

TEST_RESULT CStreamOutMultiBuffer::Setup()
{
	TEST_RESULT tr = RESULT_PASS;

	tr = SetupSOClearObjects();

	return tr;
}

//////////////////////////////////////////////////////////////////////////

void CStreamOutMultiBuffer::Cleanup()
{
	CleanupSOClearObjects();
}

//////////////////////////////////////////////////////////////////////////

static UINT s_soBufferSizeTable[4][5] =
{
	{	1152,		288,		0,		2304,		576	},
	{	1152,		1152,		0,		1152,		288	},
	{	1152,		576,		1152,	2304,		576	},
	{	1152,		1152,		0,		1152,		288	},
};

static UINT s_soOffsetTable[4][3] =
{
	{	0,	200,	200	},
	{	0,	500,	500	},
	{	0,	200,	64	},
	{	0,	1000,	700	},
};

static UINT s_maxVertexCountTable[] =
{
	6,
	8
};

//////////////////////////////////////////////////////////////////////////

CStreamOutMultiBuffer::CStreamOutMultiBuffer() :
	m_numInputPrimitives( 0 ),
	m_gsInnerLoopCount( 0 ),
	m_numDrawCalls( 0 )
{
	m_verticesPerPrimitive = 3;
}

//////////////////////////////////////////////////////////////////////////

// Input topologies for feature level < 11.0
static const D3D11_PRIMITIVE_TOPOLOGY TOPOLOGY_LIST_FL_10[] = 
{
	D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
	D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
	D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
	D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
};

// Input topologies for feature level >= 11.0
static const D3D11_PRIMITIVE_TOPOLOGY TOPOLOGY_LIST_FL_11[] = 
{
	D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
	D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
	D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
	D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
	D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST
};

// Returns the number of control points a a patch list
static UINT ControlPointCount( D3D11_PRIMITIVE_TOPOLOGY topology )
{
	assert( topology >= D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST );
	assert( topology <= D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST );

	UINT result = 0;

	if( (topology >= D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST) && 
		(topology <= D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST) )
	{
		result = (UINT)topology - (UINT)D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST + 1;
	}

	return result;
}

void CStreamOutMultiBuffer::InitTestParameters()
{
	CTableValueSet<UINT> *pBufferSizeTableValueSet0 = new CTableValueSet<UINT>( s_soBufferSizeTable[0], sizeof( s_soBufferSizeTable[0][0] ), sizeof( s_soBufferSizeTable[0] ) / ( sizeof( s_soBufferSizeTable[0][0] ) ) );
	CTableValueSet<UINT> *pBufferSizeTableValueSet1 = new CTableValueSet<UINT>( s_soBufferSizeTable[1], sizeof( s_soBufferSizeTable[1][0] ), sizeof( s_soBufferSizeTable[1] ) / ( sizeof( s_soBufferSizeTable[1][0] ) ) );
	CTableValueSet<UINT> *pBufferSizeTableValueSet2 = new CTableValueSet<UINT>( s_soBufferSizeTable[2], sizeof( s_soBufferSizeTable[2][0] ), sizeof( s_soBufferSizeTable[2] ) / ( sizeof( s_soBufferSizeTable[2][0] ) ) );
	CTableValueSet<UINT> *pBufferSizeTableValueSet3 = new CTableValueSet<UINT>( s_soBufferSizeTable[3], sizeof( s_soBufferSizeTable[3][0] ), sizeof( s_soBufferSizeTable[3] ) / ( sizeof( s_soBufferSizeTable[3][0] ) ) );

	CTableValueSet<UINT> *pOffsetTableValueSet0 = new CTableValueSet<UINT>( s_soOffsetTable[0], sizeof( s_soOffsetTable[0][0] ), sizeof( s_soOffsetTable[0] ) / ( sizeof( s_soOffsetTable[0][0] ) ) );
	CTableValueSet<UINT> *pOffsetTableValueSet1 = new CTableValueSet<UINT>( s_soOffsetTable[1], sizeof( s_soOffsetTable[1][0] ), sizeof( s_soOffsetTable[1] ) / ( sizeof( s_soOffsetTable[1][0] ) ) );
	CTableValueSet<UINT> *pOffsetTableValueSet2 = new CTableValueSet<UINT>( s_soOffsetTable[2], sizeof( s_soOffsetTable[2][0] ), sizeof( s_soOffsetTable[2] ) / ( sizeof( s_soOffsetTable[2][0] ) ) );
	CTableValueSet<UINT> *pOffsetTableValueSet3 = new CTableValueSet<UINT>( s_soOffsetTable[3], sizeof( s_soOffsetTable[3][0] ), sizeof( s_soOffsetTable[3] ) / ( sizeof( s_soOffsetTable[3][0] ) ) );

	CTableValueSet<D3D11_PRIMITIVE_TOPOLOGY> *pTopologyValueSetFL10 = new CTableValueSet<D3D11_PRIMITIVE_TOPOLOGY>( TOPOLOGY_LIST_FL_10, sizeof( TOPOLOGY_LIST_FL_10[0] ), ARRAY_SIZE( TOPOLOGY_LIST_FL_10 ) );
	CTableValueSet<D3D11_PRIMITIVE_TOPOLOGY> *pTopologyValueSetFL11 = new CTableValueSet<D3D11_PRIMITIVE_TOPOLOGY>( TOPOLOGY_LIST_FL_11, sizeof( TOPOLOGY_LIST_FL_11[0] ), ARRAY_SIZE( TOPOLOGY_LIST_FL_11 ) );

	CTestCaseParameter<UINT> *pNumInputPrimsParam = AddParameter<UINT>( "NumInputPrimitives", &m_numInputPrimitives );
	CTestCaseParameter<UINT> *pGSInnerLoopCount = AddParameter<UINT>( "GSInnerLoopCount", &m_gsInnerLoopCount );

	CTestCaseParameter<UINT> *pSOOffset0 = AddParameter<UINT>( "SOOffset0", &m_soOffsets[0] );
	CTestCaseParameter<UINT> *pSOOffset1 = AddParameter<UINT>( "SOOffset1", &m_soOffsets[1] );
	CTestCaseParameter<UINT> *pSOOffset2 = AddParameter<UINT>( "SOOffset2", &m_soOffsets[2] );
	CTestCaseParameter<UINT> *pSOOffset3 = AddParameter<UINT>( "SOOffset3", &m_soOffsets[3] );

	CTestCaseParameter<UINT> *pSOBufferSize0 = AddParameter<UINT>( "SOBufferSize0", &m_soBufferSize[0] );
	CTestCaseParameter<UINT> *pSOBufferSize1 = AddParameter<UINT>( "SOBufferSize1", &m_soBufferSize[1] );
	CTestCaseParameter<UINT> *pSOBufferSize2 = AddParameter<UINT>( "SOBufferSize2", &m_soBufferSize[2] );
	CTestCaseParameter<UINT> *pSOBufferSize3 = AddParameter<UINT>( "SOBufferSize3", &m_soBufferSize[3] );

	testfactor::RFactor numInputPrims = AddParameterValueSet<UINT>( pNumInputPrimsParam, new CRangeValueSet<UINT>( 1, 4, 1 ) );
	testfactor::RFactor gsInnerLoopCount = AddParameterValueSet<UINT>( pGSInnerLoopCount, new CRangeValueSet<UINT>( 2, 10, 2 ) );
	testfactor::RFactor gsMaxVertexCount = AddParameter<UINT>( "GSMaxVertexCount", &m_gsMaxVertexCount, new CTableValueSet<UINT>( s_maxVertexCountTable, sizeof( s_maxVertexCountTable[0] ), sizeof( s_maxVertexCountTable ) / sizeof( s_maxVertexCountTable[0] ) ) );

	testfactor::RFactor soOffset0 = AddParameterValueSet<UINT>( pSOOffset0, pOffsetTableValueSet0 );
	testfactor::RFactor soOffset1 = AddParameterValueSet<UINT>( pSOOffset1, pOffsetTableValueSet1 );
	testfactor::RFactor soOffset2 = AddParameterValueSet<UINT>( pSOOffset2, pOffsetTableValueSet2 );
	testfactor::RFactor soOffset3 = AddParameterValueSet<UINT>( pSOOffset3, pOffsetTableValueSet3 );

	testfactor::RFactor numDrawCalls = AddParameter<UINT>( "NumDrawCalls", &m_numDrawCalls, new CRangeValueSet<UINT>( 1, 7, 8 ) );	

	testfactor::RFactor soBufferSize0 = AddParameterValueSet<UINT>( pSOBufferSize0, pBufferSizeTableValueSet0 );
	testfactor::RFactor soBufferSize1 = AddParameterValueSet<UINT>( pSOBufferSize1, pBufferSizeTableValueSet1 );
	testfactor::RFactor soBufferSize2 = AddParameterValueSet<UINT>( pSOBufferSize2, pBufferSizeTableValueSet2 );
	testfactor::RFactor soBufferSize3 = AddParameterValueSet<UINT>( pSOBufferSize3, pBufferSizeTableValueSet3 );

	testfactor::RFactor soOffset = soOffset0 % soOffset1 % soOffset2 % soOffset3;

	testfactor::RFactor soDeclMode = AddParameter( _T( "DeclMode" ), &m_declMode, new CTableValueSet<DECL_MODE>( s_declModeTable, sizeof( s_declModeTable[0] ), sizeof( s_declModeTable ) / sizeof( s_declModeTable[0] ) ) );

	testfactor::RFactor soConfig = ( soBufferSize0 % soBufferSize1 % soBufferSize2 % soBufferSize3 ) * gsMaxVertexCount * soDeclMode;

	AddParameter<D3D11_PRIMITIVE_TOPOLOGY>( "InputTopology", &m_inputTopology );

	testfactor::RFactor inputTopologyFL10 = AddParameterValueSet( "InputTopology", pTopologyValueSetFL10 );
	testfactor::RFactor inputTopologyFL11 = AddParameterValueSet( "InputTopology", pTopologyValueSetFL11 );

	testfactor::RFactor inputTopology = GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ? inputTopologyFL11 : inputTopologyFL10;

	// BUGBUG - The following parameter needs a value set
	testfactor::RFactor outputPrimitiveType = AddParameter( "OutputPrimitiveType", &m_gsOutputPrimitiveType, tstring( "Triangles" ) );

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{soConfig, 1, NULL, 0},
		{numInputPrims, 1, NULL, 0},
		{gsInnerLoopCount, 1, NULL, 0},
		{soOffset, 1, NULL, 0},
		{numDrawCalls, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 3), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	testfactor::RFactor rfPairwiseFactors = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), _T("Pairwise"), groupOrders, _countof(groupOrders));
	SetRootTestFactor( (rfPairwiseFactors) % (inputTopology * outputPrimitiveType) );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumInputPrimitives"), 4 ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("GSInnerLoopCount"), 10 ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("GSMaxVertexCount"),  8 ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DECL_MODE>(_T("DeclMode"), DECL_MODE_SIMPLE ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DECL_MODE>(_T("DeclMode"), DECL_MODE_PARTIAL_ELEMENTS ), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<DECL_MODE>(_T("DeclMode"), DECL_MODE_SPLIT_ELEMENTS ), WeightFactorSmall);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CStreamOutMultiBuffer::GetInputVertexData( UINT* pCountOut, VSIN** pVertexDataOut, UINT* pInputVertsPerInvocationOut )
{
	UINT numVerts = 0;
	VSIN *pVertexData = NULL;
	UINT numInputVertsPerInvocation = 0;
	
	switch( m_inputTopology )
	{
	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		numVerts = s_pointsSize;
		pVertexData = s_vbdataPoints;
		numInputVertsPerInvocation = 1;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		numVerts = s_lineListSize;
		pVertexData = s_vbdataLineList;
		numInputVertsPerInvocation = 2;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
		numVerts = s_lineStripSize;
		pVertexData = s_vbdataLineStrip;
		numInputVertsPerInvocation = 2;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		numVerts = s_triangleListSize;
		pVertexData = s_vbdataTriangleList;
		numInputVertsPerInvocation = 3;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		numVerts = s_triangleStripSize;
		pVertexData = s_vbdataTriangleStrip;
		numInputVertsPerInvocation = 3;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		numVerts = s_lineStripAdjSize;
		pVertexData = s_vbdataLineStripAdj;
		numInputVertsPerInvocation = 4;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		numVerts = s_lineListAdjSize;
		pVertexData = s_vbdataLineListAdj;
		numInputVertsPerInvocation = 4;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		numVerts = s_triangleListAdjSize;
		pVertexData = s_vbdataTriangleListAdj;
		numInputVertsPerInvocation = 6;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		numVerts = s_triangleStripAdjSize;
		pVertexData = s_vbdataTriangleStripAdj;
		numInputVertsPerInvocation = 6;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		numVerts = s_32CPPatchListSize;
		pVertexData = s_vbdata32CPPatchList;
		numInputVertsPerInvocation = ControlPointCount( m_inputTopology );
		break;

	default:
		WriteToLog( _T("Unknown input topology") );
		return RESULT_FAIL;
	}

	if( pCountOut ) *pCountOut = numVerts;
	if( pVertexDataOut )*pVertexDataOut = pVertexData;
	if( pInputVertsPerInvocationOut )*pInputVertsPerInvocationOut = numInputVertsPerInvocation;

	return RESULT_PASS;
}

TEST_RESULT CStreamOutMultiBuffer::SetupTestCase()
{
	TEST_RESULT tr = RESULT_PASS;

	//
	// Setup Output Streams
	//	
	memset( &m_soBufferDesc, 0, sizeof( m_soBufferDesc ) );
	memset( &m_soBuffers, 0, sizeof( m_soBuffers ) );

	for( UINT i = 0; i < 4; ++i )
	{
		m_soBufferDesc[i].BindFlags = D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_VERTEX_BUFFER; // BUGBUG - Consider cycling BindFlags
		m_soBufferDesc[i].CPUAccessFlags = 0;
		m_soBufferDesc[i].ByteWidth = m_soBufferSize[i];
		m_soBufferDesc[i].Usage = D3D11_USAGE_DEFAULT;
		m_soBufferDesc[i].MiscFlags = 0;
	}
	
	tr = CreateSOBuffers( m_soBuffers, m_soBufferDesc );
	if( tr != RESULT_PASS )
		return tr;

	// Setup Shaders
	std::string vsName;
	FromString( &vsName, tstring( _T( "VSChangeData" ) ) );

	tr = SetupVSFromResource( "ShaderSource.sh", vsName.c_str(), NULL, 0, NULL, NULL, 0, &m_pVS, &m_pInputLayout );
	if( tr != RESULT_PASS )
		return tr;

	D3D11_SO_DECLARATION_ENTRY *soDecl;
	UINT numEntries;
	GetCurSODecl( &soDecl, &numEntries );

	// ShaderSourceMulti.sh contains a template filled with sprintf escapes like %i and %s
	// determine concrete values to insert there
	// The parameters are:
	//
	// MaxVertexCount
	// Input parameter decl ("triangle GSIn input[3]")
	// Input vertex count per primitive (line = 2, triangle = 3)
	// repeat
	char inputString[256] = {'\0'};
	UINT numVertsPerPrimitive = 0;

	switch( m_inputTopology )
	{
	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		sprintf_s(inputString, ARRAY_SIZE(inputString), "point GSIn input[1]");
		numVertsPerPrimitive = 1;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
		sprintf_s(inputString, ARRAY_SIZE(inputString), "line GSIn input[2]");
		numVertsPerPrimitive = 2;
		break;
	
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		sprintf_s(inputString, ARRAY_SIZE(inputString), "triangle GSIn input[3]");
		numVertsPerPrimitive = 3;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		sprintf_s(inputString, ARRAY_SIZE(inputString), "lineadj GSIn input[4]");
		numVertsPerPrimitive = 4;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		sprintf_s(inputString, ARRAY_SIZE(inputString), "triangleadj GSIn input[6]");
		numVertsPerPrimitive = 6;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		numVertsPerPrimitive = ControlPointCount( m_inputTopology );
		sprintf_s(inputString, ARRAY_SIZE(inputString), "InputPatch<GSIn,%i> input", numVertsPerPrimitive);
		break;

	default:
		assert( false );
	}

	char maxVertexCountBuffer[16];
	sprintf_s( maxVertexCountBuffer, ARRAY_SIZE(maxVertexCountBuffer), "%i", m_gsMaxVertexCount );

	char vertsPerPrimitiveBuffer[16];
	sprintf_s( vertsPerPrimitiveBuffer, ARRAY_SIZE(vertsPerPrimitiveBuffer), "%i", numVertsPerPrimitive );

	D3D10_SHADER_MACRO macros[4];
	
	macros[0].Name = "GS_MAX_VERTEX_COUNT";
	macros[0].Definition = maxVertexCountBuffer;

	macros[1].Name = "INPUT_DECL";
	macros[1].Definition = inputString;

	macros[2].Name = "INPUT_VERTS_PER_PRIM";
	macros[2].Definition = vertsPerPrimitiveBuffer;

	macros[3].Name = NULL;
	macros[3].Definition = NULL;


	tr = SetupGSFromResource( "ShaderSourceMulti.sh", "GSEntryPoint", soDecl, numEntries, macros, NULL, 0, false, &m_pGS );
	if( tr != RESULT_PASS )
		return tr;


	// Setup vertex buffer
	UINT numVerts = 0;
	VSIN *pVertexData = NULL;

	tr = GetInputVertexData( &numVerts, &pVertexData, NULL );
	if( tr != RESULT_PASS )
		return tr;

	tr = SetupVertexBuffer( numVerts, pVertexData );
	if( tr != RESULT_PASS )
		return tr;

	//
	// Create constant buffer
	//
	const D3D11_BUFFER_DESC cbdesc =
	{
		1 * sizeof( CONSTDATA ),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		0
	};

	HRESULT hr = GetDevice()->CreateBuffer( &cbdesc, NULL, &m_pCB );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateBuffer() unexpectedly failed creating constant buffer - result code '%s'" ), D3DHResultToString( hr ).c_str() );
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

void CStreamOutMultiBuffer::GetCurSODecl( D3D11_SO_DECLARATION_ENTRY **soDecl, UINT *numEntries )
{
	switch( m_declMode )
	{
		case DECL_MODE_SIMPLE:
			*soDecl = s_soDeclSimpleMulti;
			*numEntries = sizeof( s_soDeclSimpleMulti ) / sizeof( s_soDeclSimpleMulti[0] );
			break;
		case DECL_MODE_SPLIT_ELEMENTS:
			*soDecl = s_soDeclSplitElementsMulti;
			*numEntries = sizeof( s_soDeclSplitElementsMulti ) / sizeof( s_soDeclSplitElementsMulti[0] );
			break;
		case DECL_MODE_SKIP_SLOT:
			*soDecl = s_soDeclSkipSlotMulti;
			*numEntries = sizeof( s_soDeclSkipSlotMulti ) / sizeof( s_soDeclSkipSlotMulti[0] );
			break;
		case DECL_MODE_PARTIAL_ELEMENTS:
			*soDecl = s_soDeclPartialMulti;
			*numEntries = sizeof( s_soDeclPartialMulti ) / sizeof( s_soDeclPartialMulti[0] );
			break;
	}
}

void CStreamOutMultiBuffer::CleanupTestCase()
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
}

TEST_RESULT CStreamOutMultiBuffer::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;

	//
	// Determine input daya
	//
	UINT numVerts = 0;
	VSIN* pInputVertexData = NULL;
	UINT numInputVertsPerInvocation = 0;

	{
		TEST_RESULT trtemp = GetInputVertexData( &numVerts, &pInputVertexData, &numInputVertsPerInvocation );
		if( trtemp != RESULT_PASS )
			tr = trtemp;
	}

	//
	// Clear streamout buffers
	//
	for( UINT i = 0; i < 4; ++i )
	{	
		tr = ClearSOBuffer( m_soBuffers[i], m_soBufferSize[i] );
		if( tr != RESULT_PASS )
			return tr;
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
	GetEffectiveContext()->RSSetState( m_pRS );

	// Disable pixel pipeline
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 ); 
	GetEffectiveContext()->OMSetDepthStencilState( m_pDSDisable, 0 );

	// Setup stream out
	GetEffectiveContext()->SOSetTargets( 4, m_soBuffers, m_soOffsets );


	//
	// Draw
	//
	GetEffectiveContext()->Begin( m_pSOStatsQuery );
	GetEffectiveContext()->Begin( m_pSOOverflowPredicate );

	UINT numEntries;
	D3D11_SO_DECLARATION_ENTRY *soDecl;
	GetCurSODecl( &soDecl, &numEntries );
	WriteDeclToLog( soDecl, numEntries );
	UINT soElementWidth[4] = {0};
	UINT soElementComponents[4][4] = { 0xffffffff };
	for( UINT i = 0; i < numEntries; ++i )
	{
		UINT streamIndex = soDecl[i].OutputSlot;
		UINT componentIndex = soDecl[i].StartComponent;
		assert( soElementWidth[streamIndex] == 0 );
		for( UINT j = 0; j < soDecl[i].ComponentCount; ++j )
			soElementComponents[streamIndex][j] = componentIndex + j;
		soElementWidth[streamIndex] = soDecl[i].ComponentCount;
	}

	// The GS inner loop stops executing when the maximum vertex count is reached
	const UINT gsInnerLoopCount = min( m_gsMaxVertexCount, m_gsInnerLoopCount );

	// Each iteration of the GS inner loop emits 1 vertex
	// the GS emits triangle strips, so this calculation determines the number of primitives emitted per GS invocation
	const UINT numGSPrimitivesEmittedPerInvocation = gsInnerLoopCount > 2 ? gsInnerLoopCount - 2 : 0;
	assert( 3 == m_verticesPerPrimitive ); // this code assumes that triangle strips are being used

	// Each invocation emits numGSPrimitivesEmittedPerInvocation primitives, so each draw emits
	// the following number of primitives
	const UINT numGSEmittedPrimitivesPerDraw = numGSPrimitivesEmittedPerInvocation * m_numInputPrimitives;

	// The total number of primitives emitted by the GS
	const UINT maxPossiblePrimitives = numGSEmittedPrimitivesPerDraw * m_numDrawCalls;

	UINT bufferSizeRemaining[4];
	UINT numExpectedPrimitives = 0;
	for( UINT i = 0; i < 4; ++i )
		bufferSizeRemaining[i] = m_soBufferSize[i] > m_soOffsets[i] ? m_soBufferSize[i] - m_soOffsets[i] : 0;

	for( UINT i = 0; i < m_numDrawCalls; ++i )
	{
		CONSTDATA *pConstData;
		D3D11_MAPPED_SUBRESOURCE mappedRes;

		// Setup constant data for draw call
		hr = GetEffectiveContext()->Map( m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
		if( FAILED( hr ) )
		{
			WriteToLog( _T( "Map() unexpectedly failed mapping dynamic constant buffer - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			return RESULT_FAIL;
		}
		pConstData = (CONSTDATA*) mappedRes.pData;

		pConstData->gsInnerLoopCount[0] = m_gsInnerLoopCount;
		pConstData->drawIndex[0] = i;
		GetEffectiveContext()->Unmap(m_pCB,0);

		const UINT inputVertexCount = GetInputVertexCount( m_numInputPrimitives );
		assert( numVerts >= inputVertexCount ); // ensure that the vertex buffer contains enough data for the vertices

		GetEffectiveContext()->Draw( inputVertexCount , 0 );

		UINT numExpectedPrimitivesThisDraw = numGSEmittedPrimitivesPerDraw;

		// Decrease the number of primitives emitted by this draw call according to the SO output buffer size
		UINT primitiveSize[4];
		for( UINT j = 0; j < 4; ++j )
		{
			primitiveSize[j] = m_verticesPerPrimitive * soElementWidth[j] * sizeof( UINT );
			if( soElementWidth[j] > 0 )
				numExpectedPrimitivesThisDraw = min( bufferSizeRemaining[j] / primitiveSize[j], numExpectedPrimitivesThisDraw );
		}
		
		if( numExpectedPrimitivesThisDraw == 0 )
			break; // No more primitives can be streamed out
		
		numExpectedPrimitives += numExpectedPrimitivesThisDraw;
		for( UINT j = 0; j < numEntries; ++j )
		{
			if( soElementWidth[j] > 0 )
				bufferSizeRemaining[j] -= numExpectedPrimitivesThisDraw * primitiveSize[j];
		}
	}
	GetEffectiveContext()->End( m_pSOOverflowPredicate );
	GetEffectiveContext()->End( m_pSOStatsQuery );


	// execute any deferred operations
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext failed. hr = %s", D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	//
	// Verify the expected number of output primitives
	//
	size_t size = m_pSOStatsQuery->GetDataSize();
	if( size != sizeof( D3D11_QUERY_DATA_SO_STATISTICS ) )
	{
		WriteToLog( _T( "GetnumDataElements() return value incorrect - expected: %i, actual %i" ), sizeof( D3D11_QUERY_DATA_SO_STATISTICS ), size );
		return RESULT_FAIL;
	}

	D3D11_QUERY_DATA_SO_STATISTICS soStats;
	hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pSOStatsQuery, &soStats, size, 0 );
	if (hr != S_OK)
	{
		WriteToLog( _T( "GetData() failed or timed out, hr=%x" ), hr );
		return RESULT_FAIL;
	}

	//
	// Check overflow status
	//
	BOOL overflow;
	BOOL expectOverflow = maxPossiblePrimitives != numExpectedPrimitives;
	hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pSOOverflowPredicate, &overflow, sizeof( BOOL ), 0 );
	if (hr != S_OK)
	{
		WriteToLog( _T( "GetData() failed or timed out, hr=%x" ), hr );
		return RESULT_FAIL;
	}


	//
	// Look at the output stream result
	//

	if( maxPossiblePrimitives != soStats.PrimitivesStorageNeeded )
	{
		WriteToLog( _T( "D3D11_QUERY_DATA_SO_STATISTICS.PrimitivesStorageNeeded incorrect - expected: %i, actual %i" ), maxPossiblePrimitives, soStats.PrimitivesStorageNeeded );
		tr = RESULT_FAIL;
	}

	if( numExpectedPrimitives != soStats.NumPrimitivesWritten )
	{
		WriteToLog( _T( "D3D11_QUERY_DATA_SO_STATISTICS.NumPrimitivesWritten incorrect - expected: %i, actual %i" ), numExpectedPrimitives, soStats.NumPrimitivesWritten );
		tr = RESULT_FAIL;
	}

	if( expectOverflow && !overflow )
	{
		WriteToLog( _T( "Stream out overflow predicate failed to indicate expected buffer overflow" ) );
		tr = RESULT_FAIL;
	}

	if( overflow && !expectOverflow )
	{
		WriteToLog( _T( "Stream out overflow predicate incorrectly indicated buffer overflow" ) );
		tr = RESULT_FAIL;
	}
	
	D3D11_BUFFER_DESC tempdesc;
	tempdesc.BindFlags = 0;
	tempdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	tempdesc.MiscFlags = 0;
	tempdesc.Usage = D3D11_USAGE_STAGING;
	
	ID3D11Buffer *pTempBuffers[4] = { 0 };
	UINT *pSOBufferCopy[4] = { 0 };

	for( UINT i = 0; i < 4; ++i )
	{
		if( m_soBufferSize[i] )
		{
			tempdesc.ByteWidth = m_soBufferDesc[i].ByteWidth;

			hr = GetDevice()->CreateBuffer( &tempdesc, NULL, &pTempBuffers[i] );
			if( FAILED( hr ) )
			{
				WriteToLog( _T( "CreateBuffer() unexpectedly failed creating temporary buffer - result code '%s'" ), D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}

			GetImmediateContext()->CopyResource( pTempBuffers[i], m_soBuffers[i] );

			D3D11_MAPPED_SUBRESOURCE mappedRes;
			hr = GetImmediateContext()->Map( pTempBuffers[i], 0, D3D11_MAP_READ, 0, &mappedRes );
			if( FAILED( hr ) )
			{
				WriteToLog( _T( "ID3D11Buffer::Map() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}
			pSOBufferCopy[i] = (UINT*)mappedRes.pData;
		}
	}
	
	UINT *pSrcSOVerts[4];
	UINT *pSrcSOVertsLast[4];
	pSrcSOVerts[0] = pSOBufferCopy[0];
	pSrcSOVerts[1] = pSOBufferCopy[1];
	pSrcSOVerts[2] = pSOBufferCopy[2];
	pSrcSOVerts[3] = pSOBufferCopy[3];
	pSrcSOVertsLast[0] = (UINT *) ( (char *) pSOBufferCopy[0] + m_soBufferSize[0] ) - 1;
	pSrcSOVertsLast[1] = (UINT *) ( (char *) pSOBufferCopy[1] + m_soBufferSize[1] ) - 1;
	pSrcSOVertsLast[2] = (UINT *) ( (char *) pSOBufferCopy[2] + m_soBufferSize[2] ) - 1;
	pSrcSOVertsLast[3] = (UINT *) ( (char *) pSOBufferCopy[3] + m_soBufferSize[3] ) - 1;
	
	//
	// Verify buffer whitespace before data
	//
	for( UINT i = 0; i < numEntries; ++i )
	{
		int slot = soDecl[i].OutputSlot;

		if( m_soBufferSize[slot] )
		{
			TEST_RESULT trtemp = RESULT_PASS;
			UINT *last = (UINT *) ( (char *) pSOBufferCopy[slot] + m_soOffsets[slot] ) - 1;
			if( last > pSrcSOVertsLast[slot] )
				last = pSrcSOVertsLast[slot];
			for( ; pSrcSOVerts[slot] <= last; pSrcSOVerts[slot]++ )
			{
				if( *pSrcSOVerts[slot] != MAGIC_NUMBER )
					trtemp = RESULT_FAIL;
			}
			
			if( trtemp == RESULT_FAIL )
			{
				WriteToLog( _T( "Data corruption in offset region of SO buffer in slot %i" ), slot );
				tr = RESULT_FAIL;
			}
		}
	}
	
	//
	// Verify GS output
	//
	
	GSOUT primitives[24];
	assert( numGSPrimitivesEmittedPerInvocation <= ARRAY_SIZE( primitives ) );

	for( UINT i = 0; i < 4; ++i )
		bufferSizeRemaining[i] = m_soBufferSize[i] > m_soOffsets[i] ? m_soBufferSize[i] - m_soOffsets[i] : 0;
	
	UINT errorStart[4] = { -1, -1, -1, -1 };
	UINT elementIndex[4] = { 0 };

	UINT numVerticesChecked = 0;
	UINT numVerticesExpected = numExpectedPrimitives * m_verticesPerPrimitive;
	
	// For each Draw() call...
	for( UINT i = 0; i < m_numDrawCalls; ++i )
	{
		errorStart[0] =
		errorStart[1] =
		errorStart[2] =
		errorStart[3] = (UINT) -1;

		// For each input primitive...
		for( UINT j = 0; j < m_numInputPrimitives; ++j )
		{
			UINT inputIndices[32];
			GetVertexIndices(j, m_numInputPrimitives, inputIndices);

			// Calculate VS output
			GSIN gsin[32];

			for( UINT inputVtx = 0; inputVtx < numInputVertsPerInvocation; inputVtx++ )
			{
				gsin[inputVtx].pos[0] = pInputVertexData[ inputIndices[inputVtx] ].pos[0];
				gsin[inputVtx].pos[1] = pInputVertexData[ inputIndices[inputVtx] ].pos[1];
				gsin[inputVtx].pos[2] = pInputVertexData[ inputIndices[inputVtx] ].pos[2];
				gsin[inputVtx].pos[3] = pInputVertexData[ inputIndices[inputVtx] ].pos[3];
				gsin[inputVtx].data   = Encode( pInputVertexData[ inputIndices[inputVtx] ].data );
			}

			// Calculate
			CalculateGSOut( gsin, primitives, i, numInputVertsPerInvocation, numGSPrimitivesEmittedPerInvocation );

			const UINT numVerticesToCheck = numGSPrimitivesEmittedPerInvocation * m_verticesPerPrimitive;

			for( UINT k = 0; k < numVerticesToCheck; k++ )
			{
				TEST_RESULT trtemp = RESULT_PASS;
				UINT *refElement = NULL;

				// don't check buffer contents past so overflow
				if( numVerticesChecked >= numVerticesExpected )
					break;

				numVerticesChecked++;

				for( UINT n = 0; n < numEntries; ++n )
				{
					UINT startComponent = soDecl[n].StartComponent;
					UINT componentCount= soDecl[n].ComponentCount;
					UINT slot = soDecl[n].OutputSlot;

					if( m_soBufferSize[slot] == 0 )
						continue;

					switch( soDecl[n].SemanticName[5] )
					{
					case 'a':
						refElement = &primitives[k].data_a[startComponent];
						break;
					case 'b':
						refElement = &primitives[k].data_b[startComponent];
						break;
					case 'c':
						refElement = &primitives[k].data_c[startComponent];
						break;
					case 'd':
						refElement = &primitives[k].data_d[startComponent];
						break;
					}

					for( UINT m = 0; m < componentCount; ++m )
					{
						if( *refElement != pSrcSOVerts[slot][m] )
							trtemp = RESULT_FAIL;
						refElement++;
					}

					if( trtemp == RESULT_FAIL )
					{
						if( errorStart[slot] == (UINT) -1 )
							errorStart[slot] = elementIndex[slot];
						tr = RESULT_FAIL;
					}
					else
					{
						if( errorStart[slot] != (UINT) -1 )
						{
							UINT s = errorStart[slot];
							errorStart[slot] = (UINT) -1;
							WriteToLog( _T( "%i incorrect elements in stream out data - slot %i, Draw() pass %i, starting at element offset %i" ), elementIndex[slot] - s, slot, i, errorStart[i] );
						}
					}


					elementIndex[slot]++;

					pSrcSOVerts[slot] += soDecl[n].ComponentCount;
				}
			}
		}

		for( UINT slot = 0; slot < 4; ++slot )
		{
			if( errorStart[slot] != (UINT) -1 )
			{
				UINT s = errorStart[slot];
				errorStart[slot] = (UINT) -1;
				WriteToLog( _T( "%u incorrect elements in stream out data - slot %i, Draw() pass %i, starting at element offset %u" ), elementIndex[slot] - errorStart[slot], slot, i, s );
				tr = RESULT_FAIL;
			}
		}
	}
	
	
	//
	// Verify buffer whitespace after streamed data
	//
	for( UINT i = 0; i < numEntries; ++i )
	{
		_DPF( 0, "%i\n", i );

		int slot = soDecl[i].OutputSlot;

		pSrcSOVerts[slot] = pSOBufferCopy[slot] + m_soOffsets[slot] + m_numDrawCalls * numGSEmittedPrimitivesPerDraw * m_verticesPerPrimitive * soDecl[i].ComponentCount;
		if( m_soBufferSize[slot] )
		{
			TEST_RESULT trtemp = RESULT_PASS;
			UINT *last = pSrcSOVertsLast[slot];
			for( ; pSrcSOVerts[slot] <= last; pSrcSOVerts[slot]++ )
			{
				if( *pSrcSOVerts[slot] != MAGIC_NUMBER )
					trtemp = RESULT_FAIL;
			}
			
			if( trtemp == RESULT_FAIL )
			{
				WriteToLog( _T( "Data corruption in region after streamed data of SO buffer in slot %i" ), slot );
				tr = RESULT_FAIL;
			}
		}
	}
	
	if( pTempBuffers[0] ) GetImmediateContext()->Unmap( pTempBuffers[0], 0 );
	if( pTempBuffers[1] ) GetImmediateContext()->Unmap( pTempBuffers[1], 0 );
	if( pTempBuffers[2] ) GetImmediateContext()->Unmap( pTempBuffers[2], 0 );
	if( pTempBuffers[3] ) GetImmediateContext()->Unmap( pTempBuffers[3], 0 );
	
	SAFE_RELEASE( pTempBuffers[0] );
	SAFE_RELEASE( pTempBuffers[1] );
	SAFE_RELEASE( pTempBuffers[2] );
	SAFE_RELEASE( pTempBuffers[3] );

	return tr;
}

void CStreamOutMultiBuffer::CalculateGSOut( const GSIN *in, GSOUT *out, UINT drawIndex, UINT numInputVertsPerInvocation, UINT numOutputPrimitives )
{
	GSOUT gsoutverts[32];
	assert( m_gsInnerLoopCount <= ARRAY_SIZE(gsoutverts) );

	for( UINT i = 0; i < m_gsInnerLoopCount; ++i )
	{
		UINT inputVtxIdx = (i * 17) % numInputVertsPerInvocation;

		UINT n = drawIndex + i + in[inputVtxIdx].data;
		
		gsoutverts[i].pos[0] = in[inputVtxIdx].pos[0] + (float) ( i );
		gsoutverts[i].pos[1] = in[inputVtxIdx].pos[1] + (float) ( i & 1 );
		gsoutverts[i].pos[2] = in[inputVtxIdx].pos[2];
		gsoutverts[i].pos[3] = in[inputVtxIdx].pos[3];

		gsoutverts[i].data_a[0] = n;
		n = Encode( n );
		gsoutverts[i].data_a[1] = n;
		n = Encode( n );
		gsoutverts[i].data_a[2] = n;
		n = Encode( n );
		gsoutverts[i].data_a[3] = n;

		n = Encode( n );
		gsoutverts[i].data_b[0] = n;
		n = Encode( n );
		gsoutverts[i].data_b[1] = n;
		n = Encode( n );
		gsoutverts[i].data_b[2] = n;
		n = Encode( n );
		gsoutverts[i].data_b[3] = n;

		n = Encode( n );
		gsoutverts[i].data_c[0] = n;
		n = Encode( n );
		gsoutverts[i].data_c[1] = n;
		n = Encode( n );
		gsoutverts[i].data_c[2] = n;
		n = Encode( n );
		gsoutverts[i].data_c[3] = n;

		n = Encode( n );
		gsoutverts[i].data_d[0] = n;
		n = Encode( n );
		gsoutverts[i].data_d[1] = n;
		n = Encode( n );
		gsoutverts[i].data_d[2] = n;
		n = Encode( n );
		gsoutverts[i].data_d[3] = n;
	}

	assert( 3 == m_verticesPerPrimitive );
	
	// Build output triangles
	for( UINT n = 0; n < numOutputPrimitives; n += 2)
	{
		// assemble a quad
		memcpy( &out[0], &gsoutverts[n],     sizeof( GSOUT ) );
		memcpy( &out[1], &gsoutverts[n + 1], sizeof( GSOUT ) );
		memcpy( &out[2], &gsoutverts[n + 2], sizeof( GSOUT ) );
						 
		if( (n + 1) < numOutputPrimitives )
		{
			memcpy( &out[3], &gsoutverts[n + 1], sizeof( GSOUT ) );
			memcpy( &out[4], &gsoutverts[n + 3], sizeof( GSOUT ) );
			memcpy( &out[5], &gsoutverts[n + 2], sizeof( GSOUT ) );
		}

		out += 6;
	}
}

UINT CStreamOutMultiBuffer::GetInputVertexCount( UINT numPrimitives )
{
	UINT result = 0;

	switch( m_inputTopology )
	{
	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		result = numPrimitives;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		result = numPrimitives * 2;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
		result = numPrimitives + 1;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		result = numPrimitives * 3;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		result = numPrimitives + 2;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		result = numPrimitives + 3;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		result = numPrimitives * 4;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		result = numPrimitives * 6;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		result = 2 * (numPrimitives + 2);
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		result = ControlPointCount( m_inputTopology ) * numPrimitives;
		break;
	default:
		assert( false );
	}

	return result;
}

// Given a primitive index, this returns indices into the vertex buffer of the vertices that form the primitive
void CStreamOutMultiBuffer::GetVertexIndices(UINT primitiveIdx, UINT primitiveCount, UINT indicesOut[32])
{
	switch( m_inputTopology )
	{
	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		indicesOut[0] = primitiveIdx;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		indicesOut[0] = primitiveIdx * 2 + 0;
		indicesOut[1] = primitiveIdx * 2 + 1;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
		indicesOut[0] = primitiveIdx + 0;
		indicesOut[1] = primitiveIdx + 1;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		indicesOut[0] = primitiveIdx * 3 + 0;
		indicesOut[1] = primitiveIdx * 3 + 1;
		indicesOut[2] = primitiveIdx * 3 + 2;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		indicesOut[0] = primitiveIdx + 0;
		indicesOut[1] = primitiveIdx + 1;
		indicesOut[2] = primitiveIdx + 2;
		if(primitiveIdx % 2)
		{
			std::swap( indicesOut[1], indicesOut[2] );
		}
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		indicesOut[0] = primitiveIdx + 0;
		indicesOut[1] = primitiveIdx + 1;
		indicesOut[2] = primitiveIdx + 2;
		indicesOut[3] = primitiveIdx + 3;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		indicesOut[0] = primitiveIdx * 4 + 0;
		indicesOut[1] = primitiveIdx * 4 + 1;
		indicesOut[2] = primitiveIdx * 4 + 2;
		indicesOut[3] = primitiveIdx * 4 + 3;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		indicesOut[0] = primitiveIdx * 6 + 0;
		indicesOut[1] = primitiveIdx * 6 + 1;
		indicesOut[2] = primitiveIdx * 6 + 2;
		indicesOut[3] = primitiveIdx * 6 + 3;
		indicesOut[4] = primitiveIdx * 6 + 4;
		indicesOut[5] = primitiveIdx * 6 + 5;
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		{
			// this one is complicated,
			// Simulate the behavior of submitting all vertices to the input assembler
			UINT totalVertexCount = 2 * (primitiveCount + 2);

			bool oddPrimitive = true;
			UINT numQueuedIndices = 0;
			UINT indexQueue[8];
			UINT numEmittedPrimitives = 0;

			for( UINT v = 0; v < totalVertexCount; v++ )
			{
				assert( numQueuedIndices < ARRAY_SIZE( indexQueue ) );
				indexQueue[ numQueuedIndices ] = v;
				numQueuedIndices++;

				if( 8 == numQueuedIndices )
				{
					if(oddPrimitive)
					{
						indicesOut[0] = indexQueue[0];
						indicesOut[1] = indexQueue[1];
						indicesOut[2] = indexQueue[2];
						indicesOut[3] = indexQueue[6];
						indicesOut[4] = indexQueue[4];
						indicesOut[5] = indexQueue[3];
					}
					else
					{
						indicesOut[0] = indexQueue[0];
						indicesOut[1] = indexQueue[3];
						indicesOut[2] = indexQueue[4];
						indicesOut[3] = indexQueue[6];
						indicesOut[4] = indexQueue[2];
						indicesOut[5] = indexQueue[1];
					}

					UINT newIndexQueue[6];

					newIndexQueue[0] = indexQueue[2];
					newIndexQueue[1] = indexQueue[0];
					newIndexQueue[2] = indexQueue[4];
					newIndexQueue[3] = indexQueue[5];
					newIndexQueue[4] = indexQueue[6];
					newIndexQueue[5] = indexQueue[7];

					for(UINT i = 0; i < 6; i++)
					{
						indexQueue[i] = newIndexQueue[i];
					}

					numQueuedIndices = 6;
					oddPrimitive = !oddPrimitive;

					if( numEmittedPrimitives == primitiveIdx )
					{
						return;
					}
					else
					{
						numEmittedPrimitives++;
					}
				}
			}

			// flush incomplete primitives
			if( numQueuedIndices >= 6 )
			{
				if(oddPrimitive)
				{
					indicesOut[0] = indexQueue[0];
					indicesOut[1] = indexQueue[1];
					indicesOut[2] = indexQueue[2];
					indicesOut[3] = indexQueue[5];
					indicesOut[4] = indexQueue[4];
					indicesOut[5] = indexQueue[3];
				}
				else
				{
					indicesOut[0] = indexQueue[0];
					indicesOut[1] = indexQueue[3];
					indicesOut[2] = indexQueue[4];
					indicesOut[3] = indexQueue[5];
					indicesOut[4] = indexQueue[2];
					indicesOut[5] = indexQueue[1];
				}

				assert( numEmittedPrimitives == primitiveIdx );
			}
		}
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
	case D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		{
			UINT numControlPoints = ControlPointCount( m_inputTopology );
			assert( numControlPoints <= 32 );

			for( UINT i = 0; i < numControlPoints; i++ )
			{
				indicesOut[i] = primitiveIdx * numControlPoints + i;
			}
		}
		break;

	default:
		assert( false );
	}
}