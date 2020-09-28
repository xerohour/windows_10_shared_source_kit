//
// Project headers
//

#include <initguid.h>
#include "TestApp.h"


CMyTestApp::CMyTestApp()
{
};


CMyTestApp::~CMyTestApp()
{
};


void CMyTestApp::InitOptionVariables()
{
	//
	// Call the base class implementation.
	//

	CDXGITestFramework::InitOptionVariables();


	int id = AddOptionCategory("App Specific");
	RegisterOptionVariable("BreakOnFailure", 
		                   "Breaks for debugging after the failure" \
						   "for windows7 bug 43807.)", 
		                   &m_BreakOnFailure, false, 0, id, NULL);
};


bool CMyTestApp::InitTestGroups()
{
	// Register test class with the group manager.

	if (!ADD_TEST(_T("DXGIWaitVBl"), CDXGIWaitVBl)) return false;
		return true;
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
}


CMyTestApp g_Application;
