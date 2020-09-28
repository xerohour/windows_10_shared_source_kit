 a loop aL, i0 break_eq c0.x, c0.y and rep i0 break block";
string PS_449 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_450_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_450_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_450 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_451_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_451_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 breakp p0.x block";
string PS_451 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_452_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_452_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 break_eq c0.x, c0.y block";
string PS_452 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_453_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_453_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 break_eq c0.x, c0.y block";
string PS_453 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_454_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_454_Desc = "ps_3_0 : sub is allowed in a rep i0 break_eq c0.x, c0.y and if b0 block";
string PS_454 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_455_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_455_Desc = "ps_3_0 : sub is allowed in a rep i0 break_eq c0.x, c0.y and if b0 nop else block";
string PS_455 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_456_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_456_Desc = "ps_3_0 : sub is allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 block";
string PS_456 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_457_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_457_Desc = "ps_3_0 : sub is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 block";
string PS_457 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_458_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_458_Desc = "ps_3_0 : sub is allowed in a rep i0 break_eq c0.x, c0.y and if_eq c0.x, c0.y block";
string PS_458 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_459_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_459_Desc = "ps_3_0 : sub is allowed in a rep i0 break_eq c0.x, c0.y and if_eq c0.x, c0.y nop else block";
string PS_459 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if_eq c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_460_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_460_Desc = "ps_3_0 : sub is allowed in a rep i0 break_eq c0.x, c0.y and if p0.x block";
string PS_460 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_461_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_461_Desc = "ps_3_0 : sub is allowed in a rep i0 break_eq c0.x, c0.y and if p0.x nop else block";
string PS_461 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_462_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_462_Desc = "ps_3_0 : sub is allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 break block";
string PS_462 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_463_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_463_Desc = "ps_3_0 : sub is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 break block";
string PS_463 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_464_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_464_Desc = "ps_3_0 : sub is allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_464 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_465_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_465_Desc = "ps_3_0 : sub is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 breakp p0.x block";
string PS_465 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_466_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_466_Desc = "ps_3_0 : sub is allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 break_eq c0.x, c0.y block";
string PS_466 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_467_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_467_Desc = "ps_3_0 : sub is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 break_eq c0.x, c0.y block";
string PS_467 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";


//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft
// premium shared source license agreement under which you licensed
// this source code. If you did not accept the terms of the license
// agreement, you are not authorized to use this source code.
// For the terms of the license, please see the license agreement
// signed by you and Microsoft.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
#pragma once

#include "common.h"

// From sysvad.h
#define VALUE_NORMALIZE_P(v, step) \
    ((((v) + (step)/2) / (step)) * (step))

#define VALUE_NORMALIZE(v, step) \
    ((v) > 0 ? VALUE_NORMALIZE_P((v), (step)) : -(VALUE_NORMALIZE_P(-(v), (step))))
    
#define VALUE_NORMALIZE_IN_RANGE_EX(v, min, max, step) \
    ((v) > (max) ? (max) : \
     (v) < (min) ? (min) : \
     VALUE_NORMALIZE((v), (step)))

class FmController
{

public:
    HRESULT Initialize(IMMDevice *pMMDevice);
    HRESULT SetFmVolume(_In_ LONG volume);
    HRESULT GetFmVolume(_Out_ LONG * plVolume);
    HRESULT SetFmEndpointId(_In_ LONG index);
    HRESULT GetFmEndpointId(_Out_ PKSTOPOLOGY_ENDPOINTID pEndpointId);
    HRESULT GetFmAntennaEndpointId(_Out_ PKSTOPOLOGY_ENDPOINTID pEndpointId);
    HRESULT SetFmState(_In_ BOOL state);
    HRESULT GetFmState(_Out_ BOOL * pbState);

private:
    CComPtr<IKsControl>     m_spWaveKsControl;      // KsControl for wave filter
    CComPtr<IKsControl>     m_spTopologyKsControl;  // KsControl for topology filter
};
#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// InvDestColor/Zero Test functions

CInvDestColorZeroTest::CInvDestColorZeroTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: Zero";
	m_szCommandKey = "InvDestColorZero";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "Zero";
}

//************************************************************************
// InvDestColor/One Test functions

CInvDestColorOneTest::CInvDestColorOneTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: One";
	m_szCommandKey = "InvDestColorOne";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_ONE;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "One";
}

//************************************************************************
// InvDestColor/SrcColor Test functions

CInvDestColorSrcColorTest::CInvDestColorSrcColorTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: SrcColor";
	m_szCommandKey = "InvDestColorSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "SrcColor";
}

//************************************************************************
// InvDestColor/InvSrcColor Test functions

CInvDestColorInvSrcColorTest::CInvDestColorInvSrcColorTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: InvSrcColor";
	m_szCommandKey = "InvDestColorInvSrcColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "InvSrcColor";
}

//************************************************************************
// InvDestColor/SrcAlpha Test functions

CInvDestColorSrcAlphaTest::CInvDestColorSrcAlphaTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: SrcAlpha";
	m_szCommandKey = "InvDestColorSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "SrcAlpha";
}

