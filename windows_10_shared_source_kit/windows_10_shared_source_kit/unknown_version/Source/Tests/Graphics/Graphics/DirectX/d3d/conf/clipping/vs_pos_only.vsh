struct VSIn
{
	float2 pos : pos;
	uint vertNum : vertNum;
};

struct VSOutPosOnly
{
	float4 pos : SV_Position;
};

cbuffer cb0 : register(b0)
{
	float4 inverseViewport;
};

VSOutPosOnly main(VSIn input)
{
	VSOutPosOnly output;
	
	output.pos = float4( 0, 0, 0, 1.0 );
	output.pos.xy = input.pos * inverseViewport.zw + inverseViewport.xy;
	
	return output;
}
