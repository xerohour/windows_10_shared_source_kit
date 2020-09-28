#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// InvDestColor/Zero Test functions

CInvDestColorZeroTest::CInvDestColorZeroTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: Zero";
	m_szCommandKey = "InvDestColorZero";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "Zero";
}

//************************************************************************
// InvDestColor/One Test functions

CInvDestColorOneTest::CInvDestColorOneTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: One";
	m_szCommandKey = "InvDestColorOne";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_ONE;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "One";
}

//************************************************************************
// InvDestColor/SrcColor Test functions

CInvDestColorSrcColorTest::CInvDestColorSrcColorTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: SrcColor";
	m_szCommandKey = "InvDestColorSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "SrcColor";
}

//************************************************************************
// InvDestColor/InvSrcColor Test functions

CInvDestColorInvSrcColorTest::CInvDestColorInvSrcColorTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: InvSrcColor";
	m_szCommandKey = "InvDestColorInvSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "InvSrcColor";
}

//************************************************************************
// InvDestColor/SrcAlpha Test functions

CInvDestColorSrcAlphaTest::CInvDestColorSrcAlphaTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: SrcAlpha";
	m_szCommandKey = "InvDestColorSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "SrcAlpha";
}

//************************************************************************
// InvDestColor/InvSrcAlpha Test functions

CInvDestColorInvSrcAlphaTest::CInvDestColorInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: InvSrcAlpha";
	m_szCommandKey = "InvDestColorInvSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "InvSrcAlpha";
}

//************************************************************************
// InvDestColor/DestAlpha Test functions

CInvDestColorDestAlphaTest::CInvDestColorDestAlphaTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: DestAlpha";
	m_szCommandKey = "InvDestColorDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "DestAlpha";
}

//************************************************************************
// InvDestColor/InvDestAlpha Test functions

CInvDestColorInvDestAlphaTest::CInvDestColorInvDestAlphaTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: InvDestAlpha";
	m_szCommandKey = "InvDestColorInvDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "InvDestAlpha";
}

//************************************************************************
// InvDestColor/DestColor Test functions

CInvDestColorDestColorTest::CInvDestColorDestColorTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: DestColor";
	m_szCommandKey = "InvDestColorDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "DestColor";
}

//************************************************************************
// InvDestColor/InvDestColor Test functions

CInvDestColorInvDestColorTest::CInvDestColorInvDestColorTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: InvDestColor";
	m_szCommandKey = "InvDestColorInvDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "InvDestColor";
}

//************************************************************************
// InvDestColor/SrcAlphaSat Test functions

CInvDestColorSrcAlphaSatTest::CInvDestColorSrcAlphaSatTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: SrcAlphaSat";
	m_szCommandKey = "InvDestColorSrcAlphaSat";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "SrcAlphaSat";
}

//************************************************************************
// InvDestColor/BlendFactor Test functions

CInvDestColorBlendFactorTest::CInvDestColorBlendFactorTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: BlendFactor";
	m_szCommandKey = "InvDestColorBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_BLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}

//************************************************************************
// InvDestColor/InvBlendFactor Test functions

CInvDestColorInvBlendFactorTest::CInvDestColorInvBlendFactorTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: InvBlendFactor";
	m_szCommandKey = "InvDestColorInvBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_INVBLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}
