// MultiheadPresent.cpp: implementation of the CMultiheadPresent class.
//
//////////////////////////////////////////////////////////////////////

#include "MultiheadPresent.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiheadPresent::CMultiheadPresent()
{
	m_TestType		= TEST_FULLSCREENPRESENT;
	m_szCommandKey	= _T("MultiheadPresent");
	m_szTestName	= _T("MultiheadPresent");
    m_Options.D3DOptions.DisplayMode.Format = FMT_X8R8G8B8; 
	m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
	m_Options.D3DOptions.dwBehaviors |= DEVICECREATE_ADAPTERGROUP_DEVICE;
}

CMultiheadPresent::~CMultiheadPresent()
{

}


//////////////////////////////////////////////////////////////////////
// Name:	Name()
// Desc:	return current test name
//////////////////////////////////////////////////////////////////////
char* CMultiheadPresent::Name( char* szName )
{
	sprintf( szName, "%s (%s,%d)", m_szTestName, 
											FORMAT2STRING( m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat ), 
											m_nIndexPresentParameters 
			);
	return szName;
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentParameters()
// Desc:	build the vector of present parameters to test
//////////////////////////////////////////////////////////////////////
void CMultiheadPresent::InitPresentParameters()
{
	PRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	GetFrontBufferRect();

	m_vecPresentParameters.clear();
    m_vecPresentParametersRotation.clear();
    


	d3dpp.bWindowed	= false;
	d3dpp.dwFlags = PRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.AutoDepthStencilFormat = FMT_UNKNOWN;
	d3dpp.bAutoDepthStencil	= false;
	d3dpp.uBackBufferCount = 1;
	d3dpp.MultiSample = MULTISAMPLE_NONE;
	
	d3dpp.bCachedBackBuffers = false;
	d3dpp.DesktopFormat = m_fmtFrontBuffer;

	FMT backbufferFormats[] = 
	{
		FMT_X8R8G8B8,
#ifndef FAST_TEST
		FMT_R5G6B5,
		FMT_X1R5G5B5,
		FMT_A2R10G10B10
#endif
	};
	vector< FMT > vecBackBufferFormats;

	for( int iFormat = 0; iFormat < sizeof( backbufferFormats ) / sizeof FMT; iFormat++ )
	{
		// check against CheckDeviceType
		if( FAILED( m_pD3D->CheckDeviceType(	m_pSrcDevice->GetAdapterID(),
												m_pSrcDevice->GetDevType(),
												backbufferFormats[iFormat],
												backbufferFormats[iFormat],
												FALSE
											  )
					 ) ||
			 FAILED( m_pD3D->CheckDeviceType(	m_pSrcDevice->GetAdapterID() + 1,
												m_pSrcDevice->GetDevType(),
												backbufferFormats[iFormat],
												backbufferFormats[iFormat],
												FALSE
											  )
					 )
			)
			continue;

		vecBackBufferFormats.push_back( backbufferFormats[iFormat] );
	}
		
	// find display modes for 800 x 600 and 1024 x 768
	UINT nMode = 0;
	DISPLAYMODE displayMode, displayMode1[2], displayMode2[2];
	ZeroMemory( &displayMode1, 2 * sizeof DISPLAYMODE );
	ZeroMemory( &displayMode2, 2 * sizeof DISPLAYMODE );
    UINT adapterID_Orientation = 0;
    UINT adapterID_1_Orientation = 0;

    adapterID_Orientation = GetGDIDisplayOrientation(m_pSrcDevice->GetAdapterID());
	while( SUCCEEDED( m_pD3D->EnumAdapterModes( m_pSrcDevice->GetAdapterID(), FMT_X8R8G8B8, nMode++, &displayMode ) ) )
	{
		
        
        // skip interlaced modes if required
		if( displayMode.dwRefreshRate < 60 && KeySet( _T( "NoInterlaced" ) ) )
			continue;

		// look for 800 x 600
		if( ( displayMode.dwWidth == 800 && displayMode.dwHeight == 600 ) ||
			( displayMode.dwWidth == 600 && displayMode.dwHeight == 800 ) )
		{
			if( displayMode1[0].dwWidth == displayMode.dwWidth &&
				displayMode1[0].dwHeight == displayMode.dwHeight )
			{
				if( displayMode1[0].dwRefreshRate > displayMode.dwRefreshRate )
					displayMode1[0] = displayMode;
			}
			else
				displayMode1[0] = displayMode;
		}

		// look for 1024 x 768
		if( ( displayMode.dwWidth == 1024 && displayMode.dwHeight == 768 ) ||
			( displayMode.dwWidth == 768 && displayMode.dwHeight == 1024 ) )
			if( displayMode1[1].dwWidth == displayMode.dwWidth &&
				displayMode1[1].dwHeight == displayMode.dwHeight )
			{
				if( displayMode1[1].dwRefreshRate > displayMode.dwRefreshRate )
					displayMode1[1] = displayMode;
			}
			else
				displayMode1[1] = displayMode;
	}

	nMode = 0;
    adapterID_1_Orientation = GetGDIDisplayOrientation(m_pSrcDevice->GetAdapterID() + 1);
	while( SUCCEEDED( m_pD3D->EnumAdapterModes( m_pSrcDevice->GetAdapterID() + 1, FMT_X8R8G8B8, nMode++, &displayMode ) ) )
	{
		// skip interlaced modes if required
		if( displayMode.dwRefreshRate < 60 && KeySet( _T( "NoInterlaced" ) ) )
			continue;

		// look for 800 x 600
		if( ( displayMode.dwWidth == 800 && displayMode.dwHeight == 600 ) ||
			( displayMode.dwWidth == 600 && displayMode.dwHeight == 800 ) )
			if( displayMode2[0].dwWidth == displayMode.dwWidth &&
				displayMode2[0].dwHeight == displayMode.dwHeight )
			{
				if( displayMode2[0].dwRefreshRate > displayMode.dwRefreshRate )
					displayMode2[0] = displayMode;
			}
			else
				displayMode2[0] = displayMode;

		// look for 1024 x 768
		if( ( displayMode.dwWidth == 1024 && displayMode.dwHeight == 768 ) ||
			( displayMode.dwWidth == 768 && displayMode.dwHeight == 1024 ) )
			if( displayMode2[1].dwWidth == displayMode.dwWidth &&
				displayMode2[1].dwHeight == displayMode.dwHeight )
			{
				if( displayMode2[1].dwRefreshRate > displayMode.dwRefreshRate )
					displayMode2[1] = displayMode;
			}
			else
				displayMode2[1] = displayMode;
	}

	// cycle through swapeffects
	DWORD swapEffects[] = 
	{
		SWAPEFFECT_DISCARD,
#ifndef FAST_TEST
		SWAPEFFECT_FLIP,
		SWAPEFFECT_COPY
#endif
	};
	vector< DWORD > vecSwapEffects;
	for( int iSwapEffect = 0; iSwapEffect < sizeof( swapEffects ) / sizeof DWORD; iSwapEffect++ )
	{
		if( d3dpp.uBackBufferCount > 1 && SWAPEFFECT_COPY == swapEffects[iSwapEffect] )
			continue;
				
		// verify command line restrictions
		if( KeySet(_T("discard")) && SWAPEFFECT_DISCARD != swapEffects[iSwapEffect] )
			continue;
		if( KeySet(_T("flip")) && SWAPEFFECT_FLIP != swapEffects[iSwapEffect] )
			continue;
		if( KeySet(_T("copy")) && SWAPEFFECT_COPY != swapEffects[iSwapEffect] )
			continue;

		vecSwapEffects.push_back( swapEffects[iSwapEffect] );
	}

	// cycle through present intervals
	DWORD presentIntervals[] = 
	{
		PRESENT_INTERVAL_DEFAULT,
#ifndef FAST_TEST
		PRESENT_INTERVAL_ONE,
		PRESENT_INTERVAL_IMMEDIATE
#endif
	};
	vector< DWORD > vecPresentIntervals;

	for( int iPresentInterval = 0; iPresentInterval < sizeof( presentIntervals ) / sizeof DWORD; iPresentInterval++ )
	{
		if( PRESENT_INTERVAL_DEFAULT != presentIntervals[iPresentInterval] &&
			!(m_deviceCaps.PresentationIntervals & presentIntervals[iPresentInterval])
			)
			continue;

		// verify command line restrictions
		if( KeySet(_T("interval_default")) && PRESENT_INTERVAL_DEFAULT != presentIntervals[iPresentInterval] )
			continue;
		if( KeySet(_T("interval_one")) && PRESENT_INTERVAL_ONE != presentIntervals[iPresentInterval] )
			continue;
		if( KeySet(_T("interval_immediate")) && PRESENT_INTERVAL_IMMEDIATE != presentIntervals[iPresentInterval] )
			continue;

		vecPresentIntervals.push_back( presentIntervals[iPresentInterval] );
	}

	// combine display modes / formats / swap effects / present intervals in combinations of 2

	// first combine them in order
	for( UINT uDisplayMode = 0; uDisplayMode < 2; uDisplayMode++ )
		for( UINT uFormat = 0; uFormat < vecBackBufferFormats.size(); uFormat++ )
			for( UINT uSwapEffect = 0; uSwapEffect < vecSwapEffects.size(); uSwapEffect++ )
				for( UINT uInterval = 0; uInterval < vecPresentIntervals.size(); uInterval++ )
				{
					// swapchain 1
					d3dpp.uWidth = displayMode1[uDisplayMode].dwWidth;
					d3dpp.uHeight = displayMode1[uDisplayMode].dwHeight;
					d3dpp.uFullscreenRefresh = displayMode1[uDisplayMode].dwRefreshRate;

					d3dpp.BackBufferFormat = vecBackBufferFormats[uFormat];

					d3dpp.SwapEffect = vecSwapEffects[uSwapEffect];

					d3dpp.uPresentInterval = vecPresentIntervals[uInterval];
					
					d3dpp.hDeviceWindow	= m_hWndReset;
					m_vecPresentParameters.push_back( d3dpp );
                    m_vecPresentParametersRotation.push_back( adapterID_Orientation );

					// swapchain 2
					d3dpp.uWidth = displayMode2[uDisplayMode].dwWidth;
					d3dpp.uHeight = displayMode2[uDisplayMode].dwHeight;
					d3dpp.uFullscreenRefresh = displayMode2[uDisplayMode].dwRefreshRate;

					d3dpp.hDeviceWindow = m_hWndOverride;
					m_vecPresentParameters.push_back( d3dpp );
                    m_vecPresentParametersRotation.push_back( adapterID_1_Orientation );
				}

	// now combine them in reverse order
	for( UINT uDisplayMode = 0; uDisplayMode < 2; uDisplayMode++ )
		for( UINT uFormat = 0; uFormat < vecBackBufferFormats.size(); uFormat++ )
			for( UINT uSwapEffect = 0; uSwapEffect < vecSwapEffects.size(); uSwapEffect++ )
				for( UINT uInterval = 0; uInterval < vecPresentIntervals.size(); uInterval++ )
				{
					// swapchain 1
					d3dpp.uWidth = displayMode1[uDisplayMode].dwWidth;
					d3dpp.uHeight = displayMode1[uDisplayMode].dwHeight;
					d3dpp.uFullscreenRefresh = displayMode1[uDisplayMode].dwRefreshRate;

					d3dpp.BackBufferFormat = vecBackBufferFormats[uFormat];

					d3dpp.SwapEffect = vecSwapEffects[uSwapEffect];

					d3dpp.uPresentInterval = vecPresentIntervals[uInterval];
					
					d3dpp.hDeviceWindow	= m_hWndReset;
					m_vecPresentParameters.push_back( d3dpp );
                    m_vecPresentParametersRotation.push_back( adapterID_Orientation );

					// swapchain 2
					d3dpp.uWidth = displayMode2[1 - uDisplayMode].dwWidth;
					d3dpp.uHeight = displayMode2[1 - uDisplayMode].dwHeight;
					d3dpp.uFullscreenRefresh = displayMode2[1 - uDisplayMode].dwRefreshRate;

					d3dpp.BackBufferFormat = vecBackBufferFormats[vecBackBufferFormats.size() - 1 - uFormat];

					d3dpp.SwapEffect = vecSwapEffects[vecSwapEffects.size() - 1 - uSwapEffect];

					d3dpp.uPresentInterval = vecPresentIntervals[vecPresentIntervals.size() - 1 - uInterval];
					
					d3dpp.hDeviceWindow = m_hWndOverride;
					m_vecPresentParameters.push_back( d3dpp );
                    m_vecPresentParametersRotation.push_back( adapterID_1_Orientation );
				}

	// now combine the display modes in order and the rest in reverse order
	for( UINT uDisplayMode = 0; uDisplayMode < 2; uDisplayMode++ )
		for( UINT uFormat = 0; uFormat < vecBackBufferFormats.size(); uFormat++ )
			for( UINT uSwapEffect = 0; uSwapEffect < vecSwapEffects.size(); uSwapEffect++ )
				for( UINT uInterval = 0; uInterval < vecPresentIntervals.size(); uInterval++ )
				{
					// swapchain 1
					d3dpp.uWidth = displayMode1[uDisplayMode].dwWidth;
					d3dpp.uHeight = displayMode1[uDisplayMode].dwHeight;
					d3dpp.uFullscreenRefresh = displayMode1[uDisplayMode].dwRefreshRate;

					d3dpp.BackBufferFormat = vecBackBufferFormats[uFormat];

					d3dpp.SwapEffect = vecSwapEffects[uSwapEffect];

					d3dpp.uPresentInterval = vecPresentIntervals[uInterval];
					
					d3dpp.hDeviceWindow	= m_hWndReset;
					m_vecPresentParameters.push_back( d3dpp );
                    m_vecPresentParametersRotation.push_back( adapterID_1_Orientation );

					// swapchain 2
					d3dpp.uWidth = displayMode2[uDisplayMode].dwWidth;
					d3dpp.uHeight = displayMode2[uDisplayMode].dwHeight;
					d3dpp.uFullscreenRefresh = displayMode2[uDisplayMode].dwRefreshRate;

					d3dpp.BackBufferFormat = vecBackBufferFormats[vecBackBufferFormats.size() - 1 - uFormat];

					d3dpp.SwapEffect = vecSwapEffects[vecSwapEffects.size() - 1 - uSwapEffect];

					d3dpp.uPresentInterval = vecPresentIntervals[vecPresentIntervals.size() - 1 - uInterval];
					
					d3dpp.hDeviceWindow = m_hWndOverride;
					m_vecPresentParameters.push_back( d3dpp );
                    m_vecPresentParametersRotation.push_back( adapterID_Orientation );

				}
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentWindows()
// Desc:	build the vector of Present windows to test
//////////////////////////////////////////////////////////////////////
void CMultiheadPresent::InitPresentWindows()
{
	m_vecPresentWindow.clear();
	m_vecPresentWindow.push_back( NULL );
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of present source rects to test
//////////////////////////////////////////////////////////////////////
void CMultiheadPresent::InitPresentSourceRect()
{
	m_vecRectPresentSource.clear();

	m_vecRectPresentSource.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of present dest rects to test
//////////////////////////////////////////////////////////////////////
void CMultiheadPresent::InitPresentDestRect()
{
	m_vecRectPresentDest.clear();

	m_vecRectPresentDest.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
}


//////////////////////////////////////////////////////////////////////
// Name:	InitTextures()
// Desc:	build the vector of textures to test
//////////////////////////////////////////////////////////////////////
void CMultiheadPresent::InitTextures()
{
	m_vecStringTexture.clear();
	m_vecStringTexture.push_back( "lake" );
}
