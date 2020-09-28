#define D3D_OVERLOADS
#include <d3d.h>
#include "Blend.h"

//************************************************************************
// Decal Test functions

CDecalTest::CDecalTest()
{
	m_szTestName = "Texture Blend Decal";
	m_szCommandKey = "Decal";

	// Tell parent class what cap to look for
	dwBlendCap = D3DPTBLENDCAPS_DECAL;
    dwTextureOpCap = D3DTEXOPCAPS_SELECTARG1;
    dwTextureOpCap2 = 0;

  	// Give parent a printable name for the caps
    szTextureOp = "SelectArg1";

	// Tell parent what state we are
	dwBlendState = D3DTBLEND_DECAL;
}

CDecalTest::~CDecalTest()
{
}
   
bool CDecalTest::SetDefaultRenderStates(void)
{
	// Turn on Decal blending
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);

	if (m_dwVersion <= 0x0600)
		SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)D3DTBLEND_DECAL);

	return true;
}

//************************************************************************
// DecalAlpha Test functions

CDecalAlphaTest::CDecalAlphaTest()
{
	m_szTestName = "Texture Blend DecalAlpha";
	m_szCommandKey = "DecalAlpha";

	// Tell parent class what cap to look for
	dwBlendCap = D3DPTBLENDCAPS_DECALALPHA;
    dwTextureOpCap = D3DTEXOPCAPS_SELECTARG2;
    dwTextureOpCap2 = D3DTEXOPCAPS_BLENDTEXTUREALPHA;

  	// Give parent a printable name for the caps
    szTextureOp = "SelectArg2";
    szTextureOp2 = "BlendTextureAlpha";

	// Tell parent what state we are
	dwBlendState = D3DTBLEND_DECALALPHA;
}

CDecalAlphaTest::~CDecalAlphaTest()
{
}

bool CDecalAlphaTest::SetDefaultRenderStates(void)
{
	// Turn on DecalAlpha blending
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);

	if (m_dwVersion <= 0x0600)
		SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)D3DTBLEND_DECALALPHA);

	return true;
}

//************************************************************************
// Copy Test functions

CCopyTest::CCopyTest()
{
	m_szTestName = "Texture Blend Copy";
	m_szCommandKey = "Copy";

    // Init Framework options
	m_Options.D3DOptions.fMaxDXVersion = 7.0f;

	// Tell parent class what cap to look for
	dwBlendCap = D3DPTBLENDCAPS_COPY;
    dwTextureOpCap = D3DTEXOPCAPS_SELECTARG1;
    dwTextureOpCap2 = 0;

  	// Give parent a printable name for the caps
    szTextureOp = "SelectArg1";

	// Tell parent what state we are
	dwBlendState = D3DTBLEND_COPY;
}

CCopyTest::~CCopyTest()
{
}

bool CCopyTest::SetDefaultRenderStates(void)
{
	// Turn on Copy blending
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);

	if (m_dwVersion <= 0x0600)
		SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)D3DTBLEND_COPY);

	return true;
}
 