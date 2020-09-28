// ======================================================================================
//
//  CD3D7.cpp
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================

#include "D3DPCH.h"

#include <assert.h>

#include "Framework.h"
#include "CSurface.h"
#include "D3D.h"

#include <strsafe.h>

typedef HRESULT (WINAPI * DDCREATEEX)(GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter);

FMT g_BackBufferFormats7[] = {
	FMT_X8R8G8B8,
	FMT_R5G6B5
};
static DWORD g_dwBackBufferFormats7 = sizeof(g_BackBufferFormats7) / sizeof(FMT);

// --------------------------------------------------------------------------------------

HRESULT CD3D::InitD3D7()
{
	int i = 0;
	HRESULT hr = E_FAIL;
	LPDIRECTDRAWENUMERATEEXA DirectDrawEnumerateEx = 0;
	DDCREATEEX  pProc = NULL;
	

	m_hSrcD3DDLL = LoadLibraryExW( L"DDRAW.DLL", NULL, 0 );
	if(m_hSrcD3DDLL == NULL)
	{
		DPF(1, _T("CD3D::InitD3D7() - Failed to load DDraw.DLL.\n"));
		return E_FAIL;
	}
	

	DirectDrawEnumerateEx = (LPDIRECTDRAWENUMERATEEXA)GetProcAddress(m_hSrcD3DDLL, _T("DirectDrawEnumerateExA"));
	pProc = (DDCREATEEX) GetProcAddress(m_hSrcD3DDLL,"DirectDrawCreateEx");
	if(!DirectDrawEnumerateEx || !pProc)
	{
		DPF(1, _T("CD3D::InitD3D7 -  Couldn't get proc address for DirectDrawEnumerateExA"));
		return E_FAIL;
	}		  

	// Get the runtime type
	m_bDebug = IsFileDebug(L"DDraw.dll");
	
	// Allocate the ADAPTERs
	int nAdapters = g_pFramework->GetAdapterCount();
	m_nAdapters = 0;
	m_pAdapters = new ADAPTER[nAdapters];
	if(m_pAdapters == NULL)
	{
		DPF(1, _T("CD3D::InitD3D7() - Out of memory.\n"));
		goto Error;
	}
	memset(m_pAdapters, 0, sizeof(ADAPTER) * nAdapters);

	// Allocate the DISPLAY Modes, we'll grow later if needed.
	for(i=0; i<nAdapters; i++)
	{
		m_pAdapters[i].m_nDisplyModesAlloc = m_nDefaultDisplyModesAlloc;
		m_pAdapters[i].pModes = new DISPLAYMODE[m_pAdapters[i].m_nDisplyModesAlloc];
		if(m_pAdapters[i].pModes == NULL)
		{
			DPF(1, _T("CD3D::InitD3D7() - Out of memory.\n"));
			goto Error;
		}
		memset(m_pAdapters[i].pModes, 0, sizeof(DISPLAYMODE) * m_pAdapters[i].m_nDisplyModesAlloc);
	}

	// Enum display devices
	hr = DirectDrawEnumerateEx(DirectDrawEnumCallbackEx7, this, DDENUM_ATTACHEDSECONDARYDEVICES);
	if(FAILED(hr))
	{
		DPF(1, _T("CD3D: Could not enumerate display devices, hr=%s(%x)\n"),
					   HResultToString(hr), hr);
		goto Error;
	}

	UpdateAdapterInfo();
	
	// If the number of devices is greater than 1, we are running
	// on a multi-mon system. Chop off the primary because it enumerated twice.
	if(m_nAdapters > 1)
	{
		// Swap the adapter positions if necessary
		for(i = 1; i < m_nAdapters; i++)
		{
			MONITORINFO Mon;
			Mon.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(m_pAdapters[i].hMonitor, &Mon);

			if(Mon.dwFlags & MONITORINFOF_PRIMARY)
			{
				ADAPTER Adapt;
				memcpy(&Adapt, m_pAdapters, sizeof(ADAPTER));
				memcpy(m_pAdapters, &m_pAdapters[i], sizeof(ADAPTER));
				memcpy(&m_pAdapters[i], &Adapt, sizeof(ADAPTER));
				break;
			}
		}
	}
	
	COPYSTRING(m_sSWRast, _T("RGB"));

	if(NULL == m_sSWRast)
	{
		DPF(1, _T("CD3D::InitD3D7() - Out of memory.\n"));
		goto Error;
	}


Error:
	return hr;
}

// --------------------------------------------------------------------------------------

bool CD3D::ResolveBehaviors7
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
		DPF(1, _T("CD3D::ResolveBehaviors7() - invalid pointer.\n"));
		return false;
	};


	bool bOverallResult = false;
	DWORD ResolvedBehaviors = fBehaviors;
	HRESULT hr;


	DDCREATEEX  pProc = (DDCREATEEX) GetProcAddress(GetModuleHandle("DDRAW.DLL"),"DirectDrawCreateEx");
	if(pProc == NULL)
	{
		DPF(1, _T("CD3D::ResolveBehaviors7() - GetProcAddress() failed for DirectDrawCreateEx.\n"));
		goto Cleanup;
	}

	// Release any lingering interfaces
	ReleaseD3D();
	
	// Create a DirectDraw object
	hr = (*pProc)
	(
		(GUID *) DDCREATE_HARDWAREONLY,
		(void**) &m_pDDraw7,
		IID_IDirectDraw7,
		NULL
	);
	if(FAILED(hr))
	{
		DPF(1, _T("CD3D::ResolveBehaviors7() - Failed to query DDraw object. hr=%s(%x).\n"),
			HResultToString(hr), hr);
		goto Cleanup;
	}
	
	// Initialize the D3D object
	hr = m_pDDraw7->QueryInterface
	(
		IID_IDirect3D7,
		(void **)&m_pD3D7
	);
	if (FAILED(hr))
	{
		DPF(1, _T("CD3D::ResolveBehaviors7() - QI of IDirect3D7 from IDirectDraw7 failed.\n"));
		goto Cleanup;
	}


	//
	// Decode custom framework behavior flags.
	//
	// N.B. [SEdmison]:
	// Essentially, we're resolving *this-or-that* aliases to
	// the particular *this* or *that*, so that what we return
	// to the framework will be an unambiguous set of behavior
	// flags.
	//

	if
	(
		(DEVICECREATE_HWVP_OR_PURE & ResolvedBehaviors) != 0
	)
	{
		ResolvedBehaviors &= ~DEVICECREATE_HWVP_OR_PURE;

		ResolvedBehaviors |= DEVICECREATE_STANDARD;
		ResolvedBehaviors |= DEVICECREATE_HWVP;
	};


	if
	(
		(DEVICECREATE_SW_OR_HW_VP & ResolvedBehaviors) != 0 ||
		(DEVICECREATE_BESTAVAILABLE & ResolvedBehaviors) != 0
	)
	{
		ResolvedBehaviors &= ~DEVICECREATE_SW_OR_HW_VP;
		ResolvedBehaviors &= ~DEVICECREATE_BESTAVAILABLE;

		ResolvedBehaviors |= DEVICECREATE_STANDARD;

		// Determine whether we have support for hardware vertex
		// processing.
		ADAPTER FakeAdapter;
		ASSERT( m_pDDraw7 != NULL );
		ASSERT( m_pD3D7 != NULL );
		HRESULT hr = m_pD3D7->EnumDevices(EnumAdapterInfo7, &FakeAdapter);
		if (FAILED(hr))
		{
			DPF(1, _T("CD3D::ResolveBehaviors7() - Failed to enumerate devices."));
			goto Cleanup;
		};

		if ( FakeAdapter.bHardwareVP == true )
		{
			ResolvedBehaviors |= DEVICECREATE_HWVP;
		}
		else
		{
			ResolvedBehaviors |= DEVICECREATE_SWVP;
			ResolvedBehaviors &= ~DEVICECREATE_HWVP;
			ResolvedBehaviors &= ~DEVICECREATE_MIXED;
		};
	}


	// Force SWVP for SW/REF.
	if ( (DeviceType & DEVICETYPE_HAL) == 0 )
	{
		ResolvedBehaviors |= DEVICECREATE_SWVP;
		ResolvedBehaviors &= ~DEVICECREATE_HWVP;
		ResolvedBehaviors &= ~DEVICECREATE_MIXED;
	}


	// Make sure that there's some sort of device subtype.
	if ( (ResolvedBehaviors & 0xF00) == 0 )
	{
		ResolvedBehaviors |= DEVICECREATE_STANDARD;
	};


	//
	// Fill in our outgoing parameter.
	//

	*pfResolvedBehaviors = ResolvedBehaviors;


	bOverallResult = true;


