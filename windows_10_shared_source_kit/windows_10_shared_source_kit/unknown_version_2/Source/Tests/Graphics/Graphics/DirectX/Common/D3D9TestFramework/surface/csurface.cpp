#include "SurfacePCH.h"

//#define STRICT
#include "CSurface.h"
#include "d3dx9.h"

// -------------------------------------------------------------------------------------

CSurface::CSurface()
{
    m_pSurface4 = NULL;
	m_pSurface7 = NULL;
	m_pSurface8 = NULL;
    m_pSurface9 = NULL;
	m_pParent = NULL;
	m_pDevice = NULL;

	pGetDesc = NULL;
	pUnlockRect = NULL;
	pRelSurf = NULL;
	pGetPalette = NULL;
	pLockRect = NULL;
	pGetDC = NULL;
	pReleaseDC = NULL;
	pAddAttachedSurface = NULL;
	pDeleteAttachedSurface = NULL;
}

// -------------------------------------------------------------------------------------

CSurface::~CSurface()
{
    ReleaseAll();
}

// -------------------------------------------------------------------------------------

ULONG WINAPI CSurface::AddRef(void)
{
	if(m_pDevice)
		m_pDevice->AddRef();
	return CObject::AddRef();
}

// -------------------------------------------------------------------------------------

ULONG WINAPI CSurface::Release(void)
{
	if(m_pDevice)
		m_pDevice->Release();
	return CObject::Release();
}

// -------------------------------------------------------------------------------------

void CSurface::ReleaseAll()
{
	if(pRelSurf)
		RelSurf();
	return;
}

// -------------------------------------------------------------------------------------

