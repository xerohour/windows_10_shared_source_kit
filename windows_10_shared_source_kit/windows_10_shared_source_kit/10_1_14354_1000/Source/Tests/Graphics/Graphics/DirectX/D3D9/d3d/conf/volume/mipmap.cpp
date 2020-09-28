#include <d3d8.h>
#include <d3dx8.h>
#include "Volume.h"

const int NUM_ANGLES = 8;

//************************************************************************
// MipVolUT Test functions

CMipVolUTTest::CMipVolUTTest()
{
	m_szTestName = "Volume MipMap (UpdateTex)";
	m_szCommandKey = "MipMapUT";

    // Indicate that we are a MipMap test
	bMipMap = true;
	bPointFilter = false;

    dwLoadOption = TEXLOAD_UPDATETEXTURE;
}

//************************************************************************
// MipVolLC Test functions

CMipVolLCTest::CMipVolLCTest()
{
	m_szTestName = "Volume MipMap (LockCopy)";
	m_szCommandKey = "MipMapLC";

    // Indicate that we are a MipMap test
	bMipMap = true;
	bPointFilter = false;

    dwLoadOption = TEXLOAD_LOCKCOPY;
}

UINT CMipVolTest::TestInitialize(void)
{
	// Setup the Test range 
    SetTestRange(1, MAX_MIP_TESTS * NUM_ANGLES);

 	// Make sure we are on at lease DX8
	if (m_dwVersion <= 0x0700)
	{
		WriteToLog("This test requires at least Dx8.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Check if Volumes are available	
	DWORD dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;
    DWORD dwVolumeTextureFilterCaps = m_pSrcDevice->GetCaps()->dwVolumeTextureFilterCaps;

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

    // Check if MipVolumes are available	
	if (!(dwTextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP)) 
	{
        // Do VolumeTextureFilter cap consistency checking
        if ((dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT) ||
            (dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
        {
	    	WriteToLog("Device capability inconsistency: Texture MipVolumeMap should be set.\n");
	        return D3DTESTINIT_ABORT;
        }

		WriteToLog("Device capability not supported: Texture MipVolumeMap.\n");
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
		return D3DTESTINIT_SKIPALL;
    }

	// On Dx9c, need to check for UsageQueryWrapAndMip
	if (m_pD3D->GetRuntimeVersion() >= 9.029f)
	{
		HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
											 USAGE_QUERY_WRAPANDMIP, RTYPE_VOLUMETEXTURE, m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat]);
		if (FAILED(Result))
		{
			WriteToLog("Unable to mipmap this format. CheckDeviceFormat(WrapAndMip) failed: %s.\n",m_pD3D->HResultToString(Result));
			return D3DTESTINIT_SKIPALL;
		}
	}

	// On Dx9, need to check for UsageQueryFilter
	if (m_dwVersion >= 0x0900)
	{
		if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
											 USAGE_QUERY_FILTER, RTYPE_VOLUMETEXTURE, m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat])))
		{
			WriteToLog("Unable to filter this format, trying point sampling.\n");
			bPointFilter = true;
		}
		else
			bPointFilter = false;
	}

	// Setup custom image compare to avoid LOD approximations
    m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.20f);
    m_pFramework->GetImageCmp()->SetTolPerPixel(0.15f);
    m_pFramework->GetImageCmp()->SetImgDeltaWght(0.80f);

    // Load sphere vertices
	LoadSphere();

	// Setup the colors for the AllStripes array
	D3DCOLOR		dwAllStripes[16];

	dwAllStripes[0]  = 0xffffffff;  // White
	dwAllStripes[1]  = 0xff4b4b4b;	// Off White
	dwAllStripes[2]  = 0xffff0000;	// Red
	dwAllStripes[3]  = 0xff4b0000;	// Off Red
	dwAllStripes[4]  = 0xff00ff00;	// Green
	dwAllStripes[5]  = 0xff004b00;	// Off Green
	dwAllStripes[6]  = 0xff0000ff;	// Blue
	dwAllStripes[7]  = 0xff00004b;  // Off Blue
	dwAllStripes[8]  = 0xffffff00;  // Yellow
	dwAllStripes[9]  = 0xff4b4b00;	// Off Yellow
	dwAllStripes[10] = 0xffff00ff;	// Magenta
	dwAllStripes[11] = 0xff4b004b;	// Off Magenta
	dwAllStripes[12] = 0xff00ffff;	// Cyan
	dwAllStripes[13] = 0xff004b4b;	// Off Cyan
	dwAllStripes[14] = 0xff808080;	// Grey
	dwAllStripes[15] = 0xff252525;  // Off Grey

	// Setup the colors for the EightStripes array
	D3DCOLOR		dwEightStripes[8];

	dwEightStripes[0] = 0xffffffff;     // White
	dwEightStripes[1] = 0xffff0000;     // Red
	dwEightStripes[2] = 0xff00ff00;     // Green
	dwEightStripes[3] = 0xff0000ff;     // Blue
	dwEightStripes[4] = 0xffffff00;     // Yellow
	dwEightStripes[5] = 0xffff00ff;     // Magenta
	dwEightStripes[6] = 0xff00ffff;     // Cyan
	dwEightStripes[7] = 0xff808080;     // Grey

    // Setup the colors for the FourStripes array
	D3DCOLOR		dwFourStripes[4];

	dwFourStripes[0] = 0xffffff00;	// Yellow  
	dwFourStripes[1] = 0xffff00ff;	// Magenta 
	dwFourStripes[2] = 0xff00ffff;	// Cyan    
	dwFourStripes[3] = 0xff808080;  // Grey    

	// Setup the colors for the GreenBlue array
	D3DCOLOR		dwTwoStripes[2];

	dwTwoStripes[0] = 0xff00ff00;    // Green
	dwTwoStripes[1] = 0xff0000ff;	// Blue

	// Setup the colors for the Red array
	D3DCOLOR		dwOneStripe[1];

	dwOneStripe[0] = 0xffff0000;  // Red

    // Setup mip volume depths
    int i,j;
	UINT nMipLevels = 5;
	UINT nMipDepth = 16;
	UINT nMipWidth = 128;
    UINT nMipHeight = 128;
    TCHAR szBuffer[80];
	bool bResult = true;
	DWORD Usage = FMWK_DEFAULT;

	// Log the current Volume format and load method.
    FmtToString(FORMAT_TEXTURE,&m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],szBuffer);

    switch (dwLoadOption)
    {    
        case TEXLOAD_UPDATETEXTURE:
            WriteToLog("Tex: %dx%dx%d, %s (UpdateTex).\n",nMipWidth,nMipHeight,nMipDepth,szBuffer);
            break;
        case TEXLOAD_COPYRECTS:
            WriteToLog("Tex: %dx%dx%d, %s (CopyRects).\n",nMipWidth,nMipHeight,nMipDepth,szBuffer);
            break;
        case TEXLOAD_LOCKCOPY:
            WriteToLog("Tex: %dx%dx%d, %s (LockCopy).\n",nMipWidth,nMipHeight,nMipDepth,szBuffer);
            break;
        case TEXLOAD_UPDATESURFACE:
            WriteToLog("Tex: %dx%dx%d, %s (UpdateSurf).\n",nMipWidth,nMipHeight,nMipDepth,szBuffer);
            break;
        default:
		    WriteToLog("Error: Unknown texture load option (%x).\n",dwLoadOption);
            return D3DTESTINIT_ABORT;
    }

    // Create the stripe image data
    CImage *** pppImages = new CImage**[nMipLevels];
	if (NULL == pppImages)
	{
		WriteToLog("TestInitialize() - Out of memory.\n");
		return D3DTESTINIT_ABORT;
	}

	ZeroMemory(pppImages, sizeof(CImage **) * nMipLevels);

	D3DCOLOR* dwStripeIndex[] = { (D3DCOLOR*)dwAllStripes, (D3DCOLOR*)dwEightStripes, 
								  (D3DCOLOR*)dwFourStripes, (D3DCOLOR*)dwTwoStripes, (D3DCOLOR*)dwOneStripe};

	// Allocate/load the CImages
	for (i=0; i<nMipLevels; i++)
	{
		UINT nDepth = nMipDepth / min((UINT)((DWORD)1 <<  i),nMipDepth);

        pppImages[i] = new CImage*[nDepth];
        if (NULL == pppImages[i])
		{
			// Cleanup image data
			UINT uDepth = nMipDepth;
			for (UINT ii=0; ii < nMipLevels; ii++)
			{
				if (NULL != pppImages[ii])
				for (UINT jj = 0; jj < uDepth; jj++)
					SAFEDELETE(pppImages[ii][jj]);

				SAFEDELETEA(pppImages[ii]);
				uDepth = max( 1, uDepth>>1 );
			}
			SAFEDELETEA(pppImages);

			WriteToLog("TestInitialize() - Out of memory.\n");
			return D3DTESTINIT_ABORT;
		}

		ZeroMemory(pppImages[i], sizeof(CImage *) * nDepth);
    
		for (j=0; j<nDepth; j++)
		{
            UINT nWidth = nMipWidth / min((UINT)((DWORD)1 <<  i),nMipWidth);
            UINT nHeight = nMipHeight / min((UINT)((DWORD)1 <<  i),nMipHeight);
			
			pppImages[i][j] = new CImage();
			
			if (NULL == pppImages[i][j])
			{
				UINT uDepth = nMipDepth;
				for (UINT ii=0; ii < nMipLevels; ii++)
				{
					if (NULL != pppImages[ii])
					for (UINT jj = 0; jj < uDepth; jj++)
						SAFEDELETE(pppImages[ii][jj]);

					SAFEDELETEA(pppImages[ii]);
					uDepth = max( 1, uDepth>>1 );
				}
				SAFEDELETEA(pppImages);

				WriteToLog("TestInitialize() - Out of memory.\n");
				return D3DTESTINIT_ABORT;
			}

            pppImages[i][j]->LoadStripes(nWidth,nHeight,1, &(dwStripeIndex[i][j]),false); 
		}
	}

	// Make sure the Usage field is correct for the load method
	if (dwLoadOption == TEXLOAD_LOCKCOPY)
		Usage = USAGE_DYNAMIC;

	// Create the mipmap volume texture
	bResult = CreateVolumeTexture(nMipWidth,nMipHeight,nMipDepth,nMipLevels,Usage,m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],POOL_DEFAULT,&pVolTexture);

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
	if (!pVolTexture->LoadImage(pppImages, dwLoadOption))
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

	// Cleanup mip image data
	for (i=0; i<nMipLevels; i++)
	{
        UINT nDepth = nMipDepth / min((UINT)((DWORD)1 <<  i),nMipDepth);
        for (j=0; j<nDepth; j++)
		{
			SAFEDELETE(pppImages[i][j]);
		}
        SAFEDELETEA(pppImages[i]);
	}
	SAFEDELETEA(pppImages);

    return D3DTESTINIT_RUN;
}

