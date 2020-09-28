#include "TexturesPCH.h"

#include <d3d.h>
#include "CTextures.h"

// -------------------------------------------------------------------------------------

bool CTexture::Init7(PVOID pTex)
{
	HRESULT hr;
	DDSURFACEDESC2 ddDesc2;
    LPDIRECTDRAWSURFACE7 pTexture = (LPDIRECTDRAWSURFACE7)pTex;

	if(pTexture == NULL)
	{
		DPF(1, _T("CTexture::Init7() - invalid params.\n"));
		return false;
	}

	// Get the surface description
	memset(&ddDesc2, 0, sizeof(DDSURFACEDESC2));
	ddDesc2.dwSize = sizeof(DDSURFACEDESC2);

	hr = pTexture->GetSurfaceDesc(&ddDesc2);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::Init7() - GetSurfaceDesc failed. hr=%s(%x)\n"),
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
		DPF(1, _T("CTexture::Init7() - Out of memory.\n"));
		return false;
	}
	memset(m_pSurfaces, 0, sizeof(CSurface*) * m_dwLevelCount);

	// Assign this surface as surface 0
    CSurface *pNew = new CSurface();
    if (NULL == pNew)
	{
		DPF(1, _T("CTexture::Init7() - Out of memory.\n"));
		return false;
	}

	if(!pNew->Init(pTexture, m_pDevice, this))
	{
		DPF(1, _T("CTexture::EnumAttached7() - CSurface::Init failed.\n"));
		SAFEDELETE(pNew);
		return false;
	}
	m_pSurfaces[0] = pNew;

	// Wrap all attached surfaces
    hr = pTexture->EnumAttachedSurfaces(this,EnumAttached7);
	if(FAILED(hr))
    {
		DPF(1, _T("CTexture::Init7() - EnumAttachedSurfaces failed. hr=%s(%x)\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return false;
	}

	m_pTexture7 = pTexture;
	m_pTexture7->AddRef();
	
	return true;
}

// -------------------------------------------------------------------------------------

HRESULT WINAPI CTexture::EnumAttached7(LPDIRECTDRAWSURFACE7 pSurf, LPDDSURFACEDESC2 pDesc, LPVOID pContext)
{
    HRESULT hr;
    CTexture *pThis = (CTexture*) pContext;
	CSurface *pNew = NULL;
	int nIndex = 0;

    pNew = new CSurface();

    if (NULL == pNew)
        return DDENUMRET_CANCEL;

	if(!pNew->Init(pSurf, ((LPBASETEXTURE)pThis)->GetDevice(), pThis))
	{
		DPF(1, _T("CTexture::EnumAttached7() - CSurface::Init failed.\n"));
		SAFEDELETE(pNew);
		return DDENUMRET_CANCEL;
	}

	while(nIndex < pThis->m_dwLevelCount && pThis->m_pSurfaces[nIndex] != NULL)
		nIndex++;

	pThis->m_pSurfaces[nIndex] = pNew;

    // Wrap all attached surfaces
    hr = pSurf->EnumAttachedSurfaces(pThis,EnumAttached7);
	if(FAILED(hr))
    {
		DPF(1, _T("CTexture::EnumAttached7() - EnumAttachedSurfaces failed.\n"));
		return DDENUMRET_CANCEL;
	}

    return DDENUMRET_OK;
}

// -------------------------------------------------------------------------------------

void CTexture::ReleaseD3D7()
{
	for(int i=0; i<m_dwLevelCount; i++)
	{
		RELEASE(m_pSurfaces[i]);
	}
	SAFEDELETE(m_pSurfaces);
	m_dwLevelCount = 0;

	RELEASE(m_pTexture7);
	RELEASE(m_pPalette);
    RELEASE(m_pSysTexture);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::AddDirtyRect7(CONST RECT* pDirtyRect)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

DWORD CTexture::SetLOD7(DWORD dwNewLOD)
{
    if (NULL == m_pTexture7)
		return 0;

	DWORD dwPrevLOD;
	HRESULT hr = m_pTexture7->GetLOD(&dwPrevLOD);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::SetLOD7() - GetLOD() failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return 0;
	}

	hr = m_pTexture7->SetLOD(dwNewLOD);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::SetLOD7() - SetLOD() failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return dwPrevLOD;
	}

	return dwPrevLOD;
}

// -------------------------------------------------------------------------------------

DWORD CTexture::GetLOD7()
{
	if (NULL == m_pTexture7)
		return 0;

	DWORD dwLOD;
	HRESULT hr = m_pTexture7->GetLOD(&dwLOD);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::GetLOD7() - GetLOD() failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return 0;
	}

	return dwLOD;
}

// -------------------------------------------------------------------------------------

