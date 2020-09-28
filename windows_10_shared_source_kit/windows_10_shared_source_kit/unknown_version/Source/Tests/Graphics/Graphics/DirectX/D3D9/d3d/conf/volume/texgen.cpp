#include <d3d8.h>
#include <d3dx8.h>
#include "Volume.h"

CVolTexGenTest::CVolTexGenTest()
{
	m_szTestName = "Volume Texture Generation";
	m_szCommandKey = "TexGen";

	// Inidicate that we are a TexGen test
    bTexGen = true;

    // Initialize some variables
	pSingleVolTexture = NULL;

    // We only want one volume format
    if (m_Options.ModeOptions.VolumeFmt == FMT_ALL)
        m_Options.ModeOptions.VolumeFmt = FMT_ANY;
}

CVolTexGenTest::~CVolTexGenTest()
{
}

bool CVolTexGenTest::SetDefaultMatrices(void)
{
    D3DVECTOR MinZ = {0.0f,0.0f,3.0f};
    D3DXMATRIX Matrix, TransMatrix;

    if (!CVolumeTest::SetDefaultMatrices())
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

UINT CVolTexGenTest::TestInitialize(void)
{
    // Setup the Test range
	SetTestRange(1, MAX_TEXGEN_TESTS * 6);

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
 
	// Check for the TexGen cap
	DWORD dwVertexProcessingCaps = m_pSrcDevice->GetCaps()->dwVertexProcessingCaps;

    if (!(dwVertexProcessingCaps & D3DVTXPCAPS_TEXGEN))
    {
		WriteToLog("Device capability not supported: VertexProcessing TexGen.\n");
		return D3DTESTINIT_SKIPALL;
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

    // Load sphere vertices
	LoadSphere();

	// Color array and Image variables
	DWORD   dwColors[4];

	// Setup the color array
	dwColors[0] = 0xff00ff00;   // Green
	dwColors[1] = 0xff0000ff;	// Blue
	dwColors[2] = 0xffffffff;   // White
	dwColors[3] = 0xffff0000;	// Red

    // Set the Image depth, width, and height
    int i,j;
    UINT nImageDepth = 1;
    UINT nImageWidth = 256;
    UINT nImageHeight = 256;
    TCHAR szBuffer[80];
	bool bResult = true;
	szStatus[0] = '\0';

	// Log the current Volume format and load method.
    FmtToString(FORMAT_TEXTURE,&m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],szBuffer);
    WriteToLog("Tex: %s (UpdateTex).\n",szBuffer);
   	sprintf(szStatus, "%s$aUpdateTex",szStatus);

    // Create the gradient image data
    CImage * pImage = new CImage;
	if(pImage == NULL)
	{
    	WriteToLog("TestInitialize() - Out of memory.\n");
		return D3DTESTINIT_ABORT;
	}

    pImage->LoadGradient(nImageWidth,nImageHeight,dwColors); 

	// Create the volume texture
	bResult = CreateVolumeTexture(nImageWidth,nImageHeight,nImageDepth,1,0,m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],POOL_DEFAULT,&pSingleVolTexture);

	// Tell log about CreateVolumeTexture failure
    if (!bResult || NULL == pSingleVolTexture) 
	{
    	WriteToLog("CreateVolumeTexture(pSingleVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

	// Set the palette
	if (pSingleVolTexture->IsPalettized())
        pSingleVolTexture->SetPalette(NULL);

	// Load the images
	if (!pSingleVolTexture->LoadImage(1, &pImage, TEXLOAD_UPDATETEXTURE))
	{
		WriteToLog("LoadImage(pSingleVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

    // Cleanup image data
    SAFEDELETE(pImage); 

    // Setup the color array
	dwColors[0] = 0xff00ffff;   // Cyan
	dwColors[1] = 0xff707070;	// Grey
	dwColors[2] = 0xffffff00;   // Yellow
	dwColors[3] = 0xffff00ff;	// Magenta

    // Set the Image depth, width, and height
    nImageDepth = 4;
    nImageWidth = 8;
    nImageHeight = 8;

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

        ppImage[i]->LoadStripes(nImageWidth,nImageHeight,1,&dwColors[i],false); 
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

    return D3DTESTINIT_RUN;
}

bool CVolTexGenTest::ExecuteTest(UINT uTestNum)
{
 	// Initialize some variables
	int nTest = (uTestNum-1) / MAX_TEXGEN_TESTS;
	float fAngle = 30.0f * ((uTestNum-1) % MAX_TEXGEN_TESTS);  // Get angle for current test
	while (fAngle>=360.0f) { fAngle-=360.0f; }
    D3DXMATRIX Matrix, RotMatrix;
	msgString[0] = '\0';

    // Reset current texture being used
	SetTexture(0,NULL);

    // Tell log and status window about TextureTransform states
    switch (nTest)
    {
        case 0: sprintf(msgString,"%sCameraSpaceNormal, %.1f degrees, TexSize: 256x256x1",msgString,fAngle);
                break;
        case 1: sprintf(msgString,"%sCameraSpacePosition, %.1f degrees, TexSize: 256x256x1",msgString,fAngle);
                break;
        case 2: sprintf(msgString,"%sCameraSpaceReflectionVector, %.1f degrees, TexSize: 256x256x1",msgString,fAngle);
                break;
        case 3: sprintf(msgString,"%sCameraSpaceNormal, %.1f degrees, TexSize: 8x8x4",msgString,fAngle);
                break;
        case 4: sprintf(msgString,"%sCameraSpacePosition, %.1f degrees, TexSize: 8x8x4",msgString,fAngle);
                break;
        case 5: sprintf(msgString,"%sCameraSpaceReflectionVector, %.1f degrees, TexSize: 8x8x4",msgString,fAngle);
                break;
    }

    // Tell the log that we are starting
    BeginTestCase(msgString,uTestNum);

    // Setup the TextureTransform states
    switch (nTest)
    {
        case 0:
        case 3: SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,(DWORD)D3DTSS_TCI_CAMERASPACENORMAL);
                break;
        case 1:
        case 4: SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,(DWORD)D3DTSS_TCI_CAMERASPACEPOSITION);
                break;
        case 2: 
        case 5: SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,(DWORD)D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
                break;
    }

    SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,(DWORD)D3DTTFF_COUNT3);

	if (nTest < 3)
	{
	    // Set the valid single volume texture
		if (!SetTexture(0,pSingleVolTexture))
		{
			WriteToLog("SetTexture(pSingleVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
	}
	else
	{
	    // Set the valid volume texture
		if (!SetTexture(0,pVolTexture))
		{
			WriteToLog("SetTexture(pVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
	}

    // Setup the texture matrix for rotating 
    D3DXMatrixIdentity(&Matrix);
    D3DXMatrixRotationY(&RotMatrix, fAngle*pi/180);
    D3DXMatrixMultiply(&Matrix, &RotMatrix, &Matrix);
    if (!SetTransform(D3DTS_TEXTURE0, (D3DMATRIX *) &Matrix))
	{
        // Tell the logfile about the SetTransform failure
		WriteToLog("SetTransform(Texture0) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

	return true;
}

bool CVolTexGenTest::TestTerminate()
{
    CVolumeTest::TestTerminate();

    // Cleanup single texture
    RELEASE(pSingleVolTexture);

	return true;
}
