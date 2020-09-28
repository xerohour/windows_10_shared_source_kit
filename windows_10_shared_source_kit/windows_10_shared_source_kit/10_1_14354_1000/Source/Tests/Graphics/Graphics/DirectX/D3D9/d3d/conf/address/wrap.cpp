#define D3D_OVERLOADS
#include <d3d.h>
#include "Address.h"

//************************************************************************
// Wrap Test functions

CWrapTest::CWrapTest()
{
	m_szTestName = "Texture Address Wrap";
	m_szCommandKey = "Wrap";

    // Init framework options
	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE | PF_ALPHA;
	m_Options.ModeOptions.dwTexExclude = PF_METN;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_WRAP;
	dwAddressCapV = D3DPTADDRESSCAPS_WRAP;
}

bool CWrapTest::Setup(void)
{
	// Turn on Wrap addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, (DWORD)D3DTADDRESS_WRAP);

	return CAddressTest::Setup();
}
