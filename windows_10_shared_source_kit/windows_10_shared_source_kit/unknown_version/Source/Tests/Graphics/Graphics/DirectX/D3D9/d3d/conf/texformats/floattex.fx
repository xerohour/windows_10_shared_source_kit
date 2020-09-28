//-----------------------------------------------------------------------------
//
// Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
//
// File:	floattex.fx
// Desc:	Contains the shaders for floating point texture tests.
// Author:	Matthew E. Haigh - September 2003
//-----------------------------------------------------------------------------

const float MAX_FLOAT = 3.4e38;

float4	g_ChannelFilter = float4 (1.0f, 1.0f, 1.0f, 1.0f);
float4	g_GlarePos = float4 (0.5f, 0.5f, 0.0f, 0.01f);
float4	g_LightPos = float4(0.5f, 0.5f, 0.0f, 1.0f);
float	g_ScaleFactor = 1.0f;
float	g_Zero = 0.0f;
float	g_TexXCoordOffset = 1.0f;
float	g_TexYCoordOffset = 1.0f;

TEXTURE floatTex0;
TEXTURE floatTex1;
TEXTURE floatTex2;
TEXTURE floatTex3;
TEXTURE glareTex0;
TEXTURE glareTex1;
TEXTURE glareTex2;
TEXTURE glareTex3;

sampler floatTexSampler0 = sampler_state { Texture = (floatTex0); };
sampler floatTexSampler1 = sampler_state { Texture = (floatTex1); };
sampler floatTexSampler2 = sampler_state { Texture = (floatTex2); };
sampler floatTexSampler3 = sampler_state { Texture = (floatTex3); };
sampler glareTexSampler0 = sampler_state { Texture = (glareTex0); };
sampler glareTexSampler1 = sampler_state { Texture = (glareTex1); };
sampler glareTexSampler2 = sampler_state { Texture = (glareTex2); };
sampler glareTexSampler3 = sampler_state { Texture = (glareTex3); };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//	Shaders
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name:	makeScaledGradient()
// Desc:	Creates a gradient texture that spans from 0 to g_ScaleFactor.  The
//			red channel scales based upon x texture coordinate, green scales
//			based upon y texture coordinate, and blue scales based on both.
//-----------------------------------------------------------------------------

float4 makeScaledGradient (float2 tCoords : TEXCOORD0) : COLOR
{	
	tCoords += float2 (g_TexXCoordOffset, g_TexYCoordOffset);

	return g_ScaleFactor * float4 (tCoords.x, tCoords.y, (tCoords.x + tCoords.y) / 2.0, 1.0);
}

//-----------------------------------------------------------------------------
// Name:	makeScaledStripes()
// Desc:	Creates a striped texture with values equal to g_ScaleFactor.
//			The stripes are based upon the y texture coordinate.
//-----------------------------------------------------------------------------

float4 makeScaledStripes (float2 tCoords : TEXCOORD0) : COLOR
{
	tCoords += float2 (g_TexXCoordOffset, g_TexYCoordOffset);
	
	float4 retCol = 0.0f;
	
	if (tCoords.y <= 0.125f)
		retCol = float4 (1.0f, 1.0f, 1.0f, 1.0f);	//white
	else if (tCoords.y <= 0.25f)
		retCol = float4 (0.0f, 1.0f, 0.0f, 1.0f);	//green
	else if (tCoords.y <= 0.375f)
		retCol = float4 (1.0f, 0.0f, 0.0f, 1.0f);	//red
	else if (tCoords.y <= 0.5f)
		retCol = float4 (0.0f, 0.0f, 1.0f, 1.0f);	//blue
	else if (tCoords.y <= 0.625f)
		retCol = float4 (1.0f, 1.0f, 0.0f, 1.0f);	//yellow
	else if (tCoords.y <= 0.75f)
		retCol = float4 (0.0f, 1.0f, 1.0f, 1.0f);	//cyan
	else if (tCoords.y <= 0.875f)
		retCol = float4 (1.0f, 0.0f, 1.0f, 1.0f);	//magenta
	else
		retCol = float4 (0.0f, 0.0f, 0.0f, 1.0f);	//black
		
	return g_ScaleFactor * retCol;
}

