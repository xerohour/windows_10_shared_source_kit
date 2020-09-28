// ======================================================================================
//
//  CDevice9.cpp
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================

#include "DevicePCH.h"

#include "CDevice.h"
#include "D3DX9.h"
#include "CD3DX.h"
#include "CEffect.h"

HRESULT CDevice::Wrap(LPDIRECT3DTEXTURE9 pTexture, CTexture** ppTexture)
{
	CTexture* pct = new CTexture;
	if (pct == NULL)
		return E_OUTOFMEMORY;
	if(pct->Init(pTexture, this) )
	{
		*ppTexture = pct;
	}
	else
	{
		delete pct;
		*ppTexture = NULL;
	}
	return S_OK;
}

HRESULT CDevice::Wrap(LPDIRECT3DCUBETEXTURE9 pTexture, CCubeTexture** ppTexture)
{
	CCubeTexture* pct = new CCubeTexture;
	if (pct == NULL)
		return E_OUTOFMEMORY;
	if(pct->Init(pTexture, this) )
	{
		*ppTexture = pct;
	}
	else
	{
		delete pct;
		*ppTexture = NULL;
	}

	return S_OK;
}

HRESULT CDevice::Wrap(LPDIRECT3DVOLUMETEXTURE9 pTexture, CVolTexture** ppTexture)
{
	CVolTexture* pct = new CVolTexture;
	if (pct == NULL)
		return E_OUTOFMEMORY;
	if(pct->Init(pTexture, this) )
	{
		*ppTexture = pct;
	}
	else
	{
		delete pct;
		*ppTexture = NULL;
	}

	return S_OK;
}

HRESULT CDevice::D3DXCreateMesh9(DWORD NumFaces, DWORD NumVertices, DWORD Options, CONST DWORD* pDeclaration, LPD3DXMESH* ppMesh)
{
	return ::D3DXCreateMesh(NumFaces, NumVertices, Options, (D3DVERTEXELEMENT9 *)pDeclaration, m_pDevice9, ppMesh);
}

HRESULT CDevice::D3DXCreateMeshFVF9(DWORD NumFaces, DWORD NumVertices, DWORD Options, DWORD FVF, LPD3DXMESH* ppMesh)
{
	return ::D3DXCreateMeshFVF(NumFaces, NumVertices, Options, FVF, m_pDevice9, ppMesh);
}

HRESULT CDevice::D3DXCreateTexture9(UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, CTexture** ppTexture)
{
	// D3DPOOL_MANAGED no longer available; use POOL_MANAGED_INTERNAL (6) instead
	if ( FLOAT_EQUAL( m_pD3D->GetVersion(), 9.1f ) && POOL_MANAGED == Pool )
		Pool = (D3DPOOL)POOL_MANAGED_INTERNAL;

	LPDIRECT3DTEXTURE9 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTexture(m_pDevice9, Width, Height, MipLevels, Usage, Format, Pool, &pTexture);
	if(FAILED(hr))
		return hr;

	hr = Wrap(pTexture, ppTexture);
	RELEASE(pTexture);
	return hr;
}

HRESULT CDevice::D3DXCreateTextureFromFileA9(LPCSTR pSrcFile, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromFileA(m_pDevice9, pSrcFile, &pTexture);
	if(FAILED(hr))
		return hr;

	hr = Wrap(pTexture, ppTexture);
	RELEASE(pTexture);
	return hr;
}

HRESULT CDevice::D3DXCreateTextureFromFileExA9(LPCSTR pSrcFile, UINT Size, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pScrInfo, PALETTEENTRY* pPalette, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromFileExA(m_pDevice9, pSrcFile, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pScrInfo, pPalette, &pTexture);
	if(FAILED(hr))
		return hr;
	
	hr = Wrap(pTexture, ppTexture);
	RELEASE(pTexture);
	return hr;
}

HRESULT CDevice::D3DXCreateTextureFromFileInMemory9(LPCVOID pSrcData, UINT SrcData, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromFileInMemory(m_pDevice9, pSrcData, SrcData, &pTexture);
	if(FAILED(hr))
		return hr;
	
	hr = Wrap(pTexture, ppTexture);
	RELEASE(pTexture);
	return hr;
}