bool CSurface::Init(LPVOID pD3DSurface, CDevice *pDevice, void *pParent)
{
	bool bRet = false;
    SURFACEDESC Desc;

	if (NULL == pDevice)
		return false;

	m_pDevice = pDevice;
	m_pDevice->AddRef();
    m_pParent = pParent;

	if (6.0f == m_pDevice->GetInterface())
	{
                pGetDesc = &CSurface::GetDesc6;
                pUnlockRect     = &CSurface::UnlockRect6;
                pRelSurf = &CSurface::RelSurf6;
                pGetPalette = &CSurface::GetPalette6;
                pLockRect       = &CSurface::LockRect6;
                pUnlockRect     = &CSurface::UnlockRect6;
                pGetDC          = &CSurface::GetDC6;
                pReleaseDC      = &CSurface::ReleaseDC6;
                pAddAttachedSurface = &CSurface::AddAttachedSurface6;
                pDeleteAttachedSurface = &CSurface::DeleteAttachedSurface6;
                pUpdateSurfaceImg = &CSurface::UpdateSurfaceImg7;
        pUpdateSurfaceFromSurface = &CSurface::UpdateSurfaceFromSurface7;
                pColorFill = &CSurface::ColorFill6;
                pGetContainer = &CSurface::GetContainer6;
                pFreePrivateData = &CSurface::FreePrivateData6;
                pGetPrivateData = &CSurface::GetPrivateData6;
                pSetPrivateData = &CSurface::SetPrivateData6;
                pGetDevice = &CSurface::GetDevice6;
		bRet = Init6(pD3DSurface);
	}
	else if (7.0f == m_pDevice->GetInterface())
	{
                pGetDesc = &CSurface::GetDesc7;
                pUnlockRect     = &CSurface::UnlockRect7;
                pRelSurf = &CSurface::RelSurf7;
                pGetPalette = &CSurface::GetPalette7;
                pLockRect       = &CSurface::LockRect7;
                pUnlockRect     = &CSurface::UnlockRect7;
                pGetDC          = &CSurface::GetDC7;
                pReleaseDC      = &CSurface::ReleaseDC7;
                pAddAttachedSurface = &CSurface::AddAttachedSurface7;
                pDeleteAttachedSurface = &CSurface::DeleteAttachedSurface7;
                pUpdateSurfaceImg = &CSurface::UpdateSurfaceImg7;
        pUpdateSurfaceFromSurface = &CSurface::UpdateSurfaceFromSurface7;
                pColorFill = &CSurface::ColorFill7;
                pGetContainer = &CSurface::GetContainer7;
                pFreePrivateData = &CSurface::FreePrivateData7;
                pGetPrivateData = &CSurface::GetPrivateData7;
                pSetPrivateData = &CSurface::SetPrivateData7;
                pGetDevice = &CSurface::GetDevice7;
		bRet = Init7(pD3DSurface);
	}
	else if (8.0f <= m_pDevice->GetInterface() && 9.0f > m_pDevice->GetInterface())
	{
                pGetDesc = &CSurface::GetDesc8;
                pUnlockRect     = &CSurface::UnlockRect8;
                pRelSurf = &CSurface::RelSurf8;
                pGetPalette = &CSurface::GetPalette8;
                pLockRect       = &CSurface::LockRect8;
                pUnlockRect     = &CSurface::UnlockRect8;
                pGetDC          = &CSurface::GetDC8;
                pReleaseDC      = &CSurface::ReleaseDC8;
                pAddAttachedSurface = &CSurface::AddAttachedSurface8;
                pDeleteAttachedSurface = &CSurface::DeleteAttachedSurface8;
                pUpdateSurfaceImg = &CSurface::UpdateSurfaceImg8;
                pUpdateSurfaceFromSurface = &CSurface::UpdateSurfaceFromSurface8;
                pColorFill = &CSurface::ColorFill8;
                pGetContainer = &CSurface::GetContainer8;
                pFreePrivateData = &CSurface::FreePrivateData8;
                pGetPrivateData = &CSurface::GetPrivateData8;
                pSetPrivateData = &CSurface::SetPrivateData8;
                pGetDevice = &CSurface::GetDevice8;
		bRet = Init8(pD3DSurface);
	}
    else if (9.0f <= m_pDevice->GetInterface() && 10.0f > m_pDevice->GetInterface())
	{
                pGetDesc = &CSurface::GetDesc9;
                pUnlockRect     = &CSurface::UnlockRect9;
                pRelSurf = &CSurface::RelSurf9;
                pGetPalette = &CSurface::GetPalette9;
                pLockRect       = &CSurface::LockRect9;
                pUnlockRect     = &CSurface::UnlockRect9;
                pGetDC          = &CSurface::GetDC9;
                pReleaseDC      = &CSurface::ReleaseDC9;
                pAddAttachedSurface = &CSurface::AddAttachedSurface9;
                pDeleteAttachedSurface = &CSurface::DeleteAttachedSurface9;
                pUpdateSurfaceImg = &CSurface::UpdateSurfaceImg9;
                pUpdateSurfaceFromSurface = &CSurface::UpdateSurfaceFromSurface9;
                pColorFill = &CSurface::ColorFill9;
                pGetContainer = &CSurface::GetContainer9;
                pFreePrivateData = &CSurface::FreePrivateData9;
                pGetPrivateData = &CSurface::GetPrivateData9;
                pSetPrivateData = &CSurface::SetPrivateData9;
                pGetDevice = &CSurface::GetDevice9;
		bRet = Init9(pD3DSurface);
	}

    GetDesc(&m_Desc);

	// Determine the masks
	FormatToMask(&(m_Desc.Format), &m_Mask);

	return bRet;
}

// -------------------------------------------------------------------------------------

DWORD CSurface::ReadColor(PVOID * ppSurfacePtr, bool bInc)
{
	return ::ReadColor(ppSurfacePtr, &(m_Desc.Format), &m_Mask, bInc);
}

// -------------------------------------------------------------------------------------

