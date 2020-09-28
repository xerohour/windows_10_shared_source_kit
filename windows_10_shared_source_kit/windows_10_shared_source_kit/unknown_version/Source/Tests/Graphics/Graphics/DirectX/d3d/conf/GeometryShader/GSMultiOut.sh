// GSMultiOut.sh
// Copyright (c) 2006 Microsoft Corporation. All rights reserved.
//

////////////////////////////////////////////////////////////

cbuffer ConstData
{
	float4 vpSize;
};

////////////////////////////////////////////////////////////

struct VSIn
{
	uint numOut : numOut;
	uint restartFreq : restartFreq;
	uint beginRestarts : beginRestarts;
	uint endRestarts : endRestarts;
	uint instanceID : SV_InstanceID;
};

////////////////////////////////////////////////////////////

struct VSOut
{
	uint numOut : numOut;
	uint restartFreq : restartFreq;
	uint beginRestarts : beginRestarts;
	uint endRestarts : endRestarts;
	uint instanceID : instanceID;
};

////////////////////////////////////////////////////////////

struct GSIn
{
	uint numOut : numOut;
	uint restartFreq : restartFreq;
	uint beginRestarts : beginRestarts;
	uint endRestarts : endRestarts;
	uint instanceID : instanceID;
};

////////////////////////////////////////////////////////////

struct GSOut
{
	float4 position : SV_Position;
	uint primID : primID;
};

struct GSOutTiny
{
	float4 position : SV_Position;
};

struct GSOutHuge
{
	float4 position : SV_Position;
	float4 colors[23] : color0;
	float4 tangent : tangent;
	float4 normal : normal;
	float4 bitangent : bitangent;
	uint primID : primID;
};

struct GSPosition
{
	float4 pos;
	int cut;
};

////////////////////////////////////////////////////////////

struct PSIn
{
	float4 position : SV_Position;
};

////////////////////////////////////////////////////////////

struct PSOut
{
	float4 color : SV_Target;
};

////////////////////////////////////////////////////////////

VSOut VSPassThrough( VSIn input )
{
	VSOut output;
	
	output.numOut = input.numOut;
	output.restartFreq = input.restartFreq;
	output.beginRestarts = input.beginRestarts;
	output.endRestarts = input.endRestarts;
	output.instanceID = input.instanceID;

	return output;
}

////////////////////////////////////////////////////////////

float4 pointPosition(uint index)
{
	float4 pos;

	uint col = index % (uint)vpSize.x;
	uint row = index / (uint)vpSize.x;

	// All vertices are in row 0 and are just a bit to the right of pixel center	
	pos.x = ( ( 2.0 * ( (float)col + 0.6 ) ) / vpSize.x ) - 1.0;
	pos.y = 1.0 - ( ( 2.0 * ( (float)row + 0.6 ) ) / vpSize.y );
	pos.zw = float2( 0.0, 1.0 );
	
	return pos;
}

[maxvertexcount (128)]
void GSPoint( inout PointStream<GSOut> stream, point GSIn input[1], uint primID : SV_PrimitiveID )
{
	uint outNum = 0;
	for (outNum = 0; outNum < input[0].numOut; ++outNum)
	{
		if (   outNum > input[0].beginRestarts
			&& outNum < input[0].endRestarts
			&& input[0].restartFreq > 0
			&& (0 == (outNum % input[0].restartFreq)))
		{
			stream.RestartStrip();
		}
		
		GSOut output;
		
		uint index = outNum + vpSize.x * primID;
		output.position = pointPosition(index);
		
		output.primID = primID;
		
		stream.Append( output );
	}
}


[maxvertexcount (256)]
void GSPointTiny( inout PointStream<GSOutTiny> stream, point GSIn input[1], uint primID : SV_PrimitiveID )
{
	uint outNum = 0;
	for (outNum = 0; outNum < input[0].numOut; ++outNum)
	{
		if (   outNum > input[0].beginRestarts
			&& outNum < input[0].endRestarts
			&& input[0].restartFreq > 0
			&& (0 == (outNum % input[0].restartFreq)))
		{
			stream.RestartStrip();
		}
		
		GSOutTiny output;
		
		uint index = outNum + vpSize.x * primID;
		output.position = pointPosition(index);
		
		stream.Append( output );
	}
}


