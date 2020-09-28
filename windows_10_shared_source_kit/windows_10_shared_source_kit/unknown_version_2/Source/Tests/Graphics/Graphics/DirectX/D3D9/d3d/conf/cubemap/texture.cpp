//////////////////////////////////////////////////////////////////////
// Texture.cpp: implementation of the texture classes.
//////////////////////////////////////////////////////////////////////

#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "Cubemap.h"

//************************************************************************
// CubeUpdateTex Test functions

CCubeUpdateTexTest::CCubeUpdateTexTest()
{
	m_szTestName = "Cube Texture (UpdateTex)";
	m_szCommandKey = "UpdateTex";

	// Set degree increase per test
#ifdef FAST_TEST
	fDegInc = 135.0f;
#else
	fDegInc = 15.0f;
#endif

    dwLoadOption = TEXLOAD_UPDATETEXTURE;
}

//************************************************************************
// CubeCopyRects Test functions

CCubeCopyRectsTest::CCubeCopyRectsTest()
{
	m_szTestName = "Cube Texture (CopyRects)";
	m_szCommandKey = "CopyRects";

    // Init Framework options
	m_Options.D3DOptions.fMaxDXVersion = 8.1f;

	// Set degree increase per test
#ifdef FAST_TEST
	fDegInc = 135.0f;
#else
	fDegInc = 15.0f;
#endif

    dwLoadOption = TEXLOAD_COPYRECTS;
}

//************************************************************************
// CubeLockCopy Test functions

CCubeLockCopyTest::CCubeLockCopyTest()
{
	m_szTestName = "Cube Texture (LockCopy)";
	m_szCommandKey = "LockCopy";

	// Set degree increase per test
#ifdef FAST_TEST
	fDegInc = 135.0f;
#else
	fDegInc = 15.0f;
#endif

    dwLoadOption = TEXLOAD_LOCKCOPY;
}

//************************************************************************
// CubeUpdateSurf Test functions

CCubeUpdateSurfTest::CCubeUpdateSurfTest()
{
	m_szTestName = "Cube Texture (UpdateSurf)";
	m_szCommandKey = "UpdateSurf";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Set degree increase per test
#ifdef FAST_TEST
	fDegInc = 135.0f;
#else
	fDegInc = 15.0f;
#endif

    dwLoadOption = TEXLOAD_UPDATESURFACE;
}

