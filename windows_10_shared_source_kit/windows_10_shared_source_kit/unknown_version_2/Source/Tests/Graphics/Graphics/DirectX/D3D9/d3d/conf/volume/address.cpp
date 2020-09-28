#include <d3d8.h>
#include "Volume.h"

//************************************************************************
// VolAddressWrap Test functions

CVolAddressWrapTest::CVolAddressWrapTest()
{
	m_szTestName = "Volume Address Wrap";
	m_szCommandKey = "Wrap";

    dwAddressMode = D3DTADDRESS_WRAP;

    // We only want one volume format
    if (m_Options.ModeOptions.VolumeFmt == FMT_ALL)
        m_Options.ModeOptions.VolumeFmt = FMT_ANY;
}

//************************************************************************
// VolAddressMirror Test functions

CVolAddressMirrorTest::CVolAddressMirrorTest()
{
	m_szTestName = "Volume Address Mirror";
	m_szCommandKey = "Mirror";

    dwAddressMode = D3DTADDRESS_MIRROR;

    // We only want one volume format
    if (m_Options.ModeOptions.VolumeFmt == FMT_ALL)
        m_Options.ModeOptions.VolumeFmt = FMT_ANY;
}

//************************************************************************
// VolAddressClamp Test functions
CVolAddressClampTest::CVolAddressClampTest()
{
	m_szTestName = "Volume Address Clamp";
	m_szCommandKey = "Clamp";

    dwAddressMode = D3DTADDRESS_CLAMP;

    // We only want one volume format
    if (m_Options.ModeOptions.VolumeFmt == FMT_ALL)
        m_Options.ModeOptions.VolumeFmt = FMT_ANY;
};

//************************************************************************
// VolAddressBorder Test functions
CVolAddressBorderTest::CVolAddressBorderTest()
{
	m_szTestName = "Volume Address Border";
	m_szCommandKey = "Border";

    dwAddressMode = D3DTADDRESS_BORDER;

    // We only want one volume format
    if (m_Options.ModeOptions.VolumeFmt == FMT_ALL)
        m_Options.ModeOptions.VolumeFmt = FMT_ANY;
};

//************************************************************************
// VolAddressMirrorOnce Test functions
CVolAddressMirrorOnceTest::CVolAddressMirrorOnceTest()
{
	m_szTestName = "Volume Address MirrorOnce";
	m_szCommandKey = "MirrorOnce";

    dwAddressMode = D3DTADDRESS_MIRRORONCE;

    // We only want one volume format
    if (m_Options.ModeOptions.VolumeFmt == FMT_ALL)
        m_Options.ModeOptions.VolumeFmt = FMT_ANY;
};