//-----------------------------------------------------------------------------
// Name:	makeGlareTexture()
// Desc:	creates a glare effect centered on the point g_GlarePos.  Creates
//			glare 'flare lines' along the lines x-axis, y-axis, y = x, and
//			y = -x local to the point g_GlarePos, as well as a glare 'spot'
//			centered on this position.
//	
// Notes:	g_GlarePos.xy should contain the glare point position
//			g_GlarePos.w should contain the desired glare brightness
//			
//			Also note that currently the flare lines along y=x and y=-x, as 
//			well as the glare spot are disabled.
//-----------------------------------------------------------------------------

float4 makeGlareTexture (float2 tCoords : TEXCOORD0) : COLOR
{	
	tCoords += float2 (g_TexXCoordOffset, g_TexYCoordOffset);

	float4 brightness = 0.0f;

	//get distance from glare point
	float dist = length (g_GlarePos.xy - tCoords);
	float xAxis = abs (tCoords.x - g_GlarePos.x);
	float yAxis = abs (tCoords.y - g_GlarePos.y);
	//float yxLineDist = abs ((tCoords.x - g_GlarePos.x) - (tCoords.y - g_GlarePos.y));
	//float ynegxLineDist = abs ((tCoords.x - g_GlarePos.x) + (tCoords.y - g_GlarePos.y));
	
	
	//further the distance, the dimmer it should be
	//the glare brightness is set in 'w' of the glare position vector
	//brightness += g_GlarePos.w * (1.0f / (dist + 0.001f));
	brightness += g_GlarePos.w * (1.0f / max (pow (xAxis, 0.75f) + 0.3f * dist, 0.001f));
	brightness += g_GlarePos.w * (1.0f / max (pow (yAxis, 0.75f) + 0.3f * dist, 0.001f));
	//brightness += g_GlarePos.w * (1.0f / max (pow (yxLineDist, 0.75) + 0.3f * dist, 0.001f));
	//brightness += g_GlarePos.w * (1.0f / max (pow (ynegxLineDist, 0.75) +  0.3f * dist, 0.001f));
	
	return brightness;
}

//-----------------------------------------------------------------------------
// Name:	divideTextureByFactor()
// Desc:	Divides the texture set to sampler 'floatTexSampler0' by
//			the value set in g_ScaleFactor.
//-----------------------------------------------------------------------------

float4 divideTextureByFactor (float2 tCoords : TEXCOORD0) : COLOR
{	
	tCoords += float2 (g_TexXCoordOffset, g_TexYCoordOffset);

	return g_ChannelFilter * (tex2D (floatTexSampler0, tCoords) / g_ScaleFactor);
}

//-----------------------------------------------------------------------------
// Name:	scaleTextureByConstant()
// Desc:	Multiples the texture set to sampler 'floatTexSampler0' by
//			the value set in 'scaleVal'
//-----------------------------------------------------------------------------

float scaleVal;

float4 scaleTextureByConstant (float2 tCoords : TEXCOORD0) : COLOR
{	
	tCoords += float2 (g_TexXCoordOffset, g_TexYCoordOffset);

	return scaleVal * tex2D (floatTexSampler0, tCoords);
}

//-----------------------------------------------------------------------------
// Name:	addTwoTextures()
// Desc:	Sums the values in the first two samplers.  Note that the texture
//			format doesn't necesarily contain green & blue channels, and we
//			want the glare effect to appear white, so we swizzle its red 
//			channel to cover all three color channels.
//-----------------------------------------------------------------------------

float4 addTwoTextures (float2 tCoords : TEXCOORD0) : COLOR
{	
	tCoords += float2 (g_TexXCoordOffset, g_TexYCoordOffset);

	float4 glare = tex2D (glareTexSampler0, tCoords);
	
	return glare.xxxw + g_ChannelFilter * tex2D (floatTexSampler0, tCoords);
}

