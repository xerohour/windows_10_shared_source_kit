#include "TexturesPCH.h"

#include "CTextures.h"

// -------------------------------------------------------------------------------------

CVolTexture::CVolTexture()
{
	m_ResourceType = RTYPE_VOLUMETEXTURE;
	m_dwLevelCount = 0;
	m_pDevice = 0L;
	m_bPalettized = false;
	pReleaseD3D = NULL;
    m_pTexture8 = NULL;
    m_pTexture9 = NULL;
}

// -------------------------------------------------------------------------------------

CVolTexture::~CVolTexture()
{
	if(pReleaseD3D)
		ReleaseD3D();
}

// -------------------------------------------------------------------------------------

ULONG WINAPI CVolTexture::AddRef(void)
{
    if(m_pDevice)
		m_pDevice->AddRef();
	return CObject::AddRef();
}

// -------------------------------------------------------------------------------------

ULONG WINAPI CVolTexture::Release(void)
{
    if(m_pDevice)
		m_pDevice->Release();
	return CObject::Release();
}

// -------------------------------------------------------------------------------------

bool CVolTexture::Init(LPVOID pTexture, CDevice *pDevice)
{
	bool bRet = false;
    HRESULT hr;
    VOLUME_DESC Desc;

	if (NULL == pDevice)
		return false;

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	if (8.0f <= m_pDevice->GetInterface() && 9.0f > m_pDevice->GetInterface())
	{
                pSetPriority    = &CVolTexture::SetPriority8;
                pGetPriority    = &CVolTexture::GetPriority8;
                pPreLoad        = &CVolTexture::PreLoad8;
                pSetLOD         = &CVolTexture::SetLOD8;
                pGetLOD         = &CVolTexture::GetLOD8;
                pGetLevelDesc   = &CVolTexture::GetLevelDesc8;
                pGetVolumeLevel = &CVolTexture::GetVolumeLevel8;
                pLockBox        = &CVolTexture::LockBox8;
                pUnlockBox      = &CVolTexture::UnlockBox8;
                pAddDirtyBox    = &CVolTexture::AddDirtyBox8;
                pReleaseD3D     = &CVolTexture::ReleaseD3D8;
                pSetPalette             = &CVolTexture::SetPalette8;
        pGetDevice      = &CVolTexture::GetDevice8;
        pFreePrivateData = &CVolTexture::FreePrivateData8;
        pGetPrivateData = &CVolTexture::GetPrivateData8;
        pSetPrivateData = &CVolTexture::SetPrivateData8;
        pSetAutoGenFilterType = &CVolTexture::SetAutoGenFilterTypeUnsup;
        pGetAutoGenFilterType = &CVolTexture::GetAutoGenFilterTypeUnsup;
                pGenerateMipSubLevels = &CVolTexture::GenerateMipSubLevelsUnsup;
		bRet = Init8( pTexture );
	}
    else if (9.0f <= m_pDevice->GetInterface() && 10.0f > m_pDevice->GetInterface())
	{
                pSetPriority    = &CVolTexture::SetPriority9;
                pGetPriority    = &CVolTexture::GetPriority9;
                pPreLoad        = &CVolTexture::PreLoad9;
                pSetLOD         = &CVolTexture::SetLOD9;
                pGetLOD         = &CVolTexture::GetLOD9;
                pGetLevelDesc   = &CVolTexture::GetLevelDesc9;
                pGetVolumeLevel = &CVolTexture::GetVolumeLevel9;
                pLockBox        = &CVolTexture::LockBox9;
                pUnlockBox      = &CVolTexture::UnlockBox9;
                pAddDirtyBox    = &CVolTexture::AddDirtyBox9;
                pReleaseD3D     = &CVolTexture::ReleaseD3D9;
                pSetPalette             = &CVolTexture::SetPalette9;
        pGetDevice      = &CVolTexture::GetDevice9;
        pFreePrivateData = &CVolTexture::FreePrivateData9;
        pGetPrivateData = &CVolTexture::GetPrivateData9;
        pSetPrivateData = &CVolTexture::SetPrivateData9;
        pSetAutoGenFilterType = &CVolTexture::SetAutoGenFilterType9;
        pGetAutoGenFilterType = &CVolTexture::GetAutoGenFilterType9;
                pGenerateMipSubLevels = &CVolTexture::GenerateMipSubLevels9;
		bRet = Init9( pTexture );
	}
	else
		return false;

    // Gather some info about this texture
    hr = GetLevelDesc(0, &Desc);
	if(FAILED(hr))
	{
		DPF(1, _T("CVolTexture::Init() - GetLevelDesc failed. hr=%s(%x).\n"),
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

HRESULT CVolTexture::SetAutoGenFilterTypeUnsup(DWORD FilterType)
{
	DebugOut.Write(1, "CVolTexture: SetAutoGenFilterType() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return E_FAIL;
}

// -------------------------------------------------------------------------------------

DWORD CVolTexture::GetAutoGenFilterTypeUnsup(void)
{
	DebugOut.Write(1, "CVolTexture: GetAutoGenFilterType() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
	return (DWORD)0;
}

// -------------------------------------------------------------------------------------

void CVolTexture::GenerateMipSubLevelsUnsup(void)
{
	DebugOut.Write(1, "CVolTexture: GenerateMipSubLevels() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::LockCopy(CVolTexture* pSrcTex)
{
	HRESULT hr = 0;
	CVolume *pTargetVol = NULL, *pSrcVol = NULL;
    
	for(int i=0; i<m_dwLevelCount; i++)
	{
		// Get the target volume
		hr = GetVolumeLevel(i, &pTargetVol);
		if(FAILED(hr))
			break;

		// Get the src volume
		hr = pSrcTex->GetVolumeLevel(i, &pSrcVol);
		if(FAILED(hr))
			break;

		// Load the target volume from the src volume
		hr = pTargetVol->UpdateVolume(pSrcVol);

		if(FAILED(hr))
			break;

		RELEASE(pTargetVol);
		RELEASE(pSrcVol);
	}

	RELEASE(pTargetVol);
	RELEASE(pSrcVol);

	return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::UpdateVolume(UINT uLevel, UINT uSlice, CImage *pImg, DWORD dwTexLoad)
{
	HRESULT  hr = S_OK;
    CVolume *pVol = NULL;
	    
	if (m_dwLevelCount < uLevel)
		return E_FAIL;
	
	// Calculate the level range
	UINT uStartLevel = uLevel == 0 ? 1 : uLevel;
	UINT uEndLevel = uLevel == 0 ? m_dwLevelCount : uStartLevel;
    UINT uCurrentSlice = uSlice;

	for(uStartLevel; uStartLevel <= uEndLevel; uStartLevel++)
	{
		// Load the images into the textures
		// If there is no sysmem copy, attempt to load the surface directly
		if(NULL == m_pSysTexture)
			hr = GetVolumeLevel(uStartLevel-1, &pVol);
		else
			hr = ((CVolTexture*)m_pSysTexture)->GetVolumeLevel(uStartLevel-1, &pVol);
		if(FAILED(hr))
			goto Exit;
	
		hr = pVol->UpdateVolume(uCurrentSlice, pImg);
		if(FAILED(hr))
			goto Exit;

		RELEASE(pVol);
		uCurrentSlice /= 2;
	}

	// Update the target texture
	if(m_pSysTexture)
	{
		if(dwTexLoad == TEXLOAD_UPDATETEXTURE)
		{
			// If we're updating a sub-volume, we need to tell the runtime to update all
			// since they only keep dirty-region info for Level 0.
			if(uLevel > 1)
				((CVolTexture*)m_pSysTexture)->AddDirtyBox(NULL);

			m_pDevice->SetTexLoad(TEXLOAD_UPDATETEXTURE);
			hr = m_pDevice->UpdateTexture(m_pSysTexture, this);
		}
		else if(dwTexLoad == TEXLOAD_LOCKCOPY)
		{
			m_pDevice->SetTexLoad(TEXLOAD_LOCKCOPY);
			hr = LockCopy((CVolTexture*)m_pSysTexture);
		}
		else
		{
			DPF(1, _T("CVolTexture::UpdateTexture() - requested TEXLOAD(%d) not supported.\n"), dwTexLoad);
			hr = E_FAIL;
		}
		if(FAILED(hr))
			goto Exit;
	}

Exit:
	if(FAILED(hr))
	{
		DPF(1, _T("CVolTexture::UpdateTexture() - failed to load texture.  hr=%s(%x).\n"),
            m_pDevice->m_pD3D->HResultToString(hr), hr);
	}
    RELEASE(pVol);
    return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::UpdateVolume(UINT uLevel, CImage **pImg, DWORD dwTexLoad)
{
	HRESULT hr = S_OK;
    CVolume *pVol = NULL;
	    
	if (m_dwLevelCount < uLevel )
		return E_FAIL;
	
	// Calculate the level range
	UINT uStartLevel = uLevel == 0 ? 1 : uLevel;
	UINT uEndLevel = uLevel == 0 ? m_dwLevelCount : uStartLevel;
    
	for(uStartLevel; uStartLevel <= uEndLevel; uStartLevel++)
	{
		// Load the images into the textures
		// If there is no sysmem copy, attempt to load the surface directly
		if(NULL == m_pSysTexture)			
			hr = GetVolumeLevel(uStartLevel-1, &pVol);
		else
			hr = ((CVolTexture*)m_pSysTexture)->GetVolumeLevel(uStartLevel-1, &pVol);
		if(FAILED(hr))
			goto Exit;
		
		hr = pVol->UpdateVolume(pImg);
		if(FAILED(hr))
			goto Exit;
	       
		RELEASE(pVol);
	}

	// Update the target texture
	if (m_pSysTexture)
	{
		if(dwTexLoad == TEXLOAD_UPDATETEXTURE)
		{
			// If we're updating a sub-volume, we need to tell the runtime to update all
			// since they only keep dirty-region info for Level 0.
			if(uLevel > 1)
				((CVolTexture*)m_pSysTexture)->AddDirtyBox(NULL);

			m_pDevice->SetTexLoad(TEXLOAD_UPDATETEXTURE);
			hr = m_pDevice->UpdateTexture(m_pSysTexture, this);
		}
		else if(dwTexLoad == TEXLOAD_LOCKCOPY)
		{
			m_pDevice->SetTexLoad(TEXLOAD_LOCKCOPY);
			hr = LockCopy((CVolTexture*)m_pSysTexture);
		}
		else
		{
			DPF(1, _T("CVolTexture::UpdateTexture() - requested TEXLOAD(%d) not supported.\n"), dwTexLoad);
			hr = E_FAIL;
		}
		if(FAILED(hr))
			goto Exit;
	}

Exit:
	if(FAILED(hr))
	{
		DPF(1, _T("CVolTexture::UpdateTexture() - failed to load texture.  hr=%s(%x).\n"),
            m_pDevice->m_pD3D->HResultToString(hr), hr);
	}
    RELEASE(pVol);
    return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::UpdateVolume(CImage ***pImg, DWORD dwTexLoad)
{
	HRESULT hr = S_OK;
    CVolume *pVol = NULL;
	    
	if (pImg == NULL)
		return E_FAIL;
	
	for(int i=0; i<m_dwLevelCount; i++)
	{
		// Load the images into the textures
		// If there is no sysmem copy, attempt to load the surface directly
		if(NULL == m_pSysTexture)			
			hr = GetVolumeLevel(i, &pVol);
		else
    		hr = ((CVolTexture*)m_pSysTexture)->GetVolumeLevel(i, &pVol);
    
		if(FAILED(hr))
			goto Exit;
	
		hr = pVol->UpdateVolume(pImg[i]);
        
		RELEASE(pVol);

		if(FAILED(hr))
			goto Exit;
	}

	if (m_pSysTexture)
	{
	    // Update the target texture
	    if(dwTexLoad == TEXLOAD_UPDATETEXTURE)
	    {
		    m_pDevice->SetTexLoad(TEXLOAD_UPDATETEXTURE);
		    hr = m_pDevice->UpdateTexture(m_pSysTexture, this);
	    }
	    else if(dwTexLoad == TEXLOAD_LOCKCOPY)
	    {
		    m_pDevice->SetTexLoad(TEXLOAD_LOCKCOPY);
		    hr = LockCopy((CVolTexture*)m_pSysTexture);
	    }
	    else
	    {
		    DPF(1, _T("CVolTexture::UpdateTexture() - requested TEXLOAD(%d) not supported.\n"), dwTexLoad);
		    hr = E_FAIL;
	    }
    }
Exit:
	if(FAILED(hr))
	{
		DPF(1, _T("CVolTexture::UpdateTexture() - failed to load texture.  hr=%s(%x).\n"),
            m_pDevice->m_pD3D->HResultToString(hr), hr);
	}
	RELEASE(pVol);

    return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::LoadImage(UINT uLevel, CImage **pImg, DWORD dwTexLoad)
{
	HRESULT hr = S_OK;
	VOLUME_DESC Desc;
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

	hr = UpdateVolume(uLevel, pImg, dwLoad);

Exit:
	return m_pDevice->GetD3D()->SetLastError(hr);
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::LoadImage(CImage ***pImg, DWORD dwTexLoad)
{
	HRESULT hr = S_OK;
	VOLUME_DESC Desc;
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

    hr = UpdateVolume(pImg, dwLoad);

Exit:
	return m_pDevice->GetD3D()->SetLastError(hr);
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::LoadTexture(UINT uLevel, UINT uSlice, LPCTSTR pFilename, float fAlphaOverride, DWORD dwTexLoad)
{
	CImage *pImage = NULL;
    HRESULT hr = S_OK;
	VOLUME_DESC Desc;
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

    hr = UpdateVolume(uLevel, uSlice, pImage, dwLoad);

Exit:
	SAFEDELETE(pImage);
	return m_pDevice->GetD3D()->SetLastError(hr);
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::LoadStripes(UINT uLevel, UINT uSlice, DWORD dwW, DWORD dwH, DWORD dwStripes, DWORD dwColorArray[], bool  bHorizontal, bool  bDiagonal, DWORD dwTexLoad)
{
	CImage *pImage = NULL;
    HRESULT hr = S_OK;
	VOLUME_DESC Desc;
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

    hr = UpdateVolume(uLevel, uSlice, pImage, dwLoad);

Exit:
	SAFEDELETE(pImage);
	return m_pDevice->GetD3D()->SetLastError(hr);
}

// -------------------------------------------------------------------------------------

HRESULT	CVolTexture::LoadGradient(UINT uLevel, UINT uSlice, DWORD dwW,DWORD dwH,DWORD dwColors[4], DWORD dwTexLoad)
{
	CImage *pImage = NULL;
    HRESULT hr = S_OK;
	VOLUME_DESC Desc;
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

    hr = UpdateVolume(uLevel, uSlice, pImage, dwLoad);

Exit:
	SAFEDELETE(pImage);
	return m_pDevice->GetD3D()->SetLastError(hr);
}
