// ======================================================================================
//
//  CD3D8.cpp
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================

#include "D3DPCH.h"

#include <d3d8rgbrast.h>
#include <d3dx8.h>
#include "CD3D.h"
#include <strsafe.h>

typedef IDirect3D8* (WINAPI *D3DCREATE)(UINT);

// --------------------------------------------------------------------------------------
// Define all known D3D formats

static FMT g_TextureFormatList8[32] = 
{
	FMT_A8R8G8B8,
	FMT_X8R8G8B8,
	FMT_R5G6B5,
	FMT_X1R5G5B5,
	FMT_A1R5G5B5,
	FMT_A4R4G4B4,
	FMT_R3G3B2,
	FMT_A8,
	FMT_A8R3G3B2,
	FMT_X4R4G4B4,
	FMT_R8G8B8,
	FMT_A2B10G10R10,
	FMT_G16R16,

	FMT_A8P8,
	FMT_P8,

	FMT_L8,
	FMT_A8L8,
	FMT_A4L4,

	FMT_V8U8,
	FMT_L6V5U5,
	FMT_X8L8V8U8,
	FMT_Q8W8V8U8,
	FMT_V16U16,
	FMT_W11V11U10,
	FMT_A2W10V10U10,
	
	FMT_UYVY,
	FMT_YUY2,
	FMT_DXT1,
	FMT_DXT2,
	FMT_DXT3,
	FMT_DXT4,
	FMT_DXT5
};
static DWORD g_dwTextureFormatListSize8 = 32;

static FORMAT g_ZBufferFormatList9[7] = 
{
	FMT_D16,

	FMT_D32,
	FMT_D15S1,
	FMT_D24S8,
	FMT_D24X8,
	FMT_D24X4S4,

	FMT_D16_LOCKABLE,
};
static DWORD g_dwZBufferFormatListSize8 = 7;

MULTISAMPLE_TYPE g_MultiSampleTypeList8[] = 
{
	MULTISAMPLE_NONE,
	MULTISAMPLE_2_SAMPLES,
	MULTISAMPLE_3_SAMPLES,
	MULTISAMPLE_4_SAMPLES,
	MULTISAMPLE_5_SAMPLES,
	MULTISAMPLE_6_SAMPLES,
	MULTISAMPLE_7_SAMPLES,
	MULTISAMPLE_8_SAMPLES,
	MULTISAMPLE_9_SAMPLES,
	MULTISAMPLE_10_SAMPLES,
	MULTISAMPLE_11_SAMPLES,
	MULTISAMPLE_12_SAMPLES,
	MULTISAMPLE_13_SAMPLES,
	MULTISAMPLE_14_SAMPLES,
	MULTISAMPLE_15_SAMPLES,
	MULTISAMPLE_16_SAMPLES
};
DWORD g_dwMultiSampleTypeListSize8 = 16;

FMT g_BackBufferFormats8[] = {
	FMT_X8R8G8B8,
	FMT_A8R8G8B8,
	FMT_R5G6B5
};
static DWORD g_dwBackBufferFormats8 = sizeof(g_BackBufferFormats8) / sizeof(FMT);

// --------------------------------------------------------------------------------------

