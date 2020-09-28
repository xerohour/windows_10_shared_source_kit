Texture1D myTex1D;
Texture2D myTex2D;
Texture3D myTex3D;
TextureCube myTexCube;
sampler samp;

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

float calcLevel(float2 pos)
{
	float xFrac = xFraction(pos);
	float yFrac = yFraction(pos);
	
	float levelVal = (xFrac + yFrac) / 2;
	return (levelVal * 7) - 1;
}

VSOut main(VSIn input)
{
	VSOut output;
	output.color = float4( 1.0, 1.0, 1.0, 1.0 );
	output.pos = outPosition(input.pos);
	return output;
}

VSOut main_1(VSIn input)
{
	VSOut output;
	float1 tex = interpolateCoords(input.pos);
	output.color = myTex1D.SampleLevel(samp, tex, calcLevel(input.pos));
	output.pos = outPosition(input.pos);
	return output;
}

VSOut main_2(VSIn input)
{
	VSOut output;
	float2 tex = interpolateCoords(input.pos);
	output.color = myTex2D.SampleLevel(samp, tex, calcLevel(input.pos));
	output.pos = outPosition(input.pos);
	return output;
}

VSOut main_3(VSIn input)
{
	VSOut output;
	float3 tex = interpolateCoords(input.pos);
	output.color = myTex3D.SampleLevel(samp, tex, calcLevel(input.pos));
	output.pos = outPosition(input.pos);
	return output;
}

VSOut main_4(VSIn input)
{
	VSOut output;
	float3 tex = interpolateCoords(input.pos);
	output.color = myTexCube.SampleLevel(samp, tex, calcLevel(input.pos));
	output.pos = outPosition(input.pos);
	return output;
}

VSOut main_1_aoff(VSIn input)
{
	VSOut output;
	float1 tex = interpolateCoords(input.pos);
	if (input.pos.x < 1/inverseViewport.z)
	{
		if (input.pos.y < -1/inverseViewport.w)
			output.color = myTex1D.SampleLevel(samp, tex, calcLevel(input.pos), int3(-1,0,-1));
		else
			output.color = myTex1D.SampleLevel(samp, tex, calcLevel(input.pos), int3(0,-1, 0));
	}
	else
	{
		if (input.pos.y < -1/inverseViewport.w)
			output.color = myTex1D.SampleLevel(samp, tex, calcLevel(input.pos), int3(1,1,2));
		else
			output.color = myTex1D.SampleLevel(samp, tex, calcLevel(input.pos), int3(0,2,1));
	}
	output.pos = outPosition(input.pos);
	return output;
}

VSOut main_2_aoff(VSIn input)
{
	VSOut output;
	float2 tex = interpolateCoords(input.pos);
	if (input.pos.x < 1/inverseViewport.z)
	{
		if (input.pos.y < -1/inverseViewport.w)
			output.color = myTex2D.SampleLevel(samp, tex, calcLevel(input.pos), int3(-1,0,-1));
		else
			output.color = myTex2D.SampleLevel(samp, tex, calcLevel(input.pos), int3(0,-1, 0));
	}
	else
	{
		if (input.pos.y < -1/inverseViewport.w)
			output.color = myTex2D.SampleLevel(samp, tex, calcLevel(input.pos), int3(1,1,2));
		else
			output.color = myTex2D.SampleLevel(samp, tex, calcLevel(input.pos), int3(0,2,1));
	}
	output.pos = outPosition(input.pos);
	return output;
}

VSOut main_3_aoff(VSIn input)
{
	VSOut output;
	float3 tex = interpolateCoords(input.pos);
	if (input.pos.x < 1/inverseViewport.z)
	{
		if (input.pos.y < -1/inverseViewport.w)
			output.color = myTex3D.SampleLevel(samp, tex, calcLevel(input.pos), int3(-1,0,-1));
		else
			output.color = myTex3D.SampleLevel(samp, tex, calcLevel(input.pos), int3(0,-1, 0));
	}
	else
	{
		if (input.pos.y < -1/inverseViewport.w)
			output.color = myTex3D.SampleLevel(samp, tex, calcLevel(input.pos), int3(1,1,2));
		else
			output.color = myTex3D.SampleLevel(samp, tex, calcLevel(input.pos), int3(0,2,1));
	}
	output.pos = outPosition(input.pos);
	return output;
}

