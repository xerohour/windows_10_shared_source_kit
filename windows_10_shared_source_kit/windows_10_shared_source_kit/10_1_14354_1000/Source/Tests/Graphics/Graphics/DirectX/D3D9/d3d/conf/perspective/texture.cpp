#define D3D_OVERLOADS
#include <d3d.h>
#include "Perspective.h"

//************************************************************************
// Perspective Texture Test functions

CTextureTest::CTextureTest()
{
	m_szTestName = "Perspective Texture";
	m_szCommandKey = "Texture";
}

CTextureTest::~CTextureTest()
{
}

UINT CTextureTest::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

#if FAST_TEST
    m_totalNumTestCases = (MAX_GRID_TESTS * 4) + MAX_TRI_TESTS;
    // Setup the Test range
    SetTestRange(1, 1);
#else
    // Setup the Test range
	SetTestRange(1, (MAX_GRID_TESTS * 4) + MAX_TRI_TESTS);
#endif

	// Check the caps we are interested in
	DWORD dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;

	// Check the appropriate Texture cap
	if (!(dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE))
	{
		WriteToLog("Device capability not supported: Texture Perspective.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Setup the colors for the Stripes array
	bool bResult;
	D3DCOLOR Stripes[8];

	Stripes[0] = RGBA_MAKE(255,255,255,255);// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);	// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);	// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);// Grey

    // Create the texture
    bResult = CreateTexture(64,64,1,0,m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat],FMWK_DEFAULT,&pTexture);

	// Indicate a CreateTexture failure
	if (!bResult || (NULL == pTexture))
	{
		WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

	// Set the palette if needed
	if (pTexture->IsPalettized())
		pTexture->SetPalette(NULL);

    // Load a stripe image into the texture
    if (!pTexture->LoadStripes(1,64,64,8,Stripes,false))
	{
		WriteToLog("LoadStripes() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

    // Set the valid texture
	if (!SetTexture(0,pTexture))
	{
		WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

	return D3DTESTINIT_RUN;
}