UINT CVolAddressTest::TestInitialize(void)
{
    // Set test range 
    SetTestRange(1, MAX_ADDRESS_TESTS);

    // Make sure we are on at lease DX8
	if (m_dwVersion <= 0x0700)
	{
		WriteToLog("This test requires at least Dx8.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Check if Volumes are available	
	DWORD dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;
    DWORD dwVolumeTextureAddressCaps = m_pSrcDevice->GetCaps()->dwVolumeTextureAddressCaps;

	if (!(dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP))
	{
		if (m_uCommonVolumeFormats != 0) 
		{
  			WriteToLog("Device capability inconsistency: Found volume texture formats with Texture VolumeMap not set.");
			return D3DTESTINIT_ABORT;
		}

		WriteToLog("Device capability not supported: Texture VolumeMap.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Do VolumeTextureAddress cap consistency checking
    if (!dwVolumeTextureAddressCaps)
    {
	    WriteToLog("Device capability inconsistency: No VolumeTextureAddress caps are set.\n");
	    return D3DTESTINIT_ABORT;
    }

    // Check if Address mode is supported
    switch (dwAddressMode)
    {
        case D3DTADDRESS_WRAP:
            
                if (!(dwVolumeTextureAddressCaps & D3DPTADDRESSCAPS_WRAP))
                {
		            WriteToLog("Device capability not supported: VolumeTextureAddress Wrap.\n");
		            return D3DTESTINIT_SKIPALL;
                }
                break;

        case D3DTADDRESS_MIRROR: 

                if (!(dwVolumeTextureAddressCaps & D3DPTADDRESSCAPS_MIRROR))
                {
		            WriteToLog("Device capability not supported: VolumeTextureAddress Mirror.\n");
		            return D3DTESTINIT_SKIPALL;
                }
                break;

        case D3DTADDRESS_CLAMP: 

                if (!(dwVolumeTextureAddressCaps & D3DPTADDRESSCAPS_CLAMP))
                {
		            WriteToLog("Device capability not supported: VolumeTextureAddress Clamp.\n");
		            return D3DTESTINIT_SKIPALL;
                }

                // Reset test range for Clamp
                SetTestRange(65, MAX_ADDRESS_TESTS);
        		WriteToLog("Skipping non-relevant cases (1 to 64) for Clamp addressing.\n");
                break;

        case D3DTADDRESS_BORDER: 

                if (!(dwVolumeTextureAddressCaps & D3DPTADDRESSCAPS_BORDER))
                {
		            WriteToLog("Device capability not supported: VolumeTextureAddress Border.\n");
		            return D3DTESTINIT_SKIPALL;
                }

                // Reset test range for Border
                SetTestRange(65, MAX_ADDRESS_TESTS);
        		WriteToLog("Skipping non-relevant cases (1 to 64) for Border addressing.\n");
                break;

        case D3DTADDRESS_MIRRORONCE: 

                if (!(dwVolumeTextureAddressCaps & D3DPTADDRESSCAPS_MIRRORONCE))
                {
		            WriteToLog("Device capability not supported: VolumeTextureAddress MirrorOnce.\n");
		            return D3DTESTINIT_SKIPALL;
                }
                break;

        default: WriteToLog("Error: Unknown Address Mode: %d\n",dwAddressMode);
          		 return D3DTESTINIT_ABORT;
    }

    // Check if any volume formats were available
	if (m_uCommonVolumeFormats == 0) 
    {
		WriteToLog("No volume texture formats found.\n");
		return D3DTESTINIT_ABORT;
    }

	// Reset some image compare variables to defaults
    m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.15f);
    m_pFramework->GetImageCmp()->SetTolPerPixel(0.10f);
    m_pFramework->GetImageCmp()->SetImgDeltaWght(1.00f);

    // Setup the colors for the Stripes array
	DWORD   dwColors[2][4];

	dwColors[0][0] = 0xffffffff;    // White
	dwColors[0][1] = 0xffff0000;	// Red
	dwColors[0][2] = 0xff00ff00;    // Green
	dwColors[0][3] = 0xff0000ff;	// Blue

	dwColors[1][0] = 0xffffff00;    // Yellow
	dwColors[1][1] = 0xff00ffff;	// Cyan
	dwColors[1][2] = 0xffff00ff;    // Magenta
	dwColors[1][3] = 0xff808080;	// Grey

    // Set the Image depth, width, and height
    int i,j;
    UINT nImageDepth = 2;
    UINT nImageWidth = 64;
    UINT nImageHeight = 64;
    TCHAR szBuffer[80];
	bool bResult = true;
	szStatus[0] = '\0';

	// Log the current Volume format and load method.
    FmtToString(FORMAT_TEXTURE,&m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],szBuffer);
    WriteToLog("Tex: %dx%dx%d, %s (UpdateTex).\n",nImageWidth,nImageHeight,nImageDepth,szBuffer);
   	sprintf(szStatus, "%s$aUpdateTex",szStatus);

    // Create the gradient image data
    CImage ** ppImage = new CImage*[nImageDepth];
	if(ppImage == NULL)
	{
    	WriteToLog("TestInitialize() - Out of memory.\n");
		return D3DTESTINIT_ABORT;
	}

    for (i=0; i<nImageDepth; i++)
    {
        ppImage[i] = new CImage;
		if(ppImage[i] == NULL)
		{
			// Cleanup image data
			for (j=0; j<i; j++)
			{
			   SAFEDELETE(ppImage[j]); 
			}
			SAFEDELETEA(ppImage);

			WriteToLog("TestInitialize() - Out of memory.\n");
			return D3DTESTINIT_ABORT;
		}

        ppImage[i]->LoadGradient(nImageWidth,nImageHeight,dwColors[i]); 
    }

	// Create the volume texture
	bResult = CreateVolumeTexture(nImageWidth,nImageHeight,nImageDepth,1,0,m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],POOL_DEFAULT,&pVolTexture);

	// Tell log about CreateVolumeTexture failure
    if (!bResult || NULL == pVolTexture) 
	{
    	WriteToLog("CreateVolumeTexture(pVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

	// Set the palette
	if (pVolTexture->IsPalettized())
        pVolTexture->SetPalette(NULL);

	// Load the images
	if (!pVolTexture->LoadImage(1, ppImage, TEXLOAD_UPDATETEXTURE))
	{
		WriteToLog("LoadImage(pVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

    // Set the valid texture
	if (!SetTexture(0,pVolTexture))
	{
		WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

    // Cleanup image data
    for (i=0; i<nImageDepth; i++)
    {
       SAFEDELETE(ppImage[i]); 
    }
	SAFEDELETEA(ppImage);

    return D3DTESTINIT_RUN;
}

bool CVolAddressTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
    int nU, nV, nW;
	int nTest = uTestNum - 1;
	msgString[0] = '\0';

    // Setup U,V,W texture coordinates
    nV = nTest % 4;
    nU = (nTest / 4) % 4;
    nW = (nTest / 16) % 4;
    
    // Use last four test cases for (-n to n) texture coordinates
    if (nTest >= (MAX_ADDRESS_TESTS - 4))
    {
        int nNeg = nTest - (MAX_ADDRESS_TESTS - 5);

        // Tell the log our texture coordinates
        sprintf(msgString,"%sCoord (u,v,w): (-%d,-%d,-%d)(%d,-%d,%d)(-%d,%d,-%d)(%d,%d,%d)",msgString,nNeg,nNeg,nNeg,nNeg,nNeg,nNeg,nNeg,nNeg,nNeg,nNeg,nNeg,nNeg);

        // Load (-n to n) grid vertices
	    LoadGrid(-1.0f * nNeg,-1.0f * nNeg,-1.0f * nNeg,1.0f * nNeg,1.0f * nNeg,1.0f * nNeg);
    }
    else
    {
        // Use first half of test cases for (-n to 0) texture coordinates
        if (nTest < (MAX_ADDRESS_TESTS - 4) / 2)
        {
            nV = (nV < 0) ? 0 : -1*(nV+1);
            nU = (nU < 0) ? 0 : -1*(nU+1);
            nW = (nW < 0) ? 0 : -1*(nW+1);

            // Tell the log our texture coordinates
            sprintf(msgString,"%sCoord (u,v,w): (%d,%d,%d)(0,%d,0)(%d,0,%d)(0,0,0)",msgString,nU,nV,nW,nV,nU,nW);

            // Load (-n to 0) grid vertices
	        LoadGrid(1.0f * nU,1.0f * nV,1.0f * nW,0.0f,0.0f,0.0f);
        }
        // Use second half of test cases for (0 to n) texture coordinates
        else
        {
            nV = (nV < 0) ? 0 : nV+1;
            nU = (nU < 0) ? 0 : nU+1;
            nW = (nW < 0) ? 0 : nW+1;

            // Tell the log our texture coordinates
            sprintf(msgString,"%sCoord (u,v,w): (0,0,0)(%d,0,%d)(0,%d,0)(%d,%d,%d)",msgString,nU,nW,nV,nU,nV,nW);

            // Load (0 to n) grid vertices
	        LoadGrid(0.0f,0.0f,0.0f,1.0f * nU,1.0f * nV,1.0f *nW);
        }
    }

    // Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    // Turn on addressing mode
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)dwAddressMode);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)dwAddressMode);
	SetTextureStageState(0, D3DTSS_ADDRESSW, (DWORD)dwAddressMode);

    return true;
}
