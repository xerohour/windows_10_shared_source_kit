//
// ModeEnumeration.hpp
//
// CModeEnumerationTestApp Class declaration.
//

#pragma once

#include <d3d9types.h>
#include "GetDeviceInfo9.h"


//////////////////////////////////////////////////////////////////////////
// CModeEnumerationTestApp - An instance of a single object derived from
// CWGFTestFramework is required by the framework.
//////////////////////////////////////////////////////////////////////////
class CModeEnumerationTestApp : public clsGetDeviceInfo9
{
public:

	// D3D9 driver type.
	D3DDEVTYPE      m_nD3D9DriverType;

    CModeEnumerationTestApp();
    ~CModeEnumerationTestApp();
    
    virtual TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
    
    virtual bool  InitTestGroups();

    virtual bool  Setup();
};


extern CModeEnumerationTestApp  g_Application;
