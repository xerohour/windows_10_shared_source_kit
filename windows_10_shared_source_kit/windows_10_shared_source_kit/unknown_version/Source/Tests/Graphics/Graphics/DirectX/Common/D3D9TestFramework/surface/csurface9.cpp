#include "SurfacePCH.h"

#include "d3dx9.h"
#include "CD3DX.h"
#include "CSurface.h"
#include "CTextures.h"
#include "CSwapChain.h"

// -------------------------------------------------------------------------------------

bool CSurface::Init9(void* pSurface)
{
	m_pSurface9 = (LPDIRECT3DSURFACE9)pSurface;
	m_pSurface9->AddRef();
	return true;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetDesc9(PSURFACEDESC pDesc)
{	 
	HRESULT hr;
    D3DSURFACE_DESC D3DDesc;
	
	if (m_pSurface9 == NULL)
		return E_FAIL;
	
	if (pDesc == NULL)
	{
		hr = m_pSurface9->GetDesc(NULL);
		return hr;
	}

    ZeroMemory(pDesc, sizeof(SURFACEDESC));

	hr = m_pSurface9->GetDesc(&D3DDesc);

	if(FAILED(hr))
		return hr;
	

	//Fill in the version independent struct.
	pDesc->dwWidth = D3DDesc.Width;
	pDesc->dwHeight = D3DDesc.Height;
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
	
	return S_OK;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetPalette9(PALETTEENTRY rgPal[256], DWORD &dwNumEntries)
{
	dwNumEntries = 256;
	return m_pDevice->GetPaletteEntries(0, (LPPALETTEENTRY)rgPal);
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::LockRect9(LOCKED_RECT *pLockedRect, CONST RECT* pRect, DWORD Flags)
{   
	D3DLOCKED_RECT DxLockRect;
	HRESULT hr = ERROR;

    if (NULL != m_pSurface9)
    {
		if (NULL == pLockedRect)
		{
			hr = m_pSurface9->LockRect(NULL, pRect, Flags);
			return hr;
		}

		hr = m_pSurface9->LockRect(&DxLockRect, pRect, Flags);
		pLockedRect->iPitch = DxLockRect.Pitch;
		pLockedRect->pBits = DxLockRect.pBits;
    }
    return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::UnlockRect9(void)
{
	if (NULL != m_pSurface9)
    {
		return m_pSurface9->UnlockRect();
    }
	return ERROR;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetDC9(HDC *phDC)
{
	if (NULL != m_pSurface9)
	{
		return m_pSurface9->GetDC(phDC);
	}
	return ERROR;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::ReleaseDC9(HDC hDC)
{
	if (NULL != m_pSurface9)
	{
		return m_pSurface9->ReleaseDC(hDC);
	}
	return ERROR;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::UpdateSurfaceImg9(CImage *pSrc)
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
		m_Desc.Format.d3dfFormat == D3DFMT_R8G8_B8G8 ||
		m_Desc.Format.d3dfFormat == D3DFMT_YUY2 ||
		m_Desc.Format.d3dfFormat == D3DFMT_G8R8_G8B8)
	{
        RECT SrcRect = {0,0,pSrc->GetWidth(),pSrc->GetHeight()};

		return ::D3DXLoadSurfaceFromMemory(
									m_pSurface9,			    //LPDIRECT3DSURFACE9    pDestSurface,
									NULL,                       //D3DCOLOR*             pDestPalette,
									NULL,					    //RECT*                 pDestRect,
									(LPVOID)pSrc->GetData(),    //LPVOID                pSrcMemory
									D3DFMT_A32B32G32R32F,		    //D3DFORMAT             SrcFormat,
									pSrc->GetWidth() * 16,	    //UINT                  cbSrcPitch,
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

HRESULT CSurface::UpdateSurfaceFromSurface9(CSurface *pSrc)
{
    if (NULL == pSrc)
    {
		DPF(1, _T("CSurface::UpdateSurfaceFromSurface9 - invalid params.\n"));
        return E_FAIL;
    }

	// D3DX does not support D3DFMT_A1 yet, do it ourselves
	if (m_Desc.Format.d3dfFormat == FMT_A1)
		return UpdateSurfaceFromSurface(pSrc);

	return ::D3DXLoadSurfaceFromSurface(
									m_pSurface9,			    //LPDIRECT3DSURFACE9    pDestSurface,
									NULL,                       //D3DCOLOR*             pDestPalette,
									NULL,					    //RECT*                 pDestRect,
									pSrc->m_pSurface9,  		//LPDIRECT3DSURFACE9    pSrcSurface
									NULL,					    //D3DCOLOR*             pSrcPalette,
									NULL,						//RECT*                 pSrcRect,
									D3DX_FILTER_POINT,          //DWORD                 dwFilter,
									0);                         //D3DCOLOR              ColorKey
}

// -------------------------------------------------------------------------------------

bool CSurface::RelSurf9()
{
    RELEASE(m_pSurface9);
	return true;
}

// -------------------------------------------------------------------------------------

HRESULT	CSurface::AddAttachedSurface9(CSurface *pSurf)
{
	DPF(1, _T("CSurface::AddAttachedSurface9() - unsupported on DX9.\n"));
	return E_FAIL;
}

// -------------------------------------------------------------------------------------

HRESULT	CSurface::DeleteAttachedSurface9(CSurface *pSurf)
{
	DPF(1, _T("CSurface::DeleteAttachedSurface9() - unsupported on DX9.\n"));
	return E_FAIL;
}

// -------------------------------------------------------------------------------------

bool CSurface::ColorFill9(DWORD dwRGB, LPRECT pRect)
{
    BYTE*               pBuf;
    BYTE*               pLine;
    DWORD               dwX, dwY;
    DWORD               dwWidth;
    DWORD               dwHeight;
	DWORD				dwFillColor;
	INT					iPitch;
	LOCKED_RECT			Rect;
    
    if (NULL == m_pSurface9)
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

HRESULT CSurface::GetContainer9(REFIID riid, void** ppContainer)
{
	IUnknown* pObject = NULL;
	HRESULT hr;
	
	if (ppContainer == NULL)
		hr = m_pSurface9->GetContainer(riid, NULL);
	else
		hr = m_pSurface9->GetContainer(riid, (void**)&pObject);

	if (SUCCEEDED(hr))
	{
		if (! pObject)
			return E_FAIL;

		if (riid == IID_IDirect3DDevice9)
		{
			if (m_pDevice->GetDevicePointer9() == pObject)
			{
				*ppContainer = m_pDevice;
				m_pDevice->AddRef();
			}
			else
				hr = E_FAIL;
		}
		else if (riid == IID_IDirect3DTexture9)
		{
			LPTEXTURE pTex = new CTexture;
			if (pTex == NULL)
			{
				RELEASE(pObject);
				return E_FAIL;
			}
			if (!pTex->Init((IDirect3DTexture9*)pObject, m_pDevice))
			{
				RELEASE(pObject);
				delete pTex;
				return E_FAIL;
			}
			*ppContainer = pTex;
		}
		else if (riid == IID_IDirect3DCubeTexture9)
		{
			LPCUBETEXTURE pCubeTex = new CCubeTexture;
			if (pCubeTex == NULL)
			{
				RELEASE(pObject);
				return E_FAIL;
			}
			if (!pCubeTex->Init((IDirect3DCubeTexture9*)pObject, m_pDevice))
			{
				RELEASE(pObject);
				delete pCubeTex;
				return E_FAIL;
			}
			*ppContainer = pCubeTex;
		}
		else if (riid == IID_IDirect3DSurface9)
		{
			LPSURFACE pSurf = new CSurface;
			if (pSurf == NULL)
			{
				RELEASE(pObject);
				return E_FAIL;
			}
			if (!pSurf->Init((IDirect3DSurface9*)pObject, m_pDevice, this))
			{
				RELEASE(pObject);
				delete pSurf;
				return E_FAIL;
			}
			*ppContainer = pSurf;
		}
		else if (riid == IID_IDirect3DSwapChain9)
		{
			LPSWAPCHAIN pSwapChain = new CSwapChain;
			if (pSwapChain == NULL)
			{
				RELEASE(pObject);
				return E_FAIL;
			}

			//No way to know how many backbuffer in the swapchain, so we put 1
			PRESENT_PARAMETERS params;
			ZeroMemory(&params, sizeof(params));
			params.uBackBufferCount = 1;

			if (!pSwapChain->Init((IDirect3DSwapChain9*)pObject, &params, m_pDevice))
			{
				RELEASE(pObject);
				delete pSwapChain;
				return E_FAIL;
			}
			*ppContainer = pSwapChain;
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

HRESULT CSurface::FreePrivateData9(REFGUID refguid)
{
	return m_pSurface9->FreePrivateData(refguid);
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetPrivateData9(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return m_pSurface9->GetPrivateData(refguid, pData, pSizeOfData);
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::SetPrivateData9(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags)
{
	return m_pSurface9->SetPrivateData(refguid, pData, dwSizeOfData, dwFlags);
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::GetDevice9(LPDEVICE *ppDevice)
{
	IDirect3DDevice9 *pDev = NULL;
	HRESULT hr;
	if (ppDevice == NULL)
		hr = m_pSurface9->GetDevice(NULL);
	else
		hr = m_pSurface9->GetDevice(&pDev);

	if (SUCCEEDED(hr))
	{
		if (pDev != m_pDevice->GetDevicePointer9())
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
