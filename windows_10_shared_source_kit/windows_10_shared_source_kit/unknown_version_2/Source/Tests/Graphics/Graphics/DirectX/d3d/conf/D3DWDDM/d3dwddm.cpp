// Include
#include "d3dwddm.h"
#include "D3DWDDMDriverVersionTest.h"
#include "D3D9DriverRuntimeVersionTest.h"
#include "DetoursTest.h"

// Globals
CD3DWDDMApp g_testApp;

////////////////////////

void CD3DWDDMApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();

	// Indicate this test can run on D3D9 hardware
	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;

	RegisterOptionVariable<bool>( 
		_T("RunOnAllWDDMVersions"), _T("The \"Detours\" group only runs on adapters that support the WDDM 1.3+ driver version. Use this flag to run on all WDDM versions."),
		&m_bRunOnAllWDDMVersions, false, 0, 0, _T("-RunOnAllWDDMVersions") );
}

bool CD3DWDDMApp::InitTestGroups()
{
	// Register test classes with the group manager.
	if( !ADD_TEST( _T( "D3DWDDMDriverVersion" ), CD3DWDDMDriverVersionTest ) ) return false;
	if( !ADD_TEST( _T( "D3D9DriverRuntimeVersion" ), CD3D9DriverRuntimeVersionTest ) ) return false;
	if( !ADD_TEST( _T( "Detours" ), CDetoursTest ) ) return false;

	return true;
}
