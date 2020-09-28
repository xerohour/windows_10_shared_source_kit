// PresentEx.cpp: implementation of the CPresentEx class. LH extensions to Present
//
//////////////////////////////////////////////////////////////////////

#include "PresentEx.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPresentEx::CPresentEx()
{
	m_TestType		= TEST_PRESENT;
	m_szCommandKey	= _T("PresentEx");
	m_szTestName	= _T("PresentEx");
	m_Options.D3DOptions.fMinDXVersion = 9.1f;

	m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
}

CPresentEx::~CPresentEx()
{
}


//////////////////////////////////////////////////////////////////////
// Name:	CapsCheck()
// Desc:	check caps
//////////////////////////////////////////////////////////////////////
bool CPresentEx::CapsCheck()
{
	bool bCapsCheck = CPresentII::CapsCheck();

	if( bCapsCheck )
	{
		CAPS deviceCaps;
		if( FAILED( m_pSrcDevice->GetDeviceCaps( &deviceCaps ) ) )
		{
			WriteToLog( _T("Failed to get device caps. Test cannot continue.") );
			return false;
		}

		if( !( deviceCaps.Caps2 & D3DCAPS2_CANSHARERESOURCE ) )
		{
			WriteToLog( _T("XDDM Driver detected. PresentEx requires LDDM.") );
			return false;
		}
	}

	return bCapsCheck;
}


