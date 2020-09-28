// DirtyRegion.cpp: implementation of the CDirtyRegion class.
//
//////////////////////////////////////////////////////////////////////

#include "DirtyRegion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirtyRegion::CDirtyRegion()
{
	m_TestType		= TEST_DIRTYREGION;
	m_szCommandKey	= _T("DirtyRegion");
	m_szTestName	= _T("DirtyRegion");
}

CDirtyRegion::~CDirtyRegion()
{

}


//////////////////////////////////////////////////////////////////////
// Name:	Name()
// Desc:	return current test name
//////////////////////////////////////////////////////////////////////
char* CDirtyRegion::Name( char* szName )
{
	sprintf( szName, "%s (%d,%d,%d)", m_szTestName, m_nIndexPresentParameters, m_nIndexRectSource, m_nIndexDirtyRegion );
	return szName;
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentParameters()
// Desc:	build the vector of present parameters to test
//////////////////////////////////////////////////////////////////////
void CDirtyRegion::InitPresentParameters()
{
	PRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	GetFrontBufferRect();

	m_vecPresentParameters.clear();

	d3dpp.bWindowed						= true;
	d3dpp.uBackBufferCount				= 1;
	d3dpp.SwapEffect					= m_SwapEffect;
	d3dpp.hDeviceWindow					= m_hWndReset;
	d3dpp.dwFlags						= PRESENTFLAG_LOCKABLE_BACKBUFFER;
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

#ifndef FAST_TEST
	// try one color conversion test, too
	if( D3DFMT_A8R8G8B8 == m_fmtFrontBuffer.d3dfFormat || D3DFMT_X8R8G8B8 == m_fmtFrontBuffer.d3dfFormat )
	{
		FMT backbufferFormats[] = 
		{
			FMT_A2R10G10B10,
			FMT_R5G6B5,
			FMT_X1R5G5B5,
			FMT_A1R5G5B5
		};

		for( int iFormat = 0; iFormat < sizeof( backbufferFormats ) / sizeof FMT; iFormat++ )
		{
			d3dpp.BackBufferFormat = backbufferFormats[iFormat];
			if( CheckDeviceFormat ( d3dpp.BackBufferFormat ) )
			{
				m_vecPresentParameters.push_back( d3dpp );
				return;
			}
		}
	}
	else
	{
		FMT backbufferFormats[] = 
		{
			FMT_A2R10G10B10,
			FMT_X8R8G8B8,
			FMT_A8R8G8B8
		};
		for( int iFormat = 0; iFormat < sizeof( backbufferFormats ) / sizeof FMT; iFormat++ )
		{
			d3dpp.BackBufferFormat = backbufferFormats[iFormat];
			if( CheckDeviceFormat ( d3dpp.BackBufferFormat ) )
			{
				m_vecPresentParameters.push_back( d3dpp );
				return;
			}
		}
	}
#endif
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of present dest rects to test
//////////////////////////////////////////////////////////////////////
void CDirtyRegion::InitPresentDestRect()
{
	m_vecRectPresentDest.clear();

	m_vecRectPresentDest.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
}

#ifdef FAST_TEST
//////////////////////////////////////////////////////////////////////
void CDirtyRegion::InitPresentSourceRect()
{
	m_vecRectPresentSource.clear();

	m_vecRectPresentSource.push_back(CreateRect(0, 0, 0, 0));        // NULL
	m_vecRectPresentSource.push_back(CreateRect(1, 1, 6));        // full area, offset on top left
	m_vecRectPresentSource.push_back(CreateRect(1, 1, 4));        // full area, offset top

	m_vecRectPresentSource.push_back(CreateRect(2, 2, 6));        // half width/height, offset on top left
	m_vecRectPresentSource.push_back(CreateRect(5, 5, 9));        // 1 & half height/width, offset bottom right
	m_vecRectPresentSource.push_back(CreateRect(15, 15, 122, 86));// unaligned rect
}
#endif


//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of regions to test
//////////////////////////////////////////////////////////////////////
void CDirtyRegion::InitDirtyRegions()
{
	vector<RegionDescription> vecRegDescr;
	RegionDescription RegDescr;

	m_vecRegData.clear();

	// Index 0 NULL
	vecRegDescr.clear();
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 1. 1 rect: full area, origin
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 1; RegDescr.indexYScale = 1; RegDescr.indexOffset = 1; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 2. 1 rect: full area, offset on top left
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 1; RegDescr.indexYScale = 1; RegDescr.indexOffset = 6; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 3. 1 rect: full area, offset on bottom right
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 1; RegDescr.indexYScale = 1; RegDescr.indexOffset = 9; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 4. 1 rect: full area, offset top right
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 1; RegDescr.indexYScale = 1; RegDescr.indexOffset = 7; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 5. 1 rect: full area, offset bottom left
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 1; RegDescr.indexYScale = 1; RegDescr.indexOffset = 8; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 6. 1 rect: full area, offset top
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 1; RegDescr.indexYScale = 1; RegDescr.indexOffset = 4; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 7. 1 rect: full area, offset bottom
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 1; RegDescr.indexYScale = 1; RegDescr.indexOffset = 5; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 8. 1 rect: full area, offset left
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 1; RegDescr.indexYScale = 1; RegDescr.indexOffset = 2; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 9. 1 rect: full area, offset right
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 1; RegDescr.indexYScale = 1; RegDescr.indexOffset = 3; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 10. 1 rect: half width, half height, origin
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 2; RegDescr.indexYScale = 2; RegDescr.indexOffset = 1; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 11. 1 rect: full width, half height, origin
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 1; RegDescr.indexYScale = 2; RegDescr.indexOffset = 1; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 12. 1 rect: half width, full height, origin
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 2; RegDescr.indexYScale = 1; RegDescr.indexOffset = 1; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 13. 1 rect: half width/height, offset on top left
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 2; RegDescr.indexYScale = 2; RegDescr.indexOffset = 6; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 14. 1 rect: third width/height, offset on bottom right
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 4; RegDescr.indexYScale = 4; RegDescr.indexOffset = 9; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 15. 1 rect: double height/width, offset top left
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 3; RegDescr.indexYScale = 3; RegDescr.indexOffset = 6; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 16. 1 rect: 1 & half height/width, offset bottom right
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = false; RegDescr.indexXScale = 5; RegDescr.indexYScale = 5; RegDescr.indexOffset = 9; vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 17. 2 rects both inside: (0,0,50,50), (50,50,100,100). Percent of window size
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 0, 0, 50, 50 ); vecRegDescr.push_back( RegDescr );
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 50, 50, 100, 100 ); vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 18. 2 rects one outside: (-30, -30, 0, 0) (clipped to 0,0,0,0), (25,25,50,50)
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( -30, -30, 0, 0 ); vecRegDescr.push_back( RegDescr );
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 25, 25, 50, 50 ); vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 19. 2 rects both clipped: (-31,-24,37,23) (81,76,110,123)
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( -31, -24, 37, 23 ); vecRegDescr.push_back( RegDescr );
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 81, 76, 110, 123 ); vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 20. 2 rects both inside, overlapped
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 25, 24, 87, 75 ); vecRegDescr.push_back( RegDescr );
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 45, 35, 101, 99 ); vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 21. 3 rects: 2 clipped, 1 inside: (-50,-10,-20,-5), (10,20,80,70), (5,90,50,150)
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( -50, -10, -20, -5 ); vecRegDescr.push_back( RegDescr );
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 10, 20, 80, 70 ); vecRegDescr.push_back( RegDescr );
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 5, 90, 50, 150 ); vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 22. 3 rects: overlapped
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 10, 10, 50, 50 ); vecRegDescr.push_back( RegDescr );
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 45, 40, 90, 90 ); vecRegDescr.push_back( RegDescr );
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 60, 15, 85, 65 ); vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 23. these tests are impossible with HighDPI due to rounding/stretching errors, don't even bother
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

	
		// 625 rects all inside: (x,y, x+10, y+10) (x+20,y+20, x+20,y+20) etc
		vecRegDescr.clear();
		for( int y = 0; y < 100; y += 4 )
			for( int x = 0; x < 100; x += 4 )
			{
				RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( x, y, x+2, y+2 ); vecRegDescr.push_back( RegDescr );
			}
		m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );
	}
}
