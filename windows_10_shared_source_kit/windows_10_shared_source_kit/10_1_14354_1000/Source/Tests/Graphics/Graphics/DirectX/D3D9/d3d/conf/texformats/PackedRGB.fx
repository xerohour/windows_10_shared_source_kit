//-----------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation.  All Rights Reserved.
//
// File:	PackedRGB.fx
// Desc:	HLSL Shaders for PackedRGB texformats test group.
// Author:	Christopher Zamanillo
//-----------------------------------------------------------------------------

texture texPacked;
texture texEncoded;

sampler2D texPackedSampler =	
sampler_state 
{ 
	MagFilter	= Point;
	MinFilter	= Point;
	Texture		= <texPacked>;
};

sampler2D texEncodedSampler =	
sampler_state 
{ 
	MagFilter	= Point;
	MinFilter	= Point;
	AddressU	= Clamp;
	AddressV	= Clamp;
	Texture		= <texEncoded>;
};

float	g_fTolerance = 0.15f;		
float	g_fWidth = 64.f;			// texture width
float	g_fHeight = 64.f;			// texture height
float	g_fPairWidth = 0.03125f;	// width of two texels
float	g_fSingleWidth = 0.015625f; // width of one texel
bool	g_bGreenHiByte = true;		// whether the green component is in the high byte

/***************
 * Compares the color stored in the packed RGB texture with the color
 * we calculate in this shader.  Each 32-bit texel in the ARGB texture
 * contains a pair of 16-bit packed RGB texels.
 ***************/
float4 GetColorPS( float2 texCoords : TEXCOORD0 ) : COLOR
{
	// actual color
	float4 clrPacked = tex2D( texPackedSampler, texCoords );
	
	// encoded data
	float4 clrEnc;
	float2 texCoordEnc = texCoords;

	// are we on an even texel?
	bool bEven = fmod( texCoords.x, g_fPairWidth ) < g_fSingleWidth;
	float2 texelWidth = float2( g_fSingleWidth, 0 );
		
	float4 dataEnc = tex2D( texEncodedSampler, texCoordEnc );
	float4 dataEncRight = tex2D( texEncodedSampler, texCoordEnc + texelWidth );
	
	if( g_bGreenHiByte ) // FMT_G8R8_G8B8
	{
		if( bEven )
		{
			clrEnc.g = dataEnc.b;
			clrEnc.r = ( dataEnc.g + dataEncRight.g ) * 0.5f;
			clrEnc.b = ( dataEnc.a + dataEncRight.a ) * 0.5f;
		}
		else
		{
			clrEnc.g = dataEnc.r;
			clrEnc.r = dataEnc.g;
			clrEnc.b = dataEnc.a;
		}
	}
	else // FMT_R8G8_B8G8
	{
		if( bEven )
		{
			clrEnc.g = dataEnc.g;
			clrEnc.r = ( dataEnc.b + dataEncRight.b ) * 0.5f;
			clrEnc.b = ( dataEnc.r + dataEncRight.r ) * 0.5f;
		}
		else
		{
			clrEnc.g = dataEnc.a;
			clrEnc.r = dataEnc.b;
			clrEnc.b = dataEnc.r;
		}
	}
	
	clrEnc.a = 1.f;
	clrPacked.rgb /= 255.f;
	
	// if the actual and calculated colors are off by more 
	// than the tolerance then kill the pixel...
	if( distance( clrEnc.rgb, clrPacked.rgb ) > g_fTolerance )
		clip( -1 ); 
		
	return clrPacked;
}

technique GetColor
{
	pass p0
	{
		PixelShader = compile ps_2_0 GetColorPS( );
	}
}