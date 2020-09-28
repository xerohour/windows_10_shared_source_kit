#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// Zero/Zero Test functions

CZeroZeroTest::CZeroZeroTest()
{
	m_szTestName = "SrcBlend: Zero DestBlend: Zero";
	m_szCommandKey = "ZeroZero";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ZERO;
	dwDestBlend = D3DBLEND_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "Zero";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// Zero/One Test functions

CZeroOneTest::CZeroOneTest()
{
	m_szTestName = "SrcBlend: Zero DestBlend: One";
	m_szCommandKey = "ZeroOne";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ZERO;
	dwDestBlend = D3DBLEND_ONE;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "One";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// Zero/SrcColor Test functions

CZeroSrcColorTest::CZeroSrcColorTest()
{
	m_szTestName = "SrcBlend: Zero DestBlend: SrcColor";
	m_szCommandKey = "ZeroSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ZERO;
	dwDestBlend = D3DBLEND_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "SrcColor";
}

//************************************************************************
// Zero/InvSrcColor Test functions

CZeroInvSrcColorTest::CZeroInvSrcColorTest()
{
	m_szTestName = "SrcBlend: Zero DestBlend: InvSrcColor";
	m_szCommandKey = "ZeroInvSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ZERO;
	dwDestBlend = D3DBLEND_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "InvSrcColor";
}

//************************************************************************
// Zero/SrcAlpha Test functions

CZeroSrcAlphaTest::CZeroSrcAlphaTest()
{
	m_szTestName = "SrcBlend: Zero DestBlend: SrcAlpha";
	m_szCommandKey = "ZeroSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ZERO;
	dwDestBlend = D3DBLEND_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "SrcAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// Zero/InvSrcAlpha Test functions

CZeroInvSrcAlphaTest::CZeroInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: Zero DestBlend: InvSrcAlpha";
	m_szCommandKey = "ZeroInvSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ZERO;
	dwDestBlend = D3DBLEND_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "InvSrcAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// Zero/DestAlpha Test functions

CZeroDestAlphaTest::CZeroDestAlphaTest()
{
	m_szTestName = "SrcBlend: Zero DestBlend: DestAlpha";
	m_szCommandKey = "ZeroDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ZERO;
	dwDestBlend = D3DBLEND_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "DestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// Zero/InvDestAlpha Test functions

CZeroInvDestAlphaTest::CZeroInvDestAlphaTest()
{
	m_szTestName = "SrcBlend: Zero DestBlend: InvDestAlpha";
	m_szCommandKey = "ZeroInvDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ZERO;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "InvDestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// Zero/DestColor Test functions

CZeroDestColorTest::CZeroDestColorTest()
{
	m_szTestName = "SrcBlend: Zero DestBlend: DestColor";
	m_szCommandKey = "ZeroDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ZERO;
	dwDestBlend = D3DBLEND_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "DestColor";
}

//************************************************************************
// Zero/InvDestColor Test functions

CZeroInvDestColorTest::CZeroInvDestColorTest()
{
	m_szTestName = "SrcBlend: Zero DestBlend: InvDestColor";
	m_szCommandKey = "ZeroInvDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ZERO;
	dwDestBlend = D3DBLEND_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "InvDestColor";
}

//************************************************************************
// Zero/SrcAlphaSat Test functions

CZeroSrcAlphaSatTest::CZeroSrcAlphaSatTest()
{
	m_szTestName = "SrcBlend: Zero DestBlend: SrcAlphaSat";
	m_szCommandKey = "ZeroSrcAlphaSat";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ZERO;
	dwDestBlend = D3DBLEND_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "SrcAlphaSat";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// Zero/BlendFactor Test functions

CZeroBlendFactorTest::CZeroBlendFactorTest()
{
	m_szTestName = "SrcBlend: Zero DestBlend: BlendFactor";
	m_szCommandKey = "ZeroBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ZERO;
	dwDestBlend = D3DBLEND_BLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// Zero/InvBlendFactor Test functions

CZeroInvBlendFactorTest::CZeroInvBlendFactorTest()
{
	m_szTestName = "SrcBlend: Zero DestBlend: InvBlendFactor";
	m_szCommandKey = "ZeroInvBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_ZERO;
	dwDestBlend = D3DBLEND_INVBLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}
