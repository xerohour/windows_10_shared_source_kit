#include "SurfacePCH.h"

#include <d3dx8.h>
#include "CD3DX.h"
#include "CSurface.h"
#include "CObjectUnknown.h"


bool CVolume::Init8(void* pVolume)
{
	m_pVolume8 = (LPDIRECT3DVOLUME8)pVolume;
	m_pVolume8->AddRef();
	
	return true;
}

bool CVolume::RelVol8()
{
    RELEASE(m_pVolume8);
	return true;
}

HRESULT CVolume::GetDesc8(LPVOLUME_DESC pDesc)
{
	HRESULT hr;
    D3DVOLUME_DESC D3DDesc;
	
	if (m_pVolume8 == NULL)
		return E_FAIL;

	hr = m_pVolume8->GetDesc( PassPointer(&D3DDesc, pDesc) );
	if(FAILED(hr))
		return hr;

    ZeroMemory(pDesc, sizeof(VOLUME_DESC));

	//Fill in the version independent struct.
	pDesc->Width = D3DDesc.Width;
	pDesc->Height = D3DDesc.Height;
	pDesc->Size = D3DDesc.Size;
	pDesc->Depth = D3DDesc.Depth;
	pDesc->Type = (RESOURCETYPE)D3DDesc.Type;
	pDesc->Usage = D3DDesc.Usage;
	pDesc->Pool = D3DDesc.Pool;
	pDesc->Format.d3dfFormat = D3DDesc.Format;
	d3df2ddpf(D3DDesc.Format, &(pDesc->Format.ddpfPixelFormat));
	
	return S_OK;
}

HRESULT CVolume::LockBox8(LOCKED_BOX* pLockedBox, CONST BOX* pBox, DWORD dwFlags)
{
	D3DLOCKED_BOX Dx8LockBox;
	HRESULT hr = ERROR;

    if (NULL != m_pVolume8)
    {
		hr = m_pVolume8->LockBox( PassPointer(&Dx8LockBox, pLockedBox), (D3DBOX*)pBox, dwFlags);

        if (ValidPointer(pLockedBox))
        {
		    pLockedBox->RowPitch = Dx8LockBox.RowPitch;
		    pLockedBox->SlicePitch = Dx8LockBox.SlicePitch;
		    pLockedBox->pBits = Dx8LockBox.pBits;
        }
    }
    return hr;
}

HRESULT CVolume::UnlockBox8()
{
	return (NULL == m_pVolume8)? ERROR : m_pVolume8->UnlockBox();
}

