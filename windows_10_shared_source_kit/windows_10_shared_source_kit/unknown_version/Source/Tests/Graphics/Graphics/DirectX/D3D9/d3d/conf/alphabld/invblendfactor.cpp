#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// InvBlendFactor/Zero Test functions

CInvBlendFactorZeroTest::CInvBlendFactorZeroTest()
{
	m_szTestName = "SrcBlend: InvBlendFactor DestBlend: Zero";
	m_szCommandKey = "InvBlendFactorZero";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
	dwDestBlend = D3DBLEND_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "Zero";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvBlendFactor/One Test functions

CInvBlendFactorOneTest::CInvBlendFactorOneTest()
{
	m_szTestName = "SrcBlend: InvBlendFactor DestBlend: One";
	m_szCommandKey = "InvBlendFactorOne";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
	dwDestBlend = D3DBLEND_ONE;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "One";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvBlendFactor/SrcColor Test functions

CInvBlendFactorSrcColorTest::CInvBlendFactorSrcColorTest()
{
	m_szTestName = "SrcBlend: InvBlendFactor DestBlend: SrcColor";
	m_szCommandKey = "InvBlendFactorSrcColor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
	dwDestBlend = D3DBLEND_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "SrcColor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}

//************************************************************************
// InvBlendFactor/InvSrcColor Test functions

CInvBlendFactorInvSrcColorTest::CInvBlendFactorInvSrcColorTest()
{
	m_szTestName = "SrcBlend: InvBlendFactor DestBlend: InvSrcColor";
	m_szCommandKey = "InvBlendFactorInvSrcColor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
	dwDestBlend = D3DBLEND_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "InvSrcColor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}

//************************************************************************
// InvBlendFactor/SrcAlpha Test functions

CInvBlendFactorSrcAlphaTest::CInvBlendFactorSrcAlphaTest()
{
	m_szTestName = "SrcBlend: InvBlendFactor DestBlend: SrcAlpha";
	m_szCommandKey = "InvBlendFactorSrcAlpha";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
	dwDestBlend = D3DBLEND_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "SrcAlpha";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvBlendFactor/InvSrcAlpha Test functions

CInvBlendFactorInvSrcAlphaTest::CInvBlendFactorInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: InvBlendFactor DestBlend: InvSrcAlpha";
	m_szCommandKey = "InvBlendFactorInvSrcAlpha";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
	dwDestBlend = D3DBLEND_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "InvSrcAlpha";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvBlendFactor/DestAlpha Test functions

CInvBlendFactorDestAlphaTest::CInvBlendFactorDestAlphaTest()
{
	m_szTestName = "SrcBlend: InvBlendFactor DestBlend: DestAlpha";
	m_szCommandKey = "InvBlendFactorDestAlpha";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
	dwDestBlend = D3DBLEND_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "DestAlpha";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvBlendFactor/InvDestAlpha Test functions

CInvBlendFactorInvDestAlphaTest::CInvBlendFactorInvDestAlphaTest()
{
	m_szTestName = "SrcBlend: InvBlendFactor DestBlend: InvDestAlpha";
	m_szCommandKey = "InvBlendFactorInvDestAlpha";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "InvDestAlpha";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvBlendFactor/DestColor Test functions

CInvBlendFactorDestColorTest::CInvBlendFactorDestColorTest()
{
	m_szTestName = "SrcBlend: InvBlendFactor DestBlend: DestColor";
	m_szCommandKey = "InvBlendFactorDestColor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
	dwDestBlend = D3DBLEND_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "DestColor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}

//************************************************************************
// InvBlendFactor/InvDestColor Test functions

CInvBlendFactorInvDestColorTest::CInvBlendFactorInvDestColorTest()
{
	m_szTestName = "SrcBlend: InvBlendFactor DestBlend: InvDestColor";
	m_szCommandKey = "InvBlendFactorInvDestColor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
	dwDestBlend = D3DBLEND_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "InvDestColor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}

//************************************************************************
// InvBlendFactor/SrcAlphaSat Test functions

CInvBlendFactorSrcAlphaSatTest::CInvBlendFactorSrcAlphaSatTest()
{
	m_szTestName = "SrcBlend: InvBlendFactor DestBlend: SrcAlphaSat";
	m_szCommandKey = "InvBlendFactorSrcAlphaSat";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
	dwDestBlend = D3DBLEND_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "SrcAlphaSat";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvBlendFactor/BlendFactor Test functions

CInvBlendFactorBlendFactorTest::CInvBlendFactorBlendFactorTest()
{
	m_szTestName = "SrcBlend: InvBlendFactor DestBlend: BlendFactor";
	m_szCommandKey = "InvBlendFactorBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
	dwDestBlend = D3DBLEND_BLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

//************************************************************************
// InvBlendFactor/InvBlendFactor Test functions

CInvBlendFactorInvBlendFactorTest::CInvBlendFactorInvBlendFactorTest()
{
	m_szTestName = "SrcBlend: InvBlendFactor DestBlend: InvBlendFactor";
	m_szCommandKey = "InvBlendFactorInvBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
	dwDestBlend = D3DBLEND_INVBLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}
