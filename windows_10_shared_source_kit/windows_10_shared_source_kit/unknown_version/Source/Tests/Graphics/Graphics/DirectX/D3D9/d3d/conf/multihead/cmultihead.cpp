//////////////////////////////////////////////////////////////////////
// File:  CMultiHead.cpp
//
// Copyright (c) Microsoft Corporation
//
// Purpose:
// Test cases for Direct3D multi-head devices.
//
// History:
// ?? ??? ????  BruceN    Created.
// 15 May 2006  SEdmison  Cleaned up.
// 5  Dec 2011  Olanh	  Filtered modes to only those supported by
//                        all heads in the adaptergroup.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#include "CMultiHead.h"

#include <d3d9.h> // We only need this for the D3DDISPLAYMODEFILTER
				  // struct, but it's safe to include it since this 
                  // test is "D3D9 only" anyway.

/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

CMultiHead::CMultiHead()
:
	m_nParams( 1 ),
	m_MyWindows( NULL )
{
}


bool CMultiHead::CapsCheck()
{
	UINT iMasterAdapter = m_pSrcDevice->GetAdapterID();
	CAPS caps;
	HRESULT hr = m_pD3D->GetDeviceCaps( iMasterAdapter, m_pSrcDevice->GetDevType(), &caps );

	m_Adapters.clear();

	// must find a multihead adapter to run this test
	UINT nNumInGroup = caps.NumberOfAdaptersInGroup;
	if ( SUCCEEDED( hr ) && nNumInGroup > 1 )
	{
		UINT nAdapters = m_pD3D->GetAdapterCount();

		// find all secondary heads driven by this primary head
		for( UINT iAdapter = iMasterAdapter; iAdapter < nAdapters; iAdapter++ )
		{
		   hr =  m_pD3D->GetDeviceCaps( iAdapter, m_pSrcDevice->GetDevType(), &caps );

			if ( SUCCEEDED( hr ) && caps.MasterAdapterOrdinal == iMasterAdapter )
			{
				m_Adapters.push_back( iAdapter );
			}
		}

		return nNumInGroup == m_Adapters.size();
	}

	m_pFramework->WriteToLog(1, TEXT("Test requires a multi-head adapter (i.e. caps.NumberOfAdaptersInGroup > 1).\n") );
	return false;
}


UINT CMultiHead::TestInitialize()
{
	return CD3DTest::TestInitialize();
}


bool CMultiHead::GetDepthStencil
(
	UINT iSwapChain,
	LPSURFACES *pDepthStencil
)
{
	bool bResult = true;
	if ( pDepthStencil )
	{
		*pDepthStencil = NULL;
	}
	PRESENT_PARAMETERS *pParms = m_pSrcDevice->GetPresParams();
	if ( pParms->bAutoDepthStencil )
	{
		bResult = GetDepthStencilSurface( pDepthStencil );
	}
	return bResult;
}


HRESULT CMultiHead::CreatePresentParams
(
	TESTMODE *pMode,
	PRESENT_PARAMETERS **ppParams
)
{
	m_nParams = 1;

	for(int i=0; i<m_uDeviceList; i++)
	{
		if(m_DeviceList[i] == NULL)
			continue;

		m_nParams = max(m_nParams, m_DeviceList[i]->GetCaps()->NumberOfAdaptersInGroup);
	}

	HRESULT hr = CD3DTest::CreatePresentParams(pMode, ppParams);

	for(int j=1; j<m_nParams; j++)
	{
		CWindow *pWindow = new CWindow;
		pWindow->InitializeWindow
		(
			WS_EX_TOPMOST,
			_T("FMWK-MAIN"), 
			_T(""),
			NULL,
			NULL,
			WS_POPUP | WS_VISIBLE, 
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			(*ppParams)[0].hDeviceWindow
		);

		(*ppParams)[j].hDeviceWindow = pWindow->m_hWindow;

		if ( !m_MyWindows )
		{
			m_MyWindows = new CPtrList<CWindow>;
		}
		m_MyWindows->push_back( pWindow );
	}

	return hr;
}


