// PresentInterval.cpp: implementation of the CPresentInterval class.
//
//////////////////////////////////////////////////////////////////////

#include "PresentIntervalWindowedFlipEx.h"
#include "CSwapChain.h"
#include "CSurface.h"
#include <mmsystem.h>

CPresentIntervalWindowedFlipEx::CPresentIntervalWindowedFlipEx()
{
	m_bFullscreen = false;
    m_BackBufferCount = 10;
    m_nMaximumFrameLatency = 20;
    m_nTestIntervals = 500;

    m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
    m_szCommandKey	= _T("PresentIntervalWindowedFlipEx");
    m_szTestName	= _T("PresentInterval - Windowed FlipEx");
}

//////////////////////////////////////////////////////////////////////
// Name: CommandLineHelp()
// Desc: command line help for the test
//////////////////////////////////////////////////////////////////////
void CPresentIntervalWindowedFlipEx::CommandLineHelp()
{

}


bool CPresentIntervalWindowedFlipEx::CapsCheck()
{
	return CPresentInterval::CapsCheck() & CanUseFlipEx();
}

UINT CPresentIntervalWindowedFlipEx::TestInitialize()
{
	
    if(KeySet(_T("WHQL")))
    {
        WriteToLog(_T("Windowed Presentinterval FlipEx tests not are not for WHQL"));
        SetTestRange(1, 1);
    }
    
    return KeySet(_T("WHQL"))? D3DTESTINIT_SKIPALL : CPresentInterval::TestInitialize();

}

void CPresentIntervalWindowedFlipEx::SetTestParameters()
{
    //
    //  add the swapeffects and determine whether or not to use force immediate 
    //
    m_vecSwapEffects.push_back(SWAPEFFECT_FLIPEX);
    m_vecFlagsEx.push_back(0);
    m_vecFlagInterval.clear();
    m_nTests = m_vecPresentIntervals.size() * m_vecSwapEffects.size() * m_vecFlagsEx.size();
    SetTestRange( 1, m_nTests );
}

