#define D3D_OVERLOADS
#include <d3d.h>
#include "Anisotropic.h"

//************************************************************************
// PointAniso Filter Test functions

CPointAnisoTest::CPointAnisoTest()
{
	m_szTestName = "Anisotropic Filter Point(Min) Aniso(Mag)";
	m_szCommandKey = "PointAniso";

	// Tell the parent which caps to look for
    dwMinFilterCap = D3DPTFILTERCAPS_MINFPOINT;
    dwMagFilterCap = D3DPTFILTERCAPS_MAGFANISOTROPIC;

	// Tell parent class what render states to set
	dwMinFilter = TEXF_POINT;
	dwMagFilter = TEXF_ANISOTROPIC;
}

//************************************************************************
// LinearAniso Filter Test functions

CLinearAnisoTest::CLinearAnisoTest()
{
	m_szTestName = "Anisotropic Filter Linear(Min) Aniso(Mag)";
	m_szCommandKey = "LinearAniso";

	// Tell the parent which caps to look for
    dwMinFilterCap = D3DPTFILTERCAPS_MINFLINEAR;
	dwMagFilterCap = D3DPTFILTERCAPS_MAGFANISOTROPIC;

	// Tell parent class what render states to set
	dwMinFilter = TEXF_LINEAR;
	dwMagFilter = TEXF_ANISOTROPIC;
}

//************************************************************************
// AnisoPoint Filter Test functions

CAnisoPointTest::CAnisoPointTest()
{
	m_szTestName = "Anisotropic Filter Aniso(Min) Point(Mag)";
	m_szCommandKey = "AnisoPoint";

	// Tell the parent which caps to look for
    dwMinFilterCap = D3DPTFILTERCAPS_MINFANISOTROPIC;
	dwMagFilterCap = D3DPTFILTERCAPS_MAGFPOINT;

	// Tell parent class what render states to set
	dwMinFilter = TEXF_ANISOTROPIC;
	dwMagFilter = TEXF_POINT;
}

//************************************************************************
// AnisoLinear Filter Test functions

CAnisoLinearTest::CAnisoLinearTest()
{
	m_szTestName = "Anisotropic Filter Aniso(Min) Linear(Mag)";
	m_szCommandKey = "AnisoLinear";

	// Tell the parent which caps to look for
    dwMinFilterCap = D3DPTFILTERCAPS_MINFANISOTROPIC;
	dwMagFilterCap = D3DPTFILTERCAPS_MAGFLINEAR;

	// Tell parent class what render states to set
	dwMinFilter = TEXF_ANISOTROPIC;
	dwMagFilter = TEXF_LINEAR;
}

//************************************************************************
// AnisoAniso Filter Test functions

CAnisoAnisoTest::CAnisoAnisoTest()
{
	m_szTestName = "Anisotropic Filter Aniso(Min) Aniso(Mag)";
	m_szCommandKey = "AnisoAniso";

	// Tell the parent which caps to look for
    dwMinFilterCap = D3DPTFILTERCAPS_MINFANISOTROPIC;
	dwMagFilterCap = D3DPTFILTERCAPS_MAGFANISOTROPIC;

	// Tell parent class what render states to set
	dwMinFilter = TEXF_ANISOTROPIC;
	dwMagFilter = TEXF_ANISOTROPIC;
}

