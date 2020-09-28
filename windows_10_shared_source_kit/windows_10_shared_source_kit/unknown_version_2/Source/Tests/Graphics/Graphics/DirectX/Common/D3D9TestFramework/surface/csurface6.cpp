#include "SurfacePCH.h"

#include "d3d.h"
#include "CSurface.h"
#include "CTextures.h"

bool CSurface::Init6(void* pSurface)
{
	m_pSurface4 = (LPDIRECTDRAWSURFACE4)pSurface;
	m_pSurface4->AddRef();
	
	return true;
}

HRESULT	CSurface::AddAttachedSurface6(CSurface *pSurf)
{
	if(NULL == pSurf || NULL == m_pSurface4 || NULL == pSurf->m_pSurface4)
		return E_FAIL;

	return m_pSurface4->AddAttachedSurface(pSurf->m_pSurface4);
}

HRESULT	CSurface::DeleteAttachedSurface6(CSurface *pSurf)
{
	if(NULL == pSurf || NULL == m_pSurface4 || NULL == pSurf->m_pSurface4)
		return E_FAIL;

	return m_pSurface4->DeleteAttachedSurface(0,pSurf->m_pSurface4);
}

HRESULT CSurface::LockRect6(LOCKED_RECT *pLockedRect, CONST RECT* pRect, DWORD Flags)
{
    DDSURFACEDESC2  Desc;
	HRESULT hr = 0;
    DWORD dwFlags = DDLOCK_WAIT;

	if(Flags & LOCK_READONLY)
		dwFlags |= DDLOCK_READONLY;
	if(Flags & LOCK_NOSYSLOCK)
		dwFlags |= DDLOCK_NOSYSLOCK;
   
	ZeroMemory(&Desc,sizeof(DDSURFACEDESC2));
    if (NULL != m_pSurface4)
    {
        Desc.dwSize = sizeof(DDSURFACEDESC2);
        if (FAILED ( hr = m_pSurface4->Lock((RECT*)pRect, &Desc, dwFlags,NULL) ) )
			return hr;
    }    
	pLockedRect->iPitch = Desc.lPitch;
	pLockedRect->pBits = Desc.lpSurface; 
//  ProcessSurfaceDescription(&Desc);
    return hr;
}


HRESULT CSurface::UnlockRect6(void)
{
	if (NULL != m_pSurface4)
    {
        return m_pSurface4->Unlock(NULL);
    }
	return ERROR;
}

HRESULT CSurface::GetDC6(HDC *phDC)
{
	return E_NOTIMPL;
}

HRESULT CSurface::ReleaseDC6(HDC hDC)
{
	return E_NOTIMPL;
}

HRESULT CSurface::GetPalette6(PALETTEENTRY rgPal[256], DWORD &dwNumEntries)
{
	LPDIRECTDRAWPALETTE	pPalette;
	HRESULT hr;
	DWORD dwCaps;

	if ( FAILED ( hr = m_pSurface4->GetPalette(&pPalette) ) )
	{
		RELEASE(pPalette);
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

HRESULT CSurface::GetDesc6(PSURFACEDESC pDesc)
{
	HRESULT hr;
	DDSURFACEDESC2 Desc;
	
	if(pDesc == NULL || m_pSurface4 == NULL)
		return E_FAIL;

    ZeroMemory(pDesc, sizeof(SURFACEDESC));
	Desc.dwSize = sizeof(DDSURFACEDESC2);

	hr = m_pSurface4->GetSurfaceDesc(&Desc);
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

bool CSurface::RelSurf6()
{
	RELEASE(m_pSurface4);
	return true;
}

bool CSurface::ColorFill6(DWORD dwRGB, LPRECT pRect)
{
    RECT                rRect;
    DDBLTFX             fx;
    LRESULT             Result;
	DWORD				dwFillColor;

    if (NULL == m_pSurface4)
        return false;

    if (DDERR_SURFACELOST == m_pSurface4->IsLost())
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
    
    Result=m_pSurface4->Blt(&rRect,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL, &fx);

    if (DD_OK != Result)
        return false;

    return true;
}

HRESULT CSurface::GetContainer6(REFIID riid, void** ppContainer)
{
	return E_NOTIMPL;
}

HRESULT CSurface::FreePrivateData6(REFGUID refguid)
{
	return E_NOTIMPL;
}

HRESULT CSurface::GetPrivateData6(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return E_NOTIMPL;
}

HRESULT CSurface::SetPrivateData6(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags)
{
	return E_NOTIMPL;
}

HRESULT CSurface::GetDevice6(LPDEVICE *ppDevice)
{
	*ppDevice = m_pDevice; 
	m_pDevice->AddRef();
	return S_OK; 
}