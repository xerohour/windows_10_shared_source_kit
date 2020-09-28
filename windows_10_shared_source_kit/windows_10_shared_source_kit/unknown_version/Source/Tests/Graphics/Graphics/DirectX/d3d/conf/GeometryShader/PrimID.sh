// PrimID.sh
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//


cbuffer ConstData
{
	float4 vpSize;
};

struct VSIn
{
	uint4 data : data;
};

struct VSOut
{
	uint4 data : data;
	uint instanceID : instanceID;
};

struct GSIn
{
	uint4 data : data;
	uint instanceID : instanceID;
};

struct GSOut
{
	uint data : data;
	float4 pos : SV_Position;
	uint3 primIDExpected : primIDExpected;
	uint primID : SV_PrimitiveID;
};

struct PSIn
{
	uint data : data;
	float4 pos : SV_Position;
	uint3 primIDExpected : primIDExpected;
	uint primID : SV_PrimitiveID;
};

struct PSOut
{
	float4 color : SV_Target;
};

uint Encode( uint i )
{
	return i + 1;
	//return ( i >> 1 ) ^ i;
}


VSOut VSPassThrough( VSIn input, uint instanceID : SV_InstanceID )
{
	VSOut output;
	
	output.data = input.data;
	output.instanceID = instanceID;
	
	return output;
}


VSOut VSChangeData( VSIn input )
{
	VSOut output;
	
	output.data = Encode( input.data.x );
	
	return output;
}

[maxvertexcount (4)]
void GSPrimIDTriangleIn( inout PointStream<GSOut> stream, triangle GSIn input[3], uint primID : SV_PrimitiveID )
{
	GSOut output;
	uint expectedPrimID = ( input[0].data.x & 0x0f00 ) >> 8;
	output.pos.x = ( ( 2.0 * (float) expectedPrimID + 1.0 ) / vpSize.x ) - 1.0;
	output.pos.y = ( ( -2.0 * (float) input[2].instanceID - 1.0 ) / vpSize.y ) + 1.0;
	output.pos.zw = float2( 0.0, 1.0 );
	output.data = expectedPrimID == primID ? 1 : 0;
	output.primID = expectedPrimID + 7;
	output.primIDExpected = expectedPrimID + 7;
	stream.Append( output );
}

[maxvertexcount (4)]
void GSPrimIDLineIn( inout PointStream<GSOut> stream, line GSIn input[2], uint primID : SV_PrimitiveID )
{
	GSOut output;
	uint expectedPrimID = ( input[0].data.x & 0x0f00 ) >> 8;
	output.pos.x = ( ( 2.0 * (float) expectedPrimID + 1.0 ) / vpSize.x ) - 1.0;
	output.pos.y = ( ( -2.0 * (float) input[1].instanceID - 1.0 ) / vpSize.y ) + 1.0;
	output.pos.zw = float2( 0.0, 1.0 );
	output.data = expectedPrimID == primID ? 1 : 0;
	output.primID = expectedPrimID + 7;
	output.primIDExpected = expectedPrimID + 7;
	stream.Append( output );
}

[maxvertexcount (4)]
void GSPrimIDPointIn( inout PointStream<GSOut> stream, point GSIn input[1], uint primID : SV_PrimitiveID )
{
	GSOut output;
	uint expectedPrimID = ( input[0].data.x & 0x0f00 ) >> 8;
	output.pos.x = ( ( 2.0 * (float) expectedPrimID + 1.0 ) / vpSize.x ) - 1.0;
	output.pos.y = ( ( -2.0 * (float) input[0].instanceID - 1.0 ) / vpSize.y ) + 1.0;
	output.pos.zw = float2( 0.0, 1.0 );
	output.data = expectedPrimID == primID ? 1 : 0;
	output.primID = expectedPrimID + 7;
	output.primIDExpected = expectedPrimID + 7;
	stream.Append( output );
}

PSOut PSMain( PSIn input )
{
	PSOut output;
	
	output.color = float4( 1.0, 1.0, 1.0, 1.0 );
	
	if( input.data == 0 )
		output.color.y = 0;
		
	if( input.primID != input.primIDExpected.x )
		output.color.z = 0;
	
	return output;
}
