#define D3D_OVERLOADS
#include <d3d.h>
#include "Blend.h"

//************************************************************************
// Default Test functions

CDefaultTest::CDefaultTest()
{
	m_szTestName = "Texture Blend Default";
	m_szCommandKey = "Default";

	// Tell parent class what cap to look for
	dwBlendCap = 0xffffffff;
	dwTextureOpCap = 0xffffffff;
    dwTextureOpCap2 = 0;
}

CDefaultTest::~CDefaultTest()
{
}
  
bool CDefaultTest::SetDefaultRenderStates(void)
{
	// Turn off texture perspection
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	return true;
}
