// ColorConvertingFlipEx.cpp: implementation of the CColorConvertingFlipEx class.
//
//////////////////////////////////////////////////////////////////////

#include "ColorConvertingFlipEx.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColorConvertingFlipEx::CColorConvertingFlipEx()
{
	m_TestType		= TEST_COLOR_CONVERSION;
	m_szCommandKey	= _T("ColorConvertingFlipEx");
	m_szTestName	= _T("ColorConvertingFlipEx");

	m_SwapEffect = SWAPEFFECT_FLIPEX;
    m_Options.D3DOptions.DisplayMode.Format = FMT_ALL;
    //m_Options.D3DOptions.DisplayMode.Format = FMT_X8R8G8B8; 
    m_Options.D3DOptions.dwIncVersions = DXVERSION_91;
	m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
    m_Options.D3DOptions.dwSwapEffect = SWAPEFFECT_FLIP;
    m_Options.ModeOptions.BackBufferFmt = FMT_X8R8G8B8;
    m_Options.D3DOptions.dwPresentFlags = 0;
    m_Options.TestOptions.bDisableDWM = false;
}

CColorConvertingFlipEx::~CColorConvertingFlipEx()
{

}

//////////////////////////////////////////////////////////////////////
// Name:	CapsCheck()
//////////////////////////////////////////////////////////////////////
bool CColorConvertingFlipEx::CapsCheck()
{
	return CPresentII::CanUseFlipEx();
}

//////////////////////////////////////////////////////////////////////
// Name:	InitPresentParameters()
// Desc:	build the vector of present parameters to test
//////////////////////////////////////////////////////////////////////
void CColorConvertingFlipEx::InitPresentParameters()
{
	PRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	GetFrontBufferRect();

	m_vecPresentParameters.clear();
    vector<RECT> vecRectBackBufferSizes;
    InitBackBufferRects( vecRectBackBufferSizes, m_vecAdapterMonitors[0].m_rectFrontBuffer.right - m_vecAdapterMonitors[0].m_rectFrontBuffer.left, m_vecAdapterMonitors[0].m_rectFrontBuffer.bottom - m_vecAdapterMonitors[0].m_rectFrontBuffer.top  );


	d3dpp.bWindowed						= true;
	d3dpp.uBackBufferCount				= 1;
	d3dpp.SwapEffect					= m_SwapEffect;
	d3dpp.hDeviceWindow					= m_hWndReset;
	d3dpp.dwFlags						= 0;
	d3dpp.uPresentInterval				= D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.uFullscreenRefresh			= D3DPRESENT_RATE_DEFAULT;
	d3dpp.AutoDepthStencilFormat		= FMT_UNKNOWN;
	d3dpp.bAutoDepthStencil				= false;
	d3dpp.MultiSample					= MULTISAMPLE_NONE;

	d3dpp.bCachedBackBuffers			= false;
	d3dpp.DesktopFormat					= m_fmtFrontBuffer;
    
	FMT backbufferFormats[] = 
	{
		//FMT_X8R8G8B8,
		//FMT_A8R8G8B8,
		//FMT_A2R10G10B10, 
        FMT_A2B10G10R10,
		FMT_A16B16G16R16F 

	};

	for( int iFormat = 0; iFormat < sizeof( backbufferFormats ) / sizeof FMT; iFormat++ )
	{
		d3dpp.BackBufferFormat = backbufferFormats[iFormat];
		if( SuppressAlphaChannel( m_fmtFrontBuffer ).d3dfFormat != SuppressAlphaChannel( d3dpp.BackBufferFormat ).d3dfFormat && CheckDeviceFormat ( d3dpp.BackBufferFormat ) )
        {			
            for(int i = 0; i < vecRectBackBufferSizes.size(); i++)
            {
                d3dpp.uWidth = vecRectBackBufferSizes[i].right - vecRectBackBufferSizes[i].left;
	            d3dpp.uHeight = vecRectBackBufferSizes[i].bottom - vecRectBackBufferSizes[i].top;

                m_vecPresentParameters.push_back( d3dpp );
            }
        }
	}
}

//////////////////////////////////////////////////////////////////////
// Name:	InitPresentSourceRect()
// Desc:	build the vector of present source rects to test
//////////////////////////////////////////////////////////////////////
void CColorConvertingFlipEx::InitPresentSourceRect()
{
	m_vecRectPresentSource.clear();
	m_vecRectPresentSource.push_back( CreateRect( 0, 0, 0, 0 ) );	// NULL
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of present dest rects to test
//////////////////////////////////////////////////////////////////////
void CColorConvertingFlipEx::InitPresentDestRect()
{
	m_vecRectPresentDest.clear();
	m_vecRectPresentDest.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
}

//////////////////////////////////////////////////////////////////////
// Name:	InitPresentWindows()
// Desc:	build the vector of Present windows to test
//////////////////////////////////////////////////////////////////////
void CColorConvertingFlipEx::InitPresentWindows()
{
	m_vecPresentWindow.clear();
	m_vecPresentWindow.push_back( NULL );
}

//////////////////////////////////////////////////////////////////////
// Name:    ProcessREFBuffer( UINT )
// Desc:    Do any postprocessing to the REF buffer (like gamma correct it)
//////////////////////////////////////////////////////////////////////
void CColorConvertingFlipEx::ProcessREFBuffer( UINT uSwapChain )
{
    if( FMT_A16B16G16R16F == m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat && IsDWMEnabled())
    {
        AddGammaToRef(uSwapChain);
    }
}
