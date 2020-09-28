// GammaPresent.cpp: implementation of the CGammaPresent class.
//
//////////////////////////////////////////////////////////////////////

#include "GammaPresent.h"
#include "CSwapChain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGammaPresent::CGammaPresent()
{
	m_TestType		= TEST_GAMMA;
	m_szCommandKey	= _T("GammaPresent");
	m_szTestName	= _T("GammaPresent");
	m_bGammaSupported	= FALSE;
    m_SwapEffect = SWAPEFFECT_COPY;

	m_Options.D3DOptions.DisplayMode.Format	= FMT_ALL;
}

CGammaPresent::~CGammaPresent()
{

}

//////////////////////////////////////////////////////////////////////
// Name:	Name()
// Desc:	return current test name
//////////////////////////////////////////////////////////////////////
char* CGammaPresent::Name( char* szName )
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
// Desc:	build the vector of presentparameters to test
//////////////////////////////////////////////////////////////////////
void CGammaPresent::InitPresentParameters()
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

	// FMT_A2R10G10B10 for everyone
	d3dpp.BackBufferFormat = FMT_A2R10G10B10; 
	if( CheckDeviceFormat ( d3dpp.BackBufferFormat ) )
		m_vecPresentParameters.push_back( d3dpp );

    // FMT_A16B16G16R16F for everyone
	d3dpp.BackBufferFormat = FMT_A16B16G16R16F; 
	if( CheckDeviceFormat ( d3dpp.BackBufferFormat ) )
		m_vecPresentParameters.push_back( d3dpp );

    
#ifndef FAST_TEST
	// try one color conversion test, too
	if( D3DFMT_A8R8G8B8 == m_fmtFrontBuffer.d3dfFormat || D3DFMT_X8R8G8B8 == m_fmtFrontBuffer.d3dfFormat )
	{
		FMT backbufferFormats[] = 
		{
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
#endif
	{
		FMT backbufferFormats[] = 
		{
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
}


//////////////////////////////////////////////////////////////////////
// Name:	InitTextures()
// Desc:	build the vector of textures to test
//////////////////////////////////////////////////////////////////////
void CGammaPresent::InitTextures()
{
	m_vecStringTexture.clear();
	m_vecStringTexture.push_back( "lake" );
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentSourceRect()
// Desc:	build the vector of present source rects to test
//////////////////////////////////////////////////////////////////////
void CGammaPresent::InitPresentSourceRect()
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
void CGammaPresent::InitPresentDestRect()
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


//////////////////////////////////////////////////////////////////////
// Name:	TestInitialize
// Desc:	Init the testcase
//////////////////////////////////////////////////////////////////////
UINT CGammaPresent::TestInitialize()
{
	// check caps3 for HW support for gamma presentation
	HRESULT hr;
	CAPS caps;
	if( FAILED( hr = m_pSrcDevice->GetDeviceCaps(&caps) ) )
		WriteToLog(
				_T("Error: GetDeviceCaps failed: %s (0x%x). Assuming device cannot perform gamma conversion\n"),
				m_pD3D->HResultToString(hr),
				hr 
			);
	else
	{
		m_lastBBFormat = FMT_UNKNOWN;
		// need to verify the display mode, since gamma present should ONLY be available for X8R8G8B8
		DISPLAYMODE modeDisplay;
		m_pSrcDevice->GetDisplayMode( &modeDisplay );
		if( FMT_X8R8G8B8 == modeDisplay.Format.d3dfFormat )
			WriteToLog( _T( "Mode FMT_X8R8G8B8 is the only desktop mode that supports Gamma Present.\n" ) );
		else
			WriteToLog( _T( "Format %s doesn't support Gamma Present.\n" ), FORMAT2STRING(modeDisplay.Format.d3dfFormat) );

		m_bGammaSupported = (caps.Caps3 & D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION) && FMT_X8R8G8B8 == modeDisplay.Format.d3dfFormat;
		if( m_bGammaSupported )
			WriteToLog( _T("HW supports gamma correction. Test verifies the feature is implemented for compatible backbuffer formats\n") );
		else
			WriteToLog( _T("HW does NOT support gamma correction. Test verifies the feature is NOT implemented.\n") );
	}

	return CPresentII::TestInitialize();
}

//////////////////////////////////////////////////////////////////////
// Name:	Present()
// Desc:	implementation of Present (either using the device's shortcut or swapchain's method)
//////////////////////////////////////////////////////////////////////
void CGammaPresent::Present()
{
	HRESULT hrPresent = E_FAIL;
	CSwapChain *pSwapChain = NULL;
	try
	{
		Check_HRFail( m_pRenderingDevice->GetSwapChain( 0, &pSwapChain ), "GetSwapChain" );
		hrPresent = pSwapChain->Present( m_prectPresentSource, m_prectPresentDest, m_vecPresentWindow[m_nIndexPresentWindow], m_pDirtyRegion, D3DPRESENT_LINEAR_CONTENT );
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


//////////////////////////////////////////////////////////////////////
// Name:	ProcessREFBuffer( UINT )
// Desc:	Do any postprocessing to the REF buffer (like gamma correct it)
//////////////////////////////////////////////////////////////////////
void CGammaPresent::ProcessREFBuffer( UINT uSwapChain )
{
	// if gamma is not supported, just return
	if( !m_bGammaSupported )
		return;
	else	// verify the backbuffer is compatible
		if( D3DFMT_X8R8G8B8 == m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat ||
			D3DFMT_A8R8G8B8 == m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat ||
			D3DFMT_A2R10G10B10 == m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat ||
            D3DFMT_A16B16G16R16F  == m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat )	// friendly formats
		{
			// do we need to print the message?
			if( (D3DFORMAT)m_lastBBFormat != m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat )
			{
				m_lastBBFormat = (FMT)m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat;
				WriteToLog( _T("Gamma conversion supported for friendly back buffer format: %d\n"), m_lastBBFormat );
			}
		}
		else		// unfriendly format
		{
			// do we need to print the message?
			if( (D3DFORMAT)m_lastBBFormat != m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat )
			{
				m_lastBBFormat = (FMT)m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat;
				WriteToLog( _T("Gamma conversion NOT supported for unfriendly back buffer format: %d\n"), m_lastBBFormat );
			}
			return;
        }

        CPresentII::AddGammaToRef(uSwapChain);

}
