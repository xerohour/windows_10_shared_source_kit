#include "TexturesPCH.h"

#include <d3d8.h>
#include "CTextures.h"


bool CCubeTexture::Init8(LPVOID pTexture)
{
	m_pTexture8 = (LPDIRECT3DCUBETEXTURE8)pTexture;
	m_pTexture8->AddRef();
	m_dwLevelCount = ((LPDIRECT3DCUBETEXTURE8)pTexture)->GetLevelCount();	
	return true;
}


// --------------------------------------------------------------------------------------

void CCubeTexture::ReleaseD3D8()
{
	RELEASE(m_pTexture8);
	RELEASE(m_pSysTexture);
}

// --------------------------------------------------------------------------------------

HRESULT CCubeTexture::AddDirtyRect8(CUBEMAP_FACES FaceType, CONST RECT* pDirtyRect)
{
	if (NULL == m_pTexture8)
		return 0;

	return ((IDirect3DCubeTexture8*)m_pTexture8)->AddDirtyRect((D3DCUBEMAP_FACES)FaceType, pDirtyRect);
}

// --------------------------------------------------------------------------------------

void CCubeTexture::PreLoad8()
{
	if (NULL == m_pTexture8)
		return;
	((IDirect3DCubeTexture8*)m_pTexture8)->PreLoad();
}

// --------------------------------------------------------------------------------------

DWORD CCubeTexture::SetLOD8(DWORD dwLODNew)
{
	if (NULL == m_pTexture8)
		return 0;
	return ((IDirect3DCubeTexture8*)m_pTexture8)->SetLOD(dwLODNew);
}

// --------------------------------------------------------------------------------------


DWORD CCubeTexture::GetLOD8()
{
	if (NULL == m_pTexture8)
		return 0;
	return ((IDirect3DCubeTexture8*)m_pTexture8)->GetLOD();
}

// -------------------------------------------------------------------------------------

DWORD CCubeTexture::SetPriority8(DWORD dwNewPriority)
{
	return ((LPDIRECT3DCUBETEXTURE8)m_pTexture8)->SetPriority(dwNewPriority);
}

// -------------------------------------------------------------------------------------

DWORD CCubeTexture::GetPriority8(void)
{
	return ((LPDIRECT3DCUBETEXTURE8)m_pTexture8)->GetPriority();
}

// --------------------------------------------------------------------------------------

HRESULT CCubeTexture::GetCubeMapSurface8(CUBEMAP_FACES FaceType, UINT uLevel, CSurface** ppCubeMapSurface)
{
	HRESULT hr;
	CSurface *pSurf = NULL;
	LPDIRECT3DSURFACE8 pSurf8 = NULL;

	hr = ((LPDIRECT3DCUBETEXTURE8)m_pTexture8)->GetCubeMapSurface((D3DCUBEMAP_FACES)FaceType, uLevel, ppCubeMapSurface == NULL ? NULL : &pSurf8);
	if(FAILED(hr))
		return hr;

	pSurf = new CSurface();
	if(NULL == pSurf)
	{
		RELEASE(pSurf8);
		return E_OUTOFMEMORY;
	}

	if(!pSurf->Init(pSurf8, m_pDevice, this))
	{
		RELEASE(pSurf8);
		RELEASE(pSurf);
		return E_FAIL;
	}

	RELEASE(pSurf8);

	if (ppCubeMapSurface)
		*ppCubeMapSurface = pSurf;

	return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CCubeTexture::GetLevelDesc8(UINT uLevel, PSURFACEDESC pDesc)
{
	HRESULT hr;
	D3DSURFACE_DESC D3DDesc;

	if(NULL == pDesc)
		return E_INVALIDARG;

    ZeroMemory( pDesc, sizeof(*pDesc) );

	hr = ((LPDIRECT3DCUBETEXTURE8)m_pTexture8)->GetLevelDesc(uLevel, &D3DDesc);
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

// --------------------------------------------------------------------------------------

HRESULT CCubeTexture::LockRect8(CUBEMAP_FACES FaceType, UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD dwFlags)
{
	HRESULT hr = 0;
	D3DLOCKED_RECT lr;

	if( 0L == pLockedRect )
		return ((LPDIRECT3DCUBETEXTURE8)m_pTexture8)->LockRect((D3DCUBEMAP_FACES)FaceType, uLevel, 0L, pRect, dwFlags );

	hr = ((LPDIRECT3DCUBETEXTURE8)m_pTexture8)->LockRect((D3DCUBEMAP_FACES)FaceType, uLevel, &lr, pRect, dwFlags);
	pLockedRect->iPitch = lr.Pitch;
	pLockedRect->pBits = lr.pBits;

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CCubeTexture::UnlockRect8(CUBEMAP_FACES FaceType, UINT uLevel)
{
	return ((LPDIRECT3DCUBETEXTURE8)m_pTexture8)->UnlockRect((D3DCUBEMAP_FACES)FaceType, uLevel);
}

// --------------------------------------------------------------------------------------

HRESULT CCubeTexture::SetPalette8(PALETTEENTRY *pPalette)
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


// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::GetDevice8(CDevice** ppDevice)
{
	IDirect3DDevice8 *pDev = NULL;
	HRESULT hr;

    hr = ((LPDIRECT3DCUBETEXTURE8)m_pTexture8)->GetDevice( PassPointer( &pDev, ppDevice) );

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

HRESULT CCubeTexture::FreePrivateData8(REFGUID refguid)
{
	return ((LPDIRECT3DCUBETEXTURE8)m_pTexture8)->FreePrivateData(refguid);
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::GetPrivateData8(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return ((LPDIRECT3DCUBETEXTURE8)m_pTexture8)->GetPrivateData(refguid, pData, pSizeOfData);
}

// -------------------------------------------------------------------------------------

HRESULT CCubeTexture::SetPrivateData8(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags)
{
	return ((LPDIRECT3DCUBETEXTURE8)m_pTexture8)->SetPrivateData(refguid, pData, dwSizeOfData, dwFlags);
}
