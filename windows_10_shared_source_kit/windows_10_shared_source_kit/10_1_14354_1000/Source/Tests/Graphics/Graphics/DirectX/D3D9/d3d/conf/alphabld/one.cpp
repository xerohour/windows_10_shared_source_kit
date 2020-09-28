#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// One/Zero Test functions

COneZeroTest::COneZeroTest()
{
	m_szTestName = "SrcBlend: One DestBlend: Zero";
	m_szCommandKey = "OneZero";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ONE;
	dwDestBlend = D3DBLEND_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "Zero";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// One/One Test functions

COneOneTest::COneOneTest()
{
	m_szTestName = "SrcBlend: One DestBlend: One";
	m_szCommandKey = "OneOne";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ONE;
	dwDestBlend = D3DBLEND_ONE;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "One";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// One/SrcColor Test functions

COneSrcColorTest::COneSrcColorTest()
{
	m_szTestName = "SrcBlend: One DestBlend: SrcColor";
	m_szCommandKey = "OneSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ONE;
	dwDestBlend = D3DBLEND_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "SrcColor";
}

//************************************************************************
// One/InvSrcColor Test functions

COneInvSrcColorTest::COneInvSrcColorTest()
{
	m_szTestName = "SrcBlend: One DestBlend: InvSrcColor";
	m_szCommandKey = "OneInvSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ONE;
	dwDestBlend = D3DBLEND_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "InvSrcColor";
}

//************************************************************************
// One/SrcAlpha Test functions

COneSrcAlphaTest::COneSrcAlphaTest()
{
	m_szTestName = "SrcBlend: One DestBlend: SrcAlpha";
	m_szCommandKey = "OneSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ONE;
	dwDestBlend = D3DBLEND_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "SrcAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// One/InvSrcAlpha Test functions

COneInvSrcAlphaTest::COneInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: One DestBlend: InvSrcAlpha";
	m_szCommandKey = "OneInvSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ONE;
	dwDestBlend = D3DBLEND_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "InvSrcAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// One/DestAlpha Test functions

COneDestAlphaTest::COneDestAlphaTest()
{
	m_szTestName = "SrcBlend: One DestBlend: DestAlpha";
	m_szCommandKey = "OneDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ONE;
	dwDestBlend = D3DBLEND_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "DestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// One/InvDestAlpha Test functions

COneInvDestAlphaTest::COneInvDestAlphaTest()
{
	m_szTestName = "SrcBlend: One DestBlend: InvDestAlpha";
	m_szCommandKey = "OneInvDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ONE;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "InvDestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// One/DestColor Test functions

COneDestColorTest::COneDestColorTest()
{
	m_szTestName = "SrcBlend: One DestBlend: DestColor";
	m_szCommandKey = "OneDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ONE;
	dwDestBlend = D3DBLEND_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "DestColor";
}

//************************************************************************
// One/InvDestColor Test functions

COneInvDestColorTest::COneInvDestColorTest()
{
	m_szTestName = "SrcBlend: One DestBlend: InvDestColor";
	m_szCommandKey = "OneInvDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ONE;
	dwDestBlend = D3DBLEND_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "InvDestColor";
}

//************************************************************************
// One/SrcAlphaSat Test functions

COneSrcAlphaSatTest::COneSrcAlphaSatTest()
{
	m_szTestName = "SrcBlend: One DestBlend: SrcAlphaSat";
	m_szCommandKey = "OneSrcAlphaSat";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ONE;
	dwDestBlend = D3DBLEND_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "SrcAlphaSat";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// One/BlendFactor Test functions

COneBlendFactorTest::COneBlendFactorTest()
{
	m_szTestName = "SrcBlend: One DestBlend: BlendFactor";
	m_szCommandKey = "OneBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ONE;
	dwDestBlend = D3DBLEND_BLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// One/InvBlendFactor Test functions

COneInvBlendFactorTest::COneInvBlendFactorTest()
{
	m_szTestName = "SrcBlend: One DestBlend: InvBlendFactor";
	m_szCommandKey = "OneInvBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ONE;
	dwDestBlend = D3DBLEND_INVBLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}
