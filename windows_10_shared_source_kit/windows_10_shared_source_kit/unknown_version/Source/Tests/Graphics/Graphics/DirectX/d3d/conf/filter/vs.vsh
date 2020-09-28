struct VSIn
{
	float2 pos : pos;
};

struct VSOut
{
	float4 pos : SV_Position;
	float2 pos2: pixelPos;
	float4 tex : tex0;
};

cbuffer cb0 : register(b0)
{
	float4 inverseViewport;
};

cbuffer cb1 : register(b1)
{
	float4 texCoords[4];
};

VSOut main(VSIn input, uint vertNum : SV_VertexID)
{
	VSOut output;
	
	output.pos = float4( 0, 0, 0, 1.0 );
	output.pos.xy = input.pos * inverseViewport.zw + inverseViewport.xy;

	output.pos2 = input.pos;
	
	if (vertNum > 3)
		vertNum = 0;
	output.tex.xyz = texCoords[vertNum].xyz;
	output.tex.w = 1.0;
	
	return output;
}
