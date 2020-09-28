#include "Clipping.h"

// TODO: vary x,y,z relative to W
// TODO: enable/disable Z clipping
// TODO: positive/negative W interpolation
// TODO: interpolation of perspective correct & not

float g_TriDists[][4] =
{
	// Here is the 2x2 render target - (0,0) is top left - with the pixels numbered and vertex positions marked as X's:
	// X---|---X
	// | 0 | 1 |
	// |---|---|
	// | 2 | 3 |
	// X---|---X

	// Vertex ID's (see CClippingConfTest::SetupGeometry()) for index buffer
	// Vertex 0 is the top left, vertex 1 is the top right, Vertex 2 is the bottom right, Vertex 3 is the bottom left

	// Vertex dists are below:
	// { Top Left (Vert0_Dist0), Top Right (Vert1_Dist0), Bottom Right (Vert2_Dist0), Bottom Left (Vert3_Dist0) }

	//
	// IMPORTANT: The 2 triangles (Tri 0 and 1) are INDEX BUFFER vertices {0, 3, 1} and Vertices {1, 2, 0} respectively, which correspond to actual vertices {0, 1, 2} and {2, 3, 0}
	// Split into eqivalence classes.
	// IMPORTANT: These test cases follow the fill rules for triangles (through pixel center - only lit when top or left edge).
	// These values and their expected output assume 1x MSAA.
	//
	
	// No slope (dists the same)						// Clipping														// Culling
	{ f_getNaN(), f_getNaN(), f_getNaN(), f_getNaN() },	// Tri 0 is not lit, Tri 1 is not lit							// Tri 0 is culled, Tri 1 is culled
	{ -4.0f, -4.0f, -4.0f, -4.0f },						// Tri 0 is not lit, Tri 1 is not lit							// Tri 0 is culled, Tri 1 is culled
	{ 0.0f, 0.0f, 0.0f, f_getNaN() },					// Tri 0 is lit, Tri 1 is not lit (Tri 1 has NaN)				// Tri 0 is lit, Tri 1 is lit (Tri 1 has NaN -> as if < 0)
	{ 0.1f, 0.1f, 0.1f, 0.1f },							// Tri 0 is lit, Tri 1 is lit									// Tri 0 is lit, Tri 1 is lit

	// Two vertex dists are the same for both triangles (share vertex 0 and 1)
	{ -4.0, 5.0f, -4.0f, 5.0f },						// Pixels lit: 1, 2; Pixels not lit: 0, 3						// Pixels lit: 0, 1, 2, 3
	{ -4.0f, 5.0f, -4.0f, -1.0f },						// Pixels lit: 1; Pixels not lit: 0, 2, 3 (Tri 1 not above 0)	// Pixels lit: 0, 1, 3; Pixels not lit: 2 (Tri 1 is culled)
	{ -4.0f, f_getNaN(), -4.0f, 5.0f },					// Pixels lit: 2; Pixels not lit: 0, 1, 3 (Tri 0 has NaN)		// Pixels lit: 2; Pixels not lit: 0, 1, 3 (Tri 0 is culled)
	{ -1.0f, 0.5f, -1.0f, 0.5f },						// Pixels not lit: 0, 1, 2, 3									// Pixels lit: 0, 1, 2, 3
	{ -1.1f, -0.5f, -1.1f, 2.0f },						// Pixels lit: 2; Pixels not lit: 0, 1, 3 (Tri 0 not above 0)	// Pixels lit: 2; Pixels not lit: 0, 1, 3 (Tri 0 is culled)
	{ -1000.0f, 2000.0f, -1000.0f, -0.5f },				// Pixels lit: 1; Pixels not lit: 0, 2, 3 (Tri 1 not above 0)	// Pixels lit: 0, 1, 3; Pixels not lit: 2 (Tri 1 is culled)
	
	// These are shared NaN cases
	{ 0.0f, 0.0f, f_getNaN(), 0.0f },					// Pixels not lit: 0, 1, 2, 3	(Tri 0 and 1 have NaN)			// Pixels lit: 0, 1, 2, 3
	{ f_getNaN(), 0.0f, 0.0f, 0.0f },					// Pixels not lit: 0, 1, 2, 3	(Tri 0 and 1 have NaN)			// Pixels lit: 0, 1, 2, 3

	// Two vertex dists are the same for Tri 0 (Vertex 0, 1, and 3)
	{ -4.0f, 1.0f, -4.0f, -4.0f },						// Pixels not lit: 0, 1, 2, 3									// Pixels lit: 0, 1, 3; Pixels not lit: 2 (Tri 1 is culled)
	{ -0.1f, 4.0f, -0.1f, -0.1f },						// Pixels lit: 1; Pixels not lit: 0, 2, 3						// Pixels lit: 0, 1, 3; Pixels not lit: 2 (Tri 1 is culled)

	// Two vertex dists are the same for Tri 1 (Vertex 0, 1, and 2)
	{ -40.0, -40.0f, -40.0f, 10.0f },					// Pixels not lit: 0, 1, 2, 3									// Pixels lit: 2; Pixels not lit: 0, 1, 3
	{ -0.1f, -0.1f, -0.1f, 4.1f },						// Pixels lit: 2; Pixels not lit: 0, 1, 3						// Pixels lit: 2; Pixels not lit: 0, 1, 3

	// All vertex dists are different
	{ -1000.0f, -0.1f, -1000.1f, -1.3f },				// Pixels not lit: 0, 1, 2, 3 (Tri 0 and 1 not above 0)			// Pixels not lit: 0, 1, 2, 3 (Tri 0 and 1 are culled)
	{ 4.0f, -2.0f, -0.5f, 3.0f },						// Pixels lit: 0, 2, 3; Pixels not lit: 1						// Pixels lit: 0, 1, 2, 3
	{ 4.0f, -2.0f, -1.75f, 3.0f },						// Pixels lit: 0, 2; Pixels not lit: 1, 3						// Pixels lit: 0, 1, 2, 3
	{ 0.5f, -8.0f, -1.75f, 3.0f },						// Pixels lit: 2; Pixels not lit: 0, 1, 3						// Pixels lit: 0, 1, 2, 3
	{ -4.0f, 0.8f, 10.0f, -3.5f },						// Pixels lit: 1, 3; Pixels not lit: 0, 2						// Pixels lit: 0, 1, 2, 3
	{ -4.0f, 0.8f, 10.0f, f_getNaN() },					// Pixels lit: 1, 3; Pixels not lit: 0, 2						// Pixels lit: 0, 1, 2, 3
};