HRESULT CDevice::D3DXCreateTextureFromFileInMemoryEx9(LPCVOID pSrcData, UINT SrcData, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pScrInfo, PALETTEENTRY* pPalette, CTexture** ppTexture)
{
	// D3DPOOL_MANAGED no longer available; use POOL_MANAGED_INTERNAL (6) instead
	if ( FLOAT_EQUAL( m_pD3D->GetVersion(), 9.1f ) && POOL_MANAGED == Pool )
		Pool = (D3DPOOL)POOL_MANAGED_INTERNAL;

	LPDIRECT3DTEXTURE9 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromFileInMemoryEx(m_pDevice9, pSrcData, SrcData, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pScrInfo, pPalette, &pTexture);
	if(FAILED(hr))
		return hr;
	
	hr = Wrap(pTexture, ppTexture);
	RELEASE(pTexture);
	return hr;
}

HRESULT CDevice::D3DXCreateTextureFromResourceA9(HMODULE hSrcModule, LPCSTR pSrcResource, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromResourceA(m_pDevice9, hSrcModule, pSrcResource, &pTexture);
	if(FAILED(hr))
		return hr;
	
	hr = Wrap(pTexture, ppTexture);
	RELEASE(pTexture);
	return hr;
}

HRESULT CDevice::D3DXCreateTextureFromResourceExA9(HMODULE hSrcModule, LPCSTR pSrcResource, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pScrInfo, PALETTEENTRY* pPalette, CTexture** ppTexture)
{
	// D3DPOOL_MANAGED no longer available; use POOL_MANAGED_INTERNAL (6) instead
	if ( FLOAT_EQUAL( m_pD3D->GetVersion(), 9.1f ) && POOL_MANAGED == Pool )
		Pool = (D3DPOOL)POOL_MANAGED_INTERNAL;

	LPDIRECT3DTEXTURE9 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromResourceExA(m_pDevice9, hSrcModule, pSrcResource, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pScrInfo, pPalette, &pTexture);
	if(FAILED(hr))
		return hr;
	
	hr = Wrap(pTexture, ppTexture);
	RELEASE(pTexture);
	return hr;
}

HRESULT CDevice::D3DXFilterTexture9(CBaseTexture* pBaseTexture,CONST PALETTEENTRY* pPalette,UINT SrcLevel,DWORD MipFilter)
{
	return ::D3DXFilterTexture( pBaseTexture->GetD3DTexture9(), pPalette, SrcLevel, MipFilter );
}

HRESULT CDevice::D3DXLoadVolumeFromMemory9(CVolume* pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST BOX* pDestBox,LPCVOID  pSrcMemory,FORMAT SrcFormat,UINT SrcRowPitch,UINT SrcSlicePitch,CONST PALETTEENTRY* pSrcPalette,CONST BOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey)
{
	return ::D3DXLoadVolumeFromMemory(NULL == pDestVolume ? NULL : pDestVolume->GetD3DVolume9(),pDestPalette,(D3DBOX*)pDestBox,pSrcMemory,SrcFormat.d3dfFormat,SrcRowPitch,SrcSlicePitch,pSrcPalette,(D3DBOX*)pSrcBox,Filter,ColorKey);
}

HRESULT CDevice::D3DXLoadVolumeFromVolume9(CVolume* pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST BOX* pDestBox,CVolume* pSrcVolume,CONST PALETTEENTRY* pSrcPalette,CONST BOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey)
{
	return ::D3DXLoadVolumeFromVolume(pDestVolume == NULL ? NULL : pDestVolume->GetD3DVolume9(),pDestPalette,(D3DBOX*)pDestBox,pSrcVolume == NULL ? NULL : pSrcVolume->GetD3DVolume9(),pSrcPalette,(D3DBOX*)pSrcBox,Filter,ColorKey);
}

