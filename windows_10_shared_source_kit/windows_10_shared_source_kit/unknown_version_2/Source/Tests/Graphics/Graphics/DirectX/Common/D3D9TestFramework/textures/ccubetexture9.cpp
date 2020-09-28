#include "TexturesPCH.h"

#include <d3d9.h>
#include "CTextures.h"


bool CCubeTexture::Init9(LPVOID pTexture)
{
	m_pTexture9 = (LPDIRECT3DCUBETEXTURE9)pTexture;
	m_pTexture9->AddRef();
	m_dwLevelCount = ((LPDIRECT3DCUBETEXTURE9)pTexture)->GetLevelCount();	
	return true;
}


// --------------------------------------------------------------------------------------

void CCubeTexture::ReleaseD3D9()
{
	RELEASE(m_pTexture9);
	RELEASE(m_pSysTexture);
}

// --------------------------------------------------------------------------------------

HRESULT CCubeTexture::AddDirtyRect9(CUBEMAP_FACES FaceType, CONST RECT* pDirtyRect)
{
	if (NULL == m_pTexture9)
		return 0;

	return ((IDirect3DCubeTexture9*)m_pTexture9)->AddDirtyRect((D3DCUBEMAP_FACES)FaceType, pDirtyRect);
}

// --------------------------------------------------------------------------------------

DWORD CCubeTexture::SetLOD9(DWORD dwLODNew)
{
	if (NULL == m_pTexture9)
		return 0;
	return m_pTexture9->SetLOD(dwLODNew);
}

// --------------------------------------------------------------------------------------


DWORD CCubeTexture::GetLOD9()
{
	if (NULL == m_pTexture9)
		return 0;
	return m_pTexture9->GetLOD();
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::SetAutoGenFilterType9(DWORD FilterType)
{
	if (NULL == m_pTexture9)
		return E_FAIL;
	return m_pTexture9->SetAutoGenFilterType((D3DTEXTUREFILTERTYPE)FilterType);
}

// -------------------------------------------------------------------------------------

DWORD CCubeTexture::GetAutoGenFilterType9(void)
{
	if (NULL == m_pTexture9)
		return 0;
	return m_pTexture9->GetAutoGenFilterType();
}

// -------------------------------------------------------------------------------------

void CCubeTexture::GenerateMipSubLevels9(void)
{
	if (NULL == m_pTexture9)
		return;
	m_pTexture9->GenerateMipSubLevels();
}

// --------------------------------------------------------------------------------------

void CCubeTexture::PreLoad9()
{
	if (NULL == m_pTexture9)
		return;
	m_pTexture9->PreLoad();
}

// -------------------------------------------------------------------------------------

DWORD CCubeTexture::SetPriority9(DWORD dwNewPriority)
{
	return ((LPDIRECT3DCUBETEXTURE9)m_pTexture9)->SetPriority(dwNewPriority);
}

// -------------------------------------------------------------------------------------

DWORD CCubeTexture::GetPriority9(void)
{
	return ((LPDIRECT3DCUBETEXTURE9)m_pTexture9)->GetPriority();
}

// --------------------------------------------------------------------------------------

HRESULT CCubeTexture::GetCubeMapSurface9(CUBEMAP_FACES FaceType, UINT uLevel, CSurface** ppCubeMapSurface)
{
	HRESULT hr;
	CSurface *pSurf = NULL;
	LPDIRECT3DSURFACE9 pSurf9 = NULL;

	hr = ((LPDIRECT3DCUBETEXTURE9)m_pTexture9)->GetCubeMapSurface((D3DCUBEMAP_FACES)FaceType, uLevel, ppCubeMapSurface == NULL ? NULL : &pSurf9);
	if(FAILED(hr))
		return hr;

	pSurf = new CSurface();
	if(NULL == pSurf)
	{
		RELEASE(pSurf9);
		return E_OUTOFMEMORY;
	}

	if(!pSurf->Init(pSurf9, m_pDevice, this))
	{
		RELEASE(pSurf9);
		RELEASE(pSurf);
		return E_FAIL;
	}

	RELEASE(pSurf9);

	if (ppCubeMapSurface)
		*ppCubeMapSurface = pSurf;

	return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CCubeTexture::GetLevelDesc9(UINT uLevel, PSURFACEDESC pDesc)
{
	HRESULT hr;
	D3DSURFACE_DESC D3DDesc;

	if(NULL == pDesc)
		return E_INVALIDARG;

    ZeroMemory( pDesc, sizeof(*pDesc) );

    hr = ((LPDIRECT3DCUBETEXTURE9)m_pTexture9)->GetLevelDesc(uLevel, &D3DDesc);
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

// --------------------------------------------------------------------------------------

HRESULT CCubeTexture::LockRect9(CUBEMAP_FACES FaceType, UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD dwFlags)
{
	HRESULT hr = 0;
	D3DLOCKED_RECT lr;

	if( 0L == pLockedRect )
		return ((LPDIRECT3DCUBETEXTURE9)m_pTexture9)->LockRect((D3DCUBEMAP_FACES)FaceType, uLevel, 0L, pRect, dwFlags );

	hr = ((LPDIRECT3DCUBETEXTURE9)m_pTexture9)->LockRect((D3DCUBEMAP_FACES)FaceType, uLevel, &lr, pRect, dwFlags);
	pLockedRect->iPitch = lr.Pitch;
	pLockedRect->pBits = lr.pBits;

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CCubeTexture::UnlockRect9(CUBEMAP_FACES FaceType, UINT uLevel)
{
	return ((LPDIRECT3DCUBETEXTURE9)m_pTexture9)->UnlockRect((D3DCUBEMAP_FACES)FaceType, uLevel);
}

// --------------------------------------------------------------------------------------

HRESULT CCubeTexture::SetPalette9(PALETTEENTRY *pPalette)
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

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::GetDevice9(CDevice** ppDevice)
{
	IDirect3DDevice9 *pDev = NULL;
	HRESULT hr;

    hr = ((LPDIRECT3DCUBETEXTURE9)m_pTexture9)->GetDevice( PassPointer( &pDev, ppDevice) );

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

HRESULT CCubeTexture::FreePrivateData9(REFGUID refguid)
{
	return ((LPDIRECT3DCUBETEXTURE9)m_pTexture9)->FreePrivateData(refguid);
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::GetPrivateData9(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return ((LPDIRECT3DCUBETEXTURE9)m_pTexture9)->GetPrivateData(refguid, pData, pSizeOfData);
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::SetPrivateData9(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags)
{
	return ((LPDIRECT3DCUBETEXTURE9)m_pTexture9)->SetPrivateData(refguid, pData, dwSizeOfData, dwFlags);
}
