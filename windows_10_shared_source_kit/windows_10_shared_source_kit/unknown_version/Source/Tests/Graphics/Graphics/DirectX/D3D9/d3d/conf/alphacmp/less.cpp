#define D3D_OVERLOADS
#include <d3d.h>
#include "AlphaCmp.h"

//************************************************************************
// Less Test functions

CAlphaLessTest::CAlphaLessTest()
{
	m_szTestName = "Alpha Compare Less";
	m_szCommandKey = "Less";

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_LESS;
}

bool CAlphaLessTest::SetDefaultRenderStates(void)
{
	// Turn on Alpha testing
	SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, (DWORD)TRUE);

	// Set the Alpha compare func to Less
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_LESS);
	return true;
}

//************************************************************************
// LessEqual Test functions

CAlphaLessEqualTest::CAlphaLessEqualTest()
{
	m_szTestName = "Alpha Compare LessEqual";
	m_szCommandKey = "LessEqual";

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_LESSEQUAL;
}

bool CAlphaLessEqualTest::SetDefaultRenderStates(void)
{
	// Turn on Alpha testing
	SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, (DWORD)TRUE);

	// Set the Alpha compare func to LessEqual
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_LESSEQUAL);
	return true;
}

