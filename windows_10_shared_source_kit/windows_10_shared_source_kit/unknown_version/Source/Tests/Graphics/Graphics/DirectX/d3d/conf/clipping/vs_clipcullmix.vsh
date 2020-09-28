struct VSIn
{
	float2 pos : pos;
	uint vertNum : vertNum;
};

#define CLIPCULL1 SV_ClipDistance0
#define CLIPCULL2 SV_CullDistance0
#define CLIPCULL3 SV_ClipDistance1
#define CLIPCULL4 SV_CullDistance1

struct VSOutClipDist
{
	float4 pos : SV_Position;
	float dist1 : CLIPCULL1; // requires macro input
	float dist2 : CLIPCULL2;
	float dist3 : CLIPCULL3;
	float dist4 : CLIPCULL4;
};

cbuffer cb0 : register(b0)
{
	float4 inverseViewport;
};

cbuffer cb1 : register(b1)
{
	float4 vertDists[8];
};

VSOutClipDist main(VSIn input)
{
	VSOutClipDist output;
	
	output.pos = float4( 0, 0, 0, 1.0 );
	output.pos.xy = input.pos * inverseViewport.zw + inverseViewport.xy;
	output.dist1 = vertDists[input.vertNum*2].x;
	output.dist2 = vertDists[input.vertNum*2].y;
	output.dist3 = vertDists[input.vertNum*2].z;
	output.dist4 = vertDists[input.vertNum*2].w;
	
	return output;
}
