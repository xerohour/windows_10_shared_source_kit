#include "Tessellation.h"


// hull shader stuff related to tessellation testing

static const char*  g_hsQuadEdgeTessFunc =
	"\n""HS_CONSTANT_DATA_OUTPUT QuadEdgeTessFunc( "
	"\n""	InputPatch<VS_OUT, 4> inPatch,"
	"\n""	uint PatchID : SV_PrimitiveID )"
	"\n""{	"
	"\n""	HS_CONSTANT_DATA_OUTPUT Output;"
	"\n""	Output.PatchID = PatchID;"
	"\n""	Output.Edges[0] = (tessVals[0] + tessVals[1]) * 0.5; // U0"
	"\n""	Output.Edges[1] = (tessVals[0] + tessVals[2]) * 0.5; // V0"
	"\n""	Output.Edges[2] = (tessVals[2] + tessVals[3]) * 0.5; // U1"
	"\n""	Output.Edges[3] = (tessVals[1] + tessVals[3]) * 0.5; // V1"
	"\n""	Output.Inside[0] = (Output.Edges[0] + Output.Edges[2]) * 0.5; // U"
	"\n""	Output.Inside[1] = (Output.Edges[1] + Output.Edges[3]) * 0.5; // V"
	"\n""	return Output;"
	"\n""}"
	"\n";

static const char*  g_hsTriEdgeTessFunc =
	"\n""HS_CONSTANT_DATA_OUTPUT TriEdgeTessFunc( "
	"\n""	InputPatch<VS_OUT, 3> inPatch,"
	"\n""	uint PatchID : SV_PrimitiveID )"
	"\n""{	"
	"\n""	HS_CONSTANT_DATA_OUTPUT Output;"
	"\n""	Output.PatchID = PatchID;"
	"\n""	Output.Edges[0] = tessFactors[0];"
	"\n""	Output.Edges[1] = tessFactors[1];"
	"\n""	Output.Edges[2] = tessFactors[2];"
	"\n""	Output.Inside[0] = tessFactors[3];"
	"\n""	return Output;"
	"\n""}"
	"\n";

static const char*  g_hsLineEdgeTessFunc =
	"\n""HS_CONSTANT_DATA_OUTPUT LineEdgeTessFunc( "
	"\n""	InputPatch<VS_OUT, 4> inPatch,"
	"\n""	uint PatchID : SV_PrimitiveID )"
	"\n""{	"
	"\n""	HS_CONSTANT_DATA_OUTPUT Output;"
	"\n""	Output.PatchID = PatchID;"
	"\n""	Output.DensityDetail[0] = (tessVals[0] + tessVals[1]) * 0.5;"
	"\n""	Output.DensityDetail[1] = (tessVals[0] + tessVals[2]) * 0.5;"
	"\n""	return Output;"
	"\n""}"
	"\n";

// domain shader stuff

static const char*  g_dsOutDecl_domainPos_patchID =
	"struct DS_DOMAIN_OUTPUT"
	"{"
	"	float4 domainPos : SV_Position;"
	"	uint  HSPatchID : HSPatchID;"
	"	uint  DSPatchID : DSPatchID;"
	"};"
	"\n";

static const char*  g_dsQuadDomainPosFunc =
	"\n""DS_DOMAIN_OUTPUT ds_main( "
	"\n""	HS_CONSTANT_DATA_OUTPUT input, "
	"\n""	float2 UV : SV_DomainLocation,"
	"\n""	const OutputPatch<HS_CONTROL_POINT_OUT, 4> outCPs,"
	"\n""	uint patchID : SV_PrimitiveID  )"
	"\n""{"
	"\n""	DS_DOMAIN_OUTPUT Output;"
	"\n""	Output.domainPos = float4(0,0,0,1);"
	"\n""	Output.domainPos.xy = UV;"
	"\n""	Output.HSPatchID = input.PatchID;"
	"\n""	Output.DSPatchID = patchID;"
	"\n""	return Output;"
	"\n""}"
	"\n";

