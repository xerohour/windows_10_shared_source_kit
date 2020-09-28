#include "multisample.h"
#include "fill.h"
#include "FillLine.h"


bool CMultisampleTestApp::InitTestGroups()
{
	if ( RESULT_PASS != SetupDXGIAndD3D() )
	{
		ADD_TEST( _T("NoGroups"), CDummyTest );
		return true;
	}

	// cannot enforce line fill rules on older hardware
	if (g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
	{
		if( !ADD_TEST( _T( "Line" ), FillLine ) ) return false;
	}

	if( !ADD_TEST( _T( "Triangle" ), CMultisampleTest_FillTriangle ) ) return false;
	if( !ADD_TEST( _T( "Point" ), CMultisampleTest_FillPoint ) ) return false;

	CleanupDXGI(); //Cleanup both d3d and dxgi

	return true;
}
