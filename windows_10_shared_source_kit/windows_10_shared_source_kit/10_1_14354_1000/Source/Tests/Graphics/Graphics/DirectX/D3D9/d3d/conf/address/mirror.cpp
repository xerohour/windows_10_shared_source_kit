#define D3D_OVERLOADS
#include <d3d.h>
#include "Address.h"

//************************************************************************
// Mirror Test functions

CMirrorTest::CMirrorTest()
{
	m_szTestName = "Texture Address Mirror";
	m_szCommandKey = "Mirror";

    // Init framework options
	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE | PF_ALPHA;
	m_Options.ModeOptions.dwTexExclude = PF_METN;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_MIRROR;
	dwAddressCapV = D3DPTADDRESSCAPS_MIRROR;
}

bool CMirrorTest::Setup(void)
{
	// Turn on Mirror addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, (DWORD)D3DTADDRESS_MIRROR);

	return CAddressTest::Setup();
}

