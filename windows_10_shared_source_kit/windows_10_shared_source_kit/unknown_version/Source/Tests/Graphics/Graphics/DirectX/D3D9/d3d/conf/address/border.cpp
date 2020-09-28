#define D3D_OVERLOADS
#include <d3d.h>
#include "Address.h"

//************************************************************************
// Border Test functions

CBorderTest::CBorderTest()
{
	m_szTestName = "Texture Address Border";
	m_szCommandKey = "Border";

    // Init framework options
	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC;
	m_Options.ModeOptions.dwTexExclude = PF_METN;

	// Tell parent to use border colors
	bBorder = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_BORDER;
	dwAddressCapV = D3DPTADDRESSCAPS_BORDER;
}

bool CBorderTest::Setup(void)
{
	// Turn on Border addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, (DWORD)D3DTADDRESS_BORDER);

	return CAddressTest::Setup();
}

