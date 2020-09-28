// GSTopology.sh
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//

////////////////////////////////////////////////////////////

cbuffer ConstData
{
	float4 vpSize;
};

cbuffer AdjacencyData
{
	uint4 adjacencyData[4096];
};

////////////////////////////////////////////////////////////

struct VSIn
{
	uint id : id;
};

////////////////////////////////////////////////////////////

struct VSOut
{
	uint id : id;
};

////////////////////////////////////////////////////////////

struct GSIn
{
	uint id : id;
};

////////////////////////////////////////////////////////////

struct GSOut
{
	uint id : id;
	uint primID : primID;
	uint4 oppositeCorrect : oppositeCorrect;
	float4 position : SV_Position;
};

////////////////////////////////////////////////////////////

struct VSOutWithPos
{
	uint id : id;
	uint primID : primID;
	uint4 oppositeCorrect : oppositeCorrect;
	float4 position : SV_Position;
};

////////////////////////////////////////////////////////////

struct PSIn
{
	uint id : id;
	uint4 oppositeCorrect : oppositeCorrect;
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
	
	output.id = input.id;
	
	return output;
}

////////////////////////////////////////////////////////////

VSOutWithPos VSPoint( VSIn input )
{
	VSOutWithPos output;
	output.oppositeCorrect = uint4( 1, 1, 1, 1 );
	
	output.id = input.id;
	
	uint index = input.id & 0x0000ffff;
	float offset = (float) index;

	// All vertices are in row 0 and are just a bit to the right of pixel center	
	output.position.x = ( ( 2.0 * ( offset + 0.6 ) ) / vpSize.x ) - 1.0;
	output.position.y = 1.0 - ( ( 2.0 * ( 0.6 ) ) / vpSize.y );
	output.position.zw = float2( 0.0, 1.0 );
	
	output.primID = 0-1;
	
	return output;
}

////////////////////////////////////////////////////////////

VSOutWithPos VSLine( VSIn input )
{
	VSOutWithPos output;
	output.oppositeCorrect = uint4( 1, 1, 1, 1 );
	
	output.id = input.id;
	
	uint index = input.id & 0x0000ffff;

	// All vertices are in row 0 and are at pixel centers
	output.position.x = ( ( 2.0 * ( (float) index + 0.5 ) ) / vpSize.x ) - 1.0;
	output.position.y = 1.0 - ( ( 2.0 * ( 0.5 ) ) / vpSize.y );
	output.position.zw = float2( 0.0, 1.0 );
	
	output.primID = 0-1;
	
	return output;
}

////////////////////////////////////////////////////////////

VSOutWithPos VSTriangle( VSIn input )
{
	VSOutWithPos output;
	output.oppositeCorrect = uint4( 1, 1, 1, 1 );
	
	output.id = input.id;
	
	uint index = input.id & 0x0000ffff;

	// Odd vertices are in row 0
	// Even vertices are in row 1
	uint col = ( index >> 1 );
	uint row = 2 * ( 1 - ( index & 1 ) );
	output.position.x = ( ( 2.0 * ( (float) col + 0.5 ) ) / vpSize.x ) - 1.0;
	output.position.y = 1.0 - ( ( 2.0 * ( (float) row + 0.5 ) ) / vpSize.y );
	output.position.zw = float2( 0.0, 1.0 );
	
	output.primID = 0-1;
	
	return output;
}

////////////////////////////////////////////////////////////

VSOutWithPos VSTriangleAdj( VSIn input )
{
	VSOutWithPos output;
	output.oppositeCorrect = uint4( 1, 1, 1, 1 );
	
	output.id = input.id;
	
	uint index = input.id & 0x0000ffff;

	// Odd vertices are in row 0
	// Even vertices are in row 1
	uint col = ( index >> 2 );
	uint row = ( 2 - ( index & 2 ) );

	output.position.x = ( ( 2.0 * ( (float) col + 0.5 ) ) / vpSize.x ) - 1.0;
	output.position.y = 1.0 - ( ( 2.0 * ( (float) row + 0.5 ) ) / vpSize.y );
	output.position.zw = float2( 0.0, 1.0 );
	
	output.primID = 0-1;
	
	return output;
}

////////////////////////////////////////////////////////////

[maxvertexcount (1)]
void GSPoint( inout PointStream<GSOut> stream, point GSIn input[1], uint primID : SV_PrimitiveID )
{
	GSOut output;
	output.oppositeCorrect = uint4( 1, 1, 1, 1 );
	
	output.id = input[0].id;
	
	uint index = input[0].id & 0x0000ffff;
	float offset = (float) index;

	// All vertices are in row 0 and are just a bit to the right of pixel center	
	output.position.x = ( ( 2.0 * ( offset + 0.6 ) ) / vpSize.x ) - 1.0;
	output.position.y = 1.0 - ( ( 2.0 * ( 0.6 ) ) / vpSize.y );
	output.position.zw = float2( 0.0, 1.0 );
	
	output.primID = primID;
	
	stream.Append( output );
}

