//
// gs.gsh
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//

struct GSIn
{
	uint	viewportIndex : VPi;
	float4	pos : SV_Position;
	float4	color : color;
};

struct GSOut
{
	uint	viewportIndex : SV_ViewportArrayIndex;
	float4	pos : SV_Position;
	float4	color : color;
};

cbuffer cb0 : register(b0)
{
	float4 inverseViewport;
};

[maxvertexcount (6)]
void main(inout TriangleStream<GSOut> Stream, triangle GSIn input[3])
{
	GSOut output;

	uint viewportShift = 0;
	uint srcVertex = 0;
	float1 yShift = 0.0f;
	for (uint iVertex = 0; iVertex < 6; ++iVertex)
	{
		output.viewportIndex = input[0].viewportIndex + viewportShift;
		output.pos = input[srcVertex].pos;
		output.pos.y += yShift;
		output.color = input[srcVertex].color;
		
		Stream.Append(output);

		if ( ++srcVertex == 3 )
		{
			viewportShift = 1;
			srcVertex = 0;
			yShift = -1.0f;
			Stream.RestartStrip();
		}
	}
}
