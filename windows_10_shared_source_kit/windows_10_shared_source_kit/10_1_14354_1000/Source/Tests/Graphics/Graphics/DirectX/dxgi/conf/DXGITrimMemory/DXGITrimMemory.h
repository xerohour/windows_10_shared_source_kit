////////////////////////////////////////
// DXGITrimMemory.h

#pragma once

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <dxmini.h>
#include <d3dkmddi.h>
#include <d3dkmthk.h>
#include "AdapterUtil.h"
#include "D3D11Test.h"
#include "WGFTestCore.h"
#include "WGFTestUI.h"
#include "Direct3DFailureCategoryStrings.h"
#include <strsafe.h>
#include <string>

// Useful macros

#define QPC_WITH_ERROR_CHECK( perfCount ) \
	if ( !QueryPerformanceCounter( &perfCount ) )\
	{\
		WriteToLog( "%s in %s at line %d: QueryPeformanceCounter() failed with Error %d.", __FUNCTION__, __FILE__, __LINE__, \
        GetLastError() );\
		tr = RESULT_FAIL;\
		goto Cleanup;\
	}


////////////////////////////////////////////////////////////////////////////////
//CTrimTestApp
////////////////////////////////////////
class CTrimTestApp : public CD3D11TestFramework
{
	virtual TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	virtual bool InitTestGroups();
	virtual void InitOptionVariables();
};

////////////////////////////////////////////////////////////////////////////////
// CTrimTest
////////////////////////////////////////
class CTrimTest : public CD3D11Test
{
public:
	CTrimTest();

	TEST_RESULT Setup();
	virtual void Cleanup();
    
protected:

};

