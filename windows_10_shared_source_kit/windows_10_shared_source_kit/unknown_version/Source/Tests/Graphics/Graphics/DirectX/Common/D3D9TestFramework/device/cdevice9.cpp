// ================================+======================================================
//
//  CDevice9.cpp
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================

#include "DevicePCH.h"
#include "CDevice.h"
#include <d3d8rgbrast.h>
#include "D3DX9.h"
#include "CD3DX.h"
#include "CTextures.h"
#include "CSurface.h"
#include "CTextures.h"
#include "CSwapChain.h"
#include "CBuffers.h"
#include "CQuery.h"
#include "CResource.h"


// --------------------------------------------------------------------------------------
HRESULT CDevice::Initialize9
(
	PRESENT_PARAMETERS * pParams
)
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;
	DWORD dwBeh = 0;
	D3DDEVTYPE DevType;
	UINT nNumAdapters = 0;
	TCHAR szFormat[64] = {0};

	D3DPRESENT_PARAMETERS * pD3DParams = NULL;
	D3DDISPLAYMODEEX * pFullScreenModes = NULL;


	//
	// Validate parameters.
	//

	if ( pParams == NULL )
	{
		DPF
		(
			1,
			_T( "CDevice::Initialize9:  pParams is NULL.\n")
		);

		hr = E_POINTER;
		goto Exit;
	};


	//
	// Validate members and state.
	//

	if ( m_pD3D == NULL )
	{
		DPF
		(
			1,
			_T( "CDevice::Initialize9:  m_pD3D is NULL.\n" )
		);

		goto Exit;
	};


	if ( !GetAdapter() )
	{
		DPF
		(
			1,
			_T( "CDevice::Initialize9:  GetAdapter failed.\n" )
		);

		goto Exit;
	};


	//
	// Convert DEVICETYPE to D3DDEVTYPE.
	//

	if ( m_dwDevType == DEVICETYPE_HAL )
	{
		DevType = D3DDEVTYPE_HAL;
	}
	else if ( m_dwDevType == DEVICETYPE_REF )
	{
		DevType = D3DDEVTYPE_REF;
	}
	else if ( m_dwDevType == DEVICETYPE_SW )
	{
		DevType = D3DDEVTYPE_SW;
	}
	else if ( m_dwDevType == DEVICETYPE_NULLREF )
	{
		DevType = D3DDEVTYPE_NULLREF;
	}
	else
	{
		DPF
		(
			1,
			_T("CDevice::Initialize9:  Unknown device type.\n")
		);

		hr = E_INVALIDARG;
		goto Exit;
	};


	//
	// Convert DEVICECREATE flags to D3DCREATE behaviors.
	//

	if ( ( m_dwBehaviors & DEVICECREATE_MIXED ) != 0 )
	{
		dwBeh |= D3DCREATE_MIXED_VERTEXPROCESSING;
		if(m_dwBehaviors & DEVICECREATE_HWVP)
			m_bHWVP = true;
	}
	else if ( ( m_dwBehaviors & DEVICECREATE_PURE ) != 0 )
	{
		dwBeh |= D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING;
		m_bPure = true;
		m_bHWVP = true;
	}
	else if ( ( m_dwBehaviors & DEVICECREATE_HWVP ) != 0 )
	{
		dwBeh |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
		m_bHWVP = true;
	}
	else if ( ( m_dwBehaviors & DEVICECREATE_SWVP ) != 0 )
	{
		dwBeh |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	};
	
	if
	(
		( m_dwBehaviors & DEVICECREATE_DISABLE_PSGP_THREADING ) != 0 &&
		FLOAT_EQUAL(m_pD3D->GetVersion(), 9.1f)
	)
	{
		dwBeh |= D3DCREATE_DISABLE_PSGP_THREADING;
	};

	if ( ( m_dwBehaviors & DEVICECREATE_MULTITHREADED ) != 0 )
	{
		dwBeh |= D3DCREATE_MULTITHREADED;
	};

	if ( ( m_dwBehaviors & DEVICECREATE_FPU_PRESERVE ) != 0 )
	{
		dwBeh |= D3DCREATE_FPU_PRESERVE;
	};

	if ( ( m_dwBehaviors & DEVICECREATE_DISABLE_DRIVER_MANAGEMENT ) != 0 )
	{
		dwBeh |= D3DCREATE_DISABLE_DRIVER_MANAGEMENT;
	};

	if ( ( m_dwBehaviors & DEVICECREATE_ADAPTERGROUP_DEVICE ) != 0 )
	{
		dwBeh |= D3DCREATE_ADAPTERGROUP_DEVICE;
	};

	if ( ( m_dwBehaviors & DEVICECREATE_DISABLE_DRIVER_MANAGEMENT_EX ) != 0 )
	{
		dwBeh |= D3DCREATE_DISABLE_DRIVER_MANAGEMENT_EX;
	};


	// D3DCREATE_SHOW_DP2ERROR (MS internal flag)
	dwBeh |= 0x40000000L;


	//
	// Determine how many adapters we'll be initializing in our
	// call to CreateDevice/CreateDeviceEx.
	//
	// In the simple case of a single-head device, this will be 1.
	// In the case of a multi-head device, where we're asking to
	// drive all heads at once, this will be equal to the number
	// of heads.
	//

	if ( ( dwBeh & D3DCREATE_ADAPTERGROUP_DEVICE ) == D3DCREATE_ADAPTERGROUP_DEVICE )
	{
		nNumAdapters = GetCaps()->NumberOfAdaptersInGroup;

		// We can only drive multiple heads from the master.
		if( nNumAdapters == 0 )
		{
			DPF
			(
				1,
				_T("CDevice::Initialize9:  Can't drive multiple heads from slave head.\n")
			);

			hr = E_UNSUPPORTED;
			goto Exit;		
		};

/*
		DPF
		(
			3,
			_T( "CDevice::Initialize9:  Initializing adapter with %u heads.\n" ),
			nNumAdapters
		);
*/
	}
	else
	{
		nNumAdapters = 1;

/*
		DPF
		(
			3,
			_T( "CDevice::Initialize9:  Initializing single-head adapter.\n" )
		);
*/
	};


	//
	// Allocate a buffer to hold D3DPRESENT_PARAMETERS structures.
	//

	pD3DParams = new D3DPRESENT_PARAMETERS[ nNumAdapters ];

	if ( NULL == pD3DParams )
	{
		DPF(1, _T("CDevice::Initialize9() - Failed to allocate pD3DParams.\n"));

		hr = E_OUTOFMEMORY;
		goto Exit;
	};

	ZeroMemory( pD3DParams, sizeof( D3DPRESENT_PARAMETERS ) * nNumAdapters );


	//
	// Allocate a buffer to hold D3DDISPLAYMODEEX structures.
	//

	pFullScreenModes = new D3DDISPLAYMODEEX[nNumAdapters];

	if ( NULL == pFullScreenModes )
	{
		DPF(1, _T("CDevice::Initialize9() - Failed to allocate pFullScreenModes.\n"));

		hr = E_OUTOFMEMORY;
		goto Exit;
	};
	ZeroMemory( pFullScreenModes, sizeof( D3DDISPLAYMODEEX ) * nNumAdapters );


	//
	// Initialize parameters for each head that we're driving.
	//

	for
	(
		int i = 0;
		i < nNumAdapters;
		++i
	)
	{
		//
		// Decode internal framework flags.
		//

		if(pParams[i].SwapEffect == FMWK_DEFAULT)
			pParams[i].SwapEffect = (pParams[i].MultiSample == MULTISAMPLE_NONE) ? SWAPEFFECT_COPY : SWAPEFFECT_DISCARD;

		if(pParams[i].dwFlags == FMWK_DEFAULT)
			pParams[i].dwFlags = (pParams[i].MultiSample == D3DMULTISAMPLE_NONE) ? D3DPRESENTFLAG_LOCKABLE_BACKBUFFER : 0;

		if(pParams[i].uPresentInterval == FMWK_DEFAULT)
			pParams[i].uPresentInterval = PRESENT_INTERVAL_IMMEDIATE;


		//
		// Initialize present and mode parameters for this head.
		//

		pD3DParams[i].BackBufferWidth = pParams[i].uWidth;
		pD3DParams[i].BackBufferHeight = pParams[i].uHeight;
		pD3DParams[i].BackBufferFormat = pParams[i].BackBufferFormat.d3dfFormat;
		pD3DParams[i].BackBufferCount = pParams[i].uBackBufferCount;
		pD3DParams[i].MultiSampleType = (D3DMULTISAMPLE_TYPE) pParams[i].MultiSample;
		pD3DParams[i].MultiSampleQuality = pParams[i].MultiSampleQuality;
		pD3DParams[i].SwapEffect = (D3DSWAPEFFECT) pParams[i].SwapEffect;
		pD3DParams[i].hDeviceWindow = pParams[i].hDeviceWindow;
		pD3DParams[i].EnableAutoDepthStencil = pParams[i].bAutoDepthStencil;
		pD3DParams[i].AutoDepthStencilFormat = pParams[i].AutoDepthStencilFormat.d3dfFormat;
		pD3DParams[i].Flags = pParams[i].dwFlags;
		pD3DParams[i].FullScreen_RefreshRateInHz = pParams[i].uFullscreenRefresh;
		pD3DParams[i].PresentationInterval = pParams[i].uPresentInterval;
		pD3DParams[i].Windowed = (BOOL) pParams[i].bWindowed;


		//
		// Initialize full-screen display modes for this head.
		//

		// Fill in our D3DDISPLAYMODEEX for fullscreen.
		pFullScreenModes[i].Size = sizeof( pFullScreenModes[i] );
		pFullScreenModes[i].ScanLineOrdering = D3DSCANLINEORDERING_UNKNOWN;
		pFullScreenModes[i].Width = pD3DParams[i].BackBufferWidth;
		pFullScreenModes[i].Height = pD3DParams[i].BackBufferHeight;
		pFullScreenModes[i].Format = pD3DParams[i].BackBufferFormat;
		pFullScreenModes[i].RefreshRate = pD3DParams[i].FullScreen_RefreshRateInHz;


		//
		// Validate multi-head mode for REF, which is windowed.
		//

		if
		(
			( dwBeh & D3DCREATE_ADAPTERGROUP_DEVICE ) == D3DCREATE_ADAPTERGROUP_DEVICE &&
			pD3DParams[i].Windowed == TRUE
		)
		{
/*
			DPF
			(
				1,
				_T( "CDevice::Initialize9() - D3DCREATE_ADAPTERGROUP_DEVICE is full-screen only.\n" )
			);
*/

			if ( DevType == D3DDEVTYPE_REF )
			{
				dwBeh &= ~D3DCREATE_ADAPTERGROUP_DEVICE;
			};
		};


		//
		// Validate the display mode.
		//

		if
		(
			!m_pD3D->IsDisplayModeSupported
			(
				GetAdapter(),
				GetAdapter()->DesktopMode.dwWidth,
				GetAdapter()->DesktopMode.dwHeight,
				(FMT)GetAdapter()->DesktopMode.Format.d3dfFormat
			)
		)
		{
			FmtToString(FORMAT_TEXTURE, &pParams[i].DesktopFormat, szFormat);
			DPF(1, _T("Display Mode (%dx%dx%s) not supported.\n"),GetAdapter()->DesktopMode.dwWidth, GetAdapter()->DesktopMode.dwHeight, szFormat);
			hr = E_UNSUPPORTED;
			goto Exit;		
		}


		//
		// Validate the back buffer format.
		//

		hr = m_pD3D->CheckDeviceType
		(
			m_uAdapter,
			m_dwDevType,
			pParams[i].DesktopFormat,
			pParams[i].BackBufferFormat,
			pParams[i].bWindowed
		);

		if(FAILED(hr))
		{
			FmtToString(FORMAT_TEXTURE, &pParams[i].BackBufferFormat, szFormat);
			DPF(1, _T("Backbuffer format(%s) not supported. CheckDeviceType() returned %s(%x).\n"),
				szFormat, m_pD3D->HResultToString(hr), hr);
			hr = E_UNSUPPORTED;
			goto Exit;		
		}


		//
		// Validate the DepthStencil vs the RenderTarget.
		//

		if(pParams[i].bAutoDepthStencil)
		{
			hr = m_pD3D->CheckDepthStencilMatch
			(
				m_uAdapter,
				m_dwDevType,
				pParams[i].DesktopFormat,
				pParams[i].BackBufferFormat,
				pParams[i].AutoDepthStencilFormat
			);

			if(FAILED(hr))
			{
				DPF(1, _T("DepthStencil format not supported.  CheckDepthStencilMatch() returned %s(%x).\n"), 
					m_pD3D->HResultToString(hr), hr);
				hr = E_UNSUPPORTED;
				goto Exit;		
			}
		}
	}


	//
	// Copy the present params locally.
	//

	m_pPresParams = new PRESENT_PARAMETERS[nNumAdapters];
	if(NULL == m_pPresParams)
	{
		DPF(1, _T("CDevice::Initialize9() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}
	memcpy(m_pPresParams, pParams, sizeof(PRESENT_PARAMETERS) * nNumAdapters);


	//
	// @TODO:  Copy the display modes locally?
	//


	//
	// Create the device.
	//

	if ( FLOAT_EQUAL( m_pD3D->GetVersion(), 9.1f ) )
	{
		// Workaround for bug 1269331:
		// Prevent managed primary behavior from being specified for ref and sw.
		if
		(
			DevType == D3DDEVTYPE_REF ||
			DevType == D3DDEVTYPE_SW
		)
		{
			// dwBeh |= D3DCREATE_DONOTALLOWMANAGEDPRIMARY;
		};


		if(pParams->dwInternalFlags & PPINTERNAL_DEVICE0)
		{
			ASSERT( NULL != m_pD3D->m_pSrcD3D9Ex );
			hr = m_pD3D->m_pSrcD3D9Ex->CreateDeviceEx
			(
				m_uAdapter,
				DevType,
				m_hFocusWindow,
				dwBeh,
				pD3DParams,
				pParams[ 0 ].bWindowed == true ? NULL : pFullScreenModes,
				&m_pDevice9Ex
			);
		}
		else
		{
			ASSERT( NULL != m_pD3D->m_pRefD3D9Ex );
			hr = m_pD3D->m_pRefD3D9Ex->CreateDeviceEx
			(
				m_uAdapter,
				DevType,
				m_hFocusWindow,
				dwBeh,
				pD3DParams,
				pParams[ 0 ].bWindowed == true ? NULL : pFullScreenModes,
				&m_pDevice9Ex
			);
		};

		if ( FAILED( hr ) )
		{
			DPF
			(
				1,
				_T("CDevice:Initialize9:  CreateDeviceEx failed with error 0x%x (%s).\n"),
				hr,
				m_pD3D->HResultToString( hr )
			);
		};
	}
	else
	{
		if ( ( pParams->dwInternalFlags & PPINTERNAL_DEVICE0 ) != 0 )
		{
			hr = m_pD3D->m_pSrcD3D9->CreateDevice
			(
				m_uAdapter,
				DevType,
				m_hFocusWindow,
				dwBeh,
				pD3DParams,
				&m_pDevice9
			);
		}
		else
		{
			hr = m_pD3D->m_pRefD3D9->CreateDevice
			(
				m_uAdapter,
				DevType,
				m_hFocusWindow,
				dwBeh,
				pD3DParams,
				&m_pDevice9
			);
		};


		if ( FAILED( hr ) )
		{
			DPF
			(
				1,
				_T("CDevice:Initialize9:  CreateDevice failed with error 0x%x (%s).\n"),
				hr,
				m_pD3D->HResultToString( hr )
			);
		};
	};


	//
	// Log details about CreateDevice/CreateDeviceEx failures.
	//

	if ( FAILED( hr ) )
	{
		if ( pD3DParams != NULL )
		{
			FORMAT fmwkBBFormat((FMT)(pD3DParams -> BackBufferFormat));

			DPF
			(
				1,
				_T("CDevice::Initialize9:  Present parameters:  %d x %d x %u bpp @ %d Hz\n"),
				pD3DParams->BackBufferWidth,
				pD3DParams->BackBufferHeight,
				fmwkBBFormat.ddpfPixelFormat.dwRGBBitCount,
				pD3DParams->FullScreen_RefreshRateInHz
			);
		};

		// Turn out-of-video-memory into E_UNSUPPORTED.
		if ( hr == D3DERR_OUTOFVIDEOMEMORY )
		{
			hr = E_UNSUPPORTED;
		};

		goto Exit;
	};

	
	//	
	// Update the caps to be device-specfic.
	//

	if ( !m_pD3D->DetectCaps( this ) )
	{
		DPF(1, _T("CDevice::Initialize9:  DetectCaps failed.\n"));
		goto Exit;
	};


	//
	// Wrap the backbuffers, z-buffers, etc.
	//

	if ( FAILED(hr = WrapDevice9( pParams ) ) )
	{
		DPF
		(
			1,
			_T("CDevice::Initialize9:  WrapDevice failed. hr=%s(%x)\n"),
			m_pD3D->HResultToString( hr ),
			hr
		);

		goto Exit;
	};


	//
	// Set "default" FVF to work around a ValidateDevice problem.
	//

	if ( FAILED( hr = m_pDevice9->SetFVF( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 ) ) )
	{
		DPF
		(
			1,
			_T("CDevice::Initialize9:  SetFVF failed. hr=%s(%x)\n"),
			m_pD3D->HResultToString( hr ),
			hr
		);

		goto Exit;
	};


	hr = S_OK;
	

Exit:
	SAFEDELETEA(pD3DParams);

	SAFEDELETEA(pFullScreenModes);

	if(FAILED(hr))
	{
		ReleaseD3D();
	}

	return hr;
};


// --------------------------------------------------------------------------------------

HRESULT CDevice::WrapDevice9(PRESENT_PARAMETERS *pParams)
{
	HRESULT hr = 0;

	// Set VP renderstate for Mixed mode
	if(m_dwBehaviors & DEVICECREATE_MIXED)
	{
		if(m_dwBehaviors & DEVICECREATE_HWVP)
			SetSoftwareVertexProcessing(FALSE);
		else
			SetSoftwareVertexProcessing(TRUE);
	}
	
	if ( pParams->dwInternalFlags & PPINTERNAL_CACHEBACKBUFFER )
	{
		// Wrap all the BackBuffers
		m_pBackBuffers = new CSurface*[pParams->uBackBufferCount];
		if(m_pBackBuffers == NULL)
		{
			DPF(1, _T("CDevice::Initialize9() - Out of memory.\n"));
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
				DPF(1, _T("CDevice::Initialize9() - GetBackBuffer(%d) failed. hr=%s(%x).\n"),
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
			DPF(1, _T("CDevice::Initialize9() - GetDepthStencilSurface() failed. hr=%s(%x).\n"),
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

HRESULT CDevice::ValidateMode9(DEVICEMODE *pMode)
{
	TCHAR szBuffer[64] = {0};

	if(((FMT)pMode->pDispMode->Format.d3dfFormat != FMT_A2R10G10B10 &&
		(FMT)pMode->pDispMode->Format.d3dfFormat != FMT_A2B10G10R10_XR_BIAS &&
		(FMT)pMode->pDispMode->Format.d3dfFormat != FMT_X8R8G8B8 &&
		(FMT)pMode->pDispMode->Format.d3dfFormat != FMT_R5G6B5) ||
	   (FAILED(m_pD3D->CheckDeviceType(pMode->nAdapter, pMode->dwDevType, pMode->pDispMode->Format, pMode->pDispMode->Format, TRUE)) &&
		FAILED(m_pD3D->CheckDeviceType(pMode->nAdapter, pMode->dwDevType, pMode->pDispMode->Format, pMode->pDispMode->Format, FALSE))))
	{
		FmtToString(FORMAT_TEXTURE,&pMode->pDispMode->Format,szBuffer);
		DPF(1, _T("D3D Device cannot render in the requested display mode(%s).\n"), szBuffer);
		return E_UNSUPPORTED;
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
		if ( (0xFFFF & (2<<8)) > (0xFFFF &	m_Desc.dwVertexShaderVersion) )
		{
			DPF(1, _T("Cannot enable pipeline emulation with hardware that does not support shader model 2.0.\n"));
			return E_UNSUPPORTED;
		}

		if ( DRIVERMODEL_LDDM != m_pD3D->GetDriverModel() )
		{						
			//Make sure we're on Longhorn
			if ( g_pFramework->m_DXGLog.GetSysInfo()->OSInf.fVersion < 6.0f )
			{
				DPF(1, _T("Cannot enable pipeline emulation without DX9L.\n"));
				return E_UNSUPPORTED;
			}

			if ( PPEMUREGCHANGE_ENABLE == m_dwPPEmuRegChange && DEVICECREATE_HWVP & pMode->dwBehaviors && VPEMUREGCHANGE_ENABLE != m_dwVPEmuRegChange )
			{
				DPF(1, _T("Cannot enable pixel pipeline emulation with hardware VP without the vertex pipeline emulation.\n"));
				return E_UNSUPPORTED;
			}
		}
		else
		{
			if ( PPEMUREGCHANGE_ENABLE == m_dwPPEmuRegChange && DEVICECREATE_HWVP & pMode->dwBehaviors && VPEMUREGCHANGE_DISABLE == m_dwVPEmuRegChange )
			{
				DPF(1, _T("Cannot enable pixel pipeline emulation with hardware VP without the vertex pipeline emulation.\n"));
				return E_UNSUPPORTED;
			}
		}
	}
	
	// MANAGED DEVICES PULLED FROM DX9
	/*if( (pMode->dwBehaviors & DEVICECREATE_MANAGED) && 
		!(m_Desc.dwCaps3 & D3DCAPS3_MANAGEDDEVICE) )
	{
		DPF(1, _T("Managed device not supported by this adapter.\n"));
		return E_UNSUPPORTED;
	}*/

	return S_OK;
}

// --------------------------------------------------------------------------------------

void CDevice::ReleaseWrappers9()
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

void CDevice::ReleaseD3D9()
{
	ReleaseWrappers9();

	if(m_pDevice9)
	{
		UINT uRef = 0;
		try
		{
			uRef = m_pDevice9->Release();
			m_pDevice9 = NULL;
		}
		catch ( ... )
		{
			DPF(1, _T("FAULT OCCURED in CDevice::ReleaseD3D9() - m_pDevice9->Release().\n"), uRef);
			SAFEDELETEA(m_pPresParams);
			g_pFramework->SuggestExitCode(EXITCODE_ABORT);
			return;
		}
		if(uRef)
		{
			DPF(1, _T("CDevice::ReleaseD3D9() - m_pDevice9 ref count is %d after final Release().\n"), uRef);

			ASSERT(g_pFramework != NULL);
			if(g_pFramework->m_bStressMode)
			{
				ASSERT(uRef == 0);
			}
		}
		
	}

	SAFEDELETEA(m_pPresParams);
	// Reset the state
	m_bFullscreen = false;
}


// --------------------------------------------------------------------------------------
bool CDevice::IsDeviceLost9()
{
	return FAILED(TestCooperativeLevel());
}


// --------------------------------------------------------------------------------------
bool CDevice::IsDeviceLost9L()
{
	return FAILED(CheckDeviceState(m_hFocusWindow));
}


// --------------------------------------------------------------------------------------
bool CDevice::IsDeviceReadyForReset9()
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
bool CDevice::IsDeviceReadyForReset9L()
{
	HRESULT hDeviceState = E_FAIL;

	hDeviceState = CheckDeviceState(m_hFocusWindow);

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
bool CDevice::HandleDeviceLost9()
{
	HRESULT hr;

	hr = m_fInterface > 9.0f? CheckDeviceState( m_hFocusWindow ) : TestCooperativeLevel();
	
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
			ReleaseWrappers9();

			if(FAILED(Reset(m_pPresParams)))
			{
				DPF(1, _T("CDevice::HandleDeviceLost9() - Reset failed. hr=%s(%x).\n"),
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
			DPF(2, _T("CDevice::HandleDeviceLost9() - TestCooperativeLevel() failed with %s(%x).\n"),
				m_pD3D->HResultToString(hr), hr);
		}
	}
	return false;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::D3DXFontOnLostDevice9()
{
	if(m_pD3DXFont)
	{
		D3DXHANDLEVERSION(return, ID3DXFont, m_pD3DXFont, OnLostDevice());
	}

	return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::D3DXFontOnResetDevice9()
{
	if(m_pD3DXFont)
	{
		D3DXHANDLEVERSION(return, ID3DXFont, m_pD3DXFont, OnResetDevice());
	}

	return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::Resize9(UINT uWidth, UINT uHeight)
{
	HRESULT hr;

	// Release all wrappers we have for the Device
	ReleaseWrappers9();

	m_pPresParams->uWidth = uWidth;
	m_pPresParams->uHeight = uHeight;

	hr = Reset(m_pPresParams);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::Resize9() - Reset failed. hr=%s(%x).\n"),
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

bool CDevice::TextureFormatSupported9(FORMAT *pFmt)
{
	if(pFmt == NULL)
	{
		DPF(1, _T("CDevice::TextureFormatSupported9() - Invalid params.\n"));
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

bool CDevice::ZBufferFormatSupported9(FORMAT *pFmt)
{
	if(pFmt == NULL)
	{
		DPF(1, _T("CDevice::ZBufferFormatSupported9() - Invalid params.\n"));
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

bool CDevice::CubeFormatSupported9(FORMAT *pFmt)
{
	if(pFmt == NULL)
	{
		DPF(1, _T("CDevice::CubeFormatSupported9() - Invalid params.\n"));
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

bool CDevice::VolumeFormatSupported9(FORMAT *pFmt)
{
	if(pFmt == NULL)
	{
		DPF(1, _T("CDevice::VolumeFormatSupported9() - Invalid params.\n"));
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

bool CDevice::Draw2DText9(DWORD dwColor, TCHAR *szString, int nLen, RECT *pRect, DWORD dwFormat)
{
	HRESULT hr = S_OK;
	INT nRes = 0;
	bool bRes = true;
	
	if(!szString || !pRect)
	{
		DPF(1, _T("CDevice::Draw2DText9() - invalid params.\n"));
		return false;
	}
	
	// Create the font if needed
	if(m_pD3DXFont == NULL)
	{
		hr = ::D3DXCreateFontIndirect8And90(m_pDevice9, & m_LogFont, &m_pD3DXFont);
		if(FAILED(hr))
		{
			DPF(1, _T("CDevice::Draw2DText9() - D3DXCreateFont failed. hr=%s(%x).\n"),
						   m_pD3D->HResultToString(hr), hr); 
			return false;
		}
	}

	
	if(g_fD3DX > 9.19f)
		nRes = ((ID3DXFont92*)m_pD3DXFont)->DrawText(NULL, szString, nLen, pRect, dwFormat, dwColor);
	else if (g_fD3DX < 9.19f && g_fD3DX > 9.09f)
		nRes = ((ID3DXFont91*)m_pD3DXFont)->DrawText(NULL, szString, nLen, pRect, dwFormat, dwColor);
	else
		nRes = ((ID3DXFont90*)m_pD3DXFont)->DrawText(szString, nLen, pRect, dwFormat, dwColor);

	if(0 == nRes)
	{
		DPF(1, _T("CDevice::Draw2DText9() - D3DXDrawText failed.\n")); 
		return false;
	}

	return true;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::QueryInterface9(REFIID riid, void** ppvObj)
{
	return SetLastError(m_pDevice9->QueryInterface(riid, ppvObj));
}

// --------------------------------------------------------------------------------------

bool CDevice::IsDeviceEqual9(PVOID pDevice)
{
	if(m_pDevice9 == (LPDIRECT3DDEVICE9) pDevice)
		return true;
	else
		return false;
}

// --------------------------------------------------------------------------------------

ULONG CDevice::AddRef9(void)
{
	return m_pDevice9->AddRef( );
}

// --------------------------------------------------------------------------------------

ULONG CDevice::Release9(void)
{
	return m_pDevice9->Release( );
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::TestCooperativeLevel9(void)
{
	return SetLastError(m_pDevice9->TestCooperativeLevel( ));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::EvictManagedResources9()
{
	return SetLastError(m_pDevice9->EvictManagedResources());
}

// --------------------------------------------------------------------------------------

UINT CDevice::GetAvailableTextureMem9(void)
{
	return m_pDevice9->GetAvailableTextureMem( );
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetDirect3D9(IDirect3D9** ppD3D9)
{
	return SetLastError(m_pDevice9->GetDirect3D(ppD3D9));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetDeviceCaps9(CAPS* pCaps)
{
	return SetLastError( m_pDevice9->GetDeviceCaps((D3DCAPS9*)pCaps) );
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetDisplayMode8To9(DISPLAYMODE* pMode)
{
	D3DDISPLAYMODE Mode;

	HRESULT hr=m_pDevice9->GetDisplayMode(0, pMode != NULL ? &Mode : NULL);
	
	if( pMode != NULL )
	{
		pMode->dwWidth	= Mode.Width;
		pMode->dwHeight = Mode.Height;
		pMode->dwRefreshRate = Mode.RefreshRate;
		pMode->Format.d3dfFormat = Mode.Format;
		d3df2ddpf(Mode.Format,&pMode->Format.ddpfPixelFormat);
		pMode->dwBPP = pMode->Format.ddpfPixelFormat.dwRGBBitCount;
	}

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetDisplayMode9(UINT uSwapchain, DISPLAYMODE* pMode)
{
	D3DDISPLAYMODE Mode;

	HRESULT hr=m_pDevice9->GetDisplayMode(uSwapchain, pMode != NULL ? &Mode : NULL);
	
	if( pMode != NULL )
	{
		pMode->dwWidth	= Mode.Width;
		pMode->dwHeight = Mode.Height;
		pMode->dwRefreshRate = Mode.RefreshRate;
		pMode->Format.d3dfFormat = Mode.Format;
		d3df2ddpf(Mode.Format,&pMode->Format.ddpfPixelFormat);
		pMode->dwBPP = pMode->Format.ddpfPixelFormat.dwRGBBitCount;
	}

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetCreationParameters9(DEVICE_CREATION_PARAMETERS *pParameters)
{
	return SetLastError(m_pDevice9->GetCreationParameters(pParameters));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetCursorProperties9(UINT XHotSpot,UINT YHotSpot,
									  CSurface* pCursorBitmap)
{
	return SetLastError(m_pDevice9->SetCursorProperties(XHotSpot,YHotSpot,pCursorBitmap->GetD3DSurface9()));
}

// --------------------------------------------------------------------------------------

void CDevice::SetCursorPosition9(UINT XScreenSpace,UINT YScreenSpace,DWORD Flags)
{
	m_pDevice9->SetCursorPosition(XScreenSpace,YScreenSpace,Flags);
}

// --------------------------------------------------------------------------------------

BOOL CDevice::ShowCursor9(BOOL bShow)
{
	return m_pDevice9->ShowCursor(bShow);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::Reset9(PRESENT_PARAMETERS* pPresentationParameters)
{
	HRESULT hr = S_OK;
	UINT nNumAdapters = 0;
    if ( ( m_dwBehaviors & DEVICECREATE_ADAPTERGROUP_DEVICE ) == DEVICECREATE_ADAPTERGROUP_DEVICE )
	{
		nNumAdapters = GetCaps()->NumberOfAdaptersInGroup;

		// We can only drive multiple heads from the master.
		if( nNumAdapters == 0 )
		{
			DPF
			(
				1,
				_T("CDevice::ResetEx9:  Can't drive multiple heads from slave head.\n")
			);

			hr = E_UNSUPPORTED;
			goto Exit;		
		};

/*
		DPF
		(
			3,
			_T( "CDevice::Initialize9:  Initializing adapter with %u heads.\n" ),
			nNumAdapters
		);
*/
	}
	else
	{
		nNumAdapters = 1;

/*
		DPF
		(
			3,
			_T( "CDevice::Initialize9:  Initializing single-head adapter.\n" )
		);
*/
	};

#if 0
	// we have to reroute Reset to ResetEx for 9L devices
	if ( FLOAT_EQUAL( m_pD3D->GetVersion(), 9.1f ) )
	{
		D3DDISPLAYMODEEX *pFullScreenModes = NULL;
		if( !pPresentationParameters[0].bWindowed )
		{
			pFullScreenModes = new D3DDISPLAYMODEEX[nNumAdapters];
			if( NULL == pFullScreenModes )
			{
				DPF(1, _T("CDevice::Reset9() - Out of memory.\n"));
				return E_OUTOFMEMORY;
			}
			for( UINT nAdapter = 0; nAdapter < nNumAdapters; nAdapter++ )
			{
				pFullScreenModes[nAdapter].Size = sizeof( D3DDISPLAYMODEEX );
				pFullScreenModes[nAdapter].ScanLineOrdering = D3DSCANLINEORDERING_UNKNOWN;
				pFullScreenModes[nAdapter].Width = pPresentationParameters[nAdapter].uWidth;
				pFullScreenModes[nAdapter].Height = pPresentationParameters[nAdapter].uHeight;
				pFullScreenModes[nAdapter].Format = pPresentationParameters[nAdapter].BackBufferFormat.d3dfFormat;
				pFullScreenModes[nAdapter].RefreshRate = pPresentationParameters[nAdapter].uFullscreenRefresh;
			}
		}

		hr = ResetEx9( pPresentationParameters, pFullScreenModes );

		SAFEDELETEA( pFullScreenModes );
	}
	else
#endif
	{
		D3DPRESENT_PARAMETERS *pd3dpp = new D3DPRESENT_PARAMETERS[nNumAdapters];
		if( NULL == pd3dpp )
		{
			DPF(1, _T("CDevice::Reset9() - Out of memory.\n"));
			return E_OUTOFMEMORY;
		}
		memset( pd3dpp, 0, sizeof(D3DPRESENT_PARAMETERS) * nNumAdapters );
	
		for(int i=0; i<nNumAdapters; i++)
		{
			pd3dpp[i].BackBufferWidth					= pPresentationParameters[i].uWidth;
			pd3dpp[i].BackBufferHeight					= pPresentationParameters[i].uHeight;
			pd3dpp[i].BackBufferFormat					= pPresentationParameters[i].BackBufferFormat.d3dfFormat;
			pd3dpp[i].BackBufferCount					= pPresentationParameters[i].uBackBufferCount;
			pd3dpp[i].MultiSampleType					= (D3DMULTISAMPLE_TYPE)pPresentationParameters[i].MultiSample;
			pd3dpp[i].SwapEffect						= (D3DSWAPEFFECT)pPresentationParameters[i].SwapEffect;
			pd3dpp[i].hDeviceWindow 					= pPresentationParameters[i].hDeviceWindow;
			pd3dpp[i].Windowed							= pPresentationParameters[i].bWindowed;
			pd3dpp[i].EnableAutoDepthStencil			= pPresentationParameters[i].bAutoDepthStencil;
			pd3dpp[i].AutoDepthStencilFormat			= pPresentationParameters[i].AutoDepthStencilFormat.d3dfFormat;
			pd3dpp[i].Flags 							= pPresentationParameters[i].dwFlags;
			pd3dpp[i].FullScreen_RefreshRateInHz		= pPresentationParameters[i].uFullscreenRefresh;
			pd3dpp[i].PresentationInterval				= pPresentationParameters[i].uPresentInterval;
		}

		hr = m_pDevice9->Reset(pPresentationParameters == NULL ? NULL : pd3dpp);

		SAFEDELETEA(pd3dpp);
	}

Exit:
    return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::Present9(RECT* pSourceRect, RECT* pDestRect,
						  HWND hDestWindowOverride, RGNDATA* pDirtyRegion)
{
	return SetLastError(m_pDevice9->Present(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetBackBuffer8To9(UINT BackBuffer,DWORD dwType, CSurface** ppBackBuffer)
{
	LPDIRECT3DSURFACE9 pSurf = NULL;
	CSurface* pBackBuffer = NULL;

	// Alloc the surface wrapper
	pBackBuffer = new CSurface();
	if(pBackBuffer == NULL)
	{
		DPF(1, _T("CDevice::GetBackBuffer8To9() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}

	// Get the real backbuffer ptr
	HRESULT hr = m_pDevice9->GetBackBuffer(0, BackBuffer, (D3DBACKBUFFER_TYPE)dwType, ppBackBuffer == 0L ? 0L : &pSurf);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::GetBackBuffer8To9() - GetBackBuffer(%d) failed. hr=%s(%x).\n"),
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
		DPF(1, _T("CDevice::GetBackBuffer8To9() - CSurface::Init failed for backbuffer #%d.\n"), BackBuffer);
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

HRESULT CDevice::GetBackBuffer9(UINT iSwapChain, UINT BackBuffer,DWORD dwType, CSurface** ppBackBuffer)
{
	LPDIRECT3DSURFACE9 pSurf = NULL;
	CSurface* pBackBuffer = NULL;

	// Alloc the surface wrapper
	pBackBuffer = new CSurface();
	if(pBackBuffer == NULL)
	{
		DPF(1, _T("CDevice::GetBackBuffer9() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}

	// Get the real backbuffer ptr
	HRESULT hr = m_pDevice9->GetBackBuffer(iSwapChain, BackBuffer, (D3DBACKBUFFER_TYPE)dwType, ppBackBuffer == 0L ? 0L : &pSurf);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::GetBackBuffer9() - GetBackBuffer(%d) failed. hr=%s(%x).\n"),
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
		DPF(1, _T("CDevice::GetBackBuffer9() - CSurface::Init failed for backbuffer #%d.\n"), BackBuffer);
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

HRESULT CDevice::GetRasterStatus8To9(RASTER_STATUS* pRasterStatus)
{
	return SetLastError(m_pDevice9->GetRasterStatus(0, pRasterStatus));
}

// --------------------------------------------------------------------------------------

void CDevice::SetGammaRamp8To9(DWORD Flags,GAMMARAMP* pRamp)
{
	m_pDevice9->SetGammaRamp(0, Flags,pRamp);
}

// --------------------------------------------------------------------------------------

void CDevice::GetGammaRamp8To9(GAMMARAMP* pRamp)
{
	m_pDevice9->GetGammaRamp(0, pRamp);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetRasterStatus9(UINT uSwapChain, RASTER_STATUS* pRasterStatus)
{
	return SetLastError(m_pDevice9->GetRasterStatus(uSwapChain, pRasterStatus));
}

// --------------------------------------------------------------------------------------

void CDevice::SetGammaRamp9(UINT uSwapChain, DWORD Flags,GAMMARAMP* pRamp)
{
	m_pDevice9->SetGammaRamp(uSwapChain, Flags,pRamp);
}

// --------------------------------------------------------------------------------------

void CDevice::GetGammaRamp9(UINT uSwapChain, GAMMARAMP* pRamp)
{
	m_pDevice9->GetGammaRamp(uSwapChain, pRamp);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateImageSurface9(UINT Width,UINT Height,FORMAT Format,
									 CSurface** ppSurface)
{
	return CreateOffscreenPlainSurface(Width, Height, Format, POOL_SCRATCH, ppSurface, NULL);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateTexture8To9(UINT Width,UINT Height,UINT Levels,DWORD Usage,
								FORMAT Format,DWORD Pool,CTexture** ppTexture)
{
	HRESULT hr = CreateTexture9(Width, Height, Levels, Usage, Format, Pool, ppTexture, NULL);
	if (FAILED(hr))
	{
		DPF(2, _T("CDevice::CreateTexture8To9() - CreateTexture9() failed.\n") );
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateTexture9(UINT Width,UINT Height,UINT Levels,DWORD Usage,
								FORMAT Format,DWORD Pool,CTexture** ppTexture,HANDLE *pSharedHandle)
{
	HRESULT hr;
	CTexture *pTexture = NULL, *pTexOut = NULL;
	LPDIRECT3DTEXTURE9 pD3DTexture = NULL;
	LPDIRECT3DTEXTURE9 pTempTexture = NULL;
	LPDIRECT3DTEXTURE9 *ppD3DTexture = &pD3DTexture;
	D3DPOOL d3dpool;

	if (ppTexture)
	{
		if (*ppTexture && pSharedHandle)
		{
			pTempTexture = (LPDIRECT3DTEXTURE9)(*ppTexture)->GetD3DTexture9();
			ppD3DTexture = &pTempTexture;
		}
	}

	//
	// Create target texture
	//

	pTexture = new CTexture;
	if(pTexture == NULL)
	{
		DPF(2, _T("CDevice::CreateTexture9() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}
	
	if (FMWK_DEFAULT == Pool || POOL_DEFAULT_NOBACKINGSTORE == Pool)
	{
		d3dpool = D3DPOOL_DEFAULT;
	}
	else if (FMWK_VSTEXTURE == Pool)
	{
		if ( (m_dwBehaviors & DEVICECREATE_SWVP) || ((m_dwBehaviors & DEVICECREATE_MIXED) && !(m_dwBehaviors & DEVICECREATE_HWVP)))
			d3dpool = D3DPOOL_SCRATCH;
		else
			d3dpool = D3DPOOL_DEFAULT;
	}
	else
		d3dpool = (D3DPOOL)Pool;

	if (FMWK_DEFAULT == Usage)
	{
		Usage = 0;
	}

	// D3DPOOL_MANAGED no longer available; use POOL_MANAGED_INTERNAL (6) instead
	if ( FLOAT_EQUAL( m_pD3D->GetVersion(), 9.1f ) && POOL_MANAGED == Pool )
		d3dpool = (D3DPOOL)POOL_MANAGED_INTERNAL;

	hr = m_pDevice9->CreateTexture(Width,Height,Levels,Usage,Format.d3dfFormat,d3dpool, (ppTexture == 0L) ? 0 : ppD3DTexture, pSharedHandle);
	if(FAILED(hr) || *ppD3DTexture == NULL)
	{
		DPF(2, _T("CDevice::CreateTexture9() - CDevice::CreateTexture() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	if(!pTexture->Init(*ppD3DTexture, this))
	{
		DPF(2, _T("CDevice::CreateTexture9() - CTexture::Init() failed.\n") );
		hr = E_FAIL;
		goto Exit;
	}
	
	RELEASE(pD3DTexture);
	RELEASE(pTempTexture);

	pTexOut = pTexture;

	//
	// Create sysmem texture
	//

	if ((d3dpool != D3DPOOL_SCRATCH) &&
		(d3dpool != D3DPOOL_MANAGED) &&
		(d3dpool != (D3DPOOL)POOL_MANAGED_INTERNAL) &&
		(Pool != POOL_DEFAULT_NOBACKINGSTORE) &&
		!(Format.ddpfPixelFormat.dwFlags & PF_METN) &&
		!(Format.ddpfPixelFormat.dwFlags & PF_UNKNOWNFOURCC) &&
		((Usage & USAGE_DEPTHSTENCIL) == 0))
	{
		pTexture = new CTexture;
		if(pTexture == NULL)
		{
			DPF(2, _T("CDevice::CreateTexture9() - SysMem Copy - Out of memory.\n"));
			hr = E_OUTOFMEMORY;
			goto Exit;
		}
	
		// Remove all usages
		Usage = 0;

		hr = m_pDevice9->CreateTexture(Width,Height,Levels,Usage,Format.d3dfFormat,D3DPOOL_SYSTEMMEM,&pD3DTexture,NULL);
		if(FAILED(hr))
		{
			DPF(2, _T("CDevice::CreateTexture9() - SysMem Copy - CDevice::CreateTexture9() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
		
		if(!pTexture->Init(pD3DTexture, this))
		{
			DPF(2, _T("CDevice::CreateTexture9() - SysMem Copy - CTexture::Init() failed.\n") );
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
	else if(ppTexture != NULL)
		*ppTexture = pTexOut;

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateVolumeTexture8To9(UINT Width,UINT Height,UINT Depth,UINT Levels,
									  DWORD Usage,FORMAT Format,DWORD Pool,
									  CVolTexture** ppVolumeTexture)
{
	HRESULT hr = CreateVolumeTexture9(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, NULL);
	if (FAILED(hr))
	{
		DPF(2, _T("CDevice::CreateVolumeTexture8To9() - CreateVolumeTexture9() failed.\n") );
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateVolumeTexture9(UINT Width,UINT Height,UINT Depth,UINT Levels,
									  DWORD Usage,FORMAT Format,DWORD Pool,
									  CVolTexture** ppVolumeTexture,HANDLE *pSharedHandle)
{
	HRESULT hr;
	CVolTexture *pTexture = NULL, *pTexOut = NULL;
	LPDIRECT3DVOLUMETEXTURE9 pD3DTexture = NULL;
	LPDIRECT3DVOLUMETEXTURE9 pTempTexture = NULL;
	LPDIRECT3DVOLUMETEXTURE9 *ppD3DTexture = &pD3DTexture;
	
	if(ppVolumeTexture)
	{
		if (*ppVolumeTexture && pSharedHandle)
		{
			pTempTexture = (LPDIRECT3DVOLUMETEXTURE9)(*ppVolumeTexture)->GetD3DTexture9();
			ppD3DTexture = &pTempTexture;
		}
	}
	
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
	{
		Pool = POOL_DEFAULT;
	}
	else if (FMWK_VSTEXTURE == Pool)
	{
		if ( (m_dwBehaviors & DEVICECREATE_SWVP) || ((m_dwBehaviors & DEVICECREATE_MIXED) && !(m_dwBehaviors & DEVICECREATE_HWVP)))
			Pool = POOL_SCRATCH;
		else
			Pool = POOL_DEFAULT;
	}

	if (FMWK_DEFAULT == Usage)
	{
		Usage = 0;
	}

	// D3DPOOL_MANAGED no longer available; use POOL_MANAGED_INTERNAL (6) instead
	if ( FLOAT_EQUAL( m_pD3D->GetVersion(), 9.1f ) && POOL_MANAGED == Pool )
		Pool = POOL_MANAGED_INTERNAL;

	pD3DTexture = NULL;
	hr = m_pDevice9->CreateVolumeTexture(Width,Height,Depth,Levels,Usage,Format.d3dfFormat,(D3DPOOL)Pool,ppD3DTexture,pSharedHandle);
	if(FAILED(hr) || *ppD3DTexture == NULL)
	{
		DPF(1, _T("CDevice::CreateVolumeTexture() - CDevice::CreateVolumeTexture() failed.\n") );
		goto Exit;
	}

	if(!pTexture->Init(*ppD3DTexture, this))
	{
		DPF(1, _T("CDevice::CreateVolumeTexture() - CVolTexture::Init() failed.\n") );
		hr = E_FAIL;
		goto Exit;
	}

	RELEASE(pD3DTexture);
	RELEASE(pTempTexture);

	pTexOut = pTexture;

	//
	// Create the sysmem texture
	//
	
	if ((Pool != POOL_SCRATCH) && (Pool != POOL_MANAGED) && !(Format.ddpfPixelFormat.dwFlags & PF_METN) && !(Format.ddpfPixelFormat.dwFlags & PF_UNKNOWNFOURCC) && ((Usage & USAGE_DEPTHSTENCIL) == 0))
	{
		pTexture = new CVolTexture;
		if(pTexture == NULL)
		{
			DPF(1, _T("CDevice::CreateVolumeTexture() - SysMem Copy - Out of memory.\n"));
			return SetLastError(E_FAIL);
		}

		// Remove all usages
		Usage = 0;

		pD3DTexture = NULL;
		hr = m_pDevice9->CreateVolumeTexture(Width,Height,Depth,Levels,Usage,Format.d3dfFormat, D3DPOOL_SYSTEMMEM, &pD3DTexture, NULL);
		if( FAILED( hr ) )
		{
			DPF(1, _T("CDevice::CreateVolumeTexture() - SysMem Copy - CDevice::CreateVolumeTexture() failed.\n") );
			goto Exit;
		}

		if(!pTexture->Init(pD3DTexture, this))
		{
			DPF(1, _T("CDevice::CreateVolumeTexture() - SysMem Copy - CVolTexture::Init() failed.\n") );
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
	else if(ppVolumeTexture != NULL)
		*ppVolumeTexture = pTexOut;

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateCubeTexture8To9(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,
									DWORD Pool,CCubeTexture** ppCubeTexture)
{
	HRESULT hr = CreateCubeTexture9(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, NULL);
	if (FAILED(hr))
	{
		DPF(2, _T("CDevice::CreateCubeTexture8To9() - CreateCubeTexture9() failed.\n") );
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateCubeTexture9(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,
									DWORD Pool,CCubeTexture** ppCubeTexture,HANDLE *pSharedHandle)
{
	HRESULT hr;
	CCubeTexture* pTexture = NULL, *pTexOut = NULL;
	LPDIRECT3DCUBETEXTURE9 pD3DTexture = NULL;
	LPDIRECT3DCUBETEXTURE9 pTempTexture = NULL;
	LPDIRECT3DCUBETEXTURE9 *ppD3DTexture = &pD3DTexture;
	
	if (ppCubeTexture)
	{
		if (*ppCubeTexture && pSharedHandle)
		{
			pTempTexture = (LPDIRECT3DCUBETEXTURE9)(*ppCubeTexture)->GetD3DTexture9();
			ppD3DTexture = &pTempTexture;
		}
	}
	
	// 
	// Create the target texture
	//

	pTexture = new CCubeTexture;
	if(pTexture == NULL)
	{
		DPF(1, _T("CDevice::CreateCubeTexture() - Out of memory.\n"));
		return SetLastError(E_OUTOFMEMORY);
	}
	
	if(FMWK_DEFAULT == Pool)
	{
		Pool = POOL_DEFAULT;
	}
	else if (FMWK_VSTEXTURE == Pool)
	{
		if ( (m_dwBehaviors & DEVICECREATE_SWVP) || ((m_dwBehaviors & DEVICECREATE_MIXED) && !(m_dwBehaviors & DEVICECREATE_HWVP)))
			Pool = POOL_SCRATCH;
		else
			Pool = POOL_DEFAULT;
	}

	if (FMWK_DEFAULT == Usage)
	{
		Usage = 0;
	}

	// D3DPOOL_MANAGED no longer available; use POOL_MANAGED_INTERNAL (6) instead
	if ( FLOAT_EQUAL( m_pD3D->GetVersion(), 9.1f ) && POOL_MANAGED == Pool )
		Pool = POOL_MANAGED_INTERNAL;

	hr = m_pDevice9->CreateCubeTexture(EdgeLength,Levels,Usage,Format.d3dfFormat,(D3DPOOL)Pool,ppD3DTexture, pSharedHandle);
	if(FAILED(hr) || NULL == *ppD3DTexture)
	{
		DPF(1, _T("CDevice::CreateCubeTexture() - CDevice::CreateCubeTexture() failed.\n") );
		goto Exit;
	}

	if(!pTexture->Init(*ppD3DTexture, this))
	{
		DPF(1, _T("CDevice::CreateCubeTexture() - CCubeTexture::Init() failed.\n") );
		hr = E_FAIL;
		goto Exit;
	}
	RELEASE(pD3DTexture);
	RELEASE(pTempTexture);

	pTexOut = pTexture;

	//
	// Create the sysmem copy
	//

	if ((Pool != POOL_SCRATCH) && (Pool != POOL_MANAGED) && !(Format.ddpfPixelFormat.dwFlags & PF_METN) && !(Format.ddpfPixelFormat.dwFlags & PF_UNKNOWNFOURCC) && ((Usage & USAGE_DEPTHSTENCIL) == 0))
	{
		pTexture = new CCubeTexture;
		if(pTexture == NULL)
		{
			DPF(1, _T("CDevice::CreateCubeTexture() - SysMem Copy - Out of memory.\n"));
			return SetLastError(E_OUTOFMEMORY);
		}
		
		// Remove all usages
		Usage = 0;

		hr = m_pDevice9->CreateCubeTexture(EdgeLength,Levels,Usage,Format.d3dfFormat, D3DPOOL_SYSTEMMEM, &pD3DTexture, NULL);
		if(FAILED(hr))
		{
			DPF(1, _T("CDevice::CreateCubeTexture() - SysMem Copy - CDevice::CreateCubeTexture() failed.\n") );
			goto Exit;
		}

		if(!pTexture->Init(pD3DTexture, this))
		{
			DPF(1, _T("CDevice::CreateCubeTexture() - SysMem Copy - CCubeTexture::Init() failed.\n") );
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
	else if(ppCubeTexture != NULL)
		*ppCubeTexture = pTexOut;

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateVertexBuffer8To9(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB)
{
	HRESULT hr = CreateVertexBuffer9(Length, Usage, FVF, Pool, ppVB, NULL);
	if (FAILED(hr))
	{
		DPF(2, _T("CDevice::CreateVertexBuffer8To9() - CreateVertexBuffer9() failed.\n") );
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateVertexBuffer9(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB, HANDLE *pSharedHandle)
{
	CVertexBuffer*				pVB = NULL;
	HRESULT 					hr;

	// D3DPOOL_MANAGED no longer available; use POOL_MANAGED_INTERNAL (6) instead
	if ( FLOAT_EQUAL( m_pD3D->GetVersion(), 9.1f ) && POOL_MANAGED == Pool )
		Pool = POOL_MANAGED_INTERNAL;
	D3DPOOL 					d3dPool = (D3DPOOL)Pool;

	LPDIRECT3DVERTEXBUFFER9 pD3DVB = NULL;
	LPDIRECT3DVERTEXBUFFER9 pTempVB = NULL;
	LPDIRECT3DVERTEXBUFFER9 *ppD3DVB = &pD3DVB;
	
	if (ppVB)
	{
		if (*ppVB && pSharedHandle)
		{
			pTempVB = (*ppVB)->GetVertexBufferPointer9();
			ppD3DVB = &pTempVB;
		}
	}


	if (Pool == FMWK_DEFAULT)
	{
		// MANAGED DEVICES PULLED FROM DX9
		/*if(m_dwBehaviors & DEVICECREATE_MANAGED)
			d3dPool = D3DPOOL_MANAGED;
		else*/
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

	hr = m_pDevice9->CreateVertexBuffer(Length, Usage, FVF, d3dPool, ppD3DVB, pSharedHandle);
	if ( FAILED(hr) || NULL == ppD3DVB)
	{
		DPF(1, _T("CDevice::CreateVertexBuffer() - CDevice::CreateVertexBuffer() failed.\n") );
		RELEASE( pVB );
		return SetLastError(hr);
	}

	if (pVB->Initialize(*ppD3DVB, this))
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
		RELEASE( pTempVB );
		return SetLastError(E_FAIL);
	}
	
	RELEASE( pD3DVB );
	RELEASE( pTempVB );

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateIndexBuffer8To9(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB)
{
	HRESULT hr = CreateIndexBuffer9(Length, Usage, Format, Pool, ppIB, NULL);
	if (FAILED(hr))
	{
		DPF(2, _T("CDevice::CreateIndexBuffer8To9() - CreateIndexBuffer9() failed.\n") );
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateIndexBuffer9(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB, HANDLE *pSharedHandle)
{
	CIndexBuffer*				pIB = NULL;
	HRESULT 					hr;

	// D3DPOOL_MANAGED no longer available; use POOL_MANAGED_INTERNAL (6) instead
	if ( FLOAT_EQUAL( m_pD3D->GetVersion(), 9.1f ) && POOL_MANAGED == Pool )
		Pool = POOL_MANAGED_INTERNAL;
	D3DPOOL 					d3dPool = (D3DPOOL)Pool;

	LPDIRECT3DINDEXBUFFER9 pD3DIB = NULL;
	LPDIRECT3DINDEXBUFFER9 pTempIB = NULL;
	LPDIRECT3DINDEXBUFFER9 *ppD3DIB = NULL;
	
	if (ppIB)
	{
		if (*ppIB && pSharedHandle)
		{
			pTempIB = (*ppIB)->GetIndexBufferPointer9();
			ppD3DIB = &pTempIB;
		}
		else
		{
			ppD3DIB = &pD3DIB;
		}
	}

	if (Pool == FMWK_DEFAULT)
	{
		/*if(m_dwBehaviors & DEVICECREATE_MANAGED)
			d3dPool = D3DPOOL_MANAGED;
		else*/
		// MANAGED DEVICES PULLED FROM DX9
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

	hr = m_pDevice9->CreateIndexBuffer(Length, Usage, (D3DFORMAT)Format, d3dPool, ppD3DIB, pSharedHandle);
	if ( FAILED(hr)||0L==ppIB)
	{
		DPF(1, _T("CDevice::CreateIndexBuffer() - CDevice::CreateIndexBuffer() failed.\n") );
		RELEASE( pIB );
		return SetLastError(hr);
	}

	if (pIB->Initialize(*ppD3DIB, this))
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
		RELEASE( pTempIB );
		return SetLastError(E_FAIL);
	}
	
	RELEASE( pD3DIB );
	RELEASE( pTempIB );

	return hr;
}


// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateRenderTarget8To9(UINT Width,UINT Height,FORMAT Format,
									 MULTISAMPLE_TYPE MultiSample,BOOL Lockable,
									 CSurface** ppSurface)
{
	LPDIRECT3DSURFACE9 pD3DSurface = NULL;
	CSurface* pSurface = new CSurface;
	if( pSurface == NULL )
	{
		DPF(1, _T("CDevice::CreateRenderTarget8to9() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}

	HRESULT hr = m_pDevice9->CreateRenderTarget(Width,Height,Format.d3dfFormat,(D3DMULTISAMPLE_TYPE)MultiSample,0,Lockable,ppSurface==0L?0L:&pD3DSurface, NULL);
	if( FAILED( hr ) || 0L == ppSurface )
	{
		DPF(1, _T("CDevice::CreateRenderTarget8to9() - CreateRenderTarget() failed.\n"));
		RELEASE(pSurface);
		return SetLastError(hr);
	}

	if( !pSurface->Init(pD3DSurface, this, NULL) )
	{
		DPF(1, _T("CDevice::CreateRenderTarget8to9() - CSurface::Init() failed.\n"));
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

HRESULT CDevice::CreateRenderTarget9(UINT Width,UINT Height,FORMAT Format,
									 MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,
									 BOOL Lockable,CSurface** ppSurface, HANDLE *pSharedHandle)
{
	LPDIRECT3DSURFACE9 pD3DSurface = NULL;
	LPDIRECT3DSURFACE9 pTempSurface = NULL;
	LPDIRECT3DSURFACE9 *ppD3DSurface = NULL;
	CSurface* pSurface = new CSurface;
	
	if( pSurface == NULL )
	{
		DPF(1, _T("CDevice::CreateRenderTarget9() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}

	if (ppSurface)
	{
		if (*ppSurface && pSharedHandle)
		{
			pTempSurface = (*ppSurface)->GetD3DSurface9();
			ppD3DSurface = &pTempSurface;
		}
		else
		{
			ppD3DSurface = &pD3DSurface;
		}
	}

	HRESULT hr = m_pDevice9->CreateRenderTarget(Width,Height,Format.d3dfFormat,(D3DMULTISAMPLE_TYPE)MultiSample,MultisampleQuality,Lockable,ppD3DSurface, pSharedHandle);
	if( FAILED( hr ) || 0L == ppSurface )
	{
		DPF(1, _T("CDevice::CreateRenderTarget9() - CreateRenderTarget() failed.\n"));
		RELEASE(pSurface);
		return SetLastError(hr);
	}

	if( !pSurface->Init(*ppD3DSurface, this, NULL) )
	{
		DPF(1, _T("CDevice::CreateRenderTarget9() - CSurface::Init() failed.\n"));
		RELEASE( pD3DSurface );
		RELEASE(pSurface);
		RELEASE(pTempSurface);
		return SetLastError(E_FAIL);
	}

	if( ppSurface )
		*ppSurface = pSurface;

	RELEASE(pD3DSurface);
	RELEASE(pTempSurface);

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateDepthStencilSurface8To9(UINT Width,UINT Height,FORMAT Format,
											MULTISAMPLE_TYPE MultiSample,
											CSurface** ppSurface)
{
	LPDIRECT3DSURFACE9 pD3DSurface = NULL;
	CSurface* pSurface = new CSurface;
	if( pSurface == NULL )
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface8to9() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}
	
	HRESULT hr = m_pDevice9->CreateDepthStencilSurface(Width,Height,Format.d3dfFormat,(D3DMULTISAMPLE_TYPE)MultiSample,0,FALSE,ppSurface==0L?0L:&pD3DSurface, NULL);
	if( FAILED( hr ) )
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface8to9() - CreateDepthStencilSurface() failed.\n"));
		RELEASE( pSurface );
		return SetLastError(hr);
	}

	if( !pSurface->Init(pD3DSurface, this, NULL) )
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface8to9() - CSurface::Init() failed.\n"));
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

HRESULT CDevice::CreateDepthStencilSurface9(
	UINT Width,
	UINT Height,
	FORMAT Format,
	MULTISAMPLE_TYPE MultiSample,
	DWORD MultisampleQuality,
	BOOL Discard,
	CSurface** ppSurface,
	HANDLE *pSharedHandle)
{
	//
	// Initialize locals.
	//

	LPDIRECT3DSURFACE9 pD3DSurface = NULL;
	LPDIRECT3DSURFACE9 pTempSurface = NULL;
	LPDIRECT3DSURFACE9 * ppD3DSurface = NULL;
	CSurface * pSurface = NULL;


	//
	// Allocate a new surface.
	//

	pSurface = new CSurface;
	
	if( pSurface == NULL )
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface9() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}
	

	if (ppSurface)
	{
		if (*ppSurface && pSharedHandle)
		{
			pTempSurface = (*ppSurface)->GetD3DSurface9();
			ppD3DSurface = &pTempSurface;
		}
		else
		{
			ppD3DSurface = &pD3DSurface;
		}
	}


	ASSERT(ppD3DSurface != NULL );

	HRESULT hr = m_pDevice9->CreateDepthStencilSurface(
		Width,
		Height,
		Format.d3dfFormat,
		(D3DMULTISAMPLE_TYPE) MultiSample,
		MultisampleQuality,
		Discard,
		ppD3DSurface,
		pSharedHandle);

	if( FAILED( hr ) )
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface9() - CreateDepthStencilSurface() failed.\n"));
		RELEASE( pSurface );
		return SetLastError(hr);
	}

	ASSERT(*ppD3DSurface != NULL);


	if( !pSurface->Init(*ppD3DSurface, this, NULL) )
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurface9() - CSurface::Init() failed.\n"));
		RELEASE( pSurface );
		RELEASE( pD3DSurface );
		RELEASE(pTempSurface);
		return SetLastError(E_FAIL);
	}

	if ( ppSurface != NULL )
	{
		*ppSurface = pSurface;
	}


	RELEASE(pD3DSurface);
	RELEASE(pTempSurface);

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::UpdateSurface9(CSurface* pSourceSurface,RECT* pSourceRects,
							CSurface* pDestinationSurface,POINT* pDestPoint)
{
	IDirect3DSurface9* pD3DSourceSurface = 0L;
	IDirect3DSurface9* pD3DDestinationSurface = 0L;

	if (0L != pSourceSurface)
		pD3DSourceSurface = pSourceSurface->GetD3DSurface9();
	if (0L != pDestinationSurface)
		pD3DDestinationSurface = pDestinationSurface->GetD3DSurface9();

	return SetLastError(m_pDevice9->UpdateSurface(pD3DSourceSurface,
														pSourceRects,
														pD3DDestinationSurface,
														pDestPoint));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::UpdateTexture9(CBaseTexture* pSourceTexture,
								CBaseTexture* pDestinationTexture)
{
	IDirect3DBaseTexture9* pD3DSourceTexture = 0L;
	IDirect3DBaseTexture9* pD3DDestinationTexture = 0L;

	if( 0L != pSourceTexture )
		pD3DSourceTexture = pSourceTexture->GetD3DTexture9();
	if( 0L != pDestinationTexture )
		pD3DDestinationTexture = pDestinationTexture->GetD3DTexture9();

	return SetLastError(m_pDevice9->UpdateTexture(pD3DSourceTexture,pD3DDestinationTexture));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetFrontBuffer8To9(CSurface* pDestSurface)
{
	return SetLastError(m_pDevice9->GetFrontBufferData(0, pDestSurface ? pDestSurface->GetD3DSurface9() : NULL));
}


// --------------------------------------------------------------------------------------

HRESULT CDevice::GetRenderTargetData9(CSurface* pSrcRenderTarget, CSurface* pDestSurface)
{
	return SetLastError(m_pDevice9->GetRenderTargetData(pSrcRenderTarget ? pSrcRenderTarget->GetD3DSurface9() : NULL,
														pDestSurface ? pDestSurface->GetD3DSurface9() : NULL));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetFrontBufferData9(UINT iSwapChain, CSurface* pDestSurface)
{
	return SetLastError(m_pDevice9->GetFrontBufferData(iSwapChain, pDestSurface ? pDestSurface->GetD3DSurface9() : NULL));
}

// ------------------------------------------------------------------------------

HRESULT CDevice::SetRenderTarget8To9(CSurface *pNew, CSurface *pNewDepth)
{
	// Get a pointer to the original render target
	LPDIRECT3DSURFACE9 pD3DSurface = NULL;
	HRESULT hr = m_pDevice9->GetRenderTarget(0, &pD3DSurface);
	if (FAILED(hr)) {
		DPF(1, _T("CDevice::SetRenderTarget8To9() - GetRenderTarget() failed.\n"));
		return SetLastError(hr);
	}

	if( NULL != pNew ) {
		hr = m_pDevice9->SetRenderTarget( 0, pNew->GetD3DSurface9());
		if (FAILED(hr)) {
			DPF(1, _T("CDevice::SetRenderTarget8To9() - SetRenderTarget() failed.\n"));
			RELEASE( pD3DSurface );
			return SetLastError(hr);
		}
	}

	hr = m_pDevice9->SetDepthStencilSurface(pNewDepth == NULL ? NULL : pNewDepth->GetD3DSurface9());
	if (FAILED(hr)) {
		DPF(1, _T("CDevice::SetRenderTarget8To9() - SetDepthStencilSurface() failed.\n"));

		// Reset the render target back to the original
		m_pDevice9->SetRenderTarget( 0, pD3DSurface);
		RELEASE( pD3DSurface );
		return SetLastError(hr);
	}

	RELEASE( pD3DSurface );
	return SetLastError(hr);
}

// ------------------------------------------------------------------------------

HRESULT CDevice::GetRenderTarget8To9(CSurface** ppRenderTarget)
{

	LPDIRECT3DSURFACE9 pD3DSurface = NULL;
	CSurface* pSurface = new CSurface;
	if (0L == pSurface) {
		DPF(1, _T("CDevice::GetRenderTarget9() - Out of Memory.\n"));
		return SetLastError(E_FAIL);
	}

	HRESULT hr = m_pDevice9->GetRenderTarget(0, ppRenderTarget == 0L ? 0L: &pD3DSurface);
	if (FAILED(hr)) {
		DPF(1, _T("CDevice::GetRenderTarget9() - GetRenderTarget() failed.\n"));
		RELEASE(pSurface);
		return SetLastError(hr);
	}

	if (!pSurface->Init(pD3DSurface, this, NULL)) {
		DPF(1, _T("CDevice::GetRenderTarget9() - CSurface::Init() failed.\n"));
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

HRESULT CDevice::SetRenderTarget9(DWORD RenderTargetIndex, CSurface *pNew)
{
	return SetLastError(m_pDevice9->SetRenderTarget( RenderTargetIndex, pNew == NULL ? NULL : pNew->GetD3DSurface9()));
}

// ------------------------------------------------------------------------------

HRESULT CDevice::SetDepthStencilSurface9(CSurface *pNewDepth)
{
	return SetLastError(m_pDevice9->SetDepthStencilSurface(pNewDepth == NULL ? NULL : pNewDepth->GetD3DSurface9()));
}

// ------------------------------------------------------------------------------

HRESULT CDevice::GetRenderTarget9(DWORD RenderTargetIndex, CSurface** ppRenderTarget)
{

	LPDIRECT3DSURFACE9 pD3DSurface = NULL;
	CSurface* pSurface = new CSurface;
	if (0L == pSurface) {
		DPF(1, _T("CDevice::GetRenderTarget9() - Out of Memory.\n"));
		return SetLastError(E_FAIL);
	}

	HRESULT hr = m_pDevice9->GetRenderTarget(RenderTargetIndex, ppRenderTarget == 0L ? 0L: &pD3DSurface);
	if (FAILED(hr)) {
		DPF(1, _T("CDevice::GetRenderTarget9() - GetRenderTarget() failed.\n"));
		RELEASE(pSurface);
		return SetLastError(hr);
	}

	if (!pSurface->Init(pD3DSurface, this, NULL)) {
		DPF(1, _T("CDevice::GetRenderTarget9() - CSurface::Init() failed.\n"));
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

HRESULT CDevice::GetDepthStencilSurface9(CSurface** ppZStencilSurface)
{
	LPDIRECT3DSURFACE9 pD3DSurface = NULL;
	CSurface* pSurface = new CSurface;

	HRESULT hr = m_pDevice9->GetDepthStencilSurface( ppZStencilSurface == 0L ? 0L : &pD3DSurface );
	if( FAILED( hr ) )
	{
		if ((hr != D3DERR_NOTFOUND) || (!bSuppressGetDepthStencilDBGOutput))
			DPF(1, _T("CDevice::GetDepthStencilSurface9() - GetDepthStencilSurface() failed.\n"));
		RELEASE( pSurface );
		return SetLastError(hr);
	}

	if( !pSurface->Init( pD3DSurface, this, NULL ) )
	{
		DPF(1, _T("CDevice::GetDepthStencilSurface9() - CSurface::Init() failed.\n"));
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

HRESULT CDevice::BeginScene9(void)
{
	return SetLastError(m_pDevice9->BeginScene( ));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::EndScene9(void)
{
	return SetLastError(m_pDevice9->EndScene( ));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::Clear9(DWORD Count,D3DRECT* pRects,DWORD dwFlags,D3DCOLOR Color,float Z,DWORD Stencil)
{
	if(dwFlags == FMWK_DEFAULT)
	{
		dwFlags = 0;
		dwFlags |= CLEARF_TARGET;
		dwFlags |= m_pPresParams->bAutoDepthStencil ? CLEARF_ZBUFFER : 0;
	}

	return SetLastError(m_pDevice9->Clear(Count,pRects,dwFlags,Color,Z,Stencil));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetTransform9(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix)
{
	// Remap pre DX9 transform types
	switch (State)
	{
	case 1: 	// WORLD
		State = D3DTS_WORLDMATRIX(0);
		break;
	case 4: 	// WORLD1
		State = D3DTS_WORLDMATRIX(1);
		break;
	case 5: 	// WORLD2
		State = D3DTS_WORLDMATRIX(2);
		break;
	case 6: 	// WORLD3
		State = D3DTS_WORLDMATRIX(3);
		break;
	}

	return SetLastError(m_pDevice9->SetTransform(State,pMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetTransform9(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix)
{
	return SetLastError(m_pDevice9->GetTransform(State,pMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::MultiplyTransform9(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix)
{
	return SetLastError(m_pDevice9->MultiplyTransform(State,pMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetViewport9(LPVIEWPORT pViewport)
{
	// convert from private struct
	D3DVIEWPORT9	Viewport;

	if (pViewport)
	{
		Viewport.X			  = pViewport->X;
		Viewport.Y			  = pViewport->Y;
		Viewport.Width		  = pViewport->Width;
		Viewport.Height 	  = pViewport->Height;
		Viewport.MinZ		  = pViewport->MinZ;
		Viewport.MaxZ		  = pViewport->MaxZ;
	}

	return SetLastError(m_pDevice9->SetViewport(pViewport==0L?0L:&Viewport));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetViewport9(LPVIEWPORT pViewport)
{
	D3DVIEWPORT9	Viewport;
	ZeroMemory( &Viewport, sizeof(Viewport) );

	// Call GetViewport even if pViewport == NULL (for invalid param testing)
	HRESULT 		hr = SetLastError(m_pDevice9->GetViewport(pViewport==0L?0L:&Viewport));

	if( pViewport == NULL )
		return hr;

	// convert to private struct
	pViewport->X		= Viewport.X;
	pViewport->Y		= Viewport.Y;
	pViewport->Width	= Viewport.Width;
	pViewport->Height	= Viewport.Height;
	pViewport->MinZ 	= Viewport.MinZ;
	pViewport->MaxZ 	= Viewport.MaxZ;

   return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetMaterial9(MATERIAL* pMaterial)
{
	return SetLastError(m_pDevice9->SetMaterial((LPD3DMATERIAL9)pMaterial));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetMaterial9(MATERIAL* pMaterial)
{
	return SetLastError(m_pDevice9->GetMaterial((LPD3DMATERIAL9)pMaterial));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetLight9(DWORD Index,LIGHT* pLight)
{
	return SetLastError(m_pDevice9->SetLight(Index,(D3DLIGHT9*)pLight));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetLight9(DWORD Index,LIGHT* pLight)
{
	return SetLastError(m_pDevice9->GetLight(Index,(D3DLIGHT9*)pLight));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::LightEnable9(DWORD Index, BOOL bEnable)
{
	return SetLastError(m_pDevice9->LightEnable(Index,bEnable));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetLightEnable9(DWORD Index,BOOL* pEnable)
{
	return SetLastError(m_pDevice9->GetLightEnable(Index,pEnable));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetClipPlane9(DWORD Index,float* pPlane)
{
	return SetLastError(m_pDevice9->SetClipPlane(Index,pPlane));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetClipPlane9(DWORD Index,float* pPlane)
{
	return SetLastError(m_pDevice9->GetClipPlane(Index,pPlane));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetRenderState9(D3DRENDERSTATETYPE State,DWORD Value)
{
	switch(State)
	{
		case	3: // D3DRENDERSTATE_TEXTUREADDRESS
				SetSamplerState(0,SAMP_ADDRESSU,Value);
				SetSamplerState(0,SAMP_ADDRESSV,Value);
				return GetLastError();
		case	4: // D3DRENDERSTATE_TEXTUREPERSPECTIVE
				return D3D_OK; // retired renderstate
		case	5: // D3DRENDERSTATE_WRAPU
				{
					if (TRUE == Value)
						m_dwWrapUV |= D3DWRAP_U;
					else
						m_dwWrapUV &= ~D3DWRAP_U;

					return SetRenderState(D3DRS_WRAP0, m_dwWrapUV);
				}
		case	6: // D3DRENDERSTATE_WRAPV
				{
					if (TRUE == Value)
						m_dwWrapUV |= D3DWRAP_V;
					else
						m_dwWrapUV &= ~D3DWRAP_V;

					return SetRenderState(D3DRS_WRAP0, m_dwWrapUV);
				}
		case	17: // D3DRENDERSTATE_TEXTUREMAG
				switch (Value)
				{
					case	1: // D3DFILTER_NEAREST
							return SetSamplerState(0,SAMP_MAGFILTER,(DWORD)D3DTEXF_POINT);
					case	2: // D3DFILTER_LINEAR
							return SetSamplerState(0,SAMP_MAGFILTER,(DWORD)D3DTEXF_LINEAR);
				}
				return E_FAIL;
		case	18: // D3DRENDERSTATE_TEXTUREMIN
				switch(Value)
				{
					case	1: // D3DFILTER_NEAREST
							SetSamplerState(0,SAMP_MIPFILTER, (DWORD)D3DTEXF_NONE);
							return SetSamplerState(0,SAMP_MINFILTER,(DWORD)D3DTEXF_POINT);
					case	2: // D3DFILTER_LINEAR
							SetSamplerState(0,SAMP_MIPFILTER,(DWORD)D3DTEXF_NONE);
							return SetSamplerState(0,SAMP_MINFILTER,(DWORD)D3DTEXF_LINEAR);
					case	4: // D3DFILTER_MIPLINEAR
							SetSamplerState(0,SAMP_MIPFILTER,(DWORD)D3DTEXF_POINT);
							return SetSamplerState(0,SAMP_MINFILTER,(DWORD)D3DTEXF_LINEAR);
					case	3: //D3DFILTER_MIPNEAREST
							SetSamplerState(0,SAMP_MIPFILTER,(DWORD)D3DTEXF_POINT);
							return SetSamplerState(0,SAMP_MINFILTER,(DWORD)D3DTEXF_POINT);
					case	5: //D3DFILTER_LINEARMIPNEAREST
							SetSamplerState(0,SAMP_MIPFILTER,(DWORD)D3DTEXF_LINEAR);
							return SetSamplerState(0,SAMP_MINFILTER,(DWORD)D3DTEXF_POINT);
					case	6: // D3DFILTER_LINEARMIPLINEAR
							SetSamplerState(0,SAMP_MIPFILTER,(DWORD)D3DTEXF_LINEAR);
							return SetSamplerState(0,SAMP_MINFILTER,(DWORD)D3DTEXF_LINEAR);
				}
				return E_FAIL;

		case	21: // D3DRENDERSTATE_TEXTUREMAPBLEND:
				{
					HRESULT hrResult;
					DWORD	dwValid;

					switch(Value)
					{
						case	7: // D3DTBLEND_COPY
						case	1: // D3DTBLEND_DECAL
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
						case	2: // D3DTBLEND_MODULATE
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
									SetTextureStageState(0,D3DTSS_COLOROP,	(DWORD)D3DTOP_MODULATE);

									hrResult = ValidateDevice(&dwValid);

									return hrResult;
								}
								break;
						case	3: // D3DTBLEND_DECALALPHA
								{
									SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
									SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
									SetTextureStageState(0,D3DTSS_COLOROP,	(DWORD)D3DTOP_BLENDTEXTUREALPHA);
									SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
									SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
									SetTextureStageState(0,D3DTSS_ALPHAOP,	(DWORD)D3DTOP_SELECTARG2);

									hrResult = ValidateDevice(&dwValid);

									return hrResult;
								}
								break;
						case	4: // D3DTBLEND_MODULATEALPHA
								{
									SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
									SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
									SetTextureStageState(0,D3DTSS_COLOROP,	(DWORD)D3DTOP_MODULATE);
									SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
									SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
									SetTextureStageState(0,D3DTSS_ALPHAOP,	(DWORD)D3DTOP_MODULATE);

									hrResult = ValidateDevice(&dwValid);

									if (D3D_OK == hrResult)
										return D3D_OK;

									SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_TEXTURE);
									SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_CURRENT);
									SetTextureStageState(0,D3DTSS_COLOROP,	(DWORD)D3DTOP_MODULATE);
									SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_TEXTURE);
									SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_CURRENT);
									SetTextureStageState(0,D3DTSS_ALPHAOP,	(DWORD)D3DTOP_MODULATE);

									hrResult = ValidateDevice(&dwValid);

									return hrResult;
								}
								break;
						case	8: // D3DTBLEND_ADD
								{
									SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
									SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
									SetTextureStageState(0,D3DTSS_COLOROP,	(DWORD)D3DTOP_ADD);
									SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
									SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
									SetTextureStageState(0,D3DTSS_ALPHAOP,	(DWORD)D3DTOP_SELECTARG2);

									hrResult = ValidateDevice(&dwValid);

									if (D3D_OK == hrResult)
										return D3D_OK;

									SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_TEXTURE);
									SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_CURRENT);
									SetTextureStageState(0,D3DTSS_COLOROP,	(DWORD)D3DTOP_ADD);

									hrResult = ValidateDevice(&dwValid);

									return hrResult;
								}
								break;
					}
				}
				break;
		case	44: // D3DRENDERSTATE_TEXTUREADDRESSU
				return SetSamplerState(0,SAMP_ADDRESSU,Value);
		case	45: // D3DRENDERSTATE_TEXTUREADDRESSV
				return SetSamplerState(0,SAMP_ADDRESSV,Value);
		case	46: // D3DRENDERSTATE_MIPMAPLODBIAS
				return SetSamplerState(0,SAMP_MIPMAPLODBIAS,Value);
		case	49: // D3DRENDERSTATE_ANISOTROPY
				return SetSamplerState(0,SAMP_MAXANISOTROPY,Value);
		case	43: // D3DRENDERSTATE_BORDERCOLOR
				return SetSamplerState(0,SAMP_BORDERCOLOR,Value);
		case	D3DRS_LIGHTING:
			m_bRPLighting = (Value == TRUE) ? true : false;
			break;
		case	D3DRS_CLIPPING:
			m_bRPClipping = (Value == TRUE) ? true : false;
			break;
		case	153:// D3DRS_SOFTWAREVERTEXPROCESSING
				return m_pDevice9->SetSoftwareVertexProcessing(Value);
	}

	return SetLastError(m_pDevice9->SetRenderState(State,Value));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetRenderState9(D3DRENDERSTATETYPE State,DWORD* pValue)
{
	return SetLastError(m_pDevice9->GetRenderState(State,pValue));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::BeginStateBlock9(void)
{
	return SetLastError(m_pDevice9->BeginStateBlock( ));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::EndStateBlock9(UINT_PTR* pToken)
{
	return SetLastError(EndStateBlockInterface9((CStateblock**)pToken));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::EndStateBlockInterface9(CStateblock **ppStateblock)
{
	HRESULT hr;
	LPDIRECT3DSTATEBLOCK9 pSB9	= NULL;
	CStateblock *pSB = new CStateblock();

	if(NULL == pSB)
	{
		DPF(1, _T("CDevice::EndStateBlock9() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	hr = m_pDevice9->EndStateBlock(&pSB9);
	if(FAILED(hr))
		goto Exit;

	if(!pSB->Init(pSB9, this))
	{
		DPF(1, _T("CDevice::EndStateBlock9() - Init() failed.\n"));
		hr = E_FAIL;
		goto Exit;
	}

Exit:
	if(FAILED(hr))
	{
		RELEASE(pSB);
	}

	RELEASE(pSB9);
	*ppStateblock = pSB;

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::ApplyStateBlock9(UINT_PTR SB)
{
	return SetLastError(((CStateblock*)SB)->Apply());
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CaptureStateBlock9(UINT_PTR SB)
{
	return SetLastError(((CStateblock*)SB)->Capture());
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DeleteStateBlock9(UINT_PTR SB)
{
	if(NULL != SB)
		((CStateblock*)SB)->Release();
	return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateStateBlock9(D3DSTATEBLOCKTYPE Type,UINT_PTR* pToken)
{
	return SetLastError(CreateStateBlockInterface9(Type, (CStateblock**)pToken));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateStateBlockInterface9(D3DSTATEBLOCKTYPE Type,CStateblock **ppStateblock)
{
	HRESULT hr;
	LPDIRECT3DSTATEBLOCK9 pSB9	= NULL;
	CStateblock *pSB = new CStateblock();

	if(NULL == pSB)
	{
		DPF(1, _T("CDevice::CreateStateBlock9() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	hr = m_pDevice9->CreateStateBlock(Type, &pSB9);
	if(FAILED(hr))
		goto Exit;

	if(!pSB->Init(pSB9, this))
	{
		DPF(1, _T("CDevice::CreateStateBlock9() - Init() failed.\n"));
		hr = E_FAIL;
		goto Exit;
	}

Exit:
	if(FAILED(hr))
	{
		RELEASE(pSB);
	}

	RELEASE(pSB9);
	*ppStateblock = pSB;

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetClipStatus9(LPCLIPSTATUS pClipStatus)
{
	D3DCLIPSTATUS9	ClipStatus;
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
	
	return SetLastError(m_pDevice9->SetClipStatus(pClipStatus==0L?0L:&ClipStatus));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetClipStatus9(LPCLIPSTATUS pClipStatus)
{
	D3DCLIPSTATUS9	ClipStatus;
	HRESULT 		hr;

	hr = m_pDevice9->GetClipStatus(pClipStatus==0L?0L:&ClipStatus);

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

// Note: GetTexure9 is not valid for invalid param testing where ppTexture == NULL
HRESULT CDevice::GetTexture9(int nIndex, CBaseTexture **ppTexture)
{
	//
	// Initialize locals.
	//

	CBaseTexture * pTexture = NULL;
	IDirect3DBaseTexture9 * pD3DTexture = NULL;
	D3DRESOURCETYPE d3dType;


	//
	// Validate and initialize parameters.
	//

	if ( ppTexture == NULL )
	{
		DPF(1, _T("CDevice::GetTexture9() - ppTexture is NULL.\n") );
		return SetLastError(E_POINTER);
	}

	*ppTexture = NULL;


	HRESULT hr = m_pDevice9->GetTexture( (DWORD) nIndex, &pD3DTexture );
	
	if ( FAILED( hr ) )
	{
		DPF(1, _T("CDevice::GetTexture9() - GetTexture() failed.\n"));
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
		DPF(1, _T("CDevice::GetTexture9() - GetType() failed.\n"));
		RELEASE( pD3DTexture );
		return E_FAIL;
	}

	if(pTexture == NULL)
	{
		DPF(1, _T("CDevice::GetTexture9() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}

	if( d3dType == D3DRTYPE_TEXTURE )
	{
		((CTexture*)pTexture)->Init((LPDIRECT3DTEXTURE9)pD3DTexture, this );
	}
	else if (d3dType == D3DRTYPE_VOLUMETEXTURE)
	{
		((CVolTexture*)pTexture)->Init( (LPDIRECT3DVOLUMETEXTURE9)pD3DTexture, this );
	}
	else if (d3dType == D3DRTYPE_CUBETEXTURE)
	{
		((CCubeTexture*)pTexture)->Init( (LPDIRECT3DCUBETEXTURE9)pD3DTexture, this );
	}
	else
		hr=E_FAIL;


	if(SUCCEEDED(hr))
	{
		// we succeeded!
		if( ppTexture )
			*ppTexture = pTexture;
	}
	else
	{
		DPF(1, _T("CDevice::GetTexture9() - CTexture::Init() failed.\n") );
		RELEASE( pTexture );
		RELEASE( pD3DTexture );
		hr = E_FAIL;
	}

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetTexture9(int nIndex, CBaseTexture *pTexture)
{
	if (m_pDevice9 == NULL)
	{
		DPF(1, _T("CDevice::SetTexture9() - Invalid Device.\n"));
		return SetLastError(E_FAIL);
	}

	return SetLastError(m_pDevice9->SetTexture(nIndex, pTexture != NULL ? pTexture->GetD3DTexture9() : NULL));
}
	
// --------------------------------------------------------------------------------------

HRESULT CDevice::GetTextureStageState9(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue)
{
	switch(Type)
	{
	case 13:	//D3DTSS_ADDRESSU
		return SetLastError(m_pDevice9->GetSamplerState(Stage,D3DSAMP_ADDRESSU,pValue));
	case 14:	//D3DTSS_ADDRESSV
		return SetLastError(m_pDevice9->GetSamplerState(Stage,D3DSAMP_ADDRESSV,pValue));
	case 25:	//D3DTSS_ADDRESSW
		return SetLastError(m_pDevice9->GetSamplerState(Stage,D3DSAMP_ADDRESSW,pValue));
	case 15:	//D3DTSS_BORDERCOLOR
		return SetLastError(m_pDevice9->GetSamplerState(Stage,D3DSAMP_BORDERCOLOR,pValue));
	case 16:	//D3DTSSP_MAGFILTER
		return SetLastError(m_pDevice9->GetSamplerState(Stage,D3DSAMP_MAGFILTER,pValue));
	case 17:	//D3DTSS_MINFILTER
		return SetLastError(m_pDevice9->GetSamplerState(Stage,D3DSAMP_MINFILTER,pValue));
	case 18:	//D3DTSS_MIPFILTER
		return SetLastError(m_pDevice9->GetSamplerState(Stage,D3DSAMP_MIPFILTER,pValue));
	case 19:	//D3DTSS_MIPMAPLODBIAS
		return SetLastError(m_pDevice9->GetSamplerState(Stage,D3DSAMP_MIPMAPLODBIAS,pValue));
	case 20:	//D3DTSS_MAXMIPLEVEL
		return SetLastError(m_pDevice9->GetSamplerState(Stage,D3DSAMP_MAXMIPLEVEL,pValue));
	default:
		return SetLastError(m_pDevice9->GetTextureStageState(Stage,Type,pValue));
	}
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetTextureStageState9(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	switch(Type)
	{
	case 13:	//D3DTSS_ADDRESSU
		return SetLastError(m_pDevice9->SetSamplerState(Stage,D3DSAMP_ADDRESSU,Value));
	case 14:	//D3DTSS_ADDRESSV
		return SetLastError(m_pDevice9->SetSamplerState(Stage,D3DSAMP_ADDRESSV,Value));
	case 25:	//D3DTSS_ADDRESSW
		return SetLastError(m_pDevice9->SetSamplerState(Stage,D3DSAMP_ADDRESSW,Value));
	case 15:	//D3DTSS_BORDERCOLOR
		return SetLastError(m_pDevice9->SetSamplerState(Stage,D3DSAMP_BORDERCOLOR,Value));
	case 16:	//D3DTSSP_MAGFILTER
		return SetLastError(m_pDevice9->SetSamplerState(Stage,D3DSAMP_MAGFILTER,Value));
	case 17:	//D3DTSS_MINFILTER
		return SetLastError(m_pDevice9->SetSamplerState(Stage,D3DSAMP_MINFILTER,Value));
	case 18:	//D3DTSS_MIPFILTER
		return SetLastError(m_pDevice9->SetSamplerState(Stage,D3DSAMP_MIPFILTER,Value));
	case 19:	//D3DTSS_MIPMAPLODBIAS
		return SetLastError(m_pDevice9->SetSamplerState(Stage,D3DSAMP_MIPMAPLODBIAS,Value));
	case 20:	//D3DTSS_MAXMIPLEVEL
		return SetLastError(m_pDevice9->SetSamplerState(Stage,D3DSAMP_MAXMIPLEVEL,Value));
	case 21:	//D3DTSS_MAXANISOTROPY
		return SetLastError(m_pDevice9->SetSamplerState(Stage,D3DSAMP_MAXANISOTROPY,Value));
	default:
		return SetLastError(m_pDevice9->SetTextureStageState(Stage,Type,Value));
	}
	
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::ValidateDevice9(DWORD* pdwPasses)
{
	return SetLastError(m_pDevice9->ValidateDevice(pdwPasses));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetInfo9(DWORD DevInfoID,void* pDevInfoStruct,DWORD DevInfoStructSize)
{
	CQuery *pQuery = NULL;

	if ( 0L == pDevInfoStruct || 0 == DevInfoStructSize )
		return D3DERR_INVALIDCALL;

	HRESULT hr = CreateQuery((QUERYTYPE) DevInfoID, &pQuery);

	if( FAILED( hr ) || 0L==pQuery)
	{
		DPF(1, _T("CDevice::GetInfo9() - CreateQuery() failed.\n"));
		return SetLastError(hr);
	}

	hr = pQuery->Issue(ISSUE_END);

	if( FAILED( hr ) )
	{
		DPF(1, _T("CDevice::GetInfo9() - Issue() failed.\n"));
		RELEASE(pQuery);
		return SetLastError(hr);
	}

	hr = pQuery->GetData(pDevInfoStruct, DevInfoStructSize, GETDATA_FLUSH);

	if( hr == S_FALSE )
	{
		clock_t time0 = clock();
		while ( SUCCEEDED( hr = pQuery->GetData(pDevInfoStruct, DevInfoStructSize, GETDATA_FLUSH) ) && hr != S_OK )
		{
			// poll at most a second
			if ( (clock() - time0) / CLOCKS_PER_SEC > 1 )
			{
				hr = E_FAIL;
				break;
			}
		}
	}

	if( hr != S_OK )
	{
		DPF(1, _T("CDevice::GetInfo9() - GetData() failed.\n"));
		RELEASE(pQuery);
		return SetLastError(hr);
	}

	RELEASE(pQuery);

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetPaletteEntries9(UINT PaletteNumber,PALETTEENTRY* pEntries)
{
	return SetLastError(m_pDevice9->SetPaletteEntries(PaletteNumber,pEntries));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetPaletteEntries9(UINT PaletteNumber,PALETTEENTRY* pEntries)
{
	return SetLastError(m_pDevice9->GetPaletteEntries(PaletteNumber,pEntries));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetCurrentTexturePalette9(UINT PaletteNumber)
{
	return SetLastError(m_pDevice9->SetCurrentTexturePalette(PaletteNumber));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetCurrentTexturePalette9(UINT *PaletteNumber)
{
	return SetLastError(m_pDevice9->GetCurrentTexturePalette(PaletteNumber));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawPrimitive9(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
	return SetLastError(m_pDevice9->DrawPrimitive(PrimitiveType,StartVertex,PrimitiveCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawIndexedPrimitive9(D3DPRIMITIVETYPE PrimitiveType,UINT minIndex,
									   UINT NumVertices,UINT startIndex,UINT primCount,INT BaseVertexIndex)
{
	return SetLastError(m_pDevice9->DrawIndexedPrimitive(PrimitiveType,BaseVertexIndex,minIndex,NumVertices,startIndex,primCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawPrimitiveUP9(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,
								  void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	return SetLastError(m_pDevice9->DrawPrimitiveUP(	PrimitiveType,PrimitiveCount,
										pVertexStreamZeroData,VertexStreamZeroStride));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawIndexedPrimitiveUP9(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,
										 UINT NumVertexIndices,UINT PrimitiveCount,
										 void* pIndexData,FORMAT IndexDataFormat,
										 void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	return SetLastError(m_pDevice9->DrawIndexedPrimitiveUP( PrimitiveType,MinVertexIndex,
												NumVertexIndices,PrimitiveCount,
												pIndexData,IndexDataFormat.d3dfFormat,
												pVertexStreamZeroData,VertexStreamZeroStride));
}

// --------------------------------------------------------------------------------------

bool CDevice::DrawPrimitiveUserMemory9(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc,
					LPVOID lpVertices, DWORD  dwVertexCount, LPVOID lpIndices, DWORD  dwIndexCount, 
					DWORD  dwFlags, bool bIs32BitIndices)
{
	bool	fResult = true;

	DWORD dwStride = GetVertexSize(dwVertexTypeDesc);
	DWORD dwSize = dwStride * dwVertexCount;

	// Set FVF vertex shader
	if (FAILED(SetFVF(dwVertexTypeDesc)))
		return false;

	// DX8 DrawPrimitive 
	if (NULL == lpIndices)
	{
		DWORD dwPrimitiveCount = GetPrimitiveCount(d3dptPrimitiveType, dwVertexCount);

//		dwProcessedFlags = ProcessDrawPrimitiveFlags(dwFlags, false);
		if (FAILED(DrawPrimitiveUP9(d3dptPrimitiveType, dwPrimitiveCount, lpVertices, dwStride)))
			fResult = false;
//		RestoreDrawPrimitiveFlags();
	}
	// DX9 DrawIndixedPrimitive
	else
	{
		DWORD		dwPrimitiveCount = GetPrimitiveCount(d3dptPrimitiveType, dwIndexCount);
		FORMAT		d3dfIndexFormat( (bIs32BitIndices) ? FMT_INDEX32 : FMT_INDEX16 );

//		dwProcessedFlags = ProcessDrawPrimitiveFlags(dwFlags, false);
		if (FAILED(DrawIndexedPrimitiveUP9(d3dptPrimitiveType, 0, dwVertexCount, dwPrimitiveCount, lpIndices, d3dfIndexFormat, lpVertices, dwStride)))
			fResult = false;
//		RestoreDrawPrimitiveFlags();
	}
	return fResult;
}

// --------------------------------------------------------------------------------------

bool CDevice::DrawPrimitiveVertexBuffer9(D3DPRIMITIVETYPE d3dptPrimitiveType, 
					CVertexBuffer *pBuffer, DWORD dwStartVertex, DWORD dwNumVertices, 
					LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags,
					DWORD dwUsage, DWORD dwPool, bool bIs32BitIndices)
{
	DWORD	dwStride = 0;
	bool	fResult = true;

	if (NULL == pBuffer)
		return false;
	
	dwStride = GetVertexSize(pBuffer->GetVertexFormat());

	// Set FVF vertex shader
	if (FAILED(SetFVF(pBuffer->GetVertexFormat())))
		return false;

	// Set source stream
	if (FAILED(SetStreamSource9(0, pBuffer, dwStride, 0)))
		return false;


	// DX9 DrawPrimitive 
	if (NULL == lpwIndices)
	{
		DWORD dwPrimitiveCount = GetPrimitiveCount(d3dptPrimitiveType, dwNumVertices);

//		ProcessDrawPrimitiveFlags(dwFlags, false);
		if (FAILED(DrawPrimitive9(d3dptPrimitiveType, dwStartVertex, dwPrimitiveCount)))
			return false;
//		RestoreDrawPrimitiveFlags();
	}
	// DX9 DrawIndixedPrimitive
	else
	{
		DWORD			dwPrimitiveCount = GetPrimitiveCount(d3dptPrimitiveType, dwIndexCount);
		FMT 			d3dfIndexBuffer = (bIs32BitIndices) ? FMT_INDEX32 : FMT_INDEX16;
		DWORD			dwLenght = (bIs32BitIndices) ? dwIndexCount * sizeof(DWORD) : dwIndexCount * sizeof(WORD);
		WORD*			pBuf;
		CIndexBuffer*	pIB;

		if (FAILED(CreateIndexBuffer9(dwLenght, dwUsage, d3dfIndexBuffer, dwPool, &pIB, NULL)))
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
		SetIndices9(pIB, 0);
		RELEASE(pIB);

//		dwProcessedFlags = ProcessDrawPrimitiveFlags(dwFlags, false);
		if (FAILED(DrawIndexedPrimitive9(d3dptPrimitiveType, dwStartVertex, dwNumVertices, 0, dwPrimitiveCount, 0)))
			fResult = false;
//		RestoreDrawPrimitiveFlags();
	}

	return fResult;
}


// --------------------------------------------------------------------------------------

bool CDevice::DrawPrimitiveStridedData9(D3DPRIMITIVETYPE d3dptPrimitiveType,
					DWORD  dwVertexTypeDesc, LPDRAWPRIMITIVESTRIDEDDATA lpVertexArray,
					DWORD  dwVertexCount, LPWORD lpwIndices, DWORD	dwIndexCount,
					DWORD  dwFlags, DWORD  dwUsage, DWORD  dwPool, bool bIs32BitIndices)
{

	LPDRAWPRIMITIVESTRIDEDDATA	pVert = (LPDRAWPRIMITIVESTRIDEDDATA)lpVertexArray;
	CVertexBuffer*				pVB = NULL;
	BYTE*						pSrc = NULL;
	BYTE*						pDst = NULL;
	DWORD	dwWho = dwFlags & (CD3D_SRC | CD3D_REF);
	bool	fResult = true;
	DWORD	dwTextureFormats = dwVertexTypeDesc >> CDIRECT3D_TEXTUREFORMATS_SHIFT;
	DWORD	dwNumTexCoords = ((dwVertexTypeDesc & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);
	D3DVERTEXELEMENT9 VE[26];
	D3DVERTEXELEMENT9 veEnd = D3DDECL_END();
	DWORD	dwIndex = 0;
	WORD	wStream = 0;
	DWORD	dwShaderHandle = 0;
	CVertexDeclaration	*pDecl = NULL;
	CVertexShader *pShader = NULL;


	memset(VE, 0, sizeof(VE));

	// Construct vertex shader declaration for multistream (strided) data and set streams
	// NOTE: Strided format does not support transformed data

	// Position data and blend weights stream
	switch (dwVertexTypeDesc & D3DFVF_POSITION_MASK)
	{
		case D3DFVF_XYZ:
			VE[dwIndex].Type  =  D3DDECLTYPE_FLOAT3;
			VE[dwIndex].Usage =  D3DDECLUSAGE_POSITION;
			dwIndex++;
			break;
		case D3DFVF_XYZB1:
			VE[dwIndex].Type  =  D3DDECLTYPE_FLOAT3;
			VE[dwIndex].Usage =  D3DDECLUSAGE_POSITION;
			VE[dwIndex+1].Type	=  D3DDECLTYPE_FLOAT1;
			VE[dwIndex+1].Usage =  D3DDECLUSAGE_BLENDWEIGHT;
			VE[dwIndex+1].Offset = 3*sizeof(float);
			dwIndex += 2;
			break;
		case D3DFVF_XYZB2:
			VE[dwIndex].Type = D3DDECLTYPE_FLOAT3;
			VE[dwIndex].Usage =  D3DDECLUSAGE_POSITION;
			VE[dwIndex+1].Type	=  D3DDECLTYPE_FLOAT2;
			VE[dwIndex+1].Usage =  D3DDECLUSAGE_BLENDWEIGHT;
			VE[dwIndex+1].Offset = 3*sizeof(float);
			dwIndex += 2;
			break;
		case D3DFVF_XYZB3:
			VE[dwIndex].Type = D3DDECLTYPE_FLOAT3;
			VE[dwIndex].Usage =  D3DDECLUSAGE_POSITION;
			VE[dwIndex+1].Type	=  D3DDECLTYPE_FLOAT3;
			VE[dwIndex+1].Usage =  D3DDECLUSAGE_BLENDWEIGHT;
			VE[dwIndex+1].Offset = 3*sizeof(float);
			dwIndex += 2;
			break;
		case D3DFVF_XYZB4:
			VE[dwIndex].Type = D3DDECLTYPE_FLOAT3;
			VE[dwIndex].Usage =  D3DDECLUSAGE_POSITION;
			VE[dwIndex+1].Type	=  D3DDECLTYPE_FLOAT4;
			VE[dwIndex+1].Usage =  D3DDECLUSAGE_BLENDWEIGHT;
			VE[dwIndex+1].Offset = 3*sizeof(float);
			dwIndex += 2;
			break;
	}
	
	if (fResult)
	{
		if (FAILED(ConvertToVertexBuffer((dwVertexTypeDesc & D3DFVF_POSITION_MASK), dwVertexCount, pVert->position.lpvData, pVert->position.dwStride, dwUsage, dwPool, false, &pVB)))
			fResult = false;
		else
		if (FAILED(SetStreamSource9(wStream, pVB, GetVertexSize(dwVertexTypeDesc & D3DFVF_POSITION_MASK), 0)))
			fResult = false;
		RELEASE(pVB);
	}
	wStream++;

	// Normal and point size stream
	if (0 != (dwVertexTypeDesc & (D3DFVF_NORMAL | D3DFVF_PSIZE)))
	{
		if (0 != (dwVertexTypeDesc & D3DFVF_NORMAL))
		{
			VE[dwIndex].Stream = wStream;
			VE[dwIndex].Type = D3DDECLTYPE_FLOAT3;
			VE[dwIndex++].Usage = D3DDECLUSAGE_NORMAL;
		}
		if (0 != (dwVertexTypeDesc & D3DFVF_PSIZE))
		{
			VE[dwIndex].Stream = wStream;
			VE[dwIndex].Type = D3DDECLTYPE_FLOAT1;
			VE[dwIndex].Usage = D3DDECLUSAGE_PSIZE;
			if (0 != (dwVertexTypeDesc & D3DFVF_NORMAL))
				VE[dwIndex].Offset = 3 * sizeof(float);
			dwIndex++;
		}

		if (fResult)
		{
			if (FAILED(ConvertToVertexBuffer((dwVertexTypeDesc & (D3DFVF_NORMAL | D3DFVF_PSIZE)), dwVertexCount, pVert->normal.lpvData, pVert->normal.dwStride, dwUsage, dwPool, false, &pVB)))
				fResult = false;
			else
			if (FAILED(SetStreamSource9(wStream, pVB, GetVertexSize(dwVertexTypeDesc & (D3DFVF_NORMAL | D3DFVF_PSIZE)), 0)))
				fResult = false;
			RELEASE(pVB);
		}
		wStream++;
	}

	// Diffuse color stream
	if (0 != (dwVertexTypeDesc & D3DFVF_DIFFUSE))
	{
		VE[dwIndex].Stream = wStream;
		VE[dwIndex].Type = D3DDECLTYPE_D3DCOLOR;
		VE[dwIndex++].Usage = D3DDECLUSAGE_COLOR;
		if (fResult)
		{
			if (FAILED(ConvertToVertexBuffer(D3DFVF_DIFFUSE, dwVertexCount, pVert->diffuse.lpvData, pVert->diffuse.dwStride, dwUsage, dwPool, false, &pVB)))
				fResult = false;
			else
			if (FAILED(SetStreamSource9(wStream, pVB, sizeof(DWORD), 0)))
				fResult = false;
			RELEASE(pVB);
		}
		wStream++;
	}

	// Specular color stream
	if (0 != (dwVertexTypeDesc & D3DFVF_SPECULAR))
	{
		VE[dwIndex].Stream = wStream;
		VE[dwIndex].Type = D3DDECLTYPE_D3DCOLOR;
		VE[dwIndex].Usage = D3DDECLUSAGE_COLOR;
		VE[dwIndex++].UsageIndex = 1;

		if (fResult)
		{
			if(FAILED(ConvertToVertexBuffer(D3DFVF_SPECULAR, dwVertexCount, pVert->specular.lpvData, pVert->specular.dwStride, dwUsage, dwPool, false, &pVB)))
				fResult = false;
			else
			if (FAILED(SetStreamSource9(wStream, pVB, sizeof(DWORD), 0)))
				fResult = false;
			RELEASE(pVB);
		}
		wStream++;
	}

	// Texture streams
	for (DWORD dwCount = 0; dwCount < dwNumTexCoords; dwCount++)
	{
		static D3DDECLTYPE dwTexFormat[] = {  D3DDECLTYPE_FLOAT2, D3DDECLTYPE_FLOAT3, 
										D3DDECLTYPE_FLOAT4, D3DDECLTYPE_FLOAT1};
		static DWORD dwTexSize[] = { 2*sizeof(float), 3*sizeof(float), 4*sizeof(float), sizeof(float)};
		
		VE[dwIndex].Stream = wStream;
		VE[dwIndex].Type = (dwTexFormat[dwTextureFormats & CDIRECT3D_TEXTUREFORMAT_MASK]);
		VE[dwIndex].Usage = D3DDECLUSAGE_TEXCOORD;
		VE[dwIndex++].UsageIndex = (BYTE)dwCount;

		if (fResult)
		{
			if (FAILED(ConvertToVertexBuffer((dwTextureFormats & CDIRECT3D_TEXTUREFORMAT_MASK) | (1 << D3DFVF_TEXCOUNT_SHIFT), dwVertexCount, pVert->textureCoords[dwCount].lpvData, pVert->textureCoords[dwCount].dwStride, dwUsage, dwPool, false, &pVB)))
				fResult = false;
			else
			if (FAILED(SetStreamSource9(wStream, pVB, dwTexSize[dwTextureFormats & CDIRECT3D_TEXTUREFORMAT_MASK], 0)))
				fResult = false;
			RELEASE(pVB);
		}
		dwTextureFormats >>= CDIRECT3D_TEXTUREFORMAT_SIZE;
		wStream++;
	}

	// Close shader declaration
	VE[dwIndex++] =  veEnd;

	// Create temporary shader
	if (fResult) 
	{
		if (FAILED(CreateVertexDeclaration9((VERTEXELEMENT*)VE, &pDecl)) ||
			FAILED(SetVertexDeclaration9(pDecl)) ||
			FAILED(SetVertexShaderInt9(NULL)))
			fResult = false;
	}

	// DX9 DrawPrimitive (strided data)
	if (NULL == lpwIndices)
	{
		DWORD dwPrimitiveCount = GetPrimitiveCount(d3dptPrimitiveType, dwVertexCount);

		if (fResult)
		{
			if (FAILED(DrawPrimitive9(d3dptPrimitiveType, 0, dwPrimitiveCount)))
				fResult = false;
		}
	}
	// DX9 DrawIndexedPrimitive (strided data)
	else
	{
		DWORD			dwPrimitiveCount = GetPrimitiveCount(d3dptPrimitiveType, dwIndexCount);
		FMT 			d3dfIndexBuffer = (bIs32BitIndices) ? FMT_INDEX32 : FMT_INDEX16;
		DWORD			dwLenght = (bIs32BitIndices) ? dwIndexCount * sizeof(DWORD) : dwIndexCount * sizeof(WORD);
		WORD*			pBuf;

		if (fResult)
		{
			CIndexBuffer	*pIB;
			
			if (FAILED(CreateIndexBuffer9(dwLenght, dwUsage, d3dfIndexBuffer, dwPool, &pIB, NULL)))
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
			memcpy(pBuf, lpwIndices, pIB->GetSize9());
			pIB->Unlock();
			SetIndices9(pIB, 0);
			RELEASE(pIB);

			if (fResult)
			{
				if (FAILED(DrawIndexedPrimitive9(d3dptPrimitiveType, 0, dwVertexCount, 0, dwPrimitiveCount, 0)))
					fResult = false;
			}
		}
	}
	
	RELEASE(pDecl);
	RELEASE(pShader);

	return fResult;
}



// --------------------------------------------------------------------------------------

HRESULT CDevice::ProcessVertices9(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,
								  CVertexBuffer* pDestBuffer,CVertexDeclaration *pDecl, DWORD Flags)
{
	return SetLastError(m_pDevice9->ProcessVertices(SrcStartIndex,DestIndex,VertexCount,
						pDestBuffer==0L?0L:pDestBuffer->GetVertexBufferPointer9(),
						(pDecl)?pDecl->GetDecl9():NULL, Flags));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateVertexDeclaration9(VERTEXELEMENT *pVE, CVertexDeclaration **ppDecl)
{
	HRESULT hr = S_OK;
	LPDIRECT3DVERTEXDECLARATION9 pDecl9 = NULL;
	CVertexDeclaration *pDecl = NULL;
	
	hr = m_pDevice9->CreateVertexDeclaration((LPD3DVERTEXELEMENT9)pVE, &pDecl9);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateVertexDeclaration9() - CreateVertexDeclaration failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	pDecl = new CVertexDeclaration();
	if(NULL == pDecl)
	{
		DPF(1, _T("CDevice::CreateVertexDeclaration9() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	if(!pDecl->Init(pDecl9, this))
	{
		DPF(1, _T("CDevice::CreateVertexDeclaration9() - failed to init wrappers.\n"));
		hr = E_FAIL;
		goto Exit;
	}

Exit:
	RELEASE(pDecl9);

	if(FAILED(hr))
	{
		RELEASE(pDecl);
	}
	else
		*ppDecl = pDecl;
	
	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateVertexShader9(DWORD* pDeclaration,DWORD* pFunction,CVertexShader** pHandle,DWORD Usage)
{
	VERTEXELEMENT *pVE = NULL;
	HRESULT hr = S_OK;
	CVertexShader *pShader = NULL;
	CVertexDeclaration *pDecl = NULL;
	
	hr = VDecl8ToVDecl9(pDeclaration, &pVE);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateVertexShader9() - VS8ToVS9() failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	// Create the declarator
	hr = CreateVertexDeclaration(pVE, &pDecl);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateVertexShader9() - CreateVertexDeclaration() failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	// Create the shader
	hr = CreateVertexShaderInt9(pFunction, &pShader);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateVertexShader9() - CreateVertexShader9() failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	pShader->SetDecl(pDecl);


Exit:
	SAFEDELETEA(pVE);
	RELEASE(pDecl);

	if(FAILED(hr))
	{
		RELEASE(pShader);
	}
	else
		*pHandle = pShader;

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DeleteVertexShader9(CVertexShader *pShader)
{
	if(NULL != pShader)
		pShader->Release();
	return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateVertexShaderInt9(DWORD* pFunction,CVertexShader **ppShader)
{
	HRESULT hr = S_OK;
	CVertexShader *pShader = NULL;
	LPDIRECT3DVERTEXSHADER9 pShader9 = NULL;

	pShader = new CVertexShader();
	if(NULL == pShader)
	{
		DPF(1, _T("CDevice::CreateVertexShader9() - Out of memory.\n"));
		return E_OUTOFMEMORY;
	}
	
	if(NULL != pFunction)
	{
		hr = m_pDevice9->CreateVertexShader(pFunction,&pShader9);
		if(FAILED(hr))
		{
			DPF(1, _T("CDevice::CreateVertexShader9() - CreateVertexShader failed. hr=%s(%x).\n"), 
				m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

	if(!pShader->Init(pShader9, this))
	{
		DPF(1, _T("CDevice::CreateVertexShader9() - Failed to init shader wrapper.\n"));
		hr = E_FAIL;
		goto Exit;
	}

Exit:
	RELEASE(pShader9);	

	if(FAILED(hr))
	{
		RELEASE(pShader);
	}
	else
		*ppShader = pShader;

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetVertexShaderInt9(CVertexShader *pVS)
{
	HRESULT hr;

	if(NULL != pVS && NULL != pVS->m_pDecl)
	{
		hr = SetVertexDeclaration9(pVS->m_pDecl);
		if(FAILED(hr))
		{
			DPF(1, _T("CDevice::SetVertexShaderInt9() - SetVertexDeclaration9() failed. hr=%s(%x).\n"),
				m_pD3D->HResultToString(hr), hr);
			return SetLastError(hr);
		}
	}

	return SetLastError(m_pDevice9->SetVertexShader(pVS == NULL ? NULL : pVS->GetShader9()));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetVertexDeclaration9(CVertexDeclaration *pDecl)
{
	return SetLastError(m_pDevice9->SetVertexDeclaration(pDecl ? pDecl->m_pDecl9 : NULL));
}


// --------------------------------------------------------------------------------------

HRESULT CDevice::SetFVF9(DWORD dwFVF)
{
	return SetLastError(m_pDevice9->SetFVF(dwFVF));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetVertexShaderInt9(CVertexShader **ppShader)
{
	HRESULT hr = S_OK;
	LPDIRECT3DVERTEXSHADER9 pVS = NULL;
	CVertexShader *pShader = NULL;

	hr = m_pDevice9->GetVertexShader(&pVS);
	if(FAILED(hr))
		goto Exit;

	pShader = new CVertexShader();
	if(pShader == NULL)
	{
		DPF(1, _T("CDevice::GetVertexShaderInt9() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	if(!pShader->Init(pVS, this))
	{
		DPF(1, _T("CDevice::GetVertexShaderInt9() - Failed to init shader wrapper.\n"));
		hr = E_FAIL;
		goto Exit;
	}

Exit:
	RELEASE(pVS);
	if(FAILED(hr))
	{
		RELEASE(pShader);
	}
	else
		*ppShader = pShader;

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetStreamSource9(UINT streamNumber, CVertexBuffer *pStreamData, UINT stride, UINT offset)
{
	return SetLastError(m_pDevice9->SetStreamSource(streamNumber,
													pStreamData==0L?0L:pStreamData->GetVertexBufferPointer9(),
													offset,stride));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetStreamSource9(UINT streamNumber, CVertexBuffer **ppStreamData, UINT *pStride)
{
	LPDIRECT3DVERTEXBUFFER9 pd3dVertexBuffer = NULL;
	UINT nOffset = 0;
	HRESULT hr = m_pDevice9->GetStreamSource(streamNumber, ppStreamData==0L?0L:&pd3dVertexBuffer, &nOffset, pStride);
	if (FAILED(hr)||0L==ppStreamData)	{
		DPF(1, _T("CDevice::GetStreamSource9() - GetStreamSource() failed.\n"));
		return SetLastError(hr);
	}

	if(NULL == pd3dVertexBuffer)
	{
		ppStreamData = NULL;
		return S_OK;
	}

	CVertexBuffer *pVertexBuffer = new CVertexBuffer;
	if (0L == pVertexBuffer) {
		DPF(1, _T("CDevice::GetStreamSource9() - Out of Memory.\n"));
		return SetLastError(E_FAIL);
	}

	if (!pVertexBuffer->Initialize(pd3dVertexBuffer, this)) {
		DPF(1, _T("CDevice::GetStreamSource9() - CVertexBuffer::SetVertexBuffer() failed.\n"));
		RELEASE(pVertexBuffer);
		RELEASE(pd3dVertexBuffer);
		return SetLastError(E_FAIL);
	}

	*ppStreamData = pVertexBuffer;
	RELEASE(pd3dVertexBuffer);
	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetIndices9(CIndexBuffer *pIndexData, UINT baseVertexIndex)
{
	return SetLastError(m_pDevice9->SetIndices(pIndexData==0L?0L:pIndexData->GetIndexBufferPointer9()));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetIndices9(CIndexBuffer **ppIndexData, UINT *pBaseVertexIndex)
{
	LPDIRECT3DINDEXBUFFER9 pd3dIndexBuffer = NULL;
	HRESULT hr = m_pDevice9->GetIndices(ppIndexData==0L?0L:&pd3dIndexBuffer);
	if (FAILED(hr)||0L==ppIndexData)	{
		DPF(1, _T("CDevice::GetIndices9() - GetIndices() failed.\n"));
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
		DPF(1, _T("CDevice::GetIndices9() - Out of Memory.\n"));
		return SetLastError(E_FAIL);
	}

	if (!pIndexBuffer->Initialize(pd3dIndexBuffer,this)) {
		DPF(1, _T("CDevice::GetIndices9() - CIndexBuffer::SetIndexBuffer() failed.\n"));
		RELEASE(pIndexBuffer);
		RELEASE(pd3dIndexBuffer);
		return SetLastError(E_FAIL);
	}

	*ppIndexData = pIndexBuffer;
	RELEASE(pd3dIndexBuffer);
	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreatePixelShader9(DWORD* pFunction,CPShader** ppShader)
{
	HRESULT hr = S_OK;
	CPShader *pShader = NULL;
	LPDIRECT3DPIXELSHADER9 pShader9 = NULL;
	
	pShader = new CPShader();
	if(NULL == pShader)
	{
		DPF(1, _T("CDevice::CreatePixelShader9() - Out of memory.\n"));
		return E_OUTOFMEMORY;
	}
	
	hr = m_pDevice9->CreatePixelShader(pFunction,&pShader9);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreatePixelShader9() - CreatePixelShader failed. hr=%s(%x).\n"), 
			m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}
	
	if(!pShader->Init(pShader9, this))
	{
		DPF(1, _T("CDevice::CreatePixelShader9() - Failed to init shader wrapper.\n"));
		hr = E_FAIL;
		goto Exit;
	}
	
Exit:
	
	RELEASE(pShader9);

	if(FAILED(hr))
	{
		RELEASE(pShader);
	}
	else
		*ppShader = pShader;
	
	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetPixelShader9(CPShader** ppShader)
{
	HRESULT hr = S_OK;
	LPDIRECT3DPIXELSHADER9 pPS = NULL;
	CPShader *pShader = NULL;

	hr = m_pDevice9->GetPixelShader(&pPS);
	if(FAILED(hr))
		goto Exit;

	pShader = new CPShader();
	if(pShader == NULL)
	{
		DPF(1, _T("CDevice::GetPixelShader9() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	if(!pShader->Init(pPS, this))
	{
		DPF(1, _T("CDevice::GetPixelShader9() - Failed to init shader wrapper.\n"));
		hr = E_FAIL;
		goto Exit;
	}

Exit:
	RELEASE(pPS);
	if(FAILED(hr))
	{
		RELEASE(pShader);
	}
	else
		*ppShader = pShader;

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetPixelShaderConstantF9(UINT StartRegister,const float* pConstantData,UINT Vector4fCount)
{
	return SetLastError(m_pDevice9->SetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetPixelShaderConstantF9(UINT StartRegister,float* pConstantData,UINT Vector4fCount)
{
	return SetLastError(m_pDevice9->GetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetPixelShaderConstantI9(UINT StartRegister,const int* pConstantData,UINT Vector4iCount)
{
	return SetLastError(m_pDevice9->SetPixelShaderConstantI(StartRegister,pConstantData,Vector4iCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetPixelShaderConstantI9(UINT StartRegister,int* pConstantData,UINT Vector4iCount)
{
	return SetLastError(m_pDevice9->GetPixelShaderConstantI(StartRegister,pConstantData,Vector4iCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetPixelShaderConstantB9(UINT StartRegister,const BOOL* pConstantData,UINT BoolCount)
{
	return SetLastError(m_pDevice9->SetPixelShaderConstantB(StartRegister,pConstantData,BoolCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetPixelShaderConstantB9(UINT StartRegister,BOOL* pConstantData,UINT BoolCount)
{
	return SetLastError(m_pDevice9->GetPixelShaderConstantB(StartRegister,pConstantData,BoolCount));
}
/*
// --------------------------------------------------------------------------------------

HRESULT CDevice::SetPixelShaderConstant8to9(DWORD Register,void* pConstantData,DWORD ConstantCount)
{
	return SetLastError(m_pDevice9->SetPixelShaderConstantF((UINT)Register,(const float*)pConstantData,(UINT)ConstantCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetPixelShaderConstant8to9(DWORD Register,void* pConstantData,DWORD ConstantCount)
{
	return SetLastError(m_pDevice9->GetPixelShaderConstantF((UINT)Register,(float*)pConstantData,(UINT)ConstantCount));
}
*/
// --------------------------------------------------------------------------------------

HRESULT CDevice::DeletePixelShader9(CPShader* pShader)
{
	if(NULL != pShader)
		pShader->Release();
	return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawRectPatch9(UINT Handle,float* pNumSegs,RECTPATCH_INFO* pRectPatchInfo)
{
	return SetLastError(m_pDevice9->DrawRectPatch(Handle,pNumSegs,pRectPatchInfo));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DrawTriPatch9(UINT Handle,float* pNumSegs,TRIPATCH_INFO* pTriPatchInfo)
{
	return SetLastError(m_pDevice9->DrawTriPatch(Handle,pNumSegs,pTriPatchInfo));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::DeletePatch9(UINT Handle)
{
	return SetLastError(m_pDevice9->DeletePatch(Handle));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetNPatchMode9(float nSegments)

{
	return SetLastError(m_pDevice9->SetNPatchMode(nSegments));
}

// --------------------------------------------------------------------------------------

float CDevice::GetNPatchMode9()

{
	return m_pDevice9->GetNPatchMode();
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateAdditionalSwapChain9(PRESENT_PARAMETERS* pParams, CSwapChain** ppSwapChain)
{
	HRESULT hr = 0;
	IDirect3DSwapChain9 *pSwap9 = NULL;
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
		Params.MultiSampleQuality = pParams->MultiSampleQuality;
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
		if(pParams->uPresentInterval == FMWK_DEFAULT)
			Params.PresentationInterval = PRESENT_INTERVAL_IMMEDIATE;
		else
			Params.PresentationInterval = pParams->uPresentInterval;
		Params.Windowed = (UINT)pParams->bWindowed;
	}
	
	hr = m_pDevice9->CreateAdditionalSwapChain(pParams==0L?0L:&Params, ppSwapChain==0L?0L:&pSwap9);
	if(FAILED(hr)||0L==ppSwapChain)
	{
		DPF(1, _T("CDevice::CreateAdditionalSwapChain9() - CreateAdditionalSwapChain failed. hr=%s(%x).\n"), m_pD3D->HResultToString(hr), hr);
		goto Error;
	}

	*ppSwapChain = new CSwapChain();
	if(*ppSwapChain == NULL)
	{
		DPF(1, _T("CDevice::CreateAdditionalSwapChain9() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	if(!(*ppSwapChain)->Init(pSwap9, pParams, this))
	{
		DPF(1, _T("CDevice::CreateAdditionalSwapChain9() - failed to init CSurface.\n"));
		hr = E_FAIL;
		goto Error;
	}

	RELEASE(pSwap9);
	return S_OK;

Error:
	RELEASE(pSwap9);
	if (ppSwapChain)
		RELEASE(*ppSwapChain);

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::StretchRect9(CSurface* pSourceSurface,CONST RECT* pSourceRect,CSurface* pDestSurface,CONST RECT* pDestRect, DWORD dwFilter)
{
	return SetLastError(m_pDevice9->StretchRect(pSourceSurface == NULL ? NULL : pSourceSurface->GetD3DSurface9(), pSourceRect, pDestSurface == NULL ? NULL : pDestSurface->GetD3DSurface9(), pDestRect, (D3DTEXTUREFILTERTYPE)dwFilter));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::ColorFill9(CSurface* pSurface,CONST RECT* pRect,DWORD color)
{
	return SetLastError(m_pDevice9->ColorFill(pSurface == NULL ? NULL : pSurface->GetD3DSurface9(), pRect, (D3DCOLOR)color));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateOffscreenPlainSurface9(UINT Width,UINT Height,FORMAT Format, DWORD dwPool, CSurface** ppSurface, HANDLE *pSharedHandle)
{
	LPDIRECT3DSURFACE9 pD3DSurface = NULL;
	LPDIRECT3DSURFACE9 pTempSurface = NULL;
	LPDIRECT3DSURFACE9 *ppD3DSurface = NULL;
	CSurface* pSurface = new CSurface;
	
	if( pSurface == NULL )
	{
		DPF(1, _T("CDevice::CreateOffscreenPlainSurface9() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}

	if (ppSurface)
	{
		if (*ppSurface && pSharedHandle)
		{
			pTempSurface = (*ppSurface)->GetD3DSurface9();
			ppD3DSurface = &pTempSurface;
		}
		else
		{
			ppD3DSurface = &pD3DSurface;
		}
	}
	
	HRESULT hr = m_pDevice9->CreateOffscreenPlainSurface(Width,Height,Format.d3dfFormat,(D3DPOOL)dwPool, ppD3DSurface, pSharedHandle);
	if( FAILED( hr ) || 0L==ppSurface )
	{
		DPF(1, _T("CDevice::CreateOffscreenPlainSurface9() - CreateOffscreenPlainSurface() failed.\n"));
		RELEASE( pSurface );
		return SetLastError(hr);
	}
	
	if( !pSurface->Init(*ppD3DSurface, this, NULL) )
	{
		DPF(1, _T("CDevice::CreateOffscreenPlainSurface9() - CSurface::Init() failed.\n"));
		RELEASE( pSurface );
		RELEASE( pD3DSurface );
		RELEASE(pTempSurface);
		return SetLastError(E_FAIL);
	}
	
	if( ppSurface )
		*ppSurface = pSurface;
	
	RELEASE(pD3DSurface);
	RELEASE(pTempSurface);
	
	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetScissorRect9(CONST RECT* pRect)
{
	return SetLastError(m_pDevice9->SetScissorRect(pRect));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetScissorRect9(RECT* pRect)
{
	return SetLastError(m_pDevice9->GetScissorRect(pRect));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetVertexDeclaration9(CVertexDeclaration** ppDecl)
{
	HRESULT hr = S_OK;
	LPDIRECT3DVERTEXDECLARATION9 pDecl9 = NULL;
	CVertexDeclaration *pDecl = NULL;

	hr = m_pDevice9->GetVertexDeclaration(&pDecl9);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::GetVertexDeclaration9() - GetVertexDeclaration failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	pDecl = new CVertexDeclaration();
	if(NULL == pDecl)
	{
		DPF(1, _T("CDevice::GetVertexDeclaration9() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	if(!pDecl->Init(pDecl9, this))
	{
		DPF(1, _T("CDevice::GetVertexDeclaration9() - failed to init wrappers.\n"));
		hr = E_FAIL;
		goto Exit;
	}

Exit:
	RELEASE(pDecl9);
	if(FAILED(hr))
	{
		RELEASE(pDecl);
	}
	else
		*ppDecl = pDecl;

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetFVF9(DWORD* pFVF)
{
	return SetLastError(m_pDevice9->GetFVF(pFVF));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetVertexShaderConstantF9(UINT Register,CONST float* pConstantData,UINT ConstantCount)
{
	return SetLastError(m_pDevice9->SetVertexShaderConstantF(Register, pConstantData, ConstantCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetVertexShaderConstantF9(UINT Register,float* pConstantData,UINT ConstantCount)
{
	return SetLastError(m_pDevice9->GetVertexShaderConstantF(Register, pConstantData, ConstantCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetVertexShaderConstantI9(UINT Register,CONST int* pConstantData,UINT ConstantCount)
{
	return SetLastError(m_pDevice9->SetVertexShaderConstantI(Register, pConstantData, ConstantCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetVertexShaderConstantI9(UINT Register,int* pConstantData,UINT ConstantCount)
{
	return SetLastError(m_pDevice9->GetVertexShaderConstantI(Register, pConstantData, ConstantCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetVertexShaderConstantB9(UINT StartRegister,CONST BOOL* pConstantData, UINT BoolCount)
{
	return SetLastError(m_pDevice9->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetVertexShaderConstantB9(UINT StartRegister,BOOL* pConstantData, UINT BoolCount)
{
	return SetLastError(m_pDevice9->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetPixelShader9(CPShader* pShader)
{
	return SetLastError(m_pDevice9->SetPixelShader(pShader ? pShader->GetShader9() : NULL));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetSamplerState9(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD Value)
{
	return SetLastError(m_pDevice9->SetSamplerState(Sampler, (D3DSAMPLERSTATETYPE) Type, Value));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetSamplerState9(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD *Value)
{
	return SetLastError(m_pDevice9->GetSamplerState(Sampler, (D3DSAMPLERSTATETYPE) Type, Value));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetDialogBoxMode9(BOOL bEnableDialogs)
{
	return SetLastError(m_pDevice9->SetDialogBoxMode(bEnableDialogs));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetSoftwareVertexProcessing9(BOOL bSoftware)
{
	return SetLastError(m_pDevice9->SetSoftwareVertexProcessing(bSoftware));
}

// --------------------------------------------------------------------------------------

BOOL CDevice::GetSoftwareVertexProcessing9()
{
	return m_pDevice9->GetSoftwareVertexProcessing();
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetSwapChain9(UINT iSwapChain,CSwapChain** pSwapChain)
{
	HRESULT hr = 0;
	IDirect3DSwapChain9 *pSwap9 = NULL;
	
	hr = m_pDevice9->GetSwapChain(iSwapChain, pSwapChain==0L?0L:&pSwap9);
	if(FAILED(hr)||0L==pSwapChain)
	{
		DPF(1, _T("CDevice::GetSwapChain9() - GetSwapChain failed. hr=%s(%x).\n"), m_pD3D->HResultToString(hr), hr);
		goto Error;
	}

	*pSwapChain = new CSwapChain();
	if(*pSwapChain == NULL)
	{
		DPF(1, _T("CDevice::GetSwapChain9() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	if(!(*pSwapChain)->Init(pSwap9, m_pPresParams + iSwapChain, this))
	{
		DPF(1, _T("CDevice::GetSwapChain9() - failed to init CSurface.\n"));
		hr = E_FAIL;
		goto Error;
	}

	RELEASE(pSwap9);
	return S_OK;

Error:
	RELEASE(pSwap9);
	if (pSwapChain)
		RELEASE(*pSwapChain);

	return hr;
}

// --------------------------------------------------------------------------------------

UINT CDevice::GetNumberOfSwapChains9()
{
	return m_pDevice9->GetNumberOfSwapChains();
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::CreateQuery9(QUERYTYPE Type, CQuery** ppQuery)
{
	HRESULT hr = S_OK;
	CQuery *pQuery = NULL;
	IDirect3DQuery9 *pQuery9 = NULL;

	hr = m_pDevice9->CreateQuery((D3DQUERYTYPE)Type, PassPointer(&pQuery9, ppQuery));
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreateQuery9() - CreateQuery failed. hr=%s(%x).\n"), m_pD3D->HResultToString(hr), hr);
		goto Error;
	}

	if (0L==ppQuery)
		return SetLastError(hr);

	pQuery = *ppQuery = new CQuery();
	if(pQuery == NULL)
	{
		DPF(1, _T("CDevice::CreateQuery9() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	if(!pQuery->Init(pQuery9, this))
	{
		DPF(1, _T("CDevice::CreateQuery9() - failed to init CQuery.\n"));
		hr = E_FAIL;
		goto Error;
	}

	RELEASE(pQuery9);
	return SetLastError(hr);

Error:
	RELEASE(pQuery9);
	RELEASE(pQuery);

	return SetLastError(hr);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetStreamSourceFreq9(UINT nStreamNumber, UINT nDivider)
{
	return SetLastError(m_pDevice9->SetStreamSourceFreq(nStreamNumber, nDivider));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetStreamSourceFreq9(UINT nStreamNumber, UINT* pDividerOut)
{
	return SetLastError(m_pDevice9->GetStreamSourceFreq(nStreamNumber, pDividerOut));
}

// --------------------------------------------------------------------------------------

void CDevice::D3DXInitializePSGP9()
{
	D3DXMATRIX Matrix;
	D3DXMatrixIdentity(&Matrix);
	D3DXMatrixMultiply(&Matrix, &Matrix, &Matrix);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetConvolutionMonoKernel9(UINT uWidth, UINT uHeight, float * pfRow, float * pfCol)
{
	ASSERT( NULL != m_pDevice9Ex );
	return SetLastError(m_pDevice9Ex->SetConvolutionMonoKernel(uWidth,uHeight,pfRow,pfCol));
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::ComposeRects9( LPSURFACE pSrc, LPSURFACE pDst, LPVERTEXBUFFER pSrcRectDescs, UINT NumRects, LPVERTEXBUFFER pDstRectDescs, UINT Operation, int Xoffset, int Yoffset )
{
	ASSERT( NULL != m_pDevice9Ex );
	return SetLastError( m_pDevice9Ex->ComposeRects( pSrc->m_pSurface9, pDst->m_pSurface9, pSrcRectDescs->m_pVB9, NumRects, pDstRectDescs->m_pVB9, (D3DCOMPOSERECTSOP)Operation, Xoffset, Yoffset ) );
}

HRESULT CDevice::PresentEx9( CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags )
{
	ASSERT( NULL != m_pDevice9Ex );
	return SetLastError( m_pDevice9Ex->PresentEx( pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags ) );
}

HRESULT CDevice::ResetEx9( PRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode )
{
	ASSERT( NULL != m_pDevice9Ex );
	HRESULT hr = S_OK;
	UINT nNumAdapters = 0;
    if ( ( m_dwBehaviors & DEVICECREATE_ADAPTERGROUP_DEVICE ) == DEVICECREATE_ADAPTERGROUP_DEVICE )
	{
		nNumAdapters = GetCaps()->NumberOfAdaptersInGroup;

		// We can only drive multiple heads from the master.
		if( nNumAdapters == 0 )
		{
			DPF
			(
				1,
				_T("CDevice::ResetEx9:  Can't drive multiple heads from slave head.\n")
			);

			hr = E_UNSUPPORTED;
			goto Exit;		
		};

/*
		DPF
		(
			3,
			_T( "CDevice::Initialize9:  Initializing adapter with %u heads.\n" ),
			nNumAdapters
		);
*/
	}
	else
	{
		nNumAdapters = 1;

/*
		DPF
		(
			3,
			_T( "CDevice::Initialize9:  Initializing single-head adapter.\n" )
		);
*/
	};

	D3DPRESENT_PARAMETERS *pd3dpp = new D3DPRESENT_PARAMETERS[nNumAdapters];

	if(NULL == pd3dpp)
	{
		DPF(1, _T("CDevice::Reset9() - Out of memory.\n"));
		return E_OUTOFMEMORY;
	}
	memset(pd3dpp, 0, sizeof(D3DPRESENT_PARAMETERS) * nNumAdapters);
	
	for(int i=0; i<nNumAdapters; i++)
	{
		pd3dpp[i].BackBufferWidth					= pPresentationParameters[i].uWidth;
		pd3dpp[i].BackBufferHeight					= pPresentationParameters[i].uHeight;
		pd3dpp[i].BackBufferFormat					= pPresentationParameters[i].BackBufferFormat.d3dfFormat;
		pd3dpp[i].BackBufferCount					= pPresentationParameters[i].uBackBufferCount;
		pd3dpp[i].MultiSampleType					= (D3DMULTISAMPLE_TYPE)pPresentationParameters[i].MultiSample;
		pd3dpp[i].SwapEffect						= (D3DSWAPEFFECT)pPresentationParameters[i].SwapEffect;
		pd3dpp[i].hDeviceWindow 					= pPresentationParameters[i].hDeviceWindow;
		pd3dpp[i].Windowed							= pPresentationParameters[i].bWindowed;
		pd3dpp[i].EnableAutoDepthStencil			= pPresentationParameters[i].bAutoDepthStencil;
		pd3dpp[i].AutoDepthStencilFormat			= pPresentationParameters[i].AutoDepthStencilFormat.d3dfFormat;
		pd3dpp[i].Flags 							= pPresentationParameters[i].dwFlags;
		pd3dpp[i].FullScreen_RefreshRateInHz		= pPresentationParameters[i].uFullscreenRefresh;
		pd3dpp[i].PresentationInterval				= pPresentationParameters[i].uPresentInterval;
	}

	hr = m_pDevice9Ex->ResetEx( pPresentationParameters == NULL ? NULL : pd3dpp, pFullscreenDisplayMode );

	SAFEDELETEA(pd3dpp);

Exit:
	return SetLastError(hr);
}

HRESULT CDevice::GetGPUThreadPriority9( INT * pPriority )
{
	ASSERT( NULL != m_pDevice9Ex );
	return SetLastError( m_pDevice9Ex->GetGPUThreadPriority( pPriority ) );
}

HRESULT CDevice::SetGPUThreadPriority9( INT Priority )
{
	ASSERT( NULL != m_pDevice9Ex );
	return SetLastError( m_pDevice9Ex->SetGPUThreadPriority( Priority ) );
}

HRESULT CDevice::WaitForVBlank9( UINT SwapChainIndex )
{
	ASSERT( NULL != m_pDevice9Ex );
	return SetLastError( m_pDevice9Ex->WaitForVBlank( SwapChainIndex ) );
}

HRESULT CDevice::CheckResourceResidency9(CResource** pResourceArray, UINT numResources)
{
	HRESULT hResult = E_FAIL;
	IDirect3DResource9** pResources = new IDirect3DResource9*[numResources];
	if(NULL == pResources)
	{
		DPF(1, _T("CDevice::CheckResourceResidency9() - Out of memory.\n"));
		return SetLastError(E_OUTOFMEMORY);
	}
	
	if( NULL != pResources )
	{
		for( int i = 0; i < numResources; i++)
		{
			pResources[i] = (IDirect3DResource9*)pResourceArray[i]->GetResource();
		}

		ASSERT( NULL != m_pDevice9Ex );
		hResult = m_pDevice9Ex->CheckResourceResidency(pResources, numResources);
	}
	delete [] pResources;
	return SetLastError(hResult);
}

HRESULT CDevice::SetMaximumFrameLatency9(UINT MaxLatency)
{
	ASSERT( NULL != m_pDevice9Ex );
	return SetLastError( m_pDevice9Ex->SetMaximumFrameLatency( MaxLatency ) );
}

HRESULT CDevice::GetMaximumFrameLatency9(UINT* pMaxLatency)
{
	ASSERT( NULL != m_pDevice9Ex );
	return SetLastError( m_pDevice9Ex->GetMaximumFrameLatency( pMaxLatency ) );
}

HRESULT CDevice::ExtractResource9(PVOID pResourceIn, RESOURCETYPE type, PVOID* ppResourceOut)
{
	HRESULT hResult = E_FAIL;
	LPDIRECT3DRESOURCE9 pD3DResource = NULL;

	if( RTYPE_TEXTURE == type || RTYPE_VOLUMETEXTURE == type || RTYPE_CUBETEXTURE == type )
	{
		CBaseTexture* pTex = (CBaseTexture*)pResourceIn;
		pD3DResource = (LPDIRECT3DRESOURCE9)pTex->GetD3DTexture9();
	}
	else if( RTYPE_VERTEXBUFFER == type )
	{
		CVertexBuffer* pVB = (CVertexBuffer*)pResourceIn;
		pD3DResource = (LPDIRECT3DRESOURCE9)pVB->GetVertexBufferPointer9();
	}
	else if( RTYPE_INDEXBUFFER == type )
	{
		CIndexBuffer* pIB = (CIndexBuffer*)pResourceIn;
		pD3DResource = (LPDIRECT3DRESOURCE9)pIB->GetIndexBufferPointer9();
	}
	else if( RTYPE_SURFACE == type )
	{
		CSurface* pSurface = (CSurface*)pResourceIn;
		pD3DResource = (LPDIRECT3DRESOURCE9)pSurface->GetD3DSurface9();
	}

	if( NULL != pD3DResource && NULL != ppResourceOut )
	{
		*ppResourceOut = (PVOID)pD3DResource;
		hResult = S_OK;
	}

	return SetLastError( hResult );
}

HRESULT CDevice::CreateOffscreenPlainSurfaceEx9(UINT Width,UINT Height,FORMAT Format, DWORD dwPool, CSurface** ppSurface, HANDLE *pSharedHandle, DWORD dwUsage)
{
	LPDIRECT3DSURFACE9 pD3DSurface = NULL;
	LPDIRECT3DSURFACE9 pTempSurface = NULL;
	LPDIRECT3DSURFACE9 *ppD3DSurface = NULL;
	CSurface* pSurface = new CSurface;
	
	if( pSurface == NULL )
	{
		DPF(1, _T("CDevice::CreateOffscreenPlainSurfaceEx9() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}

	if (ppSurface)
	{
		if (*ppSurface && pSharedHandle)
		{
			pTempSurface = (*ppSurface)->GetD3DSurface9();
			ppD3DSurface = &pTempSurface;
		}
		else
		{
			ppD3DSurface = &pD3DSurface;
		}
	}
	
	ASSERT( NULL != m_pDevice9Ex );
	HRESULT hr = m_pDevice9Ex->CreateOffscreenPlainSurfaceEx(Width,Height,Format.d3dfFormat,(D3DPOOL)dwPool, ppD3DSurface, pSharedHandle, dwUsage);
	if( FAILED( hr ) || 0L==ppSurface )
	{
		DPF(1, _T("CDevice::CreateOffscreenPlainSurfaceEx9() - CreateOffscreenPlainSurfaceEx() failed.\n"));
		RELEASE( pSurface );
		return SetLastError(hr);
	}
	
	if( !pSurface->Init(*ppD3DSurface, this, NULL) )
	{
		DPF(1, _T("CDevice::CreateOffscreenPlainSurfaceEx9() - CSurface::Init() failed.\n"));
		RELEASE( pSurface );
		RELEASE( pD3DSurface );
		RELEASE(pTempSurface);
		return SetLastError(E_FAIL);
	}
	
	if( ppSurface )
		*ppSurface = pSurface;
	
	RELEASE(pD3DSurface);
	RELEASE(pTempSurface);
	
	return SetLastError(hr);
}

HRESULT CDevice::CreateRenderTargetEx9(UINT Width,UINT Height,FORMAT Format,
									 MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,
									 BOOL Lockable,CSurface** ppSurface, HANDLE *pSharedHandle,
									 DWORD dwUsage)
{
	LPDIRECT3DSURFACE9 pD3DSurface = NULL;
	LPDIRECT3DSURFACE9 pTempSurface = NULL;
	LPDIRECT3DSURFACE9 *ppD3DSurface = NULL;
	CSurface* pSurface = new CSurface;
	
	if( pSurface == NULL )
	{
		DPF(1, _T("CDevice::CreateRenderTargetEx9() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}

	if (ppSurface)
	{
		if (*ppSurface && pSharedHandle)
		{
			pTempSurface = (*ppSurface)->GetD3DSurface9();
			ppD3DSurface = &pTempSurface;
		}
		else
		{
			ppD3DSurface = &pD3DSurface;
		}
	}

	ASSERT( NULL != m_pDevice9Ex );
	HRESULT hr = m_pDevice9Ex->CreateRenderTargetEx(Width,Height,Format.d3dfFormat,(D3DMULTISAMPLE_TYPE)MultiSample,MultisampleQuality,Lockable,ppD3DSurface, pSharedHandle, dwUsage);
	if( FAILED( hr ) || 0L == ppSurface )
	{
		DPF(1, _T("CDevice::CreateRenderTargetEx9() - CreateRenderTargetEx() failed.\n"));
		RELEASE(pSurface);
		return SetLastError(hr);
	}

	if( !pSurface->Init(*ppD3DSurface, this, NULL) )
	{
		DPF(1, _T("CDevice::CreateRenderTargetEx9() - CSurface::Init() failed.\n"));
		RELEASE( pD3DSurface );
		RELEASE(pSurface);
		RELEASE(pTempSurface);
		return SetLastError(E_FAIL);
	}

	if( ppSurface )
		*ppSurface = pSurface;

	RELEASE(pD3DSurface);
	RELEASE(pTempSurface);

	return SetLastError(hr);
}

HRESULT CDevice::CreateDepthStencilSurfaceEx9(UINT Width,UINT Height,FORMAT Format,
											MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,
											BOOL Discard, CSurface** ppSurface, HANDLE *pSharedHandle,
											DWORD dwUsage)
{
	LPDIRECT3DSURFACE9 pD3DSurface = NULL;
	LPDIRECT3DSURFACE9 pTempSurface = NULL;
	LPDIRECT3DSURFACE9 *ppD3DSurface = NULL;
	CSurface* pSurface = new CSurface;
	
	if( pSurface == NULL )
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurfaceEx9() - Out of memory.\n"));
		return SetLastError(E_FAIL);
	}
	
	if (ppSurface)
	{
		if (*ppSurface && pSharedHandle)
		{
			pTempSurface = (*ppSurface)->GetD3DSurface9();
			ppD3DSurface = &pTempSurface;
		}
		else
		{
			ppD3DSurface = &pD3DSurface;
		}
	}

	ASSERT( NULL != ppD3DSurface );
	ASSERT( NULL != m_pDevice9Ex );

	HRESULT hr = m_pDevice9Ex->CreateDepthStencilSurfaceEx(
		Width,
		Height,
		Format.d3dfFormat,
		(D3DMULTISAMPLE_TYPE) MultiSample,
		MultisampleQuality,
		Discard,
		ppD3DSurface,
		pSharedHandle,
		dwUsage);

	if( FAILED( hr ) )
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurfaceEx9() - CreateDepthStencilSurfaceEx() failed.\n"));
		RELEASE( pSurface );
		return SetLastError(hr);
	}

	ASSERT( NULL != *ppD3DSurface );

	if( !pSurface->Init(*ppD3DSurface, this, NULL) )
	{
		DPF(1, _T("CDevice::CreateDepthStencilSurfaceEx9() - CSurface::Init() failed.\n"));
		RELEASE( pSurface );
		RELEASE( pD3DSurface );
		RELEASE(pTempSurface);
		return SetLastError(E_FAIL);
	}

	if( ppSurface )
		*ppSurface = pSurface;

	RELEASE(pD3DSurface);
	RELEASE(pTempSurface);

	return SetLastError(hr);
}

HRESULT CDevice::RenderToQuad( RECT * pRect, CTexture * pTexture )
{
	HRESULT hResult = S_OK;
	DWORD dwFVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

	struct FLEXVERTEX
	{
		float fPx;
		float fPy;
		float fPz;
		float fRHW;
	};

	struct TEXCOORD
	{
		float fPx;
		float fPy;
	};

	struct TLVERT
	{
		FLEXVERTEX	Position;
		TEXCOORD	Texcoords;
	};

	TLVERT pVert[4];

	//Prepare geometry
	{
		pVert[0].Position.fPx = pRect->left - 0.5f; pVert[0].Position.fPy = pRect->bottom - 0.5f; pVert[0].Position.fPz = 0; pVert[0].Position.fRHW = 1.0f;
		pVert[0].Texcoords.fPx = 0; pVert[0].Texcoords.fPy = 1;
		pVert[1].Position.fPx = pRect->left - 0.5f; pVert[1].Position.fPy = pRect->top - 0.5f; pVert[1].Position.fPz = 0; pVert[1].Position.fRHW = 1.0f;
		pVert[1].Texcoords.fPx = 0; pVert[1].Texcoords.fPy = 0;
		pVert[2].Position.fPx = pRect->right - 0.5f; pVert[2].Position.fPy = pRect->bottom - 0.5f; pVert[2].Position.fPz = 0; pVert[2].Position.fRHW = 1.0f;
		pVert[2].Texcoords.fPx = 1; pVert[2].Texcoords.fPy = 1;
		pVert[3].Position.fPx = pRect->right - 0.5f; pVert[3].Position.fPy = pRect->top - 0.5f; pVert[3].Position.fPz = 0; pVert[3].Position.fRHW = 1.0f;
		pVert[3].Texcoords.fPx = 1; pVert[3].Texcoords.fPy = 0;
	}

	do
	{
		//Set new FVF
		hResult = SetFVF( dwFVF );
		if ( FAILED( hResult ) )
		{
			DPF( 1, _T( "CDevice::RenderToQuad9() - SetFVF failed. hr=%s(%x).\n" ), 
				m_pD3D->HResultToString( hResult ), hResult );
			break;
		}

		hResult = SetTexture(0, pTexture);
		if ( FAILED( hResult ) )
		{
			DPF( 1, _T( "CDevice::RenderToQuad9() - SetTexture failed. hr=%s(%x).\n" ), 
				m_pD3D->HResultToString( hResult ), hResult );
			break;
		}
		hResult = SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		if ( FAILED( hResult ) )
		{
			DPF( 1, _T( "CDevice::RenderToQuad9() - SetTextureStageState failed. hr=%s(%x).\n" ), 
				m_pD3D->HResultToString( hResult ), hResult );
			break;
		}
		hResult = SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		if ( FAILED( hResult ) )
		{
			DPF( 1, _T( "CDevice::RenderToQuad9() - SetTextureStageState failed. hr=%s(%x).\n" ), 
				m_pD3D->HResultToString( hResult ), hResult );
			break;
		}

		hResult = DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVert, sizeof(TLVERT) );
		if ( FAILED( hResult ) )
		{
			DPF( 1, _T( "CDevice::RenderToQuad9() - DrawPrimitive failed. hr=%s(%x).\n" ), 
				m_pD3D->HResultToString( hResult ), hResult );
			break;
		}
	}while ( 0 );

	return hResult;
}

typedef IDirect3D9* (WINAPI *D3DCREATE)(UINT);
const CAPS* CDevice::GetCachedCaps9()
{
	if ( !m_pCaps9Cached )
	{
		D3DCREATE pProc;
		HMODULE hdll = LoadLibraryExW( L"d3d9.dll", NULL, 0 );
		LPDIRECT3D9 pD3D9;
		D3DDEVTYPE devType;

		switch (m_dwDevType)
		{
			case DEVICETYPE_HAL:
				devType = D3DDEVTYPE_HAL;
			break;

			case DEVICETYPE_REF:
				devType = D3DDEVTYPE_REF;
			break;

			case DEVICETYPE_SW:
				devType = D3DDEVTYPE_SW;
			break;

			case DEVICETYPE_NULLREF:
				devType = D3DDEVTYPE_NULLREF;
			break;

			default:
				DPF(1, _T("CDevice::GetCachedCaps9() - Unknown device type.\n"));
			return NULL;
		}

		if(NULL == hdll)
		{
			DPF(1, _T("CDevice::GetCachedCaps9() - LoadLibrary failed for d3d9.dll.\n"));
			return NULL;
		}

		pProc = (D3DCREATE)GetProcAddress(hdll, "Direct3DCreate9");
		if (NULL == pProc)
		{
			DPF(1, _T("CDevice::GetCachedCaps9() - Couldn't get Direct3DCreate9 address for d3d9.dll.\n"));
			return NULL;
		}

		pD3D9 = pProc(D3D_SDK_VERSION);
		if (NULL == pD3D9)
		{
			DPF(1, _T("CDevice::GetCachedCaps9() - Direct3DCreate9() failed for d3d9.dll.  Header/Runtime mismatch likely.\n"));
			return NULL;
		}


		if ( D3DDEVTYPE_SW == devType )
		{
			HMODULE hSWRast = NULL;			
			typedef HRESULT (WINAPI *PD3D9GETSWINFO)
			(
				LPVOID pCaps,
				LPVOID pCallbacks,
				DWORD*,
				LPVOID,
				DWORD*,
				LPVOID
			);
			PD3D9GETSWINFO fn = ( PD3D9GETSWINFO ) NULL;

			if (!m_pD3D->m_bUserSelectedSWRast)
			{
				hSWRast = LoadLibraryExW( L"d3dref9.dll", NULL, 0 );
				fn = (PD3D9GETSWINFO) GetProcAddress( hSWRast, _T("D3D9GetSWInfo") );
			}
			else
			{
				hSWRast = LoadLibraryEx( m_pD3D->m_sSWRast, NULL, 0 );
				fn = (PD3D9GETSWINFO) GetProcAddress( hSWRast, m_pD3D->m_sSWRastEntry );
			}		

			if ( hSWRast == NULL )
			{
				DPF
				(
					1,
					"CDevice::GetCachedCaps9() - Failed to load software rasterizer %s.\n",
					m_pD3D->m_bUserSelectedSWRast? m_pD3D->m_sSWRast : _T("d3dref9.dll")
				);

				return false;
			}
						
			if ( fn == NULL )
			{
				DPF
				(
					1,
					"CDevice::GetCachedCaps9() - Failed to locate software rasterizer entry point %s.\n",
					m_pD3D->m_bUserSelectedSWRast ? m_pD3D->m_sSWRastEntry : _T("D3D9GetSWInfo")
				);

				if ( hSWRast != NULL )
				{
					FreeLibrary( hSWRast );
					hSWRast = NULL;
				};

				return false;
			}

			
			HRESULT hr = pD3D9->RegisterSoftwareDevice( fn );
			if ( FAILED( hr ) ) 
			{
				DPF(1, _T("CDevice::GetCachedCaps9() - Failed to register software rasterizer. hr=%x\n"), hr);

				if ( hSWRast != NULL )
				{
					FreeLibrary( hSWRast );
					hSWRast = NULL;
				};

				return false;
			}

			
			m_pCaps9Cached = new CAPS;
			if ( m_pCaps9Cached == NULL )
			{
				DPF(1, _T("CDevice::GetCachedCaps9() - Out of memory.\n"));

				if ( hSWRast != NULL )
				{
					FreeLibrary( hSWRast );
					hSWRast = NULL;
				};

				return false;
			}


			if ( FAILED(pD3D9->GetDeviceCaps( m_uAdapter, devType, (D3DCAPS9*)m_pCaps9Cached )) )
			{
				DPF(1, _T("CDevice::GetCachedCaps9() - GetDeviceCaps failed.\n"));

				delete m_pCaps9Cached;
				m_pCaps9Cached = NULL;

				if ( hSWRast != NULL )
				{
					FreeLibrary( hSWRast );
					hSWRast = NULL;
				};

				return NULL;
			}

			if ( hSWRast != NULL )
			{
				FreeLibrary( hSWRast );
				hSWRast = NULL;
			};
		}
		else
		{
			m_pCaps9Cached = new CAPS;
			if ( m_pCaps9Cached == NULL )
			{
				DPF(1, _T("CDevice::GetCachedCaps9() - Out of memory.\n"));
				return false;
			}


			if ( FAILED(pD3D9->GetDeviceCaps( m_uAdapter, devType, (D3DCAPS9*)m_pCaps9Cached )) )
			{
				DPF(1, _T("CDevice::GetCachedCaps9() - GetDeviceCaps failed.\n"));

				delete m_pCaps9Cached;
				m_pCaps9Cached = NULL;

				return NULL;
			}
		}
		
		pD3D9->Release();
		pD3D9 = NULL;

		FreeLibrary( hdll );
		hdll = NULL;

		return m_pCaps9Cached;
	}


	return m_pCaps9Cached;
}


HRESULT	CDevice::CheckDeviceState9(HWND hWindow)
{
	ASSERT( NULL != m_pDevice9Ex );
    return SetLastError( m_pDevice9Ex->CheckDeviceState( hWindow ) );
}