Cleanup:
	RELEASE(m_pD3D7);
	RELEASE(m_pDDraw7);

	return bOverallResult;
}

// --------------------------------------------------------------------------------------

bool CD3D::DetectCaps7(CDevice* pDevice)
{
	HRESULT		hr;
	DDCREATEEX pProc = (DDCREATEEX) GetProcAddress(GetModuleHandle("DDRAW.DLL"),"DirectDrawCreateEx");
	
	if(pDevice == NULL || pProc == NULL)
	{
		DPF(1, _T("CD3D::DetectCaps7() - invalid pointer.\n"));
		return false;
	}

	// Clear the caps structure
	ZeroMemory(&(pDevice->m_Desc), sizeof(pDevice->m_Desc));

	// Get the adapter for this device
	int nAdapter = pDevice->GetAdapterID();
	ADAPTER *pAdapter = pDevice->GetAdapter();
	if(pAdapter == NULL)
	{
		DPF(1, _T("CD3D::DetectCaps7() - invalid adapter ID(#%d).\n"), nAdapter);
		return false;
	}

	// Create the DDraw object for this adapter
	hr = pProc(&pAdapter->Guid, (PVOID*)&m_pDDraw7, IID_IDirectDraw7, NULL);
	if (FAILED(hr))
	{
		DPF(1, _T("CD3D::DetectCaps7() - Failed to create DirectDraw device for %s, hr=%s(%x)\n"), 
					   pAdapter->sDescription, HResultToString(hr), hr);
		return false;
	}

	// Set a default cooperative level
	hr = m_pDDraw7->SetCooperativeLevel(GetActiveWindow(),DDSCL_NORMAL);
	if (FAILED(hr))
	{
		DPF(1, _T("CD3D::DetectCaps7() - Error setting coopertive level on %s, hr=%s(%x)\n"), 
					   pAdapter->sDescription, HResultToString(hr), hr);
		return false;
	}

	// Enumerate Direct3D devices
	hr = m_pDDraw7->QueryInterface(IID_IDirect3D7, (PVOID*)&m_pD3D7);
	if (FAILED(hr))
	{
		DPF(1, _T("CD3D::DetectCaps7() - Could not query Direct3D interface on %s, hr=%s(%x)\n"), 
					   pAdapter->sDescription, HResultToString(hr), hr);
		return false;
	}

	// Get the DDRAW caps
	m_pDDraw7->GetCaps((DDCAPS*)&m_ddCaps, NULL);

	// Query the caps for this device
	hr = m_pD3D7->EnumDevices(EnumD3DDevices7, pDevice);
	if (FAILED(hr))
	{
		DPF(1, _T("CD3D::DetectCaps7() - Error enumerating Direct3D devices on %s, hr=%s(%x)\n"), 
					   pAdapter->sDescription, HResultToString(hr), hr);
		return false;
	}

	// Create the device description
	pDevice->UpdateDescription();

	RELEASE(m_pDDraw7);
	RELEASE(m_pD3D7);

	return true;
}

// --------------------------------------------------------------------------------------

void CD3D::ReleaseD3D7()
{
	DWORD dwRef = 0;

	RELEASE(m_pPrimary);
	RELEASE(m_pD3D7);

	if(m_pDDraw7)
	{
		if(dwRef = m_pDDraw7->Release())
		{
			DPF(1, _T("CD3D::ReleaseD3D7() - m_pDDraw ref count nonzero(%d).  TestApp leaking D3D objects.\n"), dwRef);

			ASSERT(g_pFramework != NULL);
			if(g_pFramework->m_bStressMode)
			{
				ASSERT(dwRef == 0);
			}
		}
		m_pDDraw7 = NULL;
	}
}

// --------------------------------------------------------------------------------------

void CD3D::ProcessError7(HRESULT hr)
{
	// Only set this flag - it'll get reset when the device recovers.
	if(hr == DDERR_OUTOFMEMORY)
		m_bOutofMemory = true;

	if(hr == DDERR_SURFACELOST)
		m_bDeviceLost = true;
}

// --------------------------------------------------------------------------------------

bool CD3D::CompareFormats7(FORMAT *pSrcFmt, FORMAT *pRefFmt)
{
	if(!pSrcFmt || !pRefFmt)
	{
		DPF(1, _T("CD3D::CompareFormats7() - Invalid parameters.\n"));
		return false;
	}

	return pSrcFmt->d3dfFormat == pRefFmt->d3dfFormat;
}

// --------------------------------------------------------------------------------------