HRESULT CD3D::InitD3D8()
{
	HRESULT                     hr = S_OK;
	D3DCREATE                   pProc;
	D3DADAPTER_IDENTIFIER8      Adapter;
	D3DDISPLAYMODE              Mode;
	DWORD                       dwNumModes;
	DWORD                       dwAdapters;

	// Load the D3D8 library
	m_hSrcD3DDLL = LoadLibraryExW( L"D3D8.DLL", NULL, 0 );
	if(m_hSrcD3DDLL == NULL)
	{
		DPF(1, _T("CD3D::InitD3D8() - failed LoadLibrary for D3D8.DLL.\n"));
		return E_FAIL;
	}
	// Get the D3D creation entry point
	pProc = (D3DCREATE)GetProcAddress(m_hSrcD3DDLL, "Direct3DCreate8");
	if (NULL == pProc)
	{
		DPF(1, _T("CD3D::InitD3D8() - Couldn't get Direct3DCreate8 address\n"));
		hr = E_FAIL;
		goto Error;
	}

	// Create the D3D object
	if(m_fVersion == 8.0f)
		m_pD3D8 = pProc(120);
	else
		m_pD3D8 = pProc(D3D_SDK_VERSION);
	if (NULL == m_pD3D8)
	{
		DPF(1, _T("CD3D::InitD3D8() - Direct3DCreate8() failed.  Header/Runtime mismatch likely.\n"));
		hr = E_ABORT;
		goto Error;
	}

	// Get the debug info from the registry
	GetDebugInfoFromReg();

	// Register any SW rasterizers
	if(!RegisterRasterizer8(m_sSWRast, m_sSWRastEntry))
	{
		DPF(1, _T("CD3D::InitD3D8() - Failed to register SW rasterizer.\n"));
	}

	// Get the number of adapters
	m_nAdapters = m_pD3D8->GetAdapterCount();
	if(0 == m_nAdapters)
	{
		DPF(1, _T("CD3D::InitD3D8() - GetAdapterCount() returned 0.  No display devices available.\n"));
		hr = E_FAIL;
		goto Error;
	}

	// Allocate the ADAPTER infos
	m_pAdapters = new ADAPTER[m_nAdapters];
	if(m_pAdapters == NULL)
	{
		DPF(1, _T("CD3D::InitD3D8() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	// Zero the memory
	memset(m_pAdapters, 0, m_nAdapters * sizeof(ADAPTER));

	// Enumerate the adapters
	for (dwAdapters = 0; dwAdapters < m_nAdapters; dwAdapters++)
	{
		PADAPTER pAdapter = &m_pAdapters[dwAdapters];

		hr = m_pD3D8->GetAdapterIdentifier(dwAdapters, D3DENUM_NO_WHQL_LEVEL, &Adapter);
		if (FAILED(hr))
		{
			DPF(1, _T("CD3D::InitD3D8() - GetAdapterIdentifier() failed.  hr=%s(%x)"), HResultToString(hr));
			goto Error;
		}

		// create the version string.
		{
			TCHAR tcsVer[20];
			memset( tcsVer, 0, sizeof tcsVer );
		
			StringCchPrintf(tcsVer, ARRAYSIZE(tcsVer) - 1, _T("%d.%d.%d.%d"),
				HIWORD(Adapter.DriverVersion.HighPart), // product
				LOWORD(Adapter.DriverVersion.HighPart), // version
				HIWORD(Adapter.DriverVersion.LowPart),  // sub-version
				LOWORD(Adapter.DriverVersion.LowPart)); // build
			COPYSTRING(pAdapter->sVersion,tcsVer);
		}
		
		// get the adapter name
		COPYSTRING(pAdapter->sAdapterName, Adapter.Description);

		// Get the device path for this adapter.
		{
			HMONITOR hMon = GetAdapterMonitor( dwAdapters );
			MONITORINFOEX monInfo;
			monInfo.cbSize = sizeof(MONITORINFOEX);
			if ( !GetMonitorInfo( hMon, &monInfo ) )
			{
				DPF(1, _T("CD3D::InitD3D8() - GetMonitorInfo() failed."));
				goto Error;
			}

			COPYSTRING(pAdapter->sDevicePath, monInfo.szDevice);
		}
		
		// get the driver name
		COPYSTRING(pAdapter->sDriver, Adapter.Driver);
		
		// the vendor and device ID
		pAdapter->dwVendorID = Adapter.VendorId;
		pAdapter->dwChipID = Adapter.DeviceId;

		// Save adapter information to the internal structure
		memcpy(&pAdapter->Guid, &Adapter.DeviceIdentifier, sizeof(GUID));
		COPYSTRING(pAdapter->sDescription, Adapter.Description);
		pAdapter->hMonitor = m_pD3D8->GetAdapterMonitor(dwAdapters);

		// create PnP ID string
		char szBuffer[48];
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T("VEN_%04X&DEV_%04X&SUBSYS_%08X&REV_%02X"),
				Adapter.VendorId, Adapter.DeviceId, Adapter.SubSysId, Adapter.Revision);
		COPYSTRING(pAdapter->sPnPID, szBuffer);
	
		if( NULL == pAdapter->sVersion || NULL == pAdapter->sAdapterName ||
			NULL == pAdapter->sDriver  || NULL == pAdapter->sDescription ||
			NULL == pAdapter->sPnPID || NULL == pAdapter->sDevicePath )
		{
			DPF(1, _T("CD3D::InitD3D8() - Out of memory.\n"));
			goto Error;
		}

		// Obtain current format
		hr = m_pD3D8->GetAdapterDisplayMode(dwAdapters, &Mode);
		if (FAILED(hr))
		{
			DPF(1, _T("CD3D::InitD3D8() - Failed to obtain current display mode, hr=%s(%x)\n"),
						   HResultToString(hr), hr);
			goto Error;
		}
		
    
		// Generate the resolution string
		pAdapter->DesktopMode.dwWidth = Mode.Width;
		pAdapter->DesktopMode.dwHeight = Mode.Height;
		pAdapter->DesktopMode.Format.d3dfFormat = Mode.Format;
		d3df2ddpf(Mode.Format, &pAdapter->DesktopMode.Format.ddpfPixelFormat);
		pAdapter->DesktopMode.dwBPP = pAdapter->DesktopMode.Format.ddpfPixelFormat.dwRGBBitCount;
		pAdapter->DesktopMode.dwRefreshRate = Mode.RefreshRate;

		// Enumerate display modes
		dwNumModes = m_pD3D8->GetAdapterModeCount(dwAdapters);
		
		// Allocate the display modes
		pAdapter->nModes = dwNumModes;
		pAdapter->pModes = new DISPLAYMODE[dwNumModes];
		if(pAdapter->pModes == NULL)
		{
			DPF(1, _T("CD3D::InitD3D8() - Out of memory.\n"));
			hr = E_OUTOFMEMORY;
			goto Error;
		}

		for (int i=0; i < pAdapter->nModes; i++)
		{
			DISPLAYMODE     *pMode = &pAdapter->pModes[i];

			hr = m_pD3D8->EnumAdapterModes(dwAdapters, i, &Mode);
			if (FAILED(hr))
			{
				DPF(1, _T("CD3D::InitD3D8() - EnumAdapterModes failed. hr=%s(%x)\n"), HResultToString(hr), hr);
				goto Error;
			}

			pMode->dwWidth = (DWORD)Mode.Width;
			pMode->dwHeight = (DWORD)Mode.Height;
			pMode->dwRefreshRate = (DWORD)Mode.RefreshRate;
			pMode->Format.d3dfFormat = Mode.Format;
			d3df2ddpf(pMode->Format.d3dfFormat, &pMode->Format.ddpfPixelFormat);
			pMode->dwBPP = pMode->Format.ddpfPixelFormat.dwRGBBitCount;
		}
	}

	UpdateAdapterInfo();

	return hr;

Error:
	ReleaseAll();
	return hr;
}

// --------------------------------------------------------------------------------------

bool CD3D::RegisterRasterizer8
(
	__in LPCTSTR pcsDLLName,
	__in LPCTSTR pcsEntryPointName
)
{
	DPF(4, _T("CD3D::RegisterRasterizer8()\n"));


	bool bOverallResult = false;

	LPCSTR pcsAdjustedDLLName = pcsDLLName;
	LPCSTR pcsAdjustedEntryPointName = pcsEntryPointName;

	typedef HRESULT (WINAPI *PD3D8GETSWINFO)
	(
		LPVOID pCaps,
		LPVOID pCallbacks,
		DWORD*,
		LPVOID
	);

	PD3D8GETSWINFO fn = ( PD3D8GETSWINFO ) NULL;


	if ((m_fMaxDXVersion >= 9.f || m_fBuild > 526.f) && !m_bUserSelectedSWRast)
	{
		pcsAdjustedDLLName = _T("d3dref8.dll");
		pcsAdjustedEntryPointName = _T("D3D8GetSWInfo");

		// Save the rasterizer name
		COPYSTRING(m_sSWRast, pcsAdjustedDLLName);
		if (m_sSWRast == NULL)
		{
			DPF(1, _T("CD3D::RegisterRasterizer8() - Out of memory.\n"));

			goto Cleanup;
		}

		COPYSTRING(m_sSWRastEntry, pcsAdjustedEntryPointName);
		if (m_sSWRastEntry == NULL)
		{
			DPF(1, _T("CD3D::RegisterRasterizer8() - Out of memory.\n"));

			goto Cleanup;
		}
	};


	m_hSWRast = LoadLibraryEx( pcsAdjustedDLLName, NULL, 0 );
	if ( m_hSWRast == NULL )
	{
		DPF
		(
			1,
			"CD3D::RegisterRasterizer8() - Failed to load software rasterizer %s.\n",
			pcsAdjustedDLLName
		);

		goto Cleanup;
	}


	fn = (PD3D8GETSWINFO) GetProcAddress( m_hSWRast, pcsAdjustedEntryPointName );
	if ( fn == NULL )
	{
		DPF
		(
			1,
			"CD3D::RegisterRasterizer8() - Failed to locate software rasterizer entry point %s.\n",
			pcsAdjustedEntryPointName
		);

		goto Cleanup;
	}


	HRESULT hRegisterRasterizerResult = m_pD3D8->RegisterSoftwareDevice(fn);
	if ( FAILED( hRegisterRasterizerResult ) ) 
	{
		DPF
		(
			1,
			_T("CD3D::RegisterRasterizer8() - Failed to register software rasterizer. hr=%x\n"),
			hRegisterRasterizerResult
		);

		goto Cleanup;
	}


	bOverallResult = true;


Cleanup:
	fn = NULL;

	if ( bOverallResult != true )
	{
		if ( m_hSWRast != NULL )
		{
			FreeLibrary( m_hSWRast );
			m_hSWRast = NULL;
		};
	}


	return bOverallResult;
}

// --------------------------------------------------------------------------------------

void CD3D::ReleaseD3D8()
{
	ULONG dwRef = 0;

	UnRegisterRasterizer();

	if(m_pD3D8)
	{
		dwRef = m_pD3D8->Release();
		if ( dwRef > 0 )
		{
			DPF(1, _T("CD3D::ReleaseAll8() - m_pD3D8 ref count nonzero(%d).  TestApp leaking D3D objects.\n"), dwRef);

			ASSERT(g_pFramework != NULL);
			if(g_pFramework->m_bStressMode)
			{
				ASSERT(dwRef == 0);
			}
		}

		m_pD3D8 = NULL;
	}
}


///////////////////////////////////////////////////////
// Function:  ResolveBehaviors8
//
// Purpose:
// Resolves ambiguous behavior flags requested by a
// test group to a mask of actual behavior flags.
//
// Details:
// On entry to this function, fBehaviors is a mask of
// DEVICECREATE_xxx flags.  Several of these are
// either/or types of flags which need to be turned
// into one or the other:
// - DEVICECREATE_SW_OR_HW_VP maps to DEVICECREATE_HWVP
//   if the device supports hardware vertex processing,
//   or DEVICECREATE_SWVP otherwise.
// - DEVICECREATE_HWVP_OR_PURE maps to the union of
//   DEVICECREATE_HWVP with either DEVICECREATE_PURE
//   if the device supports PURE, or else just
//   DEVICECREATE_STANDARD if it doesn't.
// - DEVICECREATE_BESTAVAILABLE turns out to be the
//   same as DEVICECREATE_HWVP_OR_PURE.
//
// Additionally, the test may have requested features
// that the device doesn't support:
// - If the device doesn't support hardware vertex
//   processing, then DEVICECREATE_HWVP and
//   DEVICECREATE_MIXED are both bumped down to
//   DEVICECREATE_SWVP.
// - DEVICECREATE_PURE negates DEVICECREATE_MIXED.
//
// Note that we don't necessarily mask out behavior
// flags like DEVICECREATE_STANDARD when we see other
// flags like DEVICECREATE_PURE.  It is possible that
// the test has asked for both of those, in which case
// it's saying that it wants to be run first on one and
// then a second time on the other.  It could also
// request hardware and software vertex processing,
// meaning that it wants to run first with HWVP, then
// with SWVP.  This all means that we can really only
// mask out flags when we know for certain that the
// device doesn't support them, and thus there's no
// point in including them in the device mode list.
//
// And finally, DEVICECREATE_MIXED is an interesting
// character, because it can be paired with HWVP or
// SWVP.  We only ever mask it out if we determine that
// the device doesn't support HWVP, because that means
// that it doesn't support MIXED either.
//
// This is what happens when people start throwing
// features into a test framework without clearly and
// rigidly thinking through and specifying the expected
// semantics, or the interactions with other features.
///////////////////////////////////////////////////////
bool CD3D::ResolveBehaviors8
(
	IN UINT Adapter,
	IN DWORD DeviceType,
	IN DWORD fBehaviors,
	__out DWORD * pfResolvedBehaviors
)
{
	//
	// Validate parameters
	//

	if ( pfResolvedBehaviors == NULL )
	{
		DPF(1, _T("CD3D::ResolveBehaviors() - invalid pointer.\n"));
		return false;
	};

	CAPS DevCaps;
	HRESULT hr = GetDeviceCaps8(Adapter, DeviceType, &DevCaps);
	if(FAILED(hr))
	{
		DPF(1, _T("CD3D::ResolveBehaviors() - GetDeviceCaps() failed. hr=%s(%x)\n"), HResultToString(hr), hr);
		return false;
	};


	DWORD ResolvedBehaviors = fBehaviors;


	//
	// Decode custom framework behavior flags.
	//
	// N.B. [SEdmison]:
	// Essentially, we're resolving *this-or-that* aliases to
	// the particular *this* or *that*, so that what we return
	// to the framework will be an unambiguous set of behavior
	// flags.
	//

	if ( (DEVICECREATE_SW_OR_HW_VP & ResolvedBehaviors) != 0 )
	{
		ResolvedBehaviors &= ~DEVICECREATE_SW_OR_HW_VP;

		// Assume HWVP.  If the device can't do HWTRANSFORMANDLIGHT,
		// then this will get overridden below.
		if ( (DevCaps.DeviceType == D3DDEVTYPE_HAL) != 0 )
		{
			ResolvedBehaviors |= DEVICECREATE_HWVP;
		}
		else
		{
			ResolvedBehaviors |= DEVICECREATE_SWVP;
		};


		// Determine whether the device can do hardware vertex processing.
		if
		(
			(DevCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0
		)
		{
			ResolvedBehaviors |= DEVICECREATE_SWVP;
			ResolvedBehaviors &= ~DEVICECREATE_HWVP;
			ResolvedBehaviors &= ~DEVICECREATE_MIXED;
		};
	};


	if ( (DEVICECREATE_HWVP_OR_PURE & ResolvedBehaviors) != 0 )
	{
		ResolvedBehaviors &= ~DEVICECREATE_HWVP_OR_PURE;

		if((DevCaps.DevCaps & D3DDEVCAPS_PUREDEVICE) != 0 )
		{
			ResolvedBehaviors &= ~DEVICECREATE_STANDARD;
			ResolvedBehaviors &= ~DEVICECREATE_MIXED;
			ResolvedBehaviors |= DEVICECREATE_PURE | DEVICECREATE_HWVP;
		}
		else
		{
			ResolvedBehaviors &= ~DEVICECREATE_PURE;
			ResolvedBehaviors |= DEVICECREATE_STANDARD | DEVICECREATE_HWVP;
		};
	};

	
	if ( (DEVICECREATE_BESTAVAILABLE & ResolvedBehaviors) != 0 )
	{
		ResolvedBehaviors &= ~DEVICECREATE_BESTAVAILABLE;

		// Assume HWVP.  If the device can't do HWTRANSFORMANDLIGHT,
		// then this will get overridden below.
		if((DevCaps.DevCaps & D3DDEVCAPS_PUREDEVICE) != 0)
		{
			ResolvedBehaviors &= ~DEVICECREATE_STANDARD;
			ResolvedBehaviors &= ~DEVICECREATE_MIXED;
			ResolvedBehaviors |= DEVICECREATE_PURE | DEVICECREATE_HWVP;
		}
		else
		{
			ResolvedBehaviors &= ~DEVICECREATE_PURE;
			ResolvedBehaviors |= DEVICECREATE_STANDARD | DEVICECREATE_HWVP;
		};


		// Determine whether the device can do hardware vertex processing.
		if
		(
			(DevCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0
		)
		{
			ResolvedBehaviors |= DEVICECREATE_SWVP;
			ResolvedBehaviors &= ~DEVICECREATE_HWVP;
			ResolvedBehaviors &= ~DEVICECREATE_MIXED;
		};
	};


	// Make sure that there's some sort of device subtype.
	if ( (ResolvedBehaviors & 0xF00) == 0 )
	{
		ResolvedBehaviors |= DEVICECREATE_STANDARD;
	};


	//
	// Fill in our outgoing parameter.
	//

	*pfResolvedBehaviors = ResolvedBehaviors;

	return true;
};


// --------------------------------------------------------------------------------------

bool CD3D::DetectCaps8(CDevice* pDevice)
{
	HRESULT		hr;
	CAPS	    DevCaps;
	DWORD 		dwCount;
	FORMAT		**pFmt;
	bool		bRawCaps;
		
	if(m_pD3D8 == NULL || pDevice == NULL)
	{
		DPF(1, _T("CD3D::DetectCaps8() - invalid pointer.\n"));
		return false;
	}

	// Get the adapter for this device
	int nAdapter = pDevice->GetAdapterID();
	ADAPTER *pAdapter = pDevice->GetAdapter();
	if(pAdapter == NULL)
	{
		DPF(1, _T("CD3D::DetectCaps8() - invalid adapter ID(#%d).\n"), nAdapter);
		return false;
	}

	// Get the device caps.  If there's an actual device available, use it otherwise use the raw adapter caps
	if(pDevice->m_pDevice8)
	{
		bRawCaps = false;

		hr = pDevice->GetDeviceCaps(&DevCaps);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3D::DetectCaps8() - GetDeviceCaps() failed. hr=%s(%x)\n"), HResultToString(hr), hr);
			return false;
		}
	}
	else
	{
		bRawCaps = true;

		hr = GetDeviceCaps(nAdapter, pDevice->GetDevType(), &DevCaps);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3D::DetectCaps8() - GetDeviceCaps() failed. hr=%s(%x)\n"), HResultToString(hr), hr);
			return false;
		}

		// Check DDI level - we need raw caps for this
		if (DevCaps.MaxStreams > 0)
			pDevice->m_nDDILevel = 8;
		else if(DevCaps.DevCaps & D3DDEVCAPS_DRAWPRIMITIVES2EX)
			pDevice->m_nDDILevel = 7;
		else if(DevCaps.DevCaps & D3DDEVCAPS_DRAWPRIMITIVES2)
			pDevice->m_nDDILevel = 6;
		else if(DevCaps.DevCaps & D3DDEVCAPS_DRAWPRIMTLVERTEX)
			pDevice->m_nDDILevel = 5;
	}

	// Check the device caps
	if(DevCaps.DeviceType == D3DDEVTYPE_HAL && (DevCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT))
	   pAdapter->bHardwareVP = true;
	if(DevCaps.DeviceType == D3DDEVTYPE_HAL && (DevCaps.DevCaps & D3DDEVCAPS_PUREDEVICE))
		pAdapter->bPure = true;
	if(DevCaps.DevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM)
		pAdapter->eBusType = AGPBUS;
	else
		pAdapter->eBusType = PCIBUS;


	// Save the cap information
	pDevice->m_Desc.dwCaps = DevCaps.Caps;
	pDevice->m_Desc.dwCaps2 = DevCaps.Caps2;
	pDevice->m_Desc.dwCaps3 = DevCaps.Caps3;
	pDevice->m_Desc.dwPresentationIntervals = DevCaps.PresentationIntervals;
	pDevice->m_Desc.dwCursorCaps = DevCaps.CursorCaps;
	pDevice->m_Desc.dwDevCaps = DevCaps.DevCaps;
	pDevice->m_Desc.dpcTriCaps.dwMiscCaps = DevCaps.PrimitiveMiscCaps;
	pDevice->m_Desc.dpcTriCaps.dwRasterCaps = DevCaps.RasterCaps;
	pDevice->m_Desc.dpcTriCaps.dwZCmpCaps = DevCaps.ZCmpCaps;
	pDevice->m_Desc.dpcTriCaps.dwSrcBlendCaps = DevCaps.SrcBlendCaps;
	pDevice->m_Desc.dpcTriCaps.dwDestBlendCaps = DevCaps.DestBlendCaps;
	pDevice->m_Desc.dpcTriCaps.dwAlphaCmpCaps = DevCaps.AlphaCmpCaps;
	pDevice->m_Desc.dpcTriCaps.dwShadeCaps = DevCaps.ShadeCaps;
	pDevice->m_Desc.dpcTriCaps.dwTextureCaps = DevCaps.TextureCaps;
	pDevice->m_Desc.dpcTriCaps.dwTextureFilterCaps = DevCaps.TextureFilterCaps;
	pDevice->m_Desc.dpcTriCaps.dwTextureBlendCaps = 0;
	pDevice->m_Desc.dpcTriCaps.dwTextureAddressCaps = DevCaps.TextureAddressCaps;
	pDevice->m_Desc.dpcTriCaps.dwStippleWidth = 0;
	pDevice->m_Desc.dpcTriCaps.dwStippleHeight = 0;

	pDevice->m_Desc.dpcLineCaps.dwMiscCaps = DevCaps.PrimitiveMiscCaps;
	pDevice->m_Desc.dpcLineCaps.dwRasterCaps = DevCaps.RasterCaps;
	if (0 == (DevCaps.LineCaps & D3DLINECAPS_FOG))
		pDevice->m_Desc.dpcLineCaps.dwRasterCaps |= ~(D3DPRASTERCAPS_FOGVERTEX | D3DPRASTERCAPS_FOGTABLE |
													D3DPRASTERCAPS_WFOG | D3DPRASTERCAPS_ZFOG | 
													D3DPRASTERCAPS_FOGRANGE);
	if (DevCaps.LineCaps & D3DLINECAPS_ZTEST)
		pDevice->m_Desc.dpcLineCaps.dwZCmpCaps = DevCaps.ZCmpCaps;
	else
		pDevice->m_Desc.dpcLineCaps.dwZCmpCaps = 0;
	if (DevCaps.LineCaps & D3DLINECAPS_BLEND)
	{
		pDevice->m_Desc.dpcLineCaps.dwSrcBlendCaps = DevCaps.SrcBlendCaps;
		pDevice->m_Desc.dpcLineCaps.dwDestBlendCaps = DevCaps.DestBlendCaps;
	}
	else
	{
		pDevice->m_Desc.dpcLineCaps.dwSrcBlendCaps = 0;
		pDevice->m_Desc.dpcLineCaps.dwDestBlendCaps = 0;
	}
	if (DevCaps.LineCaps & D3DLINECAPS_ALPHACMP)
		pDevice->m_Desc.dpcLineCaps.dwAlphaCmpCaps = DevCaps.AlphaCmpCaps;
	else
		pDevice->m_Desc.dpcLineCaps.dwAlphaCmpCaps = 0;
	pDevice->m_Desc.dpcLineCaps.dwShadeCaps = DevCaps.ShadeCaps;
	pDevice->m_Desc.dpcLineCaps.dwTextureBlendCaps = 0;
	if (DevCaps.LineCaps & D3DLINECAPS_TEXTURE)
	{
		pDevice->m_Desc.dpcLineCaps.dwTextureCaps = DevCaps.TextureCaps;
		pDevice->m_Desc.dpcLineCaps.dwTextureFilterCaps = DevCaps.TextureFilterCaps;
		pDevice->m_Desc.dpcLineCaps.dwTextureAddressCaps = DevCaps.TextureAddressCaps;
	}
	else
	{
		pDevice->m_Desc.dpcLineCaps.dwTextureCaps = 0;
		pDevice->m_Desc.dpcLineCaps.dwTextureFilterCaps = 0;
		pDevice->m_Desc.dpcLineCaps.dwTextureAddressCaps = 0;
	}
	pDevice->m_Desc.dpcLineCaps.dwStippleWidth = 0;
	pDevice->m_Desc.dpcLineCaps.dwStippleHeight = 0;

	pDevice->m_Desc.dwPrimitiveMiscCaps = DevCaps.PrimitiveMiscCaps;
	pDevice->m_Desc.dwRasterCaps = DevCaps.RasterCaps;
	pDevice->m_Desc.dwZCmpCaps = DevCaps.ZCmpCaps;
	pDevice->m_Desc.dwSrcBlendCaps = DevCaps.SrcBlendCaps;
	pDevice->m_Desc.dwDestBlendCaps = DevCaps.DestBlendCaps;
	pDevice->m_Desc.dwAlphaCmpCaps = DevCaps.AlphaCmpCaps;
	pDevice->m_Desc.dwShadeCaps = DevCaps.ShadeCaps;
	pDevice->m_Desc.dwTextureCaps = DevCaps.TextureCaps;
	pDevice->m_Desc.dwTextureFilterCaps = DevCaps.TextureFilterCaps;
	pDevice->m_Desc.dwCubeTextureFilterCaps = DevCaps.CubeTextureFilterCaps;
	pDevice->m_Desc.dwVolumeTextureFilterCaps = DevCaps.VolumeTextureFilterCaps;
	pDevice->m_Desc.dwTextureAddressCaps = DevCaps.TextureAddressCaps;
	pDevice->m_Desc.dwVolumeTextureAddressCaps = DevCaps.VolumeTextureAddressCaps;
	pDevice->m_Desc.dwLineCaps = DevCaps.LineCaps;
	
	pDevice->m_Desc.dwMinTextureWidth = 1;
	pDevice->m_Desc.dwMinTextureHeight = 1;
	pDevice->m_Desc.dwMaxTextureWidth = DevCaps.MaxTextureWidth;
	pDevice->m_Desc.dwMaxTextureHeight = DevCaps.MaxTextureHeight;
	pDevice->m_Desc.dwMinVolumeExtent = 1;
	pDevice->m_Desc.dwMaxVolumeExtent = DevCaps.MaxVolumeExtent;
	pDevice->m_Desc.dwMaxTextureRepeat = DevCaps.MaxTextureRepeat;
	pDevice->m_Desc.dwMaxTextureAspectRatio = DevCaps.MaxTextureAspectRatio;
	pDevice->m_Desc.dwMaxAnisotropy = DevCaps.MaxAnisotropy;
	pDevice->m_Desc.dvMaxVertexW = DevCaps.MaxVertexW;
	pDevice->m_Desc.dvGuardBandLeft = DevCaps.GuardBandLeft;
	pDevice->m_Desc.dvGuardBandTop = DevCaps.GuardBandTop;
	pDevice->m_Desc.dvGuardBandRight = DevCaps.GuardBandRight;
	pDevice->m_Desc.dvGuardBandBottom = DevCaps.GuardBandBottom;
	pDevice->m_Desc.dvExtentsAdjust = DevCaps.ExtentsAdjust;
	pDevice->m_Desc.dwStencilCaps = DevCaps.StencilCaps;
	pDevice->m_Desc.dwFVFCaps = DevCaps.FVFCaps;
	pDevice->m_Desc.dwTextureOpCaps = DevCaps.TextureOpCaps;
	pDevice->m_Desc.wMaxTextureBlendStages = (WORD)DevCaps.MaxTextureBlendStages;
	pDevice->m_Desc.wMaxSimultaneousTextures = (WORD)DevCaps.MaxSimultaneousTextures;
	pDevice->m_Desc.dwVertexProcessingCaps = DevCaps.VertexProcessingCaps;
	pDevice->m_Desc.dwMaxActiveLights = DevCaps.MaxActiveLights;
	pDevice->m_Desc.wMaxUserClipPlanes = (WORD)DevCaps.MaxUserClipPlanes;
	pDevice->m_Desc.wMaxVertexBlendMatrices = (WORD)DevCaps.MaxVertexBlendMatrices;
	pDevice->m_Desc.dwMaxVertexBlendMatrixIndex = DevCaps.MaxVertexBlendMatrixIndex;
	pDevice->m_Desc.dvMaxPointSize = DevCaps.MaxPointSize;
	pDevice->m_Desc.dwMaxPrimitiveCount = DevCaps.MaxPrimitiveCount;
	pDevice->m_Desc.dwMaxVertexIndex = DevCaps.MaxVertexIndex;
	pDevice->m_Desc.dwMaxStreams = DevCaps.MaxStreams;
	pDevice->m_Desc.dwMaxStreamStride = DevCaps.MaxStreamStride;
	pDevice->m_Desc.dwVertexShaderVersion = DevCaps.VertexShaderVersion;
	pDevice->m_Desc.dwMaxVertexShaderConst = DevCaps.MaxVertexShaderConst;
	pDevice->m_Desc.dwPixelShaderVersion = DevCaps.PixelShaderVersion;
	pDevice->m_Desc.dvMaxPixelShaderValue = DevCaps.MaxPixelShaderValue;
	pDevice->m_Desc.dwDeclTypes = DevCaps.DeclTypes;
	pDevice->m_Desc.VS20Caps.Caps = 0;
	pDevice->m_Desc.VS20Caps.DynamicFlowControlDepth = 0;
	pDevice->m_Desc.VS20Caps.NumTemps = 0;
	pDevice->m_Desc.VS20Caps.StaticFlowControlDepth = 0;
	pDevice->m_Desc.PS20Caps.Caps = 0;
	pDevice->m_Desc.PS20Caps.DynamicFlowControlDepth = 0;
	pDevice->m_Desc.PS20Caps.NumTemps = 0;
	pDevice->m_Desc.PS20Caps.StaticFlowControlDepth = 0;
	pDevice->m_Desc.PS20Caps.NumInstructionSlots = 0;
	pDevice->m_Desc.dwVertexTextureFilterCaps = 0;
	pDevice->m_Desc.dwMaxVShaderInstructionsExecuted = 0;
	pDevice->m_Desc.dwMaxPShaderInstructionsExecuted = 0;

	// Decode the flag if ANY display mode is requested
	if(pAdapter->DesktopMode.Format.d3dfFormat == FMT_ANY)
	{
		FMT PreferredFormat = FMT_X8R8G8B8;
		D3DFORMAT PreferredD3DFormat = D3DFMT_X8R8G8B8;

		FMT AlternateFormat = FMT_R5G6B5;
		D3DFORMAT AlternateD3DFormat = D3DFMT_R5G6B5;

		DEVMODE CurrentMode;
		ZeroMemory( &CurrentMode, sizeof( CurrentMode) );
		CurrentMode.dmSize = sizeof( CurrentMode );
		CurrentMode.dmSpecVersion = DM_SPECVERSION;
		BOOL bGetCurrentModeResult = EnumDisplaySettingsEx
		(
			NULL,
			ENUM_CURRENT_SETTINGS,
			&CurrentMode,
			0
		);

		if
		(
			bGetCurrentModeResult == TRUE &&
			CurrentMode.dmBitsPerPel < 32
		)
		{
			PreferredFormat = FMT_R5G6B5;
			PreferredD3DFormat = D3DFMT_R5G6B5;

			AlternateFormat = FMT_X8R8G8B8;
			AlternateD3DFormat = D3DFMT_X8R8G8B8;
		};

		if
		(
			SUCCEEDED
			(
				CheckDeviceType
				(
					nAdapter,
					pDevice->GetDevType(),
					(FORMAT) PreferredFormat,
					(FORMAT) PreferredFormat,
					TRUE
				)
			) &&
			IsDisplayModeSupported
			(
				pAdapter,
				pAdapter->DesktopMode.dwWidth,
				pAdapter->DesktopMode.dwHeight,
				PreferredFormat
			)
		)
		{
			pAdapter->DesktopMode.Format.d3dfFormat = PreferredD3DFormat;
		}
		else if
		(
			SUCCEEDED
			(
				CheckDeviceType
				(
					nAdapter,
					pDevice->GetDevType(),
					(FORMAT) AlternateFormat,
					(FORMAT) AlternateFormat,
					TRUE
				)
			) &&
			IsDisplayModeSupported
			(
				pAdapter,
				pAdapter->DesktopMode.dwWidth,
				pAdapter->DesktopMode.dwHeight,
				AlternateFormat
			)
		)
		{
			pAdapter->DesktopMode.Format.d3dfFormat = AlternateD3DFormat;
		}
		else
		{
			DPF(1, _T("D3D Device is not hardware accelerated in any supported display modes.\n"));
			goto Error;
		};
	};

	d3df2ddpf( pAdapter->DesktopMode.Format.d3dfFormat, &pAdapter->DesktopMode.Format.ddpfPixelFormat);
	pAdapter->DesktopMode.dwBPP = pAdapter->DesktopMode.Format.ddpfPixelFormat.dwRGBBitCount;

	// Enumerate Z buffer / stencil formats
	if(bRawCaps)
	{
		SAFEDELETEA(pDevice->m_pZBufferFormat);
	
		pFmt = &(pDevice->m_pZBufferFormat);
		for (dwCount = 0; dwCount < g_dwZBufferFormatListSize8; dwCount++)
		{
			FORMAT fmt(g_ZBufferFormatList9[dwCount]);
			hr = CheckDeviceFormat(nAdapter, pDevice->GetDevType(), pAdapter->DesktopMode.Format, USAGE_DEPTHSTENCIL, RTYPE_SURFACE, fmt);
			if (FAILED(hr))
				continue;
			
			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D::DetectCaps8() - Out of memory.\n"));
				goto Error;
			}
			memcpy(*pFmt, &fmt, sizeof(FORMAT));
			(*pFmt)->pNext = NULL;

			pDevice->m_nZBufferFormat++;
			pFmt = &((*pFmt)->pNext);
		}
	}

	// Enumerate texture formats
	if(bRawCaps)
	{
		SAFEDELETEA(pDevice->m_pTextureFormat);

		pFmt = &(pDevice->m_pTextureFormat);
		for (dwCount = 0; dwCount < g_dwTextureFormatListSize8; dwCount++)
		{
			FORMAT fmt(g_TextureFormatList8[dwCount]);
			hr = CheckDeviceFormat(nAdapter, pDevice->GetDevType(), pAdapter->DesktopMode.Format, 0, RTYPE_TEXTURE, fmt);
			if (FAILED(hr))
				continue;

			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D::DetectCaps8() - Out of memory.\n"));
				goto Error;
			}
			memcpy(*pFmt, &fmt, sizeof(FORMAT));
			(*pFmt)->pNext = NULL;

			pDevice->m_nTextureFormat++;
			pFmt = &((*pFmt)->pNext);
		}
	}

	// Enumerate cube texture formats
	if(bRawCaps)
	{
		SAFEDELETEA(pDevice->m_pCubeFormat);

		pFmt = &(pDevice->m_pCubeFormat);
		for (dwCount = 0; dwCount < g_dwTextureFormatListSize8; dwCount++)
		{
			FORMAT fmt(g_TextureFormatList8[dwCount]);
			hr = CheckDeviceFormat(nAdapter, pDevice->GetDevType(), pAdapter->DesktopMode.Format, 0, RTYPE_CUBETEXTURE, (FORMAT)g_TextureFormatList8[dwCount]);
			if (FAILED(hr))
				continue;

			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D::DetectCaps8() - Out of memory.\n"));
				goto Error;
			}
			memcpy(*pFmt, &fmt, sizeof(FORMAT));
			(*pFmt)->pNext = NULL;

			pDevice->m_nCubeFormat++;
			pFmt = &((*pFmt)->pNext);
		}
	}

	// Enumerate volume texture formats
	if(bRawCaps)
	{
		SAFEDELETEA(pDevice->m_pMultisampType);

		pFmt = &(pDevice->m_pVolumeFormat);
		for (dwCount = 0; dwCount < g_dwTextureFormatListSize8; dwCount++)
		{
			FORMAT fmt(g_TextureFormatList8[dwCount]);
			hr = CheckDeviceFormat(nAdapter, pDevice->GetDevType(), pAdapter->DesktopMode.Format, 0, RTYPE_VOLUMETEXTURE, fmt);
			if (FAILED(hr))
				continue;

			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D::DetectCaps8() - Out of memory.\n"));
				goto Error;
			}
			memcpy(*pFmt, &fmt, sizeof(FORMAT));
			(*pFmt)->pNext = NULL;

			pDevice->m_nVolumeFormat++;
			pFmt = &((*pFmt)->pNext);
		}
	}
	// Enumerate BackBuffer types - these require mode information(ModeFormat) so we'll add 
	// them all for now, we'll filter them out later when constructing the modelist
	if(bRawCaps)
	{
		SAFEDELETEA(pDevice->m_pBackBufferFormat);

		pFmt = &(pDevice->m_pBackBufferFormat);
		for (dwCount = 0; dwCount < g_dwBackBufferFormats8; dwCount++)
		{
			FORMAT fmt(g_BackBufferFormats8[dwCount]);

			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D::DetectCaps9() - Out of memory.\n"));
				goto Error;
			}
			memcpy(*pFmt, &fmt, sizeof(FORMAT));
			(*pFmt)->pNext = NULL;

			pDevice->m_nBackBufferFormat++;
			pFmt = &((*pFmt)->pNext);
		}
	}

	// Enumerate multisample types - these require mode information(BackBufferFormat) so we'll add 
	// them all for now, we'll filter them out later when constructing the modelist
	if(bRawCaps)
	{
		SAFEDELETEA(pDevice->m_pMultisampType);

		pDevice->m_pMultisampType = new MULTISAMPLE_TYPE[g_dwMultiSampleTypeListSize8];
		if(NULL == pDevice->m_pMultisampType)
		{
			DPF(1, _T("CD3D::DetectCaps8() - Out of memory.\n"));
			goto Error;
		}
		pDevice->m_nMultisampType = g_dwMultiSampleTypeListSize8;
		memcpy(pDevice->m_pMultisampType, &g_MultiSampleTypeList8, sizeof(MULTISAMPLE_TYPE) * g_dwMultiSampleTypeListSize8);
	}

	// Create the device description
	pDevice->UpdateDescription();

	return true;

