//////////////////////////////////////////////////////////////////////////
//  WGFInterfaces.cpp
//  created:	2009/04/17
//
//  Description: D3D11 Interfaces provides a way for the developer to 
//		change the behavior of a shader at runtime.  Different variations 
//		of a function can be written in separate classes and then
//		embedded in the shader.  At runtime, the application can pick 
//		which implementation the shader uses and reduce the overall 
//		need for managing huge numbers of shaders in a library.
//////////////////////////////////////////////////////////////////////////

// Includes
#include "WGFInterfaces.h"
#include "CInterfaceFlowControl.h"
#include "ResourceIndexing.h"
#include "FCallLimits.h"

// Globals
CInterfacesTestApp  g_TestApp;

///////////////////////////////////////////////////////////////////////////////
//	Base class and the application class for WGFInterfaces test
///////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------------
// CInterfacesTest
//------------------------------------------------------------------------------------------------

TEST_RESULT CInterfacesTest::Setup()
{
	// Local variables
	HRESULT hr = S_OK;
	TEST_RESULT tRes = RESULT_PASS;

	// Interfaces is only supported on FL11+
	if( g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
		tRes = RESULT_SKIP;

	/// Create the class linkage
	if( FAILED( hr = GetDevice()->CreateClassLinkage( &m_pClassLinkage ) ) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("CInterfacesTest::Setup() - CreateClassLinkage() failed. HR = %s"), D3DHResultToString(hr).c_str());
		goto setupDone;
	}

setupDone:
	return tRes;
}

//------------------------------------------------------------------------------------------------

TEST_RESULT CInterfacesTest::SetupTestCase()
{
	// Local variables
	TEST_RESULT tRes = RESULT_PASS;

	return tRes;
}

//------------------------------------------------------------------------------------------------

void CInterfacesTest::CleanupTestCase()
{
	// No need to cleanup COM auto pointers...
}

//------------------------------------------------------------------------------------------------

void CInterfacesTest::Cleanup()
{
	SAFE_RELEASE( m_pClassLinkage );
}

//------------------------------------------------------------------------------------------------
// CInterfacesTestApp
//------------------------------------------------------------------------------------------------

bool CInterfacesTestApp::InitTestGroups()
{
	// Register test classes with the group manager.
	if( !ADD_TEST( _T( "IndexingResources\\ConstantBuffers" ), CConstantBufferIndexing ) ) return false;
	if( !ADD_TEST( _T( "IndexingResources\\Textures" ), CTextureIndexing ) ) return false;
	if( !ADD_TEST( _T( "IndexingResources\\Samplers" ), CSamplerIndexing ) ) return false;
//	if( !ADD_TEST( _T( "FlowControl" ), CInterfaceFlowControl ) ) return false;
	if( !ADD_TEST( _T( "FcallLimits" ), CFcallLimits ) ) return false;

	return true;
}

//------------------------------------------------------------------------------------------------

void CInterfacesTestApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();
	UnRegisterOptionVariable("srconly");
	m_D3DOptions.SrcOnly = true;

	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	m_D3DOptions.wMaxFeatureLevel = D3D_FEATURE_LEVEL_11_1;

	RegisterOptionVariable("LogShaderText", "Log the HLSL text of the shader for each test case", &m_bLogShaderText, false);
}
