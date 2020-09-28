// PresentInterval.cpp: implementation of the CPresentInterval class.
//
//////////////////////////////////////////////////////////////////////

#include "PresentintervalWindowedFlipExFlags.h"
#include "CSwapChain.h"
#include "CSurface.h"
#include <mmsystem.h>

CPresentintervalWindowedFlipExFlags::CPresentintervalWindowedFlipExFlags()
{
	m_bFullscreen = false;
    m_BackBufferCount = D3DPRESENT_BACK_BUFFERS_MAX_EX;
    m_nMaximumFrameLatency = 20;
    m_nTestIntervals = 500;

    m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
    m_szCommandKey	= _T("PresentintervalWindowedFlipExFlags");
    m_szTestName	= _T("PresentInterval - Windowed FlipEx with present flags");
}

//////////////////////////////////////////////////////////////////////
// Name: CommandLineHelp()
// Desc: command line help for the test
//////////////////////////////////////////////////////////////////////
void CPresentintervalWindowedFlipExFlags::CommandLineHelp()
{

}


bool CPresentintervalWindowedFlipExFlags::CapsCheck()
{
	return CPresentInterval::CapsCheck() & CanUseFlipEx();
}

UINT CPresentintervalWindowedFlipExFlags::TestInitialize()
{
	
    if(KeySet(_T("WHQL")))
    {
        WriteToLog(_T("Windowed Presentinterval FlipEx Flags tests not are not for WHQL"));
        SetTestRange(1, 1);
    }
    
    return KeySet(_T("WHQL"))? D3DTESTINIT_SKIPALL : CPresentInterval::TestInitialize();

}

void CPresentintervalWindowedFlipExFlags::SetTestParameters()
{

    //
    //  add the swapeffects and determine whether or not to use force immediate 
    //
    m_vecSwapEffects.push_back(SWAPEFFECT_FLIPEX);
    
    //
    // Present flags
    //
    m_vecFlagsEx.push_back(D3DPRESENT_FORCEIMMEDIATE);
    m_vecFlagsEx.push_back(D3DPRESENT_DONOTFLIP);
    
    m_vecFlagInterval.push_back(2);
    m_vecFlagInterval.push_back(6);

    //
    // No presents w/o flags
    //
    m_vecFlagInterval.push_back(m_nTestIntervals +1);
    
    m_nTests = m_vecPresentIntervals.size() * m_vecSwapEffects.size() * m_vecFlagsEx.size() * m_vecFlagInterval.size();
    SetTestRange( 1, m_nTests );
}

