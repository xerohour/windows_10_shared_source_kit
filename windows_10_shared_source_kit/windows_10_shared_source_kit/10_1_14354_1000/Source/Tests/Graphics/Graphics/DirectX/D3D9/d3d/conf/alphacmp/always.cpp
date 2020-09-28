#define D3D_OVERLOADS
#include <d3d.h>
#include "AlphaCmp.h"

//************************************************************************
// Never Test functions

CAlphaNeverTest::CAlphaNeverTest()
{
	m_szTestName = "Alpha Compare Never";
	m_szCommandKey = "Never";

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_NEVER;
}

bool CAlphaNeverTest::SetDefaultRenderStates(void)
{
	// Turn on Alpha testing
	SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, (DWORD)TRUE);

	// Set the Alpha compare func to Never
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_NEVER);
	return true;
}

//************************************************************************
// Always Test functions

CAlphaAlwaysTest::CAlphaAlwaysTest()
{
	m_szTestName = "Alpha Compare Always";
	m_szCommandKey = "Always";

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_ALWAYS;
}

bool CAlphaAlwaysTest::SetDefaultRenderStates(void)
{
	// Turn on Alpha testing
	SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, (DWORD)TRUE);

	// Set the Alpha compare func to Always
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_ALWAYS);
	return true;
}
