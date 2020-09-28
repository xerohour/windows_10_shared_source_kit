// ======================================================================================
//
//  CDevice7.cpp
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================


#include "DevicePCH.h"

#include "CDevice.h"
#include <d3d.h>


#include "CSurface.h"
#include "CTextures.h"
#include "CSwapChain.h"
#include "CBuffers.h"


typedef HRESULT (WINAPI * DDCREATEEX)(GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter);

// --------------------------------------------------------------------------------------

HRESULT CDevice::Initialize7(PRESENT_PARAMETERS *pParams)
{
	ADAPTER    *pAdapter = GetAdapter();
	DDCREATEEX  pProc = (DDCREATEEX) GetProcAddress(GetModuleHandle("DDRAW.DLL"),"DirectDrawCreateEx");
	HRESULT hr;
	DWORD dwZFlags=0;
	TCHAR szFormat[64] = {0};
	
	if(pAdapter == NULL || pProc == NULL || m_pD3D == NULL || pParams == NULL)
	{
		DPF(1, _T("CDevice::Initialize7() - invalid params.\n"));
		return E_FAIL;
	}

	// Validate the display mode
	if(!m_pD3D->IsDisplayModeSupported(GetAdapter(), GetAdapter()->DesktopMode.dwWidth, GetAdapter()->DesktopMode.dwHeight,(FMT)GetAdapter()->DesktopMode.Format.d3dfFormat))
	{
		FmtToString(FORMAT_TEXTURE, &GetAdapter()->DesktopMode.Format, szFormat);
		DPF(1, _T("Display Mode (%dx%dx%s) not supported.\n"),GetAdapter()->DesktopMode.dwWidth, GetAdapter()->DesktopMode.dwHeight, szFormat);
		return E_UNSUPPORTED;
	}

	// Copy the present params locally
    m_pPresParams = new PRESENT_PARAMETERS;
	if(NULL == m_pPresParams)
	{
    	DPF(1, _T("CDevice::Initialize7() - Out of memory.\n"));
		return  E_OUTOFMEMORY;
	}

	if(m_pD3D->m_pDDraw7 == NULL || m_pD3D->m_pD3D7 == NULL)
	{		
		// Setup D3D for this adapter
		hr = m_pD3D->SetupD3D7(this, pParams);
		if(FAILED(hr))
		{
			DPF(2, _T("CDevice::Initialize7() - SetupD3D() failed.\n"));
			ReleaseD3D();
			return hr;
		}
	}

	// Get a local handle to the Primary
	m_pPrimary = m_pD3D->m_pPrimary;
	m_pPrimary->AddRef();
	
	// Create the BackBuffers
	hr = CreateBackBuffers7(pParams);
	if (FAILED(hr))
	{
		DPF(1, _T("CDevice::Initialize7() - CreateBackBuffers7 failed. hr=%s(%x)\n"),
			m_pD3D->HResultToString(hr), hr);
		ReleaseD3D();

        // Make out of memory a valid failure
        if(hr == DDERR_OUTOFVIDEOMEMORY)
        {
            hr = E_UNSUPPORTED;
        }
		return hr;
	}

	//If the format of the backbuffer changed, we need to update the pres params.
	memcpy(m_pPresParams, pParams, sizeof(PRESENT_PARAMETERS));

    // If it's a hardware device, create in video, else create in system
    if (m_dwDevType & DEVICETYPE_HAL)
        dwZFlags = POOL_VIDEO;
    else
        dwZFlags = POOL_SYSTEMMEM;

	if(pParams->bAutoDepthStencil)
	{
		// Create the Z Buffer
		hr = CreateDepthStencilSurface7(pParams->uWidth, pParams->uHeight, pParams->AutoDepthStencilFormat, MULTISAMPLE_NONE, &m_pZBuffer);
		if (FAILED(hr))
		{
			DPF(1, _T("CDevice::Initialize7() - Failed to create the ZBuffer. hr=%s(%x)\n"),
				m_pD3D->HResultToString(hr), hr);
			ReleaseD3D();

            // Make out of memory a valid failure
            if(hr == DDERR_OUTOFVIDEOMEMORY)
            {
                hr = E_UNSUPPORTED;
            }
			return hr;
		}
	
		// Attach the ZBuffer to the backbuffer
		hr = m_pBackBuffers[0]->AddAttachedSurface(m_pZBuffer);
		if(FAILED(hr))
		{
			DPF(1, _T("CDevice::Initialize7() - AddAttachedSurface failed. hr=%s(%x).\n"),
				m_pD3D->HResultToString(hr), hr);
			ReleaseD3D();
			return hr;
		}
	}	
	// Decode the device type
	if(m_dwDevType & DEVICETYPE_HAL)
	{
		if(m_dwBehaviors & DEVICECREATE_HWVP)
		{
			memcpy(&m_Guid, &IID_IDirect3DTnLHalDevice, sizeof(m_Guid));
			m_bHWVP = true;
		}
		else
            memcpy(&m_Guid, &IID_IDirect3DHALDevice, sizeof(m_Guid));
	}
	else if(m_dwDevType & DEVICETYPE_REF)
		memcpy(&m_Guid, &IID_IDirect3DRefDevice, sizeof(m_Guid));

	else if(m_dwDevType & DEVICETYPE_SW)
		memcpy(&m_Guid, &IID_IDirect3DRGBDevice, sizeof(m_Guid));
	else
	{
		DPF(1, _T("CDevice::Initialize7() - unknown device type.\n"));
		ReleaseD3D();
		return E_FAIL;
	}

	// Create the actual device
	hr = m_pD3D->CreateDevice(m_Guid, m_pBackBuffers[0]->GetD3DSurface7(), &m_pDevice7);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::Initialize7() - CreateDevice failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr); 
		ReleaseD3D();


        // Make out of memory a valid failure
        if(hr == DDERR_OUTOFVIDEOMEMORY)
        {
            hr = E_UNSUPPORTED;
        }
		return S_FALSE;
	}

	return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::ValidateMode7(DEVICEMODE *pMode)
{
	TCHAR szBuffer[64] = {0};
	PADAPTER pAdapter = m_pD3D->GetAdapter(pMode->nAdapter);

	if
	(
		(
			(pMode->dwBehaviors & DEVICECREATE_HWVP_OR_PURE) ||
			(pMode->dwBehaviors & DEVICECREATE_MIXED) ||
			(pMode->dwBehaviors & DEVICECREATE_HWVP)
		) &&
		(m_Desc.dwDevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0
	)
	{
		DPF(1, _T("Hardware vertex processing not supported by this adapter.\n"));
		return E_UNSUPPORTED;
	}

	if(((FMT)pMode->pDispMode->Format.d3dfFormat != FMT_A2R10G10B10 &&
		(FMT)pMode->pDispMode->Format.d3dfFormat != FMT_A2B10G10R10_XR_BIAS &&
	    (FMT)pMode->pDispMode->Format.d3dfFormat != FMT_X8R8G8B8 &&
	    (FMT)pMode->pDispMode->Format.d3dfFormat != FMT_R5G6B5) ||
	   ((FMT)pMode->pDispMode->Format.d3dfFormat == FMT_A2R10G10B10 || (FMT)pMode->pDispMode->Format.d3dfFormat == FMT_A2B10G10R10_XR_BIAS) ||
	   ((FMT)pMode->pDispMode->Format.d3dfFormat == FMT_X8R8G8B8 && !(m_Desc.dwDeviceRenderBitDepth & DDBD_32)) ||
	   ((FMT)pMode->pDispMode->Format.d3dfFormat == FMT_R5G6B5   && !(m_Desc.dwDeviceRenderBitDepth & DDBD_16)))
	{
		FmtToString(FORMAT_TEXTURE,&pMode->pDispMode->Format,szBuffer);
		DPF(1, _T("D3D Device cannot render in the requested display mode(%s).\n"), szBuffer);
		return E_UNSUPPORTED;
	}

    if( (m_dwDevType == DEVICETYPE_REF) && !m_pD3D->IsRefInstalled())
	{
        DPF(1, _T("Reference driver(%s) not detected.\n"), m_pD3D->GetRefDLL());
		return E_ABORT;
	}

	if( VPEMUREGCHANGE_ENABLE == m_dwVPEmuRegChange || PPEMUREGCHANGE_ENABLE == m_dwPPEmuRegChange )
	{
		if ( PPEMUREGCHANGE_ENABLE == m_dwPPEmuRegChange && DEVICECREATE_HWVP & pMode->dwBehaviors && VPEMUREGCHANGE_ENABLE != m_dwVPEmuRegChange )
		{
			DPF(1, _T("Cannot enable pixel pipeline emulation with hardware VP without the vertex pipeline emulation.\n"));
			return E_UNSUPPORTED;
		}

		if ( DEVICETYPE_REF == pMode->dwDevType )
		{
			DPF(1, _T("Cannot enable pipeline emulation with hardware that does not support shader model 2.0.\n"));
			return E_UNSUPPORTED;
		}

		if ( DRIVERMODEL_LDDM == m_pD3D->GetDriverModel() )
		{
			const CAPS *caps = GetCachedCaps9();

			if ( caps && (0xFFFF & (2<<8)) > (0xFFFF &  caps->VertexShaderVersion) )
			{
				DPF(1, _T("Cannot enable pipeline emulation with hardware that does not support shader model 2.0.\n"));
				return E_UNSUPPORTED;
			}
			
			if ( DEVICECREATE_SWVP & pMode->dwBehaviors && VPEMUREGCHANGE_ENABLE == m_dwVPEmuRegChange )
			{
				DPF(1, _T("Cannot enable vertex pipeline emulation with software VP on pre-DX9 devices.\n"));
				return E_UNSUPPORTED;
			}
		}
		else
		{
			//Not LDDM, and not a 9 device
			DPF(1, _T("Cannot enable pipeline emulation on non LDDM drivers for pre-DX9 devices.\n"));
			return E_UNSUPPORTED;
		}
	}

	return S_OK;
}


// --------------------------------------------------------------------------------------
bool CDevice::IsDeviceLost7()
{
	HRESULT hr = m_pD3D->m_pDDraw7->TestCooperativeLevel();
	if ( FAILED( hr ) )
	{
		return true;
	};

	hr = m_pBackBuffers[0]->GetD3DSurface7()->IsLost();
	if ( FAILED( hr ) )
	{
		return true;
	};

	return false;
}


// --------------------------------------------------------------------------------------
bool CDevice::IsDeviceReadyForReset7()
{
	HRESULT hDeviceState = m_pD3D->m_pDDraw7->TestCooperativeLevel();

	if ( SUCCEEDED( hDeviceState ) )
	{
		return true;
	};

	return false;
}


// --------------------------------------------------------------------------------------
bool CDevice::HandleDeviceLost7()
{
	HRESULT hr;

	hr = m_pD3D->m_pDDraw7->TestCooperativeLevel();
	if(FAILED(hr))
		return false;

	hr = m_pD3D->m_pDDraw7->RestoreAllSurfaces();
    if(FAILED(hr))
	{
		DPF(1, _T("CDevice::HandleDeviceLost7() - RestoreAllSurfaces() failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		return false;
	}

	return true;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::Resize7(UINT uWidth, UINT uHeight)
{
	DPF(1, _T("CDevice::Resize7() - Currently not implemented.\n"));
	return E_NOTIMPL;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateTexture7(UINT Width,UINT Height,UINT Levels,DWORD Usage,
								FORMAT Format,DWORD Pool,CTexture** ppTexture)
{
    HRESULT hr;
    CTexture *pTexture = NULL, *pTexOut = NULL;
	LPDIRECTDRAWSURFACE7 pD3DTexture = NULL;
	DDSURFACEDESC2 ddDesc;
	
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::CreateTexture7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    memset(&ddDesc, 0, sizeof(DDSURFACEDESC2));
    ddDesc.dwSize = sizeof(DDSURFACEDESC2);
	ddDesc.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_CAPS;
	ddDesc.dwHeight = Height;
	ddDesc.dwWidth = Width;
	ddDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
	memcpy(&(ddDesc.ddpfPixelFormat), &(Format.ddpfPixelFormat), sizeof(DDPIXELFORMAT));

   	// Remove any private pixel format flags that were added.  We are also including
	// the PF_YUV flag as the YUV fourcc textures don't use it. 
	ddDesc.ddpfPixelFormat.dwFlags &= ~(PF_PRIVATE | PF_YUV);

	// Create the appropriate MipMap chain based on Levels requested
	if(Levels > 1)
	{
		ddDesc.dwFlags |= DDSD_MIPMAPCOUNT;
		ddDesc.dwMipMapCount = Levels;
	}

	if(Levels != 1)
	{
		ddDesc.ddsCaps.dwCaps |= (DDSCAPS_MIPMAP | DDSCAPS_COMPLEX);
	}

	// ROBALD - Driver can fail REF create on DX7/DX6 if POOL != SYSTEMMEM.
	if(Pool == FMWK_DEFAULT)
		Pool = (m_dwDevType == DEVICETYPE_REF || m_dwDevType == DEVICETYPE_SW) ? POOL_SYSTEMMEM : POOL_DEFAULT;

	// Convert the D3DPOOL
	switch(Pool)
	{
	case POOL_DEFAULT:
	case POOL_VIDEO:
		ddDesc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
		break;
	case POOL_AGP:
		ddDesc.ddsCaps.dwCaps |= (DDSCAPS_NONLOCALVIDMEM | DDSCAPS_VIDEOMEMORY);
		break;
	case POOL_MANAGED:
        ddDesc.ddsCaps.dwCaps2 |= DDSCAPS2_TEXTUREMANAGE;
		break;
    case POOL_SYSTEMMEM:
		ddDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
		break;
	default:
		DPF(1, _T("CDevice::CreateTexture7() - invalid POOL flag.\n"));
		return SetLastError(E_FAIL);
	}

	// Convert the Usage flags
	switch(Usage)
	{
	case USAGE_RENDERTARGET:
		ddDesc.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;
		break;
	case USAGE_DEPTHSTENCIL:
        ddDesc.ddsCaps.dwCaps |= DDSCAPS_ZBUFFER;
		break;
	default:
		// Eat all non Dx7 usage types
		break;
	}

	//
	// Create the target surface
	// 

	pTexture = new CTexture;
	if(pTexture == NULL)
	{
		DPF(1, _T("CDevice::CreateTexture() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	hr = m_pD3D->m_pDDraw7->CreateSurface( &ddDesc, &pD3DTexture, NULL);
	if(FAILED(hr))
	{
		DPF(2, _T("CDevice::CreateTexture7() - CreateSurface failed. hr=%s(%x)\n"),
				  m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	if(!pTexture->Init(pD3DTexture, this))
	{
		DPF(1, _T("CDevice::CreateTexture() - CTexture::Init() failed.\n") );
		hr = E_FAIL;
		goto Exit;
	}

    RELEASE(pD3DTexture);

    pTexOut = pTexture;

    //
    // Create the SYSMEM copy
	//
    if (Pool != POOL_MANAGED)
    {
        pTexture = new CTexture;
	    if(pTexture == NULL)
	    {
		    DPF(1, _T("CDevice::CreateTexture() - Out of memory.\n"));
		    hr = E_OUTOFMEMORY;
		    goto Exit;
	    }

	    // Set SYSMEM flags
        ddDesc.ddsCaps.dwCaps &= ~(DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM);
        ddDesc.ddsCaps.dwCaps2 &= ~(DDSCAPS2_TEXTUREMANAGE);
        ddDesc.ddsCaps.dwCaps &= ~(DDSCAPS_3DDEVICE | DDSCAPS_ZBUFFER);
	    ddDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	    hr = m_pD3D->m_pDDraw7->CreateSurface( &ddDesc, &pD3DTexture, NULL);
	    if(FAILED(hr))
	    {
		    DPF(1, _T("CDevice::CreateTexture7() - CreateSurface failed. hr=%s(%x)\n"),
				    m_pD3D->HResultToString(hr), hr);
		    goto Exit;
	    }

	    if(!pTexture->Init(pD3DTexture, this))
	    {
		    DPF(1, _T("CDevice::CreateTexture() - CTexture::Init() failed.\n") );
		    hr = E_FAIL;
		    goto Exit;
	    }

        RELEASE(pD3DTexture);

	    // Set the texture
	    pTexOut->SetSysTexture(pTexture);
    }
Exit:
    
	if(FAILED(hr))
	{
		RELEASE(pTexture);
        RELEASE(pD3DTexture);
        RELEASE(pTexOut);
	}
	else
		*ppTexture = pTexOut;

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateCubeTexture7(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,
									DWORD Pool,CCubeTexture** ppCubeTexture)
{
    HRESULT hr;
    CCubeTexture *pTexture = NULL, *pTexOut = NULL;
	LPDIRECTDRAWSURFACE7 pD3DTexture = NULL;
	DDSURFACEDESC2 ddDesc;
	
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::CreateCubeTexture7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    memset(&ddDesc, 0, sizeof(DDSURFACEDESC2));
    ddDesc.dwSize = sizeof(DDSURFACEDESC2);
	ddDesc.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_CAPS;
	ddDesc.dwHeight = EdgeLength;
	ddDesc.dwWidth = EdgeLength;
	ddDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_COMPLEX;
    ddDesc.ddsCaps.dwCaps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEX | DDSCAPS2_CUBEMAP_NEGATIVEY | DDSCAPS2_CUBEMAP_NEGATIVEZ;
	memcpy(&(ddDesc.ddpfPixelFormat), &(Format.ddpfPixelFormat), sizeof(DDPIXELFORMAT));

   	// Remove any private pixel format flags that were added.  We are also including
	// the PF_YUV flag as the YUV fourcc textures don't use it. 
	ddDesc.ddpfPixelFormat.dwFlags &= ~(PF_PRIVATE | PF_YUV);

	// Create the appropriate MipMap chain based on Levels requested
	if(Levels > 1)
	{
		ddDesc.dwFlags |= DDSD_MIPMAPCOUNT;
		ddDesc.dwMipMapCount = Levels;
	}

	if(Levels != 1)
	{
		ddDesc.ddsCaps.dwCaps |= DDSCAPS_MIPMAP;
	}

	// ROBALD - Driver can fail REF create on DX7/DX6 if POOL != SYSTEMMEM.
	if(Pool == FMWK_DEFAULT)
		Pool = (m_dwDevType == DEVICETYPE_REF || m_dwDevType == DEVICETYPE_SW) ? POOL_SYSTEMMEM : POOL_DEFAULT;

	// Convert the D3DPOOL
	switch(Pool)
	{
	case POOL_DEFAULT:
	case POOL_VIDEO:
		ddDesc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
		break;
	case POOL_AGP:
		ddDesc.ddsCaps.dwCaps |= (DDSCAPS_NONLOCALVIDMEM | DDSCAPS_VIDEOMEMORY);
		break;
	case POOL_MANAGED:
        ddDesc.ddsCaps.dwCaps2 |= DDSCAPS2_TEXTUREMANAGE;
		break;
    case POOL_SYSTEMMEM:
		ddDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
		break;
	default:
		DPF(1, _T("CDevice::CreateCubeTexture7() - invalid POOL flag.\n"));
		return SetLastError(E_FAIL);
	}

	// Convert the Usage flags
	switch(Usage)
	{
	case USAGE_RENDERTARGET:
		ddDesc.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;
		break;
	case USAGE_DEPTHSTENCIL:
        ddDesc.ddsCaps.dwCaps |= DDSCAPS_ZBUFFER;
		break;
	default:
		// Eat all non Dx7 usage types
		break;
	}

	//
	// Create the target surface
	// 

	pTexture = new CCubeTexture;
	if(pTexture == NULL)
	{
		DPF(1, _T("CDevice::CreateCubeTexture7() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	hr = m_pD3D->m_pDDraw7->CreateSurface( &ddDesc, &pD3DTexture, NULL);
	if( FAILED( hr ) )
	{
		DPF(2, _T("CDevice::CreateCubeTexture7() - CreateSurface failed. hr=%s(%x)\n"),
				  m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	if(!pTexture->Init(pD3DTexture, this))
	{
		DPF(1, _T("CDevice::CreateCubeTexture7() - CTexture::Init() failed.\n") );
		hr = E_FAIL;
		goto Exit;
	}

    RELEASE(pD3DTexture);

    pTexOut = pTexture;

    
	//
    // Create the SYSMEM copy
	//

    if (Pool != POOL_SCRATCH)
    {
        pTexture = new CCubeTexture;
	    if(pTexture == NULL)
	    {
		    DPF(1, _T("CDevice::CreateCubeTexture7() - Out of memory.\n"));
		    hr = E_OUTOFMEMORY;
		    goto Exit;
	    }

        ddDesc.ddsCaps.dwCaps &= ~(DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM);
	    ddDesc.ddsCaps.dwCaps2 &= ~(DDSCAPS2_TEXTUREMANAGE);
        ddDesc.ddsCaps.dwCaps &= ~(DDSCAPS_3DDEVICE | DDSCAPS_ZBUFFER);
	    ddDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	    hr = m_pD3D->m_pDDraw7->CreateSurface( &ddDesc, &pD3DTexture, NULL);
	    if( FAILED( hr ) )
	    {
		    DPF(1, _T("CDevice::CreateCubeTexture7() - CreateSurface failed. hr=%s(%x)\n"),
				    m_pD3D->HResultToString(hr), hr);
		    RELEASE( pTexture );
		    goto Exit;
	    }

	    if(!pTexture->Init(pD3DTexture, this))
	    {
		    DPF(1, _T("CDevice::CreateCubeTexture7() - CTexture::Init() failed.\n") );
            RELEASE( pTexture );
		    hr = E_FAIL;
		    goto Exit;
	    }

        RELEASE(pD3DTexture);

	    pTexOut->SetSysTexture(pTexture);
    
    }
Exit:

	if(FAILED(hr))
	{
		RELEASE(pTexture);
		RELEASE(pD3DTexture);
		RELEASE(pTexOut);
	}
	else
	{
		*ppCubeTexture = pTexOut;
	}

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateBackBuffers7(PRESENT_PARAMETERS *pParams)
{
	HRESULT hr;
    LPDIRECTDRAWSURFACE7 pSurf = NULL;
    int i;

	// Allocate the BackBuffer Ptrs
	m_pBackBuffers = new CSurface*[pParams->uBackBufferCount];
	if(m_pBackBuffers == NULL)
	{
		DPF(1, _T("CDevice::CreateBackBuffers7() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Error;
	}
	memset(m_pBackBuffers, 0, sizeof(CSurface*) * pParams->uBackBufferCount);

	// Create the actual buffers
	for(i=0; i<pParams->uBackBufferCount; i++)
	{
        m_pBackBuffers[i] = new CSurface();
		if(m_pBackBuffers[i] == NULL)
		{
			DPF(1, _T("CDevice::CreateBackBuffers7() - Out of memory.\n"));
			hr = E_OUTOFMEMORY;
			goto Error;
		}

		// If fullscreen, the backbuffer was created us for us and we need to get
		// a ptr to it.  Otherwise, we create backbuffers for windowed mode.

		if(!pParams->bWindowed)
		{
            DDSCAPS2 ddsCaps;
			memset(&ddsCaps, 0, sizeof(DDSCAPS2));
			ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

			hr = m_pPrimary->GetD3DSurface7()->GetAttachedSurface(&ddsCaps, &pSurf);
			if(FAILED(hr))
			{
				DPF(1, _T("CDevice::CreateBackBuffers7() - GetAttachedSurface failed. hr=%s(%x)\n"),
					m_pD3D->HResultToString(hr), hr);
				goto Error;
			}
		}
		else
		{
			DDSURFACEDESC2  ddDesc;
			ZeroMemory(&ddDesc,sizeof(DDSURFACEDESC2));
            ddDesc.dwSize  = sizeof(DDSURFACEDESC2);
            ddDesc.dwWidth = pParams->uWidth;
            ddDesc.dwHeight = pParams->uHeight;
			ddDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
			ddDesc.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN;            
			if(m_dwDevType & DEVICETYPE_HAL)
	            ddDesc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
			else
                ddDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

		    // Create the surface
			hr = m_pD3D->m_pDDraw7->CreateSurface(&ddDesc, &pSurf, NULL);
			if(FAILED(hr))
			{
				DPF(1, _T("CDevice::CreateBackBuffers7() - CreateSurface failed. hr=%s(%x).\n"), m_pD3D->HResultToString(hr), hr);
				goto Error;
			}

			//Set the real backbuffer format in the present params
			pSurf->GetSurfaceDesc(&ddDesc);
			memcpy(&(pParams->BackBufferFormat.ddpfPixelFormat), &(ddDesc.ddpfPixelFormat), sizeof(DDPIXELFORMAT));
			GetD3D()->DDPFToD3DFmt((LPDDPIXELFORMAT)&(pParams->BackBufferFormat.ddpfPixelFormat), (FMT*)&(pParams->BackBufferFormat.d3dfFormat), NULL);
		}

		if(!m_pBackBuffers[i]->Init(pSurf, this, NULL))
		{
			DPF(2, _T("CDevice::CreateBackBuffers7() - pBackBuffer->Init() failed.\n"));
			hr = E_FAIL;
			goto Error;
		}

        RELEASE(pSurf);

        m_nBackBuffers++;
	}

	return S_OK;

Error:
	
	RELEASE(pSurf);
	if (m_pBackBuffers != NULL)
	{
		for(i=0; i<pParams->uBackBufferCount; i++)
		{
			RELEASE(m_pBackBuffers[i]);
		}
		SAFEDELETEA(m_pBackBuffers);
	}

    return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateVertexBuffer7(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB)
{
	CVertexBuffer			*pVB = NULL;
    D3DVERTEXBUFFERDESC     Desc;
	LPDIRECT3DVERTEXBUFFER7	pD3DVB;
	HRESULT					hr;
	
    Desc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
    Desc.dwCaps = 0;
    Desc.dwFVF = FVF;
    Desc.dwNumVertices = Length / GetVertexSize(FVF);
	if (Pool == FMWK_DEFAULT)
	{
		if ( ((FVF & D3DFVF_POSITION_MASK) != D3DFVF_XYZRHW && 0 == (m_dwBehaviors & DEVICECREATE_HWVP)) || 
			m_dwDevType == DEVICETYPE_SW || m_dwDevType == DEVICETYPE_REF)
		{
			Desc.dwCaps |= D3DVBCAPS_SYSTEMMEMORY;
		}
	}
	if (Usage == USAGE_SOFTWAREPROCESSING || Pool == POOL_SYSTEMMEM || Pool == POOL_MANAGED)
		Desc.dwCaps |= D3DVBCAPS_SYSTEMMEMORY;
	if (Usage == USAGE_WRITEONLY)
		Desc.dwCaps |= D3DVBCAPS_WRITEONLY;
	if (Usage == USAGE_DONOTCLIP)
		Desc.dwCaps |= D3DVBCAPS_DONOTCLIP;

	pVB = new CVertexBuffer();
	if (NULL == pVB)
	{
		DPF(1, _T("CDevice::CreateVertexBuffer() - Out of memory.\n"));
		return E_FAIL;
	}

	hr = m_pD3D->m_pD3D7->CreateVertexBuffer(&Desc, &pD3DVB, 0);
	if ( FAILED(hr) )
	{
		DPF(1, _T("CDevice::CreateVertexBuffer() - CDevice::CreateVertexBuffer() failed.\n") );
		RELEASE( pVB );
		return SetLastError(hr);
	}

	if (pVB->Initialize(pD3DVB, this))
	{
		if (ppVB != NULL)
		{
			*ppVB = pVB;
			pVB->SetVertexFormat(FVF);
		}
	}
	else
	{
		DPF(1, _T("CDevice::CreateVertexBuffer() - CDevice::CreateVertexBuffer() failed.\n") );
		RELEASE( pD3DVB );
		RELEASE( pVB );
		return SetLastError(E_FAIL);
	}
	
	RELEASE( pD3DVB );

	return hr;
}

// --------------------------------------------------------------------------------------

void CDevice::ReleaseD3D7()
{
	if(m_pD3D && m_pD3D->m_pDDraw7 && m_bFullscreen)
	{
		m_pD3D->m_pDDraw7->SetCooperativeLevel(m_hFocusWindow, DDSCL_NORMAL);
        m_pD3D->m_pDDraw7->RestoreDisplayMode();
	}

	// Release ZBuffer before releasing render targets
	RELEASE(m_pZBuffer);

    // Release the backbuffers
    for(int i=0; i<m_nBackBuffers; i++)
	{
		RELEASE(m_pBackBuffers[i]);
	}
	SAFEDELETEA(m_pBackBuffers);
	m_nBackBuffers = 0;
    SAFEDELETE(m_pPresParams);

	RELEASE(m_pPrimary);

    if(m_pDevice7)
	{
		UINT uRef = m_pDevice7->Release();
		m_pDevice7 = NULL;
		if(uRef)
		{
			DPF(1, _T("CDevice::ReleaseD3D7() - m_pDevice7 ref count is %d after final Release().\n"), uRef);
            if(g_pFramework != NULL && g_pFramework->m_bStressMode)
			{
				ASSERT(uRef == 0);
			}
		}
	}

    // Reset the state
	m_bFullscreen = false;
}

// --------------------------------------------------------------------------------------

bool CDevice::TextureFormatSupported7(FORMAT *pFmt)
{
	if(pFmt == NULL)
	{
		DPF(1, _T("CDevice::TextureFormatSupported7() - Invalid params.\n"));
		return false;
	}

	FORMAT *pTempFmt = m_pTextureFormat;
	while(pTempFmt)
	{
		if(m_pD3D->CompareFormats(pTempFmt, pFmt))
            return true;
		pTempFmt = pTempFmt->pNext;
	}

   	return false;
}

// --------------------------------------------------------------------------------------

bool CDevice::ZBufferFormatSupported7(FORMAT *pFmt)
{
	if(pFmt == NULL)
	{
		DPF(1, _T("CDevice::ZBufferFormatSupported7() - Invalid params.\n"));
		return false;
	}

	FORMAT *pTempFmt = m_pZBufferFormat;
	while(pTempFmt)
	{
		if(m_pD3D->CompareFormats(pTempFmt, pFmt))
            return true;
		pTempFmt = pTempFmt->pNext;
	}

   	return false;
}

// --------------------------------------------------------------------------------------

bool CDevice::CubeFormatSupported7(FORMAT *pFmt)
{
	if(pFmt == NULL)
	{
		DPF(1, _T("CDevice::CubeFormatSupported7() - Invalid params.\n"));
		return false;
	}

	FORMAT *pTempFmt = m_pCubeFormat;
	while(pTempFmt)
	{
		if(m_pD3D->CompareFormats(pTempFmt, pFmt))
            return true;
		pTempFmt = pTempFmt->pNext;
	}

   	return false;
}

// --------------------------------------------------------------------------------------

bool CDevice::VolumeFormatSupported7(FORMAT *pFmt)
{
	if(pFmt == NULL)
	{
		DPF(1, _T("CDevice::VolumeFormatSupported7() - Invalid params.\n"));
		return false;
	}

	FORMAT *pTempFmt = m_pVolumeFormat;
	while(pTempFmt)
	{
		if(m_pD3D->CompareFormats(pTempFmt, pFmt))
            return true;
		pTempFmt = pTempFmt->pNext;
	}

   	return false;
}

// --------------------------------------------------------------------------------------

bool CDevice::Draw2DText7(DWORD dwColor, TCHAR *szString, int nLen, RECT* pRect, DWORD dwFlags)
{
	HDC hDC = 0;
	HGDIOBJ hGDIObj = NULL;
    HRESULT hr;
	UINT nRed = (dwColor & 0x00FF0000) >> 16;
    UINT nGreen = (dwColor & 0x0000FF00) >> 8;
    UINT nBlue = (dwColor & 0x000000FF);

	if(szString == NULL || pRect == NULL)
	{
	    DPF(1, _T("CDevice::Draw2DText7 - Invalid params.\n"));
		return false;
	}

	UINT nX = (dwFlags & DT_CENTER) ? (pRect->right - pRect->left) / 2 : pRect->left;
    	
    hr = m_pBackBuffers[0]->GetD3DSurface7()->GetDC(&hDC);
    if(FAILED(hr))
	{
		DPF(1, _T("CDevice::Draw2DText7 - GetDC failed. hr=%s(%x)\n"),
			m_pD3D->HResultToString(hr), hr);
		return false;
	}

	HFONT hFont = ::CreateFontIndirect( & m_LogFont );
	hGDIObj = SelectObject(hDC, hFont);
	if(dwFlags & DT_CENTER)
		SetTextAlign(hDC, TA_CENTER);
	else
		SetTextAlign(hDC, TA_LEFT);
    SetBkMode(hDC,TRANSPARENT);
    SetTextColor(hDC,RGB(nRed, nGreen, nBlue));
#pragma prefast(suppress: 528, "SHExtTextOut does not appear in publics.")
	ExtTextOut(hDC, nX, pRect->top, 0, pRect, szString, _tcslen(szString), NULL);
	//Make sure we deselect the font so that it won't leak resources on win9x
	SelectObject(hDC, hGDIObj);
	DeleteObject( hFont );
	
	if(hDC)
	{
		hr = m_pBackBuffers[0]->GetD3DSurface7()->ReleaseDC(hDC);
		if(FAILED(hr))
		{
			DPF(1, _T("CDevice::Draw2DText7 - ReleaseDC failed. hr=%s(%x)\n"),
				m_pD3D->HResultToString(hr), hr);
			return false;
		}
	}

	return true;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::QueryInterface7(REFIID riid, LPVOID * ppvObj)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::QueryInterface7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->QueryInterface(riid,ppvObj));
}

// --------------------------------------------------------------------------------------

bool CDevice::IsDeviceEqual7(PVOID pDevice)
{
	if(m_pDevice7 == (LPDIRECT3DDEVICE7) pDevice)
		return true;
	else
		return false;
}

// --------------------------------------------------------------------------------------

ULONG CDevice::AddRef7(void)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::AddRef7() - Invalid Device.\n"));
		return (ULONG)E_FAIL;
	}

    return m_pDevice7->AddRef( );
}

// --------------------------------------------------------------------------------------

ULONG CDevice::Release7(void)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::Release7() - Invalid Device.\n"));
		return (ULONG)E_FAIL;
	}

    return m_pDevice7->Release( );
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetVerticalBlankStatus7(LPBOOL lpbIsInVB)
{
    if (!m_pD3D || !m_pD3D->m_pDDraw7)
    {
        DPF(1, _T("CDevice::GetVerticalBlankStatus7() - Invalid Device.\n"));
        return SetLastError(E_FAIL);
    }

    return SetLastError(m_pD3D->m_pDDraw7->GetVerticalBlankStatus(lpbIsInVB));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetCaps7(LPD3DDEVICEDESC7 lpD3DDevDesc)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::GetCaps7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->GetCaps(lpD3DDevDesc));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::EnumTextureFormats7(LPD3DENUMPIXELFORMATSCALLBACK lpd3dEnumPixelProc, LPVOID lpArg)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::EnumTextureFormats7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->EnumTextureFormats(lpd3dEnumPixelProc,lpArg));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::BeginScene7(void)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::BeginScene7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->BeginScene( ));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::EndScene7(void)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::EndScene7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->EndScene( ));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetDirect3D7(LPDIRECT3D7* lplpD3D)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::GetDirect3D7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->GetDirect3D(lplpD3D));
}

// ------------------------------------------------------------------------------

HRESULT CDevice::SetRenderTarget7(CSurface *pNew, CSurface *pNewDepth)
{
    HRESULT hr = S_OK;

    if ((pNew != NULL) && (pNewDepth != NULL))
    {
        DDSCAPS2 ddsCaps;
        LPDIRECTDRAWSURFACE7 pSurf = NULL;
		memset(&ddsCaps, 0, sizeof(DDSCAPS2));
		ddsCaps.dwCaps = DDSCAPS_ZBUFFER;

        // Check if surface already has a ZBuffer
		hr = pNew->GetD3DSurface7()->GetAttachedSurface(&ddsCaps, &pSurf);
        RELEASE(pSurf);

        if (FAILED(hr))
		{
            // Attach new ZBuffer 
            hr = pNew->AddAttachedSurface(pNewDepth);			

            if (FAILED(hr))
	        {
		        DPF(1, _T("CDevice::SetRenderTarget7() - AddAttatchedSurface() failed.\n"));
		        return SetLastError(hr);
	        }
        }
    }


    return SetLastError(m_pDevice7->SetRenderTarget(pNew == NULL ? NULL : pNew->GetD3DSurface7(),0));
}

// ------------------------------------------------------------------------------

HRESULT CDevice::GetRenderTarget7(CSurface** ppRenderTarget)
{
	LPDIRECTDRAWSURFACE7 pD3DSurface;
	CSurface* pSurface = new CSurface;

    if (pSurface == NULL)
	{
		DPF(1, _T("CDevice::GetRenderTarget7() - Out of Memory.\n"));
		return SetLastError(E_FAIL);
	}

	HRESULT hr = m_pDevice7->GetRenderTarget( &pD3DSurface );
	if( FAILED( hr ) )
	{
		DPF(1, _T("CDevice::GetRenderTarget7() - GetRenderTarget() failed.\n"));
		RELEASE( pSurface );
		return SetLastError(hr);
	}

	if( !pSurface->Init( pD3DSurface, this, NULL ) )
	{
		DPF(1, _T("CDevice::GetRenderTarget7() - CSurface::Init() failed.\n"));
		RELEASE( pSurface );
		RELEASE( pD3DSurface );
		return SetLastError(E_FAIL);
	}

	RELEASE(pD3DSurface);

	if( ppRenderTarget != NULL )
		*ppRenderTarget = pSurface;

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::Clear7(DWORD dwCount,LPD3DRECT lpRects,DWORD dwFlags,D3DCOLOR dwColor,
						D3DVALUE dvZ,DWORD dwStencil)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::Clear7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    if(dwFlags == FMWK_DEFAULT)
	{
		dwFlags = 0;
        dwFlags |= CLEARF_TARGET;
		dwFlags |= m_pPresParams->bAutoDepthStencil ? CLEARF_ZBUFFER : 0;
	}

    return SetLastError(m_pDevice7->Clear(dwCount,lpRects,dwFlags,dwColor,dvZ,dwStencil));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetTransform7(D3DTRANSFORMSTATETYPE State,LPD3DMATRIX lpD3DMatrix)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::SetTransform7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}
    	
	// Remap DX8 transform types
	switch (State)
	{
	case 256:     // WORLD
		State = D3DTRANSFORMSTATE_WORLD;
		break;
	case 257:     // WORLD1
		State = D3DTRANSFORMSTATE_WORLD1;
		break;
	case 258:     // WORLD2
		State = D3DTRANSFORMSTATE_WORLD2;
		break;
	case 259:     // WORLD3
		State = D3DTRANSFORMSTATE_WORLD3;
		break;
	}

    return SetLastError(m_pDevice7->SetTransform(State,lpD3DMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetTransform7(D3DTRANSFORMSTATETYPE dtstTransformStateType,LPD3DMATRIX lpD3DMatrix)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::GetTransform7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->GetTransform(dtstTransformStateType,lpD3DMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetViewport7(LPVIEWPORT pViewport)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::SetViewport7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

	// convert from private struct
	D3DVIEWPORT7    Viewport;

	Viewport.dwX            = pViewport->X;
	Viewport.dwY            = pViewport->Y;
	Viewport.dwWidth        = pViewport->Width;
	Viewport.dwHeight       = pViewport->Height;
	Viewport.dvMinZ         = pViewport->MinZ;
	Viewport.dvMaxZ         = pViewport->MaxZ;

   return SetLastError(m_pDevice7->SetViewport(&Viewport));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetViewport7(LPVIEWPORT pViewport)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::GetViewport7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

	D3DVIEWPORT7    Viewport;
	ZeroMemory( &Viewport, sizeof(Viewport) );

	// Call GetViewport even if pViewport == NULL (for invalid param testing)
	D3DVIEWPORT7*	pGetViewportParam;
	if(NULL != pViewport)
	{
		pGetViewportParam = &Viewport;
	}
	else
	{
		pGetViewportParam = NULL;// Call GetViewport even if pViewport == NULL (for invalid param testing)
	}

	HRESULT			hr = SetLastError(m_pDevice7->GetViewport(pGetViewportParam));

	if( pViewport == NULL )
		return hr;

	// convert to private struct
	pViewport->X		= Viewport.dwX;
	pViewport->Y		= Viewport.dwY;
	pViewport->Width	= Viewport.dwWidth;
	pViewport->Height	= Viewport.dwHeight;
	pViewport->MinZ		= Viewport.dvMinZ;
	pViewport->MaxZ		= Viewport.dvMaxZ;

   return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::MultiplyTransform7(D3DTRANSFORMSTATETYPE dtstTransformStateType,LPD3DMATRIX lpD3DMatrix)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::MultiplyTransform7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->MultiplyTransform(dtstTransformStateType,lpD3DMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetMaterial7(LPMATERIAL lpMaterial)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::SetMaterial7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->SetMaterial((LPD3DMATERIAL7)lpMaterial));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetMaterial7(LPMATERIAL lpMaterial)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::GetMaterial7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->GetMaterial((LPD3DMATERIAL7)lpMaterial));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetLight7(DWORD dwLightIndex,LPLIGHT lpLight)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::SetLight7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->SetLight(dwLightIndex,(LPD3DLIGHT7)lpLight));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetLight7(DWORD dwLightIndex,LPLIGHT lpLight)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::GetLight7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->GetLight(dwLightIndex,(LPD3DLIGHT7)lpLight));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetRenderState7(D3DRENDERSTATETYPE State,DWORD Value)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::SetRenderState7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    switch(State)
    {
        // Eat these
//          case    D3DRENDERSTATE_TEXTUREPERSPECTIVE:  // MPL: Has been "un-retired"
        case    D3DRENDERSTATE_ANTIALIAS:
                return D3D_OK;
        case    D3DRENDERSTATE_TEXTUREADDRESS:
                return SetTextureStageState(0,D3DTSS_ADDRESS,Value);
        case    D3DRENDERSTATE_WRAPU:
                {
                    DWORD   dwState;
                    
                    GetRenderState(D3DRENDERSTATE_WRAP0,&dwState);

                    dwState &= ~D3DWRAP_U;

                    if (TRUE == Value)
                        dwState |= D3DWRAP_U;

                    return SetRenderState(D3DRENDERSTATE_WRAP0,dwState);
                }
        case    D3DRENDERSTATE_WRAPV:
                {
                    DWORD   dwState;
                    
                    GetRenderState(D3DRENDERSTATE_WRAP0,&dwState);

                    dwState &= ~D3DWRAP_V;

                    if (TRUE == Value)
                        dwState |= D3DWRAP_V;

                    return SetRenderState(D3DRENDERSTATE_WRAP0,dwState);
                }
        case    D3DRENDERSTATE_TEXTUREMAG:
                switch (Value)
                {
                    case    D3DFILTER_NEAREST:
                            return SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
                    case    D3DFILTER_LINEAR:
                            return SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
                }
                return E_FAIL;
        case    D3DRENDERSTATE_TEXTUREMIN:
                switch(Value)
                {
                    case    D3DFILTER_NEAREST:
                            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
                            return SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
                    case    D3DFILTER_LINEAR:
                            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
                            return SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
                    case    D3DFILTER_MIPLINEAR:
                            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
                            return SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
                    case    D3DFILTER_MIPNEAREST:
                            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
                            return SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
                    case    D3DFILTER_LINEARMIPNEAREST:
                            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);
                            return SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
                    case    D3DFILTER_LINEARMIPLINEAR:
                            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);
                            return SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
                }
                return E_FAIL;

        case    D3DRENDERSTATE_TEXTUREMAPBLEND:
                {
                    HRESULT hrResult;
                    DWORD   dwValid;

                    switch(Value)
                    {
                        case    D3DTBLEND_COPY:
                        case    D3DTBLEND_DECAL:
                                {
                                    SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLOROP,(DWORD)D3DTOP_SELECTARG1);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_ALPHAOP,(DWORD)D3DTOP_SELECTARG1);

                                    hrResult = ValidateDevice(&dwValid);

                                    if (D3D_OK == hrResult)
                                        return D3D_OK;

                                    SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLOROP,(DWORD)D3DTOP_SELECTARG2);

                                    hrResult = ValidateDevice(&dwValid);

                                    return hrResult;
                                }
                                break;
                        case    D3DTBLEND_MODULATE:
                                {
                                    SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_COLOROP,(DWORD)D3DTOP_MODULATE);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_ALPHAOP,(DWORD)D3DTOP_SELECTARG1);

                                    hrResult = ValidateDevice(&dwValid);

                                    if (D3D_OK == hrResult)
                                        return D3D_OK;

                                    SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_COLOROP,(DWORD)D3DTOP_MODULATE);

                                    hrResult = ValidateDevice(&dwValid);

                                    return hrResult;
                                }
                                break;
                        case    D3DTBLEND_DECALALPHA:
                                {
                                    SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_COLOROP,(DWORD)D3DTOP_BLENDTEXTUREALPHA);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_ALPHAOP,(DWORD)D3DTOP_SELECTARG2);

                                    hrResult = ValidateDevice(&dwValid);

                                    return hrResult;
                                }
                                break;
                        case    D3DTBLEND_MODULATEALPHA:
                                {
                                    SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_COLOROP,(DWORD)D3DTOP_MODULATE);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_ALPHAOP,(DWORD)D3DTOP_MODULATE);

                                    hrResult = ValidateDevice(&dwValid);

                                    if (D3D_OK == hrResult)
                                        return D3D_OK;

                                    SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_COLOROP,(DWORD)D3DTOP_MODULATE);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_ALPHAOP,(DWORD)D3DTOP_MODULATE);

                                    hrResult = ValidateDevice(&dwValid);

                                    return hrResult;
                                }
                                break;
                        case    D3DTBLEND_ADD:
                                {
                                    SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_COLOROP,(DWORD)D3DTOP_ADD);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_ALPHAOP,(DWORD)D3DTOP_SELECTARG2);

                                    hrResult = ValidateDevice(&dwValid);

                                    if (D3D_OK == hrResult)
                                        return D3D_OK;

                                    SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_COLOROP,(DWORD)D3DTOP_ADD);

                                    hrResult = ValidateDevice(&dwValid);

                                    return hrResult;
                                }
                                break;
                    }
                }
                break;
        case    D3DRENDERSTATE_TEXTUREADDRESSU:
                return SetTextureStageState(0,D3DTSS_ADDRESSU,Value);
        case    D3DRENDERSTATE_TEXTUREADDRESSV:
                return SetTextureStageState(0,D3DTSS_ADDRESSV,Value);
        case    D3DRENDERSTATE_MIPMAPLODBIAS:
                return SetTextureStageState(0,D3DTSS_MIPMAPLODBIAS,Value);
        case    D3DRENDERSTATE_ANISOTROPY:
                return SetTextureStageState(0,D3DTSS_MAXANISOTROPY,Value);
        case    D3DRENDERSTATE_BORDERCOLOR:
                return SetTextureStageState(0,D3DTSS_BORDERCOLOR,Value);
        case    D3DRENDERSTATE_LIGHTING:
            m_bRPLighting = (Value == TRUE) ? true : false;
            break;
        case    D3DRENDERSTATE_CLIPPING:
            m_bRPClipping = (Value == TRUE) ? true : false;
            break;
    }

    return SetLastError(m_pDevice7->SetRenderState(State,Value));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetRenderState7(D3DRENDERSTATETYPE dwRenderStateType,LPDWORD lpdwRenderState)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::GetRenderState7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->GetRenderState(dwRenderStateType,lpdwRenderState));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::BeginStateBlock7(void)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::BeginStateBlock7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->BeginStateBlock( ));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::EndStateBlock7(UINT_PTR* lpdwBlockHandle)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::EndStateBlock7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->EndStateBlock((DWORD*)lpdwBlockHandle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::PreLoad7(LPDIRECTDRAWSURFACE7 lpddsTexture)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::PreLoad7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->PreLoad(lpddsTexture));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawPrimitive7(D3DPRIMITIVETYPE dptPrimitiveType,
								DWORD dwVertexTypeDesc,LPVOID lpvVertices,
								DWORD dwVertexCount,DWORD dwFlags)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::DrawPrimitive7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->DrawPrimitive(dptPrimitiveType,dwVertexTypeDesc,lpvVertices,dwVertexCount,dwFlags));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawIndexedPrimitive7(D3DPRIMITIVETYPE d3dptPrimitiveType,DWORD dwVertexTypeDesc,
									   LPVOID lpvVertices,DWORD dwVertexCount,LPWORD lpwIndices,
									   DWORD dwIndexCount,DWORD dwFlags)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::DrawIndexedPrimitive7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->DrawIndexedPrimitive(d3dptPrimitiveType,
											dwVertexTypeDesc,
											lpvVertices,
											dwVertexCount,
											lpwIndices,
											dwIndexCount,
											dwFlags));
}

