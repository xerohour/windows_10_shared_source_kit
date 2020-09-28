//
// Project headers
//

#include "TestApp.h"
#include <ParameterHelper.h>

CVerifySecondDeviceUnaffected::CVerifySecondDeviceUnaffected()
{
};


CVerifySecondDeviceUnaffected::~CVerifySecondDeviceUnaffected()
{
};


TEST_RESULT CVerifySecondDeviceUnaffected::Setup()
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


void CVerifySecondDeviceUnaffected::InitTestParameters()
{
    DebugOut.Write("Entering InitTestParameters.\n");


    //
    // Call the base class implementation.
    //

    CDXGITest::InitTestParameters();


    if (InitializeTestGroup())
    {
        UINT uiModeCount = cuiNoOfTempFSModes;

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

        AddParameter(_T("BufferCount"), &m_iNumBuffersIndex, RangeValueSet(3, 3, 1));
        
        AddParameter(_T("FrameLatency"), &m_iMaxFrameLatencyIndex, RangeValueSet(10, 10, 1));

        CUserValueSet< TestType > *pTestType = new CUserValueSet< TestType >();
        pTestType->AddValue(SECOND_DEVICE_CREATED_BEFORE_SET_TESTS);
        pTestType->AddValue(SECOND_DEVICE_CREATED_AFTER_SET_TESTS);
        AddParameter< TestType >( _T("TestType") , &m_TestType, pTestType);

        //
        // Testing single sync internal 1.
        //
        CUserValueSet< int > *pnSyncIntervals= new CUserValueSet< int>();
        pnSyncIntervals->AddValue(1);
        AddParameter<int>(_T("SyncInterval"), &m_nSyncInterval, pnSyncIntervals);


        DebugOut.Write("Exiting InitTestParameters.\n");
    }

};