void
CTriangleClipTest::InitTestParameters()
{
	memset(m_ClipDists, 0, sizeof(m_ClipDists));

	bool needPriorityFilter = false;
	m_bEnableXYClip = false;
	m_bEnableZClip = false;
	m_bEnableClipDist = false;
	m_bEnableCullDist = false;

	AddParameter<int>( _T("MultiSamples"), &m_nMultisamples );
	testfactor::RFactor rSamples_1 = AddParameterValue<int>( _T("MultiSamples"), 1);
	testfactor::RFactor rSamples_2 = AddParameterValue<int>( _T("MultiSamples"), 2);
	testfactor::RFactor rSamples_4 = AddParameterValue<int>( _T("MultiSamples"), 4);
	testfactor::RFactor rSamples_16 = AddParameterValue<int>( _T("MultiSamples"), 16);
	testfactor::RFactor rSamples_All = rSamples_1 + rSamples_2 + rSamples_4;// + rSamples_16;

	AddParameter<bool>( _T("Pipeline_Stats"), &m_bEnablePipelineStats );
	SetParameterDefaultValue( _T("Pipeline_Stats"), false );
	testfactor::RFactor rPipelineStats_T = AddParameterValue<bool>( _T("Pipeline_Stats"), true);
	testfactor::RFactor rPipelineStats_F = AddParameterValue<bool>( _T("Pipeline_Stats"), false);

	AddParameter<float>( _T("Vert0_Dist0"), &m_ClipDists[0][0].a);
	AddParameter<float>( _T("Vert1_Dist0"), &m_ClipDists[1][0].a);
	AddParameter<float>( _T("Vert2_Dist0"), &m_ClipDists[2][0].a);
	AddParameter<float>( _T("Vert3_Dist0"), &m_ClipDists[3][0].a);

	// Setup factors for clip and cull tests
	const size_t numTriDists = ARRAY_SIZE(g_TriDists);
	const size_t numComponentsPerElementBytes = sizeof(g_TriDists[0]);
	assert( numTriDists >= 1 );
	assert( 4 * sizeof( float ) == numComponentsPerElementBytes  );	// Should only have 4 floats per entry (one float for each vertex)
	testfactor::RFactor rV0_D0_ClipCullDists = AddParameterValueSet( _T("Vert0_Dist0"), TableValueSet<float>( &(g_TriDists[0][0]), numComponentsPerElementBytes, numTriDists ) );
	testfactor::RFactor rV1_D0_ClipCullDists = AddParameterValueSet( _T("Vert1_Dist0"), TableValueSet<float>( &(g_TriDists[0][1]), numComponentsPerElementBytes, numTriDists ) );
	testfactor::RFactor rV2_D0_ClipCullDists = AddParameterValueSet( _T("Vert2_Dist0"), TableValueSet<float>( &(g_TriDists[0][2]), numComponentsPerElementBytes, numTriDists ) );
	testfactor::RFactor rV3_D0_ClipCullDists = AddParameterValueSet( _T("Vert3_Dist0"), TableValueSet<float>( &(g_TriDists[0][3]), numComponentsPerElementBytes, numTriDists ) );

	testfactor::RFactor rClipCullDists =	rV0_D0_ClipCullDists % 
											rV1_D0_ClipCullDists % 
											rV2_D0_ClipCullDists % 
											rV3_D0_ClipCullDists;

	testfactor::RFactor rPipelineStats;
	const bool bTestPipelineStats = GetFramework()->GetExecutionPriorityLevel() != 1;

	if( bTestPipelineStats )
		rPipelineStats = rPipelineStats_T + rPipelineStats_F;
	else
		rPipelineStats = rPipelineStats_F;

	// Setup factors for other tests
	CGradientValueSet<float> *pDistVals_Lots = GradientValueSet<float>(-4, 4, 9);
	CGradientValueSet<float> *pDistVals_Few = GradientValueSet<float>(-1, 1, 3);

	const float fNaNVal = f_getNaN();
	testfactor::RFactor rV0_D0_Lots = AddParameterValueSet<float>( _T("Vert0_Dist0"), pDistVals_Lots) + AddParameterValue<float>( _T("Vert0_Dist0"), fNaNVal);
	testfactor::RFactor rV0_D0_Few = AddParameterValueSet<float>( _T("Vert0_Dist0"), pDistVals_Few) + AddParameterValue<float>( _T("Vert0_Dist0"), fNaNVal);

	testfactor::RFactor rV1_D0_Lots = AddParameterValueSet<float>( _T("Vert1_Dist0"), pDistVals_Lots) + AddParameterValue<float>( _T("Vert1_Dist0"), fNaNVal);
	testfactor::RFactor rV1_D0_Few = AddParameterValueSet<float>( _T("Vert1_Dist0"), pDistVals_Few) + AddParameterValue<float>( _T("Vert1_Dist0"), fNaNVal);

	testfactor::RFactor rV2_D0_Lots = AddParameterValueSet<float>( _T("Vert2_Dist0"), pDistVals_Lots) + AddParameterValue<float>( _T("Vert2_Dist0"), fNaNVal);
	testfactor::RFactor rV2_D0_Few = AddParameterValueSet<float>( _T("Vert2_Dist0"), pDistVals_Few) + AddParameterValue<float>( _T("Vert2_Dist0"), fNaNVal);

	testfactor::RFactor rV3_D0_Lots = AddParameterValueSet<float>( _T("Vert3_Dist0"), pDistVals_Lots) + AddParameterValue<float>( _T("Vert3_Dist0"), fNaNVal);
	testfactor::RFactor rV3_D0_Few = AddParameterValueSet<float>( _T("Vert3_Dist0"), pDistVals_Few) + AddParameterValue<float>( _T("Vert3_Dist0"), fNaNVal);

	testfactor::RFactor rDist0_Lots 
		= rV0_D0_Lots * rV1_D0_Lots * rV2_D0_Lots * rV3_D0_Few // cycle V3 (bottom left) quickly
		+ rV0_D0_Lots * rV1_D0_Few * rV2_D0_Lots * rV3_D0_Lots; // cycle V1 (top right) quickly
	testfactor::RFactor rDist0_Few = rV0_D0_Few * rV1_D0_Few * rV2_D0_Few * rV3_D0_Few;

	testfactor::RFactor rootFactor;
	tstring sPath = GetPath().ToString( GetPath().Depth() - 1 );
	if (sPath == _T("ZClip"))
	{
		rootFactor = (rSamples_All * rDist0_Lots * rPipelineStats_F + rSamples_1 * rDist0_Few * rPipelineStats_T);
		m_bEnableZClip = true;
		needPriorityFilter = true;
	}
	else if (sPath == _T("ClipDist"))
	{
		rootFactor = (rSamples_All * rClipCullDists) % rPipelineStats;
		m_bEnableClipDist = true;
		needPriorityFilter = false;
	}
	else if (sPath == _T("CullDist"))
	{
		rootFactor = (rSamples_1 * rClipCullDists) % rPipelineStats; // no MSAA needed on cull test
		m_bEnableCullDist = true;
		needPriorityFilter = false;
	}
	else if (sPath == _T("XYClip"))
	{
		rootFactor = (rSamples_All * rDist0_Lots * rPipelineStats_T);
		m_bEnableXYClip = true;
		needPriorityFilter = true;
	}
	else
	{
		assert (!"Unexpected path value");
	}

	SetRootTestFactor( rootFactor );

	if( needPriorityFilter )
	{
		if (g_App.GetTestMode() == WGFTEST_MODE_BVT)
		{
			SkipMatching(GetSkipMatchingFilter() || (
				   FilterLess<float>(_T("Vert0_Dist0"), -1.0) 
				|| FilterGreater<float>(_T("Vert0_Dist0"), 1.0) 
				|| FilterLess<float>(_T("Vert1_Dist0"), -1.0) 
				|| FilterGreater<float>(_T("Vert1_Dist0"), 1.0) 
				|| FilterLess<float>(_T("Vert2_Dist0"), -1.0) 
				|| FilterGreater<float>(_T("Vert2_Dist0"), 1.0) 
				|| FilterLess<float>(_T("Vert3_Dist0"), -1.0) 
				|| FilterGreater<float>(_T("Vert3_Dist0"), 1.0) 
				|| FilterNotEqual<bool>(_T("Pipeline_Stats"), false)
				|| FilterNotEqual<int>(_T("MultiSamples"), 2)
				));
		}

		AddPriorityPatternFilter(FilterLess<float>(_T("Vert0_Dist0"), -1.0), 0.5f); 
		AddPriorityPatternFilter(FilterGreater<float>(_T("Vert0_Dist0"), 1.0), 0.5f); 
		AddPriorityPatternFilter(FilterLess<float>(_T("Vert1_Dist0"), -1.0), 0.5f); 
		AddPriorityPatternFilter(FilterGreater<float>(_T("Vert1_Dist0"), 1.0), 0.5f); 
		AddPriorityPatternFilter(FilterLess<float>(_T("Vert2_Dist0"), -1.0), 0.5f); 
		AddPriorityPatternFilter(FilterGreater<float>(_T("Vert2_Dist0"), 1.0), 0.5f); 
		AddPriorityPatternFilter(FilterLess<float>(_T("Vert3_Dist0"), -1.0), 0.5f); 
		AddPriorityPatternFilter(FilterGreater<float>(_T("Vert3_Dist0"), 1.0), 0.5f); 
		AddPriorityPatternFilter(FilterNotEqual<bool>(_T("Pipeline_Stats"), false), 0.5f);
		AddPriorityPatternFilter(FilterNotEqual<int>(_T("MultiSamples"), 2), 0.5f);

		DeclarePriorityLevelByPriorityValue(1, 1.0f);
	}
}