static const char*  g_dsTriDomainPosFunc =
	"\n""DS_DOMAIN_OUTPUT ds_main( "
	"\n""	HS_CONSTANT_DATA_OUTPUT input, "
	"\n""	float3 UVW : SV_DomainLocation,"
	"\n""	const OutputPatch<HS_CONTROL_POINT_OUT, 3> outCPs,"
	"\n""	uint patchID : SV_PrimitiveID  )"
	"\n""{"
	"\n""	DS_DOMAIN_OUTPUT Output;"
	"\n""	Output.domainPos = float4(0,0,0,1);"
	"\n""	Output.domainPos.xyz = UVW;"
	"\n""	Output.HSPatchID = input.PatchID;"
	"\n""	Output.DSPatchID = patchID;"
	"\n""	return Output;"
	"\n""}"
	"\n";

extern const UINT  g_numTriangles;

//
void  CTessellatorTest::InitTestParameters()
{
	CTestCaseParameter<tstring>*  pVersusParam = AddParameter<tstring>( _T("Versus"), &m_Versus );
	testfactor::RFactor  rVersus_Ref = AddParameterValue<tstring>( pVersusParam, _T("ref"));
	testfactor::RFactor  rVersus_Self = AddParameterValue<tstring>( pVersusParam, _T("self"));

	CTestCaseParameter<tstring>*  pDomainParam = AddParameter<tstring>( _T("Domain"), &m_DomainName );
	testfactor::RFactor  rDomain_Quad = AddParameterValue<tstring>( pDomainParam, _T("quad"));
	testfactor::RFactor  rDomain_Tri = AddParameterValue<tstring>( pDomainParam, _T("tri"));
	testfactor::RFactor  rDomain_Isoline = AddParameterValue<tstring>( pDomainParam, _T("isoline"));

	CTestCaseParameter<tstring>*  pPartitionParam = AddParameter<tstring>( _T("Partition"), &m_PartitionName );
	testfactor::RFactor  rPartition_Int = AddParameterValue<tstring>( pPartitionParam, _T("integer"));
	testfactor::RFactor  rPartition_Pow2 = AddParameterValue<tstring>( pPartitionParam, _T("pow2"));
	testfactor::RFactor  rPartition_FracOdd = AddParameterValue<tstring>( pPartitionParam, _T("fractional_odd"));
	testfactor::RFactor  rPartition_FracEven = AddParameterValue<tstring>( pPartitionParam, _T("fractional_even"));
	testfactor::RFactor  rPartition_All = rPartition_Int + rPartition_Pow2 + rPartition_FracOdd + rPartition_FracEven;

	CTestCaseParameter<tstring>*  pTopologyParam = AddParameter<tstring>( _T("Topology"), &m_TopologyName );
	testfactor::RFactor  rTopology_Point = AddParameterValue<tstring>( pTopologyParam, _T("point"));
	testfactor::RFactor  rTopology_Line = AddParameterValue<tstring>( pTopologyParam, _T("line"));
	testfactor::RFactor  rTopology_Triangle_CW = AddParameterValue<tstring>( pTopologyParam, _T("triangle_cw"));
	testfactor::RFactor  rTopology_Triangle_CCW = AddParameterValue<tstring>( pTopologyParam, _T("triangle_ccw"));

	CTestCaseParameter<float>*  pTessValParam0 = AddParameter<float>( _T("TessVal0"), &m_TessVals[0] );
	CTestCaseParameter<float>*  pTessValParam1 = AddParameter<float>( _T("TessVal1"), &m_TessVals[1] );
	CTestCaseParameter<float>*  pTessValParam2 = AddParameter<float>( _T("TessVal2"), &m_TessVals[2] );
	CTestCaseParameter<float>*  pTessValParam3 = AddParameter<float>( _T("TessVal3"), &m_TessVals[3] );

	testfactor::RFactor  rTessVal0_1 = AddParameterValue<float>( pTessValParam0, 1 );
	testfactor::RFactor  rTessVal1_1 = AddParameterValue<float>( pTessValParam1, 1 );
	testfactor::RFactor  rTessVal2_1 = AddParameterValue<float>( pTessValParam2, 1 );
	testfactor::RFactor  rTessVal3_1 = AddParameterValue<float>( pTessValParam3, 1 );
	testfactor::RFactor  rTessVal0_64 = AddParameterValue<float>( pTessValParam0, 64 );
	testfactor::RFactor  rTessVal1_64 = AddParameterValue<float>( pTessValParam1, 64 );
	testfactor::RFactor  rTessVal2_64 = AddParameterValue<float>( pTessValParam2, 64 );
	testfactor::RFactor  rTessVal3_64 = AddParameterValue<float>( pTessValParam3, 64 );
  
	testfactor::RFactor  rTessVal_All_1_Or_64 = (rTessVal0_1 + rTessVal0_64) 
											  * (rTessVal1_1 + rTessVal1_64)
											  * (rTessVal2_1 + rTessVal2_64)
											  * (rTessVal3_1 + rTessVal3_64);
  
	CTestCaseParameter<UINT>* pEdgesTessFactorParam[3] = {0};
	tstring  parmName = "";
	testfactor::RFactor rTessFactor1[3]; 
	testfactor::RFactor rTessFactor2[3]; 
	testfactor::RFactor rTessFactor3[3]; 
	testfactor::RFactor rTessFactor32[3];
	testfactor::RFactor rTessFactor62[3];
	testfactor::RFactor rTessFactor63[3];
	testfactor::RFactor rTessFactor64[3];

	for ( int i = 0; i < 3; i++ )
	{
		parmName = _T("EdgesTessFactor") + ToString( i ); 
		pEdgesTessFactorParam[i] = AddParameter<UINT>( parmName, &m_uintTessFactors[i] );

		rTessFactor1[i] = AddParameterValue<UINT>( pEdgesTessFactorParam[i], 1 );
		rTessFactor2[i] = AddParameterValue<UINT>( pEdgesTessFactorParam[i], 2 );
		rTessFactor3[i] = AddParameterValue<UINT>( pEdgesTessFactorParam[i], 3 );
		rTessFactor32[i] = AddParameterValue<UINT>( pEdgesTessFactorParam[i], 32 );
		rTessFactor62[i] = AddParameterValue<UINT>( pEdgesTessFactorParam[i], 62 );
		rTessFactor63[i] = AddParameterValue<UINT>( pEdgesTessFactorParam[i], 63 );
		rTessFactor64[i] = AddParameterValue<UINT>( pEdgesTessFactorParam[i], 64 );
	}

	testfactor::RFactor rfEdgeTessFactors[3];
	rfEdgeTessFactors[0] = rTessFactor1[0] + rTessFactor2[0] + rTessFactor3[0] 
		+ rTessFactor32[0] + rTessFactor62[0] + rTessFactor63[0] + rTessFactor64[0];
	rfEdgeTessFactors[1] = rTessFactor1[1] + rTessFactor2[1] + rTessFactor3[1] 
		+ rTessFactor32[1] + rTessFactor62[1] + rTessFactor63[1] + rTessFactor64[1];
	rfEdgeTessFactors[2] = rTessFactor1[2] + rTessFactor2[2] + rTessFactor3[2] 
		+ rTessFactor32[2] + rTessFactor62[2] + rTessFactor63[2] + rTessFactor64[2];

	testfactor::XFACTOR xfEdgeTessPairwise[] =
	{
		{rfEdgeTessFactors[0], 1, NULL, 0},	
		{rfEdgeTessFactors[1], 1, NULL, 0},	
		{rfEdgeTessFactors[2], 1, NULL, 0}	
	};

	size_t groupOrders [] = { XGROUPORDER(1, 2) };

	testfactor::RFactor rfEdgeTessFactors_Pairwise = 
		NewCrossFactor( 
			xfEdgeTessPairwise,
			_countof(xfEdgeTessPairwise),
			_T("EdgeTessFactors"),
			groupOrders,
			_countof(groupOrders)
		);

	CTestCaseParameter<UINT>* pInsideTessFactorParam = AddParameter<UINT>( _T("InsideTessFactor"), &m_uintTessFactors[3] );

	testfactor::RFactor rTessFactorRange1_10 = AddParameterValueSet<UINT>( pInsideTessFactorParam, new CUserValueSet<UINT>( 1, 5, 10 ));


	CTestCaseParameter<UINT>* pNumTrisInput = AddParameter<UINT>( _T("NumTrisInput"), &m_nNumTrisInput );
	testfactor::RFactor  rNumTrisInput = AddParameterValueSet<UINT>( pNumTrisInput, new CUserValueSet<UINT>( 1, 3 ));

	testfactor::RFactor  rTriFactors0 = rDomain_Tri * (rPartition_Int + rPartition_Pow2)
										* (rTopology_Point + rTopology_Triangle_CW + rTopology_Triangle_CCW)
										* rfEdgeTessFactors_Pairwise
										* rTessFactorRange1_10 * rNumTrisInput;
	testfactor::RFactor  rTriFactors1 = rDomain_Tri * rPartition_FracOdd
										* (rTopology_Point + rTopology_Triangle_CW + rTopology_Triangle_CCW)
										* rfEdgeTessFactors_Pairwise
										* rTessFactorRange1_10 * rNumTrisInput;
	testfactor::RFactor  rTriFactors2 = rDomain_Tri * rPartition_FracEven
										* (rTopology_Point + rTopology_Triangle_CW + rTopology_Triangle_CCW)
										* rfEdgeTessFactors_Pairwise
										* rTessFactorRange1_10 * rNumTrisInput;

	testfactor::RFactor  rQuadFactors = rDomain_Quad * rPartition_All
										* (rTopology_Point + rTopology_Triangle_CW + rTopology_Triangle_CCW)
										* rTessVal_All_1_Or_64;

	testfactor::RFactor  rIsolineFactors = rDomain_Isoline * rPartition_All * (rTopology_Point + rTopology_Line)
										   * rTessVal_All_1_Or_64;
										
	testfactor::RFactor  rVersusFactors = rQuadFactors * ( rVersus_Ref + rVersus_Self );
	
	testfactor::RFactor  rootFactor = rVersusFactors + rTriFactors0 + rTriFactors1 + rTriFactors2 + rIsolineFactors;

	SetRootTestFactor( rootFactor );

	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Versus"), _T("self")), 0.5f);

	AddPriorityPatternFilter(FilterGreaterEqual<UINT>(_T("EdgesTessFactor0"), 3) && FilterLessEqual<UINT>(_T("EdgesTessFactor0"), 62), 0.5f);
	AddPriorityPatternFilter(FilterGreaterEqual<UINT>(_T("EdgesTessFactor1"), 3) && FilterLessEqual<UINT>(_T("EdgesTessFactor1"), 62), 0.5f);
	AddPriorityPatternFilter(FilterGreaterEqual<UINT>(_T("EdgesTessFactor2"), 3) && FilterLessEqual<UINT>(_T("EdgesTessFactor2"), 62), 0.5f);
	AddPriorityPatternFilter(FilterNotEqual<float>(_T("TessVal0"), 1), 0.5f);
	AddPriorityPatternFilter(FilterNotEqual<float>(_T("TessVal1"), 64), 0.5f);
	AddPriorityPatternFilter(FilterNotRegExp<tstring>(_T("Topology"), _T("triangle")), 0.5f);
	AddPriorityPatternFilter(FilterNotEqual<tstring>(_T("Domain"), _T("isoline")), 0.5f);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumTrisInput"), 3), 2.0f);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Versus"), _T("ref")), 2.0f);

	DeclarePriorityLevelByPriorityValue(1, 1.0f);
}

