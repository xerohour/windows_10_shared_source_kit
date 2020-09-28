// ======================================================================================
//
//  CDevice8.cpp
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================

#include "DevicePCH.h" 

#include <d3dx8.h>
#include "CD3DX.h"
#include "CDevice.h"
#include "CTextures.h"
#include "CSurface.h"
#include "CTextures.h"
#include "CSwapChain.h"
#include "CBuffers.h"


// --------------------------------------------------------------------------------------

HRESULT CDevice::Initialize8(PRESENT_PARAMETERS *pParams)
{
    DWORD dwBeh = 0;
	HRESULT hr = S_OK;
	D3DDEVTYPE DevType;
	D3DPRESENT_PARAMETERS Params;
	TCHAR szFormat[64] = {0};

	if(m_pD3D == NULL || !GetAdapter() || !pParams)
	{
		DPF(1, _T("CDevice::Initialize8() - Invalid ptrs.\n"));
		return E_FAIL;
	}

	// Convert to D3DDEVTYPE
	if(m_dwDevType == DEVICETYPE_HAL)
		DevType = D3DDEVTYPE_HAL;
	else if(m_dwDevType == DEVICETYPE_REF)
		DevType = D3DDEVTYPE_REF;
	else if(m_dwDevType == DEVICETYPE_SW)
		DevType = D3DDEVTYPE_SW;
	else
	{
		DPF(1, _T("CDevice::Initialize8() - unknown device type.\n"));
		return E_FAIL;
	}

	// Convert to D3D Behaviors
	if(m_dwBehaviors & DEVICECREATE_MIXED)
	{
		dwBeh |= D3DCREATE_MIXED_VERTEXPROCESSING;
        if(m_dwBehaviors & DEVICECREATE_HWVP)
            m_bHWVP = true;
	}
	else if(m_dwBehaviors & DEVICECREATE_PURE)
	{
		dwBeh |= D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING;
		m_bPure = true;
        m_bHWVP = true;
	}
    else if(m_dwBehaviors & DEVICECREATE_HWVP)
	{
		dwBeh |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
		m_bHWVP = true;
	}
    else if(m_dwBehaviors & DEVICECREATE_SWVP)
	{
		dwBeh |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
    
    if(m_dwBehaviors & DEVICECREATE_MULTITHREADED)
		dwBeh |= D3DCREATE_MULTITHREADED;
    if(m_dwBehaviors & DEVICECREATE_FPU_PRESERVE)
		dwBeh |= D3DCREATE_FPU_PRESERVE;
    if(m_dwBehaviors & DEVICECREATE_DISABLE_DRIVER_MANAGEMENT)
		dwBeh |= D3DCREATE_DISABLE_DRIVER_MANAGEMENT;
	
    // D3DCREATE_SHOW_DP2ERROR (MS internal flag)
    dwBeh |= 0x40000000L;

	// Decode any internal flags
	if(pParams->SwapEffect == FMWK_DEFAULT)
		pParams->SwapEffect = (pParams->MultiSample == MULTISAMPLE_NONE) ? SWAPEFFECT_COPY : SWAPEFFECT_DISCARD;
	if(pParams->dwFlags == FMWK_DEFAULT)
        pParams->dwFlags = (pParams->MultiSample == D3DMULTISAMPLE_NONE) ? D3DPRESENTFLAG_LOCKABLE_BACKBUFFER : 0;
	if(pParams->uFullscreenPresentInterval == FMWK_DEFAULT)
        pParams->uFullscreenPresentInterval = PRESENT_INTERVAL_DEFAULT;
	
	// Copy the present params locally
	m_pPresParams = new PRESENT_PARAMETERS;
	if(NULL == m_pPresParams)
	{
    	DPF(1, _T("CDevice::Initialize8() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}
    memcpy(m_pPresParams, pParams, sizeof(PRESENT_PARAMETERS));

	// Generate the present param
	Params.BackBufferWidth = pParams->uWidth;
    Params.BackBufferHeight = pParams->uHeight;
    Params.BackBufferFormat = pParams->BackBufferFormat.d3dfFormat;
    Params.BackBufferCount = pParams->uBackBufferCount;
    Params.MultiSampleType = (D3DMULTISAMPLE_TYPE)pParams->MultiSample;
	Params.SwapEffect = (D3DSWAPEFFECT)pParams->SwapEffect;
    Params.hDeviceWindow = pParams->hDeviceWindow;
    Params.EnableAutoDepthStencil = pParams->bAutoDepthStencil;
    Params.AutoDepthStencilFormat = pParams->AutoDepthStencilFormat.d3dfFormat;
	Params.Flags = pParams->dwFlags;
	Params.FullScreen_RefreshRateInHz = pParams->uFullscreenRefresh;
    Params.FullScreen_PresentationInterval = pParams->uFullscreenPresentInterval;
    Params.Windowed = (UINT)pParams->bWindowed;

	// Validate the display mode
	if(!m_pD3D->IsDisplayModeSupported(GetAdapter(), GetAdapter()->DesktopMode.dwWidth, GetAdapter()->DesktopMode.dwHeight,(FMT)GetAdapter()->DesktopMode.Format.d3dfFormat))
	{
		FmtToString(FORMAT_TEXTURE, &GetAdapter()->DesktopMode.Format, szFormat);
		DPF(1, _T("Display Mode (%dx%dx%s) not supported.\n"),GetAdapter()->DesktopMode.dwWidth, GetAdapter()->DesktopMode.dwHeight, szFormat);
		hr = E_UNSUPPORTED;
		goto Exit;
	}

	// Validate the back buffer format
	hr = m_pD3D->CheckDeviceType(m_uAdapter, m_dwDevType, pParams->DesktopFormat,
									  pParams->BackBufferFormat, pParams->bWindowed);
    if(FAILED(hr))
	{
		// 
		DPF(1, _T("Backbuffer format not supported. CheckDeviceType() returned %s(%x).\n"), 
			m_pD3D->HResultToString(hr), hr);
		hr = E_UNSUPPORTED;
		goto Exit;		
	}

	if(Params.EnableAutoDepthStencil)
	{
		// Validate the DepthStencil vs the RenderTarget
		hr = m_pD3D->CheckDepthStencilMatch(m_uAdapter, m_dwDevType, pParams->DesktopFormat,
										  pParams->BackBufferFormat, pParams->AutoDepthStencilFormat);
		if(FAILED(hr))
		{
			DPF(1, _T("DepthStencil format not supported.  CheckDepthStencilMatch() returned %s(%x).\n"), 
				m_pD3D->HResultToString(hr), hr);
			hr = E_UNSUPPORTED;
			goto Exit;		
		}
	}

	// Create the device
	hr = m_pD3D->m_pD3D8->CreateDevice(m_uAdapter, DevType, m_hFocusWindow, dwBeh, &Params, &m_pDevice8);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice:Initialize8() - CreateDevice failed. hr=%s(%x).\n"), 
			m_pD3D->HResultToString(hr), hr);

		// Make out of memory a valid failure
		if(hr == D3DERR_OUTOFVIDEOMEMORY)
			hr = E_UNSUPPORTED;
		goto Exit;
	}
	
	// Update the caps to be device specfic
	if(!m_pD3D->DetectCaps(this))
	{
		DPF(1, _T("CD3D::Initialize8() - DetectCaps failed.\n"));
		goto Exit;
	}

	// Wrap the backbuffers, ZBuffers, etc.
	if(FAILED(WrapDevice8(pParams)))
	{
		DPF(1, _T("CD3D::Initialize8() - WrapDevice failed. hr=%s(%x)\n"),
			m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}
	
Exit:
	if(FAILED(hr))
	{
		ReleaseD3D();
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::WrapDevice8(PRESENT_PARAMETERS *pParams)
{
	HRESULT hr = 0;

	// Set VP renderstate for Mixed mode
    if(m_dwBehaviors & DEVICECREATE_MIXED)
	{
		if(m_dwBehaviors & DEVICECREATE_HWVP)
			SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, FALSE);
		else
            SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, TRUE);
	}
	
	if ( pParams->dwInternalFlags & PPINTERNAL_CACHEBACKBUFFER )
	{
       	// Wrap all the BackBuffers
        m_pBackBuffers = new CSurface*[pParams->uBackBufferCount];
	    if(m_pBackBuffers == NULL)
	    {
		    DPF(1, _T("CDevice::Initialize8() - Out of memory.\n"));
			hr = E_FAIL;
		    goto Exit;
	    }
	    memset(m_pBackBuffers, 0, sizeof(CSurface*) * pParams->uBackBufferCount);

        for(int i=0; i < pParams->uBackBufferCount; i++)
		{
			// Alloc the surface wrapper
			hr = GetBackBuffer( i, BACKBUFFER_TYPE_MONO, &m_pBackBuffers[i] );
			if( FAILED( hr ) )
			{
				DPF(1, _T("CDevice::Initialize8() - GetBackBuffer(%d) failed. hr=%s(%x).\n"),
					i, m_pD3D->HResultToString(hr), hr);
				goto Exit;
			}

			// Increment the buffer counter
			m_nBackBuffers++;
		}
	}

    if (pParams->bAutoDepthStencil)
    {
        hr = GetDepthStencilSurface(&m_pZBuffer);

	    if(FAILED(hr))
	    {
		    DPF(1, _T("CDevice::Initialize8() - GetDepthStencilSurface() failed. hr=%s(%x).\n"),
			    m_pD3D->HResultToString(hr), hr);
			goto Exit;
	    }
    }

Exit:
	if(FAILED(hr))
	{
		ReleaseD3D();
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::ValidateMode8(DEVICEMODE *pMode)
{
	TCHAR szBuffer[64] = {0};
	PADAPTER pAdapter = m_pD3D->GetAdapter(pMode->nAdapter);

	if(((FMT)pMode->pDispMode->Format.d3dfFormat != FMT_A2R10G10B10 &&
		(FMT)pMode->pDispMode->Format.d3dfFormat != FMT_A2B10G10R10_XR_BIAS &&
	    (FMT)pMode->pDispMode->Format.d3dfFormat != FMT_X8R8G8B8 &&
	    (FMT)pMode->pDispMode->Format.d3dfFormat != FMT_R5G6B5) ||
		((FMT)pMode->pDispMode->Format.d3dfFormat == FMT_A2R10G10B10 || (FMT)pMode->pDispMode->Format.d3dfFormat == FMT_A2B10G10R10_XR_BIAS)||
	   (FAILED(m_pD3D->CheckDeviceType(pMode->nAdapter, pMode->dwDevType, pMode->pDispMode->Format, pMode->pDispMode->Format, TRUE)) &&
	    FAILED(m_pD3D->CheckDeviceType(pMode->nAdapter, pMode->dwDevType, pMode->pDispMode->Format, pMode->pDispMode->Format, FALSE))))
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


	if( (pMode->dwBehaviors & DEVICECREATE_PURE) && 
		!(m_Desc.dwDevCaps & D3DDEVCAPS_PUREDEVICE) )
	{
        DPF(1, _T("Pure device not supported by this adapter.\n"));
		return E_UNSUPPORTED;
	}

	if( ((pMode->dwBehaviors & DEVICECREATE_HWVP_OR_PURE) || (pMode->dwBehaviors & DEVICECREATE_MIXED) || (pMode->dwBehaviors & DEVICECREATE_HWVP)) &&
		!(m_Desc.dwDevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) )
	{
		DPF(1, _T("Hardware vertex processing not supported by this adapter.\n"));
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

void CDevice::ReleaseWrappers8()
{
    // Release the backbuffers
    for(int i=0; i<m_nBackBuffers; i++)
	{
		RELEASE(m_pBackBuffers[i]);
	}
    SAFEDELETEA(m_pBackBuffers);
	m_nBackBuffers = 0;

	RELEASE(m_pZBuffer);
	RELEASE(m_pPrimary);
    RELEASE(m_pD3DXFont);
}

// --------------------------------------------------------------------------------------

void CDevice::ReleaseD3D8()
{
	ReleaseWrappers8();

	if(m_pDevice8)
	{
		UINT uRef = m_pDevice8->Release();
		m_pDevice8 = NULL;
		if(uRef)
		{
			DPF(1, _T("CDevice::ReleaseD3D8() - m_pDevice8 ref count is %d after final Release().\n"), uRef);

			ASSERT(g_pFramework != NULL);
            if(g_pFramework->m_bStressMode)
			{
				ASSERT(uRef == 0);
			}
		}
		
	}

    SAFEDELETE(m_pPresParams);

	// Reset the state
	m_bFullscreen = false;
}


// --------------------------------------------------------------------------------------
bool CDevice::IsDeviceLost8()
{
	return FAILED(TestCooperativeLevel());
}


// --------------------------------------------------------------------------------------
bool CDevice::IsDeviceReadyForReset8()
{
	HRESULT hDeviceState = TestCooperativeLevel();

	if
	(
		SUCCEEDED( hDeviceState ) ||
		hDeviceState == D3DERR_DEVICENOTRESET
	)
	{
		return true;
	};

	return false;
}


// --------------------------------------------------------------------------------------
bool CDevice::HandleDeviceLost8()
{
	HRESULT hr;

	hr = TestCooperativeLevel();
	
	if(SUCCEEDED(hr))
	{
		return true;
	}
	else
	{
		// Device is no longer lost, but needs to be reset
		if(hr == D3DERR_DEVICENOTRESET)
		{
			// Release all wrappers we have for the Device
		    ReleaseWrappers8();

			if(FAILED(Reset(m_pPresParams)))
			{
				DPF(1, _T("CDevice::HandleDeviceLost8() - Reset failed. hr=%s(%x).\n"),
					m_pD3D->HResultToString(hr), hr);
				return false;
			}
			else
			{
				// Restore the wrappers
				WrapDevice(m_pPresParams);
				return true;
			}
		}
		else
		{
			DPF(2, _T("CDevice::HandleDeviceLost8() - TestCooperativeLevel() failed with %s(%x).\n"),
				m_pD3D->HResultToString(hr), hr);
		}
	}
	return false;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::D3DXFontOnLostDevice8()
{
	if(m_pD3DXFont)
		return m_pD3DXFont->OnLostDevice();

	return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::D3DXFontOnResetDevice8()
{
	if(m_pD3DXFont)
		return m_pD3DXFont->OnResetDevice();

	return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::Resize8(UINT uWidth, UINT uHeight)
{
	HRESULT hr;

	// Release all wrappers we have for the Device
    ReleaseWrappers8();

	m_pPresParams->uWidth = uWidth;
	m_pPresParams->uHeight = uHeight;

	hr = Reset(m_pPresParams);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::HandleDeviceLost9() - Reset failed. hr=%s(%x).\n"),
				m_pD3D->HResultToString(hr), hr);
		return hr;
	}
	else
	{
		// Restore the wrappers
		WrapDevice(m_pPresParams);
		return hr;
	}
}

// --------------------------------------------------------------------------------------

bool CDevice::TextureFormatSupported8(FORMAT *pFmt)
{
	if(pFmt == NULL)
	{
		DPF(1, _T("CDevice::TextureFormatSupported8() - Invalid params.\n"));
		return false;
	}

	FORMAT *pTempFmt = m_pTextureFormat;
	while(pTempFmt)
	{
		if(pTempFmt->d3dfFormat == pFmt->d3dfFormat)
            return true;
		pTempFmt = pTempFmt->pNext;
	}

   	return false;
}

// --------------------------------------------------------------------------------------

bool CDevice::ZBufferFormatSupported8(FORMAT *pFmt)
{
	if(pFmt == NULL)
	{
		DPF(1, _T("CDevice::ZBufferFormatSupported8() - Invalid params.\n"));
		return false;
	}

	FORMAT *pTempFmt = m_pZBufferFormat;
	while(pTempFmt)
	{
		if(pTempFmt->d3dfFormat == pFmt->d3dfFormat)
			return true;
		pTempFmt = pTempFmt->pNext;
	}

   	return false;
}

// --------------------------------------------------------------------------------------

bool CDevice::CubeFormatSupported8(FORMAT *pFmt)
{
	if(pFmt == NULL)
	{
		DPF(1, _T("CDevice::CubeFormatSupported8() - Invalid params.\n"));
		return false;
	}

	FORMAT *pTempFmt = m_pCubeFormat;
	while(pTempFmt)
	{
		if(pTempFmt->d3dfFormat == pFmt->d3dfFormat)
            return true;
		pTempFmt = pTempFmt->pNext;
	}

   	return false;
}

// --------------------------------------------------------------------------------------

bool CDevice::VolumeFormatSupported8(FORMAT *pFmt)
{
	if(pFmt == NULL)
	{
		DPF(1, _T("CDevice::VolumeFormatSupported8() - Invalid params.\n"));
		return false;
	}

	FORMAT *pTempFmt = m_pVolumeFormat;
	while(pTempFmt)
	{
		if(pTempFmt->d3dfFormat == pFmt->d3dfFormat)
            return true;
		pTempFmt = pTempFmt->pNext;
	}

   	return false;
}

// --------------------------------------------------------------------------------------

bool CDevice::Draw2DText8(DWORD dwColor, TCHAR *szString, int nLen, RECT *pRect, DWORD dwFormat)
{
	HRESULT hr = S_OK;
	bool bRes = true;
	
	if(!szString || !pRect)
	{
		DPF(1, _T("CDevice::DrawText8() - invalid params.\n"));
		return false;
	}
	
	// Create the font if needed
	if(m_pD3DXFont == NULL)
	{
		hr = ::D3DXCreateFontIndirect8And90(m_pDevice8, & m_LogFont, &m_pD3DXFont);
		if(FAILED(hr))
		{
			DPF(1, _T("CDevice::DrawText8() - D3DXCreateFont failed. hr=%s(%x).\n"),
						   m_pD3D->HResultToString(hr), hr); 
			return false;
		}
	}

	if(0 == m_pD3DXFont->DrawText(szString, nLen, pRect, dwFormat, (D3DCOLOR)dwColor))
	{
		DPF(1, _T("CDevice::DrawText8() - D3DXDrawText failed.\n")); 
		return false;
	}
    
	return true;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::QueryInterface8(REFIID riid, void** ppvObj)
{
    return SetLastError(m_pDevice8->QueryInterface(riid, ppvObj));
}

// --------------------------------------------------------------------------------------

bool CDevice::IsDeviceEqual8(PVOID pDevice)
{
	if(m_pDevice8 == (LPDIRECT3DDEVICE8) pDevice)
		return true;
	else
		return false;
}

// --------------------------------------------------------------------------------------

ULONG CDevice::AddRef8(void)
{
    return m_pDevice8->AddRef( );
}

// --------------------------------------------------------------------------------------

ULONG CDevice::Release8(void)
{
    return m_pDevice8->Release( );
}

// --------------------------------------------------------------------------------------

HRESULT	CDevice::TestCooperativeLevel8(void)
{
    return SetLastError(m_pDevice8->TestCooperativeLevel( ));
}

// --------------------------------------------------------------------------------------

UINT CDevice::GetAvailableTextureMem8(void)
{
    return m_pDevice8->GetAvailableTextureMem( );
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::EvictManagedResources8()
{
    return SetLastError(m_pDevice8->ResourceManagerDiscardBytes(0));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::ResourceManagerDiscardBytes8(DWORD Bytes)
{
    return SetLastError(m_pDevice8->ResourceManagerDiscardBytes(Bytes));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetDirect3D8(IDirect3D8** ppD3D8)
{
    return SetLastError(m_pDevice8->GetDirect3D(ppD3D8));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetDeviceCaps8(CAPS* pCaps)
{
	return SetLastError( m_pDevice8->GetDeviceCaps((D3DCAPS8*)pCaps) );
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetDisplayMode8(DISPLAYMODE* pMode)
{
	D3DDISPLAYMODE Mode;

	HRESULT hr=m_pDevice8->GetDisplayMode(pMode != NULL ? &Mode : NULL);
	
	if( pMode != NULL )
	{
		pMode->dwWidth  = Mode.Width;
		pMode->dwHeight = Mode.Height;
		pMode->dwRefreshRate = Mode.RefreshRate;
		pMode->Format.d3dfFormat = Mode.Format;
		d3df2ddpf(Mode.Format,&pMode->Format.ddpfPixelFormat);
		pMode->dwBPP = pMode->Format.ddpfPixelFormat.dwRGBBitCount;
	}

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetCreationParameters8(DEVICE_CREATION_PARAMETERS *pParameters)
{
    return SetLastError(m_pDevice8->GetCreationParameters(pParameters));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetCursorProperties8(UINT XHotSpot,UINT YHotSpot,
									  CSurface* pCursorBitmap)
{
    return SetLastError(m_pDevice8->SetCursorProperties(XHotSpot,YHotSpot,pCursorBitmap->GetD3DSurface8()));
}

// --------------------------------------------------------------------------------------

void CDevice::SetCursorPosition8(UINT XScreenSpace,UINT YScreenSpace,DWORD Flags)
{
    m_pDevice8->SetCursorPosition(XScreenSpace,YScreenSpace,Flags);
}

// --------------------------------------------------------------------------------------

BOOL CDevice::ShowCursor8(BOOL bShow)
{
    return m_pDevice8->ShowCursor(bShow);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::Reset8(PRESENT_PARAMETERS* pPresentationParameters)
{

	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));
    
	if( pPresentationParameters )
	{
		d3dpp.BackBufferWidth					= pPresentationParameters->uWidth;
		d3dpp.BackBufferHeight					= pPresentationParameters->uHeight;
		d3dpp.BackBufferFormat					= pPresentationParameters->BackBufferFormat.d3dfFormat;
		d3dpp.BackBufferCount					= pPresentationParameters->uBackBufferCount;
		d3dpp.MultiSampleType					= (D3DMULTISAMPLE_TYPE)pPresentationParameters->MultiSample;
		d3dpp.SwapEffect						= (D3DSWAPEFFECT)pPresentationParameters->SwapEffect;
		d3dpp.hDeviceWindow						= pPresentationParameters->hDeviceWindow;
		d3dpp.Windowed							= pPresentationParameters->bWindowed;
		d3dpp.EnableAutoDepthStencil			= pPresentationParameters->bAutoDepthStencil;
		d3dpp.AutoDepthStencilFormat			= pPresentationParameters->AutoDepthStencilFormat.d3dfFormat;
		d3dpp.Flags								= pPresentationParameters->dwFlags;
		d3dpp.FullScreen_RefreshRateInHz		= pPresentationParameters->uFullscreenRefresh;
		d3dpp.FullScreen_PresentationInterval	= pPresentationParameters->uFullscreenPresentInterval;
	}

    return SetLastError(m_pDevice8->Reset(pPresentationParameters == NULL ? NULL : &d3dpp));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::Present8(RECT* pSourceRect, RECT* pDestRect,
						  HWND hDestWindowOverride, RGNDATA* pDirtyRegion)
{
    return SetLastError(m_pDevice8->Present(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetBackBuffer8(UINT BackBuffer,DWORD dwType, CSurface** ppBackBuffer)
{
	LPDIRECT3DSURFACE8 pSurf = NULL;
	CSurface* pBackBuffer = NULL;

	// Alloc the surface wrapper
	pBackBuffer = new CSurface();
	if(pBackBuffer == NULL)
	{
		DPF(1, _T("CDevice::GetBackBuffer8() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}

	// Get the real backbuffer ptr
    HRESULT hr = m_pDevice8->GetBackBuffer(BackBuffer, (D3DBACKBUFFER_TYPE)dwType, ppBackBuffer == 0L ? 0L : &pSurf);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::GetBackBuffer8() - GetBackBuffer(%d) failed. hr=%s(%x).\n"),
			BackBuffer, m_pD3D->HResultToString(hr), hr);
		goto Error;
	}

	// Initialize the surface with the actual surface ptr
	if(pBackBuffer->Init(pSurf, this, NULL))
	{
		// we succeeded!
		if( ppBackBuffer )
			*ppBackBuffer = pBackBuffer;
	}
	else
	{
		DPF(1, _T("CDevice::GetBackBuffer8() - CSurface::Init failed for backbuffer #%d.\n"), BackBuffer);
		hr = E_FAIL;
		goto Error;
	}

	// Release our BackBuffer
	RELEASE(pSurf);

	return SetLastError(S_OK);

Error:
	RELEASE(pBackBuffer);
	RELEASE(pSurf);
    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetRasterStatus8(RASTER_STATUS* pRasterStatus)
{
    return SetLastError(m_pDevice8->GetRasterStatus(pRasterStatus));
}

// --------------------------------------------------------------------------------------

void CDevice::SetGammaRamp8(DWORD Flags,GAMMARAMP* pRamp)
{
    m_pDevice8->SetGammaRamp(Flags,pRamp);
}

// --------------------------------------------------------------------------------------

void CDevice::GetGammaRamp8(GAMMARAMP* pRamp)
{
    m_pDevice8->GetGammaRamp(pRamp);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateTexture8(UINT Width,UINT Height,UINT Levels,DWORD Usage,
								FORMAT Format,DWORD Pool,CTexture** ppTexture)
{
    HRESULT hr;
    CTexture *pTexture = NULL, *pTexOut = NULL;
	LPDIRECT3DTEXTURE8 pD3DTexture = NULL;
	D3DPOOL d3dpool;

	//
	// Create target texture
	//

	pTexture = new CTexture;
	if(pTexture == NULL)
	{
		DPF(2, _T("CDevice::CreateTexture8() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}
	
    if (FMWK_DEFAULT == Pool || POOL_DEFAULT_NOBACKINGSTORE == Pool)
		d3dpool = D3DPOOL_DEFAULT;
	else 
		d3dpool = (D3DPOOL)Pool;

	// If framework default don't use a usage, else remove any Dx9 usage bits
	if (FMWK_DEFAULT == Usage)
		Usage = 0;
	else
		Usage &= ~(USAGE_AUTOGENMIPMAP | USAGE_DMAP | USAGE_QUERY_LEGACYBUMPMAP | USAGE_QUERY_SRGBREAD | USAGE_QUERY_FILTER | USAGE_QUERY_SRGBWRITE);

	hr = m_pDevice8->CreateTexture(Width,Height,Levels,Usage,Format.d3dfFormat,d3dpool,ppTexture == 0L ? 0 : &pD3DTexture);
	if(FAILED(hr))
	{
		DPF(2, _T("CDevice::CreateTexture8() - CDevice::CreateTexture() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	if(!pTexture->Init(pD3DTexture, this))
	{
		DPF(2, _T("CDevice::CreateTexture8() - CTexture::Init() failed.\n") );
		hr = E_FAIL;
		goto Exit;
	}
	
	RELEASE(pD3DTexture);

	pTexOut = pTexture;

    //
	// Create sysmem texture
	//

    if ((Pool != POOL_SCRATCH) &&
        (Pool != POOL_MANAGED) &&        
        (Pool != POOL_DEFAULT_NOBACKINGSTORE) &&        
        (Usage & USAGE_DEPTHSTENCIL) == 0)
	{
		pTexture = new CTexture;
		if(pTexture == NULL)
		{
			DPF(2, _T("CDevice::CreateTexture8() - Out of memory.\n"));
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		hr = m_pDevice8->CreateTexture(Width,Height,Levels,0,Format.d3dfFormat,D3DPOOL_SYSTEMMEM,&pD3DTexture);
		if(FAILED(hr))
		{
			DPF(2, _T("CDevice::CreateTexture8() - CDevice::CreateTexture8() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
		
		if(!pTexture->Init(pD3DTexture, this))
		{
			DPF(2, _T("CDevice::CreateTexture8() - CTexture::Init() failed.\n") );
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
		*ppTexture = pTexOut;

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateVolumeTexture8(UINT Width,UINT Height,UINT Depth,UINT Levels,
									  DWORD Usage,FORMAT Format,DWORD Pool,
									  CVolTexture** ppVolumeTexture)
{
	HRESULT hr;
    CVolTexture *pTexture = NULL, *pTexOut = NULL;
	LPDIRECT3DVOLUMETEXTURE8 pD3DTexture = NULL;
	
	//
	// Create the target texture
	//

	pTexture = new CVolTexture;
	if(pTexture == NULL)
	{
		DPF(1, _T("CDevice::CreateVolumeTexture() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}

    if (FMWK_DEFAULT == Pool)
        Pool = POOL_DEFAULT;

	// If framework default don't use a usage, else remove any Dx9 usage bits
	if (FMWK_DEFAULT == Usage)
		Usage = 0;
	else
		Usage &= ~(USAGE_AUTOGENMIPMAP | USAGE_DMAP | USAGE_QUERY_LEGACYBUMPMAP | USAGE_QUERY_SRGBREAD | USAGE_QUERY_FILTER | USAGE_QUERY_SRGBWRITE);

	pD3DTexture = NULL;
	hr = m_pDevice8->CreateVolumeTexture(Width,Height,Depth,Levels,Usage,Format.d3dfFormat,(D3DPOOL)Pool,ppVolumeTexture==0L?0:&pD3DTexture);
	if( FAILED( hr ) )
	{
		DPF(1, _T("CDevice::CreateVolumeTexture() - CDevice::CreateVolumeTexture() failed.\n") );
		goto Exit;
	}

	if(!pTexture->Init(pD3DTexture, this))
	{
		DPF(1, _T("CDevice::CreateVolumeTexture() - CVolTexture::Init() failed.\n") );
		hr = E_FAIL;
		goto Exit;
	}

	RELEASE(pD3DTexture);

	pTexOut = pTexture;

	//
	// Create the sysmem texture
	//
	
	if ((Pool != POOL_SCRATCH) && (Usage & USAGE_DEPTHSTENCIL) == 0)
	{
		pTexture = new CVolTexture;
		if(pTexture == NULL)
		{
			DPF(1, _T("CDevice::CreateVolumeTexture() - Out of memory.\n"));
			return SetLastError(E_FAIL);
		}

		pD3DTexture = NULL;
		hr = m_pDevice8->CreateVolumeTexture(Width,Height,Depth,Levels,0,Format.d3dfFormat, D3DPOOL_SYSTEMMEM, &pD3DTexture);
		if( FAILED( hr ) )
		{
			DPF(1, _T("CDevice::CreateVolumeTexture() - CDevice::CreateVolumeTexture() failed.\n") );
			goto Exit;
		}

		if(!pTexture->Init(pD3DTexture, this))
		{
			DPF(1, _T("CDevice::CreateVolumeTexture() - CVolTexture::Init() failed.\n") );
			hr = E_FAIL;
			goto Exit;
		}

		RELEASE(pD3DTexture);

		pTexOut->SetSysTexture(pTexture);
	}

Exit:	
	if(FAILED(hr))
	{
        RELEASE(pTexOut);
		RELEASE(pD3DTexture);
		RELEASE(pTexture);
	}
	else
		*ppVolumeTexture = pTexOut;

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateCubeTexture8(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,
									DWORD Pool,CCubeTexture** ppCubeTexture)
{
	HRESULT hr;
    CCubeTexture* pTexture = NULL, *pTexOut = NULL;
	LPDIRECT3DCUBETEXTURE8 pD3DTexture = NULL;
	
	// 
	// Create the target texture
	//

	pTexture = new CCubeTexture;
	if(pTexture == NULL)
	{
		DPF(1, _T("CDevice::CreateCubeTexture() - Out of memory.\n"));
		return SetLastError(E_OUTOFMEMORY);
	}
	
    if(Pool == FMWK_DEFAULT)
		Pool = POOL_DEFAULT;

	// If framework default don't use a usage, else remove any Dx9 usage bits
	if (FMWK_DEFAULT == Usage)
		Usage = 0;
	else
		Usage &= ~(USAGE_AUTOGENMIPMAP | USAGE_DMAP | USAGE_QUERY_LEGACYBUMPMAP | USAGE_QUERY_SRGBREAD | USAGE_QUERY_FILTER | USAGE_QUERY_SRGBWRITE);

	hr = m_pDevice8->CreateCubeTexture(EdgeLength,Levels,Usage,Format.d3dfFormat,(D3DPOOL)Pool,ppCubeTexture==0L?0:&pD3DTexture);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateCubeTexture() - CDevice::CreateCubeTexture() failed.\n") );
		goto Exit;
	}

	if(!pTexture->Init(pD3DTexture, this))
	{
		DPF(1, _T("CDevice::CreateCubeTexture() - CCubeTexture::Init() failed.\n") );
		hr = E_FAIL;
		goto Exit;
	}
	RELEASE(pD3DTexture);

	pTexOut = pTexture;

	//
	// Create the sysmem copy
	//
	if ((Pool != POOL_SCRATCH) && (Usage & USAGE_DEPTHSTENCIL) == 0)
	{
		pTexture = new CCubeTexture;
		if(pTexture == NULL)
		{
			DPF(1, _T("CDevice::CreateCubeTexture() - Out of memory.\n"));
			return SetLastError(E_OUTOFMEMORY);
		}
		
		hr = m_pDevice8->CreateCubeTexture(EdgeLength,Levels,0,Format.d3dfFormat, D3DPOOL_SYSTEMMEM, &pD3DTexture);
		if(FAILED(hr))
		{
			DPF(1, _T("CDevice::CreateCubeTexture() - CDevice::CreateCubeTexture() failed.\n") );
			goto Exit;
		}

		if(!pTexture->Init(pD3DTexture, this))
		{
			DPF(1, _T("CDevice::CreateCubeTexture() - CCubeTexture::Init() failed.\n") );
			hr = E_FAIL;
			goto Exit;
		}
		RELEASE(pD3DTexture);

		pTexOut->SetSysTexture(pTexture);
	}

Exit:
		
	if(FAILED(hr))
	{
		RELEASE(pTexOut);
		RELEASE(pD3DTexture);
		RELEASE(pTexture);
	}
	else
        *ppCubeTexture = pTexOut;

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateVertexBuffer8(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB)
{
	LPDIRECT3DVERTEXBUFFER8		pD3DVB = NULL;
	CVertexBuffer*				pVB = NULL;
	HRESULT						hr;
	D3DPOOL						d3dPool = (D3DPOOL)Pool;

	if (Pool == FMWK_DEFAULT)
	{
		if ( ( ((FVF & D3DFVF_POSITION_MASK) != D3DFVF_XYZRHW) && ( (m_dwBehaviors & DEVICECREATE_SWVP) || ((m_dwBehaviors & DEVICECREATE_MIXED) && !(m_dwBehaviors & DEVICECREATE_HWVP))) ) ||
			m_dwDevType == DEVICETYPE_SW)
		{
			d3dPool = D3DPOOL_SYSTEMMEM;
		}
		else
			d3dPool = D3DPOOL_DEFAULT;
	}

	if (Usage & FMWK_DEFAULT)
	{
		if ( (m_dwBehaviors & DEVICECREATE_MIXED) && !(m_dwBehaviors & DEVICECREATE_HWVP) )
			Usage |= USAGE_SOFTWAREPROCESSING;
		Usage &= ~FMWK_DEFAULT;
	}

	pVB = new CVertexBuffer();
	if (NULL == pVB)
	{
		DPF(1, _T("CDevice::CreateVertexBuffer() - Out of memory.\n"));
		return E_FAIL;
	}

	hr = m_pDevice8->CreateVertexBuffer(Length, Usage, FVF, d3dPool, ppVB == 0L ? 0L : &pD3DVB);
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

HRESULT CDevice::CreateIndexBuffer8(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB)
{
	LPDIRECT3DINDEXBUFFER8		pD3DIB = NULL;
	CIndexBuffer*				pIB = NULL;
	HRESULT						hr;
	D3DPOOL						d3dPool = (D3DPOOL)Pool;

	if (Pool == FMWK_DEFAULT)
	{
		if ( (m_dwBehaviors & DEVICECREATE_SWVP) || ((m_dwBehaviors & DEVICECREATE_MIXED) && !(m_dwBehaviors & DEVICECREATE_HWVP)) || (m_dwDevType == DEVICETYPE_SW) )
		{
			d3dPool = D3DPOOL_SYSTEMMEM;
		}
		else
			d3dPool = D3DPOOL_DEFAULT;
	}

	if (Usage & FMWK_DEFAULT)
	{
		if ( (m_dwBehaviors & DEVICECREATE_MIXED) && !(m_dwBehaviors & DEVICECREATE_HWVP) )
			Usage |= USAGE_SOFTWAREPROCESSING;
		Usage &= ~FMWK_DEFAULT;
	}

	pIB = new CIndexBuffer();
	if (NULL == pIB)
	{
		DPF(1, _T("CDevice::CreateIndexBuffer() - Out of memory.\n"));
		return E_FAIL;
	}

	hr = m_pDevice8->CreateIndexBuffer(Length, Usage, (D3DFORMAT)Format, d3dPool, ppIB==0L?0L:&pD3DIB);
	if ( FAILED(hr)||0L==ppIB)
	{
		DPF(1, _T("CDevice::CreateIndexBuffer() - CDevice::CreateIndexBuffer() failed.\n") );
		RELEASE( pIB );
		return SetLastError(hr);
	}

	if (pIB->Initialize(pD3DIB, this))
	{
		if (ppIB != NULL)
		{
			*ppIB = pIB;
		}
	}
	else
	{
		DPF(1, _T("CDevice::CreateIndexBuffer() - CDevice::CreateIndexBuffer() failed.\n") );
		RELEASE( pD3DIB );
		RELEASE( pIB );
		return SetLastError(E_FAIL);
	}
	
	RELEASE( pD3DIB );

	return hr;
}


// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateRenderTarget8(UINT Width,UINT Height,FORMAT Format,
									 MULTISAMPLE_TYPE MultiSample,BOOL Lockable,
									 CSurface** ppSurface)
{
	LPDIRECT3DSURFACE8 pD3DSurface = NULL;
	CSurface* pSurface = new CSurface;
	if( pSurface == NULL )
	{
		DPF(1, _T("CDevice::CreateRenderTarget8() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}

	HRESULT hr = m_pDevice8->CreateRenderTarget(Width,Height,Format.d3dfFormat,(D3DMULTISAMPLE_TYPE)MultiSample,Lockable,ppSurface==0L?0L:&pD3DSurface);
	if( FAILED( hr ) || 0L == ppSurface )
	{
		DPF(1, _T("CDevice::CreateRenderTarget8() - CreateRenderTarget() failed.\n"));
		RELEASE(pSurface);
		return SetLastError(hr);
	}

	if( !pSurface->Init(pD3DSurface, this, NULL) )
	{
		DPF(1, _T("CDevice::CreateRenderTarget8() - CSurface::Init() failed.\n"));
		RELEASE( pD3DSurface );
		RELEASE(pSurface);
		return SetLastError(E_FAIL);
	}

	if( ppSurface )
		*ppSurface = pSurface;

	RELEASE(pD3DSurface);

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateDepthStencilSurface8(UINT Width,UINT Height,FORMAT Format,
											MULTISAMPLE_TYPE MultiSample,
											CSurface** ppSurface)
{
	LPDIRECT3DSURFACE8 pD3DSurface = NULL;
	CSurface* pSurface = new CSurface;
	if( pSurface == NULL )
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface8() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}
	
	HRESULT hr = m_pDevice8->CreateDepthStencilSurface(Width,Height,Format.d3dfFormat,(D3DMULTISAMPLE_TYPE)MultiSample,ppSurface==0L?0L:&pD3DSurface);
	if( FAILED( hr ) )
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface8() - CreateDepthStencilSurface() failed.\n"));
		RELEASE( pSurface );
		return SetLastError(hr);
	}

	if( !pSurface->Init(pD3DSurface, this, NULL) )
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface8() - CSurface::Init() failed.\n"));
		RELEASE( pSurface );
		RELEASE( pD3DSurface );
		return SetLastError(E_FAIL);
	}

	if( ppSurface )
		*ppSurface = pSurface;

	RELEASE(pD3DSurface);

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateImageSurface8(UINT Width,UINT Height,FORMAT Format,
									 CSurface** ppSurface)
{
	LPDIRECT3DSURFACE8 pD3DSurface = NULL;
	CSurface* pSurface = new CSurface;

	if( pSurface == NULL )
	{
		DPF(1, _T("CDevice::CreateImageSurface8() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}

	HRESULT hr = m_pDevice8->CreateImageSurface(Width,Height,Format.d3dfFormat,ppSurface==0L?0L:&pD3DSurface);
	if( FAILED( hr ) || 0L==ppSurface )
	{
		DPF(1, _T("CDevice::CreateImageSurface8() - CreateImageSurface() failed.\n"));
		RELEASE( pSurface );
		return SetLastError(hr);
	}

	if( !pSurface->Init(pD3DSurface, this, NULL) )
	{
		DPF(1, _T("CDevice::CreateImageSurface8() - CSurface::Init() failed.\n"));
		RELEASE( pSurface );
		RELEASE( pD3DSurface );
		return SetLastError(E_FAIL);
	}

	if( ppSurface )
		*ppSurface = pSurface;

	RELEASE(pD3DSurface);

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetSamplerState8(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD Value)
{
    switch (Type)
    {
        case SAMP_ADDRESSU:      // D3DTEXTUREADDRESS for U coordinate
            return SetTextureStageState(Sampler, D3DTSS_ADDRESSU, Value);

        case SAMP_ADDRESSV:      // D3DTEXTUREADDRESS for V coordinate
            return SetTextureStageState(Sampler, D3DTSS_ADDRESSV, Value);

        case SAMP_ADDRESSW:      // D3DTEXTUREADDRESS for W coordinate
            return SetTextureStageState(Sampler, D3DTSS_ADDRESSW, Value);

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

HRESULT CDevice::GetSamplerState8(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD *Value)
{
    switch (Type)
    {
        case SAMP_ADDRESSU:      // D3DTEXTUREADDRESS for U coordinate
            return GetTextureStageState(Sampler, D3DTSS_ADDRESSU, Value);
        
        case SAMP_ADDRESSV:      // D3DTEXTUREADDRESS for V coordinate
            return GetTextureStageState(Sampler, D3DTSS_ADDRESSV, Value);

        case SAMP_ADDRESSW:      // D3DTEXTUREADDRESS for W coordinate
            return GetTextureStageState(Sampler, D3DTSS_ADDRESSW, Value);

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

HRESULT CDevice::SetSoftwareVertexProcessing8(BOOL bSoftware)
{
    return SetLastError( m_pDevice8->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, *(DWORD*)&bSoftware ) );
}

// --------------------------------------------------------------------------------------

BOOL CDevice::GetSoftwareVertexProcessing8()
{
    DWORD dwSWVertProc = TRUE;
	HRESULT hr;

    hr = GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, &dwSWVertProc );
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice:GetSoftwareVertexProcessing8() - GetRenderState failed. hr=%s(%x).\n"), 
			m_pD3D->HResultToString(hr), hr);
		return TRUE;
	}
    
	return dwSWVertProc;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CopyRects8(CSurface* pSourceSurface,RECT* pSourceRectsArray,
							UINT cRects,CSurface* pDestinationSurface,POINT* pDestPointsArray)
{
    return SetLastError(m_pDevice8->CopyRects(pSourceSurface==0L?0L:pSourceSurface->GetD3DSurface8(),
											 pSourceRectsArray,
											 cRects,
											 pDestinationSurface==0L?0L:pDestinationSurface->GetD3DSurface8(),
											 pDestPointsArray));
}

// --------------------------------------------------------------------------------------
HRESULT CDevice::UpdateSurface9ToCopyRects8(CSurface* pSourceSurface,RECT* pSourceRects,CSurface* pDestinationSurface,POINT* pDestPoint)
{
	return CopyRects8( pSourceSurface, pSourceRects, 1, pDestinationSurface, pDestPoint );
}

HRESULT CDevice::UpdateTexture8(CBaseTexture* pSourceTexture,
								CBaseTexture* pDestinationTexture)
{
	IDirect3DBaseTexture8* pD3DSourceTexture = 0L;
	IDirect3DBaseTexture8* pD3DDestinationTexture = 0L;

	if( 0L != pSourceTexture )
		pD3DSourceTexture = pSourceTexture->GetD3DTexture8();
	if( 0L != pDestinationTexture )
		pD3DDestinationTexture = pDestinationTexture->GetD3DTexture8();

    return SetLastError(m_pDevice8->UpdateTexture(pD3DSourceTexture,pD3DDestinationTexture));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetFrontBuffer8(CSurface* pDestSurface)
{
    return SetLastError(m_pDevice8->GetFrontBuffer(pDestSurface ? pDestSurface->GetD3DSurface8() : NULL));
}

// ------------------------------------------------------------------------------

HRESULT CDevice::SetRenderTarget8(CSurface *pNew, CSurface *pNewDepth)
{
    return SetLastError(
		m_pDevice8->SetRenderTarget( pNew == NULL ? NULL : pNew->GetD3DSurface8(),
									 pNewDepth == NULL ? NULL : pNewDepth->GetD3DSurface8()
									)
						);
}

// ------------------------------------------------------------------------------

HRESULT CDevice::GetRenderTarget8(CSurface** ppRenderTarget)
{

	LPDIRECT3DSURFACE8 pD3DSurface = NULL;
	CSurface* pSurface = new CSurface;
    if (0L == pSurface) {
		DPF(1, _T("CDevice::GetRenderTarget8() - Out of Memory.\n"));
		return SetLastError(E_FAIL);
	}

	HRESULT hr = m_pDevice8->GetRenderTarget(ppRenderTarget == 0L ? 0L: &pD3DSurface);
	if (FAILED(hr)) {
		DPF(1, _T("CDevice::GetRenderTarget8() - GetRenderTarget() failed.\n"));
		RELEASE(pSurface);
		return SetLastError(hr);
	}

	if (!pSurface->Init(pD3DSurface, this, NULL)) {
		DPF(1, _T("CDevice::GetRenderTarget8() - CSurface::Init() failed.\n"));
		RELEASE( pSurface );
		RELEASE( pD3DSurface );
		return SetLastError(E_FAIL);
	}

	if (ppRenderTarget)
		*ppRenderTarget = pSurface;

	RELEASE(pD3DSurface);
    return SetLastError(hr);
}

// ------------------------------------------------------------------------------

HRESULT CDevice::GetDepthStencilSurface8(CSurface** ppZStencilSurface)
{
	LPDIRECT3DSURFACE8 pD3DSurface = NULL;
	CSurface* pSurface = new CSurface;

	HRESULT hr = m_pDevice8->GetDepthStencilSurface( ppZStencilSurface == 0L ? 0L : &pD3DSurface );
	if( FAILED( hr ) )
	{
		if ((hr != D3DERR_NOTFOUND) || (!bSuppressGetDepthStencilDBGOutput))
			DPF(1, _T("CDevice::GetDepthStencilSurface8() - GetDepthStencilSurface() failed.\n"));
		RELEASE( pSurface );
		return SetLastError(hr);
	}

	if( !pSurface->Init( pD3DSurface, this, NULL ) )
	{
		DPF(1, _T("CDevice::GetDepthStencilSurface8() - CSurface::Init() failed.\n"));
		RELEASE( pSurface );
		RELEASE( pD3DSurface );
		return SetLastError(E_FAIL);
	}

	if( ppZStencilSurface != NULL )
		*ppZStencilSurface = pSurface;

	RELEASE(pD3DSurface);

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::BeginScene8(void)
{
    return SetLastError(m_pDevice8->BeginScene( ));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::EndScene8(void)
{
    return SetLastError(m_pDevice8->EndScene( ));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::Clear8(DWORD Count,D3DRECT* pRects,DWORD dwFlags,D3DCOLOR Color,float Z,DWORD Stencil)
{
    if(dwFlags == FMWK_DEFAULT)
	{
		dwFlags = 0;
        dwFlags |= CLEARF_TARGET;
		dwFlags |= m_pPresParams->bAutoDepthStencil ? CLEARF_ZBUFFER : 0;
	}

    return SetLastError(m_pDevice8->Clear(Count,pRects,dwFlags,Color,Z,Stencil));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetTransform8(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix)
{
	// Remap pre DX8 transform types
	switch (State)
	{
	case 1:     // WORLD
		State = D3DTS_WORLDMATRIX(0);
		break;
	case 4:     // WORLD1
		State = D3DTS_WORLDMATRIX(1);
		break;
	case 5:     // WORLD2
		State = D3DTS_WORLDMATRIX(2);
		break;
	case 6:     // WORLD3
		State = D3DTS_WORLDMATRIX(3);
		break;
	}

    return SetLastError(m_pDevice8->SetTransform(State,pMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetTransform8(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix)
{
    return SetLastError(m_pDevice8->GetTransform(State,pMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::MultiplyTransform8(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix)
{
    return SetLastError(m_pDevice8->MultiplyTransform(State,pMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetViewport8(LPVIEWPORT pViewport)
{
	// convert from private struct
	D3DVIEWPORT8    Viewport;

	if (pViewport)
	{
		Viewport.X            = pViewport->X;
		Viewport.Y            = pViewport->Y;
		Viewport.Width        = pViewport->Width;
		Viewport.Height       = pViewport->Height;
		Viewport.MinZ         = pViewport->MinZ;
		Viewport.MaxZ         = pViewport->MaxZ;
	}

	return SetLastError(m_pDevice8->SetViewport(pViewport==0L?0L:&Viewport));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetViewport8(LPVIEWPORT pViewport)
{
	D3DVIEWPORT8    Viewport;
	ZeroMemory( &Viewport, sizeof(Viewport) );

	// Call GetViewport even if pViewport == NULL (for invalid param testing)
	HRESULT			hr = SetLastError(m_pDevice8->GetViewport(pViewport==0L?0L:&Viewport));

	if( pViewport == NULL )
		return hr;

	// convert to private struct
	pViewport->X		= Viewport.X;
	pViewport->Y		= Viewport.Y;
	pViewport->Width	= Viewport.Width;
	pViewport->Height	= Viewport.Height;
	pViewport->MinZ		= Viewport.MinZ;
	pViewport->MaxZ		= Viewport.MaxZ;

   return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetMaterial8(MATERIAL* pMaterial)
{
    return SetLastError(m_pDevice8->SetMaterial((LPD3DMATERIAL8)pMaterial));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetMaterial8(MATERIAL* pMaterial)
{
    return SetLastError(m_pDevice8->GetMaterial((LPD3DMATERIAL8)pMaterial));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetLight8(DWORD Index,LIGHT* pLight)
{
    return SetLastError(m_pDevice8->SetLight(Index,(D3DLIGHT8*)pLight));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetLight8(DWORD Index,LIGHT* pLight)
{
    return SetLastError(m_pDevice8->GetLight(Index,(D3DLIGHT8*)pLight));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::LightEnable8(DWORD Index, BOOL bEnable)
{
    return SetLastError(m_pDevice8->LightEnable(Index,bEnable));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetLightEnable8(DWORD Index,BOOL* pEnable)
{
    return SetLastError(m_pDevice8->GetLightEnable(Index,pEnable));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetClipPlane8(DWORD Index,float* pPlane)
{
    return SetLastError(m_pDevice8->SetClipPlane(Index,pPlane));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetClipPlane8(DWORD Index,float* pPlane)
{
    return SetLastError(m_pDevice8->GetClipPlane(Index,pPlane));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetRenderState8(D3DRENDERSTATETYPE State,DWORD Value)
{
	switch(State)
    {
        case    3: // D3DRENDERSTATE_TEXTUREADDRESS
                SetTextureStageState(0,D3DTSS_ADDRESSU,Value);
				SetTextureStageState(0,D3DTSS_ADDRESSV,Value);
				return GetLastError();
        case    4: // D3DRENDERSTATE_TEXTUREPERSPECTIVE
                return D3D_OK; // retired renderstate
        case    5: // D3DRENDERSTATE_WRAPU
                {
                    if (TRUE == Value)
                        m_dwWrapUV |= D3DWRAP_U;
					else
                        m_dwWrapUV &= ~D3DWRAP_U;

                    return SetRenderState(D3DRS_WRAP0, m_dwWrapUV);
                }
        case    6: // D3DRENDERSTATE_WRAPV
                {
                    if (TRUE == Value)
                        m_dwWrapUV |= D3DWRAP_V;
					else
                        m_dwWrapUV &= ~D3DWRAP_V;

                    return SetRenderState(D3DRS_WRAP0, m_dwWrapUV);
                }
        case    17: // D3DRENDERSTATE_TEXTUREMAG
                switch (Value)
                {
                    case    1: // D3DFILTER_NEAREST
                            return SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)D3DTEXF_POINT);
                    case    2: // D3DFILTER_LINEAR
                            return SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)D3DTEXF_LINEAR);
                }
                return E_FAIL;
        case    18: // D3DRENDERSTATE_TEXTUREMIN
                switch(Value)
                {
                    case    1: // D3DFILTER_NEAREST
                            SetTextureStageState(0,D3DTSS_MIPFILTER, (DWORD)D3DTEXF_NONE);
                            return SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)D3DTEXF_POINT);
                    case    2: // D3DFILTER_LINEAR
                            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)D3DTEXF_NONE);
                            return SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)D3DTEXF_LINEAR);
                    case    4: // D3DFILTER_MIPLINEAR
                            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)D3DTEXF_POINT);
                            return SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)D3DTEXF_LINEAR);
                    case    3: //D3DFILTER_MIPNEAREST
                            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)D3DTEXF_POINT);
                            return SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)D3DTEXF_POINT);
                    case    5: //D3DFILTER_LINEARMIPNEAREST
                            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)D3DTEXF_LINEAR);
                            return SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)D3DTEXF_POINT);
                    case    6: // D3DFILTER_LINEARMIPLINEAR
                            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)D3DTEXF_LINEAR);
                            return SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)D3DTEXF_LINEAR);
                }
                return E_FAIL;

        case    21: // D3DRENDERSTATE_TEXTUREMAPBLEND:
                {
                    HRESULT hrResult;
                    DWORD   dwValid;

                    switch(Value)
                    {
                        case    7: // D3DTBLEND_COPY
                        case    1: // D3DTBLEND_DECAL
                                {
                                    SetTextureStageState(0,D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);

                                    hrResult = ValidateDevice(&dwValid);

                                    if (D3D_OK == hrResult)
                                        return D3D_OK;

                                    SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLOROP,(DWORD)D3DTOP_SELECTARG2);

                                    hrResult = ValidateDevice(&dwValid);

                                    return hrResult;
                                }
                                break;
                        case    2: // D3DTBLEND_MODULATE
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
                                    SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_MODULATE);

                                    hrResult = ValidateDevice(&dwValid);

                                    return hrResult;
                                }
                                break;
                        case    3: // D3DTBLEND_DECALALPHA
                                {
                                    SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_BLENDTEXTUREALPHA);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_ALPHAOP,  (DWORD)D3DTOP_SELECTARG2);

                                    hrResult = ValidateDevice(&dwValid);

                                    return hrResult;
                                }
                                break;
                        case    4: // D3DTBLEND_MODULATEALPHA
                                {
                                    SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_MODULATE);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_ALPHAOP,  (DWORD)D3DTOP_MODULATE);

                                    hrResult = ValidateDevice(&dwValid);

                                    if (D3D_OK == hrResult)
                                        return D3D_OK;

                                    SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_MODULATE);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_ALPHAOP,  (DWORD)D3DTOP_MODULATE);

                                    hrResult = ValidateDevice(&dwValid);

                                    return hrResult;
                                }
                                break;
                        case    8: // D3DTBLEND_ADD
                                {
                                    SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_ADD);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_ALPHAOP,  (DWORD)D3DTOP_SELECTARG2);

                                    hrResult = ValidateDevice(&dwValid);

                                    if (D3D_OK == hrResult)
                                        return D3D_OK;

                                    SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_TEXTURE);
                                    SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_CURRENT);
                                    SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_ADD);

                                    hrResult = ValidateDevice(&dwValid);

                                    return hrResult;
                                }
                                break;
                    }
                }
                break;
        case    44: // D3DRENDERSTATE_TEXTUREADDRESSU
                return SetTextureStageState(0,D3DTSS_ADDRESSU,Value);
        case    45: // D3DRENDERSTATE_TEXTUREADDRESSV
                return SetTextureStageState(0,D3DTSS_ADDRESSV,Value);
        case    46: // D3DRENDERSTATE_MIPMAPLODBIAS
                return SetTextureStageState(0,D3DTSS_MIPMAPLODBIAS,Value);
        case    49: // D3DRENDERSTATE_ANISOTROPY
                return SetTextureStageState(0,D3DTSS_MAXANISOTROPY,Value);
        case    43: // D3DRENDERSTATE_BORDERCOLOR
                return SetTextureStageState(0,D3DTSS_BORDERCOLOR,Value);
        case    D3DRS_LIGHTING:
            m_bRPLighting = (Value == TRUE) ? true : false;
            break;
        case    D3DRS_CLIPPING:
            m_bRPClipping = (Value == TRUE) ? true : false;
            break;
	}

    return SetLastError(m_pDevice8->SetRenderState(State,Value));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetRenderState8(D3DRENDERSTATETYPE State,DWORD* pValue)
{
    return SetLastError(m_pDevice8->GetRenderState(State,pValue));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::BeginStateBlock8(void)
{
    return SetLastError(m_pDevice8->BeginStateBlock( ));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::EndStateBlock8(UINT_PTR* pToken)
{
    return SetLastError(m_pDevice8->EndStateBlock((DWORD*)pToken));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::ApplyStateBlock8(UINT_PTR Token)
{
    return SetLastError(m_pDevice8->ApplyStateBlock((DWORD)Token));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CaptureStateBlock8(UINT_PTR Token)
{
    return SetLastError(m_pDevice8->CaptureStateBlock((DWORD)Token));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DeleteStateBlock8(UINT_PTR Token)
{
    return SetLastError(m_pDevice8->DeleteStateBlock((DWORD)Token));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateStateBlock8(D3DSTATEBLOCKTYPE Type,UINT_PTR* pToken)
{
    return SetLastError(m_pDevice8->CreateStateBlock(Type,(DWORD*)pToken));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetClipStatus8(LPCLIPSTATUS pClipStatus)
{
    D3DCLIPSTATUS8  ClipStatus;
    ZeroMemory(&ClipStatus, sizeof(ClipStatus));

	if( pClipStatus )
	{
		if (pClipStatus->dwClipIntersection & CLIPSTATUS_BACK)
			ClipStatus.ClipIntersection |= D3DCS_BACK;
		if (pClipStatus->dwClipIntersection & CLIPSTATUS_BOTTOM)
			ClipStatus.ClipIntersection |= D3DCS_BOTTOM;
		if (pClipStatus->dwClipIntersection & CLIPSTATUS_FRONT)
			ClipStatus.ClipIntersection |= D3DCS_FRONT;
		if (pClipStatus->dwClipIntersection & CLIPSTATUS_LEFT)
			ClipStatus.ClipIntersection |= D3DCS_LEFT;
		if (pClipStatus->dwClipIntersection & CLIPSTATUS_RIGHT)
			ClipStatus.ClipIntersection |= D3DCS_RIGHT;
		if (pClipStatus->dwClipIntersection & CLIPSTATUS_TOP)
			ClipStatus.ClipIntersection |= D3DCS_TOP;

		if (pClipStatus->dwClipIntersection & CLIPSTATUS_PLANE0)
			ClipStatus.ClipIntersection |= D3DCS_PLANE0;
		if (pClipStatus->dwClipIntersection & CLIPSTATUS_PLANE1)
			ClipStatus.ClipIntersection |= D3DCS_PLANE1;
		if (pClipStatus->dwClipIntersection & CLIPSTATUS_PLANE2)
			ClipStatus.ClipIntersection |= D3DCS_PLANE2;
		if (pClipStatus->dwClipIntersection & CLIPSTATUS_PLANE3)
			ClipStatus.ClipIntersection |= D3DCS_PLANE3;
		if (pClipStatus->dwClipIntersection & CLIPSTATUS_PLANE4)
			ClipStatus.ClipIntersection |= D3DCS_PLANE4;
		if (pClipStatus->dwClipIntersection & CLIPSTATUS_PLANE5)
			ClipStatus.ClipIntersection |= D3DCS_PLANE5;

		if (pClipStatus->dwClipUnion & CLIPSTATUS_BACK)
			ClipStatus.ClipUnion |= D3DCS_BACK;
		if (pClipStatus->dwClipUnion & CLIPSTATUS_BOTTOM)
			ClipStatus.ClipUnion |= D3DCS_BOTTOM;
		if (pClipStatus->dwClipUnion & CLIPSTATUS_FRONT)
			ClipStatus.ClipUnion |= D3DCS_FRONT;
		if (pClipStatus->dwClipUnion & CLIPSTATUS_LEFT)
			ClipStatus.ClipUnion |= D3DCS_LEFT;
		if (pClipStatus->dwClipUnion & CLIPSTATUS_RIGHT)
			ClipStatus.ClipUnion |= D3DCS_RIGHT;
		if (pClipStatus->dwClipUnion & CLIPSTATUS_TOP)
			ClipStatus.ClipUnion |= D3DCS_TOP;

		if (pClipStatus->dwClipUnion & CLIPSTATUS_PLANE0)
			ClipStatus.ClipUnion |= D3DCS_PLANE0;
		if (pClipStatus->dwClipUnion & CLIPSTATUS_PLANE1)
			ClipStatus.ClipUnion |= D3DCS_PLANE1;
		if (pClipStatus->dwClipUnion & CLIPSTATUS_PLANE2)
			ClipStatus.ClipUnion |= D3DCS_PLANE2;
		if (pClipStatus->dwClipUnion & CLIPSTATUS_PLANE3)
			ClipStatus.ClipUnion |= D3DCS_PLANE3;
		if (pClipStatus->dwClipUnion & CLIPSTATUS_PLANE4)
			ClipStatus.ClipUnion |= D3DCS_PLANE4;
		if (pClipStatus->dwClipUnion & CLIPSTATUS_PLANE5)
			ClipStatus.ClipUnion |= D3DCS_PLANE5;
	}
    
	return SetLastError(m_pDevice8->SetClipStatus(pClipStatus==0L?0L:&ClipStatus));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetClipStatus8(LPCLIPSTATUS pClipStatus)
{
	D3DCLIPSTATUS8  ClipStatus;
    HRESULT         hr;

	hr = m_pDevice8->GetClipStatus(pClipStatus==0L?0L:&ClipStatus);

    if (SUCCEEDED(hr) && 0L != pClipStatus)
    {
        pClipStatus->dwClipIntersection = 0;
        pClipStatus->dwClipUnion = 0;

        if (ClipStatus.ClipIntersection & D3DCS_BACK)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_BACK;
        if (ClipStatus.ClipIntersection & D3DCS_BOTTOM)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_BOTTOM;
        if (ClipStatus.ClipIntersection & D3DCS_FRONT)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_FRONT;
        if (ClipStatus.ClipIntersection & D3DCS_LEFT)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_LEFT;
        if (ClipStatus.ClipIntersection & D3DCS_RIGHT)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_RIGHT;
        if (ClipStatus.ClipIntersection & D3DCS_TOP)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_TOP;

        if (ClipStatus.ClipIntersection & D3DCS_PLANE0)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_PLANE0;
        if (ClipStatus.ClipIntersection & D3DCS_PLANE1)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_PLANE1;
        if (ClipStatus.ClipIntersection & D3DCS_PLANE2)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_PLANE2;
        if (ClipStatus.ClipIntersection & D3DCS_PLANE3)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_PLANE3;
        if (ClipStatus.ClipIntersection & D3DCS_PLANE4)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_PLANE4;
        if (ClipStatus.ClipIntersection & D3DCS_PLANE5)
            pClipStatus->dwClipIntersection |= CLIPSTATUS_PLANE5;

        if (ClipStatus.ClipUnion & D3DCS_BACK)
            pClipStatus->dwClipUnion |= CLIPSTATUS_BACK;
        if (ClipStatus.ClipUnion & D3DCS_BOTTOM)
            pClipStatus->dwClipUnion |= CLIPSTATUS_BOTTOM;
        if (ClipStatus.ClipUnion & D3DCS_FRONT)
            pClipStatus->dwClipUnion |= CLIPSTATUS_FRONT;
        if (ClipStatus.ClipUnion & D3DCS_LEFT)
            pClipStatus->dwClipUnion |= CLIPSTATUS_LEFT;
        if (ClipStatus.ClipUnion & D3DCS_RIGHT)
            pClipStatus->dwClipUnion |= CLIPSTATUS_RIGHT;
        if (ClipStatus.ClipUnion & D3DCS_TOP)
            pClipStatus->dwClipUnion |= CLIPSTATUS_TOP;

        if (ClipStatus.ClipUnion & D3DCS_PLANE0)
            pClipStatus->dwClipUnion |= CLIPSTATUS_PLANE0;
        if (ClipStatus.ClipUnion & D3DCS_PLANE1)
            pClipStatus->dwClipUnion |= CLIPSTATUS_PLANE1;
        if (ClipStatus.ClipUnion & D3DCS_PLANE2)
            pClipStatus->dwClipUnion |= CLIPSTATUS_PLANE2;
        if (ClipStatus.ClipUnion & D3DCS_PLANE3)
            pClipStatus->dwClipUnion |= CLIPSTATUS_PLANE3;
        if (ClipStatus.ClipUnion & D3DCS_PLANE4)
            pClipStatus->dwClipUnion |= CLIPSTATUS_PLANE4;
        if (ClipStatus.ClipUnion & D3DCS_PLANE5)
            pClipStatus->dwClipUnion |= CLIPSTATUS_PLANE5;

		pClipStatus->minx = 0.f;
		pClipStatus->miny = 0.f;
		pClipStatus->minz = 0.f;
		pClipStatus->maxx = 0.f;
		pClipStatus->maxy = 0.f;
		pClipStatus->maxz = 0.f;
    }

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

// Note: GetTexure8 is not valid for invalid param testing where ppTexture == NULL
HRESULT CDevice::GetTexture8(int nIndex, CBaseTexture **ppTexture)
{
    CBaseTexture* pTexture = NULL;
	IDirect3DBaseTexture8* pD3DTexture = NULL;
	D3DRESOURCETYPE d3dType;
	pD3DTexture = NULL;
	pTexture = NULL;

	if ( NULL == ppTexture )
	{
		DPF(1, _T("CDevice::GetTexture8() - ppTexture is NULL.\n"));
		return SetLastError(E_POINTER);
	}

	*ppTexture = NULL;

	HRESULT hr = m_pDevice8->GetTexture( (DWORD) nIndex, &pD3DTexture );
	
	if ( FAILED( hr ) )
	{
		DPF(1, _T("CDevice::GetTexture8() - GetTexture() failed.\n"));
		return SetLastError(hr);
	}

	d3dType = pD3DTexture->GetType();


	// Alloc the texture wrapper
	if( d3dType == D3DRTYPE_TEXTURE )
	{
		pTexture = new CTexture;
	}
	else if (d3dType == D3DRTYPE_VOLUMETEXTURE)
	{
		pTexture = new CVolTexture;
	}
	else if (d3dType == D3DRTYPE_CUBETEXTURE)
	{
		pTexture = new CCubeTexture;
	}
	else
	{
		DPF(1, _T("CDevice::GetTexture8() - GetType() failed.\n"));
		RELEASE( pD3DTexture );
		return E_FAIL;
	}

	if(pTexture == NULL)
	{
		DPF(1, _T("CDevice::GetTexture8() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}

	if( d3dType == D3DRTYPE_TEXTURE )
	{
		((CTexture*)pTexture)->Init((LPDIRECT3DTEXTURE8)pD3DTexture, this );
	}
	else if (d3dType == D3DRTYPE_VOLUMETEXTURE)
	{
		((CVolTexture*)pTexture)->Init( (LPDIRECT3DVOLUMETEXTURE8)pD3DTexture, this );
	}
	else if (d3dType == D3DRTYPE_CUBETEXTURE)
	{
		((CCubeTexture*)pTexture)->Init( (LPDIRECT3DCUBETEXTURE8)pD3DTexture, this );
	}
	else
	{
		hr = E_FAIL;
	}

	if(SUCCEEDED(hr))
	{
		// we succeeded!
		*ppTexture = pTexture;
	}
	else
	{
		DPF(1, _T("CDevice::GetTexture8() - CTexture::Init() failed.\n") );

		RELEASE( pTexture );
		RELEASE( pD3DTexture );

		hr = E_FAIL;
	}

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetTexture8(int nIndex, CBaseTexture *pTexture)
{
    if (m_pDevice8 == NULL)
	{
		DPF(1, _T("CDevice::SetTexture8() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

	return SetLastError(m_pDevice8->SetTexture(nIndex, pTexture != NULL ? pTexture->GetD3DTexture8() : NULL));
}
	
// --------------------------------------------------------------------------------------

HRESULT CDevice::GetTextureStageState8(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue)
{
    return SetLastError(m_pDevice8->GetTextureStageState(Stage,Type,pValue));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetTextureStageState8(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
    return SetLastError(m_pDevice8->SetTextureStageState(Stage,Type,Value));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::ValidateDevice8(DWORD* pdwPasses)
{
    return SetLastError(m_pDevice8->ValidateDevice(pdwPasses));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetInfo8(DWORD DevInfoID,void* pDevInfoStruct,DWORD DevInfoStructSize)
{
    return SetLastError(m_pDevice8->GetInfo(DevInfoID,pDevInfoStruct,DevInfoStructSize));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetPaletteEntries8(UINT PaletteNumber,PALETTEENTRY* pEntries)
{
    return SetLastError(m_pDevice8->SetPaletteEntries(PaletteNumber,pEntries));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetPaletteEntries8(UINT PaletteNumber,PALETTEENTRY* pEntries)
{
    return SetLastError(m_pDevice8->GetPaletteEntries(PaletteNumber,pEntries));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetCurrentTexturePalette8(UINT PaletteNumber)
{
    return SetLastError(m_pDevice8->SetCurrentTexturePalette(PaletteNumber));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetCurrentTexturePalette8(UINT *PaletteNumber)
{
    return SetLastError(m_pDevice8->GetCurrentTexturePalette(PaletteNumber));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawPrimitive8(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
    return SetLastError(m_pDevice8->DrawPrimitive(PrimitiveType,StartVertex,PrimitiveCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawIndexedPrimitive8(D3DPRIMITIVETYPE PrimitiveType,UINT minIndex,
									   UINT NumVertices,UINT startIndex,UINT primCount,INT BaseVertexIndex)
{
    return SetLastError(m_pDevice8->DrawIndexedPrimitive(PrimitiveType,minIndex,NumVertices,startIndex,primCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawPrimitiveUP8(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,
								  void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
    return SetLastError(m_pDevice8->DrawPrimitiveUP(	PrimitiveType,PrimitiveCount,
										pVertexStreamZeroData,VertexStreamZeroStride));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawIndexedPrimitiveUP8(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,
										 UINT NumVertexIndices,UINT PrimitiveCount,
										 void* pIndexData,FORMAT IndexDataFormat,
										 void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
    return SetLastError(m_pDevice8->DrawIndexedPrimitiveUP(	PrimitiveType,MinVertexIndex,
												NumVertexIndices,PrimitiveCount,
												pIndexData,IndexDataFormat.d3dfFormat,
												pVertexStreamZeroData,VertexStreamZeroStride));
}

// --------------------------------------------------------------------------------------

bool CDevice::DrawPrimitiveUserMemory8(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc,
					LPVOID lpVertices, DWORD  dwVertexCount, LPVOID lpIndices, DWORD  dwIndexCount, 
					DWORD  dwFlags, bool bIs32BitIndices)
{
    bool    fResult = true;

    DWORD dwStride = GetVertexSize(dwVertexTypeDesc);
    DWORD dwSize = dwStride * dwVertexCount;

    // Set FVF vertex shader
    if (FAILED(SetFVF(dwVertexTypeDesc)))
		return false;

    // DX8 DrawPrimitive 
    if (NULL == lpIndices)
    {
        DWORD dwPrimitiveCount = GetPrimitiveCount(d3dptPrimitiveType, dwVertexCount);

//      dwProcessedFlags = ProcessDrawPrimitiveFlags(dwFlags, false);
        if (FAILED(DrawPrimitiveUP8(d3dptPrimitiveType, dwPrimitiveCount, lpVertices, dwStride)))
			fResult = false;
//      RestoreDrawPrimitiveFlags();
    }
    // DX8 DrawIndixedPrimitive
    else
    {
        DWORD		dwPrimitiveCount = GetPrimitiveCount(d3dptPrimitiveType, dwIndexCount);
		FORMAT		d3dfIndexFormat( (bIs32BitIndices) ? FMT_INDEX32 : FMT_INDEX16 );

//      dwProcessedFlags = ProcessDrawPrimitiveFlags(dwFlags, false);
        if (FAILED(DrawIndexedPrimitiveUP8(d3dptPrimitiveType, 0, dwVertexCount, dwPrimitiveCount, lpIndices, d3dfIndexFormat, lpVertices, dwStride)))
			fResult = false;
//      RestoreDrawPrimitiveFlags();
    }
    return fResult;
}

// --------------------------------------------------------------------------------------

bool CDevice::DrawPrimitiveVertexBuffer8(D3DPRIMITIVETYPE d3dptPrimitiveType, 
					CVertexBuffer *pBuffer, DWORD dwStartVertex, DWORD dwNumVertices, 
					LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags,
					DWORD dwUsage, DWORD dwPool, bool bIs32BitIndices)
{
    DWORD   dwStride = 0;
    bool    fResult = true;

    if (NULL == pBuffer)
		return false;
    
	dwStride = GetVertexSize(pBuffer->GetVertexFormat());

    // Set FVF vertex shader
    if (FAILED(SetFVF(pBuffer->GetVertexFormat())))
		return false;

    // Set source stream
    if (FAILED(SetStreamSource8(0, pBuffer, dwStride, 0)))
		return false;


    // DX8 DrawPrimitive 
    if (NULL == lpwIndices)
    {
        DWORD dwPrimitiveCount = GetPrimitiveCount(d3dptPrimitiveType, dwNumVertices);

//      ProcessDrawPrimitiveFlags(dwFlags, false);
        if (FAILED(DrawPrimitive8(d3dptPrimitiveType, dwStartVertex, dwPrimitiveCount)))
			return false;
//      RestoreDrawPrimitiveFlags();
    }
    // DX8 DrawIndixedPrimitive
    else
    {
		DWORD			dwPrimitiveCount = GetPrimitiveCount(d3dptPrimitiveType, dwIndexCount);
		FMT				d3dfIndexBuffer = (bIs32BitIndices) ? FMT_INDEX32 : FMT_INDEX16;
		DWORD			dwLenght = (bIs32BitIndices) ? dwIndexCount * sizeof(DWORD) : dwIndexCount * sizeof(WORD);
		WORD*			pBuf;
		CIndexBuffer*	pIB;

		if (FAILED(CreateIndexBuffer8(dwLenght, dwUsage, d3dfIndexBuffer, dwPool, &pIB)))
		{
			DebugOut.Write("CDevice: Failed to create index buffer\n");
			return false;
		}
        if (FAILED(pIB->Lock(0, 0, (PBYTE*)&pBuf, 0)))
        {
            RELEASE(pIB);
            DebugOut.Write("CDevice: Failed to lock index buffer\n");
            return false;
        }
        memcpy(pBuf, lpwIndices, pIB->GetSize());
        pIB->Unlock();
        SetIndices8(pIB, 0);
        RELEASE(pIB);

//		dwProcessedFlags = ProcessDrawPrimitiveFlags(dwFlags, false);
        if (FAILED(DrawIndexedPrimitive8(d3dptPrimitiveType, dwStartVertex, dwNumVertices, 0, dwPrimitiveCount, 0)))
			fResult = false;
//		RestoreDrawPrimitiveFlags();
    }

    return fResult;
}


// --------------------------------------------------------------------------------------

bool CDevice::DrawPrimitiveStridedData8(D3DPRIMITIVETYPE d3dptPrimitiveType,
					DWORD  dwVertexTypeDesc, LPDRAWPRIMITIVESTRIDEDDATA lpVertexArray,
					DWORD  dwVertexCount, LPWORD lpwIndices, DWORD  dwIndexCount,
					DWORD  dwFlags, DWORD  dwUsage, DWORD  dwPool, bool bIs32BitIndices)
{

    LPDRAWPRIMITIVESTRIDEDDATA  pVert = (LPDRAWPRIMITIVESTRIDEDDATA)lpVertexArray;
    CVertexBuffer*              pVB = NULL;
    BYTE*                       pSrc = NULL;
    BYTE*                       pDst = NULL;
    DWORD   dwWho = dwFlags & (CD3D_SRC | CD3D_REF);
    bool    fResult = true;

	DWORD   dwTextureFormats = dwVertexTypeDesc >> CDIRECT3D_TEXTUREFORMATS_SHIFT;
	DWORD   dwNumTexCoords = ((dwVertexTypeDesc & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);
    DWORD   pdwDeclaration[26];
    DWORD   dwIndex = 0;
    DWORD   dwStream = 0;
	CVertexShader* pShader = 0;

    // Construct vertex shader declaration for multistream (strided) data and set streams
    // NOTE: Strided format does not support transformed data

    // Position data and blend weights stream
    pdwDeclaration[dwIndex] = D3DVSD_STREAM(dwStream);
    dwIndex++;
    switch (dwVertexTypeDesc & D3DFVF_POSITION_MASK)
    {
        case D3DFVF_XYZ:
            pdwDeclaration[dwIndex] = D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3);
            dwIndex++;
            break;
        case D3DFVF_XYZB1:
            pdwDeclaration[dwIndex] = D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3);
            pdwDeclaration[dwIndex+1] = D3DVSD_REG(D3DVSDE_BLENDWEIGHT, D3DVSDT_FLOAT1);
            dwIndex += 2;
            break;
        case D3DFVF_XYZB2:
            pdwDeclaration[dwIndex] = D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3);
            pdwDeclaration[dwIndex+1] = D3DVSD_REG(D3DVSDE_BLENDWEIGHT, D3DVSDT_FLOAT2);
            dwIndex += 2;
            break;
        case D3DFVF_XYZB3:
            pdwDeclaration[dwIndex] = D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3);
            pdwDeclaration[dwIndex+1] = D3DVSD_REG(D3DVSDE_BLENDWEIGHT, D3DVSDT_FLOAT3);
            dwIndex += 2;
            break;
        case D3DFVF_XYZB4:
            pdwDeclaration[dwIndex] = D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3);
            pdwDeclaration[dwIndex+1] = D3DVSD_REG(D3DVSDE_BLENDWEIGHT, D3DVSDT_FLOAT4);
            dwIndex += 2;
            break;
    }
    
    if (fResult)
    {
        if (FAILED(ConvertToVertexBuffer((dwVertexTypeDesc & D3DFVF_POSITION_MASK), dwVertexCount, pVert->position.lpvData, pVert->position.dwStride, dwUsage, dwPool, false, &pVB)))
            fResult = false;
        else
        if (FAILED(SetStreamSource8(dwStream, pVB, GetVertexSize(dwVertexTypeDesc & D3DFVF_POSITION_MASK), 0)))
            fResult = false;
        RELEASE(pVB);
    }
    dwStream++;

    // Normal and point size stream
    if (0 != (dwVertexTypeDesc & (D3DFVF_NORMAL | D3DFVF_PSIZE)))
    {
        pdwDeclaration[dwIndex++] = D3DVSD_STREAM(dwStream);
        if (0 != (dwVertexTypeDesc & D3DFVF_NORMAL))
            pdwDeclaration[dwIndex++] = D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3);
        if (0 != (dwVertexTypeDesc & D3DFVF_PSIZE))
            pdwDeclaration[dwIndex++] = D3DVSD_REG(D3DVSDE_PSIZE, D3DVSDT_FLOAT1);

        if (fResult)
        {
            if (FAILED(ConvertToVertexBuffer((dwVertexTypeDesc & (D3DFVF_NORMAL | D3DFVF_PSIZE)), dwVertexCount, pVert->normal.lpvData, pVert->normal.dwStride, dwUsage, dwPool, false, &pVB)))
                fResult = false;
            else
            if (FAILED(SetStreamSource8(dwStream, pVB, GetVertexSize(dwVertexTypeDesc & (D3DFVF_NORMAL | D3DFVF_PSIZE)), 0)))
                fResult = false;
            RELEASE(pVB);
        }
        dwStream++;
    }

    // Diffuse color stream
    if (0 != (dwVertexTypeDesc & D3DFVF_DIFFUSE))
    {
        pdwDeclaration[dwIndex] = D3DVSD_STREAM(dwStream);
        pdwDeclaration[dwIndex+1] = D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR);
        dwIndex += 2;
        if (fResult)
        {
            if (FAILED(ConvertToVertexBuffer(D3DFVF_DIFFUSE, dwVertexCount, pVert->diffuse.lpvData, pVert->diffuse.dwStride, dwUsage, dwPool, false, &pVB)))
                fResult = false;
            else
            if (FAILED(SetStreamSource8(dwStream, pVB, sizeof(DWORD), 0)))
                fResult = false;
            RELEASE(pVB);
        }
        dwStream++;
    }

    // Specular color stream
    if (0 != (dwVertexTypeDesc & D3DFVF_SPECULAR))
    {
        pdwDeclaration[dwIndex] = D3DVSD_STREAM(dwStream);
        pdwDeclaration[dwIndex+1] = D3DVSD_REG(D3DVSDE_SPECULAR, D3DVSDT_D3DCOLOR);
        dwIndex += 2;
        if (fResult)
        {
            if(FAILED(ConvertToVertexBuffer(D3DFVF_SPECULAR, dwVertexCount, pVert->specular.lpvData, pVert->specular.dwStride, dwUsage, dwPool, false, &pVB)))
                fResult = false;
            else
            if (FAILED(SetStreamSource8(dwStream, pVB, sizeof(DWORD), 0)))
                fResult = false;
            RELEASE(pVB);
        }
        dwStream++;
    }

    // Texture streams
    for (DWORD dwCount = 0; dwCount < dwNumTexCoords; dwCount++)
    {
        static DWORD dwTexNum[] = { D3DVSDE_TEXCOORD0, D3DVSDE_TEXCOORD1, D3DVSDE_TEXCOORD2, 
                                    D3DVSDE_TEXCOORD3, D3DVSDE_TEXCOORD4, D3DVSDE_TEXCOORD5, 
                                    D3DVSDE_TEXCOORD6, D3DVSDE_TEXCOORD7};
        static DWORD dwTexFormat[] = {  D3DVSDT_FLOAT2, D3DVSDT_FLOAT3, 
                                        D3DVSDT_FLOAT4, D3DVSDT_FLOAT1};
        static DWORD dwTexSize[] = { 2*sizeof(float), 3*sizeof(float), 4*sizeof(float), sizeof(float)};

        pdwDeclaration[dwIndex] = D3DVSD_STREAM(dwStream);
        pdwDeclaration[dwIndex+1] = D3DVSD_REG((dwTexNum[dwCount]),
            (dwTexFormat[dwTextureFormats & CDIRECT3D_TEXTUREFORMAT_MASK]));
        dwIndex += 2;
        if (fResult)
        {
            if (FAILED(ConvertToVertexBuffer((dwTextureFormats & CDIRECT3D_TEXTUREFORMAT_MASK) | (1 << D3DFVF_TEXCOUNT_SHIFT), dwVertexCount, pVert->textureCoords[dwCount].lpvData, pVert->textureCoords[dwCount].dwStride, dwUsage, dwPool, false, &pVB)))
                fResult = false;
            else
            if (FAILED(SetStreamSource8(dwStream, pVB, dwTexSize[dwTextureFormats & CDIRECT3D_TEXTUREFORMAT_MASK], 0)))
                fResult = false;
            RELEASE(pVB);
        }
        dwTextureFormats >>= CDIRECT3D_TEXTUREFORMAT_SIZE;
        dwStream++;
    }

    // Close shader declaration
    pdwDeclaration[dwIndex] =  D3DVSD_END();

    // Create temporary shader
    if (fResult) 
	{
		if (FAILED(CreateVertexShader8(pdwDeclaration, NULL, &pShader, (dwUsage & D3DUSAGE_SOFTWAREPROCESSING) | (IsHWVP() ? 0 : D3DUSAGE_SOFTWAREPROCESSING) )))
			fResult = false;
		if (fResult)
		if (FAILED(SetVertexShaderInt8(pShader)))
			fResult = false;
	}

    // DX8 DrawPrimitive (strided data)
    if (NULL == lpwIndices)
    {
        DWORD dwPrimitiveCount = GetPrimitiveCount(d3dptPrimitiveType, dwVertexCount);

        if (fResult)
        {
//			dwProcessedFlags = ProcessDrawPrimitiveFlags(dwFlags, false);
            if (FAILED(DrawPrimitive8(d3dptPrimitiveType, 0, dwPrimitiveCount)))
				fResult = false;
//			RestoreDrawPrimitiveFlags();
        }
    }
    // DX8 DrawIndexedPrimitive (strided data)
    else
    {
        DWORD			dwPrimitiveCount = GetPrimitiveCount(d3dptPrimitiveType, dwIndexCount);
		FMT				d3dfIndexBuffer = (bIs32BitIndices) ? FMT_INDEX32 : FMT_INDEX16;
		DWORD			dwLenght = (bIs32BitIndices) ? dwIndexCount * sizeof(DWORD) : dwIndexCount * sizeof(WORD);
        WORD*			pBuf;

        if (fResult)
        {
            CIndexBuffer    *pIB;
			
			if (FAILED(CreateIndexBuffer8(dwLenght, dwUsage, d3dfIndexBuffer, dwPool, &pIB)))
            {
                DebugOut.Write("CDevice: Failed to create index buffer\n");
                return false;
            }
            if (FAILED(pIB->Lock(0, 0, (PBYTE*)&pBuf, 0)))
            {
                RELEASE(pIB);
                DebugOut.Write("CDevice: Failed to lock index buffer\n");
                return false;
            }
            memcpy(pBuf, lpwIndices, pIB->GetSize8());
            pIB->Unlock();
            SetIndices8(pIB, 0);
            RELEASE(pIB);

            if (fResult)
            {
//				dwProcessedFlags = ProcessDrawPrimitiveFlags(dwFlags, false);
                if (FAILED(DrawIndexedPrimitive8(d3dptPrimitiveType, 0, dwVertexCount, 0, dwPrimitiveCount, 0)))
					fResult = false;
//				RestoreDrawPrimitiveFlags();
            }
        }
    }
    
	if (pShader)
		DeleteVertexShader(pShader);
	
	return fResult;
}



// --------------------------------------------------------------------------------------

HRESULT CDevice::ProcessVertices8(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,
								  CVertexBuffer* pDestBuffer,CVertexDeclaration *pDecl,DWORD Flags)
{
    return SetLastError(m_pDevice8->ProcessVertices(SrcStartIndex,DestIndex,VertexCount,pDestBuffer==0L?0L:pDestBuffer->GetVertexBufferPointer8(),Flags));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateVertexShader8(DWORD* pDeclaration,DWORD* pFunction,CVertexShader** pHandle,DWORD Usage)
{
    HRESULT hr = S_OK;
	CVertexShader *pShader = NULL;
	DWORD dwHandle = 0;

	pShader = new CVertexShader();
	if(NULL == pShader)
	{
		DPF(1, _T("CDevice::CreateVertexShader8() - Out of memory.\n"));
		return E_OUTOFMEMORY;
	}
	
	hr = m_pDevice8->CreateVertexShader(pDeclaration,pFunction, (DWORD*)&dwHandle,Usage);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateVertexShader8() - CreateVertexShader failed. hr=%s(%x).\n"), 
			m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	if(!pShader->Init((PVOID)&dwHandle, this))
	{
		DPF(1, _T("CDevice::CreateVertexShader8() - Failed to init shader wrapper.\n"));
		hr = E_FAIL;
		goto Exit;
	}

Exit:

	if(FAILED(hr))
	{
		RELEASE(pShader);
	}
    else
		*pHandle = pShader;

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetVertexShader8(DWORD Handle)
{
    return SetLastError(m_pDevice8->SetVertexShader(Handle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetVertexShaderInt8(CVertexShader* pShader)
{
	return SetLastError(m_pDevice8->SetVertexShader(pShader == NULL ? NULL : pShader->m_dwVS));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetFVF8(DWORD dwFVF)
{
    return SetLastError(m_pDevice8->SetVertexShader(dwFVF));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetVertexShader8(DWORD* pHandle)
{
    return SetLastError(m_pDevice8->GetVertexShader((DWORD*)pHandle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DeleteVertexShader8(CVertexShader* pShader)
{
	DWORD dwHandle = pShader->m_dwVS;
	pShader->Release();
    return SetLastError(m_pDevice8->DeleteVertexShader(dwHandle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetVertexShaderConstant8(UINT Register, CONST float* pConstantData,UINT ConstantCount)
{
    return SetLastError(m_pDevice8->SetVertexShaderConstant(Register,pConstantData,ConstantCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetVertexShaderConstant8(UINT Register,float* pConstantData,UINT ConstantCount)
{
    return SetLastError(m_pDevice8->GetVertexShaderConstant(Register,pConstantData,ConstantCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetVertexShaderDeclaration8(DWORD Handle,void* pData,DWORD* pSizeOfData)
{
    return SetLastError(m_pDevice8->GetVertexShaderDeclaration(Handle,pData,pSizeOfData));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetVertexShaderFunction8(DWORD Handle,void* pData,DWORD* pSizeOfData)
{
    return SetLastError(m_pDevice8->GetVertexShaderFunction(Handle,pData,pSizeOfData));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetStreamSource8(UINT streamNumber, CVertexBuffer *pStreamData, UINT stride, UINT offset)
{
    return SetLastError(m_pDevice8->SetStreamSource(streamNumber,
													pStreamData==0L?0L:pStreamData->GetVertexBufferPointer8(),
													stride));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetStreamSource8(UINT streamNumber, CVertexBuffer **ppStreamData, UINT *pStride)
{
	LPDIRECT3DVERTEXBUFFER8 pd3dVertexBuffer = NULL;
	HRESULT hr = m_pDevice8->GetStreamSource(streamNumber, ppStreamData==0L?0L:&pd3dVertexBuffer, pStride);
	if (FAILED(hr)||0L==ppStreamData)	{
		DPF(1, _T("CDevice::GetStreamSource8() - GetStreamSource() failed.\n"));
		return SetLastError(hr);
	}

	if(NULL == pd3dVertexBuffer)
	{
		ppStreamData = NULL;
		return S_OK;
	}

	CVertexBuffer *pVertexBuffer = new CVertexBuffer;
    if (0L == pVertexBuffer) {
		DPF(1, _T("CDevice::GetStreamSource8() - Out of Memory.\n"));
		return SetLastError(E_FAIL);
	}

	if (!pVertexBuffer->Initialize(pd3dVertexBuffer, this)) {
		DPF(1, _T("CDevice::GetStreamSource8() - CVertexBuffer::SetVertexBuffer() failed.\n"));
		RELEASE(pVertexBuffer);
		RELEASE(pd3dVertexBuffer);
		return SetLastError(E_FAIL);
	}

	*ppStreamData = pVertexBuffer;
	RELEASE(pd3dVertexBuffer);
    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetIndices8(CIndexBuffer *pIndexData, UINT baseVertexIndex)
{
    return SetLastError(m_pDevice8->SetIndices(pIndexData==0L?0L:pIndexData->GetIndexBufferPointer8(),
											   baseVertexIndex));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetIndices8(CIndexBuffer **ppIndexData, UINT *pBaseVertexIndex)
{
	LPDIRECT3DINDEXBUFFER8 pd3dIndexBuffer = NULL;
	HRESULT hr = m_pDevice8->GetIndices(ppIndexData==0L?0L:&pd3dIndexBuffer, pBaseVertexIndex);
	if (FAILED(hr)||0L==ppIndexData)	{
		DPF(1, _T("CDevice::GetIndices8() - GetIndices() failed.\n"));
		return SetLastError(hr);
	}

	// Check for NULL
	if(NULL == pd3dIndexBuffer)
	{
        *ppIndexData = NULL;
		return S_OK;
	}

	CIndexBuffer *pIndexBuffer = new CIndexBuffer;

    if (0L == pIndexBuffer) {
		DPF(1, _T("CDevice::GetIndices8() - Out of Memory.\n"));
		return SetLastError(E_FAIL);
	}

	if (!pIndexBuffer->Initialize(pd3dIndexBuffer,this)) {
		DPF(1, _T("CDevice::GetIndices8() - CIndexBuffer::SetIndexBuffer() failed.\n"));
		RELEASE(pIndexBuffer);
		RELEASE(pd3dIndexBuffer);
		return SetLastError(E_FAIL);
	}

	*ppIndexData = pIndexBuffer;
	RELEASE(pd3dIndexBuffer);
    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreatePixelShader8(DWORD* pFunction,DWORD* pHandle)
{
    return SetLastError(m_pDevice8->CreatePixelShader(pFunction, pHandle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetPixelShader8(DWORD Handle)
{
    return SetLastError(m_pDevice8->SetPixelShader(Handle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetPixelShader8(DWORD* pHandle)
{
    return SetLastError(m_pDevice8->GetPixelShader(pHandle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DeletePixelShader8(DWORD dwHandle)
{
    return SetLastError(m_pDevice8->DeletePixelShader(dwHandle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreatePixelShaderInt8(DWORD* pFunction,CPShader** ppShader)
{
	HRESULT hr = S_OK;
	CPShader *pShader = NULL;
	DWORD dwHandle = 0;
	
	pShader = new CPShader();
	if(NULL == pShader)
	{
		DPF(1, _T("CDevice::CreatePixelShaderInt8() - Out of memory.\n"));
		return E_OUTOFMEMORY;
	}
	
	hr = m_pDevice8->CreatePixelShader(pFunction,&dwHandle);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreatePixelShaderInt8() - CreatePixelShader failed. hr=%s(%x).\n"), 
			m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}
	
	if(!pShader->Init((PVOID)&dwHandle, this))
	{
		DPF(1, _T("CDevice::CreatePixelShaderInt8() - Failed to init shader wrapper.\n"));
		hr = E_FAIL;
		goto Exit;
	}
	
Exit:
	
	if(FAILED(hr))
	{
		RELEASE(pShader);
	}
	else
		*ppShader = pShader;
	
	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetPixelShaderInt8(CPShader *pShader)
{
    return SetLastError(m_pDevice8->SetPixelShader(pShader ? pShader->GetShader8() : NULL));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetPixelShaderInt8(CPShader** ppShader)
{
	HRESULT hr = S_OK;
	DWORD dwShader = 0;
	CPShader *pShader = NULL;

    hr = m_pDevice8->GetPixelShader(&dwShader);
	if(FAILED(hr))
		goto Exit;

	pShader = new CPShader();
	if(pShader == NULL)
	{
		DPF(1, _T("CDevice::GetPixelShaderInt8() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	if(!pShader->Init(&dwShader, this))
	{
	    DPF(1, _T("CDevice::GetPixelShaderInt8() - Failed to init shader wrapper.\n"));
		hr = E_FAIL;
		goto Exit;
	}

Exit:

	if(FAILED(hr))
	{
		RELEASE(pShader);
	}
	else
		*ppShader = pShader;

    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DeletePixelShaderInt8(CPShader *pShader)
{
    DWORD dwHandle = pShader->GetShader8();
	pShader->Release();
    return SetLastError(m_pDevice8->DeletePixelShader(dwHandle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetPixelShaderConstant8(UINT Register,const float* pConstantData,UINT ConstantCount)
{
    return SetLastError(m_pDevice8->SetPixelShaderConstant(Register,pConstantData,ConstantCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetPixelShaderConstant8(UINT Register,float* pConstantData,UINT ConstantCount)
{
    return SetLastError(m_pDevice8->GetPixelShaderConstant(Register,pConstantData,ConstantCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetPixelShaderFunction8(DWORD Handle,void* pData,DWORD* pSizeOfData)
{
    return SetLastError(m_pDevice8->GetPixelShaderFunction(Handle,pData,pSizeOfData));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawRectPatch8(UINT Handle,float* pNumSegs,RECTPATCH_INFO* pRectPatchInfo)
{
    return SetLastError(m_pDevice8->DrawRectPatch(Handle,pNumSegs,pRectPatchInfo));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawTriPatch8(UINT Handle,float* pNumSegs,TRIPATCH_INFO* pTriPatchInfo)
{
    return SetLastError(m_pDevice8->DrawTriPatch(Handle,pNumSegs,pTriPatchInfo));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DeletePatch8(UINT Handle)
{
    return SetLastError(m_pDevice8->DeletePatch(Handle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateAdditionalSwapChain8(PRESENT_PARAMETERS* pParams, CSwapChain** ppSwapChain)
{
	HRESULT hr = 0;
	IDirect3DSwapChain8 *pSwap8 = NULL;
	D3DPRESENT_PARAMETERS Params;

	if (ppSwapChain)
		*ppSwapChain = NULL;

	if( pParams )
	{
		// Generate the present param
		Params.BackBufferWidth = pParams->uWidth;
		Params.BackBufferHeight = pParams->uHeight;
		Params.BackBufferFormat = pParams->BackBufferFormat.d3dfFormat;
		Params.BackBufferCount = pParams->uBackBufferCount;
		Params.MultiSampleType = (D3DMULTISAMPLE_TYPE)pParams->MultiSample;
		if(pParams->SwapEffect == FMWK_DEFAULT)
			Params.SwapEffect = (Params.MultiSampleType == D3DMULTISAMPLE_NONE) ? D3DSWAPEFFECT_COPY : D3DSWAPEFFECT_DISCARD;
		else
			Params.SwapEffect = (D3DSWAPEFFECT)pParams->SwapEffect;
		Params.hDeviceWindow = pParams->hDeviceWindow;
		Params.EnableAutoDepthStencil = pParams->bAutoDepthStencil;
		Params.AutoDepthStencilFormat = pParams->AutoDepthStencilFormat.d3dfFormat;
		if(pParams->dwFlags == FMWK_DEFAULT)
			Params.Flags = (Params.MultiSampleType == D3DMULTISAMPLE_NONE) ? D3DPRESENTFLAG_LOCKABLE_BACKBUFFER : 0;
		else
			Params.Flags = pParams->dwFlags;
		Params.FullScreen_RefreshRateInHz = pParams->uFullscreenRefresh;
		if(pParams->uFullscreenPresentInterval == FMWK_DEFAULT)
			Params.FullScreen_PresentationInterval = PRESENT_INTERVAL_DEFAULT;
		else
			Params.FullScreen_PresentationInterval = pParams->uFullscreenPresentInterval;
		Params.Windowed = (UINT)pParams->bWindowed;
	}
	
	hr = m_pDevice8->CreateAdditionalSwapChain(pParams==0L?0L:&Params, ppSwapChain==0L?0L:&pSwap8);
	if(FAILED(hr)||0L==ppSwapChain)
	{
		DPF(1, _T("CDevice::CreateAdditionalSwapChain8() - CreateAdditionalSwapChain failed. hr=%s(%x).\n"), m_pD3D->HResultToString(hr), hr);
		goto Error;
	}

	*ppSwapChain = new CSwapChain();
	if(*ppSwapChain == NULL)
	{
		DPF(1, _T("CDevice::CreateAdditionalSwapChain8() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	if(!(*ppSwapChain)->Init(pSwap8, pParams, this))
	{
		DPF(1, _T("CDevice::CreateAdditionalSwapChain8() - failed to init CSurface.\n"));
		hr = E_FAIL;
		goto Error;
	}

    RELEASE(pSwap8);
	return S_OK;

Error:
	RELEASE(pSwap8);
	if (ppSwapChain)
		RELEASE(*ppSwapChain);

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetNPatchMode8(float fSegments)

{
    return SetLastError(SetRenderState(D3DRS_PATCHSEGMENTS, (DWORD)*((unsigned*)&fSegments)));
}

// --------------------------------------------------------------------------------------

void CDevice::D3DXInitializePSGP8()
{
	D3DXMATRIX Matrix;
	D3DXMatrixIdentity(&Matrix);
    D3DXMatrixMultiply(&Matrix, &Matrix, &Matrix);
}
