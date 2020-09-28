#include "testapp.h"
#include "views.h"
#include "state.h"
#include "clamp.h"
#include "bias.h"
#include "stencil.h"


CDepthStencilTestApp g_TestApp;

/******************************
* CDepthStencilTestApp methods
******************************/

void CDepthStencilTestApp::InitOptionVariables( )
{
	CD3D11TestFramework::InitOptionVariables();
	int n = AddOptionCategory( _T( "Debug" ) );

	UnRegisterOptionVariable("srconly");
	UnRegisterOptionVariable("Ref");
	UnRegisterOptionVariable("OutputRef");
	UnRegisterOptionVariable("AdapterRef");

	if (GetShell()->IsConsole())
	{
		m_bPresent = true; // Present() will dump to file if "-viewresources" is used
	}
	else
	{
		RegisterOptionVariable( _T( "Present" ), _T( "Causes the tests to actually Present the render target." ), &m_bPresent, false, 0, n );
	}
	m_D3DOptions.SrcOnly = true;
}

//---------------------------------------------------------------

bool CDepthStencilTestApp::InitTestGroups( )
{
	// depth
	if( !ADD_TEST( _T( "Depth/Bias" ), CBiasTest ) ) return false;
	if( !ADD_TEST( _T( "Depth/Clamp" ), CClampTest ) ) return false;
	if( !ADD_TEST( _T( "Depth/State" ), CStateTest ) ) return false;
	if( !ADD_TEST( _T( "Depth/StateStencilRO" ), CStateStencilROTest ) ) return false;
	if( !ADD_TEST( _T( "Depth/StateLimits" ), CStateLimitsTest ) ) return false;

	// views
	if( !ADD_TEST( _T( "Clear" ), CDepthStencilClearTest ) ) return false;
	if( !ADD_TEST( _T( "Views" ), CDepthStencilTest ) ) return false;

	if( !ADD_TEST( _T( "Stencil/Ops" ), CStencilOps ) ) return false;
	if( !ADD_TEST( _T( "Stencil/OpsDepthRO" ), CStencilOpsDepthRO ) ) return false;
	if( !ADD_TEST( _T( "Stencil/DepthOps" ), CStencilDepthOp ) ) return false;
	if( !ADD_TEST( _T( "Stencil/DepthOpsDepthRO" ), CStencilDepthOpDepthRO ) ) return false;
	if( !ADD_TEST( _T( "Stencil/Funcs" ), CStencilFuncs ) ) return false;
	if( !ADD_TEST( _T( "Stencil/FuncsDepthRO" ), CStencilFuncsDepthRO ) ) return false;
	if( !ADD_TEST( _T( "Stencil/Mask" ), CStencilMask ) ) return false;
	if( !ADD_TEST( _T( "Stencil/MaskDepthRO" ), CStencilMaskDepthRO ) ) return false;
		
	return true;
}