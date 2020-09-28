//
// gs.gsh
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//

struct GSIn
{
	float4	pos : SV_Position;
	float4	color : color;
};

struct GSOut
{
	float4	pos : SV_Position;
	float4	color : color;
};

[maxvertexcount (3)]
void main(inout TriangleStream<GSOut> Stream, triangle GSIn input[3])
{
	GSOut output;
	
	for (uint iVertex = 0; iVertex < 3; ++iVertex)
	{
		output.pos = input[iVertex].pos;
		output.color = input[iVertex].color;
		
		Stream.Append(output);
	}
}