HRESULT	CDevice::D3DXSaveSurfaceToFileA9(LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect)
{
	return ::D3DXSaveSurfaceToFileA( pDestFile, DestFormat, pSrcSurface == NULL ? NULL : pSrcSurface->GetD3DSurface9(), pSrcPalette, pSrcRect );
}

HRESULT CDevice::D3DXSaveTextureToFileA9(LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, LPBASETEXTURE pSrcTexture, CONST PALETTEENTRY* pSrcPalette)
{
	return ::D3DXSaveTextureToFileA(pDestFile, DestFormat, pSrcTexture == NULL ? NULL : pSrcTexture->GetD3DTexture9(), pSrcPalette);
}

HRESULT CDevice::D3DXLoadSurfaceFromSurface9(CSurface* pDestSurface,CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey)
{
    return ::D3DXLoadSurfaceFromSurface(pDestSurface == NULL ? NULL : pDestSurface->GetD3DSurface9(), pDestPalette, pDestRect, pSrcSurface->GetD3DSurface9(), pSrcPalette, pSrcRect, Filter, ColorKey);
}

HRESULT CDevice::D3DXLoadSurfaceFromFileInMemory9(CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcData, UINT SrcData, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo)
{
	return ::D3DXLoadSurfaceFromFileInMemory(pDestSurface == NULL ? NULL : pDestSurface->GetD3DSurface9(), pDestPalette, pDestRect, pSrcData, SrcData, pSrcRect, Filter, ColorKey, pSrcInfo);
}

HRESULT CDevice::D3DXLoadSurfaceFromFile9(CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCSTR sFilename, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo)
{
	return ::D3DXLoadSurfaceFromFile(pDestSurface == NULL ? NULL : pDestSurface->GetD3DSurface9(), pDestPalette, pDestRect, sFilename, pSrcRect, Filter, ColorKey, pSrcInfo);
}

HRESULT CDevice::D3DXLoadSurfaceFromMemory9( CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcMemory, D3DFORMAT SrcFormat, UINT SrcPitch, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey)
{
	return ::D3DXLoadSurfaceFromMemory(pDestSurface == NULL ? NULL : pDestSurface->GetD3DSurface9(), pDestPalette, pDestRect, pSrcMemory, SrcFormat, SrcPitch, pSrcPalette, pSrcRect, Filter, ColorKey);
}

HRESULT CDevice::D3DXFillTexture9( LPTEXTURE pTexture, void* pFunction, LPVOID pData)
{
	return ::D3DXFillTexture( pTexture == NULL ? NULL : (LPDIRECT3DTEXTURE9)pTexture->GetD3DTexture9(), (LPD3DXFILL2D)pFunction, pData );
}

HRESULT CDevice::D3DXTessellateRectPatch9( CVertexBuffer* pVertexBuffer, CONST FLOAT* pNumSegs, VERTEXELEMENT* pInDecl, CONST RECTPATCH_INFO* pRectPatchInfo, LPD3DXMESH pMesh )
{
	return ::D3DXTessellateRectPatch( pVertexBuffer == NULL ? NULL : (LPDIRECT3DVERTEXBUFFER9)pVertexBuffer->GetVertexBufferPointer9(), pNumSegs, (CONST D3DVERTEXELEMENT9*)pInDecl, pRectPatchInfo, pMesh );
}

HRESULT CDevice::D3DXTessellateTriPatch9( CVertexBuffer* pVertexBuffer, CONST FLOAT* pNumSegs, VERTEXELEMENT* pInDecl, CONST TRIPATCH_INFO* pRectPatchInfo, LPD3DXMESH pMesh )
{
	return ::D3DXTessellateTriPatch( pVertexBuffer == NULL ? NULL : (LPDIRECT3DVERTEXBUFFER9)pVertexBuffer->GetVertexBufferPointer9(), pNumSegs, (CONST D3DVERTEXELEMENT9*)pInDecl, pRectPatchInfo, pMesh );
}

