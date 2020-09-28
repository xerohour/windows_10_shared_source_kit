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
	"\n""	Output.domainPos = outCPs[0].pos + outCPs[1].pos + outCPs[2].pos + outCPs[3].pos;"
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
	"\n""	Output.domainPos = outCPs[0].pos + outCPs[1].pos + outCPs[2].pos;"
	"\n""	Output.domainPos.xyz = UVW;"
	"\n""	Output.HSPatchID = input.PatchID;"
	"\n""	Output.DSPatchID = patchID;"
	"\n""	return Output;"
	"\n""}"
	"\n";

extern const UINT  g_numTriangles;

//
void  CDomainShaderTest::InitTestParameters()
{
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
	m_TessVals[2] = m_TessVals[3] = 64;

	testfactor::RFactor  rTessVal0_4 = AddParameterValue<float>( pTessValParam0, 4 );
	testfactor::RFactor  rTessVal0_32 = AddParameterValue<float>( pTessValParam0, 32 );

	testfactor::RFactor  rTessVal1_4 = AddParameterValue<float>( pTessValParam1, 4 );
	testfactor::RFactor  rTessVal1_32 = AddParameterValue<float>( pTessValParam1, 32 );
  
	testfactor::RFactor  rTessVal_All_4_32 = (rTessVal0_4 + rTessVal0_32) * (rTessVal1_4 + rTessVal1_32);

	CTestCaseParameter<UINT>* pEdgesTessFactorParam0 = AddParameter<UINT>( _T("EdgesTessFactor0"), &m_uintTessFactors[0] );
	testfactor::RFactor rTessFactor0_8 = AddParameterValue<UINT>( pEdgesTessFactorParam0, 8 );
	testfactor::RFactor rTessFactor0_64 = AddParameterValue<UINT>( pEdgesTessFactorParam0, 64 );

	CTestCaseParameter<UINT>* pEdgesTessFactorParam1 = AddParameter<UINT>( _T("EdgesTessFactor1"), &m_uintTessFactors[1] );
	testfactor::RFactor rTessFactor1_8 = AddParameterValue<UINT>( pEdgesTessFactorParam1, 8 );
	testfactor::RFactor rTessFactor1_64 = AddParameterValue<UINT>( pEdgesTessFactorParam1, 64 );

	CTestCaseParameter<UINT>* pEdgesTessFactorParam2 = AddParameter<UINT>( _T("EdgesTessFactor2"), &m_uintTessFactors[2] );
	testfactor::RFactor rTessFactor2_8 = AddParameterValue<UINT>( pEdgesTessFactorParam2, 8 );
	testfactor::RFactor rTessFactor2_64 = AddParameterValue<UINT>( pEdgesTessFactorParam2, 64 );

	
	m_uintTessFactors[3] = 4;
	m_nNumTrisInput = g_numTriangles;


	testfactor::RFactor  rTriFactors = rDomain_Tri * (rPartition_Int + rPartition_Pow2 + rPartition_FracOdd + rPartition_FracEven)
										* (rTopology_Point + rTopology_Triangle_CW + rTopology_Triangle_CCW)
										* (rTessFactor0_8 + rTessFactor0_64) * (rTessFactor1_8 + rTessFactor1_64)
										* (rTessFactor2_8 + rTessFactor2_64);
										
	testfactor::RFactor  rQuadFactors = rDomain_Quad * rPartition_All
										* (rTopology_Point + rTopology_Triangle_CW + rTopology_Triangle_CCW)
										* rTessVal_All_4_32;

	testfactor::RFactor  rIsolineFactors = rDomain_Isoline * rPartition_All * (rTopology_Point + rTopology_Line)
										   * rTessVal_All_4_32;
										
	testfactor::RFactor  rootFactor = rQuadFactors + rTriFactors + rIsolineFactors;

	SetRootTestFactor( rootFactor );

	return;
}

//
TEST_RESULT  CDomainShaderTest::SetupTestCase()
{
	TEST_RESULT                 tRes = RESULT_PASS;
	D3D11_PRIMITIVE_TOPOLOGY    nTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

	m_VertexShader.shaderCode = _T("vs_vertid.sh");

	tstring constantDeclarations("");
	tstring hsConstantFunction("");
	tstring hsCPFunction("");
	tstring dsDomainPosFunction("");
	int noOfPoints(0);

	if (m_DomainName == "quad")
	{
		constantDeclarations = g_hsConstDecl_quadTessFactors; //*
		hsConstantFunction = g_hsQuadEdgeTessFunc; //*
		hsCPFunction = "QuadEdgeTessFunc";//*
		noOfPoints = 4;
		dsDomainPosFunction = g_dsQuadDomainPosFunc;//*
		nTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;//*
	}
	else
	if (m_DomainName == "tri")
	{
		constantDeclarations = g_hsConstDecl_triTessFactors; //*
		hsConstantFunction = g_hsTriEdgeTessFunc; //*
		hsCPFunction = "TriEdgeTessFunc";//*
		noOfPoints = 3;
		dsDomainPosFunction = g_dsTriDomainPosFunc;//*
		nTopology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;//*
	}
	else
	if (m_DomainName == "isoline")
	{
		constantDeclarations = g_hsConstDecl_lineTessFactors;//*
		hsConstantFunction = g_hsLineEdgeTessFunc;//*
		hsCPFunction = "LineEdgeTessFunc";//*
		noOfPoints = 4;
		dsDomainPosFunction = g_dsQuadDomainPosFunc;//*
		nTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;//*
	}
	else
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "%s Unexpected domain type" ), LOG_HEADER );
	}
	
	
	tstring  domainAttribute = g_domainOpen + m_DomainName + g_attributeClose;
	tstring  partitionAttribute = g_partitionOpen + m_PartitionName + g_attributeClose;
	tstring  outputTopoAttribute = g_outputTopologyOpen + m_TopologyName + g_attributeClose;

	// HS 
	// BUGBUG - 
	m_HullShader.shaderCode = _T("#pragma ruledisable \"search for instancing opportunities in hull shaders\"\n");
	m_HullShader.shaderCode += g_cbTessValsDecl + g_cbTessFactorsDecl + g_vsOutDecl + g_hsCpDecl_pos_vertId
		+ constantDeclarations + g_hsConstDecl_finish
		+ hsConstantFunction
		+ domainAttribute + partitionAttribute + outputTopoAttribute
		+ g_patchConstantFuncOpen + hsCPFunction + g_attributeClose
		+ FormatString(g_hsPassThruCpFunc_2Sizes_OutIn.c_str(), noOfPoints, noOfPoints);

	// DS 
	m_DomainShader.shaderCode = g_hsCpDecl_pos_vertId + g_dsOutDecl_domainPos_patchID + constantDeclarations + g_hsConstDecl_finish
		+ domainAttribute + dsDomainPosFunction;//*

	D3D11_SO_DECLARATION_ENTRY soDecls[] =
	{//   Stream                    SemanticIndex
	 //       SemanticName             StartComponent
	 //                                   ComponentCount
	 //                                      OutputSlot
		{ 0, "SV_Position",         0, 0, 4, 0},
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
void  CDomainShaderTest::CleanupTestCase()
{
	CTessellationConfTest::CleanupResources();
}

//
TEST_RESULT  CDomainShaderTest::ExecuteTestCase()
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
