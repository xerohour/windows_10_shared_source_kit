#define D3D_OVERLOADS
#include <d3d8.h>
#include "types.h"
#include "Perspective.h"

//************************************************************************
// Perspective Color Test functions

CColorTest::CColorTest()
{
	m_szTestName = "Perspective Color";
	m_szCommandKey = "Color";

    // Init Framework options
    m_Options.D3DOptions.fMinDXVersion = 8.0f;
}

CColorTest::~CColorTest()
{
}

UINT CColorTest::TestInitialize(void)
{
#if FAST_TEST
    m_totalNumTestCases = MAX_GRID_TESTS + MAX_TRI_TESTS;
    // Setup the Test range
    SetTestRange(1, 1);
#else
	// Setup the Test range
	SetTestRange(1, MAX_GRID_TESTS + MAX_TRI_TESTS);
#endif

	// Make sure we are on at lease DX8
	if (m_dwVersion <= 0x0700)
	{
		WriteToLog("This test requires at least Dx8.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Check the caps we are interested in
	DWORD dwRasterCaps = m_pSrcDevice->GetCaps()->dwRasterCaps;

	// Check the appropriate Raster cap
	if (!(dwRasterCaps & D3DPRASTERCAPS_COLORPERSPECTIVE))
	{
		WriteToLog("Device capability not supported: Raster ColorPerspective.\n");
		return D3DTESTINIT_SKIPALL;
    }

	return D3DTESTINIT_RUN;
}
