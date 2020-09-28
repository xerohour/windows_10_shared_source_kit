#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// InvDestAlpha/Zero Test functions

CInvDestAlphaZeroTest::CInvDestAlphaZeroTest()
{
	m_szTestName = "SrcBlend: InvDestAlpha DestBlend: Zero";
	m_szCommandKey = "InvDestAlphaZero";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTALPHA;
	dwDestBlend = D3DBLEND_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "Zero";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvDestAlpha/One Test functions

CInvDestAlphaOneTest::CInvDestAlphaOneTest()
{
	m_szTestName = "SrcBlend: InvDestAlpha DestBlend: One";
	m_szCommandKey = "InvDestAlphaOne";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTALPHA;
	dwDestBlend = D3DBLEND_ONE;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "One";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvDestAlpha/SrcColor Test functions

CInvDestAlphaSrcColorTest::CInvDestAlphaSrcColorTest()
{
	m_szTestName = "SrcBlend: InvDestAlpha DestBlend: SrcColor";
	m_szCommandKey = "InvDestAlphaSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTALPHA;
	dwDestBlend = D3DBLEND_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "SrcColor";
}

//************************************************************************
// InvDestAlpha/InvSrcColor Test functions

CInvDestAlphaInvSrcColorTest::CInvDestAlphaInvSrcColorTest()
{
	m_szTestName = "SrcBlend: InvDestAlpha DestBlend: InvSrcColor";
	m_szCommandKey = "InvDestAlphaInvSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTALPHA;
	dwDestBlend = D3DBLEND_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "InvSrcColor";
}

//************************************************************************
// InvDestAlpha/SrcAlpha Test functions

CInvDestAlphaSrcAlphaTest::CInvDestAlphaSrcAlphaTest()
{
	m_szTestName = "SrcBlend: InvDestAlpha DestBlend: SrcAlpha";
	m_szCommandKey = "InvDestAlphaSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTALPHA;
	dwDestBlend = D3DBLEND_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "SrcAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvDestAlpha/InvSrcAlpha Test functions

CInvDestAlphaInvSrcAlphaTest::CInvDestAlphaInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: InvDestAlpha DestBlend: InvSrcAlpha";
	m_szCommandKey = "InvDestAlphaInvSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTALPHA;
	dwDestBlend = D3DBLEND_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "InvSrcAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvDestAlpha/DestAlpha Test functions

CInvDestAlphaDestAlphaTest::CInvDestAlphaDestAlphaTest()
{
	m_szTestName = "SrcBlend: InvDestAlpha DestBlend: DestAlpha";
	m_szCommandKey = "InvDestAlphaDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTALPHA;
	dwDestBlend = D3DBLEND_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "DestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvDestAlpha/InvDestAlpha Test functions

CInvDestAlphaInvDestAlphaTest::CInvDestAlphaInvDestAlphaTest()
{
	m_szTestName = "SrcBlend: InvDestAlpha DestBlend: InvDestAlpha";
	m_szCommandKey = "InvDestAlphaInvDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTALPHA;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "InvDestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvDestAlpha/DestColor Test functions

CInvDestAlphaDestColorTest::CInvDestAlphaDestColorTest()
{
	m_szTestName = "SrcBlend: InvDestAlpha DestBlend: DestColor";
	m_szCommandKey = "InvDestAlphaDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTALPHA;
	dwDestBlend = D3DBLEND_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "DestColor";
}

//************************************************************************
// InvDestAlpha/InvDestColor Test functions

CInvDestAlphaInvDestColorTest::CInvDestAlphaInvDestColorTest()
{
	m_szTestName = "SrcBlend: InvDestAlpha DestBlend: InvDestColor";
	m_szCommandKey = "InvDestAlphaInvDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTALPHA;
	dwDestBlend = D3DBLEND_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "InvDestColor";
}

//************************************************************************
// InvDestAlpha/SrcAlphaSat Test functions

CInvDestAlphaSrcAlphaSatTest::CInvDestAlphaSrcAlphaSatTest()
{
	m_szTestName = "SrcBlend: InvDestAlpha DestBlend: SrcAlphaSat";
	m_szCommandKey = "InvDestAlphaSrcAlphaSat";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTALPHA;
	dwDestBlend = D3DBLEND_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "SrcAlphaSat";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvDestAlpha/BlendFactor Test functions

CInvDestAlphaBlendFactorTest::CInvDestAlphaBlendFactorTest()
{
	m_szTestName = "SrcBlend: InvDestAlpha DestBlend: BlendFactor";
	m_szCommandKey = "InvDestAlphaBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTALPHA;
	dwDestBlend = D3DBLEND_BLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvDestAlpha/InvBlendFactor Test functions

CInvDestAlphaInvBlendFactorTest::CInvDestAlphaInvBlendFactorTest()
{
	m_szTestName = "SrcBlend: InvDestAlpha DestBlend: InvBlendFactor";
	m_szCommandKey = "InvDestAlphaInvBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTALPHA;
	dwDestBlend = D3DBLEND_INVBLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}