//************************************************************************
// InvDestColor/InvSrcAlpha Test functions

CInvDestColorInvSrcAlphaTest::CInvDestColorInvSrcAlphaTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: InvSrcAlpha";
	m_szCommandKey = "InvDestColorInvSrcAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "InvSrcAlpha";
}

//************************************************************************
// InvDestColor/DestAlpha Test functions

CInvDestColorDestAlphaTest::CInvDestColorDestAlphaTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: DestAlpha";
	m_szCommandKey = "InvDestColorDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "DestAlpha";
}

//************************************************************************
// InvDestColor/InvDestAlpha Test functions

CInvDestColorInvDestAlphaTest::CInvDestColorInvDestAlphaTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: InvDestAlpha";
	m_szCommandKey = "InvDestColorInvDestAlpha";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "InvDestAlpha";
}

//************************************************************************
// InvDestColor/DestColor Test functions

CInvDestColorDestColorTest::CInvDestColorDestColorTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: DestColor";
	m_szCommandKey = "InvDestColorDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "DestColor";
}

//************************************************************************
// InvDestColor/InvDestColor Test functions

CInvDestColorInvDestColorTest::CInvDestColorInvDestColorTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: InvDestColor";
	m_szCommandKey = "InvDestColorInvDestColor";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "InvDestColor";
}

//************************************************************************
// InvDestColor/SrcAlphaSat Test functions

CInvDestColorSrcAlphaSatTest::CInvDestColorSrcAlphaSatTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: SrcAlphaSat";
	m_szCommandKey = "InvDestColorSrcAlphaSat";

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "SrcAlphaSat";
}

//************************************************************************
// InvDestColor/BlendFactor Test functions

CInvDestColorBlendFactorTest::CInvDestColorBlendFactorTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: BlendFactor";
	m_szCommandKey = "InvDestColorBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_BLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}

//************************************************************************
// InvDestColor/InvBlendFactor Test functions

CInvDestColorInvBlendFactorTest::CInvDestColorInvBlendFactorTest()
{
	m_szTestName = "SrcBlend: InvDestColor DestBlend: InvBlendFactor";
	m_szCommandKey = "InvDestColorInvBlendFactor";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_BLENDFACTOR;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_INVDESTCOLOR;
	dwDestBlend = D3DBLEND_INVBLENDFACTOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "BlendFactor";
	
	// Overwrite Blend Factor variable
	bBlendFactor = true;
}
//////////////////////////////////////////////////////////////////////////
//  WGFCompute.h
//  created:	2009/03/23
//
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

// Includes
#include "D3D11Test.h"
#include <D3DSampleLocation.h> // for TRException
#include <strsafe.h>

#define ZeroStructMem(x) ZeroMemory(&(x), sizeof(x))
const size_t MAX_DEFINES = 100;

enum SHARED_MEMORY_TYPE
{
	SHARED_MEMORY_TGSM,
	SHARED_MEMORY_UAV,
};
DECLARE_NAMED_VALUES(SHARED_MEMORY_TYPE);

//------------------------------------------------------------------------------------------------
class CComputeTest : public CD3D11Test
{
protected:
	ID3D11Device				*m_pDevice;
	ID3D11DeviceContext			*m_pEffectiveContext;
	ID3D11Resource				*m_pTargetResource;
	ID3D11Resource				*m_pTargetResourceCopy;
	ID3D11UnorderedAccessView	*m_pTargetUAV;
	UINT						m_TargetUAVSize[3];
	DXGI_FORMAT					m_TargetUAVFormat;
	D3D11_UAV_DIMENSION			m_TargetUAVDimension;
	// optional
	D3D11_RESOURCE_MISC_FLAG    m_TargetUAVMiscFlags;
	D3D11_BUFFER_UAV_FLAG		m_TargetUAVBufferFlag;
	UINT						m_TargetUAVArraySize;
	UINT						m_TargetUAVElementByteStride;
	ID3D11ComputeShader			*m_pCS;
	// for test setup of pixel shader stage
	ID3D11InputLayout			*m_pInputLayout;
	ID3D11VertexShader			*m_pVertexShader;
	ID3D11Buffer				*m_pVertexBuffer;
	ID3D11Texture2D *m_pRTTex2D;
	ID3D11RenderTargetView *m_pRTView;

	CComputeTest() 
		: m_pDevice(NULL)
		, m_pEffectiveContext(NULL)
		, m_pTargetResource(NULL)
		, m_pTargetResourceCopy(NULL)
		, m_pTargetUAV(NULL)
		, m_TargetUAVFormat( DXGI_FORMAT_R32_UINT )
		, m_TargetUAVDimension( D3D11_UAV_DIMENSION_TEXTURE2DARRAY )
		, m_TargetUAVMiscFlags( D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )													  
		, m_TargetUAVBufferFlag( static_cast<D3D11_BUFFER_UAV_FLAG>(0) )																																  
		, m_TargetUAVArraySize( 0u )
		, m_TargetUAVElementByteStride( sizeof(UINT) )		 
		, m_pCS(NULL)
		, m_pInputLayout(NULL)
		, m_pVertexShader(NULL)
		, m_pVertexBuffer(NULL)
		, m_pRTTex2D(NULL)
		, m_pRTView(NULL)
	{ 
		m_TargetUAVSize[0] = 1;
		m_TargetUAVSize[1] = 1;
		m_TargetUAVSize[2] = 1;
	}

	virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase();
	virtual void CleanupTestCase();
	virtual void Cleanup();

	TEST_RESULT SetupVSStage();
	TEST_RESULT SetupRTV(UINT rtWidth, UINT rtHeight);

	// Helper functions
	HRESULT CompileAndCreateComputeShaderFromMemory(LPCSTR shaderStr, size_t shaderLen, LPCSTR entryPt, D3D_FEATURE_LEVEL minFL, ID3D11ComputeShader** pCS, CONST D3D10_SHADER_MACRO *pDefines = NULL );
	HRESULT CompileAndCreateComputeShaderFromResource(LPCSTR resourceStr, LPCSTR entryPt, D3D_FEATURE_LEVEL minFL, ID3D11ComputeShader** pCS, CONST D3D10_SHADER_MACRO *pDefines = NULL );
	HRESULT CompileAndCreatePixelShaderFromResource(LPCSTR resourceStr, LPCSTR entryPt, D3D_FEATURE_LEVEL minFL, 
													ID3D11PixelShader** ppPS, CONST D3D10_SHADER_MACRO *pDefines );
	bool SkipSlowOnRefWithMessage( bool isSlow ); // Test cases will be skipped on REF if they run too slow 

	template<typename T>
	HRESULT MapAndRead(ID3D11DeviceContext* pContext, ID3D11Resource* pResource, T & outVal)
	{
		HRESULT hr = E_UNEXPECTED;
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if( FAILED( hr = pContext->Map( pResource, 0, D3D11_MAP_READ, 0, &mappedResource ) ) )
			return hr;

		memcpy(&outVal, mappedResource.pData, sizeof(outVal));

		pContext->Unmap( pResource, 0 );

		return hr;
	}

	template<typename T> 
	HRESULT MapAndWrite(ID3D11DeviceContext* pContext, ID3D11Resource* pResource, const T & inVal)
	{
		HRESULT hr = E_UNEXPECTED;
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if( FAILED( hr = pContext->Map( pResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			return hr;

		memcpy(mappedResource.pData, &inVal, sizeof(inVal));

		pContext->Unmap( pResource, 0 );

		return hr;
	}
};

#include "Interaction.h"

//------------------------------------------------------------------------------------------------
class CComputeTestApp : public CD3D11TestFramework
{
public:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	virtual bool InitTestGroups();
	virtual void InitOptionVariables();
	UINT GetDefaultQueryTimeout() { return 30000; }

	bool m_bRunSlowOnRef;
	bool m_bSkipSlowOnHardware;
	bool m_bLogShaderText;
};

extern CComputeTestApp  g_TestApp;
#include "AsyncConf.h" 

// TODO: test well-known counters
// TODO: test custom counters

TEST_RESULT
CCounterConfTest::Setup()
{
	CAsyncConfTest::Setup();
	D3D11_COUNTER_INFO counterInfo;
	GetDevice()->CheckCounterInfo(&counterInfo);
	if( counterInfo.LastDeviceDependentCounter > 0 )
		SkipMatching( FilterEqual( _T( "CounterName" ), D3D11_COUNTER_DEVICE_DEPENDENT_0 ) && FilterGreaterEqual( _T( "Dev_Dep_Ctr" ), counterInfo.LastDeviceDependentCounter - D3D11_COUNTER_DEVICE_DEPENDENT_0 ) );
	else
		SkipMatching( FilterEqual( _T( "CounterName" ), D3D11_COUNTER_DEVICE_DEPENDENT_0 ) && FilterGreaterEqual( _T( "Dev_Dep_Ctr" ), 0 ) );
	return RESULT_PASS;
}

void
CCounterConfTest::Cleanup()
{
	CAsyncConfTest::Cleanup();
}

void CCounterConfTest::InitTestParameters()
{
	CUserValueSet<D3D11_COUNTER> *pCounterNameValues = new CUserValueSet< D3D11_COUNTER >();

	AddParameter<D3D11_COUNTER>( _T( "CounterName" ), &m_CounterName);
	testfactor::RFactor rCounterName = AddParameterValueSet( _T( "CounterName"), pCounterNameValues );
	testfactor::RFactor rCounterName_Dev_Dep = AddParameterValue( _T( "CounterName"), D3D11_COUNTER_DEVICE_DEPENDENT_0 );
	AddParameter<int>( _T( "Dev_Dep_Ctr" ), &m_nDeviceDependentNbr);
	SetParameterDefaultValue<int>( _T( "Dev_Dep_Ctr" ), -1);
	testfactor::RFactor rDevDepCtr = AddParameterValueSet( _T( "Dev_Dep_Ctr"), RangeValueSet<int>(0, 100, 1) );

	testfactor::RFactor rGpuJob = AddParameter<int>( _T( "Num_GpuJobs" ), &m_nGpuJobs, RangeValueSet(0, 5, 1) );

	SetRootTestFactor( (rCounterName + rCounterName_Dev_Dep * rDevDepCtr) * rGpuJob );
}

TEST_RESULT CCounterConfTest::ExecuteTestCase()
{
	HRESULT hr = S_OK, hr2 = S_OK;
	TEST_RESULT tRes = RESULT_PASS;
	ID3D11Counter *pCounter = NULL;

	D3D11_COUNTER_DESC counterDesc;
	if (m_CounterName == D3D11_COUNTER_DEVICE_DEPENDENT_0)
		counterDesc.Counter = (D3D11_COUNTER)(m_CounterName + m_nDeviceDependentNbr);
	else
		counterDesc.Counter = m_CounterName;
	counterDesc.MiscFlags = 0;
	hr = GetDevice()->CreateCounter(&counterDesc, &pCounter);
	if ( FAILED(hr) || pCounter == NULL)
	{
		if (hr == DXGI_ERROR_UNSUPPORTED || counterDesc.Counter >= D3D11_COUNTER_DEVICE_DEPENDENT_0)
		{
			tRes = RESULT_SKIP;
			WriteToLog( _T("Unsupported Counter. hr=%s\n"), D3DHResultToString(hr).c_str() );
		}
		else
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T("Failed to create Counter. hr=%s\n"), D3DHResultToString(hr).c_str() );
		}
		goto testDone;
	}

	D3D11_COUNTER_TYPE sCounterType;
	UINT sActiveCounters;
	hr2 = GetDevice()->CheckCounter(
		&counterDesc, 
		&sCounterType, 
		&sActiveCounters, 
		NULL, NULL, 
		NULL, NULL,
		NULL, NULL
		);

	if ( FAILED(hr2) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("Failed to perform CheckCounter call for this counter type. hr=%s\n"), D3DHResultToString(hr2).c_str() );
		goto testDone;
	}

