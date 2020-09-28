#define D3D_OVERLOADS
#include <d3d.h>
#include "AlphaCmp.h"

//************************************************************************
// Equal Test functions

CAlphaEqualTest::CAlphaEqualTest()
{
	m_szTestName = "Alpha Compare Equal";
	m_szCommandKey = "Equal";

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_EQUAL;
}

bool CAlphaEqualTest::SetDefaultRenderStates(void)
{
	// Turn on Alpha testing
	SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, (DWORD)TRUE);

	// Set the Alpha compare func to Equal
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_EQUAL);
	return true;
}

//************************************************************************
// NotEqual Test functions

CAlphaNotEqualTest::CAlphaNotEqualTest()
{
	m_szTestName = "Alpha Compare NotEqual";
	m_szCommandKey = "NotEqual";

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_NOTEQUAL;
}

bool CAlphaNotEqualTest::SetDefaultRenderStates(void)
{
	// Turn on Alpha testing
	SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, (DWORD)TRUE);

	// Set the Alpha compare func to NotEqual
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_NOTEQUAL);
	return true;
}
