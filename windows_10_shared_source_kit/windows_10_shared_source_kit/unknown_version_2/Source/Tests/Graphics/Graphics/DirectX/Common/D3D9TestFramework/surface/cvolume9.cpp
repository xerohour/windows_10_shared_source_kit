#include "SurfacePCH.h"

#include "d3dx9.h"
#include "CD3DX.h"
#include "CSurface.h"
#include "CObjectUnknown.h"


bool CVolume::Init9(void* pVolume)
{
	m_pVolume9 = (LPDIRECT3DVOLUME9)pVolume;
	m_pVolume9->AddRef();

	return true;
}

bool CVolume::RelVol9()
{
    RELEASE(m_pVolume9);
	return true;
}

HRESULT CVolume::GetDesc9(LPVOLUME_DESC pDesc)
{
	HRESULT hr;
    D3DVOLUME_DESC D3DDesc;
	
	if (m_pVolume9 == NULL)
		return E_FAIL;

	hr = m_pVolume9->GetDesc( PassPointer(&D3DDesc, pDesc) );
	if(FAILED(hr))
		return hr;

    ZeroMemory(pDesc, sizeof(VOLUME_DESC));

	//Fill in the version independent struct.
	pDesc->Width = D3DDesc.Width;
	pDesc->Height = D3DDesc.Height;
	pDesc->Size = 0;
	pDesc->Depth = D3DDesc.Depth;
	pDesc->Type = (RESOURCETYPE)D3DDesc.Type;
	pDesc->Usage = D3DDesc.Usage;
	pDesc->Pool = D3DDesc.Pool;
	pDesc->Format.d3dfFormat = D3DDesc.Format;
	d3df2ddpf(D3DDesc.Format, &(pDesc->Format.ddpfPixelFormat));
	
	return S_OK;
}

HRESULT CVolume::LockBox9(LOCKED_BOX* pLockedBox, CONST BOX* pBox, DWORD dwFlags)
{
	D3DLOCKED_BOX Dx9LockBox;
	HRESULT hr = ERROR;

    if (NULL != m_pVolume9)
    {
		hr = m_pVolume9->LockBox( PassPointer(&Dx9LockBox, pLockedBox), (D3DBOX*)pBox, dwFlags);

        if (ValidPointer(pLockedBox))
        {
		    pLockedBox->RowPitch = Dx9LockBox.RowPitch;
		    pLockedBox->SlicePitch = Dx9LockBox.SlicePitch;
		    pLockedBox->pBits = Dx9LockBox.pBits;
        }
    }
    return hr;
}

HRESULT CVolume::UnlockBox9()
{
	return (NULL == m_pVolume9)? ERROR : m_pVolume9->UnlockBox();
}

