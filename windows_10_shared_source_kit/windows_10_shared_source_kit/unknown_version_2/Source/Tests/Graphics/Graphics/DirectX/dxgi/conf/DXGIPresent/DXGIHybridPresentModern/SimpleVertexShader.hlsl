cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

struct VertexShaderInput
{
	uint vertexId : VertexId;
};

struct VertexShaderOutput
{
	float4 pos : SV_POSITION;
	float2 texCoord : texCoord;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	output.pos = 0;
	output.texCoord = 0;
	float4 pos = 0;
	float2 texCoord = 0;

	switch(input.vertexId)
	{
	case 0:
		pos = float4(-0.5f, -0.5f, -0.5f, 1.0f);
		texCoord = float2(0.0f, 1.0f);
		break;
	case 1:
		pos = float4(-0.5f, -0.5f,  0.5f, 1.0f);
		texCoord = float2(1.0f, 0.0f);
		break;
	case 2:
		pos = float4(-0.5f,  0.5f, -0.5f, 1.0f);
		texCoord = float2(0.0f, 1.0f);
		break;
	case 3:
		pos = float4(-0.5f,  0.5f,  0.5f, 1.0f);
		texCoord = float2(1.0f, 1.0f);
		break;
	case 4:
		pos = float4( 0.5f, -0.5f, -0.5f, 1.0f);
		texCoord = float2(0.0f, 1.0f);
		break;
	case 5:
		pos = float4( 0.5f, -0.5f,  0.5f, 1.0f);
		texCoord = float2(1.0f, 0.0f);
		break;
	case 6:
		pos = float4( 0.5f,  0.5f, -0.5f, 1.0f);
		texCoord = float2(0.0f, 1.0f);
		break;
	case 7:
		pos = float4( 0.5f,  0.5f,  0.5f, 1.0f);
		texCoord = float2(1.0f, 1.0f);
		break;
	}

	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;
	output.texCoord = texCoord;

	return output;
}