HRESULT CVolume::UpdateVolumeImgs8(CImage **ppImage)
{
	HRESULT hr = 0;
	CImage *pScaledImg = NULL;
	DWORD  *pImageData = NULL;

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
		m_Desc.Format.d3dfFormat == D3DFMT_YUY2)
	{
        D3DBOX SrcBox  = {0,0,m_Desc.Width,m_Desc.Height,0,1};
        D3DBOX DestBox = {0,0,m_Desc.Width,m_Desc.Height,0,1};

		for (int i=0; i < m_Desc.Depth; i++)
        {
            DestBox.Front = i;
            DestBox.Back  = i + 1;

			//create a local copy of the image and rescale if necessary
			if (m_Desc.Width != ppImage[i]->GetWidth() || 
				m_Desc.Height != ppImage[i]->GetHeight())
			{
				pScaledImg = new CImage();
				if ( pScaledImg == NULL ||
					!pScaledImg->Copy(ppImage[i]) || !pScaledImg->ScaleData(m_Desc.Width, m_Desc.Height))
				{
					return E_OUTOFMEMORY;
				}				
				pImageData = pScaledImg->GetPackedData();
   			}
			else
				pImageData = ppImage[i]->GetPackedData();

			if (pImageData == NULL)
			{
				SAFEDELETE(pScaledImg);
				return E_OUTOFMEMORY;
			}

	        hr = D3DXLoadVolumeFromMemory(
								        m_pVolume8,			          //LPDIRECT3DVOLUME8     pDestVolume,
								        NULL,					      //D3DCOLOR*             pDestPalette,
								        &DestBox,				      //D3DBOX*               pDestRect,
								        (LPVOID)pImageData,           //LPVOID                pSrcMemory
								        D3DFMT_A8R8G8B8,		      //D3DFORMAT             SrcFormat,
								        m_Desc.Width * 4,	          //UINT                  cbSrcRowPitch,
								        m_Desc.Width * 4 * m_Desc.Height,   //UINT            cbSrcSlicePitch,
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

HRESULT CVolume::UpdateVolumeImg8(UINT uSlice, CImage *pImage)
{
	HRESULT hr = 0;
	CImage *pScaledImg = NULL;
	DWORD  *pImageData = NULL;

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
		m_Desc.Format.d3dfFormat == D3DFMT_YUY2)
	{
        BOX SrcBox  = {0,0,m_Desc.Width,m_Desc.Height,0,1};
        BOX DestBox = {0,0,m_Desc.Width,m_Desc.Height,uSlice,uSlice+1};

		//create and rescale the CImage if necessary
		if (m_Desc.Width != pImage->GetWidth() || 
			m_Desc.Height != pImage->GetHeight())
		{
			pScaledImg = new CImage();
			if ( pScaledImg == NULL ||
				!pScaledImg->Copy(pImage) || !pScaledImg->ScaleData(m_Desc.Width, m_Desc.Height))
			{
				return E_OUTOFMEMORY;
			}
			pImageData = pScaledImg->GetPackedData();
   		}
		else
			pImageData = pImage->GetPackedData();

		if (pImageData == NULL)
		{
			SAFEDELETE(pScaledImg);
			return E_OUTOFMEMORY;
		}

	    hr = D3DXLoadVolumeFromMemory(
								    m_pVolume8,			          //LPDIRECT3DVOLUME8     pDestVolume,
								    NULL,					      //D3DCOLOR*             pDestPalette,
								    (D3DBOX*)&DestBox,			  //D3DBOX*               pDestRect,
								    (LPVOID)pImageData,	          //LPVOID                pSrcMemory
								    D3DFMT_A8R8G8B8,		      //D3DFORMAT             SrcFormat,
								    m_Desc.Width * 4,	          //UINT                  cbSrcRowPitch,
								    m_Desc.Width * 4 * m_Desc.Height,   //UINT            cbSrcSlicePitch,
								    NULL,					      //D3DCOLOR*             pSrcPalette,
                                    (D3DBOX*)&SrcBox,             //D3DBOX*               pSrcBox,
								    D3DX_FILTER_POINT,  	      //DWORD                 dwFilter
                                    0);                           //D3DCOLOR              ColorKey
		SAFEDELETE(pScaledImg);
	    return hr;
    }
	else
		return LoadImage(uSlice, pImage);

}

HRESULT CVolume::UpdateVolumeFromVolume8(CVolume *pVol)
{
	HRESULT hr = S_OK;

    if (NULL == pVol)
    {
        return E_FAIL;
    }

    // Use D3DX to setup the volume for us
    BOX SrcBox  = {0,0,m_Desc.Width,m_Desc.Height,0,m_Desc.Depth};
    BOX DestBox = {0,0,m_Desc.Width,m_Desc.Height,0,m_Desc.Depth};

    hr = D3DXLoadVolumeFromVolume(  m_pVolume8,			          //LPDIRECT3DVOLUME8     pDestVolume,
							        NULL,					      //D3DCOLOR*             pDestPalette,
							        (D3DBOX*)&DestBox,			  //D3DBOX*               pDestBox,
							        pVol->m_pVolume8,	  		  //LPVOID                pSrcVolume
                                    NULL,					      //D3DCOLOR*             pSrcPalette,
                                    (D3DBOX*)&SrcBox,             //D3DBOX*               pSrcBox,
							        D3DX_FILTER_POINT,  	      //DWORD                 dwFilter
                                    0);                          //D3DCOLOR              ColorKey
	return hr;
}

// -------------------------------------------------------------------------------------

HRESULT CVolume::GetContainer8(REFIID riid, void** ppContainer)
{
	IUnknown* pObject = NULL;
	HRESULT hr = m_pVolume8->GetContainer(riid, PassPointer((void**)&pObject, ppContainer));

	if (SUCCEEDED(hr))
	{
		if (riid == IID_IDirect3DVolumeTexture8)
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
		else if (riid == IID_IDirect3DVolume8)
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

HRESULT CVolume::FreePrivateData8(REFGUID refguid)
{
	return m_pVolume8->FreePrivateData(refguid);
}

// -------------------------------------------------------------------------------------

HRESULT CVolume::GetPrivateData8(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return m_pVolume8->GetPrivateData(refguid, pData, pSizeOfData);
}

// -------------------------------------------------------------------------------------

HRESULT CVolume::SetPrivateData8(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags)
{
	return m_pVolume8->SetPrivateData(refguid, pData, dwSizeOfData, dwFlags);
}

// -------------------------------------------------------------------------------------

HRESULT CVolume::GetDevice8(LPDEVICE *ppDevice)
{
	IDirect3DDevice8 *pDev = NULL;
	HRESULT hr;

    hr = m_pVolume8->GetDevice( PassPointer(&pDev, ppDevice) );

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
