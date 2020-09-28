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
{
};


CMyTestApp::~CMyTestApp()
{
};


bool CMyTestApp::InitTestGroups()
{
	//
	// Register test groups with the group manager.
	//

	if
	(
		!ADD_TEST
		(
			_T("ValidateModeList"),
			CValidateModeListTestGroup
		)
	)
    {
        return false;
    };


	if
	(
		!ADD_TEST
		(
			_T("SwitchToEachMode"),
			CDXGIModeEnum
		)
	)
    {
        return false;
    };

	if
	(
		!ADD_TEST
		(
			_T("ValidateStereoModeList"),
			CValidateStereoModeListTestGroup
		)
	)
    {
        return false;
    };

	if
	(
		!ADD_TEST
		(
			_T("SwitchToEachStereoMode"),
			CDXGIStereoModeEnum
		)
	)
    {
        return false;
    };

	return true;
};


void CMyTestApp::InitOptionVariables()
{
	// Call the base class implementation.
	CDXGITestFramework::InitOptionVariables();
};


#pragma prefast(suppress : 22109 22112)  // Ignore global variable security weaknesses.
CMyTestApp g_Application;
