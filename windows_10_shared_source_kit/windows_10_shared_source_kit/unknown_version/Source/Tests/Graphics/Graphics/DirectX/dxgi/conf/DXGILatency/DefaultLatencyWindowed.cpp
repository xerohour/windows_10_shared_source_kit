//
// Project headers
//

#include "TestApp.h"
#include <ParameterHelper.h>

CDefaultLatencyWindowed::CDefaultLatencyWindowed()
{
};


CDefaultLatencyWindowed::~CDefaultLatencyWindowed()
{
};


TEST_RESULT CDefaultLatencyWindowed::Setup()
{
	return RESULT_PASS;
};


void CDefaultLatencyWindowed::InitTestParameters()
{
	DebugOut.Write("Entering InitTestParameters.\n");


	//
	// Call the base class implementation
	//

	CDXGITest::InitTestParameters();


	if (InitializeTestGroup())
	{

        InitCommonTestParameters();

        AddParameter(_T("FrameLatency"), &m_iMaxFrameLatencyIndex, RangeValueSet(DEFAULT_FRAME_LATENCY, DEFAULT_FRAME_LATENCY, 1));
        
        //
        // DEFAULT_TEST setting causes us not to call SetMaximumFrameLatency.  Instead we just use the default latency (of 3)
        //
        CUserValueSet< TestType > *pTestType = new CUserValueSet< TestType >();
        pTestType->AddValue(DEFAULT_TESTS);      
        AddParameter< TestType >( _T("TestType") , &m_TestType, pTestType);

        // Cycle on select back buffer counts.
        CUserValueSet< int > *pBufferCounts = new CUserValueSet< int>();
        pBufferCounts->AddValue(2);
        pBufferCounts->AddValue(3);
//        pBufferCounts->AddValue(5);  // Exclude 5 back buffer case to speed things up.
        pBufferCounts->AddValue(8);
        AddParameter<int>(_T("BufferCount"), &m_iNumBuffersIndex, pBufferCounts);

        //
        // Cycle on all supported sync intervals.  Sync intervals greater 
        // than 4 are not supported and will cause Present to fail.
        //
        CUserValueSet< int > *pnSyncIntervals= new CUserValueSet< int>();
        pnSyncIntervals->AddValue(1);
        pnSyncIntervals->AddValue(2);
//        pnSyncIntervals->AddValue(3);  // Exclude interval 3 to speed things up.
        pnSyncIntervals->AddValue(4);
        AddParameter<int>(_T("SyncInterval"), &m_nSyncInterval, pnSyncIntervals);

        AddPriorityPatternFilter(FilterEqual<int>("BufferCount", 3), WeightFactorLarge);

        AddPriorityPatternFilter(FilterEqual<int>("SyncInterval", 1), WeightFactorLarge);
        AddPriorityPatternFilter(FilterEqual<int>("SyncInterval", 2), WeightFactorLarge);

        DeclarePriorityLevelByPriorityValue(1, 3.0);
        DeclarePriorityLevelByPriorityValue(2, 2.0);

	}

	DebugOut.Write("Exiting InitTestParameters.\n");
};

void CDefaultLatencyWindowed::Cleanup()
{
	DebugOut.Write("Entering Cleanup.\n");

	CleanupTestGroup();
	//CleanupWindowClass();

	DebugOut.Write("Exiting Cleanup.\n");
};
