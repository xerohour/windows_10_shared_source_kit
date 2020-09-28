#include "TexturesPCH.h"

#include <d3d9.h>
#include "CTextures.h"

// --------------------------------------------------------------------------------------

bool CTexture::Init9(PVOID pTex)
{
    LPDIRECT3DTEXTURE9 pTexture = (LPDIRECT3DTEXTURE9)pTex;

	if(pTexture == NULL)
	{
		DPF(1, _T("CTexture::Init9() - invalid params.\n"));
		return false;
	}

	//This is important as Dx will accept 0 and create all levels.
	m_dwLevelCount = pTexture->GetLevelCount();

	m_pTexture9 = pTexture;
	m_pTexture9->AddRef();

	return true;
}

// --------------------------------------------------------------------------------------

void CTexture::ReleaseD3D9()
{
	RELEASE(m_pSysTexture);
	RELEASE(m_pTexture9);
}

// --------------------------------------------------------------------------------------

HRESULT CTexture::AddDirtyRect9(CONST RECT* pDirtyRect)
{
	if (NULL == m_pTexture9)
		return E_FAIL;

	return ((IDirect3DTexture9*)m_pTexture9)->AddDirtyRect(pDirtyRect);
}

// --------------------------------------------------------------------------------------

DWORD CTexture::SetLOD9(DWORD dwLODNew)
{
	if (NULL == m_pTexture9)
		return 0;
	return m_pTexture9->SetLOD(dwLODNew);
}

// --------------------------------------------------------------------------------------