BOOL WINAPI CD3D::DirectDrawEnumCallbackEx7
(
	__in LPGUID pGuid,
	__in LPSTR szDescription, 
	__in LPSTR szName,
	__in PVOID pContext,
	HMONITOR hMonitor
)
{
	CD3D * pD3D = (CD3D*) pContext;
	if ( pContext == NULL )
	{
		DPF( 1, _T( "CD3D::DirectDrawEnumCallbackEx7() - pContext is NULL.\n" ) );

		return DDENUMRET_CANCEL;
	}


	assert( g_pFramework != NULL );


	// If we've already discovered all the adapters that we're expecting...
	if ( pD3D->m_nAdapters >= g_pFramework->GetAdapterCount() )
	{
		// ...then stop enumerating.
		return DDENUMRET_CANCEL;
	}

	assert( pD3D->m_pAdapters != NULL );
	if ( pD3D->m_pAdapters == NULL )
	{
		DPF( 1, _T( "CD3D::DirectDrawEnumCallbackEx7() - m_pAdapters is NULL.\n" ) );

		return DDENUMRET_CANCEL;
	}

	ADAPTER * pAdapter = &pD3D->m_pAdapters[pD3D->m_nAdapters];
	assert( pAdapter != NULL );

	HRESULT hr = E_FAIL;
	DDSURFACEDESC2 Desc;
	DDDEVICEIDENTIFIER2 Id;

	DDCREATEEX pProc = (DDCREATEEX) GetProcAddress
	(
		GetModuleHandle("DDRAW.DLL"),
		"DirectDrawCreateEx"
	);

	if (NULL == pProc)
	{
		DPF(1, _T("CD3D::DirectDrawEnumCallbackEx7() - GetProcAddress on DirectDrawCreateEx failed.\n"));

		return DDENUMRET_CANCEL;
	}

	// On single monitor machines, primary CAN be enum'd twice, but isn't always enum'd twice, so
	// we always grab the NULL Guid primary for that case.  If multimon, we can just skip the NULL
	// Guid primary.
	if(g_pFramework->GetAdapterCount() == 1)
	{
		if(pGuid != NULL)
		{
			return DDENUMRET_OK;
		}
	}
	else
	{
		if(pGuid == NULL)
		{
			return DDENUMRET_OK;
		}
	}

	// Copy information to the device structure
	if(pGuid != NULL)
	{
		memcpy(&pAdapter->Guid, pGuid, sizeof(GUID));
	}
	else
	{
		ZeroMemory(&pAdapter->Guid,sizeof(GUID));
	}

	pAdapter->hMonitor = hMonitor;

	assert( pAdapter->sDescription == NULL );
	assert( szDescription != NULL );
	COPYSTRING(pAdapter->sDescription, szDescription);
	if (NULL == pAdapter->sDescription)
	{
		DPF(1, _T("CD3D::DirectDrawEnumCallbackEx7() - Out of memory,\n"));

		return DDENUMRET_CANCEL;
	}

	// Create DirectDraw device
	hr = pProc(pGuid, (PVOID*)&pD3D->m_pDDraw7, IID_IDirectDraw7, NULL);
	if (FAILED(hr))
	{
		DPF(1, _T("CD3D: Failed to create DirectDraw device for %s, hr=%s(%x)\n"), 
					   szDescription, pD3D->HResultToString(hr), hr);

		return DDENUMRET_OK;
	}

	// Set a default cooperative level
	hr = pD3D->m_pDDraw7->SetCooperativeLevel(GetActiveWindow(),DDSCL_NORMAL);
	if (FAILED(hr))
	{
		DPF(1, _T("CD3D: Error setting coopertive level on %s, hr=%s(%x)\n"), 
					   pAdapter->sDescription, pD3D->HResultToString(hr), hr);

		return DDENUMRET_OK;
	}

	// Get the device infor	
	hr = pD3D->m_pDDraw7->GetDeviceIdentifier(&Id, 0);
	if(!FAILED(hr))
	{
		// create the version string.
		{
			TCHAR tcsVer[20] = {0};
			memset( tcsVer, 0, sizeof tcsVer );
	
			StringCchPrintf(tcsVer, ARRAYSIZE(tcsVer) - 1, _T("%d.%d.%d.%d\0"),
				HIWORD(Id.liDriverVersion.HighPart), // product
				LOWORD(Id.liDriverVersion.HighPart), // version
				HIWORD(Id.liDriverVersion.LowPart),  // sub-version
				LOWORD(Id.liDriverVersion.LowPart)); // build

			assert( pAdapter->sVersion == NULL );	
			COPYSTRING(pAdapter->sVersion, tcsVer);
		}
	
		// get the adapter name
		assert( pAdapter->sAdapterName == NULL );
		COPYSTRING(pAdapter->sAdapterName, Id.szDescription);

		assert( pAdapter->sDevicePath == NULL );
		COPYSTRING(pAdapter->sDevicePath, szName);
	
		// get the driver name
		assert( pAdapter->sDriver == NULL );
		COPYSTRING(pAdapter->sDriver, Id.szDriver);
	
		// the vendor and device ID
		pAdapter->dwVendorID = Id.dwVendorId;
		pAdapter->dwChipID = Id.dwDeviceId;
	
		// create PnP ID string
		char szBuffer[48];
		StringCchPrintf
		(
			szBuffer,
			ARRAYSIZE(szBuffer),
			_T("VEN_%04X&DEV_%04X&SUBSYS_%08X&REV_%02X"),
			Id.dwVendorId,
			Id.dwDeviceId,
			Id.dwSubSysId,
			Id.dwRevision
		);

		assert( pAdapter->sPnPID == NULL );
		COPYSTRING(pAdapter->sPnPID, szBuffer);

		if
		(
			NULL == pAdapter->sVersion ||
			NULL == pAdapter->sAdapterName ||
			NULL == pAdapter->sDriver ||
			NULL == pAdapter->sPnPID ||
			NULL == pAdapter->sDevicePath
		)
		{
			DPF(1, _T("CD3D::DirectDrawEnumCallbackEx7() - Out of memory,\n"));

			return DDENUMRET_CANCEL;
		}
	}

	// Get the current desktop format
	memset(&Desc, 0, sizeof(DDSURFACEDESC2));
	Desc.dwSize = sizeof(DDSURFACEDESC2);
	hr = pD3D->m_pDDraw7->GetDisplayMode(&Desc);
	if(FAILED(hr))
	{
		DPF(1, _T("CD3D: GetDisplayMode failed on %s. hr=%s(%x).\n"),pAdapter->sDescription,
			pD3D->HResultToString(hr), hr);

		return DDENUMRET_OK;
	}

	// Fill in the desktop format struct
	pAdapter->DesktopMode.dwWidth = Desc.dwWidth;
	pAdapter->DesktopMode.dwHeight = Desc.dwHeight;
	pAdapter->DesktopMode.dwRefreshRate = Desc.dwRefreshRate;
	pAdapter->DesktopMode.dwBPP = Desc.ddpfPixelFormat.dwRGBBitCount;

	memcpy(&(pAdapter->DesktopMode.Format.ddpfPixelFormat), &(Desc.ddpfPixelFormat), sizeof(DDPIXELFORMAT));
	DDPFToD3DFmt(&(Desc.ddpfPixelFormat), (FMT*)&(pAdapter->DesktopMode.Format.d3dfFormat), NULL);

	// Enumerate display modes
	hr = pD3D->m_pDDraw7->EnumDisplayModes(DDEDM_REFRESHRATES, NULL, pContext, EnumDisplayModesCallback7);
	if (FAILED(hr))
	{
		DPF(1, _T("CD3D: Failed to enumerate display modes on %s, hr=%s(%x)\n"), 
					   pAdapter->sDescription, pD3D->HResultToString(hr), hr);

		return DDENUMRET_OK;
	}

	// Enumerate Direct3D devices
	hr = pD3D->m_pDDraw7->QueryInterface(IID_IDirect3D7, (PVOID*)&pD3D->m_pD3D7);
	if (FAILED(hr))
	{
		DPF(1, _T("CD3D: Could not query Direct3D interface on %s, hr=%s(%x)\n"), 
					   pAdapter->sDescription, pD3D->HResultToString(hr), hr);

		return DDENUMRET_OK;
	}

	// Query adapter caps(HWVP, etc.) needed for device selection
	hr = pD3D->m_pD3D7->EnumDevices(EnumAdapterInfo7, pAdapter);
	if (FAILED(hr))
	{
		DPF(1, _T("CD3D: Error enumerating Direct3D devices on %s, hr=%s(%x)\n"), 
					   pAdapter->sDescription, pD3D->HResultToString(hr), hr);

		return DDENUMRET_OK;
	}

	RELEASE(pD3D->m_pD3D7);
	RELEASE(pD3D->m_pDDraw7);

	pD3D->m_nAdapters++;

	return DDENUMRET_OK;
}


