//////////////////////////////////////////////////////////////////////
// TexGen.cpp: implementation of the CTexGen class.
//////////////////////////////////////////////////////////////////////

#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "Cubemap.h"

CCubeTexGenTest::CCubeTexGenTest()
{
	m_szTestName = "Cube Texture Generation";
	m_szCommandKey = "TexGen";

	// Inidicate that we are a TexGen test
    bTexGen = true;

	// Set degree increase per test
#ifdef FAST_TEST
	fDegInc = 135.0f;
#else
	fDegInc = 30.0f;
#endif

    // We only want one cube format
    if (m_Options.ModeOptions.CubeFmt == FMT_ALL)
        m_Options.ModeOptions.CubeFmt = FMT_ANY;

	m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_BESTAVAILABLE | DEVICECREATE_SWVP;
}

bool CCubeTexGenTest::SetDefaultMatrices(void)
{
    D3DVECTOR MinZ = {0.0f,0.0f,3.0f};
    D3DXMATRIX Matrix, TransMatrix;

    if (!CCubeTest::SetDefaultMatrices())
        return false;

    // Translate sphere to min depth
    D3DXMatrixIdentity(&Matrix);
    D3DXMatrixTranslation(&TransMatrix, MinZ.x, MinZ.y, MinZ.z);
    D3DXMatrixMultiply(&Matrix, &TransMatrix, &Matrix);
    if (!SetTransform(D3DTS_WORLD, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

UINT CCubeTexGenTest::TestInitialize(void)
{
    // Setup the Test range
	SetTestRange(1, (int)(360.0f/fDegInc) * 3);

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

	// Check for the TexGen cap
	DWORD dwVertexProcessingCaps = m_pSrcDevice->GetCaps()->dwVertexProcessingCaps;

    if (!(dwVertexProcessingCaps & D3DVTXPCAPS_TEXGEN))
    {
		WriteToLog("Device capability not supported: VertexProcessing TexGen.\n");
		return D3DTESTINIT_SKIPALL;
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
	DWORD dwColors[4];
    TCHAR szBuffer[80];
	bool bResult = true;
    UINT nImageLength = 256;

	// Allocate the CImages
	for (int i=0; i<6; i++)
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

	// Get the current cube format.
    FmtToString(FORMAT_TEXTURE,&m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat],szBuffer);

	// Create the cube map texture
	bResult = CreateCubeTexture(nImageLength, 1, 0, m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat], FMWK_DEFAULT, &pCubeTexture);

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

    // Load sphere vertices
	LoadSphere();					

    return D3DTESTINIT_RUN;
}

bool CCubeTexGenTest::ExecuteTest(UINT uTestNum)
{
 	// Initialize some variables
	int nTest = (uTestNum-1) % (int)(360.0f/fDegInc);
	float fAngle = fDegInc * (float)nTest;				// Get angle for current test
	while (fAngle>=360.0f) { fAngle-=360.0f; }
    D3DXMATRIX Matrix, RotMatrix;
    TCHAR szBuffer[80];
	msgString[0] = '\0';
	szStatus[0] = '\0';

	// Setup the TSS based on test case
    switch ((uTestNum-1) / (int)(360.0f/fDegInc))
    {
        case 0: SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,(DWORD)D3DTSS_TCI_CAMERASPACENORMAL);
            	sprintf(msgString,"%sCameraSpaceNormal, ",msgString);
                break;
        case 1: SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,(DWORD)D3DTSS_TCI_CAMERASPACEPOSITION);
            	sprintf(msgString,"%sCameraSpacePosition, ",msgString);
                break;
        case 2: SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,(DWORD)D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
            	sprintf(msgString,"%sCameraSpaceReflectionVector, ",msgString);
                break;
    }

    SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,(DWORD)D3DTTFF_COUNT3);

	// Tell log what our angle is
	sprintf(msgString,"%s%.2f degrees, ",msgString,fAngle);

	// Log the current cube format.
    FmtToString(FORMAT_TEXTURE,&m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat],szBuffer);
    sprintf(msgString, "%sTex: %s",msgString,szBuffer);

    // Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

	// Load the images
	if (!pCubeTexture->LoadImage(1, pImage))
	{
		WriteToLog("LoadImage() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}
	
    // Inidicate how the texture was loaded
    switch (m_pSrcDevice->GetTexLoad())
    {    
        case TEXLOAD_UPDATETEXTURE:
        	WriteToLog("TexLoad: UpdateTex\n");
        	sprintf(szStatus, "%s$aUpdateTex",szStatus);
            break;
        case TEXLOAD_COPYRECTS:
        	WriteToLog("TexLoad: CopyRects\n");
        	sprintf(szStatus, "%s$aCopyRects",szStatus);
            break;
        case TEXLOAD_LOCKCOPY:
        	WriteToLog("TexLoad: LockCopy\n");
        	sprintf(szStatus, "%s$aLockCopy",szStatus);
            break;
        case TEXLOAD_UPDATESURFACE:
        	WriteToLog("TexLoad: UpdateSurf\n");
        	sprintf(szStatus, "%s$aUpdateSurf",szStatus);
            break;
        default:
		    WriteToLog("Error: Unknown texture load option (%x).\n",m_pSrcDevice->GetTexLoad());
        	sprintf(szStatus, "%s$rUnknown (%x)",szStatus,m_pSrcDevice->GetTexLoad());
            Fail();
		    return false;
    }

    // Set the valid texture
	if (!SetTexture(0,pCubeTexture))
	{
		WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

    // Setup the texture matrix for the rotating
    D3DXMatrixIdentity(&Matrix);
    D3DXMatrixRotationY(&RotMatrix, fAngle*pi/180);
    D3DXMatrixMultiply(&Matrix, &RotMatrix, &Matrix);
    if (!SetTransform(D3DTS_TEXTURE0, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(Texture0) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

	return true;
}

bool CCubeTexGenTest::TestTerminate()
{
    CCubeTest::TestTerminate();

	// Cleanup image data
    for (int i=0; i<6 ;i++)
	{
		SAFEDELETE(pImage[i]);
	}

	return true;
}
