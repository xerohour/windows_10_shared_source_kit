#include "TexturesPCH.h"

#include <d3d.h>
#include "CTextures.h"

// -------------------------------------------------------------------------------------

bool CCubeTexture::Init7(LPVOID pTex)
{
	HRESULT hr;
	DDSURFACEDESC2 ddDesc2;
    LPDIRECTDRAWSURFACE7 pTexture = (LPDIRECTDRAWSURFACE7)pTex;
    CUBEMAP_FACES Face;

	if(pTexture == NULL)
	{
		DPF(1, _T("CCubeTexture::Init7() - invalid params.\n"));
		return false;
	}

	// Get the surface description
	memset(&ddDesc2, 0, sizeof(DDSURFACEDESC2));
	ddDesc2.dwSize = sizeof(DDSURFACEDESC2);

	hr = pTexture->GetSurfaceDesc(&ddDesc2);
	if(FAILED(hr))
	{
		DPF(1, _T("CCubeTexture::Init7() - GetSurfaceDesc failed. hr=%s(%x)\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return false;
	}

    if(ddDesc2.dwMipMapCount == 0)
        m_dwLevelCount = 1;
    else
        m_dwLevelCount = ddDesc2.dwMipMapCount;

	// Allocate the surface array
	m_pSurfaces = new CSurface**[m_dwLevelCount];
	if(NULL == m_pSurfaces)
	{
		DPF(1, _T("CCubeTexture::Init7() - Out of memory.\n"));
		return false;
	}
	
	// Allocate the faces
	for(int i=0; i<m_dwLevelCount; i++)
	{
		m_pSurfaces[i] = new CSurface*[6];
		if (NULL == m_pSurfaces[i])
		{
			DPF(1, _T("CCubeTexture::Init7() - Out of memory.\n"));
			return false;
		}
		memset(m_pSurfaces[i], 0, sizeof(CSurface*) * 6);
	}

	// Assign this surface as surface 0
    CSurface *pNew = new CSurface();
    if (NULL == pNew)
	{
		DPF(1, _T("CCubeTexture::Init7() - Out of memory.\n"));
		return false;
	}

	if(!pNew->Init(pTexture, m_pDevice, this))
	{
		DPF(1, _T("CCubeTexture::EnumAttached7() - CSurface::Init failed.\n"));
		SAFEDELETE(pNew);
		return false;
	}
	
	if(!DescToFace(&ddDesc2, Face))
	{
		DPF(1, _T("CCubeTexture::EnumAttachedCube7() - Enumerated surface is not a valid cube face.\n"));
		SAFEDELETE(pNew);
		return DDENUMRET_CANCEL;
	}

	m_pSurfaces[0][(UINT)Face] = pNew;

	// Wrap all attached surfaces
    hr = pTexture->EnumAttachedSurfaces(this,EnumAttachedCube7);
	if(FAILED(hr))
    {
		DPF(1, _T("CCubeTexture::Init7() - EnumAttachedSurfaces failed. hr=%s(%x)\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return false;
	}

	m_pTexture7 = pTexture;
	m_pTexture7->AddRef();
	
	return true;
}

// -------------------------------------------------------------------------------------

bool CCubeTexture::DescToFace(LPDDSURFACEDESC2 pDesc, CUBEMAP_FACES &Face)
{
	if (pDesc->ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEX)
		Face = CUBEMAP_FACE_POSITIVE_X;
	else if(pDesc->ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEY)
		Face = CUBEMAP_FACE_POSITIVE_Y;
	else if (pDesc->ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEZ)
		Face = CUBEMAP_FACE_POSITIVE_Z;
	else if (pDesc->ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEX)
		Face = CUBEMAP_FACE_NEGATIVE_X;
	else if(pDesc->ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEY)
		Face = CUBEMAP_FACE_NEGATIVE_Y;
	else if(pDesc->ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEZ)
		Face = CUBEMAP_FACE_NEGATIVE_Z;
	else
		return false;

	return true;
}

// -------------------------------------------------------------------------------------

HRESULT WINAPI CCubeTexture::EnumAttachedCube7(LPDIRECTDRAWSURFACE7 pSurf, LPDDSURFACEDESC2 pDesc, LPVOID pContext)
{
    HRESULT hr;
    CCubeTexture *pThis = (CCubeTexture*) pContext;
	CSurface *pNew = NULL;
	UINT nLevel = 0;

    pNew = new CSurface();

    if (NULL == pNew)
        return DDENUMRET_CANCEL;

	if(!pNew->Init(pSurf, ((LPBASETEXTURE)pThis)->GetDevice(), pThis))
	{
		DPF(1, _T("CCubeTexture::EnumAttachedCube7() - CSurface::Init failed.\n"));
		SAFEDELETE(pNew);
		return DDENUMRET_CANCEL;
	}

	// Find the appropriate index
	while(nLevel < pThis->GetLevelCount())
	{
		CUBEMAP_FACES Face;

		if(!pThis->DescToFace(pDesc, Face))
		{
			DPF(1, _T("CCubeTexture::EnumAttachedCube7() - Enumerated surface is not a valid cube face.\n"));
			SAFEDELETE(pNew);
			return DDENUMRET_CANCEL;
		}

		if(pThis->m_pSurfaces[nLevel][(UINT)Face] == NULL)
		{
			pThis->m_pSurfaces[nLevel][(UINT)Face] = pNew;
			break;
		}
		nLevel++;
	}

	// Check that we found a slot
	if(nLevel >= pThis->GetLevelCount())
	{
		DPF(1, _T("CCubeTexture::EnumAttachedCube7() - Failed to find face index.\n"));
		SAFEDELETE(pNew);
		return DDENUMRET_CANCEL;
	}

    // Wrap all attached surfaces
    hr = pSurf->EnumAttachedSurfaces(pThis,EnumAttachedCube7);
	if(FAILED(hr))
    {
		DPF(1, _T("CCubeTexture::EnumAttached7() - EnumAttachedSurfaces failed.\n"));
		return DDENUMRET_CANCEL;
	}

    return DDENUMRET_OK;
}

// --------------------------------------------------------------------------------------

void CCubeTexture::ReleaseD3D7()
{
	for(int i=0; i<m_dwLevelCount; i++)
	{
		for(int j=0; j<6; j++)
		{
			RELEASE(m_pSurfaces[i][j])
		}
		SAFEDELETEA(m_pSurfaces[i]);
	}
	SAFEDELETEA(m_pSurfaces);

    RELEASE(m_pPalette);
	RELEASE(m_pTexture7);
	RELEASE(m_pSysTexture);
}

// --------------------------------------------------------------------------------------

HRESULT CCubeTexture::AddDirtyRect7(CUBEMAP_FACES FaceType, CONST RECT* pDirtyRect)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

void CCubeTexture::PreLoad7()
{
	if (NULL == m_pTexture7)
		return;
	m_pDevice->m_pDevice7->PreLoad(m_pTexture7);
}

// -------------------------------------------------------------------------------------

DWORD CCubeTexture::SetLOD7(DWORD dwNewLOD)
{
    if (NULL == m_pTexture7)
		return 0;

	DWORD dwPrevLOD;
	HRESULT hr = m_pTexture7->GetLOD(&dwPrevLOD);
	if(FAILED(hr))
	{
		DPF(1, _T("CCubeTexture::SetLOD7() - GetLOD() failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return 0;
	}

	hr = m_pTexture7->SetLOD(dwNewLOD);
	if(FAILED(hr))
	{
		DPF(1, _T("CCubeTexture::SetLOD7() - SetLOD() failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return dwPrevLOD;
	}

	return dwPrevLOD;
}

// -------------------------------------------------------------------------------------

DWORD CCubeTexture::GetLOD7()
{
	if (NULL == m_pTexture7)
		return 0;

	DWORD dwLOD;
	HRESULT hr = m_pTexture7->GetLOD(&dwLOD);
	if(FAILED(hr))
	{
		DPF(1, _T("CCubeTexture::SetLOD7() - GetLOD() failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return 0;
	}

	return dwLOD;
}

// -------------------------------------------------------------------------------------

DWORD CCubeTexture::SetPriority7(DWORD dwNewPriority)
{
	if (NULL == m_pTexture7)
		return 0;

	DWORD dwPrevPriority;
	HRESULT hr = m_pTexture7->GetPriority(&dwPrevPriority);
	if(FAILED(hr))
	{
		DPF(1, _T("CCubeTexture::SetPriority7() - GetPriority failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return 0;
	}

	hr = m_pTexture7->SetPriority(dwNewPriority);
	if(FAILED(hr))
	{
		DPF(1, _T("CCubeTexture::SetPriority7() - SetPriority failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return dwPrevPriority;
	}

	return dwPrevPriority;
}

// -------------------------------------------------------------------------------------

DWORD CCubeTexture::GetPriority7()
{
    if (NULL == m_pTexture7)
		return 0;

	DWORD dwPriority;
	HRESULT hr = m_pTexture7->GetPriority(&dwPriority);
	if(FAILED(hr))
	{
		DPF(1, _T("CCubeTexture::SetPriority7() - GetPriority failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return 0;
	}

	return dwPriority;
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::SetPalette7(PALETTEENTRY *pPalette)
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
		DPF(1, _T("CCubeTexture::SetPalette7() - Texture format doesn't support palettes.\n"));
		return E_FAIL;
	}

	if(m_Format.ddpfPixelFormat.dwFlags & DDPF_ALPHA)
		dwFlags |= DDPCAPS_ALPHA;

	// Create the palette
	hr = m_pDevice->GetD3D()->m_pDDraw7->CreatePalette(dwFlags, pPal, &m_pPalette, NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CCubeTexture::SetPalette7() - CreatePalette failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}

	// Attach the palette to each surface
    hr = GetCubeMapSurface(CUBEMAP_FACE_POSITIVE_X, 0, &pSurf);
	if(FAILED(hr))
	{
		DPF(1, _T("CCubeTexture::SetPalette7() - GetSurfaceLevel failed. hr=%s(%x).\n"),
			m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}
		
	// Set the new palette
    hr = pSurf->GetD3DSurface7()->SetPalette(m_pPalette);
    if(FAILED(hr))
	{
		DPF(1, _T("CCubeTexture::SetPalette7() - SetPalette failed. hr=%s(%x).\n"),
				m_pDevice->GetD3D()->HResultToString(hr), hr);
		return hr;
	}
    
	RELEASE(pSurf);

	return S_OK;
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::GetDeviceN(CDevice** ppDevice)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::FreePrivateDataN(REFGUID refguid)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::GetPrivateDataN(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::SetPrivateDataN(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags)
{
	return E_NOTIMPL;
}
