// Include
#include "SystemCheck.h"
#include "MinimumFeatureLevelTest.h"
#include "DiscreteListTest.h"

// Globals
CSystemCheckApp g_testApp;

////////////////////////

void MinimumFeatureLevelCallback( const tstring &FeatureLevel )
{
    if      ( FeatureLevel == _T("9.1") ) g_testApp.SetMinimumFeatureLevel( D3D_FEATURE_LEVEL_9_1 );
    else if ( FeatureLevel == _T("9.2") ) g_testApp.SetMinimumFeatureLevel( D3D_FEATURE_LEVEL_9_2 );
    else if ( FeatureLevel == _T("9.3") ) g_testApp.SetMinimumFeatureLevel( D3D_FEATURE_LEVEL_9_3 );
    else if ( FeatureLevel == _T("10.0")) g_testApp.SetMinimumFeatureLevel( D3D_FEATURE_LEVEL_10_0 );
    else if ( FeatureLevel == _T("10.1")) g_testApp.SetMinimumFeatureLevel( D3D_FEATURE_LEVEL_10_1 );
    else if ( FeatureLevel == _T("11.0")) g_testApp.SetMinimumFeatureLevel( D3D_FEATURE_LEVEL_11_0 );
    else if ( FeatureLevel == _T("11.1")) g_testApp.SetMinimumFeatureLevel( D3D_FEATURE_LEVEL_11_1 );
    else g_testApp.SetMinimumFeatureLevel( D3D_FEATURE_LEVEL_9_1 );
}

////////////////////////

void CSystemCheckApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();

	// Indicate this test can run on D3D9 hardware
	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;

	RegisterOptionVariable<tstring>( "MinimumFeatureLevel", "The minimum required feature level for all adapters in the system."
        , MinimumFeatureLevelCallback, tstring(_T("9.1")), OPTION_VALUE_CONSTANT, 0, _T("-MinimumFeatureLevel:[9.1 (default), 9.2, 9.3, 10.0, 10.1, 11.0, 11.1]") );
}

bool CSystemCheckApp::InitTestGroups()
{
	// Register test classes with the group manager.
	if( !ADD_TEST( _T( "MinimumFeatureLevel" ), CMinimumFeatureLevelTest ) ) return false;
	if( !ADD_TEST( _T( "Hybrid\\DiscreteList" ), CDiscreteListTest ) ) return false;

	return true;
}
