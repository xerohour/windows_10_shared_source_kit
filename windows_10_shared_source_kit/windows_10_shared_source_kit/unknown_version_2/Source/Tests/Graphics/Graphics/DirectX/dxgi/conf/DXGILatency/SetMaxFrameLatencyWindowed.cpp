//
// Project headers
//

#include "TestApp.h"
#include <ParameterHelper.h>

#define MAX_WINDOWED_LATENCY 8  // Limit on the range of MaxFrameLatencies we test.

CSetMaximumFrameLatencyWindowed::CSetMaximumFrameLatencyWindowed()
{
};


CSetMaximumFrameLatencyWindowed::~CSetMaximumFrameLatencyWindowed()
{
};


TEST_RESULT CSetMaximumFrameLatencyWindowed::Setup()
{
	if ( g_Application.m_pCreateDXGIFactory1 == NULL )
	{
        WriteToLog
	    (
		    _T( "Testing Get\\SetMaximumFrameLatency requires DXGIFactory1" )
	    );

        return RESULT_SKIP;    
    }
    
    return RESULT_PASS;
};


void CSetMaximumFrameLatencyWindowed::InitTestParameters()
{
    DebugOut.Write("Entering InitTestParameters.\n");


    //
    // Call the base class implementation.
    //

    CDXGITest::InitTestParameters();


    if (InitializeTestGroup())
    {

        InitCommonTestParameters();        

        //
        // Select a single back buffer count.  We want to use a large number 
        // to ensure that Frame Latency is the limiting factor in queueable frames.
        //
        CUserValueSet< int > *pBufferCounts = new CUserValueSet< int>();
        pBufferCounts->AddValue(16);
        AddParameter<int>(_T("BufferCount"), &m_iNumBuffersIndex, pBufferCounts);

        
        //
        // Do Test a range of latency settings
        //
        AddParameter(_T("FrameLatency"), &m_iMaxFrameLatencyIndex, RangeValueSet(0, MAX_WINDOWED_LATENCY, 1));

        //
        // Adding this causes SetMaximumFrameLatency to be called with the list above.
        //
        CUserValueSet< TestType > *pTestType = new CUserValueSet< TestType >();
        pTestType->AddValue(SET_TESTS);
        AddParameter< TestType >( _T("TestType") , &m_TestType, pTestType);
       
        //
        // Testing a single sync internal of 1 should be sufficient.
        // We cover a broader range of intervals inside the 
        // DefaultLatencyWindowed test.
        //
        CUserValueSet< int > *pnSyncIntervals= new CUserValueSet< int>();
        pnSyncIntervals->AddValue(1);
        AddParameter<int>(_T("SyncInterval"), &m_nSyncInterval, pnSyncIntervals);

        AddPriorityPatternFilter(FilterEqual<int>("FrameLatency", 3), WeightFactorLarge);

        DeclarePriorityLevelByPriorityValue(1, 3.0);
        DeclarePriorityLevelByPriorityValue(2, 2.0);

    }

    DebugOut.Write("Exiting InitTestParameters.\n");

};