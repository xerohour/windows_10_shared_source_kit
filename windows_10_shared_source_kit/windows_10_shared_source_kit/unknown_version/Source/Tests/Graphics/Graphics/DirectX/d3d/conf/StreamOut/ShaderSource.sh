// ShaderSource.sh
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//

struct VSIn
{
	float4 pos : pos;
	uint data : data;
};

struct GSIn
{
	float4 pos : pos;
	uint data : data;
};

struct GSOut8 // this writes 8 components, 3 registers, 1 component, 1 register is skipped
{
	uint2 data[2] : data;
	float nostream : nostream;
	float4 pos : SV_Position;
};

struct GSOut32 // this writes 32 components, 15 registers, 1 component/ 1 regiseter is skipped.
{
	float nostream : nostream;
	float4 pos : SV_Position;
	uint2 data[14] : data;
};

struct GSOut64 // this writes 64 components, 31 registers
{
	float4 pos : SV_Position;
	uint2 data[30] : data;
};

struct GSOut128 // this writes 128 components, 32 registers
{
	float4 pos : SV_Position;
	uint4 data[31] : data;
};

cbuffer ConstData : register(b0)
{
	uint4 numPrimsPerInputVertex;
	uint4 drawIndex;
	uint4 numDrawCalls;
	uint4 viewportHeight;
};

////////////////////////////////////////////////////////////

uint Encode( uint i )
{
	return i + 1;
	//return ( i >> 1 ) ^ i;
}

////////////////////////////////////////////////////////////

GSIn VSChangeData( VSIn input )
{
	GSIn output;
	
	output.pos = input.pos;
	output.data = Encode( input.data );
	
	return output;
}

GSOut8 VSNoGS8( VSIn input )
{
	GSOut8 output;

	output.pos = input.pos;
	uint data = Encode( input.data );
	
	// Generate a single point with the same data that GSPointInPointsOut8() 
	// would have generated with numPrimsPerInputVertex set to 1
	output.nostream = 3.14;

	uint n = drawIndex.x + data;

	n = Encode( n );
	output.data[0].x = n;
	n = Encode( n );
	output.data[0].y = n;
	n = Encode( n );
	output.data[1].x = n;
	n = Encode( n );
	output.data[1].y = n;
	
	return output;
}

GSOut32 VSNoGS32( VSIn input )
{
	GSOut32 output;

	output.pos = input.pos;
	uint data = Encode( input.data );
	
	// Generate a single point with the same data that GSPointInPointsOut64() 
	// would have generated with numPrimsPerInputVertex set to 1
	uint n = drawIndex.x + data;

	for( uint k = 0; k < 14; ++k )
	{
		n = Encode( n );
		output.data[k].x = n;
		n = Encode( n );
		output.data[k].y = n;
	}
	
	return output;
}

////////////////////////////////////////////////////////////
// Triangles
////////////////////////////////////////////////////////////

void GSPointInTrianglesOut8( inout TriangleStream<GSOut8> stream, /*point*/ GSIn input[1] )
{
	GSOut8 output;
	
	// Generate a strip of triangles of height = 1, width = numPrimsPerInputVertex.x / 2
	uint count = numPrimsPerInputVertex.x / 2;
	output.pos = input[0].pos;
	for( uint i = 0; i <= count; i++ )
	{
		output.pos.y = input[0].pos.y;
		output.pos.x = input[0].pos.x + (float) i;
		output.nostream = 3.14;

		uint n = drawIndex.x + 2 * i + input[0].data;

		for( uint j = 0; j < 2; ++j )
		{		
			n = Encode( n );
			output.data[0].x = n;
			n = Encode( n );
			output.data[0].y = n;
			n = Encode( n );
			output.data[1].x = n;
			n = Encode( n );
			output.data[1].y = n;
			
			stream.Append( output );
			output.pos.y += 1.0;
		}
	}
}

////////////////////////////////////////////////////////////

void GSPointInTrianglesOut64( inout TriangleStream<GSOut64> stream, /*point*/ GSIn input[1] )
{
	GSOut64 output;
	
	// Generate a strip of triangles of height = 1, width = numPrimsPerInputVertex.x / 2
	uint count = numPrimsPerInputVertex.x / 2;
	output.pos = input[0].pos;
	for( uint i = 0; i <= count; i++ )
	{
		output.pos.y = input[0].pos.y;
		output.pos.x = input[0].pos.x + (float) i;

		uint n = drawIndex.x + 2 * i + input[0].data;

		for( uint j = 0; j < 2; ++j )
		{		
			for( uint k = 0; k < 30; ++k )
			{
				n = Encode( n );
				output.data[k].x = n;
				n = Encode( n );
				output.data[k].y = n;
			}
						
			stream.Append( output );
			output.pos.y += 1.0;
		}
	}
}

