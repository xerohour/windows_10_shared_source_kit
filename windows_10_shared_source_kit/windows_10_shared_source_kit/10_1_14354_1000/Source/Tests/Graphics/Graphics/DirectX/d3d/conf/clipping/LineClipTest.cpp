#include "Clipping.h"

float g_LineDists[][4] =
{
	// Here is the 2x2 render target - (0,0) is top left - with the pixels numbered and vertex positions marked as X's:
	// X---|---X
	// | 0 | 1 |
	// |---|---|
	// | 2 | 3 |
	// X---|---X

	// Vertex ID's (see CClippingConfTest::SetupGeometry()) for index buffer
	// Vertex 0 is the top left, vertex 3 is the top right
	// Vertex 1 is the bottom right, Vertex 2 is the bottom left

	// Vertex dists are below:
	// { Top Left, Top Right, Bottom Right, Bottom Left }

	//
	// IMPORTANT: The 2 lines (Line 0 and 1) are Vertices {0, 1} and Vertices {2, 3} respectively to form an X shape
	// Split into eqivalence classes
	//
	
	// No slope (dists the same)		// Clipping														// Culling
	{ -4.0f, 4.0f, -4.0f, 4.0f },		// Line 0 is not lit, Line 1 is lit								// Line 0 is culled, Line 1 is lit
	{ 0.0f, f_getNaN(), 0.0f, 0.0f },	// Line 0 is lit, Line 1 is not lit (Line 1 has NaN)			// Line 0 is lit, Line 1 is lit (Line 1 has NaN -> as if < 0)
	{ 0.0f, f_getNaN(), 0.0f, -0.5f },	// Line 0 is lit, Line 1 is not lit (Line 1 has NaN)			// Line 0 is lit, Line 1 is culled (Line 1 has NaN -> as if < 0)
	{ 0.1f, 0.2f, 0.1f, 0.2f },			// Line 0 is lit, Line 1 is lit									// Line 0 is lit, Line 1 is lit

	// Positive slope (second vertex's dist > first)
	{ -4.0f, -8.0f, 3.0f, 6.0f },		// Pixels lit: 2, 3; Pixels not lit: 0, 1						// Pixels lit: 0, 1, 2, 3
	{ -2.0f, -0.1f, 25.0f, 1.0f },		// Pixels lit: 0, 1, 2, 3										// Pixels lit: 0, 1, 2, 3
	{ 4.0f, -0.1f, f_getNaN(), 0.1f },	// Pixels lit: 2; Pixels not lit: 0, 1, 3 (Line 0 has NaN)		// Pixels lit: 0, 1, 2, 3
	{ -4.0f, -0.1f, f_getNaN(), 0.1f },	// Pixels lit: 2; Pixels not lit: 0, 1, 3 (Line 0 has NaN)		// Pixels lit: 1, 2; Pixels not lit: 0, 3 (Line 0 is culled)
	{ -4.0f, -0.2f, 0.0f, -0.1f },		// Pixels not lit: 0, 1, 2, 3 (No pixels go above 0)			// Pixels lit: 0, 3; Pixels not lit: 1, 2 (Line 1 is culled)

	// Negative slope (second vertex's dist < first)
	{ 0.0f, -0.1f, -2.0f, -1000.1f },	// Pixels not lit: 0, 1, 2, 3 (No pixels go above 0)			// Pixels lit: 0, 3; Pixels not lit: 1, 2 (Line 1 is culled)
	{ 4.0f, 2.0f, -40.0f, -20.0f },		// Pixels not lit: 0, 1, 2, 3									// Pixels lit: 0, 1, 2, 3
	{ 4.0f, 8.0, -4.5f, -9.0f },		// Pixels lit: 0, 1; Pixels not lit: 2, 3						// Pixels lit: 0, 1, 2, 3
	{ 4.0f, 1.0f, -3.5f, f_getNaN() },	// Pixels lit: 0; Pixels not lit: 0, 2, 3 (Line 1 has NaN)		// Pixels lit: 0, 1, 2, 3
	{ 4.0f, -1.0f, -3.5f, f_getNaN() },	// Pixels lit: 0; Pixels not lit: 0, 2, 3 (Line 1 has NaN)		// Pixels lit: 0, 3; Pixels not lit: 1, 2 (Line 1 is culled)
	{ 5.0f, 0.2f, -0.7f, -0.2f},		// Pixels lit: 0, 1, 3; Pixels not lit: 2						// Pixels lit: 0, 1, 2, 3
	{ 25.0f, 1.0f, -2.0f, -0.1f },		// Pixels lit: 0, 1, 2, 3										// Pixels lit: 0, 1, 2, 3
};

