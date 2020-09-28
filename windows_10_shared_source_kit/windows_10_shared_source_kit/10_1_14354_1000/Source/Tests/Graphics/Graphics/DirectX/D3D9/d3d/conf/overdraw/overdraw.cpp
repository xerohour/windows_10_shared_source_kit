#define D3D_OVERLOADS
#include <d3d.h>
#include "OverDraw.h"

// Global Test and App instance
CD3DWindowFramework	App;
CAlpha_OverdrawTest	AlphaOverdraw;

COverdrawTest::COverdrawTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);

	// Force test to cycle desktop resolutions 16bpp/32bpp
	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ALL;

	// Force test to use relevant DirectX versions
	m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_8_OR_81 | DXVERSION_7 | DXVERSION_6;

    // Only initialize the source
    m_Options.D3DOptions.bReference = false;

    // Disable ZBuffers
    m_Options.D3DOptions.bZBuffer = false;
}

COverdrawTest::~COverdrawTest()
{
}

bool COverdrawTest::SetDefaultRenderStates(void)
{
	// Set the alpha blending renderstates for alpha method
	SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
	return true;
}

UINT COverdrawTest::TestInitialize(void)
{
	// Setup the Test range
	SetTestRange(1, MAX_TESTS);

    if (m_dwVersion <= 0x0700)
    {
	    // Check the caps we are interested in
	    DWORD dwRasterCaps = m_pSrcDevice->GetCaps()->dwRasterCaps;

	    // Check the SubPixel caps
	    if (!(dwRasterCaps & D3DPRASTERCAPS_SUBPIXEL) &&
		    !(dwRasterCaps & D3DPRASTERCAPS_SUBPIXELX))
	    {
		    WriteToLog("Device capability not supported: Raster Subpixel.\n");
		    return D3DTESTINIT_ABORT;
	    }
    }

    // Get the current render target
    if (FAILED(m_pSrcDevice->GetRenderTarget(&pSrcTarget)))
    {
		WriteToLog("GetRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		RELEASE(pSrcTarget);
    	return D3DTESTINIT_ABORT;
    }

    return D3DTESTINIT_RUN;
}

bool COverdrawTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,0,255), 1.0f, 0);
}

bool COverdrawTest::ExecuteTest(UINT uTestNum)
{
	// Generate a unique random number for the grid offset
	srand(((MAX_TESTS + 1) - uTestNum) * 9);
	nOffset = rand() % (MAX_TESTS/2);
	float fStart = (float)nOffset;
    int nXDivisions, nYDivisions;

	// Initialize some variables
	bool bResult = true;
	msgString[0] = '\0';

	// Start the test case message
	sprintf(msgString, "%sStarting Pos: ",msgString);

    // Setup the X & Y divisions based on test number
    if (uTestNum <= 10)
	{
        nXDivisions = 1;
        nYDivisions = 1;
	}
	else if (uTestNum <= 15)
	{
        nXDivisions = 5;
        nYDivisions = uTestNum - 10;
	}
	else if (uTestNum <= 20)
	{
        nXDivisions = uTestNum - 15;
        nYDivisions = 5;
	}
	else 
	{
        nXDivisions = uTestNum - 10;
        nYDivisions = uTestNum - 10;
	}

	// Start rendering the primitives.
	bResult = AlphaGrid(
						uTestNum,		// Test case 
						fStart, fStart,	// Start X, Start Y
						nXDivisions,	// X divisions 
						nYDivisions		// Y divisions
					   );			

    sprintf(msgString, "%s(%2d, %2d), ",msgString,nOffset,nOffset);
	sprintf(msgString, "%sWidth Div: %2d, ",msgString,nXDivisions);
	sprintf(msgString, "%sHeight Div: %2d",msgString,nYDivisions);

	// Check the result from AlphaGrid()
	if (bResult)
	{
		// Tell the log that we are starting
		BeginTestCase(msgString,uTestNum);
	}

    return bResult;
}

