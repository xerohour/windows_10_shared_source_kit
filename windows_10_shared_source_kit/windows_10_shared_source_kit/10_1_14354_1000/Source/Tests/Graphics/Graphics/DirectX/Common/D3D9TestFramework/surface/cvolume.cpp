#include "SurfacePCH.h"

//#define STRICT
#include "CSurface.h"

// -------------------------------------------------------------------------------------

CVolume::CVolume()
{
	m_pParent = NULL;
	m_pDevice = NULL;
	m_Desc.Depth = 0;
	m_pVolume8 = NULL;
    m_pVolume9 = NULL;
	
	pRelVol = NULL;
	pGetDesc = NULL;
	pLockBox = NULL;
	pUnlockBox = NULL;
}

// -------------------------------------------------------------------------------------

bool CVolume::Init(void* pVolume, CDevice *pDevice, void *pParent)
{
	bool bRet = false;

    if (NULL == pDevice)
		return false;

	m_pDevice = pDevice;
	m_pDevice->AddRef();
    m_pParent = pParent;

    
	if (9.0f <= pDevice->GetInterface() && 10.0f > pDevice->GetInterface())
	{
                pRelVol = &CVolume::RelVol9;
        pGetDevice = &CVolume::GetDevice9;
                pGetDesc = &CVolume::GetDesc9;
                pLockBox = &CVolume::LockBox9;
                pUnlockBox = &CVolume::UnlockBox9;
                pUpdateVolumeImg = &CVolume::UpdateVolumeImg9;
                pUpdateVolumeImgs = &CVolume::UpdateVolumeImgs9;
        pUpdateVolumeFromVolume = &CVolume::UpdateVolumeFromVolume9;
        pGetDevice = &CVolume::GetDevice9;
                pGetContainer = &CVolume::GetContainer9;
        pFreePrivateData = &CVolume::FreePrivateData9;
        pGetPrivateData = &CVolume::GetPrivateData9;
        pSetPrivateData = &CVolume::SetPrivateData9;
		bRet = Init9(pVolume);
	}
	else if (8.0f <= pDevice->GetInterface() && 9.0f > pDevice->GetInterface())
	{
                pRelVol = &CVolume::RelVol8;
        pGetDevice = &CVolume::GetDevice8;
                pGetDesc = &CVolume::GetDesc8;
                pLockBox = &CVolume::LockBox8;
                pUnlockBox = &CVolume::UnlockBox8;
                pUpdateVolumeImg = &CVolume::UpdateVolumeImg8;
                pUpdateVolumeImgs = &CVolume::UpdateVolumeImgs8;
        pUpdateVolumeFromVolume = &CVolume::UpdateVolumeFromVolume8;
        pGetDevice = &CVolume::GetDevice8;
        pGetContainer = &CVolume::GetContainer8;
        pFreePrivateData = &CVolume::FreePrivateData8;
        pGetPrivateData = &CVolume::GetPrivateData8;
        pSetPrivateData = &CVolume::SetPrivateData8;
        bRet = Init8(pVolume);
	}

	// Get the volume description
	GetDesc(&m_Desc);

	// Get the fmt mask
	FormatToMask(&(m_Desc.Format), &m_Mask);

	return bRet;
}

// -------------------------------------------------------------------------------------

CVolume::~CVolume()
{
	ReleaseAll();
	return;
}

// -------------------------------------------------------------------------------------

ULONG WINAPI CVolume::AddRef(void)
{
	if(m_pDevice)
		m_pDevice->AddRef();
	return CObject::AddRef();
}

// -------------------------------------------------------------------------------------

ULONG WINAPI CVolume::Release(void)
{
	if(m_pDevice)
		m_pDevice->Release();
	return CObject::Release();
}

// -------------------------------------------------------------------------------------

void CVolume::ReleaseAll()
{
	if(pRelVol)
		RelVol();
	return;
}

// -------------------------------------------------------------------------------------

DWORD CVolume::ReadColor(PVOID * ppSurfacePtr, bool bInc)
{
	return ::ReadColor(ppSurfacePtr, &(m_Desc.Format), &m_Mask, bInc);
}

// -------------------------------------------------------------------------------------

DWORD CVolume::ColorConvert(DWORD dwColor)
{
	return ::ColorConvert(dwColor, &m_Mask);
}

// -------------------------------------------------------------------------------------

