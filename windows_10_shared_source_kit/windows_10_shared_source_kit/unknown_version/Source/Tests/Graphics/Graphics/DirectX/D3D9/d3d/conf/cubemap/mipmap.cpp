//////////////////////////////////////////////////////////////////////
// Mipmaps.cpp: implementation of the CMipmaps class.
//////////////////////////////////////////////////////////////////////

#include <d3d8.h>
#include <d3dx8.h>
#include "Cubemap.h"


#ifdef FAST_TEST
const int MIPMAP_LEVELS = 2;
#else
const int MIPMAP_LEVELS = 5;
#endif

//************************************************************************
// MipCubeUT Test functions

CMipCubeUTTest::CMipCubeUTTest()
{
	m_szTestName = "Cube MipMap (UpdateTex)";
	m_szCommandKey = "MipMapUT";

	// Set degree increase per test
#ifdef FAST_TEST
	fDegInc = 135.0f;
#else
	fDegInc = 45.0f;
#endif

	nTestMax = 0;
	nLevelMax = 0;
	bPointFilter = false;

    dwLoadOption = TEXLOAD_UPDATETEXTURE;
}

//************************************************************************
// MipCubeCR Test functions

CMipCubeCRTest::CMipCubeCRTest()
{
	m_szTestName = "Cube MipMap (CopyRects)";
	m_szCommandKey = "MipMapCR";

    // Init Framework options
	m_Options.D3DOptions.fMaxDXVersion = 8.1f;

	// Set degree increase per test
#ifdef FAST_TEST
	fDegInc = 135.0f;
#else
	fDegInc = 45.0f;
#endif

	nTestMax = 0;
	nLevelMax = 0;
	bPointFilter = false;

    dwLoadOption = TEXLOAD_COPYRECTS;
}

//************************************************************************
// MipCubeLC Test functions

CMipCubeLCTest::CMipCubeLCTest()
{
	m_szTestName = "Cube MipMap (LockCopy)";
	m_szCommandKey = "MipMapLC";

	// Set degree increase per test
#ifdef FAST_TEST
	fDegInc = 135.0f;
#else
	fDegInc = 45.0f;
#endif

	nTestMax = 0;
	nLevelMax = 0;
	bPointFilter = false;

    dwLoadOption = TEXLOAD_LOCKCOPY;
}

//************************************************************************
// MipCubeUS Test functions

CMipCubeUSTest::CMipCubeUSTest()
{
	m_szTestName = "Cube MipMap (UpdateSurf)";
	m_szCommandKey = "MipMapUS";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Set degree increase per test
#ifdef FAST_TEST
	fDegInc = 135.0f;
#else
	fDegInc = 45.0f;
#endif

	nTestMax = 0;
	nLevelMax = 0;
	bPointFilter = false;

    dwLoadOption = TEXLOAD_UPDATESURFACE;
}

