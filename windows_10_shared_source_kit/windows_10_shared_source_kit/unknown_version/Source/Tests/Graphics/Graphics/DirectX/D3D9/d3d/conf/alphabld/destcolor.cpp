#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// DestColor/Zero Test functions

CDestColorZeroTest::CDestColorZeroTest()
{
	m_szTestName = "SrcBlend: DestColor DestBlend: Zero";
	m_szCommandKey = "DestColorZero";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTCOLOR;
	dwDestBlend = D3DBLEND_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "Zero";
}

//************************************************************************
// DestColor/One Test functions

CDestColorOneTest::CDestColorOneTest()
{
	m_szTestName = "SrcBlend: DestColor DestBlend: One";
	m_szCommandKey = "DestColorOne";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTCOLOR;
	dwDestBlend = D3DBLEND_ONE;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "One";
}

//************************************************************************
// DestColor/SrcColor Test functions

CDestColorSrcColorTest::CDestColorSrcColorTest()
{
	m_szTestName = "SrcBlend: DestColor DestBlend: SrcColor";
	m_szCommandKey = "DestColorSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTCOLOR;
	dwDestBlend = D3DBLEND_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "SrcColor";
}

//************************************************************************
// DestColor/InvSrcColor Test functions

CDestColorInvSrcColorTest::CDestColorInvSrcColorTest()
{
	m_szTestName = "SrcBlend: DestColor DestBlend: InvSrcColor";
	m_szCommandKey = "DestColorInvSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTCOLOR;
	dwDestBlend = D3DBLEND_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "InvSrcColor";
}

//************************************************************************
// DestColor/SrcAlpha Test functions

CDestColorSrcAlphaTest::CDestColorSrcAlphaTest()
{
	m_szTestName = "SrcBlend: DestColor DestBlend: SrcAlpha";
	m_szCommandKey = "DestColorSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTCOLOR;
	dwDestBlend = D3DBLEND_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "SrcAlpha";
}

//************************************************************************
// DestColor/InvSrcAlpha Test functions

CDestColorInvSrcAlphaTest::CDestColorInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: DestColor DestBlend: InvSrcAlpha";
	m_szCommandKey = "DestColorInvSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTCOLOR;
	dwDestBlend = D3DBLEND_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "InvSrcAlpha";
}

//************************************************************************
// DestColor/DestAlpha Test functions

CDestColorDestAlphaTest::CDestColorDestAlphaTest()
{
	m_szTestName = "SrcBlend: DestColor DestBlend: DestAlpha";
	m_szCommandKey = "DestColorDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTCOLOR;
	dwDestBlend = D3DBLEND_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "DestAlpha";
}

//************************************************************************
// DestColor/InvDestAlpha Test functions

CDestColorInvDestAlphaTest::CDestColorInvDestAlphaTest()
{
	m_szTestName = "SrcBlend: DestColor DestBlend: InvDestAlpha";
	m_szCommandKey = "DestColorInvDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTCOLOR;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "InvDestAlpha";
}

//************************************************************************
// DestColor/DestColor Test functions

CDestColorDestColorTest::CDestColorDestColorTest()
{
	m_szTestName = "SrcBlend: DestColor DestBlend: DestColor";
	m_szCommandKey = "DestColorDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTCOLOR;
	dwDestBlend = D3DBLEND_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "DestColor";
}

//************************************************************************
// DestColor/InvDestColor Test functions

CDestColorInvDestColorTest::CDestColorInvDestColorTest()
{
	m_szTestName = "SrcBlend: DestColor DestBlend: InvDestColor";
	m_szCommandKey = "DestColorInvDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTCOLOR;
	dwDestBlend = D3DBLEND_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "InvDestColor";
}

//************************************************************************
// DestColor/SrcAlphaSat Test functions

CDestColorSrcAlphaSatTest::CDestColorSrcAlphaSatTest()
{
	m_szTestName = "SrcBlend: DestColor DestBlend: SrcAlphaSat";
	m_szCommandKey = "DestColorSrcAlphaSat";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTCOLOR;
	dwDestBlend = D3DBLEND_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "SrcAlphaSat";
}

//************************************************************************
// DestColor/BlendFactor Test functions

CDestColorBlendFactorTest::CDestColorBlendFactorTest()
{
	m_szTestName = "SrcBlend: DestColor DestBlend: BlendFactor";
	m_szCommandKey = "DestColorBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTCOLOR;
	dwDestBlend = D3DBLEND_BLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}

//************************************************************************
// DestColor/InvBlendFactor Test functions

CDestColorInvBlendFactorTest::CDestColorInvBlendFactorTest()
{
	m_szTestName = "SrcBlend: DestColor DestBlend: InvBlendFactor";
	m_szCommandKey = "DestColorInvBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTCOLOR;
	dwDestBlend = D3DBLEND_INVBLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}
