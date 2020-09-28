//
// vs.vsh
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//

struct VSIn
{
	uint	viewportIndex : VPi;
	float3	pos : pos;
	float4	color : color;
};

struct VSOut
{
	uint	viewportIndex : VPi;
	float4	pos : SV_Position;
	float4	color : color;
};

cbuffer cb0 : register(b0)
{
	float4 inverseViewport;
};

VSOut main(VSIn input)
{
	VSOut output;
	
	output.pos.w = 1.0;
	output.pos.xy = input.pos * inverseViewport.zw + inverseViewport.xy;
	output.pos.z = input.pos.z;
	output.color = input.color;
	output.viewportIndex = input.viewportIndex;
	
	return output;
}