[maxvertexcount (9)]
void GSPointHuge( inout PointStream<GSOutHuge> stream, point GSIn input[1], uint primID : SV_PrimitiveID )
{
	uint outNum = 0;
	for (outNum = 0; outNum < input[0].numOut; ++outNum)
	{
		if (   outNum > input[0].beginRestarts
			&& outNum < input[0].endRestarts
			&& input[0].restartFreq > 0
			&& (0 == (outNum % input[0].restartFreq)))
		{
			stream.RestartStrip();
		}
		
		GSOutHuge output;
		uint index = outNum + vpSize.x * primID;
		output.position = pointPosition(index);
		
		output.primID = primID;
		
		[unroll]
		for (int n = 0; n < 23; ++n)
		{
			output.colors[n] = float4(n/3.0, n/8.0, n/24.0, 0);
		}
		output.normal = float4(1.0, 0.0, 0.0, 0.0);
		output.tangent = float4(0.0, 1.0, 0.0, 0.0);
		output.bitangent = float4(0.0, 0.0, 1.0, 0.0);
		
		stream.Append( output );
	}
}

////////////////////////////////////////////////////////////

GSPosition linePosition(uint index)
{
	GSPosition retval;

	uint col = index % (uint)vpSize.x;
	uint row = index / (uint)vpSize.x;
	
	// prevent a loop-back line
	retval.cut = (col == 0);

	// Vertices flow from one row to the next, according to vpSize.x dimension
	retval.pos.x = ( ( 2.0 * ( (float) col + 0.5 ) ) / vpSize.x ) - 1.0;
	retval.pos.y = 1.0 - ( ( 2.0 * ( (float) row + 0.5 ) ) / vpSize.y );
	retval.pos.zw = float2( 0.0, 1.0 );
	
	return retval;
}

[maxvertexcount (128)]
void GSLine( inout LineStream<GSOut> stream, point GSIn input[1], uint primID : SV_PrimitiveID )
{
	uint outNum = 0;
	for (outNum = 0; outNum < input[0].numOut; ++outNum)
	{
		if (   outNum > input[0].beginRestarts
			&& outNum < input[0].endRestarts
			&& input[0].restartFreq > 0
			&& (0 == (outNum % input[0].restartFreq)))
		{
			stream.RestartStrip();
		}
		
		GSOut output;
		
		//for( uint i = 0; i < 2; ++i )
		{	
			uint index = outNum + vpSize.x * primID;
			
			GSPosition pos = linePosition(index);
			if (pos.cut)
				stream.RestartStrip();
			output.position = pos.pos;
			
			output.primID = primID;
		
			stream.Append( output );
		}
	}
}


[maxvertexcount (256)]
void GSLineTiny( inout LineStream<GSOutTiny> stream, point GSIn input[1], uint primID : SV_PrimitiveID )
{
	uint outNum = 0;
	for (outNum = 0; outNum < input[0].numOut; ++outNum)
	{
		if (   outNum > input[0].beginRestarts
			&& outNum < input[0].endRestarts
			&& input[0].restartFreq > 0
			&& (0 == (outNum % input[0].restartFreq)))
		{
			stream.RestartStrip();
		}
		
		GSOutTiny output;
		
		//for( uint i = 0; i < 2; ++i )
		{	
			uint index = outNum + vpSize.x * primID;
			
			GSPosition pos = linePosition(index);
			if (pos.cut)
				stream.RestartStrip();
			output.position = pos.pos;
			
			stream.Append( output );
		}
	}
}


[maxvertexcount (9)]
void GSLineHuge( inout LineStream<GSOutHuge> stream, point GSIn input[1], uint primID : SV_PrimitiveID )
{
	uint outNum = 0;
	for (outNum = 0; outNum < input[0].numOut; ++outNum)
	{
		if (   outNum > input[0].beginRestarts
			&& outNum < input[0].endRestarts
			&& input[0].restartFreq > 0
			&& (0 == (outNum % input[0].restartFreq)))
		{
			stream.RestartStrip();
		}
		
		GSOutHuge output;
		
		//for( uint i = 0; i < 2; ++i )
		{	
			uint index = outNum + vpSize.x * primID;
			
			GSPosition pos = linePosition(index);
			if (pos.cut)
				stream.RestartStrip();
			output.position = pos.pos;
			
			output.primID = primID;
			
			[unroll]
			for (int n = 0; n < 23; ++n)
			{
				output.colors[n] = float4(n/3.0, n/8.0, n/24.0, 0);
			}
			output.normal = float4(1.0, 0.0, 0.0, 0.0);
			output.tangent = float4(0.0, 1.0, 0.0, 0.0);
			output.bitangent = float4(0.0, 0.0, 1.0, 0.0);
		
			stream.Append( output );
		}
	}
}