HRESULT WINAPI CD3D::EnumDisplayModesCallback7(LPDDSURFACEDESC2 pDesc, PVOID pContext)
{
	CD3D   *pThis = (CD3D*)pContext;
	DISPLAYMODE *pMode = NULL;

	// Grow the list as needed
	if(pThis->m_pAdapters[pThis->m_nAdapters].nModes >= pThis->m_pAdapters[pThis->m_nAdapters].m_nDisplyModesAlloc)
	{
		UINT nNewCount = pThis->m_pAdapters[pThis->m_nAdapters].m_nDisplyModesAlloc + pThis->m_nDefaultDisplyModesAlloc;
		PDISPLAYMODE pNewList = new DISPLAYMODE[nNewCount];
		if(NULL == pNewList)
		{
			DPF(1, _T("CD3D::EnumDisplayModesCallback7() - Out of memory.\n"));
			return D3DENUMRET_CANCEL;
		}
		// Copy the new size info
		memset(pNewList, 0, sizeof(DISPLAYMODE) * nNewCount);
		memcpy(pNewList, pThis->m_pAdapters[pThis->m_nAdapters].pModes, sizeof(DISPLAYMODE) * pThis->m_pAdapters[pThis->m_nAdapters].m_nDisplyModesAlloc);
		pThis->m_pAdapters[pThis->m_nAdapters].m_nDisplyModesAlloc = nNewCount;
		// Delete the old and use the new
		SAFEDELETEA(pThis->m_pAdapters[pThis->m_nAdapters].pModes);
		pThis->m_pAdapters[pThis->m_nAdapters].pModes = pNewList;
	}

	// Get the pointer to the next displaymode
	pMode = &pThis->m_pAdapters[pThis->m_nAdapters].pModes[pThis->m_pAdapters[pThis->m_nAdapters].nModes];

	// Record all the RGB display modes for this device
	pMode->dwWidth = pDesc->dwWidth;
	pMode->dwHeight = pDesc->dwHeight;
	pMode->dwBPP = pDesc->ddpfPixelFormat.dwRGBBitCount;
	pMode->dwRefreshRate = pDesc->dwRefreshRate;
	memcpy(&(pMode->Format.ddpfPixelFormat), &(pDesc->ddpfPixelFormat), sizeof(DDPIXELFORMAT));
	DDPFToD3DFmt((LPDDPIXELFORMAT)&(pMode->Format.ddpfPixelFormat), (FMT*)&(pMode->Format.d3dfFormat), NULL);
	pThis->m_pAdapters[pThis->m_nAdapters].nModes++;

	return DDENUMRET_OK;
}

HRESULT WINAPI CD3D::EnumAdapterInfo7(__in LPTSTR szDesc, __in LPTSTR szDevice, __in LPD3DDEVICEDESC7 pDev, __in LPVOID pContext)
{
	ADAPTER		*pAdapter = (ADAPTER*)pContext;
	
	if(pAdapter == NULL || pDev == NULL)
	{
		DPF(1, _T("CD3D::EnumD3DDevices7() - invalid params.\n"));
		return D3DENUMRET_CANCEL;
	}

	if(IID_IDirect3DTnLHalDevice == pDev->deviceGUID)
	   pAdapter->bHardwareVP = true;

	return D3DENUMRET_OK;
}

