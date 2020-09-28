#include "TexturesPCH.h"

#include <d3d9.h>
#include "CTextures.h"


// --------------------------------------------------------------------------------------

bool CVolTexture::Init9(PVOID pTex)
{
    LPDIRECT3DVOLUMETEXTURE9 pTexture = (LPDIRECT3DVOLUMETEXTURE9)pTex;

	if(pTexture == NULL)
	{
		DPF(1, _T("CVolTexture::Init9() - invalid params.\n"));
		return false;
	}

	//This is important as Dx will accept 0 and create all levels.
	m_dwLevelCount = pTexture->GetLevelCount();
	
	m_pTexture9 = pTexture;
	m_pTexture9->AddRef();

	return true;
}

// --------------------------------------------------------------------------------------

void CVolTexture::ReleaseD3D9()
{
	RELEASE(m_pTexture9);	
	RELEASE(m_pSysTexture);
}

// --------------------------------------------------------------------------------------

HRESULT CVolTexture::AddDirtyBox9(CONST BOX* pDirtyBox)
{
	if( 0L == m_pTexture9 )
		return E_FAIL;

	D3DBOX d3dBox;
	memset( &d3dBox, 0, sizeof(d3dBox) );

    if ( ValidPointer( pDirtyBox ) )
    {
	    d3dBox.Left   = pDirtyBox->Left;
	    d3dBox.Top    = pDirtyBox->Top;
	    d3dBox.Right  = pDirtyBox->Right;
	    d3dBox.Bottom = pDirtyBox->Bottom;
	    d3dBox.Front  = pDirtyBox->Front;
	    d3dBox.Back   = pDirtyBox->Back;
    }

	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->AddDirtyBox( PassPointer( &d3dBox, pDirtyBox ) );
}

// --------------------------------------------------------------------------------------

DWORD CVolTexture::SetLOD9(DWORD dwLODNew)
{
	if (0L == m_pTexture9)
		return 0;
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->SetLOD(dwLODNew);
}

// --------------------------------------------------------------------------------------


