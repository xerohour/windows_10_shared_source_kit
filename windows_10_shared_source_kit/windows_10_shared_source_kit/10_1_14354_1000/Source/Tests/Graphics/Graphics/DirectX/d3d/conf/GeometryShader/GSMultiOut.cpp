// ***************************************************************
//  GSMultiOut   version:  1.0   ·  date: 08/30/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#include "GSMultiOut.h"
#include "GeometryShader.h"
#include <cmath>


struct MO_VERTEX
{
	UINT numOut;
	UINT restartFreq;
	UINT beginRestarts;
	UINT endRestarts;
};

struct SOSTRUCT
{
	float	pos[4];
	UINT	primID;
	float	color[4];
	float	normal[4];
	float	tangent[4];
	float	bitangent[4];
};

BEGIN_NAMED_VALUES(eVertexSize)
	NAMED_VALUE( _T( "TINY" ), VertSize_Tiny )
	NAMED_VALUE( _T( "NORMAL" ), VertSize_Normal )
	NAMED_VALUE( _T( "HUGE" ), VertSize_Huge )
END_NAMED_VALUES(eVertexSize)

typedef unsigned char PIXEL[4];

#define MAX_VERTICES	(128)
#define MAX_INDICES		(128)
#define MAX_INSTANCES	(4)
#define MAX_GS_OUT		(1024) // hardware constant
#define RT_SIZE_X		(256)
#define RT_SIZE_Y		(33)

#define MAX_STREAM_OUT	(MAX_INSTANCES * MAX_INDICES * MAX_GS_OUT * 3 * sizeof(float)) // enough space for maxing out with triangles

static UINT MakeNumOut(UINT x)
{
	return (x+1) * 9 - 5;
}
static UINT MakeRestartFreq(UINT x)
{
	return x / 3 + x / 2;
}
static UINT MakeRestartBegin(UINT x)
{
	return MakeNumOut(x) / 3;
}
static UINT MakeRestartEnd(UINT x)
{
	return (2 * MakeNumOut(x)) / 3;
}

static void GenerateNonAdjVertices( MO_VERTEX *outVerts, UINT numVerts )
{
	for( UINT i = 0; i < numVerts; ++i )
	{
		outVerts[i].numOut = MakeNumOut(i);
		outVerts[i].restartFreq = MakeRestartFreq(i);
		outVerts[i].beginRestarts = MakeRestartBegin(i);
		outVerts[i].endRestarts = MakeRestartEnd(i);
	}
}

static void GeneratePointListIndices( UINT16 *outIndices, UINT numIndices )
{
	UINT16 *indices = outIndices;
	for( UINT i = 0; i < numIndices; ++i )
	{
		indices[i] = (UINT16)i;
	}
}



static TEST_RESULT TestPixelOn( UINT x, UINT y, PIXEL *pPixels, UINT pitch, bool reportFailure )
{
	if (x >= RT_SIZE_X || y >= RT_SIZE_Y)
	{
		assert(!"RenderTarget access out of bounds");
		return RESULT_FAIL;
	}

	TEST_RESULT tr = RESULT_PASS;
	PIXEL *pixel = (PIXEL *) ( (char *) pPixels + y * pitch ) + x;

	if( (*pixel)[3] == 0 )
	{
		if (reportFailure)
		{
			GetFramework()->WriteToLogF( "Failed to render expected primitive at pixel location %i, %i", x, y );
		}
		tr = RESULT_FAIL;
	}
	else if (0) // TODO FIXME: decide whether color is important
	{
		if( (*pixel)[0] < 200 )
		{
			if (reportFailure)
			{
				GetFramework()->WriteToLogF( "Incorrect FIRST adjacent index for primitive rendered at pixel location %i, %i", x, y );
				tr = RESULT_FAIL;
			}
		}

		if( (*pixel)[1] < 200 )
		{
			if (reportFailure)
			{
				GetFramework()->WriteToLogF( "Incorrect SECOND adjacent index for primitive rendered at pixel location %i, %i", x, y );
				tr = RESULT_FAIL;
			}
		}

		if( (*pixel)[2] < 200 )
		{
			if (reportFailure)
			{
				GetFramework()->WriteToLogF( "Incorrect THIRD adjacent index for primitive rendered at pixel location %i, %i", x, y );
				tr = RESULT_FAIL;
			}
		}
	}

	return tr;
}

