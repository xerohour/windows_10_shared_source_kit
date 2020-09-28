#include "Tessellation.h"

// VS_OUT declaration to allow for variable size of the input to HS
const tstring  hs_vsOutDecl =
"struct VS_OUT"
"{"
"	float4 pos   : pos;"
"	uint vertID  : vertexID;"
"	uint patchID : patchID;"; // used to ensure that when hs_hsNoCPFunc is used, the input and output control point signatures match

const tstring hs_CPDecl_floatVals_1Size =
"	float4 vals[%d] : fvals;";

const tstring hs_CPDecl_intVals_1Size = 
"	int4 vals[%d] : ivals;";

const tstring hs_CPDecl_finish = 
"};"
"\n";


const tstring hs_VSMain = 
"\n""	VS_OUT  vs_main( uint vertexID : SV_VertexID )"
"\n""	{"
"\n""		VS_OUT  output;"
"\n""		int cVals = %d;"	// number of additional elements in VS_OUT
"\n"	
"\n""		output.pos = float4( -1, -1, 0, 1.0 );"
"\n""		output.vertID  = vertexID;"
"\n""		output.patchID = vertexID * 7;"
"\n"
"\n""		if (vertexID == 0)"
"\n""			output.pos.xy = float2( 0, 0 );"
"\n"	
"\n""		if (vertexID == 1)"
"\n""			output.pos.xy = float2( 1, 0 );"
"\n"	
"\n""		if (vertexID == 2)"
"\n""			output.pos.xy = float2( 0, 1 );"
"\n"
"\n""		if (vertexID == 3)"
"\n""			output.pos.xy = float2( 1, 1 );"
"\n""		%s"		// Initialization of additional elements
"\n""		return output;"
"\n""	}";

const tstring hs_VSMain_InitElements = 
"\n"	
"\n""		[unroll]"
"\n""		for (int i = 0; i < cVals; ++i)"
"\n""			output.vals[i] = i * 10.1;"
"\n";


// HS declaration to allow for variable sizes of HS_CP_OUT
const tstring  hs_hsCpDecl_pos_vertId =
"struct HS_CONTROL_POINT_OUT"
"{"
"	float4 pos   : pos;"
"	uint vertID  : vertexID;"
"	uint patchID : patchID;"
"\n";


const tstring  hs_hsPassThruCpFuncClamped =
"\n""[outputcontrolpoints(%d)]"
"\n""HS_CONTROL_POINT_OUT hs_main("
"\n""	InputPatch<VS_OUT, %d> InPts,"	// number of input CPs
"\n""	uint cpId : SV_OutputControlPointID,"
"\n""	uint PatchID : SV_PrimitiveID )"
"\n""{"
"\n""	 int cVSVals = %d;"		// number of values in VS_OUT
"\n""	 int cHSVals = %d;"		// number of values in HS_CP_OUT
"\n""	 HS_CONTROL_POINT_OUT result;"
"\n""	 uint cpIdx = min(cpId, (%d-1));"	// number of input CPs
"\n""	 result.pos = InPts[cpIdx].pos;"
"\n""	 result.vertID = InPts[cpIdx].vertID;"
"\n""	 result.patchID = PatchID;"
"\n""	 result.pos *= float4(10,10,10,10);"
"\n""    %s"	// init additional elements
"\n""	 return result;"
"\n""}"
"\n";

const tstring hs_hsPassThruCpFuncClamped_InitElements = 
"\n"	
"\n""	 [unroll]"
"\n""	 for (int i = 0; i < cHSVals; ++i)"
"\n""	 {"
"\n""		int ValsIndx = min(cVSVals-1, i);"
"\n""	 	result.vals[i] = InPts[cpIdx].vals[ValsIndx] * 10.1;"
"\n""	 }"
"\n";

const tstring  hs_hsNoCPFunc =
"\n""[outputcontrolpoints(%d)]"
"\n""void hs_main(InputPatch<VS_OUT, %d> InPts)"	// number of input CPs)
"\n""{"             
"\n""	 HS_CONTROL_POINT_OUT result;"
"\n""	 int cVSVals = %d;"		// The body of this shader will be optimized out
"\n""	 int cHSVals = %d;"		// it is only here to include the correct number of % so that FormatString will be happy
"\n""	 uint cpIdx = %d-1;"
"\n""    %s"
"\n""}"
"\n";

//
struct int4_sortable
{
	struct {
		INT32 x, y, z, w;
	} it4;

	// HW and REF results should be bit-for-bit similar and able to be compared
	// as RAW bits.
	bool operator < ( const int4_sortable& rhs ) 
	{
		if ( ( memcmp( &it4, &rhs.it4, sizeof(it4) ) >= 0 ) )
		{
			return true;
		}

		return false;
	}

	bool operator == ( const int4_sortable& rhs ) 
	{
		if ( ( memcmp( &it4, &rhs.it4, sizeof(it4) ) == 0 ) )
		{
			return true;
		}

		return false;
	}

	tstring  ToString()
	{
		return FormatString( _T("{0x%x, 0x%x, 0x%x, 0x%x}"), it4.x, it4.y, it4.z, it4.w );
	}
};