TEST_RESULT
CTriangleClipTest::ExecuteTestCase()
{
	HRESULT hr = S_OK, hr2 = S_OK;
	TEST_RESULT tRes = RESULT_PASS;
	ID3D11RasterizerState* pRastState = NULL;
	ID3D11Query *pPipelineQuery = NULL;

	//Handle 8-bit precision
	const float loc_denominator = (float)(1 << 8);
	
	// Find sample locations
	SampleFinder sampleFinder;
	POINT pixpos = {0,0};
	if (RESULT_PASS != ( sampleFinder.Setup( GetFramework(), m_pRTView, m_pTextureRT, pixpos ) ) )
	{
		tRes = RESULT_FAIL;
		goto testDone;
	}
	if (RESULT_PASS != ( sampleFinder.FindSamples() ) )
	{
		tRes = RESULT_FAIL;
		goto testDone;
	}
	if (RESULT_PASS != ( sampleFinder.OrderSamples() ) )
	{
		tRes = RESULT_FAIL;
		goto testDone;
	}
	// cleanup sample finder resources, note: locations and count are still valid
	sampleFinder.Cleanup();
	UINT numSamples = sampleFinder.SampleCount();

	if (numSamples != m_nMultisamples)
	{
		WriteToLog( _T("Number of MSAA samples detected did not match. Expected %d, found %u\n"), m_nMultisamples, numSamples );
		tRes = RESULT_FAIL;
		goto testDone;
	}
	if (numSamples == 1)
	{
		const CD3D11TestFramework* pFW = GetFramework();
		POINT loc = sampleFinder.GetLoc(0);
		const float xLoc = loc.x / loc_denominator;
		const float yLoc = loc.y / loc_denominator;
		if (xLoc != 0.5 || yLoc != 0.5)
		{
			WriteToLog( _T("MSAA location for single-sample did not match expectation. Expected (0.5,0.5), found (%g,%g)\n"), xLoc, yLoc );
			tRes = RESULT_FAIL;
		}
	}

	{
		D3D11_QUERY_DESC soDesc;
		soDesc.MiscFlags = 0;
		soDesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
		hr = GetDevice()->CreateQuery(&soDesc, &pPipelineQuery);
		if (FAILED(hr))
		{
			tRes = RESULT_FAIL; 
			WriteToLog( _T( "CreateQuery(PIPELINE_STATISTICS) failed. hr=%s" ), D3DHResultToString( hr ).c_str() );
			goto testDone;
		}
	}
	tRes = UpdateResources();
	if (tRes != RESULT_PASS)
		return tRes;

	// set up the clip distances

	{
		D3D11_RASTERIZER_DESC rastDesc;
		// defaults
		rastDesc.AntialiasedLineEnable = FALSE;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.FrontCounterClockwise = FALSE;
		rastDesc.DepthBias = 0; //D3D10_DEFAULT_DEPTH_BIAS;
		rastDesc.DepthBiasClamp = 0.0f; //D3D10_DEFAULT_DEPTH_BIAS_CLAMP;
		rastDesc.SlopeScaledDepthBias = 0.0f; //D3D10_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rastDesc.ScissorEnable = FALSE;

		// our params
		rastDesc.CullMode = D3D11_CULL_NONE;
		rastDesc.MultisampleEnable = (m_nMultisamples>1)?TRUE:FALSE;
		rastDesc.DepthClipEnable = m_bEnableZClip?TRUE:FALSE;

		if (   FAILED(GetDevice()->CreateRasterizerState(&rastDesc, &pRastState))
			|| pRastState == NULL)
		{
			WriteToLog( _T("Failed with call to CreateRasterizerState. hr=S_FALSE\n") );
			tRes = RESULT_FAIL;
			goto testDone;
		}

		GetEffectiveContext()->RSSetState(pRastState);
	}

	for (int n = 0; n < (int)numSamples; ++n)
	{
		// This begin/end pair must be on the immediate context since
		// the DrawAndCheckTris function will be executing the effective context.
		GetImmediateContext()->Begin(pPipelineQuery);
		TEST_RESULT tResTemp = DrawAndCheckTris(m_ClipDists, n, sampleFinder.GetLoc(n));
		GetImmediateContext()->End(pPipelineQuery);
		if (tRes != RESULT_FAIL)
			tRes = tResTemp;

		D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineStats;
		hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), pPipelineQuery, &pipelineStats, sizeof(pipelineStats), 0 );
		if (hr != S_OK)
		{
			tRes = RESULT_FAIL; 
			WriteToLog( _T( "pPipelineQuery->GetData() failed or timed out. hr=%s" ), D3DHResultToString( hr ).c_str() );
		}
		else if (m_bEnablePipelineStats)
		{
			if (pipelineStats.IAPrimitives != 2)
			{
				tRes = RESULT_FAIL; 
				WriteToLog( _T( "Wrong number of IA Primitives. Expected %d, Found %I64d." ), 2, pipelineStats.IAPrimitives );
			}
			if (pipelineStats.IAVertices < 4 || pipelineStats.IAVertices > 6)
			{
				tRes = RESULT_FAIL; 
				WriteToLog( _T( "Wrong number of IA Vertices. Expected %d-%d, Found %I64d." ), 4, 6, pipelineStats.IAVertices );
			}
			if (pipelineStats.CInvocations != 2)
			{
				tRes = RESULT_FAIL; 
				WriteToLog( _T( "Wrong number of Clipper Invocations. Expected %d, Found %I64d." ), 2, pipelineStats.CInvocations );
			}
			int numPointsClipped[2] = {0,0};
			int numPointsZero[2] = {0,0};
			float clipVals[4];
			for (int n = 0; n < 4; ++n)
				clipVals[n] = m_ClipDists[n][0].f[0];
			// check points < 0
			if (clipVals[0] < 0)
			{
				numPointsClipped[0] += 1;
				numPointsClipped[1] += 1;
			}
			if (clipVals[1] < 0)
				numPointsClipped[0] += 1;
			if (clipVals[2] < 0)
			{
				numPointsClipped[0] += 1;
				numPointsClipped[1] += 1;
			}
			if (clipVals[3] < 0)
				numPointsClipped[1] += 1;

			// check points for NaN
			if ((m_bEnableZClip || m_bEnableClipDist))
			{
				if (isNaN(clipVals[0]) || isNaN(clipVals[2]))
				{
					numPointsClipped[0] = 3;
					numPointsClipped[1] = 3;
				}
			}
			else
			{
				if (isNaN(clipVals[0]))
				{
					numPointsClipped[0] += 1;
					numPointsClipped[1] += 1;
				}
				if (isNaN(clipVals[2]))
				{
					numPointsClipped[0] += 1;
					numPointsClipped[1] += 1;
				}
			}
			if ((m_bEnableZClip || m_bEnableClipDist))
			{
				if (isNaN(clipVals[1]))
					numPointsClipped[0] = 3;
				if (isNaN(clipVals[3]))
					numPointsClipped[1] = 3;
			}
			else
			{
				if (isNaN(clipVals[1]))
					numPointsClipped[0] += 1;
				if (isNaN(clipVals[3]))
					numPointsClipped[1] += 1;
			}

			int minPrimitives = 0;
			if (numPointsClipped[0] < 3)
				minPrimitives += 1;
			if (numPointsClipped[1] < 3)
				minPrimitives += 1;

			int maxPrimitives = minPrimitives;
			if (numPointsClipped[0] > 0 && numPointsClipped[0] < 3)
				maxPrimitives += 1;
			if (numPointsClipped[1] > 0 && numPointsClipped[1] < 3)
				maxPrimitives += 1;
			if (pipelineStats.CPrimitives < minPrimitives || pipelineStats.CPrimitives > maxPrimitives)
			{
				tRes = RESULT_FAIL; 
				WriteToLog( _T( "Wrong number of Clipper Primitives. Expected %d-%d, Found %I64d." ), minPrimitives, maxPrimitives, pipelineStats.CPrimitives );
			}
		}
	}