void
CLineClipTest::InitTestParameters()
{
	bool needPriorityFilter = false;
	m_bEnableXYClip = false;
	m_bEnableZClip = false;
	m_bEnableClipDist = false;
	m_bEnableCullDist = false;

	AddParameter<int>( _T("MultiSamples"), &m_nMultisamples );
	testfactor::RFactor rSamples_1 = AddParameterValue<int>( _T("MultiSamples"), 1);
	testfactor::RFactor rSamples_2 = AddParameterValue<int>( _T("MultiSamples"), 2);
	testfactor::RFactor rSamples_4 = AddParameterValue<int>( _T("MultiSamples"), 4);
	// TODO: enable MSAA
	//testfactor::RFactor rSamples_All = rSamples_1 + rSamples_2 + rSamples_4;
	testfactor::RFactor rSamples_All = rSamples_1;

	m_bAntiAliasLine = false;
	AddParameter<bool>( _T("Anti_Alias_Line"), &m_bAntiAliasLine );
	testfactor::RFactor rAALine_T = AddParameterValue<bool>( _T("Anti_Alias_Line"), true);
	testfactor::RFactor rAALine_F = AddParameterValue<bool>( _T("Anti_Alias_Line"), false);

	AddParameter<bool>( _T("Pipeline_Stats"), &m_bEnablePipelineStats );
	SetParameterDefaultValue( _T("Pipeline_Stats"), false );
	testfactor::RFactor rPipelineStats_T = AddParameterValue<bool>( _T("Pipeline_Stats"), true);
	testfactor::RFactor rPipelineStats_F = AddParameterValue<bool>( _T("Pipeline_Stats"), false);

	AddParameter<float>( _T("Vert0_Dist0"), &m_fClipDists[0]);
	AddParameter<float>( _T("Vert1_Dist0"), &m_fClipDists[1]);
	AddParameter<float>( _T("Vert2_Dist0"), &m_fClipDists[2]);
	AddParameter<float>( _T("Vert3_Dist0"), &m_fClipDists[3]);

	// Setup factors for clip and cull tests
	const size_t numLineDists = ARRAY_SIZE(g_LineDists);
	const size_t numComponentsPerElementBytes = sizeof(g_LineDists[0]);
	assert( numLineDists >= 1 );
	assert( 4 * sizeof( float ) == numComponentsPerElementBytes  );	// Should only have 4 floats per entry (one float for each vertex)
	testfactor::RFactor rV0_D0_ClipCullDists = AddParameterValueSet( _T("Vert0_Dist0"), TableValueSet<float>( &(g_LineDists[0][0]), numComponentsPerElementBytes, numLineDists ) );
	testfactor::RFactor rV1_D0_ClipCullDists = AddParameterValueSet( _T("Vert1_Dist0"), TableValueSet<float>( &(g_LineDists[0][1]), numComponentsPerElementBytes, numLineDists ) );
	testfactor::RFactor rV2_D0_ClipCullDists = AddParameterValueSet( _T("Vert2_Dist0"), TableValueSet<float>( &(g_LineDists[0][2]), numComponentsPerElementBytes, numLineDists ) );
	testfactor::RFactor rV3_D0_ClipCullDists = AddParameterValueSet( _T("Vert3_Dist0"), TableValueSet<float>( &(g_LineDists[0][3]), numComponentsPerElementBytes, numLineDists ) );

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

	testfactor::RFactor rDist0_Lots = rV0_D0_Lots * rV1_D0_Lots * rV2_D0_Lots * rV3_D0_Lots;
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

	SetRootTestFactor( rAALine_F * rootFactor );

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
				|| FilterNotEqual<int>(_T("MultiSamples"), 1)
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
CLineClipTest::ExecuteTestCase()
{
	HRESULT hr = S_OK, hr2 = S_OK;
	TEST_RESULT tRes = RESULT_PASS;
	ID3D11RasterizerState* pRastState = NULL;

	tRes = UpdateResources();
	if (tRes != RESULT_PASS)
		return tRes;

	// set up the clip distances

	D3D11_RASTERIZER_DESC rastDesc;
	// defaults
	rastDesc.AntialiasedLineEnable = m_bAntiAliasLine?TRUE:FALSE;
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

	FLOAT clearColor[4] = {0,0,0,0};
	GetEffectiveContext()->ClearRenderTargetView(m_pRTView, clearColor);

	tRes = DrawAndCheckLine(m_fClipDists);

testDone:
	// cleanup
	GetEffectiveContext()->RSSetState(NULL);

	if( FAILED(hr = ExecuteEffectiveContext()) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		tRes = RESULT_FAIL;
	}

	SAFE_RELEASE(pRastState);

	return tRes;
}

static bool testPixelCovered(float zeroPoint, float comparison, float gradient, float startVal, bool allowEqual)
{
	if (gradient < 0)
	{
		if (zeroPoint > comparison || (allowEqual && zeroPoint == comparison))
			return true;
	}
	else if (gradient > 0)
	{
		if (zeroPoint < comparison || (allowEqual && zeroPoint == comparison))
			return true;
	}
	else if (gradient == 0)
	{
		if (startVal >= 0)
			return true;
	}
	return false;
}

TEST_RESULT
CLineClipTest::DrawAndCheckLine(const float clipdists[4])
{
	TEST_RESULT tRes = RESULT_PASS;

	// need more sophisticated test for non-square targets
	assert(m_nPrimitiveW == m_nPrimitiveH);

	if (FAILED(DrawLine(clipdists))) // Modifies effective context
		return RESULT_FAIL;
	pixelLit_t* lit = new pixelLit_t[m_nPrimitiveW*m_nPrimitiveH];
	int pix_num, clip_num;
	for (pix_num = 0; pix_num < m_nPrimitiveW*m_nPrimitiveH; ++pix_num)
		lit[pix_num] = pixel_unlit;
	int num_clip_vals = (m_bEnableZClip?1:8);
	num_clip_vals = 1;
	for (clip_num = 0; clip_num < num_clip_vals; ++clip_num)
	{
		if (m_bEnableClipDist || m_bEnableZClip) // TODO: add DepthClip capability here
		{
			const float lineLength = (m_bEnableCullDist | m_bEnableClipDist) ? sqrt((float)(m_nPrimitiveW*m_nPrimitiveW + m_nPrimitiveH*m_nPrimitiveH)) : m_nPrimitiveW;
			// negative gradient means line connects left edge to zero-point (clipdist is greatest at left edge)
			float clipGrad[2];
			clipGrad[0] = (clipdists[2] - clipdists[0]) / lineLength;
			clipGrad[1] = (clipdists[1] - clipdists[3]) / lineLength;

			float zeroPoint[2];
			zeroPoint[0] = -clipdists[0]/clipGrad[0];
			zeroPoint[1] = -clipdists[3]/clipGrad[1];
			for (int n = 0 ; n < 2; ++n)
			{
				WriteToLog(_T("zeroPoint[%d] = %g (Normalized = %g, Line length = %g)"), n, zeroPoint[n], zeroPoint[n]/lineLength, lineLength);
			}
			// top-left to bottom-right
			if (testPixelCovered(zeroPoint[0], 0.75, clipGrad[0], clipdists[0], (clipGrad[0] > 0)))
				lit[0] = pixel_lit;
			if (testPixelCovered(zeroPoint[0], 1.75, clipGrad[0], clipdists[0], (clipGrad[0] > 0)))
				lit[3] = pixel_lit;
			// bottom-left to top-right
			if (testPixelCovered(zeroPoint[1], 0.75, clipGrad[1], clipdists[1], (clipGrad[0] < 0)))
				lit[2] = pixel_lit;
			if (testPixelCovered(zeroPoint[1], 1.75, clipGrad[1], clipdists[1], (clipGrad[0] < 0)))
				lit[1] = pixel_lit;
		}
		else if (m_bEnableCullDist)
		{
			float maxvals[2] = {
				maxNoNaN(clipdists[0],clipdists[2]),
				maxNoNaN(clipdists[1],clipdists[3])
			};
			if (!isNaN(maxvals[0]) && f_quantize(maxvals[0], 16) >= 0)
			{
				int x,y;
				for (x=0, y=0; x < m_nPrimitiveW; ++x,++y)
				{
					size_t index = y*m_nPrimitiveW+x;
					lit[index] = pixel_lit;
				}
			}
			if (!isNaN(maxvals[1]) && f_quantize(maxvals[1], 16) >= 0)
			{
				int x,y;
				for (x=m_nPrimitiveW-1, y=0; x >= 0; --x,++y)
				{
					size_t index = y*m_nPrimitiveW+x;
					lit[index] = pixel_lit;
				}
			}
		}
	}
	tRes = CheckQuadPattern(lit); // Executes the context

	delete[] lit;

	// TODO: enable this test more fully.
	// just running with skip for coverage currently
	if (m_nMultisamples > 1)
	{
		tRes = RESULT_SKIP;
		WriteToLog("%s Line Clipping test only run for coverage data currently", LOG_HEADER);
	}

	return tRes;
}