// --------------------------------------------------------------------------------------


bool CDevice::DrawPrimitiveUserMemory7(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc,
				LPVOID lpVertices, DWORD  dwVertexCount, LPVOID lpIndices, DWORD  dwIndexCount, 
				DWORD  dwFlags, bool bIs32BitIndices)
{
    DWORD   dwWho = dwFlags & (CD3D_SRC | CD3D_REF);
    DWORD   dwProcessedFlags = 0;
    bool    fResult = true;


    // DX7 DrawPrimitive
    if (NULL == lpIndices)
    {
//      dwProcessedFlags = ProcessDrawPrimitiveFlags(dwFlags, false);
        if (FAILED(DrawPrimitive7(d3dptPrimitiveType, dwVertexTypeDesc, lpVertices, dwVertexCount, dwProcessedFlags)))
			fResult = false;
//      RestoreDrawPrimitiveFlags();
    }
    // DX7 DrawIndexedPrimitive
    else
    {
//      dwProcessedFlags = ProcessDrawPrimitiveFlags(dwFlags, false);
        if (FAILED(DrawIndexedPrimitive7(d3dptPrimitiveType, dwVertexTypeDesc, lpVertices, dwVertexCount, (LPWORD)lpIndices, dwIndexCount, dwProcessedFlags)))
			fResult = false;
//      RestoreDrawPrimitiveFlags();
    }
    return fResult;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawPrimitiveVB7(D3DPRIMITIVETYPE d3dptPrimitiveType,LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer,
								  DWORD dwStartVertex,DWORD dwNumVertices,DWORD dwFlags)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::DrawPrimitiveVB7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->DrawPrimitiveVB(d3dptPrimitiveType,lpd3dVertexBuffer,dwStartVertex,dwNumVertices,dwFlags));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawIndexedPrimitiveVB7(D3DPRIMITIVETYPE d3dptPrimitiveType,
										 LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer,DWORD dwStartVertex,
										 DWORD dwNumVertices,LPWORD lpwIndices,DWORD dwIndexCount,DWORD dwFlags)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::DrawIndexedPrimitiveVB7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->DrawIndexedPrimitiveVB(	d3dptPrimitiveType,
												lpd3dVertexBuffer,
												dwStartVertex,
												dwNumVertices,
												lpwIndices,
												dwIndexCount,
												dwFlags));
}

