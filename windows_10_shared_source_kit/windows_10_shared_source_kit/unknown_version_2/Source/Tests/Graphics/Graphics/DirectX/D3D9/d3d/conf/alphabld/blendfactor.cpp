#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// BlendFactor/Zero Test functions

CBlendFactorZeroTest::CBlendFactorZeroTest()
{
	m_szTestName = "SrcBlend: BlendFactor DestBlend: Zero";
	m_szCommandKey = "BlendFactorZero";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BLENDFACTOR;
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
// BlendFactor/One Test functions

CBlendFactorOneTest::CBlendFactorOneTest()
{
	m_szTestName = "SrcBlend: BlendFactor DestBlend: One";
	m_szCommandKey = "BlendFactorOne";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BLENDFACTOR;
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
// BlendFactor/SrcColor Test functions

CBlendFactorSrcColorTest::CBlendFactorSrcColorTest()
{
	m_szTestName = "SrcBlend: BlendFactor DestBlend: SrcColor";
	m_szCommandKey = "BlendFactorSrcColor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BLENDFACTOR;
	dwDestBlend = D3DBLEND_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "SrcColor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}

//************************************************************************
// BlendFactor/InvSrcColor Test functions

CBlendFactorInvSrcColorTest::CBlendFactorInvSrcColorTest()
{
	m_szTestName = "SrcBlend: BlendFactor DestBlend: InvSrcColor";
	m_szCommandKey = "BlendFactorInvSrcColor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BLENDFACTOR;
	dwDestBlend = D3DBLEND_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "InvSrcColor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}

//************************************************************************
// BlendFactor/SrcAlpha Test functions

CBlendFactorSrcAlphaTest::CBlendFactorSrcAlphaTest()
{
	m_szTestName = "SrcBlend: BlendFactor DestBlend: SrcAlpha";
	m_szCommandKey = "BlendFactorSrcAlpha";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BLENDFACTOR;
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
// BlendFactor/InvSrcAlpha Test functions

CBlendFactorInvSrcAlphaTest::CBlendFactorInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: BlendFactor DestBlend: InvSrcAlpha";
	m_szCommandKey = "BlendFactorInvSrcAlpha";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BLENDFACTOR;
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
// BlendFactor/DestAlpha Test functions

CBlendFactorDestAlphaTest::CBlendFactorDestAlphaTest()
{
	m_szTestName = "SrcBlend: BlendFactor DestBlend: DestAlpha";
	m_szCommandKey = "BlendFactorDestAlpha";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BLENDFACTOR;
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
// BlendFactor/InvDestAlpha Test functions

CBlendFactorInvDestAlphaTest::CBlendFactorInvDestAlphaTest()
{
	m_szTestName = "SrcBlend: BlendFactor DestBlend: InvDestAlpha";
	m_szCommandKey = "BlendFactorInvDestAlpha";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BLENDFACTOR;
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
// BlendFactor/DestColor Test functions

CBlendFactorDestColorTest::CBlendFactorDestColorTest()
{
	m_szTestName = "SrcBlend: BlendFactor DestBlend: DestColor";
	m_szCommandKey = "BlendFactorDestColor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BLENDFACTOR;
	dwDestBlend = D3DBLEND_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "DestColor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}

//************************************************************************
// BlendFactor/InvDestColor Test functions

CBlendFactorInvDestColorTest::CBlendFactorInvDestColorTest()
{
	m_szTestName = "SrcBlend: BlendFactor DestBlend: InvDestColor";
	m_szCommandKey = "BlendFactorInvDestColor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BLENDFACTOR;
	dwDestBlend = D3DBLEND_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "InvDestColor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}

//************************************************************************
// BlendFactor/SrcAlphaSat Test functions

CBlendFactorSrcAlphaSatTest::CBlendFactorSrcAlphaSatTest()
{
	m_szTestName = "SrcBlend: BlendFactor DestBlend: SrcAlphaSat";
	m_szCommandKey = "BlendFactorSrcAlphaSat";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BLENDFACTOR;
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
// BlendFactor/BlendFactor Test functions

CBlendFactorBlendFactorTest::CBlendFactorBlendFactorTest()
{
	m_szTestName = "SrcBlend: BlendFactor DestBlend: BlendFactor";
	m_szCommandKey = "BlendFactorBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BLENDFACTOR;
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
// BlendFactor/InvBlendFactor Test functions

CBlendFactorInvBlendFactorTest::CBlendFactorInvBlendFactorTest()
{
	m_szTestName = "SrcBlend: BlendFactor DestBlend: InvBlendFactor";
	m_szCommandKey = "BlendFactorInvBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_BLENDFACTOR;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_BLENDFACTOR;
	dwDestBlend = D3DBLEND_INVBLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "BlendFactor";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}
