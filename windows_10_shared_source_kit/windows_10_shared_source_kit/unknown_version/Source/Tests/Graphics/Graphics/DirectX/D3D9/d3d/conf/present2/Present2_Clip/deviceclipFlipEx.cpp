// DeviceClipFlipEx.cpp: implementation of the CDeviceClipFlipEx class.
//
//////////////////////////////////////////////////////////////////////

#include "DeviceClipFlipEx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeviceClipFlipEx::CDeviceClipFlipEx()
{
	m_TestType		= TEST_DEVICECLIP;
	m_szCommandKey	= _T("DeviceClipFlipEx");
	m_szTestName	= _T("DeviceClipFlipEx");
	m_SwapEffect = SWAPEFFECT_FLIPEX;
    m_nLastIndexPrivateData = -1;
    //added for bug 109414  Flip model test hangs with DWM off
    m_Options.D3DOptions.DisplayMode.Format = FMT_X8R8G8B8; 
}

CDeviceClipFlipEx::~CDeviceClipFlipEx()
{

}

//////////////////////////////////////////////////////////////////////
// Name:	CapsCheck()
// Desc:	check caps
//////////////////////////////////////////////////////////////////////
bool CDeviceClipFlipEx::CapsCheck()
{
	
    bool retVal = CPresentII::CanUseFlipEx();
 
    // do not run this test on single mon systems
	if( m_pD3D->GetAdapterCount() < 2 )
    {
		WriteToLog("Device Clip test not supported when GetAdapterCount < 2 GetAdapterCount=%d" , m_pD3D->GetAdapterCount());
        retVal = false;
    }
	return retVal;
}

//////////////////////////////////////////////////////////////////////
// Name:	InitPresentParameters()
// Desc:	build the vector of present parameters to test
//////////////////////////////////////////////////////////////////////
void CDeviceClipFlipEx::InitPresentParameters()
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
	d3dpp.BackBufferFormat				= m_fmtFrontBuffer;
	d3dpp.uFullscreenRefresh			= D3DPRESENT_RATE_DEFAULT;
	d3dpp.AutoDepthStencilFormat		= FMT_UNKNOWN;
	d3dpp.bAutoDepthStencil				= false;
	d3dpp.MultiSample					= MULTISAMPLE_NONE;

	d3dpp.bCachedBackBuffers			= false;
	d3dpp.DesktopFormat					= m_fmtFrontBuffer;

	//
    // D3DPRESENTFLAG_DEVICECLIP is not supported with flipEx.
    //

    // 9Ex support for D3DPRESENT_VIDEO_RESTRICT_TO_MONITOR
	if( m_pSrcDevice->GetInterface() >= 9.1f )
	{
		// assume that D3DPRESENTFLAG_VIDEO is only used for the new RESTRICT flag
		d3dpp.dwFlags						=  D3DPRESENTFLAG_VIDEO;
		
        for(int i = 0; i < vecRectBackBufferSizes.size(); i++)
        {
            d3dpp.uWidth = vecRectBackBufferSizes[i].right - vecRectBackBufferSizes[i].left;
            d3dpp.uHeight = vecRectBackBufferSizes[i].bottom - vecRectBackBufferSizes[i].top;

            m_vecPresentParameters.push_back( d3dpp );
        }
	}

	d3dpp.dwFlags						=  0;
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
void CDeviceClipFlipEx::InitPresentDestRect()
{
	m_vecRectPresentDest.clear();
	m_vecRectPresentDest.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentSourceRect()
// Desc:	build the vector of present source rects to test
//////////////////////////////////////////////////////////////////////
void CDeviceClipFlipEx::InitPresentSourceRect()
{
	m_vecRectPresentSource.clear();
	m_vecRectPresentSource.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
}

//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of regions to test
//////////////////////////////////////////////////////////////////////
void CDeviceClipFlipEx::InitDirtyRegions()
{
    CPresentII::InitDirtyRegions();
}

//////////////////////////////////////////////////////////////////////
// Name:    InitPresentWindows()
// Desc:    build the vector of Present windows to test
//////////////////////////////////////////////////////////////////////
void CDeviceClipFlipEx::InitPresentWindows()
{
    m_vecPresentWindow.clear();
    m_vecPresentWindow.push_back( NULL );
}