bool CMultiHead::CleanupTest()
{
	delete m_MyWindows;
	m_MyWindows = NULL;

	return CD3DTest::CleanupTest();
}

// --------------------------------------------------------------------------------------
// CompareMode
//
// Compares the D3D9 relevant portions of two framework DISPLAYMODE structures.
// returns true if they match, otherwise false.
// --------------------------------------------------------------------------------------
bool CMultiHead::CompareMode(PDISPLAYMODE pmode1, PDISPLAYMODE pmode2)
{
	if (pmode1->dwHeight == pmode2->dwHeight &&
		pmode1->dwWidth == pmode2->dwWidth  &&
		pmode1->dwRefreshRate == pmode2->dwRefreshRate &&
		pmode1->Format.d3dfFormat == pmode2->Format.d3dfFormat)

		return true;

	else 
		return false;
	
}

// --------------------------------------------------------------------------------------
// IsModeSupportedByAllAdaptersInGroup
//
// Enumerates all modes on all adapters in an adaptergroup to determine whether or not a 
// provided mode is universally supported by all adapters.
// --------------------------------------------------------------------------------------
bool CMultiHead::IsModeSupportedByAllAdaptersInGroup(PDISPLAYMODE pMode, CDevice *pDevice)
{
	CAPS caps;
	DISPLAYMODE ModeToCompare;
	bool bMatchFound=false;
	D3DDISPLAYMODEFILTER filter;

	filter.Size = sizeof(D3DDISPLAYMODEFILTER);
	filter.Format=pMode->Format.d3dfFormat;

	//
	// Assume the current Adapter is a master adapter.   We could validate this, 
	// but I'm pretty sure the rest of the test will fall over anyway if this isn't true.
	//
	UINT uMasterAdapterIndex = pDevice->GetAdapterID();

	//
	// GetCaps so we can determine how many adapters are in the adapter group.
	// This is a poor choice of terminology by D3D9.  What we are really getting
	// is the number of heads (monitors) on a single adapter.
	//
	pDevice->GetD3D()->GetDeviceCaps(uMasterAdapterIndex, DEVICETYPE_HAL, &caps);

	//
	// Compare the DISPLAYMODE passed in against every mode enumerated by all heads
	// We only want modes that are supported by all heads .
	//
	for (int i=uMasterAdapterIndex; i<(uMasterAdapterIndex+caps.NumberOfAdaptersInGroup); i++)
	{

		filter.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
		UINT uNumModes = pDevice->GetD3D()->GetAdapterModeCountEx(i, &filter);
		bMatchFound = false;

		// Enumerate all modes for this head.
		for (UINT u=0; u<uNumModes; u++)
		{
			pDevice->GetD3D()->EnumAdapterModes(i, pMode->Format, u, &ModeToCompare);
			// Does this mode exactly match the mode passed in?
			if (CompareMode(&ModeToCompare, pMode))
			{
				bMatchFound = true;	
				break;
			}
		}

		// If no match was found for a particular head, then we'll need to reject it.  Return false.
		if (!bMatchFound) 
			return false; 
	}

	// We got this far, our mode must be universally supported by all heads.
	return true;
}