struct ds_cp_passthru_vals_no_elements_sortable
{
	float4_sortable     pos;
	UINT                vertID;
	UINT                CPPatchID;
	UINT                HSPatchID;
	UINT                DSPatchID;

	bool operator < ( const ds_cp_passthru_vals_no_elements_sortable& rhs )
	{
		if (pos < rhs.pos)
			return true;
		if (!(pos == rhs.pos))
			return false;

		if (vertID < rhs.vertID)
			return true;
		if (vertID >= rhs.vertID)
			return false;

		if (CPPatchID < rhs.CPPatchID)
			return true;
		if (CPPatchID >= rhs.CPPatchID)
			return false;

		if (HSPatchID < rhs.HSPatchID)
			return true;
		if (HSPatchID >= rhs.HSPatchID)
			return false;

		if (DSPatchID < rhs.DSPatchID)
			return true;
		if (DSPatchID >= rhs.DSPatchID)
			return false;

		return false;
	}

	bool operator == ( const ds_cp_passthru_vals_no_elements_sortable& rhs )
	{
		if (!(pos == rhs.pos))
			return false;
		if (vertID != rhs.vertID)
			return false;
		if (CPPatchID != rhs.CPPatchID)
			return false;
		if (HSPatchID != rhs.HSPatchID)
			return false;
		if (DSPatchID != rhs.DSPatchID)
			return false;

		return true;
	}
	tstring  ToString()
	{
		return _T("{") + pos.ToString() + FormatString( _T(",{%d,%d,%d,%d}}"), vertID, CPPatchID, HSPatchID, DSPatchID );
	}
};

//
template< typename TElement >
struct ds_cp_passthru_vals_sortable :
	public ds_cp_passthru_vals_no_elements_sortable
{
	TElement element;

	bool operator < ( const ds_cp_passthru_vals_sortable< TElement >& rhs )
	{
		if (ds_cp_passthru_vals_no_elements_sortable::operator<( rhs ))
			return true;
		if (!ds_cp_passthru_vals_no_elements_sortable::operator==( rhs ))
			return false;
		if (element < rhs.element)
			return true;
		if (!(element == rhs.element))
			return false;

		return false;
	}

	bool operator == ( const ds_cp_passthru_vals_sortable< TElement >& rhs )
	{
		if (!ds_cp_passthru_vals_no_elements_sortable::operator==( rhs ))
			return false;
		if (!(element == rhs.element))
			return false;

		return true;
	}
	tstring  ToString()
	{
		return _T("{") +
			ds_cp_passthru_vals_no_elements_sortable::ToString() + _T(",") + element.ToString() +
			_T("}");
	}
};

// hull shader stuff

static const char*  g_hsQuadEdgeTessFunc_1Size =
	"\n""HS_CONSTANT_DATA_OUTPUT QuadEdgeTessFunc( "
	"\n""	InputPatch<VS_OUT, %d> inPatch,"
	"\n""	uint PatchID : SV_PrimitiveID )"
	"\n""{	"
	"\n""	HS_CONSTANT_DATA_OUTPUT Output;"
	"\n""	Output.PatchID = PatchID;"
	"\n""	Output.Edges[0] = numCPs; // U0"
	"\n""	Output.Edges[1] = 1; // V0"
	"\n""	Output.Edges[2] = 1; // U1"
	"\n""	Output.Edges[3] = 1; // V1"
	"\n""	Output.Inside[0] = 1; // U"
	"\n""	Output.Inside[1] = 1; // V"
	"\n""	return Output;"
	"\n""}"
	"\n";

static const char*  g_hsTriEdgeTessFunc_1Size =
	"\n""HS_CONSTANT_DATA_OUTPUT TriEdgeTessFunc( "
	"\n""	InputPatch<VS_OUT, %d> inPatch,"
	"\n""	uint PatchID : SV_PrimitiveID )"
	"\n""{	"
	"\n""	HS_CONSTANT_DATA_OUTPUT Output;"
	"\n""	Output.PatchID = PatchID;"
	"\n""	Output.Edges[0] = numCPs; // U"
	"\n""	Output.Edges[1] = 1; // V"
	"\n""	Output.Edges[2] = 1; // W"
	"\n""	Output.Inside[0] = 1; // Inside"
	"\n""	return Output;"
	"\n""}"
	"\n";

static const char*  g_hsLineEdgeTessFunc_1Size =
	"\n""HS_CONSTANT_DATA_OUTPUT LineEdgeTessFunc( "
	"\n""	InputPatch<VS_OUT, %d> inPatch,"
	"\n""	uint PatchID : SV_PrimitiveID )"
	"\n""{	"
	"\n""	HS_CONSTANT_DATA_OUTPUT Output;"
	"\n""	Output.PatchID = PatchID;"
	"\n""	Output.DensityDetail[0] = numCPs;"
	"\n""	Output.DensityDetail[1] = 1;"
	"\n""	return Output;"
	"\n""}"
	"\n";

