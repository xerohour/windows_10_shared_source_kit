// DeviceClip.cpp: implementation of the CDeviceClip class.
//
//////////////////////////////////////////////////////////////////////

#include "DeviceClip.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeviceClip::CDeviceClip() :
	m_nLastIndexPrivateData( -1 ),
    m_IsTaskBarAlwaysOnTop(true)
{
	m_TestType		= TEST_DEVICECLIP;
	m_szCommandKey	= _T("DeviceClip");
	m_szTestName	= _T("DeviceClip");
}

CDeviceClip::~CDeviceClip()
{

}


//////////////////////////////////////////////////////////////////////
// Name:	CapsCheck()
// Desc:	check caps
//////////////////////////////////////////////////////////////////////
bool CDeviceClip::CapsCheck()
{
	// do not run this test on single mon systems
	if( m_pD3D->GetAdapterCount() < 2 )
		return false;

	return CPresentII::CapsCheck();
}


//////////////////////////////////////////////////////////////////////
// Name:	TestInitialize()
// Desc:	Init the testcase
//////////////////////////////////////////////////////////////////////
UINT CDeviceClip::TestInitialize()
{
	// test not supported in DCT

       m_IsTaskBarAlwaysOnTop = IsTaskBarAlwaysOnTop();
       SetTaskBarState(false);
  
	return KeySet(_T("WHQL"))? D3DTESTINIT_SKIPALL : CPresentII::TestInitialize();


}

//////////////////////////////////////////////////////////////////////
// Name:	TestTerminate()
// Desc:	Terminate the testcase
//////////////////////////////////////////////////////////////////////
bool CDeviceClip::TestTerminate()
{
    
    SetTaskBarState(m_IsTaskBarAlwaysOnTop);
    return CPresentII::TestTerminate();
}