Error:
	// Device info will get released when the device is destroyed

	return false;
}

// --------------------------------------------------------------------------------------

void CD3D::ProcessError8(HRESULT hr)
{
	// Only set this flag - it'll get reset when the device recovers.
	if(hr == E_OUTOFMEMORY)
		m_bOutofMemory = true;

	if(hr == D3DERR_DEVICELOST)
		m_bDeviceLost = true;
}

// --------------------------------------------------------------------------------------

bool CD3D::CompareFormats8(FORMAT *pSrcFmt, FORMAT *pRefFmt)
{
	if(!pSrcFmt || !pRefFmt)
	{
		DPF(1, _T("CD3D::CompareFormats8() - Invalid parameters.\n"));
		return false;
	}

	return pSrcFmt->d3dfFormat == pRefFmt->d3dfFormat;
}

// --------------------------------------------------------------------------------------

VECTOR& VECTOR::operator=(const D3DVECTOR &v)
{
	x = v.x; y = v.y; z = v.z;
	return *this;
}

// --------------------------------------------------------------------------------------
// D3D8 object wrappers
// --------------------------------------------------------------------------------------

HRESULT CD3D::RegisterSoftwareDevice8(void* pInitializeFunction)
{
	return m_pD3D8->RegisterSoftwareDevice( pInitializeFunction );
}