	SetupGpuJob();

	GetEffectiveContext()->Begin(pCounter);

	// TODO: draw or stream-out something

	for (int n = 0; n < m_nGpuJobs; ++n)
	{
		GpuBoundJob(false);
	}

	GetEffectiveContext()->End(pCounter);

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	// TODO: test counter for reasonable value
	if (m_CounterName >= D3D11_COUNTER_DEVICE_DEPENDENT_0)
	{

	}
	else
		assert(!"Unexpected counter value");


testDone:
	// cleanup
	SAFE_RELEASE( pCounter );

	return tRes;
}

#ifndef __TEST_EVICT_H
#define __TEST_EVICT_H

// This component tests eviction.

class CTestSlowEviction : public CGenericI3DComponent
{

public:
    //  In the constructor, we describe this component, and each variable it contains
    CTestSlowEviction();    

protected:
    //  Overrides from CGenericI3DComponent
    //  This is where we actually execute the test.
    virtual void RunTest();
    virtual void EndTest();

private:

    HRESULT EvictVB();
    HRESULT EvictTex2();
    HRESULT EvictVolTex();

    // utility function to fill with or check allocation pattern
    HRESULT ResourcePattern(UINT size, BYTE* Start, DWORD dwFillPattern, DWORD Mask, BOOL bFill, BOOL bCheck);

    LONGLONG                m_size;    // Size of total video memory in bytes
    UINT                    m_fault;   // fault that will be set    
    bool                    m_bBVT;	   // run in BVT mode
};

void SetFault(UINT mask);
DWORD WINAPI ControlFault(LPVOID mask); 

#endif

#define D3D_OVERLOADS
#include <d3d.h>
#include "BasicRst.h"

#define FVF_VERTEX ( D3DFVF_XYZRHW | D3DFVF_TEX1 )

//************************************************************************
// NoDiffuse Test functions

CNoDiffuseTest::CNoDiffuseTest()
{
	m_szTestName = "BasicRst NoDiffuse";
	m_szCommandKey = "NoDiffuse";
}

CNoDiffuseTest::~CNoDiffuseTest()
{
}

UINT CNoDiffuseTest::TestInitialize(void)
{
    PRENDERPRIMITIVEAPI m_pFullAPIList;
    DWORD               m_dwFullAPIListSize;

    if (m_dwVersion >= 0x0800)
    {
        m_pFullAPIList = m_pRPFullDX8APIList;
        m_dwFullAPIListSize = m_dwRPFullDX8APIListSize;
    }
    else if (m_dwVersion >= 0x0700)
    {
        m_pFullAPIList = m_pRPFullDX7APIList;
        m_dwFullAPIListSize = m_dwRPFullDX7APIListSize;
    }
    else
    {
        m_pFullAPIList = m_pRPFullDX6APIList;
        m_dwFullAPIListSize = m_dwRPFullDX6APIListSize;
    }

    // Setup the Test range
	SetTestRange(1, m_dwFullAPIListSize);

    // Tell RenderPrimitive to cycle through all APIs
    SetAPI(m_pFullAPIList, m_dwFullAPIListSize, 1);

    // Set compare function to require 85%
	m_pFramework->GetImageCmp()->SetPassRequired(0.85f);

	return D3DTESTINIT_RUN;
}

