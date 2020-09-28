#define D3D_OVERLOADS
#include <d3d.h>
#include "TexFormats.h"

//************************************************************************
// Verify Test functions

CVerifyTest::CVerifyTest()
{
	m_szTestName = "Verify Texture Formats";
	m_szCommandKey = "Verify";

    // Init Framework options
	m_Options.D3DOptions.fMaxDXVersion = 7.0f;
	m_Options.ModeOptions.dwTexInclude = 0;
    pFormat = NULL;
}

UINT CVerifyTest::TestInitialize(void)
{
	int nExtraFormats = m_pSrcDevice->GetNumTexFormats() - m_uCommonTextureFormats;

    if (nExtraFormats)
        WriteToLog("Found %d unsupported formats!!\n", nExtraFormats);

	// Setup the Test range 
	SetTestRange(1, m_pSrcDevice->GetNumTexFormats());

    // Get pointer to first format
    pFormat = m_pSrcDevice->GetTexFormats();

	return D3DTESTINIT_RUN;
}

bool CVerifyTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
    TCHAR szBuffer[80];
	errString[0] = '\0';
	msgString[0] = '\0';

	// Perform some texture format validation
	bValidFormat = VerifyFormat(&pFormat->ddpfPixelFormat);

    // Tell log what format we are using
    FmtToString(FORMAT_TEXTURE,pFormat,szBuffer);
    sprintf(msgString, "%sTexture Format: %s",msgString,szBuffer);

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    // Advance to next format
    pFormat = pFormat->pNext;

    return true;
}

void CVerifyTest::SceneRefresh(void)
{
}

bool CVerifyTest::ProcessFrame(void)
{
	// Tell the logfile how they did
	if (bValidFormat)
	{
		m_fPassPercentage = 1.0;
		Pass();
	}
	else
	{
		Fail();

		// Print any errors to the log
		if (errString[0] != '\0')
			WriteToLog(errString);
	}
    
    return bValidFormat;
}

bool CVerifyTest::VerifyFormat(const PIXELFORMAT *pDDPF)
{
    bool bError           = false;
    DWORD dwFlags         = pDDPF->dwFlags;
    DWORD dwFourCC        = pDDPF->dwFourCC;
    DWORD dwTotalBitCount = pDDPF->dwRGBBitCount;
    DWORD dwAlphaBitCount = 0;
    DWORD dwRedBitCount   = 0;
    DWORD dwGreenBitCount = 0;
    DWORD dwBlueBitCount  = 0;
    DWORD mask;

	// Remove any private pixel format flags that were added.  We are also including
	// the PF_YUV flag as the YUV fourcc textures don't use it. 
	dwFlags &= ~(PF_PRIVATE | PF_YUV);

    // Count number of bits for each color component
    for (mask = pDDPF->dwRGBAlphaBitMask; mask; mask>>=1)
        dwAlphaBitCount += (mask & 0x1);
    for (mask = pDDPF->dwRBitMask; mask; mask>>=1)
        dwRedBitCount += (mask & 0x1);
    for (mask = pDDPF->dwGBitMask; mask; mask>>=1)
        dwGreenBitCount += (mask & 0x1);
    for (mask = pDDPF->dwBBitMask; mask; mask>>=1)
        dwBlueBitCount += (mask & 0x1);

    // Check for invalid formats
    if ((dwFlags & PF_ALPHAPIXELS) && (dwAlphaBitCount == 0))
	{
		sprintf(errString,"%sError: DDPF_ALPHAPIXELS flag set with zero alpha mask!\n",errString);
		bError = true;
	}
    if (!(dwFlags & PF_ALPHAPIXELS) && !(dwFlags & PF_ALPHA) && (dwAlphaBitCount != 0))
	{
		sprintf(errString,"%sError: Alpha bit mask set without DDPF_ALPHAPIXELS or DDPF_ALPHA flag set!\n",errString);
		bError = true;
	}
    if (!(dwFlags & PF_FOURCC) && (dwTotalBitCount == 0))
	{
		sprintf(errString,"%sError: Format has zero bit count!\n",errString);
		bError = true;
	}

    if (dwFlags & PF_ALPHA)
    {
        // Alpha-only formats
        if ((dwFlags & PF_ALPHA) && (dwFlags != PF_ALPHA))
		{
			sprintf(errString,"%sError: Other flags illegally combined with DDPF_ALPHA!\n",errString);
			bError = true;
		}
		if (!((pDDPF->dwAlphaBitDepth == 1) || (pDDPF->dwAlphaBitDepth == 2) ||
			(pDDPF->dwAlphaBitDepth == 4) || (pDDPF->dwAlphaBitDepth == 8)))
		{
			sprintf(errString,"%sError: DDPF_ALPHA flag set with dwAlphaBitDepth != 1,2,4, or 8!\n",errString);
			bError = true;
		}
    }
    else if (dwFlags & PF_BUMPLUMINANCE)
    {
		// BumpLuminance formats.  Check for invalid X8L8V8U8 format.
		if ((dwBlueBitCount == 8) && (dwGreenBitCount == 8) && (dwRedBitCount == 8))
		{
			if (pDDPF->dwBumpBitCount != 32)
			{
				sprintf(errString,"%sError: dwBumpBitCount is %d, should be 32!\n",errString,pDDPF->dwBumpBitCount);
				bError = true;
			}
		}
    }
    else if (dwFlags & PF_BUMPDUDV)
    {
		// BumpDuDv specific checks go here
    }
    else if (dwFlags & PF_LUMINANCE)
    {
		// Luminance specific checks go here
    }
    else if (dwFlags & PF_FOURCC)
    {
        // FourCC formats. Check for a valid format.
        if ((dwTotalBitCount != 0) || (dwRedBitCount != 0) || (dwGreenBitCount != 0) ||
             (dwBlueBitCount != 0) || (dwAlphaBitCount != 0))
		{
			sprintf(errString,"%sError: FourCC code has non-zero bitcount!\n",errString);
			bError = true;
		}
        if ((dwFlags & PF_FOURCC) && (dwFlags != PF_FOURCC))
		{
			sprintf(errString,"%sError: Other flags illegally combined with DDPF_FOURCC!\n",errString);
			bError = true;
		}
    }
    else if (dwFlags & (PF_PALETTEINDEXED1|PF_PALETTEINDEXED2|
                        PF_PALETTEINDEXED4|PF_PALETTEINDEXED8))
    {
		// Palette specific checks go here
    }
    else if (dwFlags & PF_RGB)
    {
		// RGB specific checks go here
    }
    else
    {
		// Unknown format, assume error
		sprintf(errString,"%sError: Unknown format flags 0x%x\n",errString,dwFlags);
		bError = true;
	}

	return (!bError);
}