bool COverdrawTest::ProcessFrame(void)
{
    HRESULT     hr;
	RECT	    rSurf;
	void	    *pColor;
    LOCKED_RECT LockRect;

	// Build the subrect we want
	rSurf.top = nOffset;
	rSurf.left = nOffset;
	rSurf.bottom = rSurf.top + 256;
	rSurf.right = rSurf.left + 256;

	// Get the Surface data pointer for our subrect
    hr = pSrcTarget->LockRect(&LockRect,&rSurf,NULL);

    if (FAILED(hr))
	{
		WriteToLog("ProcessFrame: LockRect() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
        Fail();
        return false;
	}

	// Initialize variables
	int nRedPixels = 0;
	int nGreenPixels = 0;
	int nBlackPixels = 0;
	int nYellowPixels = 0;

	// Look for any Yellow or Black pixels
	for (int j=0; j < 256; j++)		// Height loop
	{
		pColor = (char*)LockRect.pBits + (LockRect.iPitch * j);

		for (int i=0; i < 256; i++)	// Width loop
		{
			DWORD dwColor = pSrcTarget->ReadColor(&pColor);

			DWORD red   = RGBA_GETRED(dwColor);
			DWORD green = RGBA_GETGREEN(dwColor);
			DWORD blue  = RGBA_GETBLUE(dwColor);

            // Count the Underdraw & Overdraw pixels in the surface
            if (!(red || green || blue) || (red && green) || (red > 150) || (green > 150))
            {
    			if (!(red || green || blue))
                {
	    			nBlackPixels++;
                    dwColor = pSrcTarget->ColorConvert(RGBA_MAKE(255,255,255,255));
                }

                if (red && green)
                {
				    nYellowPixels++;
                    dwColor = pSrcTarget->ColorConvert(RGBA_MAKE(255,255,0,255));
                }

                if (red > 150)
                {
                    nRedPixels++;
                    dwColor = pSrcTarget->ColorConvert(RGBA_MAKE(255,0,0,255));
                }

                if (green > 150)
                {
                    nGreenPixels++;
                    dwColor = pSrcTarget->ColorConvert(RGBA_MAKE(0,255,0,255));
                }

                int nBytes = pSrcTarget->m_Desc.Format.ddpfPixelFormat.dwRGBBitCount / 8;
                BYTE *pHighlight = (PBYTE)&dwColor;
                BYTE *pWrite = (PBYTE)pColor;
                pHighlight += nBytes;
        
                for (int k=0; k < nBytes; k++)
                {
                    pWrite--;
                    pHighlight--;
                    *pWrite = *pHighlight;
                }
            }
		}
	}

    hr = pSrcTarget->UnlockRect();

    if (FAILED(hr))
	{
		WriteToLog("ProcessFrame: UnlockRect() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
        Fail();
        return false;
	}

	// Tell the logfile how it went
	if ((nBlackPixels == 0) && (nYellowPixels == 0) && (nRedPixels == 0) && (nGreenPixels == 0))
	{
		m_fPassPercentage = 1.0;
		Pass();
        return true;
	}
	else 
	{
        // Highlight the failures 
        DisplayFrame();

        if (nBlackPixels)
		{
			WriteToLog("UnderDraw Pixels detected!\n");
			WriteToLog("Found %d Black pixels (highlighted White).\n", nBlackPixels);
		}

		if (nYellowPixels)
		{
			WriteToLog("OverDraw Pixels detected!\n");
			WriteToLog("Found %d Yellow pixels.\n", nYellowPixels);
		}

		if (nRedPixels)
		{
			WriteToLog("OverDraw Pixels detected!\n");
			WriteToLog("Found %d overbright Red pixels.\n", nRedPixels);
		}

		if (nGreenPixels)
		{
			WriteToLog("OverDraw Pixels detected!\n");
			WriteToLog("Found %d overbright Green pixels.\n", nGreenPixels);
		}

		m_fPassPercentage = 1.0f - ((nBlackPixels + nYellowPixels + nGreenPixels + nRedPixels) / 65536.0f);
		Fail();
        return false;
	}
}

bool COverdrawTest::TestTerminate(void)
{
	// Cleanup buffers that we created
	RELEASE(pSrcTarget);

    return true;
}

