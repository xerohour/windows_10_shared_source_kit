#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// SrcAlphaSat/Zero Test functions

CSrcAlphaSatZeroTest::CSrcAlphaSatZeroTest()
{
	m_szTestName = "SrcBlend: SrcAlphaSat DestBlend: Zero";
	m_szCommandKey = "SrcAlphaSatZero";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHASAT;
	dwDestBlend = D3DBLEND_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "Zero";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlphaSat/One Test functions

CSrcAlphaSatOneTest::CSrcAlphaSatOneTest()
{
	m_szTestName = "SrcBlend: SrcAlphaSat DestBlend: One";
	m_szCommandKey = "SrcAlphaSatOne";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHASAT;
	dwDestBlend = D3DBLEND_ONE;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "One";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlphaSat/SrcColor Test functions

CSrcAlphaSatSrcColorTest::CSrcAlphaSatSrcColorTest()
{
	m_szTestName = "SrcBlend: SrcAlphaSat DestBlend: SrcColor";
	m_szCommandKey = "SrcAlphaSatSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHASAT;
	dwDestBlend = D3DBLEND_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "SrcColor";
}

//************************************************************************
// SrcAlphaSat/InvSrcColor Test functions

CSrcAlphaSatInvSrcColorTest::CSrcAlphaSatInvSrcColorTest()
{
	m_szTestName = "SrcBlend: SrcAlphaSat DestBlend: InvSrcColor";
	m_szCommandKey = "SrcAlphaSatInvSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHASAT;
	dwDestBlend = D3DBLEND_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "InvSrcColor";
}

//************************************************************************
// SrcAlphaSat/SrcAlpha Test functions

CSrcAlphaSatSrcAlphaTest::CSrcAlphaSatSrcAlphaTest()
{
	m_szTestName = "SrcBlend: SrcAlphaSat DestBlend: SrcAlpha";
	m_szCommandKey = "SrcAlphaSatSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHASAT;
	dwDestBlend = D3DBLEND_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "SrcAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlphaSat/InvSrcAlpha Test functions

CSrcAlphaSatInvSrcAlphaTest::CSrcAlphaSatInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: SrcAlphaSat DestBlend: InvSrcAlpha";
	m_szCommandKey = "SrcAlphaSatInvSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHASAT;
	dwDestBlend = D3DBLEND_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "InvSrcAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlphaSat/DestAlpha Test functions

CSrcAlphaSatDestAlphaTest::CSrcAlphaSatDestAlphaTest()
{
	m_szTestName = "SrcBlend: SrcAlphaSat DestBlend: DestAlpha";
	m_szCommandKey = "SrcAlphaSatDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHASAT;
	dwDestBlend = D3DBLEND_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "DestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlphaSat/InvDestAlpha Test functions

CSrcAlphaSatInvDestAlphaTest::CSrcAlphaSatInvDestAlphaTest()
{
	m_szTestName = "SrcBlend: SrcAlphaSat DestBlend: InvDestAlpha";
	m_szCommandKey = "SrcAlphaSatInvDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHASAT;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "InvDestAlpha";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlphaSat/DestColor Test functions

CSrcAlphaSatDestColorTest::CSrcAlphaSatDestColorTest()
{
	m_szTestName = "SrcBlend: SrcAlphaSat DestBlend: DestColor";
	m_szCommandKey = "SrcAlphaSatDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHASAT;
	dwDestBlend = D3DBLEND_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "DestColor";
}

//************************************************************************
// SrcAlphaSat/InvDestColor Test functions

CSrcAlphaSatInvDestColorTest::CSrcAlphaSatInvDestColorTest()
{
	m_szTestName = "SrcBlend: SrcAlphaSat DestBlend: InvDestColor";
	m_szCommandKey = "SrcAlphaSatInvDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHASAT;
	dwDestBlend = D3DBLEND_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "InvDestColor";
}

//************************************************************************
// SrcAlphaSat/SrcAlphaSat Test functions

CSrcAlphaSatSrcAlphaSatTest::CSrcAlphaSatSrcAlphaSatTest()
{
	m_szTestName = "SrcBlend: SrcAlphaSat DestBlend: SrcAlphaSat";
	m_szCommandKey = "SrcAlphaSatSrcAlphaSat";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHASAT;
	dwDestBlend = D3DBLEND_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "SrcAlphaSat";

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlphaSat/BlendFactor Test functions

CSrcAlphaSatBlendFactorTest::CSrcAlphaSatBlendFactorTest()
{
	m_szTestName = "SrcBlend: SrcAlphaSat DestBlend: BlendFactor";
	m_szCommandKey = "SrcAlphaSatBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHASAT;
	dwDestBlend = D3DBLEND_BLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// SrcAlphaSat/InvBlendFactor Test functions

CSrcAlphaSatInvBlendFactorTest::CSrcAlphaSatInvBlendFactorTest()
{
	m_szTestName = "SrcBlend: SrcAlphaSat DestBlend: InvBlendFactor";
	m_szCommandKey = "SrcAlphaSatInvBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_SRCALPHASAT;
	dwDestBlend = D3DBLEND_INVBLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}
