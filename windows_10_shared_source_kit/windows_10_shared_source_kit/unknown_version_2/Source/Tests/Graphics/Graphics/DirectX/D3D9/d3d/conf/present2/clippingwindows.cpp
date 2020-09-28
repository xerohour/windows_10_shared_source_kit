// ClippingWindows.cpp: implementation of the CClippingWindows class.
//
//////////////////////////////////////////////////////////////////////

#include "ClippingWindows.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClippingWindows::CClippingWindows()
{
	m_TestType		= TEST_CLIPPINGWINDOWS;
	m_szCommandKey	= _T("Clipping");
	m_szTestName	= _T("ClippingWindows");
}

CClippingWindows::~CClippingWindows()
{

}


//////////////////////////////////////////////////////////////////////
// Name:	Name()
// Desc:	return current test name
//////////////////////////////////////////////////////////////////////
char* CClippingWindows::Name( char* szName )
{
	sprintf( szName, "%s (%d,%d,%d)", m_szTestName, m_nIndexRectDest, m_nIndexDirtyRegion, m_nIndexDialogs );
	return szName;
}


//////////////////////////////////////////////////////////////////////
// Name:	TestInitialize()
// Desc:	Init the testcase
//////////////////////////////////////////////////////////////////////
UINT CClippingWindows::TestInitialize()
{
	// test not supported in DCT
	return KeySet(_T("WHQL"))? D3DTESTINIT_SKIPALL:CPresentII::TestInitialize();
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentSourceRect()
// Desc:	build the vector of present source rects to test
//////////////////////////////////////////////////////////////////////
void CClippingWindows::InitPresentSourceRect()
{
	m_vecRectPresentSource.clear();

	m_vecRectPresentSource.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
}

#ifdef FAST_TEST
//////////////////////////////////////////////////////////////////////
void CClippingWindows::InitPresentDestRect()
{
	m_vecRectPresentDest.clear();

	m_vecRectPresentDest.push_back(CreateRect(0, 0, 0, 0));        // NULL
	m_vecRectPresentDest.push_back(CreateRect(1, 1, 6));        // full area, offset on top left
	m_vecRectPresentDest.push_back(CreateRect(1, 1, 4));        // full area, offset top

	m_vecRectPresentDest.push_back(CreateRect(2, 2, 6));        // half width/height, offset on top left
	m_vecRectPresentDest.push_back(CreateRect(5, 5, 9));        // 1 & half height/width, offset bottom right
	m_vecRectPresentDest.push_back(CreateRect(15, 15, 122, 86));// unaligned rect
}
#endif


//////////////////////////////////////////////////////////////////////
// Name:	InitWindows()
// Desc:	build the vector of clip windows to test
//////////////////////////////////////////////////////////////////////
void CClippingWindows::InitWindows()
{
	DialogBoxDescription clipwindowDescription;
	VECINT vecClipWindows;

	m_vecIntWindows.clear();
	m_vecWindowDescription.clear();

	// 1: half width, half height, top-left
	vecClipWindows.clear();
	clipwindowDescription.hDlg = CreateWindow(	_T("Clipping"),
												_T( "ClippingWindow1" ),
												WS_POPUP,
												0,
												0,
												DEFAULT_WINDOW_WIDTH,
												DEFAULT_WINDOW_HEIGHT,
												m_hWndReset,
												0L,
												0L, 
												0L );
	clipwindowDescription.bAbsolutePosition = false;
	clipwindowDescription.indexXScale = 2;
	clipwindowDescription.indexYScale = 2;
	clipwindowDescription.indexOffset = 1;
	m_vecWindowDescription.push_back( clipwindowDescription );

	// 2: half width, half height, centered
	vecClipWindows.clear();
	clipwindowDescription.hDlg = CreateWindow(	_T("Clipping"),
												_T( "ClippingWindow1" ),
												WS_POPUP,
												0,
												0,
												DEFAULT_WINDOW_WIDTH,
												DEFAULT_WINDOW_HEIGHT,
												m_hWndReset,
												0L,
												0L, 
												0L );
	clipwindowDescription.bAbsolutePosition = false;
	clipwindowDescription.indexXScale = 1;
	clipwindowDescription.indexYScale = 1;
	clipwindowDescription.indexOffset = 0;
	m_vecWindowDescription.push_back( clipwindowDescription );

	// 3: full width, full height, origin
	vecClipWindows.clear();
	clipwindowDescription.hDlg = CreateWindow(	_T("Clipping"),
												_T( "ClippingWindow1" ),
												WS_POPUP,
												0,
												0,
												DEFAULT_WINDOW_WIDTH,
												DEFAULT_WINDOW_HEIGHT,
												m_hWndReset,
												0L,
												0L, 
												0L );
	clipwindowDescription.bAbsolutePosition = false;
	clipwindowDescription.indexXScale = 1;
	clipwindowDescription.indexYScale = 1;
	clipwindowDescription.indexOffset = 1;
	m_vecWindowDescription.push_back( clipwindowDescription );

	// 4: full width, half height, origin
	vecClipWindows.clear();
	clipwindowDescription.hDlg = CreateWindow(	_T("Clipping"),
												_T( "ClippingWindow1" ),
												WS_POPUP,
												0,
												0,
												DEFAULT_WINDOW_WIDTH,
												DEFAULT_WINDOW_HEIGHT,
												m_hWndReset,
												0L,
												0L, 
												0L );
	clipwindowDescription.bAbsolutePosition = false;
	clipwindowDescription.indexXScale = 1;
	clipwindowDescription.indexYScale = 2;
	clipwindowDescription.indexOffset = 1;
	m_vecWindowDescription.push_back( clipwindowDescription );

	// 5: hald width, full height, origin
	vecClipWindows.clear();
	clipwindowDescription.hDlg = CreateWindow(	_T("Clipping"),
												_T( "ClippingWindow1" ),
												WS_POPUP,
												0,
												0,
												DEFAULT_WINDOW_WIDTH,
												DEFAULT_WINDOW_HEIGHT,
												m_hWndReset,
												0L,
												0L, 
												0L );
	clipwindowDescription.bAbsolutePosition = false;
	clipwindowDescription.indexXScale = 2;
	clipwindowDescription.indexYScale = 1;
	clipwindowDescription.indexOffset = 1;
	m_vecWindowDescription.push_back( clipwindowDescription );

	// 6: half width, third height, offset bottom right
	vecClipWindows.clear();
	clipwindowDescription.hDlg = CreateWindow(	_T("Clipping"),
												_T( "ClippingWindow1" ),
												WS_POPUP,
												0,
												0,
												DEFAULT_WINDOW_WIDTH,
												DEFAULT_WINDOW_HEIGHT,
												m_hWndReset,
												0L,
												0L, 
												0L );
	clipwindowDescription.bAbsolutePosition = false;
	clipwindowDescription.indexXScale = 2;
	clipwindowDescription.indexYScale = 4;
	clipwindowDescription.indexOffset = 9;
	m_vecWindowDescription.push_back( clipwindowDescription );

	// 1st window
	vecClipWindows.clear();
	vecClipWindows.push_back(0);
	m_vecIntWindows.push_back( vecClipWindows );

	// 2nd window
	vecClipWindows.clear();
	vecClipWindows.push_back(1);
	m_vecIntWindows.push_back( vecClipWindows );

	// 3rd windows
	vecClipWindows.clear();
	vecClipWindows.push_back(2);
	m_vecIntWindows.push_back( vecClipWindows );

	// 4th windows
	vecClipWindows.clear();
	vecClipWindows.push_back(3);
	m_vecIntWindows.push_back( vecClipWindows );

	// 5th windows
	vecClipWindows.clear();
	vecClipWindows.push_back(4);
	m_vecIntWindows.push_back( vecClipWindows );

	// 6th windows
	vecClipWindows.clear();
	vecClipWindows.push_back(5);
	m_vecIntWindows.push_back( vecClipWindows );

	// windows 1 & 2
	vecClipWindows.clear();
	vecClipWindows.push_back(0);
	vecClipWindows.push_back(1);
	m_vecIntWindows.push_back( vecClipWindows );
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of regions to test
//////////////////////////////////////////////////////////////////////
void CClippingWindows::InitDirtyRegions()
{
	vector<RegionDescription> vecRegDescr;
	RegionDescription RegDescr;

	m_vecRegData.clear();

	// NULL
	vecRegDescr.clear();
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );
	
	// 1 rect: half width, half height, origin
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 2; RegDescr.indexYScale = 2; RegDescr.indexOffset = 1; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// 2 rects both inside: (0,0,50,50), (50,50,100,100)
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 0, 0, 50, 50 ); vecRegDescr.push_back( RegDescr );
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 50, 50, 100, 100 ); vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// 2 rects both clipped: (-31,-24,37,23) (81,76,110,123)
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( -31, -24, 37, 23 ); vecRegDescr.push_back( RegDescr );
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 81, 76, 110, 123 ); vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// these tests are impossible with HighDPI due to rounding/stretching errors, don't even bother
	if( m_fDPIFactor <= 1.0f )
	{
		// 25 rects all inside: (x,y, x+10, y+10) (x+20,y+20, x+20,y+20) etc
		vecRegDescr.clear();
		for( int y = 0; y < 100; y += 20 )
			for( int x = 0; x < 100; x += 20 )
			{
				RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( x, y, x+10, y+10 ); vecRegDescr.push_back( RegDescr );
			}
		m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );
	}
}
