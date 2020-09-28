//
// Project headers
//

#include "TestApp.h"
#include <ParameterHelper.h>

CDefaultLatencyFS::CDefaultLatencyFS()
{
};


CDefaultLatencyFS::~CDefaultLatencyFS()
{
};


TEST_RESULT CDefaultLatencyFS::Setup()
{
	return RESULT_PASS;
};


void CDefaultLatencyFS::InitTestParameters()
{
	DebugOut.Write("Entering InitTestParameters.\n");


	//
	// Call the base class implementation
	//

	CDXGITest::InitTestParameters();


	if (InitializeTestGroup())
	// if (FindRenderTargetFormats(0, RTFormatsVector))
	{
		//EnumerateModes();
		//uiModeCount = (UINT) TestedModes.size();

		UINT uiModeCount = cuiNoOfTempFSModes;
         DebugOut.Write("Number of modes: %d\n", cuiNoOfTempFSModes);

		if ( uiModeCount > 0 )
		{
			AddParameter(_T("Mode"), &m_iModeIndex, RangeValueSet(0, (int)(uiModeCount - 1), 1));
		}
		else
		{
			m_iModeIndex = -1;
		};
        m_iLastModeIndex = m_iModeIndex;
        
        CUserValueSet< DXGI_SWAP_EFFECT > *pSwapEffect = new CUserValueSet< DXGI_SWAP_EFFECT >();
        pSwapEffect->AddValue(DXGI_SWAP_EFFECT_DISCARD);
        pSwapEffect->AddValue(DXGI_SWAP_EFFECT_SEQUENTIAL); 
        AddParameter< DXGI_SWAP_EFFECT >( _T("SwapEffect") , &m_SwapEffect, pSwapEffect);


        //
        // Test a few different buffer counts
        //
        CUserValueSet< int > *pBufferCounts = new CUserValueSet< int >();
        pBufferCounts->AddValue(1);
        pBufferCounts->AddValue(5); 
        pBufferCounts->AddValue(MAX_BUFFERS); 
        AddParameter(_T("BufferCount"), &m_iNumBuffersIndex, pBufferCounts);

        AddParameter(_T("FrameLatency"), &m_iMaxFrameLatencyIndex, RangeValueSet(DEFAULT_FRAME_LATENCY, DEFAULT_FRAME_LATENCY, 1));
        
        CUserValueSet< TestType > *pTestType = new CUserValueSet< TestType >();
        pTestType->AddValue(DEFAULT_TESTS);      
        AddParameter< TestType >( _T("TestType") , &m_TestType, pTestType);

        //
        // Testing single sync internal 1.
        //
        CUserValueSet< int > *pnSyncIntervals= new CUserValueSet< int>();
        pnSyncIntervals->AddValue(1);
        AddParameter<int>(_T("SyncInterval"), &m_nSyncInterval, pnSyncIntervals);


	}

	DebugOut.Write("Exiting InitTestParameters.\n");
};

void CDefaultLatencyFS::Cleanup()
{
	DebugOut.Write("Entering Cleanup.\n");

	CleanupTestGroup();
	//CleanupWindowClass();

	DebugOut.Write("Exiting Cleanup.\n");
};