DWORD CVolTexture::GetLOD9()
{
	if (0L == m_pTexture9)
		return 0;
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->GetLOD();
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::SetAutoGenFilterType9(DWORD FilterType)
{
	if (NULL == m_pTexture9)
		return E_FAIL;
	return m_pTexture9->SetAutoGenFilterType((D3DTEXTUREFILTERTYPE)FilterType);
}

// -------------------------------------------------------------------------------------

DWORD CVolTexture::GetAutoGenFilterType9(void)
{
	if (NULL == m_pTexture9)
		return 0;
	return m_pTexture9->GetAutoGenFilterType();
}

// -------------------------------------------------------------------------------------

void CVolTexture::GenerateMipSubLevels9(void)
{
	if (NULL == m_pTexture9)
		return;
	m_pTexture9->GenerateMipSubLevels();
}

// --------------------------------------------------------------------------------------

DWORD CVolTexture::SetPriority9(DWORD PriorityNew)
{
	if (0L == m_pTexture9)
		return 0;
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->SetPriority(PriorityNew);
}

// --------------------------------------------------------------------------------------

DWORD CVolTexture::GetPriority9()
{
	if (0L == m_pTexture9)
		return 0;
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->GetPriority();
}

// --------------------------------------------------------------------------------------

void CVolTexture::PreLoad9()
{
	if (0L == m_pTexture9)
		return;
	((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->PreLoad();
}

// --------------------------------------------------------------------------------------

HRESULT CVolTexture::GetLevelDesc9(UINT Level, VOLUME_DESC *pDesc)
{
	if (0L == m_pTexture9)
		return E_FAIL;

	D3DVOLUME_DESC d3dVolDesc;
	memset( &d3dVolDesc, 0, sizeof( d3dVolDesc ) );

	HRESULT hr = ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->GetLevelDesc(Level, PassPointer(&d3dVolDesc, pDesc));
	if( FAILED(hr) )
		return hr;

    if (ValidPointer(pDesc))
    {
	    pDesc->Format.d3dfFormat = d3dVolDesc.Format;
	    pDesc->Size = 0;
	    pDesc->Width = d3dVolDesc.Width;
	    pDesc->Height = d3dVolDesc.Height;
	    pDesc->Depth = d3dVolDesc.Depth;
	    pDesc->Usage = d3dVolDesc.Usage;	// SDK Docs "Not currently used, always returned as 0."
	    d3df2ddpf(d3dVolDesc.Format, &(pDesc->Format.ddpfPixelFormat));

	    switch( d3dVolDesc.Type )
	    {
	    case D3DRTYPE_SURFACE:
		    pDesc->Type = RTYPE_SURFACE;
		    break;
	    case D3DRTYPE_VOLUME:
		    pDesc->Type = RTYPE_VOLUME;			// the only valid case hopefully
		    break;
	    case D3DRTYPE_TEXTURE:
		    pDesc->Type = RTYPE_TEXTURE;
		    break;
	    case D3DRTYPE_VOLUMETEXTURE:
		    pDesc->Type = RTYPE_VOLUMETEXTURE;
		    break;
	    case D3DRTYPE_CUBETEXTURE:
		    pDesc->Type = RTYPE_CUBETEXTURE;
		    break;
	    case D3DRTYPE_VERTEXBUFFER:
		    pDesc->Type = RTYPE_VERTEXBUFFER;
		    break;
	    case D3DRTYPE_INDEXBUFFER:
		    pDesc->Type = RTYPE_INDEXBUFFER;
		    break;
	    default:
		    pDesc->Type = (RESOURCETYPE)0;
	    }
	    
	    switch( d3dVolDesc.Pool )
	    {
	    case D3DPOOL_DEFAULT:
		    pDesc->Pool = POOL_DEFAULT;
		    break;
	    case D3DPOOL_MANAGED:
		    pDesc->Pool = POOL_MANAGED;
		    break;
		case (D3DPOOL)6:
			pDesc->Pool = POOL_MANAGED_INTERNAL;
	    case D3DPOOL_SYSTEMMEM:
		    pDesc->Pool = POOL_SYSTEMMEM;
		    break;
	    default:
		    pDesc->Pool = (DWORD)0;
	    }
    }

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVolTexture::GetVolumeLevel9(UINT Level, LPVOLUME* ppVolumeLevel)
{
	HRESULT hr;
    LPDIRECT3DVOLUME9 pd3dVolume = NULL;
	LPVOLUME          pVolume = NULL;

	if (NULL == m_pTexture9)
		return E_FAIL;

    hr = ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->GetVolumeLevel(Level, PassPointer(&pd3dVolume, ppVolumeLevel) );
	if( FAILED(hr) )
		return hr;

    if (ValidPointer(ppVolumeLevel))
    {
	    pVolume = new CVolume;
	    if(pVolume == NULL || !pVolume->Init(pd3dVolume, m_pDevice, NULL))
	    {
		    hr = E_FAIL;
		    goto Exit;
	    }

	    *ppVolumeLevel = pVolume;
    }

Exit:
	
	if(FAILED(hr))
		RELEASE(pVolume)

	RELEASE(pd3dVolume);
	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVolTexture::LockBox9(UINT Level, LOCKED_BOX* pLockedVolume, CONST BOX* pBox, DWORD Flags)
{
	if (0L == m_pTexture9)
		return E_FAIL;

	D3DBOX d3dBox;
	memset(&d3dBox,0,sizeof(d3dBox));
	D3DLOCKED_BOX d3dLockedVolume;
	memset(&d3dLockedVolume, 0, sizeof(d3dLockedVolume));
	HRESULT hr;

    if ( ValidPointer( pBox ) )
    {
		d3dBox.Left   = pBox->Left;
		d3dBox.Top    = pBox->Top;
		d3dBox.Right  = pBox->Right;
		d3dBox.Bottom = pBox->Bottom;
		d3dBox.Front  = pBox->Front;
		d3dBox.Back   = pBox->Back;
    }

	hr = ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->LockBox(Level, 
                                                          PassPointer(&d3dLockedVolume, pLockedVolume), 
                                                          PassPointer(&d3dBox, pBox),
                                                          Flags);

	if( FAILED(hr) )
		return hr;

	pLockedVolume->RowPitch   = d3dLockedVolume.RowPitch;
	pLockedVolume->SlicePitch = d3dLockedVolume.SlicePitch;
	pLockedVolume->pBits      = d3dLockedVolume.pBits;

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVolTexture::UnlockBox9(UINT Level)
{
	if (0L == m_pTexture9)
		return E_FAIL;
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->UnlockBox(Level);
}

// --------------------------------------------------------------------------------------

HRESULT CVolTexture::SetPalette9(PALETTEENTRY *pPalette)
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
		DPF(1, _T("CVolTexture::SetPalette9() - Format is not palettized.\n"));
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

HRESULT CVolTexture::GetDevice9(CDevice** ppDevice)
{
	IDirect3DDevice9 *pDev = NULL;
	HRESULT hr;

    hr = ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->GetDevice( PassPointer( &pDev, ppDevice) );

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

HRESULT CVolTexture::FreePrivateData9(REFGUID refguid)
{
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->FreePrivateData(refguid);
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::GetPrivateData9(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->GetPrivateData(refguid, pData, pSizeOfData);
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::SetPrivateData9(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags)
{
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->SetPrivateData(refguid, pData, dwSizeOfData, dwFlags);
}