UINT CMipCubeTest::TestInitialize(void)
{
    // Setup the Test range
	nLevelMax = MIPMAP_LEVELS + 1;		// Total mipmap levels to test
	nTestMax = (int)(360.0f/fDegInc) * nLevelMax;	// Total tests
	SetTestRange(1, nTestMax);

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

    // On Dx8, check for MipCubeMap cap
    if (m_dwVersion >= 0x0800)
    {
		// Check the caps we are interested in
		DWORD dwCubeTextureFilterCaps = m_pSrcDevice->GetCaps()->dwCubeTextureFilterCaps;

		if (!(dwTextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP))
		{
			// Do CubeTextureFilter cap consistency checking
			if ((dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT) ||
				(dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
			{
	    		WriteToLog("Device capability inconsistency: Texture MipCubeMap should be set.\n");
				return D3DTESTINIT_ABORT;
			}

			WriteToLog("Device capability not supported: Texture MipCubeMap.\n");
			return D3DTESTINIT_SKIPALL;
		}
    }

    // Check the dynamic textures cap
	if (m_dwVersion >= 0x0800 && dwLoadOption == TEXLOAD_LOCKCOPY)
	{
		if (!(m_pSrcDevice->GetCaps()->dwCaps2 & D3DCAPS2_DYNAMICTEXTURES))
		{
			WriteToLog("Dynamic textures not supported.\n");
			return D3DTESTINIT_SKIPALL;
		}
	}

    // Check if any cube formats were available
	if (m_uCommonCubeFormats == 0) 
    {
		WriteToLog("No cube texture formats found.\n");
		return D3DTESTINIT_ABORT;
    }

	// On Dx9c, need to check for UsageQueryWrapAndMip
	if (m_pD3D->GetRuntimeVersion() >= 9.029f)
	{
		HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
											 USAGE_QUERY_WRAPANDMIP, RTYPE_CUBETEXTURE, m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat]);
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
											 USAGE_QUERY_FILTER, RTYPE_CUBETEXTURE, m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat])))
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

	// Setup the colors for the RedStripes array
	D3DCOLOR		RedStripes[7];

	RedStripes[0] = RGBA_MAKE(255,0,0,255);		// Red
	RedStripes[1] = RGBA_MAKE(255,255,255,255);	// White
	RedStripes[2] = RGBA_MAKE(255,0,0,255);		// Red
	RedStripes[3] = RGBA_MAKE(255,0,0,255);		// Red
	RedStripes[4] = RGBA_MAKE(255,0,0,255);		// Red
	RedStripes[5] = RGBA_MAKE(255,255,255,255);	// White
	RedStripes[6] = RGBA_MAKE(255,0,0,255);		// Red

	// Setup the colors for the GreenStripes array
	D3DCOLOR		GreenStripes[7];

	GreenStripes[0] = RGBA_MAKE(0,255,0,255);		// Green
	GreenStripes[1] = RGBA_MAKE(255,255,255,255);	// White
	GreenStripes[2] = RGBA_MAKE(0,255,0,255);		// Green
	GreenStripes[3] = RGBA_MAKE(0,255,0,255);		// Green
	GreenStripes[4] = RGBA_MAKE(0,255,0,255);		// Green
	GreenStripes[5] = RGBA_MAKE(255,255,255,255);	// White
	GreenStripes[6] = RGBA_MAKE(0,255,0,255);		// Green

	// Setup the colors for the BlueStripes array
	D3DCOLOR		BlueStripes[7];

	BlueStripes[0] = RGBA_MAKE(0,0,255,255);	// Blue
	BlueStripes[1] = RGBA_MAKE(255,255,255,255);// White
	BlueStripes[2] = RGBA_MAKE(0,0,255,255);	// Blue
	BlueStripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	BlueStripes[4] = RGBA_MAKE(0,0,255,255);	// Blue
	BlueStripes[5] = RGBA_MAKE(255,255,255,255);// White
	BlueStripes[6] = RGBA_MAKE(0,0,255,255);	// Blue

	// Setup the colors for the YellowStripes array
	D3DCOLOR		YellowStripes[7];

	YellowStripes[0] = RGBA_MAKE(255,255,0,255);  // Yellow
	YellowStripes[1] = RGBA_MAKE(255,255,255,255);// White
	YellowStripes[2] = RGBA_MAKE(255,255,0,255);  // Yellow
	YellowStripes[3] = RGBA_MAKE(255,255,0,255);  // Yellow
	YellowStripes[4] = RGBA_MAKE(255,255,0,255);  // Yellow
	YellowStripes[5] = RGBA_MAKE(255,255,255,255);// White
	YellowStripes[6] = RGBA_MAKE(255,255,0,255);  // Yellow

	// Setup the colors for the MagentaStripes array
	D3DCOLOR		MagentaStripes[7];

	MagentaStripes[0] = RGBA_MAKE(255,0,255,255);  // Magenta
	MagentaStripes[1] = RGBA_MAKE(255,255,255,255);// White
	MagentaStripes[2] = RGBA_MAKE(255,0,255,255);  // Magenta
	MagentaStripes[3] = RGBA_MAKE(255,0,255,255);  // Magenta
	MagentaStripes[4] = RGBA_MAKE(255,0,255,255);  // Magenta
	MagentaStripes[5] = RGBA_MAKE(255,255,255,255);// White
	MagentaStripes[6] = RGBA_MAKE(255,0,255,255);  // Magenta

	// Setup mip cube dimensions
	int i,j;
    UINT nMipLevels = MIPMAP_LEVELS;
    UINT nMipLength = 256;
    TCHAR szBuffer[80];
	bool bResult = true;
	DWORD Usage = FMWK_DEFAULT;

    // Create the stripe image data
	pppImages = new CImage**[nMipLevels];
	if (NULL == pppImages)
	{
		WriteToLog("TestInitialize() - Out of memory.\n");
		return D3DTESTINIT_ABORT;
	}
	
	D3DCOLOR* dwStripeIndex[] = { (D3DCOLOR*)RedStripes, (D3DCOLOR*)GreenStripes, 
								  (D3DCOLOR*)BlueStripes, (D3DCOLOR*)YellowStripes, (D3DCOLOR*)MagentaStripes};
	
	// Allocate/load the CImages
	for (i=0; i<nMipLevels; i++)
	{
		UINT nDepth = 6;
	
		pppImages[i] = new CImage*[nDepth];
		if (NULL == pppImages[i])
		{
			WriteToLog("TestInitialize() - Out of memory.\n");
			return D3DTESTINIT_ABORT;
		}
	
		for (j=0; j<nDepth; j++)
		{
                        UINT nLength = nMipLength / min((UINT)(float)pow(2.0f, i),nMipLength);
			pppImages[i][j] = new CImage();
			if (NULL == pppImages[i][j])
			{
				WriteToLog("TestInitialize() - Out of memory.\n");
				return D3DTESTINIT_ABORT;
			}
			pppImages[i][j]->LoadStripes(nLength,nLength,7,dwStripeIndex[i],false); 
		}
	}
	
	// Get the current cube format.
    FmtToString(FORMAT_TEXTURE,&m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat],szBuffer);

	// Make sure the Usage field is correct for the load method
	if (dwLoadOption == TEXLOAD_LOCKCOPY)
		Usage = USAGE_DYNAMIC;

	// Create the mipmap cube texture
	bResult = CreateCubeTexture(nMipLength, nMipLevels, Usage, m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat], FMWK_DEFAULT, &pCubeTexture);
    
	// Tell log about CreateCubeTexture failure
    if (!bResult || NULL == pCubeTexture) 
	{
		HRESULT hr = GetLastError();
		WriteToLog("CreateCubeTexture(%s) failed with HResult = %s.\n",szBuffer,m_pD3D->HResultToString(hr));

		// In Dx7 there was no MipCube cap, so consider E_NOTIMPL a skip
		if ((m_dwVersion == 0x0700) && (hr == E_NOTIMPL))
			return D3DTESTINIT_SKIPALL;
		else
			return D3DTESTINIT_ABORT;
	}

    // Set the palette if needed
	if (pCubeTexture->IsPalettized())
		pCubeTexture->SetPalette(NULL);

	// Load the images
	if (!pCubeTexture->LoadImage(pppImages, dwLoadOption))
	{
		WriteToLog("LoadImage() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

	// Log the current cube format and load method.
    switch (m_pSrcDevice->GetTexLoad())
    {    
        case TEXLOAD_UPDATETEXTURE:
            WriteToLog("Tex: %dx%d, %s (UpdateTex).\n",nMipLength,nMipLength,szBuffer);
            break;
        case TEXLOAD_COPYRECTS:
            WriteToLog("Tex: %dx%d, %s (CopyRects).\n",nMipLength,nMipLength,szBuffer);
            break;
        case TEXLOAD_LOCKCOPY:
            WriteToLog("Tex: %dx%d, %s (LockCopy).\n",nMipLength,nMipLength,szBuffer);
            break;
        case TEXLOAD_UPDATESURFACE:
            WriteToLog("Tex: %dx%d, %s (UpdateSurf).\n",nMipLength,nMipLength,szBuffer);
            break;
        default:
		    WriteToLog("Error: Unknown texture load option (%x).\n",m_pSrcDevice->GetTexLoad());
            return D3DTESTINIT_ABORT;
    }

    // Set the valid texture
	if (!SetTexture(0,pCubeTexture))
	{
		WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

    // Load sphere vertices
	LoadSphere();					

	return D3DTESTINIT_RUN;
}

bool CMipCubeTest::ExecuteTest(UINT uTestNum)
{
	int nTest = uTestNum - 1;
    D3DXMATRIX Matrix, RotMatrix, TransMatrix;
	float fAngle,fZDist;
	int iLevel;
	msgString[0] = '\0';
	szStatus[0] = '\0';

	// Get the Level
	iLevel=(int)((float)nTest/(float)nTestMax*(float)nLevelMax);

	// Get the Z distance of the current level
	fZDist=((float)iLevel*(fZDepthMax-fZDepthMin)/(float)(nLevelMax-1))+fZDepthMin;

	// Tweak the middle levels a touch to avoid precision problems
	if ((fZDist != fZDepthMin) && (fZDist != fZDepthMax))
		fZDist += 0.1f;

    D3DVECTOR ZVector = {0.0f,0.0f,fZDist};

	// Get the angle of the current test
	fAngle=fDegInc*(float)nTest;
	while (fAngle>=360.0f) { fAngle-=360.0f; }

    // Indicate what level we are rending at and sphere angle
    sprintf(msgString,"%sLevel %d, Pitch %.2f degrees",msgString,iLevel,fAngle);

    // Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    // Inidicate how the texture was loaded
    switch (m_pSrcDevice->GetTexLoad())
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
        	sprintf(szStatus, "%s$rUnknown (%x)",szStatus,m_pSrcDevice->GetTexLoad());
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

		// Set mip filter according to the Cube mip filter cap
		if (m_pSrcDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
			SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);
		else
			SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
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

bool CMipCubeTest::TestTerminate()
{
    CCubeTest::TestTerminate();

	// Cleanup mip image data
	if (pppImages)
	{
		for (int i=0; i<MIPMAP_LEVELS; i++)
		{
			UINT nDepth = 6;
			for (int j=0; j<nDepth; j++)
			{
				SAFEDELETE(pppImages[i][j]);
			}
			SAFEDELETEA(pppImages[i]);
		}
		SAFEDELETEA(pppImages);
	}

	return true;
}