static const char*  g_hsQuadGenConstsTessFunc_1Size =
	"\n""HS_CONSTANT_DATA_OUTPUT QuadEdgeTessFunc( "
	"\n""	InputPatch<VS_OUT, %d> inPatch,"
	"\n""	uint PatchID : SV_PrimitiveID )"
	"\n""{	"
	"\n""	HS_CONSTANT_DATA_OUTPUT Output;"
	"\n""	Output.PatchID = PatchID;"
	"\n""	Output.Edges[0] = numCPs; // U0"
	"\n""	Output.Edges[1] = 1; // V0"
	"\n""	Output.Edges[2] = 1; // U1"
	"\n""	Output.Edges[3] = 1; // V1"
	"\n""	Output.Inside[0] = 1; // U"
	"\n""	Output.Inside[1] = 1; // V"
	"\n""	[unroll]"
	"\n""	for (int i = 0; i < (int)numConsts; ++i)"
	"\n""		Output.vals[i] = i * 10.1;"
	"\n""	return Output;"
	"\n""}"
	"\n";

static const char*  g_hsTriGenConstsTessFunc_1Size =
	"\n""HS_CONSTANT_DATA_OUTPUT TriEdgeTessFunc( "
	"\n""	InputPatch<VS_OUT, %d> inPatch,"
	"\n""	uint PatchID : SV_PrimitiveID )"
	"\n""{	"
	"\n""	HS_CONSTANT_DATA_OUTPUT Output;"
	"\n""	Output.PatchID = PatchID;"
	"\n""	Output.Edges[0] = numCPs; // U"
	"\n""	Output.Edges[1] = 1; // V"
	"\n""	Output.Edges[2] = 1; // W"
	"\n""	Output.Inside[0] = 1; // I"
	"\n""	[unroll]"
	"\n""	for (int i = 0; i < (int)numConsts; ++i)"
	"\n""		Output.vals[i] = i * 10.1;"
	"\n""	return Output;"
	"\n""}"
	"\n";

static const char*  g_hsLineGenConstsTessFunc_1Size =
	"\n""HS_CONSTANT_DATA_OUTPUT LineEdgeTessFunc( "
	"\n""	InputPatch<VS_OUT, %d> inPatch,"
	"\n""	uint PatchID : SV_PrimitiveID )"
	"\n""{	"
	"\n""	HS_CONSTANT_DATA_OUTPUT Output;"
	"\n""	Output.PatchID = PatchID;"
	"\n""	Output.DensityDetail[0] = numCPs;"
	"\n""	Output.DensityDetail[1] = 1;"
	"\n""	[unroll]"
	"\n""	for (int i = 0; i < (int)numConsts; ++i)"
	"\n""		Output.vals[i] = i * 10.1;"
	"\n""	return Output;"
	"\n""}"
	"\n";

// domain shader stuff

static const char*  g_dsOutDecl_cpPos_vertID_patchID =
	"struct DS_HSCP_OUTPUT"
	"{"
	"	float4 cpPos     : SV_Position;"
	"	uint   vertID    : vertexID;"
	"	uint   CPPatchID : CPPatchID;"
	"	uint   HSPatchID : HSPatchID;"
	"	uint   DSPatchID : DSPatchID;"
	"\n";

static const char*  g_dsOutDecl_floatVal =
	"struct DS_CONSTANT_OUTPUT"
	"{"
	"	float4 val     : fval;"
	"};"
	"\n";

static const char*  g_dsOutDecl_intVal =
	"struct DS_CONSTANT_OUTPUT"
	"{"
	"	int4 val     : ival;"
	"};"
	"\n";

static const char*  g_dsQuadCPOutFunc_1Size =
	"\n""DS_HSCP_OUTPUT ds_main( "
	"\n""	HS_CONSTANT_DATA_OUTPUT input, "
	"\n""	float2 UV : SV_DomainLocation,"
	"\n""	const OutputPatch<HS_CONTROL_POINT_OUT, %d> outCPs,"
	"\n""	uint patchID : SV_PrimitiveID  )"
	"\n""{"
	"\n""	int cHSVals = %d;"		// number of additional elements in HS_CP_OUT
	"\n""	int cpIndex = 0;"
	"\n""	if (UV.x == 0) cpIndex = UV.x * numCPs;"
	"\n""	if (cpIndex >= numCPs) cpIndex = 0;"
	"\n""	DS_HSCP_OUTPUT Output;"
	"\n""	Output.cpPos = outCPs[cpIndex].pos;"
	"\n""	Output.vertID = outCPs[cpIndex].vertID;"
	"\n""	Output.CPPatchID = outCPs[cpIndex].patchID;"
	"\n""	Output.HSPatchID = input.PatchID;"
	"\n""	Output.DSPatchID = patchID;"
	"\n""	%s"	// init additional elements
	"\n""	return Output;"
	"\n""}"
	"\n";

static const char*  g_dsTriCPOutFunc_1Size =
	"\n""DS_HSCP_OUTPUT ds_main( "
	"\n""	HS_CONSTANT_DATA_OUTPUT input, "
	"\n""	float3 UVW : SV_DomainLocation,"
	"\n""	const OutputPatch<HS_CONTROL_POINT_OUT, %d> outCPs,"
	"\n""	uint patchID : SV_PrimitiveID  )"
	"\n""{"
	"\n""	int cHSVals = %d;"		// number of additional elements in HS_CP_OUT
	"\n""	int cpIndex = 0;"
	"\n""	if (UVW.x == 0) cpIndex = UVW.x * numCPs;"
	"\n""	if (cpIndex >= numCPs) cpIndex = 0;"
	"\n""	DS_HSCP_OUTPUT Output;"
	"\n""	Output.cpPos = outCPs[cpIndex].pos;"
	"\n""	Output.vertID = outCPs[cpIndex].vertID;"
	"\n""	Output.CPPatchID = outCPs[cpIndex].patchID;"
	"\n""	Output.HSPatchID = input.PatchID;"
	"\n""	Output.DSPatchID = patchID;"
	"\n""	%s"	// init additional elements
	"\n""	return Output;"
	"\n""}"
	"\n";

