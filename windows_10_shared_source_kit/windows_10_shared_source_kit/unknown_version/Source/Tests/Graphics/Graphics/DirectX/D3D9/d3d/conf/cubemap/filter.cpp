//////////////////////////////////////////////////////////////////////
// Filter.cpp: implementation of the CCubeFilter class.
//////////////////////////////////////////////////////////////////////

#include <d3d8.h>
#include <d3dx8.h>
#include "Cubemap.h"

#ifdef FAST_TEST
const int MIPMAP_LEVELS = 2;
const int NUM_ANGLES = 3;
#else
const int MIPMAP_LEVELS = 3;
const int NUM_ANGLES = 8;
#endif

CCubeFilterTest::CCubeFilterTest()
{
	m_szTestName = "Cube Filter";
	m_szCommandKey = "Filter";

    // Initialize some variables
	pMagCubeTexture = NULL;
	pMipCubeTexture = NULL;
    bMipDelay = false;
    bMipSkip = false;

 	// Set degree increase per test
#ifdef FAST_TEST
	fDegInc = 135.0f;
#else
	fDegInc = 45.0f;
#endif

	// We only want one cube format
    if (m_Options.ModeOptions.CubeFmt == FMT_ALL)
        m_Options.ModeOptions.CubeFmt = FMT_ANY;
}

