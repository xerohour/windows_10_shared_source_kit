// ShaderSourceMulti.sh
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//

struct GSIn
{
	float4 pos : pos;
	uint data : data;
};

struct GSOut
{
	float4 pos : pos;
	uint4 data_a : data_a;
	uint4 data_b : data_b;
	uint nostream : nostream; // Throw this is for fun
	uint4 data_c : data_c;
	uint4 data_d : data_d;
};

cbuffer ConstData
{
	uint4 innerLoopCount;
	uint4 drawIndex;
};

uint Encode( uint i )
{
	return i + 1;
}

void GSTrianglesOut( inout TriangleStream<GSOut> stream, GSIn input[32], uint data_count )
{
	GSOut output;
	
	output.pos = input[0].pos;
	
	for( uint i = 0; i < innerLoopCount.x; i++ )
	{
		// select a "random" input vertex
		uint data_idx = (i * 17) % data_count;
		
		output.pos.x = input[data_idx].pos.x + (float) (i);
		output.pos.y = input[data_idx].pos.y + (float) (i & 1);
		output.nostream = 3.14;

		uint n = drawIndex.x + i + input[data_idx].data;

		output.data_a.x = n;
		n = Encode( n );
		output.data_a.y = n;
		n = Encode( n );
		output.data_a.z = n;
		n = Encode( n );
		output.data_a.w = n;
			
		n = Encode( n );
		output.data_b.x = n;
		n = Encode( n );
		output.data_b.y = n;
		n = Encode( n );
		output.data_b.z = n;
		n = Encode( n );
		output.data_b.w = n;
			
		n = Encode( n );
		output.data_c.x = n;
		n = Encode( n );
		output.data_c.y = n;
		n = Encode( n );
		output.data_c.z = n;
		n = Encode( n );
		output.data_c.w = n;
			
		n = Encode( n );
		output.data_d.x = n;
		n = Encode( n );
		output.data_d.y = n;
		n = Encode( n );
		output.data_d.z = n;
		n = Encode( n );
		output.data_d.w = n;
		
		stream.Append( output );
	}
}

[maxvertexcount (GS_MAX_VERTEX_COUNT)]
void GSEntryPoint( inout TriangleStream<GSOut> stream, INPUT_DECL )
{
	GSIn data[32];
	
	int i;
	
	[unroll]
	for(i = 0; i < 32; i++)
	{
		data[i] = (GSIn)0;
	}
	
	[unroll]
	for(i = 0; i < INPUT_VERTS_PER_PRIM; i++)
	{
		data[i] = input[i];
	}
	
	GSTrianglesOut( stream, data, INPUT_VERTS_PER_PRIM );
}