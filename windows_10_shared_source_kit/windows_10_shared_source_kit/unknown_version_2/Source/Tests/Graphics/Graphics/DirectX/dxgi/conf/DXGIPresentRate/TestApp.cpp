//
// Project headers
//

#include <initguid.h>
#include "TestApp.h"


//////////////////////////////////////////////////////////////////////////
// CMyTestApp - An instance of a single object derived from CWGFTestFramework
// is required by the framework.
//////////////////////////////////////////////////////////////////////////
CMyTestApp::CMyTestApp()
    : m_bDFlip(false)
{
};


CMyTestApp::~CMyTestApp()
{
};

	
bool CMyTestApp::InitTestGroups()
{
	// Register test class with the group manager.
	/*
	if(!ADD_TEST(_T("BBDimensions"), clsBBDimensions)) return false;
		return true;
	*/

	if (!ADD_TEST(_T("WinSequential"), CWinSequential)) return false;
    
    if (!ADD_TEST(_T("WinSequentialVaryingSyncInterval"), CWinSequentialVaryingSyncInterval)) return false;

	if (!ADD_TEST(_T("FSSequential"), CFSSequential)) return false;

	if (!ADD_TEST(_T("WinDiscard"), CWinDiscard)) return false;

	if (!ADD_TEST(_T("FSDiscard"), CFSDiscard)) return false;

	return true;
}


void CMyTestApp::InitOptionVariables()
{
	// Call the base class implementation.
	CDXGITestFramework::InitOptionVariables();

    CWGFTestFramework::RegisterOptionVariable
    (
        _T("DFlip"), 
        _T("Provides a way to specify only DirectFlip Testcases should be run, "\
           "only Windowed Tets groups are applicable."), 
        &m_bDFlip, 
        false, 
        0, 
        0, 
        _T("DXGIPresentRate.exe -DFlip [testgroup]")
    );
};


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
}


CMyTestApp g_Application;