// --------------------------------------------------------------------------------------

bool CDevice::DrawPrimitiveVertexBuffer7(D3DPRIMITIVETYPE d3dptPrimitiveType, CVertexBuffer *pBuffer, 
							DWORD dwStartVertex, DWORD dwNumVertices, LPWORD lpwIndices, DWORD dwIndexCount, 
							DWORD dwFlags, DWORD dwUsage, DWORD dwPool, bool bIs32BitIndices)
{
	DWORD   dwProcessedFlags = dwFlags;
	bool    fResult = true;

	// DX7 DrawPrimitive
	if (NULL == lpwIndices)
    {
//		dwProcessedFlags = ProcessDrawPrimitiveFlags(dwFlags, false);
        if (FAILED(DrawPrimitiveVB7(d3dptPrimitiveType,pBuffer->GetVertexBufferPointer7(),dwStartVertex,dwNumVertices,dwProcessedFlags)))
			fResult = false;
//		RestoreDrawPrimitiveFlags();
    }
    // DX7 DrawIndexedPrimitive
    else
    {
        // Source device
//		dwProcessedFlags = ProcessDrawPrimitiveFlags(dwFlags, false);
		if (FAILED(DrawIndexedPrimitiveVB7(d3dptPrimitiveType, pBuffer->GetVertexBufferPointer7(),dwStartVertex,dwNumVertices,lpwIndices, dwIndexCount, dwProcessedFlags)))
			fResult = false;
//		RestoreDrawPrimitiveFlags();
    }
    return fResult;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawPrimitiveStrided7(D3DPRIMITIVETYPE dptPrimitiveType,DWORD dwVertexTypeDesc,
									   LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray,DWORD dwVertexCount,
									   DWORD dwFlags)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::DrawPrimitiveStrided7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->DrawPrimitiveStrided(dptPrimitiveType,
											dwVertexTypeDesc,
											lpVertexArray,
											dwVertexCount,
											dwFlags));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawIndexedPrimitiveStrided7(D3DPRIMITIVETYPE dptPrimitiveType,DWORD dwVertexTypeDesc,
											  LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray,DWORD dwVertexCount,
											  LPWORD lpwIndices,DWORD dwIndexCount,DWORD dwFlags)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::DrawIndexedPrimitiveStrided7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->DrawIndexedPrimitiveStrided(	dptPrimitiveType,
													dwVertexTypeDesc,
													lpVertexArray,
													dwVertexCount,
													lpwIndices,
													dwIndexCount,
													dwFlags));
}

