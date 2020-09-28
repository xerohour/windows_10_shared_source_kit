// ======================================================================================
//
//  CD3D9.cpp
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================

#include "D3DPCH.h"
#include <d3d8rgbrast.h>
#include "Framework.h"
#include "D3DX9.h"
#include "rtl_simple.h"
#include <strsafe.h>

typedef IDirect3D9* (WINAPI *D3DCREATE)(UINT);
typedef HRESULT (WINAPI *D3DCREATEEX)(UINT, IDirect3D9Ex** );
typedef void (WINAPI *FORCEHYBRIDENUMERATION)(UINT);

D3DDEVTYPE ConvertDeviceType(DWORD DeviceType);


// --------------------------------------------------------------------------------------
// Define all known D3D formats

static FMT g_TextureFormatList9[] = 
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
	FMT_A8B8G8R8,
	FMT_X8B8G8R8,
	FMT_G16R16,
	FMT_A2R10G10B10,
	FMT_A2B10G10R10_XR_BIAS,
	FMT_A16B16G16R16,
	FMT_R16F,
	FMT_G16R16F,
	FMT_A16B16G16R16F,
	FMT_R32F,
	FMT_G32R32F,
	FMT_A32B32G32R32F,
	FMT_A1,

	FMT_A8P8,
	FMT_P8,

	FMT_L8,
	FMT_A8L8,
	FMT_A4L4,
	FMT_L16,

	FMT_V8U8,
	FMT_L6V5U5,
	FMT_X8L8V8U8,
	FMT_Q8W8V8U8,
	FMT_V16U16,
	FMT_A2W10V10U10,
	FMT_Q16W16V16U16,
	FMT_CxV8U8,
	
	FMT_UYVY,
	FMT_R8G8_B8G8,
	FMT_YUY2,
	FMT_G8R8_G8B8,
	FMT_DXT1,
	FMT_DXT2,
	FMT_DXT3,
	FMT_DXT4,
	FMT_DXT5,

	FMT_MULTI2_ARGB8
};

static DWORD g_dwTextureFormatListSize9 = sizeof(g_TextureFormatList9) / sizeof(FMT);

static FMT g_ZBufferFormatList9[] = 
{
	FMT_D16,

	FMT_D32,
	FMT_D15S1,
	FMT_D24S8,
	FMT_D24X8,
	FMT_D24FS8,
	FMT_D24X4S4,

	FMT_D16_LOCKABLE,
	FMT_D32F_LOCKABLE
};
static DWORD g_dwZBufferFormatListSize9 = sizeof(g_ZBufferFormatList9) / sizeof(FMT);

