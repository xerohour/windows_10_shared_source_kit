// PresentIntervalFullScreen.h: interface for the CPresentInterval class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <d3d9.h>
#include "PresentInterval.h"
#include <vector>

using namespace std;
#pragma warning( disable : 4786 4503 )

class CPresentIntervalFullScreen : public CPresentInterval
{
public:
    CPresentIntervalFullScreen()
    {
		m_bFullscreen = true;
        m_BackBufferCount = 1;
        m_nMaximumFrameLatency = 3;
        m_nTestIntervals = 1000;
        m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
        m_szCommandKey	= _T("PresentIntervalFullScreen");
	    m_szTestName	= _T("PresentInterval - FullScreen");
    }
};
