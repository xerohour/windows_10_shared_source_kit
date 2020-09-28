#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// SrcColor/Zero Test functions

CSrcColorZeroTest::CSrcColorZeroTest()
{
	m_szTestName = "SrcBlend: SrcColor DestBlend: Zero";
	m_szCommandKey = "SrcColorZero";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCCOLOR;
	dwDestBlend = D3DBLEND_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "Zero";
}

//************************************************************************
// SrcColor/One Test functions

CSrcColorOneTest::CSrcColorOneTest()
{
	m_szTestName = "SrcBlend: SrcColor DestBlend: One";
	m_szCommandKey = "SrcColorOne";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCCOLOR;
	dwDestBlend = D3DBLEND_ONE;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "One";
}

//************************************************************************
// SrcColor/SrcColor Test functions

CSrcColorSrcColorTest::CSrcColorSrcColorTest()
{
	m_szTestName = "SrcBlend: SrcColor DestBlend: SrcColor";
	m_szCommandKey = "SrcColorSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCCOLOR;
	dwDestBlend = D3DBLEND_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "SrcColor";
}

//************************************************************************
// SrcColor/InvSrcColor Test functions

CSrcColorInvSrcColorTest::CSrcColorInvSrcColorTest()
{
	m_szTestName = "SrcBlend: SrcColor DestBlend: InvSrcColor";
	m_szCommandKey = "SrcColorInvSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCCOLOR;
	dwDestBlend = D3DBLEND_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "InvSrcColor";
}

//************************************************************************
// SrcColor/SrcAlpha Test functions

CSrcColorSrcAlphaTest::CSrcColorSrcAlphaTest()
{
	m_szTestName = "SrcBlend: SrcColor DestBlend: SrcAlpha";
	m_szCommandKey = "SrcColorSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCCOLOR;
	dwDestBlend = D3DBLEND_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "SrcAlpha";
}

//************************************************************************
// SrcColor/InvSrcAlpha Test functions

CSrcColorInvSrcAlphaTest::CSrcColorInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: SrcColor DestBlend: InvSrcAlpha";
	m_szCommandKey = "SrcColorInvSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCCOLOR;
	dwDestBlend = D3DBLEND_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "InvSrcAlpha";
}

//************************************************************************
// SrcColor/DestAlpha Test functions

CSrcColorDestAlphaTest::CSrcColorDestAlphaTest()
{
	m_szTestName = "SrcBlend: SrcColor DestBlend: DestAlpha";
	m_szCommandKey = "SrcColorDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCCOLOR;
	dwDestBlend = D3DBLEND_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "DestAlpha";
}

//************************************************************************
// SrcColor/InvDestAlpha Test functions

CSrcColorInvDestAlphaTest::CSrcColorInvDestAlphaTest()
{
	m_szTestName = "SrcBlend: SrcColor DestBlend: InvDestAlpha";
	m_szCommandKey = "SrcColorInvDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCCOLOR;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "InvDestAlpha";
}

//************************************************************************
// SrcColor/DestColor Test functions

CSrcColorDestColorTest::CSrcColorDestColorTest()
{
	m_szTestName = "SrcBlend: SrcColor DestBlend: DestColor";
	m_szCommandKey = "SrcColorDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCCOLOR;
	dwDestBlend = D3DBLEND_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "DestColor";
}

//************************************************************************
// SrcColor/InvDestColor Test functions

CSrcColorInvDestColorTest::CSrcColorInvDestColorTest()
{
	m_szTestName = "SrcBlend: SrcColor DestBlend: InvDestColor";
	m_szCommandKey = "SrcColorInvDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCCOLOR;
	dwDestBlend = D3DBLEND_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "InvDestColor";
}

//************************************************************************
// SrcColor/SrcAlphaSat Test functions

CSrcColorSrcAlphaSatTest::CSrcColorSrcAlphaSatTest()
{
	m_szTestName = "SrcBlend: SrcColor DestBlend: SrcAlphaSat";
	m_szCommandKey = "SrcColorSrcAlphaSat";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCCOLOR;
	dwDestBlend = D3DBLEND_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "SrcAlphaSat";
}

//************************************************************************
// SrcColor/BlendFactor Test functions

CSrcColorBlendFactorTest::CSrcColorBlendFactorTest()
{
	m_szTestName = "SrcBlend: SrcColor DestBlend: BlendFactor";
	m_szCommandKey = "SrcColorBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCCOLOR;
	dwDestBlend = D3DBLEND_BLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}

//************************************************************************
// SrcColor/InvBlendFactor Test functions

CSrcColorInvBlendFactorTest::CSrcColorInvBlendFactorTest()
{
	m_szTestName = "SrcBlend: SrcColor DestBlend: InvBlendFactor";
	m_szCommandKey = "SrcColorInvBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCCOLOR;
	dwDestBlend = D3DBLEND_INVBLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}