//
TEST_RESULT  CTessellatorTest::SetupTestCase()
{
	TEST_RESULT                 tRes = RESULT_PASS;
	D3D11_PRIMITIVE_TOPOLOGY    nTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

	m_VertexShader.shaderCode = _T("vs_vertid.sh");

	// HS decls
	m_HullShader.shaderCode = _T("#pragma ruledisable \"search for instancing opportunities in hull shaders\"\n");
	m_HullShader.shaderCode += g_cbTessValsDecl;
	m_HullShader.shaderCode += g_cbTessFactorsDecl;
	m_HullShader.shaderCode += g_vsOutDecl;
	m_HullShader.shaderCode += g_hsCpDecl_pos_vertId;

	// DS decls
	m_DomainShader.shaderCode = g_hsCpDecl_pos_vertId;
	m_DomainShader.shaderCode += g_dsOutDecl_domainPos_patchID;

	tstring  domainAttribute = g_domainOpen + m_DomainName + g_attributeClose;
	tstring  partitionAttribute = g_partitionOpen + m_PartitionName + g_attributeClose;
	tstring  outputTopoAttribute = g_outputTopologyOpen + m_TopologyName + g_attributeClose;

	if (m_DomainName == "quad")
	{
		m_HullShader.shaderCode += g_hsConstDecl_quadTessFactors;
		m_HullShader.shaderCode += g_hsConstDecl_finish;
		m_DomainShader.shaderCode += g_hsConstDecl_quadTessFactors;
		m_DomainShader.shaderCode += g_hsConstDecl_finish;
		// HS constant func
		m_HullShader.shaderCode += g_hsQuadEdgeTessFunc;
		// HS control point func
		m_HullShader.shaderCode += domainAttribute;
		m_HullShader.shaderCode += partitionAttribute;
		m_HullShader.shaderCode += outputTopoAttribute;
		m_HullShader.shaderCode += g_patchConstantFuncOpen + "QuadEdgeTessFunc" + g_attributeClose;
		m_HullShader.shaderCode += FormatString(g_hsPassThruCpFunc_2Sizes_OutIn.c_str(), 4, 4);
		nTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

		// DS domain func
		m_DomainShader.shaderCode += domainAttribute;
		m_DomainShader.shaderCode += g_dsQuadDomainPosFunc;
	}
	else
	if (m_DomainName == "tri")
	{
		m_HullShader.shaderCode += g_hsConstDecl_triTessFactors;
		m_HullShader.shaderCode += g_hsConstDecl_finish;
		m_DomainShader.shaderCode += g_hsConstDecl_triTessFactors;
		m_DomainShader.shaderCode += g_hsConstDecl_finish;
		// HS constant func
		m_HullShader.shaderCode += g_hsTriEdgeTessFunc;
		// HS control point func
		m_HullShader.shaderCode += domainAttribute;
		m_HullShader.shaderCode += partitionAttribute;
		m_HullShader.shaderCode += outputTopoAttribute;
		m_HullShader.shaderCode += g_patchConstantFuncOpen + "TriEdgeTessFunc" + g_attributeClose;
		m_HullShader.shaderCode += FormatString(g_hsPassThruCpFunc_2Sizes_OutIn.c_str(), 3, 3);
		nTopology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;

		// DS domain func
		m_DomainShader.shaderCode += domainAttribute;
		m_DomainShader.shaderCode += g_dsTriDomainPosFunc;
	}
	else
	if (m_DomainName == "isoline")
	{
		m_HullShader.shaderCode += g_hsConstDecl_lineTessFactors;
		m_HullShader.shaderCode += g_hsConstDecl_finish;
		m_DomainShader.shaderCode += g_hsConstDecl_lineTessFactors;
		m_DomainShader.shaderCode += g_hsConstDecl_finish;
		// HS constant func
		m_HullShader.shaderCode += g_hsLineEdgeTessFunc;
		// HS control point func
		m_HullShader.shaderCode += domainAttribute;
		m_HullShader.shaderCode += partitionAttribute;
		m_HullShader.shaderCode += outputTopoAttribute;
		m_HullShader.shaderCode += g_patchConstantFuncOpen + "LineEdgeTessFunc" + g_attributeClose;
		m_HullShader.shaderCode += FormatString(g_hsPassThruCpFunc_2Sizes_OutIn.c_str(), 4, 4);
		nTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

		// DS domain func
		m_DomainShader.shaderCode += domainAttribute;
		m_DomainShader.shaderCode += g_dsQuadDomainPosFunc;
	}
	else
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "%s Unexpected domain type" ), LOG_HEADER );
	}

	D3D11_SO_DECLARATION_ENTRY soDecls[] =
	{//   Stream                    SemanticIndex
	 //       SemanticName             StartComponent
	 //                                   ComponentCount
	 //                                      OutputSlot
		{ 0, "SV_Position",         0, 0, 4, 0},
//		{ 0, "SV_DomainLocation",   0, 0, 3, 0},
//		{ 0, "SV_PrimitiveID",      0, 0, 1, 0},
	};

	if ( (tRes = CTessellationConfTest::SetupResources( soDecls, ARRAYSIZE(soDecls) ) ) != RESULT_PASS )
	{
		goto setupFail;
	}

	UINT  offset = 0;

	ON_EACH_CONTEXT->SOSetTargets( 1, &m_pSOBuffer[CONTEXT_ID], &offset );
	ON_EACH_CONTEXT->IASetPrimitiveTopology( nTopology );

