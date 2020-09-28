

struct VSIn
{
	float2 pos   : pos;
	float4 color : color;
};

struct VSOut
{
	float4 pos   : SV_Position;
	float4 color : color;
};

cbuffer cb0 : register(b0)
{
	float4 inverseViewport;
};

VSOut VSMain(VSIn input)
{
	VSOut output;
	
	output.pos = float4( 0, 0, 0, 1.0 );
	output.pos.xy = input.pos * inverseViewport.zw + inverseViewport.xy;
	
	output.color = input.color;
	
	return output;
}

float4 PSMain(VSOut input) : SV_Target
{
	return input.color;
}
