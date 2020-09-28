#define D3D_OVERLOADS
#include <d3d.h>
#include "AlphaBld.h"

//************************************************************************
// BothSrcAlpha Test functions

CBothSrcAlphaTest::CBothSrcAlphaTest()
{
	m_szTestName = "SrcBlend: BothSrcAlpha";
	m_szCommandKey = "BothSrcAlpha";

	// Tell parent class what cap to look for
	dwSrcCap = D3DPBLENDCAPS_BOTHSRCALPHA;
	dwDestCap = 0xffffffff;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BOTHSRCALPHA;
	dwDestBlend = 0;

	// Give parent a printable name for the caps
	szSrcName = "BothSrcAlpha";
}

//************************************************************************
// BothInvSrcAlpha Test functions

CBothInvSrcAlphaTest::CBothInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: BothInvSrcAlpha";
	m_szCommandKey = "BothSrcAlpha";

	// Tell parent class what cap to look for
	dwSrcCap = D3DPBLENDCAPS_BOTHINVSRCALPHA;
	dwDestCap = 0xffffffff;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BOTHINVSRCALPHA;
	dwDestBlend = 0;

	// Give parent a printable name for the caps
	szSrcName = "BothInvSrcAlpha";
}

