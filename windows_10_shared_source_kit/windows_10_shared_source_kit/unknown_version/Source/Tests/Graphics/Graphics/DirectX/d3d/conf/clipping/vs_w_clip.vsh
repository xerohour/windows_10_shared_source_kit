struct VSIn
{
	float2 pos : pos;
	uint vertNum : vertNum;
};

struct VSOut
{
	float4 pos : SV_Position;
};

cbuffer cb0 : register(b0)
{
	float4 inverseViewport;
};

cbuffer cb1 : register(b1)
{
	float4 vertDists[8];
};

VSOut main(VSIn input)
{
	VSOut output;
	
	output.pos = float4( 0, 0, 0, 1.0 );
	output.pos.xy = input.pos * inverseViewport.zw + inverseViewport.xy;
	output.pos.w = vertDists[input.vertNum*2].x;
	
	return output;
}