HRESULT WINAPI CD3D::EnumD3DDevices7(__in LPTSTR szDesc, __in LPTSTR szDevice, __in LPD3DDEVICEDESC7 pDev, __in LPVOID pContext)
{
	HRESULT     hr;
	CDevice     *pDevice = (CDevice*)pContext;
	CD3D		*pD3D = NULL;
	ADAPTER		*pAdapter = NULL;


	if(pDevice == NULL || pDev == NULL || (pAdapter = pDevice->GetAdapter()) == NULL || (pD3D = pDevice->GetD3D()) == NULL)
	{
		DPF(1, _T("CD3D::EnumD3DDevices7() - invalid params.\n"));
		return D3DENUMRET_CANCEL;
	}

	// The framework does not support the Null Device
	if( pDev->deviceGUID == IID_IDirect3DNullDevice )
		return D3DENUMRET_OK;

	// WORKAROUND(ROBALD) - On DX6/DX7, the SW devices(REF & RGB) do not correctly report all caps.  Some things
	// that they technically could do are still restricted by what the HW can do(RenderBitDepth for ex).  As a
	// result, we are going to use HW caps in some cases instead of those reported by the device itself.  To do
	// this, we actually need to spin through all enumerated devices when initializing a SW device as the following
	// code does.
	if( pDevice->m_dwDevType != DEVICETYPE_HAL )
	{
		if( (pDev->deviceGUID == IID_IDirect3DTnLHalDevice) ||
			(pDev->deviceGUID == IID_IDirect3DRefDevice && (pDevice->m_dwDevType != DEVICETYPE_REF)) ||
			(pDev->deviceGUID == IID_IDirect3DRGBDevice && (pDevice->m_dwDevType != DEVICETYPE_SW)) )
			return D3DENUMRET_OK;
	}
	else 
	{
		if( (pDev->deviceGUID == IID_IDirect3DHALDevice && (!(pDevice->m_dwBehaviors & DEVICECREATE_SWVP) || (pDevice->m_dwDevType != DEVICETYPE_HAL))) ||
			(pDev->deviceGUID == IID_IDirect3DTnLHalDevice && (!(pDevice->m_dwBehaviors & DEVICECREATE_HWVP) || (pDevice->m_dwDevType != DEVICETYPE_HAL))) ||
			(pDev->deviceGUID == IID_IDirect3DRefDevice && (!(pDevice->m_dwBehaviors & DEVICECREATE_SWVP) || (pDevice->m_dwDevType != DEVICETYPE_REF))) ||
			(pDev->deviceGUID == IID_IDirect3DRGBDevice && (!(pDevice->m_dwBehaviors & DEVICECREATE_SWVP) || (pDevice->m_dwDevType != DEVICETYPE_SW))) )
			return D3DENUMRET_OK;
	}


	if(pDevice->m_dwDevType != DEVICETYPE_HAL && pDev->deviceGUID == IID_IDirect3DHALDevice)
	{
		pDevice->m_Desc.dwDeviceRenderBitDepth = pDev->dwDeviceRenderBitDepth;
		
		// Decode the flag if ANY display mode is requested
		if(pAdapter->DesktopMode.Format.d3dfFormat == FMT_ANY)
		{
			if(pDevice->m_Desc.dwDeviceRenderBitDepth & DDBD_32 &&
				pD3D->IsDisplayModeSupported(pAdapter, pAdapter->DesktopMode.dwWidth, pAdapter->DesktopMode.dwHeight, FMT_X8R8G8B8))
				pAdapter->DesktopMode.Format.d3dfFormat = (D3DFORMAT)FMT_X8R8G8B8;
			else if(pDevice->m_Desc.dwDeviceRenderBitDepth & DDBD_16 &&
				pD3D->IsDisplayModeSupported(pAdapter, pAdapter->DesktopMode.dwWidth, pAdapter->DesktopMode.dwHeight, FMT_R5G6B5))
				pAdapter->DesktopMode.Format.d3dfFormat = (D3DFORMAT)FMT_R5G6B5;
			else
			{
				DPF(1, _T("D3D Device is not hardware accelerated in any supported display modes.\n"));
				return D3DENUMRET_CANCEL;
			}
		}
		d3df2ddpf( pAdapter->DesktopMode.Format.d3dfFormat, &pAdapter->DesktopMode.Format.ddpfPixelFormat);
		pAdapter->DesktopMode.dwBPP = pAdapter->DesktopMode.Format.ddpfPixelFormat.dwRGBBitCount;
	}
	else
	{
		// Check DDI level
		if(pDev->dwDevCaps & D3DDEVCAPS_DRAWPRIMITIVES2EX)
			pDevice->m_nDDILevel = 7;
		else if(pDev->dwDevCaps & D3DDEVCAPS_DRAWPRIMITIVES2)
			pDevice->m_nDDILevel = 6;
		else if(pDev->dwDevCaps & D3DDEVCAPS_DRAWPRIMTLVERTEX)
			pDevice->m_nDDILevel = 5;
		else if(pDev->dwDevCaps & D3DDEVCAPS_FLOATTLVERTEX)
			pDevice->m_nDDILevel = 3;
		
		// Save device description
		memcpy(&pDevice->m_Guid, &pDev->deviceGUID, sizeof(GUID));
		pDevice->m_Desc.dwDevCaps = pDev->dwDevCaps;
		pDevice->m_Desc.dpcTriCaps.dwMiscCaps = pDev->dpcTriCaps.dwMiscCaps;
		pDevice->m_Desc.dpcTriCaps.dwRasterCaps = pDev->dpcTriCaps.dwRasterCaps;
		pDevice->m_Desc.dpcTriCaps.dwZCmpCaps = pDev->dpcTriCaps.dwZCmpCaps;
		pDevice->m_Desc.dpcTriCaps.dwSrcBlendCaps = pDev->dpcTriCaps.dwSrcBlendCaps;
		pDevice->m_Desc.dpcTriCaps.dwDestBlendCaps = pDev->dpcTriCaps.dwDestBlendCaps;
		pDevice->m_Desc.dpcTriCaps.dwAlphaCmpCaps = pDev->dpcTriCaps.dwAlphaCmpCaps;
		pDevice->m_Desc.dpcTriCaps.dwShadeCaps = pDev->dpcTriCaps.dwShadeCaps;
		pDevice->m_Desc.dpcTriCaps.dwTextureCaps = pDev->dpcTriCaps.dwTextureCaps;
		pDevice->m_Desc.dpcTriCaps.dwTextureFilterCaps = pDev->dpcTriCaps.dwTextureFilterCaps;
		pDevice->m_Desc.dpcTriCaps.dwTextureBlendCaps = pDev->dpcTriCaps.dwTextureBlendCaps;
		pDevice->m_Desc.dpcTriCaps.dwTextureAddressCaps = pDev->dpcTriCaps.dwTextureAddressCaps;
		pDevice->m_Desc.dpcTriCaps.dwStippleWidth = pDev->dpcTriCaps.dwStippleWidth;
		pDevice->m_Desc.dpcTriCaps.dwStippleHeight = pDev->dpcTriCaps.dwStippleHeight;
		pDevice->m_Desc.dpcLineCaps.dwMiscCaps = pDev->dpcLineCaps.dwMiscCaps;
		pDevice->m_Desc.dpcLineCaps.dwRasterCaps = pDev->dpcLineCaps.dwRasterCaps;
		pDevice->m_Desc.dpcLineCaps.dwZCmpCaps = pDev->dpcLineCaps.dwZCmpCaps;
		pDevice->m_Desc.dpcLineCaps.dwSrcBlendCaps = pDev->dpcLineCaps.dwSrcBlendCaps;
		pDevice->m_Desc.dpcLineCaps.dwDestBlendCaps = pDev->dpcLineCaps.dwDestBlendCaps;
		pDevice->m_Desc.dpcLineCaps.dwAlphaCmpCaps = pDev->dpcLineCaps.dwAlphaCmpCaps;
		pDevice->m_Desc.dpcLineCaps.dwShadeCaps = pDev->dpcLineCaps.dwShadeCaps;
		pDevice->m_Desc.dpcLineCaps.dwTextureCaps = pDev->dpcLineCaps.dwTextureCaps;
		pDevice->m_Desc.dpcLineCaps.dwTextureFilterCaps = pDev->dpcLineCaps.dwTextureFilterCaps;
		pDevice->m_Desc.dpcLineCaps.dwTextureBlendCaps = pDev->dpcLineCaps.dwTextureBlendCaps;
		pDevice->m_Desc.dpcLineCaps.dwTextureAddressCaps = pDev->dpcLineCaps.dwTextureAddressCaps;
		pDevice->m_Desc.dpcLineCaps.dwStippleWidth = pDev->dpcLineCaps.dwStippleWidth;
		pDevice->m_Desc.dpcLineCaps.dwStippleHeight = pDev->dpcLineCaps.dwStippleHeight;
		pDevice->m_Desc.dwDeviceZBufferBitDepth = pDev->dwDeviceZBufferBitDepth;
		if(pDevice->m_dwDevType == DEVICETYPE_HAL)
		{
			pDevice->m_Desc.dwDeviceRenderBitDepth = pDev->dwDeviceRenderBitDepth;

			// Decode the flag if ANY display mode is requested
			if(pAdapter->DesktopMode.Format.d3dfFormat == FMT_ANY)
			{
				if(pDevice->m_Desc.dwDeviceRenderBitDepth & DDBD_32)
					pAdapter->DesktopMode.Format.d3dfFormat = (D3DFORMAT)FMT_X8R8G8B8;
				else if(pDevice->m_Desc.dwDeviceRenderBitDepth & DDBD_16)
					pAdapter->DesktopMode.Format.d3dfFormat = (D3DFORMAT)FMT_R5G6B5;
				else
				{
					DPF(1, _T("D3D Device is not hardware accelerated in any supported display modes.\n"));
					return D3DENUMRET_CANCEL;
				}
			}
			d3df2ddpf( pAdapter->DesktopMode.Format.d3dfFormat, &pAdapter->DesktopMode.Format.ddpfPixelFormat);
			pAdapter->DesktopMode.dwBPP = pAdapter->DesktopMode.Format.ddpfPixelFormat.dwRGBBitCount;
		}
		
		pDevice->m_Desc.dwPrimitiveMiscCaps = pDev->dpcTriCaps.dwMiscCaps;
		pDevice->m_Desc.dwRasterCaps = pDev->dpcTriCaps.dwRasterCaps;
		pDevice->m_Desc.dwZCmpCaps = pDev->dpcTriCaps.dwZCmpCaps;
		pDevice->m_Desc.dwSrcBlendCaps = pDev->dpcTriCaps.dwSrcBlendCaps;
		pDevice->m_Desc.dwDestBlendCaps = pDev->dpcTriCaps.dwDestBlendCaps;
		pDevice->m_Desc.dwAlphaCmpCaps = pDev->dpcTriCaps.dwAlphaCmpCaps;
		pDevice->m_Desc.dwShadeCaps = pDev->dpcTriCaps.dwShadeCaps;
		pDevice->m_Desc.dwTextureCaps = pDev->dpcTriCaps.dwTextureCaps;
		pDevice->m_Desc.dwTextureFilterCaps = pDev->dpcTriCaps.dwTextureFilterCaps;
		pDevice->m_Desc.dwCubeTextureFilterCaps = pDev->dpcTriCaps.dwTextureFilterCaps;
		pDevice->m_Desc.dwVolumeTextureFilterCaps = 0;
		pDevice->m_Desc.dwTextureAddressCaps = pDev->dpcTriCaps.dwTextureAddressCaps;
		pDevice->m_Desc.dwVolumeTextureAddressCaps = 0;
		
		pDevice->m_Desc.dwLineCaps = 0;
		if (pDevice->m_Desc.dpcLineCaps.dwTextureCaps)
			pDevice->m_Desc.dwLineCaps |= LINECAPS_TEXTURE;
		if (pDevice->m_Desc.dpcLineCaps.dwZCmpCaps ==  pDevice->m_Desc.dpcTriCaps.dwZCmpCaps)
			pDevice->m_Desc.dwLineCaps |= LINECAPS_ZTEST;
		if ( (pDevice->m_Desc.dpcLineCaps.dwSrcBlendCaps == pDevice->m_Desc.dpcTriCaps.dwSrcBlendCaps) &&
			(pDevice->m_Desc.dpcLineCaps.dwDestBlendCaps == pDevice->m_Desc.dpcTriCaps.dwDestBlendCaps) )
			pDevice->m_Desc.dwLineCaps |= LINECAPS_BLEND;
		if (pDevice->m_Desc.dpcLineCaps.dwAlphaCmpCaps)
			pDevice->m_Desc.dwLineCaps |= LINECAPS_ALPHACMP;
		if (pDevice->m_Desc.dpcLineCaps.dwRasterCaps & (D3DPRASTERCAPS_FOGVERTEX | D3DPRASTERCAPS_FOGTABLE))
			pDevice->m_Desc.dwLineCaps |= LINECAPS_FOG;

		pDevice->m_Desc.dwMinTextureWidth = pDev->dwMinTextureWidth;
		pDevice->m_Desc.dwMinTextureHeight = pDev->dwMinTextureHeight;
		pDevice->m_Desc.dwMaxTextureWidth = pDev->dwMaxTextureWidth;
		pDevice->m_Desc.dwMaxTextureHeight = pDev->dwMaxTextureHeight;
		pDevice->m_Desc.dwMinVolumeExtent = 0;
		pDevice->m_Desc.dwMaxVolumeExtent = 0;
		pDevice->m_Desc.dwMaxTextureRepeat = pDev->dwMaxTextureRepeat;
		pDevice->m_Desc.dwMaxTextureAspectRatio = pDev->dwMaxTextureAspectRatio;
		pDevice->m_Desc.dwMaxAnisotropy = pDev->dwMaxAnisotropy;
		pDevice->m_Desc.dvMaxVertexW = pDev->dvMaxVertexW;
		pDevice->m_Desc.dvGuardBandLeft = pDev->dvGuardBandLeft;
		pDevice->m_Desc.dvGuardBandTop = pDev->dvGuardBandTop;
		pDevice->m_Desc.dvGuardBandRight = pDev->dvGuardBandRight;
		pDevice->m_Desc.dvGuardBandBottom = pDev->dvGuardBandBottom;
		pDevice->m_Desc.dvExtentsAdjust = pDev->dvExtentsAdjust;
		pDevice->m_Desc.dwStencilCaps = pDev->dwStencilCaps;
		pDevice->m_Desc.dwFVFCaps = pDev->dwFVFCaps;
		pDevice->m_Desc.dwTextureOpCaps = pDev->dwTextureOpCaps;
		pDevice->m_Desc.wMaxTextureBlendStages = pDev->wMaxTextureBlendStages;
		pDevice->m_Desc.wMaxSimultaneousTextures = pDev->wMaxSimultaneousTextures;
		pDevice->m_Desc.dwVertexProcessingCaps = pDev->dwVertexProcessingCaps;
		pDevice->m_Desc.dwMaxActiveLights = pDev->dwMaxActiveLights;
		pDevice->m_Desc.wMaxUserClipPlanes = pDev->wMaxUserClipPlanes;
		pDevice->m_Desc.wMaxVertexBlendMatrices = pDev->wMaxVertexBlendMatrices;
		pDevice->m_Desc.dwMaxVertexBlendMatrixIndex = 0;
		pDevice->m_Desc.dvMaxPointSize = 0;
		pDevice->m_Desc.dwMaxPrimitiveCount = 0xffff;
		pDevice->m_Desc.dwMaxVertexIndex = 0xffff;
		pDevice->m_Desc.dwMaxStreams = 0;
		pDevice->m_Desc.dwMaxStreamStride = 0;
		pDevice->m_Desc.dwPixelShaderVersion = 0;
		pDevice->m_Desc.dvMaxPixelShaderValue = 0.f;

		pDevice->m_Desc.dwReserved1 = pDev->dwReserved1;
		pDevice->m_Desc.dwReserved2 = pDev->dwReserved2;
		pDevice->m_Desc.dwReserved3 = pDev->dwReserved3;
		pDevice->m_Desc.dwReserved4 = pDev->dwReserved4;


		// Get ZBuffer formats
		hr = pD3D->m_pD3D7->EnumZBufferFormats(pDevice->m_Guid, EnumZBufferFormats7, pContext);
		if (FAILED(hr))
		{
			DPF(1, _T("CD3D::EnumD3DDevices7() -  Error enumeration Z buffer formats on %s, hr=%s(%x)\n"), 
						szDevice, pD3D->HResultToString(hr), hr);
			return D3DENUMRET_OK;
		}

		// Create Direct3D device and get texture formats
		LPDIRECT3DDEVICE7   pD3DDevice;
		LPDIRECTDRAWSURFACE7 pSurf = NULL;
		DDSURFACEDESC2  Desc;

		ZeroMemory(&Desc,sizeof(DDSURFACEDESC2));
		Desc.dwSize = sizeof(DDSURFACEDESC2);
		Desc.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
		Desc.ddsCaps.dwCaps = DDSCAPS_3DDEVICE|DDSCAPS_OFFSCREENPLAIN;
		Desc.dwWidth = 640;
		Desc.dwHeight = 480;
		hr = pD3D->m_pDDraw7->CreateSurface(&Desc, &pSurf, NULL);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3D::EnumD3DDevices7() -  CreateSurface failed on %s, hr=%s(%x)\n"), 
						szDevice, pD3D->HResultToString(hr), hr);
			return D3DENUMRET_OK;
		}

		hr = pD3D->m_pD3D7->CreateDevice(pDevice->m_Guid, pSurf, &pD3DDevice);
		if (FAILED(hr))
		{
			DPF(2, _T("CD3D: Error creating Direct3D device \"%s\", hr=%s(%x)\n"), 
						szDevice, pD3D->HResultToString(hr), hr);
			return D3DENUMRET_OK;
		}

		hr = pD3DDevice->EnumTextureFormats(EnumTextureFormats7, pContext);
		if (FAILED(hr))
		{
			DPF(1, _T("CD3D: Error enumerating texture formats for \"%s\", hr=%s(%x)\n"), 
						szDevice, pD3D->HResultToString(hr), hr);
			RELEASE(pD3DDevice);
			return D3DENUMRET_OK;
		}

		SAFEDELETEA(pDevice->m_pBackBufferFormat);

		FORMAT **pFmt = &(pDevice->m_pBackBufferFormat);
		for (DWORD dwCount = 0; dwCount < g_dwBackBufferFormats7; dwCount++)
		{
			FORMAT fmt(g_BackBufferFormats7[dwCount]);

			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D: - Out of memory.\n"));
				return D3DENUMRET_CANCEL;
			}
			memcpy(*pFmt, &fmt, sizeof(FORMAT));
			(*pFmt)->pNext = NULL;

			pDevice->m_nBackBufferFormat++;
			pFmt = &((*pFmt)->pNext);
		}
		
		RELEASE(pD3DDevice);
		RELEASE(pSurf);
	}

	return DDENUMRET_OK;
}

