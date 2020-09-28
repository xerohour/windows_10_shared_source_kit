#define D3D_OVERLOADS
#include <d3d.h>
#include "Mapping.h"

//************************************************************************
// Point Sample Test functions

CMapPointTest::CMapPointTest()
{
	m_szTestName = "Texture Mapping Nearest";
	m_szCommandKey = "Nearest";
}  

bool CMapPointTest::SetDefaultRenderStates(void)
{
	// Tell the parent which caps to look for
    if (m_dwVersion < 0x0800)
    {
        dwMinFilterCap = D3DPTFILTERCAPS_NEAREST;
        dwMagFilterCap = D3DPTFILTERCAPS_NEAREST;
    }
    else
    {
        dwMinFilterCap = D3DPTFILTERCAPS_MINFPOINT;
	    dwMagFilterCap = D3DPTFILTERCAPS_MAGFPOINT;
    }

	// Make sure that perspection is off and use Point Sample filtering
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)false);
	SetRenderState(D3DRENDERSTATE_TEXTUREMAG, (DWORD)D3DFILTER_NEAREST);
	SetRenderState(D3DRENDERSTATE_TEXTUREMIN, (DWORD)D3DFILTER_NEAREST);

	return true;
}
