#include "TestApp.h"

TestApp g_TestApp;

bool TestApp::InitTestGroups()
{
    if( !ADD_TEST( _T( "Fullscreen" ), CDXGIStereoPresentFS ) ) return false;
    if( !ADD_TEST( _T( "Windowed" ), CDXGIStereoPresentWin ) ) return false;
    if( !ADD_TEST( _T( "DComp" ), CDXGIStereoPresentDComp ) ) return false;

    return true;
}

void TestApp::InitOptionVariables()
{
    CD3D11TestFramework::InitOptionVariables();

    GetD3DDeviceManager()->m_DXGIOptions.Version = TEST_DXGI_VERSION_1_2;
}
