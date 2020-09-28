//-----------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation.  All Rights Reserved.
//
// File:	PackedRGB.cpp
// Desc:	PackedRGB texformats test group (D3DFMT_G8R8_G8B8 and D3DFMT_R8G8_B8G8).
// Author:	Christopher Zamanillo
//-----------------------------------------------------------------------------

#define D3D_OVERLOADS
#include <d3dx9.h>
#include <types.h>

#include "CEffect.h"
#include "texformats.h"

DWORD CPackedRGBTest::dwGradientColors [ ] = 
{
	RGB_MAKE( 255, 0, 0 ),
	RGB_MAKE( 0, 255, 0 ),
	RGB_MAKE( 0, 0, 255 ),
	RGB_MAKE( 255, 255, 255 )
};

DWORD CPackedRGBTest::dwStripes[ ] =
{
	RGB_MAKE( 255, 255, 255),
	RGB_MAKE( 57, 0, 0 ),	
	RGB_MAKE( 255, 0, 0 ),	
	RGB_MAKE( 0, 100, 0 ),	
	RGB_MAKE( 0, 255, 0 ),	
	RGB_MAKE( 0, 0, 203 ),	
	RGB_MAKE( 0, 0, 255 ),
	RGB_MAKE( 35, 35, 35 ),
	RGB_MAKE( 128, 128, 128 )
};

CPackedRGBTest::CPackedRGBTest( )
{
	m_szTestName	= "Packed RGB Texture Formats";
	m_szCommandKey	= "PackedRGB";

	m_Options.D3DOptions.fMinDXVersion = 9.0f;
	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	m_Options.ModeOptions.dwTexInclude = PF_FOURCC;	
	m_Options.ModeOptions.dwTexExclude = PF_METN | PF_DXTN | PF_YUV;

	m_pEffects = NULL;
	m_ppTextures = NULL;
	m_ppEncodedTextures = NULL;

}

bool CPackedRGBTest::CapsCheck( )
{
	m_nTexWidth = 256;
	m_nTexHeight = 256;

	if( m_pSrcDevice != NULL )
	{
		if( m_pSrcDevice->GetCaps( )->dwPixelShaderVersion < D3DPS_VERSION( 2, 0 ) )
		{
			WriteToLog( "Device does not support minimum pixel shader version (ps_2_0).\n" );
			return false;
		}

		if( !( m_pSrcDevice->GetCaps( )->dwTextureCaps & D3DPTEXTURECAPS_POW2 ) )
		{
			m_nTexWidth -= 4;
			m_nTexHeight -= 4;
		}
	}

	if ( m_uCommonTextureFormats == 0 )
	{
		WriteToLog( "CapsCheck() - No Packed RGB formats found\n" );
		return false;
	}

	return true;
}

bool CPackedRGBTest::Setup( )
{
	LPD3DXBUFFER lpErrors = NULL;

	m_nLevels = 0;
	
	if( !CreateEffectFromResource( NULL, "PackedRGB.fx", NULL, NULL, D3DXSHADER_DEBUG, NULL, &m_pEffects, &lpErrors ) )
	{
		WriteToLog( "CreateEffectFromResource() failed on PackedRGB.fx\n" );
		if( lpErrors != NULL )
		{
			WriteToLog( "CreateEffectFromResource() compile errors: %s\n", lpErrors->GetBufferPointer( ) );
		}
		RELEASE( lpErrors );
		return false;
	}

	return CreateTextures( );
}