DWORD CSurface::ColorConvert(DWORD dwColor)
{
	return ::ColorConvert(dwColor, &m_Mask);
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::UpdateSurfaceFromSurface(CSurface *pSurf)
{
    HRESULT hr;
	float fSize;
	LOCKED_RECT srcLock, trgLock;

	if(NULL == pSurf)
		return E_FAIL;

	// Lock the surf
	hr = pSurf->LockRect(&srcLock, NULL, LOCK_NOSYSLOCK);
	if(FAILED(hr))
 	{
		return hr;
	}

    hr = LockRect(&trgLock, NULL, LOCK_NOSYSLOCK);
	if(FAILED(hr))
 	{
		pSurf->UnlockRect();
		return hr;
	}

	// Need to special case the double byte YUV formats
	if ((MAKEFOURCC('Y','U','Y','2') == m_Desc.Format.ddpfPixelFormat.dwFourCC) ||
		(MAKEFOURCC('U','Y','V','Y') == m_Desc.Format.ddpfPixelFormat.dwFourCC))
		fSize = (m_Desc.Width == 1) ? 4.0f : 2.0f;
	else
		fSize = m_Desc.Format.ddpfPixelFormat.dwRGBBitCount / 8.0f;

	for (unsigned y = 0; y < m_Desc.Height; ++y)
	{
		PBYTE pTrgData = (PBYTE) trgLock.pBits + (trgLock.iPitch * y);
        PBYTE pSrcData = (PBYTE) srcLock.pBits + (srcLock.iPitch * y);
	
		memcpy(pTrgData, pSrcData, (UINT)ceilf(fSize * m_Desc.Width));
	}

	// Unlock and return
    pSurf->UnlockRect();

	return UnlockRect();;
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::LoadImage(CImage *pImg)
{
	BYTE    bByte;
	PBYTE   pData = NULL;
	PBYTE   pSurfData;
	DWORD   dwX, dwY;
	LOCKED_RECT pLRect;
	HRESULT hr;
	CImage *pScaledImg = NULL;
    FColor *pImageData = NULL;
	signed char cY0, cU0, cV0, cY1, cU1, cV1; // For YUV formats
	
	if(NULL == pImg || pImg->GetWidth() < 1 || pImg->GetHeight() < 1)
		return E_INVALIDARG;

	//create a local copy of the image and rescale if necessary
    if (m_Desc.dwWidth != pImg->GetWidth() || 
		m_Desc.dwHeight != pImg->GetHeight())
	{
		pScaledImg = new CImage();
		if( pScaledImg == NULL ||
		   !pScaledImg->Copy(pImg) || !pScaledImg->ScaleData(m_Desc.dwWidth, m_Desc.dwHeight))
		{
			hr = E_OUTOFMEMORY;
			goto Exit;
		}
		pImageData = pScaledImg->GetData();
   	}
	else
		pImageData = pImg->GetData();
	
	// Validate the CImageData
	if(pImageData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}
    
	// Lock the surface
	hr = LockRect(&pLRect, NULL, LOCK_NOSYSLOCK);
	if(FAILED(hr))
	{
		goto Exit;
	}
                  
	// Get the actual data
	pData = (PBYTE)pLRect.pBits;
	
	for (dwY=0; dwY < m_Desc.dwHeight; dwY++)
	{
        pSurfData = pData;
        for (dwX=0; dwX < m_Desc.dwWidth; dwX++)
        {
			FColor Color = *(pImageData + (dwY * m_Desc.dwWidth) + dwX);

			switch((FMT)m_Desc.Format.d3dfFormat)
			{
			case FMT_A1:
				{
					// Only advance the ptr every 8th time
					Color.ConvertTo((FMT)m_Desc.Format.d3dfFormat, .5f, (char*)&bByte, FALSE, ELEMENTINDEX_0);
					if (dwX == 0)
						*pSurfData = 0;

					if(!(dwX % 8) && dwX)
					{
						pSurfData++;
						*pSurfData = 0;
					}

					*pSurfData |= (bByte << (dwX % 8));
				}
				break;
			case FMT_P4:
				{
					//Only advance the ptr every other time
					Color.ConvertTo((FMT)m_Desc.Format.d3dfFormat, .5f, (char*)&bByte, FALSE, ELEMENTINDEX_0);
					if(dwX & 1)
					{
						*pSurfData |= bByte;
						pSurfData++;
					}
					else
					{
						*pSurfData = (bByte << 4);
					}
				}
				break;
			case FMT_YUY2:
			case FMT_UYVY:
				{
					FColor Color2;

					if (m_Desc.dwWidth == 1)
						Color2 = Color;
					else
						Color2 = *(pImageData + (dwY * m_Desc.dwWidth) + dwX + 1);

					RGBTOYUV((UINT32)(Color.R * 255.f), (UINT32)(Color.G * 255.f), (UINT32)(Color.B * 255.f), cY0, cU0, cV0);
					RGBTOYUV((UINT32)(Color2.R * 255.f), (UINT32)(Color2.G * 255.f), (UINT32)(Color2.B * 255.f), cY1, cU1, cV1);
					if((FMT)m_Desc.Format.d3dfFormat == FMT_YUY2)
					{
						PACKYUY2(*((DWORD*)pSurfData), cY0, cY1, cU0, cV0);
					}
					else
					{
						PACKUYVY(*((DWORD*)pSurfData), cY0, cY1, cU0, cV0);
					}
					
					dwX++;  // We write two pixels at once
					pSurfData += sizeof(DWORD);
				}
				break;
			default:
				{
					Color.ConvertTo((FMT)m_Desc.Format.d3dfFormat, .5f, (char*)pSurfData, FALSE, ELEMENTINDEX_0);
					pSurfData += m_Desc.Format.ddpfPixelFormat.dwRGBBitCount / 8;
				}
				break;
			}
        }
        pData += pLRect.iPitch;
    }

Exit:
	if(FAILED(hr))
	{
		DPF(1, _T("CSurface::LoadImage() - failed to load image.  hr=%s(%x).\n"),
			m_pDevice->m_pD3D->HResultToString(hr), hr);
		m_pDevice->GetD3D()->SetLastError(hr);
	}
    SAFEDELETE(pScaledImg);
    
	return UnlockRect();
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::LoadSurfaceFromSurface(CSurface *pSurfFrom)
{
    return LoadSurfaceFromSurface(pSurfFrom, FILTER_NONE);
}

// -------------------------------------------------------------------------------------

HRESULT CSurface::LoadSurfaceFromSurface(CSurface *pSurfFrom, DWORD filter)
{
    FSurface* pFSurf = NULL;
    HRESULT hr = S_OK;

    // We can't handle DXTNs, so use D3DX
    if((FMT)pSurfFrom->m_Desc.Format.d3dfFormat == FMT_DXT1 ||
       (FMT)pSurfFrom->m_Desc.Format.d3dfFormat == FMT_DXT2 ||
       (FMT)pSurfFrom->m_Desc.Format.d3dfFormat == FMT_DXT3 ||
       (FMT)pSurfFrom->m_Desc.Format.d3dfFormat == FMT_DXT4 ||
       (FMT)pSurfFrom->m_Desc.Format.d3dfFormat == FMT_DXT5 ||
       (FMT)m_Desc.Format.d3dfFormat == FMT_DXT1 ||
       (FMT)m_Desc.Format.d3dfFormat == FMT_DXT2 ||
       (FMT)m_Desc.Format.d3dfFormat == FMT_DXT3 ||
       (FMT)m_Desc.Format.d3dfFormat == FMT_DXT4 ||
       (FMT)m_Desc.Format.d3dfFormat == FMT_DXT5)
    {
        return m_pDevice->D3DXLoadSurfaceFromSurface(this,m_pDevice->m_Palette8, NULL, pSurfFrom, m_pDevice->m_Palette8, NULL, filter, 0);
    }

    pFSurf = new FSurface();
    if(NULL == pFSurf)
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pFSurf->ConvertFrom(pSurfFrom, filter);
    if(FAILED(hr))
    {
        goto Exit;
    }

    hr = pFSurf->ConvertTo(this, filter);
    if(FAILED(hr))
    {
        goto Exit;
    }

Exit:
    SAFEDELETE(pFSurf);
    return hr;
}

// -------------------------------------------------------------------------------------

LPVOID CSurface::GetInternalPointer()
{
	if(m_pSurface9)
		return m_pSurface9;
    else if(m_pSurface8)
		return m_pSurface8;
	else if(m_pSurface7)
		return m_pSurface7;
	else if(m_pSurface4)
		return m_pSurface4;
	else
		return NULL;
}
