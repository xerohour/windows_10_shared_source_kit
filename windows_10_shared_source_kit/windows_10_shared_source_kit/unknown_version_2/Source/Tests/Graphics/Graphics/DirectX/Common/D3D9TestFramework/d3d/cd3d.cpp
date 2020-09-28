// ======================================================================================
//
//  CD3D.cpp
//
//  Copyright (C) 1999 Microsoft Corporation
//
// ======================================================================================

#include "D3DPCH.h"

#include "CD3D.h"
#include "CD3DX.h"
#include <math.h>
#include <bcrypt.h>
#include <d3dx.h>
#include <shlwapi.h>
#include <strsafe.h>

#if defined(_PREFAST_)
#pragma prefast(disable: 12110)
#endif

bool g_bStaticD3DX = true;


///////////////////////////////////////////////////////
// Macro:  HANDLE_HRESULT
//
// Purpose:
// Expands to a block of code like the following:
//   case D3DERR_DEVICELOST:
//   {
//      return _T( "D3DERR_DEVICELOST" );
//   } break
///////////////////////////////////////////////////////
#define HANDLE_HRESULT( x ) \
		case x: \
		{ \
			return _T( #x ); \
		} break

// --------------------------------------------------------------------------------------

CD3D::CD3D()
{
	m_pLog 			= NULL;
	m_pSrcD3D9 		= NULL;
	m_pRefD3D9 		= NULL;
	m_pD3D8 		= NULL;
	m_pD3D7 		= NULL;
	m_pD3D3 		= NULL;
	m_pDDraw7		= NULL;
	m_pDDraw4		= NULL;
	m_pPrimary 		= NULL;
	m_hSWRast		= NULL;
	m_bUserSelectedSWRast = false;
	m_pAdapters		= NULL;
	m_hrLastError	= S_OK;
	m_hSrcD3DDLL	= NULL;
	m_hRefD3DDLL	= NULL;
	m_hDebugD3DDLL	= NULL;
	m_bOutofMemory	= false;
	m_bDeviceLost	= false;
	m_bDriverInternalError = false;
	m_bReferenceDetected = false;
	m_sReferenceDLL	= _T("Unknown");
	m_sSWRast = m_sSWRastEntry = NULL;
	COPYSTRING(m_sSWRast, _T("d3dref9.dll"));
	COPYSTRING(m_sSWRastEntry,_T("D3D9GetSWInfo"));
	m_nDefaultDisplyModesAlloc = 128;
	m_sSrcDLL = m_sRefDLL = m_sDebugDLL = NULL;
	COPYSTRING(m_sSrcDLL, _T("D3D9.DLL"));
	COPYSTRING(m_sRefDLL, _T("D3D9.DLL"));
	COPYSTRING(m_sDebugDLL, _T("D3D9D.DLL"));
    
	m_bDebug 	= false;
	m_fVersion 	= 0.f;
	m_fBuild 	= 0.f;
	memset(&m_ddCaps, 0, sizeof(m_ddCaps));
	m_ddCaps.dwSize = sizeof(m_ddCaps);
	m_DriverModel = DRIVERMODEL_XPDM;

	m_bD3DDebugHelper = false;

#ifdef _AMD64_
	m_fMinDXVersion = 9.f;
#elif _IA64_
	m_fMinDXVersion = 8.f;
#else // x86
	m_fMinDXVersion = 6.f;
#endif

	// Init all the function ptrs
	pInitD3D = NULL;
	pReleaseD3D = NULL;

	m_pDebugMute = NULL;
}

// --------------------------------------------------------------------------------------

CD3D::~CD3D()
{
	ReleaseAll();

	SAFEDELETEA(m_sRefDLL);
	SAFEDELETEA(m_sSrcDLL);
	SAFEDELETEA(m_sDebugDLL);
	SAFEDELETEA(m_sSWRast);
	SAFEDELETEA(m_sSWRastEntry);
}


///////////////////////////////////////////////////////
// Function:  DetectDXVersion
//
// Purpose:
// Determines the installed version and SDK build of
// Direct3D and the DirectX SDK.
//
// Notes:
// The DetectDXVersion function is separate from the
// CD3D class so that it can be called from non-
// D3D9Framework apps wishing to log to the DXG test
// database.
///////////////////////////////////////////////////////
bool DetectDXVersion
(
	__out_opt float * pfDXBuildNumber,
	__out_opt float * pfDXVersion
)
{
	DPF(4, _T("CD3D::DetectVersion()\n"));

	HKEY hRegKey = NULL;
	TCHAR tcsVersion[31 + 1] = {0};
	TCHAR tcsRC[3] = {0};
	ULONG nStrSize = sizeof(TCHAR) * 31;
	ULONG nValType = REG_SZ;
	TCHAR * ptcVer = NULL;
	TCHAR * ptcBld = NULL;
	TCHAR * ptcStop = NULL; // place-markers
	HMODULE hMod = NULL;
	DWORD dwRCVal = 0;
	float fVersion = 0.f;
	float fBuild = 0.f;
	float fSDKBuild = 0.f;
	bool bOverallResult = false;


	//
	// Validate and initialize parameters.
	//

	if
	(
		pfDXBuildNumber == NULL &&
		pfDXVersion == NULL
	)
	{
		DPF(1, _T( "DetectDXVersion:  Invalid parameters.\n" ) );

		goto Cleanup;
	};


	if ( pfDXBuildNumber != NULL )
	{
		*pfDXBuildNumber = 0.0f;
	};

	if ( pfDXVersion != NULL )
	{
		*pfDXVersion = 0.0f;
	};


	//
	// Open the DirectX registry key.
	//

	LONG OpenRegistryKeyResult = RegOpenKeyEx
	(
		HKEY_LOCAL_MACHINE,
		_T("Software\\Microsoft\\DirectX"),
		NULL,
		KEY_QUERY_VALUE,
		&hRegKey
	);

	if ( OpenRegistryKeyResult != ERROR_SUCCESS )
	{
		DPF(1, _T("DetectDXVersion:  Failed to open DirectX registry key.\n" ) );

		goto Cleanup;
	};


	//
	// Read the core version string.
	//

	LONG ReadCoreVersionResult = RegQueryValueEx
	(
		hRegKey,
		_T("Version"),
		NULL,
		&nValType,
		(BYTE *) tcsVersion,
		&nStrSize
	);

	if ( ReadCoreVersionResult != ERROR_SUCCESS )
	{
		DPF( 1, _T( "DetectDXVersion:  Failed to read value Version.\n" ) );

		goto Cleanup;
	};

	if ( nValType != REG_SZ )
	{
		DPF
		(
			1,
			_T( "DetectDXVersion:  Version value found is of type %u rather than REG_SZ." ),
			nValType
		);

		goto Cleanup;
	};


	//
	// Parse the core version string.
	//

	ptcVer = _tcschr(tcsVersion, _T('.'));
	if(ptcVer != NULL)
	{
		ptcVer++; // one pos past the first '.'
	
		ptcBld = _tcsrchr(ptcVer, _T('.'));
		if(ptcBld != NULL)
		{
			*ptcBld = _T('\0');
			ptcBld++;
		
			fVersion = (float)_tcstod(ptcVer, &ptcStop);
			fBuild = (float)_tcstod(ptcBld, &ptcStop);

			// Remove any "dot" info
			fVersion = floor(fVersion);

			// Check for RC flag to indicate "dot" builds
			LONG ReadRCVersionResult = RegQueryValueEx
			(
				hRegKey,
				_T("RC"),
				NULL,
				&nValType,
				(BYTE *)tcsRC,
				&nStrSize
			);

			if ( ReadRCVersionResult == ERROR_SUCCESS )
				fBuild += ((float)_tstof(tcsRC)) * .01f;
		};
	};


	// Create the appropriate dot versions
	if(fVersion == 8.f)
	{
		if(fBuild > 400.f && fBuild < 500.f)
			fVersion = 8.01f;
		else if(fBuild >= 500.f && fBuild < 900.f)
			fVersion = 8.1f;
		else if(fBuild > 900 && fBuild < 901.02)
			fVersion = 8.11f;
		else
			fVersion = 8.12f;
	}
	if(fVersion == 9.f)
	{
		if(fBuild > 900.f && fBuild < 902.f)
			fVersion = 9.01f;
		else if(fBuild >= 902.f && fBuild < 904.f)
			fVersion = 9.02f;
		else if(fBuild >= 904.f)
			fVersion = (DetectDriverModel() == DRIVERMODEL_LDDM) ? 9.1f : 9.03f;
	}


	HRESULT hr = E_UNSUPPORTED;


	//
	// Get the SDK version.
	//
#if 0
	// N.B. [SEdmison]:
	// Easier said than done.  If we're on D3D 9 or higher, and
	// we're being called from a D3D9Framework application (that
	// is, g_pFramework is non-NULL), then we hand off to a helper
	// function that builds and executes a shader to retrieve the
	// D3DX SDK version.  This covers the case that there might
	// not be full SDK installation by not depending on a registry
	// value that wouldn't be present.
	//
	// In all other cases...
#endif
	// ...we just read the SDK version from the
	// registry.
	//
#if 0
#ifndef _IA64_ // D3DX is not supported on IA64
	if
	(
		g_pFramework == NULL ||
		FAILED
		(
			hr = GetD3DXBuildNumber
			(
				fVersion,
				fSDKBuild
			)
		)
	)
#endif
#endif
	{
		// If we encountered any critical failures...
		if ( hr != E_UNSUPPORTED )
		{
			// ...then bail.
			return false;
		};


		//
		// Read the SDK version from the registry.
		//

		nStrSize = sizeof(TCHAR) * 31;
		nValType = REG_SZ;
		LONG ReadSDKVersionResult = RegQueryValueEx
		(
			hRegKey,
			_T("SDKVersion"),
			NULL,
			&nValType,
			(BYTE *) tcsVersion,
			&nStrSize
		);

		if ( ReadSDKVersionResult == ERROR_SUCCESS )
		{
			ptcBld = _tcschr(tcsVersion, _T('.'));
			ptcBld = _tcschr(ptcBld+1, _T('.'));
			ptcBld++;

			fSDKBuild = (float)_tcstod(ptcBld, &ptcStop);
		};
	};


	//
	// Validate that we can in fact load the D3D version
	// that we've detected, or refine our version based
	// on what we find on the system.
	//

	hMod = LoadLibraryExW( L"d3d9.dll", NULL, 0 );
	if ( hMod != NULL )
	{
		if(fVersion < 9.f || fVersion >= 10.f)
			fVersion = 9.f;

		FreeLibrary(hMod);
		hMod = NULL;
	}
	else if ( hMod = LoadLibraryExW( L"d3d8.dll", NULL, 0 ) )
	{
		if(fVersion < 8.f || fVersion >= 9.f)
			fVersion = 8.f;

		FreeLibrary(hMod);
		hMod = NULL;
	}
	else if ( hMod = LoadLibraryExW( L"d3dim700.dll", NULL, 0 ) )
	{
		if(fVersion < 7.f || fVersion >= 8.f)
			fVersion = 7.f;

		FreeLibrary(hMod); 
		hMod = NULL;
	}
	else if ( hMod = LoadLibraryExW( L"d3dim.dll", NULL, 0 ) )
	{
		if(fVersion < 6.f || fVersion >= 7.f)
			fVersion = 6.f;

		FreeLibrary(hMod); 
		hMod = NULL;
	};


	//
	// Fill in our outgoing parameters.
	//

	if ( pfDXBuildNumber != NULL )
	{
		*pfDXBuildNumber = fSDKBuild;
	};

	if ( pfDXVersion != NULL )
	{
		*pfDXVersion = fVersion;
	};


	bOverallResult = true;


Cleanup:
	//
	// Close the registry key.
	//

	if ( hRegKey != NULL )
	{
		RegCloseKey( hRegKey );
		hRegKey = NULL;
	};

	return bOverallResult;
}


#if !defined(D3DDDIERR_WASSTILLDRAWING)
#define _FACD3DDDI  0x876
#define MAKE_D3DDDIHRESULT(code)  MAKE_HRESULT(1, _FACD3DDDI, code)

#define D3DDDIERR_WASSTILLDRAWING               MAKE_D3DDDIHRESULT(540)
#define D3DDDIERR_NOTAVAILABLE                  MAKE_D3DDDIHRESULT(2154)
#define D3DDDIERR_DEVICEREMOVED                 MAKE_D3DDDIHRESULT(2160)
#define D3DDDIERR_PRIVILEGEDINSTRUCTION         MAKE_D3DDDIHRESULT(2161)
#define D3DDDIERR_INVALIDINSTRUCTION            MAKE_D3DDDIHRESULT(2162)
#define D3DDDIERR_INVALIDHANDLE                 MAKE_D3DDDIHRESULT(2163)
#define D3DDDIERR_CANTEVICTPINNEDALLOCATION     ERROR_GRAPHICS_CANT_EVICT_PINNED_ALLOCATION
#define D3DDDIERR_CANTRENDERLOCKEDALLOCATION    ERROR_GRAPHICS_CANT_RENDER_LOCKED_ALLOCATION
#define D3DDDIERR_INVALIDUSERBUFFER             MAKE_D3DDDIHRESULT(2169)
#define D3DDDIERR_INCOMPATIBLEPRIVATEFORMAT     MAKE_D3DDDIHRESULT(2170)
#endif

/*
 * DirectX File errors.
 */

#define _FACD3DXF 0x876

#define D3DXFERR_BADOBJECT              MAKE_HRESULT( 1, _FACD3DXF, 900 )
#define D3DXFERR_BADVALUE               MAKE_HRESULT( 1, _FACD3DXF, 901 )
#define D3DXFERR_BADTYPE                MAKE_HRESULT( 1, _FACD3DXF, 902 )
#define D3DXFERR_NOTFOUND               MAKE_HRESULT( 1, _FACD3DXF, 903 )
#define D3DXFERR_NOTDONEYET             MAKE_HRESULT( 1, _FACD3DXF, 904 )
#define D3DXFERR_FILENOTFOUND           MAKE_HRESULT( 1, _FACD3DXF, 905 )
#define D3DXFERR_RESOURCENOTFOUND       MAKE_HRESULT( 1, _FACD3DXF, 906 )
#define D3DXFERR_BADRESOURCE            MAKE_HRESULT( 1, _FACD3DXF, 907 )
#define D3DXFERR_BADFILETYPE            MAKE_HRESULT( 1, _FACD3DXF, 908 )
#define D3DXFERR_BADFILEVERSION         MAKE_HRESULT( 1, _FACD3DXF, 909 )
#define D3DXFERR_BADFILEFLOATSIZE       MAKE_HRESULT( 1, _FACD3DXF, 910 )
#define D3DXFERR_BADFILE                MAKE_HRESULT( 1, _FACD3DXF, 911 )
#define D3DXFERR_PARSEERROR             MAKE_HRESULT( 1, _FACD3DXF, 912 )
#define D3DXFERR_BADARRAYSIZE           MAKE_HRESULT( 1, _FACD3DXF, 913 )
#define D3DXFERR_BADDATAREFERENCE       MAKE_HRESULT( 1, _FACD3DXF, 914 )
#define D3DXFERR_NOMOREOBJECTS          MAKE_HRESULT( 1, _FACD3DXF, 915 )
#define D3DXFERR_NOMOREDATA             MAKE_HRESULT( 1, _FACD3DXF, 916 )
#define D3DXFERR_BADCACHEFILE           MAKE_HRESULT( 1, _FACD3DXF, 917 )


LPCTSTR CD3D::HResultToString(HRESULT hr) const
{
	switch( hr )
	{
		//
		// Framework internal error codes
		//

		HANDLE_HRESULT( E_UNSUPPORTED );


		//
		// D3D DDI error codes
		//

		HANDLE_HRESULT( D3DDDIERR_WASSTILLDRAWING );
		HANDLE_HRESULT( D3DDDIERR_NOTAVAILABLE );
		HANDLE_HRESULT( D3DDDIERR_DEVICEREMOVED );
		HANDLE_HRESULT( D3DDDIERR_PRIVILEGEDINSTRUCTION );
		HANDLE_HRESULT( D3DDDIERR_INVALIDINSTRUCTION );
		HANDLE_HRESULT( D3DDDIERR_INVALIDHANDLE );
		HANDLE_HRESULT( D3DDDIERR_CANTEVICTPINNEDALLOCATION );
		HANDLE_HRESULT( D3DDDIERR_CANTRENDERLOCKEDALLOCATION );
		HANDLE_HRESULT( D3DDDIERR_INVALIDUSERBUFFER );
		HANDLE_HRESULT( D3DDDIERR_INCOMPATIBLEPRIVATEFORMAT );

		//
		// D3DX9 File error codes not handled by DXGetErrorString9 
		//

		HANDLE_HRESULT( D3DXFERR_BADOBJECT );
		HANDLE_HRESULT( D3DXFERR_BADVALUE );
		HANDLE_HRESULT( D3DXFERR_BADTYPE );
		HANDLE_HRESULT( D3DXFERR_NOTFOUND );
		HANDLE_HRESULT( D3DXFERR_NOTDONEYET );
		HANDLE_HRESULT( D3DXFERR_FILENOTFOUND );
		HANDLE_HRESULT( D3DXFERR_RESOURCENOTFOUND );
		HANDLE_HRESULT( D3DXFERR_BADRESOURCE );
		HANDLE_HRESULT( D3DXFERR_BADFILETYPE );
		HANDLE_HRESULT( D3DXFERR_BADFILEVERSION );
		HANDLE_HRESULT( D3DXFERR_BADFILEFLOATSIZE );
		HANDLE_HRESULT( D3DXFERR_BADFILE );
		HANDLE_HRESULT( D3DXFERR_PARSEERROR );
		HANDLE_HRESULT( D3DXFERR_BADARRAYSIZE );
		HANDLE_HRESULT( D3DXFERR_BADDATAREFERENCE );
		HANDLE_HRESULT( D3DXFERR_NOMOREOBJECTS );
		HANDLE_HRESULT( D3DXFERR_NOMOREDATA );
		HANDLE_HRESULT( D3DXFERR_BADCACHEFILE );
	};

	return DXGetErrorString9( hr );	
}

// --------------------------------------------------------------------------------------

void CD3D::SetSWRast(__in LPCTSTR pcsDLL, __in LPCTSTR pcsEntry)
{
	DPF(4, _T("CD3D::SetSWRast()\n"));

	if(pcsDLL == NULL || pcsEntry == NULL)
	{
		DPF(1, _T("CD3D::SetSWRast() - Invalid ptr.\n"));
		return;
	}

	COPYSTRING(m_sSWRast,pcsDLL);
	COPYSTRING(m_sSWRastEntry,pcsEntry);

	m_bUserSelectedSWRast = true;
}

// --------------------------------------------------------------------------------------

void CD3D::SetRuntimeDLLName(__in LPCTSTR sSrcDLL, __in LPCTSTR sRefDLL)
{
	if(NULL != sSrcDLL)
	{
		COPYSTRING(m_sSrcDLL,sSrcDLL);
	}
	if(NULL != sRefDLL)
	{
		COPYSTRING(m_sRefDLL,sRefDLL);
	}
}

// --------------------------------------------------------------------------------------

bool CD3D::GetPSGPFromReg()
{
	DPF(4, _T("CD3D::GetPSGPFromReg()\n"));

	HKEY      hRegKey;
	DWORD     dwVal;
	LRESULT   lr;
	ULONG     nSize = sizeof(DWORD);
	ULONG     nValType = REG_DWORD;

	lr = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		_T("Software\\Microsoft\\Direct3d"),
		NULL,
		KEY_QUERY_VALUE,
		&hRegKey);

	if(lr != ERROR_SUCCESS)
	{
		DPF(2, _T("CD3D::GetPSGPFromReg() - Failed to open registry! Assuming PSGP enabled.\n"));
		//return false;
		m_bPSGP = true;
		return true;
	}

	lr = RegQueryValueEx(
		hRegKey,
		_T("DisablePSGP"),
		NULL,
		&nValType,
		(BYTE *)&dwVal,
		&nSize);

	RegCloseKey(hRegKey);

	if(lr != ERROR_SUCCESS)
	{
		DPF(2, _T("CD3D::GetPSGPFromReg() - failed to read registry, assuming PSGP enabled.\n"));
		m_bPSGP = true;
		return true;
	}

	//
	// Extract the Version from the registry string
	//

	if(dwVal == 0)
		m_bPSGP = true;
	else
		m_bPSGP = false;

	return true;
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::InitFunctionPtrs(float fVersion)
{
	HRESULT hr = S_OK;

	//
	// Init D3DX ptrs
	//
	hr = InitD3DX(fVersion);
	if(FAILED(hr))
	{
		DPF(2, _T("CD3D::InitFunctionPtrs() - failed to init D3DX.\n"));
		return hr;
	}
	
	//
	// Init the function ptrs
	//
	if(fVersion >= 9.f && fVersion < 10.f)
	{
		pInitD3D = &CD3D::InitD3D9;
		pReleaseD3D = &CD3D::ReleaseD3D9;
		pDetectCaps = &CD3D::DetectCaps9;
		pResolveBehaviors = &CD3D::ResolveBehaviors9;
		pCompareFormats = &CD3D::CompareFormats9;
		pEvictManagedTextures = &CD3D::EvictManagedTexturesUnsup;
		pCreateDDrawPalette = &CD3D::CreateDDrawPaletteUnsup;
		pRegisterSoftwareDevice = &CD3D::RegisterSoftwareDevice9;
		pGetAdapterCount = &CD3D::GetAdapterCount9;
		pGetAdapterIdentifier = &CD3D::GetAdapterIdentifier9;
		pGetAdapterModeCount = &CD3D::GetAdapterModeCountUnsup;
		pGetAdapterModeCountEx = &CD3D::GetAdapterModeCountEx9;
		pEnumAdapterModes = &CD3D::EnumAdapterModes8To9;
		pEnumAdapterModes2 = &CD3D::EnumAdapterModes9;
		pEnumAdapterModesEx = &CD3D::EnumAdapterModesEx9;
		pGetAdapterDisplayMode = &CD3D::GetAdapterDisplayMode9;
		pCheckDeviceType = &CD3D::CheckDeviceType9;
		pCheckDeviceFormat = &CD3D::CheckDeviceFormat9;
		pCheckDeviceMultiSampleType = &CD3D::CheckDeviceMultiSampleType8To9;
		pCheckDeviceMultiSampleType9= &CD3D::CheckDeviceMultiSampleType9;
		pCheckDepthStencilMatch = &CD3D::CheckDepthStencilMatch9;
		pGetDeviceCaps = &CD3D::GetDeviceCaps9;
		pGetAdapterMonitor = &CD3D::GetAdapterMonitor9;
		pCheckDeviceFormatConversion = &CD3D::CheckDeviceFormatConversion9;
		pProcessError = &CD3D::ProcessError9;
	}
	else if(fVersion >= 8.f && fVersion < 9.f)
	{
		pInitD3D = &CD3D::InitD3D8;
		pReleaseD3D = &CD3D::ReleaseD3D8;
		pDetectCaps = &CD3D::DetectCaps8;
		pResolveBehaviors = &CD3D::ResolveBehaviors8;
		pCompareFormats = &CD3D::CompareFormats8;
		pEvictManagedTextures = &CD3D::EvictManagedTexturesUnsup;
		pCreateDDrawPalette = &CD3D::CreateDDrawPaletteUnsup;
		pRegisterSoftwareDevice = &CD3D::RegisterSoftwareDevice8;
		pGetAdapterCount = &CD3D::GetAdapterCount8;
		pGetAdapterIdentifier = &CD3D::GetAdapterIdentifier8;
		pGetAdapterModeCount = &CD3D::GetAdapterModeCount8;
		pGetAdapterModeCountEx = &CD3D::GetAdapterModeCountExUnsup;
		pEnumAdapterModes = &CD3D::EnumAdapterModes8;
		pEnumAdapterModes2 = &CD3D::EnumAdapterModesUnsup9;
		pEnumAdapterModesEx = &CD3D::EnumAdapterModesExUnsup;
		pGetAdapterDisplayMode = &CD3D::GetAdapterDisplayMode8;
		pCheckDeviceType = &CD3D::CheckDeviceType8;
		pCheckDeviceFormat = &CD3D::CheckDeviceFormat8;
		pCheckDeviceMultiSampleType = &CD3D::CheckDeviceMultiSampleType8;
		pCheckDeviceMultiSampleType9= &CD3D::CheckDeviceMultiSampleType9To8;
		pCheckDepthStencilMatch = &CD3D::CheckDepthStencilMatch8;
		pGetDeviceCaps = &CD3D::GetDeviceCaps8;
		pGetAdapterMonitor = &CD3D::GetAdapterMonitor8;
		pProcessError = &CD3D::ProcessError8;
		pCheckDeviceFormatConversion = &CD3D::CheckDeviceFormatConversionUnsup;
	}
	else if(fVersion == 7.f)
	{
		pInitD3D = &CD3D::InitD3D7;
		pReleaseD3D = &CD3D::ReleaseD3D7;
		pDetectCaps = &CD3D::DetectCaps7;
		pResolveBehaviors = &CD3D::ResolveBehaviors7;
		pCompareFormats = &CD3D::CompareFormats7;
		pEvictManagedTextures = &CD3D::EvictManagedTextures7;
		pCreateDDrawPalette = &CD3D::CreateDDrawPalette7;
		pRegisterSoftwareDevice = &CD3D::RegisterSoftwareDeviceUnsup;
		pGetAdapterCount = &CD3D::GetAdapterCountUnsup;
		pGetAdapterIdentifier = &CD3D::GetAdapterIdentifierUnsup;
		pGetAdapterModeCount = &CD3D::GetAdapterModeCountUnsup;
		pGetAdapterModeCountEx = &CD3D::GetAdapterModeCountExUnsup;
		pEnumAdapterModes = &CD3D::EnumAdapterModesUnsup;
		pEnumAdapterModes2 = &CD3D::EnumAdapterModesUnsup9;
		pEnumAdapterModesEx = &CD3D::EnumAdapterModesExUnsup;
		pGetAdapterDisplayMode = &CD3D::GetAdapterDisplayModeUnsup;
		pCheckDeviceType = &CD3D::CheckDeviceTypeUnsup;
		pCheckDeviceFormat = &CD3D::CheckDeviceFormatUnsup;
		pCheckDeviceMultiSampleType = &CD3D::CheckDeviceMultiSampleTypeUnsup;
		pCheckDeviceMultiSampleType9= &CD3D::CheckDeviceMultiSampleType9Unsup;
		pCheckDepthStencilMatch = &CD3D::CheckDepthStencilMatchUnsup;
		pGetDeviceCaps = &CD3D::GetDeviceCapsUnsup;
		pGetAdapterMonitor = &CD3D::GetAdapterMonitorUnsup;
		pProcessError = &CD3D::ProcessError7;
		pCheckDeviceFormatConversion = &CD3D::CheckDeviceFormatConversionUnsup;
	}
	else if(fVersion == 6.f)
	{
		pInitD3D = &CD3D::InitD3D6;
		pReleaseD3D = &CD3D::ReleaseD3D6;
		pDetectCaps = &CD3D::DetectCaps6;
		pResolveBehaviors = &CD3D::ResolveBehaviors6;
		pCompareFormats = &CD3D::CompareFormats7;
		pEvictManagedTextures = &CD3D::EvictManagedTextures6;
		pCreateDDrawPalette = &CD3D::CreateDDrawPalette6;
		pRegisterSoftwareDevice = &CD3D::RegisterSoftwareDeviceUnsup;
		pGetAdapterCount = &CD3D::GetAdapterCountUnsup;
		pGetAdapterIdentifier = &CD3D::GetAdapterIdentifierUnsup;
		pGetAdapterModeCount = &CD3D::GetAdapterModeCountUnsup;
		pGetAdapterModeCountEx = &CD3D::GetAdapterModeCountExUnsup;
		pEnumAdapterModes = &CD3D::EnumAdapterModesUnsup;
		pEnumAdapterModes2 = &CD3D::EnumAdapterModesUnsup9;
		pEnumAdapterModesEx = &CD3D::EnumAdapterModesExUnsup;
		pGetAdapterDisplayMode = &CD3D::GetAdapterDisplayModeUnsup;
		pCheckDeviceType = &CD3D::CheckDeviceTypeUnsup;
		pCheckDeviceFormat = &CD3D::CheckDeviceFormatUnsup;
		pCheckDeviceMultiSampleType = &CD3D::CheckDeviceMultiSampleTypeUnsup;
		pCheckDepthStencilMatch = &CD3D::CheckDepthStencilMatchUnsup;
		pGetDeviceCaps = &CD3D::GetDeviceCapsUnsup;
		pGetAdapterMonitor = &CD3D::GetAdapterMonitorUnsup;
		pProcessError = &CD3D::ProcessError7;
		pCheckDeviceFormatConversion = &CD3D::CheckDeviceFormatConversionUnsup;
	}
	else
	{
		DPF(1, _T("CD3D::InitFunctionPtrs() - Invalid D3D version requested(%f).\n"), fVersion);
		return E_NOTIMPL;
	}

	return S_OK;
}

// --------------------------------------------------------------------------------------

float CD3D::GetRuntimeVersion()
{
	const float fTolerance = 0.0001f;

	// DX9 interfaces
	if(m_fVersion == 9.f)
	{
		// DX9.1 - Longhorn with LDDM enabled
		if(FLOAT_EQUAL(m_fMaxDXVersion,9.1f))
			return 9.1f;
		// DX9.0c - redist
		if(FLOAT_EQUAL(m_fMaxDXVersion,9.03f))
			return 9.03f;
		// DX9.0b - redist
		else if(FLOAT_EQUAL(m_fMaxDXVersion,9.02f))
			return 9.02f;
		// DX9.0a - redist
		else if(FLOAT_EQUAL(m_fMaxDXVersion,9.01f))
			return 9.01f;
		// DX9.0 - redist + SDK
		else if(FLOAT_EQUAL(m_fMaxDXVersion,9.f))
			return 9.f;
		// Downlevel - should never get here
		else
			return 0.f;
	}
	else if(m_fVersion == 8.f) // DX8 interfaces
	{
		// DX9 and beyond
		if(m_fMaxDXVersion >= 9.f)
			return 8.12f;
		// DX8x - SDK install
		else if(m_fMaxDXVersion < 9.f && m_fMaxDXVersion >= 8.f)
			return m_fVersion;
		// Downlevel - should never get here
		else
			return 0.f;
	}
	else
		return m_fVersion;
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::SetupD3D(float fVersion)
{
	HRESULT hr = S_OK;

	DPF(4, _T("CD3D::SetupVersion()\n"));

	if(m_fVersion != fVersion)
	{
		// Release any existing state
		ReleaseAll();

		// Initialize the function ptrs to this new version
		hr = InitFunctionPtrs(fVersion);
		if(FAILED(hr))
			return hr;
	
		// Set the version
		m_fVersion = fVersion;

		// Get the reference info
		UpdateDLLInfo();

		// Initialize to this new version
		hr = InitD3D();
		if(FAILED(hr))
		{
			m_fVersion = 0.f;
			ReleaseAll();
			return hr;
		}

		//TODO: Investigate differences with display paths on interface <dx8
		//Removed because on dx7 and below the display path is allll wrong
		/*for ( UINT i = 0; i < m_nAdapters; ++i )
		{
			//Get the d3d ordinal
			m_pAdapters[i].nAdapterOrdinal = GetDisplayIDFromAdapterOrdinal( i );
			if ( -1 == m_pAdapters[i].nAdapterOrdinal )
			{
				DPF(1, _T("CD3D::SetupD3D() - Unable to find matching display device.") );
				return E_FAIL;
			}
		}*/
	}

	return hr;
}

// --------------------------------------------------------------------------------------

void CD3D::UpdateDLLInfo()
{
	HMODULE hMod = NULL;
	
	if(m_fVersion >= 9.f && m_fVersion < 10.f)
		m_sReferenceDLL = _T("d3dref9.dll");
	else if(m_fVersion >= 8.f && m_fVersion < 9.f)
		m_sReferenceDLL = _T("d3dref8.dll");
	else if(m_fVersion == 7.f || m_fVersion == 6.f)
		m_sReferenceDLL = _T("d3dref.dll");

	if ( hMod = LoadLibraryEx( m_sReferenceDLL, NULL, 0 ) )
	{
		FreeLibrary(hMod);
		m_bReferenceDetected = true;
	}
	else
	{
		m_bReferenceDetected = false;
	}
}

// --------------------------------------------------------------------------------------

bool CD3D::Initialize()
{
	DPF(4, _T("CD3D::Initialize()\n"));

	// Get the driver model
	m_DriverModel = DetectDriverModel();

	// Get the current D3D version
	if(!DetectDXVersion(&m_fBuild, &m_fMaxDXVersion))
		return false;

	// Get PSGP info
	if(!GetPSGPFromReg())
		return false;

	return true;
}

// --------------------------------------------------------------------------------------

void CD3D::ReleaseAll() 
{
	if(pReleaseD3D)
		ReleaseD3D();

	ReleaseD3DX();

	// Unload the D3D DLL
	if(m_hSrcD3DDLL)
	{
		FreeLibrary(m_hSrcD3DDLL);
		m_hSrcD3DDLL = NULL;
	}
	if(m_hRefD3DDLL)
	{
		FreeLibrary(m_hRefD3DDLL);
		m_hRefD3DDLL = NULL;
	}
	if(m_hDebugD3DDLL)
	{
		FreeLibrary(m_hDebugD3DDLL);
		m_hDebugD3DDLL = NULL;
	}

	if(m_pAdapters)
	{
		// Release all the CD3D structures
		for(int i=0; i<m_nAdapters; i++)
		{
			SAFEDELETEA(m_pAdapters[i].sAdapterName);
			SAFEDELETEA(m_pAdapters[i].sChipName);
			SAFEDELETEA(m_pAdapters[i].sDescription);
			SAFEDELETEA(m_pAdapters[i].sDevicePath);
			SAFEDELETEA(m_pAdapters[i].sChipDAC);
			SAFEDELETEA(m_pAdapters[i].sDriver);
			SAFEDELETEA(m_pAdapters[i].sVersion);
			SAFEDELETEA(m_pAdapters[i].sPnPID);

			if(m_pAdapters[i].pModes)
			{
				delete[] m_pAdapters[i].pModes;
				m_pAdapters[i].pModes = NULL;
			}
		}
		delete[] m_pAdapters;
		m_pAdapters = NULL;
	}
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CreateDevice(UINT uAdapter, DWORD dwDevType, HWND hFocus, DWORD dwBehaviors, PRESENT_PARAMETERS *pParams, CDevice **pDeviceOut)
{
	HRESULT hr = S_OK;
	CDevice *pDevice = NULL;
	int nResult = 0;

	// Check for invalid params
	if(pDeviceOut == NULL)
	{
		DPF(1, _T("CD3D::CreateDevice() - Invalid params.\n"));
		return E_FAIL;
	}

	// Make sure the ref is available
	if((dwDevType == DEVICETYPE_REF) && !IsRefInstalled())
	{
		// Break in the debugger
		if(IsDebuggerPresent())
		{
			DebugOut.Write
			(
				_T( "CD3D::CreateDevice:  Reference driver %s not found.  Please copy it to the app directory before continuing.\n" ),
				GetRefDLL()
			);

			DebugBreak();
		}

		do
		{
			// Give the user a chance to add the DLL to the system
			nResult = g_pFramework->MsgBox(MB_ICONSTOP|MB_RETRYCANCEL|MB_APPLMODAL, _T("Reference driver(%s) not detected.\n"), GetRefDLL());
			// Update the DLL info after the button is pushed
			UpdateDLLInfo();
		}while(nResult == IDRETRY && !IsRefInstalled());

		if(!IsRefInstalled())
		{	
			DPF(1, _T("\nReference driver(%s) not detected.\n"), GetRefDLL());
			return E_ABORT;
		}
	}

	// Allocate device
	pDevice = new CDevice(this, uAdapter, dwDevType, dwBehaviors, hFocus);

	if(pDevice == NULL)
	{
		DPF(1, _T("CD3D::CreateDevice() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	// Check caps if needed
	if(!pDevice->CapsDetected())
	{
		if(!DetectCaps(pDevice))
		{
			DPF(1, _T("CD3D::CreateDevice() - CheckCaps failed.\n"));
			hr = E_FAIL;
			goto Exit;
		}
	}

	// Create the actual D3D device
	if(pParams)
	{
		hr = pDevice->UpdateDevice(pParams);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3D::CreateDevice() - failed to instantiate D3D device. hr = %s(%x)\n"),
				HResultToString(hr), hr);
			goto Exit;
		}
	}

	// Set the out ptr
	*pDeviceOut = pDevice;

Exit:

	if(FAILED(hr))
	{
		RELEASE(pDevice);
	}

	return hr;

}

// --------------------------------------------------------------------------------------

PADAPTER CD3D::GetAdapter(int nAdapter)
{
	// Verify the adapter index.
	if(nAdapter < 0 || nAdapter >= m_nAdapters)
	{
		DPF(1, _T("CD3D::GetAdapter() - Invalid adapter index %d, Current number of adapters %d.\n"), nAdapter, m_nAdapters);
		return NULL;
	}

	// Verify the adapter list pointer.
	if(m_pAdapters == NULL)
	{
		return NULL;
	}

	return &(m_pAdapters[nAdapter]);
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::SetLastError(HRESULT hr)
{
	if(SUCCEEDED(hr))
		return hr;
	else
	{
		ProcessError(hr);
		
		if(SUCCEEDED(m_hrLastError))
		{
			m_hrLastError = hr;
	
		}
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::GetLastError()
{
	HRESULT hr = m_hrLastError;

	m_hrLastError = S_OK;

	return hr;
}

// --------------------------------------------------------------------------------------

bool CD3D::UnRegisterRasterizer()
{

	if(m_hSWRast)
	{
		FreeLibrary(m_hSWRast);
		m_hSWRast = NULL;
	}

	return true;
}


bool CD3D::UpdateAdapterInfo()
{
	HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
	if (hUser32 == NULL )
	{
		return false;
	}

	typedef BOOL (WINAPI *MONITORINFOPROC)(HMONITOR, LPMONITORINFOEX);
	PADAPTER pAdapter = NULL;
	MONITORINFOPROC GetMonitorInfoProc = (MONITORINFOPROC) GetProcAddress
	(
		hUser32,
		_T("GetMonitorInfoA")
	);
	MONITORINFOEX   Info;

	for(UINT nAdapter=0; nAdapter<m_nAdapters; nAdapter++)
	{
		pAdapter = GetAdapter(nAdapter);

		if(NULL == pAdapter)
			continue;

		if (NULL != GetMonitorInfoProc)
		{
			Info.cbSize = sizeof(MONITORINFOEX);
			if (GetMonitorInfoProc(pAdapter->hMonitor,&Info))
				memcpy(&pAdapter->rMonitorRect, &Info.rcMonitor, sizeof(RECT));
			else
			{
				// Need to check the guid of the first screen
				if (NULL == pAdapter->Guid.Data1)
					SetRect(&pAdapter->rMonitorRect, 0, 0, 
						GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
				else
					ZeroMemory(&pAdapter->rMonitorRect, sizeof(RECT));
			}
		}
		else
		{
			// This is not a multi-mon system, or pre-Win98/NT 5.0.
			SetRect(&pAdapter->rMonitorRect, 0, 0, GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
		}
	}

	return true;
}

// --------------------------------------------------------------------------------------

bool CD3D::GetDebugInfoFromReg()
{

	DPF(4, _T("CD3D::GetDebugInfoFromReg()\n"));

	HKEY      	hRegKey;
	DWORD     	dwVal;
	LRESULT   	lr;
	ULONG     	nSize = sizeof(DWORD);
	ULONG     	nValType = REG_DWORD;

	lr = RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Direct3d"),
					 NULL, KEY_QUERY_VALUE, &hRegKey);

	if(lr != ERROR_SUCCESS)
	{
		DPF(1, _T("CD3D::GetDebugInfoFromReg() - Failed to open registry!\n"));
		return false;
	}
	else
	{
		lr = RegQueryValueEx(hRegKey, _T("LoadDebugRuntime"), NULL, &nValType, (BYTE *)&dwVal, &nSize);

		RegCloseKey(hRegKey);

		if(lr == ERROR_SUCCESS && dwVal != 0)
			m_bDebug = true;
		
		return true;
	}
}

// --------------------------------------------------------------------------------------

bool CD3D::IsDisplayModeSupported(PADAPTER pAdapter, UINT nWidth, UINT nHeight, FMT fDisplayFmt)
{
	if(NULL == pAdapter)
		return false;

	for(UINT i=0; i<pAdapter->nModes; i++)
	{
		if(pAdapter->pModes[i].dwWidth == nWidth &&
		   pAdapter->pModes[i].dwHeight == nHeight &&
		   (FMT)pAdapter->pModes[i].Format.d3dfFormat == fDisplayFmt)
		   return true;
	}
	return false;
}

// --------------------------------------------------------------------------------------

DRIVERMODEL DetectDriverModel(void)
{
	//
	// Initialize locals.
	//

	HDC hdc = NULL;

	HMODULE hInst = NULL;
	PFND3DKMT_OPENADAPTERFROMHDC pfnOsThunkDDIOpenAdapterFromHdc = NULL;
	PFND3DKMT_CLOSEADAPTER pfnOsThunkDDICloseAdapter = NULL;

	DISPLAY_DEVICE displayDevice;
	ZeroMemory( &displayDevice, sizeof( displayDevice ) );
	displayDevice.cb = sizeof( displayDevice );

	DRIVERMODEL DriverModel = DRIVERMODEL_XPDM;


	//
	// Enumerate display devices, and create a DC for the primary
	// display.
	//

	for
	(
		DWORD iDevNum = 0;
		NULL == hdc && EnumDisplayDevices( NULL, iDevNum, &displayDevice, 0 );
		++iDevNum
	)
	{
		if
		(
			0 != ( displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) &&
			0 != ( displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE )
		)
		{
			hdc = CreateDC(displayDevice.DeviceName, NULL, NULL, NULL);

			break;
		};
	};

	if ( NULL == hdc )
	{
		hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	}

	if (hdc == NULL)
	{
		goto Cleanup;
	}


	//
	// Load GDI32.
	//

	hInst = LoadLibraryExW( L"gdi32.dll", NULL, 0 );
	if (hInst == NULL)
	{
		goto Cleanup;
	}


	//
	// Find our LDDM thunks.
	//

	pfnOsThunkDDIOpenAdapterFromHdc = (PFND3DKMT_OPENADAPTERFROMHDC) GetProcAddress(hInst, "D3DKMTOpenAdapterFromHdc" );
	if (pfnOsThunkDDIOpenAdapterFromHdc == NULL)
	{
		goto Cleanup;
	}

	pfnOsThunkDDICloseAdapter = (PFND3DKMT_CLOSEADAPTER)GetProcAddress(hInst, "D3DKMTCloseAdapter" );
	if (pfnOsThunkDDICloseAdapter == NULL)
	{
		goto Cleanup;
	}


	//
	// Open the adapter from our HDC.
	//

	D3DKMT_OPENADAPTERFROMHDC OpenAdapterFromHdc;

	OpenAdapterFromHdc.hDc = hdc;
	if ((*pfnOsThunkDDIOpenAdapterFromHdc)(&OpenAdapterFromHdc) != 0)
	{
		goto Cleanup;
	}


	DriverModel = DRIVERMODEL_LDDM;


	//
	// Close the adapter.
	//

	D3DKMT_CLOSEADAPTER CloseAdapter;

	CloseAdapter.hAdapter = OpenAdapterFromHdc.hAdapter;
	(*pfnOsThunkDDICloseAdapter)(&CloseAdapter);


Cleanup:
	if ( hdc != NULL )
	{
		DeleteDC( hdc );
		hdc = NULL;
	};

	if ( hInst != NULL )
	{
		FreeLibrary(hInst);
		hInst = NULL;
	};

	return DriverModel;
}

// --------------------------------------------------------------------------------------

#define STATUS_BUFFER_OVERFLOW ((NTSTATUS)0x80000005L)

bool LDDMApertureCorruption(void)
{
	PFND3DKMT_OPENADAPTERFROMHDC pfnKTOpenAdapterFromHdc = NULL;
	PFND3DKMT_ESCAPE pfnKTEscape = NULL;
    HINSTANCE hInstThunk = NULL;
    D3DKMT_OPENADAPTERFROMHDC OpenAdapterData;
    DISPLAY_DEVICE dd;
    HDC hdc;
    int i;

    memset(&dd, 0, sizeof (dd));
    dd.cb = sizeof dd;

	// Aperture Corruption only valid on LDDM driver model
	if (DetectDriverModel() != DRIVERMODEL_LDDM)
		return false;

    // Get entry points needed
    hInstThunk = LoadLibraryExW( L"gdi32.dll", NULL, 0 );
    if (hInstThunk == NULL)
    {
		DPF(1, _T("LDDMApertureCorruption: Could not load gdi32.dll.\n"));
        return true;
    }

    pfnKTOpenAdapterFromHdc = (PFND3DKMT_OPENADAPTERFROMHDC)GetProcAddress((HMODULE)hInstThunk, "D3DKMTOpenAdapterFromHdc" );
    if (pfnKTOpenAdapterFromHdc == NULL)
    {
		DPF(1, _T("LDDMApertureCorruption: Failed to get D3DKMTOpenAdapterFromHdc() address.\n"));
		FreeLibrary(hInstThunk);
        return true;
    }

    pfnKTEscape = (PFND3DKMT_ESCAPE)GetProcAddress((HMODULE)hInstThunk, "D3DKMTEscape" );
    if (pfnKTEscape == NULL)
    {
		DPF(1, _T("LDDMApertureCorruption: Failed to get D3DKMTEscape() address.\n"));
		FreeLibrary(hInstThunk);
        return true;
    }

    for (i = 0; EnumDisplayDevicesA(NULL, i, &dd, NULL); ++i)
    {
        hdc = CreateDC (NULL, dd.DeviceName, NULL, NULL);
        if (hdc == NULL)
        {
            continue;
        }

        OpenAdapterData.hDc = hdc;
        if (NT_SUCCESS((*pfnKTOpenAdapterFromHdc)(&OpenAdapterData)))
        {   
            D3DKMT_ESCAPE   sEsc;
            
            NTSTATUS        ntStatus;
            D3DKMT_VIDMM_ESCAPE VidMMEscape;

            memset(&sEsc, 0, sizeof(sEsc));
            memset(&VidMMEscape, 0, sizeof(VidMMEscape));

            sEsc.hAdapter=OpenAdapterData.hAdapter;
            sEsc.hDevice=(D3DKMT_HANDLE)NULL;
            sEsc.Type=D3DKMT_ESCAPE_VIDMM;
            sEsc.pPrivateDriverData = &VidMMEscape;
            sEsc.PrivateDriverDataSize = sizeof(VidMMEscape);

            VidMMEscape.Type = D3DKMT_VIDMMESCAPETYPE_APERTURE_CORRUPTION_CHECK;

            ntStatus = pfnKTEscape(&sEsc);

			if (ntStatus == STATUS_BUFFER_OVERFLOW)
            {
                // Corruption found
				FreeLibrary(hInstThunk);
				DeleteDC(hdc);
                return true;
            }           
        }        

		DeleteDC(hdc);
    }

    // Corruption NOT found
	FreeLibrary(hInstThunk);
    return false;
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::RegisterSoftwareDeviceUnsup(void* pInitializeFunction)
{
	DPF(1, _T("CD3D::RegisterSoftwareDevice unsupported on this interface!\n"));
	return E_FAIL;
}
UINT CD3D::GetAdapterCountUnsup(void)
{
	DPF(1, _T("CD3D::GetAdapterCount unsupported on this interface!\n"));
	return (UINT)0;
}
HRESULT CD3D::GetAdapterIdentifierUnsup(UINT Adapter,DWORD Flags,ADAPTER_IDENTIFIER* pIdentifier)
{
	DPF(1, _T("CD3D::GetAdapterIdentifier unsupported on this interface!\n"));
	return E_FAIL;
}
UINT CD3D::GetAdapterModeCountUnsup(UINT Adapter)
{
	DPF(1, _T("CD3D::GetAdapterModeCount unsupported on this interface!\n"));
	return (UINT)0;
}
UINT CD3D::GetAdapterModeCountExUnsup( UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter )
{
	DPF(1, _T("CD3D::GetAdapterModeCountEx unsupported on this interface!\n"));
	return (UINT)0;
}
HRESULT CD3D::EnumAdapterModesUnsup(UINT Adapter,UINT Mode,DISPLAYMODE* pMode)
{
	DPF(1, _T("CD3D::EnumAdapterModes unsupported on this interface!\n"));
	return E_FAIL;
}
HRESULT CD3D::EnumAdapterModesUnsup9(UINT Adapter, FORMAT Format, UINT Mode,DISPLAYMODE* pMode)
{
	DPF(1, _T("CD3D::EnumAdapterModes(UINT, FORMAT, UINT, DISPLAYMODE*) unsupported on this interface!\n"));
	return E_FAIL;
}
HRESULT CD3D::EnumAdapterModesExUnsup( UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter,UINT Mode, D3DDISPLAYMODEEX* pMode )
{
	DPF(1, _T("CD3D::EnumAdapterModesEx( UINT, D3DDISPLAYMODEFILTER*, UINT, DISPLAYMODEEX* ) unsupported on this interface!\n"));
	return E_FAIL;
}
HRESULT CD3D::GetAdapterDisplayModeUnsup(UINT Adapter,DISPLAYMODE* pMode)
{
	DPF(1, _T("CD3D::GetAdapterDisplayMode unsupported on this interface!\n"));
	return E_FAIL;
}
HRESULT CD3D::CheckDeviceTypeUnsup(UINT Adapter,DWORD CheckType,FORMAT DisplayFormat,FORMAT BackBufferFormat,BOOL Windowed)
{
	DPF(1, _T("CD3D::CheckDeviceType unsupported on this interface!\n"));
	return E_FAIL;
}
HRESULT CD3D::CheckDeviceFormatUnsup(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,DWORD Usage,RESOURCETYPE RType,FORMAT CheckFormat)
{
	DPF(1, _T("CD3D::CheckDeviceFormat unsupported on this interface!\n"));
	return E_FAIL;
}
HRESULT CD3D::CheckDeviceMultiSampleTypeUnsup(UINT Adapter,DWORD DeviceType,FORMAT SurfaceFormat,BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType)
{
	DPF(1, _T("CD3D::CheckDeviceMultiSampleType unsupported on this interface!\n"));
	return E_FAIL;
}
HRESULT CD3D::CheckDeviceMultiSampleType9Unsup(UINT Adapter,DWORD DeviceType,FORMAT SurfaceFormat,BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType, DWORD *dwQualityLevels)
{
	DPF(1, _T("CD3D::CheckDeviceMultiSampleType unsupported on this interface!\n"));
	return E_FAIL;
}
HRESULT CD3D::CheckDepthStencilMatchUnsup(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,FORMAT RenderTargetFormat,FORMAT DepthStencilFormat)
{
	DPF(1, _T("CD3D::CheckDepthStencilMatch unsupported on this interface!\n"));
	return E_FAIL;
}
HRESULT CD3D::GetDeviceCapsUnsup(UINT Adapter,DWORD DeviceType,CAPS* pCaps)
{
	DPF(1, _T("CD3D::GetDeviceCaps unsupported on this interface!\n"));
	return E_FAIL;
}
HMONITOR CD3D::GetAdapterMonitorUnsup(UINT Adapter)
{
	DPF(1, _T("CD3D::GetAdapterMonitor unsupported on this interface!\n"));
	return (HMONITOR)0;
}
HRESULT CD3D::EvictManagedTexturesUnsup()
{
	DPF(1, _T("CD3D::CreatePalette unsupported on this interface!\n"));
	return E_FAIL;
}
HRESULT CD3D::CreateDDrawPaletteUnsup(DWORD dwFlags, LPPALETTEENTRY lpDDColorArray, LPDIRECTDRAWPALETTE *lplpDDPalette)
{
	DPF(1, _T("CD3D::CreatePalette unsupported on this interface!\n"));
	return E_FAIL;
}

HRESULT CD3D::CheckDeviceFormatConversionUnsup(UINT Adapter,DWORD DeviceType,FORMAT SourceFormat,FORMAT TargetFormat)
{
	DPF(1, _T("CD3D::CheckDeviceFormatConversion unsupported on this interface!\n"));
	return E_FAIL;
}

UINT CD3D::GetDisplayIDFromAdapterOrdinal( UINT nAdapter )
{
	UINT uRet = -1;
	DISPLAY_DEVICE ddev;

	memset( &ddev, 0, sizeof(DISPLAY_DEVICE) );
	ddev.cb = sizeof(DISPLAY_DEVICE);

	for ( UINT i = 0; -1 == uRet && EnumDisplayDevicesA(NULL, i, &ddev, NULL); ++i )
	{		
		if ( !_tcscmp( ddev.DeviceName, m_pAdapters[nAdapter].sDevicePath ) )
		{
			uRet = i;
		}
	}

	return uRet;
}

// --------------------------------------------------------------------------------------
// D3DX Stub library - Loads D3DX dynamically.  See CD3DX class for actual stub functions.
//					   This code only initializes the function ptrs that the stub uses
//					   based on the current D3D version.
// --------------------------------------------------------------------------------------

HINSTANCE				g_hD3DX	= NULL;				// Handle to the D3DX dll
float					g_fD3DX = 0.f;				// D3DX version number

D3DXVEC2NORMALIZE                         pD3DXVec2Normalize;
D3DXVEC2HERMITE                           pD3DXVec2Hermite;
D3DXVEC2CATMULLROM                        pD3DXVec2CatmullRom;
D3DXVEC2BARYCENTRIC                       pD3DXVec2BaryCentric;
D3DXVEC2TRANSFORM                         pD3DXVec2Transform;
D3DXVEC2TRANSFORMCOORD                    pD3DXVec2TransformCoord;
D3DXVEC2TRANSFORMNORMAL                   pD3DXVec2TransformNormal;
D3DXVEC3NORMALIZE                         pD3DXVec3Normalize;
D3DXVEC3HERMITE                           pD3DXVec3Hermite;
D3DXVEC3CATMULLROM                        pD3DXVec3CatmullRom;
D3DXVEC3BARYCENTRIC                       pD3DXVec3BaryCentric;
D3DXVEC3TRANSFORM                         pD3DXVec3Transform;
D3DXVEC3TRANSFORMCOORD                    pD3DXVec3TransformCoord;
D3DXVEC3TRANSFORMNORMAL                   pD3DXVec3TransformNormal;
D3DXVEC3PROJECT                           pD3DXVec3Project;
D3DXVEC3UNPROJECT                         pD3DXVec3Unproject;
D3DXVEC4CROSS                             pD3DXVec4Cross;
D3DXVEC4NORMALIZE                         pD3DXVec4Normalize;
D3DXVEC4HERMITE                           pD3DXVec4Hermite;
D3DXVEC4CATMULLROM                        pD3DXVec4CatmullRom;
D3DXVEC4BARYCENTRIC                       pD3DXVec4BaryCentric;
D3DXVEC4TRANSFORM                         pD3DXVec4Transform;
D3DXMATRIXDETERMINANT                     pD3DXMatrixDeterminant;
D3DXMATRIXTRANSPOSE                       pD3DXMatrixTranspose;
D3DXMATRIXMULTIPLY                        pD3DXMatrixMultiply;
D3DXMATRIXMULTIPLYTRANSPOSE               pD3DXMatrixMultiplyTranspose;
D3DXMATRIXINVERSE                         pD3DXMatrixInverse;
D3DXMATRIXSCALING                         pD3DXMatrixScaling;
D3DXMATRIXTRANSLATION                     pD3DXMatrixTranslation;
D3DXMATRIXROTATIONX                       pD3DXMatrixRotationX;
D3DXMATRIXROTATIONY                       pD3DXMatrixRotationY;
D3DXMATRIXROTATIONZ                       pD3DXMatrixRotationZ;
D3DXMATRIXROTATIONAXIS                    pD3DXMatrixRotationAxis;
D3DXMATRIXROTATIONQUATERNION              pD3DXMatrixRotationQuaternion;
D3DXMATRIXROTATIONYAWPITCHROLL            pD3DXMatrixRotationYawPitchRoll;
D3DXMATRIXTRANSFORMATION                  pD3DXMatrixTransformation;
D3DXMATRIXAFFINETRANSFORMATION            pD3DXMatrixAffineTransformation;
D3DXMATRIXLOOKATRH                        pD3DXMatrixLookAtRH;
D3DXMATRIXLOOKATLH                        pD3DXMatrixLookAtLH;
D3DXMATRIXPERSPECTIVERH                   pD3DXMatrixPerspectiveRH;
D3DXMATRIXPERSPECTIVELH                   pD3DXMatrixPerspectiveLH;
D3DXMATRIXPERSPECTIVEFOVRH                pD3DXMatrixPerspectiveFovRH;
D3DXMATRIXPERSPECTIVEFOVLH                pD3DXMatrixPerspectiveFovLH;
D3DXMATRIXPERSPECTIVEOFFCENTERRH          pD3DXMatrixPerspectiveOffCenterRH;
D3DXMATRIXPERSPECTIVEOFFCENTERLH          pD3DXMatrixPerspectiveOffCenterLH;
D3DXMATRIXORTHORH                         pD3DXMatrixOrthoRH;
D3DXMATRIXORTHOLH                         pD3DXMatrixOrthoLH;
D3DXMATRIXORTHOOFFCENTERRH                pD3DXMatrixOrthoOffCenterRH;
D3DXMATRIXORTHOOFFCENTERLH                pD3DXMatrixOrthoOffCenterLH;
D3DXMATRIXSHADOW                          pD3DXMatrixShadow;
D3DXMATRIXREFLECT                         pD3DXMatrixReflect;
D3DXQUATERNIONTOAXISANGLE                 pD3DXQuaternionToAxisAngle;
D3DXQUATERNIONROTATIONMATRIX              pD3DXQuaternionRotationMatrix;
D3DXQUATERNIONROTATIONAXIS                pD3DXQuaternionRotationAxis;
D3DXQUATERNIONROTATIONYAWPITCHROLL        pD3DXQuaternionRotationYawPitchRoll;
D3DXQUATERNIONMULTIPLY                    pD3DXQuaternionMultiply;
D3DXQUATERNIONNORMALIZE                   pD3DXQuaternionNormalize;
D3DXQUATERNIONINVERSE                     pD3DXQuaternionInverse;
D3DXQUATERNIONLN                          pD3DXQuaternionLn;
D3DXQUATERNIONEXP                         pD3DXQuaternionExp;
D3DXQUATERNIONSLERP                       pD3DXQuaternionSlerp;
D3DXQUATERNIONSQUAD                       pD3DXQuaternionSquad;
D3DXQUATERNIONSQUADSETUP                  pD3DXQuaternionSquadSetup;
D3DXQUATERNIONBARYCENTRIC                 pD3DXQuaternionBaryCentric;
D3DXPLANENORMALIZE                        pD3DXPlaneNormalize;
D3DXPLANEINTERSECTLINE                    pD3DXPlaneIntersectLine;
D3DXPLANEFROMPOINTNORMAL                  pD3DXPlaneFromPointNormal;
D3DXPLANEFROMPOINTS                       pD3DXPlaneFromPoints;
D3DXPLANETRANSFORM                        pD3DXPlaneTransform;
D3DXCOLORADJUSTSATURATION                 pD3DXColorAdjustSaturation;
D3DXCOLORADJUSTCONTRAST                   pD3DXColorAdjustContrast;
D3DXASSEMBLESHADER9                       pD3DXAssembleShader9;
D3DXASSEMBLESHADER8                       pD3DXAssembleShader8;
D3DXCREATEEFFECTFROMFILEA                 pD3DXCreateEffectFromFileA;
D3DXCREATEEFFECTFROMRESOURCEA             pD3DXCreateEffectFromResourceA;
D3DXCOMPILESHADER						  pD3DXCompileShader;
D3DXDEBUGMUTE							  pD3DXDebugMute;

D3DXCHECKVERSION						  pD3DXCheckVersion;
D3DXCREATEFONTINDIRECTA					  pD3DXCreateFontIndirect;
D3DXCREATEMESH                            pD3DXCreateMesh;
D3DXCREATEMESHFVF                         pD3DXCreateMeshFVF;
D3DXGENERATEOUTPUTDECL					  pD3DXGenerateOutputDecl;
D3DXRECTPATCHSIZE						  pD3DXRectPatchSize;
D3DXTRIPATCHSIZE						  pD3DXTriPatchSize;
D3DXTESSELLATERECTPATCH					  pD3DXTessellateRectPatch;
D3DXTESSELLATETRIPATCH					  pD3DXTessellateTriPatch;
D3DXCREATETEXTUREFROMFILEINMEMORY         pD3DXCreateTextureFromFileInMemory;
D3DXCREATETEXTURE                         pD3DXCreateTexture;
D3DXCREATETEXTUREFROMFILEEXA              pD3DXCreateTextureFromFileExA;
D3DXLOADSURFACEFROMFILEINMEMORY           pD3DXLoadSurfaceFromFileInMemory;
D3DXLOADSURFACEFROMSURFACE                pD3DXLoadSurfaceFromSurface;
D3DXLOADVOLUMEFROMVOLUME                  pD3DXLoadVolumeFromVolume;
D3DXLOADVOLUMEFROMMEMORY                  pD3DXLoadVolumeFromMemory;
D3DXFILTERTEXTURE                         pD3DXFilterTexture;
D3DXFILLTEXTURE                           pD3DXFillTexture;
D3DXCREATETEXTUREFROMFILEINMEMORYEX       pD3DXCreateTextureFromFileInMemoryEx;
D3DXLOADSURFACEFROMFILEA                  pD3DXLoadSurfaceFromFileA;
D3DXSAVESURFACETOFILEA                    pD3DXSaveSurfaceToFileA;
D3DXCREATETEXTUREFROMRESOURCEA            pD3DXCreateTextureFromResourceA;
D3DXLOADSURFACEFROMMEMORY                 pD3DXLoadSurfaceFromMemory;
D3DXSAVETEXTURETOFILEA                    pD3DXSaveTextureToFileA;
D3DXTESSELLATENPATCHES                    pD3DXTessellateNPatches;
D3DXCREATETEXTUREFROMFILEA                pD3DXCreateTextureFromFileA;
D3DXCREATETEXTUREFROMRESOURCEEXA          pD3DXCreateTextureFromResourceExA;

// --------------------------------------------------------------------------------------

HRESULT InitD3DX(float fVersion)
{
	TCHAR *sDLL = NULL;
	TCHAR sBuffer[MAX_PATH] = {0};
	int nResult = 0;

	// Quick out if already set to that version
	if(fVersion == floor(g_fD3DX))
		return S_OK;

	// Release all previos interfaces
	ReleaseD3DX();

	if(fVersion >= 9.f && fVersion <= 10.f)
	{
		// Need to handle different D3DX naming schemes.
		WIN32_FIND_DATA FileData;
		HANDLE hFile = NULL;
		TCHAR *sFilePattern = _T("\\d3dx9*_*.dll");
		bool bDefault = true;

		// Try the apps local directory first
		GetModuleFileName(NULL, sBuffer, MAX_PATH);
		PathRemoveFileSpec(sBuffer);
		StringCchCat
		(
			sBuffer,
			MAX_PATH,
			sFilePattern
		);

		hFile = FindFirstFile(sBuffer, &FileData);
		if(hFile != INVALID_HANDLE_VALUE)
			bDefault = false;

		if (bDefault)
		{
			// Not in apps directory, so try system directory
			GetSystemDirectory(sBuffer, MAX_PATH);
			StringCchCat
			(
				sBuffer,
				MAX_PATH,
				sFilePattern
			);

			hFile = FindFirstFile(sBuffer, &FileData);
			if(hFile != INVALID_HANDLE_VALUE)
				bDefault = false;
		}

		if (bDefault)
		{
			// Didn't find a match, use our default d3dx dll
			sDLL = 	_T("d3dx9d.dll");
		}
		else
		{
			UINT nMaxVersion = 0;
			do
			{
				TCHAR *sStart = _tcsrchr(FileData.cFileName, _T('_'));
				TCHAR *sEnd = _tcschr(sStart, _T('.'));
				*sEnd = _T('\0');
				UINT nVersion = _ttoi(sStart+1);
				*sEnd = _T('.');

				if(nVersion >= nMaxVersion)
				{
					nMaxVersion = nVersion;
					StringCchCopy(sBuffer, MAX_PATH, FileData.cFileName);
				}
			}
			while(FindNextFile(hFile, &FileData));
			
			sDLL = sBuffer;
			FindClose(hFile);
		}
	}
	else
	{
		sDLL = _T("d3dx8d.dll");
	}

	// Make sure the DLL is available
	g_hD3DX = LoadLibraryEx( sDLL, NULL, 0 );
	if ( NULL == g_hD3DX )
	{
		// Break into the user-mode debugger if present.
		if(IsDebuggerPresent())
		{
			DebugOut.Write
			(
				_T( "InitD3DX(%f):  Could not find D3DX DLL %s.  Please copy it to the app directory before continuing.\n" ),
				fVersion,
				sDLL
			);

			DebugBreak();
		}

		do
		{
			// Give the user a chance to add the DLL to the system
			nResult = g_pFramework->MsgBox(MB_ICONSTOP|MB_RETRYCANCEL|MB_APPLMODAL, _T("D3DX(%s) not detected.\n"), sDLL);
			// Update the DLL info after the button is pushed
			g_hD3DX = LoadLibraryEx( sDLL, NULL, 0 );
		}while(nResult == IDRETRY && (NULL == g_hD3DX));

		if(NULL == g_hD3DX)
		{
			DPF(1, _T("D3DX(%s) not detected.  This is required for all DX%f tests.\n"), sDLL, fVersion);  		
			return E_ABORT;
		}
	}

	if(fVersion >= 9.f && fVersion <= 10.f)
	{
		if(GetProcAddress(g_hD3DX, "D3DXComputeTangentFrameEx") != NULL)
			g_fD3DX = 9.3f;
		else if(GetProcAddress(g_hD3DX, "D3DXDisassembleEffect") != NULL)
			g_fD3DX = 9.2f;
		else if(GetProcAddress(g_hD3DX, "D3DXSHEvalDirection") != NULL)
			g_fD3DX = 9.1f;
		else
			g_fD3DX = 9.0f;
	}
	else
		g_fD3DX = fVersion;

#define INIT_FNPOINTER(hModule, fnName, fnPtr, fnType) \
	fnPtr = (fnType)GetProcAddress(hModule, #fnName);\
	if(NULL == fnPtr)\
	{\
		DPF(1,_T("InitD3DX - GetProcAddress() failed on %s.\n"),#fnName);\
	}
   																	   
	INIT_FNPOINTER(g_hD3DX, D3DXCreateMesh,                            pD3DXCreateMesh, D3DXCREATEMESH);
	INIT_FNPOINTER(g_hD3DX, D3DXCreateMeshFVF,                         pD3DXCreateMeshFVF,D3DXCREATEMESHFVF);
	INIT_FNPOINTER(g_hD3DX, D3DXTessellateNPatches,                    pD3DXTessellateNPatches,D3DXTESSELLATENPATCHES);
	INIT_FNPOINTER(g_hD3DX, D3DXLoadSurfaceFromFileA,                  pD3DXLoadSurfaceFromFileA,D3DXLOADSURFACEFROMFILEA);
	INIT_FNPOINTER(g_hD3DX, D3DXLoadSurfaceFromFileInMemory,           pD3DXLoadSurfaceFromFileInMemory,D3DXLOADSURFACEFROMFILEINMEMORY);
	INIT_FNPOINTER(g_hD3DX, D3DXLoadSurfaceFromSurface,                pD3DXLoadSurfaceFromSurface,D3DXLOADSURFACEFROMSURFACE);
	INIT_FNPOINTER(g_hD3DX, D3DXLoadSurfaceFromMemory,                 pD3DXLoadSurfaceFromMemory,D3DXLOADSURFACEFROMMEMORY);
	INIT_FNPOINTER(g_hD3DX, D3DXSaveSurfaceToFileA,                    pD3DXSaveSurfaceToFileA,D3DXSAVESURFACETOFILEA);
	INIT_FNPOINTER(g_hD3DX, D3DXLoadVolumeFromVolume,                  pD3DXLoadVolumeFromVolume,D3DXLOADVOLUMEFROMVOLUME);
	INIT_FNPOINTER(g_hD3DX, D3DXLoadVolumeFromMemory,                  pD3DXLoadVolumeFromMemory,D3DXLOADVOLUMEFROMMEMORY);
	INIT_FNPOINTER(g_hD3DX, D3DXCreateTexture,                         pD3DXCreateTexture,D3DXCREATETEXTURE);
	INIT_FNPOINTER(g_hD3DX, D3DXCreateTextureFromFileA,                pD3DXCreateTextureFromFileA,D3DXCREATETEXTUREFROMFILEA);
	INIT_FNPOINTER(g_hD3DX, D3DXCreateTextureFromResourceA,            pD3DXCreateTextureFromResourceA,D3DXCREATETEXTUREFROMRESOURCEA);
	INIT_FNPOINTER(g_hD3DX, D3DXCreateTextureFromFileExA,              pD3DXCreateTextureFromFileExA,D3DXCREATETEXTUREFROMFILEEXA);
	INIT_FNPOINTER(g_hD3DX, D3DXCreateTextureFromResourceExA,          pD3DXCreateTextureFromResourceExA,D3DXCREATETEXTUREFROMRESOURCEEXA);
	INIT_FNPOINTER(g_hD3DX, D3DXCreateTextureFromFileInMemory,         pD3DXCreateTextureFromFileInMemory,D3DXCREATETEXTUREFROMFILEINMEMORY);
	INIT_FNPOINTER(g_hD3DX, D3DXCreateTextureFromFileInMemoryEx,       pD3DXCreateTextureFromFileInMemoryEx,D3DXCREATETEXTUREFROMFILEINMEMORYEX);
	INIT_FNPOINTER(g_hD3DX, D3DXSaveTextureToFileA,                    pD3DXSaveTextureToFileA,D3DXSAVETEXTURETOFILEA);
	INIT_FNPOINTER(g_hD3DX, D3DXFilterTexture,                         pD3DXFilterTexture,D3DXFILTERTEXTURE);
	INIT_FNPOINTER(g_hD3DX, D3DXFillTexture,                           pD3DXFillTexture,D3DXFILLTEXTURE);

	INIT_FNPOINTER(g_hD3DX, D3DXVec2Normalize,                         pD3DXVec2Normalize,D3DXVEC2NORMALIZE);
	INIT_FNPOINTER(g_hD3DX, D3DXVec2Hermite,                           pD3DXVec2Hermite,D3DXVEC2HERMITE);
	INIT_FNPOINTER(g_hD3DX, D3DXVec2CatmullRom,                        pD3DXVec2CatmullRom,D3DXVEC2CATMULLROM);
	INIT_FNPOINTER(g_hD3DX, D3DXVec2BaryCentric,                       pD3DXVec2BaryCentric,D3DXVEC2BARYCENTRIC);
	INIT_FNPOINTER(g_hD3DX, D3DXVec2Transform,                         pD3DXVec2Transform,D3DXVEC2TRANSFORM);
	INIT_FNPOINTER(g_hD3DX, D3DXVec2TransformCoord,                    pD3DXVec2TransformCoord,D3DXVEC2TRANSFORMCOORD);
	INIT_FNPOINTER(g_hD3DX, D3DXVec2TransformNormal,                   pD3DXVec2TransformNormal,D3DXVEC2TRANSFORMNORMAL);
	INIT_FNPOINTER(g_hD3DX, D3DXVec3Normalize,                         pD3DXVec3Normalize,D3DXVEC3NORMALIZE);
	INIT_FNPOINTER(g_hD3DX, D3DXVec3Hermite,                           pD3DXVec3Hermite,D3DXVEC3HERMITE);
	INIT_FNPOINTER(g_hD3DX, D3DXVec3CatmullRom,                        pD3DXVec3CatmullRom,D3DXVEC3CATMULLROM);
	INIT_FNPOINTER(g_hD3DX, D3DXVec3BaryCentric,                       pD3DXVec3BaryCentric,D3DXVEC3BARYCENTRIC);
	INIT_FNPOINTER(g_hD3DX, D3DXVec3Transform,                         pD3DXVec3Transform,D3DXVEC3TRANSFORM);
	INIT_FNPOINTER(g_hD3DX, D3DXVec3TransformCoord,                    pD3DXVec3TransformCoord,D3DXVEC3TRANSFORMCOORD);
	INIT_FNPOINTER(g_hD3DX, D3DXVec3TransformNormal,                   pD3DXVec3TransformNormal,D3DXVEC3TRANSFORMNORMAL);
	INIT_FNPOINTER(g_hD3DX, D3DXVec3Project,                           pD3DXVec3Project,D3DXVEC3PROJECT);
	INIT_FNPOINTER(g_hD3DX, D3DXVec3Unproject,                         pD3DXVec3Unproject,D3DXVEC3UNPROJECT);
	INIT_FNPOINTER(g_hD3DX, D3DXVec4Cross,                             pD3DXVec4Cross,D3DXVEC4CROSS);
	INIT_FNPOINTER(g_hD3DX, D3DXVec4Normalize,                         pD3DXVec4Normalize,D3DXVEC4NORMALIZE);
	INIT_FNPOINTER(g_hD3DX, D3DXVec4Hermite,                           pD3DXVec4Hermite,D3DXVEC4HERMITE);
	INIT_FNPOINTER(g_hD3DX, D3DXVec4CatmullRom,                        pD3DXVec4CatmullRom,D3DXVEC4CATMULLROM);
	INIT_FNPOINTER(g_hD3DX, D3DXVec4BaryCentric,                       pD3DXVec4BaryCentric,D3DXVEC4BARYCENTRIC);
	INIT_FNPOINTER(g_hD3DX, D3DXVec4Transform,                         pD3DXVec4Transform,D3DXVEC4TRANSFORM);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixTranspose,                       pD3DXMatrixTranspose,D3DXMATRIXTRANSPOSE);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixMultiply,                        pD3DXMatrixMultiply,D3DXMATRIXMULTIPLY);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixMultiplyTranspose,               pD3DXMatrixMultiplyTranspose,D3DXMATRIXMULTIPLYTRANSPOSE);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixInverse,                         pD3DXMatrixInverse,D3DXMATRIXINVERSE);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixScaling,                         pD3DXMatrixScaling,D3DXMATRIXSCALING);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixTranslation,                     pD3DXMatrixTranslation,D3DXMATRIXTRANSLATION);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixRotationX,                       pD3DXMatrixRotationX,D3DXMATRIXROTATIONX);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixRotationY,                       pD3DXMatrixRotationY,D3DXMATRIXROTATIONY);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixRotationZ,                       pD3DXMatrixRotationZ,D3DXMATRIXROTATIONZ);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixRotationAxis,                    pD3DXMatrixRotationAxis,D3DXMATRIXROTATIONAXIS);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixRotationQuaternion,              pD3DXMatrixRotationQuaternion,D3DXMATRIXROTATIONQUATERNION);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixRotationYawPitchRoll,            pD3DXMatrixRotationYawPitchRoll,D3DXMATRIXROTATIONYAWPITCHROLL);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixTransformation,                  pD3DXMatrixTransformation,D3DXMATRIXTRANSFORMATION);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixAffineTransformation,            pD3DXMatrixAffineTransformation,D3DXMATRIXAFFINETRANSFORMATION);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixLookAtRH,                        pD3DXMatrixLookAtRH,D3DXMATRIXLOOKATRH);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixLookAtLH,                        pD3DXMatrixLookAtLH,D3DXMATRIXLOOKATLH);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixPerspectiveRH,                   pD3DXMatrixPerspectiveRH,D3DXMATRIXPERSPECTIVERH);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixPerspectiveLH,                   pD3DXMatrixPerspectiveLH,D3DXMATRIXPERSPECTIVELH);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixPerspectiveFovRH,                pD3DXMatrixPerspectiveFovRH,D3DXMATRIXPERSPECTIVEFOVRH);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixPerspectiveFovLH,                pD3DXMatrixPerspectiveFovLH,D3DXMATRIXPERSPECTIVEFOVLH);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixPerspectiveOffCenterRH,          pD3DXMatrixPerspectiveOffCenterRH,D3DXMATRIXPERSPECTIVEOFFCENTERRH);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixPerspectiveOffCenterLH,          pD3DXMatrixPerspectiveOffCenterLH,D3DXMATRIXPERSPECTIVEOFFCENTERLH);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixOrthoRH,                         pD3DXMatrixOrthoRH,D3DXMATRIXORTHORH);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixOrthoLH,                         pD3DXMatrixOrthoLH,D3DXMATRIXORTHOLH);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixOrthoOffCenterRH,                pD3DXMatrixOrthoOffCenterRH,D3DXMATRIXORTHOOFFCENTERRH);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixOrthoOffCenterLH,                pD3DXMatrixOrthoOffCenterLH,D3DXMATRIXORTHOOFFCENTERLH);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixShadow,                          pD3DXMatrixShadow,D3DXMATRIXSHADOW);
	INIT_FNPOINTER(g_hD3DX, D3DXMatrixReflect,                         pD3DXMatrixReflect,D3DXMATRIXREFLECT);
	INIT_FNPOINTER(g_hD3DX, D3DXQuaternionToAxisAngle,                 pD3DXQuaternionToAxisAngle,D3DXQUATERNIONTOAXISANGLE);
	INIT_FNPOINTER(g_hD3DX, D3DXQuaternionRotationMatrix,              pD3DXQuaternionRotationMatrix,D3DXQUATERNIONROTATIONMATRIX);
	INIT_FNPOINTER(g_hD3DX, D3DXQuaternionRotationAxis,                pD3DXQuaternionRotationAxis,D3DXQUATERNIONROTATIONAXIS);
	INIT_FNPOINTER(g_hD3DX, D3DXQuaternionRotationYawPitchRoll,        pD3DXQuaternionRotationYawPitchRoll,D3DXQUATERNIONROTATIONYAWPITCHROLL);
	INIT_FNPOINTER(g_hD3DX, D3DXQuaternionMultiply,                    pD3DXQuaternionMultiply,D3DXQUATERNIONMULTIPLY);
	INIT_FNPOINTER(g_hD3DX, D3DXQuaternionNormalize,                   pD3DXQuaternionNormalize,D3DXQUATERNIONNORMALIZE);
	INIT_FNPOINTER(g_hD3DX, D3DXQuaternionInverse,                     pD3DXQuaternionInverse,D3DXQUATERNIONINVERSE);
	INIT_FNPOINTER(g_hD3DX, D3DXQuaternionLn,                          pD3DXQuaternionLn,D3DXQUATERNIONLN);
	INIT_FNPOINTER(g_hD3DX, D3DXQuaternionExp,                         pD3DXQuaternionExp,D3DXQUATERNIONEXP);
	INIT_FNPOINTER(g_hD3DX, D3DXQuaternionSlerp,                       pD3DXQuaternionSlerp,D3DXQUATERNIONSLERP);
	INIT_FNPOINTER(g_hD3DX, D3DXQuaternionSquad,                       pD3DXQuaternionSquad,D3DXQUATERNIONSQUAD);
	INIT_FNPOINTER(g_hD3DX, D3DXQuaternionSquadSetup,                  pD3DXQuaternionSquadSetup,D3DXQUATERNIONSQUADSETUP);
	INIT_FNPOINTER(g_hD3DX, D3DXQuaternionBaryCentric,                 pD3DXQuaternionBaryCentric,D3DXQUATERNIONBARYCENTRIC);
	INIT_FNPOINTER(g_hD3DX, D3DXPlaneNormalize,                        pD3DXPlaneNormalize,D3DXPLANENORMALIZE);
	INIT_FNPOINTER(g_hD3DX, D3DXPlaneIntersectLine,                    pD3DXPlaneIntersectLine,D3DXPLANEINTERSECTLINE);
	INIT_FNPOINTER(g_hD3DX, D3DXPlaneFromPointNormal,                  pD3DXPlaneFromPointNormal,D3DXPLANEFROMPOINTNORMAL);
	INIT_FNPOINTER(g_hD3DX, D3DXPlaneFromPoints,                       pD3DXPlaneFromPoints,D3DXPLANEFROMPOINTS);
	INIT_FNPOINTER(g_hD3DX, D3DXPlaneTransform,                        pD3DXPlaneTransform,D3DXPLANETRANSFORM);
	INIT_FNPOINTER(g_hD3DX, D3DXColorAdjustSaturation,                 pD3DXColorAdjustSaturation,D3DXCOLORADJUSTSATURATION);
	INIT_FNPOINTER(g_hD3DX, D3DXColorAdjustContrast,                   pD3DXColorAdjustContrast,D3DXCOLORADJUSTCONTRAST);
																						  
	if(g_fD3DX >= 9.f && g_fD3DX < 10.f)
	{
		INIT_FNPOINTER(g_hD3DX, D3DXCheckVersion,						   pD3DXCheckVersion, D3DXCHECKVERSION);
		INIT_FNPOINTER(g_hD3DX, D3DXAssembleShader,						   pD3DXAssembleShader9, D3DXASSEMBLESHADER9);
		INIT_FNPOINTER(g_hD3DX, D3DXMatrixDeterminant,                     pD3DXMatrixDeterminant,D3DXMATRIXDETERMINANT);
		INIT_FNPOINTER(g_hD3DX, D3DXGenerateOutputDecl,					   pD3DXGenerateOutputDecl,D3DXGENERATEOUTPUTDECL);
		INIT_FNPOINTER(g_hD3DX, D3DXRectPatchSize,						   pD3DXRectPatchSize,D3DXRECTPATCHSIZE);
		INIT_FNPOINTER(g_hD3DX, D3DXTriPatchSize,						   pD3DXTriPatchSize,D3DXTRIPATCHSIZE);
		INIT_FNPOINTER(g_hD3DX, D3DXTessellateRectPatch,                   pD3DXTessellateRectPatch,D3DXTESSELLATERECTPATCH);
		INIT_FNPOINTER(g_hD3DX, D3DXTessellateTriPatch,                    pD3DXTessellateTriPatch,D3DXTESSELLATETRIPATCH);
		INIT_FNPOINTER(g_hD3DX, D3DXCreateEffectFromFileA,                 pD3DXCreateEffectFromFileA,D3DXCREATEEFFECTFROMFILEA);
		INIT_FNPOINTER(g_hD3DX, D3DXCreateEffectFromResourceA,             pD3DXCreateEffectFromResourceA,D3DXCREATEEFFECTFROMRESOURCEA);
		INIT_FNPOINTER(g_hD3DX, D3DXCompileShader,						   pD3DXCompileShader,D3DXCOMPILESHADER);

		if( g_fD3DX > 9.29f )
		{
			INIT_FNPOINTER(g_hD3DX, D3DXDebugMute,		                   pD3DXDebugMute, D3DXDEBUGMUTE);
		}

		// Handle differences between 9.0 and 9.1
		if(g_fD3DX > 9.0)
		{
			INIT_FNPOINTER(g_hD3DX, D3DXCreateFontIndirectA,               pD3DXCreateFontIndirect, D3DXCREATEFONTINDIRECTA);
		}
		else
		{
			INIT_FNPOINTER(g_hD3DX, D3DXCreateFontIndirect,                pD3DXCreateFontIndirect, D3DXCREATEFONTINDIRECTA);
		}
	}
	else if(g_fD3DX >= 8.f && g_fD3DX < 9.f)
	{
		INIT_FNPOINTER(g_hD3DX, D3DXAssembleShader,						   pD3DXAssembleShader8, D3DXASSEMBLESHADER8);
		INIT_FNPOINTER(g_hD3DX, D3DXMatrixfDeterminant,                    pD3DXMatrixDeterminant,D3DXMATRIXDETERMINANT);
		INIT_FNPOINTER(g_hD3DX, D3DXCreateFontIndirect,                    pD3DXCreateFontIndirect, D3DXCREATEFONTINDIRECTA);
	}

	return S_OK;
}