//////////////////////////////////////////////////////////////////////
// Name:	Name()
// Desc:	return current test name
//////////////////////////////////////////////////////////////////////
char* CPresentEx::Name( char* szName )
{
	sprintf( szName, "%s (%d,%d,%s)", m_szTestName,
											m_nIndexRectSource, 
											m_nIndexRectDest,
											(m_nIndexFlagsEx == 0)? "0" : (m_nIndexFlagsEx == 1)? "D3DPRESENT_DONOTFLIP" : "OTHER" );
	return szName;
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentWindows()
// Desc:	build the vector of Present windows to test
//////////////////////////////////////////////////////////////////////
void CPresentEx::InitPresentWindows()
{
	m_vecPresentWindow.clear();
	m_vecPresentWindow.push_back( NULL );
}


//////////////////////////////////////////////////////////////////////
// Name:	InitTextures()
// Desc:	build the vector of textures to test
//////////////////////////////////////////////////////////////////////
void CPresentEx::InitTextures()
{
	m_vecStringTexture.clear();
	m_vecStringTexture.push_back( "lake" );
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentParameters()
// Desc:	build the vector of present parameters to test
//////////////////////////////////////////////////////////////////////
void CPresentEx::InitPresentParameters()
{
	PRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	GetFrontBufferRect();

	m_vecPresentParameters.clear();

	d3dpp.bCachedBackBuffers			= false;
	d3dpp.DesktopFormat					= m_fmtFrontBuffer;
	
	d3dpp.uBackBufferCount				= 1;
	d3dpp.dwFlags						= PRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.BackBufferFormat				= m_fmtFrontBuffer;
	d3dpp.uFullscreenRefresh			= D3DPRESENT_RATE_DEFAULT;
	d3dpp.AutoDepthStencilFormat		= FMT_UNKNOWN;
	d3dpp.bAutoDepthStencil				= false;
	d3dpp.MultiSample					= MULTISAMPLE_NONE;

	// cycle through fullscreen / windowed
	bool windowed[] = 
	{
		//true,
		false,
	};

	// cycle through swapeffects
	DWORD swapEffects[] = 
	{
		SWAPEFFECT_FLIP,
	};
	// cycle through present intervals
	DWORD presentIntervals[] = 
	{
		PRESENT_INTERVAL_DEFAULT,
		PRESENT_INTERVAL_ONE,
		PRESENT_INTERVAL_TWO,
#ifndef FAST_TEST
		PRESENT_INTERVAL_THREE,
		PRESENT_INTERVAL_FOUR,
#endif
		PRESENT_INTERVAL_IMMEDIATE
	};

	for( int iWindowed = 0; iWindowed < sizeof( windowed ) / sizeof( bool ); iWindowed++ )
	{
		d3dpp.bWindowed = windowed[iWindowed];
		POINT bbSize[] = 
		{
			{m_vecAdapterMonitors[0].m_rectFrontBuffer.right - m_vecAdapterMonitors[0].m_rectFrontBuffer.left, m_vecAdapterMonitors[0].m_rectFrontBuffer.bottom - m_vecAdapterMonitors[0].m_rectFrontBuffer.top},
#ifndef FAST_TEST
			{1024, 768},
			{800, 600},
			{768, 1024},
			{600, 800},
#endif
		};

		// find acceptable backbuffer size
		int ibbSize = 0;
		for( ; ibbSize < sizeof( bbSize ) / sizeof POINT; ibbSize++ )
			if( SUCCEEDED( m_pD3D->CheckDeviceType(	m_pSrcDevice->GetAdapterID(),
													m_pSrcDevice->GetDevType(),
													d3dpp.BackBufferFormat,
													d3dpp.BackBufferFormat,
													d3dpp.bWindowed )
						)
				)
			{
				if( true == windowed[iWindowed] )
					break;

				UINT nMode = 0;
				DISPLAYMODE displayMode;
				while( SUCCEEDED( m_pD3D->EnumAdapterModes( m_pSrcDevice->GetAdapterID(), d3dpp.BackBufferFormat, nMode++, &displayMode ) ) )
				{
					if( displayMode.dwWidth == bbSize[ibbSize].x && displayMode.dwHeight == bbSize[ibbSize].y )
						break;
				}

				if( displayMode.dwWidth == bbSize[ibbSize].x && displayMode.dwHeight == bbSize[ibbSize].y )
					break;
			}

		if( ibbSize == sizeof( bbSize ) / sizeof POINT )
			break;	// no match found

		d3dpp.uWidth = bbSize[ibbSize].x;
		d3dpp.uHeight = bbSize[ibbSize].y;

		for( int iSwapEffect = 0; iSwapEffect < sizeof( swapEffects ) / sizeof DWORD; iSwapEffect++ )
		{
			d3dpp.SwapEffect = swapEffects[iSwapEffect];
			for( int iPresentInterval = 0; iPresentInterval < sizeof( presentIntervals ) / sizeof DWORD; iPresentInterval++ )
			{
				if( PRESENT_INTERVAL_DEFAULT == presentIntervals[iPresentInterval] || ( m_deviceCaps.PresentationIntervals & presentIntervals[iPresentInterval] ) )
				{
					d3dpp.uPresentInterval = presentIntervals[iPresentInterval];

					d3dpp.hDeviceWindow	= m_hWndReset;
					m_vecPresentParameters.push_back( d3dpp );

					for( UINT uSwapChain = 1; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
					{
						d3dpp.hDeviceWindow = m_hWndOverride;
						m_vecPresentParameters.push_back( d3dpp );
					}
				}
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentSourceRect()
// Desc:	build the vector of present source rects to test
//////////////////////////////////////////////////////////////////////
void CPresentEx::InitPresentSourceRect()
{
	m_vecRectPresentSource.clear();
	m_vecRectPresentSource.push_back( CreateRect( 0, 0, 0, 0 ) );	// NULL
	/*m_vecRectPresentSource.push_back( CreateRect( 1, 1, 1 ) );		// full area, origin
	m_vecRectPresentSource.push_back( CreateRect( 1, 1, 6 ) );		// full area, offset on top left
	m_vecRectPresentSource.push_back( CreateRect( 2, 2, 1 ) );		// half width, half height, origin
	m_vecRectPresentSource.push_back( CreateRect( 3, 3, 6 ) );		// double height/width, offset top left
	m_vecRectPresentSource.push_back( CreateRect( 15, 15, 122, 86 ) );// unaligned rect*/
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentDestRect()
// Desc:	build the vector of present dest rects to test
//////////////////////////////////////////////////////////////////////
void CPresentEx::InitPresentDestRect()
{
	m_vecRectPresentDest.clear();
	m_vecRectPresentDest.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
	/*m_vecRectPresentDest.push_back( CreateRect( 1, 1, 1 ) );		// full area, origin
	m_vecRectPresentDest.push_back( CreateRect( 1, 1, 6 ) );		// full area, offset on top left
	m_vecRectPresentDest.push_back( CreateRect( 2, 2, 1 ) );		// half width, half height, origin
	m_vecRectPresentDest.push_back( CreateRect( 3, 3, 6 ) );		// double height/width, offset top left
	m_vecRectPresentDest.push_back( CreateRect( 15, 15, 122, 86 ) );// unaligned rect*/
}


//////////////////////////////////////////////////////////////////////
// Name:	InitFlagsEx()
// Desc:	LONGHORN ONLY: build the vector of Ex flags to test
//////////////////////////////////////////////////////////////////////
void CPresentEx::InitFlagsEx()
{
	m_vecFlagsEx.clear();
	m_vecFlagsEx.push_back( 0 );
	m_vecFlagsEx.push_back( D3DPRESENT_DONOTFLIP );
}


//////////////////////////////////////////////////////////////////////
// Name:	ClearBackBuffers()
// Desc:	true if the backbuffer(s) should be cleared
//////////////////////////////////////////////////////////////////////
bool CPresentEx::ClearBackBuffers()
{
	return CPresentII::ClearBackBuffers();
}


//////////////////////////////////////////////////////////////////////
// Name:	UpdateBackBuffer()
// Desc:	true if the backbuffer contents should be updated in the REF image
//////////////////////////////////////////////////////////////////////
bool CPresentEx::UpdateBackBuffer()
{
	return CPresentII::UpdateBackBuffer();
}


//////////////////////////////////////////////////////////////////////
// Name:	Present()
// Desc:	implementation of Present (either using the device's shortcut or swapchain's method)
//////////////////////////////////////////////////////////////////////
void CPresentEx::Present()
{
	HRESULT hrPresent = E_FAIL;
	// render the image
	hrPresent = m_pRenderingDevice->PresentEx( m_prectPresentSource, m_prectPresentDest, m_vecPresentWindow[m_nIndexPresentWindow], m_pDirtyRegion, m_vecFlagsEx[m_nIndexFlagsEx] & ~D3DPRESENT_DONOTFLIP );

	// clear the backbuffers; verify nothing happens
	if( SUCCEEDED( hrPresent ) && 0 != ( m_vecFlagsEx[m_nIndexFlagsEx] & D3DPRESENT_DONOTFLIP ) )
	{
		// RECT rectSource = CreateRect( m_vecRectPresentSource[5 - m_nIndexRectSource], m_vecPresentParameters[m_nIndexPresentParameters].uWidth, m_vecPresentParameters[m_nIndexPresentParameters].uHeight );
		// RECT rectDest = CreateRect( m_vecRectPresentDest[5 - m_nIndexRectDest], m_dwWindowWidth, m_dwWindowHeight );

		// just do a clear - should NOT be presented
		for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
		{
			LPSURFACE pBBSurface = NULL;

			ASSERT( uSwapChain < m_vecAdapterMonitors.size() );
			ASSERT( NULL != m_vecAdapterMonitors[uSwapChain].m_pSwapChain );

			// set the render target to the swapchain's backbuffer
			if( m_uNumberSwapChains > 1 )
			{
				Check_HRFail( m_vecAdapterMonitors[uSwapChain].m_pSwapChain->GetBackBuffer( m_nIndexBB, D3DBACKBUFFER_TYPE_MONO, &pBBSurface ), "GetBackBuffer" );
				Check_HRFail( m_pRenderingDevice->SetRenderTarget( (DWORD)0, pBBSurface ), "SetRenderTarget" );
			}
	
			Check_HRFail( m_pRenderingDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, COLOR_ERASE.color, 1.0f, 0L ), "Clear" );
			
			// release the swapchain's backbuffer
			if( m_uNumberSwapChains > 1 )
			{
				RELEASE( pBBSurface );
			}
		}

		if( SUCCEEDED( hrPresent ) )
			hrPresent = m_pRenderingDevice->PresentEx( m_prectPresentSource, m_prectPresentDest , m_vecPresentWindow[m_nIndexPresentWindow], m_pDirtyRegion, m_vecFlagsEx[m_nIndexFlagsEx] );
	}

	switch( HandlePresentReturnCode( hrPresent ) )
	{
	case E_ABORT:
	case E_OUTOFMEMORY:
		throw hrPresent;
	default:
		Check_HRFail( hrPresent, "PresentEx" );
	};
}
