Texture1D myTex1D;
Texture2D myTex2D;
Texture3D myTex3D;
TextureCube myTexCube;
SamplerComparisonState samp_c;

cbuffer cb0 : register(b0)
{
	float4 inverseViewport;
};

cbuffer cb1 : register(b1)
{
	float4 texCoords[4];
};

struct GSIn
{
	float4 pos : SV_Position;
	float2 pos2: pixelPos;
};

struct GSOut
{
	float4 color : outColor;
	float4 pos : SV_Position;
};

float4 outPosition(float2 vPos)
{
	float4 outPos;
	
	outPos = float4( 0, 0, 0, 1.0 );
	outPos.xy = vPos.xy;
	
	return outPos;
}

float xFraction(float2 pos)
{
	return (pos.x * inverseViewport.z * 0.5);
}

float yFraction(float2 pos)
{
	return (pos.y * -inverseViewport.w * 0.5);
}

float4 interpolateCoords(float2 pos)
{
	float xFrac = xFraction(pos);
	float yFrac = yFraction(pos);
	
	float4 topVals = texCoords[0] * (1-xFrac) + texCoords[1] * xFrac;
	float4 botVals = texCoords[3] * (1-xFrac) + texCoords[2] * xFrac;
	float4 finalVal = topVals * (1-yFrac) + botVals * (yFrac);
	
	return finalVal;
}

float calcComparison(float2 pos)
{
	float xFrac = xFraction(pos);
	float yFrac = yFraction(pos);
	
	float compVal = (xFrac + yFrac) / 2;

	return compVal;
}

[maxvertexcount (1)]
void main( inout PointStream<GSOut> stream, point GSIn input[1] )
{
	GSOut output;
	output.pos = outPosition(input[0].pos);
	output.color = float4( 1.0, 1.0, 1.0, 1.0 );
	stream.Append( output );
}

[maxvertexcount (1)]
void main_1_L0( inout PointStream<GSOut> stream, point GSIn input[1])
{
	GSOut output;
	float1 tex = interpolateCoords(input[0].pos2);
	float comparison = calcComparison(input[0].pos2);
	output.color = myTex1D.SampleCmpLevelZero(samp_c, tex, comparison);
	output.pos = outPosition(input[0].pos);
	stream.Append( output );
}

[maxvertexcount (1)]
void main_2_L0( inout PointStream<GSOut> stream, point GSIn input[1])
{
	GSOut output;
	float2 tex = interpolateCoords(input[0].pos2);
	float comparison = calcComparison(input[0].pos2);
	output.color = myTex2D.SampleCmpLevelZero(samp_c, tex, comparison);
	output.pos = outPosition(input[0].pos);
	stream.Append( output );
}

[maxvertexcount (1)]
void main_4_L0( inout PointStream<GSOut> stream, point GSIn input[1])
{
	GSOut output;
	float3 tex = interpolateCoords(input[0].pos2);
	float comparison = calcComparison(input[0].pos2);
	output.color = myTexCube.SampleCmpLevelZero(samp_c, tex, comparison);
	output.pos = outPosition(input[0].pos);
	stream.Append( output );
}

[maxvertexcount (1)]
void main_1_L0_aoff( inout PointStream<GSOut> stream, point GSIn input[1])
{
	GSOut output;
	float1 tex = interpolateCoords(input[0].pos2);
	float comparison = calcComparison(input[0].pos2);
	if (input[0].pos.x < 0)
	{
		if (input[0].pos.y > 0)
			output.color = myTex1D.SampleCmpLevelZero(samp_c, tex, comparison, int3(-1,0,-1));
		else
			output.color = myTex1D.SampleCmpLevelZero(samp_c, tex, comparison, int3(0,-1, 0));
	}
	else
	{
		if (input[0].pos.y > 0)
			output.color = myTex1D.SampleCmpLevelZero(samp_c, tex, comparison, int3(1,1,2));
		else
			output.color = myTex1D.SampleCmpLevelZero(samp_c, tex, comparison, int3(0,2,1));
	}
	output.pos = outPosition(input[0].pos);
	stream.Append( output );
}

[maxvertexcount (1)]
void main_2_L0_aoff( inout PointStream<GSOut> stream, point GSIn input[1])
{
	GSOut output;
	float2 tex = interpolateCoords(input[0].pos2);
	float comparison = calcComparison(input[0].pos2);
	if (input[0].pos.x < 0)
	{
		if (input[0].pos.y > 0)
			output.color = myTex2D.SampleCmpLevelZero(samp_c, tex, comparison, int3(-1,0,-1));
		else
			output.color = myTex2D.SampleCmpLevelZero(samp_c, tex, comparison, int3(0,-1, 0));
	}
	else
	{
		if (input[0].pos.y > 0)
			output.color = myTex2D.SampleCmpLevelZero(samp_c, tex, comparison, int3(1,1,2));
		else
			output.color = myTex2D.SampleCmpLevelZero(samp_c, tex, comparison, int3(0,2,1));
	}
	output.pos = outPosition(input[0].pos);
	stream.Append( output );
}