//////////////////////////////////////////////////////////////////////
// Name:	Name()
// Desc:	return current test name
//////////////////////////////////////////////////////////////////////
char* CDeviceClip::Name( char* szName )
{
	sprintf( szName, "%s (PP:%d,SRC:%d, DST:%d, DR:%d, W:%d)", m_szTestName, m_nIndexPresentParameters, m_nIndexRectSource, m_nIndexRectDest, m_nIndexDirtyRegion, m_nIndexPrivateData );
	return szName;
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentParameters()
// Desc:	build the vector of present parameters to test
//////////////////////////////////////////////////////////////////////
void CDeviceClip::InitPresentParameters()
{
	PRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	GetFrontBufferRect();

	m_vecPresentParameters.clear();

	d3dpp.bWindowed						= true;
	d3dpp.uBackBufferCount				= 1;
	d3dpp.SwapEffect					= m_SwapEffect;
	d3dpp.hDeviceWindow					= m_hWndReset;
	d3dpp.dwFlags						= PRESENTFLAG_LOCKABLE_BACKBUFFER | D3DPRESENTFLAG_DEVICECLIP;
	d3dpp.uPresentInterval				= D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.uWidth						= m_vecAdapterMonitors[0].m_rectFrontBuffer.right - m_vecAdapterMonitors[0].m_rectFrontBuffer.left;
	d3dpp.uHeight						= m_vecAdapterMonitors[0].m_rectFrontBuffer.bottom - m_vecAdapterMonitors[0].m_rectFrontBuffer.top;
	d3dpp.BackBufferFormat				= m_fmtFrontBuffer;
	d3dpp.uFullscreenRefresh			= D3DPRESENT_RATE_DEFAULT;
	d3dpp.AutoDepthStencilFormat		= FMT_UNKNOWN;
	d3dpp.bAutoDepthStencil				= false;
	d3dpp.MultiSample					= MULTISAMPLE_NONE;

	d3dpp.bCachedBackBuffers			= false;
	d3dpp.DesktopFormat					= m_fmtFrontBuffer;

	m_vecPresentParameters.push_back( d3dpp );

	// 9Ex support for D3DPRESENT_VIDEO_RESTRICT_TO_MONITOR
	if( m_pSrcDevice->GetInterface() >= 9.1f )
	{
		// assume that D3DPRESENTFLAG_VIDEO is only used for the new RESTRICT flag
		d3dpp.dwFlags						= PRESENTFLAG_LOCKABLE_BACKBUFFER | D3DPRESENTFLAG_VIDEO;
		m_vecPresentParameters.push_back( d3dpp );
	}

	d3dpp.dwFlags						= PRESENTFLAG_LOCKABLE_BACKBUFFER;
	m_vecPresentParameters.push_back( d3dpp );
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentDestRect()
// Desc:	build the vector of present dest rects to test
//////////////////////////////////////////////////////////////////////
void CDeviceClip::InitPresentDestRect()
{
	m_vecRectPresentDest.clear();

	m_vecRectPresentDest.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
	m_vecRectPresentDest.push_back( CreateRect( 1, 1, 6 ) );		// full area, offset on top left
	m_vecRectPresentDest.push_back( CreateRect( 1, 1, 9 ) );		// full area, offset on bottom right
	m_vecRectPresentDest.push_back( CreateRect( 2, 2, 1 ) );		// half width, half height, origin
	m_vecRectPresentDest.push_back( CreateRect( 2, 2, 6 ) );		// half width/height, offset on top left
	m_vecRectPresentDest.push_back( CreateRect( 4, 4, 9 ) );		// third width/height, offset on bottom right
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentSourceRect()
// Desc:	build the vector of present source rects to test
//////////////////////////////////////////////////////////////////////
void CDeviceClip::InitPresentSourceRect()
{
	m_vecRectPresentSource.clear();

	m_vecRectPresentSource.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
	m_vecRectPresentSource.push_back( CreateRect( 1, 1, 6 ) );		// full area, offset on top left
	m_vecRectPresentSource.push_back( CreateRect( 1, 1, 9 ) );		// full area, offset on bottom right
	m_vecRectPresentSource.push_back( CreateRect( 2, 2, 1 ) );		// half width, half height, origin
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of regions to test
//////////////////////////////////////////////////////////////////////
void CDeviceClip::InitDirtyRegions()
{
	vector<RegionDescription> vecRegDescr;
	RegionDescription RegDescr;

	m_vecRegData.clear();

	// NULL
	vecRegDescr.clear();
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// 1 rect: full area, offset on top left
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 1; RegDescr.indexYScale = 1; RegDescr.indexOffset = 6; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// 1 rect: half width, half height, origin
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 2; RegDescr.indexYScale = 2; RegDescr.indexOffset = 1; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );
}


//////////////////////////////////////////////////////////////////////
// Name:	InitTextures()
// Desc:	build the vector of textures to test
//////////////////////////////////////////////////////////////////////
void CDeviceClip::InitTextures()
{
	m_vecStringTexture.clear();
	m_vecStringTexture.push_back( "lake" );
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPrivateData()
// Desc:	build the vector of special data
//////////////////////////////////////////////////////////////////////
void CDeviceClip::InitPrivateData()
{
	m_vecPrivateData.clear();

	// window contained in main monitor, just like classic Present
	m_vecPrivateData.push_back(0);

	// clip left side
	m_vecPrivateData.push_back(1);

	// clip top side
	m_vecPrivateData.push_back(2);

	// clip right side
	m_vecPrivateData.push_back(3);

	// clip bottom side
	m_vecPrivateData.push_back(4);

	// clip left bottom corner
	m_vecPrivateData.push_back(5);

	// clip left top corner
	m_vecPrivateData.push_back(6);

	// clip right top corner
	m_vecPrivateData.push_back(7);

	// clip right bottom cornet
	m_vecPrivateData.push_back(8);

	// clip left and right
	m_vecPrivateData.push_back(9);

	// clip top and bottom
	m_vecPrivateData.push_back(10);

	// clip all
	m_vecPrivateData.push_back(11);
}


//////////////////////////////////////////////////////////////////////
// Name:	PrivateDataProcessing()
// Desc:	process private data case before rendering
//////////////////////////////////////////////////////////////////////
void CDeviceClip::PrivateDataProcessing()
{
	if( m_nLastIndexPrivateData != m_nIndexPrivateData )
	{
		MSG msg;

		m_nLastIndexPrivateData = m_nIndexPrivateData;

		int clipTest = m_vecPrivateData[m_nIndexPrivateData];

		// place the window on whatever adapter the test is running on
		HMONITOR hMon = MonitorFromWindow( m_pFramework->m_hWindow, MONITOR_DEFAULTTONEAREST );
		MONITORINFO mi;
		ZeroMemory( &mi, sizeof( mi ) );
		mi.cbSize = sizeof( mi );
		GetMonitorInfo( hMon, &mi );

		RECT windowRect;
		switch( clipTest )
		{
		// window contained in main monitor, just like classic Present
		case 0:
			windowRect.left = ( mi.rcMonitor.left + mi.rcMonitor.right - DEFAULT_WINDOW_WIDTH ) / 2;
			windowRect.right = windowRect.left + DEFAULT_WINDOW_WIDTH;
			windowRect.top = ( mi.rcMonitor.top + mi.rcMonitor.bottom - DEFAULT_WINDOW_HEIGHT ) / 2;
			windowRect.bottom = windowRect.top + DEFAULT_WINDOW_HEIGHT;
			break;

		// clip left side
		case 1:
			windowRect.left = mi.rcMonitor.left - DEFAULT_WINDOW_WIDTH / 2;
			windowRect.right = windowRect.left + DEFAULT_WINDOW_WIDTH;
			windowRect.top = ( mi.rcMonitor.top + mi.rcMonitor.bottom - DEFAULT_WINDOW_HEIGHT ) / 2;
			windowRect.bottom = windowRect.top + DEFAULT_WINDOW_HEIGHT;
			break;

		// clip top side
		case 2:
			windowRect.left = ( mi.rcMonitor.left + mi.rcMonitor.right - DEFAULT_WINDOW_WIDTH ) / 2;
			windowRect.right = windowRect.left + DEFAULT_WINDOW_WIDTH;
			windowRect.top = mi.rcMonitor.top - DEFAULT_WINDOW_HEIGHT / 2;
			windowRect.bottom = windowRect.top + DEFAULT_WINDOW_HEIGHT;
			break;
			
		// clip right side
		case 3:
			windowRect.left = mi.rcMonitor.right - DEFAULT_WINDOW_WIDTH / 2;
			windowRect.right = windowRect.left + DEFAULT_WINDOW_WIDTH;
			windowRect.top = ( mi.rcMonitor.top + mi.rcMonitor.bottom - DEFAULT_WINDOW_HEIGHT ) / 2;
			windowRect.bottom = windowRect.top + DEFAULT_WINDOW_HEIGHT;
			break;

		// clip bottom side
		case 4:
			windowRect.left = ( mi.rcMonitor.left + mi.rcMonitor.right - DEFAULT_WINDOW_WIDTH ) / 2;
			windowRect.right = windowRect.left + DEFAULT_WINDOW_WIDTH;
			windowRect.top = mi.rcMonitor.bottom - DEFAULT_WINDOW_HEIGHT / 2;
			windowRect.bottom = windowRect.top + DEFAULT_WINDOW_HEIGHT;
			break;

		// clip left bottom corner
		case 5:
			windowRect.left = mi.rcMonitor.left - DEFAULT_WINDOW_WIDTH / 2;
			windowRect.right = windowRect.left + DEFAULT_WINDOW_WIDTH;
			windowRect.top = mi.rcMonitor.bottom - DEFAULT_WINDOW_HEIGHT / 2;
			windowRect.bottom = windowRect.top + DEFAULT_WINDOW_HEIGHT;
			break;

		// clip left top corner
		case 6:
			windowRect.left = mi.rcMonitor.left - DEFAULT_WINDOW_WIDTH / 2;
			windowRect.right = windowRect.left + DEFAULT_WINDOW_WIDTH;
			windowRect.top = mi.rcMonitor.top - DEFAULT_WINDOW_HEIGHT / 2;
			windowRect.bottom = windowRect.top + DEFAULT_WINDOW_HEIGHT;
			break;

		// clip right top corner
		case 7:
			windowRect.left = mi.rcMonitor.right - DEFAULT_WINDOW_WIDTH / 2;
			windowRect.right = windowRect.left + DEFAULT_WINDOW_WIDTH;
			windowRect.top = mi.rcMonitor.top - DEFAULT_WINDOW_HEIGHT / 2;
			windowRect.bottom = windowRect.top + DEFAULT_WINDOW_HEIGHT;
			break;

		// clip right bottom cornet
		case 8:
			windowRect.left = mi.rcMonitor.right - DEFAULT_WINDOW_WIDTH / 2;
			windowRect.right = windowRect.left + DEFAULT_WINDOW_WIDTH;
			windowRect.top = mi.rcMonitor.bottom - DEFAULT_WINDOW_HEIGHT / 2;
			windowRect.bottom = windowRect.top + DEFAULT_WINDOW_HEIGHT;
			break;

		// clip left and right
		case 9:
			windowRect.left = mi.rcMonitor.left - DEFAULT_WINDOW_WIDTH / 2;
			windowRect.right = windowRect.left + 3 * DEFAULT_WINDOW_WIDTH / 2 + ( mi.rcMonitor.right - mi.rcMonitor.left );
			windowRect.top = ( mi.rcMonitor.top + mi.rcMonitor.bottom - DEFAULT_WINDOW_HEIGHT ) / 2;
			windowRect.bottom = windowRect.top + DEFAULT_WINDOW_HEIGHT;
			break;

		// clip top and bottom
		case 10:
			windowRect.left = ( mi.rcMonitor.left + mi.rcMonitor.right - DEFAULT_WINDOW_WIDTH ) / 2;
			windowRect.right = windowRect.left + DEFAULT_WINDOW_WIDTH;
			windowRect.top = mi.rcMonitor.top - DEFAULT_WINDOW_HEIGHT / 2;
			windowRect.bottom = windowRect.top + 3 * DEFAULT_WINDOW_HEIGHT / 2 + ( mi.rcMonitor.bottom - mi.rcMonitor.top );
			break;

		// clip all
		case 11:
			windowRect.left = mi.rcMonitor.left - DEFAULT_WINDOW_WIDTH / 2;
			windowRect.right = windowRect.left + 3 * DEFAULT_WINDOW_WIDTH / 2 + ( mi.rcMonitor.right - mi.rcMonitor.left );
			windowRect.top = mi.rcMonitor.top - DEFAULT_WINDOW_HEIGHT / 2;
			windowRect.bottom = windowRect.top + 3 * DEFAULT_WINDOW_HEIGHT / 2 + ( mi.rcMonitor.bottom - mi.rcMonitor.top );
			break;

		default:
			_ASSERT( FALSE );
			break;
		};

		MoveWindow(	m_hWndReset, 
					windowRect.left,
					windowRect.top,
					windowRect.right - windowRect.left,
					windowRect.bottom - windowRect.top,
					TRUE );

		// invalidate the window and force an update
		InvalidateRect( m_hWndReset, NULL, TRUE );
		UpdateWindow( m_hWndReset );
		Sleep( 100 );
		while( PeekMessage( &msg, m_hWndReset, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		MoveWindow(	m_hWndOverride, 
					windowRect.left,
					windowRect.top,
					windowRect.right - windowRect.left,
					windowRect.bottom - windowRect.top,
					TRUE );

		// invalidate the window and force an update
		InvalidateRect( m_hWndOverride, NULL, TRUE );
		UpdateWindow( m_hWndOverride );
		Sleep( 100 );
		while( PeekMessage( &msg, m_hWndOverride, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		GetFrontBufferRect();
	}
}


//////////////////////////////////////////////////////////////////////
// Name:	Present()
// Desc:	implementation of Present (either using the device's shortcut or swapchain's method)
//////////////////////////////////////////////////////////////////////
void CDeviceClip::Present()
{
	if( 0 == ( D3DPRESENTFLAG_VIDEO & m_vecPresentParameters[m_nIndexPresentParameters].dwFlags ) )
		CPresentII::Present();
	else
	{
		HRESULT hrPresent = E_FAIL;
		CSwapChain *pSwapChain = NULL;
		try
		{
			Check_HRFail( m_pRenderingDevice->GetSwapChain( 0, &pSwapChain ), "GetSwapChain" );
			hrPresent = pSwapChain->Present( m_prectPresentSource, m_prectPresentDest, m_vecPresentWindow[m_nIndexPresentWindow], m_pDirtyRegion, D3DPRESENT_VIDEO_RESTRICT_TO_MONITOR );
		}
		catch( HRESULT hResult )
		{
			RELEASE( pSwapChain );
			throw hResult;
		}
		RELEASE( pSwapChain );

		switch( HandlePresentReturnCode( hrPresent ) )
		{
		case E_ABORT:
		case E_OUTOFMEMORY:
			throw hrPresent;
		default:
			Check_HRFail( hrPresent, "Present" );
		};
	}
}
