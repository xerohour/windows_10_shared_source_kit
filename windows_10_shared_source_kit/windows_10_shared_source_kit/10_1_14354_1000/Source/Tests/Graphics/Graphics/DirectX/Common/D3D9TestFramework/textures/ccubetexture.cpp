#include "TexturesPCH.h"

#include "CTextures.h"

// -------------------------------------------------------------------------------------

CCubeTexture::CCubeTexture()
{
	m_dwLevelCount = 0;
	m_pTexture8 = NULL;
    m_pTexture7 = NULL;
	m_pDevice = NULL;
	pReleaseD3D = NULL;
	m_ResourceType = RTYPE_CUBETEXTURE;
   	m_pSurfaces = NULL;
}

// -------------------------------------------------------------------------------------

CCubeTexture::~CCubeTexture()
{   
	if(pReleaseD3D)
		ReleaseD3D();
}

// -------------------------------------------------------------------------------------

ULONG WINAPI CCubeTexture::AddRef(void)
{
    if(m_pDevice)
		m_pDevice->AddRef();
	return CObject::AddRef();
}

// -------------------------------------------------------------------------------------

ULONG WINAPI CCubeTexture::Release(void)
{
    if(m_pDevice)
		m_pDevice->Release();
	return CObject::Release();
}

// -------------------------------------------------------------------------------------

