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

struct VSIn
{
	float2 pos : pos;
};

struct VSOut
{
	float4 color : outColor;
	float4 pos : SV_Position;
};

float4 outPosition(float2 vPos)
{
	float4 outPos;
	
	outPos = float4( 0, 0, 0, 1.0 );
	outPos.xy = vPos * inverseViewport.zw + inverseViewport.xy;
	
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

VSOut main(VSIn input)
{
	VSOut output;
	output.color = float4( 1.0, 1.0, 1.0, 1.0 );
	return output;
}

VSOut main_1_L0(VSIn input)
{
	VSOut output;
	float1 tex = interpolateCoords(input.pos);
	float comparison = calcComparison(input.pos);
	output.color = myTex1D.SampleCmpLevelZero(samp_c, tex, comparison);
	output.pos = outPosition(input.pos);
	return output;
}

VSOut main_2_L0(VSIn input)
{
	VSOut output;
	float2 tex = interpolateCoords(input.pos);
	float comparison = calcComparison(input.pos);
	output.color = myTex2D.SampleCmpLevelZero(samp_c, tex, comparison);
	output.pos = outPosition(input.pos);
	return output;
}

VSOut main_4_L0(VSIn input)
{
	VSOut output;
	float3 tex = interpolateCoords(input.pos);
	float comparison = calcComparison(input.pos);
	output.color = myTexCube.SampleCmpLevelZero(samp_c, tex, comparison);
	output.pos = outPosition(input.pos);
	return output;
}

VSOut main_1_L0_aoff(VSIn input)
{
	VSOut output;
	float1 tex = interpolateCoords(input.pos);
	float comparison = calcComparison(input.pos);
	if (input.pos.x < 1/inverseViewport.z)
	{
		if (input.pos.y < -1/inverseViewport.w)
			output.color = myTex1D.SampleCmpLevelZero(samp_c, tex, comparison, int3(-1,0,-1));
		else
			output.color = myTex1D.SampleCmpLevelZero(samp_c, tex, comparison, int3(0,-1, 0));
	}
	else
	{
		if (input.pos.y < -1/inverseViewport.w)
			output.color = myTex1D.SampleCmpLevelZero(samp_c, tex, comparison, int3(1,1,2));
		else
			output.color = myTex1D.SampleCmpLevelZero(samp_c, tex, comparison, int3(0,2,1));
	}
	output.pos = outPosition(input.pos);
	return output;
}

VSOut main_2_L0_aoff(VSIn input)
{
	VSOut output;
	float2 tex = interpolateCoords(input.pos);
	float comparison = calcComparison(input.pos);
	if (input.pos.x < 1/inverseViewport.z)
	{
		if (input.pos.y < -1/inverseViewport.w)
			output.color = myTex2D.SampleCmpLevelZero(samp_c, tex, comparison, int3(-1,0,-1));
		else
			output.color = myTex2D.SampleCmpLevelZero(samp_c, tex, comparison, int3(0,-1, 0));
	}
	else
	{
		if (input.pos.y < -1/inverseViewport.w)
			output.color = myTex2D.SampleCmpLevelZero(samp_c, tex, comparison, int3(1,1,2));
		else
			output.color = myTex2D.SampleCmpLevelZero(samp_c, tex, comparison, int3(0,2,1));
	}
	output.pos = outPosition(input.pos);
	return output;
}
