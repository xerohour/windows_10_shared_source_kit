#include "TexturesPCH.h"

#include <d3d.h>
#include "CTextures.h"

// -------------------------------------------------------------------------------------

bool CTexture::Init6(PVOID pTex)
{
	HRESULT hr;
	DDSURFACEDESC2 ddDesc2;
    LPDIRECTDRAWSURFACE4 pTexture = (LPDIRECTDRAWSURFACE4)pTex;

	if(pTexture == NULL)
	{
		DPF(1, _T("CTexture::Init6() - invalid params.\n"));
		return false;
	}

	// Get the surface description
	memset(&ddDesc2, 0, sizeof(DDSURFACEDESC2));
	ddDesc2.dwSize = sizeof(DDSURFACEDESC2);

	hr = pTexture->GetSurfaceDesc(&ddDesc2);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::Init6() - GetSurfaceDesc failed. hr=%s(%x)\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return false;
	}

    if(ddDesc2.dwMipMapCount == 0)
        m_dwLevelCount = 1;
    else
        m_dwLevelCount = ddDesc2.dwMipMapCount;

	m_pSurfaces = new CSurface*[m_dwLevelCount];
	if (NULL == m_pSurfaces)
	{
		DPF(1, _T("CTexture::Init6() - Out of memory.\n"));
		return false;
	}
	memset(m_pSurfaces, 0, sizeof(CSurface*) * m_dwLevelCount);

	// Assign this surface as surface 0
    CSurface *pNew = new CSurface();
    if (NULL == pNew)
	{
		DPF(1, _T("CTexture::Init6() - Out of memory.\n"));
		return false;
	}

	if(!pNew->Init(pTexture, m_pDevice, this))
	{
		DPF(1, _T("CTexture::EnumAttached6() - CSurface::Init failed.\n"));
		SAFEDELETE(pNew);
		return false;
	}
	m_pSurfaces[0] = pNew;

	// Wrap all attached surfaces
    hr = pTexture->EnumAttachedSurfaces(this,EnumAttached6);
	if(FAILED(hr))
    {
		DPF(1, _T("CTexture::Init6() - EnumAttachedSurfaces failed. hr=%s(%x)\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return false;
	}

	m_pTexture6 = pTexture;
	m_pTexture6->AddRef();
	
	return true;
}

// -------------------------------------------------------------------------------------

HRESULT WINAPI CTexture::EnumAttached6(LPDIRECTDRAWSURFACE4 pSurf, LPDDSURFACEDESC2 pDesc, LPVOID pContext)
{
    CTexture *pThis = (CTexture*) pContext;
	CSurface *pNew = NULL;
	int nIndex = 0;
    HRESULT hr;

    pNew = new CSurface();

    if (NULL == pNew)
        return DDENUMRET_CANCEL;

	if(!pNew->Init(pSurf, ((LPBASETEXTURE)pThis)->GetDevice(), pThis))
	{
		DPF(1, _T("CTexture::EnumAttached6() - CSurface::Init failed.\n"));
		SAFEDELETE(pNew);
		return DDENUMRET_CANCEL;
	}

	while(nIndex < pThis->m_dwLevelCount && pThis->m_pSurfaces[nIndex] != NULL)
		nIndex++;

	pThis->m_pSurfaces[nIndex] = pNew;

    // Wrap all attached surfaces
    hr = pSurf->EnumAttachedSurfaces(pThis,EnumAttached6);
	if(FAILED(hr))
    {
		DPF(1, _T("CTexture::EnumAttached6() - EnumAttachedSurfaces failed.\n"));
		return DDENUMRET_CANCEL;
	}

    return DDENUMRET_OK;
}

// -------------------------------------------------------------------------------------

void CTexture::ReleaseD3D6()
{
    for(int i=0; i<m_dwLevelCount; i++)
	{
		RELEASE(m_pSurfaces[i]);
	}
    SAFEDELETE(m_pSurfaces);
    m_dwLevelCount = 0;
	
	RELEASE(m_pTexture6);
	RELEASE(m_pPalette);
    RELEASE(m_pSysTexture);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::AddDirtyRect6(CONST RECT* pDirtyRect)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::SetPalette6(PALETTEENTRY *pPalette)
{
	DWORD dwFlags = 0;
	HRESULT hr;
	PALETTEENTRY *pPal = NULL;
	CSurface *pSurf = NULL;

	// Set the Palette flags
   	if(m_Format.ddpfPixelFormat.dwFlags & PF_PALETTEINDEXED4)
	{
		dwFlags |= DDPCAPS_4BIT;
		pPal = pPalette == NULL ? m_pDevice->m_Palette4 : pPalette;
	}
	else if(m_Format.ddpfPixelFormat.dwFlags & PF_PALETTEINDEXED8)
	{
		dwFlags |= (DDPCAPS_8BIT | DDPCAPS_ALLOW256);
        pPal = pPalette == NULL ? m_pDevice->m_Palette8 : pPalette;
	}
	else
	{
		DPF(1, _T("CTexture::SetPalette6() - Texture format doesn't support palettes.\n"));
		return E_FAIL;
	}

	if(m_Format.ddpfPixelFormat.dwFlags & DDPF_ALPHA)
		dwFlags |= DDPCAPS_ALPHA;

	// Create the palette
	hr = m_pDevice->GetD3D()->m_pDDraw4->CreatePalette(dwFlags, pPal, &m_pPalette, NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::SetPalette6() - CreatePalette failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}

	// Attach the palette to each surface
    hr = GetSurfaceLevel(0, &pSurf);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::SetPalette6() - GetSurfaceLevel failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}
		
	// Set the new palette
    hr = pSurf->GetD3DSurface6()->SetPalette(m_pPalette);
    if(FAILED(hr))
	{
		DPF(1, _T("CTexture::SetPalette6() - SetPalette failed. hr=%s(%x).\n"),
				m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}
    
	RELEASE(pSurf);

	return S_OK;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::SetDDrawPalette6(LPDIRECTDRAWPALETTE pPalette)
{
	HRESULT		hr;
	CSurface	*pSurf = NULL;

	RELEASE(m_pPalette);
	m_pPalette = pPalette;
	m_pPalette->AddRef();

    hr = GetSurfaceLevel(0, &pSurf);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::SetDDrawPalette6() - GetSurfaceLevel failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}
		
	// Set the new palette
    hr = pSurf->GetD3DSurface6()->SetPalette(m_pPalette);
    if(FAILED(hr))
	{
		DPF(1, _T("CTexture::SetDDrawPalette6() - SetPalette failed. hr=%s(%x).\n"),
				m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}
    
	RELEASE(pSurf);

	return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::GetDDrawPalette6(LPDIRECTDRAWPALETTE *ppPalette)
{
	HRESULT		hr;
	CSurface	*pSurf = NULL;

	// Set the new palette
    hr = m_pTexture6->GetPalette(ppPalette);
    if(FAILED(hr))
	{
		DPF(1, _T("CTexture::GetDDrawPalette6() - GetPalette failed. hr=%s(%x).\n"),
				m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}
    
	return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::SetColorKey6(DWORD dwColorKeyFlags, DWORD *pdwColorKey)
{
	HRESULT hr;
    DDCOLORKEY ColorKey;
	CSurface *pSurf = NULL;

    // Set the colorkey value to each surface
    for (int i=0; i < m_dwLevelCount; i++)
	{	
        hr = GetSurfaceLevel(i, &pSurf);
		if(FAILED(hr))
		{
			DPF(1, _T("CTexture::SetColorKey6() - GetSurfaceLevel failed. hr=%s(%x).\n"),
				m_pDevice->GetD3D()->HResultToString(hr), hr);
			return hr;
		}

        if (pdwColorKey != NULL)
        {
            DWORD dwTemp;

            // Convert ARGB into surface specific format
    // MPL
    //        if(IsPalettized())
    //            dwTemp = GetNearestColor(*pdwColorKey);
    //        else
                dwTemp = pSurf->ColorConvert(*pdwColorKey);

            ColorKey.dwColorSpaceLowValue = dwTemp;
            ColorKey.dwColorSpaceHighValue = dwTemp;
        }

		// Set the colorkey
        hr = pSurf->GetD3DSurface6()->SetColorKey(dwColorKeyFlags,(pdwColorKey != NULL) ? &ColorKey : NULL);
        if(FAILED(hr))
		{
			DPF(1, _T("CTexture::SetColorKey6() - SetColorKey failed. hr=%s(%x).\n"),
				m_pDevice->GetD3D()->HResultToString(hr), hr);
            RELEASE(pSurf);
			return hr;
		}

        RELEASE(pSurf);
	}

	return S_OK;
}