bool CCubeTexture::Init(LPVOID pD3DTexture, CDevice *pDevice)
{
	bool bRet = false;
    HRESULT hr;
    SURFACEDESC Desc;

	if (NULL == pDevice)
		return false;

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	if (7.0f == m_pDevice->GetInterface())
	{
                pAddDirtyRect = &CCubeTexture::AddDirtyRect7;
                pPreLoad = &CCubeTexture::PreLoad7;
                pSetLOD = &CCubeTexture::SetLOD7;
                pGetLOD = &CCubeTexture::GetLOD7;
                pSetPriority = &CCubeTexture::SetPriority7;
                pGetPriority = &CCubeTexture::GetPriority7;
                pReleaseD3D = &CCubeTexture::ReleaseD3D7;
        pGetCubeMapSurface = &CCubeTexture::GetCubeMapSurfaceN;
                pGetLevelDesc = &CCubeTexture::GetLevelDescN;
                pLockRect = &CCubeTexture::LockRectN;
                pUnlockRect = &CCubeTexture::UnlockRectN;
                pSetPriority = &CCubeTexture::SetPriority7;
                pGetPriority = &CCubeTexture::GetPriority7;
        pSetPalette = &CCubeTexture::SetPalette7;
        pGetDevice = &CCubeTexture::GetDeviceN;
        pFreePrivateData = &CCubeTexture::FreePrivateDataN;
        pGetPrivateData = &CCubeTexture::GetPrivateDataN;
        pSetPrivateData = &CCubeTexture::SetPrivateDataN;
        pSetAutoGenFilterType = &CCubeTexture::SetAutoGenFilterTypeUnsup;
        pGetAutoGenFilterType = &CCubeTexture::GetAutoGenFilterTypeUnsup;
                pGenerateMipSubLevels = &CCubeTexture::GenerateMipSubLevelsUnsup;
		bRet = Init7( pD3DTexture );
	}
	else if (8.0f <= m_pDevice->GetInterface() && 9.0f > m_pDevice->GetInterface())
	{
                pAddDirtyRect = &CCubeTexture::AddDirtyRect8;
        pPreLoad = &CCubeTexture::PreLoad8;
                pSetLOD = &CCubeTexture::SetLOD8;
                pGetLOD = &CCubeTexture::GetLOD8;
                pSetPriority = &CCubeTexture::SetPriority8;
                pGetPriority = &CCubeTexture::GetPriority8;
                pReleaseD3D = &CCubeTexture::ReleaseD3D8;
                pGetCubeMapSurface = &CCubeTexture::GetCubeMapSurface8;
                pGetLevelDesc = &CCubeTexture::GetLevelDesc8;
                pLockRect = &CCubeTexture::LockRect8;
                pUnlockRect = &CCubeTexture::UnlockRect8;
                pSetPalette = &CCubeTexture::SetPalette8;
        pGetDevice = &CCubeTexture::GetDevice8;
        pFreePrivateData = &CCubeTexture::FreePrivateData8;
        pGetPrivateData = &CCubeTexture::GetPrivateData8;
        pSetPrivateData = &CCubeTexture::SetPrivateData8;
        pSetAutoGenFilterType = &CCubeTexture::SetAutoGenFilterTypeUnsup;
        pGetAutoGenFilterType = &CCubeTexture::GetAutoGenFilterTypeUnsup;
                pGenerateMipSubLevels = &CCubeTexture::GenerateMipSubLevelsUnsup;
		bRet = Init8( pD3DTexture );
	}
	else if (9.0f <= m_pDevice->GetInterface() && 10.0f > m_pDevice->GetInterface())
	{
                pAddDirtyRect = &CCubeTexture::AddDirtyRect9;
        pPreLoad = &CCubeTexture::PreLoad9;
                pSetLOD = &CCubeTexture::SetLOD9;
                pGetLOD = &CCubeTexture::GetLOD9;
                pSetPriority = &CCubeTexture::SetPriority9;
                pGetPriority = &CCubeTexture::GetPriority9;
                pReleaseD3D = &CCubeTexture::ReleaseD3D9;
                pGetCubeMapSurface = &CCubeTexture::GetCubeMapSurface9;
                pGetLevelDesc = &CCubeTexture::GetLevelDesc9;
                pLockRect = &CCubeTexture::LockRect9;
                pUnlockRect = &CCubeTexture::UnlockRect9;
                pSetPalette = &CCubeTexture::SetPalette9;
        pGetDevice = &CCubeTexture::GetDevice9;
        pFreePrivateData = &CCubeTexture::FreePrivateData9;
        pGetPrivateData = &CCubeTexture::GetPrivateData9;
        pSetPrivateData = &CCubeTexture::SetPrivateData9;
        pSetAutoGenFilterType = &CCubeTexture::SetAutoGenFilterType9;
        pGetAutoGenFilterType = &CCubeTexture::GetAutoGenFilterType9;
                pGenerateMipSubLevels = &CCubeTexture::GenerateMipSubLevels9;
		bRet = Init9( pD3DTexture );
	}
	else
		return false;

   	// Gather some info about this texture
    hr = GetLevelDesc(0, &Desc);
	if(FAILED(hr))
	{
		DPF(1, _T("CCubeTexture::Init() - GetLevelDesc failed. hr=%s(%x).\n"),
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

HRESULT CCubeTexture::GetCubeMapSurfaceN(CUBEMAP_FACES FaceType, UINT uLevel, CSurface** ppCubeMapSurface)
{
	if (m_dwLevelCount <= uLevel)
		return E_INVALIDARG;
	
	m_pSurfaces[uLevel][FaceType]->AddRef();
	*ppCubeMapSurface = m_pSurfaces[uLevel][FaceType];
	return S_OK;
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::GetLevelDescN(UINT uLevel, PSURFACEDESC pDesc)
{
	if (m_dwLevelCount <= uLevel)
		return E_INVALIDARG;

	return m_pSurfaces[uLevel][0]->GetDesc(pDesc);
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::SetAutoGenFilterTypeUnsup(DWORD FilterType)
{
	DebugOut.Write(1, "CCubeTexture: SetAutoGenFilterType() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return E_FAIL;
}

// -------------------------------------------------------------------------------------

DWORD CCubeTexture::GetAutoGenFilterTypeUnsup(void)
{
	DebugOut.Write(1, "CCubeTexture: GetAutoGenFilterType() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return (DWORD)0;
}

// -------------------------------------------------------------------------------------

void CCubeTexture::GenerateMipSubLevelsUnsup(void)
{
	DebugOut.Write(1, "CCubeTexture: GenerateMipSubLevels() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::LockRectN(CUBEMAP_FACES FaceType, UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD dwFlags)
{
	if (m_dwLevelCount <= uLevel)
		return E_INVALIDARG;

	return m_pSurfaces[uLevel][FaceType]->LockRect(pLockedRect, pRect, dwFlags);
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::UnlockRectN(CUBEMAP_FACES FaceType, UINT uLevel)
{
	if (m_dwLevelCount <= uLevel)
		return E_INVALIDARG;

	return m_pSurfaces[uLevel][FaceType]->UnlockRect();
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::LockCopy(CCubeTexture* pSrcTexture)
{
	HRESULT hr = 0;
	CSurface *pTarget = NULL, *pSrc = NULL;
    
	for(int j=0; j < 6; j++)
	for(int i=0; i<m_dwLevelCount; i++)
	{
		// Get the target surface
		hr = GetCubeMapSurface((CUBEMAP_FACES)j, i, &pTarget);
		if(FAILED(hr))
			break;

		// Get the src surface
		hr = pSrcTexture->GetCubeMapSurface((CUBEMAP_FACES)j, i, &pSrc);
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

HRESULT CCubeTexture::CopyRects(CCubeTexture *pSrcTexture)
{
	HRESULT hr = S_OK;
	CSurface *pSrc = NULL, *pDest = NULL;

	// For each mip level, use COPYRECTS to blt from Sys -> Vid
    for(int j=0; j < 6; j++)
	for(int i=0; i<m_dwLevelCount; i++)
	{
		hr = pSrcTexture->GetCubeMapSurface((CUBEMAP_FACES)j, i, &pSrc);
		if(FAILED(hr))
			goto Exit;
		
		hr = GetCubeMapSurface((CUBEMAP_FACES)j, i, &pDest);
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

HRESULT CCubeTexture::UpdateSurface(CCubeTexture *pSrcTexture)
{
	HRESULT hr = S_OK;
	CSurface *pSrc = NULL, *pDest = NULL;

	// For each mip level, use UpdateSurface to load from Sys -> Vid
    for(int j=0; j < 6; j++)
	for(int i=0; i<m_dwLevelCount; i++)
	{
		hr = pSrcTexture->GetCubeMapSurface((CUBEMAP_FACES)j, i, &pSrc);
		if(FAILED(hr))
			goto Exit;
		
		hr = GetCubeMapSurface((CUBEMAP_FACES)j, i, &pDest);
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

HRESULT CCubeTexture::UpdateTexture(UINT uLevel, CImage **pImg, DWORD dwTexLoad)
{
	HRESULT     hr;
	SURFACEDESC Desc;
    CSurface   *pSurf = NULL;
    DWORD       dwLoad = dwTexLoad;

    if (m_dwLevelCount < uLevel)
		return E_FAIL;

	// Get the texture DESC
    hr = GetLevelDesc(0, &Desc);
	if(FAILED(hr))
		return hr;

	// Calculate the level range
    UINT uStartLevel = uLevel == 0 ? 1 : uLevel;
	UINT uEndLevel = uLevel == 0 ? m_dwLevelCount : uStartLevel;

	for(int j=uStartLevel; j <= uEndLevel; j++)
	{
		for(int i=0; i < 6; i++)
		{
			// If there is no sysmem copy, attempt to load the surface directly
			if(NULL == m_pSysTexture)
				hr = GetCubeMapSurface((CUBEMAP_FACES)i, j-1, &pSurf);
			else
				hr = ((CCubeTexture*)m_pSysTexture)->GetCubeMapSurface((CUBEMAP_FACES)i, j-1, &pSurf);
			if(FAILED(hr))
				goto Exit;

			hr = pSurf->UpdateSurface(pImg[i]);
			if(FAILED(hr))
				goto Exit;
			RELEASE(pSurf);
		}
	}

	// Update the target texture
	if(m_pSysTexture)
	{
		if(!BltSupported(&Desc.Format, Desc.Pool) || dwLoad == TEXLOAD_LOCKCOPY)
		{
			m_pDevice->SetTexLoad(TEXLOAD_LOCKCOPY);
			hr = LockCopy((CCubeTexture*)m_pSysTexture);
		}
		else if(dwLoad == TEXLOAD_UPDATETEXTURE)
		{
			// If we're updating a sub-cube, we need to tell the runtime to update all
			// since they only keep dirty-region info for Level 0.
			if(uLevel > 1)
				((CCubeTexture*)m_pSysTexture)->AddDirtyRect((CUBEMAP_FACES)0, NULL);

			m_pDevice->SetTexLoad(TEXLOAD_UPDATETEXTURE);
			hr = m_pDevice->UpdateTexture(m_pSysTexture, this);
		}
		else if(dwLoad == TEXLOAD_COPYRECTS)
		{	
			m_pDevice->SetTexLoad(TEXLOAD_COPYRECTS);
			hr = CopyRects((CCubeTexture*)m_pSysTexture);
		}
		else if(dwLoad == TEXLOAD_UPDATESURFACE)
		{	
			m_pDevice->SetTexLoad(TEXLOAD_UPDATESURFACE);
			hr = UpdateSurface((CCubeTexture*)m_pSysTexture);
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
		DPF(1, _T("CCubeTexture::UpdateTexture() - failed to load texture.  hr=%s(%x).\n"),
            m_pDevice->m_pD3D->HResultToString(hr), hr);
	}
    RELEASE(pSurf);
    return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::UpdateTexture(UINT uLevel, CUBEMAP_FACES Face, CImage *pImg, DWORD dwTexLoad)
{
	HRESULT     hr;
    SURFACEDESC Desc;
    UINT        uWidth;
	CSurface   *pSurf = NULL;
	CImage     *pScaledImg = NULL;
	DWORD       dwLoad = dwTexLoad;

    if (m_dwLevelCount < uLevel)
		return E_FAIL;

	// Calculate the level ranges
    UINT uStartLevel = uLevel == 0 ? 1 : uLevel;
	UINT uEndLevel = uLevel == 0 ? m_dwLevelCount : uStartLevel;
    UINT uStartFaces = Face == CUBEMAP_FACE_ALL ? 0 : (UINT)Face;
	UINT uEndFaces = Face == CUBEMAP_FACE_ALL ? 5 : (UINT)Face;

    // Get the texture DESC
    hr = GetLevelDesc(uStartLevel-1, &Desc);
	if(FAILED(hr))
		goto Exit;
	uWidth = Desc.Width;
   	
	for(int j=uStartLevel; j <= uEndLevel; j++)
	{
		if (uWidth != pImg->GetWidth() || uWidth != pImg->GetHeight())
		{
			//create a local copy of the image
			pScaledImg = new CImage();
			if ( pScaledImg == NULL ||
				!pScaledImg->Copy(pImg) || !pScaledImg->ScaleData(uWidth, uWidth))
			{
				hr = E_OUTOFMEMORY;
				goto Exit;
			}
		}

		for(int i=uStartFaces; i <= uEndFaces; i++)
		{
			// If there is no sysmem copy, attempt to load the surface directly
			if(NULL == m_pSysTexture)
				hr = GetCubeMapSurface((CUBEMAP_FACES)i, j-1, &pSurf);
			else
				hr = ((CCubeTexture*)m_pSysTexture)->GetCubeMapSurface((CUBEMAP_FACES)i, j-1, &pSurf);
			if(FAILED(hr))
				goto Exit;

			hr = pSurf->UpdateSurface(pScaledImg ? pScaledImg : pImg);
			if(FAILED(hr))
				goto Exit;
			RELEASE(pSurf);
		}
		uWidth /= 2;
		SAFEDELETE(pScaledImg);
	}

	// Update the target texture
	if (m_pSysTexture)	
	{
		if(!BltSupported(&Desc.Format, Desc.Pool) || dwLoad == TEXLOAD_LOCKCOPY)
		{
			m_pDevice->SetTexLoad(TEXLOAD_LOCKCOPY);
			hr = LockCopy((CCubeTexture*)m_pSysTexture);
		}
		else if(dwLoad == TEXLOAD_UPDATETEXTURE)
		{
			// If we're updating a sub-cube, we need to tell the runtime to update all
			// since they only keep dirty-region info for Level 0.
			if(uLevel > 1)
				((CCubeTexture*)m_pSysTexture)->AddDirtyRect(Face, NULL);

			m_pDevice->SetTexLoad(TEXLOAD_UPDATETEXTURE);
			hr = m_pDevice->UpdateTexture(m_pSysTexture, this);
		}
		else if(dwLoad == TEXLOAD_COPYRECTS)
		{	
			m_pDevice->SetTexLoad(TEXLOAD_COPYRECTS);
			hr = CopyRects((CCubeTexture*)m_pSysTexture);
		}
		else if(dwLoad == TEXLOAD_UPDATESURFACE)
		{	
			m_pDevice->SetTexLoad(TEXLOAD_UPDATESURFACE);
			hr = UpdateSurface((CCubeTexture*)m_pSysTexture);
		}
		else
		{
			DPF(1, _T("CCubeTexture::UpdateTexture() - requested TEXLOAD(%d) not supported.\n"), dwLoad);
			hr = E_FAIL;
		}
		if(FAILED(hr))
			goto Exit;
	}

Exit:
	if(FAILED(hr))
	{
		DPF(1, _T("CCubeTexture::UpdateTexture() - failed to load texture.  hr=%s(%x).\n"),
            m_pDevice->m_pD3D->HResultToString(hr), hr);
	}
    RELEASE(pSurf);
	SAFEDELETE(pScaledImg);
    return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::UpdateTexture(CImage ***pImg, DWORD dwTexLoad)
{
	HRESULT hr;
    CSurface *pSurf = NULL;
    SURFACEDESC Desc;
    DWORD dwLoad = dwTexLoad;

	// Get the texture DESC
    hr = GetLevelDesc(0, &Desc);
	
	if(FAILED(hr))
	   return hr;

    for(int i=0; i < 6; i++)
	for(int j=0; j < m_dwLevelCount; j++)
	{
			// If there is no sysmem copy, attempt to load the surface directly
			if(NULL == m_pSysTexture)
				hr = GetCubeMapSurface((CUBEMAP_FACES)i, j, &pSurf);
			else
        		hr = ((CCubeTexture*)m_pSysTexture)->GetCubeMapSurface((CUBEMAP_FACES)i, j, &pSurf);
		if(FAILED(hr))
			goto Exit;

		hr = pSurf->UpdateSurface(pImg[j][i]);
		if(FAILED(hr))
			goto Exit;

		RELEASE(pSurf);
	}

	if (m_pSysTexture)	
	{
	    // Update the target texture
	    if(!BltSupported(&Desc.Format, Desc.Pool) || dwLoad == TEXLOAD_LOCKCOPY)
	    {
		    m_pDevice->SetTexLoad(TEXLOAD_LOCKCOPY);
		    hr = LockCopy((CCubeTexture*)m_pSysTexture);
	    }
	    else if(dwLoad == TEXLOAD_UPDATETEXTURE)
        {
		    m_pDevice->SetTexLoad(TEXLOAD_UPDATETEXTURE);
		    hr = m_pDevice->UpdateTexture(m_pSysTexture, this);
	    }
	    else if(dwLoad == TEXLOAD_COPYRECTS)
	    {	
		    m_pDevice->SetTexLoad(TEXLOAD_COPYRECTS);
		    hr = CopyRects((CCubeTexture*)m_pSysTexture);
	    }
	    else if(dwLoad == TEXLOAD_UPDATESURFACE)
	    {	
		    m_pDevice->SetTexLoad(TEXLOAD_UPDATESURFACE);
		    hr = UpdateSurface((CCubeTexture*)m_pSysTexture);
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

HRESULT CCubeTexture::LoadImage(UINT uLevel, CImage **pImg, DWORD dwTexLoad)
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

HRESULT CCubeTexture::LoadImage(CImage ***pImg, DWORD dwTexLoad)
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

    hr = UpdateTexture(pImg, dwLoad);

Exit:
	return m_pDevice->GetD3D()->SetLastError(hr);
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::LoadTexture(UINT uLevel, CUBEMAP_FACES Face, LPCTSTR pFilename, float fAlphaOverride, DWORD dwTexLoad)
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

	if(!pImage->Load(pFilename, fAlphaOverride))
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

    hr = UpdateTexture(uLevel, Face, pImage, dwLoad);

Exit:
	SAFEDELETE(pImage);
	return m_pDevice->GetD3D()->SetLastError(hr);
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::LoadStripes(UINT uLevel, CUBEMAP_FACES Face, DWORD dwW, DWORD dwH, DWORD dwStripes, DWORD dwColorArray[], bool  bHorizontal, bool  bDiagonal, DWORD dwTexLoad)
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

    hr = UpdateTexture(uLevel, Face, pImage, dwLoad);

Exit:
	SAFEDELETE(pImage);
	return m_pDevice->GetD3D()->SetLastError(hr);
}

// -------------------------------------------------------------------------------------

HRESULT	CCubeTexture::LoadGradient(UINT uLevel, CUBEMAP_FACES Face, DWORD dwW,DWORD dwH,DWORD dwColors[4], DWORD dwTexLoad)
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

    hr = UpdateTexture(uLevel, Face, pImage, dwLoad);

Exit:
	SAFEDELETE(pImage);
	return m_pDevice->GetD3D()->SetLastError(hr);
}
