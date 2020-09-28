#define D3D_OVERLOADS
#include <d3d.h>
#include "Address.h"

//************************************************************************
// Clamp Test functions

CClampTest::CClampTest()
{
	m_szTestName = "Texture Address Clamp";
	m_szCommandKey = "Clamp";

    // Init framework options
	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE | PF_ALPHA;
	m_Options.ModeOptions.dwTexExclude = PF_METN;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_CLAMP;
	dwAddressCapV = D3DPTADDRESSCAPS_CLAMP;
}

bool CClampTest::Setup(void)
{
	// Turn on Clamp addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, (DWORD)D3DTADDRESS_CLAMP);

	return CAddressTest::Setup();
}

