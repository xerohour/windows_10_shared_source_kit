#include "SurfacePCH.h"

//#define STRICT
#include <d3dx8.h>
#include "CD3DX.h"
#include "CSurface.h"
#include "CTextures.h"

// -------------------------------------------------------------------------------------

bool CSurface::Init8(void* pSurface)
{
	m_pSurface8 = (LPDIRECT3DSURFACE8)pSurface;
	m_pSurface8->AddRef();
	return true;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetDesc8(PSURFACEDESC pDesc)
{	 
	HRESULT hr;
    D3DSURFACE_DESC D3DDesc;
	
	if (m_pSurface8 == NULL)
		return E_FAIL;
	
	if (pDesc == NULL)
	{
		hr = m_pSurface8->GetDesc(NULL);
		return hr;
	}

    ZeroMemory(pDesc, sizeof(SURFACEDESC));

	hr = m_pSurface8->GetDesc(&D3DDesc);

	if(FAILED(hr))
		return hr;
	

	//Fill in the version independent struct.
	pDesc->dwWidth = D3DDesc.Width;
	pDesc->dwHeight = D3DDesc.Height;
	pDesc->Format.d3dfFormat = D3DDesc.Format;
	pDesc->Type = D3DDesc.Type;
	pDesc->Usage = (DWORD)D3DDesc.Usage;
	pDesc->Pool = (DWORD)D3DDesc.Pool;
	pDesc->Size = D3DDesc.Size;
	pDesc->MultiSampleType = (MULTISAMPLE_TYPE)D3DDesc.MultiSampleType;
	d3df2ddpf(D3DDesc.Format, &(pDesc->Format.ddpfPixelFormat));
	pDesc->dwFlags = pDesc->Format.ddpfPixelFormat.dwFlags;
	pDesc->lPitch = (pDesc->Format.ddpfPixelFormat.dwRGBBitCount / 8) * pDesc->dwWidth;
	
	return S_OK;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetPalette8(PALETTEENTRY rgPal[256], DWORD &dwNumEntries)
{
	dwNumEntries = 256;
	return m_pDevice->GetPaletteEntries(0, (LPPALETTEENTRY)rgPal);
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::LockRect8(LOCKED_RECT *pLockedRect, CONST RECT* pRect, DWORD Flags)
{   
	D3DLOCKED_RECT Dx8LockRect;
	HRESULT hr = ERROR;

    if (NULL != m_pSurface8)
    {
		if (NULL == pLockedRect)
		{
			hr = m_pSurface8->LockRect(NULL, pRect, Flags);
			return hr;
		}

		hr = m_pSurface8->LockRect(&Dx8LockRect, pRect, Flags);
		pLockedRect->iPitch = Dx8LockRect.Pitch;
		pLockedRect->pBits = Dx8LockRect.pBits;
    }
    return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::UnlockRect8(void)
{
	if (NULL != m_pSurface8)
    {
		return m_pSurface8->UnlockRect();
    }
	return ERROR;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetDC8(HDC *phDC)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::ReleaseDC8(HDC hDC)
{
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::UpdateSurfaceImg8(CImage *pSrc)
{
    if (NULL == pSrc)
    {
		DPF(1, _T("CSurface::UpdateSurface - invalid params.\n"));
        return E_FAIL;
    }

	if (m_Desc.Format.d3dfFormat == D3DFMT_DXT1 ||
		m_Desc.Format.d3dfFormat == D3DFMT_DXT2 ||
		m_Desc.Format.d3dfFormat == D3DFMT_DXT3 ||
		m_Desc.Format.d3dfFormat == D3DFMT_DXT4 ||
		m_Desc.Format.d3dfFormat == D3DFMT_DXT5 ||
		m_Desc.Format.d3dfFormat == D3DFMT_UYVY ||
		m_Desc.Format.d3dfFormat == D3DFMT_YUY2)
	{
        RECT SrcRect = {0,0,pSrc->GetWidth(),pSrc->GetHeight()};

		return ::D3DXLoadSurfaceFromMemory(
									m_pSurface8,			    //LPDIRECT3DSURFACE8    pDestSurface,
									NULL,                       //D3DCOLOR*             pDestPalette,
									NULL,					    //RECT*                 pDestRect,
									(LPVOID)pSrc->GetPackedData(),//LPVOID                pSrcMemory
									D3DFMT_A8R8G8B8,		    //D3DFORMAT             SrcFormat,
									pSrc->GetWidth() * 4,	    //UINT                  cbSrcPitch,
									NULL,					    //D3DCOLOR*             pSrcPalette,
									&SrcRect,					//RECT*                 pSrcRect,
									D3DX_FILTER_POINT,          //DWORD                 dwFilter,
									0);                         //D3DCOLOR              ColorKey
	}
	else
	{
		return LoadImage(pSrc);
	}
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::UpdateSurfaceFromSurface8(CSurface *pSrc)
{
    if (NULL == pSrc)
    {
		DPF(1, _T("CSurface::UpdateSurface - invalid params.\n"));
        return E_FAIL;
    }

	return ::D3DXLoadSurfaceFromSurface(
									m_pSurface8,			    //LPDIRECT3DSURFACE8    pDestSurface,
									NULL,                       //D3DCOLOR*             pDestPalette,
									NULL,					    //RECT*                 pDestRect,
									pSrc->m_pSurface8,  		//LPDIRECT3DSURFACE8    pSrcSurface
									NULL,					    //D3DCOLOR*             pSrcPalette,
									NULL,						//RECT*                 pSrcRect,
									D3DX_FILTER_POINT,          //DWORD                 dwFilter,
									0);                         //D3DCOLOR              ColorKey
}

// -------------------------------------------------------------------------------------

bool CSurface::RelSurf8()
{
    RELEASE(m_pSurface8);
	return true;
}

// -------------------------------------------------------------------------------------

HRESULT	CSurface::AddAttachedSurface8(CSurface *pSurf)
{
	DPF(1, _T("CSurface::AddAttachedSurface8() - unsupported on DX8.\n"));
	return E_FAIL;
}

// -------------------------------------------------------------------------------------

HRESULT	CSurface::DeleteAttachedSurface8(CSurface *pSurf)
{
	DPF(1, _T("CSurface::DeleteAttachedSurface8() - unsupported on DX8.\n"));
	return E_FAIL;
}

// -------------------------------------------------------------------------------------

bool CSurface::ColorFill8(DWORD dwRGB, LPRECT pRect)
{
    BYTE*               pBuf;
    BYTE*               pLine;
    DWORD               dwX, dwY;
    DWORD               dwWidth;
    DWORD               dwHeight;
	DWORD				dwFillColor;
	INT					iPitch;
	LOCKED_RECT			Rect;
    
    if (NULL == m_pSurface8)
        return false;

    if (pRect)
    {
        dwWidth = pRect->right - pRect->left;
        dwHeight = pRect->bottom - pRect->top;
    }
    else
    {
        dwWidth = m_Desc.dwWidth;
        dwHeight = m_Desc.dwHeight;
    }
    
    if (FAILED(LockRect(&Rect, pRect, LOCK_NOSYSLOCK)))
        return false;
	pLine = (BYTE*)Rect.pBits;
	iPitch = Rect.iPitch;
	dwFillColor = ColorConvert(dwRGB);

    
    for (dwY = 0; dwY < dwHeight; dwY++)
    {
        pBuf = pLine;
        pLine += iPitch;
        for (dwX = 0; dwX < dwWidth; dwX++)
        {
            switch(m_Desc.Format.ddpfPixelFormat.dwRGBBitCount)
            {
            case 8:
                break;
            case 16:
                *(WORD*)pBuf = (WORD)dwFillColor;
                pBuf += 2;
                break;
            case 24:
                break;
            case 32:
                *(DWORD*)pBuf = dwFillColor;
                pBuf += 4;
                break;
            }
        }
    }

    UnlockRect();
    return true;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetContainer8(REFIID riid, void** ppContainer)
{
	IUnknown* pObject = NULL;
	HRESULT hr;

	if (ppContainer == NULL)
	{
		hr = m_pSurface8->GetContainer(riid, NULL);
	}
	else
		hr = m_pSurface8->GetContainer(riid, (void**)&pObject);

	if (SUCCEEDED(hr))
	{
		if (! pObject)
			return E_FAIL;

		if (riid == IID_IDirect3DDevice8)
		{
			if (m_pDevice->GetDevicePointer8() == pObject)
			{
				*ppContainer = m_pDevice;
				m_pDevice->AddRef();
			}
			else
				hr = E_FAIL;
		}
		else if (riid == IID_IDirect3DTexture8)
		{
			LPTEXTURE pTex = new CTexture;
			if (pTex == NULL)
			{
				RELEASE(pObject);
				return E_FAIL;
			}
			if (!pTex->Init((IDirect3DTexture8*)pObject, m_pDevice))
			{
				RELEASE(pObject);
				delete pTex;
				return E_FAIL;
			}
			*ppContainer = pTex;
		}
		else if (riid == IID_IDirect3DCubeTexture8)
		{
			LPCUBETEXTURE pCubeTex = new CCubeTexture;
			if (pCubeTex == NULL)
			{
				RELEASE(pObject);
				return E_FAIL;
			}
			if (!pCubeTex->Init((IDirect3DCubeTexture8*)pObject, m_pDevice))
			{
				RELEASE(pObject);
				delete pCubeTex;
				return E_FAIL;
			}
			*ppContainer = pCubeTex;
		}
		else if (riid == IID_IDirect3DSurface8)
		{
			LPSURFACE pSurf = new CSurface;
			if (pSurf == NULL)
			{
				RELEASE(pObject);
				return E_FAIL;
			}
			if (!pSurf->Init((IDirect3DSurface8*)pObject, m_pDevice, this))
			{
				RELEASE(pObject);
				delete pSurf;
				return E_FAIL;
			}
			*ppContainer = pSurf;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	RELEASE(pObject);

	return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::FreePrivateData8(REFGUID refguid)
{
	return m_pSurface8->FreePrivateData(refguid);
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetPrivateData8(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return m_pSurface8->GetPrivateData(refguid, pData, pSizeOfData);
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::SetPrivateData8(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags)
{
	return m_pSurface8->SetPrivateData(refguid, pData, dwSizeOfData, dwFlags);
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetDevice8(LPDEVICE *ppDevice)
{
	IDirect3DDevice8 *pDev = NULL;
	HRESULT hr;
	if (ppDevice == NULL)
		hr = m_pSurface8->GetDevice(NULL);
	else
		hr = m_pSurface8->GetDevice(&pDev);

	if (SUCCEEDED(hr))
	{
		if (pDev != m_pDevice->GetDevicePointer8())
		{
			ppDevice = NULL;
			RELEASE(pDev);
			return S_OK;
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