////////////////////////////////////////////////////////////

void GSPointInTrianglesOut128( inout TriangleStream<GSOut128> stream, /*point*/ GSIn input[1] )
{
	GSOut128 output;
	
	// Generate a strip of triangles of height = 1, width = numPrimsPerInputVertex.x / 2
	uint count = numPrimsPerInputVertex.x / 2;
	output.pos = input[0].pos;
	for( uint i = 0; i <= count; i++ )
	{
		output.pos.y = input[0].pos.y;
		output.pos.x = input[0].pos.x + (float) i;

		uint n = drawIndex.x + 2 * i + input[0].data;

		for( uint j = 0; j < 2; ++j )
		{		
			for( uint k = 0; k < 31; ++k )
			{
				n = Encode( n );
				output.data[k].x = n;
				n = Encode( n );
				output.data[k].y = n;
				n = Encode( n );
				output.data[k].z = n;
				n = Encode( n );
				output.data[k].w = n;
			}
						
			stream.Append( output );
			output.pos.y += 1.0;
		}
	}
}

////////////////////////////////////////////////////////////

[maxvertexcount (6)] // 4 Triangle strip
void GSPointInTrianglesOut6x8( inout TriangleStream<GSOut8> stream, point GSIn input[1])
{
	GSPointInTrianglesOut8( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (8)] // 6 Triangle strip
void GSPointInTrianglesOut8x8( inout TriangleStream<GSOut8> stream, point GSIn input[1])
{
	GSPointInTrianglesOut8( stream, input );
}


////////////////////////////////////////////////////////////

[maxvertexcount (6)] // 4 Triangle strip
void GSPointInTrianglesOut6x64( inout TriangleStream<GSOut64> stream, point GSIn input[1])
{
	GSPointInTrianglesOut64( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (8)] // 6 Triangle strip
void GSPointInTrianglesOut8x64( inout TriangleStream<GSOut64> stream, point GSIn input[1])
{
	GSPointInTrianglesOut64( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (6)] // 4 Triangle strip
void GSPointInTrianglesOut6x128( inout TriangleStream<GSOut128> stream, point GSIn input[1])
{
	GSPointInTrianglesOut128( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (8)] // 6 Triangle strip
void GSPointInTrianglesOut8x128( inout TriangleStream<GSOut128> stream, point GSIn input[1])
{
	GSPointInTrianglesOut128( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (12)] // Emulate line list
void GSPointInLinePointsOut( inout PointStream<GSOut8> stream, point GSIn input[1] )
{
	GSOut8 output;
	
	// Generate a list of triangles (from raw points) of height = 1, width = numPrimsPerInputVertex.x / 2
	uint count = numPrimsPerInputVertex.x / 2;
	for( uint i = 0; i <= count; i++ )
	{
		uint data = drawIndex.x + i + input[0].data;
		uint data0 = Encode( data++ );
		uint data1 = Encode( data++ );
		uint data2 = Encode( data++ );
		uint data3 = Encode( data++ );
		
		float4 pos0 = input[0].pos;
		pos0.x += (float) i;
		float4 pos1 = pos0;
		pos1.y += 1;
		float4 pos2 = pos0;
		pos2.x += 1;
		float4 pos3 = pos2;
		pos2.y += 1;
		
		// First Triangle
		output.pos = pos0;
		output.data[0] = data0;
		stream.Append( output );
		output.pos = pos1;
		output.data[0] = data1;
		stream.Append( output );
		output.pos = pos2;
		output.data[0] = data2;
		stream.Append( output );
		
		// Second Triangle
		output.pos = pos1;
		output.data[0] = data1;
		stream.Append( output );
		output.pos = pos3;
		output.data[0] = data3;
		stream.Append( output );
		output.pos = pos2;
		output.data[0] = data2;
		stream.Append( output );
	}
} 



////////////////////////////////////////////////////////////
// Lines
////////////////////////////////////////////////////////////

void GSPointInLinesOut8( inout LineStream<GSOut8> stream, /*point*/ GSIn input[1] )
{
	GSOut8 output;
	
	// Generate a strip of Lines of length = numPrimsPerInputVertex.x
	uint count = numPrimsPerInputVertex.x;
	output.pos = input[0].pos;
	for( uint i = 0; i <= count; i++ )
	{
		output.nostream = 3.14;

		uint n = drawIndex.x + 2 * i + input[0].data;

		n = Encode( n );
		output.data[0].x = n;
		n = Encode( n );
		output.data[0].y = n;
		n = Encode( n );
		output.data[1].x = n;
		n = Encode( n );
		output.data[1].y = n;
		
		stream.Append( output );
		output.pos.y += 1.0;
	}
}

////////////////////////////////////////////////////////////

void GSPointInLinesOut64( inout LineStream<GSOut64> stream, /*point*/ GSIn input[1] )
{
	GSOut64 output;
	
	// Generate a strip of Lines of length = numPrimsPerInputVertex.x
	uint count = numPrimsPerInputVertex.x;
	output.pos = input[0].pos;
	for( uint i = 0; i <= count; i++ )
	{
		uint n = drawIndex.x + 2 * i + input[0].data;

		for( uint k = 0; k < 30; ++k )
		{
			n = Encode( n );
			output.data[k].x = n;
			n = Encode( n );
			output.data[k].y = n;
		}
					
		stream.Append( output );
		output.pos.y += 1.0;
	}
}

////////////////////////////////////////////////////////////

void GSPointInLinesOut128( inout LineStream<GSOut128> stream, /*point*/ GSIn input[1] )
{
	GSOut128 output;
	
	// Generate a strip of Lines of length = numPrimsPerInputVertex.x
	uint count = numPrimsPerInputVertex.x;
	output.pos = input[0].pos;
	for( uint i = 0; i <= count; i++ )
	{
		uint n = drawIndex.x + 2 * i + input[0].data;

		for( uint k = 0; k < 31; ++k )
		{
			n = Encode( n );
			output.data[k].x = n;
			n = Encode( n );
			output.data[k].y = n;
			n = Encode( n );
			output.data[k].z = n;
			n = Encode( n );
			output.data[k].w = n;
		}
					
		stream.Append( output );
		output.pos.y += 1.0;
	}
}

////////////////////////////////////////////////////////////

[maxvertexcount (6)] // 6 point list
void GSPointInLinesOut6x8( inout LineStream<GSOut8> stream, point GSIn input[1])
{
	GSPointInLinesOut8( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (8)] // 8 point list
void GSPointInLinesOut8x8( inout LineStream<GSOut8> stream, point GSIn input[1])
{
	GSPointInLinesOut8( stream, input );
}


////////////////////////////////////////////////////////////

[maxvertexcount (6)] // 6 point list
void GSPointInLinesOut6x64( inout LineStream<GSOut64> stream, point GSIn input[1])
{
	GSPointInLinesOut64( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (8)] // 8 point list
void GSPointInLinesOut8x64( inout LineStream<GSOut64> stream, point GSIn input[1])
{
	GSPointInLinesOut64( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (6)] // 6 point list
void GSPointInLinesOut6x128( inout LineStream<GSOut128> stream, point GSIn input[1])
{
	GSPointInLinesOut128( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (8)] // 8 point list
void GSPointInLinesOut8x128( inout LineStream<GSOut128> stream, point GSIn input[1])
{
	GSPointInLinesOut128( stream, input );
}

////////////////////////////////////////////////////////////
// Points
////////////////////////////////////////////////////////////

void GSPointInPointsOut8( inout PointStream<GSOut8> stream, /*point*/ GSIn input[1] )
{
	GSOut8 output;
	
	// Generate a list of Points of length = numPrimsPerInputVertex.x
	uint count = numPrimsPerInputVertex.x;
	output.pos = input[0].pos;
	for( uint i = 0; i < count; i++ )
	{
		output.nostream = 3.14;

		uint n = drawIndex.x + 2 * i + input[0].data;

		n = Encode( n );
		output.data[0].x = n;
		n = Encode( n );
		output.data[0].y = n;
		n = Encode( n );
		output.data[1].x = n;
		n = Encode( n );
		output.data[1].y = n;
		
		stream.Append( output );
		output.pos.y += 1.0;
	}
}

////////////////////////////////////////////////////////////

void GSPointInPointsOut64( inout PointStream<GSOut64> stream, /*point*/ GSIn input[1] )
{
	GSOut64 output;
	
	// Generate a list of Points of length = numPrimsPerInputVertex.x
	uint count = numPrimsPerInputVertex.x;
	output.pos = input[0].pos;
	for( uint i = 0; i < count; i++ )
	{
		uint n = drawIndex.x + 2 * i + input[0].data;

		for( uint k = 0; k < 30; ++k )
		{
			n = Encode( n );
			output.data[k].x = n;
			n = Encode( n );
			output.data[k].y = n;
		}
					
		stream.Append( output );
		output.pos.y += 1.0;
	}
}

////////////////////////////////////////////////////////////

void GSPointInPointsOut128( inout PointStream<GSOut128> stream, /*point*/ GSIn input[1] )
{
	GSOut128 output;
	
	// Generate a list of Points of length = numPrimsPerInputVertex.x
	uint count = numPrimsPerInputVertex.x;
	output.pos = input[0].pos;
	for( uint i = 0; i < count; i++ )
	{
		uint n = drawIndex.x + 2 * i + input[0].data;

		for( uint k = 0; k < 31; ++k )
		{
			n = Encode( n );
			output.data[k].x = n;
			n = Encode( n );
			output.data[k].y = n;
			n = Encode( n );
			output.data[k].z = n;
			n = Encode( n );
			output.data[k].w = n;
		}
					
		stream.Append( output );
		output.pos.y += 1.0;
	}
}

////////////////////////////////////////////////////////////

[maxvertexcount (6)] // 5 Line list
void GSPointInPointsOut6x8( inout PointStream<GSOut8> stream, point GSIn input[1])
{
	GSPointInPointsOut8( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (8)] // 7 Line list
void GSPointInPointsOut8x8( inout PointStream<GSOut8> stream, point GSIn input[1])
{
	GSPointInPointsOut8( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (6)] // 5 Line list
void GSPointInPointsOut6x64( inout PointStream<GSOut64> stream, point GSIn input[1])
{
	GSPointInPointsOut64( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (8)] // 7 Line list
void GSPointInPointsOut8x64( inout PointStream<GSOut64> stream, point GSIn input[1])
{
	GSPointInPointsOut64( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (6)] // 5 Line list
void GSPointInPointsOut6x128( inout PointStream<GSOut128> stream, point GSIn input[1])
{
	GSPointInPointsOut128( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (8)] // 7 Line list
void GSPointInPointsOut8x128( inout PointStream<GSOut128> stream, point GSIn input[1])
{
	GSPointInPointsOut128( stream, input );
}

////////////////////////////////////////////////////////////

[maxvertexcount (12)] // Emulate 4-triangle list
void GSPointInTrianglePointsOut( inout PointStream<GSOut8> stream, point GSIn input[1] )
{
	GSOut8 output;
	
	// Generate a list of Lines (from raw points) of height = 1, width = numPrimsPerInputVertex.x
	uint count = numPrimsPerInputVertex.x;
	for( uint i = 0; i <= count; i++ )
	{
		uint data = drawIndex.x + i + input[0].data;
		uint data0 = Encode( data++ );
		uint data1 = Encode( data++ );
		uint data2 = Encode( data++ );
		uint data3 = Encode( data++ );
		
		float4 pos0 = input[0].pos;
		pos0.x += (float) i;
		float4 pos1 = pos0;
		pos1.y += 1;
		float4 pos2 = pos0;
		pos2.x += 1;
		float4 pos3 = pos2;
		pos2.y += 1;
		
		// First Triangle
		output.pos = pos0;
		output.data[0] = data0;
		stream.Append( output );
		output.pos = pos1;
		output.data[0] = data1;
		stream.Append( output );
		output.pos = pos2;
		output.data[0] = data2;
		stream.Append( output );
		
		// Second Triangle
		output.pos = pos1;
		output.data[0] = data1;
		stream.Append( output );
		output.pos = pos3;
		output.data[0] = data3;
		stream.Append( output );
		output.pos = pos2;
		output.data[0] = data2;
		stream.Append( output );
	}
} 

/////////////////////////////////////
// Shaders used by MultiStream.cpp
/////////////////////////////////////
void EncodeData128(inout GSOut128 vertex, inout uint n)
{   
    for( uint i = 0; i < 31; i++)
    {
        n = Encode( n );
        vertex.data[i].x = n;
        
        n = Encode( n );
        vertex.data[i].y = n;
        
        n = Encode( n );
        vertex.data[i].z = n;
        
        n = Encode( n );
        vertex.data[i].w = n;
    }
}

void IncrementData128(inout GSOut128 vertex)
{    
    for( uint i = 0; i < 31; i++ )
    {
        vertex.data[i].x += 1;
        vertex.data[i].y += 2;
        vertex.data[i].z += 3;
        vertex.data[i].w += 4;
    }
}

void GSPointInPointsOutOneStreamFourBuffer( inout PointStream<GSOut128> stream0,
										    GSIn input[1], uint primID )
{	
	GSOut128 output;
	uint count = numPrimsPerInputVertex.x;
	float pixelStride = 2.0 / viewportHeight.x;
	float pixelOffset = 1.0 / viewportHeight.x;
	float drawStride = 2.0 / numDrawCalls.x;
	output.pos = input[0].pos;

	for( uint i = 0; i < count; i++ )
	{
		uint blockOffset = primID * count + i;
		float finalPixelY = (blockOffset * pixelStride) + pixelOffset - 1;
		uint n = drawIndex.x + 2 * i + input[0].data;

		// Note: -1.0 to account for the reversal of y-axis during rasterization
		// Also need to accound for what draw index we are at, since all draw calls go to the same rendertarget
		output.pos.y = -1.0 * (finalPixelY + (drawIndex.x * drawStride));

        EncodeData128(output, n);
		
		if(1 != (n % 5))
  		    stream0.Append( output );
	}
}

[maxvertexcount (8)] 
void GSPointInPointsOut8xOneStreamFourBuffer( inout PointStream<GSOut128> stream0, 
					       point GSIn input[1], uint primID : SV_PrimitiveID )
{
    GSPointInPointsOutOneStreamFourBuffer( stream0, input, primID );
}

[maxvertexcount (6)] 
void GSPointInPointsOut6xOneStreamFourBuffer( inout PointStream<GSOut128> stream0, 
					      point GSIn input[1], uint primID : SV_PrimitiveID )
{
    GSPointInPointsOutOneStreamFourBuffer( stream0, input, primID );
}

void GSPointInPointsOutTwoStreamTwoBuffer( inout PointStream<GSOut128> stream0, 
					    inout PointStream<GSOut128> stream1,
					    GSIn input[1], uint primID )
{	
	GSOut128 output;
	uint count = numPrimsPerInputVertex.x;
	float pixelStride = 2.0 / viewportHeight.x;
	float pixelOffset = 1.0 / viewportHeight.x;
	float drawStride = 2.0 / numDrawCalls.x;
	output.pos = input[0].pos;

	for( uint i = 0; i < count; i++ )
	{
		uint blockOffset = primID * count + i;
		float finalPixelY = (blockOffset * pixelStride) + pixelOffset - 1;
		uint n = drawIndex.x + 2 * i + input[0].data;

		// Note: -1.0 to account for the reversal of y-axis during rasterization
		// Also need to accound for what draw index we are at, since all draw calls go to the same rendertarget
		output.pos.y = -1.0 * (finalPixelY + (drawIndex.x * drawStride));

		EncodeData128(output, n);
		
		if(1 != (n % 5))
		    stream0.Append( output );

        IncrementData128(output);

		if(1 != (n % 5))
		    stream1.Append( output );
	}
}

[maxvertexcount (8)] 
void GSPointInPointsOut8xTwoStreamTwoBuffer( inout PointStream<GSOut128> stream0, 
					      inout PointStream<GSOut128> stream1, 
					      point GSIn input[1], uint primID : SV_PrimitiveID )
{
    GSPointInPointsOutTwoStreamTwoBuffer( stream0, stream1, input, primID );
}

[maxvertexcount (6)] 
void GSPointInPointsOut6xTwoStreamTwoBuffer( inout PointStream<GSOut128> stream0, 
					      inout PointStream<GSOut128> stream1, 
					      point GSIn input[1], uint primID : SV_PrimitiveID )
{
    GSPointInPointsOutTwoStreamTwoBuffer( stream0, stream1, input, primID );
}

void GSPointInPointsOutThreeStreamFourBuffer( inout PointStream<GSOut128> stream0, 
					    inout PointStream<GSOut128> stream1,
					    inout PointStream<GSOut128> stream2,
					    GSIn input[1], uint primID )
{	
	GSOut128 output;
	uint count = numPrimsPerInputVertex.x;
	float pixelStride = 2.0 / viewportHeight.x;
	float pixelOffset = 1.0 / viewportHeight.x;
	float drawStride = 2.0 / numDrawCalls.x;
	output.pos = input[0].pos;

	for( uint i = 0; i < count; i++ )
	{
		uint blockOffset = primID * count + i;
		float finalPixelY = (blockOffset * pixelStride) + pixelOffset - 1;
		uint n = drawIndex.x + 2 * i + input[0].data;

		// Note: -1.0 to account for the reversal of y-axis during rasterization
		// Also need to accound for what draw index we are at, since all draw calls go to the same rendertarget
		output.pos.y = -1.0 * (finalPixelY + (drawIndex.x * drawStride));

		EncodeData128(output, n);
		
		if(1 != (n % 5))
		    stream0.Append( output );

		IncrementData128(output);

		if(1 != (n % 5))
		    stream1.Append( output );

		IncrementData128(output);

		if(1 != (n % 5))
		    stream2.Append( output );
	}
}

[maxvertexcount (8)] 
void GSPointInPointsOut8xThreeStreamFourBuffer( inout PointStream<GSOut128> stream0, 
					      inout PointStream<GSOut128> stream1, 
					      inout PointStream<GSOut128> stream2,
					      point GSIn input[1], uint primID : SV_PrimitiveID )
{
    GSPointInPointsOutThreeStreamFourBuffer( stream0, stream1, stream2, input, primID );
}

[maxvertexcount (6)] 
void GSPointInPointsOut6xThreeStreamFourBuffer( inout PointStream<GSOut128> stream0, 
					      inout PointStream<GSOut128> stream1, 
					      inout PointStream<GSOut128> stream2,
					      point GSIn input[1], uint primID : SV_PrimitiveID )
{
    GSPointInPointsOutThreeStreamFourBuffer( stream0, stream1, stream2, input, primID );
}

void GSPointInPointsOutFourStreamFourBuffer( inout PointStream<GSOut128> stream0, 
					    inout PointStream<GSOut128> stream1,
					    inout PointStream<GSOut128> stream2,
					    inout PointStream<GSOut128> stream3,
					    GSIn input[1], uint primID )
{	
	GSOut128 output;
	uint count = numPrimsPerInputVertex.x;
	float pixelStride = 2.0 / viewportHeight.x;
	float pixelOffset = 1.0 / viewportHeight.x;
	float drawStride = 2.0 / numDrawCalls.x;
	output.pos = input[0].pos;

	for( uint i = 0; i < count; i++ )
	{
		uint blockOffset = primID * count + i;
		float finalPixelY = (blockOffset * pixelStride) + pixelOffset - 1;
		uint n = drawIndex.x + 2 * i + input[0].data;

		// Note: -1.0 to account for the reversal of y-axis during rasterization
		// Also need to accound for what draw index we are at, since all draw calls go to the same rendertarget
		output.pos.y = -1.0 * (finalPixelY + (drawIndex.x * drawStride));

		EncodeData128(output, n);
		
		if(1 != (n % 5))
		    stream0.Append( output );

		IncrementData128(output);

		if(1 != (n % 5))
		    stream1.Append( output );

		IncrementData128(output);

		if(1 != (n % 5))
		    stream2.Append( output );

		IncrementData128(output);

		if(1 != (n % 5))
		    stream3.Append( output );
	}
}

[maxvertexcount (8)] 
void GSPointInPointsOut8xFourStreamFourBuffer( inout PointStream<GSOut128> stream0, 
					      inout PointStream<GSOut128> stream1, 
					      inout PointStream<GSOut128> stream2,
			  		      inout PointStream<GSOut128> stream3,
					      point GSIn input[1], uint primID : SV_PrimitiveID )
{
    GSPointInPointsOutFourStreamFourBuffer( stream0, stream1, stream2, stream3, input, primID );
}

[maxvertexcount (6)] 
void GSPointInPointsOut6xFourStreamFourBuffer( inout PointStream<GSOut128> stream0, 
					      inout PointStream<GSOut128> stream1, 
					      inout PointStream<GSOut128> stream2,
			  		      inout PointStream<GSOut128> stream3,
					      point GSIn input[1], uint primID : SV_PrimitiveID )
{
    GSPointInPointsOutFourStreamFourBuffer( stream0, stream1, stream2, stream3, input, primID );
}

////////////////////////

struct PSOUT
{
	uint4 color : SV_TARGET0;
};

PSOUT PSMain( GSOut128 input )
{
	PSOUT output;

	output.color.xyzw = input.data[0].xyzw;

	return output;
}
