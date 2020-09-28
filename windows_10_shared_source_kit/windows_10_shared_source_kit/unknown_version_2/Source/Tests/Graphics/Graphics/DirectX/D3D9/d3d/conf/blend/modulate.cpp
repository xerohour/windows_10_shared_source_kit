#define D3D_OVERLOADS
#include <d3d.h>
#include "Blend.h"

//************************************************************************
// Modulate Test functions

CModulateTest::CModulateTest()
{
	m_szTestName = "Texture Blend Modulate";
	m_szCommandKey = "Modulate";

	// Tell parent class what cap to look for
	dwBlendCap = D3DPTBLENDCAPS_MODULATE;
    dwTextureOpCap = D3DTEXOPCAPS_SELECTARG1;
    dwTextureOpCap2 = D3DTEXOPCAPS_MODULATE;

  	// Give parent a printable name for the caps
    szTextureOp = "SelectArg1";
    szTextureOp2 = "Modulate";

	// Tell parent what state we are
	dwBlendState = D3DTBLEND_MODULATE;
}

CModulateTest::~CModulateTest()
{
}
  
bool CModulateTest::SetDefaultRenderStates(void)
{
	// Turn on Modulate blending
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);

	if (m_dwVersion <= 0x0600)
		SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)D3DTBLEND_MODULATE);

	return true;
}
  
//************************************************************************
// ModulateAlpha Test functions

CModulateAlphaTest::CModulateAlphaTest()
{
	m_szTestName = "Texture Blend ModulateAlpha";
	m_szCommandKey = "ModulateAlpha";

	// Tell parent class what cap to look for
	dwBlendCap = D3DPTBLENDCAPS_MODULATEALPHA;
    dwTextureOpCap = D3DTEXOPCAPS_MODULATE;
    dwTextureOpCap2 = 0;

  	// Give parent a printable name for the caps
    szTextureOp = "Modulate";

	// Tell parent what state we are
	dwBlendState = D3DTBLEND_MODULATEALPHA;
}

CModulateAlphaTest::~CModulateAlphaTest()
{
}

bool CModulateAlphaTest::SetDefaultRenderStates(void)
{
	// Turn on ModulateAlpha blending
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);

	if (m_dwVersion <= 0x0600)
		SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)D3DTBLEND_MODULATEALPHA);

	return true;
}

