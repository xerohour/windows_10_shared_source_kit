//////////////////////////////////////////////////////////////////////////
//  testapp.h
//  Created:	10/03/2007
//
//  Purpose: This is the main test application for testing the functionality
//			of the D3D10on9 feature levels.
//////////////////////////////////////////////////////////////////////////

// Includes
#include "create.h"

// The global test app instance
CTestApp g_TestApp;

CTestApp::CTestApp() : m_minimumFeatureLevel( (D3D_FEATURE_LEVEL) 0 )
{
}

bool CTestApp::InitTestGroups()
{
	// Register test classes with the group manager
	if( !ADD_TEST( _T( "Create" ), CCreateTest ) ) return false;
	
	return true;
}


/////

bool CTestApp::Setup()
{
	// Make sure the minimum specified feature level is 0, 9_1, 9_2, or 9_3
	if( m_minimumFeatureLevel != (D3D_FEATURE_LEVEL) 0 &&
		m_minimumFeatureLevel != D3D_FEATURE_LEVEL_9_1 &&
		m_minimumFeatureLevel != D3D_FEATURE_LEVEL_9_2 &&
		m_minimumFeatureLevel != D3D_FEATURE_LEVEL_9_3 )
	{
		WriteError(_T("Minimum feature level specified on command line must be 9_1, 9_2, or 9_3.  The provided level was %s." ), ToString( m_minimumFeatureLevel ).c_str() );
		m_minimumFeatureLevel = (D3D_FEATURE_LEVEL) 0;
	}
	
	return true;
}

/////

void CTestApp::InitOptionVariables()
{
	CWGFTestFramework::InitOptionVariables();

	// used to be "\\\\davis\\public\\andycamp\\10L9\\FeatureLogs\\"
	RegisterOptionVariable(_T("FeatureLevelLogPath"), _T("Where to store results for video card feature level caps"), &m_FeatureLevelLogPath, tstring("") );

	// Used for ensuring minimum feature level support
	RegisterOptionVariable<D3D_FEATURE_LEVEL>(_T("MinimumFeatureLevel"), _T("Test will fail if device does not support this feature level" ), &m_minimumFeatureLevel, (D3D_FEATURE_LEVEL) 0 );

	UnRegisterOptionVariable("srconly");
	UnRegisterOptionVariable("Ref");
	UnRegisterOptionVariable("OutputRef");
	UnRegisterOptionVariable("AdapterRef");
	//m_D3DOptions.SrcOnly = true;
	return;
}