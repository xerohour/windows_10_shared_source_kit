//
// vs.vsa
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//

struct VSIn
{
	float2 pos : pos;
	float2 tex : tex;
};

struct VSOut
{
	float4 pos : SV_Position;
	float2 tex : tex;
};

cbuffer cb0 : register(b0)
{
	float4 inverseViewport;
	float alpha;
	float depth;
};

VSOut main(VSIn input)
{
	VSOut output;
	
	output.pos = float4( 0, 0, depth, 1.0 );
	output.pos.xy = input.pos * inverseViewport.zw + inverseViewport.xy;
	output.tex = input.tex;
	
	return output;
}