const tstring g_dsInitElements = 
"\n"
"\n""	[unroll]"
"\n""	for (int i = 0; i < cHSVals; ++i)"
"\n""		Output.vals[i] = outCPs[cpIndex].vals[i];"
"\n";

static const char*  g_dsQuadPatchConstsOutFunc_1Size =
	"\n""DS_CONSTANT_OUTPUT ds_main( "
	"\n""	HS_CONSTANT_DATA_OUTPUT input, "
	"\n""	float2 UV : SV_DomainLocation,"
	"\n""	const OutputPatch<HS_CONTROL_POINT_OUT, %d> outCPs,"
	"\n""	uint patchID : SV_PrimitiveID  )"
	"\n""{"
	"\n""	int constIndex = 0;"
	"\n""	if (UV.x == 0) constIndex = UV.x * numConsts;"
	"\n""	if (constIndex >= numConsts) constIndex = 0;"
	"\n""	DS_CONSTANT_OUTPUT Output;"
	"\n""	Output.val = input.vals[constIndex];"
	"\n""	return Output;"
	"\n""}"
	"\n";

static const char*  g_dsTriPatchConstsOutFunc_1Size =
	"\n""DS_CONSTANT_OUTPUT ds_main( "
	"\n""	HS_CONSTANT_DATA_OUTPUT input, "
	"\n""	float3 UVW : SV_DomainLocation,"
	"\n""	const OutputPatch<HS_CONTROL_POINT_OUT, %d> outCPs,"
	"\n""	uint patchID : SV_PrimitiveID  )"
	"\n""{"
	"\n""	int constIndex = 0;"
	"\n""	if (UVW.x == 0) constIndex = UVW.x * numConsts;"
	"\n""	if (constIndex >= numConsts) constIndex = 0;"
	"\n""	DS_CONSTANT_OUTPUT Output;"
	"\n""	Output.val = input.vals[constIndex];"
	"\n""	return Output;"
	"\n""}"
	"\n";

static float g_numConstants[] =
{
	1, 2, 3, 5, 8, 16, 25, 27, 29,
};

static UINT g_numCPElements[] = 
{
	1, 9, 30,
};

