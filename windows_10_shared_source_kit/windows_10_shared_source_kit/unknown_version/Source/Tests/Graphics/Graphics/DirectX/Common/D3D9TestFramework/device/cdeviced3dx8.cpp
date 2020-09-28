// ======================================================================================
//
//  CDevice8.cpp
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================

#include "DevicePCH.h"
#include "CDevice.h"
#include "D3DX8.h"

HRESULT CDevice::D3DXCreateMesh8(DWORD NumFaces, DWORD NumVertices, DWORD Options, CONST DWORD* pDeclaration, LPD3DXMESH* ppMesh)
{
	return ::D3DXCreateMesh(NumFaces, NumVertices, Options, pDeclaration, m_pDevice8, ppMesh);
}

HRESULT CDevice::D3DXCreateMeshFVF8(DWORD NumFaces, DWORD NumVertices, DWORD Options, DWORD FVF, LPD3DXMESH* ppMesh)
{
	return ::D3DXCreateMeshFVF(NumFaces, NumVertices, Options, FVF, m_pDevice8, ppMesh);
}

HRESULT CDevice::GenerateTexture8(HRESULT hr, LPDIRECT3DTEXTURE8 pTexture, CTexture** ppTexture, CDevice* pDevice)
{
	if(FAILED(hr))
		return hr;

	CTexture* pct = new CTexture;
	if (pct == NULL)
		return E_FAIL;
	if(pct->Init(pTexture, pDevice) )
	{
		*ppTexture = pct;
	}
	else
	{
		delete pct;
		*ppTexture = NULL;
	}

	RELEASE(pTexture);
	return hr;
}

HRESULT CDevice::D3DXCreateTexture8(UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTexture(m_pDevice8, Width, Height, MipLevels, Usage, Format, Pool, &pTexture);
	return GenerateTexture8(hr, pTexture, ppTexture, this);
}

HRESULT CDevice::D3DXCreateTextureFromFileA8(LPCSTR pSrcFile, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromFileA(m_pDevice8, pSrcFile, &pTexture);
	return GenerateTexture8(hr, pTexture, ppTexture, this);
}

HRESULT CDevice::D3DXCreateTextureFromFileExA8(LPCSTR pSrcFile, UINT Size, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pScrInfo, PALETTEENTRY* pPalette, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromFileExA(m_pDevice8, pSrcFile, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pScrInfo, pPalette, &pTexture);
	return GenerateTexture8(hr, pTexture, ppTexture, this);
}

HRESULT CDevice::D3DXCreateTextureFromFileInMemory8(LPCVOID pSrcData, UINT SrcData, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromFileInMemory(m_pDevice8, pSrcData, SrcData, &pTexture);
	return GenerateTexture8(hr, pTexture, ppTexture, this);
}

HRESULT CDevice::D3DXCreateTextureFromFileInMemoryEx8(LPCVOID pSrcData, UINT SrcData, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pScrInfo, PALETTEENTRY* pPalette, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromFileInMemoryEx(m_pDevice8, pSrcData, SrcData, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pScrInfo, pPalette, &pTexture);
	return GenerateTexture8(hr, pTexture, ppTexture, this);
}

HRESULT CDevice::D3DXCreateTextureFromResourceA8(HMODULE hSrcModule, LPCSTR pSrcResource, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromResourceA(m_pDevice8, hSrcModule, pSrcResource, &pTexture);
	return GenerateTexture8(hr, pTexture, ppTexture, this);
}

HRESULT CDevice::D3DXCreateTextureFromResourceExA8(HMODULE hSrcModule, LPCSTR pSrcResource, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pScrInfo, PALETTEENTRY* pPalette, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromResourceExA(m_pDevice8, hSrcModule, pSrcResource, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pScrInfo, pPalette, &pTexture);
	return GenerateTexture8(hr, pTexture, ppTexture, this);
}

HRESULT	CDevice::D3DXSaveSurfaceToFileA8(LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect)
{
	return ::D3DXSaveSurfaceToFileA( pDestFile, DestFormat, pSrcSurface == NULL ? NULL : pSrcSurface->GetD3DSurface8(), pSrcPalette, pSrcRect );
}