bool CNoDiffuseTest::ExecuteTest(UINT uTestNum)
{
    DWORD dwAPI;

	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Let's build some triangles.
	DrawNonDiffuseGrid();

	// Get the current API
    dwAPI = m_pSrcDevice->RemapAPI(GetAPI(), D3DPT_TRIANGLELIST, FVF_VERTEX);

    // Tell log which API we are using
    switch (dwAPI)
    {
        case RP_BE:             sprintf(msgString, "%sFVF: 0x%08x, API: Begin/Vertex/End", msgString, FVF_VERTEX);
                                break;
        case RP_BIE:            sprintf(msgString, "%sFVF: 0x%08x, API: BeginIndexed/Index/End", msgString, FVF_VERTEX);
                                break;
        case RP_DP:             sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitive", msgString, FVF_VERTEX);
                                break;
        case RP_DIP:            sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitive", msgString, FVF_VERTEX);
                                break;
        case RP_DPS:            sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveStrided", msgString, FVF_VERTEX);
                                break;
        case RP_DIPS:           sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveStrided", msgString, FVF_VERTEX);
                                break;
        case RP_DPVB:           sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB", msgString, FVF_VERTEX);
                                break;
        case RP_DIPVB:          sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB", msgString, FVF_VERTEX);
                                break;
        case RP_DPVB_PV:        sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB after ProcessVertices", msgString, FVF_VERTEX);
                                break;
        case RP_DIPVB_PV:       sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB after ProcessVertices", msgString, FVF_VERTEX);
                                break;
        case RP_DPVB_OPT:       sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB with Optimized VB", msgString, FVF_VERTEX);
                                break;
        case RP_DIPVB_OPT:      sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB with Optimized VB", msgString, FVF_VERTEX);
                                break;
        case RP_DPVB_OPT_PV:    sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB after ProcessVert with Optimized VB", msgString, FVF_VERTEX);
                                break;
        case RP_DIPVB_OPT_PV:   sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB after ProcessVert with Optimized VB", msgString, FVF_VERTEX);
                                break;
        case RP_DPVB_PVS:       sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB after ProcessVerticesStrided", msgString, FVF_VERTEX);
                                break;
        case RP_DIPVB_PVS:      sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB after ProcessVerticesStrided", msgString, FVF_VERTEX);
                                break;
        default:                sprintf(msgString, "%sError: Unknown API", msgString);
                                break;
    }

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    return true;
}

void CNoDiffuseTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,FVF_VERTEX,NoDiffuseList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