// --------------------------------------------------------------------------------------
										   	
void ReleaseD3DX()						   	
{										   	
	if(NULL != g_hD3DX)
	{
		FreeLibrary(g_hD3DX);
		g_fD3DX = 0.f;
	}
}

// --------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

HRESULT WINAPI D3DXAssembleShader8(LPCVOID pSrcData, UINT SrcDataLen, DWORD dwFlags, LPD3DXBUFFER* ppConstants, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrors)
{
	ASSERT( pD3DXAssembleShader8 != NULL );
	if (!pD3DXAssembleShader8)
	{
		return HRESULT_FROM_WIN32( ERROR_PROC_NOT_FOUND );
	};

	return (*pD3DXAssembleShader8)(pSrcData, SrcDataLen, dwFlags, ppConstants, ppShader, ppErrors);
}

HRESULT WINAPI D3DXAssembleShader9(LPCVOID pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrors)
{
	ASSERT( pD3DXAssembleShader9 != NULL );
	if (!pD3DXAssembleShader9)
	{
		return HRESULT_FROM_WIN32( ERROR_PROC_NOT_FOUND );
	};

	return (*pD3DXAssembleShader9)(pSrcData, SrcDataLen, pDefines, pInclude, dwFlags, ppShader, ppErrors);
}


#ifdef __cplusplus
}
#endif