// --------------------------------------------------------------------------------------

	
	
bool CDevice::DrawPrimitiveStridedData7(D3DPRIMITIVETYPE d3dptPrimitiveType,
							DWORD  dwVertexTypeDesc, LPDRAWPRIMITIVESTRIDEDDATA lpVertexArray,
							DWORD  dwVertexCount, LPWORD lpwIndices, DWORD  dwIndexCount,
							DWORD  dwFlags, DWORD  dwUsage, DWORD  dwPool, bool bIs32BitIndices)
{
	DWORD   dwProcessedFlags = dwFlags;
	bool    fResult = true;

	// DX7 DrawPrimitive
	if (NULL == lpwIndices)
    {
//		dwProcessedFlags = ProcessDrawPrimitiveFlags(dwFlags, false);
        if (FAILED(DrawPrimitiveStrided7(d3dptPrimitiveType,dwVertexTypeDesc,(LPD3DDRAWPRIMITIVESTRIDEDDATA)lpVertexArray,dwVertexCount,dwProcessedFlags)))
			fResult = false;
//		RestoreDrawPrimitiveFlags();
    }
    // DX7 DrawIndexedPrimitive
    else
    {
        // Source device
//		dwProcessedFlags = ProcessDrawPrimitiveFlags(dwFlags, false);
		if (FAILED(DrawIndexedPrimitiveStrided7(d3dptPrimitiveType,dwVertexTypeDesc,(LPD3DDRAWPRIMITIVESTRIDEDDATA)lpVertexArray,dwVertexCount,lpwIndices, dwIndexCount, dwProcessedFlags)))
			fResult = false;
//		RestoreDrawPrimitiveFlags();
    }
    return fResult;
}

