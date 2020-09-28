//
// GetDispSurfData.cpp
//
// CGetDispSurfDataTestApp Class definition.
//

//
// Project headers
//

#include "Tests.hpp"


//////////////////////////////////////////////////////////////////////////
// CGetDispSurfDataTestApp - An instance of a single object derived from
// CWGFTestFramework is required by the framework.
//////////////////////////////////////////////////////////////////////////

CGetDispSurfDataTestApp::CGetDispSurfDataTestApp()
{
}


CGetDispSurfDataTestApp::~CGetDispSurfDataTestApp()
{
}


bool CGetDispSurfDataTestApp::InitTestGroups()
{
 
    // Register test class with the group manager.

    if ( !ADD_TEST(_T("GetDispSurfData"), CTests) )
    {
        return false;
    }

    return true;
}


bool CGetDispSurfDataTestApp::Setup()
{			
    return true;
}

#pragma prefast(suppress : 22109 22112)  // Ignore global variable security weaknesses.
CGetDispSurfDataTestApp   g_Application;