DWORD CTexture::SetPriority7(DWORD dwNewPriority)
{
	if (NULL == m_pTexture7)
		return 0;

	DWORD dwPrevPriority;
	HRESULT hr = m_pTexture7->GetPriority(&dwPrevPriority);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::SetPriority7() - GetPriority() failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return 0;
	}

	hr = m_pTexture7->SetPriority(dwNewPriority);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::SetPriority7() - SetPriority() failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return dwPrevPriority;
	}

	return dwPrevPriority;
}

// -------------------------------------------------------------------------------------

DWORD CTexture::GetPriority7()
{
    if (NULL == m_pTexture7)
		return 0;

	DWORD dwPriority;
	HRESULT hr = m_pTexture7->GetPriority(&dwPriority);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::GetPriority7() - GetPriority() failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return 0;
	}

	return dwPriority;
}

// -------------------------------------------------------------------------------------

void CTexture::PreLoad7()
{
	if (NULL == m_pTexture7)
		return;
	m_pDevice->m_pDevice7->PreLoad(m_pTexture7);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::SetPalette7(PALETTEENTRY *pPalette)
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
		DPF(1, _T("CTexture::SetPalette7() - Texture format doesn't support palettes.\n"));
		return E_FAIL;
	}

	if(m_Format.ddpfPixelFormat.dwFlags & DDPF_ALPHA)
		dwFlags |= DDPCAPS_ALPHA;

	// Create the palette
	hr = m_pDevice->GetD3D()->m_pDDraw7->CreatePalette(dwFlags, pPal, &m_pPalette, NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::SetPalette7() - CreatePalette failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}

        // Attach the palette to each surface
    hr = GetSurfaceLevel(0, &pSurf);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::SetPalette7() - GetSurfaceLevel failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}
		
	// Set the new palette
    hr = pSurf->GetD3DSurface7()->SetPalette(m_pPalette);
    if(FAILED(hr))
	{
		DPF(1, _T("CTexture::SetPalette7() - SetPalette failed. hr=%s(%x).\n"),
				m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}

	RELEASE(pSurf);

        // Attach the palette to sys mem backing store
        if (m_pSysTexture)
        {
            hr = ((CTexture*)m_pSysTexture)->GetSurfaceLevel(0, &pSurf);
            if(FAILED(hr))
            {
                DPF(1, _T("CTexture::SetPalette7() - GetSurfaceLevel failed for sys mem copy. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
            }
		
            // Set the new palette
            hr = pSurf->GetD3DSurface7()->SetPalette(m_pPalette);
            if(FAILED(hr))
            {
		DPF(1, _T("CTexture::SetPalette7() - SetPalette failed. hr=%s(%x).\n"),
				m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
            }

            RELEASE(pSurf);
        }

	return S_OK;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::SetDDrawPalette7(LPDIRECTDRAWPALETTE pPalette)
{
	HRESULT		hr;
	CSurface	*pSurf = NULL;

	RELEASE(m_pPalette);
	m_pPalette = pPalette;
	m_pPalette->AddRef();

    hr = GetSurfaceLevel(0, &pSurf);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::SetDDrawPalette7() - GetSurfaceLevel failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}
		
	// Set the new palette
    hr = pSurf->GetD3DSurface7()->SetPalette(m_pPalette);
    if(FAILED(hr))
	{
		DPF(1, _T("CTexture::SetDDrawPalette7() - SetPalette failed. hr=%s(%x).\n"),
				m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}
    
	RELEASE(pSurf);

	return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::GetDDrawPalette7(LPDIRECTDRAWPALETTE *ppPalette)
{
	HRESULT		hr;
	CSurface	*pSurf = NULL;

	// Set the new palette
    hr = m_pTexture7->GetPalette(ppPalette);
    if(FAILED(hr))
	{
		DPF(1, _T("CTexture::GetDDrawPalette7() - GetPalette failed. hr=%s(%x).\n"),
				m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}
    
	return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::SetColorKey7(DWORD dwColorKeyFlags, DWORD *pdwColorKey)
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
			DPF(1, _T("CTexture::SetColorKey7() - GetSurfaceLevel failed. hr=%s(%x).\n"),
				m_pDevice->GetD3D()->HResultToString(hr), hr);
			return hr;
		}

        if (pdwColorKey != NULL)
        {
            DWORD dwTemp = pSurf->ColorConvert(*pdwColorKey);
            ColorKey.dwColorSpaceLowValue = dwTemp;
            ColorKey.dwColorSpaceHighValue = dwTemp;
        }

		// Set the colorkey
        hr = pSurf->GetD3DSurface7()->SetColorKey(dwColorKeyFlags,(pdwColorKey != NULL) ? &ColorKey : NULL);
        if(FAILED(hr))
		{
			DPF(1, _T("CTexture::SetColorKey7() - SetColorKey failed. hr=%s(%x).\n"),
				m_pDevice->GetD3D()->HResultToString(hr), hr);
            RELEASE(pSurf);
			return hr;
		}

        RELEASE(pSurf);
	}

	return S_OK;
}