// --------------------------------------------------------------------------------------





HRESULT CDevice::SetClipStatus7(LPCLIPSTATUS pClipStatus)
{
    D3DCLIPSTATUS   ClipStatus;

    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::SetClipStatus7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    if (pClipStatus == NULL)
	{
		DPF(1, _T("CDevice::SetClipStatus7() - Invalid Params.\n"));
		return SetLastError(E_FAIL);
	}

    ZeroMemory(&ClipStatus, sizeof(ClipStatus));
    ClipStatus.dwFlags = D3DCLIPSTATUS_STATUS | D3DCLIPSTATUS_EXTENTS2;

    if (pClipStatus->dwClipIntersection & CLIPSTATUS_BACK)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPINTERSECTIONBACK;
    if (pClipStatus->dwClipIntersection & CLIPSTATUS_BOTTOM)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPINTERSECTIONBOTTOM;
    if (pClipStatus->dwClipIntersection & CLIPSTATUS_FRONT)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPINTERSECTIONFRONT;
    if (pClipStatus->dwClipIntersection & CLIPSTATUS_LEFT)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPINTERSECTIONLEFT;
    if (pClipStatus->dwClipIntersection & CLIPSTATUS_RIGHT)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPINTERSECTIONRIGHT;
    if (pClipStatus->dwClipIntersection & CLIPSTATUS_TOP)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPINTERSECTIONTOP;

    if (pClipStatus->dwClipIntersection & CLIPSTATUS_PLANE0)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPINTERSECTIONGEN0;
    if (pClipStatus->dwClipIntersection & CLIPSTATUS_PLANE1)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPINTERSECTIONGEN1;
    if (pClipStatus->dwClipIntersection & CLIPSTATUS_PLANE2)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPINTERSECTIONGEN2;
    if (pClipStatus->dwClipIntersection & CLIPSTATUS_PLANE3)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPINTERSECTIONGEN3;
    if (pClipStatus->dwClipIntersection & CLIPSTATUS_PLANE4)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPINTERSECTIONGEN4;
    if (pClipStatus->dwClipIntersection & CLIPSTATUS_PLANE5)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPINTERSECTIONGEN5;

    if (pClipStatus->dwClipUnion & CLIPSTATUS_BACK)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPUNIONBACK;
    if (pClipStatus->dwClipUnion & CLIPSTATUS_BOTTOM)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPUNIONBOTTOM;
    if (pClipStatus->dwClipUnion & CLIPSTATUS_FRONT)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPUNIONFRONT;
    if (pClipStatus->dwClipUnion & CLIPSTATUS_LEFT)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPUNIONLEFT;
    if (pClipStatus->dwClipUnion & CLIPSTATUS_RIGHT)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPUNIONRIGHT;
    if (pClipStatus->dwClipUnion & CLIPSTATUS_TOP)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPUNIONTOP;

    if (pClipStatus->dwClipUnion & CLIPSTATUS_PLANE0)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPUNIONGEN0;
    if (pClipStatus->dwClipUnion & CLIPSTATUS_PLANE1)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPUNIONGEN1;
    if (pClipStatus->dwClipUnion & CLIPSTATUS_PLANE2)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPUNIONGEN2;
    if (pClipStatus->dwClipUnion & CLIPSTATUS_PLANE3)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPUNIONGEN3;
    if (pClipStatus->dwClipUnion & CLIPSTATUS_PLANE4)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPUNIONGEN4;
    if (pClipStatus->dwClipUnion & CLIPSTATUS_PLANE5)
        ClipStatus.dwStatus |= D3DSTATUS_CLIPUNIONGEN5;

	ClipStatus.minx = pClipStatus->minx;
	ClipStatus.maxx = pClipStatus->maxx;
	ClipStatus.miny = pClipStatus->miny;
	ClipStatus.maxy = pClipStatus->maxy;
	ClipStatus.minz = pClipStatus->minz;
	ClipStatus.maxz = pClipStatus->maxz;

    return SetLastError(m_pDevice7->SetClipStatus(&ClipStatus));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetClipStatus7(LPCLIPSTATUS pClipStatus)
{
	D3DCLIPSTATUS   ClipStatus;
    HRESULT			hr;

    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::GetClipStatus7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    if (pClipStatus == NULL)
	{
		DPF(1, _T("CDevice::GetClipStatus7() - Invalid Params.\n"));
		return SetLastError(E_FAIL);
	}

    hr = m_pDevice7->GetClipStatus(&ClipStatus);

    if (SUCCEEDED(hr))
    {
        pClipStatus->dwClipIntersection = 0;
        pClipStatus->dwClipUnion = 0;

        if (ClipStatus.dwStatus & D3DSTATUS_CLIPINTERSECTIONBACK)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_BACK;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPINTERSECTIONBOTTOM)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_BOTTOM;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPINTERSECTIONFRONT)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_FRONT;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPINTERSECTIONLEFT)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_LEFT;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPINTERSECTIONRIGHT)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_RIGHT;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPINTERSECTIONTOP)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_TOP;

        if (ClipStatus.dwStatus & D3DSTATUS_CLIPINTERSECTIONGEN0)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_PLANE0;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPINTERSECTIONGEN1)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_PLANE1;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPINTERSECTIONGEN2)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_PLANE2;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPINTERSECTIONGEN3)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_PLANE3;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPINTERSECTIONGEN4)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_PLANE4;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPINTERSECTIONGEN5)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_PLANE5;

        if (ClipStatus.dwStatus & D3DSTATUS_CLIPUNIONBACK)
            pClipStatus->dwClipUnion |= CLIPSTATUS_BACK;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPUNIONBOTTOM)
            pClipStatus->dwClipUnion |= CLIPSTATUS_BOTTOM;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPUNIONFRONT)
            pClipStatus->dwClipUnion |= CLIPSTATUS_FRONT;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPUNIONLEFT)
            pClipStatus->dwClipUnion |= CLIPSTATUS_LEFT;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPUNIONRIGHT)
            pClipStatus->dwClipUnion |= CLIPSTATUS_RIGHT;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPUNIONTOP)
            pClipStatus->dwClipUnion |= CLIPSTATUS_TOP;

        if (ClipStatus.dwStatus & D3DSTATUS_CLIPUNIONGEN0)
            pClipStatus->dwClipUnion |= CLIPSTATUS_PLANE0;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPUNIONGEN1)
            pClipStatus->dwClipUnion |= CLIPSTATUS_PLANE1;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPUNIONGEN2)
            pClipStatus->dwClipUnion |= CLIPSTATUS_PLANE2;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPUNIONGEN3)
            pClipStatus->dwClipUnion |= CLIPSTATUS_PLANE3;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPUNIONGEN4)
            pClipStatus->dwClipUnion |= CLIPSTATUS_PLANE4;
        if (ClipStatus.dwStatus & D3DSTATUS_CLIPUNIONGEN5)
            pClipStatus->dwClipUnion |= CLIPSTATUS_PLANE5;

		pClipStatus->minx = ClipStatus.minx;
		pClipStatus->miny = ClipStatus.miny;
		pClipStatus->minz = ClipStatus.minz;
		pClipStatus->maxx = ClipStatus.maxx;
		pClipStatus->maxy = ClipStatus.maxy;
		pClipStatus->maxz = ClipStatus.maxz;
    }

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::ComputeSphereVisibility7(LPD3DVECTOR lpCenters,LPD3DVALUE lpRadii,DWORD dwNumSpheres,
										  DWORD dwFlags,LPDWORD lpdwReturnValues)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::ComputeSphereVisibility7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->ComputeSphereVisibility(lpCenters,lpRadii,dwNumSpheres,dwFlags,lpdwReturnValues));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetTexture7(int nIndex, CBaseTexture *pTexture)
{	
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::SetTexture7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

	return SetLastError(m_pDevice7->SetTexture(nIndex, pTexture != NULL ? pTexture->GetD3DTexture7() : NULL));
}

