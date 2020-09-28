// PresentInterval.h: interface for the CPresentInterval class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__CPRESENTIMMEDIATE_H__)
#define __CPRESENTIMMEDIATE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <d3d9.h>
#include "CD3DTest.h"
#include <vector>

using namespace std;
#pragma warning( disable : 4786 4503 )
typedef vector<int> VECINT;

class CPresentInterval : public CD3DTest
{
public:
	CPresentInterval();
    virtual ~CPresentInterval(){};

	virtual bool	CapsCheck();
	virtual UINT	TestInitialize();
    virtual bool	TestTerminate();

	virtual bool	ExecuteTest(UINT);
	virtual void	CommandLineHelp();
    virtual void    SetTestParameters();
    virtual bool    DisplayFrame(){ return true;};


    virtual bool    VerifyPresentInterval(
                        float ActualPresentInterval,
                        float ExpectedPresentInterval,
                        UINT uPresentIntervalsID);

    vector<UINT>	m_vecPresentIntervals;
    vector<DWORD>	m_vecSwapEffects;
    vector<DWORD>	m_vecFlagsEx;
    vector<DWORD>	m_vecFlagInterval;

private:
    
protected:
	void			Check_HRFail( HRESULT hResult, const char* sMethod );
    char*           INTERVAL2STRING(UINT uInterval);
    char*           SWAPEFFECT2STRING(UINT uSwapEffect);
    bool            CanUseFlipEx();
    
    float           m_fRefreshInterval;
    float           m_fAllowedVariancePrecent;
    UINT            m_nTestIntervals;
	bool			m_bFullscreen;
    UINT            m_nTests;
    UINT            m_BackBufferCount;
    UINT            m_nMaximumFrameLatency;
	float           m_LastExpectedRefreshInterval;
};

#endif
