////////////////////////////////////////
// DXGITrimMemory.cpp
// Author: JeffWick
// Purpose: 
// a) Verifies the time to first frame after a Trim call.
// b) Verifies the driver leaves resources in expected state after Trim call.
////////////////////////////////////////

#include "DXGITrimMemory.h"
#include "DXGITrimMemoryTest.h"

CTrimTestApp g_TestApp;

////////////////////////////////////////////////////////////////////////////////
//
// CTrimTestApp
//
////////////////////////////////////////////////////////////////////////////////

bool CTrimTestApp::InitTestGroups()
{
	ADD_TEST_AND_DESC( "TrimMemoryTest", CTrimMemoryTest, "Verifies the time to first frame after a Trim call, and verifies resources aren't corrupted.");

	return true;
}

void CTrimTestApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();
	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;
}

CTrimTest::CTrimTest()
{
}

TEST_RESULT CTrimTest::Setup()
{
	TEST_RESULT tr = RESULT_PASS;
	return tr;
}

void CTrimTest::Cleanup()
{
    
}
