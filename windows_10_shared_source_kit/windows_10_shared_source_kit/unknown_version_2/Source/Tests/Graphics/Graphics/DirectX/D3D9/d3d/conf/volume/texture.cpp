#include <d3d8.h>
#include "Volume.h"

//************************************************************************
// VolUpdateTex Test functions

CVolUpdateTexTest::CVolUpdateTexTest()
{
	m_szTestName = "Volume Texture (UpdateTex)";
	m_szCommandKey = "UpdateTex";

    dwLoadOption = TEXLOAD_UPDATETEXTURE;
}

//************************************************************************
// VolLockCopy Test functions

CVolLockCopyTest::CVolLockCopyTest()
{
	m_szTestName = "Volume Texture (LockCopy)";
	m_szCommandKey = "LockCopy";

    dwLoadOption = TEXLOAD_LOCKCOPY;
}

UINT CVolTexTest::TestInitialize(void)
{
	// Setup the Test range 
    SetTestRange(1, MAX_TEXTURE_TESTS);

 	// Make sure we are on at lease DX8
	if (m_dwVersion <= 0x0700)
	{
		WriteToLog("This test requires at least Dx8.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Check if Volumes are available	
	DWORD dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;

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
 
    // Check the dynamic textures cap
	if (dwLoadOption == TEXLOAD_LOCKCOPY)
	{
		if (!(m_pSrcDevice->GetCaps()->dwCaps2 & D3DCAPS2_DYNAMICTEXTURES))
		{
			WriteToLog("Dynamic textures not supported.\n");
			return D3DTESTINIT_SKIPALL;
		}
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
	D3DCOLOR    Stripes[40];

	Stripes[0]  = 0xffffffff;   // White
	Stripes[1]  = 0xffffffff;   // White
	Stripes[2]  = 0xff707070;	// Grey
	Stripes[3]  = 0xffffffff;   // White
	Stripes[4]  = 0xffffffff;   // White

	Stripes[5]  = 0xffff0000;	// Red
	Stripes[6]  = 0xffff0000;	// Red
    Stripes[7]  = 0xff707070;	// Grey
	Stripes[8]  = 0xffff0000;	// Red
	Stripes[9]  = 0xffff0000;	// Red

    Stripes[10] = 0xff00ff00;	// Green
    Stripes[11] = 0xff00ff00;	// Green
	Stripes[12] = 0xff707070;	// Grey
    Stripes[13] = 0xff00ff00;	// Green
    Stripes[14] = 0xff00ff00;	// Green

    Stripes[15] = 0xff0000ff;	// Blue
    Stripes[16] = 0xff0000ff;	// Blue
	Stripes[17] = 0xff707070;   // Grey
    Stripes[18] = 0xff0000ff;	// Blue
    Stripes[19] = 0xff0000ff;	// Blue

    Stripes[20] = 0xffffff00;   // Yellow
    Stripes[21] = 0xffffff00;   // Yellow
	Stripes[22] = 0xff707070;   // Grey
    Stripes[23] = 0xffffff00;   // Yellow
    Stripes[24] = 0xffffff00;   // Yellow

    Stripes[25] = 0xffff00ff;	// Magenta
    Stripes[26] = 0xffff00ff;	// Magenta
	Stripes[27] = 0xff707070;	// Grey
    Stripes[28] = 0xffff00ff;	// Magenta
    Stripes[29] = 0xffff00ff;	// Magenta

    Stripes[30] = 0xff00ffff;	// Cyan
    Stripes[31] = 0xff00ffff;	// Cyan
	Stripes[32] = 0xff707070;	// Grey
    Stripes[33] = 0xff00ffff;	// Cyan
    Stripes[34] = 0xff00ffff;	// Cyan

    Stripes[35] = 0xff000000;	// Black
    Stripes[36] = 0xff000000;	// Black
	Stripes[37] = 0xff707070;   // Grey
    Stripes[38] = 0xff000000;	// Black
    Stripes[39] = 0xff000000;	// Black

    // Set the Image depth, width, and height
    int i,j;
    UINT nImageDepth = 8;
    UINT nImageWidth = 16;
    UINT nImageHeight = 16;
    TCHAR szBuffer[80];
	bool bResult = true;
	DWORD Usage = (dwLoadOption == TEXLOAD_LOCKCOPY) ? USAGE_DYNAMIC : 0;

	// Make the texture larger for YUV's to avoid YUV precision issues
	if (m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat].ddpfPixelFormat.dwFlags & PF_YUV)
	{
		nImageWidth = 64;
		nImageHeight = 64;
	}

	// Log the current Volume format and load method.
    FmtToString(FORMAT_TEXTURE,&m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],szBuffer);

    switch (dwLoadOption)
    {    
        case TEXLOAD_UPDATETEXTURE:
            WriteToLog("Tex: %dx%dx%d, %s (UpdateTex).\n",nImageWidth,nImageHeight,nImageDepth,szBuffer);
            break;
        case TEXLOAD_COPYRECTS:
            WriteToLog("Tex: %dx%dx%d, %s (CopyRects).\n",nImageWidth,nImageHeight,nImageDepth,szBuffer);
            break;
        case TEXLOAD_LOCKCOPY:
            WriteToLog("Tex: %dx%dx%d, %s (LockCopy).\n",nImageWidth,nImageHeight,nImageDepth,szBuffer);
            break;
        case TEXLOAD_UPDATESURFACE:
            WriteToLog("Tex: %dx%dx%d, %s (UpdateSurf).\n",nImageWidth,nImageHeight,nImageDepth,szBuffer);
            break;
        default:
		    WriteToLog("Error: Unknown texture load option (%x).\n",dwLoadOption);
            return D3DTESTINIT_ABORT;
    }

    // Create the stripe image data
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

        ppImage[i]->LoadStripes(nImageWidth,nImageHeight,5,&Stripes[i*5],true); 
    }

	// Create the volume texture
	bResult = CreateVolumeTexture(nImageWidth,nImageHeight,nImageDepth,1,Usage,m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],POOL_DEFAULT,&pVolTexture);

	// Tell log about CreateVolumeTexture failure
    if (!bResult || NULL == pVolTexture) 
	{
    	WriteToLog("CreateVolumeTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

	// Set the palette
	if (pVolTexture->IsPalettized())
        pVolTexture->SetPalette(NULL);

	// Load the images
	if (!pVolTexture->LoadImage(1, ppImage, dwLoadOption))
	{
		WriteToLog("LoadImage() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
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

bool CVolTexTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
    float fW0, fW1, fUV1;
	msgString[0] = '\0';
	szStatus[0] = '\0';

    // Determine which slices to display
    switch (uTestNum)
    {
        // Use slices 0 to 3
        case 9: fW0 = 0.0f;
                fW1 = 0.5f;
                fUV1 = 4.0f;

                // Tell the log and status window about test case
                sprintf(msgString,"%sSlices: 0 - 3",msgString);
                break;

        // Use slices 2 to 5
        case 10:fW0 = 0.25f;
                fW1 = 0.75f;
                fUV1 = 4.0f;

                // Tell the log and status window about test case
                sprintf(msgString,"%sSlices: 2 - 5",msgString);
                break;

        // Use slices 4 to 7
        case 11:fW0 = 0.5f;
                fW1 = 1.0f;
                fUV1 = 4.0f;

                // Tell the log and status window about test case
                sprintf(msgString,"%sSlices: 4 - 7",msgString);
                break;

        // Use all eight slices
        case 12:fW0 = 0.0f;
                fW1 = 1.0f;
                fUV1 = 8.0f;

                // Tell the log and status window about test case
                sprintf(msgString,"%sSlices: 0 - 7",msgString);
                break;

        // Use each slice separately
        default:fW1 = uTestNum / 8.0f;
                fW0 = fW1 - 0.125f;
                fUV1 = 1.0f;

                // Tell the log and status window about test case
                sprintf(msgString,"%sSlice: %d",msgString,uTestNum-1);
                break;
    }

    // Load grid vertices
	LoadGrid(0,0,fW0,fUV1,fUV1,fW1);

    // Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);
	
    // Inidicate how the texture was loaded
    switch (dwLoadOption)
    {    
        case TEXLOAD_UPDATETEXTURE:
        	sprintf(szStatus, "%s$aUpdateTex",szStatus);
            break;
        case TEXLOAD_COPYRECTS:
        	sprintf(szStatus, "%s$aCopyRects",szStatus);
            break;
        case TEXLOAD_LOCKCOPY:
        	sprintf(szStatus, "%s$aLockCopy",szStatus);
            break;
        case TEXLOAD_UPDATESURFACE:
        	sprintf(szStatus, "%s$aUpdateSurf",szStatus);
            break;
        default:
        	sprintf(szStatus, "%s$rUnknown (%x)",szStatus,dwLoadOption);
            Fail();
		    return false;
    }

    return true;
}
