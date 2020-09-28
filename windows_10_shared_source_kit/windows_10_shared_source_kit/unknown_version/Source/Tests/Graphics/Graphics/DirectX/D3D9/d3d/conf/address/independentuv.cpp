#define D3D_OVERLOADS
#include <d3d.h>
#include "Address.h"

//************************************************************************
// WrapMirror Test functions

CWrapMirrorTest::CWrapMirrorTest()
{
	m_szTestName = "Texture Address WrapU MirrorV";
	m_szCommandKey = "WrapMirror";

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_WRAP;
	dwAddressCapV = D3DPTADDRESSCAPS_MIRROR;

	// Give parent a printable name for the caps
	szAddrU = "WrapU";
	szAddrV = "MirrorV";
}

bool CWrapMirrorTest::Setup(void)
{
	// Turn on WrapU MirrorV addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_WRAP);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_MIRROR);

	return CAddressTest::Setup();
}

//************************************************************************
// WrapClamp Test functions

CWrapClampTest::CWrapClampTest()
{
	m_szTestName = "Texture Address WrapU ClampV";
	m_szCommandKey = "WrapClamp";

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_WRAP;
	dwAddressCapV = D3DPTADDRESSCAPS_CLAMP;

	// Give parent a printable name for the caps
	szAddrU = "WrapU";
	szAddrV = "ClampV";
}

bool CWrapClampTest::Setup(void)
{
	// Turn on WrapU ClampV addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_WRAP);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_CLAMP);

	return CAddressTest::Setup();
}

//************************************************************************
// WrapBorder Test functions

CWrapBorderTest::CWrapBorderTest()
{
	m_szTestName = "Texture Address WrapU BorderV";
	m_szCommandKey = "WrapBorder";

	// Tell parent to use border colors and check indpendent address modes
	bBorder = true;
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_WRAP;
	dwAddressCapV = D3DPTADDRESSCAPS_BORDER;

	// Give parent a printable name for the caps
	szAddrU = "WrapU";
	szAddrV = "BorderV";
}

bool CWrapBorderTest::Setup(void)
{
	// Turn on WrapU BorderV addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_WRAP);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_BORDER);

	return CAddressTest::Setup();
}

//************************************************************************
// MirrorWrap Test functions

CMirrorWrapTest::CMirrorWrapTest()
{
	m_szTestName = "Texture Address MirrorU WrapV";
	m_szCommandKey = "MirrorWrap";

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_MIRROR;
	dwAddressCapV = D3DPTADDRESSCAPS_WRAP;

	// Give parent a printable name for the caps
	szAddrU = "MirrorU";
	szAddrV = "WrapV";
}

bool CMirrorWrapTest::Setup(void)
{
	// Turn on MirrorU WrapV addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_WRAP);

	return CAddressTest::Setup();
}

//************************************************************************
// MirrorClamp Test functions

CMirrorClampTest::CMirrorClampTest()
{
	m_szTestName = "Texture Address MirrorU ClampV";
	m_szCommandKey = "MirrorClamp";

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_MIRROR;
	dwAddressCapV = D3DPTADDRESSCAPS_CLAMP;

	// Give parent a printable name for the caps
	szAddrU = "MirrorU";
	szAddrV = "ClampV";
}

bool CMirrorClampTest::Setup(void)
{
	// Turn on MirrorU ClampV addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_CLAMP);

	return CAddressTest::Setup();
}

//************************************************************************
// MirrorBorder Test functions

CMirrorBorderTest::CMirrorBorderTest()
{
	m_szTestName = "Texture Address MirrorU BorderV";
	m_szCommandKey = "MirrorBorder";

	// Tell parent to use border colors and check indpendent address modes
	bBorder = true;
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_MIRROR;
	dwAddressCapV = D3DPTADDRESSCAPS_BORDER;

	// Give parent a printable name for the caps
	szAddrU = "MirrorU";
	szAddrV = "BorderV";
}