UINT CCubeFilterTest::TestInitialize(void)
{
    // Setup the Test range
	SetTestRange(1, (int)((360.0f/fDegInc) * MIPMAP_LEVELS * NUM_ANGLES));

	// Make sure we are on at lease DX7
	if (m_dwVersion <= 0x0600)
	{
		WriteToLog("This test requires at least Dx7.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Check the caps we are interested in
	DWORD dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;

	if (!(dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP))
	{
		if (m_uCommonCubeFormats != 0) 
		{
  			WriteToLog("Device capability inconsistency: Found cube texture formats with Texture CubeMap not set.");
			return D3DTESTINIT_ABORT;
		}

		WriteToLog("Device capability not supported: Texture Cubemap.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Check if any cube formats were available
	if (m_uCommonCubeFormats == 0) 
    {
		WriteToLog("No cube texture formats found.\n");
		return D3DTESTINIT_ABORT;
    }

    // Setup the colors for the RedStripes array
	D3DCOLOR		RedStripes[8];

	RedStripes[0] = RGBA_MAKE(255,255,255,255);	// White
	RedStripes[1] = RGBA_MAKE(255,0,0,255);		// Red
	RedStripes[2] = RGBA_MAKE(255,255,255,255);	// White
	RedStripes[3] = RGBA_MAKE(255,0,0,255);		// Red
	RedStripes[4] = RGBA_MAKE(255,255,255,255);	// White
	RedStripes[5] = RGBA_MAKE(255,0,0,255);		// Red
	RedStripes[6] = RGBA_MAKE(255,255,255,255);	// White
	RedStripes[7] = RGBA_MAKE(255,0,0,255);		// Red

	// Setup the colors for the GreenStripes array
	D3DCOLOR		GreenStripes[8];

	GreenStripes[0] = RGBA_MAKE(255,255,255,255);	// White
	GreenStripes[1] = RGBA_MAKE(0,255,0,255);		// Green
	GreenStripes[2] = RGBA_MAKE(255,255,255,255);	// White
	GreenStripes[3] = RGBA_MAKE(0,255,0,255);		// Green
	GreenStripes[4] = RGBA_MAKE(255,255,255,255);	// White
	GreenStripes[5] = RGBA_MAKE(0,255,0,255);		// Green
	GreenStripes[6] = RGBA_MAKE(255,255,255,255);	// White
	GreenStripes[7] = RGBA_MAKE(0,255,0,255);		// Green

    // Setup some variables 
	bool bResult = true;
    UINT nImageLength = 256;
    TCHAR szBuffer[80];

	// Allocate/Load the CImages
	for (int i=0; i<6; i++)
	{
		pMinImage[i] = new CImage();
		if (NULL == pMinImage[i])
		{
			WriteToLog("TestInitialize() - Out of memory.\n");
			return D3DTESTINIT_ABORT;
		}

		pMinImage[i]->LoadStripes(16,16,8,RedStripes,false);

		pMagImage[i] = new CImage();
		if (NULL == pMagImage[i])
		{
			WriteToLog("TestInitialize() - Out of memory.\n");
			return D3DTESTINIT_ABORT;
		}

		pMagImage[i]->LoadStripes(nImageLength,nImageLength,8,GreenStripes,false);
	}

	// Get the current cube format.
    FmtToString(FORMAT_TEXTURE,&m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat],szBuffer);

	// Create the min cube map texture
	bResult = CreateCubeTexture(16, 1, 0, m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat], FMWK_DEFAULT, &pCubeTexture);

	// Tell log about CreateCubeTexture failure
    if (!bResult || NULL == pCubeTexture) 
	{
		HRESULT hr = GetLastError();
		WriteToLog("CreateCubeTexture(%s) failed with HResult = %s.\n",szBuffer,m_pD3D->HResultToString(hr));

		// In Dx7 there was no CubeTexture list, so consider E_NOTIMPL a skip
		if ((m_dwVersion == 0x0700) && (hr == E_NOTIMPL))
			return D3DTESTINIT_SKIPALL;
		else
			return D3DTESTINIT_ABORT;
	}

	// Set the palette if needed
	if (pCubeTexture->IsPalettized())
		pCubeTexture->SetPalette(NULL);

	// Create the mag cube map texture
	bResult = CreateCubeTexture(nImageLength, 1, 0, m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat], FMWK_DEFAULT, &pMagCubeTexture);

	// Tell log about CreateCubeTexture failure
    if (!bResult || NULL == pMagCubeTexture) 
	{
		WriteToLog("CreateCubicTexture(pMagCubeTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

	// Set the palette if needed
	if (pMagCubeTexture->IsPalettized())
		pMagCubeTexture->SetPalette(NULL);

    // Check if MipCubes are available on Dx8
	if ((m_dwVersion < 0x0800) || (dwTextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP))
	{
		// Setup the colors for the BlueStripes array
		D3DCOLOR		BlueStripes[8];

		BlueStripes[0] = RGBA_MAKE(255,255,255,255);// White
		BlueStripes[1] = RGBA_MAKE(0,0,255,255);	// Blue
		BlueStripes[2] = RGBA_MAKE(255,255,255,255);// White
		BlueStripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
		BlueStripes[4] = RGBA_MAKE(255,255,255,255);// White
		BlueStripes[5] = RGBA_MAKE(0,0,255,255);	// Blue
		BlueStripes[6] = RGBA_MAKE(255,255,255,255);// White
		BlueStripes[7] = RGBA_MAKE(0,0,255,255);	// Blue

		// Setup the colors for the YellowStripes array
		D3DCOLOR		YellowStripes[8];

		YellowStripes[0] = RGBA_MAKE(255,255,255,255);// White
		YellowStripes[1] = RGBA_MAKE(255,255,0,255);  // Yellow
		YellowStripes[2] = RGBA_MAKE(255,255,255,255);// White
		YellowStripes[3] = RGBA_MAKE(255,255,0,255);  // Yellow
		YellowStripes[4] = RGBA_MAKE(255,255,255,255);// White
		YellowStripes[5] = RGBA_MAKE(255,255,0,255);  // Yellow
		YellowStripes[6] = RGBA_MAKE(255,255,255,255);// White
		YellowStripes[7] = RGBA_MAKE(255,255,0,255);  // Yellow

		// Setup the colors for the MagentaStripes array
		D3DCOLOR		MagentaStripes[8];

		MagentaStripes[0] = RGBA_MAKE(255,255,255,255);// White
		MagentaStripes[1] = RGBA_MAKE(255,0,255,255);  // Magenta
		MagentaStripes[2] = RGBA_MAKE(255,255,255,255);// White
		MagentaStripes[3] = RGBA_MAKE(255,0,255,255);  // Magenta
		MagentaStripes[4] = RGBA_MAKE(255,255,255,255);// White
		MagentaStripes[5] = RGBA_MAKE(255,0,255,255);  // Magenta
		MagentaStripes[6] = RGBA_MAKE(255,255,255,255);// White
		MagentaStripes[7] = RGBA_MAKE(255,0,255,255);  // Magenta

		// Setup mip cube dimensions
		UINT nMipLevels = MIPMAP_LEVELS;
		UINT nMipLength = 128;

		// Create the stripe image data
		pppMipImages = new CImage**[nMipLevels];
		if (NULL == pppMipImages)
		{
			WriteToLog("TestInitialize() - Out of memory.\n");
			return D3DTESTINIT_ABORT;
		}
		
		D3DCOLOR* dwStripeIndex[] = { (D3DCOLOR*)BlueStripes, (D3DCOLOR*)YellowStripes, (D3DCOLOR*)MagentaStripes};
		
		// Allocate/load the CImages
		for (int i=0; i<nMipLevels; i++)
		{
			UINT nDepth = 6;
		
			pppMipImages[i] = new CImage*[nDepth];
			if (NULL == pppMipImages[i])
			{
				WriteToLog("TestInitialize() - Out of memory.\n");
				return D3DTESTINIT_ABORT;
			}
		
			for (int j=0; j<nDepth; j++)
			{
                                UINT nLength = nMipLength / min((UINT)(float)pow(2.0f, i),nMipLength);
				pppMipImages[i][j] = new CImage();
				if (NULL == pppMipImages[i][j])
				{
					WriteToLog("TestInitialize() - Out of memory.\n");
					return D3DTESTINIT_ABORT;
				}
				pppMipImages[i][j]->LoadStripes(nLength,nLength,8,dwStripeIndex[i],false); 
			}
		}
		
		// Create the mip cube map texture
		bResult = CreateCubeTexture(nMipLength, nMipLevels, FMWK_DEFAULT, m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat], FMWK_DEFAULT, &pMipCubeTexture);

		// Tell log about CreateCubeTexture failure
		if (!bResult || NULL == pMipCubeTexture) 
		{
			HRESULT hr = GetLastError();

			WriteToLog("CreateCubicTexture(pMipCubeTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(hr));

            // For Dx7 delay action until after running the non mip tests
            if (m_dwVersion == 0x0700)
			{
				bMipDelay = true;

				// In Dx7 there was no MipCube cap, so consider E_NOTIMPL a skip
				if (hr == E_NOTIMPL)
					bMipSkip = true;
			}
            else
			    return D3DTESTINIT_ABORT;
		}

		// Set the palette if needed
		if ((NULL != pMipCubeTexture) && pMipCubeTexture->IsPalettized())
			pMipCubeTexture->SetPalette(NULL);
	}

    // Load sphere vertices
	LoadSphere();					

    return D3DTESTINIT_RUN;
}

bool CCubeFilterTest::ExecuteTest(UINT uTestNum)
{
	int nTest = (uTestNum - 1) % (MIPMAP_LEVELS * NUM_ANGLES);
	int nGroup = (uTestNum - 1) / (MIPMAP_LEVELS * NUM_ANGLES);
	int iLevel = nTest / (int)(360.0f / fDegInc);
	float fZDist = ((float)iLevel * MIPMAP_LEVELS) + fZDepthMin;
	float fAngle = fDegInc * (float)nTest;
	while (fAngle>=360.0f) { fAngle-=360.0f; }
    D3DVECTOR ZVector = {0.0f,0.0f,fZDist};
    D3DXMATRIX Matrix, RotMatrix, TransMatrix;
    TCHAR szBuffer[80];
	int nLength;
    UINT Result;
	msgString[0] = '\0';
	szStatus[0] = '\0';

	// Reset some image compare variables to defaults
    m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.15f);
    m_pFramework->GetImageCmp()->SetTolPerPixel(0.10f);
    m_pFramework->GetImageCmp()->SetImgDeltaWght(1.00f);

	// Reset current texture being used
	SetTexture(0,NULL);

    // Do some special checks for the mipmap cases
    if (nGroup >= 4)
    {
        // If Dx7, check for the delay
        if (m_dwVersion == 0x0700)
        {
            if (bMipDelay)
            {
				if (bMipSkip)
		            TestFinish(D3DTESTFINISH_SKIPREMAINDER);
				else
		            TestFinish(D3DTESTFINISH_ABORT);
	            return false;
            }
        }
        // Check the MipCubeMap cap
        else
        {
		    // Check the caps we are interested in
		    DWORD dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;
		    DWORD dwCubeTextureFilterCaps = m_pSrcDevice->GetCaps()->dwCubeTextureFilterCaps;

		    if (!(dwTextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP))
		    {
			    // Do CubeTextureFilter cap consistency checking
			    if ((dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT) ||
				    (dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
			    {
	    		    WriteToLog("Device capability inconsistency: Texture MipCubeMap should be set.\n");
				    TestFinish(D3DTESTFINISH_ABORT);
				    return false;
			    }

			    WriteToLog("Device capability not supported: Texture MipCubeMap.\n");
                TestFinish(D3DTESTFINISH_SKIPREMAINDER);
			    return false;
		    }
        }

		// Setup custom image compare to avoid LOD approximations
		m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.20f);
		m_pFramework->GetImageCmp()->SetTolPerPixel(0.15f);
		m_pFramework->GetImageCmp()->SetImgDeltaWght(0.80f);
    }

    // Setup the filter mode
    Result = SetupFilterMode(nGroup);
    
    if (Result == D3DTESTINIT_SKIPALL)
    {
        SkipTests((UINT)(360.0f / fDegInc) * MIPMAP_LEVELS);
	    return false;
    }

    if (Result == D3DTESTINIT_ABORT)
    {
        TestFinish(D3DTESTFINISH_ABORT);
	    return false;
    }

	sprintf(msgString,"%sLevel %d, Pitch %.2f degrees, ",msgString,iLevel,fAngle);

	// Log the current cube format.
    FmtToString(FORMAT_TEXTURE,&m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat],szBuffer);
    sprintf(msgString, "%sTex: %s",msgString,szBuffer);

    // Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

	// Load images and set texture based on group
    if (nGroup <= 1)
	{
        // Use min cube texture
		if (!pCubeTexture->LoadImage(1, pMinImage))
		{
			WriteToLog("LoadImage(pMinImage) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}

		// Set the min texture
		if (!SetTexture(0,pCubeTexture))
		{
			WriteToLog("SetTexture(pCubeTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}

		// Tell log the texture size
		nLength = 16;
	}
    else if (nGroup <= 3)
    {
        // Use mag cube texture
		if (!pMagCubeTexture->LoadImage(1, pMagImage))
		{
			WriteToLog("LoadImage(pMagImage) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}

		// Set the mag texture
		if (!SetTexture(0,pMagCubeTexture))
		{
			WriteToLog("SetTexture(pMagCubeTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}

		// Tell log the texture size
		nLength = 256;
    }
    else
    {
        // Use mip cube texture
		if (!pMipCubeTexture->LoadImage(pppMipImages))
		{
			WriteToLog("LoadImage(pppMipImages) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}

		// Set the mip texture
		if (!SetTexture(0,pMipCubeTexture))
		{
			WriteToLog("SetTexture(pMipCubeTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}

		// Tell log the texture size
		nLength = 128;
    }

    // Inidicate how the texture was loaded
    switch (m_pSrcDevice->GetTexLoad())
    {    
        case TEXLOAD_UPDATETEXTURE:
        	WriteToLog("TexSize: %dx%d, TexLoad: UpdateTex\n",nLength,nLength);
        	sprintf(szStatus, "%s$aUpdateTex",szStatus);
            break;
        case TEXLOAD_COPYRECTS:
        	WriteToLog("TexSize: %dx%d, TexLoad: CopyRects\n",nLength,nLength);
        	sprintf(szStatus, "%s$aCopyRects",szStatus);
            break;
        case TEXLOAD_LOCKCOPY:
        	WriteToLog("TexSize: %dx%d, TexLoad: LockCopy\n",nLength,nLength);
        	sprintf(szStatus, "%s$aLockCopy",szStatus);
            break;
        case TEXLOAD_UPDATESURFACE:
        	WriteToLog("TexSize: %dx%d, TexLoad: UpdateSurf\n",nLength,nLength);
        	sprintf(szStatus, "%s$aUpdateSurf",szStatus);
            break;
        default:
		    WriteToLog("Error: Unknown texture load option (%x).\n",m_pSrcDevice->GetTexLoad());
        	sprintf(szStatus, "%s$rUnknown (%x)",szStatus,m_pSrcDevice->GetTexLoad());
            Fail();
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
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

	return true;
}

bool CCubeFilterTest::TestTerminate()
{
	int i,j;

    CCubeTest::TestTerminate();

    // Cleanup image data
    for (i=0; i<6 ;i++)
	{
		SAFEDELETE(pMinImage[i]);
		SAFEDELETE(pMagImage[i]);
	}

	// Cleanup mip image data
	if (pppMipImages)
	{
		for (i=0; i<MIPMAP_LEVELS; i++)
		{
			UINT nDepth = 6;
			for (j=0; j<nDepth; j++)
			{
				SAFEDELETE(pppMipImages[i][j]);
			}
			SAFEDELETEA(pppMipImages[i]);
		}
		SAFEDELETEA(pppMipImages);
	}

	// Cleanup textures
    RELEASE(pMagCubeTexture);
    RELEASE(pMipCubeTexture);

	return true;
}

UINT CCubeFilterTest::SetupFilterMode(int nMode)
{
    DWORD dwCubeTextureFilterCaps;

	// Check the caps we are interested in
    if (m_dwVersion >= 0x0800)
    	dwCubeTextureFilterCaps = m_pSrcDevice->GetCaps()->dwCubeTextureFilterCaps;
    else
    	dwCubeTextureFilterCaps = m_pSrcDevice->GetCaps()->dwTextureFilterCaps;

    switch (nMode)
    {
        case 0:
        case 2:
            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))       
            {
                WriteToLog("Device capability not supported: %sTexture Filter MinFLinear.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))       
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MagFPoint.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_ABORT;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
       	    sprintf(msgString,"%sFilter: LPN, ",msgString);
            break;
        case 1:
        case 3:
            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))       
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MinFPoint.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))       
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MagFLinear.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_SKIPALL;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
        	sprintf(msgString,"%sFilter: PLN, ",msgString);
            break;
        case 4:
            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT)) 
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MinFPoint.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))       
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MagFPoint.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))       
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MipFPoint.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_ABORT;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
        	sprintf(msgString,"%sFilter: PPP, ",msgString);
            break;
        case 5:
            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)) 
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MinFLinear.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))       
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MagFLinear.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))       
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MipFPoint.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_ABORT;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
        	sprintf(msgString,"%sFilter: LLP, ",msgString);
            break;
        case 6:
            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT)) 
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MinFPoint.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))       
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MagFPoint.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))       
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MipFLinear.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_SKIPALL;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);
        	sprintf(msgString,"%sFilter: PPL, ",msgString);
            break;
        case 7:
            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)) 
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MinFLinear.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))       
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MagFLinear.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))       
            {
	            WriteToLog("Device capability not supported: %sTexture Filter MipFLinear.\n",(m_dwVersion >= 0x0800) ? "Cube " : "");
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
