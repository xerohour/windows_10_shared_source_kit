struct VERTEX
{
	uint data  : RESOURCE_DATA;
};

struct RAST_VERTEX
{
	float4 pos : SV_POSITION;
	uint data  : RESOURCE_DATA;
};

VERTEX VS_TEST(uint vid : SV_VERTEXID)
{
	VERTEX output;
	output.data = TestEntryPoint();
	return output;
}

VERTEX VS_PASSTHROUGH(uint vid : SV_VERTEXID)
{
	VERTEX output;
	output.data = vid;
	return output;
}

struct PATCH_CONSTANT_OUT
{
	float OutsideTessFactor[3]  : SV_TessFactor;
	float InsideTessFactor      : SV_InsideTessFactor;
};

PATCH_CONSTANT_OUT HS_PATCH_CONSTANT(InputPatch<VERTEX,3> input_patch, OutputPatch<VERTEX,3> output_patch)
{
	PATCH_CONSTANT_OUT ret;
	
	ret.OutsideTessFactor[0] = 1;
	ret.OutsideTessFactor[1] = 1;
	ret.OutsideTessFactor[2] = 1;
	
	ret.InsideTessFactor = 1;
	
	return ret;
}

[patchconstantfunc("HS_PATCH_CONSTANT")]
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[maxtessfactor(1.0)]
[outputcontrolpoints(3)]
VERTEX HS_TEST(InputPatch<VERTEX, 3> input_patch, uint id : SV_OutputControlPointID) : RESOURCE_DATA
{
	VERTEX output;
	output.data = TestEntryPoint();
	return output;
}

[patchconstantfunc("HS_PATCH_CONSTANT")]
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[maxtessfactor(1.0)]
[outputcontrolpoints(3)]
VERTEX HS_PASSTHROUGH(InputPatch<VERTEX, 3> input_patch, uint id : SV_OutputControlPointID) : RESOURCE_DATA
{
	VERTEX output;
	output.data = input_patch[0].data;
	return output;
}

[domain("tri")]
VERTEX DS_TEST(OutputPatch<VERTEX, 3> patch, PATCH_CONSTANT_OUT constant, float2 uv : SV_DomainLocation)
{
	VERTEX output;
	output.data = TestEntryPoint();
	return output;
}

[domain("tri")]
VERTEX DS_PASSTHROUGH(OutputPatch<VERTEX, 3> patch, PATCH_CONSTANT_OUT constant, float2 uv : SV_DomainLocation)
{
	VERTEX output;
	output.data = patch[0].data;
	return output;
}

[maxvertexcount(3)]
void GS_TEST(triangle VERTEX input[3], inout TriangleStream<VERTEX> output)
{
	for(uint i = 0; i < 3; i++)
	{
		VERTEX result = input[i];
		result.data = TestEntryPoint();
		output.Append(result);  
	}
}

[maxvertexcount(3)]
void GS_PASSTHROUGH(triangle VERTEX input[3], inout TriangleStream<RAST_VERTEX> output)
{
	{
		RAST_VERTEX v0;
		v0.pos = float4(-1, 1, 0, 1);
		v0.data = input[0].data;
		output.Append(v0);
	}
	
	{
		RAST_VERTEX v1;
		v1.pos = float4(1, 1, 0, 1);
		v1.data = input[1].data;
		output.Append(v1);
	}
	
	{
		RAST_VERTEX v2;
		v2.pos = float4(0, -1, 0, 1);
		v2.data = input[2].data;
		output.Append(v2);
	}
}

RWByteAddressBuffer RBuff : register(u1);

float4 PS_TEST(RAST_VERTEX input) : SV_TARGET
{
	RBuff.Store(0, TestEntryPoint());
	return float4(0,0,0,1);
};

[numthreads(1,1,1)]
void CS_TEST(uint id : SV_GroupIndex)
{
	RBuff.Store(0, TestEntryPoint());
}