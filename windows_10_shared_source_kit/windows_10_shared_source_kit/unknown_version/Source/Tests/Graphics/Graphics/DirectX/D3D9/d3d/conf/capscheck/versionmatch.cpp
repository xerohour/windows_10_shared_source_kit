// FILE:        VersionMatch.cpp
// DESC:        VersionMatch test group class methods for D3D caps version matching tests
// AUTHOR:      Robert Rodrigues

#include "CapsCheck.h"
#include "BitMasks.h"

VersionMatchTestGroup::VersionMatchTestGroup()
{
	m_szTestName = "Caps Version Test";
	m_szCommandKey = "Version";
}
bool VersionMatchTestGroup::ExecuteTest( UINT uTest )
{
	m_bFail = false;
	m_bWarn = false;

	BeginTestCase(m_szTestName,uTest);

	switch (m_dwVersion)
	{
		case 0x600:
			m_bFail = !DX6Match();
			break;
		case 0x700:
			m_bFail = !DX7Match();
			break;
		case 0x800:
			m_bFail = !DX8Match();
			break;
		case 0x900:
			m_bFail = !DX9Match();
			break;
	}

	if ( m_bUnidentified )
		m_bFail = true;

	return CCaps::ExecuteTest( uTest ) ;
}
bool VersionMatchTestGroup::DX6Match()
{
	bool bRetVal = true;
	DWORD dwCurrentCompare;

	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwMiscCaps, VM_DX6_MISCPRIMMASK, "dpcLineCaps.MiscCaps", CT_MiscPrimitive )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwMiscCaps, VM_DX6_MISCPRIMMASK, "dpcTriCaps.MiscCaps", CT_MiscPrimitive )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwRasterCaps, VM_DX6_RASTERMASK, "dpcLineCaps.RasterCaps", CT_Raster )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwRasterCaps, VM_DX6_RASTERMASK, "dpcTriCaps.RasterCaps", CT_Raster )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwDestBlendCaps, VM_DX6_DESTBLENDMASK, "dpcLineCaps.DestBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwDestBlendCaps, VM_DX6_DESTBLENDMASK, "dpcTriCaps.DestBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwSrcBlendCaps, VM_DX6_DESTBLENDMASK, "dpcLineCaps.SrcBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, VM_DX6_DESTBLENDMASK, "dpcTriCaps.SrcBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwTextureCaps, VM_DX6_TEXTUREMASK, "dpcLineCaps.TextureCaps", CT_Texture )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwTextureCaps, VM_DX6_TEXTUREMASK, "dpcTriCaps.TextureCaps",CT_Texture )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps, VM_DX6_TEXTUREFILTERMASK, "dpcLineCaps.TextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps, VM_DX6_TEXTUREFILTERMASK, "dpcTriCaps.TextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps. dwTextureAddressCaps, VM_DX6_TEXTUREADDRESSMASK, "dpcLineCaps.TextureAddressCaps", CT_TextureAddress )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps. dwTextureAddressCaps, VM_DX6_TEXTUREADDRESSMASK, "dpcTriCaps.TextureAddressCaps", CT_TextureAddress )

	MASK_WARN( m_pDeviceDesc->dwDevCaps, VM_DX6_DEVMASK, "DevCaps", CT_Dev )
	MASK_WARN( m_pDeviceDesc->dwTextureOpCaps, VM_DX6_TEXTUREOPMASK, "TextureOpCaps", CT_TextureOp )
	MASK_WARN( m_pDeviceDesc->dwStencilCaps, VM_DX6_STENCILMASK, "StencilCaps", CT_Stencil )
	MASK_WARN( m_pDeviceDesc->dwFVFCaps, VM_DX6_FVFMASK, "FVFCaps", CT_FVF )
	
	return bRetVal;
}
bool VersionMatchTestGroup::DX7Match()
{
	bool bRetVal = true;
	DWORD dwCurrentCompare;
	
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwMiscCaps, VM_DX7_MISCPRIMMASK, "dpcLineCaps.MiscCaps", CT_MiscPrimitive )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwMiscCaps, VM_DX7_MISCPRIMMASK, "dpcTriCaps.MiscCaps", CT_MiscPrimitive )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwRasterCaps, VM_DX7_RASTERMASK, "dpcLineCaps.RasterCaps", CT_Raster )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwRasterCaps, VM_DX7_RASTERMASK, "dpcTriCaps.RasterCaps", CT_Raster )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwDestBlendCaps, VM_DX7_DESTBLENDMASK, "dpcLineCaps.DestBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwDestBlendCaps, VM_DX7_DESTBLENDMASK, "dpcTriCaps.DestBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwSrcBlendCaps, VM_DX7_DESTBLENDMASK, "dpcLineCaps.SrcBlendCaps",CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, VM_DX7_DESTBLENDMASK, "dpcTriCaps.SrcBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwTextureCaps, VM_DX7_TEXTUREMASK, "dpcLineCaps.TextureCaps", CT_Texture )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwTextureCaps, VM_DX7_TEXTUREMASK, "dpcTriCaps.TextureCaps", CT_Texture )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps, VM_DX7_TEXTUREFILTERMASK, "dpcLineCaps.TextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps, VM_DX7_TEXTUREFILTERMASK, "dpcTriCaps.TextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dpcLineCaps. dwTextureAddressCaps, VM_DX7_TEXTUREADDRESSMASK, "dpcLineCaps.TextureAddressCaps", CT_TextureAddress )
	MASK_WARN( m_pDeviceDesc->dpcTriCaps. dwTextureAddressCaps, VM_DX7_TEXTUREADDRESSMASK, "dpcTriCaps.TextureAddressCaps", CT_TextureAddress )

	MASK_WARN( m_pDeviceDesc->dwDevCaps, VM_DX7_DEVMASK, "DevCaps", CT_Dev )
	MASK_WARN( m_pDeviceDesc->dwTextureOpCaps, VM_DX7_TEXTUREOPMASK, "TextureOpCaps", CT_TextureOp )
	MASK_WARN( m_pDeviceDesc->dwStencilCaps, VM_DX7_STENCILMASK, "StencilCaps", CT_Stencil )
	MASK_WARN( m_pDeviceDesc->dwFVFCaps, VM_DX7_FVFMASK, "FVFCaps", CT_FVF )
	MASK_WARN( m_pDeviceDesc->dwVertexProcessingCaps, VM_DX7_VERTEXPROCESSMASK, "VertexProcessingCaps", CT_VertexProcess )
	
	return bRetVal;
}
bool VersionMatchTestGroup::DX8Match()
{
	bool bRetVal = true;
	DWORD dwCurrentCompare;

	MASK_WARN( m_pDeviceDesc->dwCaps3, VM_DX8_CAPS3MASK, "Caps3", CT_Caps3 )
	MASK_WARN( m_pDeviceDesc->dwCubeTextureFilterCaps, VM_DX8_TEXTUREFILTERMASK, "CubeTextureFilterCaps",CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dwDestBlendCaps, VM_DX8_DESTBLENDMASK, "DestBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dwLineCaps, VM_DX8_LINEMASK, "LineCaps", CT_Line )
	MASK_WARN( m_pDeviceDesc->dwPrimitiveMiscCaps, VM_DX8_MISCPRIMMASK, "PrimitiveMiscCaps", CT_MiscPrimitive )
	MASK_WARN( m_pDeviceDesc->dwRasterCaps, VM_DX8_RASTERMASK, "RasterCaps", CT_Raster )
	MASK_WARN( m_pDeviceDesc->dwSrcBlendCaps, VM_DX8_DESTBLENDMASK, "SrcBlendCaps", CT_DestSrcBlend )
	MASK_WARN( m_pDeviceDesc->dwStencilCaps, VM_DX8_STENCILMASK, "StencilCaps", CT_Stencil )
	MASK_WARN( m_pDeviceDesc->dwTextureCaps, VM_DX8_TEXTUREMASK, "TextureCaps", CT_Texture )
	MASK_WARN( m_pDeviceDesc->dwTextureFilterCaps, VM_DX8_TEXTUREFILTERMASK, "TextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dwVertexProcessingCaps, VM_DX8_VERTEXPROCESSMASK, "VertexProcessingCaps", CT_VertexProcess )
	MASK_WARN( m_pDeviceDesc->dwVertexTextureFilterCaps, VM_DX8_TEXTUREFILTERMASK, "VertexTextureFilterCaps", CT_TextureFilter )
	MASK_WARN( m_pDeviceDesc->dwVolumeTextureFilterCaps, VM_DX8_TEXTUREFILTERMASK, "VolumeTextureFilterCaps", CT_TextureFilter )
	
	return bRetVal;
}
bool VersionMatchTestGroup::DX9Match()
{
	//Currently there is no way for this to fail.
	bool		bRetVal = true;
	
	return bRetVal;
}