DWORD CTexture::GetLOD9()
{
	if (NULL == m_pTexture9)
		return 0;
	return m_pTexture9->GetLOD();
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::SetAutoGenFilterType9(DWORD FilterType)
{
	if (NULL == m_pTexture9)
		return E_FAIL;
	return m_pTexture9->SetAutoGenFilterType((D3DTEXTUREFILTERTYPE)FilterType);
}

// -------------------------------------------------------------------------------------

DWORD CTexture::GetAutoGenFilterType9(void)
{
	if (NULL == m_pTexture9)
		return 0;
	return m_pTexture9->GetAutoGenFilterType();
}

// -------------------------------------------------------------------------------------

void CTexture::GenerateMipSubLevels9(void)
{
	if (NULL == m_pTexture9)
		return;
	m_pTexture9->GenerateMipSubLevels();
}

// --------------------------------------------------------------------------------------

void CTexture::PreLoad9()
{
	if (NULL == m_pTexture9)
		return;
	m_pTexture9->PreLoad();
}

// --------------------------------------------------------------------------------------

HRESULT CTexture::SetPalette9(PALETTEENTRY *pPalette)
{
	HRESULT hr = 0;
	PALETTEENTRY *pPal = pPalette == NULL ? m_pDevice->m_Palette8 : pPalette;

	switch(m_Format.d3dfFormat)
	{
	case D3DFMT_A8P8:
    case D3DFMT_P8:
        hr = m_pDevice->SetPaletteEntries(0, pPal);
		break;
	default:
		DPF(1, _T("CTexture::SetPalette9() - Format is not palettized.\n"));
		return E_FAIL;
	}

	if(FAILED(hr))
	{
		return hr;
	}

	hr = m_pDevice->SetCurrentTexturePalette(0);
    if(FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CTexture::LockRect9(UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect,DWORD dwFlags)
{
	HRESULT hr = 0;
	D3DLOCKED_RECT lr;

	if( 0L == pLockedRect )
		return ((LPDIRECT3DTEXTURE9)m_pTexture9)->LockRect(uLevel, 0L, pRect, dwFlags );

	hr = ((LPDIRECT3DTEXTURE9)m_pTexture9)->LockRect(uLevel, &lr, pRect, dwFlags);
	pLockedRect->iPitch = lr.Pitch;
	pLockedRect->pBits = lr.pBits;

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CTexture::UnlockRect9(UINT uLevel)
{
	return ((LPDIRECT3DTEXTURE9)m_pTexture9)->UnlockRect(uLevel);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::GetLevelDesc9(UINT uLevel, SURFACEDESC *pDesc)
{
	HRESULT hr;
	D3DSURFACE_DESC D3DDesc;

	if(NULL == pDesc)
		return E_INVALIDARG;

    ZeroMemory( pDesc, sizeof(*pDesc) );

    hr = ((LPDIRECT3DTEXTURE9)m_pTexture9)->GetLevelDesc(uLevel, &D3DDesc);
	if(FAILED(hr))
		return hr;

    //Fill in the version independent struct.
	pDesc->Width = D3DDesc.Width;
	pDesc->Height = D3DDesc.Height;
	pDesc->Format.d3dfFormat = D3DDesc.Format;
	pDesc->Type = D3DDesc.Type;
	pDesc->Usage = (DWORD)D3DDesc.Usage;
	pDesc->Pool = (DWORD)D3DDesc.Pool;
	pDesc->Size = 0;
	pDesc->MultiSampleType = (MULTISAMPLE_TYPE)D3DDesc.MultiSampleType;
    pDesc->MultiSampleQuality = D3DDesc.MultiSampleQuality;
	d3df2ddpf(D3DDesc.Format, &(pDesc->Format.ddpfPixelFormat));
	pDesc->dwFlags = pDesc->Format.ddpfPixelFormat.dwFlags;
	pDesc->lPitch = (pDesc->Format.ddpfPixelFormat.dwRGBBitCount / 8) * pDesc->dwWidth;

	return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::GetSurfaceLevel9(UINT uLevel, CSurface** ppSurfaceLevel)
{
	HRESULT hr;
	LPDIRECT3DSURFACE9 pSurf = NULL;
	CSurface *pCSurface = NULL;
	
	hr = ((LPDIRECT3DTEXTURE9)m_pTexture9)->GetSurfaceLevel(uLevel, ppSurfaceLevel == NULL ? NULL : &pSurf);
	if(FAILED(hr))
		return hr;

    pCSurface = new CSurface();
	if(NULL == pCSurface)
	{
		RELEASE(pSurf);
		return E_OUTOFMEMORY;
	}

	if(!pCSurface->Init(pSurf, m_pDevice, this))
	{
		RELEASE(pSurf);
		RELEASE(pCSurface);
		return E_FAIL;
	}

	RELEASE(pSurf);

	if (ppSurfaceLevel)
		*ppSurfaceLevel = pCSurface;

	return hr;
}

// -------------------------------------------------------------------------------------

DWORD CTexture::SetPriority9(DWORD dwNewPriority)
{
	if (NULL == m_pTexture9)
		return 0;

	return m_pTexture9->SetPriority(dwNewPriority);
}

// -------------------------------------------------------------------------------------

DWORD CTexture::GetPriority9(void)
{
	if (NULL == m_pTexture9)
		return 0;

	return m_pTexture9->GetPriority();
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::GetDevice9(CDevice** ppDevice)
{
	IDirect3DDevice9 *pDev = NULL;
	HRESULT hr;

    hr = ((LPDIRECT3DTEXTURE9)m_pTexture9)->GetDevice( PassPointer( &pDev, ppDevice) );

	if (SUCCEEDED(hr))
	{
		if (pDev != m_pDevice->GetDevicePointer9())
		{
			ppDevice = NULL;
		}

		if (ppDevice)
		{
			*ppDevice = m_pDevice; 
			m_pDevice->AddRef();
		}
		RELEASE(pDev);
		return S_OK; 
	}
	else
	{
		ppDevice = NULL;
		RELEASE(pDev);
		return hr;
	}
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::FreePrivateData9(REFGUID refguid)
{
	return ((LPDIRECT3DTEXTURE9)m_pTexture9)->FreePrivateData(refguid);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::GetPrivateData9(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return ((LPDIRECT3DTEXTURE9)m_pTexture9)->GetPrivateData(refguid, pData, pSizeOfData);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::SetPrivateData9(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags)
{
	return ((LPDIRECT3DTEXTURE9)m_pTexture9)->SetPrivateData(refguid, pData, dwSizeOfData, dwFlags);
}