HRESULT WINAPI CD3D::EnumZBufferFormats7(LPDDPIXELFORMAT pddpf, LPVOID pContext)
{
	CDevice *pDevice = (CDevice*)pContext;
	FORMAT  **pFormat = &(pDevice->m_pZBufferFormat);

	while(*pFormat)
		pFormat = &((*pFormat)->pNext);

	*pFormat = new FORMAT;
	if(*pFormat == NULL)
	{
		DPF(1, _T("CD3D::EnumZBufferFormats7() - Out of memory.\n"));
		return D3DENUMRET_CANCEL;
	}
	memset(*pFormat, 0, sizeof(FORMAT));

	memcpy(&((*pFormat)->ddpfPixelFormat), pddpf, sizeof(DDPIXELFORMAT));
	DDPFToD3DFmt(pddpf, (FMT*)&((*pFormat)->d3dfFormat), NULL);
	ExpandDDPF((FMT)((*pFormat)->d3dfFormat), &((*pFormat)->ddpfPixelFormat));
	pDevice->m_nZBufferFormat++;

	return D3DENUMRET_OK;
}


HRESULT WINAPI CD3D::EnumTextureFormats7(LPDDPIXELFORMAT pddpf, LPVOID pContext)
{
	CDevice *pDevice = (CDevice*)pContext;
	FORMAT  **pFormat = &(pDevice->m_pTextureFormat);

	while(*pFormat)
		pFormat = &((*pFormat)->pNext);

	*pFormat = new FORMAT;
	if(*pFormat == NULL)
	{
		DPF(1, _T("CD3D::EnumTextureFormats7() - Out of memory.\n"));
		return D3DENUMRET_CANCEL;
	}
	memset(*pFormat, 0, sizeof(FORMAT));

	// Add some private pixel format flags to help the test issolate certain fourcc textures
	if(pddpf->dwFlags & DDPF_FOURCC)
	{
		if (pddpf->dwFourCC == MAKEFOURCC('U', 'Y', 'V', 'Y'))
			pddpf->dwFlags |= PF_YUV;
		else if (pddpf->dwFourCC == MAKEFOURCC('Y', 'U', 'Y', '2'))
			pddpf->dwFlags |= PF_YUV;
		else if (pddpf->dwFourCC == MAKEFOURCC('D', 'X', 'T', '1'))
			pddpf->dwFlags |= PF_DXTN;
		else if (pddpf->dwFourCC == MAKEFOURCC('D', 'X', 'T', '2'))
			pddpf->dwFlags |= PF_DXTN;
		else if (pddpf->dwFourCC == MAKEFOURCC('D', 'X', 'T', '3'))
			pddpf->dwFlags |= PF_DXTN;
		else if (pddpf->dwFourCC == MAKEFOURCC('D', 'X', 'T', '4'))
			pddpf->dwFlags |= PF_DXTN;
		else if (pddpf->dwFourCC == MAKEFOURCC('D', 'X', 'T', '5'))
			pddpf->dwFlags |= PF_DXTN;
		else
			pddpf->dwFlags |= PF_UNKNOWNFOURCC;
	}

	memcpy(&(*pFormat)->ddpfPixelFormat, pddpf, sizeof(DDPIXELFORMAT));
	DDPFToD3DFmt(pddpf, (FMT*)&((*pFormat)->d3dfFormat), NULL);
	pDevice->m_nTextureFormat++;

	if(pDevice->m_Desc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP)
	{
		pFormat = &(pDevice->m_pCubeFormat);

		while(*pFormat)
			pFormat = &((*pFormat)->pNext);
	
		*pFormat = new FORMAT;
		if(*pFormat == NULL)
		{
			DPF(1, _T("CD3D::EnumTextureFormats7() - Out of memory.\n"));
			return D3DENUMRET_CANCEL;
		}
		memset(*pFormat, 0, sizeof(FORMAT));

		memcpy(&(*pFormat)->ddpfPixelFormat, pddpf, sizeof(DDPIXELFORMAT));
		DDPFToD3DFmt(pddpf, (FMT*)&((*pFormat)->d3dfFormat), NULL);
		ExpandDDPF((FMT)((*pFormat)->d3dfFormat), &((*pFormat)->ddpfPixelFormat));
		pDevice->m_nCubeFormat++;
	}

	return D3DENUMRET_OK;
}