////////////////////////////////////////////////////////////

[maxvertexcount (2)]
void GSLine( inout LineStream<GSOut> stream, line GSIn input[2], uint primID : SV_PrimitiveID )
{
	GSOut output;
	output.oppositeCorrect = uint4( 1, 1, 1, 1 );

	for( uint i = 0; i < 2; ++i )
	{	
		output.id = input[i].id;
		
		uint index = input[i].id & 0x0000ffff;

		// All vertices are in row 0 and are at pixel centers
		output.position.x = ( ( 2.0 * ( (float) index + 0.5 ) ) / vpSize.x ) - 1.0;
		output.position.y = 1.0 - ( ( 2.0 * ( 0.5 ) ) / vpSize.y );
		output.position.zw = float2( 0.0, 1.0 );
		
		output.primID = primID;
	
		stream.Append( output );
	}
}

////////////////////////////////////////////////////////////

[maxvertexcount (6)]
void GSLineAdj( inout LineStream<GSOut> stream, lineadj GSIn input[4], uint primID : SV_PrimitiveID )
{
	GSOut output;
	output.oppositeCorrect = uint4( 1, 1, 1, 1 );

	// Determine if adjacency information is correct
	uint4 adjacentVerts = adjacencyData[primID];
	
	if( (input[3].id & 0x0000ffff) != adjacentVerts.x )
		output.oppositeCorrect.x = 0;
		
	if( (input[0].id & 0x0000ffff) != adjacentVerts.y )
		output.oppositeCorrect.y = 0;
			
	for( uint i = 1; i < 3; ++i )
	{	
		output.id = input[i].id;
		
		uint index = input[i].id & 0x0000ffff;

		// All vertices are in row 0 and are at pixel centers
		output.position.x = ( ( 2.0 * ( (float) index + 0.5 ) ) / vpSize.x ) - 1.0;
		output.position.y = 1.0 - ( ( 2.0 * ( 0.5 ) ) / vpSize.y );
		output.position.zw = float2( 0.0, 1.0 );
		
		output.primID = primID;
	
		stream.Append( output );
	}
}

////////////////////////////////////////////////////////////

[maxvertexcount (3)]
void GSTriangle( inout TriangleStream<GSOut> stream, triangle GSIn input[3], uint primID : SV_PrimitiveID )
{
	GSOut output;
	output.oppositeCorrect = uint4( 1, 1, 1, 1 );

	for( uint i = 0; i < 3; ++i )
	{	
		output.id = input[i].id;
		
		uint index = input[i].id & 0x0000ffff;

		// Odd vertices are in row 0
		// Even vertices are in row 1
		uint col = ( index >> 1 );
		uint row = 2 * ( 1 - ( index & 1 ) );
		output.position.x = ( ( 2.0 * ( (float) col + 0.5 ) ) / vpSize.x ) - 1.0;
		output.position.y = 1.0 - ( ( 2.0 * ( (float) row + 0.5 ) ) / vpSize.y );
		output.position.zw = float2( 0.0, 1.0 );
		
		output.primID = primID;
	
		stream.Append( output );
	}
}

////////////////////////////////////////////////////////////

[maxvertexcount (15)]
void GSTriangleAdj( inout TriangleStream<GSOut> stream, triangleadj GSIn input[6], uint primID : SV_PrimitiveID )
{
	GSOut output;
	output.oppositeCorrect = uint4( 1, 1, 1, 1 );

	uint4 adjacentVerts = adjacencyData[primID];

	if( (input[3].id & 0x0000ffff) != adjacentVerts.x )
		output.oppositeCorrect.x = 0;
		
	if( (input[5].id & 0x0000ffff) != adjacentVerts.y )
		output.oppositeCorrect.y = 0;
		
	if( (input[1].id & 0x0000ffff) != adjacentVerts.z )
		output.oppositeCorrect.z = 0;
	
	for( uint i = 0; i < 6; i += 2 )
	{	
		output.id = input[i].id;
		
		uint index = input[i].id & 0x0000ffff;

		// Odd vertices are in row 0
		// Even vertices are in row 1
		uint col = ( index >> 2 );
		uint row = ( 2 - ( index & 2 ) );
		output.position.x = ( ( 2.0 * ( (float) col + 0.5 ) ) / vpSize.x ) - 1.0;
		output.position.y = 1.0 - ( ( 2.0 * ( (float) row + 0.5 ) ) / vpSize.y );
		output.position.zw = float2( 0.0, 1.0 );
		
		output.primID = primID;
	
		stream.Append( output );
	}
}

////////////////////////////////////////////////////////////

PSOut PSMain( PSIn input )
{
	PSOut output;
	
	output.color = float4( 1.0, 1.0, 1.0, 1.0 );
	
	if( input.oppositeCorrect.x == 0 )
		output.color.x = .5;
	if( input.oppositeCorrect.y == 0 )
		output.color.y = .5;
	if( input.oppositeCorrect.z == 0 )
		output.color.z = .5;
	
	return output;
}

