//
// ModeEnumeration.hpp
//
// CModeEnumerationTestApp Class definition.
//

#include <d3dx9.h>
#include "WGFTestCore.h"
#include "WGFTestUI.h"
#include "ModeEnumeration.hpp"
#include "Tests.hpp"


//////////////////////////////////////////////////////////////////////////
// CModeEnumerationTestApp - An instance of a single object derived from
// CWGFTestFramework is required by the framework.
//////////////////////////////////////////////////////////////////////////

CModeEnumerationTestApp::CModeEnumerationTestApp()
:
    clsGetDeviceInfo9( D3D10_DRIVER_TYPE_HARDWARE )
{
}


CModeEnumerationTestApp::~CModeEnumerationTestApp()
{
}


bool CModeEnumerationTestApp::InitTestGroups()
{
 
    // Register test class with the group manager.

    if ( !ADD_TEST(_T("ModeEnumeration"), CTests) )
    {
        return false;
    }

    return true;
}


bool CModeEnumerationTestApp::Setup()
{			

    // We are a D3D9 app running in a D3D10 Framework.
    // Convert the D3D10 cmdline parameter result to its D3D9 equivalent.
    if ( m_DriverType == D3D10_DRIVER_TYPE_HARDWARE )
    {
        m_nD3D9DriverType = D3DDEVTYPE_HAL;
    }
    else
    if ( m_DriverType == D3D10_DRIVER_TYPE_REFERENCE )
    {
        m_nD3D9DriverType = D3DDEVTYPE_REF;
    }
    else
    if ( m_DriverType == D3D10_DRIVER_TYPE_NULL )
    {
        m_nD3D9DriverType = D3DDEVTYPE_NULLREF;
    }
    else
    if ( m_DriverType == D3D10_DRIVER_TYPE_SOFTWARE )
    {
        m_nD3D9DriverType = D3DDEVTYPE_SW;
    }

    return true;
}

#pragma prefast(suppress : 22109 22112)  // Ignore global variable security weaknesses.
CModeEnumerationTestApp   g_Application;
