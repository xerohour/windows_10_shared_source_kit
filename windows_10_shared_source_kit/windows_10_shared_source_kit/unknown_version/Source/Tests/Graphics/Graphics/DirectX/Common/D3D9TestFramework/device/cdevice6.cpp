// ======================================================================================
//
//  CDevice6.cpp
//
//  Copyright (C) 2001 Microsoft Corporation
//
// ======================================================================================


#include "DevicePCH.h"

#define D3D_OVERLOADS
#include "CDevice.h"
#include <d3d.h>


#include "CSurface.h"
#include "CTextures.h"
#include "CSwapChain.h"
#include "CBuffers.h"


typedef HRESULT (WINAPI * DDCREATEEX)(GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter);

// --------------------------------------------------------------------------------------

HRESULT CDevice::Initialize6(PRESENT_PARAMETERS *pParams)
{
	VIEWPORT Viewport;
	ADAPTER    *pAdapter = GetAdapter();
	DDCREATEEX  pProc = (DDCREATEEX) GetProcAddress(GetModuleHandle("DDRAW.DLL"),"DirectDrawCreateEx");
	HRESULT hr;
	DWORD dwZFlags = 0;
	TCHAR szFormat[64] = {0};
	
	if(pAdapter == NULL || pProc == NULL || m_pD3D == NULL || pParams == NULL)
	{
		DPF(1, _T("CDevice::Initialize6() - invalid params.\n"));
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
    	DPF(1, _T("CDevice::Initialize6() - Out of memory.\n"));
		return  E_OUTOFMEMORY;
	}

	if(m_pD3D->m_pDDraw4 == NULL || m_pD3D->m_pD3D3 == NULL)
	{		
		// Setup D3D for this adapter
		hr = m_pD3D->SetupD3D6(this, pParams);
		if(FAILED(hr))
		{
			DPF(2, _T("CDevice::Initialize6() - SetupD3D() failed.\n"));
			ReleaseD3D();
			return hr;
		}
	}

	// Get a local handle to the Primary
	m_pPrimary = m_pD3D->m_pPrimary;
	m_pPrimary->AddRef();
	
	// Create the BackBuffers
	hr = CreateBackBuffers6(pParams);
	if (FAILED(hr))
	{
		DPF(1, _T("CDevice::Initialize6() - CreateBackBuffers6 failed. hr=%s(%x)\n"),
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
		hr = CreateDepthStencilSurface6(pParams->uWidth, pParams->uHeight, pParams->AutoDepthStencilFormat, MULTISAMPLE_NONE, &m_pZBuffer);
		if (FAILED(hr))
		{
			DPF(1, _T("CDevice::Initialize6() - Failed to create the ZBuffer. hr=%s(%x)\n"),
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
			DPF(1, _T("CDevice::Initialize6() - AddAttachedSurface failed. hr=%s(%x).\n"),
				m_pD3D->HResultToString(hr), hr);
			ReleaseD3D();
			return hr;
		}
	}
	
	// Decode the device type
	if(m_dwDevType & DEVICETYPE_HAL)
	{
		if(m_dwBehaviors & DEVICECREATE_HWVP)
			memcpy(&m_Guid, &IID_IDirect3DTnLHalDevice, sizeof(m_Guid));
		else
            memcpy(&m_Guid, &IID_IDirect3DHALDevice, sizeof(m_Guid));
	}
	else if(m_dwDevType & DEVICETYPE_REF)
		memcpy(&m_Guid, &IID_IDirect3DRefDevice, sizeof(m_Guid));
	else if(m_dwDevType & DEVICETYPE_SW)
		memcpy(&m_Guid, &IID_IDirect3DRGBDevice, sizeof(m_Guid));
	else
	{
		DPF(1, _T("CDevice::Initialize6() - unknown device type.\n"));
		ReleaseD3D();
		return E_FAIL;
	}

	// Create the actual device
	hr = m_pD3D->CreateDevice(m_Guid, m_pBackBuffers[0]->GetD3DSurface6(), &m_pDevice3);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::Initialize6() - CreateDevice failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr); 
		ReleaseD3D();

        // Make out of memory a valid failure
        if(hr == DDERR_OUTOFVIDEOMEMORY)
        {
            hr = E_UNSUPPORTED;
        }
		return hr;
	}

	// Create the Viewport
	hr = m_pD3D->m_pD3D3->CreateViewport(&m_pViewport3,NULL);
	if(FAILED(hr))
	{
        DPF(1, _T("CDevice::Initialize6() - CreateViewport failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr); 
		ReleaseD3D();
		return hr;
	}

	hr = m_pDevice3->AddViewport(m_pViewport3);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::Initialize6() - AddViewport failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr); 
		ReleaseD3D();
		return hr;
	}

	hr = m_pDevice3->SetCurrentViewport(m_pViewport3);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::Initialize6() - SetCurrentViewport failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr); 
		ReleaseD3D();
		return hr;
	}

	// Set the default viewport
	Viewport.X = 0;    
	Viewport.Y = 0;    
	Viewport.Width = m_pPresParams->uWidth;
	Viewport.Height = m_pPresParams->uHeight;
	Viewport.MinZ = 0.f; 
	Viewport.MaxZ = 1.f; 
	SetViewport(&Viewport);

	// Create the material
    hr = m_pD3D->m_pD3D3->CreateMaterial(&m_pMaterial3, NULL);
    if(FAILED(hr))
	{
		DPF(1, _T("CDevice::Initialize6() - CreateMaterial failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr); 
		ReleaseD3D();
		return hr;
	}

	// Create light
	if (m_pLights3 == NULL)
	{
		m_pLights3 = (LPDIRECT3DLIGHT*)malloc(sizeof(LPDIRECT3DLIGHT)*8);
		if(NULL == m_pLights3)
		{
			DPF(1, _T("CDevice::Initialize6() - Out of memory.\n"));
			ReleaseD3D();
			return E_OUTOFMEMORY;
		}

		ZeroMemory(m_pLights3, sizeof(LPDIRECT3DLIGHT)*8);
		m_dwLights3 = 8;
	}

	return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::ValidateMode6(DEVICEMODE *pMode)
{
	TCHAR szBuffer[64] = {0};
    if( (m_dwDevType == DEVICETYPE_REF) && !m_pD3D->IsRefInstalled())
	{
        DPF(1, _T("Reference driver(%s) not detected.\n"), m_pD3D->GetRefDLL());
		return E_ABORT;
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

HRESULT CDevice::CreateTexture6(UINT Width,UINT Height,UINT Levels,DWORD Usage,
								FORMAT Format,DWORD Pool,CTexture** ppTexture)
{
    HRESULT hr;
    CTexture *pTexture, *pTexOut = NULL;
	LPDIRECTDRAWSURFACE4 pD3DTexture = NULL;
	DDSURFACEDESC2 ddDesc;

    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::CreateTexture6() - Invalid Device.\n"));
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
		DPF(1, _T("CDevice::CreateTexture6() - invalid POOL flag.\n"));
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
		// Eat all non Dx6 usage types
		break;
	}

	//
	// Create the target surface
	// 

	pTexture = new CTexture;
	if(pTexture == NULL)
	{
		DPF(1, _T("CDevice::CreateTexture6() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	hr = m_pD3D->m_pDDraw4->CreateSurface( &ddDesc, &pD3DTexture, NULL);
	if( FAILED( hr ) )
	{
		DPF(2, _T("CDevice::CreateTexture6() - CreateSurface failed. hr=%s(%x)\n"),
				  m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	if(!pTexture->Init(pD3DTexture, this))
	{
		DPF(1, _T("CDevice::CreateTexture6() - CTexture::Init() failed.\n") );
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
		    DPF(1, _T("CDevice::CreateTexture6() - Out of memory.\n"));
		    hr = E_OUTOFMEMORY;
		    goto Exit;
	    }

        ddDesc.ddsCaps.dwCaps &= ~(DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM);
        ddDesc.ddsCaps.dwCaps2 &= ~(DDSCAPS2_TEXTUREMANAGE);
        ddDesc.ddsCaps.dwCaps &= ~(DDSCAPS_3DDEVICE | DDSCAPS_ZBUFFER);
	    ddDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	    hr = m_pD3D->m_pDDraw4->CreateSurface( &ddDesc, &pD3DTexture, NULL);
	    if( FAILED( hr ) )
	    {
		    DPF(1, _T("CDevice::CreateTexture6() - CreateSurface failed. hr=%s(%x)\n"),
				    m_pD3D->HResultToString(hr), hr);
		    goto Exit;
	    }

	    if(!pTexture->Init(pD3DTexture, this))
	    {
		    DPF(1, _T("CDevice::CreateTexture6() - CTexture::Init() failed.\n") );
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
		RELEASE(pTexOut);
		RELEASE(pD3DTexture);
	}
	else
        *ppTexture = pTexOut;

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateBackBuffers6(PRESENT_PARAMETERS *pParams)
{
	HRESULT hr;
    LPDIRECTDRAWSURFACE4 pSurf = NULL;
    int i;

	// Allocate the BackBuffer Ptrs
	m_pBackBuffers = new CSurface*[pParams->uBackBufferCount];
	if(m_pBackBuffers == NULL)
	{
		DPF(1, _T("CDevice::CreateBackBuffers6() - Out of memory.\n"));
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
			DPF(1, _T("CDevice::CreateBackBuffers6() - Out of memory.\n"));
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

			hr = m_pPrimary->GetD3DSurface6()->GetAttachedSurface(&ddsCaps, &pSurf);
			if(FAILED(hr))
			{
				DPF(1, _T("CDevice::CreateBackBuffers6() - GetAttachedSurface failed. hr=%s(%x)\n"),
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
			hr = m_pD3D->m_pDDraw4->CreateSurface(&ddDesc, &pSurf, NULL);
			if(FAILED(hr))
			{
				DPF(1, _T("CDevice::CreateBackBuffers6() - CreateSurface failed. hr=%s(%x).\n"), m_pD3D->HResultToString(hr), hr);
				goto Error;
			}

			//Set the real backbuffer format in the present params
			pSurf->GetSurfaceDesc(&ddDesc);
			memcpy(&(pParams->BackBufferFormat.ddpfPixelFormat), &(ddDesc.ddpfPixelFormat), sizeof(DDPIXELFORMAT));
			GetD3D()->DDPFToD3DFmt((LPDDPIXELFORMAT)&(pParams->BackBufferFormat.ddpfPixelFormat), (FMT*)&(pParams->BackBufferFormat.d3dfFormat), NULL);
		}

		if(!m_pBackBuffers[i]->Init(pSurf, this, NULL))
		{
			DPF(2, _T("CDevice::CreateBackBuffers6() - pBackBuffer->Init() failed.\n"));
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

HRESULT CDevice::CreateVertexBuffer6(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB)
{
	CVertexBuffer			*pVB = NULL;
    D3DVERTEXBUFFERDESC     Desc;
	LPDIRECT3DVERTEXBUFFER	pD3DVB;
	HRESULT					hr;
	DWORD					dwFlags = 0;
	
    Desc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
    Desc.dwCaps = 0;
    Desc.dwFVF = FVF;
    Desc.dwNumVertices = Length / GetVertexSize(FVF);
	if (Pool == FMWK_DEFAULT)
	{
		if ((FVF & D3DFVF_POSITION_MASK) != D3DFVF_XYZRHW || m_dwDevType == DEVICETYPE_SW || m_dwDevType == DEVICETYPE_REF)
			Desc.dwCaps |= D3DVBCAPS_SYSTEMMEMORY;
	}
	if (Usage == USAGE_SOFTWAREPROCESSING || Pool == POOL_SYSTEMMEM || Pool == POOL_MANAGED)
		Desc.dwCaps |= D3DVBCAPS_SYSTEMMEMORY;
	if (Usage == USAGE_WRITEONLY)
		Desc.dwCaps |= D3DVBCAPS_WRITEONLY;
	if (Usage == USAGE_DONOTCLIP)
		dwFlags = D3DDP_DONOTCLIP;

	pVB = new CVertexBuffer();
	if (NULL == pVB)
	{
		DPF(1, _T("CDevice::CreateVertexBuffer() - Out of memory.\n"));
		return E_FAIL;
	}

	hr = m_pD3D->m_pD3D3->CreateVertexBuffer(&Desc, &pD3DVB, dwFlags, NULL);
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

void CDevice::ReleaseD3D6()
{
        int i;
	if(m_pD3D && m_pD3D->m_pDDraw4 && m_bFullscreen)
	{
		m_pD3D->m_pDDraw4->SetCooperativeLevel(m_hFocusWindow, DDSCL_NORMAL);
		m_pD3D->m_pDDraw4->RestoreDisplayMode();
	}

	// Release ZBuffer before releasing render targets
	RELEASE(m_pZBuffer);
    
    // Release the backbuffers
    for(i=0; i<m_nBackBuffers; i++)
	{
		RELEASE(m_pBackBuffers[i]);
	}
	SAFEDELETEA(m_pBackBuffers);
	m_nBackBuffers = 0;

	
	for (i = 0; i < m_dwLights3; i++)
	{
		if (m_pLights3[i])
		{
			if (m_pViewport3)
				m_pViewport3->DeleteLight(m_pLights3[i]);
			RELEASE(m_pLights3[i]);
		}
	}
	free(m_pLights3);
	m_pLights3 = NULL;
	m_dwLights3 = 0;
	
    SAFEDELETE(m_pPresParams);
	RELEASE(m_pViewport3);
	RELEASE(m_pMaterial3);
	RELEASE(m_pPrimary);

    if(m_pDevice3)
	{
		UINT uRef = m_pDevice3->Release();
		m_pDevice3 = NULL;
		if(uRef)
		{
			DPF(1, _T("CDevice::ReleaseD3D6() - m_pDevice3 ref count is %d after final Release().\n"), uRef);
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
bool CDevice::IsDeviceLost6()
{
	HRESULT hr = m_pD3D->m_pDDraw4->TestCooperativeLevel();
	if ( FAILED( hr ) )
	{
		return true;
	};

	hr = m_pBackBuffers[0]->GetD3DSurface6()->IsLost();
	if ( FAILED( hr ) )
	{
		return true;
	};

	return false;
}


// --------------------------------------------------------------------------------------
bool CDevice::IsDeviceReadyForReset6()
{
	HRESULT hDeviceState = m_pD3D->m_pDDraw4->TestCooperativeLevel();

	if ( SUCCEEDED( hDeviceState ) )
	{
		return true;
	};

	return false;
}


// --------------------------------------------------------------------------------------
bool CDevice::HandleDeviceLost6()
{
	HRESULT hr;

	hr = m_pD3D->m_pDDraw4->TestCooperativeLevel();
	if(FAILED(hr))
		return false;

	hr = m_pD3D->m_pDDraw4->RestoreAllSurfaces();
    if(FAILED(hr))
	{
		DPF(1, _T("CDevice::HandleDeviceLost6() - RestoreAllSurfaces() failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		return false;
	}

	return true;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::Resize6(UINT uWidth, UINT uHeight)
{
	DPF(1, _T("CDevice::Resize7() - Currently not implemented.\n"));
	return E_NOTIMPL;
}

// --------------------------------------------------------------------------------------

bool CDevice::Draw2DText6(DWORD dwColor, TCHAR *szString, int nLen, RECT* pRect, DWORD dwFlags)
{
	HDC hDC = 0;
    HRESULT hr;
	HGDIOBJ hGDIObj = NULL;
	UINT nRed = (dwColor & 0x00FF0000) >> 16;
    UINT nGreen = (dwColor & 0x0000FF00) >> 8;
    UINT nBlue = (dwColor & 0x000000FF);

	if(szString == NULL || pRect == NULL)
	{
	    DPF(1, _T("CDevice::Draw2DText6 - Invalid params.\n"));
		return false;
	}

	UINT nX = (dwFlags & DT_CENTER) ? (pRect->right - pRect->left) / 2 : pRect->left;
    	
    hr = m_pBackBuffers[0]->GetD3DSurface6()->GetDC(&hDC);
    if(FAILED(hr))
	{
		DPF(1, _T("CDevice::Draw2DText6 - GetDC failed. hr=%s(%x)\n"),
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
		hr = m_pBackBuffers[0]->GetD3DSurface6()->ReleaseDC(hDC);
		if(FAILED(hr))
		{
			DPF(1, _T("CDevice::Draw2DText6 - ReleaseDC failed. hr=%s(%x)\n"),
				m_pD3D->HResultToString(hr), hr);
			return false;
		}
	}

	return true;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::QueryInterface6(REFIID riid, LPVOID * ppvObj)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::QueryInterface6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->QueryInterface(riid,ppvObj));
}

// --------------------------------------------------------------------------------------

bool CDevice::IsDeviceEqual6(PVOID pDevice)
{
	if(m_pDevice3 == (LPDIRECT3DDEVICE3) pDevice)
		return true;
	else
		return false;
}

// --------------------------------------------------------------------------------------

ULONG CDevice::AddRef6(void)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::AddRef6() - Invalid Device.\n"));
		return (ULONG)E_FAIL;
	}

    return m_pDevice3->AddRef( );
}

// --------------------------------------------------------------------------------------

ULONG CDevice::Release6(void)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::Release6() - Invalid Device.\n"));
		return (ULONG)E_FAIL;
	}

    return m_pDevice3->Release( );
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetVerticalBlankStatus6(LPBOOL lpbIsInVB)
{
    if (!m_pD3D || !m_pD3D->m_pDDraw4)
    {
        DPF(1, _T("CDevice::GetVerticalBlankStatus6() - Invalid Device.\n"));
        return SetLastError(E_FAIL);
    }

    return SetLastError(m_pD3D->m_pDDraw4->GetVerticalBlankStatus(lpbIsInVB));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetCaps6(LPD3DDEVICEDESC lpD3DDevDesc)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::GetCaps6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return E_NOTIMPL;//SetLastError(m_pDevice3->GetCaps(lpD3DDevDesc));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::EnumTextureFormats6(LPD3DENUMPIXELFORMATSCALLBACK lpd3dEnumPixelProc, LPVOID lpArg)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::EnumTextureFormats6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->EnumTextureFormats(lpd3dEnumPixelProc,lpArg));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::BeginScene6(void)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::BeginScene6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->BeginScene( ));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::EndScene6(void)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::EndScene6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->EndScene( ));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetDirect3D6(LPDIRECT3D3* lplpD3D)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::GetDirect3D6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->GetDirect3D(lplpD3D));
}

// ------------------------------------------------------------------------------

HRESULT CDevice::SetRenderTarget6(CSurface *pNew, CSurface *pNewDepth)
{
    HRESULT hr = S_OK;

    if ((pNew != NULL) && (pNewDepth != NULL))
    {
        DDSCAPS2 ddsCaps;
        LPDIRECTDRAWSURFACE4 pSurf = NULL;
		memset(&ddsCaps, 0, sizeof(DDSCAPS2));
		ddsCaps.dwCaps = DDSCAPS_ZBUFFER;

        // Check if surface already has a ZBuffer
		hr = pNew->GetD3DSurface6()->GetAttachedSurface(&ddsCaps, &pSurf);
        RELEASE(pSurf);

        if (FAILED(hr))
		{
            // Attach new ZBuffer 
            hr = pNew->AddAttachedSurface(pNewDepth);

            if (FAILED(hr))
	        {
		        DPF(1, _T("CDevice::SetRenderTarget6() - AddAttatchedSurface() failed.\n"));
		        return SetLastError(hr);
	        }
        }
    }


    return SetLastError(m_pDevice3->SetRenderTarget(pNew == NULL ? NULL : pNew->GetD3DSurface6(),0));
}

// ------------------------------------------------------------------------------

HRESULT CDevice::GetRenderTarget6(CSurface** ppRenderTarget)
{
	LPDIRECTDRAWSURFACE4 pD3DSurface;
	CSurface* pSurface = new CSurface;

    if (pSurface == NULL)
	{
		DPF(1, _T("CDevice::GetRenderTarget6() - Out of Memory.\n"));
		return SetLastError(E_FAIL);
	}

	HRESULT hr = m_pDevice3->GetRenderTarget( &pD3DSurface );
	if( FAILED( hr ) )
	{
		DPF(1, _T("CDevice::GetRenderTarget6() - GetRenderTarget() failed.\n"));
		RELEASE( pSurface );
		return SetLastError(hr);
	}

	if( !pSurface->Init( pD3DSurface, this, NULL ) )
	{
		DPF(1, _T("CDevice::GetRenderTarget6() - CSurface::Init() failed.\n"));
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

HRESULT CDevice::Clear6(DWORD dwCount,LPD3DRECT lpRects,DWORD dwFlags,D3DCOLOR dwColor,
						D3DVALUE dvZ,DWORD dwStencil)
{
	HRESULT hr;

    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::Clear6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

	if(dwFlags == FMWK_DEFAULT)
	{
		dwFlags = 0;
        dwFlags |= CLEARF_TARGET;
		dwFlags |= m_pPresParams->bAutoDepthStencil ? CLEARF_ZBUFFER : 0;
	}
    
	// Dx6 requires a rect to clear to.  If one is not given, we will default to 
	// size of the current render target.
	if (dwCount == 0)
	{
		CSurface *pRenderTarget = NULL;
		hr = GetRenderTarget(&pRenderTarget);

		if ((pRenderTarget == NULL) || FAILED(hr))
		{
			DPF(1, _T("CDevice::Clear6() - GetRenderTarget() failed. hr=%s(%x).\n"),
				m_pD3D->HResultToString(hr), hr);
			return SetLastError(hr);
		}

        D3DRECT rc = {0,0,pRenderTarget->m_Desc.dwWidth,pRenderTarget->m_Desc.dwHeight};

		RELEASE(pRenderTarget);

		hr = m_pViewport3->Clear2(1,&rc,dwFlags,dwColor,dvZ,dwStencil);
	}
	else
	{
		hr = m_pViewport3->Clear2(dwCount,lpRects,dwFlags,dwColor,dvZ,dwStencil);
	}

    return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetTransform6(D3DTRANSFORMSTATETYPE State,LPD3DMATRIX lpD3DMatrix)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::SetTransform6() - Invalid Device.\n"));
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

    return SetLastError(m_pDevice3->SetTransform(State,lpD3DMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetTransform6(D3DTRANSFORMSTATETYPE dtstTransformStateType,LPD3DMATRIX lpD3DMatrix)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::GetTransform6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->GetTransform(dtstTransformStateType,lpD3DMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetViewport6(LPVIEWPORT pViewport)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::SetViewport6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

	// convert from private struct
    D3DVIEWPORT2    ViewData;

    ViewData.dwSize         = sizeof(ViewData);
    ViewData.dwX            = pViewport->X;
    ViewData.dwY            = pViewport->Y;
    ViewData.dwWidth        = pViewport->Width;
    ViewData.dwHeight       = pViewport->Height;
    ViewData.dvClipX        = -1.f;
    ViewData.dvClipWidth    = 2.f;
    ViewData.dvClipHeight   = 2.f;
    ViewData.dvClipY        = 1.f;
    ViewData.dvMinZ         = pViewport->MinZ;
    ViewData.dvMaxZ         = pViewport->MaxZ;

   return SetLastError(m_pViewport3->SetViewport2(&ViewData));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetViewport6(LPVIEWPORT pViewport)
{
    if (NULL==m_pDevice3)
	{
		DPF(1, _T("CDevice::GetViewport6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

	D3DVIEWPORT2    Viewport;
	ZeroMemory( &Viewport, sizeof(Viewport) );

	Viewport.dwSize = sizeof(Viewport);

	D3DVIEWPORT2* pGetViewportParam;

	if(NULL!=pViewport)
	{
		pGetViewportParam = &Viewport;
	}
	else
	{
		pGetViewportParam = NULL;
	}

	// Call GetViewport even if pViewport == NULL (for invalid param testing)
	HRESULT			hr = SetLastError(m_pViewport3->GetViewport2(pGetViewportParam));

	if(NULL == pViewport)
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

HRESULT CDevice::MultiplyTransform6(D3DTRANSFORMSTATETYPE dtstTransformStateType,LPD3DMATRIX lpD3DMatrix)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::MultiplyTransform6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->MultiplyTransform(dtstTransformStateType,lpD3DMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetMaterial6(LPMATERIAL lpMaterial)
{
	D3DMATERIALHANDLE   hHandle;
	D3DMATERIAL	Mat;
	HRESULT hr;

    if (m_pDevice3 == NULL || NULL == m_pMaterial3)
	{
		DPF(1, _T("CDevice::SetMaterial6() - Invalid params.\n"));
		return SetLastError(E_FAIL);
	}

	memset(&Mat, 0, sizeof(D3DMATERIAL));
	Mat.dwSize = sizeof(D3DMATERIAL);
	memcpy((((DWORD*)&Mat)+1), lpMaterial, sizeof(MATERIAL));

    
	hr = m_pMaterial3->SetMaterial(&Mat);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::SetMaterial6() - SetMaterial failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		return hr;
	}

	hr = m_pMaterial3->GetHandle(m_pDevice3,&hHandle);
    if(FAILED(hr))
	{
		DPF(1, _T("CDevice::SetMaterial6() - GetHandle failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		return hr;
	}

    return SetLastError(SetLightState6(D3DLIGHTSTATE_MATERIAL, hHandle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetMaterial6(LPMATERIAL lpMaterial)
{
	D3DMATERIALHANDLE hMaterialHandleFromDevice, hMaterialHandleFromMaterial;
    HRESULT hr;
	D3DMATERIAL	Mat;
	
	if (NULL == m_pDevice3 || NULL == m_pMaterial3)
	{
		DPF(1, _T("CDevice::GetMaterial6() - Invalid Device or invalid Material.  Has CDevice::Initialize6 been called?\n"));
		return SetLastError(E_FAIL);
	}

	/*Get the material handle from the material, and then get the handle from the device
	These two should be equall.  If they are then just return the properites from the current material.
	Otherwise return an error because an unknown material has been set in the device*/
	/*Get the material's handle*/
	hr = m_pMaterial3->GetHandle(m_pDevice3,&hMaterialHandleFromMaterial);
    if(FAILED(hr))
	{
		DPF(1, _T("CDevice::GetMaterial6() - GetHandle failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		return hr;
	}


	/*Get the device's handle*/
	//m_pMaterial3 is created in CDevice::Initialize6
	hr = m_pDevice3->GetLightState(D3DLIGHTSTATE_MATERIAL, &hMaterialHandleFromDevice);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::GetMaterial6() - GetLightState failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		return hr;
	}

	if(NULL == hMaterialHandleFromDevice)
	{
		DPF(1, _T("CDevice::GetMaterial6() - No material selected into the device."));
		return SetLastError(E_FAIL);
	}

	/*See if they are equal*/
	if(hMaterialHandleFromMaterial!=hMaterialHandleFromDevice)
	{
		DPF(1, _T("CDevice::GetMaterial6() Unknown material handle currently selected into the device"));
		return SetLastError(E_FAIL);
	}

	/*If it gets this far, then m_pMaterial3 is current selected into the device.
	Therefore just return the properities of m_pMaterial3*/

	memset(&Mat, 0, sizeof(D3DMATERIAL));
	Mat.dwSize = sizeof(D3DMATERIAL);

	hr = m_pMaterial3->GetMaterial(&Mat);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::GetMaterial6() - GetLightState failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		return hr;
	}


	memcpy(lpMaterial, (((DWORD*)&Mat)+1), sizeof(MATERIAL));

    return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetLight6(DWORD dwLightIndex,LPLIGHT lpLight)
{
	LPDIRECT3DLIGHT		pLight = GetLight3Pointer(dwLightIndex);
	HRESULT				hr;
	D3DLIGHT2			Light2;

	if (pLight == NULL)
	{
		DPF(1, _T("CDevice::SetLight6() - CreateLight() failed.\n"));
		return SetLastError(E_FAIL);
	}
	
	// Set light parameters
	ZeroMemory(&Light2, sizeof(Light2));
	Light2.dwSize = sizeof(Light2);
	Light2.dltType = lpLight->dltType;
	memcpy(&Light2.dcvColor, &lpLight->dcvDiffuse, sizeof(Light2.dcvColor));
	memcpy(&Light2.dvPosition, &lpLight->Position, sizeof(Light2.dvPosition));
	memcpy(&Light2.dvDirection, &lpLight->Direction, sizeof(Light2.dvDirection));
	Light2.dvRange = lpLight->Range;
	Light2.dvFalloff = lpLight->Falloff;
	Light2.dvAttenuation0 = lpLight->Attenuation0;
	Light2.dvAttenuation1 = lpLight->Attenuation1;
	Light2.dvAttenuation2 = lpLight->Attenuation2;
	Light2.dvTheta = lpLight->Theta;
	Light2.dvPhi = lpLight->Phi;
	Light2.dwFlags = 0;

	hr = m_pLights3[dwLightIndex]->SetLight((LPD3DLIGHT)&Light2);
	if (FAILED(hr))
	{
		DPF(1, _T("CDevice::SetLight6() - SetLight() failed.\n"));
		return SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetLight6(DWORD dwLightIndex,LPLIGHT lpLight)
{
	D3DLIGHT2	Light2;
	HRESULT		hr;

    // Check device
	if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::GetLight6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

	// Check light interface
	if (dwLightIndex >= m_dwLights3 || m_pLights3[dwLightIndex] == NULL)
	{
		DPF(1, _T("CDevice::GetLight6() - Invalid light index\n"));
		return SetLastError(DDERR_INVALIDPARAMS);
	}

	// Get light parameters
	ZeroMemory(&Light2, sizeof(Light2));
	Light2.dwSize = sizeof(Light2);
	hr = m_pLights3[dwLightIndex]->GetLight((D3DLIGHT*)&Light2);
	if (FAILED(hr))
	{
		DPF(1, _T("CDevice::GetLight6() - GetLight() failed.\n"));
		return SetLastError(hr);
	}

	lpLight->dltType = Light2.dltType;
	memcpy(&lpLight->Diffuse, &Light2.dcvColor, sizeof(lpLight->Diffuse));
	memcpy(&lpLight->Specular, &Light2.dcvColor, sizeof(lpLight->Specular));
	memcpy(&lpLight->Ambient, &Light2.dcvColor, sizeof(lpLight->Ambient));
	memcpy(&lpLight->Position, &Light2.dvPosition, sizeof(lpLight->Position));
	memcpy(&lpLight->Direction, &Light2.dvDirection, sizeof(lpLight->Direction));
	lpLight->Range = Light2.dvRange;
	lpLight->Falloff = Light2.dvFalloff;
	lpLight->Attenuation0 = Light2.dvAttenuation0;
	lpLight->Attenuation1 = Light2.dvAttenuation1;
	lpLight->Attenuation2 = Light2.dvAttenuation2;
	lpLight->Theta = Light2.dvTheta;
	lpLight->Phi = Light2.dvPhi;

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::LightEnable6(DWORD dwLightIndex,BOOL bEnable)
{
	HRESULT		hr;
	D3DLIGHT2	Light2;

    // Check device
	if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::LightEnable6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

	// Check light interface
	if (dwLightIndex >= m_dwLights3 || m_pLights3[dwLightIndex] == NULL)
	{
		DPF(1, _T("CDevice::LightEnable6() - Invalid light index\n"));
		return SetLastError(DDERR_INVALIDPARAMS);
	}

	ZeroMemory(&Light2, sizeof(Light2));
	Light2.dwSize = sizeof(Light2);
	hr = m_pLights3[dwLightIndex]->GetLight((LPD3DLIGHT)&Light2);
	if (FAILED(hr))
	{
		DPF(1, _T("CDevice::LightEnable6() - GetLight() failed.\n"));
		return SetLastError(hr);
	}

	if (bEnable)
		Light2.dwFlags |= D3DLIGHT_ACTIVE;
	else
		Light2.dwFlags &= ~D3DLIGHT_ACTIVE;

	hr = m_pLights3[dwLightIndex]->SetLight((LPD3DLIGHT)&Light2);
	if (FAILED(hr))
	{
		DPF(1, _T("CDevice::LightEnable6() - SetLight() failed.\n"));
		return SetLastError(hr);
	}

	return hr;
}

// --------------------------------------------------------------------------------------

LPDIRECT3DLIGHT CDevice::GetLight3Pointer(DWORD dwLightIndex)
{
	HRESULT		hr;

	// Grow lights array
	if(dwLightIndex >= m_dwLights3)
	{
		const unsigned int nGrowAmt = 2;

		//make sure the index is small enough to avoid overflow
		if(dwLightIndex >= (0xFFFFFFFF/nGrowAmt))
		{
			SetLastError(E_FAIL);
			return NULL;
		}

		DWORD dwNewLightCount = dwLightIndex*nGrowAmt;

		LPDIRECT3DLIGHT*	pNewLights3 = (LPDIRECT3DLIGHT*)realloc(m_pLights3, sizeof(LPDIRECT3DLIGHT)*dwNewLightCount);
		
		if (NULL == pNewLights3)
		{
			SetLastError(E_FAIL);
			return NULL;
		}
		else
		{
			m_pLights3 = pNewLights3;
			ZeroMemory(&(m_pLights3[m_dwLights3]), sizeof(LPDIRECT3DLIGHT)*(dwNewLightCount-m_dwLights3));
			m_dwLights3 = dwNewLightCount;
		}
	}




	// Create light
	if (m_pLights3[dwLightIndex] == NULL)
	{
		hr = m_pD3D->m_pD3D3->CreateLight(&m_pLights3[dwLightIndex], NULL);
		if (FAILED(hr))
		{
			DPF(1, _T("CDevice::GetLight3Pointer() - CreateLight() failed.\n"));
			SetLastError(hr);
			return NULL;
		}

		hr = m_pViewport3->AddLight(m_pLights3[dwLightIndex]);
		if (FAILED(hr))
		{
			DPF(1, _T("CDevice::GetLight3Pointer() - AddLight() failed.\n"));
			SetLastError(hr);
			return NULL;
		}
	}
	return m_pLights3[dwLightIndex];
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetLightState6(D3DLIGHTSTATETYPE dwState, DWORD dwValue)
{
    if (NULL == m_pDevice3)
    {
        DPF(1, _T("CDevice::SetLightState6() - Invalid Device.\n"));
        return SetLastError(E_FAIL);
    }

    return m_pDevice3->SetLightState(dwState, dwValue);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetLightState6(D3DLIGHTSTATETYPE dwState, DWORD *pdwValue)
{
    if (NULL == m_pDevice3)
    {
        DPF(1, _T("CDevice::GetLightState6() - Invalid Device.\n"));
        return SetLastError(E_FAIL);
    }

    return SetLastError(m_pDevice3->GetLightState(dwState, pdwValue));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetRenderState6(D3DRENDERSTATETYPE dwState,DWORD dwValue)
{
    if (NULL == m_pDevice3)
	{
		DPF(1, _T("CDevice::SetRenderState6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

	// For D3DRS_FOGSTART, D3DRS_FOGEND and D3DRS_FOGDENSITY we need 
	// call both SetRenderState() to set parameters of the pixel fog 
	// and SetLightState() to set parameters of the vertex for to
	// emulate DX7/DX8 functionality
    switch(dwState)
    {
		case    D3DRENDERSTATE_AMBIENT:
				return SetLightState6(D3DLIGHTSTATE_AMBIENT,dwValue);
		case    D3DRENDERSTATE_COLORVERTEX:
				return SetLightState6(D3DLIGHTSTATE_COLORVERTEX,dwValue);
		case    D3DRENDERSTATE_FOGVERTEXMODE:
				return SetLightState6(D3DLIGHTSTATE_FOGMODE,dwValue);
		case    D3DRENDERSTATE_FOGSTART:
				SetLastError(SetLightState6(D3DLIGHTSTATE_FOGSTART,dwValue));
				return SetLastError(m_pDevice3->SetRenderState(D3DRENDERSTATE_FOGSTART,dwValue));
		case    D3DRENDERSTATE_FOGEND:
				SetLastError(SetLightState6(D3DLIGHTSTATE_FOGEND,dwValue));
				return SetLastError(m_pDevice3->SetRenderState(D3DRENDERSTATE_FOGEND,dwValue));
		case    D3DRENDERSTATE_FOGDENSITY:
				SetLastError(SetLightState6(D3DLIGHTSTATE_FOGDENSITY,dwValue));
				return SetLastError(m_pDevice3->SetRenderState(D3DRENDERSTATE_FOGDENSITY,dwValue));
    }

	return SetLastError(m_pDevice3->SetRenderState(dwState,dwValue));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetRenderState6(D3DRENDERSTATETYPE dwState,DWORD *pdwValue)
{
    if (NULL == m_pDevice3)
	{
		DPF(1, _T("CDevice::GetRenderState6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->GetRenderState(dwState,pdwValue));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::BeginEnd6(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc,
                        LPVOID lpvVertices, DWORD  dwVertexCount,
                        LPWORD lpwIndices, DWORD  dwIndexCount, DWORD  dwFlags)
{
    HRESULT    hr;

	// DX6 Begin / Vertex / End
    if (lpwIndices == NULL)
    {
        DWORD	dwVertexSize = GetVertexSize(dwVertexTypeDesc);
		LPVOID	p = lpvVertices;
		DWORD	i = 0;

		// Begin
		hr = m_pDevice3->Begin(d3dptPrimitiveType, dwVertexTypeDesc, dwFlags);
		if (FAILED(hr))
		{
			DPF(1, _T("CDevice::BeginEnd6() - Begin() failed.\n"));
			return SetLastError(hr);
		}
       
		// Vertex
		for (i = 0; i < dwVertexCount; i++, p = (LPVOID)((BYTE*)p + dwVertexSize))
		{
			hr = m_pDevice3->Vertex(p);
			if (FAILED(hr))
			{
				m_pDevice3->End(0);
				DPF(1, _T("CDevice::BeginEnd6() - Vertex() failed.\n"));
				return SetLastError(hr);
			}
        }

        // End
        hr = m_pDevice3->End(0);
		if (FAILED(hr))
		{
			DPF(1, _T("CDevice::BeginEnd6() - End() failed.\n"));
			return SetLastError(hr);
		}
    }
    // DX6 BeginIndexed / Index / End
    else
    {
		// Begin
		hr = m_pDevice3->BeginIndexed(d3dptPrimitiveType, dwVertexTypeDesc, lpvVertices, dwVertexCount, dwFlags);
		if (FAILED(hr))
		{
			DPF(1, _T("CDevice::BeginEnd6() - BeginIndexed() failed.\n"));
			return SetLastError(hr);
		}

		// Index
        for (DWORD i = 0; i < dwIndexCount; i++)
        {
            hr = m_pDevice3->Index(lpwIndices[i]);
			if (FAILED(hr))
			{
				m_pDevice3->End(0);
				DPF(1, _T("CDevice::BeginEnd6() - Index() failed.\n"));
				return SetLastError(hr);
			}
        }

        // End
        hr = m_pDevice3->End(0);
		if (FAILED(hr))
		{
			DPF(1, _T("CDevice::BeginEnd6() - End() failed.\n"));
			return SetLastError(hr);
		}
	}

    return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawPrimitive6(D3DPRIMITIVETYPE dptPrimitiveType,
								DWORD dwVertexTypeDesc,LPVOID lpvVertices,
								DWORD dwVertexCount,DWORD dwFlags)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::DrawPrimitive6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->DrawPrimitive(dptPrimitiveType,dwVertexTypeDesc,lpvVertices,dwVertexCount,dwFlags));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawIndexedPrimitive6(D3DPRIMITIVETYPE d3dptPrimitiveType,DWORD dwVertexTypeDesc,
									   LPVOID lpvVertices,DWORD dwVertexCount,LPWORD lpwIndices,
									   DWORD dwIndexCount,DWORD dwFlags)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::DrawIndexedPrimitive6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->DrawIndexedPrimitive(d3dptPrimitiveType,
											dwVertexTypeDesc,
											lpvVertices,
											dwVertexCount,
											lpwIndices,
											dwIndexCount,
											dwFlags));
}

// --------------------------------------------------------------------------------------


bool CDevice::DrawPrimitiveUserMemory6(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc,
				LPVOID lpVertices, DWORD  dwVertexCount, LPVOID lpIndices, DWORD  dwIndexCount, 
				DWORD  dwFlags, bool bIs32BitIndices)
{
    DWORD   dwWho = dwFlags & (CD3D_SRC | CD3D_REF);
    DWORD   dwProcessedFlags = 0;
    bool    fResult = true;


    // DX7 DrawPrimitive
    if (NULL == lpIndices)
    {
        if (FAILED(DrawPrimitive6(d3dptPrimitiveType, dwVertexTypeDesc, lpVertices, dwVertexCount, dwProcessedFlags)))
			fResult = false;
    }
    // DX7 DrawIndexedPrimitive
    else
    {
        if (FAILED(DrawIndexedPrimitive6(d3dptPrimitiveType, dwVertexTypeDesc, lpVertices, dwVertexCount, (LPWORD)lpIndices, dwIndexCount, dwProcessedFlags)))
			fResult = false;
    }
    return fResult;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawPrimitiveVB6(D3DPRIMITIVETYPE d3dptPrimitiveType,LPDIRECT3DVERTEXBUFFER lpd3dVertexBuffer,
								  DWORD dwStartVertex,DWORD dwNumVertices,DWORD dwFlags)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::DrawPrimitiveVB6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->DrawPrimitiveVB(d3dptPrimitiveType,lpd3dVertexBuffer,dwStartVertex,dwNumVertices,dwFlags));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawIndexedPrimitiveVB6(D3DPRIMITIVETYPE d3dptPrimitiveType,
										 LPDIRECT3DVERTEXBUFFER lpd3dVertexBuffer,DWORD dwStartVertex,
										 DWORD dwNumVertices,LPWORD lpwIndices,DWORD dwIndexCount,DWORD dwFlags)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::DrawIndexedPrimitiveVB6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->DrawIndexedPrimitiveVB(	d3dptPrimitiveType,
												lpd3dVertexBuffer,
												lpwIndices,
												dwIndexCount,
												dwFlags));
}

// --------------------------------------------------------------------------------------

bool CDevice::DrawPrimitiveVertexBuffer6(D3DPRIMITIVETYPE d3dptPrimitiveType, CVertexBuffer *pBuffer, 
							DWORD dwStartVertex, DWORD dwNumVertices, LPWORD lpwIndices, DWORD dwIndexCount, 
							DWORD dwFlags, DWORD dwUsage, DWORD dwPool, bool bIs32BitIndices)
{
	DWORD   dwProcessedFlags = dwFlags;
	bool    fResult = true;

	// DX6 DrawPrimitive
	if (NULL == lpwIndices)
    {
        if (FAILED(DrawPrimitiveVB6(d3dptPrimitiveType,pBuffer->GetVertexBufferPointer6(),dwStartVertex,dwNumVertices,dwProcessedFlags)))
			fResult = false;
    }
    // DX6 DrawIndexedPrimitive
    else
    {
		if (FAILED(DrawIndexedPrimitiveVB6(d3dptPrimitiveType, pBuffer->GetVertexBufferPointer6(),dwStartVertex,dwNumVertices,lpwIndices, dwIndexCount, dwProcessedFlags)))
			fResult = false;
    }
    return fResult;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawPrimitiveStrided6(D3DPRIMITIVETYPE dptPrimitiveType,DWORD dwVertexTypeDesc,
									   LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray,DWORD dwVertexCount,
									   DWORD dwFlags)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::DrawPrimitiveStrided6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->DrawPrimitiveStrided(dptPrimitiveType,
											dwVertexTypeDesc,
											lpVertexArray,
											dwVertexCount,
											dwFlags));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawIndexedPrimitiveStrided6(D3DPRIMITIVETYPE dptPrimitiveType,DWORD dwVertexTypeDesc,
											  LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray,DWORD dwVertexCount,
											  LPWORD lpwIndices,DWORD dwIndexCount,DWORD dwFlags)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::DrawIndexedPrimitiveStrided6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->DrawIndexedPrimitiveStrided(	dptPrimitiveType,
													dwVertexTypeDesc,
													lpVertexArray,
													dwVertexCount,
													lpwIndices,
													dwIndexCount,
													dwFlags));
}

// --------------------------------------------------------------------------------------

bool CDevice::DrawPrimitiveStridedData6(D3DPRIMITIVETYPE d3dptPrimitiveType,
							DWORD  dwVertexTypeDesc, LPDRAWPRIMITIVESTRIDEDDATA lpVertexArray,
							DWORD  dwVertexCount, LPWORD lpwIndices, DWORD  dwIndexCount,
							DWORD  dwFlags, DWORD  dwUsage, DWORD  dwPool, bool bIs32BitIndices)
{
	DWORD   dwProcessedFlags = dwFlags;
	bool    fResult = true;

	// DX6 DrawPrimitive
	if (NULL == lpwIndices)
    {
        if (FAILED(DrawPrimitiveStrided6(d3dptPrimitiveType,dwVertexTypeDesc,(LPD3DDRAWPRIMITIVESTRIDEDDATA)lpVertexArray,dwVertexCount,dwProcessedFlags)))
			fResult = false;
    }
    // DX6 DrawIndexedPrimitive
    else
    {
		if (FAILED(DrawIndexedPrimitiveStrided6(d3dptPrimitiveType,dwVertexTypeDesc,(LPD3DDRAWPRIMITIVESTRIDEDDATA)lpVertexArray,dwVertexCount,lpwIndices, dwIndexCount, dwProcessedFlags)))
			fResult = false;

    }
    return fResult;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetClipStatus6(LPCLIPSTATUS pClipStatus)
{
    D3DCLIPSTATUS   ClipStatus;

    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::SetClipStatus6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    if (pClipStatus == NULL)
	{
		DPF(1, _T("CDevice::SetClipStatus6() - Invalid Params.\n"));
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

    return SetLastError(m_pDevice3->SetClipStatus(&ClipStatus));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetClipStatus6(LPCLIPSTATUS pClipStatus)
{
	D3DCLIPSTATUS   ClipStatus;
    HRESULT			hr;

    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::GetClipStatus6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    if (pClipStatus == NULL)
	{
		DPF(1, _T("CDevice::GetClipStatus6() - Invalid Params.\n"));
		return SetLastError(E_FAIL);
	}

    hr = m_pDevice3->GetClipStatus(&ClipStatus);

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

HRESULT CDevice::SetTexture6(int nIndex, CBaseTexture *pTexture)
{	
	HRESULT hr;
	LPDIRECT3DTEXTURE2  pTex2 = NULL;

    if (NULL != pTexture)
    {
		hr = pTexture->GetD3DTexture6()->QueryInterface(IID_IDirect3DTexture2,(PVOID *) &pTex2);
		if(FAILED(hr))
			goto Exit;
    }

    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::SetTexture6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    hr = m_pDevice3->SetTexture(nIndex, pTex2);

Exit:

	RELEASE(pTex2);
	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetTexture6(int nIndex, CBaseTexture **ppTexture)
{
	return E_NOTIMPL;
}
	
// --------------------------------------------------------------------------------------

HRESULT CDevice::GetTextureStageState6(DWORD dwStage,D3DTEXTURESTAGESTATETYPE dwState,LPDWORD lpdwValue)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::GetTextureStageState6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->GetTextureStageState(dwStage,dwState,lpdwValue));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetTextureStageState6(DWORD dwStage,D3DTEXTURESTAGESTATETYPE dwState,DWORD dwValue)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::SetTextureStageState6() - Invalid Device.\n"));
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
                DebugOut.Write(1, "CDirect3D: SetTextureStageState6() invalid MagFilter value %d\n", dwValue);
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
                DebugOut.Write(1, "CDirect3D: SetTextureStageState6() invalid MinFilter value %d\n", dwValue);
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
                DebugOut.Write(1, "CDirect3D: SetTextureStageState6() invalid MipFilter value %d\n", dwValue);
                return E_FAIL;
        }
    }   

	return SetLastError(m_pDevice3->SetTextureStageState(dwStage,dwState,dwValue));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::ValidateDevice6(LPDWORD lpdwPasses)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::ValidateDevice6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->ValidateDevice(lpdwPasses));
}

// --------------------------------------------------------------------------------------
	

HRESULT CDevice::Present6(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion)
{
    if (m_pPrimary == NULL || m_pBackBuffers == NULL || m_pBackBuffers[0] == NULL)
	{
		DPF(1, _T("CDevice::Present6() - Invalid params.\n"));
		return SetLastError(E_FAIL);
	}

    if(m_bFullscreen)
		return SetLastError(m_pPrimary->GetD3DSurface6()->Flip(NULL,DDFLIP_WAIT));
	else
	{
       	POINT pt = {0, 0};
		RECT Dest;
		HWND hWnd = hDestWindowOverride == 0 ? m_pPresParams->hDeviceWindow : hDestWindowOverride;
		ADAPTER *pAdapter = GetAdapter();
		
		if(!ClientToScreen(hWnd, &pt))
		{
			DPF(1, _T("CDevice::Present6() - ClientToScreen failed.\n"));
			return SetLastError(E_FAIL);
		}

        Dest.left = pDestRect->left + pt.x - pAdapter->rMonitorRect.left;
		Dest.right = pDestRect->right + pt.x - pAdapter->rMonitorRect.left;
		Dest.top = pDestRect->top + pt.y - pAdapter->rMonitorRect.top;
		Dest.bottom = pDestRect->bottom + pt.y - pAdapter->rMonitorRect.top;
		
		return SetLastError(m_pPrimary->GetD3DSurface6()->Blt(&Dest, m_pBackBuffers[0]->GetD3DSurface6(), pSourceRect, DDBLT_WAIT, NULL));
	}
}


// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateAdditionalSwapChain6(PRESENT_PARAMETERS* pParams, CSwapChain** ppSwapChain)
{
	HRESULT			hr;
	DDSURFACEDESC2  Desc;
	LPDIRECTDRAWSURFACE4 pSwap6;

	if(pParams == NULL || ppSwapChain == NULL || m_pDevice3 == NULL || pParams->uBackBufferCount != 1)
	{
		DPF(1, _T("CDevice::CreateAdditionalSwapChain6() - invalid params.\n"));
		return SetLastError(E_FAIL);
	}

    ZeroMemory(&Desc,sizeof(DDSURFACEDESC2));
	Desc.dwSize = sizeof(DDSURFACEDESC2);
    Desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    Desc.dwWidth = pParams->uWidth;
    Desc.dwHeight = pParams->uHeight;

    memcpy(&Desc.ddpfPixelFormat, &(pParams->BackBufferFormat.ddpfPixelFormat), sizeof(DDPIXELFORMAT));
	
    Desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;

    hr = m_pD3D->m_pDDraw4->CreateSurface(&Desc, &pSwap6,NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateAdditionalSwapChain6() - CreateSurface failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		goto Error;
	}

	*ppSwapChain = new CSwapChain();
	if((*ppSwapChain) == NULL)
	{
		DPF(1, _T("CDevice::CreateAdditionalSwapChain6() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	if(!(*ppSwapChain)->Init(&pSwap6, pParams, this))
	{
		DPF(2, _T("CDevice::CreateAdditionalSwapChain6() - CSurface() init failed.\n"));
		hr = E_FAIL;
		goto Error;
	}

	RELEASE(pSwap6);

    return S_OK;

Error:

	RELEASE(pSwap6);
	RELEASE(*ppSwapChain);

	return hr;

}

// --------------------------------------------------------------------------------------

HRESULT CDevice::UpdateTexture6(CBaseTexture* pSourceTexture,
								CBaseTexture* pDestinationTexture)
{
	HRESULT hr;
	LPDIRECT3DTEXTURE2 pD3DSourceTexture = NULL;
	LPDIRECT3DTEXTURE2 pD3DDestinationTexture = NULL;

    if (m_pDevice3 == NULL || pSourceTexture == NULL || pDestinationTexture == NULL)
	{
		DPF(1, _T("CDevice::UpdateTexture6() - Invalid params.\n"));
		return SetLastError(E_FAIL);
	}

	hr = pSourceTexture->GetD3DTexture6()->QueryInterface(IID_IDirect3DTexture2,(PVOID *) &pD3DSourceTexture);
	if(FAILED(hr))
		goto Exit;
    hr = pDestinationTexture->GetD3DTexture6()->QueryInterface(IID_IDirect3DTexture2,(PVOID *) &pD3DDestinationTexture);
	if(FAILED(hr))
		goto Exit;

	if( pD3DSourceTexture == NULL || pD3DDestinationTexture == NULL )
	{
		DPF(1, _T("CDevice::UpdateTexture6() - Invalid CTexture.\n"));
		hr = E_FAIL;
		goto Exit;
	}

    hr = pD3DDestinationTexture->Load(pD3DSourceTexture);

Exit:
	RELEASE(pD3DSourceTexture);
	RELEASE(pD3DDestinationTexture);

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CopyRects6(CSurface* pSourceSurface,RECT* pSourceRectsArray,
							UINT cRects,CSurface* pDestinationSurface,POINT* pDestPointsArray)
{
	HRESULT hr;
	HRESULT hrReturn = S_OK;
	int i = 0;

    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::CopyRects6() - Invalid Device.\n"));
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

		hr = pDestinationSurface->GetD3DSurface6()->Blt(pDest,pSourceSurface->GetD3DSurface6(),
												   pSrc, DDBLT_WAIT, 0);

		if(FAILED(hr))
		{
			DPF(1, _T("CDevice::CopyRects6() - Blt failed. hr=%s(%x).\n"), 
				m_pD3D->HResultToString(hr), hr);
			hrReturn = hr;
		}
	}
	while(++i < cRects);

	return hrReturn;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetDepthStencilSurface6(CSurface** ppZStencilSurface)
{
	if((ppZStencilSurface != NULL) && (m_pZBuffer != NULL))
    {
        m_pZBuffer->AddRef();
		*ppZStencilSurface = m_pZBuffer;
    }

    return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateDepthStencilSurface6(UINT Width,UINT Height,FORMAT Format,
											MULTISAMPLE_TYPE MultiSample,
											CSurface** ppSurface)
{
	HRESULT			hr;
	DDSURFACEDESC2  Desc;
	DWORD			dwPool = POOL_DEFAULT;
	LPDIRECTDRAWSURFACE4 pZBuffer4 = NULL;
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
		DPF(1, _T("CDevice::CreateDepthStencilSurface6() - invalid POOL flag.\n"));
		hr = E_FAIL;
		goto Exit;
	}

    Desc.ddsCaps.dwCaps |= DDSCAPS_ZBUFFER;

    hr = m_pD3D->m_pDDraw4->CreateSurface(&Desc, &pZBuffer4,NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface6() - CreateSurface failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	pSurf = new CSurface();
	if(pSurf == NULL)
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface6() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	if(!pSurf->Init(pZBuffer4, this, NULL))
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface6() - CSurface() init failed.\n"));
		hr = E_FAIL;
		goto Exit;
	}

Exit:

	RELEASE(pZBuffer4);
	
	if(FAILED(hr))
	{
		RELEASE(pSurf);
	}
	else
		*ppSurface = pSurf;

    return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateRenderTarget6(UINT Width,UINT Height,FORMAT Format,
									 MULTISAMPLE_TYPE MultiSample,BOOL Lockable,
									 CSurface** ppSurface)
{
    LPDIRECTDRAWSURFACE4 pSurf = NULL;
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
	hr = m_pD3D->m_pDDraw4->CreateSurface(&ddDesc, &pSurf, NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateRenderTarget6() - CreateSurface failed. hr=%s(%x).\n"), m_pD3D->HResultToString(hr), hr);
		return SetLastError(hr);
	}

    *ppSurface = new CSurface();
	if((*ppSurface) == NULL)
	{
		DPF(1, _T("CDevice::CreateRenderTarget6() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}
    	
	if(!((*ppSurface)->Init(pSurf, this, NULL)))
	{
		DPF(1, _T("CDevice::CreateRenderTarget6() - ppSurface->Init() failed.\n"));
	    RELEASE(*ppSurface);
        hr = E_FAIL;
    }

Exit:
    RELEASE(pSurf);
    return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateImageSurface6(UINT Width,UINT Height,FORMAT Format,
									 CSurface** ppSurface)
{
    LPDIRECTDRAWSURFACE4 pSurf = NULL;
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
	hr = m_pD3D->m_pDDraw4->CreateSurface(&ddDesc, &pSurf, NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateImageSurface6() - CreateSurface failed. hr=%s(%x).\n"), m_pD3D->HResultToString(hr), hr);
		return SetLastError(hr);
	}

    *ppSurface = new CSurface();
	if((*ppSurface) == NULL)
	{
		DPF(1, _T("CDevice::CreateImageSurface6() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}
    	
	if(!((*ppSurface)->Init(pSurf, this, NULL)))
	{
		DPF(1, _T("CDevice::CreateImageSurface6() - ppSurface->Init() failed.\n"));
	    RELEASE(*ppSurface);
        hr = E_FAIL;
    }

Exit:
    RELEASE(pSurf);
    return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetSamplerState6(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD Value)
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

HRESULT CDevice::GetSamplerState6(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD *Value)
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

HRESULT CDevice::ComputeSphereVisibility6(LPD3DVECTOR lpCenters,LPD3DVALUE lpRadii,DWORD dwNumSpheres,
										  DWORD dwFlags,LPDWORD lpdwReturnValues)
{
    if (m_pDevice3 == NULL)
	{
		DPF(1, _T("CDevice::ComputeSphereVisibility6() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

    return SetLastError(m_pDevice3->ComputeSphereVisibility(lpCenters,lpRadii,dwNumSpheres,dwFlags,lpdwReturnValues));
}

// --------------------------------------------------------------------------------------

UINT CDevice::GetAvailableTextureMem6(void)
{
	DDSCAPS2      ddsCaps2; 
	DWORD         dwTotal; 
	DWORD         dwFree;
	HRESULT       hr; 
 
	// Initialize the structure.
	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
 
	ddsCaps2.dwCaps = DDSCAPS_TEXTURE; 
	
	hr = m_pD3D->m_pDDraw4->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree); 
	if (FAILED(hr))
		return 0;
	else
		return dwFree;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::EvictManagedResources6()
{
    return SetLastError(m_pD3D->m_pD3D3->EvictManagedTextures());
}
