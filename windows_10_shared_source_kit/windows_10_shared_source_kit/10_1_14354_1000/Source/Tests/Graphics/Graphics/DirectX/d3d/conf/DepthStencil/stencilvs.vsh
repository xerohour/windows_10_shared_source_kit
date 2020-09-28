//
// vs.vsa
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//

struct VSIn
{
	float3 pos : pos;
	float3 col : col;
};

struct VSOut
{
	float4 pos : SV_Position;
	float4 col : col;
};

cbuffer cb0 : register(b0)
{
	float4 inverseViewport;
};

VSOut main(VSIn input)
{
	VSOut output;
	
	output.pos = float4( 0, 0, 0, 1.0 );
	output.pos.z = input.pos.z;
	output.pos.xy = input.pos.xy * inverseViewport.zw + inverseViewport.xy;
	
	output.col = float4( 0, 0, 0, 1.0 );
	output.col.xyz = input.col;	
	
	return output;
}
