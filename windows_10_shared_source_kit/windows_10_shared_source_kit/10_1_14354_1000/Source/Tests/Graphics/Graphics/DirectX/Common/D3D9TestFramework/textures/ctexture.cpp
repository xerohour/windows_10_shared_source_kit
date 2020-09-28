#include "TexturesPCH.h"

#include "CTextures.h"

// -------------------------------------------------------------------------------------

CBaseTexture::CBaseTexture() 
{
	m_ResourceType = (RESOURCETYPE)0;
    memset(&m_Format, 0, sizeof(FORMAT));
    m_pSysTexture = NULL;
	m_pTexture9 = NULL;
	m_pTexture8 = NULL;
    m_pTexture7 = NULL;
	m_pPalette = NULL;
    m_pTexture6 = NULL;
	m_bPalettized = false;
}
        

// -------------------------------------------------------------------------------------

bool CBaseTexture::BltSupported(FORMAT *pFmt, DWORD Pool)
{
	// Check whether BLT is supported for the given format
	if ((Pool == POOL_SYSTEMMEM) || (Pool == POOL_MANAGED) || (Pool == POOL_AGP))
	{
		if ( m_pDevice->GetInterface() < 8.f &&
		    ((pFmt->ddpfPixelFormat.dwFlags & PF_ALPHA) ||
			 (pFmt->ddpfPixelFormat.dwFlags & PF_BUMPDUDV) ||
			 (pFmt->ddpfPixelFormat.dwFlags & PF_LUMINANCE) ||
			 (pFmt->ddpfPixelFormat.dwFlags & PF_PALETTEINDEXED4)))
			  return false;

		if(	 m_pDevice->GetInterface() < 7.f &&
			((pFmt->ddpfPixelFormat.dwFourCC == FMT_UYVY) ||
			 (pFmt->ddpfPixelFormat.dwFourCC == FMT_YUY2)))
			 return false;
	}

	return true;
}

// -------------------------------------------------------------------------------------

CTexture::CTexture()
{
    m_pSurfaces = NULL;
	m_dwLevelCount = 0;
	m_pDevice = NULL;
	m_ResourceType = RTYPE_TEXTURE;
	pReleaseD3D = NULL;
}

// -------------------------------------------------------------------------------------

CTexture::~CTexture()
{
	if(pReleaseD3D)
		ReleaseD3D();
}

// -------------------------------------------------------------------------------------

ULONG WINAPI CTexture::AddRef(void)
{
    if(m_pDevice)
		m_pDevice->AddRef();
	return CObject::AddRef();
}

// -------------------------------------------------------------------------------------

ULONG WINAPI CTexture::Release(void)
{
    if(m_pDevice)
		m_pDevice->Release();
	return CObject::Release();
}

// -------------------------------------------------------------------------------------