HRESULT CD3D::SetupD3D7(CDevice* pDevice, PRESENT_PARAMETERS *pParams)
{
	HRESULT hr;
	DDCREATEEX  pProc = (DDCREATEEX) GetProcAddress(GetModuleHandle("DDRAW.DLL"),"DirectDrawCreateEx");

	if(pProc == NULL)
	{
		DPF(1, _T("CD3D::SetupD3D7() - GetProcAddress() failed for DirectDrawCreateEx.\n"));
		return E_FAIL;
	}

	// Release any lingering interfaces
	ReleaseD3D();
	
	// Query the DDraw object
	hr = pProc(&(pDevice->GetAdapter()->Guid), (PVOID*)&m_pDDraw7, IID_IDirectDraw7, NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CD3D::SetupD3D7() - Failed to query DDraw object. hr=%s(%x).\n"),
			HResultToString(hr), hr);
		goto Error;
	}
	
	// Set the coop mode, screen resolution
	if (!SetupScreen7(pDevice->m_hFocusWindow, pParams))
	{
		hr = E_FAIL;
		goto Error;
	}
	
	// Initialize the D3D object
	hr = m_pDDraw7->QueryInterface(IID_IDirect3D7, (PVOID*)&m_pD3D7);
	if (FAILED(hr))
	{
		DPF(1, _T("CD3D::SetupD3D7() - QI of IDirect3D7 from IDirectDraw7 failed.\n"));
		goto Error;
	}

	// Create the Primary
	hr = CreatePrimary7(pDevice, pParams);
	if(FAILED(hr))
	{
		DPF(1, _T("CD3D::SetupD3D7() - QI of IDirect3D7 from IDirectDraw7 failed.\n"));
		goto Error;
	}
	return S_OK;

Error:
	RELEASE(m_pDDraw7);
	RELEASE(m_pD3D7);
	RELEASE(m_pPrimary);
	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CreatePrimary7(CDevice* pDevice, PRESENT_PARAMETERS *pParams)
{
	DDSURFACEDESC2  Desc;
	LPDIRECTDRAWCLIPPER pClipper = NULL;
	LPDIRECTDRAWSURFACE7 pPrimary7 = NULL;
	HRESULT hr = 0;

	ZeroMemory(&Desc,sizeof(DDSURFACEDESC2));
	Desc.dwSize  = sizeof(DDSURFACEDESC2);
	Desc.dwFlags = DDSD_CAPS;
	Desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;

	if (!pParams->bWindowed)
	{
		Desc.dwFlags |= DDSD_BACKBUFFERCOUNT;
		Desc.ddsCaps.dwCaps |= DDSCAPS_FLIP | DDSCAPS_COMPLEX  | DDSCAPS_3DDEVICE;
		Desc.dwBackBufferCount = pParams->uBackBufferCount;
	}

	if (pParams->dwInternalFlags & PPINTERNAL_HINTANTIALIAS)
		Desc.ddsCaps.dwCaps2 |= DDSCAPS2_HINTANTIALIASING;

	// Create the surface
	hr = m_pDDraw7->CreateSurface(&Desc, &pPrimary7,NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreatePrimary7() - CreateSurface failed. hr=%s(%x).\n"), HResultToString(hr), hr);
		goto Error;
	}

	// Create the clipper
	hr = m_pDDraw7->CreateClipper(0,&pClipper,NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreatePrimary7() - CreateClipper failed. hr=%s(%x).\n"), HResultToString(hr), hr);
		goto Error;
	}

	// Set the clipper
	hr = pPrimary7->SetClipper(pClipper);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreatePrimary7() - SetClipper failed. hr=%s(%x).\n"), HResultToString(hr), hr);
		goto Error;
	}

	// Set the clipper's HWND
	hr = pClipper->SetHWnd(0, pParams->hDeviceWindow);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::CreatePrimary7() - SetHWnd failed. hr=%s(%x).\n"), HResultToString(hr), hr);
		goto Error;
	}

	// Wrap the primary
	m_pPrimary = new CSurface();
	if(m_pPrimary == NULL)
	{
		DPF(1, _T("CDevice::CreatePrimary7() - Out of memory.\n"));
		goto Error;
	}

	if(!m_pPrimary->Init(pPrimary7, pDevice, NULL))
	{
		DPF(2, _T("CDevice::CreatePrimary7() - CSurface() init failed.\n"));
		goto Error;
	}

	RELEASE(pPrimary7);
	RELEASE(pClipper);

	return S_OK;

Error:
	RELEASE(pPrimary7);
	RELEASE(m_pPrimary);
	RELEASE(pClipper);

	return hr;
}

// --------------------------------------------------------------------------------------

