#define D3D_OVERLOADS
#include <d3d.h>
#include "AlphaBld.h"

//************************************************************************
// False Test functions

CFalseTest::CFalseTest()
{
	m_szTestName = "False (SrcAlpha/InvSrcAlpha)";
	m_szCommandKey = "False";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHA;
	dwDestBlend = D3DBLEND_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "InvSrcAlpha";

	// Overwrite Enable variable
	nAlphaEnable = FALSE;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// Alpha Target False Test functions

CAlphaTargetFalseTest::CAlphaTargetFalseTest()
{
	m_szTestName = "Alpha Target False (DestAlpha/InvDestAlpha)";
	m_szCommandKey = "AlphaTargetFalse";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "InvDestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

bool CAlphaTargetFalseTest::PreModeSetup()
{
    // If running in 32bpp use X8R8G8B8, otherwise use default
	if (m_pCurrentMode->pDisplay->dwBPP == 32)
		GetBackBufferFormatIndex(FMT_X8R8G8B8, m_pCommonBackBufferFormats, NULL, (UINT*)&(m_pCurrentMode->nBackBufferFormat));

    return true;
}