//-----------------------------------------------------------------------------
// Name:	addFourTextures()
// Desc:	Sums the values in the first four samplers.  Note that the texture
//			format doesn't necesarily contain green & blue channels, and we
//			want the glare effect to appear white, so we swizzle its red 
//			channel to cover all three color channels.
//-----------------------------------------------------------------------------

float4 addFourTextures (float2 tCoords : TEXCOORD0) : COLOR
{	
	tCoords += float2 (g_TexXCoordOffset, g_TexYCoordOffset);

	float4 glare = tex2D (glareTexSampler0, tCoords) + 
				   tex2D (glareTexSampler1, tCoords);
	
	return glare.xxxw + g_ChannelFilter * (tex2D (floatTexSampler0, tCoords) +
										   tex2D (floatTexSampler1, tCoords));
}

//-----------------------------------------------------------------------------
// Name:	addEightTextures()
// Desc:	Sums the values in the first eight samplers.  Note that the texture
//			format doesn't necesarily contain green & blue channels, and we
//			want the glare effect to appear white, so we swizzle its red 
//			channel to cover all three color channels.
//-----------------------------------------------------------------------------

float4 addEightTextures (float2 tCoords : TEXCOORD0) : COLOR
{	
	tCoords += float2 (g_TexXCoordOffset, g_TexYCoordOffset);

	float4 glare = tex2D (glareTexSampler0, tCoords) + 
				   tex2D (glareTexSampler1, tCoords) + 
				   tex2D (glareTexSampler2, tCoords) + 
				   tex2D (glareTexSampler3, tCoords);
	
	return glare.xxxw + g_ChannelFilter * (tex2D (floatTexSampler0, tCoords) +
										   tex2D (floatTexSampler1, tCoords) +
										   tex2D (floatTexSampler2, tCoords) +
										   tex2D (floatTexSampler3, tCoords));
}

//-----------------------------------------------------------------------------
// Name:	applyLight()
// Desc:	Shader applies lighting to the source texture in floatTexSampler0.
//			It simply multiplies the textures pixel color by a calculated
//			brightness value.
//
// Notes:	g_LightPos.xy should contain the position of the light
//			g_LightPos.w should contain the brightness of the light
//-----------------------------------------------------------------------------

float4 applyLight (float2 tCoords : TEXCOORD0) : COLOR
{	
	tCoords += float2 (g_TexXCoordOffset, g_TexYCoordOffset);

	//get distance from light
	//float dist = length (g_LightPos.xy - tCoords);
	float distSquared = (g_LightPos.x - tCoords.x) * (g_LightPos.x - tCoords.x) + 
			   			(g_LightPos.y - tCoords.y) * (g_LightPos.y - tCoords.y) + 0.01;

	//further the distance, the dimmer it should be - take higher power to increase decay
	float tempVal = 1.0f / (pow (distSquared, 0.5) + 1.0f);
	float4 brightness = pow (tempVal, 6);
	
	return brightness * g_LightPos.w * tex2D (floatTexSampler0, tCoords);
}

//-----------------------------------------------------------------------------
// Name:	createInfTex
// Desc:	Creates a texture filled with Inf and negative Inf.  Uses the 
//			log() function to accomplish these values.
//-----------------------------------------------------------------------------

float4 createInfTex (float2 tCoords : TEXCOORD0) : COLOR
{	
	tCoords += float2 (g_TexXCoordOffset, g_TexYCoordOffset);

	float4 retVal;
	
	if (tCoords.x < 0.5)
		retVal = log (g_Zero);
	else
	{
		retVal = -1.0f;
		retVal *= log (g_Zero);
	}
	
	return retVal;
}

//-----------------------------------------------------------------------------
// Name:	detectInfTex
// Desc:	Detects a texture filled with infinity values.  Since it is 
//			possible for an infinity to switch to max float, it checks for 
//			these values as well.
//-----------------------------------------------------------------------------

