// InstanceID.sh
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
};

struct GSIn
{
	uint4 data : data;
};

struct GSOut
{
	uint instanceID : gsInstanceID;
};

struct PSIn
{
	uint4 data : data;
	uint instanceID : gsInstanceID;
};

struct PSOut
{
	float4 color : SV_Target;
};

VSOut VSPassThrough( VSIn input )
{
	VSOut output;
	
	output.data = input.data;
	
	return output;
}

[maxvertexcount (4)]
[instance(NUM_INSTANCES_PER_PRIMITIVE)]
void GSInstanceIDTriangleIn( inout PointStream<GSOut> stream, triangle GSIn input[3], uint instanceID : SV_GSInstanceID  )
{
	GSOut output;
	output.instanceID = instanceID;
	
	stream.Append( output );
}

[maxvertexcount (4)]
[instance(NUM_INSTANCES_PER_PRIMITIVE)]
void GSInstanceIDLineIn( inout PointStream<GSOut> stream, line GSIn input[2], uint instanceID : SV_GSInstanceID  )
{
	GSOut output;
	output.instanceID = instanceID;
	
	stream.Append( output );
}

[maxvertexcount (4)]
[instance(NUM_INSTANCES_PER_PRIMITIVE)]
void GSInstanceIDPointIn( inout PointStream<GSOut> stream, point GSIn input[1], uint instanceID : SV_GSInstanceID  )
{
	GSOut output;
	output.instanceID = instanceID;
	
	stream.Append( output );
}