bool CTexture::Init(LPVOID pD3DTexture, CDevice *pDevice)
{
	bool bRet = false;
	HRESULT hr;
    SURFACEDESC Desc;

	if (NULL == pDevice)
		return false;

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	if (6.0f == m_pDevice->GetInterface())
	{
                pAddDirtyRect = &CTexture::AddDirtyRect6;
                pPreLoad = &CTexture::PreLoadUnsup;
                pSetLOD = &CTexture::SetLODUnsup;
                pGetLOD = &CTexture::GetLODUnsup;
                pSetPriority = &CTexture::SetPriorityUnsup;
                pGetPriority = &CTexture::GetPriorityUnsup;
                pReleaseD3D = &CTexture::ReleaseD3D6;
                pSetPalette = &CTexture::SetPalette6;
                pSetDDrawPalette = &CTexture::SetDDrawPalette6;
                pGetDDrawPalette = &CTexture::GetDDrawPalette6;
        pSetColorKey = &CTexture::SetColorKey6;
        pLockRect   = &CTexture::LockRectN;
        pUnlockRect = &CTexture::UnlockRectN;
        pGetLevelDesc = &CTexture::GetLevelDescN;
                pGetSurfaceLevel = &CTexture::GetSurfaceLevelN;
        pGetDevice = &CTexture::GetDeviceN;
        pFreePrivateData = &CTexture::FreePrivateDataN;
        pGetPrivateData = &CTexture::GetPrivateDataN;
        pSetPrivateData = &CTexture::SetPrivateDataN;
        pSetAutoGenFilterType = &CTexture::SetAutoGenFilterTypeUnsup;
        pGetAutoGenFilterType = &CTexture::GetAutoGenFilterTypeUnsup;
                pGenerateMipSubLevels = &CTexture::GenerateMipSubLevelsUnsup;
		bRet = Init6(pD3DTexture);
	}
	else if (7.0f == m_pDevice->GetInterface())
	{
                pAddDirtyRect = &CTexture::AddDirtyRect7;
                pPreLoad = &CTexture::PreLoad7;
                pSetLOD = &CTexture::SetLOD7;
                pGetLOD = &CTexture::GetLOD7;
                pSetPriority = &CTexture::SetPriority7;
                pGetPriority = &CTexture::GetPriority7;
                pReleaseD3D = &CTexture::ReleaseD3D7;
                pSetPalette = &CTexture::SetPalette7;
                pSetDDrawPalette = &CTexture::SetDDrawPalette7;
                pGetDDrawPalette = &CTexture::GetDDrawPalette7;
        pSetColorKey = &CTexture::SetColorKey7;
        pLockRect   = &CTexture::LockRectN;
        pUnlockRect = &CTexture::UnlockRectN;
        pGetLevelDesc = &CTexture::GetLevelDescN;
                pGetSurfaceLevel = &CTexture::GetSurfaceLevelN;
        pGetDevice = &CTexture::GetDeviceN;
        pFreePrivateData = &CTexture::FreePrivateDataN;
        pGetPrivateData = &CTexture::GetPrivateDataN;
        pSetPrivateData = &CTexture::SetPrivateDataN;
        pSetAutoGenFilterType = &CTexture::SetAutoGenFilterTypeUnsup;
        pGetAutoGenFilterType = &CTexture::GetAutoGenFilterTypeUnsup;
                pGenerateMipSubLevels = &CTexture::GenerateMipSubLevelsUnsup;
		bRet = Init7(pD3DTexture);
	}
	else if (8.0f <= m_pDevice->GetInterface() && 9.0f > m_pDevice->GetInterface())
	{
                pAddDirtyRect = &CTexture::AddDirtyRect8;
                pPreLoad = &CTexture::PreLoad8;
                pSetLOD = &CTexture::SetLOD8;
                pGetLOD = &CTexture::GetLOD8;
                pSetPriority = &CTexture::SetPriority8;
                pGetPriority = &CTexture::GetPriority8;
                pReleaseD3D = &CTexture::ReleaseD3D8;
        pSetPalette = &CTexture::SetPalette8;
                pSetDDrawPalette = &CTexture::SetDDrawPaletteUnsup;
                pGetDDrawPalette = &CTexture::GetDDrawPaletteUnsup;
        pSetColorKey = &CTexture::SetColorKeyUnsup;
        pLockRect   = &CTexture::LockRect8;
        pUnlockRect = &CTexture::UnlockRect8;
                pGetLevelDesc = &CTexture::GetLevelDesc8;
                pGetSurfaceLevel = &CTexture::GetSurfaceLevel8;
        pGetDevice = &CTexture::GetDevice8;
        pFreePrivateData = &CTexture::FreePrivateData8;
        pGetPrivateData = &CTexture::GetPrivateData8;
        pSetPrivateData = &CTexture::SetPrivateData8;
        pSetAutoGenFilterType = &CTexture::SetAutoGenFilterTypeUnsup;
        pGetAutoGenFilterType = &CTexture::GetAutoGenFilterTypeUnsup;
                pGenerateMipSubLevels = &CTexture::GenerateMipSubLevelsUnsup;
		bRet = Init8(pD3DTexture);
	}
    else if (9.0f <= m_pDevice->GetInterface() && 10.0f > m_pDevice->GetInterface())
	{
                pAddDirtyRect = &CTexture::AddDirtyRect9;
                pSetLOD = &CTexture::SetLOD9;
                pGetLOD = &CTexture::GetLOD9;
                pPreLoad = &CTexture::PreLoad9;
                pSetPriority = &CTexture::SetPriority9;
                pGetPriority = &CTexture::GetPriority9;
                pReleaseD3D = &CTexture::ReleaseD3D9;
        pSetPalette = &CTexture::SetPalette9;
                pSetDDrawPalette = &CTexture::SetDDrawPaletteUnsup;
                pGetDDrawPalette = &CTexture::GetDDrawPaletteUnsup;
        pSetColorKey = &CTexture::SetColorKeyUnsup;
        pLockRect   = &CTexture::LockRect9;
        pUnlockRect = &CTexture::UnlockRect9;
                pGetLevelDesc = &CTexture::GetLevelDesc9;
                pGetSurfaceLevel = &CTexture::GetSurfaceLevel9;
        pGetDevice = &CTexture::GetDevice9;
        pFreePrivateData = &CTexture::FreePrivateData9;
        pGetPrivateData = &CTexture::GetPrivateData9;
        pSetPrivateData = &CTexture::SetPrivateData9;
        pSetAutoGenFilterType = &CTexture::SetAutoGenFilterType9;
        pGetAutoGenFilterType = &CTexture::GetAutoGenFilterType9;
                pGenerateMipSubLevels = &CTexture::GenerateMipSubLevels9;
		bRet = Init9(pD3DTexture);
	}

	// Gather some info about this texture
    hr = GetLevelDesc(0, &Desc);
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::Init() - GetLevelDesc failed. hr=%s(%x).\n"),
			m_pDevice->m_pD3D->HResultToString(hr), hr);
		return false;

	}

	memcpy(&m_Format, &(Desc.Format), sizeof(FORMAT));
	if(Desc.Format.ddpfPixelFormat.dwFlags & PF_PALETTEINDEXED4 ||
	   Desc.Format.ddpfPixelFormat.dwFlags & PF_PALETTEINDEXED8)
		m_bPalettized = true;


	return bRet;
}