// --------------------------------------------------------------------------------------

// Note: GetTexure7 is not valid for invalid param testing where ppTexture == NULL
HRESULT CDevice::GetTexture7(int nIndex, CBaseTexture **ppTexture)
{
    CTexture* pTexture;
	LPDIRECTDRAWSURFACE7 pD3DTexture;
	
	// Alloc the texture wrapper
	pTexture = new CTexture;
	if(pTexture == NULL)
	{
		DPF(1, _T("CDevice::GetTexture7() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}

	pD3DTexture = NULL;

	HRESULT hr = m_pDevice7->GetTexture((DWORD)nIndex,&pD3DTexture);
	
	if( FAILED( hr ) )
	{
		DPF(1, _T("CDevice::GetTexture7() - GetTexture() failed.\n"));
		RELEASE( pTexture );
		return SetLastError(hr);
	}
	
	if (pTexture->Init(pD3DTexture, this ))
	{
		// we succeeded!
		if( ppTexture )
			*ppTexture = (CBaseTexture*)pTexture;
	}
	else
	{
		DPF(1, _T("CDevice::GetTexture7() - CTexture::Init failed.\n") );
		RELEASE( pD3DTexture );
		RELEASE( pTexture );
		hr = E_FAIL;
	}

    return SetLastError(hr);
}
	
// --------------------------------------------------------------------------------------

HRESULT CDevice::GetTextureStageState7(DWORD dwStage,D3DTEXTURESTAGESTATETYPE dwState,LPDWORD lpdwValue)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::GetTextureStageState7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->GetTextureStageState(dwStage,dwState,lpdwValue));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetTextureStageState7(DWORD dwStage,D3DTEXTURESTAGESTATETYPE dwState,DWORD dwValue)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::SetTextureStageState7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}
   if (dwState == D3DTSS_MAGFILTER)
	{
		switch (dwValue)
		{
			case TEXF_POINT: 
				dwValue = D3DTFG_POINT;
				break;
			case TEXF_LINEAR:
				dwValue = D3DTFG_LINEAR;
				break;
			case TEXF_ANISOTROPIC: 
				dwValue = D3DTFG_ANISOTROPIC;
				break;
			case TEXF_FLATCUBIC:
				dwValue = D3DTFG_FLATCUBIC;
				break;
			case TEXF_GAUSSIANCUBIC:
				dwValue = D3DTFG_GAUSSIANCUBIC;
				break;
			default:
				DebugOut.Write(1, "CDirect3D: SetTextureStageState7() invalid MagFilter value %d\n",dwValue);
				return E_FAIL;
		}
	}
                
	if (dwState == D3DTSS_MINFILTER)
	{
		switch (dwValue)
		{
			case TEXF_POINT: 
				dwValue = D3DTFN_POINT;
				break;
			case TEXF_LINEAR:
				dwValue = D3DTFN_LINEAR;
				break;
			case TEXF_ANISOTROPIC: 
				dwValue = D3DTFN_ANISOTROPIC;
				break;
			default:
				DebugOut.Write(1, "CDirect3D: SetTextureStageState7() invalid MinFilter value %d\n",dwValue);
				return E_FAIL;
		}
	}

	if (dwState == D3DTSS_MIPFILTER)
	{
		switch (dwValue)
		{
			case TEXF_NONE: 
				dwValue = D3DTFP_NONE;
				break;
			case TEXF_POINT: 
				dwValue = D3DTFP_POINT;
				break;
			case TEXF_LINEAR:
				dwValue = D3DTFP_LINEAR;
				break;
			default:
				DebugOut.Write(1, "CDirect3D: SetTextureStageState7() invalid MipFilter value %d\n",dwValue);
				return E_FAIL;
		}
	}

    return SetLastError(m_pDevice7->SetTextureStageState(dwStage,dwState,dwValue));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::ValidateDevice7(LPDWORD lpdwPasses)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::ValidateDevice7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->ValidateDevice(lpdwPasses));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::ApplyStateBlock7(UINT_PTR dwBlockHandle)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::ApplyStateBlock7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->ApplyStateBlock((DWORD)dwBlockHandle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CaptureStateBlock7(UINT_PTR dwBlockHandle)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::CaptureStateBlock7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->CaptureStateBlock((DWORD)dwBlockHandle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DeleteStateBlock7(UINT_PTR dwBlockHandle)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::DeleteStateBlock7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->DeleteStateBlock((DWORD)dwBlockHandle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateStateBlock7(D3DSTATEBLOCKTYPE d3dsbType,UINT_PTR* lpdwBlockHandle)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::CreateStateBlock7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->CreateStateBlock(d3dsbType,(DWORD*)lpdwBlockHandle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::Load7(LPDIRECTDRAWSURFACE7 lpDestTex,LPPOINT lpDestPoint,
					   LPDIRECTDRAWSURFACE7 lpSrcTex,LPRECT lprcSrcRect,DWORD dwFlags)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::Load7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->Load(lpDestTex,lpDestPoint,lpSrcTex,lprcSrcRect,dwFlags));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::LightEnable7(DWORD dwLightIndex,BOOL bEnable)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::LightEnable7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->LightEnable(dwLightIndex,bEnable));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetLightEnable7(DWORD dwLightIndex,BOOL* pbEnable)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::GetLightEnable7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->GetLightEnable(dwLightIndex,pbEnable));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetClipPlane7(DWORD dwIndex,D3DVALUE* pPlaneEquation)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::SetClipPlane7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->SetClipPlane(dwIndex,pPlaneEquation));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetClipPlane7(DWORD dwIndex,D3DVALUE* pPlaneEquation)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::GetClipPlane7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->GetClipPlane(dwIndex,pPlaneEquation));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetInfo7(DWORD dwDevInfoID,LPVOID pDevInfoStruct,DWORD dwSize)
{
    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::GetInfo7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice7->GetInfo(dwDevInfoID,pDevInfoStruct,dwSize));
}