testDone:
	// cleanup
	GetEffectiveContext()->RSSetState(NULL);
	
	if( FAILED(hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context: %s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
	}

	SAFE_RELEASE(pRastState);
	SAFE_RELEASE(pPipelineQuery);

	return tRes;
}


TEST_RESULT
CTriangleClipTest::DrawAndCheckTris(const clipvec4 clipdists[4][2], int sampleNum, POINT sampleLoc)
{
	TEST_RESULT tRes = RESULT_PASS;

	//Handle 8-bit precision
	const float loc_denominator = (float)(1 << 8);
	assert(sampleNum >= 0);
	WriteToLog( _T("sampleNum=%d. samplePos=(%g,%g)\n"), sampleNum, sampleLoc.x/loc_denominator, sampleLoc.y/loc_denominator );

	const CD3D11TestFramework* pFW = GetFramework();
	const FLOAT clearColor[4] = {0,0,0,0};
	UINT sampleMask = 1<<sampleNum;
	GetEffectiveContext()->OMSetBlendState(NULL, clearColor, sampleMask);

	GetEffectiveContext()->ClearRenderTargetView(m_pRTView, clearColor);
	if (FAILED(DrawQuad(clipdists))) // modifies the context
		return RESULT_FAIL;
	pixelLit_t* lit = new pixelLit_t[m_nPrimitiveW*m_nPrimitiveH];
	float* fArray = (float*)(clipdists);
	int pix_num, clip_num;
	for (pix_num = 0; pix_num < m_nPrimitiveW*m_nPrimitiveH; ++pix_num)
		lit[pix_num] = pixel_lit;
	int num_clip_vals = (m_bEnableZClip?1:2);
	num_clip_vals = 1; // we never cycle over more than 1 clip value, others are set to 0
	for (clip_num = 0; clip_num < num_clip_vals; ++clip_num)
	{
		if (m_bEnableClipDist || m_bEnableZClip)
		{
			float vpos[4][3] = {
				{0,						0,						fArray[ 0+clip_num]},
				{(float)m_nPrimitiveW,	0,						fArray[ 8+clip_num]},
				{(float)m_nPrimitiveW,	(float)m_nPrimitiveH,	fArray[16+clip_num]},
				{0,						(float)m_nPrimitiveH,	fArray[24+clip_num]},
			};
			float planes[2][4];
			make_plane(planes[0], vpos[0], vpos[1], vpos[2]);
			make_plane(planes[1], vpos[2], vpos[3], vpos[0]);
			assert(planes[0][2] > 0 || isNaN(planes[0][2]));
			assert(planes[1][2] > 0 || isNaN(planes[1][2]));

			float slope = ((float)m_nPrimitiveW)/m_nPrimitiveH;
			int x,y;
			for (y = 0; y < m_nPrimitiveH; ++y)
			{
				//tRes = RESULT_PASS;
				for (x = 0; x < m_nPrimitiveW; ++x)
				{
					int num_lit = 1;
					int num_uncertain = 0;
					const float fX = x + sampleLoc.x / loc_denominator;
					const float fY = y + sampleLoc.y / loc_denominator;
					int corners[3] = {0,1,2};
					int plane_idx = 0;
					if (fX < (fY)*slope-1.0/1024)
					{
						plane_idx = 1;
						corners[1] = 2;
						corners[2] = 3;
					}
					double pix_val;
					pix_val = test_point(planes[plane_idx], vpos[0], fX, fY);
					if (isNaN(float(pix_val)))
					{
						num_lit -= 1;
						WriteToLog( _T("PixVal @ (%d, %d) = NaN\n"), x, y );
					}
					else
					{
						const double halfSubPixel = 1.0/(2.0 * (double)loc_denominator); // +/- 0.5 sub-pixel snap accuracy

						const double clipEpsilonX = fabs(halfSubPixel * (planes[plane_idx][0] / planes[plane_idx][2]));
						const double clipEpsilonY = fabs(halfSubPixel * (planes[plane_idx][1] / planes[plane_idx][2]));
						const double clipEpsilon = clipEpsilonX + clipEpsilonY;

						if (   (pix_val > -clipEpsilon && pix_val < clipEpsilon)) // imprecision could snap to or away from pixel center
						{
							num_uncertain += 1;
							WriteToLog( _T("PixVal @ (%d, %d) = ~0\n"), x, y );
						}
						else
						{
							pix_val = f_quantize(pix_val, 8);
							if (pix_val < 0)
							{
								num_lit -= 1;
								WriteToLog( _T("PixVal @ (%d, %d) < 0\n"), x, y );
							}
							else
							{
								WriteToLog( _T("PixVal @ (%d, %d) > 0\n"), x, y );
							}
						}
					}
					assert(num_lit >= 0 && num_lit <= 1);
					size_t index = y*m_nPrimitiveW+x;
					if (lit[index] != pixel_unlit)
					{
						if (num_uncertain > 0)
						{
							lit[index] = pixel_edgecase;
						}
						else if (lit[index] != pixel_edgecase)
						{
							if (num_lit >= 1)
							{
								if (1 < m_nMultisamples)
								{
									lit[index] = min(lit[index],pixel_partial_low);
								}
								else 
								{
									lit[index] = min(lit[index],pixel_lit);
								}
							}
							else
							{
								lit[index] = pixel_unlit;
							}
						}
						else if (num_lit == 0)
						{
							lit[index] = pixel_unlit;
						}

					}
				}
			}
		}
		else if (m_bEnableXYClip)
		{
			// TODO: implement XY clip testing?
		}
		else if (m_bEnableCullDist)
		{
			float maxvals[2] = {
				maxNoNaN(fArray[0+clip_num],maxNoNaN(fArray[ 8+clip_num],fArray[16+clip_num])),
				maxNoNaN(fArray[0+clip_num],maxNoNaN(fArray[24+clip_num],fArray[16+clip_num]))
			};
			float slope = ((float)m_nPrimitiveW)/m_nPrimitiveH;
			int x,y;
			for (y = 0; y < m_nPrimitiveH; ++y)
			{
				for (x = 0; x < (int)((y+0.5)*slope+0.5-1.0/1024); ++x)
				{
					size_t index = y*m_nPrimitiveW+x;
					if (isNaN(maxvals[1]) || maxvals[1] < 0)
						lit[index] = pixel_unlit;
				}
				for (x = (int)((y+0.5)*slope+0.5-1.0/1024); x < m_nPrimitiveW; ++x)
				{
					size_t index = y*m_nPrimitiveW+x;
					if (isNaN(maxvals[0]) || maxvals[0] < 0)
						lit[index] = pixel_unlit;
				}
			}
		}
	}
	tRes = CheckQuadPattern(lit); // Executes the effective context

	delete[] lit;

	return tRes;
}

