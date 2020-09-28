// RenderTargets Vertex Shader(s)

struct VSIn
{
	float2 vPos	: pos;
	float4 vTexCoords : texCoord;
};

struct VSOut
{
	float4 vPos : SV_Position;
	float2 vTexCoords : texCoord;
};

VSOut VShaderMain( VSIn input )
{
	VSOut output;
	
	output.vPos = float4( input.vPos, 0.0f, 1.0f );
	output.vTexCoords = input.vTexCoords;
	
	return output;
}