HRESULT CVolume::UpdateVolumeImgs9(CImage **ppImage)
{
	HRESULT hr = 0;
	CImage *pScaledImg = NULL;
	FColor *pImageData = NULL;

    if (NULL == ppImage)
    {
        return E_FAIL;
    }

    // Use D3DX to setup the volume for us
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
        D3DBOX SrcBox  = {0,0,m_Desc.Width,m_Desc.Height,0,1};
        D3DBOX DestBox = {0,0,m_Desc.Width,m_Desc.Height,0,1};

		for (int i=0; i < m_Desc.Depth; i++)
        {
            DestBox.Front = i;
            DestBox.Back  = i + 1;

			// Rescale the CImage if necessary
			if (m_Desc.Width != ppImage[i]->GetWidth() || 
				m_Desc.Height != ppImage[i]->GetHeight())
			{
				pScaledImg = new CImage();
				if (pScaledImg == NULL ||
					!pScaledImg->Copy(ppImage[i]) || !pScaledImg->ScaleData(m_Desc.Width, m_Desc.Height))
				{
					return E_OUTOFMEMORY;
				}
				pImageData = pScaledImg->GetData();
   			}
			else
				pImageData = ppImage[i]->GetData();

			if (pImageData == NULL)
			{
				SAFEDELETE(pScaledImg);
				return E_OUTOFMEMORY;
			}

	        hr = D3DXLoadVolumeFromMemory(
								        m_pVolume9,			          //LPDIRECT3DVOLUME9     pDestVolume,
								        NULL,					      //D3DCOLOR*             pDestPalette,
								        &DestBox,				      //D3DBOX*               pDestRect,
								        (LPVOID)pImageData,           //LPVOID                pSrcMemory
								        D3DFMT_A32B32G32R32F,		  //D3DFORMAT             SrcFormat,
								        m_Desc.Width * 16,	          //UINT                  cbSrcRowPitch,
								        m_Desc.Width * 16 * m_Desc.Height,   //UINT           cbSrcSlicePitch,
								        NULL,					      //D3DCOLOR*             pSrcPalette,
                                        &SrcBox,                      //D3DBOX*               pSrcBox,
								        D3DX_FILTER_POINT,  	      //DWORD                 dwFilter
                                        0);                           //D3DCOLOR              ColorKey
			if(FAILED(hr))
	        {
				SAFEDELETE(pScaledImg);
				break;
	        }
			SAFEDELETE(pScaledImg);
        }
	    return hr;
    }
	else
		return LoadImage(ppImage);	
}

HRESULT CVolume::UpdateVolumeImg9(UINT uSlice, CImage *pImage)
{
	HRESULT hr;
	CImage *pScaledImg = NULL;
	FColor *pImageData = NULL;
	
    if (NULL == pImage)
    {
        return E_FAIL;
    }

	// Use D3DX to setup the volume for us
    if (m_Desc.Format.d3dfFormat == D3DFMT_DXT1 ||
		m_Desc.Format.d3dfFormat == D3DFMT_DXT2 ||
		m_Desc.Format.d3dfFormat == D3DFMT_DXT3 ||
		m_Desc.Format.d3dfFormat == D3DFMT_DXT4 ||
		m_Desc.Format.d3dfFormat == D3DFMT_DXT5 ||
		m_Desc.Format.d3dfFormat == D3DFMT_UYVY ||
		m_Desc.Format.d3dfFormat == D3DFMT_R8G8_B8G8 ||
		m_Desc.Format.d3dfFormat == D3DFMT_YUY2 ||
		m_Desc.Format.d3dfFormat == FMT_G8R8_G8B8)
	{
        D3DBOX SrcBox  = {0,0,m_Desc.Width,m_Desc.Height,0,1};
        D3DBOX DestBox = {0,0,m_Desc.Width,m_Desc.Height,uSlice,uSlice+1};

		//create and rescale the CImage if necessary
		if (m_Desc.Width != pImage->GetWidth() || 
			m_Desc.Height != pImage->GetHeight())
		{
			pScaledImg = new CImage();
			if (pScaledImg == NULL ||
				!pScaledImg->Copy(pImage) || !pScaledImg->ScaleData(m_Desc.Width, m_Desc.Height))
			{
				return E_OUTOFMEMORY;
			}
			pImageData = pScaledImg->GetData();
   		}
		else
			pImageData = pImage->GetData();

		if (pImageData == NULL)
		{
			SAFEDELETE(pScaledImg);
			return E_OUTOFMEMORY;
		}

		hr = ::D3DXLoadVolumeFromMemory(
								    m_pVolume9,			          //LPDIRECT3DVOLUME9     pDestVolume,
								    NULL,					      //D3DCOLOR*             pDestPalette,
								    &DestBox,			          //D3DBOX*               pDestRect,
								    (LPVOID)pImageData,	          //LPVOID                pSrcMemory
								    D3DFMT_A32B32G32R32F,		  //D3DFORMAT             SrcFormat,
								    m_Desc.Width * 16,	          //UINT                  cbSrcRowPitch,
								    m_Desc.Width * 16 * m_Desc.Height,   //UINT           cbSrcSlicePitch,
								    NULL,					      //D3DCOLOR*             pSrcPalette,
                                    &SrcBox,                      //D3DBOX*               pSrcBox,
								    D3DX_FILTER_POINT,  	      //DWORD                 dwFilter
                                    0);                           //D3DCOLOR              ColorKey
		SAFEDELETE(pScaledImg);
        return hr;
    }
	else
		return LoadImage(uSlice, pImage);

}

