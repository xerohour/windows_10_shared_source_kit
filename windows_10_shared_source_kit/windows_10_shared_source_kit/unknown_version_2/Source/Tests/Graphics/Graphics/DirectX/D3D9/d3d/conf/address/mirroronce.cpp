#define D3D_OVERLOADS
#include <d3d8.h>
#include "types.h"
#include "Address.h"

//************************************************************************
// Mirror Test functions

CMirrorOnceTest::CMirrorOnceTest()
{
	m_szTestName = "Texture Address MirrorOnce";
	m_szCommandKey = "MirrorOnce";

    // Init framework options
	m_Options.D3DOptions.fMinDXVersion = 8.0f;

	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE | PF_ALPHA;
	m_Options.ModeOptions.dwTexExclude = PF_METN;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_MIRRORONCE;
	dwAddressCapV = D3DPTADDRESSCAPS_MIRRORONCE;
}

bool CMirrorOnceTest::Setup(void)
{
	// Turn on MirrorOnce addressing
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_MIRRORONCE);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_MIRRORONCE);

	return CAddressTest::Setup();
}

//************************************************************************
// WrapMirrorOnce Test functions

CWrapMirrorOnceTest::CWrapMirrorOnceTest()
{
	m_szTestName = "Texture Address WrapU MirrorOnceV";
	m_szCommandKey = "WrapMirrorOnce";

    // Init framework options
	m_Options.D3DOptions.fMinDXVersion = 8.0f;

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_WRAP;
	dwAddressCapV = D3DPTADDRESSCAPS_MIRRORONCE;

	// Give parent a printable name for the caps
	szAddrU = "WrapU";
	szAddrV = "MirrorOnceV";
}

bool CWrapMirrorOnceTest::Setup(void)
{
	// Turn on WrapU MirrorOnceV addressing
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_WRAP);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_MIRRORONCE);

	return CAddressTest::Setup();
}

//************************************************************************
// MirrorMirrorOnce Test functions

CMirrorMirrorOnceTest::CMirrorMirrorOnceTest()
{
	m_szTestName = "Texture Address MirrorU MirrorOnceV";
	m_szCommandKey = "MirrorMirrorOnce";

    // Init framework options
	m_Options.D3DOptions.fMinDXVersion = 8.0f;

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_MIRROR;
	dwAddressCapV = D3DPTADDRESSCAPS_MIRRORONCE;

	// Give parent a printable name for the caps
	szAddrU = "MirrorU";
	szAddrV = "MirrorOnceV";
}

bool CMirrorMirrorOnceTest::Setup(void)
{
	// Turn on MirrorU MirrorOnceV addressing
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_MIRRORONCE);

	return CAddressTest::Setup();
}

//************************************************************************
// ClampMirrorOnce Test functions

CClampMirrorOnceTest::CClampMirrorOnceTest()
{
	m_szTestName = "Texture Address ClampU MirrorOnceV";
	m_szCommandKey = "ClampMirrorOnce";

    // Init framework options
	m_Options.D3DOptions.fMinDXVersion = 8.0f;

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_CLAMP;
	dwAddressCapV = D3DPTADDRESSCAPS_MIRRORONCE;

	// Give parent a printable name for the caps
	szAddrU = "ClampU";
	szAddrV = "MirrorOnceV";
}

bool CClampMirrorOnceTest::Setup(void)
{
	// Turn on ClampU MirrorOnceV addressing
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_MIRRORONCE);

	return CAddressTest::Setup();
}

//************************************************************************
// BorderMirrorOnce Test functions

CBorderMirrorOnceTest::CBorderMirrorOnceTest()
{
	m_szTestName = "Texture Address BorderU MirrorOnceV";
	m_szCommandKey = "BorderMirrorOnce";

    // Init framework options
	m_Options.D3DOptions.fMinDXVersion = 8.0f;

	// Tell parent to use border colors and check indpendent address modes
	bBorder = true;
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_BORDER;
	dwAddressCapV = D3DPTADDRESSCAPS_MIRRORONCE;

	// Give parent a printable name for the caps
	szAddrU = "BorderU";
	szAddrV = "MirrorOnceV";
}

bool CBorderMirrorOnceTest::Setup(void)
{
	// Turn on BorderU MirrorOnceV addressing
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_BORDER);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_MIRRORONCE);

	return CAddressTest::Setup();
}

//************************************************************************
// MirrorOnceWrap Test functions

CMirrorOnceWrapTest::CMirrorOnceWrapTest()
{
	m_szTestName = "Texture Address MirrorOnceU WrapV";
	m_szCommandKey = "MirrorOnceWrap";

    // Init framework options
	m_Options.D3DOptions.fMinDXVersion = 8.0f;

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_MIRRORONCE;
	dwAddressCapV = D3DPTADDRESSCAPS_WRAP;

	// Give parent a printable name for the caps
	szAddrU = "MirrorOnceU";
	szAddrV = "WrapV";
}

bool CMirrorOnceWrapTest::Setup(void)
{
	// Turn on MirrorOnceU WrapV addressing
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_MIRRORONCE);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_WRAP);

	return CAddressTest::Setup();
}

//************************************************************************
// MirrorOnceMirror Test functions

CMirrorOnceMirrorTest::CMirrorOnceMirrorTest()
{
	m_szTestName = "Texture Address MirrorOnceU MirrorV";
	m_szCommandKey = "MirrorOnceMirror";

    // Init framework options
	m_Options.D3DOptions.fMinDXVersion = 8.0f;

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_MIRRORONCE;
	dwAddressCapV = D3DPTADDRESSCAPS_MIRROR;

	// Give parent a printable name for the caps
	szAddrU = "MirrorOnceU";
	szAddrV = "MirrorV";
}

bool CMirrorOnceMirrorTest::Setup(void)
{
	// Turn on MirrorOnceU MirrorV addressing
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_MIRRORONCE);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_MIRROR);

	return CAddressTest::Setup();
}

//************************************************************************
// MirrorOnceClamp Test functions

CMirrorOnceClampTest::CMirrorOnceClampTest()
{
	m_szTestName = "Texture Address MirrorOnceU ClampV";
	m_szCommandKey = "MirrorOnceClamp";

    // Init framework options
	m_Options.D3DOptions.fMinDXVersion = 8.0f;

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_MIRRORONCE;
	dwAddressCapV = D3DPTADDRESSCAPS_CLAMP;

	// Give parent a printable name for the caps
	szAddrU = "MirrorOnceU";
	szAddrV = "ClampV";
}

bool CMirrorOnceClampTest::Setup(void)
{
	// Turn on MirrorOnceU ClampV addressing
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_MIRRORONCE);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_CLAMP);

	return CAddressTest::Setup();
}

//************************************************************************
// MirrorOnceBorder Test functions

CMirrorOnceBorderTest::CMirrorOnceBorderTest()
{
	m_szTestName = "Texture Address MirrorOnceU BorderV";
	m_szCommandKey = "MirrorOnceBorder";

    // Init framework options
	m_Options.D3DOptions.fMinDXVersion = 8.0f;

	// Tell parent to use border colors and check indpendent address modes
	bBorder = true;
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_MIRRORONCE;
	dwAddressCapV = D3DPTADDRESSCAPS_BORDER;

	// Give parent a printable name for the caps
	szAddrU = "MirrorOnceU";
	szAddrV = "BorderV";
}

bool CMirrorOnceBorderTest::Setup(void)
{
	// Turn on MirrorOnceU BorderV addressing
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_MIRRORONCE);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_BORDER);

	return CAddressTest::Setup();
}
