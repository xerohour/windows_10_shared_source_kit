//
// If you change any of these vertex declarations, you must also change gStructDefinitions in FcallLimits.cpp
//
struct passthrough_vertex
{
	float4 pos : POSITION;
};

struct output_vertex
{
	row_major uint4x4 data : RESOURCE_DATA;
};

struct patch_constant_out
{
	float OutsideTessFactor[3] : SV_TessFactor;
	float InsideTessFactor     : SV_InsideTessFactor;
};

passthrough_vertex vs_main1( float3 pos : Position )
{
	passthrough_vertex ret;
	ret.pos = float4(pos,1.0);
	return ret;
}

float4 vs_main2( float3 pos : Position ) : SV_POSITION
{
	return float4(pos,1.0);
}

patch_constant_out hs_patch_constant(InputPatch<passthrough_vertex,3> input_patch, OutputPatch<passthrough_vertex,3> output_patch)
{
	patch_constant_out ret;
	
	ret.OutsideTessFactor[0] = 1;
	ret.OutsideTessFactor[1] = 1;
	ret.OutsideTessFactor[2] = 1;
	
	ret.InsideTessFactor = 1;
	
	return ret;
}

[patchconstantfunc("hs_patch_constant")]
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[maxtessfactor(1.0)]
[outputcontrolpoints(3)]
passthrough_vertex hs_main(InputPatch<passthrough_vertex, 3> input_patch, uint id : SV_OutputControlPointID)
{
	return input_patch[0];
}

[domain("tri")]
output_vertex ds_main(OutputPatch<output_vertex, 3> patch, patch_constant_out constant, float2 uv : SV_DomainLocation)
{
	return patch[0];
}

[maxvertexcount(3)]
void gs_main(triangle output_vertex input[3], inout TriangleStream<output_vertex> output)
{
	{
		output_vertex v0;
		v0.data = input[0].data;
		output.Append(v0);
	}
	
	{
		output_vertex v1;
		v1.data = input[1].data;
		output.Append(v1);
	}
	
	{
		output_vertex v2;
		v2.data = input[2].data;
		output.Append(v2);
	}
}