//
void  CHullShaderTest::InitTestParameters()
{
	CTestCaseParameter<HullOutputType>* pOutputTypeParam = AddParameter<HullOutputType>( _T("OutputType"), &m_OutputType );
	testfactor::RFactor rOutputType_CtrlPts= AddParameterValue<HullOutputType>( pOutputTypeParam, HullOutput_ControlPoints);
	testfactor::RFactor rOutputType_PConsts = AddParameterValue<HullOutputType>( pOutputTypeParam, HullOutput_PatchConstants);

	CTestCaseParameter<tstring>* pDomainParam = AddParameter<tstring>( _T("Domain"), &m_DomainName );
	testfactor::RFactor rDomain_Quad = AddParameterValue<tstring>( pDomainParam, _T("quad"));
	testfactor::RFactor rDomain_Tri = AddParameterValue<tstring>( pDomainParam, _T("tri"));
	testfactor::RFactor rDomain_Isoline = AddParameterValue<tstring>( pDomainParam, _T("isoline"));
	testfactor::RFactor rDomain_All 
		= rDomain_Quad + rDomain_Tri + rDomain_Isoline;

	// This is available for later expansion.
	CTestCaseParameter<tstring>*  pPartitionParam = AddParameter<tstring>( _T("Partition"), &m_PartitionName );
	testfactor::RFactor  rPartition_Int = AddParameterValue<tstring>( pPartitionParam, _T("integer"));
	testfactor::RFactor  rPartition_All = rPartition_Int;

	// This is available for later expansion.
	CTestCaseParameter<tstring>*  pTopologyParam = AddParameter<tstring>( _T("Topology"), &m_TopologyName );
	testfactor::RFactor  rTopology_Point = AddParameterValue<tstring>( pTopologyParam, _T("point"));
	testfactor::RFactor  rTopology_All = rTopology_Point; 

	CTestCaseParameter<UINT>* pNumCPsInParam = AddParameter<UINT>( _T("NumCPsIn"), &m_NumCPsIn );
	testfactor::RFactor rNumCPsIn_1 = AddParameterValue<UINT>( pNumCPsInParam, 1);
	testfactor::RFactor rNumCPsIn_32 = AddParameterValue<UINT>( pNumCPsInParam, 32);
	testfactor::RFactor rNumCPsIn_All = AddParameterValueSet<UINT>( pNumCPsInParam, RangeValueSet<UINT>(1,32,1));

	CTestCaseParameter<float>* pNumCPsOutParam = AddParameter<float>( _T("NumCPsOut"), &m_TessVals[0] );
	testfactor::RFactor rNumCPsOut_1 = AddParameterValue<float>( pNumCPsOutParam, 1);
	testfactor::RFactor rNumCPsOut_32 = AddParameterValue<float>( pNumCPsOutParam, 32);
	testfactor::RFactor rNumCPsOut_All = AddParameterValueSet<float>( pNumCPsOutParam, GradientValueSet<float>(1,32,32));

	CTestCaseParameter<UINT>* pNumInCPElements = AddParameter<UINT>( _T("NumInCPElements"), &m_NumInCPElements );
	testfactor::RFactor rNumInCPElements_0 = AddParameterValue<UINT>( pNumInCPElements, 0);
	testfactor::RFactor rNumInCPElements_1 = AddParameterValue<UINT>( pNumInCPElements, 1);
	testfactor::RFactor rNumInCPElements_All = AddParameterValueSet<UINT>( pNumInCPElements,
			new CTableValueSet<UINT>(g_numCPElements, sizeof(g_numCPElements[0]), ARRAYSIZE(g_numCPElements)));

	CTestCaseParameter<UINT>* pNumOutCPElements = AddParameter<UINT>( _T("NumOutCPElements"), &m_NumOutCPElements );
	testfactor::RFactor rNumOutCPElements_0 = AddParameterValue<UINT>( pNumOutCPElements, 0);
	testfactor::RFactor rNumOutCPElements_1 = AddParameterValue<UINT>( pNumOutCPElements, 1);
	testfactor::RFactor rNumOutCPElements_All = AddParameterValueSet<UINT>( pNumOutCPElements,
		new CTableValueSet<UINT>(g_numCPElements, sizeof(g_numCPElements[0]), ARRAYSIZE(g_numCPElements)));

	CTestCaseParameter<tstring>* pCPElementsDataType = AddParameter<tstring>( _T("CPElementsDataType"), &m_CPElementsDataType );
	testfactor::RFactor rCPElements_Float = AddParameterValue<tstring>( pCPElementsDataType, _T("float"));
	testfactor::RFactor rCPElements_Int = AddParameterValue<tstring>( pCPElementsDataType, _T("int"));
	testfactor::RFactor rCPElements_All 
		= rCPElements_Float + rCPElements_Int;


	CTestCaseParameter<float>* pNumConstsParam = AddParameter<float>( _T("NumConsts"), &m_TessVals[1] );
	testfactor::RFactor rNumConsts_0 = AddParameterValue<float>( pNumConstsParam, 0);
	testfactor::RFactor rNumConsts_1 = AddParameterValue<float>( pNumConstsParam, 1);
	testfactor::RFactor rNumConsts_25 = AddParameterValue<float>( pNumConstsParam, 25);
	testfactor::RFactor rNumConsts_All 
		= AddParameterValueSet<float>( pNumConstsParam, 
		new CTableValueSet<float>(g_numConstants, sizeof(g_numConstants[0]), ARRAYSIZE(g_numConstants)) );

	CTestCaseParameter<tstring>* pConstDataParam = AddParameter<tstring>( _T("ConstDataType"), &m_ConstDataType );
	testfactor::RFactor rConstData_Float = AddParameterValue<tstring>( pConstDataParam, _T("float"));
	testfactor::RFactor rConstData_Int = AddParameterValue<tstring>( pConstDataParam, _T("int"));
	testfactor::RFactor rConstData_All 
		= rConstData_Float + rConstData_Int;

	CTestCaseParameter<bool>* pHasControlPointPhase = AddParameter<bool>( _T("HSHasCPPhase"), &m_HasCPPhase );
	SetParameterDefaultValue<bool>( pHasControlPointPhase, true );
	testfactor::RFactor rHasCPPhase_false = AddParameterValue<bool>( pHasControlPointPhase, false );

	// test cases with input and output control points of variable sizes
	testfactor::RFactor rCPOutElements = rOutputType_CtrlPts * rDomain_All * rPartition_All * rTopology_All * ((rNumCPsOut_1 + rNumCPsOut_32) * (rNumCPsIn_1 + rNumCPsIn_32)) * (rNumInCPElements_All * rNumOutCPElements_All) * rCPElements_All * rNumConsts_0;

	// test cases with no control point phase in hs
	testfactor::RFactor rNoCPPhase = rHasCPPhase_false * rOutputType_CtrlPts * rDomain_All * rPartition_All * rTopology_All * (rNumCPsIn_All % rNumCPsOut_All) * rNumInCPElements_0 * rNumOutCPElements_0 * rCPElements_All * rNumConsts_0 * rConstData_All;
	
	// All test cases
	testfactor::RFactor rootFactor 
		= rOutputType_CtrlPts * rDomain_All * rPartition_All * rTopology_All * ((rNumCPsIn_All * (rNumCPsOut_1 + rNumCPsOut_32)) + ((rNumCPsIn_1 + rNumCPsIn_32) * rNumCPsOut_All)) * rNumConsts_0 * rNumInCPElements_0 * rNumOutCPElements_0
		+ rOutputType_PConsts * rDomain_All * rPartition_All * rTopology_All * (rNumCPsIn_1 * rNumCPsOut_1) * rConstData_All * rNumConsts_All * rNumInCPElements_0 * rNumOutCPElements_0
		+ rCPOutElements
		+ rNoCPPhase
		;

	SetRootTestFactor(rootFactor);
}