MULTISAMPLE_TYPE g_MultiSampleTypeList9[] = 
{
	MULTISAMPLE_NONE,
	MULTISAMPLE_NONMASKABLE,
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
DWORD g_dwMultiSampleTypeListSize9 = sizeof(g_MultiSampleTypeList9) / sizeof(MULTISAMPLE_TYPE);

FMT g_DisplayModeFormats9[] = {
	FMT_X1R5G5B5,
	FMT_R5G6B5  ,
	FMT_R8G8B8  ,
	FMT_X8R8G8B8,
	FMT_A2R10G10B10,
	FMT_A2B10G10R10_XR_BIAS
};
static DWORD g_dwDisplayModeFormats9 = sizeof(g_DisplayModeFormats9) / sizeof(FMT);

FMT g_BackBufferFormats9[] = {
	FMT_A2R10G10B10,
	FMT_A2B10G10R10_XR_BIAS,
	FMT_A8R8G8B8,
	FMT_X8R8G8B8,
	FMT_A1R5G5B5,
	FMT_X1R5G5B5,
	FMT_R5G6B5
};
static DWORD g_dwBackBufferFormats9 = sizeof(g_BackBufferFormats9) / sizeof(FMT);

// --------------------------------------------------------------------------------------

HRESULT CD3D::InitD3D9()
{
	HRESULT                     hr = S_OK;
	D3DCREATE                   pProc;
	D3DCREATEEX					pProcEx;
	FORCEHYBRIDENUMERATION      pfnForceHybridEnumeration = nullptr;
	D3DADAPTER_IDENTIFIER9      Adapter;
	D3DDISPLAYMODE              Mode;
	DWORD                       dwNumModes = 0;
	DWORD                       dwAdapters = 0;
	DWORD						dwAdapterModes = 0;

	// Load the SRC D3D9 library
	m_hSrcD3DDLL = LoadLibraryEx( m_sSrcDLL, NULL, 0 );
	if(m_hSrcD3DDLL == NULL)
	{
		DPF(1, _T("CD3D::InitD3D9() - LoadLibrary failed for %s.\n"), m_sSrcDLL);
		return E_FAIL;
	}
	
	// Get the D3D creation entry point
	pProc = (D3DCREATE)GetProcAddress( m_hSrcD3DDLL, "Direct3DCreate9" );
	if (NULL == pProc)
	{
		DPF(1, _T("CD3D::InitD3D9() - Couldn't get Direct3DCreate9 address for %s.\n"), m_sSrcDLL);
		hr = E_FAIL;
		goto Exit;
	}

    // Get the Windows version
//    OSVERSIONINFO windowsVersionInfo;
//    windowsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
//    GetVersionEx(&windowsVersionInfo);
    RTL_OSVERSIONINFOW windowsVersionInfo;
    windowsVersionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
    RtlGetVersion(&windowsVersionInfo);

	// Get the D3D9 Force Hybrid GPU Choice entry point
    // Older OS versions don't have this export, so ship this for older OS Versions.
    // Note: Exported by Ordinal, not by name
    if( windowsVersionInfo.dwMajorVersion > 6 || (windowsVersionInfo.dwMajorVersion == 6 && windowsVersionInfo.dwMinorVersion >= 3 && windowsVersionInfo.dwBuildNumber >= 9321) )
    {
	    if( g_pFramework  )
	    {
		    // Direct3D9ForceHybridEnumeration is exported at ordinal 16
		    const WORD forceHybridGPUOrdinal = LOWORD(16);
		    pfnForceHybridEnumeration = (FORCEHYBRIDENUMERATION)GetProcAddress( m_hSrcD3DDLL, reinterpret_cast<LPCSTR>(forceHybridGPUOrdinal) );
		    if( g_pFramework->GetHybridOverride() )
		    {
			    if (NULL == pfnForceHybridEnumeration)
			    {
				    DPF(1, _T("CD3D::InitD3D9() - Couldn't get Direct3D9ForceHybridEnumeration address in %s.\n"), m_sSrcDLL);
				    hr = E_FAIL;
				    goto Exit;
			    }
			    else
			    {
				    DPF(1, _T("CD3D::InitD3D9() - ForceHybridEnumeration (%d).\n"), g_pFramework->GetHybridOverride() );
				    pfnForceHybridEnumeration(g_pFramework->GetHybridOverride());
			    }
		    }
	    }
	    else
	    {
		    DPF(1, _T("CD3D::InitD3D9() - Framework pointer is NULL. Unable to check force hybrid GPU parameter.\n"));
		    hr = E_FAIL;
		    goto Exit;
	    }
    }

	if( FLOAT_EQUAL( GetVersion(), 9.1f ) )
	{
		pProcEx = (D3DCREATEEX)GetProcAddress( m_hSrcD3DDLL, "Direct3DCreate9Ex" );
		if (NULL == pProcEx)
		{
			DPF(1, _T("CD3D::InitD3D9() - Couldn't get Direct3DCreate9Ex address for %s.\n"), m_sSrcDLL);
			hr = E_FAIL;
			goto Exit;
		}

		pProcEx(D3D_SDK_VERSION | (m_bD3DDebugHelper ? 0x80000000 : 0), & m_pSrcD3D9Ex );
	}
	else
	{
		m_pSrcD3D9 = pProc(D3D_SDK_VERSION | (m_bD3DDebugHelper ? 0x80000000 : 0) );
	}
	if (NULL == m_pSrcD3D9)
	{
		DPF(1, _T("CD3D::InitD3D9() - Direct3DCreate9() failed for %s.  Header/Runtime mismatch likely.\n"), m_sSrcDLL);
		hr = E_ABORT;
		goto Exit;
	}

	// If we're using identical SRC / REF, just AddRef().
	if(_tcsicmp(m_sSrcDLL, m_sRefDLL) == 0)
	{
		m_pRefD3D9 = m_pSrcD3D9;
		m_pRefD3D9->AddRef();
	}
	else
	{
		// Load the REF D3D9 library
		m_hRefD3DDLL = LoadLibraryEx( m_sRefDLL, NULL, 0 );
		if(m_hRefD3DDLL == NULL)
		{
			DPF(1, _T("CD3D::InitD3D9() - LoadLibrary failed for %s.\n"), m_sRefDLL);
			return E_FAIL;
		}
		
		// Get the D3D creation entry point
		pProc = (D3DCREATE)GetProcAddress( m_hRefD3DDLL, "Direct3DCreate9" );
		if (NULL == pProc)
		{
			DPF(1, _T("CD3D::InitD3D9() - Couldn't get Direct3DCreate9 address for %s.\n"), m_sRefDLL);
			hr = E_FAIL;
			goto Exit;
		}

		if( FLOAT_EQUAL( GetVersion(), 9.1f ) )
		{
			pProcEx = (D3DCREATEEX)GetProcAddress( m_hRefD3DDLL, "Direct3DCreate9Ex" );
			if (NULL == pProcEx)
			{
				DPF(1, _T("CD3D::InitD3D9() - Couldn't get Direct3DCreate9Ex address for %s.\n"), m_hRefD3DDLL);
				hr = E_FAIL;
				goto Exit;
			}

			pProcEx(D3D_SDK_VERSION, & m_pRefD3D9Ex);
		}
		else
		{
			m_pRefD3D9 = pProc(D3D_SDK_VERSION);
		}
		if (NULL == m_pRefD3D9)
		{
			DPF(1, _T("CD3D::InitD3D9() - Direct3DCreate9() failed for %s.  Header/Runtime mismatch likely.\n"), m_sRefDLL);
			hr = E_ABORT;
			goto Exit;
		}
	}

	// Get the debug info from the registry
	GetDebugInfoFromReg();
	
	// Set DebugSetMute() function ptr
	if( m_bDebug && GetModuleHandle( m_sDebugDLL ) )
	{
		m_hDebugD3DDLL = LoadLibraryEx( m_sDebugDLL, NULL, 0 );
		m_pDebugMute = (LPDEBUGSETMUTE) GetProcAddress( m_hDebugD3DDLL, "DebugSetMute" );
		if( m_pDebugMute == NULL )
		{
			DPF( 1, _T( "CD3D::InitD3D9() - Couldn't get DebugSetMute address for %s.\n" ), m_sDebugDLL );
			hr = E_FAIL;
			goto Exit;
		}
	}

	// Register any SW rasterizers
	if(!RegisterRasterizer9(m_sSWRast, m_sSWRastEntry))
	{
		DPF(1, _T("CD3D::InitD3D9 - Failed to register SW rasterizer.\n"));;
	}

	// Get the number of adapters
	m_nAdapters = m_pSrcD3D9->GetAdapterCount();
	if(0 == m_nAdapters)
	{
		DPF(1, _T("CD3D::InitD3D9() - GetAdapterCount() returned 0.  No display devices available.\n"));
		hr = E_FAIL;
		goto Exit;
	}

	// Allocate the ADAPTER infos
	m_pAdapters = new ADAPTER[m_nAdapters];
	if(m_pAdapters == NULL)
	{
		DPF(1, _T("CD3D::InitD3D9() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	// Zero the memory
	memset(m_pAdapters, 0, m_nAdapters * sizeof(ADAPTER));

	// Enumerate the adapters
	for (dwAdapters = 0; dwAdapters < m_nAdapters; dwAdapters++)
	{
		PADAPTER pAdapter = &m_pAdapters[dwAdapters];

		hr = m_pSrcD3D9->GetAdapterIdentifier(dwAdapters, NULL, &Adapter);
		if (FAILED(hr))
		{
			DPF(1, _T("CD3D::InitD3D9() - GetAdapterIdentifier() failed.  hr=%s(%x)"), HResultToString(hr));
			goto Exit;
		}

		// create the version string.
		{
			TCHAR tcsVer[20] = {0};
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
		COPYSTRING(pAdapter->sDevicePath, Adapter.DeviceName);
		
		// get the driver name
		COPYSTRING(pAdapter->sDriver, Adapter.Driver);
		
		// the vendor and device ID
		pAdapter->dwVendorID = Adapter.VendorId;
		pAdapter->dwChipID = Adapter.DeviceId;

		// Save adapter information to the internal structure
		memcpy(&pAdapter->Guid, &Adapter.DeviceIdentifier, sizeof(GUID));
		COPYSTRING(pAdapter->sDescription, Adapter.Description);
		pAdapter->hMonitor = m_pSrcD3D9->GetAdapterMonitor(dwAdapters);

		// create PnP ID string
		char szBuffer[48];
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T("VEN_%04X&DEV_%04X&SUBSYS_%08X&REV_%02X"),
				Adapter.VendorId, Adapter.DeviceId, Adapter.SubSysId, Adapter.Revision);
		COPYSTRING(pAdapter->sPnPID, szBuffer);
	
		if( NULL == pAdapter->sVersion || NULL == pAdapter->sAdapterName ||
			NULL == pAdapter->sDriver  || NULL == pAdapter->sDescription ||
			NULL == pAdapter->sPnPID || NULL == pAdapter->sDevicePath )
		{
			DPF(1, _T("CD3D::InitD3D9() - Out of memory.\n"));
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		// Obtain current format
		hr = m_pSrcD3D9->GetAdapterDisplayMode(dwAdapters, &Mode);
		if (FAILED(hr))
		{
			DPF(1, _T("CD3D::InitD3D9() - Failed to obtain current display mode, hr=%s(%x)\n"),
						   HResultToString(hr), hr);
			goto Exit;
		}
		
    
		// Generate the resolution string
		pAdapter->DesktopMode.dwWidth = Mode.Width;
		pAdapter->DesktopMode.dwHeight = Mode.Height;
		pAdapter->DesktopMode.Format.d3dfFormat = Mode.Format;
		d3df2ddpf(Mode.Format, &pAdapter->DesktopMode.Format.ddpfPixelFormat);
		pAdapter->DesktopMode.dwBPP = pAdapter->DesktopMode.Format.ddpfPixelFormat.dwRGBBitCount;
		pAdapter->DesktopMode.dwRefreshRate = Mode.RefreshRate;

		
		for(dwAdapterModes = 0; dwAdapterModes < g_dwDisplayModeFormats9; dwAdapterModes++)
		{
			DISPLAYMODE *pTempModes = NULL;

			// Enumerate display modes
			dwNumModes = m_pSrcD3D9->GetAdapterModeCount(dwAdapters, (D3DFORMAT)g_DisplayModeFormats9[dwAdapterModes]);
			
			// Allocate the display modes
			pTempModes = new DISPLAYMODE[dwNumModes+pAdapter->nModes];
			if(pTempModes == NULL)
			{
				DPF(1, _T("CD3D::InitD3D9() - Out of memory.\n"));
				hr = E_OUTOFMEMORY;
				goto Exit;
			}
	
			memset(pTempModes, 0, sizeof(DISPLAYMODE) * (dwNumModes+pAdapter->nModes));
			memcpy(pTempModes, pAdapter->pModes, sizeof(DISPLAYMODE) * pAdapter->nModes);
			SAFEDELETEA(pAdapter->pModes);
			pAdapter->pModes = pTempModes;
        
			for(int i=0; i < dwNumModes; i++)
			{
				DISPLAYMODE *pMode = &pAdapter->pModes[pAdapter->nModes++];
	
				hr = m_pSrcD3D9->EnumAdapterModes(dwAdapters, (D3DFORMAT)g_DisplayModeFormats9[dwAdapterModes], i, &Mode);
				if (FAILED(hr))
				{
					DPF(1, _T("CD3D::InitD3D9() - EnumAdapterModes failed. hr=%s(%x)\n"), HResultToString(hr), hr);
					goto Exit;
				}
	
				pMode->dwWidth = (DWORD)Mode.Width;
				pMode->dwHeight = (DWORD)Mode.Height;
				pMode->dwRefreshRate = (DWORD)Mode.RefreshRate;
				pMode->Format.d3dfFormat = Mode.Format;
				d3df2ddpf(pMode->Format.d3dfFormat, &pMode->Format.ddpfPixelFormat);
				pMode->dwBPP = pMode->Format.ddpfPixelFormat.dwRGBBitCount;
			}
		}
	}

	//  Update other adapter related information
	UpdateAdapterInfo();

Exit:
	if(FAILED(hr))
	{
		ReleaseAll();
	}
	return hr;
}

// --------------------------------------------------------------------------------------

bool CD3D::RegisterRasterizer9
(
	__in LPCTSTR pcsDLLName,
	__in LPCTSTR pcsEntryPointName
)
{
	DPF(4, _T("CD3D::RegisterRasterizer()\n"));


	bool bOverallResult = false;

	LPCSTR pcsAdjustedDLLName = pcsDLLName;
	LPCSTR pcsAdjustedEntryPointName = pcsEntryPointName;

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

	if (!m_bUserSelectedSWRast)
	{
		pcsAdjustedDLLName = _T("d3dref9.dll");
		pcsAdjustedEntryPointName = _T("D3D9GetSWInfo");

		// Save the rasterizer name
		COPYSTRING(m_sSWRast, pcsAdjustedDLLName);
		if (m_sSWRast == NULL)
		{
			DPF(1, _T("CD3D::RegisterRasterizer9() - Out of memory.\n"));

			goto Cleanup;
		}

		COPYSTRING(m_sSWRastEntry, pcsAdjustedEntryPointName);
		if (m_sSWRastEntry == NULL)
		{
			DPF(1, _T("CD3D::RegisterRasterizer9() - Out of memory.\n"));

			goto Cleanup;
		}
	}

	m_hSWRast = LoadLibraryEx( pcsAdjustedDLLName, NULL, 0 );
	if ( m_hSWRast == NULL )
	{
		DPF
		(
			1,
			"CD3D::RegisterRasterizer9() - Failed to load software rasterizer %s.\n",
			pcsAdjustedDLLName
		);

		goto Cleanup;
	}


	fn = (PD3D9GETSWINFO) GetProcAddress( m_hSWRast, pcsAdjustedEntryPointName );
	if ( fn == NULL )
	{
		DPF
		(
			1,
			"CD3D::RegisterRasterizer9() - Failed to locate software rasterizer entry point %s.\n",
			pcsAdjustedEntryPointName
		);

		goto Cleanup;
	}


	HRESULT hRegisterRasterizerResult = m_pSrcD3D9->RegisterSoftwareDevice(fn);
	if ( FAILED( hRegisterRasterizerResult ) ) 
	{
		DPF
		(
			1,
			_T("CD3D::RegisterRasterizer9() - Failed to register software rasterizer. hr=%x\n"),
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

void CD3D::ReleaseD3D9()
{
	ULONG dwRef = 0;

	UnRegisterRasterizer();

	RELEASE(m_pRefD3D9);

	if(m_pSrcD3D9)
	{
		dwRef = m_pSrcD3D9->Release();
		if ( dwRef > 0 )
		{
			DPF(1, _T("CD3D::ReleaseAll9() - m_pD3D9 ref count nonzero(%d).  TestApp leaking D3D objects.\n"), dwRef);

			ASSERT(g_pFramework != NULL);
			if(g_pFramework->m_bStressMode)
			{
				ASSERT(dwRef == 0);
			}
		}

		m_pSrcD3D9 = NULL;
	}
}


///////////////////////////////////////////////////////
// Function:  ResolveBehaviors9
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
bool CD3D::ResolveBehaviors9
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
	HRESULT hr = GetDeviceCaps9(Adapter, DeviceType, &DevCaps);
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
		if((DevCaps.DeviceType == D3DDEVTYPE_HAL) != 0)
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
	};
	

	if ( (DEVICECREATE_BESTAVAILABLE & ResolvedBehaviors) != 0 )
	{
		ResolvedBehaviors &= ~DEVICECREATE_BESTAVAILABLE;

		// Assume HWVP.  If the device can't do HWTRANSFORMANDLIGHT,
		// then this will get overridden below.
		if ( (DevCaps.DevCaps & D3DDEVCAPS_PUREDEVICE) != 0 )
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

bool CD3D::DetectCaps9(CDevice* pDevice)
{
	HRESULT		hr;
	CAPS    	DevCaps;
	DWORD 		dwCount;
	FORMAT		**pFmt;
	bool		bRawCaps;
		
	if(m_pSrcD3D9 == NULL || pDevice == NULL)
	{
		DPF(1, _T("CD3D::DetectCaps9() - invalid pointer.\n"));
		return false;
	}

	// Get the adapter for this device
	int nAdapter = pDevice->GetAdapterID();
	ADAPTER *pAdapter = pDevice->GetAdapter();
	if(pAdapter == NULL)
	{
		DPF(1, _T("CD3D::DetectCaps9() - invalid adapter ID(#%d).\n"), nAdapter);
		return false;
	}

	// Get the device caps.  If there's an actual device available, use it otherwise use the raw adapter caps
	if(pDevice->m_pDevice9)
	{
		bRawCaps = false;

		hr = pDevice->GetDeviceCaps(&DevCaps);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3D::DetectCaps9() - GetDeviceCaps() failed. hr=%s(%x)\n"), HResultToString(hr), hr);
			return false;
		}
	}
	else
	{
		bRawCaps = true;

		hr = GetDeviceCaps(nAdapter, pDevice->GetDevType(), &DevCaps);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3D::DetectCaps9() - GetDeviceCaps() failed. hr=%s(%x)\n"), HResultToString(hr), hr);
			return false;
		}

		// Check DDI level - we need raw caps for this
		if (DevCaps.DevCaps2 & D3DDEVCAPS2_STREAMOFFSET)
			pDevice->m_nDDILevel = 9;
		else if (DevCaps.MaxStreams > 0)
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
	pDevice->m_Desc.dwDevCaps2 = DevCaps.DevCaps2;
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
	pDevice->m_Desc.MasterAdapterOrdinal = DevCaps.MasterAdapterOrdinal;
	pDevice->m_Desc.AdapterOrdinal = DevCaps.AdapterOrdinal;
	pDevice->m_Desc.AdapterOrdinalInGroup = DevCaps.AdapterOrdinalInGroup;
	pDevice->m_Desc.NumberOfAdaptersInGroup = DevCaps.NumberOfAdaptersInGroup;
	pDevice->m_Desc.NumSimultaneousRTs = DevCaps.NumSimultaneousRTs;
	pDevice->m_Desc.dwDeclTypes = DevCaps.DeclTypes;
	pDevice->m_Desc.VS20Caps.Caps = DevCaps.VS20Caps.Caps;
	pDevice->m_Desc.VS20Caps.DynamicFlowControlDepth = DevCaps.VS20Caps.DynamicFlowControlDepth;
	pDevice->m_Desc.VS20Caps.NumTemps = DevCaps.VS20Caps.NumTemps;
	pDevice->m_Desc.VS20Caps.StaticFlowControlDepth = DevCaps.VS20Caps.StaticFlowControlDepth;
	pDevice->m_Desc.PS20Caps.Caps = DevCaps.PS20Caps.Caps;
	pDevice->m_Desc.PS20Caps.DynamicFlowControlDepth = DevCaps.PS20Caps.DynamicFlowControlDepth;
	pDevice->m_Desc.PS20Caps.NumTemps = DevCaps.PS20Caps.NumTemps;
	pDevice->m_Desc.PS20Caps.StaticFlowControlDepth = DevCaps.PS20Caps.StaticFlowControlDepth;
	pDevice->m_Desc.PS20Caps.NumInstructionSlots = DevCaps.PS20Caps.NumInstructionSlots;
	pDevice->m_Desc.dwVertexTextureFilterCaps = DevCaps.VertexTextureFilterCaps;
	pDevice->m_Desc.dwMaxVShaderInstructionsExecuted = DevCaps.MaxVShaderInstructionsExecuted;
	pDevice->m_Desc.dwMaxPShaderInstructionsExecuted = DevCaps.MaxPShaderInstructionsExecuted;
	
	pDevice->m_Desc.dwStretchRectFilterCaps = DevCaps.StretchRectFilterCaps;
	pDevice->m_Desc.dwReserved5 = DevCaps.Reserved5;
	pDevice->m_Desc.MaxNpatchTessellationLevel = DevCaps.MaxNpatchTessellationLevel;
	pDevice->m_Desc.MaxPixelShader30InstructionSlots = DevCaps.MaxPixelShader30InstructionSlots;
	pDevice->m_Desc.MaxVertexShader30InstructionSlots = DevCaps.MaxVertexShader30InstructionSlots;

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
		for (dwCount = 0; dwCount < g_dwZBufferFormatListSize9; dwCount++)
		{
			FORMAT fmt(g_ZBufferFormatList9[dwCount]);
			hr = CheckDeviceFormat(nAdapter, pDevice->GetDevType(), pAdapter->DesktopMode.Format, USAGE_DEPTHSTENCIL, RTYPE_SURFACE, fmt);
			if (FAILED(hr))
				continue;
			
			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D::DetectCaps9() - Out of memory.\n"));
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
		for (dwCount = 0; dwCount < g_dwTextureFormatListSize9; dwCount++)
		{
			FORMAT fmt(g_TextureFormatList9[dwCount]);
			hr = CheckDeviceFormat(nAdapter, pDevice->GetDevType(), pAdapter->DesktopMode.Format, 0, RTYPE_TEXTURE, fmt);
			if (FAILED(hr))
				continue;

			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D::DetectCaps9() - Out of memory.\n"));
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
		for (dwCount = 0; dwCount < g_dwTextureFormatListSize9; dwCount++)
		{
			FORMAT fmt(g_TextureFormatList9[dwCount]);
			hr = CheckDeviceFormat(nAdapter, pDevice->GetDevType(), pAdapter->DesktopMode.Format, 0, RTYPE_CUBETEXTURE, (FORMAT)g_TextureFormatList9[dwCount]);
			if (FAILED(hr))
				continue;

			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D::DetectCaps9() - Out of memory.\n"));
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
		SAFEDELETEA(pDevice->m_pVolumeFormat);

		pFmt = &(pDevice->m_pVolumeFormat);
		for (dwCount = 0; dwCount < g_dwTextureFormatListSize9; dwCount++)
		{
			FORMAT fmt(g_TextureFormatList9[dwCount]);
			hr = CheckDeviceFormat(nAdapter, pDevice->GetDevType(), pAdapter->DesktopMode.Format, 0, RTYPE_VOLUMETEXTURE, fmt);
			if (FAILED(hr))
				continue;

			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D::DetectCaps9() - Out of memory.\n"));
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
		for (dwCount = 0; dwCount < g_dwBackBufferFormats9; dwCount++)
		{
			FORMAT fmt(g_BackBufferFormats9[dwCount]);

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

		pDevice->m_pMultisampType = new MULTISAMPLE_TYPE[g_dwMultiSampleTypeListSize9];
		if(NULL == pDevice->m_pMultisampType)
		{
			DPF(1, _T("CD3D::DetectCaps9() - Out of memory.\n"));
			goto Error;
		}
		pDevice->m_nMultisampType = g_dwMultiSampleTypeListSize9;
		memcpy(pDevice->m_pMultisampType, &g_MultiSampleTypeList9, sizeof(MULTISAMPLE_TYPE) * g_dwMultiSampleTypeListSize9);
	}

	// Create the device description
	pDevice->UpdateDescription();

	return true;

Error:
	// Device info will get released when the device is destroyed

	return false;
}

// --------------------------------------------------------------------------------------

void CD3D::ProcessError9(HRESULT hr)
{
	// Only set this flag - it'll get reset when the device recovers.
	if(hr == E_OUTOFMEMORY)
		m_bOutofMemory = true;

	if(hr == D3DERR_DEVICELOST)
		m_bDeviceLost = true;

	if(hr == D3DERR_DRIVERINTERNALERROR)
		m_bDriverInternalError = true;
}

// --------------------------------------------------------------------------------------

bool CD3D::CompareFormats9(FORMAT *pSrcFmt, FORMAT *pRefFmt)
{
	if(!pSrcFmt || !pRefFmt)
	{
		DPF(1, _T("CD3D::CompareFormats9() - Invalid parameters.\n"));
		return false;
	}

	return pSrcFmt->d3dfFormat == pRefFmt->d3dfFormat;
}

// --------------------------------------------------------------------------------------
// D3D9 object wrappers
// --------------------------------------------------------------------------------------

HRESULT CD3D::RegisterSoftwareDevice9(void* pInitializeFunction)
{
	return m_pSrcD3D9->RegisterSoftwareDevice( pInitializeFunction );
}

// --------------------------------------------------------------------------------------

UINT CD3D::GetAdapterCount9(void)
{
	return m_pSrcD3D9->GetAdapterCount();
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::GetAdapterIdentifier9(UINT Adapter,DWORD Flags,ADAPTER_IDENTIFIER* pIdentifier)
{
	return m_pSrcD3D9->GetAdapterIdentifier( Adapter,Flags,(D3DADAPTER_IDENTIFIER9*)pIdentifier );
}

// --------------------------------------------------------------------------------------

UINT CD3D::GetAdapterModeCountEx9( UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter )
{
	return m_pSrcD3D9Ex->GetAdapterModeCountEx( Adapter, pFilter );
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::GetAdapterDisplayMode9(UINT Adapter,DISPLAYMODE* pMode)
{
	if( m_pSrcD3D9 == NULL )
	{
		DPF(1, _T("CD3D::GetAdapterDisplayMode9() - Invalid D3D pointer!\n"));
		return E_FAIL;
	}

	D3DDISPLAYMODE Mode;

	HRESULT hr = m_pSrcD3D9->GetAdapterDisplayMode( Adapter,pMode==0L?0L:&Mode );

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

HRESULT CD3D::CheckDeviceType9(UINT Adapter,DWORD DeviceType,FORMAT DisplayFormat,
							   FORMAT BackBufferFormat,BOOL Windowed)
{
	D3DDEVTYPE DevType = ConvertDeviceType(DeviceType);
	D3DFORMAT  AdptFmt = DisplayFormat.d3dfFormat;

	// REF doesn't support A2R10G10B10 natively in windowed mode.  This hack
	// makes it think we're going to be rendering in a standard 32-bit mode
	// so that we can correctly get all supported formats.
	if ((DevType == D3DDEVTYPE_REF) && (AdptFmt == D3DFMT_A2R10G10B10 || AdptFmt == D3DFMT_A2B10G10R10_XR_BIAS))
		AdptFmt = D3DFMT_X8R8G8B8;

	return m_pSrcD3D9->CheckDeviceType( Adapter,DevType,AdptFmt,BackBufferFormat.d3dfFormat,Windowed );
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDeviceFormat9(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,
								 DWORD Usage,RESOURCETYPE RType,FORMAT CheckFormat)
{
	D3DDEVTYPE DevType = ConvertDeviceType(DeviceType);
	D3DFORMAT  AdptFmt = AdapterFormat.d3dfFormat;

	// REF doesn't support A2R10G10B10 natively in windowed mode.  This hack
	// makes it think we're going to be rendering in a standard 32-bit mode
	// so that we can correctly get all supported formats.
	if ((DevType == D3DDEVTYPE_REF) && (AdptFmt == D3DFMT_A2R10G10B10 || AdptFmt == D3DFMT_A2B10G10R10_XR_BIAS))
		AdptFmt = D3DFMT_X8R8G8B8;

	return (m_pSrcD3D9->CheckDeviceFormat( Adapter,DevType,AdptFmt,Usage,(D3DRESOURCETYPE)RType,CheckFormat.d3dfFormat));

}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDeviceMultiSampleType8To9(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,
										  BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType)
{
	D3DDEVTYPE DevType = ConvertDeviceType(DeviceType);
	D3DFORMAT  AdptFmt = AdapterFormat.d3dfFormat;

	// REF doesn't support A2R10G10B10 natively in windowed mode.  This hack
	// makes it think we're going to be rendering in a standard 32-bit mode
	// so that we can correctly get all supported formats.
	if ((DevType == D3DDEVTYPE_REF) && (AdptFmt == D3DFMT_A2R10G10B10 || AdptFmt == D3DFMT_A2B10G10R10_XR_BIAS))
		AdptFmt = D3DFMT_X8R8G8B8;

	return (m_pSrcD3D9->CheckDeviceMultiSampleType( Adapter,DevType,AdptFmt,
												 Windowed,(D3DMULTISAMPLE_TYPE)MultiSampleType, NULL));
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDeviceMultiSampleType9(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,
										  BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType, DWORD *dwQualityLevels)
{
	D3DDEVTYPE DevType = ConvertDeviceType(DeviceType);
	D3DFORMAT  AdptFmt = AdapterFormat.d3dfFormat;

	// REF doesn't support A2R10G10B10 natively in windowed mode.  This hack
	// makes it think we're going to be rendering in a standard 32-bit mode
	// so that we can correctly get all supported formats.
	if ((DevType == D3DDEVTYPE_REF) && (AdptFmt == D3DFMT_A2R10G10B10 || AdptFmt == D3DFMT_A2B10G10R10_XR_BIAS))
		AdptFmt = D3DFMT_X8R8G8B8;

	return (m_pSrcD3D9->CheckDeviceMultiSampleType( Adapter,DevType,AdptFmt,
												 Windowed,(D3DMULTISAMPLE_TYPE)MultiSampleType, dwQualityLevels));
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDepthStencilMatch9(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,
									  FORMAT RenderTargetFormat,FORMAT DepthStencilFormat)
{
	D3DDEVTYPE DevType = ConvertDeviceType(DeviceType);
	D3DFORMAT  AdptFmt = AdapterFormat.d3dfFormat;

	// REF doesn't support A2R10G10B10 natively in windowed mode.  This hack
	// makes it think we're going to be rendering in a standard 32-bit mode
	// so that we can correctly get all supported formats.
	if ((DevType == D3DDEVTYPE_REF) && (AdptFmt == D3DFMT_A2R10G10B10 || AdptFmt == D3DFMT_A2B10G10R10_XR_BIAS))
		AdptFmt = D3DFMT_X8R8G8B8;

	return (m_pSrcD3D9->CheckDepthStencilMatch( Adapter,DevType,AdptFmt,
											 RenderTargetFormat.d3dfFormat,
											 DepthStencilFormat.d3dfFormat ));
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::GetDeviceCaps9(UINT Adapter,DWORD DeviceType,CAPS* pCaps)
{
	D3DDEVTYPE DevType = ConvertDeviceType(DeviceType);

	if(pCaps)
		memset(pCaps, 0, sizeof(CAPS));

	return m_pSrcD3D9->GetDeviceCaps( Adapter,DevType,(D3DCAPS9*)pCaps );
}

// --------------------------------------------------------------------------------------

HMONITOR CD3D::GetAdapterMonitor9(UINT Adapter)
{
	return (m_pSrcD3D9->GetAdapterMonitor( Adapter ));
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDeviceFormatConversion9(UINT Adapter,DWORD DeviceType,FORMAT SourceFormat,FORMAT TargetFormat)
{
	D3DDEVTYPE DevType = ConvertDeviceType(DeviceType);

	return m_pSrcD3D9 -> CheckDeviceFormatConversion(Adapter, DevType, SourceFormat.d3dfFormat, TargetFormat.d3dfFormat);
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::EnumAdapterModes8To9(UINT Adapter,UINT nMode,DISPLAYMODE* pMode)
{
	D3DDISPLAYMODE Mode;

	HRESULT hr = m_pSrcD3D9->EnumAdapterModes( Adapter,GetAdapter(Adapter)->DesktopMode.Format.d3dfFormat, 
											nMode,pMode==0L?0L:&Mode );
	
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

HRESULT CD3D::EnumAdapterModes9(UINT Adapter, FORMAT Format, UINT nMode,DISPLAYMODE* pMode)
{
	D3DDISPLAYMODE Mode;

	HRESULT hr = m_pSrcD3D9->EnumAdapterModes( Adapter, Format.d3dfFormat, nMode,pMode==0L?0L:&Mode );
	
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

HRESULT CD3D::EnumAdapterModesEx9( UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter,UINT Mode, D3DDISPLAYMODEEX* pMode )
{
	return m_pSrcD3D9Ex->EnumAdapterModesEx( Adapter, pFilter, Mode, pMode );
}

// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------

void CD3D::SetDebugMute( bool bMute )
{
	if( m_pDebugMute )
		m_pDebugMute( bMute );
}


///////////////////////////////////////////////////////
// Function:  GetD3DXBuildNumber
//
// Purpose:
// Determines the core and D3DX SDK versions.
//
// Notes:
// The GetD3DXBuildNumber function is separate from the
// CD3D9 class so that it can be called by
// DetectDXVersion.
///////////////////////////////////////////////////////
HRESULT GetD3DXBuildNumber
(
	float fDXVersion,
	float & fD3DXVersion
)
{
	HRESULT hr = S_OK;
	TCHAR* sShader = _T("float4  Tex() : COLOR { return float4(0,0,0,0); }"), *sTemp = NULL;
	LPD3DXBUFFER pShader = NULL;
	LPD3DXCONSTANTTABLE pConstantTable = NULL;
	D3DXCONSTANTTABLE_DESC desc;

	//
	// Validate parameters.
	//
	if(fDXVersion < 9.f)
	{
		return E_UNSUPPORTED;
	};


	// Need to initialize D3DX dll before we attempt to detect the version
	if(FAILED(hr = InitD3DX(fDXVersion)))
		goto Exit;
	
	// This will set the g_bStaticD3DX flag if dynamic
	D3DXCheckVersion(D3D_SDK_VERSION,D3DX_SDK_VERSION);
	
	// Fail out if we're loading dynamically
	if(!g_bStaticD3DX)
	{
		hr = E_UNSUPPORTED;
		goto Exit;
	}
	
	// We are statically linked, so we will get the build number through the ConstantTable interface
	if(FAILED(hr = D3DXCompileShader(sShader,_tcslen(sShader),NULL,NULL,_T("Tex"),_T("ps_1_1"),0,&pShader,NULL,&pConstantTable)))
		goto Exit;

	if (FAILED(hr = pConstantTable->GetDesc(&desc)))
		goto Exit;

	if (desc.Creator == NULL)
	{
		hr = E_FAIL;
		goto Exit;
	}

	// Detect version for "new" build number scheme
	if(NULL == (sTemp = _tcschr(desc.Creator, _T('.'))) ||
	   NULL == (sTemp = _tcschr(sTemp+1, _T('.'))))
	{
		hr = E_UNSUPPORTED;
		goto Exit;
	}
	sTemp++;

	fD3DXVersion = (float)_tstof(sTemp);

	// Detect version for older SDK build numbers
	if(fD3DXVersion < 1.f)
	{
		sTemp = _tcsninc(desc.Creator, _tcslen(desc.Creator) - 4);
		fD3DXVersion = (float)_tstof(sTemp);	
	}

Exit:
	RELEASE(pShader);
	RELEASE(pConstantTable);
	return hr;
}


// ConvertDeviceType - Helper function to convert our framework DWORD DeviceType into a D3DDEVTYPE DeviceType
D3DDEVTYPE ConvertDeviceType(DWORD DeviceType)
{
	if (DeviceType == DEVICETYPE_HAL)
		return D3DDEVTYPE_HAL;
	else if((DeviceType == DEVICETYPE_REF)||(DeviceType == DEVICETYPE_NULLREF))
		return D3DDEVTYPE_REF;
	else if(DeviceType == DEVICETYPE_SW)
		return D3DDEVTYPE_SW;
	else
		return (D3DDEVTYPE) 0; // Invalid device type
}