setupFail:
	return tRes;
}

//
void CTessellatorTest::CleanupTestCase()
{
	CTessellationConfTest::CleanupResources();
}

//
TEST_RESULT  CTessellatorTest::ExecuteTestCase()
{
	if (m_DomainName == "quad" && m_Versus == "self")
	{
		if ( m_TopologyName == "triangle_cw" ||  m_TopologyName == "triangle_ccw" )
		{
			return VersusSelfTest<tri_sortable>();
		}
		else 
		if ( m_TopologyName == "line" )
		{
			return VersusSelfTest<line_sortable>();
		}
		else
		{
			return VersusSelfTest<float4_sortable>();
		}
	}
	else
	{
		return VersusRefTest();
	}
}

TEST_RESULT CTessellatorTest::VersusRefTest()
{
	TEST_RESULT     tRes = RESULT_PASS;

	// issue queries & draw
	ON_EACH_CONTEXT->Begin( m_pSOQuery[CONTEXT_ID] );
	ON_EACH_CONTEXT->Begin( m_pPipeStatsQuery[CONTEXT_ID] );

	if (m_DomainName == "tri")
	{
		ON_EACH_CONTEXT->Draw(3, 0);
	}
	else
	{
		ON_EACH_CONTEXT->Draw(4, 0);    // quad and isoline
	}

	ON_EACH_CONTEXT->End( m_pSOQuery[CONTEXT_ID] );
	ON_EACH_CONTEXT->End( m_pPipeStatsQuery[CONTEXT_ID] );

	// retrieve queries
	D3D11_QUERY_DATA_PIPELINE_STATISTICS    pipeStats[2] = {0};
	D3D11_QUERY_DATA_SO_STATISTICS          soStats[2] = {0};

	// For debugging purposes
	tRes = CheckPipeStats( pipeStats );

	tRes = CheckSOQueries( soStats );

	if ( tRes == RESULT_PASS )
	{
		UINT64  bufferSizes[2] = { soStats[0].NumPrimitivesWritten, soStats[1].NumPrimitivesWritten };

		if ( m_TopologyName == "triangle_cw" ||  m_TopologyName == "triangle_ccw" )
		{
			tRes = MatchTessellationBuffers<tri_sortable>( bufferSizes );
		}
		else 
		if ( m_TopologyName == "line" )
		{
			tRes = MatchTessellationBuffers<line_sortable>( bufferSizes );
		}
		else
		{
			tRes = MatchTessellationBuffers<float4_sortable>( bufferSizes );
		}
	}

	return tRes;
}