//************************************************************************
// Internal API:    DrawNonDiffuseGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CNoDiffuseTest::DrawNonDiffuseGrid(void)
{
	// Use standard ((0,0)(1,0)(0,1)(1,1)) mapping
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(D3DVECTOR(  9.5f,   9.5f, 0.9f), (1.0f / 0.9f), 0.0f,0.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(D3DVECTOR(265.5f, 265.5f, 0.9f), (1.0f / 0.9f), 1.0f,1.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(D3DVECTOR(  9.5f, 265.5f, 0.9f), (1.0f / 0.9f), 0.0f,1.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(D3DVECTOR(  9.5f,   9.5f, 0.9f), (1.0f / 0.9f), 0.0f,0.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(D3DVECTOR(265.5f,   9.5f, 0.9f), (1.0f / 0.9f), 1.0f,0.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(D3DVECTOR(265.5f, 265.5f, 0.9f), (1.0f / 0.9f), 1.0f,1.0f);
}

#include "CD3DWindowFramework.h"
#include "CCopyRects.h"

#include "dx8fmt.h"
#include "dx8usage.h"
#include "dx8pool.h"
#include "dims.h"
#include "rectsets.h"
#include "dx8multisample.h"

#include "resource.h"

#include <stdio.h>

//#include <d3d9.h>
#include <d3dx8.h>

extern CD3DWindowFramework TestApp;
extern CCopyRects MainTestEnv;

// Static class variables
clsTestSurface::TSListElement *clsTestSurface::pFirstElement = NULL;
UINT clsTestSurface::uiNoOfElements = 0;

// Global
static const float fMinimumConformanceFraction = 0.85f;

// Auxiliary functions

inline static char *EndOfString(const char *szString)
{
	return (char *)(szString + strlen(szString));
}

inline static DWORD ExtractBitsRange(DWORD dwSourceBits, UINT uiLSB, UINT uiMSB)
{
	return (dwSourceBits >> uiLSB) & (DWORD)((UINT)((DWORD)1 << (uiMSB - uiLSB + 1)) - 1);
}

static void LogAppendLine(const char *szLineString)
{
	MainTestEnv.WriteToLog(TEXT(szLineString));
}

static clsTestSurface *CreateSurfaceFromDWORD(DWORD dwSpec, D3DFORMAT d3dfmt)
{
	clsTestSurface *ptestsurfNew = NULL;
	UINT uiTypeIndex;

	switch (uiTypeIndex = (UINT)ExtractBitsRange(dwSpec, 6, 9))
	{
	case 0:
	// Texture surface
		{
			clsTextureSurface *ptexsurfNew = NULL;
			UINT uiUsageIndex, uiPoolTypeIndex, uiDimensionIndex;

			if ((uiUsageIndex = (UINT)ExtractBitsRange(dwSpec, 4, 5)) < cuiNoOfUsageSets)
				if ((uiPoolTypeIndex = (UINT)ExtractBitsRange(dwSpec, 2, 3)) < cuiNoOfPoolTypes)
					if ((uiDimensionIndex = (UINT)ExtractBitsRange(dwSpec, 0, 1)) < cuiNoOfDimensions)
						if (ptexsurfNew = new clsTextureSurface)
							if
							(
								ptexsurfNew -> Initialize
								(
									SurfaceDimensions[uiDimensionIndex].uiWidth,
									SurfaceDimensions[uiDimensionIndex].uiHeight,
									d3dfmt,
									dwUsageSets[uiUsageIndex],
									DX8PoolTypes[uiPoolTypeIndex].d3dpool
								)
							)
								ptestsurfNew = ptexsurfNew;
							else
								delete ptexsurfNew;
		}
		break;
	case 1:
	// Render target surface
		{
			clsRenderTargetSurface *prtsurfNew = NULL;
			UINT uiDimensionIndex, uiMultiSampleIndex;

			if ((uiDimensionIndex = (UINT)ExtractBitsRange(dwSpec, 0, 1)) < cuiNoOfDimensions)
				if ((uiMultiSampleIndex = (UINT)ExtractBitsRange(dwSpec, 3, 5)) < cuiNoOfMultiSampleTypes)
					if (prtsurfNew = new clsRenderTargetSurface)
						if
						(
							prtsurfNew -> Initialize
							(
								SurfaceDimensions[uiDimensionIndex].uiWidth,
								SurfaceDimensions[uiDimensionIndex].uiHeight,
								d3dfmt,
								DX8MultiSampleTypes[uiMultiSampleIndex].d3dmst,
								(dwSpec & 4) ? TRUE : FALSE
							)
						)
							ptestsurfNew = prtsurfNew;
						else
							delete prtsurfNew;
		}
		break;
	default:
		break;
	}

	if (ptestsurfNew)
		ptestsurfNew -> SetDWORDSpec(dwSpec);

	return ptestsurfNew;
}

static bool CreateCompatibleSMSurface(CSurface *lpd3dsurf8, clsTextureSurface *pSystemMemSurface)
{
	SURFACEDESC d3dsd;

	// Obtain a description of the source surface.
	lpd3dsurf8 -> GetDesc(&d3dsd);

	// Create a (lockable) system memory surface compatible with the
	// source surface.  This is used to load the source surface.
	if
	(
		pSystemMemSurface -> Initialize
		(
			d3dsd.dwWidth,
			d3dsd.dwHeight,
			(d3dsd.Format).d3dfFormat,
			0,
			D3DPOOL_SYSTEMMEM
		)
	)
		return true;

	return false;
}

static bool ReferenceCopyRects
(
  CSurface *pSourceSurface,
  CONST RECT* pSourceRectsArray,
  UINT cRects,
  CSurface *pDestinationSurface,
  CONST POINT* pDestPointsArray
);

static bool CompareRows(BYTE *pRow1, BYTE *pRow2, UINT uiByteCount, DWORD dwMask);
static bool CompareSurfaces(CSurface *lpd3dsurf81, CSurface *lpd3dsurf82);

// Methods for test surfaces, etc.

// clsTestSurface methods

void clsTestSurface::ReleaseAllDX8Objects()
{
	TSListElement *pCurrentElement;

	if (pCurrentElement = pFirstElement)
		do
		{
			(pCurrentElement -> pTestSurface) -> ReleaseDX8Objects();
		}
		while (pCurrentElement = pCurrentElement -> pNextElement);
}

void clsTestSurface::AddToClassList()
{
	TSListElement *pNewElement = new TSListElement;

	pNewElement -> pTestSurface = this;
	pNewElement -> pNextElement = pFirstElement;
	pFirstElement = pNewElement;
	++uiNoOfElements;
}

void clsTestSurface::RemoveFromClassList()
{
	TSListElement **ppCurrentElement, *pCurrentElement;

	ppCurrentElement = &pFirstElement;

	if ((pCurrentElement = *ppCurrentElement) -> pTestSurface != this)
		do
		{
			ppCurrentElement = &(pCurrentElement -> pNextElement);
			pCurrentElement = *ppCurrentElement;
		}
		while (pCurrentElement -> pTestSurface != this);

	*ppCurrentElement = pCurrentElement -> pNextElement;
	delete pCurrentElement;
	--uiNoOfElements;
}

void clsTestSurface::ReleaseDX8Objects()
{
	if (lpd3dsurf8)
	{
		lpd3dsurf8 -> Release();
		lpd3dsurf8 = NULL;
	}

	ReleaseSurfaceContainer();
}

void clsTestSurface::Describe(char *szDescription)
{
	HRESULT hresult;
	SURFACEDESC d3dsd;

	if (FAILED((hresult = GetSurfacePointer() -> GetDesc(&d3dsd))))
		sprintf(szDescription, "(Unable to retrieve surface details.  GetDesc failed.)");
	else
	{
		const D3DFORMAT_RECORD *pFormatRecord;
		const D3DPOOL_RECORD *pPoolRecord;
		const D3DMULTISAMPLE_TYPE_RECORD *pMultiSampleRecord;

		pFormatRecord = FindFormatRecord((d3dsd.Format).d3dfFormat);
		pMultiSampleRecord = FindMultiSampleRecord((D3DMULTISAMPLE_TYPE)(d3dsd.MultiSampleType));
		pPoolRecord = FindPoolRecord((D3DPOOL)(d3dsd.Pool));

		sprintf(szDescription, "Container/Type: %s; Dimensions: %dx%d; Format: %s; MultiSample Type: %s; Pool: %s; Usage Flags: ",
		SurfaceTypeName(), d3dsd.dwWidth, d3dsd.dwHeight,
		(pFormatRecord ? pFormatRecord -> szName : "(Unrecognized Format)"),
		(pMultiSampleRecord ? pMultiSampleRecord -> szName : "(Unrecognized MultiSample Type)"),
		(pPoolRecord ? pPoolRecord -> szName : "(Unrecognized Pool Type)"));
		ListUsages(d3dsd.Usage, EndOfString(szDescription));
	}
}

D3DMULTISAMPLE_TYPE clsTestSurface::GetMultisampleType()
{
	HRESULT hresult;
	SURFACEDESC d3dsd;

	hresult = GetSurfacePointer() -> GetDesc(&d3dsd);
	return (D3DMULTISAMPLE_TYPE)(d3dsd.MultiSampleType);
}

// clsTextureSurface methods

void clsTextureSurface::ReleaseSurfaceContainer()
{
	if (lpd3dtex8)
	{
		lpd3dtex8 -> Release();
		lpd3dtex8 = NULL;
	}
}

const char *clsTextureSurface::SurfaceTypeName()
{
	return "Texture";
}

bool clsTextureSurface::Initialize(UINT uiWidth, UINT uiHeight, D3DFORMAT d3dfmt, DWORD dwUsage, D3DPOOL d3dpool)
{
	HRESULT hresult;

	if
	(
		SUCCEEDED
		(
			MainTestEnv.LogCreateTexture
			(
				uiWidth,
				uiHeight,
				1,
				dwUsage, // | USAGE_HEAVYWEIGHT,
				d3dfmt,
				d3dpool,
				&lpd3dtex8
			)
		)
	)
	{
		if (SUCCEEDED((hresult = lpd3dtex8 -> GetSurfaceLevel(0, &lpd3dsurf8))))
			return true;

		lpd3dsurf8 = NULL;

		lpd3dtex8 -> Release();
		lpd3dtex8 = NULL;
	}

	return false;
}

// clsRenderTargetSurface methods

void clsRenderTargetSurface::ReleaseSurfaceContainer()
{
	if (lpd3dsurfRenderTarget)
	{
		lpd3dsurfRenderTarget -> Release();
		lpd3dsurfRenderTarget = NULL;
	}
}

const char *clsRenderTargetSurface::SurfaceTypeName()
{
	if (dwSpec & 4)
		return "Render Target (Lockable)";
	else
		return "Render Target (Not Lockable)";
}

bool clsRenderTargetSurface::Initialize(UINT uiWidth, UINT uiHeight, D3DFORMAT d3dfmt, D3DMULTISAMPLE_TYPE d3dmst, BOOL bLockable)
{
	HRESULT hresult;

	if
	(
		SUCCEEDED
		(
			(
				hresult =
				MainTestEnv.LogCreateRenderTarget
				(
					uiWidth,
					uiHeight,
					d3dfmt,
					d3dmst,
					bLockable,
					&lpd3dsurfRenderTarget
				)
			)
		)
	)
	{
		hresult = (lpd3dsurf8 = lpd3dsurfRenderTarget) -> AddRef();
		return true;
	}

	return false;
}

// clsSurfaceSpecList methods

clsSurfaceSpecList::~clsSurfaceSpecList()
{
	SurfaceSpecListElement *pCurrentElement, *pNextElement;

	if (pCurrentElement = pFirst)
		do
		{
			pNextElement = pCurrentElement -> pNext;
			delete pCurrentElement;
		}
		while (pCurrentElement = pNextElement);
}

void clsSurfaceSpecList::AddSurfaceSpec(DWORD dwSurfaceSpec)
{
	SurfaceSpecListElement *pNewElement = new SurfaceSpecListElement;

	pNewElement -> dwSurfaceSpec = dwSurfaceSpec;
	pNewElement -> pNext = NULL;

	if (pFirst)
		pLast = (pLast -> pNext = pNewElement);
	else
		pLast = (pFirst = pNewElement);

	++uiNoOfElements;
}

void clsSurfaceSpecList::InitializeSurfaceSpecArray(clsSurfaceSpecArray *pTargetArray)
{
	if (pTargetArray -> pdwSurfaceSpecs)
	{
		delete [] (pTargetArray -> pdwSurfaceSpecs);
		pTargetArray -> pdwSurfaceSpecs = NULL;
	}
	pTargetArray -> uiNoOfElements = 0;

	if (uiNoOfElements)
	{
		DWORD *dwAllocatedMemory = NULL;
		UINT uiIndex = 0;
		SurfaceSpecListElement *pCurrentElement = pFirst;

		dwAllocatedMemory = new DWORD[uiNoOfElements];

		if (!dwAllocatedMemory)
			DebugOut.Write("Memory allocation failed in clsSurfaceSpecList::InitializeSurfaceSpecArray.\n");
		else
		{
			do
			{
				dwAllocatedMemory[uiIndex++] = pCurrentElement -> dwSurfaceSpec;
			}
			while (pCurrentElement = pCurrentElement -> pNext);

			pTargetArray -> pdwSurfaceSpecs = dwAllocatedMemory;
			pTargetArray -> uiNoOfElements = uiNoOfElements;
		}
	}
}

// clsSurfaceSpecArray methods

clsSurfaceSpecArray::~clsSurfaceSpecArray()
{
	if (pdwSurfaceSpecs)
		delete [] pdwSurfaceSpecs;
}

DWORD clsSurfaceSpecArray::GetElementValue(unsigned int uiIndex)
{
	if (/*(uiIndex >= 0) &&*/ (uiIndex < uiNoOfElements))
		return pdwSurfaceSpecs[uiIndex];
	else
		return 0;
}

// clsSourceSurfaceSet methods

void clsSourceSurfaceSet::ReleaseAllSurfaces()
{
	texsurfSourceLoader.ReleaseDX8Objects();

	if (pSourceTestSurface)
	{
		delete pSourceTestSurface;
		pSourceTestSurface = NULL;
	}

	bInitialized = false;
	d3dfmtSetFormat = D3DFMT_UNKNOWN;
	dwMainSurfaceSpec = 0;
}

bool clsSourceSurfaceSet::PrepareFromDWORD(DWORD dwSurfaceSpec, D3DFORMAT d3dfmt)
{
	if (bInitialized)
		if ((d3dfmtSetFormat == d3dfmt) && (dwMainSurfaceSpec == dwSurfaceSpec))
			return true;
		else
			ReleaseAllSurfaces();

	if (!(pSourceTestSurface = CreateSurfaceFromDWORD(dwSurfaceSpec, d3dfmt)))
		MainTestEnv.WriteToLog("Unable to create source surface.\n");
	else
	{
		CSurface *lpd3dsurf8Source = NULL;

		pSourceTestSurface -> Describe(szDescription);

		// Retrieve a pointer to the DX8 surface interface
		// for the source surface.
		lpd3dsurf8Source = pSourceTestSurface -> GetSurfacePointer();

		if (!CreateCompatibleSMSurface(lpd3dsurf8Source, &texsurfSourceLoader))
			MainTestEnv.WriteToLog("Unable to create a system memory surface compatible with the source surface.\n");
		else
		{
			CSurface *lpd3dsurf8SourceLoader;
			HRESULT hresult;
			D3DXIMAGE_INFO d3dxii;

			// Retrieve a pointer to the "loader surface".
			lpd3dsurf8SourceLoader = texsurfSourceLoader.GetSurfacePointer();

			// Load the "foreground" image onto the surface.
			if
			(
				FAILED
				(
					(
						hresult =
						(MainTestEnv.m_pSrcDevice) ->
						D3DXLoadSurfaceFromFileInMemory
						(
							lpd3dsurf8SourceLoader,
							NULL,
							NULL,
							MainTestEnv.BitmapDataForeground,
							((PBITMAPFILEHEADER)(MainTestEnv.BitmapDataForeground)) -> bfSize,
							NULL,
							D3DX_FILTER_LINEAR | D3DX_FILTER_MIRROR,
							0,
							&d3dxii
						)
					)
				)
			)
				MainTestEnv.WriteToLog("Unable to load the source-compatible system memory surface with the 'foreground image'.\n");
			else
			{
				// Copy the contents of the loader surface onto
				// the source surface.
				if
				(
					FAILED
					(
						(
							hresult =
							(MainTestEnv.GetCurrentSrcDevice()) -> CopyRects
							(
								lpd3dsurf8SourceLoader,
								NULL,
								0,
								lpd3dsurf8Source,
								NULL
							)
						)
					)
				)
					MainTe