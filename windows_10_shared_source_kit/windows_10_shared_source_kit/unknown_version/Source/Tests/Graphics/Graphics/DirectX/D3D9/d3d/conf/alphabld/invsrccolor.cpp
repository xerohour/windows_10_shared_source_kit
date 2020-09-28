#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// InvSrcColor/Zero Test functions

CInvSrcColorZeroTest::CInvSrcColorZeroTest()
{
	m_szTestName = "SrcBlend: InvSrcColor DestBlend: Zero";
	m_szCommandKey = "InvSrcColorZero";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCCOLOR;
	dwDestBlend = D3DBLEND_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "Zero";
}

//************************************************************************
// InvSrcColor/One Test functions

CInvSrcColorOneTest::CInvSrcColorOneTest()
{
	m_szTestName = "SrcBlend: InvSrcColor DestBlend: One";
	m_szCommandKey = "InvSrcColorOne";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCCOLOR;
	dwDestBlend = D3DBLEND_ONE;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "One";
}

//************************************************************************
// InvSrcColor/SrcColor Test functions

CInvSrcColorSrcColorTest::CInvSrcColorSrcColorTest()
{
	m_szTestName = "SrcBlend: InvSrcColor DestBlend: SrcColor";
	m_szCommandKey = "InvSrcColorSrcColor";

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCCOLOR;
	dwDestBlend = D3DBLEND_ZERO;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DBLEND_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "SrcColor";
}

//************************************************************************
// InvSrcColor/InvSrcColor Test functions

CInvSrcColorInvSrcColorTest::CInvSrcColorInvSrcColorTest()
{
	m_szTestName = "SrcBlend: InvSrcColor DestBlend: InvSrcColor";
	m_szCommandKey = "InvSrcColorInvSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCCOLOR;
	dwDestBlend = D3DBLEND_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "InvSrcColor";
}

//************************************************************************
// InvSrcColor/SrcAlpha Test functions

CInvSrcColorSrcAlphaTest::CInvSrcColorSrcAlphaTest()
{
	m_szTestName = "SrcBlend: InvSrcColor DestBlend: SrcAlpha";
	m_szCommandKey = "InvSrcColorSrcAlpha";

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCCOLOR;
	dwDestBlend = D3DBLEND_ZERO;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DBLEND_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "SrcAlpha";
}

//************************************************************************
// InvSrcColor/InvSrcAlpha Test functions

CInvSrcColorInvSrcAlphaTest::CInvSrcColorInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: InvSrcColor DestBlend: InvSrcAlpha";
	m_szCommandKey = "InvSrcColorInvSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCCOLOR;
	dwDestBlend = D3DBLEND_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "InvSrcAlpha";
}

//************************************************************************
// InvSrcColor/DestAlpha Test functions

CInvSrcColorDestAlphaTest::CInvSrcColorDestAlphaTest()
{
	m_szTestName = "SrcBlend: InvSrcColor DestBlend: DestAlpha";
	m_szCommandKey = "InvSrcColorDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCCOLOR;
	dwDestBlend = D3DBLEND_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "DestAlpha";
}

//************************************************************************
// InvSrcColor/InvDestAlpha Test functions

CInvSrcColorInvDestAlphaTest::CInvSrcColorInvDestAlphaTest()
{
	m_szTestName = "SrcBlend: InvSrcColor DestBlend: InvDestAlpha";
	m_szCommandKey = "InvSrcColorInvDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCCOLOR;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "InvDestAlpha";
}

//************************************************************************
// InvSrcColor/DestColor Test functions

CInvSrcColorDestColorTest::CInvSrcColorDestColorTest()
{
	m_szTestName = "SrcBlend: InvSrcColor DestBlend: DestColor";
	m_szCommandKey = "InvSrcColorDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCCOLOR;
	dwDestBlend = D3DBLEND_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "DestColor";
}

//************************************************************************
// InvSrcColor/InvDestColor Test functions

CInvSrcColorInvDestColorTest::CInvSrcColorInvDestColorTest()
{
	m_szTestName = "SrcBlend: InvSrcColor DestBlend: InvDestColor";
	m_szCommandKey = "InvSrcColorInvDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCCOLOR;
	dwDestBlend = D3DBLEND_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "InvDestColor";
}

//************************************************************************
// InvSrcColor/SrcAlphaSat Test functions

CInvSrcColorSrcAlphaSatTest::CInvSrcColorSrcAlphaSatTest()
{
	m_szTestName = "SrcBlend: InvSrcColor DestBlend: SrcAlphaSat";
	m_szCommandKey = "InvSrcColorSrcAlphaSat";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCCOLOR;
	dwDestBlend = D3DBLEND_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "SrcAlphaSat";
}

//************************************************************************
// InvSrcColor/BlendFactor Test functions

CInvSrcColorBlendFactorTest::CInvSrcColorBlendFactorTest()
{
	m_szTestName = "SrcBlend: InvSrcColor DestBlend: BlendFactor";
	m_szCommandKey = "InvSrcColorBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCCOLOR;
	dwDestBlend = D3DBLEND_BLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}

//************************************************************************
// InvSrcColor/InvBlendFactor Test functions

CInvSrcColorInvBlendFactorTest::CInvSrcColorInvBlendFactorTest()
{
	m_szTestName = "SrcBlend: InvSrcColor DestBlend: InvBlendFactor";
	m_szCommandKey = "InvSrcColorInvBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCCOLOR;
	dwDestBlend = D3DBLEND_INVBLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}

