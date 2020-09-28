#define D3D_OVERLOADS
#include <d3d.h>
#include "AlphaCmp.h"

//************************************************************************
// Greater Test functions

CAlphaGreaterTest::CAlphaGreaterTest()
{
	m_szTestName = "Alpha Compare Greater";
	m_szCommandKey = "Greater";

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_GREATER;
}

bool CAlphaGreaterTest::SetDefaultRenderStates(void)
{
	// Turn on Alpha testing
	SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, (DWORD)TRUE);

	// Set the Alpha compare func to Greater
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_GREATER);
	return true;
}

//************************************************************************
// GreaterEqual Test functions

CAlphaGreaterEqualTest::CAlphaGreaterEqualTest()
{
	m_szTestName = "Alpha Compare GreaterEqual";
	m_szCommandKey = "GreaterEqual";

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_GREATEREQUAL;
}

bool CAlphaGreaterEqualTest::SetDefaultRenderStates(void)
{
	// Turn on Alpha testing
	SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, (DWORD)TRUE);

	// Set the Alpha compare func to GreaterEqual
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_GREATEREQUAL);
	return true;
}
