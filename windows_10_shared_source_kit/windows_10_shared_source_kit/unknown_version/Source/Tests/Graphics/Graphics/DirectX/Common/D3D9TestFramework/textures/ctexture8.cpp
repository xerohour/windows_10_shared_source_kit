#include "TexturesPCH.h"

#include <d3d8.h>
#include "CTextures.h"

// --------------------------------------------------------------------------------------

bool CTexture::Init8(PVOID pTex)
{
    LPDIRECT3DTEXTURE8 pTexture = (LPDIRECT3DTEXTURE8)pTex;

	if(pTexture == NULL)
	{
		DPF(1, _T("CTexture::Init8() - invalid params.\n"));
		return false;
	}

	//This is important as Dx will accept 0 and create all levels.
	m_dwLevelCount = pTexture->GetLevelCount();

	m_pTexture8 = pTexture;
	m_pTexture8->AddRef();

	return true;
}

// --------------------------------------------------------------------------------------

void CTexture::ReleaseD3D8()
{
	RELEASE(m_pSysTexture);
	RELEASE(m_pTexture8);
}

// --------------------------------------------------------------------------------------

HRESULT CTexture::AddDirtyRect8(CONST RECT* pDirtyRect)
{
	if (NULL == m_pTexture8)
		return 0;

	return ((IDirect3DTexture8*)m_pTexture8)->AddDirtyRect(pDirtyRect);
}

// --------------------------------------------------------------------------------------

void CTexture::PreLoad8()
{
	if (NULL == m_pTexture8)
		return;
	m_pTexture8->PreLoad();
}

// --------------------------------------------------------------------------------------

DWORD CTexture::SetLOD8(DWORD dwLODNew)
{
	if (NULL == m_pTexture8)
		return 0;
	return m_pTexture8->SetLOD(dwLODNew);
}

// --------------------------------------------------------------------------------------


DWORD CTexture::GetLOD8()
{
	if (NULL == m_pTexture8)
		return 0;
	return m_pTexture8->GetLOD();
}

// -------------------------------------------------------------------------------------

DWORD CTexture::SetPriority8(DWORD dwNewPriority)
{
	if (NULL == m_pTexture8)
		return 0;

	return m_pTexture8->SetPriority(dwNewPriority);
}

// -------------------------------------------------------------------------------------

DWORD CTexture::GetPriority8(void)
{
	if (NULL == m_pTexture8)
		return 0;

	return m_pTexture8->GetPriority();
}

// --------------------------------------------------------------------------------------

HRESULT CTexture::SetPalette8(PALETTEENTRY *pPalette)
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
		DPF(1, _T("CTexture::SetPalette8() - Format is not palettized.\n"));
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

HRESULT CTexture::LockRect8(UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect,DWORD dwFlags)
{
	HRESULT hr = 0;
	D3DLOCKED_RECT lr;

	if( 0L == pLockedRect )
		return ((LPDIRECT3DTEXTURE8)m_pTexture8)->LockRect(uLevel, 0L, pRect, dwFlags );

	hr = ((LPDIRECT3DTEXTURE8)m_pTexture8)->LockRect(uLevel, &lr, pRect, dwFlags);
	pLockedRect->iPitch = lr.Pitch;
	pLockedRect->pBits = lr.pBits;

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CTexture::UnlockRect8(UINT uLevel)
{
	return ((LPDIRECT3DTEXTURE8)m_pTexture8)->UnlockRect(uLevel);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::GetLevelDesc8(UINT uLevel, SURFACEDESC *pDesc)
{
	HRESULT hr;
	D3DSURFACE_DESC D3DDesc;

	if(NULL == pDesc)
		return E_INVALIDARG;

    ZeroMemory( pDesc, sizeof(*pDesc) );

    hr = ((LPDIRECT3DTEXTURE8)m_pTexture8)->GetLevelDesc(uLevel, &D3DDesc);
	if(FAILED(hr))
		return hr;

    //Fill in the version independent struct.
	pDesc->Width = D3DDesc.Width;
	pDesc->Height = D3DDesc.Height;
	pDesc->Format.d3dfFormat = D3DDesc.Format;
	pDesc->Type = D3DDesc.Type;
	pDesc->Usage = (DWORD)D3DDesc.Usage;
	pDesc->Pool = (DWORD)D3DDesc.Pool;
	pDesc->Size = D3DDesc.Size;
	pDesc->MultiSampleType = (MULTISAMPLE_TYPE)D3DDesc.MultiSampleType;
	d3df2ddpf(D3DDesc.Format, &(pDesc->Format.ddpfPixelFormat));
	pDesc->dwFlags = pDesc->Format.ddpfPixelFormat.dwFlags;
	pDesc->lPitch = (pDesc->Format.ddpfPixelFormat.dwRGBBitCount / 8) * pDesc->dwWidth;

	return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::GetSurfaceLevel8(UINT uLevel, CSurface** ppSurfaceLevel)
{
	HRESULT hr;
	LPDIRECT3DSURFACE8 pSurf = NULL;
	CSurface *pCSurface = NULL;
	
	hr = ((LPDIRECT3DTEXTURE8)m_pTexture8)->GetSurfaceLevel(uLevel, ppSurfaceLevel == NULL ? NULL : &pSurf);
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

HRESULT CTexture::GetDevice8(CDevice** ppDevice)
{
	IDirect3DDevice8 *pDev = NULL;
	HRESULT hr;

    hr = ((LPDIRECT3DTEXTURE8)m_pTexture8)->GetDevice( PassPointer( &pDev, ppDevice) );

	if (SUCCEEDED(hr))
	{
		if (pDev != m_pDevice->GetDevicePointer8())
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

HRESULT CTexture::FreePrivateData8(REFGUID refguid)
{
	return ((LPDIRECT3DTEXTURE8)m_pTexture8)->FreePrivateData(refguid);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::GetPrivateData8(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return ((LPDIRECT3DTEXTURE8)m_pTexture8)->GetPrivateData(refguid, pData, pSizeOfData);
}

// -------------------------------------------------------------------------------------

HRESULT CTexture::SetPrivateData8(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags)
{
	return ((LPDIRECT3DTEXTURE8)m_pTexture8)->SetPrivateData(refguid, pData, dwSizeOfData, dwFlags);
}
