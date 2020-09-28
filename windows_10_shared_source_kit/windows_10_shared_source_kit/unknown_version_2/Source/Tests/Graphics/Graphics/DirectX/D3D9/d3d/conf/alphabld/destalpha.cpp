#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// DestAlpha/Zero Test functions

CDestAlphaZeroTest::CDestAlphaZeroTest()
{
	m_szTestName = "SrcBlend: DestAlpha DestBlend: Zero";
	m_szCommandKey = "DestAlphaZero";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "Zero";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// DestAlpha/One Test functions

CDestAlphaOneTest::CDestAlphaOneTest()
{
	m_szTestName = "SrcBlend: DestAlpha DestBlend: One";
	m_szCommandKey = "DestAlphaOne";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_ONE;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "One";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// DestAlpha/SrcColor Test functions

CDestAlphaSrcColorTest::CDestAlphaSrcColorTest()
{
	m_szTestName = "SrcBlend: DestAlpha DestBlend: SrcColor";
	m_szCommandKey = "DestAlphaSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "SrcColor";
}

//************************************************************************
// DestAlpha/InvSrcColor Test functions

CDestAlphaInvSrcColorTest::CDestAlphaInvSrcColorTest()
{
	m_szTestName = "SrcBlend: DestAlpha DestBlend: InvSrcColor";
	m_szCommandKey = "DestAlphaInvSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "InvSrcColor";
}

//************************************************************************
// DestAlpha/SrcAlpha Test functions

CDestAlphaSrcAlphaTest::CDestAlphaSrcAlphaTest()
{
	m_szTestName = "SrcBlend: DestAlpha DestBlend: SrcAlpha";
	m_szCommandKey = "DestAlphaSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "SrcAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// DestAlpha/InvSrcAlpha Test functions

CDestAlphaInvSrcAlphaTest::CDestAlphaInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: DestAlpha DestBlend: InvSrcAlpha";
	m_szCommandKey = "DestAlphaInvSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "InvSrcAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// DestAlpha/DestAlpha Test functions

CDestAlphaDestAlphaTest::CDestAlphaDestAlphaTest()
{
	m_szTestName = "SrcBlend: DestAlpha DestBlend: DestAlpha";
	m_szCommandKey = "DestAlphaDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "DestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// DestAlpha/InvDestAlpha Test functions

CDestAlphaInvDestAlphaTest::CDestAlphaInvDestAlphaTest()
{
	m_szTestName = "SrcBlend: DestAlpha DestBlend: InvDestAlpha";
	m_szCommandKey = "DestAlphaInvDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "InvDestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// DestAlpha/DestColor Test functions

CDestAlphaDestColorTest::CDestAlphaDestColorTest()
{
	m_szTestName = "SrcBlend: DestAlpha DestBlend: DestColor";
	m_szCommandKey = "DestAlphaDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "DestColor";
}

//************************************************************************
// DestAlpha/InvDestColor Test functions

CDestAlphaInvDestColorTest::CDestAlphaInvDestColorTest()
{
	m_szTestName = "SrcBlend: DestAlpha DestBlend: InvDestColor";
	m_szCommandKey = "DestAlphaInvDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "InvDestColor";
}

//************************************************************************
// DestAlpha/SrcAlphaSat Test functions

CDestAlphaSrcAlphaSatTest::CDestAlphaSrcAlphaSatTest()
{
	m_szTestName = "SrcBlend: DestAlpha DestBlend: SrcAlphaSat";
	m_szCommandKey = "DestAlphaSrcAlphaSat";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "SrcAlphaSat";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// DestAlpha/BlendFactor Test functions

CDestAlphaBlendFactorTest::CDestAlphaBlendFactorTest()
{
	m_szTestName = "SrcBlend: DestAlpha DestBlend: BlendFactor";
	m_szCommandKey = "DestAlphaBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_BLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// DestAlpha/InvBlendFactor Test functions

CDestAlphaInvBlendFactorTest::CDestAlphaInvBlendFactorTest()
{
	m_szTestName = "SrcBlend: DestAlpha DestBlend: InvBlendFactor";
	m_szCommandKey = "DestAlphaInvBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_INVBLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}