//
TEST_RESULT  CHullShaderTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	m_VertexShader.location = SHADER_SOURCE_MEMORY_HLSL;

	tstring controlPointShader = m_HasCPPhase ? hs_hsPassThruCpFuncClamped : hs_hsNoCPFunc;

	tstring VSOutDecl = hs_vsOutDecl;
	if(m_NumInCPElements)
	{
		if(m_CPElementsDataType == "float")
		{
			VSOutDecl += hs_CPDecl_floatVals_1Size;
		}
		else
		{
			VSOutDecl += hs_CPDecl_intVals_1Size;
		}
	}
	VSOutDecl += hs_CPDecl_finish;
	VSOutDecl = FormatString(VSOutDecl.c_str(), m_NumInCPElements);
	
	const tstring VSInitElements = !m_NumInCPElements ? "" : hs_VSMain_InitElements;

	m_VertexShader.shaderCode = _T("");
	m_VertexShader.shaderCode += VSOutDecl;
	m_VertexShader.shaderCode += hs_VSMain;
	m_VertexShader.shaderCode = FormatString(m_VertexShader.shaderCode.c_str(), m_NumInCPElements, VSInitElements.c_str());

	// HS decls
	tstring HSOutDecl = hs_hsCpDecl_pos_vertId;
	tstring DSOutDecl = g_dsOutDecl_cpPos_vertID_patchID;

	if(m_NumOutCPElements)
	{
		if(m_CPElementsDataType == "float")
		{
			HSOutDecl += hs_CPDecl_floatVals_1Size;
			DSOutDecl += hs_CPDecl_floatVals_1Size;
		}
		else
		{
			HSOutDecl += hs_CPDecl_intVals_1Size;
			DSOutDecl += hs_CPDecl_intVals_1Size;
		}
	}
	HSOutDecl = FormatString(HSOutDecl.c_str(), m_NumOutCPElements);
	HSOutDecl += hs_CPDecl_finish;
	DSOutDecl = FormatString(DSOutDecl.c_str(), m_NumOutCPElements);
	DSOutDecl += hs_CPDecl_finish;

	m_HullShader.shaderCode = _T("");
	m_HullShader.shaderCode += g_cbNumCPsDecl;
	m_HullShader.shaderCode += VSOutDecl;
	m_HullShader.shaderCode += HSOutDecl;


	// DS decls
	m_DomainShader.shaderCode = _T("");
	m_DomainShader.shaderCode += g_cbNumCPsDecl;
	m_DomainShader.shaderCode += HSOutDecl;

	if (m_OutputType == HullOutput_ControlPoints)
		m_DomainShader.shaderCode += DSOutDecl;
	else
	{
		if ("float" == m_ConstDataType)
			m_DomainShader.shaderCode += g_dsOutDecl_floatVal;
		else if ("int" == m_ConstDataType)
			m_DomainShader.shaderCode += g_dsOutDecl_intVal;
	}

	tstring  domainAttribute = g_domainOpen + m_DomainName + g_attributeClose;
	tstring  partitionAttribute = g_partitionOpen + m_PartitionName + g_attributeClose;
	tstring  outputTopoAttribute = g_outputTopologyOpen + m_TopologyName + g_attributeClose;

	const int numCPsOut = (int)(m_TessVals[0] + 0.5);
	const int numConsts = (int)(m_TessVals[1]);
	const tstring  constValsStr 
		= (numConsts <= 0) 
		? "" 
		: FormatString(("float" == m_ConstDataType) ? g_hsConstDecl_floatVals_1Size.c_str() : g_hsConstDecl_intVals_1Size.c_str(), numConsts);

	const tstring HSIntiElements = !m_NumOutCPElements ? "" : hs_hsPassThruCpFuncClamped_InitElements;
	const tstring DSInitElements = !m_NumOutCPElements ? "" : g_dsInitElements;

	// max allowed CP is up to 32 4-vector values. Also, total scalars across all HS CPs have to be <= 3968
	UINT SizeOfElement = 4; // in 32-bit values
	UINT TotalScalars = ((m_NumOutCPElements * SizeOfElement) + 6 /*vertexID, PatchID, pos*/) * (UINT)m_TessVals[0]/*numCPsOut*/;
	if(m_NumInCPElements > 30 || m_NumOutCPElements > 30 || TotalScalars > 3968)	
	{
		tRes = RESULT_SKIP;
		goto setupDone;
	}

	if (m_DomainName == "quad")
	{
		if (numConsts > 25) // 4 edges + 2 inside + patchID + 25 constants = 32
		{
			tRes = RESULT_SKIP;
			goto setupDone;
		}

		m_HullShader.shaderCode += g_hsConstDecl_quadTessFactors;
		m_DomainShader.shaderCode += g_hsConstDecl_quadTessFactors;
		m_HullShader.shaderCode += constValsStr;
		m_DomainShader.shaderCode += constValsStr;
		m_HullShader.shaderCode += g_hsConstDecl_finish;
		m_DomainShader.shaderCode += g_hsConstDecl_finish;

		// HS constant func
		if (m_OutputType == HullOutput_ControlPoints)
			m_HullShader.shaderCode += FormatString(g_hsQuadEdgeTessFunc_1Size, m_NumCPsIn);
		else
			m_HullShader.shaderCode += FormatString(g_hsQuadGenConstsTessFunc_1Size, m_NumCPsIn);

		// HS control point func
		m_HullShader.shaderCode += domainAttribute;
		m_HullShader.shaderCode += partitionAttribute;
		m_HullShader.shaderCode += outputTopoAttribute;
		m_HullShader.shaderCode += g_patchConstantFuncOpen + "QuadEdgeTessFunc" + g_attributeClose;
		m_HullShader.shaderCode += FormatString(controlPointShader.c_str(), numCPsOut, m_NumCPsIn, m_NumInCPElements, m_NumOutCPElements, m_NumCPsIn, HSIntiElements.c_str());

		// DS domain func
		m_DomainShader.shaderCode += domainAttribute;
		if (m_OutputType == HullOutput_ControlPoints)
			m_DomainShader.shaderCode += FormatString(g_dsQuadCPOutFunc_1Size, numCPsOut, m_NumOutCPElements, DSInitElements.c_str());
		else
			m_DomainShader.shaderCode += FormatString(g_dsQuadPatchConstsOutFunc_1Size, numCPsOut);
	}
	else
	if (m_DomainName == "tri")
	{
		if (numConsts > 27) // 3 edges + 1 inside + patchID + 27 constants = 32
		{
			tRes = RESULT_SKIP;
			goto setupDone;
		}

		m_HullShader.shaderCode += g_hsConstDecl_triTessFactors;
		m_DomainShader.shaderCode += g_hsConstDecl_triTessFactors;
		m_HullShader.shaderCode += constValsStr;
		m_DomainShader.shaderCode += constValsStr;
		m_HullShader.shaderCode += g_hsConstDecl_finish;
		m_DomainShader.shaderCode += g_hsConstDecl_finish;

		// HS constant func
		if (m_OutputType == HullOutput_ControlPoints)
			m_HullShader.shaderCode += FormatString(g_hsTriEdgeTessFunc_1Size, m_NumCPsIn);
		else
			m_HullShader.shaderCode += FormatString(g_hsTriGenConstsTessFunc_1Size, m_NumCPsIn);

		// HS control point func
		m_HullShader.shaderCode += domainAttribute;
		m_HullShader.shaderCode += partitionAttribute;
		m_HullShader.shaderCode += outputTopoAttribute;
		m_HullShader.shaderCode += g_patchConstantFuncOpen + "TriEdgeTessFunc" + g_attributeClose;
		m_HullShader.shaderCode += FormatString(controlPointShader.c_str(), numCPsOut, m_NumCPsIn, m_NumInCPElements, m_NumOutCPElements, m_NumCPsIn, HSIntiElements.c_str());

		// DS domain func
		m_DomainShader.shaderCode += domainAttribute;

		if (m_OutputType == HullOutput_ControlPoints)
			m_DomainShader.shaderCode += FormatString(g_dsTriCPOutFunc_1Size, numCPsOut, m_NumOutCPElements, DSInitElements.c_str());
		else
			m_DomainShader.shaderCode += FormatString(g_dsTriPatchConstsOutFunc_1Size, numCPsOut);
	}
	else
	if (m_DomainName == "isoline")
	{
		if (numConsts > 29) // 2 tessfactors + patchID + 29 constants = 32
		{
			tRes = RESULT_SKIP;
			goto setupDone;
		}

		m_HullShader.shaderCode += g_hsConstDecl_lineTessFactors;
		m_DomainShader.shaderCode += g_hsConstDecl_lineTessFactors;
		m_HullShader.shaderCode += constValsStr;
		m_DomainShader.shaderCode += constValsStr;
		m_HullShader.shaderCode += g_hsConstDecl_finish;
		m_DomainShader.shaderCode += g_hsConstDecl_finish;

		// HS constant func
		if (m_OutputType == HullOutput_ControlPoints)
			m_HullShader.shaderCode += FormatString(g_hsLineEdgeTessFunc_1Size, m_NumCPsIn);
		else
			m_HullShader.shaderCode += FormatString(g_hsLineGenConstsTessFunc_1Size, m_NumCPsIn);

		// HS control point func
		m_HullShader.shaderCode += domainAttribute;
		m_HullShader.shaderCode += partitionAttribute;
		m_HullShader.shaderCode += outputTopoAttribute;
		m_HullShader.shaderCode += g_patchConstantFuncOpen + "LineEdgeTessFunc" + g_attributeClose;
		m_HullShader.shaderCode += FormatString(controlPointShader.c_str(), numCPsOut, m_NumCPsIn, m_NumInCPElements, m_NumOutCPElements, m_NumCPsIn, HSIntiElements.c_str());

		// DS domain func
		m_DomainShader.shaderCode += domainAttribute;

		if (m_OutputType == HullOutput_ControlPoints)
			m_DomainShader.shaderCode += FormatString(g_dsQuadCPOutFunc_1Size, numCPsOut, m_NumOutCPElements, DSInitElements.c_str());
		else
			m_DomainShader.shaderCode += FormatString(g_dsQuadPatchConstsOutFunc_1Size, numCPsOut);
	}
	else
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "%s Unexpected domain type" ), LOG_HEADER );
	}

	if (m_OutputType == HullOutput_ControlPoints)
	{
		const char*  semanticName = NULL;
		if (m_NumOutCPElements)
		{
			if ("float" == m_CPElementsDataType)
			{
				semanticName = "fvals";
				m_pfnMatchTessellationBuffers =
					&CHullShaderTest::MatchTessellationBuffers< ds_cp_passthru_vals_sortable< float4_sortable > >;
			}
			else if ("int" == m_CPElementsDataType)
			{
				semanticName = "ivals";
				m_pfnMatchTessellationBuffers =
					&CHullShaderTest::MatchTessellationBuffers< ds_cp_passthru_vals_sortable< int4_sortable > >;
			}
		}
		else
		{
			m_pfnMatchTessellationBuffers =
				&CHullShaderTest::MatchTessellationBuffers< ds_cp_passthru_vals_no_elements_sortable >;
		}

		D3D11_SO_DECLARATION_ENTRY soDeclsNoElements[] =
		{//  Stream             SemanticIndex
		 //      SemanticName      StartComponent
		 //                           ComponentCount
		 //                              OutputSlot
			{ 0, "SV_Position", 0, 0, 4, 0},
			{ 0, "vertexID",    0, 0, 1, 0},
			{ 0, "CPPatchID",   0, 0, 1, 0},
			{ 0, "HSPatchID",   0, 0, 1, 0},
			{ 0, "DSPatchID",   0, 0, 1, 0},
		};

		D3D11_SO_DECLARATION_ENTRY soDecls[] =
		{	//  Stream             SemanticIndex
			//      SemanticName      StartComponent
			//                           ComponentCount
			//                              OutputSlot
			{ 0, "SV_Position", 0, 0, 4, 0},
			{ 0, "vertexID",    0, 0, 1, 0},
			{ 0, "CPPatchID",   0, 0, 1, 0},
			{ 0, "HSPatchID",   0, 0, 1, 0},
			{ 0, "DSPatchID",   0, 0, 1, 0},
			{ 0, semanticName, 0, 0, 4, 0},
		};

		D3D11_SO_DECLARATION_ENTRY* pDecls = ((m_NumOutCPElements) ? soDecls : soDeclsNoElements);
		UINT pDeclsSize = ((m_NumOutCPElements) ? ARRAYSIZE(soDecls) : ARRAYSIZE(soDeclsNoElements));
		tRes = CTessellationConfTest::SetupResources(pDecls, pDeclsSize);
		if (tRes != RESULT_PASS)
			goto setupDone;
	}
	else
	{
		const char*  semanticName = NULL;
		if ("float" == m_ConstDataType)
		{
			semanticName = "fval";
			m_pfnMatchTessellationBuffers =
				&CHullShaderTest::MatchTessellationBuffers< float4_sortable >;
		}
		else if ("int" == m_ConstDataType)
		{
			semanticName = "ival";
			m_pfnMatchTessellationBuffers =
				&CHullShaderTest::MatchTessellationBuffers< int4_sortable >;
		}

		D3D11_SO_DECLARATION_ENTRY soDecls[] =
		{//  Stream             SemanticIndex
		 //      SemanticName      StartComponent
		 //                           ComponentCount
		 //                              OutputSlot
			{ 0, semanticName, 0, 0, 4, 0},
		};
		tRes = CTessellationConfTest::SetupResources(soDecls, ARRAYSIZE(soDecls));
		if (tRes != RESULT_PASS)
			goto setupDone;
	}

	UINT offset = 0;
	ON_EACH_CONTEXT->SOSetTargets(1, &m_pSOBuffer[CONTEXT_ID], &offset);

	assert(m_NumCPsIn <= 32);
	const D3D11_PRIMITIVE_TOPOLOGY topology
		= (D3D11_PRIMITIVE_TOPOLOGY)(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST + m_NumCPsIn - 1);
	ON_EACH_CONTEXT->IASetPrimitiveTopology(topology);

setupDone:
	return tRes;
}

//
void  CHullShaderTest::CleanupTestCase()
{
	m_pfnMatchTessellationBuffers = NULL;
	CTessellationConfTest::CleanupResources();
}

//
TEST_RESULT  CHullShaderTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	// issue queries & draw
	ON_EACH_CONTEXT->Begin(m_pSOQuery[CONTEXT_ID]);
	ON_EACH_CONTEXT->Draw(m_NumCPsIn, 0);
	ON_EACH_CONTEXT->End(m_pSOQuery[CONTEXT_ID]);

	// retrieve queries
	D3D11_QUERY_DATA_SO_STATISTICS soStats[2];
	tRes = CheckSOQueries(soStats);

	if (RESULT_PASS == tRes)
	{
		UINT64 bufferSizes[2] = {soStats[0].NumPrimitivesWritten, soStats[1].NumPrimitivesWritten};
		tRes = (this->*m_pfnMatchTessellationBuffers)( bufferSizes );
	}

	return tRes;
}