template <typename Ty>
TEST_RESULT CTessellatorTest::VersusSelfTest()
{
	TEST_RESULT		tr = RESULT_PASS;
	TEST_RESULT     tRes = RESULT_PASS;

	ID3D11DeviceContext* context = m_pContextArray[0];

	// 1st pass-------------------    
	// issue queries & draw
	context->Begin( m_pSOQuery[0] );
	context->Begin( m_pPipeStatsQuery[0] );

	if (m_DomainName == "tri")
	{
		context->Draw(3, 0);
	}
	else
	{
		context->Draw(4, 0);    // quad and isoline
	}

	context->End( m_pSOQuery[0] );
	context->End( m_pPipeStatsQuery[0] );

	// retrieve queries
	D3D11_QUERY_DATA_PIPELINE_STATISTICS    pipeStats[2] = {0};
	D3D11_QUERY_DATA_SO_STATISTICS          soStats[2] = {0};

	tr = CheckPipeStats( pipeStats, 1 );
	tRes = tRes == RESULT_PASS ? tr : tRes; // Do not change tRes from RESULT_FAIL to RESULT_PASS

	tr = CheckSOQueries( soStats, 1 );
	tRes = tRes == RESULT_PASS ? tr : tRes; // Do not change tRes from RESULT_FAIL to RESULT_PASS

	size_t bufferSize = (size_t)soStats[0].NumPrimitivesWritten;

	// create first buffer
	std::auto_ptr< TestBuffer<Ty> > buffer0( new TestBuffer<Ty>( 
		m_pDeviceArray[0], m_pContextArray[0], m_pSOBuffer[0], 
		bufferSize, m_TopologyName ) );

	CleanupTestCase();
	tr = SetupTestCase();
	if( tr != RESULT_PASS)
		return tr;


	// 2nd pass----------------------
	// issue queries & draw
	context->Begin( m_pSOQuery[0] );
	context->Begin( m_pPipeStatsQuery[0] );

	if (m_DomainName == "tri")
	{
		context->Draw(3, 0);
	}
	else
	{
		context->Draw(4, 0);    // quad and isoline
	}

	context->End( m_pSOQuery[0] );
	context->End( m_pPipeStatsQuery[0] );

	// retrieve queries
	tr = CheckPipeStats( pipeStats, 1 );
	tRes = tRes == RESULT_PASS ? tr : tRes; // Do not change tRes from RESULT_FAIL to RESULT_PASS

	tr = CheckSOQueries( soStats, 1 );
	tRes = tRes == RESULT_PASS ? tr : tRes; // Do not change tRes from RESULT_FAIL to RESULT_PASS

	bufferSize = (size_t)soStats[0].NumPrimitivesWritten;

	// create second buffer
	std::auto_ptr< TestBuffer<Ty> > buffer1( new TestBuffer<Ty>( 
		m_pDeviceArray[0], m_pContextArray[0], m_pSOBuffer[0], 
		bufferSize, m_TopologyName ) );
		
	
	// now compare two passes ------------------------
	tr = buffer0->Compare( buffer1.get() );
	tRes = tRes == RESULT_PASS ? tr : tRes; // Do not change tRes from RESULT_FAIL to RESULT_PASS

	if (buffer0->HasError())
	{
		WriteToLog( _T( "%s - Domain: %s   Partitioning: %s   Topology: %s\n" ), LOG_HEADER, 
			GetTestCaseParameter( _T("Domain") )->GetValueAsString().c_str(),
			GetTestCaseParameter( _T("Partition") )->GetValueAsString().c_str(),
			GetTestCaseParameter( _T("Topology") )->GetValueAsString().c_str() );
		WriteToLog( _T(" %s "), buffer0->ErrorString().c_str() ); 
	}

	return tRes;
}
