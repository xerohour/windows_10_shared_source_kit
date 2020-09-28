#define D3D_OVERLOADS
#include <d3d.h>
#include "Mapping.h"

//************************************************************************
// Bi-Linear Test functions

CMapLinearTest::CMapLinearTest()
{
	m_szTestName = "Texture Mapping Linear";
	m_szCommandKey = "Linear";
}

bool CMapLinearTest::SetDefaultRenderStates(void)
{
	// Tell the parent which caps to look for
    if (m_dwVersion < 0x0800)
    {
        dwMinFilterCap = D3DPTFILTERCAPS_LINEAR;
        dwMagFilterCap = D3DPTFILTERCAPS_LINEAR;
    }
    else
    {
        dwMinFilterCap = D3DPTFILTERCAPS_MINFLINEAR;
	    dwMagFilterCap = D3DPTFILTERCAPS_MAGFLINEAR;
    }

	// Make sure that perspection is off and use Bi-Linear filtering
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)false);
	SetRenderState(D3DRENDERSTATE_TEXTUREMAG, (DWORD)D3DFILTER_LINEAR);
	SetRenderState(D3DRENDERSTATE_TEXTUREMIN, (DWORD)D3DFILTER_LINEAR);

	return true;
}