HRESULT CVolume::LoadImage(CImage **ppImage)
{
	HRESULT hr = S_OK;

	if(ppImage == NULL)
		return E_FAIL;

	for(int i=0; i<m_Desc.Depth; i++)
	{
		hr = LoadImage(i, ppImage[i]);
		if(FAILED(hr))
			break;
	}
	return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CVolume::LoadImage(UINT uSlice, CImage *pImage)
{
    HRESULT hr;
	CImage *pScaledImage = NULL;
	FColor *pImageData = NULL;
	LOCKED_BOX LockedBox;
	BOX Box;
	BYTE bByte;
	signed char cY0, cU0, cV0, cY1, cU1, cV1; // For YUV formats

	if(NULL == pImage || pImage->GetWidth() < 1 || pImage->GetHeight() < 1 || uSlice >= m_Desc.Depth)
		return E_INVALIDARG;


	// Rescale the CImage if necessary
	if (m_Desc.Width != pImage->GetWidth() || 
		m_Desc.Height != pImage->GetHeight())
	{
		pScaledImage = new CImage();
		if( pScaledImage == NULL ||
		   !pScaledImage->Copy(pImage) || !pScaledImage->ScaleData(m_Desc.Width, m_Desc.Height))
		{
			hr = E_OUTOFMEMORY;
			goto Exit;
		}
		pImageData = pScaledImage->GetData();
   	}
	else
		pImageData = pImage->GetData();

	// Validate the CImageData
	if(pImageData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	// Lock the box
	Box.Left = 0;
	Box.Right = m_Desc.Width;
	Box.Top = 0;
	Box.Bottom = m_Desc.Height;
	Box.Front = uSlice;
	Box.Back = uSlice+1;
	hr = LockBox(&LockedBox, &Box, LOCK_NOSYSLOCK);
	if(FAILED(hr))
 	{
		goto Exit;
	}

	// Load the img into this slice
    for (unsigned y = 0; y < m_Desc.Height; ++y)
	{
		PBYTE pSurfData = (PBYTE) LockedBox.pBits;

		for (unsigned x = 0; x < m_Desc.Width; ++x)
		{
			FColor Color = *(pImageData + (y * m_Desc.Width) + x);

			switch((FMT)m_Desc.Format.d3dfFormat)
			{
			case FMT_P4:
				{
					//Only advance the ptr every other time
					Color.ConvertTo((FMT)m_Desc.Format.d3dfFormat, .5f, (char*)&bByte, FALSE, ELEMENTINDEX_0);
					if(x & 1)
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
					FColor Color2 = *(pImageData + (y * m_Desc.Width) + x + 1);
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
					
					x++;  // We write two pixels at once
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
		LockedBox.pBits = (PBYTE)LockedBox.pBits + LockedBox.RowPitch;
	}

Exit:
	if(FAILED(hr))
	{
		DPF(1, _T("CVolume::LoadImage() - failed to load image.  hr=%s(%x).\n"),
			m_pDevice->m_pD3D->HResultToString(hr), hr);
		m_pDevice->GetD3D()->SetLastError(hr);
	}
    SAFEDELETE(pScaledImage);

	// Unlock and return
    return UnlockBox();
}

// -------------------------------------------------------------------------------------

HRESULT CVolume::UpdateVolumeFromVolume(CVolume *pVol)
{
    HRESULT hr;
	LOCKED_BOX srcLock, trgLock;
	BOX Box;

	if(NULL == pVol)
		return E_FAIL;

	// Lock the box
	Box.Left = 0;
	Box.Right = m_Desc.Width;
	Box.Top = 0;
	Box.Bottom = m_Desc.Height;
	Box.Front = 0;
	Box.Back = m_Desc.Depth;
	hr = pVol->LockBox(&srcLock, &Box, LOCK_NOSYSLOCK);
	if(FAILED(hr))
 	{
		return hr;
	}

    hr = LockBox(&trgLock, &Box, LOCK_NOSYSLOCK);
	if(FAILED(hr))
 	{
		pVol->UnlockBox();
		return hr;
	}

	for (unsigned z = 0; z < m_Desc.Depth; ++z)
    {
        PBYTE pTrgSliceStart = (PBYTE) trgLock.pBits;
        PBYTE pSrcSliceStart = (PBYTE) srcLock.pBits;

		for (unsigned y = 0; y < m_Desc.Height; ++y)
		{
			PBYTE pTrgData = (PBYTE) trgLock.pBits + (trgLock.RowPitch * y);
            PBYTE pSrcData = (PBYTE) srcLock.pBits + (srcLock.RowPitch * y);
	
			memcpy(pTrgData, pSrcData, (m_Desc.Format.ddpfPixelFormat.dwRGBBitCount / 8) * m_Desc.Width);
			
		}
        trgLock.pBits = pTrgSliceStart + trgLock.SlicePitch;
        srcLock.pBits = pSrcSliceStart + srcLock.SlicePitch;
	}

	// Unlock and return
    pVol->UnlockBox();

	return UnlockBox();;
}

// -------------------------------------------------------------------------------------

void * CVolume::GetInternalPointer(void)
{
	if(m_pVolume9)
		return m_pVolume9;
    else if(m_pVolume8)
		return m_pVolume8;
	else
		return NULL;
}