bool CMirrorBorderTest::Setup(void)
{
	// Turn on MirrorU BorderV addressingaddressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_BORDER);

	return CAddressTest::Setup();
}

//************************************************************************
// ClampWrap Test functions

CClampWrapTest::CClampWrapTest()
{
	m_szTestName = "Texture Address ClampU WrapV";
	m_szCommandKey = "ClampWrap";

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_CLAMP;
	dwAddressCapV = D3DPTADDRESSCAPS_WRAP;

	// Give parent a printable name for the caps
	szAddrU = "ClampU";
	szAddrV = "WrapV";
}

bool CClampWrapTest::Setup(void)
{
	// Turn on ClampU WrapV addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_WRAP);

	return CAddressTest::Setup();
}

//************************************************************************
// ClampMirror Test functions

CClampMirrorTest::CClampMirrorTest()
{
	m_szTestName = "Texture Address ClampU MirrorV";
	m_szCommandKey = "ClampMirror";

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_CLAMP;
	dwAddressCapV = D3DPTADDRESSCAPS_MIRROR;

	// Give parent a printable name for the caps
	szAddrU = "ClampU";
	szAddrV = "MirrorV";
}

bool CClampMirrorTest::Setup(void)
{
	// Turn on ClampU MirrorV addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_MIRROR);

	return CAddressTest::Setup();
}

//************************************************************************
// ClampBorder Test functions

CClampBorderTest::CClampBorderTest()
{
	m_szTestName = "Texture Address ClampU BorderV";
	m_szCommandKey = "ClampBorder";

	// Tell parent to use border colors and check indpendent address modes
	bBorder = true;
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_CLAMP;
	dwAddressCapV = D3DPTADDRESSCAPS_BORDER;

	// Give parent a printable name for the caps
	szAddrU = "ClampU";
	szAddrV = "BorderV";
}

bool CClampBorderTest::Setup(void)
{
	// Turn on ClampU BorderV addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_BORDER);

	return CAddressTest::Setup();
}

//************************************************************************
// BorderWrap Test functions

CBorderWrapTest::CBorderWrapTest()
{
	m_szTestName = "Texture Address BorderU WrapV";
	m_szCommandKey = "BorderWrap";

	// Tell parent to use border colors and check indpendent address modes
	bBorder = true;
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_BORDER;
	dwAddressCapV = D3DPTADDRESSCAPS_WRAP;

	// Give parent a printable name for the caps
	szAddrU = "BorderU";
	szAddrV = "WrapV";
}

bool CBorderWrapTest::Setup(void)
{
	// Turn on BorderU WrapV addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_BORDER);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_WRAP);

	return CAddressTest::Setup();
}

//************************************************************************
// BorderMirror Test functions

CBorderMirrorTest::CBorderMirrorTest()
{
	m_szTestName = "Texture Address BorderU MirrorV";
	m_szCommandKey = "BorderMirror";

	// Tell parent to use border colors and check indpendent address modes
	bBorder = true;
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_BORDER;
	dwAddressCapV = D3DPTADDRESSCAPS_MIRROR;

	// Give parent a printable name for the caps
	szAddrU = "BorderU";
	szAddrV = "MirrorV";
}

bool CBorderMirrorTest::Setup(void)
{
	// Turn on BorderU MirrorV addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_BORDER);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_MIRROR);

	return CAddressTest::Setup();
}

//************************************************************************
// BorderClamp Test functions

CBorderClampTest::CBorderClampTest()
{
	m_szTestName = "Texture Address BorderU ClampV";
	m_szCommandKey = "BorderClamp";

	// Tell parent to use border colors and check indpendent address modes
	bBorder = true;
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_BORDER;
	dwAddressCapV = D3DPTADDRESSCAPS_CLAMP;

	// Give parent a printable name for the caps
	szAddrU = "BorderU";
	szAddrV = "ClampV";
}

bool CBorderClampTest::Setup(void)
{
	// Turn on BorderU ClampV addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_BORDER);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_CLAMP);

	return CAddressTest::Setup();
}

