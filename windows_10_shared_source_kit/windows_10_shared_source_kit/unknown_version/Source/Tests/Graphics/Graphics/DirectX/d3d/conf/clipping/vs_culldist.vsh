struct VSIn
{
	float2 pos : pos;
	uint vertNum : vertNum;
};

struct VSOutCullDist
{
	float4 pos : SV_Position;
	float4 dists[2] : SV_CullDistance;
};

cbuffer cb0 : register(b0)
{
	float4 inverseViewport;
};

cbuffer cb1 : register(b1)
{
	float4 vertDists[8];
};

VSOutCullDist main(VSIn input)
{
	VSOutCullDist output;
	
	output.pos = float4( 0, 0, 0, 1.0 );
	output.pos.xy = input.pos * inverseViewport.zw + inverseViewport.xy;
	output.dists[0] = vertDists[input.vertNum*2];
	output.dists[1] = vertDists[input.vertNum*2+1];
	
	return output;
}