UINT CCubeTexTest::TestInitialize(void)
{
    // Setup the Test range
	SetTestRange(1, (int)(360.0f/fDegInc));

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

	// Reset some image compare variables to defaults
    m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.15f);
    m_pFramework->GetImageCmp()->SetTolPerPixel(0.10f);
    m_pFramework->GetImageCmp()->SetImgDeltaWght(1.00f);

    // Setup some variables 
	int i;
	DWORD dwColors[4];
    TCHAR szBuffer[80];
	bool bResult = true;
    UINT nImageLength = 256;
	DWORD Usage = FMWK_DEFAULT;

	// Allocate the CImages
	for (i=0; i<6; i++)
	{
		pImage[i] = new CImage();
		if (NULL == pImage[i])
		{
			WriteToLog("TestInitialize() - Out of memory.\n");
			return D3DTESTINIT_ABORT;
		}
	}

	// Setup the color array
    dwColors[0]=0xFF0000FF;
	dwColors[1]=0xFF00FF00;
	dwColors[2]=0xFF00FFFF;
	dwColors[3]=0xFFFF0000;
	pImage[0]->LoadGradient(nImageLength,nImageLength,dwColors);
	dwColors[0]=0xFFFF00FF;
	dwColors[1]=0xFFFFFF00;
	dwColors[2]=0xFFFFFFFF;
	dwColors[3]=0xFF0000FF;
	pImage[1]->LoadGradient(nImageLength,nImageLength,dwColors);
	dwColors[0]=0xFF00FF00;
	dwColors[1]=0xFF00FFFF;
	dwColors[2]=0xFFFF0000;
	dwColors[3]=0xFFFF00FF;
	pImage[2]->LoadGradient(nImageLength,nImageLength,dwColors);
	dwColors[0]=0xFFFFFF00;
	dwColors[1]=0xFFFFFFFF;
	dwColors[2]=0xFF0000FF;
	dwColors[3]=0xFF00FF00;
	pImage[3]->LoadGradient(nImageLength,nImageLength,dwColors);
	dwColors[0]=0xFF00FFFF;
	dwColors[1]=0xFFFF0000;
	dwColors[2]=0xFFFF00FF;
	dwColors[3]=0xFFFFFF00;
	pImage[4]->LoadGradient(nImageLength,nImageLength,dwColors);
	dwColors[0]=0xFFFFFFFF;
	dwColors[1]=0xFF0000FF;
	dwColors[2]=0xFF00FF00;
	dwColors[3]=0xFF00FFFF;
	pImage[5]->LoadGradient(nImageLength,nImageLength,dwColors);

	// Make sure the Usage field is correct for the load method
	if (dwLoadOption == TEXLOAD_LOCKCOPY)
		Usage = USAGE_DYNAMIC;

	// Get the current cube format.
    FmtToString(FORMAT_TEXTURE,&m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat],szBuffer);

	// Create the cube map texture
	bResult = CreateCubeTexture(nImageLength, 1, Usage, m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat], FMWK_DEFAULT, &pCubeTexture);

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

	// Load the images
	if (!pCubeTexture->LoadImage(1, pImage, dwLoadOption))
	{
		WriteToLog("LoadImage() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}
	
	// Log the current cube format and load method.
    FmtToString(FORMAT_TEXTURE,&m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat],szBuffer);

    switch (m_pSrcDevice->GetTexLoad())
    {    
        case TEXLOAD_UPDATETEXTURE:
            WriteToLog("Tex: %dx%d, %s (UpdateTex).\n",nImageLength,nImageLength,szBuffer);
            break;
        case TEXLOAD_COPYRECTS:
            WriteToLog("Tex: %dx%d, %s (CopyRects).\n",nImageLength,nImageLength,szBuffer);
            break;
        case TEXLOAD_LOCKCOPY:
            WriteToLog("Tex: %dx%d, %s (LockCopy).\n",nImageLength,nImageLength,szBuffer);
            break;
        case TEXLOAD_UPDATESURFACE:
            WriteToLog("Tex: %dx%d, %s (UpdateSurf).\n",nImageLength,nImageLength,szBuffer);
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

bool CCubeTexTest::ExecuteTest(UINT uTestNum)
{
 	// Initialize some variables
	int nTest = uTestNum - 1;
	float fAngle = fDegInc * (float)nTest;				// Get angle for current test
    D3DXMATRIX Matrix, RotMatrix, TransMatrix;
	msgString[0] = '\0';
	szStatus[0] = '\0';

	// Tell log what our angle is
    sprintf(msgString,"%s%.2f degrees",msgString,fAngle);

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

    // Setup the world matrix for the translating/rotating sphere
    D3DXMatrixIdentity(&Matrix);
    D3DXMatrixTranslation(&TransMatrix, 0.0f, 0.0f, fZDepthMin);
    D3DXMatrixMultiply(&Matrix, &TransMatrix, &Matrix);
    D3DXMatrixRotationYawPitchRoll(&RotMatrix, fAngle*pi/180, fAngle*pi/180, 0);
    D3DXMatrixMultiply(&Matrix, &RotMatrix, &Matrix);
    if (!SetTransform(D3DTS_WORLD, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

	return true;
}

bool CCubeTexTest::TestTerminate()
{
    CCubeTest::TestTerminate();

	// Cleanup image data
    for (int i=0; i<6 ;i++)
	{
		SAFEDELETE(pImage[i]);
	}

	return true;
}