// -------------------------------------------------------------------------------------

DWORD CTexture::SetLODUnsup(DWORD dwNewLOD)
{
	DebugOut.Write(1, "CTexture: SetLOD() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return 0;
}

// -------------------------------------------------------------------------------------

DWORD CTexture::GetLODUnsup(void)
{
	DebugOut.Write(1, "CTexture: GetLOD() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return 0;
}

// -------------------------------------------------------------------------------------

void CTexture::PreLoadUnsup()
{
	DebugOut.Write(1, "CTexture: PreLoad() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
}

// -------------------------------------------------------------------------------------

DWORD CTexture::SetPriorityUnsup(DWORD dwNewPriority)
{
	DebugOut.Write(1, "CTexture: SetPriority() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return (DWORD)0;
}

// -------------------------------------------------------------------------------------

DWORD CTexture::GetPriorityUnsup(void)
{
	DebugOut.Write(1, "CTexture: GetPriority() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return (DWORD)0;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::SetAutoGenFilterTypeUnsup(DWORD FilterType)
{
	DebugOut.Write(1, "CTexture: SetAutoGenFilterType() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return E_FAIL;
}

// -------------------------------------------------------------------------------------

DWORD CTexture::GetAutoGenFilterTypeUnsup(void)
{
	DebugOut.Write(1, "CTexture: GetAutoGenFilterType() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return (DWORD)0;
}

// -------------------------------------------------------------------------------------

void CTexture::GenerateMipSubLevelsUnsup(void)
{
	DebugOut.Write(1, "CTexture: GenerateMipSubLevels() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::LockRectN(UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD dwFlags)
{
	if (m_dwLevelCount <= uLevel)
		return E_INVALIDARG;

	return m_pSurfaces[uLevel]->LockRect(pLockedRect, pRect, dwFlags);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::UnlockRectN(UINT uLevel)
{
	if (m_dwLevelCount <= uLevel)
		return E_INVALIDARG;

	return m_pSurfaces[uLevel]->UnlockRect();
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::GetLevelDescN(UINT uLevel, PSURFACEDESC pDesc)
{
	if (m_dwLevelCount <= uLevel)
		return E_INVALIDARG;

	return m_pSurfaces[uLevel]->GetDesc(pDesc);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::GetSurfaceLevelN(UINT uLevel, CSurface** ppSurfaceLevel)
{
	if (m_dwLevelCount <= uLevel)
		return E_INVALIDARG;
	
	m_pSurfaces[uLevel]->AddRef();
	*ppSurfaceLevel = m_pSurfaces[uLevel];

	return S_OK;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::LockCopy(UINT uLevel, CTexture* pSrcTexture)
{
	HRESULT hr = 0;
	CSurface *pTarget = NULL, *pSrc = NULL;
    
    UINT uStartLevel = uLevel == 0 ? 1 : uLevel;
	UINT uEndLevel = uLevel == 0 ? m_dwLevelCount : uStartLevel;

	for(uStartLevel; uStartLevel <= uEndLevel; uStartLevel++)
	{
		// Get the target surface
		hr = GetSurfaceLevel(uStartLevel-1, &pTarget);
		if(FAILED(hr))
			break;

		// Get the src surface
		hr = pSrcTexture->GetSurfaceLevel(uStartLevel-1, &pSrc);
		if(FAILED(hr))
			break;

		// Load the target surface from the src surface
		hr = pTarget->UpdateSurface(pSrc);
		if(FAILED(hr))
			break;

		RELEASE(pTarget);
		RELEASE(pSrc);
	}

	RELEASE(pTarget);
	RELEASE(pSrc);

	return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::CopyRects(UINT uLevel, CTexture *pSrcTexture)
{
	HRESULT hr = S_OK;
	CSurface *pSrc = NULL, *pDest = NULL;

    UINT uStartLevel = uLevel == 0 ? 1 : uLevel;
	UINT uEndLevel = uLevel == 0 ? m_dwLevelCount : uStartLevel;

	for(uStartLevel; uStartLevel <= uEndLevel; uStartLevel++)
	{
		hr = pSrcTexture->GetSurfaceLevel(uStartLevel-1, &pSrc);
		if(FAILED(hr))
			goto Exit;
		
		hr = GetSurfaceLevel(uStartLevel-1, &pDest);
		if(FAILED(hr))
			goto Exit;

		hr = m_pDevice->CopyRects(pSrc, NULL, 0, pDest, NULL);
		if(FAILED(hr))
			goto Exit;

		RELEASE(pSrc);
		RELEASE(pDest);
	}

Exit:
	RELEASE(pSrc);
	RELEASE(pDest);	

	return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::UpdateSurface(UINT uLevel, CTexture *pSrcTexture)
{
	HRESULT hr = S_OK;
	CSurface *pSrc = NULL, *pDest = NULL;

    UINT uStartLevel = uLevel == 0 ? 1 : uLevel;
	UINT uEndLevel = uLevel == 0 ? m_dwLevelCount : uStartLevel;

	for(uStartLevel; uStartLevel <= uEndLevel; uStartLevel++)
	{
		hr = pSrcTexture->GetSurfaceLevel(uStartLevel-1, &pSrc);
		if(FAILED(hr))
			goto Exit;
		
		hr = GetSurfaceLevel(uStartLevel-1, &pDest);
		if(FAILED(hr))
			goto Exit;

		hr = m_pDevice->UpdateSurface(pSrc, NULL, pDest, NULL);
		if(FAILED(hr))
			goto Exit;

		RELEASE(pSrc);
		RELEASE(pDest);
	}

Exit:
	RELEASE(pSrc);
	RELEASE(pDest);	

	return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::UpdateTexture(UINT uLevel, CImage *pImg, DWORD dwTexLoad)
{
	HRESULT hr;
    CSurface *pSurf = NULL;
    SURFACEDESC Desc;
    DWORD dwLoad = dwTexLoad;

    if (m_dwLevelCount < uLevel)
		return E_FAIL;

	// Get the texture DESC
    hr = GetLevelDesc(0, &Desc);
	if(FAILED(hr))
	   return hr;

	// Calculate the level range
    UINT uStartLevel = uLevel == 0 ? 1 : uLevel;
	UINT uEndLevel = uLevel == 0 ? m_dwLevelCount : uStartLevel;

	for(uStartLevel; uStartLevel <= uEndLevel; uStartLevel++)
	{
		// If there is no sysmem copy, attempt to load the surface directly
		if(NULL == m_pSysTexture)
			hr = GetSurfaceLevel(uStartLevel-1, &pSurf);
		else
			hr = ((CTexture*)m_pSysTexture)->GetSurfaceLevel(uStartLevel-1, &pSurf);
		if(FAILED(hr))
			goto Exit;

		hr = pSurf->UpdateSurface(pImg);
		if(FAILED(hr))
			goto Exit;

		RELEASE(pSurf);
	}

	// Update the target texture
	if (m_pSysTexture)
	{
		if(!BltSupported(&Desc.Format, Desc.Pool) || dwLoad == TEXLOAD_LOCKCOPY)
		{
			m_pDevice->SetTexLoad(TEXLOAD_LOCKCOPY);
			hr = LockCopy(uLevel,(CTexture*)m_pSysTexture);
		}
		else if(dwLoad == TEXLOAD_UPDATETEXTURE)
		{
			// If we're updating a sub-texture, we need to tell the runtime to update all
			// since they only keep dirty-region info for Level 0.
			if(uLevel > 1)
				((CTexture*)m_pSysTexture)->AddDirtyRect(NULL);

			m_pDevice->SetTexLoad(TEXLOAD_UPDATETEXTURE);
			hr = m_pDevice->UpdateTexture(m_pSysTexture, this);
		}
		else if(dwLoad == TEXLOAD_COPYRECTS)
		{	
			m_pDevice->SetTexLoad(TEXLOAD_COPYRECTS);
			hr = CopyRects(uLevel,(CTexture*)m_pSysTexture);
		}
		else if(dwLoad == TEXLOAD_UPDATESURFACE)
		{	
			m_pDevice->SetTexLoad(TEXLOAD_UPDATESURFACE);
			hr = UpdateSurface(uLevel,(CTexture*)m_pSysTexture);
		}
		else
		{
			DPF(1, _T("CTexture::UpdateTexture() - requested TEXLOAD(%d) not supported.\n"), dwLoad);
			hr = E_FAIL;
		}
		if(FAILED(hr))
			goto Exit;
	}
Exit:
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::UpdateTexture() - failed to load texture.  hr=%s(%x).\n"),
            m_pDevice->m_pD3D->HResultToString(hr), hr);
	}
    RELEASE(pSurf);

    return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::LoadTextureFromTexture(UINT uLevel, CTexture *pTex)
{
    return LoadTextureFromTexture(uLevel, pTex, FILTER_NONE);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::LoadTextureFromTexture(UINT uLevel, CTexture *pTex, DWORD filter)
{
    HRESULT hr;
    CSurface *pSrcSurf = NULL, *pDstSurf = NULL;
    SURFACEDESC Desc;

    if (m_dwLevelCount < uLevel)
        return E_FAIL;

    // Get the texture DESC
    hr = GetLevelDesc(0, &Desc);
	
    if(FAILED(hr))
        return hr;

    // Load the sysmem texture
    UINT uStartLevel = uLevel == 0 ? 1 : uLevel;
    UINT uEndLevel = uLevel == 0 ? m_dwLevelCount : uStartLevel;

    for(uStartLevel; uStartLevel <= uEndLevel; uStartLevel++)
    {
        hr = pTex->GetSurfaceLevel(uStartLevel-1, &pSrcSurf);
        if(FAILED(hr))
            goto Exit;

        hr = GetSurfaceLevel(uStartLevel-1, &pDstSurf);
        if(FAILED(hr))
            goto Exit;

        hr = pDstSurf->LoadSurfaceFromSurface(pSrcSurf, filter);
        if(FAILED(hr))
            goto Exit;

        RELEASE(pSrcSurf);
        RELEASE(pDstSurf);
    }

  Exit:
	
    if(FAILED(hr))
    {
        DPF(1, _T("CTexture::LoadTextureFromTexture() - failed to load texture.  hr=%s(%x).\n"),
            m_pDevice->m_pD3D->HResultToString(hr), hr);
    }

    RELEASE(pSrcSurf);
    RELEASE(pDstSurf);
    return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::UpdateTexture(UINT uLevel, CImage **pImg, DWORD dwTexLoad)
{
	HRESULT hr;
    CSurface *pSurf = NULL;
    SURFACEDESC Desc;
    DWORD dwLoad = dwTexLoad;

    if (m_dwLevelCount < uLevel)
		return E_FAIL;

	// Get the texture DESC
    hr = GetLevelDesc(0, &Desc);
	
	if(FAILED(hr))
	   return hr;

	// Load the sysmem texture
    UINT uStartLevel = uLevel == 0 ? 1 : uLevel;
	UINT uEndLevel = uLevel == 0 ? m_dwLevelCount : uStartLevel;

	for(uStartLevel; uStartLevel <= uEndLevel; uStartLevel++)
	{
		// If there is no sysmem copy, attempt to load the surface directly
		if(NULL == m_pSysTexture)
			hr = GetSurfaceLevel(uStartLevel-1, &pSurf);
		else
			hr = ((CTexture*)m_pSysTexture)->GetSurfaceLevel(uStartLevel-1, &pSurf);

        if(FAILED(hr))
			goto Exit;

		hr = pSurf->UpdateSurface(pImg[uStartLevel-1]);
		if(FAILED(hr))
			goto Exit;

		RELEASE(pSurf);
	}

	// Update the target texture
	if (m_pSysTexture)
	{
	    if(!BltSupported(&Desc.Format, Desc.Pool) || dwLoad == TEXLOAD_LOCKCOPY)
	    {
		    m_pDevice->SetTexLoad(TEXLOAD_LOCKCOPY);
		    hr = LockCopy(uLevel,(CTexture*)m_pSysTexture);
	    }
	    else if(dwLoad == TEXLOAD_UPDATETEXTURE)
        {
		    // If we're updating a sub-texture, we need to tell the runtime to update all
		    // since they only keep dirty-region info for Level 0.
		    if(uLevel > 1)
			    ((CTexture*)m_pSysTexture)->AddDirtyRect(NULL);

		    m_pDevice->SetTexLoad(TEXLOAD_UPDATETEXTURE);
		    hr = m_pDevice->UpdateTexture(m_pSysTexture, this);
	    }
	    else if(dwLoad == TEXLOAD_COPYRECTS)
	    {	
		    m_pDevice->SetTexLoad(TEXLOAD_COPYRECTS);
		    hr = CopyRects(uLevel,(CTexture*)m_pSysTexture);
	    }
	    else if(dwLoad == TEXLOAD_UPDATESURFACE)
	    {	
		    m_pDevice->SetTexLoad(TEXLOAD_UPDATESURFACE);
		    hr = UpdateSurface(uLevel,(CTexture*)m_pSysTexture);
	    }
	    else
	    {
		    DPF(1, _T("CTexture::UpdateTexture() - requested TEXLOAD(%d) not supported.\n"), dwLoad);
		    hr = E_FAIL;
	    }
    }
Exit:
	if(FAILED(hr))
	{
		DPF(1, _T("CTexture::UpdateTexture() - failed to load texture.  hr=%s(%x).\n"),
            m_pDevice->m_pD3D->HResultToString(hr), hr);
	}

    RELEASE(pSurf);
    return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::LoadTexture(UINT uLevel, LPCTSTR Filename, float fAlphaOverride, DWORD dwTexLoad)
{
	CImage *pImage = NULL;
    HRESULT hr = S_OK;
	SURFACEDESC Desc;
	DWORD dwLoad = dwTexLoad;

	// Alocate a temp CImage
	pImage = new CImage();
	if(NULL == pImage)
	{
		hr = E_FAIL;
		goto Exit;
	}

	if(!pImage->Load(Filename, fAlphaOverride))
	{
		hr = E_FAIL;
		goto Exit;
	}

	// Get the texture DESC
    hr = GetLevelDesc(0, &Desc);
	
	if(FAILED(hr))
	   goto Exit;

	// Toggle the texture loading options
    if(dwLoad == TEXLOAD_DEFAULT)
	{
		m_pDevice->NextTexLoad(&Desc);
		dwLoad = m_pDevice->GetTexLoad();
	}

    hr = UpdateTexture(uLevel, pImage, dwLoad);

Exit:
	SAFEDELETE(pImage);
	return m_pDevice->GetD3D()->SetLastError(hr);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::LoadStripes(UINT uLevel, DWORD dwW, DWORD dwH, DWORD dwStripes, DWORD dwColorArray[], bool  bHorizontal, bool  bDiagonal, DWORD dwTexLoad)
{
   	CImage *pImage = NULL;
    HRESULT hr = S_OK;
	SURFACEDESC Desc;
	DWORD dwLoad = dwTexLoad;

	// Alocate a temp CImage
	pImage = new CImage();
	if(NULL == pImage)
	{
		hr = E_FAIL;
		goto Exit;
	}

	if(!pImage->LoadStripes(dwW, dwH, dwStripes, dwColorArray, bHorizontal, bDiagonal))
	{
		hr = E_FAIL;
		goto Exit;
	}

	// Get the texture DESC
    hr = GetLevelDesc(0, &Desc);
	
	if(FAILED(hr))
	   goto Exit;

	// Toggle the texture loading options
    if(dwLoad == TEXLOAD_DEFAULT)
	{
		m_pDevice->NextTexLoad(&Desc);
		dwLoad = m_pDevice->GetTexLoad();
	}

    hr = UpdateTexture(uLevel, pImage, dwLoad);

Exit:
	SAFEDELETE(pImage);
	return m_pDevice->GetD3D()->SetLastError(hr);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::LoadGradient(UINT uLevel, DWORD dwW, DWORD dwH, DWORD dwColors[4], DWORD dwTexLoad)
{
	CImage *pImage = NULL;
	HRESULT hr = S_OK;
	SURFACEDESC Desc;
	DWORD dwLoad = dwTexLoad;
	
	// Alocate a temp CImage
	pImage = new CImage();
	if(NULL == pImage)
	{
		hr = E_FAIL;
		goto Exit;
	}
	
	if(!pImage->LoadGradient(dwW, dwH, dwColors))
	{
		hr = E_FAIL;
		goto Exit;
	}
	
	// Get the texture DESC
	hr = GetLevelDesc(0, &Desc);
	
	if(FAILED(hr))
	   goto Exit;
	
	// Toggle the texture loading options
	if(dwLoad == TEXLOAD_DEFAULT)
	{
		m_pDevice->NextTexLoad(&Desc);
		dwLoad = m_pDevice->GetTexLoad();
	}
	
	hr = UpdateTexture(uLevel, pImage, dwLoad);
	
Exit:
	SAFEDELETE(pImage);
	return m_pDevice->GetD3D()->SetLastError(hr);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::LoadImage(UINT uLevel, CImage *pImg, DWORD dwTexLoad)
{
	HRESULT hr = S_OK;
	SURFACEDESC Desc;
	DWORD dwLoad = dwTexLoad;
		
	// Get the texture DESC
	hr = GetLevelDesc(0, &Desc);
	
	if(FAILED(hr))
	   goto Exit;
	
	// Toggle the texture loading options
	if(dwLoad == TEXLOAD_DEFAULT)
	{
		m_pDevice->NextTexLoad(&Desc);
		dwLoad = m_pDevice->GetTexLoad();
	}
	
	hr = UpdateTexture(uLevel, pImg, dwLoad);
	
Exit:
	return m_pDevice->GetD3D()->SetLastError(hr);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::LoadImage(UINT uLevel, CImage **pImg, DWORD dwTexLoad)
{
	HRESULT hr = S_OK;
	SURFACEDESC Desc;
	DWORD dwLoad = dwTexLoad;
		
	// Get the texture DESC
	hr = GetLevelDesc(0, &Desc);
	
	if(FAILED(hr))
	   goto Exit;
	
	// Toggle the texture loading options
	if(dwLoad == TEXLOAD_DEFAULT)
	{
		m_pDevice->NextTexLoad(&Desc);
		dwLoad = m_pDevice->GetTexLoad();
	}
	
	hr = UpdateTexture(uLevel, pImg, dwLoad);
	
Exit:
	return m_pDevice->GetD3D()->SetLastError(hr);
}

// -------------------------------------------------------------------------------------

void *CBaseTexture::GetInternalPointer(void)
{
	if (m_pTexture9)
		return m_pTexture9;
	if (m_pTexture8)
		return m_pTexture8;
	else if (m_pTexture7)
		return m_pTexture7;
	else if (m_pTexture6)
		return m_pTexture6;

	//No internal pointer set
	return NULL;
}

// --------------------------------------------------------------------------------------

HRESULT CTexture::SetDDrawPaletteUnsup(LPDIRECTDRAWPALETTE pPalette)
{
	DebugOut.Write(1, "CTexture: SetDDrawPalette() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

HRESULT CTexture::GetDDrawPaletteUnsup(LPDIRECTDRAWPALETTE *ppPalette)
{
	DebugOut.Write(1, "CTexture: GetDDrawPalette() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return E_FAIL;
}

// --------------------------------------------------------------------------------------

HRESULT CTexture::SetColorKeyUnsup(DWORD dwColorKeyFlags, DWORD *pdwColorKey)
{
	DebugOut.Write(1, "CTexture: SetColorKey() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return E_FAIL;
}


// -------------------------------------------------------------------------------------

HRESULT CTexture::GetDeviceN(CDevice** ppDevice)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::FreePrivateDataN(REFGUID refguid)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::GetPrivateDataN(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::SetPrivateDataN(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags)
{
	return E_NOTIMPL;
}