bool CPackedRGBTest::CreateTextures( )
{
	bool bResult;
	m_ppTextures = new CnTexture*[ 2 ];
	m_ppEncodedTextures = new CnTexture*[ 2 ];
	FORMAT format = m_pCommonTextureFormats[ m_pCurrentMode->nTextureFormat ];
	
	for( int i = 0; i < 2; ++i ) 
	{
		// packed RGB format texture that will contain either gradient or stripes
		bResult = CreateTexture( m_nTexWidth, m_nTexHeight, 0, FMWK_DEFAULT, format, D3DPOOL_MANAGED, &m_ppTextures[ i ] );

		if( !bResult || m_ppTextures[ i ] == NULL )
		{
			WriteToLog( "CreateTexture() failed for packed RGB texture %d: %s\n", i, GLE( GetLastError( ) ) );
			return false;
		}

		if( m_nLevels == 0 )
			m_nLevels = m_ppTextures[ i ]->GetLevelCount( );

		if( i == 0 )
		{
			if( !m_ppTextures[ i ]->LoadGradient( 0, m_nTexWidth, m_nTexHeight, dwGradientColors ) )
			{
				WriteToLog( "CnTexture::LoadGradient() failed: %s\n", GLE( GetLastError() ) );
				return false;
			}
		}
		else
		{
			if ( !m_ppTextures[ i ]->LoadStripes( 0, m_nTexWidth, m_nTexHeight, 9, dwStripes, false, false, dwLoadOption ) )
			{
				WriteToLog( "CnTexture::LoadStripes() failed: %s\n", GLE( GetLastError() ) );
				return false;
			}
		}
		
		// 32-bit texture will hold the raw texture data from the packed RGB format texture.
		// We add one to the original texture's width because we want to ensure that half value is 
		// always rounded up.
		bResult = CreateTexture( (m_nTexWidth + 1) / 2, m_nTexHeight, 0, FMWK_DEFAULT, FMT_A8R8G8B8, D3DPOOL_MANAGED, &m_ppEncodedTextures[ i ] );

		if( !bResult || m_ppEncodedTextures[ i ] == NULL )
		{
			WriteToLog( "CreateTexture() failed for encoded texture %d: %s\n", i, GLE( GetLastError( ) ) );
			return false;
		}

		// go through each mip level and copy the data over to the encoded texture's mip levels
		for( int j = 0; j < m_nLevels; j++ )
		{
			LOCKED_RECT rectSource, rectDest;
			int width = m_nTexWidth >> j;
			int height = m_nTexHeight >> j;

			if( !m_ppTextures[ i ]->LockRect( j, &rectSource, NULL, 0 ) )
			{
				WriteToLog( "CnTexture::LockRect() failed for packed RGB texture %d, mip level %d: %s\n", i, j, GLE( GetLastError( ) ) );
				return false;
			}

			if( !m_ppEncodedTextures[ i ]->LockRect( j, &rectDest, NULL, 0 ) )
			{
				WriteToLog( "CnTexture::LockRect() failed for encoded texture %d, mip level %d: %s\n", i, j, GLE( GetLastError( ) ) );
				return false;
			}
			
			for( int row = 0; row < height; row++ )
			{
				memcpy( (BYTE*) rectDest.pBits + ( row * rectDest.iPitch ),
						(BYTE*) rectSource.pBits + ( row * rectSource.iPitch ),
						sizeof( WORD ) * width );
			}

			if( !m_ppTextures[ i ]->UnlockRect( j ) )
			{
				WriteToLog( "CnTexture::UnlockRect() failed for packed RGB texture %d, mip level %d: %s\n", i, j, GLE( GetLastError( ) ) );
				return false;
			}

			if( !m_ppEncodedTextures[ i ]->UnlockRect( j ) )
			{
				WriteToLog( "CnTexture::UnlockRect() for encoded texture %d, mip level %d: %s\n", i, j, GLE( GetLastError( ) ) );
				return false;
			}
		}
	}

	return true;
}