HRESULT CVolume::UpdateVolumeFromVolume9(CVolume *pVol)
{
	HRESULT hr = S_OK;

    if (NULL == pVol)
    {
        return E_FAIL;
    }

    // Use D3DX to setup the volume for us
    D3DBOX SrcBox  = {0,0,m_Desc.Width,m_Desc.Height,0,m_Desc.Depth};
    D3DBOX DestBox = {0,0,m_Desc.Width,m_Desc.Height,0,m_Desc.Depth};

	hr = D3DXLoadVolumeFromVolume(  m_pVolume9,			          //LPDIRECT3DVOLUME9     pDestVolume,
							        NULL,					      //D3DCOLOR*             pDestPalette,
							        &DestBox,			          //D3DBOX*               pDestBox,
							        pVol->m_pVolume9,	  		  //LPVOID                pSrcVolume
                                    NULL,					      //D3DCOLOR*             pSrcPalette,
                                    &SrcBox,                      //D3DBOX*               pSrcBox,
							        D3DX_FILTER_POINT,  	      //DWORD                 dwFilter
                                    0);                           //D3DCOLOR              ColorKey
	return hr;
}
// -------------------------------------------------------------------------------------

HRESULT CVolume::GetContainer9(REFIID riid, void** ppContainer)
{
	IUnknown* pObject = NULL;
	HRESULT hr = m_pVolume9->GetContainer(riid, PassPointer((void**)&pObject, ppContainer));

	if (SUCCEEDED(hr))
	{
		if (riid == IID_IDirect3DVolumeTexture9)
		{
			LPVOLUMETEXTURE pVolTex = new CVolTexture;
			if (pVolTex == NULL)
			{
				RELEASE(pObject);
				return E_FAIL;
			}
			if (!pVolTex->Init(pObject, m_pDevice))
			{
				RELEASE(pObject);
				delete pVolTex;
				return E_FAIL;
			}
			*ppContainer = pVolTex;
		}
		else if (riid == IID_IDirect3DVolume9)
		{
			LPVOLUME pVol = new CVolume;
			if (pVol == NULL)
			{
				RELEASE(pObject);
				return E_FAIL;
			}
			if (!pVol->Init(pObject, m_pDevice, this))
			{
				RELEASE(pObject);
				delete pVol;
				return E_FAIL;
			}
			*ppContainer = pVol;
		}
		else if (riid == IID_IUnknown)
		{
			CObjectUnknown *pUnknown = new CObjectUnknown;
			if (pUnknown == NULL || !pUnknown->Init(pObject))
			{
				RELEASE(pObject);
				delete pUnknown;
				return E_FAIL;
			}
			*ppContainer = pUnknown;
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

HRESULT CVolume::FreePrivateData9(REFGUID refguid)
{
	return m_pVolume9->FreePrivateData(refguid);
}

// -------------------------------------------------------------------------------------

HRESULT CVolume::GetPrivateData9(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return m_pVolume9->GetPrivateData(refguid, pData, pSizeOfData);
}

// -------------------------------------------------------------------------------------

HRESULT CVolume::SetPrivateData9(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags)
{
	return m_pVolume9->SetPrivateData(refguid, pData, dwSizeOfData, dwFlags);
}

// -------------------------------------------------------------------------------------

HRESULT CVolume::GetDevice9(LPDEVICE *ppDevice)
{
	IDirect3DDevice9 *pDev = NULL;
	HRESULT hr;
    
    hr = m_pVolume9->GetDevice( PassPointer(&pDev, ppDevice) );

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