//NOTE: only D3DX tests should use D3DXEffect functions which operate directly on LPD3DXEFFECT interfaces.
//all other tests should use the wrapper functions and wrapper objects CEffect & CnEffect

HRESULT CDevice::D3DXCreateEffectFromFile9(LPCTSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
{
	return ::D3DXCreateEffectFromFile(m_pDevice9, pSrcFile, pDefines, pInclude, dwFlags, pPool, ppEffect, ppCompilationErrors);
}

HRESULT CDevice::CreateEffectFromFile9(LPCTSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, CEffect** ppEffect, LPD3DXBUFFER* ppCompilationErrors)
{
	LPD3DXEFFECT pDXEffect = NULL;
	HRESULT hr = ::D3DXCreateEffectFromFile (m_pDevice9, pSrcFile, pDefines, pInclude, dwFlags, pPool, &pDXEffect, ppCompilationErrors);
	if(FAILED(hr))
		return hr;

	CEffect* pCEffect = new CEffect;
	if (pCEffect == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	if(pCEffect->Init (pDXEffect, this))
	{
		*ppEffect = pCEffect;
	}
	else
	{
		RELEASE (pCEffect);
		*ppEffect = NULL;
	}

Exit:
	RELEASE(pDXEffect);
	return hr;
}

HRESULT CDevice::D3DXCreateEffectFromResource9(HMODULE hSrcModule, LPCTSTR pSrcresource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
{
	return ::D3DXCreateEffectFromResource(m_pDevice9, hSrcModule, pSrcresource, pDefines, pInclude, dwFlags, pPool, ppEffect, ppCompilationErrors);
}

HRESULT CDevice::CreateEffectFromResource9(HMODULE hSrcModule, LPCTSTR pSrcresource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, CEffect** ppEffect, LPD3DXBUFFER* ppCompilationErrors)
{
	LPD3DXEFFECT pDXEffect = NULL;
	HRESULT hr = ::D3DXCreateEffectFromResource (m_pDevice9, hSrcModule, pSrcresource, pDefines, pInclude, dwFlags, pPool, &pDXEffect, ppCompilationErrors);
	if(FAILED(hr))
		return hr;

	CEffect* pCEffect = new CEffect;
	if (pCEffect == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	if(pCEffect->Init (pDXEffect, this))
	{
		*ppEffect = pCEffect;
	}
	else
	{
		RELEASE (pCEffect);
		*ppEffect = NULL;
	}

Exit:
	RELEASE(pDXEffect);
	return hr;
}

HRESULT CDevice::D3DXEffect_CloneEffect9(LPD3DXEFFECT pEffect, CDevice* pTargetDevice, LPD3DXEFFECT* ppResultEffect)
{
	D3DXHANDLEVERSION(return , ID3DXEffect, pEffect, CloneEffect(pTargetDevice->m_pDevice9, ppResultEffect));
}

HRESULT WINAPI D3DXCreateFontIndirect8And90(LPVOID  pDevice, CONST LOGFONT* pLogFont, LPD3DXFONT* ppFont)
{

	if(g_fD3DX >= 9.09f)
	{
		D3DXFONT_DESCA fd;
		fd.Height =				pLogFont->lfHeight;
		fd.Width =				pLogFont->lfWidth;
		fd.Weight =				pLogFont->lfWeight;
		fd.MipLevels =			1;
		fd.Italic =				pLogFont->lfItalic ? TRUE : FALSE;
		fd.CharSet =			pLogFont->lfCharSet;
		fd.OutputPrecision =	pLogFont->lfOutPrecision;
		fd.Quality =			pLogFont->lfQuality;
		fd.PitchAndFamily =		pLogFont->lfPitchAndFamily;
		_tcsncpy( fd.FaceName, pLogFont->lfFaceName, LF_FACESIZE );

		return ::D3DXCreateFontIndirect((LPDIRECT3DDEVICE9)pDevice, &fd, ppFont);
	}
	else
		return ::D3DXCreateFontIndirect((LPDIRECT3DDEVICE9)pDevice, (D3DXFONT_DESCA*)pLogFont, ppFont);
}