// --------------------------------------------------------------------------------------

UINT CD3D::GetAdapterCount8(void)
{
	return m_pD3D8->GetAdapterCount();
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::GetAdapterIdentifier8(UINT Adapter,DWORD Flags,ADAPTER_IDENTIFIER* pIdentifier)
{
	return m_pD3D8->GetAdapterIdentifier( Adapter,Flags,(D3DADAPTER_IDENTIFIER8*)pIdentifier );
}

// --------------------------------------------------------------------------------------

UINT CD3D::GetAdapterModeCount8(UINT Adapter)
{
	return m_pD3D8->GetAdapterModeCount(Adapter);
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::EnumAdapterModes8(UINT Adapter,UINT nMode,DISPLAYMODE* pMode)
{
	D3DDISPLAYMODE Mode;

	HRESULT hr = m_pD3D8->EnumAdapterModes( Adapter,nMode,pMode==0L?0L:&Mode );
	
	if( SUCCEEDED(hr) && pMode != NULL )
	{
		pMode->dwWidth  = Mode.Width;
		pMode->dwHeight = Mode.Height;
		pMode->dwRefreshRate = Mode.RefreshRate;
		pMode->Format.d3dfFormat = Mode.Format;
		d3df2ddpf(Mode.Format,&pMode->Format.ddpfPixelFormat);
		pMode->dwBPP = pMode->Format.ddpfPixelFormat.dwRGBBitCount;
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::GetAdapterDisplayMode8(UINT Adapter,DISPLAYMODE* pMode)
{
	if( m_pD3D8 == NULL )
	{
		DPF(1, _T("CD3D::GetAdapterDisplayMode8() - Invalid D3D pointer!\n"));
		return E_FAIL;
	}

	D3DDISPLAYMODE Mode;

	HRESULT hr = m_pD3D8->GetAdapterDisplayMode( Adapter,pMode==0L?0L:&Mode );

	if( SUCCEEDED(hr) &&  pMode != NULL )
	{
		pMode->dwWidth  = Mode.Width;
		pMode->dwHeight = Mode.Height;
		pMode->dwRefreshRate = Mode.RefreshRate;
		pMode->Format.d3dfFormat = Mode.Format;
		d3df2ddpf(Mode.Format,&pMode->Format.ddpfPixelFormat);
		pMode->dwBPP = pMode->Format.ddpfPixelFormat.dwRGBBitCount;
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDeviceType8(UINT Adapter,DWORD CheckType,FORMAT DisplayFormat,
							   FORMAT BackBufferFormat,BOOL Windowed)
{
	D3DDEVTYPE DevType = (D3DDEVTYPE)CheckType;
	if(CheckType == DEVICETYPE_HAL)
		DevType = D3DDEVTYPE_HAL;
	else if(CheckType == DEVICETYPE_REF)
		DevType = D3DDEVTYPE_REF;
	else if(CheckType == DEVICETYPE_SW)
		DevType = D3DDEVTYPE_SW;

	return m_pD3D8->CheckDeviceType( Adapter,DevType,DisplayFormat.d3dfFormat,
										   BackBufferFormat.d3dfFormat,Windowed );
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDeviceFormat8(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,
								 DWORD Usage,RESOURCETYPE RType,FORMAT CheckFormat)
{
	D3DDEVTYPE DevType = (D3DDEVTYPE)DeviceType;
	if(DeviceType == DEVICETYPE_HAL)
		DevType = D3DDEVTYPE_HAL;
	else if(DeviceType == DEVICETYPE_REF)
		DevType = D3DDEVTYPE_REF;
	else if(DeviceType == DEVICETYPE_SW)
		DevType = D3DDEVTYPE_SW;

	return (m_pD3D8->CheckDeviceFormat( Adapter,DevType,AdapterFormat.d3dfFormat,
										Usage,(D3DRESOURCETYPE)RType,CheckFormat.d3dfFormat ));

}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDeviceMultiSampleType8(UINT Adapter,DWORD DeviceType,FORMAT SurfaceFormat,
										  BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType)
{
	D3DDEVTYPE DevType = (D3DDEVTYPE)DeviceType;
	if(DeviceType == DEVICETYPE_HAL)
		DevType = D3DDEVTYPE_HAL;
	else if(DeviceType == DEVICETYPE_REF)
		DevType = D3DDEVTYPE_REF;
	else if(DeviceType == DEVICETYPE_SW)
		DevType = D3DDEVTYPE_SW;

	return (m_pD3D8->CheckDeviceMultiSampleType( Adapter,DevType,SurfaceFormat.d3dfFormat,
												 Windowed,(D3DMULTISAMPLE_TYPE)MultiSampleType ));
}

// --------------------------------------------------------------------------------------


HRESULT CD3D::CheckDeviceMultiSampleType9To8(UINT Adapter,DWORD DeviceType,FORMAT SurfaceFormat,
										  BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType, DWORD *dwQuality)
{
	D3DDEVTYPE DevType = (D3DDEVTYPE)DeviceType;
	if(DeviceType == DEVICETYPE_HAL)
		DevType = D3DDEVTYPE_HAL;
	else if(DeviceType == DEVICETYPE_REF)
		DevType = D3DDEVTYPE_REF;
	else if(DeviceType == DEVICETYPE_SW)
		DevType = D3DDEVTYPE_SW;
	
	if(dwQuality)
		*dwQuality = 1;

	return (m_pD3D8->CheckDeviceMultiSampleType( Adapter,DevType,SurfaceFormat.d3dfFormat,
												 Windowed,(D3DMULTISAMPLE_TYPE)MultiSampleType ));
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDepthStencilMatch8(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,
									  FORMAT RenderTargetFormat,FORMAT DepthStencilFormat)
{
   D3DDEVTYPE DevType = (D3DDEVTYPE)DeviceType;
	if(DeviceType == DEVICETYPE_HAL)
		DevType = D3DDEVTYPE_HAL;
	else if(DeviceType == DEVICETYPE_REF)
		DevType = D3DDEVTYPE_REF;
	else if(DeviceType == DEVICETYPE_SW)
		DevType = D3DDEVTYPE_SW;

	return (m_pD3D8->CheckDepthStencilMatch( Adapter,DevType,AdapterFormat.d3dfFormat,
											 RenderTargetFormat.d3dfFormat,
											 DepthStencilFormat.d3dfFormat ));
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::GetDeviceCaps8(UINT Adapter,DWORD DeviceType,CAPS* pCaps)
{
	D3DDEVTYPE DevType = (D3DDEVTYPE)DeviceType;
	if(DeviceType == DEVICETYPE_HAL)
		DevType = D3DDEVTYPE_HAL;
	else if(DeviceType == DEVICETYPE_REF)
		DevType = D3DDEVTYPE_REF;
	else if(DeviceType == DEVICETYPE_SW)
		DevType = D3DDEVTYPE_SW;

	if(pCaps)
		memset(pCaps, 0, sizeof(CAPS));

	return m_pD3D8->GetDeviceCaps( Adapter,DevType,(D3DCAPS8*)pCaps );
}

// --------------------------------------------------------------------------------------

HMONITOR CD3D::GetAdapterMonitor8(UINT Adapter)
{
	return (m_pD3D8->GetAdapterMonitor( Adapter ));
}