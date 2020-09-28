//
// Project headers
//

#include <initguid.h>
#include "TestApp.h"

#define DO_NOT_WAIT_TEST_DESC  \
    "The DoNotWait test group performs validation on DXGI Present calls with the " \
    "DXGI_PRESENT_DO_NOT_WAIT flag using DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL.  It " \
    "verifies that WAS_STILL_DRAWING is returned at the appropriate time for a " \
    "variety of frame latencies and back buffer counts.  " \
    "Each test case will perform several trials to ensure things are working " \
    "properly.  Some timing dependent failures will tolerated as long as they " \
    "impact a limited number of trials.  " \
    "Note that for the first trial, before any Presents are called, the DWM queue " \
    "is completely empty.  This allows for one additional Present call if we're " \
    "limited by back buffer count and not frame latency.  You may notice in the log file " \
    "that the test expects one more present call on the first trial."

//////////////////////////////////////////////////////////////////////////
// CMyTestApp - An instance of a single object derived from CWGFTestFramework
// is required by the framework.
//////////////////////////////////////////////////////////////////////////
CMyTestApp::CMyTestApp() :
    m_bLoosenTolerances(false),
    m_bDFlip(false)
{
};


CMyTestApp::~CMyTestApp()
{
};
	

bool CMyTestApp::InitTestGroups()
{
	// Register test class with the group manager.

	if (!ADD_TEST(_T("DefaultLatencyWindowed"), CDefaultLatencyWindowed)) return false;

    if (!ADD_TEST(_T("SetMaximumFrameLatencyWindowed"), CSetMaximumFrameLatencyWindowed)) return false;

	if (!ADD_TEST(_T("DefaultLatency"), CDefaultLatencyFS)) return false;

    if (!ADD_TEST(_T("SetMaximumFrameLatency"), CSetMaximumFrameLatencyFS)) return false;
    
    if (!ADD_TEST(_T("VerifySecondDeviceUnaffected"), CVerifySecondDeviceUnaffected)) return false;

    if (!ADD_TEST_AND_DESC(_T("DoNotWait"), CDoNotWaitTest, _T(DO_NOT_WAIT_TEST_DESC))) return false;

	return true;
}

void CMyTestApp::InitOptionVariables()
{
    //
    // Call the base class implementation.
    //

    CDXGITestFramework::InitOptionVariables();


    //
    // Register test-specific command line options.
    //

    //
    // Add an option to loosen test tolerances.  This is intentionally vague and global.
    // It's exact interpretation will vary from one test group/case to another.
    // It was initially created to work-around windows 8 bug #71861
    // Not all test cases respect it.
    //
    int CategoryTolerances = AddOptionCategory(_T("Tolerances"));

    RegisterOptionVariable
        (
        _T("LoosenTolerances"), 
        _T("Loosen's test tolerances to work around Win8 bug #71861."), 
        &m_bLoosenTolerances,
        false,
        OPTION_VALUE_CONSTANT,
        CategoryTolerances
        );

    RegisterOptionVariable
    (
        _T("DFlip"), 
        _T("Provides a way to specify only DirectFlip Testcases should be run, "\
            "only Windowed Test groups are applicable."), 
        &m_bDFlip, 
        false, 
        0, 
        0, 
        _T("DXGILatency.exe -DFlip [testgroup]")
    );
}


bool CMyTestApp::Setup()
{
    if ( g_Application.m_D3DOptions.DriverType == D3D10_DRIVER_TYPE_HARDWARE
    ||   g_Application.m_D3DOptions.DriverType == D3D10_DRIVER_TYPE_REFERENCE)
	{
		return true;
	};


	WriteToLog
	(
		"Skipping all test groups.  This test must be executed against a D3D10 HAL device."
	);

	return false;
};

CMyTestApp g_Application;
