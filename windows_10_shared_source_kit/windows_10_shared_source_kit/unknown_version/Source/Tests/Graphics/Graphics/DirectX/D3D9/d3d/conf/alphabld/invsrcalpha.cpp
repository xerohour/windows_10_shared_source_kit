#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// InvSrcAlpha/Zero Test functions

CInvSrcAlphaZeroTest::CInvSrcAlphaZeroTest()
{
	m_szTestName = "SrcBlend: InvSrcAlpha DestBlend: Zero";
	m_szCommandKey = "InvSrcAlphaZero";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCALPHA;
	dwDestBlend = D3DBLEND_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "Zero";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvSrcAlpha/One Test functions

CInvSrcAlphaOneTest::CInvSrcAlphaOneTest()
{
	m_szTestName = "SrcBlend: InvSrcAlpha DestBlend: One";
	m_szCommandKey = "InvSrcAlphaOne";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCALPHA;
	dwDestBlend = D3DBLEND_ONE;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "One";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvSrcAlpha/SrcColor Test functions

CInvSrcAlphaSrcColorTest::CInvSrcAlphaSrcColorTest()
{
	m_szTestName = "SrcBlend: InvSrcAlpha DestBlend: SrcColor";
	m_szCommandKey = "InvSrcAlphaSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCALPHA;
	dwDestBlend = D3DBLEND_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "SrcColor";
}

//************************************************************************
// InvSrcAlpha/InvSrcColor Test functions

CInvSrcAlphaInvSrcColorTest::CInvSrcAlphaInvSrcColorTest()
{
	m_szTestName = "SrcBlend: InvSrcAlpha DestBlend: InvSrcColor";
	m_szCommandKey = "InvSrcAlphaInvSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCALPHA;
	dwDestBlend = D3DBLEND_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "InvSrcColor";
}

//************************************************************************
// InvSrcAlpha/SrcAlpha Test functions

CInvSrcAlphaSrcAlphaTest::CInvSrcAlphaSrcAlphaTest()
{
	m_szTestName = "SrcBlend: InvSrcAlpha DestBlend: SrcAlpha";
	m_szCommandKey = "InvSrcAlphaSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCALPHA;
	dwDestBlend = D3DBLEND_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "SrcAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvSrcAlpha/InvSrcAlpha Test functions

CInvSrcAlphaInvSrcAlphaTest::CInvSrcAlphaInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: InvSrcAlpha DestBlend: InvSrcAlpha";
	m_szCommandKey = "InvSrcAlphaInvSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCALPHA;
	dwDestBlend = D3DBLEND_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "InvSrcAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvSrcAlpha/DestAlpha Test functions

CInvSrcAlphaDestAlphaTest::CInvSrcAlphaDestAlphaTest()
{
	m_szTestName = "SrcBlend: InvSrcAlpha DestBlend: DestAlpha";
	m_szCommandKey = "InvSrcAlphaDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCALPHA;
	dwDestBlend = D3DBLEND_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "DestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvSrcAlpha/InvDestAlpha Test functions

CInvSrcAlphaInvDestAlphaTest::CInvSrcAlphaInvDestAlphaTest()
{
	m_szTestName = "SrcBlend: InvSrcAlpha DestBlend: InvDestAlpha";
	m_szCommandKey = "InvSrcAlphaInvDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCALPHA;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "InvDestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvSrcAlpha/DestColor Test functions

CInvSrcAlphaDestColorTest::CInvSrcAlphaDestColorTest()
{
	m_szTestName = "SrcBlend: InvSrcAlpha DestBlend: DestColor";
	m_szCommandKey = "InvSrcAlphaDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCALPHA;
	dwDestBlend = D3DBLEND_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "DestColor";
}

//************************************************************************
// InvSrcAlpha/InvDestColor Test functions

CInvSrcAlphaInvDestColorTest::CInvSrcAlphaInvDestColorTest()
{
	m_szTestName = "SrcBlend: InvSrcAlpha DestBlend: InvDestColor";
	m_szCommandKey = "InvSrcAlphaInvDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCALPHA;
	dwDestBlend = D3DBLEND_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "InvDestColor";
}

//************************************************************************
// InvSrcAlpha/SrcAlphaSat Test functions

CInvSrcAlphaSrcAlphaSatTest::CInvSrcAlphaSrcAlphaSatTest()
{
	m_szTestName = "SrcBlend: InvSrcAlpha DestBlend: SrcAlphaSat";
	m_szCommandKey = "InvSrcAlphaSrcAlphaSat";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCALPHA;
	dwDestBlend = D3DBLEND_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "SrcAlphaSat";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvSrcAlpha/BlendFactor Test functions

CInvSrcAlphaBlendFactorTest::CInvSrcAlphaBlendFactorTest()
{
	m_szTestName = "SrcBlend: InvSrcAlpha DestBlend: BlendFactor";
	m_szCommandKey = "InvSrcAlphaBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCALPHA;
	dwDestBlend = D3DBLEND_BLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvSrcAlpha/InvBlendFactor Test functions

CInvSrcAlphaInvBlendFactorTest::CInvSrcAlphaInvBlendFactorTest()
{
	m_szTestName = "SrcBlend: InvSrcAlpha DestBlend: InvBlendFactor";
	m_szCommandKey = "InvSrcAlphaInvBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVSRCALPHA;
	dwDestBlend = D3DBLEND_INVBLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}