////////////////////////////////////////////////////////////

GSPosition trianglePosition(uint index)
{
	GSPosition retval;
	retval.cut = 0;

	uint col = ( index >> 1 );
	uint row = 2 * ( 1 - (index & 1) );
	
	row += 2 * (col / (uint)vpSize.x);
	col  =      col % (uint)vpSize.x;
	
	// prevent a loop-back line
	//retval.cut = (col == 0 && 0 == (row % 2)); (Win7: 630616)

	// Vertices flow from one row to the next, according to vpSize.x dimension
	retval.pos.x = ( ( 2.0 * ( (float)col + 0.5 ) ) / vpSize.x ) - 1.0;
	retval.pos.y = 1.0 - ( ( 2.0 * ( (float)row + 0.5 ) ) / vpSize.y );
	retval.pos.zw = float2( 0.0, 1.0 );
	
	return retval;
}

[maxvertexcount (128)]
void GSTriangle( inout TriangleStream<GSOut> stream, point GSIn input[1], uint primID : SV_PrimitiveID )
{
	uint outNum = 0;
	for (outNum = 0; outNum < input[0].numOut; ++outNum)
	{
		if (   outNum > input[0].beginRestarts
			&& outNum < input[0].endRestarts
			&& input[0].restartFreq > 0
			&& (0 == (outNum % input[0].restartFreq)))
		{
			stream.RestartStrip();
		}
		
		GSOut output;

		//for( uint i = 0; i < 3; ++i )
		{	
			uint index = outNum + vpSize.x * primID;
			
			GSPosition pos = trianglePosition(index);
			if (pos.cut)
				stream.RestartStrip();
			output.position = pos.pos;
			
			output.primID = primID;
		
			stream.Append( output );
		}
	}
}


[maxvertexcount (256)]
void GSTriangleTiny( inout TriangleStream<GSOutTiny> stream, point GSIn input[1], uint primID : SV_PrimitiveID )
{
	uint outNum = 0;
	for (outNum = 0; outNum < input[0].numOut; ++outNum)
	{
		if (   outNum > input[0].beginRestarts
			&& outNum < input[0].endRestarts
			&& input[0].restartFreq > 0
			&& (0 == (outNum % input[0].restartFreq)))
		{
			stream.RestartStrip();
		}
		
		GSOutTiny output;

		//for( uint i = 0; i < 3; ++i )
		{	
			uint index = outNum + vpSize.x * primID;
			
			GSPosition pos = trianglePosition(index);
			if (pos.cut)
				stream.RestartStrip();
			output.position = pos.pos;
		
			stream.Append( output );
		}
	}
}


[maxvertexcount (9)]
void GSTriangleHuge( inout TriangleStream<GSOutHuge> stream, point GSIn input[1], uint primID : SV_PrimitiveID )
{
	uint outNum = 0;
	for (outNum = 0; outNum < input[0].numOut; ++outNum)
	{
		if (   outNum > input[0].beginRestarts
			&& outNum < input[0].endRestarts
			&& input[0].restartFreq > 0
			&& (0 == (outNum % input[0].restartFreq)))
		{
			stream.RestartStrip();
		}
		
		GSOutHuge output;
		
		//for( uint i = 0; i < 3; ++i )
		{	
			uint index = outNum + vpSize.x * primID;
			
			GSPosition pos = trianglePosition(index);
			if (pos.cut)
				stream.RestartStrip();
			output.position = pos.pos;
			
			output.primID = primID;
			
			[unroll]
			for (int n = 0; n < 23; ++n)
			{
				output.colors[n] = float4(n/3.0, n/8.0, n/24.0, 0);
			}
			output.normal = float4(1.0, 0.0, 0.0, 0.0);
			output.tangent = float4(0.0, 1.0, 0.0, 0.0);
			output.bitangent = float4(0.0, 0.0, 1.0, 0.0);
		
			stream.Append( output );
		}
	}
}

////////////////////////////////////////////////////////////

PSOut PSMain( PSIn input )
{
	PSOut output;
	
	output.color = float4( 1.0, 1.0, 1.0, 1.0 );
	output.color.r = input.position.y / vpSize.y;
	output.color.g = input.position.x / vpSize.x;
	output.color.b = 0.5;
	output.color.a = 1.0;

	return output;
}

