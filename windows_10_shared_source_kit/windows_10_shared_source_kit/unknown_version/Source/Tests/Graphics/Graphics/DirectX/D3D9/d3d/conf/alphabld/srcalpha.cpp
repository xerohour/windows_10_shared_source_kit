#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// SrcAlpha/Zero Test functions

CSrcAlphaZeroTest::CSrcAlphaZeroTest()
{
	m_szTestName = "SrcBlend: SrcAlpha DestBlend: Zero";
	m_szCommandKey = "SrcAlphaZero";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHA;
	dwDestBlend = D3DBLEND_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "Zero";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlpha/One Test functions

CSrcAlphaOneTest::CSrcAlphaOneTest()
{
	m_szTestName = "SrcBlend: SrcAlpha DestBlend: One";
	m_szCommandKey = "SrcAlphaOne";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHA;
	dwDestBlend = D3DBLEND_ONE;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "One";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlpha/SrcColor Test functions

CSrcAlphaSrcColorTest::CSrcAlphaSrcColorTest()
{
	m_szTestName = "SrcBlend: SrcAlpha DestBlend: SrcColor";
	m_szCommandKey = "SrcAlphaSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHA;
	dwDestBlend = D3DBLEND_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "SrcColor";
}

//************************************************************************
// SrcAlpha/InvSrcColor Test functions

CSrcAlphaInvSrcColorTest::CSrcAlphaInvSrcColorTest()
{
	m_szTestName = "SrcBlend: SrcAlpha DestBlend: InvSrcColor";
	m_szCommandKey = "SrcAlphaInvSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHA;
	dwDestBlend = D3DBLEND_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "InvSrcColor";
}

//************************************************************************
// SrcAlpha/SrcAlpha Test functions

CSrcAlphaSrcAlphaTest::CSrcAlphaSrcAlphaTest()
{
	m_szTestName = "SrcBlend: SrcAlpha DestBlend: SrcAlpha";
	m_szCommandKey = "SrcAlphaSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHA;
	dwDestBlend = D3DBLEND_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "SrcAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlpha/InvSrcAlpha Test functions

CSrcAlphaInvSrcAlphaTest::CSrcAlphaInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: SrcAlpha DestBlend: InvSrcAlpha";
	m_szCommandKey = "SrcAlphaInvSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHA;
	dwDestBlend = D3DBLEND_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "InvSrcAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlpha/DestAlpha Test functions

CSrcAlphaDestAlphaTest::CSrcAlphaDestAlphaTest()
{
	m_szTestName = "SrcBlend: SrcAlpha DestBlend: DestAlpha";
	m_szCommandKey = "SrcAlphaDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHA;
	dwDestBlend = D3DBLEND_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "DestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlpha/InvDestAlpha Test functions

CSrcAlphaInvDestAlphaTest::CSrcAlphaInvDestAlphaTest()
{
	m_szTestName = "SrcBlend: SrcAlpha DestBlend: InvDestAlpha";
	m_szCommandKey = "SrcAlphaInvDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHA;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "InvDestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlpha/DestColor Test functions

CSrcAlphaDestColorTest::CSrcAlphaDestColorTest()
{
	m_szTestName = "SrcBlend: SrcAlpha DestBlend: DestColor";
	m_szCommandKey = "SrcAlphaDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHA;
	dwDestBlend = D3DBLEND_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "DestColor";
}

//************************************************************************
// SrcAlpha/InvDestColor Test functions

CSrcAlphaInvDestColorTest::CSrcAlphaInvDestColorTest()
{
	m_szTestName = "SrcBlend: SrcAlpha DestBlend: InvDestColor";
	m_szCommandKey = "SrcAlphaInvDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHA;
	dwDestBlend = D3DBLEND_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "InvDestColor";
}

//************************************************************************
// SrcAlpha/SrcAlphaSat Test functions

CSrcAlphaSrcAlphaSatTest::CSrcAlphaSrcAlphaSatTest()
{
	m_szTestName = "SrcBlend: SrcAlpha DestBlend: SrcAlphaSat";
	m_szCommandKey = "SrcAlphaSrcAlphaSat";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHA;
	dwDestBlend = D3DBLEND_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "SrcAlphaSat";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlpha/BlendFactor Test functions

CSrcAlphaBlendFactorTest::CSrcAlphaBlendFactorTest()
{
	m_szTestName = "SrcBlend: SrcAlpha DestBlend: BlendFactor";
	m_szCommandKey = "SrcAlphaBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHA;
	dwDestBlend = D3DBLEND_BLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlpha/InvBlendFactor Test functions

CSrcAlphaInvBlendFactorTest::CSrcAlphaInvBlendFactorTest()
{
	m_szTestName = "SrcBlend: SrcAlpha DestBlend: InvBlendFactor";
	m_szCommandKey = "SrcAlphaInvBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHA;
	dwDestBlend = D3DBLEND_INVBLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}