static TEST_RESULT TestPixelOff( UINT x, UINT y, PIXEL *pPixels, UINT pitch )
{
	PIXEL *pixel = (PIXEL *) ( (char *) pPixels + y * pitch ) + x;
	if( (*pixel)[0] || (*pixel)[1] || (*pixel)[2] )
	{
		GetFramework()->WriteToLogF( "Unexpected pixel rendered at location %i, %i", x, y );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

static bool CompareVecN(float a[], float b[], int N, float epsilon)
{
	for (int i = 0; i < N; ++i)
		if (epsilon < fabsf(a[i] - b[i]))
			return false;
	return true;
}

static TEST_RESULT TestSOPoint( UINT soIdx, UINT primID, UINT x, UINT y, SOSTRUCT* pSOVerts, bool hasPrimID, bool reportFailure )
{
	TEST_RESULT tr = RESULT_PASS;
	SOSTRUCT* pThisPrim = &pSOVerts[soIdx];
	float expectedPos[4] = {
		( ( 2.0f * ( x + 0.6f ) ) / float( RT_SIZE_X ) ) - 1.0f,
		1.0f - ( ( 2.0f * ( y + 0.6f ) ) / float( RT_SIZE_Y ) ),
		0, 1
	};
	const bool correctPosition = CompareVecN(pThisPrim->pos, expectedPos, 4, 0.000001f);
	const bool correctPrimID = (!hasPrimID) || (pThisPrim->primID == primID);
	if ( !(correctPosition && correctPrimID) )
	{
		TCHAR expectedBuffer[1024], foundBuffer[1024];
		expectedBuffer[0] = 0;
		foundBuffer[0] = 0;
		if (!correctPosition)
		{
			_sntprintf( expectedBuffer, sizeof( expectedBuffer ) - 1, _T( " (%g, %g, %g, %g)" ), 
				expectedPos[0], expectedPos[1], expectedPos[2], expectedPos[3] 
			);
			_sntprintf( foundBuffer, sizeof( foundBuffer ) - 1, _T( " (%g, %g, %g, %g)" ), 
				pThisPrim->pos[0], pThisPrim->pos[1], pThisPrim->pos[2], pThisPrim->pos[3] 
			);
		}
		if (!correctPrimID)
		{
			_sntprintf( expectedBuffer, sizeof( expectedBuffer ) - 1, _T( "%s primID=%i" ), 
				expectedBuffer,
				primID 
			);
			_sntprintf( foundBuffer, sizeof( foundBuffer ) - 1, _T( "%s primID=%i" ), 
				foundBuffer,
				pThisPrim->primID 
			);
		}
		expectedBuffer[1023] = 0;
		foundBuffer[1023] = 0;
		if (reportFailure)
		{
			GetFramework()->WriteToLogF( "Incorrect streamout value for index %i. Expected%s. Found%s.", 
				soIdx,
				expectedBuffer,
				foundBuffer
				);
		}
		tr = RESULT_FAIL;
	}
	return tr;
}

static TEST_RESULT TestSOLine( UINT soIdx, UINT primID, UINT x, UINT y, SOSTRUCT* pSOVerts, bool hasPrimID, bool reportFailure )
{
	TEST_RESULT tr = RESULT_PASS;
	for (int n = 0; n < 2; ++n)
	{
		SOSTRUCT* pThisPrim = &pSOVerts[soIdx + n];
		float expectedPos[4] = {
			( ( 2.0f * ( x + n + 0.5f ) ) / float( RT_SIZE_X ) ) - 1.0f,
			1.0f - ( ( 2.0f * ( y + 0.5f ) ) / float( RT_SIZE_Y ) ),
			0, 1
		};
		const bool correctPosition = CompareVecN(pThisPrim->pos, expectedPos, 4, 0.000001f);
		const bool correctPrimID = (!hasPrimID) || (pThisPrim->primID == primID);
		if ( !(correctPosition && correctPrimID) )
		{
			TCHAR expectedBuffer[1024], foundBuffer[1024];
			expectedBuffer[0] = 0;
			foundBuffer[0] = 0;
			if (!correctPosition)
			{
				_sntprintf( expectedBuffer, sizeof( expectedBuffer ) - 1, _T( " (%g, %g, %g, %g)" ), 
					expectedPos[0], expectedPos[1], expectedPos[2], expectedPos[3] 
				);
				_sntprintf( foundBuffer, sizeof( foundBuffer ) - 1, _T( " (%g, %g, %g, %g)" ), 
					pThisPrim->pos[0], pThisPrim->pos[1], pThisPrim->pos[2], pThisPrim->pos[3] 
				);
			}
			if (!correctPrimID)
			{
				_sntprintf( expectedBuffer, sizeof( expectedBuffer ) - 1, _T( "%s primID=%i" ), 
					expectedBuffer,
					primID 
					);
				_sntprintf( foundBuffer, sizeof( foundBuffer ) - 1, _T( "%s primID=%i" ), 
					foundBuffer,
					pThisPrim->primID 
					);
			}
			expectedBuffer[1023] = 0;
			foundBuffer[1023] = 0;
			if (reportFailure)
			{
				GetFramework()->WriteToLogF( "Incorrect streamout value for index %i. Expected%s. Found%s.", 
					soIdx + n,
					expectedBuffer,
					foundBuffer
					);
			}
			tr = RESULT_FAIL;
		}
	}
	return tr;
}

static TEST_RESULT TestSOTriangle( UINT soIdx, UINT primID, UINT x, UINT y, SOSTRUCT* pSOVerts, bool hasPrimID, bool bClockwise, bool reportFailure )
{
	TEST_RESULT tr = RESULT_PASS;
	for (int k = 0; k < 3; ++k)
	{
		int n = k;
		if (bClockwise && n > 0)
			n = 1 + (2 - n);
		SOSTRUCT* pThisPrim = &pSOVerts[soIdx + k];
		int row, col;
		if (y%2 == 0)
		{
			col = x + ((n+1) >> 1);
			row = y + 2 * (n >> 1);
		}
		else
		{
			col = x + (n >> 1);
			row = (y&~1) + 2 * (1 - (n & 1));
		}
		float expectedPos[4] = {
			( ( 2.0f * ( col + 0.5f ) ) / float( RT_SIZE_X ) ) - 1.0f,
			1.0f - ( ( 2.0f * ( row + 0.5f ) ) / float( RT_SIZE_Y ) ),
			0, 1
		};
		const bool correctPosition = CompareVecN(pThisPrim->pos, expectedPos, 4, 0.000001f);
		const bool correctPrimID = (!hasPrimID) || (pThisPrim->primID == primID);
		if ( !(correctPosition && correctPrimID) )
		{
			TCHAR expectedBuffer[1024], foundBuffer[1024];
			expectedBuffer[0] = 0;
			foundBuffer[0] = 0;
			if (!correctPosition)
			{
				_sntprintf( expectedBuffer, sizeof( expectedBuffer ) - 1, _T( " (%g, %g, %g, %g)" ), 
					expectedPos[0], expectedPos[1], expectedPos[2], expectedPos[3] 
				);
				_sntprintf( foundBuffer, sizeof( foundBuffer ) - 1, _T( " (%g, %g, %g, %g)" ), 
					pThisPrim->pos[0], pThisPrim->pos[1], pThisPrim->pos[2], pThisPrim->pos[3] 
				);
			}
			if (!correctPrimID)
			{
				_sntprintf( expectedBuffer, sizeof( expectedBuffer ) - 1, _T( "%s primID=%i" ), 
					expectedBuffer,
					primID 
					);
				_sntprintf( foundBuffer, sizeof( foundBuffer ) - 1, _T( "%s primID=%i" ), 
					foundBuffer,
					pThisPrim->primID 
					);
			}
			expectedBuffer[1023] = 0;
			foundBuffer[1023] = 0;
			if (reportFailure)
			{
				GetFramework()->WriteToLogF( "Incorrect streamout value for index %i. Expected%s. Found%s.", 
					soIdx + k,
					expectedBuffer,
					foundBuffer
					);
			}
			tr = RESULT_FAIL;
		}
	}
	return tr;
}

//////////////////////////////////////////////////////////////////////////

CGSMultiOutTest::CGSMultiOutTest() 
	: m_topology( D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED )
	, m_bStreamOut( false )
	, m_bRasterize( false )
	, m_pSOBuffer( NULL )
	, m_pInputVertsPoint( NULL )
	, m_pInputVertsLine( NULL )
	, m_pInputVertsTriangle( NULL )
	, m_pInputIndices( NULL )
	, m_numIndices( 0 )
	, m_VertexSize( VertSize_Tiny )
{
	ZeroMemory(m_pGSPoint, sizeof(m_pGSPoint));
	ZeroMemory(m_pGSLine, sizeof(m_pGSLine));
	ZeroMemory(m_pGSTriangle, sizeof(m_pGSTriangle));
}

CGSMultiOutTest::~CGSMultiOutTest()
{
}

static D3D11_PRIMITIVE_TOPOLOGY topologyTable[] =
{
	D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
	D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
	//D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	//D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	//D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
	//D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
	//D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
	//D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
};


void CGSMultiOutTest::InitTestParameters()
{
	CTestCaseParameter< D3D11_PRIMITIVE_TOPOLOGY > *pTopologyParam = AddParameter<D3D11_PRIMITIVE_TOPOLOGY>( _T( "InputTopology" ), &m_topology );
	CTestCaseParameter< UINT > *pNumIndicesParam = AddParameter< UINT >( _T( "NumIndices" ), &m_numIndices );
	CTestCaseParameter< eVertexSize > *pVertexSizeParam = AddParameter< eVertexSize >( _T( "VertexSize" ), &m_VertexSize );
	CTestCaseParameter< UINT > *pNumInstancesParam = AddParameter< UINT >( _T( "NumInstances" ), &m_numInstances );

	CTableValueSet< D3D11_PRIMITIVE_TOPOLOGY > *pTopologyValueSet = new CTableValueSet< D3D11_PRIMITIVE_TOPOLOGY >( topologyTable, sizeof( topologyTable[0] ), sizeof( topologyTable ) / sizeof( topologyTable[0] ) );
	CUserValueSet< UINT > *pNumIndicesValueSet = new CUserValueSet< UINT >(1, 4, 17, 32);
	CUserValueSet< eVertexSize > *pVertexSizeValueSet = new CUserValueSet< eVertexSize >( VertSize_Tiny, VertSize_Normal, VertSize_Huge );
	CRangeValueSet< UINT > *pNumInstancesValueSet = new CRangeValueSet< UINT >( 0, 4, 1 );

	testfactor::RFactor rTopology = AddParameterValueSet( pTopologyParam, pTopologyValueSet );
	testfactor::RFactor rNumIndices = AddParameterValueSet( pNumIndicesParam, pNumIndicesValueSet );
	testfactor::RFactor rVertexSize = AddParameterValueSet( pVertexSizeParam, pVertexSizeValueSet );
	testfactor::RFactor rNumInstances = AddParameterValueSet( pNumInstancesParam, pNumInstancesValueSet );

	testfactor::RFactor rPointTopology = AddParameterValue(pTopologyParam, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	testfactor::RFactor rLineTopology = AddParameterValue(pTopologyParam, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	testfactor::RFactor rTriTopology = AddParameterValue(pTopologyParam, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_bRasterize = true;
	m_bStreamOut = true;

	SetRootTestFactor( rTopology * rVertexSize * rNumIndices * rNumInstances );

	AddPriorityPatternFilter(
		FilterNotEqual<UINT>(pNumIndicesParam, 17) &&
		FilterNotEqual<UINT>(pNumIndicesParam, 4 ), 
		0.5f);

	AddPriorityPatternFilter(FilterNotEqual<eVertexSize>(pVertexSizeParam, VertSize_Normal), 0.5f );

	AddPriorityPatternFilter(
		FilterNotEqual<UINT>(pNumInstancesParam, 0) &&
		FilterNotEqual<UINT>(pNumInstancesParam, 3),
		0.5f );
	AddPriorityPatternFilter(FilterEqual<UINT>(pNumInstancesParam, 1), 0.5f);

	AddPriorityPatternFilter(FilterNotEqual<D3D11_PRIMITIVE_TOPOLOGY>(pTopologyParam, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST), 0.5f);

	DeclarePriorityLevelByPriorityValue( 1, 1.0f );
}

TEST_RESULT CGSMultiOutTest::Setup()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;
	D3D11_BUFFER_DESC bufDesc;

	//
	// Resize the output window
	//
	GetFramework()->m_RenderTargetOptions.SwapChainDesc.Height = RT_SIZE_Y;
	GetFramework()->m_RenderTargetOptions.SwapChainDesc.Width = RT_SIZE_X;
	GetFramework()->m_RenderTargetOptions.FullscreenDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	GetFramework()->CleanupRenderTarget();
	GetFramework()->SetupRenderTarget();

	tr = CommonSetup();
	if( tr != RESULT_PASS )
		return tr;

	//
	// Create vertex buffers for each topology class
	//
	MO_VERTEX stagingVerts[MAX_VERTICES];
	D3D11_SUBRESOURCE_DATA data;

	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.ByteWidth = MAX_VERTICES * sizeof( MO_VERTEX );
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.Usage = D3D11_USAGE_IMMUTABLE;

	// Point
	GenerateNonAdjVertices( stagingVerts, MAX_VERTICES );
	data.pSysMem = stagingVerts;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	hr = GetDevice()->CreateBuffer( &bufDesc, &data, &m_pInputVertsPoint );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreateBuffer() unexpectedly failed to create vertex buffer - result code %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	// Line
	data.pSysMem = stagingVerts;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	hr = GetDevice()->CreateBuffer( &bufDesc, &data, &m_pInputVertsLine );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreateBuffer() unexpectedly failed to create vertex buffer - result code %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	// Triangle
	data.pSysMem = stagingVerts;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	hr = GetDevice()->CreateBuffer( &bufDesc, &data, &m_pInputVertsTriangle );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreateBuffer() unexpectedly failed to create vertex buffer - result code %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Create index buffer
	//
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.ByteWidth = MAX_INDICES * sizeof( UINT16 );
	bufDesc.MiscFlags = 0;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	hr = GetDevice()->CreateBuffer( &bufDesc, NULL, &m_pInputIndices );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreateBuffer() unexpectedly failed to create index buffer - result code %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Create SO buffer
	//
	bufDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.ByteWidth = MAX_STREAM_OUT;
	bufDesc.MiscFlags = 0;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	hr = GetDevice()->CreateBuffer( &bufDesc, NULL, &m_pSOBuffer );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreateBuffer() unexpectedly failed to create streamout buffer - result code %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
	D3D11_SO_DECLARATION_ENTRY pointSODecl[] =
	{
		{ 0, "SV_POSITION", 0, 0, 4, 0 },
		{ 0, "primID", 0, 0, 1, 0 },
	};
	const UINT numSOElements = sizeof(pointSODecl)/sizeof(*pointSODecl);
	D3D11_SO_DECLARATION_ENTRY pointSODeclTiny[] =
	{
		{ 0, "SV_POSITION", 0, 0, 4, 0 },
	};
	const UINT numSOElementsTiny = sizeof(pointSODeclTiny)/sizeof(*pointSODeclTiny);
	D3D11_SO_DECLARATION_ENTRY pointSODeclHuge[] =
	{
		{ 0, "SV_POSITION", 0, 0, 4, 0 },
		{ 0, "primID", 0, 0, 1, 0 },
		{ 0, "color", 0, 0, 4, 0 },
		{ 0, "tangent", 0, 0, 4, 0 },
		{ 0, "normal", 0, 0, 4, 0 },
		{ 0, "bitangent", 0, 0, 4, 0 },
	};
	const UINT numSOElementsHuge = sizeof(pointSODeclHuge)/sizeof(*pointSODeclHuge);

	//
	// Setup vertex shaders
	//
	D3D11_INPUT_ELEMENT_DESC iedesc[] = 
	{
		{ "numOut", 0, DXGI_FORMAT_R32_UINT, 0, MEMBER_OFFSET(numOut, *stagingVerts), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "restartFreq", 0, DXGI_FORMAT_R32_UINT, 0, MEMBER_OFFSET(restartFreq, *stagingVerts), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "beginRestarts", 0, DXGI_FORMAT_R32_UINT, 0, MEMBER_OFFSET(beginRestarts, *stagingVerts), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "endRestarts", 0, DXGI_FORMAT_R32_UINT, 0, MEMBER_OFFSET(endRestarts, *stagingVerts), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	const UINT numInputElements = sizeof(iedesc)/sizeof(*iedesc);

	tr = SetupVSFromResource( "GSMultiOut.sh", "VSPassThrough", iedesc, sizeof(iedesc)/sizeof(*iedesc), &m_pVS, &m_pInputLayout );
	if( tr != RESULT_PASS )
		return tr;

	//
	// Setup geometry shaders
	//
	tr = SetupGSWithSOFromResource( "GSMultiOut.sh", "GSPoint", pointSODecl, numSOElements, sizeof(SOSTRUCT), &m_pGSPoint[VertSize_Normal] );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSMultiOut.sh", "GSPointTiny", pointSODeclTiny, numSOElementsTiny, sizeof(SOSTRUCT), &m_pGSPoint[VertSize_Tiny] );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSMultiOut.sh", "GSPointHuge", pointSODeclHuge, numSOElementsHuge, sizeof(SOSTRUCT), &m_pGSPoint[VertSize_Huge] );
	if( tr != RESULT_PASS )
		return tr;

	tr = SetupGSWithSOFromResource( "GSMultiOut.sh", "GSLine", pointSODecl, numSOElements, sizeof(SOSTRUCT), &m_pGSLine[VertSize_Normal] );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSMultiOut.sh", "GSLineTiny", pointSODeclTiny, numSOElementsTiny, sizeof(SOSTRUCT), &m_pGSLine[VertSize_Tiny] );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSMultiOut.sh", "GSLineHuge", pointSODeclHuge, numSOElementsHuge, sizeof(SOSTRUCT), &m_pGSLine[VertSize_Huge] );
	if( tr != RESULT_PASS )
		return tr;

	tr = SetupGSWithSOFromResource( "GSMultiOut.sh", "GSTriangle", pointSODecl, numSOElements, sizeof(SOSTRUCT), &m_pGSTriangle[VertSize_Normal] );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSMultiOut.sh", "GSTriangleTiny", pointSODeclTiny, numSOElementsTiny, sizeof(SOSTRUCT), &m_pGSTriangle[VertSize_Tiny] );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSMultiOut.sh", "GSTriangleHuge", pointSODeclHuge, numSOElementsHuge, sizeof(SOSTRUCT), &m_pGSTriangle[VertSize_Huge] );
	if( tr != RESULT_PASS )
		return tr;

	//
	// Setup pixel shaders
	//
	tr = SetupPSFromResource( "GSMultiOut.sh", "PSMain", &m_pPS );
	if( tr != RESULT_PASS )
		return tr;

	return tr;
}

void CGSMultiOutTest::Cleanup()
{
	SAFE_RELEASE( m_pInputIndices );
	SAFE_RELEASE( m_pInputVertsPoint );
	SAFE_RELEASE( m_pInputVertsLine );
	SAFE_RELEASE( m_pInputVertsTriangle );
	SAFE_RELEASE( m_pInputIndices );
	for (int n = 0; n < ARRAYSIZE(m_pGSPoint); ++n)
	{
		SAFE_RELEASE( m_pGSPoint[n] );
	}
	for (int n = 0; n < ARRAYSIZE(m_pGSLine); ++n)
	{
		SAFE_RELEASE( m_pGSLine[n] );
	}
	for (int n = 0; n < ARRAYSIZE(m_pGSTriangle); ++n)
	{
		SAFE_RELEASE( m_pGSTriangle[n] );
	}
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pSOBuffer );

	CommonCleanup();
}


TEST_RESULT CGSMultiOutTest::ExecuteTestCase()
{
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = E_FAIL;
	HRESULT hrMemTexMap = E_UNEXPECTED;
	TEST_RESULT tr = RESULT_PASS;
	TEST_RESULT trTemp = RESULT_PASS;
	ID3D11VertexShader *pVS = NULL;
	ID3D11GeometryShader *pGS = NULL;
	ID3D11PixelShader *pPS = m_pPS;
	ID3D11Buffer *pCBAdjacency = NULL;
	ID3D11Buffer *vertexBuffers[1] = {NULL};
	ID3D11Buffer *pTempBuffer = NULL;
	ID3D11Query *pSOQuery = NULL;
	ID3D11Query *pPipelineQuery = NULL;
	UINT vertexOffsets[1] = { 0 };
	UINT vertexStrides[1] = { sizeof( MO_VERTEX ) };

	//
	// Fetch the viewport dimensions
	//
	CONST_DATA *constData;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	hr = GetEffectiveContext()->Map( m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
	if (FAILED(hr))
	{
		tr = RESULT_FAIL; 
		WriteToLog( _T( "m_pCB->Map() failed. hr=%s" ), D3DHResultToString( hr ).c_str() );
	}
	constData = (CONST_DATA*)mappedRes.pData;
	constData->vpSize[0] = float( RT_SIZE_X );
	constData->vpSize[1] = float( RT_SIZE_Y );
	GetEffectiveContext()->Unmap(m_pCB,0);

	assert(m_VertexSize <= VertSize_Huge);

	ID3D11RenderTargetView* pCachedRenderTarget = NULL;
	ID3D11DepthStencilView* pCachedDepthStencil = NULL;
	GetEffectiveContext()->OMGetRenderTargets(1, &pCachedRenderTarget, &pCachedDepthStencil);

	{
		D3D11_QUERY_DESC soDesc;
		soDesc.Query = D3D11_QUERY_SO_STATISTICS;
		soDesc.MiscFlags = 0;
		hr = pDevice->CreateQuery(&soDesc, &pSOQuery);
		if (FAILED(hr))
		{
			tr = RESULT_FAIL; 
			WriteToLog( _T( "CreateQuery(SO_STATISTICS) failed. hr=%s" ), D3DHResultToString( hr ).c_str() );
			goto testDone;
		}

		soDesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
		hr = pDevice->CreateQuery(&soDesc, &pPipelineQuery);
		if (FAILED(hr))
		{
			tr = RESULT_FAIL; 
			WriteToLog( _T( "CreateQuery(PIPELINE_STATISTICS) failed. hr=%s" ), D3DHResultToString( hr ).c_str() );
			goto testDone;
		}
	}


	//
	// Update adjacency table constant buffer
	//
	//ADJACENCY_ENTRY *adjData;

	//
	// Update index buffers
	//
	UINT16 *pIndices;
	GetEffectiveContext()->Map( m_pInputIndices, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
	pIndices = (UINT16*) mappedRes.pData;

	UINT primitiveOverhead = 0;
	switch( m_topology )
	{
		// only lists
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
		// no need for adjacency here
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
	default:
		assert( !"UNEXPECTED INPUT TOPOLOGY" );
		WriteToLog( "UNEXPECTED INPUT TOPOLOGY" );
		return RESULT_FAIL;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		GeneratePointListIndices( pIndices, m_numIndices );
		vertexBuffers[0] = m_pInputVertsPoint;
		pGS = m_pGSPoint[(UINT) m_VertexSize];
		primitiveOverhead = 0;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		//GenerateLineListIndices( pIndices, m_numIndices, m_cutInterval );
		GeneratePointListIndices( pIndices, m_numIndices );
		vertexBuffers[0] = m_pInputVertsLine;
		pGS = m_pGSLine[(UINT) m_VertexSize];
		primitiveOverhead = 1;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		//GenerateTriangleListIndices( pIndices, m_numIndices, m_cutInterval );
		GeneratePointListIndices( pIndices, m_numIndices );
		vertexBuffers[0] = m_pInputVertsTriangle;
		pGS = m_pGSTriangle[(UINT) m_VertexSize];
		primitiveOverhead = 2;
		break;

	}

	GetEffectiveContext()->Unmap(m_pInputIndices,0);

	ID3D11Buffer *constBuffers[] = { m_pCB, pCBAdjacency };
	GetEffectiveContext()->GSSetConstantBuffers( 0, 2, constBuffers );

	pVS = m_pVS;

	//
	// Setup the pipeline states
	//
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pInputVertsPoint, vertexStrides, vertexOffsets );
	GetEffectiveContext()->IASetInputLayout( m_pInputLayout );
	//GetEffectiveContext()->IASetPrimitiveTopology( m_topology );
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	GetEffectiveContext()->IASetIndexBuffer( m_pInputIndices, DXGI_FORMAT_R16_UINT, 0 );

	GetEffectiveContext()->VSSetShader( pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( pGS, NULL, 0 );
	GetEffectiveContext()->PSSetShader( pPS, NULL, 0 );

	GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pCB );
	GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pCB );
	GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pCB );

	GetEffectiveContext()->RSSetState( m_pRS );

	if (m_bStreamOut)
	{
		// re-create the stream-out buffer to "clear" it. should at least get random junk
		D3D11_BUFFER_DESC soDesc;
		m_pSOBuffer->GetDesc(&soDesc);
		GetEffectiveContext()->SOSetTargets( 0, NULL, NULL );
		m_pSOBuffer->Release();
		hr = pDevice->CreateBuffer(&soDesc, NULL, &m_pSOBuffer);
		if (FAILED(hr))
		{
			tr = RESULT_FAIL; 
			WriteToLog( _T( "CreateBuffer(STREAM_OUT) failed. hr=%s" ), D3DHResultToString( hr ).c_str() );
			goto testDone;
		}
		UINT soOffset = 0;
		GetEffectiveContext()->SOSetTargets( 1, &m_pSOBuffer, &soOffset );
	}
	else
		GetEffectiveContext()->SOSetTargets( 0, NULL, NULL );

	if (m_bRasterize)
	{
		GetEffectiveContext()->OMSetDepthStencilState( m_pDSS, 0xffffffff );
		float clearColor[4] = { 0, 0, 0, 0 };
		GetEffectiveContext()->ClearRenderTargetView( GetFramework()->GetRTView(), clearColor );
	}
	else
	{
		GetEffectiveContext()->OMSetRenderTargets(0, NULL, NULL);
		GetEffectiveContext()->OMSetDepthStencilState( NULL, 0xffffffff );
	}

	GetEffectiveContext()->Begin(pSOQuery);
	GetEffectiveContext()->Begin(pPipelineQuery);
	if (m_numInstances > 0)
		GetEffectiveContext()->DrawIndexedInstanced( m_numIndices, m_numInstances, 0, 0, 0 );
	else
		GetEffectiveContext()->DrawIndexed( m_numIndices, 0, 0 );
	GetEffectiveContext()->End(pSOQuery);
	GetEffectiveContext()->End(pPipelineQuery);
	UINT64 actualNumInstances = (m_numInstances > 0) ? m_numInstances : 1;

	// Copy the back buffer to the memory surface
	if (m_bRasterize)
	{
		GetEffectiveContext()->CopyResource( m_pMemTexture, g_App.GetRTTexture() );
	}

	// execute any deferred operations
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		tr = RESULT_FAIL;
		WriteToLog("ExecuteEffectiveContext failed. hr = %s", D3DHResultToString(hr).c_str() );
		goto testDone;
	}

	// copy stream out to a staging buffer
	SOSTRUCT *pSOVerts = NULL;
	if (m_bStreamOut)
	{
		D3D11_BUFFER_DESC tempdesc;
		m_pSOBuffer->GetDesc(&tempdesc);
		//tempdesc.ByteWidth = m_numIndices * 6 * sizeof(SOSTRUCT); // plenty of room
		tempdesc.BindFlags = 0;
		tempdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tempdesc.MiscFlags = 0;
		tempdesc.Usage = D3D11_USAGE_STAGING;
		hr = pDevice->CreateBuffer( &tempdesc, NULL, &pTempBuffer );
		if( FAILED( hr ) || !pTempBuffer )
		{
			tr = RESULT_FAIL;
			WriteToLog( _T( "CreateBuffer() unexpectedly failed creating temporary buffer - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			goto testDone;
		}
		GetImmediateContext()->CopyResource( pTempBuffer, m_pSOBuffer );
		hr = GetImmediateContext()->Map( pTempBuffer, 0, D3D11_MAP_READ, 0, &mappedRes );
		pSOVerts = (SOSTRUCT*) mappedRes.pData;
		if( FAILED( hr ) || !pSOVerts)
		{
			SAFE_RELEASE(pTempBuffer);
			WriteToLog( _T( "ID3D11Buffer::Map() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto testDone;
		}
	}

	D3D11_QUERY_DATA_SO_STATISTICS soStats;
	hr = GetFramework()->GetDataWithTimeout(GetImmediateContext(), pSOQuery, &soStats, sizeof(soStats), 0 );
	if (hr != S_OK)
	{
		tr = RESULT_FAIL; 
		WriteToLog( _T( "pSOQuery->GetData() failed or timed out. hr=%s" ), D3DHResultToString( hr ).c_str() );
	}
	if (soStats.NumPrimitivesWritten < soStats.PrimitivesStorageNeeded)
	{
		tr = RESULT_FAIL; 
		WriteToLog( _T( "Buffer overflowed. Needed space for %I64d more primitives." ), soStats.PrimitivesStorageNeeded - soStats.NumPrimitivesWritten );
	}

	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineStats;
	hr = GetFramework()->GetDataWithTimeout(GetImmediateContext(), pPipelineQuery, &pipelineStats, sizeof(pipelineStats), 0 );
	if (hr != S_OK)
	{
		tr = RESULT_FAIL; 
		WriteToLog( _T( "pPipelineQuery->GetData() failed or timed out. hr=%s" ), D3DHResultToString( hr ).c_str() );
	}

	UINT64 numExpectedPrimitives = 0;
	D3D11_MAPPED_SUBRESOURCE map;
	hrMemTexMap = GetImmediateContext()->Map( m_pMemTexture, 0, D3D11_MAP_READ, 0, &map );
	UINT x = 0, y = 0;
	UINT soIdx = 0;
	UINT numFailedPixels = 0;
	UINT numFailedSOs = 0;
	switch( m_topology )
	{
	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		for (UINT n = 0; n < m_numIndices; ++n)
		{
			int numOut = MakeNumOut(n);
			switch (m_VertexSize)
			{
			case VertSize_Normal: // normal
				numOut = min(128, numOut);
				break;
			case VertSize_Tiny: // tiny
				numOut = min(256, numOut);
				break;
			case VertSize_Huge: // huge
				numOut = min(9, numOut);
				break;
			}
			numExpectedPrimitives += numOut - primitiveOverhead;
		}
		numExpectedPrimitives *= actualNumInstances;
		for (UINT instanceID = 0; instanceID < actualNumInstances; ++instanceID)
		{
			for(UINT i = 0; i < m_numIndices; ++i )
			{
				UINT numOut = MakeNumOut(i);
				switch (m_VertexSize)
				{
				case VertSize_Normal: // normal
					x = 128 * i;
					x = RT_SIZE_X * i;
					numOut = min(128, numOut);
					break;
				case VertSize_Tiny: // tiny
					x = 256 * i;
					x = RT_SIZE_X * i;
					numOut = min(256, numOut);
					break;
				case VertSize_Huge: // huge
					x = 9 * i;
					x = RT_SIZE_X * i;
					numOut = min(9, numOut);
					break;
				}
				for (UINT pixNum = 0; pixNum < numOut; ++pixNum)
				{
					{
						y = x / RT_SIZE_X;
						y = i;
						if (m_bRasterize && instanceID == 0)
						{
							trTemp = TestPixelOn( x % RT_SIZE_X, y, (PIXEL *) map.pData, map.RowPitch, numFailedPixels < 10 );
							tr = TRLatch(tr, trTemp);
							if (trTemp != RESULT_PASS)
								numFailedPixels += 1;
						}
						if (m_bStreamOut && soIdx+1 < soStats.NumPrimitivesWritten)
						{
							trTemp = TestSOPoint(soIdx, i, x % RT_SIZE_X, y, pSOVerts, (m_VertexSize!=VertSize_Tiny), numFailedSOs < 10);
							tr = TRLatch(tr, trTemp);
							if (trTemp != RESULT_PASS)
								numFailedSOs += 1;
						}
						++x;
					}
					soIdx += 1;
				}
			}
		}
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		numExpectedPrimitives = 0;
		for (UINT instanceID = 0; instanceID < actualNumInstances; ++instanceID)
		{
			for(UINT i = 0; i < m_numIndices; ++i )
			{
				UINT numOut = MakeNumOut(i);
				UINT restartFreq = MakeRestartFreq(i);
				UINT beginRestart = MakeRestartBegin(i);
				UINT endRestart = MakeRestartEnd(i);
				switch (m_VertexSize)
				{
				case VertSize_Normal: // normal
					x = 128 * i;
					x = RT_SIZE_X * i;
					numOut = min(128, numOut);
					break;
				case VertSize_Tiny: // tiny
					x = 256 * i;
					x = RT_SIZE_X * i;
					numOut = min(256, numOut);
					break;
				case VertSize_Huge: // huge
					x = 9 * i;
					x = RT_SIZE_X * i;
					numOut = min(9, numOut);
					break;
				}
				--x;
				UINT vertCount = 0;
				for (UINT pixNum = 0; pixNum < numOut; ++pixNum)
				{
					if (restartFreq > 0 && pixNum > beginRestart && pixNum < endRestart && 0 == (pixNum % restartFreq))
						vertCount = 0;
					vertCount += 1;
					if (vertCount == 2)
					{
						y = x / RT_SIZE_X;
						y = i;
						if (m_bRasterize && instanceID == 0)
						{
							trTemp = TestPixelOn( x % RT_SIZE_X, y, (PIXEL *) map.pData, map.RowPitch, numFailedPixels < 10 );
							tr = TRLatch(tr, trTemp);
							if (trTemp != RESULT_PASS)
								numFailedPixels += 1;
						}
						if (m_bStreamOut && soIdx/2 < soStats.NumPrimitivesWritten)
						{
							trTemp = TestSOLine(soIdx, i, x % RT_SIZE_X, y, pSOVerts, (m_VertexSize!=VertSize_Tiny), numFailedSOs < 10);
							tr = TRLatch(tr, trTemp);
							if (trTemp != RESULT_PASS)
								numFailedSOs += 1;
						}
						numExpectedPrimitives += 1;
						soIdx += 2;
						vertCount = 1;
					}
					++x;
				}
			}
		}
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		numExpectedPrimitives = 0;
		for (UINT instanceID = 0; instanceID < actualNumInstances; ++instanceID)
		{
			for(UINT i = 0; i < m_numIndices; ++i )
			{
				UINT numOut = MakeNumOut(i);
				UINT restartFreq = MakeRestartFreq(i);
				UINT beginRestart = MakeRestartBegin(i);
				UINT endRestart = MakeRestartEnd(i);
				switch (m_VertexSize)
				{
				case VertSize_Normal: // normal
					x = 128 * i;
					x = RT_SIZE_X * i;
					numOut = min(128, numOut);
					break;
				case VertSize_Tiny: // tiny
					x = 256 * i;
					x = RT_SIZE_X * i;
					numOut = min(256, numOut);
					break;
				case VertSize_Huge: // huge
					x = 9 * i;
					x = RT_SIZE_X * i;
					numOut = min(9, numOut);
					break;
				}
				x /= 2;
				--x;
				UINT vertCount = 0;
				bool bClockwise = false;
				for (UINT pixNum = 0; pixNum < numOut; ++pixNum)
				{
					if (restartFreq > 0 && pixNum > beginRestart && pixNum < endRestart && 0 == (pixNum % restartFreq))
					{
						vertCount = 0;
						bClockwise = (1 == (pixNum % 2));
					}
					vertCount += 1;
					if (vertCount == 3)
					{
						y = (1 - pixNum%2) + 2 * (x / RT_SIZE_X);
						y = (1 - pixNum%2) + 2 * (i/2);
						if (m_bRasterize && instanceID == 0)
						{
							trTemp = TestPixelOn( x % RT_SIZE_X, y, (PIXEL *) map.pData, map.RowPitch, numFailedPixels < 10 );
							tr = TRLatch(tr, trTemp);
							if (trTemp != RESULT_PASS)
								numFailedPixels += 1;
						}
						if (m_bStreamOut && soIdx/3 < soStats.NumPrimitivesWritten)
						{
							trTemp = TestSOTriangle(soIdx, i, x % RT_SIZE_X, y, pSOVerts, (m_VertexSize!=VertSize_Tiny), bClockwise, numFailedSOs < 10);
							tr = TRLatch(tr, trTemp);
							if (trTemp != RESULT_PASS)
								numFailedSOs += 1;
						}
						numExpectedPrimitives += 1;
						soIdx += 3;
						vertCount = 2;
					}
					if (pixNum % 2 == 1)
						++x;
				}
			}
		}
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		break;
	}

	bool bTestPipelineStats = GetFramework()->GetExecutionPriorityLevel() != 1;
	if (bTestPipelineStats)
	{
		if (numFailedPixels > 0)
			WriteToLog( _T("At least %d pixels failed."), numFailedPixels );
		if (numFailedSOs > 0)
			WriteToLog( _T("%d stream-out vertices failed."), numFailedSOs );

		if (soStats.NumPrimitivesWritten != numExpectedPrimitives)
		{
			tr = RESULT_FAIL; 
			WriteToLog( _T( "Wrong number of SO primitives output. Expected %I64d, Found %I64d." ), 
				numExpectedPrimitives, soStats.NumPrimitivesWritten );
		}
		if (pipelineStats.GSPrimitives != numExpectedPrimitives)
		{
			tr = RESULT_FAIL; 
			WriteToLog( _T( "Wrong number of GS primitives output. Expected %I64d, Found %I64d." ), 
				numExpectedPrimitives, pipelineStats.GSPrimitives );
		}
		if (pipelineStats.GSInvocations != m_numIndices * actualNumInstances)
		{
			tr = RESULT_FAIL; 
			WriteToLog( _T( "Wrong number of GS Invocations. Expected %I64d, Found %I64d." ), 
				m_numIndices * actualNumInstances, pipelineStats.GSInvocations );
		}
		if (pipelineStats.IAPrimitives < m_numIndices || pipelineStats.IAPrimitives > m_numIndices * actualNumInstances)
		{
			tr = RESULT_FAIL; 
			WriteToLog( _T( "Wrong number of IA Primitives. Expected %I64d-%I64d, Found %I64d." ), 
				(UINT64)m_numIndices, m_numIndices * actualNumInstances, pipelineStats.IAPrimitives );
		}
		if (pipelineStats.IAVertices < m_numIndices || pipelineStats.IAVertices > m_numIndices * actualNumInstances)
		{
			tr = RESULT_FAIL; 
			WriteToLog( _T( "Wrong number of IA Vertices. Expected %I64d-%I64d, Found %I64d." ), 
				(UINT64)m_numIndices, m_numIndices * actualNumInstances, pipelineStats.IAVertices );
		}
	}

	GetFramework()->Present( NULL, NULL, NULL, NULL, 0, 0 );

testDone:
	GetEffectiveContext()->OMSetRenderTargets(1, &pCachedRenderTarget, pCachedDepthStencil);
	SAFE_RELEASE(pCachedRenderTarget);
	SAFE_RELEASE(pCachedDepthStencil);
	if (m_pMemTexture && SUCCEEDED(hrMemTexMap))
		GetImmediateContext()->Unmap( m_pMemTexture, 0 );
	if (pTempBuffer)
		GetImmediateContext()->Unmap( pTempBuffer, 0 );
	SAFE_RELEASE(pTempBuffer);
	SAFE_RELEASE(pSOQuery);
	SAFE_RELEASE(pPipelineQuery);

	return tr;
}