float4 detectInfTex (float2 tCoords : TEXCOORD0) : COLOR
{	
	tCoords += float2 (g_TexXCoordOffset, g_TexYCoordOffset);

	float4 texVal = tex2D (floatTexSampler0, tCoords);
	float infinity;
	float4 retVal;
	
	
	if (tCoords.x < 0.5)
		infinity = log (g_Zero);
	else
	{
		infinity = -1.0f;
		infinity *= log (g_Zero);
	}

	if ((texVal.x == infinity && isinf (texVal.x)) || abs (texVal.x) >= MAX_FLOAT)
		retVal = 1.0f;
	else if (isinf (texVal.x))
		retVal = 0.65f;
	else
		retVal = 0.3f;
		
	return g_ChannelFilter * retVal;	
}

//-----------------------------------------------------------------------------
// Name:	createNaNTex
// Desc:	Creates a texture filled with NaN.  A NaN in IEEE float 32 is 
//			defined as sign bit 0, exponent FF, and mantissa not all 0's or 1's
//			so we choose 7FF11111 and 7FF77777 for our NaN values.
//-----------------------------------------------------------------------------

float4 createNaNTex (float2 tCoords : TEXCOORD0) : COLOR
{	
	tCoords += float2 (g_TexXCoordOffset, g_TexYCoordOffset);

	float4 retVal;
	
	if (tCoords.x < 0.5)
		retVal = (float)0x7FF11111;
	else
		retVal = (float)0x7FF77777;
	
	return retVal;
}

//-----------------------------------------------------------------------------
// Name:	detectNaNTex
// Desc:	Detects the texture created by 'createNaNTex'.  That is, it checks
//			to make sure the values are maintained, and outputs colors based
//			upon this. 
//-----------------------------------------------------------------------------

float4 detectNaNTex (float2 tCoords : TEXCOORD0) : COLOR
{	
	tCoords += float2 (g_TexXCoordOffset, g_TexYCoordOffset);

	float4 texVal = tex2D (floatTexSampler0, tCoords);
	float nan;
	float4 retVal;
	
	if (texVal.x == (float)0x7FF11111)
		retVal = 1.0f;
	else if (texVal.x == (float)0x7FF77777)
		retVal = 0.65f;
	else
		retVal = 0.3f;
		
	return g_ChannelFilter * retVal;	
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//	Techniques
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

technique MakeScaledGradientTech
{
	Pass P0
	{
		PixelShader = compile ps_2_0 makeScaledGradient();
	}
}

//-----------------------------------------------------------------------------

technique MakeScaledStripesTech
{
	Pass P0
	{
		PixelShader = compile ps_2_0 makeScaledStripes();
	}
}

//-----------------------------------------------------------------------------

technique MakeGlareTextureTech
{
	Pass P0
	{
		PixelShader = compile ps_2_0 makeGlareTexture();
	}
}

//-----------------------------------------------------------------------------

technique DivideTextureByFactorTech
{
	Pass P0
	{
		PixelShader = compile ps_2_0 divideTextureByFactor();
	}
}

//-----------------------------------------------------------------------------

technique ScaleTextureByConstantTech
{
	Pass P0
	{
		PixelShader = compile ps_2_0 scaleTextureByConstant();
	}
}

//-----------------------------------------------------------------------------

technique TwoTextureAddTech
{
	Pass P0
	{
		PixelShader = compile ps_2_0 addTwoTextures();
	}
}

//-----------------------------------------------------------------------------

technique FourTextureAddTech
{
	Pass P0
	{
		PixelShader = compile ps_2_0 addFourTextures();
	}
}

//-----------------------------------------------------------------------------

technique EightTextureAddTech
{
	Pass P0
	{
		VertexShader = NULL;
		PixelShader = compile ps_2_0 addEightTextures();
	}
}

//-----------------------------------------------------------------------------


technique ApplyLightTech
{
	Pass P0
	{
		PixelShader = compile ps_2_0 applyLight();
	}
}

//-----------------------------------------------------------------------------

technique InfTestTech
{
	Pass P0
	{
		PixelShader = compile ps_2_0 createInfTex();
	}
	Pass P1
	{
		PixelShader = compile ps_2_0 detectInfTex();
	}
}

//-----------------------------------------------------------------------------

technique NaNTestTech
{
	Pass P0
	{
		PixelShader = compile ps_2_0 createNaNTex();
	}
	Pass P1
	{
		PixelShader = compile ps_2_0 detectNaNTex();
	}
}