bool CD3D::SetupScreen7(HWND hWnd, PRESENT_PARAMETERS *pParams)
{
	DWORD   dwFlags = DDSCL_NORMAL;
	HRESULT hr;

	if (!pParams->bWindowed)
		dwFlags = DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT;

	hr = m_pDDraw7->SetCooperativeLevel(hWnd,dwFlags | DDSCL_MULTITHREADED);
	if (FAILED(hr))
	{
		DPF(1, _T("CD3D::SetupScreenModes7() - SetCooperativeLevel failed. hr = %s(%x)\n"),
			HResultToString(hr), hr);
		return false;
	}

	if(!pParams->bWindowed)
	{
		hr = m_pDDraw7->SetDisplayMode(pParams->uWidth, pParams->uHeight, 
									   pParams->BackBufferFormat.ddpfPixelFormat.dwRGBBitCount,
									   pParams->uFullscreenRefresh,0);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3D::SetupScreenModes7() - SetDisplayMode failed. hr = %s(%x)\n"),
				HResultToString(hr), hr);
			return false;
		}
	}
	return true;
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CreateDevice(GUID Guid, LPDIRECTDRAWSURFACE7 pSurf, LPDIRECT3DDEVICE7 *pDev7)
{
	HRESULT hr;

	if(pSurf == NULL || m_pD3D7 == NULL)
	{
		DPF(1, _T("CD3D::CreateDevice() - Invalid params.\n"));
		SetLastError(E_FAIL);
		return E_FAIL;
	}

	hr = m_pD3D7->CreateDevice(Guid, pSurf, pDev7);
	SetLastError(hr);
	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::EvictManagedTextures7()
{
	HRESULT	hr;
	
	if(m_pD3D7 == NULL)
	{
		DPF(1, _T("CD3D::EvictManagedTextures() - Invalid params.\n"));
		SetLastError(E_FAIL);
		return E_FAIL;
	}

	hr = m_pD3D7->EvictManagedTextures();
	SetLastError(hr);
	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CreateDDrawPalette7(DWORD dwFlags, LPPALETTEENTRY lpDDColorArray, LPDIRECTDRAWPALETTE *lplpDDPalette)
{
	HRESULT	hr;

	if (m_pDDraw7 == NULL || lpDDColorArray == NULL || lplpDDPalette == NULL)
	{
		DPF(1, _T("CD3D::CreatePalette() - Invalid params.\n"));
		SetLastError(E_FAIL);
		return E_FAIL;
	}
	
	hr = m_pDDraw7->CreatePalette(dwFlags, lpDDColorArray, lplpDDPalette, NULL);
	SetLastError(hr);
	return hr;
}

// --------------------------------------------------------------------------------------

bool CD3D::IsStretchSupported()
{
	if(m_fVersion >= 8.f)
		return true;
	else
		return ((m_ddCaps.dwCaps & DDCAPS_BLTSTRETCH) && (m_ddCaps.dwSVBCaps & DDCAPS_BLTSTRETCH));
}

// -------------------------------------------------------------------------------------

HRESULT CD3D::DDPFToD3DFmt(LPDDPIXELFORMAT  pDdPixFmt, FMT* pFmt, BOOL* pbIsDepth)
{
	if( pbIsDepth ) *pbIsDepth = FALSE;

	// If the FOURCC bit is set then that 
	// means that runtime is providing the
	// format, so just grab that.
	if (pDdPixFmt->dwFlags & DDPF_FOURCC)
	{
		if (pDdPixFmt->dwFourCC == 0xFF000004)
		{
			// This is an example of a IHV-specific format
			// The HIWORD must be the PCI-ID of the IHV
			// and the third byte must be zero.
			// In this case, we're using a sample PCI-ID of
			// FF00, and we're denoting the 4th format
			// by that PCI-ID
			*pFmt = FMT_D32;
		}
		else
		{
			*pFmt = (FMT)pDdPixFmt->dwFourCC;
		}
	}
	else if (pDdPixFmt->dwFlags & DDPF_ZBUFFER)
	{
		if( pbIsDepth ) *pbIsDepth = TRUE;
		switch(pDdPixFmt->dwZBitMask)
		{
		default:
		case 0x0000FFFF: *pFmt = FMT_D16; break;
		case 0xFFFFFF00:
			switch(pDdPixFmt->dwStencilBitMask)
			{
			default:
			case 0x00000000: *pFmt = FMT_D24X8; break;
			case 0x000000FF: *pFmt = FMT_D24S8; break;
			case 0x0000000F: *pFmt = FMT_D24X4S4; break;
			}
			break;
		case 0x00FFFFFF:
			switch(pDdPixFmt->dwStencilBitMask)
			{
			default:
			case 0x00000000: *pFmt = FMT_X8D24; break;
			case 0xFF000000: *pFmt = FMT_S8D24; break;
			case 0x0F000000: *pFmt = FMT_X4S4D24; break;
			}
			break;
		case 0x0000FFFE: *pFmt = FMT_D15S1; break;
		case 0x00007FFF: *pFmt = FMT_S1D15; break;
		case 0xFFFFFFFF: *pFmt = FMT_D32; break;
		}
	}
	else if (pDdPixFmt->dwFlags & DDPF_BUMPDUDV)
	{
		UINT uFmt = pDdPixFmt->dwBumpDvBitMask;
		switch (uFmt)
		{
		case 0x0000ff00:
			switch (pDdPixFmt->dwRGBBitCount)
			{
			case 32:
				*pFmt = FMT_X8L8V8U8;
				break;
			case 16:
				*pFmt = FMT_V8U8;
				break;
			}
			break;

		case 0x000003e0:
			*pFmt = FMT_L6V5U5;
			break;
		}
	}
	else if (pDdPixFmt->dwFlags & DDPF_PALETTEINDEXED8)
	{
		if (pDdPixFmt->dwFlags & DDPF_ALPHAPIXELS)
		{
			*pFmt = FMT_A8P8;
		}
		else
		{
			*pFmt = FMT_P8;
		}
	}
	else if (pDdPixFmt->dwFlags & DDPF_PALETTEINDEXED4)
	{
		*pFmt = FMT_P4;
	}
	else if (pDdPixFmt->dwFlags & DDPF_ALPHA)
	{
		if (pDdPixFmt->dwAlphaBitDepth == 8)
		{
			*pFmt = FMT_A8;
		}
		else
		{
			*pFmt = FMT_UNKNOWN;
		}
	}
	else
	{
		UINT uFmt = pDdPixFmt->dwGBitMask | pDdPixFmt->dwRBitMask;

		if (pDdPixFmt->dwFlags & DDPF_ALPHAPIXELS)
		{
			uFmt |= pDdPixFmt->dwRGBAlphaBitMask;
		}

		switch (uFmt)
		{
		case 0x00ffff00:
			switch (pDdPixFmt->dwRGBBitCount)
			{
			case 32:
				*pFmt = FMT_X8R8G8B8;
				break;
			case 24:
				*pFmt = FMT_R8G8B8;
				break;
			}
			break;
		case 0xffffff00:
			*pFmt = FMT_A8R8G8B8;
			break;
		case 0xffe0:
			if (pDdPixFmt->dwFlags & DDPF_ALPHAPIXELS)
			{
				*pFmt = FMT_A1R5G5B5;
			}
			else
			{
				*pFmt = FMT_R5G6B5;
			}
			break;
		case 0x07fe0:
			*pFmt = FMT_X1R5G5B5;
			break;
		case 0xff0:
			*pFmt = FMT_X4R4G4B4;
			break;
		case 0xfff0:
			*pFmt = FMT_A4R4G4B4;
			break;
		case 0xff:
			if (pDdPixFmt->dwFlags & DDPF_ALPHAPIXELS)
			{
				*pFmt = FMT_A4L4;
			}
			else
			{
				*pFmt = FMT_L8;
			}
			break;
		case 0xffff:
			*pFmt = FMT_A8L8;
			break;
		case 0xfc:
			*pFmt = FMT_R3G3B2;
			break;
		case 0xfffc:
			*pFmt = FMT_A8R3G3B2;
			break;
		default:
			*pFmt = FMT_UNKNOWN;
			break;
		}
	}

	return D3D_OK;
}