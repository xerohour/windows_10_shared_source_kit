#include <d3d8.h>
#include <d3dx8.h>
#include "Volume.h"

const int NUM_LEVELS = 5;   // Mipmap levels to create
const int NUM_ANGLES = 8;

CVolFilterTest::CVolFilterTest()
{
	m_szTestName = "Volume Filter";
	m_szCommandKey = "Filter";

	// Indicate that we have MipMap tests
    bMipMap = true;

    // Initialize some variables
	pMagVolTexture = NULL;
	pMipVolTexture = NULL;

    // We only want one volume format
    if (m_Options.ModeOptions.VolumeFmt == FMT_ALL)
        m_Options.ModeOptions.VolumeFmt = FMT_ANY;
}

CVolFilterTest::~CVolFilterTest()
{
}

UINT CVolFilterTest::TestInitialize(void)
{
    // Setup the Test range
	SetTestRange(1, (int)(NUM_ANGLES * NUM_LEVELS * MAX_FILTER_TESTS));

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
 
    // Check if any volume formats were available
	if (m_uCommonVolumeFormats == 0) 
    {
		WriteToLog("No volume texture formats found.\n");
		return D3DTESTINIT_ABORT;
    }

    // Load sphere vertices
	LoadSphere();
	
	//Win8 [TestBug]276365
	// Set image compare variable - default m_fPassRequired is 0.85f
	// Set it to 0.80f to loosen passing criteria
	m_pFramework->GetImageCmp()->SetPassRequired(0.80f);
    
	// Setup the colors for the RedStripes array
	D3DCOLOR		RedStripes[4];

	RedStripes[0] = 0xffffffff; // White
	RedStripes[1] = 0xffff0000;	// Red
	RedStripes[2] = 0xffffffff; // White
	RedStripes[3] = 0xffff0000;	// Red

    // Setup the Image depth, width, and height for the min volume texture
    int i,j;
    UINT nImageDepth = 4;
    UINT nImageWidth = 32;
    UINT nImageHeight = 32;
	bool bResult = true;
    TCHAR szBuffer[80];
	szStatus[0] = '\0';

	// Log the current Volume format and load method.
    FmtToString(FORMAT_TEXTURE,&m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],szBuffer);
    WriteToLog("Tex: %s (UpdateTex).\n",szBuffer);
   	sprintf(szStatus, "%s$aUpdateTex",szStatus);

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

        ppImage[i]->LoadStripes(nImageWidth,nImageHeight,1,&RedStripes[i],false); 
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

    // Cleanup image data
    for (i=0; i<nImageDepth; i++)
    {
       SAFEDELETE(ppImage[i]); 
    }
	SAFEDELETEA(ppImage);

	// Setup the colors for the GreenStripes array
	D3DCOLOR		GreenStripes[4];

	GreenStripes[0] = 0xffffffff;   // White
	GreenStripes[1] = 0xff00ff00;   // Green
	GreenStripes[2] = 0xffffffff;   // White
	GreenStripes[3] = 0xff00ff00;   // Green

    // Setup the Image depth, width, and height for the mag volume texture
    nImageDepth = 4;
    nImageWidth = 128;
    nImageHeight = 128;

    // Create the stripe image data
    ppImage = new CImage*[nImageDepth];
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

        ppImage[i]->LoadStripes(nImageWidth,nImageHeight,1,&GreenStripes[i],false); 
    }

	// Create the volume texture
	bResult = CreateVolumeTexture(nImageWidth,nImageHeight,nImageDepth,1,0,m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],POOL_DEFAULT,&pMagVolTexture);

	// Tell log about CreateVolumeTexture failure
    if (!bResult || NULL == pMagVolTexture) 
	{
    	WriteToLog("CreateVolumeTexture(pMagVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

	// Set the palette
	if (pMagVolTexture->IsPalettized())
        pMagVolTexture->SetPalette(NULL);

	// Load the images
	if (!pMagVolTexture->LoadImage(1, ppImage, TEXLOAD_UPDATETEXTURE))
	{
		WriteToLog("LoadImage(pMagVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

    // Cleanup image data
    for (i=0; i<nImageDepth; i++)
    {
       SAFEDELETE(ppImage[i]); 
    }
	SAFEDELETEA(ppImage);

    // Check if MipVolumes are available	
	if (dwTextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP)
	{
		// Setup the colors for the BlueStripes array
		D3DCOLOR		BlueStripes[8];

		BlueStripes[0] = 0xff00004b;    // Off Blue
		BlueStripes[1] = 0xff0000ff;	// Blue
		BlueStripes[2] = 0xff00004b;    // Off Blue
		BlueStripes[3] = 0xff0000ff;	// Blue
		BlueStripes[4] = 0xff00004b;    // Off Blue
		BlueStripes[5] = 0xff0000ff;	// Blue
		BlueStripes[6] = 0xff00004b;    // Off Blue
		BlueStripes[7] = 0xff0000ff;	// Blue

		// Setup the colors for the YellowStripes array
		D3DCOLOR		YellowStripes[4];

		YellowStripes[0] = 0xff4b4b00;  // Off Yellow
		YellowStripes[1] = 0xffffff00;  // Yellow
		YellowStripes[2] = 0xff4b4b00;  // Off Yellow
		YellowStripes[3] = 0xffffff00;  // Yellow

		// Setup the colors for the MagentaStripes array
		D3DCOLOR		MagentaStripes[2];

		MagentaStripes[0] = 0xff4b004b; // Off Magenta
		MagentaStripes[1] = 0xffff00ff;	// Magenta

		D3DCOLOR dwRed[1]   = {0xffff0000};	// Red
		D3DCOLOR dwGreen[1] = {0xff00ff00};  // Green

		// Setup the Image depth, width, and height for the mip volume texture
		UINT nMipLevels = NUM_LEVELS;

		nImageDepth = 8;
		nImageWidth = 256;
		nImageHeight = 256;

		// Create the stripe image data
		CImage *** pppImages = new CImage**[nMipLevels];
		if (NULL == pppImages)
		{
			WriteToLog("TestInitialize() - Out of memory.\n");
			return D3DTESTINIT_ABORT;
		}

		D3DCOLOR* dwStripeIndex[] = { (D3DCOLOR*)BlueStripes, (D3DCOLOR*)YellowStripes, 
									  (D3DCOLOR*)MagentaStripes, (D3DCOLOR*)dwRed, (D3DCOLOR*)dwGreen};

		// Allocate/load the CImages
		for (i=0; i<nMipLevels; i++)
		{
			UINT nDepth = nImageDepth / min((UINT)((DWORD)1 <<  i),nImageDepth);

			pppImages[i] = new CImage*[nDepth];
			if (NULL == pppImages[i])
			{
				WriteToLog("TestInitialize() - Out of memory.\n");
				return D3DTESTINIT_ABORT;
			}
    
			for (j=0; j<nDepth; j++)
			{
				UINT nWidth = nImageWidth / min((UINT)((DWORD)1 <<  i),nImageWidth);
				UINT nHeight = nImageHeight / min((UINT)((DWORD)1 <<  i),nImageHeight);
				pppImages[i][j] = new CImage();
				if (NULL == pppImages[i][j])
				{
					WriteToLog("TestInitialize() - Out of memory.\n");
					return D3DTESTINIT_ABORT;
				}
				pppImages[i][j]->LoadStripes(nWidth,nHeight,1,&(dwStripeIndex[i][j]),false); 
			}
		}

		// Create the mipmap volume texture
		bResult = CreateVolumeTexture(nImageWidth,nImageHeight,nImageDepth,nMipLevels,FMWK_DEFAULT,m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],POOL_DEFAULT,&pMipVolTexture);

		// Tell log about CreateVolumeTexture failure
		if (!bResult || NULL == pMipVolTexture) 
		{
    		WriteToLog("CreateVolumeTexture(pMipVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			return D3DTESTINIT_ABORT;
		}

		// Set the palette
		if (pMipVolTexture->IsPalettized())
			pMipVolTexture->SetPalette(NULL);

		// Load the images
		if (!pMipVolTexture->LoadImage(pppImages, TEXLOAD_UPDATETEXTURE))
		{
			WriteToLog("LoadImage(pMipVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			return D3DTESTINIT_ABORT;
		}

		// Cleanup mip image data
		for (i=0; i<nMipLevels; i++)
		{
			UINT nDepth = nImageDepth / min((UINT)((DWORD)1 <<  i),nImageDepth);
			for (j=0; j<nDepth; j++)
			{
				SAFEDELETE(pppImages[i][j]);
			}
			SAFEDELETEA(pppImages[i]);
		}
		SAFEDELETEA(pppImages);
	}

    return D3DTESTINIT_RUN;
}

bool CVolFilterTest::ExecuteTest(UINT uTestNum)
{
	int nTest = (uTestNum - 1) % (NUM_LEVELS * NUM_ANGLES);
	int nGroup = (uTestNum - 1) / (NUM_LEVELS * NUM_ANGLES);
	float fAngle = (360.0f / NUM_ANGLES) * nTest;
	while (fAngle>=360.0f) { fAngle-=360.0f; }
    D3DVECTOR ZVector = {0.0f,0.0f,3.0f};
    D3DXMATRIX Matrix, RotMatrix, TransMatrix;
    DWORD dwPasses;
	msgString[0] = '\0';

	// Get the Z distance of the current level
    if (nGroup <= 7)
    {
        float fZVals[5] = {3.0f, 5.0f, 8.0f, 10.0f, 13.0f};

	    ZVector.z = fZVals[nTest / NUM_ANGLES];
    }
    else
    {
        float fZVals[5] = {3.0f, 4.0f, 6.0f, 11.0f, 16.0f};

	    ZVector.z = fZVals[nTest / NUM_ANGLES];
    }

	// Reset some image compare variables to defaults
    m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.15f);
    m_pFramework->GetImageCmp()->SetTolPerPixel(0.10f);
    m_pFramework->GetImageCmp()->SetImgDeltaWght(1.00f);

    // Reset current texture being used
	SetTexture(0,NULL);

    // Check MipVolume cap for mipmap cases
    if (nGroup >= 8)
    {
    	DWORD dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;
        DWORD dwVolumeTextureFilterCaps = m_pSrcDevice->GetCaps()->dwVolumeTextureFilterCaps;

        // Check if MipVolumes are available	
	    if (!(dwTextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP)) 
	    {
            // Do VolumeTextureFilter cap consistency checking
            if ((dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT) ||
                (dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
            {
	    	    WriteToLog("\nDevice capability inconsistency: Texture MipVolumeMap should be set.");
                TestFinish(D3DTESTFINISH_ABORT);
    	    	return false;
            }

		    WriteToLog("\nDevice capability not supported: Texture MipVolumeMap.\n");
            TestFinish(D3DTESTFINISH_SKIPREMAINDER);
	    	return false;
	    }

		// Setup custom image compare to avoid LOD approximations
		m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.20f);
		m_pFramework->GetImageCmp()->SetTolPerPixel(0.15f);
		m_pFramework->GetImageCmp()->SetImgDeltaWght(0.80f);
    }

    // Setup the filter mode
    UINT Result = SetupFilterMode(nGroup);
    
    if (Result == D3DTESTINIT_SKIPALL)
    {
        SkipTests(NUM_ANGLES * NUM_LEVELS);
	    return false;
    }

    if (Result == D3DTESTINIT_ABORT)
    {
        TestFinish(D3DTESTFINISH_ABORT);
	    return false;
    }

    // Indicate what depth we are rending at
   	sprintf(msgString,"%sZ: %.2f, %.2f degrees, ",msgString,ZVector.z,fAngle);

	// Set the volume texture according to group
    if (nGroup <= 7)
    {
        if (nGroup <= 3)
        {
            // Log texture size information
            sprintf(msgString,"%sTexSize: 32x32x4 ",msgString);
        }
        else    
        {
            // Log texture size information
            sprintf(msgString,"%sTexSize: 128x128x4 ",msgString);
        }
    }
    else
    {
        // Log texture size information
        sprintf(msgString,"%sTexSize: 256x256x8 ",msgString);
    }

    // Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

	if (nGroup <= 3)
	{
        // Use the min volume texture
		if (!SetTexture(0,pVolTexture))
		{
			WriteToLog("SetTexture(pVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
	}
	else if (nGroup <= 7)
	{
        // Use the mag volume texture
        if (!SetTexture(0,pMagVolTexture))
		{
			WriteToLog("SetTexture(pMagVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
	}
	else
	{
        // Use the mipmap volume texture
      	if (!SetTexture(0,pMipVolTexture))
		{
			WriteToLog("SetTexture(pMipVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
	}

    // Check with ValidateDevice, just to be sure
	if (!ValidateDevice(&dwPasses))
	{
        WriteToLog("ValidateDevice() failed with Passes = %d and HResult = %s.\n",dwPasses,m_pD3D->HResultToString(GetLastError()));
        SkipTests(NUM_ANGLES * NUM_LEVELS);
		return false;
	}

    // Setup the world matrix for the translating/rotating sphere
    D3DXMatrixIdentity(&Matrix);
    D3DXMatrixTranslation(&TransMatrix, ZVector.x, ZVector.y, ZVector.z);
    D3DXMatrixMultiply(&Matrix, &TransMatrix, &Matrix);
    D3DXMatrixRotationX(&RotMatrix, fAngle*pi/180);
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

bool CVolFilterTest::TestTerminate()
{
    CVolumeTest::TestTerminate();

    // Cleanup textures
    RELEASE(pMagVolTexture);
    RELEASE(pMipVolTexture);

    return true;
}

UINT CVolFilterTest::SetupFilterMode(int nMode)
{
    DWORD dwVolumeTextureFilterCaps;

	// Check the caps we are interested in
   	dwVolumeTextureFilterCaps = m_pSrcDevice->GetCaps()->dwVolumeTextureFilterCaps;

    switch (nMode)
    {
        case 0:
        case 4:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
       	    sprintf(msgString,"%sFilter: PPN, ",msgString);
            break;
        case 1:
        case 5:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))       
            {
                WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
       	    sprintf(msgString,"%sFilter: LPN, ",msgString);
            break;
        case 2:
        case 6:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
        	sprintf(msgString,"%sFilter: PLN, ",msgString);
            break;
        case 3:
        case 7:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))       
            {
                WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
        	sprintf(msgString,"%sFilter: LLN, ",msgString);
            break;
        case 8:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MipFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
        	sprintf(msgString,"%sFilter: PPP, ",msgString);
            break;
        case 9:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MipFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
        	sprintf(msgString,"%sFilter: LLP, ",msgString);
            break;
        case 10:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MipFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);
        	sprintf(msgString,"%sFilter: PPL, ",msgString);
            break;
        case 11:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MipFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);
        	sprintf(msgString,"%sFilter: LLL, ",msgString);
            break;
    }

    return D3DTESTINIT_RUN;
}
