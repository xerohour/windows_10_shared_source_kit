#include "Tessellation.h"

// common stuff

const tstring  g_vsOutDecl =
	"struct VS_OUT"
	"{"
	"	float4 pos  : pos;"
	"	uint vertID : vertexID;"
	"};"
	"\n";

const tstring  g_cbTessValsDecl =
	"cbuffer cb0 : register(b0)"
	"{"
	"	float tessVals[4];"
	"};"
	"\n";

const tstring  g_cbTessFactorsDecl =
	"cbuffer cb1 : register(b1)"
	"{"
	"	float tessFactors[4];"
	"};"
	"\n";

const tstring  g_cbNumCPsDecl =
	"cbuffer cb0 : register(b0)"
	"{"
	"	float numCPs;"
	"	float numConsts;"
	"};"
	"\n";

tstring  g_domainOpen =
	"\n""[domain(\"";
tstring  g_partitionOpen =
	"\n""[partitioning(\"";
tstring  g_outputTopologyOpen =
	"\n""[outputtopology(\"";
tstring  g_patchConstantFuncOpen =
	"\n""[patchconstantfunc(\"";
tstring  g_attributeClose =
	"\")]";

// hull shader stuff

const tstring  g_hsCpDecl_pos_vertId =
	"struct HS_CONTROL_POINT_OUT"
	"{"
	"	float4 pos   : pos;"
	"	uint vertID  : vertexID;"
	"	uint patchID : patchID;"
	"};"
	"\n";

const tstring  g_hsPassThruCpFunc_2Sizes_OutIn =
	"\n""[outputcontrolpoints(%d)]"
	"\n""HS_CONTROL_POINT_OUT hs_main("
	"\n""	InputPatch<VS_OUT, %d> InPts,"
	"\n""	uint cpId : SV_OutputControlPointID,"
	"\n""	uint PatchID : SV_PrimitiveID )"
	"\n""{"
	"\n""	 HS_CONTROL_POINT_OUT result;"
	"\n""	 result.pos = InPts[cpId].pos;"
	"\n""	 result.vertID = InPts[cpId].vertID;"
	"\n""	 result.patchID = PatchID;"
	"\n""	 result.pos *= float4(10,10,10,10);"
	"\n""	 return result;"
	"\n""}"
	"\n";

const tstring  g_hsPassThruCpFuncClamped_3Sizes_OutInIn =
	"\n""[outputcontrolpoints(%d)]"
	"\n""HS_CONTROL_POINT_OUT hs_main("
	"\n""	InputPatch<VS_OUT, %d> InPts,"
	"\n""	uint cpId : SV_OutputControlPointID,"
	"\n""	uint PatchID : SV_PrimitiveID )"
	"\n""{"
	"\n""	 HS_CONTROL_POINT_OUT result;"
	"\n""	 uint cpIdx = min(cpId, (%d-1));"
	"\n""	 result.pos = InPts[cpIdx].pos;"
	"\n""	 result.vertID = InPts[cpIdx].vertID;"
	"\n""	 result.patchID = PatchID;"
	"\n""	 result.pos *= float4(10,10,10,10);"
	"\n""	 return result;"
	"\n""}"
	"\n";

const tstring  g_hsConstDecl_quadTessFactors =
	"struct HS_CONSTANT_DATA_OUTPUT"
	"{"
	"	float Edges[4]  : SV_TessFactor;"
	"	float Inside[2] : SV_InsideTessFactor;"
	"	uint  PatchID   : patchID;"
	"\n";

const tstring  g_hsConstDecl_triTessFactors =
	"struct HS_CONSTANT_DATA_OUTPUT"
	"{"
	"	float Edges[3]  : SV_TessFactor;"
	"	float Inside[1] : SV_InsideTessFactor;"
	"	uint  PatchID   : patchID;"
	"\n";

const tstring  g_hsConstDecl_lineTessFactors =
	"struct HS_CONSTANT_DATA_OUTPUT"
	"{"
	"	float DensityDetail[2] : SV_TessFactor;"
	//"	float Detail  : SV_TessFactor;"
	"	uint  PatchID : patchID;"
	"\n";

const tstring  g_hsConstDecl_floatVals_1Size =
	"	float4 vals[%d] : fvals;"
	"\n";

const tstring  g_hsConstDecl_intVals_1Size =
	"	int4 vals[%d] : ivals;"
	"\n";

const tstring  g_hsConstDecl_finish =
	"};"
	"\n";