// --------------------------------------------------------------------------------------
	

HRESULT CDevice::Present7(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion)
{
    if (m_pPrimary == NULL || m_pBackBuffers == NULL || m_pBackBuffers[0] == NULL)
	{
		DPF(1, _T("CDevice::Present7() - Invalid params.\n"));
		return SetLastError(E_FAIL);
	}

    if(m_bFullscreen)
		return SetLastError(m_pPrimary->GetD3DSurface7()->Flip(NULL,DDFLIP_WAIT));
	else
	{
       	POINT pt = {0, 0};
		RECT Dest;
		HWND hWnd = hDestWindowOverride == 0 ? m_pPresParams->hDeviceWindow : hDestWindowOverride;
		ADAPTER *pAdapter = GetAdapter();
		
		if(!ClientToScreen(hWnd, &pt))
		{
			DPF(1, _T("CDevice::Present7() - ClientToScreen failed.\n"));
			return SetLastError(E_FAIL);
		}

        Dest.left = pDestRect->left + pt.x - pAdapter->rMonitorRect.left;
		Dest.right = pDestRect->right + pt.x - pAdapter->rMonitorRect.left;
		Dest.top = pDestRect->top + pt.y - pAdapter->rMonitorRect.top;
		Dest.bottom = pDestRect->bottom + pt.y - pAdapter->rMonitorRect.top;
		
		return SetLastError(m_pPrimary->GetD3DSurface7()->Blt(&Dest, m_pBackBuffers[0]->GetD3DSurface7(), pSourceRect, DDBLT_WAIT, NULL));
	}
}


// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateAdditionalSwapChain7(PRESENT_PARAMETERS* pParams, CSwapChain** ppSwapChain)
{
	HRESULT			hr;
	DDSURFACEDESC2  Desc;
	LPDIRECTDRAWSURFACE7 pSwap7;

	if(pParams == NULL || ppSwapChain == NULL || m_pDevice7 == NULL || pParams->uBackBufferCount != 1)
	{
		DPF(1, _T("CDevice::CreateAdditionalSwapChain7() - invalid params.\n"));
		return SetLastError(E_FAIL);
	}

    ZeroMemory(&Desc,sizeof(DDSURFACEDESC2));
	Desc.dwSize = sizeof(DDSURFACEDESC2);
    Desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    Desc.dwWidth = pParams->uWidth;
    Desc.dwHeight = pParams->uHeight;

    memcpy(&Desc.ddpfPixelFormat, &(pParams->BackBufferFormat.ddpfPixelFormat), sizeof(DDPIXELFORMAT));
	
    Desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;

    hr = m_pD3D->m_pDDraw7->CreateSurface(&Desc, &pSwap7,NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateAdditionalSwapChain7() - CreateSurface failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		goto Error;
	}

	*ppSwapChain = new CSwapChain();
	if((*ppSwapChain) == NULL)
	{
		DPF(1, _T("CDevice::CreateAdditionalSwapChain7() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	if(!(*ppSwapChain)->Init(&pSwap7, pParams, this))
	{
		DPF(2, _T("CDevice::CreateAdditionalSwapChain7() - CSurface() init failed.\n"));
		hr = E_FAIL;
		goto Error;
	}

	RELEASE(pSwap7);

    return S_OK;

Error:

	RELEASE(pSwap7);
	RELEASE(*ppSwapChain);

	return hr;

}

// --------------------------------------------------------------------------------------

HRESULT CDevice::UpdateTexture7(CBaseTexture* pSourceTexture,
								CBaseTexture* pDestinationTexture)
{
	DWORD dwFlags = 0;
	LPDIRECTDRAWSURFACE7 pD3DSourceTexture = NULL;
	LPDIRECTDRAWSURFACE7 pD3DDestinationTexture = NULL;

    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::UpdateTexture7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

	pD3DSourceTexture = pSourceTexture->GetD3DTexture7();
    pD3DDestinationTexture = pDestinationTexture->GetD3DTexture7();

	if( pD3DSourceTexture == NULL || pD3DDestinationTexture == NULL )
	{
		DPF(1, _T("CDevice::UpdateTexture7() - Invalid CTexture.\n"));
		return SetLastError(E_FAIL);
	}

	if(pDestinationTexture->GetType() == RTYPE_CUBETEXTURE)
		dwFlags |= DDSCAPS2_CUBEMAP_ALLFACES;

    return SetLastError(m_pDevice7->Load(pD3DDestinationTexture, NULL, pD3DSourceTexture, NULL, dwFlags));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CopyRects7(CSurface* pSourceSurface,RECT* pSourceRectsArray,
							UINT cRects,CSurface* pDestinationSurface,POINT* pDestPointsArray)
{
	HRESULT hr;
	HRESULT hrReturn = S_OK;
	int i = 0;

    if (m_pDevice7 == NULL)
	{
		DPF(1, _T("CDevice::CopyRects7() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

	do
	{
		LPRECT pDest = NULL, pSrc = NULL;
		RECT Dest;

		if(pSourceRectsArray)
		{
			UINT uWidth = pSourceRectsArray[i].right - pSourceRectsArray[i].left;
			UINT uHeight = pSourceRectsArray[i].bottom - pSourceRectsArray[i].top;

            if(pDestPointsArray)
            {
			    Dest.left = pDestPointsArray[i].x;
			    Dest.top  = pDestPointsArray[i].y;
			    Dest.right = pDestPointsArray[i].x + uWidth;
			    Dest.bottom = pDestPointsArray[i].y + uHeight;
    			pDest = &Dest;
            }
            else
    			pDest = pSrc;

			pSrc = &(pSourceRectsArray[i]);
		}

		hr = pDestinationSurface->GetD3DSurface7()->Blt(pDest,pSourceSurface->GetD3DSurface7(),
												   pSrc, DDBLT_WAIT, 0);

		if(FAILED(hr))
		{
			DPF(1, _T("CDevice::CopyRects7() - Blt failed. hr=%s(%x).\n"), 
				m_pD3D->HResultToString(hr), hr);
			hrReturn = hr;
		}
	}
	while(++i < cRects);

	return hrReturn;
}

// ------------------------------------------------------------------------------

HRESULT CDevice::GetDepthStencilSurface7(CSurface** ppZStencilSurface)
{
	if((ppZStencilSurface != NULL) && (m_pZBuffer != NULL))
    {
        m_pZBuffer->AddRef();
		*ppZStencilSurface = m_pZBuffer;
    }

    return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateDepthStencilSurface7(UINT Width,UINT Height,FORMAT Format,
											MULTISAMPLE_TYPE MultiSample,
											CSurface** ppSurface)
{
	HRESULT			hr;
	DDSURFACEDESC2  Desc;
	DWORD			dwPool = POOL_DEFAULT;
	LPDIRECTDRAWSURFACE7 pZBuffer7 = NULL;
	CSurface		*pSurf = NULL;

    ZeroMemory(&Desc,sizeof(DDSURFACEDESC2));
	Desc.dwSize = sizeof(DDSURFACEDESC2);
    Desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
    Desc.dwWidth = Width;
    Desc.dwHeight = Height;

    memcpy(&Desc.ddpfPixelFormat, &(Format.ddpfPixelFormat), sizeof(DDPIXELFORMAT));
	
    // Always SYSMEM for REF or SW device
	if(m_dwDevType == DEVICETYPE_REF || m_dwDevType == DEVICETYPE_SW)
		dwPool = POOL_SYSTEMMEM;

	switch(dwPool)
	{
	case POOL_DEFAULT:
	case POOL_VIDEO:
		Desc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
		break;
	case POOL_AGP:
		Desc.ddsCaps.dwCaps |= (DDSCAPS_NONLOCALVIDMEM | DDSCAPS_VIDEOMEMORY);
		break;
	case POOL_MANAGED:
        Desc.ddsCaps.dwCaps2 |= DDSCAPS2_TEXTUREMANAGE;
		break;
    case POOL_SYSTEMMEM:
		Desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
		break;
	default:
		DPF(1, _T("CDevice::CreateDepthStencilSurface7() - invalid POOL flag.\n"));
		hr = E_FAIL;
		goto Exit;
	}

    Desc.ddsCaps.dwCaps |= DDSCAPS_ZBUFFER;

    hr = m_pD3D->m_pDDraw7->CreateSurface(&Desc, &pZBuffer7,NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface7() - CreateSurface failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	pSurf = new CSurface();
	if(pSurf == NULL)
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface7() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	if(!pSurf->Init(pZBuffer7, this, NULL))
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface7() - CSurface() init failed.\n"));
		hr = E_FAIL;
		goto Exit;
	}

Exit:

	RELEASE(pZBuffer7);
	
	if(FAILED(hr))
	{
		RELEASE(pSurf);
	}
	else
		*ppSurface = pSurf;

    return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateRenderTarget7(UINT Width,UINT Height,FORMAT Format,
									 MULTISAMPLE_TYPE MultiSample,BOOL Lockable,
									 CSurface** ppSurface)
{
    LPDIRECTDRAWSURFACE7 pSurf = NULL;
    DDSURFACEDESC2  ddDesc;
    HRESULT hr=S_OK;

    ZeroMemory(&ddDesc,sizeof(DDSURFACEDESC2));
    ddDesc.dwSize  = sizeof(DDSURFACEDESC2);
    ddDesc.dwWidth = Width;
    ddDesc.dwHeight = Height;
	memcpy(&(ddDesc.ddpfPixelFormat), &(Format.ddpfPixelFormat), sizeof(DDPIXELFORMAT));
	ddDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	ddDesc.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN;            
	if(m_dwDevType & DEVICETYPE_HAL)
	    ddDesc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
	else
        ddDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	// Create the surface
	hr = m_pD3D->m_pDDraw7->CreateSurface(&ddDesc, &pSurf, NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateRenderTarget7() - CreateSurface failed. hr=%s(%x).\n"), m_pD3D->HResultToString(hr), hr);
		return SetLastError(hr);
	}

    *ppSurface = new CSurface();
	if((*ppSurface) == NULL)
	{
		DPF(1, _T("CDevice::CreateRenderTarget7() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}
    	
	if(!((*ppSurface)->Init(pSurf, this, NULL)))
	{
		DPF(1, _T("CDevice::CreateRenderTarget7() - ppSurface->Init() failed.\n"));
	    RELEASE(*ppSurface);
        hr = E_FAIL;
    }

Exit:
    RELEASE(pSurf);
    return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateImageSurface7(UINT Width,UINT Height,FORMAT Format,
									 CSurface** ppSurface)
{
    LPDIRECTDRAWSURFACE7 pSurf = NULL;
    DDSURFACEDESC2  ddDesc;
    HRESULT hr=S_OK;

    ZeroMemory(&ddDesc,sizeof(DDSURFACEDESC2));
    ddDesc.dwSize  = sizeof(DDSURFACEDESC2);
    ddDesc.dwWidth = Width;
    ddDesc.dwHeight = Height;
	memcpy(&(ddDesc.ddpfPixelFormat), &(Format.ddpfPixelFormat), sizeof(DDPIXELFORMAT));
	ddDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	ddDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;            

	// Create the surface
	hr = m_pD3D->m_pDDraw7->CreateSurface(&ddDesc, &pSurf, NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateImageSurface7() - CreateSurface failed. hr=%s(%x).\n"), m_pD3D->HResultToString(hr), hr);
		return SetLastError(hr);
	}

    *ppSurface = new CSurface();
	if((*ppSurface) == NULL)
	{
		DPF(1, _T("CDevice::CreateImageSurface7() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}
    	
	if(!((*ppSurface)->Init(pSurf, this, NULL)))
	{
		DPF(1, _T("CDevice::CreateImageSurface7() - ppSurface->Init() failed.\n"));
	    RELEASE(*ppSurface);
        hr = E_FAIL;
    }

Exit:
    RELEASE(pSurf);
    return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetSamplerState7(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD Value)
{
    switch (Type)
    {
        case SAMP_ADDRESSU:      // D3DTEXTUREADDRESS for U coordinate
            return SetTextureStageState(Sampler, D3DTSS_ADDRESSU, Value);

        case SAMP_ADDRESSV:      // D3DTEXTUREADDRESS for V coordinate
            return SetTextureStageState(Sampler, D3DTSS_ADDRESSV, Value);

        case SAMP_ADDRESSW:      // D3DTEXTUREADDRESS for W coordinate
            DPF(1, "CDevice: SetSamplerState(...D3DSAMP_ADDRESSW...) unmapped on version %4.2f interfaces.\n", m_fInterface);
            return SetLastError(E_NOTIMPL);

        case SAMP_BORDERCOLOR:   // D3DCOLOR
            return SetTextureStageState(Sampler, D3DTSS_BORDERCOLOR, Value);

        case SAMP_MAGFILTER:     // D3DTEXTUREFILTER filter to use for magnification
            return SetTextureStageState(Sampler, D3DTSS_MAGFILTER, Value);

        case SAMP_MINFILTER:     // D3DTEXTUREFILTER filter to use for minification
            return SetTextureStageState(Sampler, D3DTSS_MINFILTER, Value);

        case SAMP_MIPFILTER:     // D3DTEXTUREFILTER filter to use between mipmaps during minification
            return SetTextureStageState(Sampler, D3DTSS_MIPFILTER, Value);

        case SAMP_MIPMAPLODBIAS: // float Mipmap LOD bias
            return SetTextureStageState(Sampler, D3DTSS_MIPMAPLODBIAS, Value);

        case SAMP_MAXMIPLEVEL:   // DWORD 0..(n-1) LOD index of largest map to use (0 == largest)
            return SetTextureStageState(Sampler, D3DTSS_MAXMIPLEVEL, Value);

        case SAMP_MAXANISOTROPY: // DWORD maximum anisotropy
            return SetTextureStageState(Sampler, D3DTSS_MAXANISOTROPY, Value);

        case SAMP_SRGBTEXTURE:
            DPF(1, "CDevice: SetSamplerState(...D3DSAMP_SRGBTEXTURE...) unmapped on version %4.2f interfaces.\n", m_fInterface);
            return SetLastError(E_NOTIMPL);

        default:
            DPF(1, "CDevice: SetSamplerState(%d) unmapped on version %4.2f interfaces.\n", Type, m_fInterface);
            return SetLastError(E_NOTIMPL);
        break;
    }
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetSamplerState7(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD *Value)
{
    switch (Type)
    {
        case SAMP_ADDRESSU:      // D3DTEXTUREADDRESS for U coordinate
            return GetTextureStageState(Sampler, D3DTSS_ADDRESSU, Value);

        case SAMP_ADDRESSV:      // D3DTEXTUREADDRESS for V coordinate
            return GetTextureStageState(Sampler, D3DTSS_ADDRESSV, Value);

        case SAMP_ADDRESSW:      // D3DTEXTUREADDRESS for W coordinate
            DPF(1, "CDevice: GetSamplerState(...D3DSAMP_ADDRESSW...) unmapped on version %4.2f interfaces.\n", m_fInterface);
            return SetLastError(E_NOTIMPL);

        case SAMP_BORDERCOLOR:   // D3DCOLOR
            return GetTextureStageState(Sampler, D3DTSS_BORDERCOLOR, Value);

        case SAMP_MAGFILTER:     // D3DTEXTUREFILTER filter to use for magnification
            return GetTextureStageState(Sampler, D3DTSS_MAGFILTER, Value);

        case SAMP_MINFILTER:     // D3DTEXTUREFILTER filter to use for minification
            return GetTextureStageState(Sampler, D3DTSS_MINFILTER, Value);

        case SAMP_MIPFILTER:     // D3DTEXTUREFILTER filter to use between mipmaps during minification
            return GetTextureStageState(Sampler, D3DTSS_MIPFILTER, Value);

        case SAMP_MIPMAPLODBIAS: // float Mipmap LOD bias
            return GetTextureStageState(Sampler, D3DTSS_MIPMAPLODBIAS, Value);

        case SAMP_MAXMIPLEVEL:   // DWORD 0..(n-1) LOD index of largest map to use (0 == largest)
            return GetTextureStageState(Sampler, D3DTSS_MAXMIPLEVEL, Value);

        case SAMP_MAXANISOTROPY: // DWORD maximum anisotropy
            return GetTextureStageState(Sampler, D3DTSS_MAXANISOTROPY, Value);

        case SAMP_SRGBTEXTURE:
            DPF(1, "CDevice: GetSamplerState(...D3DSAMP_SRGBTEXTURE...) unmapped on version %4.2f interfaces.\n", m_fInterface);
            return SetLastError(E_NOTIMPL);

        default:
            DPF(1, "CDevice: GetSamplerState(%d) unmapped on version %4.2f interfaces.\n", Type, m_fInterface);
            return SetLastError(E_NOTIMPL);
        break;
    }
}

// --------------------------------------------------------------------------------------

UINT CDevice::GetAvailableTextureMem7(void)
{
	DDSCAPS2      ddsCaps2; 
	DWORD         dwTotal; 
	DWORD         dwFree;
	HRESULT       hr; 
 
	// Initialize the structure.
	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
 
	ddsCaps2.dwCaps = DDSCAPS_TEXTURE; 
	
	hr = m_pD3D->m_pDDraw7->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree); 
	if (FAILED(hr))
		return 0;
	else
		return dwFree;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::EvictManagedResources7()
{
    return SetLastError(m_pD3D->m_pD3D7->EvictManagedTextures());
}