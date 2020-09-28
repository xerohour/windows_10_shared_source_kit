#include "SurfacePCH.h"

//#define STRICT
#include "d3d.h"
#include "CSurface.h"
#include "CTextures.h"

// -------------------------------------------------------------------------------------

bool CSurface::Init7(void* pSurface)
{
	m_pSurface7 = (LPDIRECTDRAWSURFACE7)pSurface;
	m_pSurface7->AddRef();
	return true;
}

// -------------------------------------------------------------------------------------

HRESULT	CSurface::AddAttachedSurface7(CSurface *pSurf)
{
	if(NULL == pSurf || NULL == m_pSurface7 || NULL == pSurf->m_pSurface7)
		return E_FAIL;

	return m_pSurface7->AddAttachedSurface(pSurf->m_pSurface7);
}

// -------------------------------------------------------------------------------------

HRESULT	CSurface::DeleteAttachedSurface7(CSurface *pSurf)
{
	if(NULL == pSurf || NULL == m_pSurface7 || NULL == pSurf->m_pSurface7)
		return E_FAIL;

	return m_pSurface7->DeleteAttachedSurface(0,pSurf->m_pSurface7);
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::LockRect7(LOCKED_RECT *pLockedRect, CONST RECT* pRect, DWORD Flags)
{
    DDSURFACEDESC2  Desc;
	HRESULT hr = 0;
	DWORD dwFlags = DDLOCK_WAIT;

	if(Flags & LOCK_READONLY)
		dwFlags |= DDLOCK_READONLY;
	if(Flags & LOCK_NOSYSLOCK)
		dwFlags |= DDLOCK_NOSYSLOCK;
	if(Flags & LOCK_DISCARD)
		dwFlags |= DDLOCK_DISCARDCONTENTS;
	if(Flags & LOCK_NOOVERWRITE)
		dwFlags |= DDLOCK_NOOVERWRITE;
   
	ZeroMemory(&Desc,sizeof(DDSURFACEDESC2));
    if (NULL != m_pSurface7)
    {
        Desc.dwSize = sizeof(DDSURFACEDESC2);
        if (FAILED ( hr = m_pSurface7->Lock((RECT*)pRect, &Desc, dwFlags,NULL) ) )
			return hr;
    }    
	pLockedRect->iPitch = Desc.lPitch;
	pLockedRect->pBits = Desc.lpSurface; 

    return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::UnlockRect7(void)
{
	if (NULL != m_pSurface7)
    {
        return m_pSurface7->Unlock(NULL);
    }
	return ERROR;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetDC7(HDC *phDC)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::ReleaseDC7(HDC hDC)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetPalette7(PALETTEENTRY rgPal[256], DWORD &dwNumEntries)
{
	LPDIRECTDRAWPALETTE	pPalette;
	HRESULT hr;
	DWORD dwCaps;

	if ( FAILED ( hr = m_pSurface7->GetPalette(&pPalette) ) )
	{
		return hr;
	}
	if (NULL == pPalette)
	{
		dwNumEntries = 0;
		ZeroMemory(rgPal, sizeof(*rgPal));
		return S_OK;
	}

	if ( FAILED ( hr = pPalette->GetCaps(&dwCaps) ) )
	{
		RELEASE(pPalette);
		return hr;
	}

	switch(dwCaps & (DDPCAPS_1BIT | DDPCAPS_2BIT | DDPCAPS_4BIT | DDPCAPS_8BIT))
	{
	case DDPCAPS_1BIT:
		dwNumEntries = 2;
		break;
	case DDPCAPS_2BIT:
		dwNumEntries = 4;
		break;
	case DDPCAPS_4BIT:
		dwNumEntries = 16;
		break;
	case DDPCAPS_8BIT:
		dwNumEntries = 256;
		break;
	}

	if ( FAILED ( hr = pPalette->GetEntries(0, 0, dwNumEntries, rgPal) ) )
	{
		RELEASE(pPalette);
		return hr;
	}
	RELEASE(pPalette);
	return S_OK;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::UpdateDXTnImage7(CImage *pSrc)
{
	HRESULT hr;
	CSurface *pSurf = NULL;
	CTexture *pTempTex = NULL;
	FORMAT Fmt(FMT_A8R8G8B8);

	hr = m_pDevice->CreateTexture(m_Desc.dwWidth, m_Desc.dwHeight, 1, 0, Fmt, POOL_SYSTEMMEM, &pTempTex);
	if(FAILED(hr))
	{
		DPF(1, _T("CSurface::UpdateDXTnImage7() - CreateTexture failed. hr=%s(%x).\n"), 
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		goto Exit;
	}

	// Load the image into the temp texture
	hr = pTempTex->LoadImage(1, pSrc, TEXLOAD_UPDATETEXTURE);
	if(FAILED(hr))
	{
		DPF(1, _T("CSurface::UpdateDXTnImage7() - LoadImage failed. hr=%s(%x).\n"), 
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		goto Exit;
	}

	// Get the actual surface
    hr = pTempTex->GetSurfaceLevel(0, &pSurf);
    if(FAILED(hr))
	{
		DPF(1, _T("CSurface::UpdateDXTnImage7() - LoadImage failed. hr=%s(%x).\n"), 
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		goto Exit;
	}

	// Copy from the temp surface to the DXTN surface
	hr = m_pDevice->CopyRects(pSurf, NULL, 0, this, NULL);
    if(FAILED(hr))
	{
		DPF(1, _T("CSurface::UpdateDXTnImage7() - LoadImage failed. hr=%s(%x).\n"), 
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		goto Exit;
	}

Exit:
	RELEASE(pSurf);
	RELEASE(pTempTex);

	return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetDesc7(PSURFACEDESC pDesc)
{
	HRESULT hr;
	DDSURFACEDESC2 Desc;
	
	if(pDesc == NULL || m_pSurface7 == NULL)
		return E_FAIL;

    ZeroMemory(pDesc, sizeof(SURFACEDESC));
	Desc.dwSize = sizeof(DDSURFACEDESC2);

	hr = m_pSurface7->GetSurfaceDesc(&Desc);
	if(FAILED(hr))
		return hr;

	pDesc->dwFlags = Desc.dwFlags;
	pDesc->dwWidth = Desc.dwWidth;
	pDesc->dwHeight = Desc.dwHeight;
	pDesc->lPitch = Desc.lPitch;
	pDesc->dwBackBufferCount = Desc.dwBackBufferCount;
	pDesc->dwMipMapCount = Desc.dwMipMapCount;
	pDesc->dwAlphaBitDepth = Desc.dwAlphaBitDepth;
	pDesc->lpSurface = Desc.lpSurface;
	memcpy(&(pDesc->ddckCKDestOverlay), &(Desc.ddckCKDestOverlay), sizeof(DDCOLORKEY));
    memcpy(&(pDesc->ddckCKDestBlt), &(Desc.ddckCKDestBlt), sizeof(DDCOLORKEY));
    memcpy(&(pDesc->ddckCKSrcOverlay), &(Desc.ddckCKSrcOverlay), sizeof(DDCOLORKEY));
    memcpy(&(pDesc->ddckCKSrcBlt), &(Desc.ddckCKSrcBlt), sizeof(DDCOLORKEY));
	memcpy(&(pDesc->Format.ddpfPixelFormat), &(Desc.ddpfPixelFormat), sizeof(DDPIXELFORMAT));
	memcpy(&(pDesc->ddsCaps), &(Desc.ddsCaps), sizeof(DDSCAPS));
	m_pDevice->GetD3D()->DDPFToD3DFmt(&Desc.ddpfPixelFormat, (FMT*)&(pDesc->Format.d3dfFormat), NULL);
	pDesc->Format.ddpfPixelFormat.dwRowsPerByteSequence = 1;

	if (pDesc->Format.ddpfPixelFormat.dwFourCC == MAKEFOURCC('D', 'X', 'T', '1') ||
		pDesc->Format.ddpfPixelFormat.dwFourCC == MAKEFOURCC('D', 'X', 'T', '2') ||
		pDesc->Format.ddpfPixelFormat.dwFourCC == MAKEFOURCC('D', 'X', 'T', '3') ||
		pDesc->Format.ddpfPixelFormat.dwFourCC == MAKEFOURCC('D', 'X', 'T', '4') ||
		pDesc->Format.ddpfPixelFormat.dwFourCC == MAKEFOURCC('D', 'X', 'T', '5') )
	{
		pDesc->Format.ddpfPixelFormat.dwRowsPerByteSequence = 4;
	}

	if(Desc.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY)
		pDesc->Pool = POOL_SYSTEMMEM;
	else if(Desc.ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM)
		pDesc->Pool = POOL_AGP;
	else if(Desc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
		pDesc->Pool = POOL_DEFAULT;
	else if(Desc.ddsCaps.dwCaps2 & DDSCAPS2_TEXTUREMANAGE)
		pDesc->Pool = POOL_MANAGED;

	if(Desc.ddsCaps.dwCaps & DDSCAPS_3DDEVICE)
		pDesc->Usage = USAGE_RENDERTARGET;

	return S_OK;
}

// -------------------------------------------------------------------------------------

bool CSurface::RelSurf7()
{
	RELEASE(m_pSurface7);
	return true;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::UpdateSurfaceImg7(CImage *pSrc)
{
    if (NULL == pSrc)
    {
		DPF(1, _T("CSurface::UpdateSurfaceImg7 - invalid params.\n"));
        return E_INVALIDARG;
    }
    	
	// If DXTN, we need to do a blt, otherwise we'll lock & copy
	if (m_Desc.Format.ddpfPixelFormat.dwFourCC == FOURCC_DXT1 ||
		m_Desc.Format.ddpfPixelFormat.dwFourCC == FOURCC_DXT2 ||
		m_Desc.Format.ddpfPixelFormat.dwFourCC == FOURCC_DXT3 ||
		m_Desc.Format.ddpfPixelFormat.dwFourCC == FOURCC_DXT4 ||
		m_Desc.Format.ddpfPixelFormat.dwFourCC == FOURCC_DXT5)
	{
		return UpdateDXTnImage7(pSrc);				
	}
	else
		return LoadImage(pSrc);
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::UpdateSurfaceFromSurface7(CSurface *pSurf)
{
	HRESULT hr = S_OK;

    if (NULL == pSurf)
    {
		DPF(1, _T("CSurface::UpdateSurfaceFromSurface7 - invalid params.\n"));
        return E_INVALIDARG;
    }
    	
	// If DXTN, we need to do a blt, otherwise we'll lock & copy
	if (m_Desc.Format.ddpfPixelFormat.dwFourCC == FOURCC_DXT1 ||
		m_Desc.Format.ddpfPixelFormat.dwFourCC == FOURCC_DXT2 ||
		m_Desc.Format.ddpfPixelFormat.dwFourCC == FOURCC_DXT3 ||
		m_Desc.Format.ddpfPixelFormat.dwFourCC == FOURCC_DXT4 ||
		m_Desc.Format.ddpfPixelFormat.dwFourCC == FOURCC_DXT5)
	{
		return m_pDevice->CopyRects(pSurf, NULL, 0, this, NULL);
	}
	else
		return UpdateSurfaceFromSurface(pSurf);
}

// -------------------------------------------------------------------------------------

bool CSurface::ColorFill7(DWORD dwRGB, LPRECT pRect)
{
    RECT                rRect;
    DDBLTFX             fx;
    LRESULT             Result;
	DWORD				dwFillColor;

    if (NULL == m_pSurface7)
        return false;

    if (DDERR_SURFACELOST == m_pSurface7->IsLost())
        return false;

	dwFillColor = ColorConvert(dwRGB);

    // Clear the surface to the requested color.
    ZeroMemory(&fx,sizeof(DDBLTFX));
    fx.dwSize = sizeof(DDBLTFX);
    fx.dwFillColor = dwFillColor;

    if (NULL == pRect)
        SetRect(&rRect,0,0,m_Desc.dwWidth,m_Desc.dwHeight);
    else
        memcpy(&rRect,pRect,sizeof(RECT));
    
    Result=m_pSurface7->Blt(&rRect,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL, &fx);

    if (DD_OK != Result)
        return false;

    return true;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetContainer7(REFIID riid, void** ppContainer)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::FreePrivateData7(REFGUID refguid)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetPrivateData7(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::SetPrivateData7(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetDevice7(LPDEVICE *ppDevice)
{
	*ppDevice = m_pDevice; 
	m_pDevice->AddRef();
	return S_OK; 
}