HRESULT CDevice::D3DXLoadSurfaceFromSurface8(CSurface* pDestSurface,CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey)
{
    return ::D3DXLoadSurfaceFromSurface(pDestSurface == NULL ? NULL : pDestSurface->GetD3DSurface8(), pDestPalette, pDestRect, pSrcSurface->GetD3DSurface8(), pSrcPalette, pSrcRect, Filter, ColorKey);
}

HRESULT CDevice::D3DXLoadSurfaceFromFileInMemory8(CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcData, UINT SrcData, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo)
{
	return ::D3DXLoadSurfaceFromFileInMemory(pDestSurface == NULL ? NULL : pDestSurface->GetD3DSurface8(), pDestPalette, pDestRect, pSrcData, SrcData, pSrcRect, Filter, ColorKey, pSrcInfo);
}

HRESULT CDevice::D3DXLoadSurfaceFromMemory8( CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcMemory, D3DFORMAT SrcFormat, UINT SrcPitch, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey)
{
	return ::D3DXLoadSurfaceFromMemory(pDestSurface == NULL ? NULL : pDestSurface->GetD3DSurface8(), pDestPalette, pDestRect, pSrcMemory, SrcFormat, SrcPitch, pSrcPalette, pSrcRect, Filter, ColorKey);
}

HRESULT CDevice::D3DXLoadSurfaceFromFile8(CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCSTR sFilename, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo)
{
	return ::D3DXLoadSurfaceFromFile(pDestSurface == NULL ? NULL : pDestSurface->GetD3DSurface8(), pDestPalette, pDestRect, sFilename, pSrcRect, Filter, ColorKey, pSrcInfo);
}

HRESULT CDevice::D3DXSaveTextureToFileA8(LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, LPBASETEXTURE pSrcTexture, CONST PALETTEENTRY* pSrcPalette)
{
	return ::D3DXSaveTextureToFileA(pDestFile, DestFormat, pSrcTexture == NULL ? NULL : pSrcTexture->GetD3DTexture8(), pSrcPalette);
}

HRESULT CDevice::D3DXLoadVolumeFromVolume8(CVolume* pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST BOX* pDestBox,CVolume* pSrcVolume,CONST PALETTEENTRY* pSrcPalette,CONST BOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey)
{
	return ::D3DXLoadVolumeFromVolume(pDestVolume == NULL ? NULL : pDestVolume->GetD3DVolume8(),pDestPalette,(D3DBOX*)pDestBox,pSrcVolume == NULL ? NULL : pSrcVolume->GetD3DVolume8(),pSrcPalette,(D3DBOX*)pSrcBox,Filter,ColorKey);
}

HRESULT CDevice::D3DXLoadVolumeFromMemory8(CVolume* pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST BOX* pDestBox,LPCVOID  pSrcMemory,FORMAT SrcFormat,UINT SrcRowPitch,UINT SrcSlicePitch,CONST PALETTEENTRY* pSrcPalette,CONST BOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey)
{
	return ::D3DXLoadVolumeFromMemory(NULL == pDestVolume ? NULL : pDestVolume->GetD3DVolume8(),pDestPalette,(D3DBOX*)pDestBox,pSrcMemory,SrcFormat.d3dfFormat,SrcRowPitch,SrcSlicePitch,pSrcPalette,(D3DBOX*)pSrcBox,Filter,ColorKey);
}

HRESULT CDevice::D3DXFillTexture8( LPTEXTURE pTexture, void* pFunction, LPVOID pData)
{
	return ::D3DXFillTexture( pTexture == NULL ? NULL : (LPDIRECT3DTEXTURE8)pTexture->GetD3DTexture8(), (LPD3DXFILL2D)pFunction, pData );
}

HRESULT CDevice::D3DXFilterTexture8(CBaseTexture* pBaseTexture,CONST PALETTEENTRY* pPalette,UINT SrcLevel,DWORD MipFilter)
{
	return ::D3DXFilterTexture( pBaseTexture->GetD3DTexture8(), pPalette, SrcLevel, MipFilter );
}