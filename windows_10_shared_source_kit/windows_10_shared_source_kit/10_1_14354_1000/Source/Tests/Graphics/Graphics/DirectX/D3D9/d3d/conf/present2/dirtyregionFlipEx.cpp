// DirtyRegionFlipEx.cpp: implementation of the CDirtyRegionFlipEx class.
//
//////////////////////////////////////////////////////////////////////

#include "DirtyRegionFlipEx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirtyRegionFlipEx::CDirtyRegionFlipEx()
{
	m_TestType		= TEST_DIRTYREGION;
	m_szCommandKey	= _T("DirtyRegionFlipEx");
	m_szTestName	= _T("DirtyRegionFlipEx");
    m_SwapEffect    = SWAPEFFECT_FLIPEX;
    //added for bug 109414  Flip model test hangs with DWM off
    m_Options.D3DOptions.DisplayMode.Format = FMT_X8R8G8B8; 

}

CDirtyRegionFlipEx::~CDirtyRegionFlipEx()
{

}


//////////////////////////////////////////////////////////////////////
// Name:	CapsCheck()
// Desc:	check caps
//////////////////////////////////////////////////////////////////////
bool CDirtyRegionFlipEx::CapsCheck()
{
    return CanUseFlipEx();
}

//////////////////////////////////////////////////////////////////////
// Name:    InitPresentParameters()
// Desc:    build the vector of present parameters to test
//////////////////////////////////////////////////////////////////////
void CDirtyRegionFlipEx::InitPresentParameters()
{
    PRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    GetFrontBufferRect();

    m_vecPresentParameters.clear();
    vector<RECT> vecRectBackBufferSizes;
    InitBackBufferRects( vecRectBackBufferSizes, m_vecAdapterMonitors[0].m_rectFrontBuffer.right - m_vecAdapterMonitors[0].m_rectFrontBuffer.left, m_vecAdapterMonitors[0].m_rectFrontBuffer.bottom - m_vecAdapterMonitors[0].m_rectFrontBuffer.top  );

    d3dpp.bWindowed                     = true;
    d3dpp.uBackBufferCount              = 1;
    d3dpp.SwapEffect                    = m_SwapEffect;
    d3dpp.hDeviceWindow                 = m_hWndReset;
    d3dpp.dwFlags                       = 0;
    d3dpp.uPresentInterval              = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.BackBufferFormat              = m_fmtFrontBuffer;
    d3dpp.uFullscreenRefresh            = D3DPRESENT_RATE_DEFAULT;
    d3dpp.AutoDepthStencilFormat        = FMT_UNKNOWN;
    d3dpp.bAutoDepthStencil             = false;
    d3dpp.MultiSample                   = MULTISAMPLE_NONE;

    d3dpp.bCachedBackBuffers            = false;
    d3dpp.DesktopFormat                 = m_fmtFrontBuffer;

    for(int i = 0; i < vecRectBackBufferSizes.size(); i++)
    {
        d3dpp.uWidth = vecRectBackBufferSizes[i].right - vecRectBackBufferSizes[i].left;
        d3dpp.uHeight = vecRectBackBufferSizes[i].bottom - vecRectBackBufferSizes[i].top;

        m_vecPresentParameters.push_back( d3dpp );
    }
}

//////////////////////////////////////////////////////////////////////
// Name:	InitPresentDestRect()
// Desc:	build the vector of present dest rects to test
//////////////////////////////////////////////////////////////////////
void CDirtyRegionFlipEx::InitPresentDestRect()
{
	m_vecRectPresentDest.clear();
	m_vecRectPresentDest.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentSourceRect()
// Desc:	build the vector of present source rects to test
//////////////////////////////////////////////////////////////////////
void CDirtyRegionFlipEx::InitPresentSourceRect()
{
	m_vecRectPresentSource.clear();
	m_vecRectPresentSource.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL

}

//////////////////////////////////////////////////////////////////////
// Name:	InitPresentWindows()
// Desc:	build the vector of Present windows to test
//////////////////////////////////////////////////////////////////////
void CDirtyRegionFlipEx::InitPresentWindows()
{
	m_vecPresentWindow.clear();
	m_vecPresentWindow.push_back( NULL );
}

//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of regions to test
//////////////////////////////////////////////////////////////////////
void CDirtyRegionFlipEx::InitDirtyRegions()
{
	vector<RegionDescription> vecRegDescr;
	RegionDescription RegDescr;

	m_vecRegData.clear();

	// Index 0. NULL
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

	//
    // Index 17. 2 rects both inside: (0,0,50,50) and 2nd rect is specific size to evade
    // Dwm's unioning Algorithm in CDirtyRegion2::_Add
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 0, 0, 1, 1  ); vecRegDescr.push_back( RegDescr );
    RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 76, 76, 95, 95  ); vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

    // Index 18. 2 rects one outside: (-30, -30, 0, 0) and 2nd rect is specific size to evade
    // Dwm's unioning Algorithm in CDirtyRegion2::_Add
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( -30, -30, 0, 0 ); vecRegDescr.push_back( RegDescr );
    RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 76, 76, 95, 95  ); vecRegDescr.push_back( RegDescr );
    m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

	// Index 19. 2 rects both clipped: (-31,-24,1,1) and 2nd rect is specific size to evade
    // Dwm's unioning Algorithm in CDirtyRegion2::_Add
	vecRegDescr.clear();
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( -31, -24, 1, 1 ); vecRegDescr.push_back( RegDescr );
	RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( 76, 76, 110, 123 ); vecRegDescr.push_back( RegDescr );
	m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );
    
    
    UINT xStep = 8;
    UINT xSize = 2;
    UINT ySize = 100;
    
    // Index 20. c_maxDirtyRegions Rects to test Dwms limits. Dwm will union these into one
	vecRegDescr.clear();
    for (UINT i = 0; i < LOCAL_c_maxDirtyRegions; i++)
    {
        RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( i * xStep, 0, xSize + (i * xStep), ySize  ); vecRegDescr.push_back( RegDescr );
    }
    m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

    // Index 21.
    // c_maxDirtyRegions + 1 Rects to test Dwms limits. Dwm will union these into one
	vecRegDescr.clear();
    for (UINT i = 0; i < LOCAL_c_maxDirtyRegions + 1; i++)
    {
        RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( i * xStep, 0, xSize + (i * xStep), ySize  ); vecRegDescr.push_back( RegDescr );
    }
    m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

    
    xStep = 5;
    xSize = 2;
    ySize = 100;
    
    // Index 22.
    // D3DKMT_MAX_PRESENT_HISTORY_RECTS to test d3d limits. d3d will send whole surface. 
	vecRegDescr.clear();
    
    for (UINT i = 0; i < LOCAL_D3DKMT_MAX_PRESENT_HISTORY_RECTS; i++)
    {
        RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( i * xStep, 0, xSize + (i * xStep), ySize  ); vecRegDescr.push_back( RegDescr );
    }
    m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );

    // Index 23.
    // D3DKMT_MAX_PRESENT_HISTORY_RECTS + 1 to test d3d limits. d3d will send whole surface. 
	vecRegDescr.clear();
    for (UINT i = 0; i < LOCAL_D3DKMT_MAX_PRESENT_HISTORY_RECTS + 1; i++)
    {
        RegDescr.bAbsoluteRect = true; RegDescr.rect = CreateRect( i * xStep, 0, xSize + (i * xStep), ySize  ); vecRegDescr.push_back( RegDescr );
    }

    m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );  
}