void CPackedRGBTest::DrawScaledQuad( float nWidth, float nHeight )
{
	VertexList[ 0 ] = D3DTLVERTEX( D3DXVECTOR3( 9.5f,  9.5f, 0.9f ),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
	VertexList[ 1 ] = D3DTLVERTEX( D3DXVECTOR3( 9.5f + nWidth,  9.5f, 0.9f) ,(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,0.0f);
	VertexList[ 2 ] = D3DTLVERTEX( D3DXVECTOR3( 9.5f + nWidth, 9.5f + nHeight, 0.9f ),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,1.0f);
	VertexList[ 3 ] = VertexList[ 0 ];
	VertexList[ 4 ] = VertexList[ 2 ];
	VertexList[ 5 ] = D3DTLVERTEX( D3DXVECTOR3( 9.5f, 9.5f + nHeight, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,1.0f);
}

bool CPackedRGBTest::SetShaderConstants( int nMipLevel, bool bGreenHiByte )
{
	int width = m_nTexWidth;
	int height = m_nTexHeight;
	
	if( nMipLevel >= 0 )
	{
		width >>= nMipLevel;
		height >>= nMipLevel;
	}
		
	if( !m_pEffects->SetBool( "g_bGreenHiByte", bGreenHiByte ) )
	{
		WriteToLog( "CnEffect::SetBool() failed for g_bGreenHiByte: %s\n", GLE( GetLastError() ) );
		return false;
	}

	if( !m_pEffects->SetFloat( "g_fWidth", (float) width ) )
	{
		WriteToLog( "CnEffect::SetFloat() failed for g_fWidth: %s\n", GLE( GetLastError() ) );
		return false;
	}

	if( !m_pEffects->SetFloat( "g_fHeight", (float) height ) )
	{
		WriteToLog( "CnEffect::SetFloat() failed for g_fHeight: %s\n", GLE( GetLastError() ) );
		return false;
	}

	if( !m_pEffects->SetFloat( "g_fSingleWidth", 1.f / width ) )
	{
		WriteToLog( "CnEffect::SetFloat() failed for g_fSingleWidth: %s\n", GLE( GetLastError() ) );
		return false;
	}

	if( !m_pEffects->SetFloat( "g_fPairWidth", 1.f / ( width / 2.f ) ) )
	{
		WriteToLog( "CnEffect::SetFloat() failed for g_fPairWidth: %s\n", GLE( GetLastError() ) );
		return false;
	}

	return true;
}

UINT CPackedRGBTest::TestInitialize( )
{
	SetTestRange( 1, 8 + m_nLevels  );
	
	if ( m_uCommonTextureFormats == 0 )
	{
		WriteToLog( "TestInitialize() - No Packed RGB formats found\n" );
		return D3DTESTINIT_SKIPALL;
	}

	return D3DTESTINIT_RUN;
}

void CPackedRGBTest::SceneRefresh( )
{
	if( !BeginScene( ) )
	{
		WriteToLog( "BeginScene() failed: %s\n", GLE( GetLastError( ) ) );
		return;
	}

	if( !m_pEffects->SetTechniqueByName( "GetColor" ) )
	{
		WriteToLog( "CnEffect::SetTechniqueByName() failed: %s\n", GLE( GetLastError( ) ) );
		return;
	}

	if( !m_pEffects->Begin( NULL, 0 ) )
	{
		WriteToLog( "CnEffect::Begin() failed: %s\n", GLE( GetLastError( ) ) );
		return;
	}

	if( !m_pEffects->BeginPass( 0 ) )
	{
		WriteToLog( "CnEffect::BeginPass() failed: %s\n", GLE( GetLastError( ) ) );
		return;
	}

	if( !RenderPrimitive( D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, VertexList, 6, NULL, 0, NULL ) )
	{
		WriteToLog( "RenderPrimitive() failed: %s\n", GLE( GetLastError( ) ) );
		return;
	}

	if( !m_pEffects->EndPass( ) )
	{
		WriteToLog( "CnEffect::EndPass() failed: %s\n", GLE( GetLastError( ) ) );
		return;
	}

	if( !m_pEffects->End( ) )
	{
		WriteToLog( "CnEffect::End() failed: %s\n", GLE( GetLastError( ) ) );
		return;
	}

	if( !EndScene( ) )
	{
		WriteToLog( "EndScene() failed: %s\n", GLE( GetLastError( ) ) );
		return;
	}
}

bool CPackedRGBTest::ExecuteTest( UINT uTestNum )
{
	TCHAR szBuffer[80];
    FORMAT *pFormat;
 	dwVertexCount = 0;
	msgString[0] = '\0';
  	szStatus[0]  = '\0';
   
	bool bMipTest = uTestNum > 8;
	int nCycle = ( uTestNum - 1 ) / 4; // determine whether we are using gradient or stripes texture
	int nMipLevel = bMipTest ? (uTestNum - 9) : 0;
	
	if( bMipTest )
	{
		nCycle = uTestNum - 1;
		DrawScaledQuad( (float) (m_nTexWidth >> nMipLevel), (float) (m_nTexHeight >> nMipLevel) );
	}
	else
	{
		DrawGrid( ( ( uTestNum - 1 ) % 4 ) + 1 );
	}

	pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

    FmtToString(FORMAT_TEXTURE,pFormat,szBuffer);
    if( bMipTest )
		sprintf( msgString, "Mip Level: %d Tex: %s Size: %dx%d", nMipLevel, szBuffer, m_nTexWidth >> nMipLevel, m_nTexHeight >> nMipLevel );
	else
		sprintf( msgString, "%sTex: %s Size: %dx%d %s", msgString, szBuffer, m_nTexWidth, m_nTexHeight, nCycle ? "Stripes" : "Gradient" );
	
	BeginTestCase(msgString,uTestNum);

	if ( !m_pEffects->SetTexture( "texPacked", m_ppTextures[ bMipTest ? 0 : nCycle ] ) )
	{
		WriteToLog("CnEffect::SetTexture() failed: %s\n", GLE( GetLastError() ) );
        Fail();
		return false;
	}
	
	if ( !m_pEffects->SetTexture( "texEncoded", m_ppEncodedTextures[ bMipTest ? 0 : nCycle ] ) )
	{
		WriteToLog("CnEffect::SetTexture() failed for encoded texture: %s\n", GLE( GetLastError() ) );
        Fail();
		return false;
	}

	if( !SetShaderConstants( nMipLevel, pFormat->d3dfFormat == D3DFMT_R8G8_B8G8 ) )
	{
		Fail( );
		return false;
	}

    return true;
}

void CPackedRGBTest::UpdateStatus( )
{
	CD3DTest::UpdateStatus( );
}

bool CPackedRGBTest::TestTerminate( )
{
	Cleanup( );

	return true;
}

void CPackedRGBTest::Cleanup( )
{
	SetTexture( 0, NULL );
	
	if( m_ppTextures && m_ppEncodedTextures )
	{
		for( int i = 0; i < 2; ++i )
		{
			RELEASE( m_ppTextures[ i ] );
			RELEASE( m_ppEncodedTextures[ i ] );
		}
	}

	RELEASE( m_pEffects );
	SAFEDELETEA( m_ppTextures );
	SAFEDELETEA( m_ppEncodedTextures );
}