//----------------------------------------------------------------------------
// FilterResolution
//
// Overload for the D3D9 framework FilterResolution function.  Limits
// the choice of modes we test.   Unfortunately the framework only tracks a 
// single list of display modes, but this is a multihead test.   Different 
// monitors may support different resolutions.   The function will first 
// reject any mode not supported by all monitors in the AdapterGroup.
// It them further restricts the mode selection to a handful of commonly 
// supported formats.
//----------------------------------------------------------------------------
bool CMultiHead::FilterResolution(PDISPLAYMODE pMode)
{
	//
	// Validate parameters.
	//

	if ( pMode == NULL )
	{
		DPF( 1, _T( "CMultiHead::FilterResolution:  pMode is NULL.\n" ) );
		return false;
	};

	//
	// Validate members.
	//

	// Make sure that we have a device.
	if (!m_DeviceList[0])
	{
		DPF(1, _T("CMultiHead::FilterResolution:  Invalid device list.\n"));
		return false;
	};


	// Make sure that we have an adapter.
	ADAPTER *pAdapter = m_DeviceList[0]->GetAdapter();

	if (!pAdapter)
	{
		DPF(1, _T("CMultiHead::FilterResolution:  Unable to get adapter.\n"));
		return false;
	};

	//
	// Skip modes not supported by all adapters in adapter group. 
	//
	if(IsModeSupportedByAllAdaptersInGroup(pMode, m_DeviceList[0]) == false)
	{
		return false;
	}

	//
	// Skip repeated modes.  (Only test a single refresh rate for each mode.)
	// This only works because modes tend to be grouped by resolution and format.
	// 
	static DISPLAYMODE ModeLast={0,0,0,0, FMT_UNKNOWN};
	if
	(
		ModeLast.dwWidth == pMode->dwWidth && 
		ModeLast.dwHeight == pMode->dwHeight &&
		ModeLast.Format.d3dfFormat  == pMode->Format.d3dfFormat
	)
	{
		// Skipping duplicate mode 
		return false;
	};
	
	// Record mode to skip duplicates in the future
	ModeLast = *pMode;

	//
	// Establish the set of Multi-head acceptable modes.
	//
	// We really don't need to test all modes, but we 
	// want to try a reasonable sampling, and we want to 
	// try and test at least one mode that differs
	// from the desktop mode.
	//

	DWORD FrameworkAcceptableResolutions[][3] =
	{
		// Landscape-orientation modes
		{  800,  600, 16 },
		{  800,  600, 32 },
		{ 1024,  768, 16 },
		{ 1024,  768, 32 },


		// Portrait-orientation modes
		{  600,  800, 16 },
		{  600,  800, 32 },
		{  768, 1024, 16 },
		{  768, 1024, 32 }
	};

	int cFrameworkAcceptableResolutions = ARRAYSIZE( FrameworkAcceptableResolutions );


	// If we want to filter the list...
	if
	(
		(FMT) m_Options.D3DOptions.DisplayMode.Format.d3dfFormat == FMT_ALL ||
		KeySet(_T("whql"))
	)
	{
		// ...then drop down to the filtering logic below...
	}
	// ...or, if we just want to use a single safe resolution...
	else if
	(
		(FMT) m_Options.D3DOptions.DisplayMode.Format.d3dfFormat == FMT_ANY
	)
	{
		// ...then manipulate the list of acceptable modes so that
		// the 1024x768x32 is the only mode in the list...
		FrameworkAcceptableResolutions[ 0 ][ 0 ] = 1024;
		FrameworkAcceptableResolutions[ 0 ][ 1 ] = 768;
		FrameworkAcceptableResolutions[ 0 ][ 2 ] = 32; 

		FrameworkAcceptableResolutions[ 1 ][ 0 ] = 768;  // and the portrait version of it.
		FrameworkAcceptableResolutions[ 1 ][ 1 ] = 1024; 
		FrameworkAcceptableResolutions[ 1 ][ 2 ] = 32; 

		cFrameworkAcceptableResolutions = 2;
	}
	// ...otherwise, even if we don't want to constrain ourselves to the list above...
	else
	{
		// ...we still need to reject modes that are too small.
		if
		(
			pMode->dwWidth >= 640 &&
			pMode->dwHeight >= 480
		)
		{
			return true;
		};

		return false;
	};


	//
	// Walk the list of framework-acceptable modes in order to
	// determine whether to accept or reject the specified mode.
	//

	for
	(
		int i = 0;
		i < cFrameworkAcceptableResolutions;
		++i
	)
	{
		if
		(
			FrameworkAcceptableResolutions[ i ][ 0 ] == pMode->dwWidth && 
			FrameworkAcceptableResolutions[ i ][ 1 ] == pMode->dwHeight &&
			FrameworkAcceptableResolutions[ i ][ 2 ] == pMode->dwBPP
		)
		{
			return true;
		};
	};

	return false;
}

