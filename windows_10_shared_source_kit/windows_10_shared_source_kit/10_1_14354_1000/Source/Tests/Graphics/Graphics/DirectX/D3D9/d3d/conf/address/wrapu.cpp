#define D3D_OVERLOADS
#include <d3d.h>
#include "Address.h"

//************************************************************************
// WrapU Test functions

CWrapUTest::CWrapUTest()
{
	m_szTestName = "Texture Address WrapU";
	m_szCommandKey = "WrapU";

#if FAST_TEST
    m_Options.D3DOptions.nBackBufferWidth = (DWORD)160;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD)140;
#endif //FAST_TEST

	// Tell parent to use wrap grid
	bWrapUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_WRAP;
	dwAddressCapV = D3DPTADDRESSCAPS_WRAP;
}

bool CWrapUTest::Setup(void)
{
	// Turn on Wrap addressing and the WrapU mode
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, (DWORD)D3DTADDRESS_WRAP);
	SetRenderState(D3DRENDERSTATE_WRAPU, (DWORD)TRUE);

	return CAddressTest::Setup();
}
