// FILE:        Consistency.cpp
// DESC:        Consistency test group class methods for D3D caps consistency tests
// AUTHOR:      Robert Rodrigues

#include "CapsCheck.h"
#include "BitMasks.h"

//X=Value being tested, Y=Value it should equal, Z=Name of the value
#define VALUE_TEST( x, y ) \
    if( ! ( x ) )\
	{\
		bRetVal = false;\
		WriteToLog( "DX version: %#0x\nUnsupported value: %s\n\n", m_dwVersion, y );\
	}\

ConsistencyTestGroup::ConsistencyTestGroup()
{
	m_szTestName = "Caps Consistency Test";
	m_szCommandKey = "Consistency";
}
bool ConsistencyTestGroup::ExecuteTest( UINT uTest )
{
	m_bFail = false;
	m_bWarn = false;
	m_bUnidentified = false;

	BeginTestCase(m_szTestName,uTest);

	switch ( m_dwVersion )
	{
		case 0x600:
			m_bFail = !DX6Consistency();
			break;
		case 0x700:
			m_bFail = !DX7Consistency();
			break;
		case 0x800:
			m_bFail = !DX8Consistency();
			break;
		case 0x900:
			m_bFail = !DX9Consistency();
			break;
	}

	if ( !LineCheck() )
	{
		m_bFail = true;
	}

	if ( !AnisotropyCheck() )
	{
		m_bFail = true;
	}

	if ( !CubeCheck() )
	{
		m_bFail = true;
	}

	if ( !FilterCheck() )
	{
		m_bFail = true;
	}

	if ( !FogCheck() )
	{
		m_bFail = true;
	}

	if ( !MipVolumeCheck() || !TextureCheck() || !VolumeCheck() )
	{
		m_bFail = true;
	}

	if ( !MultisampleStencilCheck() || !StencilCheck() )
	{
		m_bFail = true;
	}

	if ( !Pow2Check() )
	{
		m_bFail = true;
	}

	if ( !PixelShaderCheck() || !VertexShaderCheck() )
	{
		m_bFail = true;
	}

	if ( m_pDeviceDesc->wMaxSimultaneousTextures > 8 )
	{
        WriteToLog( "Device capability inconsistency: MaxSimultaneousTextures (%d) cannot be greater than 8.\n\n", m_pDeviceDesc->wMaxSimultaneousTextures );
        m_bFail = true; 
	}

	if ( m_bUnidentified )
		m_bFail = true;

	return CCaps::ExecuteTest( uTest );
}
bool ConsistencyTestGroup::DX6Consistency()
{
	bool bRetVal = true;
	DWORD dwCurrentCompare;

	MASK_WARN( m_pDeviceDesc->dwDevCaps, C_DX6_DEVMASK, "DevCaps", CT_Dev )
	MASK_WARN( m_pDeviceDesc->dwFVFCaps, C_DX6_FVFMASK, "FVFCaps", CT_FVF )
	MASK_WARN( m_pDeviceDesc->dwStencilCaps, C_DX6_STENCILMASK, "StencilCaps", CT_Stencil )
	MASK_WARN( m_pDeviceDesc->dwTextureOpCaps, C_DX6_TEXTUREOPMASK, "TextureOpCaps", CT_TextureOp )

	MASK_WARN( m_pDeviceDesc->dwTransformCaps, C_DX6_TRANSFORMMASK, "TransformCaps", CT_Transform )
	MASK_WARN( m_pDeviceDesc->dwColorModel, C_DX6_COLORMODELMASK, "ColorModel", CT_ColorModel )
	MASK_WARN( m_pDeviceDesc->dwFlags, C_DX6_FLAGMASK, "Flags", CT_Flag )
	MASK_WARN( m_pDeviceDesc->dwLightingCaps, C_DX6_LIGHTMASK, "LightingCaps", CT_Light )
	MASK_WARN( m_pDeviceDesc->dwLightingModel, C_DX6_LIGHTMODELMASK, "LightingModel", CT_LightModel )

	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwAlphaCmpCaps, C_DX6_ALPHACMPMASK, "dpcLineCaps.dwAlphaCmpCaps", CT_AlphaZCmp )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwDestBlendCaps, C_DX6_DESTBLENDMASK, "dpcLineCaps.dwDestBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwMiscCaps, C_DX6_MISCPRIMMASK, "dpcLineCaps.dwMiscCaps", CT_MiscPrimitive )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwRasterCaps, C_DX6_RASTERMASK, "dpcLineCaps.dwRasterCaps", CT_Raster )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwShadeCaps, C_DX6_SHADEMASK, "dpcLineCaps.dwShadeCaps", CT_Shade )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwSrcBlendCaps, C_DX6_DESTBLENDMASK, "dpcLineCaps.dwSrcBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwTextureAddressCaps, C_DX6_TEXTUREADDRESSMASK, "dpcLineCaps.dwTextureAddressCaps", CT_TextureAddress )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwTextureBlendCaps, C_DX6_TEXTUREBLENDMASK, "dpcLineCaps.dwTextureBlendCaps", CT_TextureBlend )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwTextureCaps, C_DX6_TEXTUREMASK, "dpcLineCaps.dwTextureCaps", CT_Texture )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps, C_DX6_TEXTUREFILTERMASK, "dpcLineCaps.dwTextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwZCmpCaps, C_DX6_ALPHACMPMASK, "dpcLineCaps.dwZCmpCaps", CT_AlphaZCmp )

	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwAlphaCmpCaps, C_DX6_ALPHACMPMASK, "dpcTriCaps.dwAlphaCmpCaps", CT_AlphaZCmp )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwDestBlendCaps, C_DX6_DESTBLENDMASK, "dpcTriCaps.dwDestBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwMiscCaps, C_DX6_MISCPRIMMASK, "dpcTriCaps.dwMiscCaps", CT_MiscPrimitive )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwRasterCaps, C_DX6_RASTERMASK, "dpcTriCaps.dwRasterCaps", CT_Raster )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwShadeCaps, C_DX6_SHADEMASK, "dpcTriCaps.dwShadeCaps", CT_Shade )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, C_DX6_DESTBLENDMASK, "dpcTriCaps.dwSrcBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwTextureAddressCaps, C_DX6_TEXTUREADDRESSMASK, "dpcTriCaps.dwTextureAddressCaps", CT_TextureAddress )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwTextureBlendCaps, C_DX6_TEXTUREBLENDMASK, "dpcTriCaps.dwTextureBlendCaps", CT_TextureBlend )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwTextureCaps, C_DX6_TEXTUREMASK, "dpcTriCaps.dwTextureCaps", CT_Texture )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps, C_DX6_TEXTUREFILTERMASK, "dpcTriCaps.dwTextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwZCmpCaps, C_DX6_ALPHACMPMASK, "dpcTriCaps.dwZCmpCaps", CT_AlphaZCmp )

	VALUE_TEST( m_pDeviceDesc->dpcLineCaps.dwStippleHeight <= 32, "dpcLineCaps.StippleHeight is greater than 32." )
	VALUE_TEST( m_pDeviceDesc->dpcTriCaps.dwStippleHeight <= 32, "dpcTriCaps.StippleHeight is greater than 32." )
	VALUE_TEST( m_pDeviceDesc->dpcLineCaps.dwStippleWidth <= 32, "dpcLineCaps.StippleWidth is greater than 32." )
	VALUE_TEST( m_pDeviceDesc->dpcTriCaps.dwStippleWidth <= 32, "dpcTriCaps.StippleWidth is greater than 32." )
    
	VALUE_TEST( m_pDeviceDesc->dwMaxVertexIndex <= 65536, "dwMaxVertexCount is greater than 65,536." )

	MASK_WARN( m_pDeviceDesc->dwDeviceRenderBitDepth, C_DX6_BITDEPTH, "dwDeviceRenderBitDepth", CT_BitDepth )
	MASK_WARN( m_pDeviceDesc->dwDeviceZBufferBitDepth, C_DX6_BITDEPTH, "dwDeviceZBufferBitDepth", CT_BitDepth )

	return bRetVal;
}
bool ConsistencyTestGroup::DX7Consistency()
{
	bool bRetVal = true;
	DWORD dwCurrentCompare;

	MASK_WARN( m_pDeviceDesc->dwDevCaps, C_DX7_DEVMASK, "DevCaps", CT_Dev )
	MASK_WARN( m_pDeviceDesc->dwFVFCaps, C_DX7_FVFMASK, "FVFCaps", CT_FVF )
	MASK_WARN( m_pDeviceDesc->dwStencilCaps, C_DX7_STENCILMASK, "StencilCaps", CT_Stencil )
	MASK_WARN( m_pDeviceDesc->dwTextureOpCaps, C_DX7_TEXTUREOPMASK, "TextureOpCaps", CT_TextureOp )
	MASK_WARN( m_pDeviceDesc->dwVertexProcessingCaps, C_DX7_VERTEXPROCESSMASK, "VertexProcessingCaps", CT_VertexProcess )

	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwAlphaCmpCaps, C_DX7_ALPHACMPMASK, "dpcLineCaps.dwAlphaCmpCaps", CT_AlphaZCmp )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwDestBlendCaps, C_DX7_DESTBLENDMASK, "dpcLineCaps.dwDestBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwMiscCaps, C_DX7_MISCPRIMMASK, "dpcLineCaps.dwMiscCaps", CT_MiscPrimitive )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwRasterCaps, C_DX7_RASTERMASK, "dpcLineCaps.dwRasterCaps", CT_Raster )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwShadeCaps, C_DX7_SHADEMASK, "dpcLineCaps.dwShadeCaps", CT_Shade )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwSrcBlendCaps, C_DX7_DESTBLENDMASK, "dpcLineCaps.dwSrcBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwTextureAddressCaps, C_DX7_TEXTUREADDRESSMASK, "dpcLineCaps.dwTextureAddressCaps", CT_TextureAddress )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwTextureBlendCaps, C_DX7_TEXTUREBLENDMASK, "dpcLineCaps.dwTextureBlendCaps", CT_TextureBlend )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwTextureCaps, C_DX7_TEXTUREMASK, "dpcLineCaps.dwTextureCaps",CT_Texture )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps, C_DX7_TEXTUREFILTERMASK, "dpcLineCaps.dwTextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwZCmpCaps, C_DX7_ALPHACMPMASK, "dpcLineCaps.dwZCmpCaps", CT_AlphaZCmp )

	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwAlphaCmpCaps, C_DX7_ALPHACMPMASK, "dpcTriCaps.dwAlphaCmpCaps", CT_AlphaZCmp )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwDestBlendCaps, C_DX7_DESTBLENDMASK, "dpcTriCaps.dwDestBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwMiscCaps, C_DX7_MISCPRIMMASK, "dpcTriCaps.dwMiscCaps", CT_MiscPrimitive )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwRasterCaps, C_DX7_RASTERMASK, "dpcTriCaps.dwRasterCaps", CT_Raster )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwShadeCaps, C_DX7_SHADEMASK, "dpcTriCaps.dwShadeCaps", CT_Shade )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, C_DX7_DESTBLENDMASK, "dpcTriCaps.dwSrcBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwTextureAddressCaps, C_DX7_TEXTUREADDRESSMASK, "dpcTriCaps.dwTextureAddressCaps", CT_TextureAddress )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwTextureBlendCaps, C_DX7_TEXTUREBLENDMASK, "dpcTriCaps.dwTextureBlendCaps", CT_TextureBlend )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwTextureCaps, C_DX7_TEXTUREMASK, "dpcTriCaps.dwTextureCaps", CT_Texture )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps, C_DX7_TEXTUREFILTERMASK, "dpcTriCaps.dwTextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwZCmpCaps, C_DX7_ALPHACMPMASK, "dpcTriCaps.dwZCmpCaps", CT_AlphaZCmp )

	MASK_WARN( m_pDeviceDesc->dwDeviceRenderBitDepth, C_DX7_BITDEPTH, "dwDeviceRenderBitDepth", CT_BitDepth )
	MASK_WARN( m_pDeviceDesc->dwDeviceZBufferBitDepth, C_DX7_BITDEPTH, "dwDeviceZBufferBitDepth", CT_BitDepth )

	VALUE_TEST( m_pDeviceDesc->dpcLineCaps.dwStippleHeight <= 32, "dpcLineCaps.StippleHeight is greater than 32." )
	VALUE_TEST( m_pDeviceDesc->dpcTriCaps.dwStippleHeight <= 32, "dpcTriCaps.StippleHeight is greater than 32." )
	VALUE_TEST( m_pDeviceDesc->dpcLineCaps.dwStippleWidth <= 32, "dpcLineCaps.StippleWidth is greater than 32." )
	VALUE_TEST( m_pDeviceDesc->dpcTriCaps.dwStippleWidth <= 32, "dpcTriCaps.StippleWidth is greater than 32." )

	VALUE_TEST( m_pDeviceDesc->wMaxUserClipPlanes <= D3DMAXUSERCLIPPLANES, "MaxUserClipPlanes must have a value no more than D3DMAXUSERCLIPPLANES" )

	VALUE_TEST( m_pDeviceDesc->dwReserved1 == 0, "Reserved1 contains a value." )
	VALUE_TEST( m_pDeviceDesc->dwReserved2 == 0, "Reserved2 contains a value." )
	VALUE_TEST( m_pDeviceDesc->dwReserved3 == 0, "Reserved3 contains a value." )
	VALUE_TEST( m_pDeviceDesc->dwReserved4 == 0, "Reserved4 contains a value." )

	return bRetVal;
}
bool ConsistencyTestGroup::DX8Consistency()
{
	bool bRetVal = true;
	DWORD dwCurrentCompare;

	MASK_WARN( m_pDeviceDesc->dwAlphaCmpCaps, C_DX8_ALPHACMPMASK, "AlphaCmpCaps", CT_AlphaZCmp )
	MASK_WARN( m_pDeviceDesc->dwCaps, C_DX8_CAPSMASK, "Caps", CT_Caps )
	MASK_WARN( m_pDeviceDesc->dwCaps2, C_DX8_CAPS2MASK, "Caps2", CT_Caps2 )
	MASK_WARN( m_pDeviceDesc->dwCaps3, C_DX8_CAPS3MASK, "Caps3", CT_Caps3 )
	MASK_WARN( m_pDeviceDesc->dwCubeTextureFilterCaps, C_DX8_TEXTUREFILTERMASK, "CubeTextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dwCursorCaps, C_DX8_CURSORMASK, "CursorCaps", CT_Cursor )
	MASK_WARN( m_pDeviceDesc->dwDestBlendCaps, C_DX8_DESTBLENDMASK, "DestBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dwDevCaps, C_DX8_DEVMASK, "DevCaps", CT_Dev )
	MASK_WARN( m_pDeviceDesc->dwFVFCaps, C_DX8_FVFMASK, "FVFCaps", CT_FVF )
	MASK_WARN( m_pDeviceDesc->dwLineCaps, C_DX8_LINEMASK, "LineCaps", CT_Line )
	MASK_WARN( m_pDeviceDesc->dwPresentationIntervals, C_DX8_PRESENTMASK, "PresentationIntervals", CT_Present )
	MASK_WARN( m_pDeviceDesc->dwPrimitiveMiscCaps, C_DX8_MISCPRIMMASK, "PrimitiveMiscCaps", CT_MiscPrimitive )
	MASK_WARN( m_pDeviceDesc->dwRasterCaps, C_DX8_RASTERMASK, "RasterCaps", CT_Raster )
	MASK_WARN( m_pDeviceDesc->dwShadeCaps, C_DX8_SHADEMASK, "ShadeCaps", CT_Shade )
	MASK_WARN( m_pDeviceDesc->dwSrcBlendCaps, C_DX8_DESTBLENDMASK, "SrcBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dwStencilCaps, C_DX8_STENCILMASK, "StencilCaps", CT_Stencil )
	MASK_WARN( m_pDeviceDesc->dwTextureAddressCaps, C_DX8_TEXTUREADDRESSMASK, "TextureAddressCaps", CT_TextureAddress )
	MASK_WARN( m_pDeviceDesc->dwTextureCaps, C_DX8_TEXTUREMASK, "TextureCaps", CT_Texture )
	MASK_WARN( m_pDeviceDesc->dwTextureFilterCaps, C_DX8_TEXTUREFILTERMASK, "TextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dwTextureOpCaps, C_DX8_TEXTUREOPMASK, "TextureOpCaps", CT_TextureOp )
	MASK_WARN( m_pDeviceDesc->dwVertexProcessingCaps, C_DX8_VERTEXPROCESSMASK, "VertexProcessingCaps", CT_VertexProcess )
	MASK_WARN( m_pDeviceDesc->dwVertexTextureFilterCaps, C_DX8_TEXTUREFILTERMASK, "VertexTextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dwVolumeTextureAddressCaps, C_DX8_TEXTUREADDRESSMASK, "VolumeTextureAddressCaps", CT_TextureAddress )
	MASK_WARN( m_pDeviceDesc->dwVolumeTextureFilterCaps, C_DX8_TEXTUREFILTERMASK, "VolumeTextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dwZCmpCaps, C_DX8_ALPHACMPMASK, "ZCmpCaps", CT_AlphaZCmp )

	VALUE_TEST( m_pDeviceDesc->wMaxUserClipPlanes <= D3DMAXUSERCLIPPLANES, "MaxUserClipPlanes must have a value no more than D3DMAXUSERCLIPPLANES" )
	VALUE_TEST( m_pDeviceDesc->dvMaxPointSize >= 1.0f, "MaxPointSize must be at least 1.0f" )
	VALUE_TEST( m_pDeviceDesc->dwMaxStreams >= 1 && 16 >= m_pDeviceDesc->dwMaxStreams, "MaxStreams must be at least 1, and no more than 16." )

	return bRetVal;
}
bool ConsistencyTestGroup::DX9Consistency()
{
	bool bRetVal = true;
	DWORD dwCurrentCompare; 

	//fail incorrect stretch rect values
	MASK_TEST( m_pDeviceDesc->dwStretchRectFilterCaps, C_DX9_STRETCHFILTERMASK, "StretchFilterCaps", CT_TextureFilter )

	//Give warnings for the rest.
	MASK_WARN( m_pDeviceDesc->dwAlphaCmpCaps, C_DX9_ALPHACMPMASK, "AlphaCmpCaps", CT_AlphaZCmp )
	MASK_WARN( m_pDeviceDesc->dwCaps, C_DX9_CAPSMASK, "Caps", CT_Caps )
	
	//Special case for 9/9.1 differences
	MASK_WARN( m_pDeviceDesc->dwCaps2, (m_pD3D->GetVersion() >= 9.1f)?C_DX91_CAPS2MASK:C_DX9_CAPS2MASK, "Caps2", CT_Caps2 )
	///////////////////////////////////////////

	MASK_WARN( m_pDeviceDesc->dwCaps3, C_DX9_CAPS3MASK, "Caps3", CT_Caps3 )
	MASK_WARN( m_pDeviceDesc->dwCubeTextureFilterCaps, C_DX9_TEXTUREFILTERMASK, "CubeTextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dwCursorCaps, C_DX9_CURSORMASK, "CursorCaps", CT_Cursor )
	MASK_WARN( m_pDeviceDesc->dwDeclTypes, C_DX9_DECLTYPEMASK, "DeclTypes", CT_DeclType )
	MASK_WARN( m_pDeviceDesc->dwDevCaps, C_DX9_DEVMASK, "DevCaps", CT_Dev )
	MASK_WARN( m_pDeviceDesc->dwDevCaps2, C_DX9_DEV2MASK, "DevCaps2", CT_Dev2 )
	MASK_WARN( m_pDeviceDesc->dwFVFCaps, C_DX9_FVFMASK, "FVFCaps", CT_FVF )
	MASK_WARN( m_pDeviceDesc->dwLineCaps, C_DX9_LINEMASK, "LineCaps", CT_Line )
	MASK_WARN( m_pDeviceDesc->dwPresentationIntervals, C_DX9_PRESENTMASK, "PresentationIntervals", CT_Present )
	MASK_WARN( m_pDeviceDesc->dwPrimitiveMiscCaps, C_DX9_MISCPRIMMASK, "PrimitiveMiscCaps", CT_MiscPrimitive )
	MASK_WARN( m_pDeviceDesc->dwRasterCaps, C_DX9_RASTERMASK, "RasterCaps", CT_Raster )
	MASK_WARN( m_pDeviceDesc->dwShadeCaps, C_DX9_SHADEMASK, "ShadeCaps", CT_Shade )

	//Special case for 9/9.1 differences
	MASK_WARN( m_pDeviceDesc->dwDestBlendCaps, (m_pD3D->GetVersion() >= 9.1f)?C_DX91_DESTBLENDMASK:C_DX9_DESTBLENDMASK, "DestBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dwSrcBlendCaps, (m_pD3D->GetVersion() >= 9.1f)?C_DX91_DESTBLENDMASK:C_DX9_DESTBLENDMASK, "SrcBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dwTextureFilterCaps, (m_pD3D->GetVersion() >= 9.1f)?C_DX91_TEXTUREFILTERMASK:C_DX9_TEXTUREFILTERMASK, "TextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dwVertexTextureFilterCaps, (m_pD3D->GetVersion() >= 9.1f)?C_DX91_TEXTUREFILTERMASK:C_DX9_TEXTUREFILTERMASK, "VertexTextureFilterCaps", CT_TextureFilter )
	///////////////////////////////////////////

	MASK_WARN( m_pDeviceDesc->dwStencilCaps, C_DX9_STENCILMASK, "StencilCaps", CT_Stencil )
	MASK_WARN( m_pDeviceDesc->dwTextureAddressCaps, C_DX9_TEXTUREADDRESSMASK, "TextureAddressCaps", CT_TextureAddress )
	MASK_WARN( m_pDeviceDesc->dwTextureCaps, C_DX9_TEXTUREMASK, "TextureCaps", CT_Texture )
	MASK_WARN( m_pDeviceDesc->dwTextureOpCaps, C_DX9_TEXTUREOPMASK, "TextureOpCaps", CT_TextureOp )
	MASK_WARN( m_pDeviceDesc->dwVertexProcessingCaps, C_DX9_VERTEXPROCESSMASK, "VertexProcessingCaps", CT_VertexProcess )
	MASK_WARN( m_pDeviceDesc->dwVolumeTextureAddressCaps, C_DX9_TEXTUREADDRESSMASK, "VolumeTextureAddressCaps", CT_TextureAddress )
	MASK_WARN( m_pDeviceDesc->dwVolumeTextureFilterCaps, C_DX9_TEXTUREFILTERMASK, "VolumeTextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dwZCmpCaps, C_DX9_ALPHACMPMASK, "ZCmpCaps", CT_AlphaZCmp )
	MASK_WARN( m_pDeviceDesc->PS20Caps.Caps, C_DX9_PSHADERCAPSMASK, "PS20Caps.Caps", CT_PixelShader )
	MASK_WARN( m_pDeviceDesc->VS20Caps.Caps, C_DX9_VSHADERCAPSMASK, "VS20Caps.Caps", CT_VertexShader )

	VALUE_TEST( m_pDeviceDesc->dwReserved1 == 0, "Reserved1 contains a value." )
	VALUE_TEST( m_pDeviceDesc->dwReserved2 == 0, "Reserved2 contains a value." )
	VALUE_TEST( m_pDeviceDesc->dwReserved3 == 0, "Reserved3 contains a value." )
	VALUE_TEST( m_pDeviceDesc->dwReserved4 == 0, "Reserved4 contains a value." )
	VALUE_TEST( m_pDeviceDesc->dwReserved5 == 0, "Reserved5 contains a value." )
	
	VALUE_TEST( m_pDeviceDesc->NumSimultaneousRTs >= 1, "NumSimultaneousRTs must be at least 1" )
	VALUE_TEST( m_pDeviceDesc->wMaxUserClipPlanes <= D3DMAXUSERCLIPPLANES, "MaxUserClipPlanes must have a value no more than D3DMAXUSERCLIPPLANES" )
	VALUE_TEST( m_pDeviceDesc->dvMaxPointSize >= 1.0f, "MaxPointSize must be at least 1.0f" )
	VALUE_TEST( m_pDeviceDesc->dwMaxStreams >= 1 && 16 >= m_pDeviceDesc->dwMaxStreams, "MaxStreams must be at least 1, and no more than 16." )
	
	return bRetVal;
}

bool ConsistencyTestGroup::AnisotropyCheck()
{
	//These may need to change with future DX versions
	bool bRetVal = true;
	DWORD dwAnisotropicFilterCaps = D3DPTFILTERCAPS_MINFANISOTROPIC | D3DPTFILTERCAPS_MAGFANISOTROPIC;
	DWORD dwAnisotropicRasterCaps = D3DPRASTERCAPS_ANISOTROPY;

	if ( m_pDeviceDesc->dwTextureFilterCaps & dwAnisotropicFilterCaps )
    {
		if ( !( m_pDeviceDesc->dwRasterCaps & dwAnisotropicRasterCaps )  )
		{
			WriteToLog( "Device capability inconsistency: Anisotropic filters are set without Raster Anisotropy.\n\n" );
			bRetVal = false;
		}
		
		if ( m_pDeviceDesc->dwMaxAnisotropy < 2 )
		{
			WriteToLog( "Device capability inconsistency: MaxAnisotropy value is %d, must be at least 2.\n\n", m_pDeviceDesc->dwMaxAnisotropy );
			bRetVal = false;
		}
    }
	else
	{
		if ( m_pDeviceDesc->dwRasterCaps & dwAnisotropicRasterCaps )
		{
			WriteToLog( "Device capability inconsistency: Raster Anisotropy is set without Anisotropic filter.\n\n" );
			bRetVal = false;
		}
		if ( m_pDeviceDesc->dwMaxAnisotropy != 1 )
		{
			WriteToLog( "Device capability inconsistency: MaxAnisotropy value is %d, must be 1.\n\n", m_pDeviceDesc->dwMaxAnisotropy );
			bRetVal = false;
		}
	}

	return bRetVal;
}

bool ConsistencyTestGroup::FilterCheck()
{
	bool bRetVal = true;

	if ( m_dwVersion < 0x800 )
	{
		//Check LINEAR caps
		if ( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEAR )
		{
			if ( !( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap LINEAR set without MINFLINEAR.(LineCaps)\n\n" );
				bRetVal = false;
			}
			if ( !( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap LINEAR set without MAGFLINEAR.(LineCaps)\n\n" );
				bRetVal = false;
			}
		}
		if ( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEAR )
		{
			if ( !( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap LINEAR set without MINFLINEAR.(TriCaps)\n\n" );
				bRetVal = false;
			}
			if ( !( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap LINEAR set without MAGFLINEAR.(TriCaps)\n\n" );
				bRetVal = false;
			}
		}
		//Check LINEARMIPLINEAR caps
		if ( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEARMIPLINEAR )
		{
			if ( !( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap LINEARMIPLINEAR set without MINFLINEAR.(LineCaps)\n\n" );
				bRetVal = false;
			}
			if ( !( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap LINEARMIPLINEAR set without MIPFLINEAR.(LineCaps)\n\n" );
				bRetVal = false;
			}
		}
		if ( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEARMIPLINEAR )
		{
			if ( !( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap LINEARMIPLINEAR set without MINFLINEAR.(TriCaps)\n\n" );
				bRetVal = false;
			}
			if ( !( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap LINEARMIPLINEAR set without MIPFLINEAR.(TriCaps)\n\n" );
				bRetVal = false;
			}
		}
		//Check LINEARMIPNEAREST caps
		if ( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEARMIPNEAREST )
		{
			if ( !( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap LINEARMIPNEAREST set without MINFPOINT.(LineCaps)\n\n" );
				bRetVal = false;
			}
			if ( !( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap LINEARMIPNEAREST set without MIPFLINEAR.(LineCaps)\n\n" );
				bRetVal = false;
			}
		}
		if ( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEARMIPNEAREST )
		{
			if ( !( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap LINEARMIPNEAREST set without MINFPOINT.(TriCaps)\n\n" );
				bRetVal = false;
			}
			if ( !( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap LINEARMIPNEAREST set without MIPFLINEAR.(TriCaps)\n\n" );
				bRetVal = false;
			}
		}
		//Check MIPLINEAR caps
		if ( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPLINEAR )
		{
			if ( !( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap MIPLINEAR set without MINFLINEAR.(LineCaps)\n\n" );
				bRetVal = false;
			}
			if ( !( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap MIPLINEAR set without MIPFPOINT.(LineCaps)\n\n" );
				bRetVal = false;
			}
		}
		if ( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPLINEAR )
		{
			if ( !( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap MIPLINEAR set without MINFLINEAR.(TriCaps)\n\n" );
				bRetVal = false;
			}
			if ( !( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap MIPLINEAR set without MIPFPOINT.(TriCaps)\n\n" );
				bRetVal = false;
			}
		}
		//Check MIPNEAREST caps
		if ( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPNEAREST )
		{
			if ( !( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap MIPNEAREST set without MINFPOINT.(LineCaps)\n\n" );
				bRetVal = false;
			}
			if ( !( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap MIPNEAREST set without MIPFPOINT.(LineCaps)\n\n" );
				bRetVal = false;
			}
		}
		if ( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPNEAREST )
		{
			if ( !( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap MIPNEAREST set without MINFPOINT.(TriCaps)\n\n" );
				bRetVal = false;
			}
			if ( !( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap MIPNEAREST set without MIPFPOINT.(TriCaps)\n\n" );
				bRetVal = false;
			}
		}
		//Check NEAREST caps
		if ( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_NEAREST )
		{
			if ( !( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap NEAREST set without MINFPOINT.(LineCaps)\n\n" );
				bRetVal = false;
			}
			if ( !( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap NEAREST set without MAGFPOINT.(LineCaps)\n\n" );
				bRetVal = false;
			}
		}
		if ( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_NEAREST )
		{
			if ( !( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap NEAREST set without MINFPOINT.(TriCaps)\n\n" );
				bRetVal = false;
			}
			if ( !( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT ) )
			{
				WriteToLog( "Device capability inconsistency: Filter cap NEAREST set without MAGFPOINT.(TriCaps)\n\n" );
				bRetVal = false;
			}
		}
	}
	else  // Version >= DX8
	{
		if ( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_MIPMAP )
		{
	        if ( !( m_pDeviceDesc->dwTextureFilterCaps & ( D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFLINEAR ) ) )
            {
	    	    WriteToLog( "Device capability inconsistency: Texture cap MIPMAP is set without mip filter caps.\n\n" );
	            bRetVal = false;
            }
	    }
		else if ( m_pDeviceDesc->dwTextureFilterCaps & ( D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFLINEAR ) )
        {
	    	WriteToLog( "Device capability inconsistency: Mip filter caps are set without MIPMAP texture cap.\n\n" );
	        bRetVal = false;
        }
	}
	
	return bRetVal;
}

bool ConsistencyTestGroup::FogCheck()
{
	bool bRetVal = true;

	if (m_dwVersion >= 0x0700)
	{
		if ( !( m_pDeviceDesc->dwRasterCaps & D3DPRASTERCAPS_FOGTABLE ) )
		{
			if ( m_pDeviceDesc->dwRasterCaps & D3DPRASTERCAPS_ZFOG )
			{
	            WriteToLog( "Device capability inconsistency: Z Fog is set without Fog Table.\n\n" );
				bRetVal = false;
			}

			if ( m_pDeviceDesc->dwRasterCaps & D3DPRASTERCAPS_WFOG )
			{
	            WriteToLog( "Device capability inconsistency: W Fog is set without Fog Table.\n\n" );
				bRetVal = false;
			}
		}
		else if ( !( m_pDeviceDesc->dwRasterCaps & D3DPRASTERCAPS_ZFOG ) )
		{
			if ( !( m_pDeviceDesc->dwRasterCaps & D3DPRASTERCAPS_WFOG ) )
			{
	            WriteToLog( "Device capability inconsistency: Fog Table is set without Z Fog or W Fog.\n\n" );
				bRetVal = false;
			}
		}
	}

	return bRetVal;
}
bool ConsistencyTestGroup::MipCubeCheck()
{
	bool bRetVal = true;

	if ( m_dwVersion >= 0x0800 )
	{
		if ( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP )
		{
			// Check if MipVolumes are available and if the device support mipmaplodbias
			if ( !( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP ) ) 
			{
				// Do VolumeTextureFilter cap consistency checking
				if ( ( m_pDeviceDesc->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT ) ||
					( m_pDeviceDesc->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR ) )
				{
	    			WriteToLog( "Device capability inconsistency: Cube Texture mipfilters set without MIPCUBEMAP texture cap.\n\n" );
					bRetVal = false;
				}
			}
			else if ( !( m_pDeviceDesc->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT ) &&
					!( m_pDeviceDesc->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR ) )
			{
	    		WriteToLog( "Device capability inconsistency: MIPCUBEMAP texture cap set without Cube Texture mipfilters.\n\n" );
				bRetVal = false;
			}
		}
		else
		{
			if ( ( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP ) ) 
			{
				WriteToLog( "Device capability inconsistency: MIPCUBEMAP texture cap set without CUBEMAP texture cap.\n\n" );
				bRetVal = false;
			}
			if ( ( m_pDeviceDesc->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT ) ||
					( m_pDeviceDesc->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR ) )
			{
	    		WriteToLog( "Device capability inconsistency: Cube Texture mipfilters set without CUBEMAP texture cap.\n\n" );
				bRetVal = false;
			}
		}
	}
	return bRetVal;
}
bool ConsistencyTestGroup::MipVolumeCheck()
{
	bool bRetVal = true;

	if ( m_dwVersion >= 0x0800 )
	{
		if ( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP )
		{
			// Check if MipVolumes are available and if the device support mipmaplodbias
			if ( !( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP ) ) 
			{
				// Do VolumeTextureFilter cap consistency checking
				if ( ( m_pDeviceDesc->dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT ) ||
					( m_pDeviceDesc->dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR ) )
				{
	    			WriteToLog( "Device capability inconsistency: Volume Texture mipfilters set without MIPVOLUMEMAP texture cap.\n\n" );
					bRetVal = false;
				}
			}
			else if ( !( m_pDeviceDesc->dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT ) &&
					!( m_pDeviceDesc->dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR ) )
			{
	    		WriteToLog( "Device capability inconsistency: MIPVOLUMEMAP texture cap set without Volume Texture mipfilters.\n\n" );
				bRetVal = false;
			}
		}
		else
		{
			if ( ( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP ) ) 
			{
				WriteToLog( "Device capability inconsistency: MIPVOLUMEMAP texture cap set without VOLUMEMAP texture cap.\n\n" );
				bRetVal = false;
			}
			if ( ( m_pDeviceDesc->dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT ) ||
					( m_pDeviceDesc->dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR ) )
			{
	    		WriteToLog( "Device capability inconsistency: Volume Texture mipfilters set without VOLUMEMAP texture cap.\n\n" );
				bRetVal = false;
			}
		}
	}
	return bRetVal;
}
bool ConsistencyTestGroup::CubeCheck()
{
	bool bRetVal = true;

	if ( m_dwVersion >= 0x700 )
	{
		if ( !( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP ) )
		{
			if ( m_uCommonCubeFormats != 0 ) 
			{
  				WriteToLog( "Device capability inconsistency: Found cube texture formats with TextureCap CubeMap not set.\n\n" );
				bRetVal = false;
			}
		}
		else if ( m_uCommonCubeFormats == 0 ) 
		{
			WriteToLog( "TextureCap CubeMap set, but no cube texture formats found.\n\n" );
			bRetVal = false;
		}
	}

		
	return bRetVal;
}

bool ConsistencyTestGroup::TextureCheck()
{
	bool bRetVal = true;
	DWORD dwWidth = m_pDeviceDesc->dwMaxTextureWidth, dwHeight = m_pDeviceDesc->dwMaxTextureHeight;

	if ( m_dwVersion >= 0x0900 )
	{
		HRESULT hr;
		bool bPass;
		if ( m_pDeviceDesc->dwTextureOpCaps & ( D3DTEXOPCAPS_BUMPENVMAP | D3DTEXOPCAPS_BUMPENVMAPLUMINANCE ) )
		{
			bPass = false;

			for ( D3DFORMAT fmt = D3DFMT_R8G8B8;  fmt <= D3DFMT_CxV8U8; fmt = (D3DFORMAT)(fmt + 1) )
			{
				hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), D3DFMT_A2R10G10B10, D3DUSAGE_QUERY_LEGACYBUMPMAP, RTYPE_TEXTURE, fmt );
				if ( SUCCEEDED(hr) )
				{
					bPass = true;
					break;
				}
				hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), D3DFMT_X8R8G8B8, D3DUSAGE_QUERY_LEGACYBUMPMAP, RTYPE_TEXTURE, fmt );
				if ( SUCCEEDED(hr) )
				{
					bPass = true;
					break;
				}
				hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), D3DFMT_X1R5G5B5, D3DUSAGE_QUERY_LEGACYBUMPMAP, RTYPE_TEXTURE, fmt );
				if ( SUCCEEDED(hr) )
				{
					bPass = true;
					break;
				}
				hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), D3DFMT_R5G6B5, D3DUSAGE_QUERY_LEGACYBUMPMAP, RTYPE_TEXTURE, fmt );
				if ( SUCCEEDED(hr) )
				{
					bPass = true;
					break;
				}
			}

			if ( !bPass )
			{
				WriteToLog( "TextureOpCaps indicate support for bumpmaps, but no textures indicate support when queried.\n\n" );
			}
		}
		else
		{
			bPass = true;
			TCHAR szFmt[64];

			for ( D3DFORMAT fmt = D3DFMT_R8G8B8;  fmt <= D3DFMT_CxV8U8; fmt = (D3DFORMAT)(fmt + 1) )
			{
				FmtToString( FORMAT_TEXTURE, &(FORMAT)fmt, szFmt );

				hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), D3DFMT_A2R10G10B10, D3DUSAGE_QUERY_LEGACYBUMPMAP, RTYPE_TEXTURE, fmt );
				if ( SUCCEEDED(hr) )
				{
					bPass = false;					
					WriteToLog( 
						"TextureCaps do not indicate support for bumpmaps, but %s indicates support when queried with an adapter format of D3DFMT_A2R10G10B10.\n\n",
						 szFmt );
				}
				hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), D3DFMT_X8R8G8B8, D3DUSAGE_QUERY_LEGACYBUMPMAP, RTYPE_TEXTURE, fmt );
				if ( SUCCEEDED(hr) )
				{
					bPass = false;
					WriteToLog( 
						"TextureCaps do not indicate support for bumpmaps, but %s indicates support when queried with an adapter format of D3DFMT_X8R8G8B8.\n\n",
						 szFmt );
				}
				hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), D3DFMT_X1R5G5B5, D3DUSAGE_QUERY_LEGACYBUMPMAP, RTYPE_TEXTURE, fmt );
				if ( SUCCEEDED(hr) )
				{
					bPass = false;
					WriteToLog( 
						"TextureCaps do not indicate support for bumpmaps, but %s indicates support when queried with an adapter format of D3DFMT_X1R5G5B5.\n\n",
						 szFmt );
				}
				hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), D3DFMT_R5G6B5, D3DUSAGE_QUERY_LEGACYBUMPMAP, RTYPE_TEXTURE, fmt );
				if ( SUCCEEDED(hr) )
				{
					bPass = false;
					WriteToLog( 
						"TextureCaps do not indicate support for bumpmaps, but %s indicates support when queried with an adapter format of D3DFMT_R5G6B5.\n\n",
						 szFmt );
				}
			}
		}

		bRetVal = bPass;
	}

	if ( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY )
	{		
		if ( dwWidth != dwHeight )
		{
			WriteToLog( "TextureCap SQUAREONLY is set, but MaxTextureHeight and MaxTextureWidth do not match.\n\n" );
			bRetVal = false;
		}
	}

	if ( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_POW2 )
	{
		while ( dwHeight > 2 )
		{					
			if ( 0 != ( dwHeight % 2 ) )
			{
				WriteToLog( "Textures are required to be powers of 2, but MaxTextureHeight is not.\n\n" );
				bRetVal = false;
			}

			dwHeight /= 2;
		}

		while ( dwWidth > 2 )
		{					
			if ( 0 != ( dwWidth % 2 ) )
			{
				WriteToLog( "Textures are required to be powers of 2, but MaxTextureWidth is not.\n\n" );
				bRetVal = false;
			}

			dwWidth /= 2;
		}
	}

	return bRetVal;
}

bool ConsistencyTestGroup::VolumeCheck()
{
	bool bRetVal = true;
	DWORD dwMaxVolume = m_pDeviceDesc->dwMaxVolumeExtent;

	if ( m_dwVersion >= 0x0800 )
	{
		if ( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP )
		{
            if ( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP_POW2 )
			{
				while ( dwMaxVolume > 2 )
				{					
					if ( 0 != ( dwMaxVolume % 2 ) )
					{
						WriteToLog( "Volumes are required to be powers of 2, but MaxVolumeExtent is not.\n\n" );
						bRetVal = false;
					}

					dwMaxVolume /= 2;
				}
			}
			if ( !m_pDeviceDesc->dwVolumeTextureAddressCaps )
			{
				WriteToLog( "Device capability inconsistency: No VolumeTextureAddress caps are set.\n\n" );
				bRetVal = false;
			}
			if ( m_uCommonVolumeFormats == 0 )
			{
  				WriteToLog( "Device capability inconsistency: Found volume texture formats with texture cap VOLUMEMAP not set.\n\n" );
				bRetVal = false;
			}
		}
		else
		{
			if ( m_pDeviceDesc->dwVolumeTextureAddressCaps )
			{
				WriteToLog( "Device capability inconsistency: VolumeTextureAddress caps are set without VOLUMEMAP Texturecap set.\n\n" );
				bRetVal = false;
			}

			if ( m_uCommonVolumeFormats != 0 )
			{
  				WriteToLog( "Device capability inconsistency: Found volume texture formats without texture cap VOLUMEMAP set.\n\n" );
				bRetVal = false;
			}
		}
	}

	
	return bRetVal;
}


bool ConsistencyTestGroup::StencilCheck()
{
	bool bRetVal = true;

	if ( D3DSTENCILCAPS_TWOSIDED & m_pDeviceDesc->dwStencilCaps )
    {
        if ( !( m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps & D3DPMISCCAPS_CULLNONE ) )
        {
            WriteToLog( "D3DPMISCCAPS_CULLNONE not set, required for D3DSTENCILCAPS_TWOSIDED.\n" );
            bRetVal = false;
        }
    }

	return bRetVal;
}

bool ConsistencyTestGroup::VertexShaderCheck()
{
	bool bRetVal = true;

	if ( m_dwVersion < 0x800 )
		return true;

	if ( m_dwVersion >= 0x900 )
	{
		VALUE_TEST( m_pDeviceDesc->MaxVertexShader30InstructionSlots <= 32768, "MaxVertexShader30InstructionSlots must be no more than 32768" )
		if ( (DWORD)D3DVS_VERSION(3, 0) <= m_pDeviceDesc->dwVertexShaderVersion )
		{
			VALUE_TEST( m_pDeviceDesc->MaxVertexShader30InstructionSlots >= 512, "MaxVertexShader30InstructionSlots must be at least 512 if vertex shader 3.0 is supported." )
		}
	}
	else
	{
		if ( (DWORD)D3DVS_VERSION(1, 4) < m_pDeviceDesc->dwVertexShaderVersion )
		{
			WriteToLog( "Vertex shaders higher than version 1.4 are unsupported for this interface version.\n\n" );
			bRetVal = false;
		}
	}

	return bRetVal;
}
bool ConsistencyTestGroup::PixelShaderCheck()
{
	bool bRetVal = true;

	if ( m_dwVersion < 0x800 )
		return true;

	if ( 0xFFFF0000 != m_pDeviceDesc->dwPixelShaderVersion )
	{
		VALUE_TEST( m_pDeviceDesc->dvMaxPixelShaderValue >= 1.0f, "PixelShader1xMaxValue must be at least 1.0f if pixel shaders are supported." )
	}

	if ( m_dwVersion >= 0x900 )
	{
		VALUE_TEST( m_pDeviceDesc->MaxPixelShader30InstructionSlots <= 32768, "MaxPixelShader30InstructionSlots must be no more than 32768" )
		if ( (DWORD)D3DPS_VERSION(3, 0) <= m_pDeviceDesc->dwPixelShaderVersion )
		{
			VALUE_TEST( m_pDeviceDesc->MaxPixelShader30InstructionSlots >= 512, "MaxPixelShader30InstructionSlots must be at least 512 if pixel shader 3.0 is supported." )
		}

		if ( (DWORD)D3DPS_VERSION(2, 0) <= m_pDeviceDesc->dwPixelShaderVersion )
		{
			if ( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_POW2 )
			{
				if ( !( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL ) )
				{
					WriteToLog( "Devices supporting pixel shader 2.0 that set the POW2 texture cap, must also set NONPOW2CONDITIONAL to indicate at least partial support.\n\n" );
					bRetVal = false;
				}
			}
		}
	}
	else
	{
		if ( (DWORD)D3DPS_VERSION(1, 4) < m_pDeviceDesc->dwPixelShaderVersion )
		{
			WriteToLog( "Pixel shaders higher than version 1.4 are unsupported for this interface version.\n\n" );
			bRetVal = false;
		}
	}

	return bRetVal;
}

bool ConsistencyTestGroup::Pow2Check()
{
	bool bRetVal = true;

	if ( !( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_POW2 ) )
	{
		if ( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL )
		{
			WriteToLog( "Device capability inconsistency: Texture cap NONPOW2CONDITIONAL set without POW2.\n\n" );
			bRetVal = false;
		}
	}

	return bRetVal;
}

bool ConsistencyTestGroup::LineCheck()
{
	bool bRetVal = true;

	if ( m_dwVersion < 0x0800 )
	{
		if ( m_pDeviceDesc->dpcLineCaps.dwMiscCaps & D3DPMISCCAPS_LINEPATTERNREP )
		{
			if ( !( m_pDeviceDesc->dpcLineCaps.dwRasterCaps & D3DPRASTERCAPS_PAT ) )
			{
				WriteToLog( "Device capability inconsistency: PrimitiveMisc cap LINEPATTERNREP set without PAT raster cap.\n\n" );
				bRetVal = false;
			}
		}
	}else if ( m_dwVersion < 0x900 )
	{
		if ( m_pDeviceDesc->dwPrimitiveMiscCaps & D3DPMISCCAPS_LINEPATTERNREP )
		{
			if ( !( m_pDeviceDesc->dwRasterCaps & D3DPRASTERCAPS_PAT ) )
			{
				WriteToLog( "Device capability inconsistency: PrimitiveMisc cap LINEPATTERNREP set without PAT raster cap.\n\n" );
				bRetVal = false;
			}
		}
	}

	return bRetVal;
}

bool ConsistencyTestGroup::MultisampleStencilCheck()
{
	bool bRetVal = true;

	if ( m_dwVersion >= 0x0800 )
	{
		MULTISAMPLE_TYPE mType = MULTISAMPLE_NONMASKABLE;

		if ( 0x800 == m_dwVersion )
		{

			mType = (MULTISAMPLE_TYPE)( mType + 1 );
		}

		//Determine if either of these are listed as compatible:
		//FMT_D32F_LOCKABLE	FMT_D16_LOCKABLE
		do
		{
			if ( SUCCEEDED( m_pD3D->CheckDeviceMultiSampleType( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), 
					FMT_D32F_LOCKABLE, !m_pSrcDevice->IsFullscreen(), mType, NULL ) ) ||
				SUCCEEDED( m_pD3D->CheckDeviceMultiSampleType( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), 
					FMT_D16_LOCKABLE, !m_pSrcDevice->IsFullscreen(), mType, NULL ) ) )
			{
				WriteToLog( "Devices must not list lockable zbuffer formats as compatible with multisampling modes.\n\n" );
				bRetVal = false;
			}

			mType = (MULTISAMPLE_TYPE)( mType + 1 );
		}while ( mType <= MULTISAMPLE_16_SAMPLES );
	}

	return bRetVal;
}