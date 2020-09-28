// PresentIntervalWindowed.h: interface for the CPresentInterval class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <d3d9.h>
#include "PresentInterval.h"
#include <vector>

using namespace std;

class CPresentIntervalWindowed : public CPresentInterval
{
public:
    CPresentIntervalWindowed()
    {
		m_bFullscreen = false;
        m_BackBufferCount = 1;
        m_nMaximumFrameLatency = 3;
        m_nTestIntervals = 1000;
        m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
        m_szCommandKey	= _T("PresentIntervalWindowed");
	    m_szTestName	= _T("PresentInterval - Windowed");
    }
};
