// ColorConverting.cpp: implementation of the CColorConverting class.
//
//////////////////////////////////////////////////////////////////////

#include "ColorConverting.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColorConverting::CColorConverting()
{
	m_TestType		= TEST_COLOR_CONVERSION;
	m_szCommandKey	= _T("ColorConverting");
	m_szTestName	= _T("ColorConverting");

	m_Options.D3DOptions.DisplayMode.Format	= FMT_ALL;
    m_SwapEffect = SWAPEFFECT_COPY;
}

CColorConverting::~CColorConverting()
{

}


//////////////////////////////////////////////////////////////////////
// Name:	Name()
// Desc:	return current test name
//////////////////////////////////////////////////////////////////////
char* CColorConverting::Name( char* szName )
{
    TCHAR swapEffect[MAX_PATH] = {0};
    SWAPEFFECT2STRING(m_vecPresentParameters[m_nIndexPresentParameters].SwapEffect, swapEffect, MAX_PATH);

    sprintf( szName, "%s (%s,%s, Test Index %d, Texture Index %d,Source Rect Index %d,Dest Rect Index %d)", m_szTestName, 
											FORMAT2STRING( m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat ),
                                            swapEffect,
											m_nIndexPresentParameters, 
											m_nIndexTexture, 
											m_nIndexRectSource, 
											m_nIndexRectDest );
	return szName;
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentParameters()
// Desc:	build the vector of present parameters to test
//////////////////////////////////////////////////////////////////////
void CColorConverting::InitPresentParameters()
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
	d3dpp.uFullscreenRefresh			= D3DPRESENT_RATE_DEFAULT;
	d3dpp.AutoDepthStencilFormat		= FMT_UNKNOWN;
	d3dpp.bAutoDepthStencil				= false;
	d3dpp.MultiSample					= MULTISAMPLE_NONE;

	d3dpp.bCachedBackBuffers			= false;
	d3dpp.DesktopFormat					= m_fmtFrontBuffer;

	FMT backbufferFormats[] = 
	{
		FMT_X8R8G8B8,
#ifndef FAST_TEST
		FMT_A8R8G8B8,
		FMT_R5G6B5,
		FMT_X1R5G5B5,
		FMT_A1R5G5B5,
		FMT_A2R10G10B10,
        FMT_A16B16G16R16F 
#endif
	};

	for( int iFormat = 0; iFormat < sizeof( backbufferFormats ) / sizeof FMT; iFormat++ )
	{
		d3dpp.BackBufferFormat = backbufferFormats[iFormat];
		if( SuppressAlphaChannel( m_fmtFrontBuffer ).d3dfFormat != SuppressAlphaChannel( d3dpp.BackBufferFormat ).d3dfFormat && CheckDeviceFormat ( d3dpp.BackBufferFormat ) )
			m_vecPresentParameters.push_back( d3dpp );
	}
}


//////////////////////////////////////////////////////////////////////
// Name:	InitTextures()
// Desc:	build the vector of textures to test
//////////////////////////////////////////////////////////////////////
void CColorConverting::InitTextures()
{
	m_vecStringTexture.clear();
	m_vecStringTexture.push_back( "lake" );
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentSourceRect()
// Desc:	build the vector of present source rects to test
//////////////////////////////////////////////////////////////////////
void CColorConverting::InitPresentSourceRect()
{
	m_vecRectPresentSource.clear();

	m_vecRectPresentSource.push_back( CreateRect( 0, 0, 0, 0 ) );	// NULL
	m_vecRectPresentSource.push_back( CreateRect( 1, 1, 6 ) );		// full area, offset on top left
#ifndef FAST_TEST
	m_vecRectPresentSource.push_back( CreateRect( 1, 1, 9 ) );		// full area, offset on bottom right
	m_vecRectPresentSource.push_back( CreateRect( 1, 1, 7 ) );		// full area, offset top right
	m_vecRectPresentSource.push_back( CreateRect( 1, 1, 8 ) );		// full area, offset bottom left
	m_vecRectPresentSource.push_back( CreateRect( 2, 2, 1 ) );		// half width, half height, origin
	m_vecRectPresentSource.push_back( CreateRect( 1, 2, 1 ) );		// full width, half height, origin
	m_vecRectPresentSource.push_back( CreateRect( 2, 1, 1 ) );		// half width, full height, origin
	m_vecRectPresentSource.push_back( CreateRect( 2, 2, 6 ) );		// half width/height, offset on top left
	m_vecRectPresentSource.push_back( CreateRect( 4, 4, 9 ) );		// third width/height, offset on bottom right
	m_vecRectPresentSource.push_back( CreateRect( 3, 3, 6 ) );		// double height/width, offset top left
	m_vecRectPresentSource.push_back( CreateRect( 5, 5, 9 ) );		// 1 & half height/width, offset bottom right
	m_vecRectPresentSource.push_back( CreateRect( 15, 15, 122, 86 ) );// unaligned rect
#endif
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of present dest rects to test
//////////////////////////////////////////////////////////////////////
void CColorConverting::InitPresentDestRect()
{
	m_vecRectPresentDest.clear();

	m_vecRectPresentDest.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
	m_vecRectPresentDest.push_back( CreateRect( 1, 1, 6 ) );		// full area, offset on top left
#ifndef FAST_TEST
	m_vecRectPresentDest.push_back( CreateRect( 1, 1, 9 ) );		// full area, offset on bottom right
	m_vecRectPresentDest.push_back( CreateRect( 1, 1, 7 ) );		// full area, offset top right
	m_vecRectPresentDest.push_back( CreateRect( 1, 1, 8 ) );		// full area, offset bottom left
	m_vecRectPresentDest.push_back( CreateRect( 2, 2, 1 ) );		// half width, half height, origin
	m_vecRectPresentDest.push_back( CreateRect( 1, 2, 1 ) );		// full width, half height, origin
	m_vecRectPresentDest.push_back( CreateRect( 2, 1, 1 ) );		// half width, full height, origin
	m_vecRectPresentDest.push_back( CreateRect( 2, 2, 6 ) );		// half width/height, offset on top left
	m_vecRectPresentDest.push_back( CreateRect( 4, 4, 9 ) );		// third width/height, offset on bottom right
	m_vecRectPresentDest.push_back( CreateRect( 3, 3, 6 ) );		// double height/width, offset top left
	m_vecRectPresentDest.push_back( CreateRect( 5, 5, 9 ) );		// 1 & half height/width, offset bottom right
	m_vecRectPresentDest.push_back( CreateRect( 15, 15, 122, 86 ) );// unaligned rect
#endif
}
