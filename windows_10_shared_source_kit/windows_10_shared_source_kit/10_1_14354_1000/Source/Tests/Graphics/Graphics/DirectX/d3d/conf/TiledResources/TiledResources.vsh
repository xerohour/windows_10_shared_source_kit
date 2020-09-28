// Vertex shader source for WGF11TiledResources

struct VS_OUTPUT
{
	float4 position		: SV_POSITION;
};

static const float2 vertexPositions[4] = 
{
	float2(-1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, 1.0),
	float2(1.0, -1.0)
};

VS_OUTPUT CopyTexVS(uint vertex : SV_VertexID)
{
	VS_OUTPUT output;
	output.position.xy = vertexPositions[vertex];
	output.position.zw = float2(0.0, 1.0);

	return output;
}