bool CMipVolTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	int nTest = uTestNum - 1;
    D3DVECTOR ZVector = {0.0f,0.0f,3.0f + ((nTest / NUM_ANGLES) * 2)};
	float fAngle = (360.0f / NUM_ANGLES) * nTest;			
	while (fAngle>=360.0f) { fAngle-=360.0f; }
    D3DXMATRIX Matrix, RotMatrix, TransMatrix;
	msgString[0] = '\0';
	szStatus[0] = '\0';

    // On last test set, double Z distance 
    if (nTest >= (MAX_MIP_TESTS * NUM_ANGLES) - NUM_ANGLES)
    {
        ZVector.z = (3.0f + ((nTest / NUM_ANGLES) * 2)) * 2.0f;
    }

    // Indicate what depth we are rending at and sphere angle
   	sprintf(msgString,"%sZ: %.2f, Angle: %.2f degrees",msgString,ZVector.z,fAngle);

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

	if (bPointFilter)
	{
		SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
		SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
		SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
	}
	else
	{
		SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
		SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);

		// Set mip filter according to the Volume mip filter cap
		if (m_pSrcDevice->GetCaps()->dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
			SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);
		else
			SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
	}

    // Setup the world matrix for the translating/rotating sphere
    D3DXMatrixIdentity(&Matrix);
    D3DXMatrixTranslation(&TransMatrix, ZVector.x, ZVector.y, ZVector.z);
    D3DXMatrixMultiply(&Matrix, &TransMatrix, &Matrix);
    D3DXMatrixRotationY(&RotMatrix, fAngle*pi/180);
    D3DXMatrixMultiply(&Matrix, &RotMatrix, &Matrix);
    if (!SetTransform(D3DTS_WORLD, (D3DMATRIX *) &Matrix))
	{
        // Tell the logfile about the SetTransform failure
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
        return false;
	}

    return true;
}

