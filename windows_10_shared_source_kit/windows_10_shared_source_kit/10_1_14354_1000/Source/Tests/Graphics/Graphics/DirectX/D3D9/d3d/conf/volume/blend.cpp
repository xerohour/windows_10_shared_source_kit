#include <d3d8.h>
#include "Volume.h"

const int MAX_MESH = 5;

CVolBlendTest::CVolBlendTest()
{
	m_szTestName = "Volume Blend";
	m_szCommandKey = "Blend";

    // Initialize some variables
	pAlphaVolTexture = NULL;
}

CVolBlendTest::~CVolBlendTest()
{
}

bool CVolBlendTest::SetDefaultRenderStates(void)
{
	// Set the appropriate alpha blending renderstates
	SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
	SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

    return true;
}

UINT CVolBlendTest::TestInitialize(void)
{
    // Setup the Test range
	SetTestRange(1, MAX_BLEND_TESTS * MAX_MESH);

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

	// Reset some image compare variables to defaults
    m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.15f);
    m_pFramework->GetImageCmp()->SetTolPerPixel(0.10f);
    m_pFramework->GetImageCmp()->SetImgDeltaWght(1.00f);

    // Setup the colors for the Stripes array
	D3DCOLOR		Stripes[8];

	Stripes[0] = 0xffffffff;    // White
	Stripes[1] = 0xffff0000;	// Red
	Stripes[2] = 0xff00ff00;	// Green
	Stripes[3] = 0xff0000ff;	// Blue
	Stripes[4] = 0xffffff00;	// Yellow
	Stripes[5] = 0xffff00ff;	// Magenta
	Stripes[6] = 0xff00ffff;	// Cyan
	Stripes[7] = 0xff808080;    // Grey

    // Set the Image depth, width, and height
    int i,j;
    UINT nImageDepth = 8;
    UINT nImageWidth = 4;
    UINT nImageHeight = 4;
    TCHAR szBuffer[80];
	bool bResult = true;
	szStatus[0] = '\0';

	// Log the current Volume format and load method.
    FmtToString(FORMAT_TEXTURE,&m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],szBuffer);
    WriteToLog("Tex: %dx%dx%d, %s (UpdateTex).\n",nImageWidth,nImageHeight,nImageDepth,szBuffer);
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

        ppImage[i]->LoadStripes(nImageWidth,nImageHeight,1,&Stripes[i],false); 
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

    // Setup the colors for the AlphaStripes array
	D3DCOLOR		AlphaStripes[8];

	AlphaStripes[0] = 0x00ffffff;    // White
	AlphaStripes[1] = 0x20ff0000;	// Red
	AlphaStripes[2] = 0x4000ff00;	// Green
	AlphaStripes[3] = 0x7f0000ff;	// Blue
	AlphaStripes[4] = 0x80ffff00;	// Yellow
	AlphaStripes[5] = 0xc0ff00ff;	// Magenta
	AlphaStripes[6] = 0xe000ffff;	// Cyan
	AlphaStripes[7] = 0xff808080;    // Grey

    // Create the AlphaStripe image data
    CImage ** ppAlphaImage = new CImage*[nImageDepth];
	if(ppAlphaImage == NULL)
	{
    	WriteToLog("TestInitialize() - Out of memory.\n");
		return D3DTESTINIT_ABORT;
	}

    for (i=0; i<nImageDepth; i++)
    {
        ppAlphaImage[i] = new CImage;
		if(ppAlphaImage[i] == NULL)
		{
			WriteToLog("TestInitialize() - Out of memory.\n");
			return D3DTESTINIT_ABORT;
		}

        ppAlphaImage[i]->LoadStripes(nImageWidth,nImageHeight,1,&AlphaStripes[i],false); 
    }

	// Create the volume texture
	bResult = CreateVolumeTexture(nImageWidth,nImageHeight,nImageDepth,1,0,m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],POOL_DEFAULT,&pAlphaVolTexture);

	// Tell log about CreateVolumeTexture failure
    if (!bResult || NULL == pAlphaVolTexture) 
	{
    	WriteToLog("CreateVolumeTexture(pAlphaVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

	// Set the palette
	if (pAlphaVolTexture->IsPalettized())
        pAlphaVolTexture->SetPalette(NULL);

	// Load the alpha images
	if (!pAlphaVolTexture->LoadImage(1, ppAlphaImage, TEXLOAD_UPDATETEXTURE))
	{
		WriteToLog("LoadImage(pAlphaVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

    // Cleanup alpha image data
    for (i=0; i<nImageDepth; i++)
    {
       SAFEDELETE(ppAlphaImage[i]); 
    }
	SAFEDELETEA(ppAlphaImage);

    return D3DTESTINIT_RUN;
}

bool CVolBlendTest::ExecuteTest(UINT uTestNum)
{
 	// Initialize some variables
	int nTest = uTestNum - 1;
	int nBlendMode = nTest / MAX_MESH;
	msgString[0] = '\0';

    // Reset current texture being used
	SetTexture(0, NULL);

    // Setup our Blending mode
    if (!SetupBlendMode(nBlendMode))
    {
		SkipTests(MAX_MESH);
        return false;
    }

    // Load Blending grid vertices
    LoadBlendingGrid((nTest % MAX_MESH), (nBlendMode == 4));

    // Tell the log that we are starting
    BeginTestCase(msgString,uTestNum);

    // Determine which texture to use
    if (((nTest % MAX_MESH) == 0) ||
        ((nTest % MAX_MESH) == 2))
    {
        // Set the alpha texture
		if (!SetTexture(0,pAlphaVolTexture))
		{
			WriteToLog("SetTexture(pAlphaVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
    }
    else
    {
        // Set the valid texture
		if (!SetTexture(0,pVolTexture))
		{
			WriteToLog("SetTexture(pVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
    }

    return true;
}

bool CVolBlendTest::TestTerminate()
{
    CVolumeTest::TestTerminate();

    // Cleanup alpha texture
    RELEASE(pAlphaVolTexture);

    return true;
}

void CVolBlendTest::LoadBlendingGrid(int nMesh, bool bBlack)
{
    bool bAlpha;
    DWORD dwColor;

    switch (nMesh)
    {
        case 0:
            {   // Black with alpha
                if (bBlack)
                {
                    dwColor = 0xff000000;
            	    sprintf(msgString,"%sMesh: Black, Alpha: Both",msgString);
                }
                else // White with alpha
                {
                    dwColor = 0xffffffff; 
            	    sprintf(msgString,"%sMesh: White, Alpha: Both",msgString);
                }
                bAlpha = true;
            }
            break;
        case 1:
            {   // Red with alpha
                dwColor = 0xffff0000; 
            	sprintf(msgString,"%sMesh: Red, Alpha: Mesh",msgString);
                bAlpha = true;
            }
            break;
        case 2:
            {   // Black with no alpha
                if (bBlack)
                {
                    dwColor = 0xff000000;
            	    sprintf(msgString,"%sMesh: Black, Alpha: Tex",msgString);
                }
                else // White with no alpha
                {
                    dwColor = 0xffffffff; 
            	    sprintf(msgString,"%sMesh: White, Alpha: Tex",msgString);
                }                                             
                bAlpha = false;
            }
            break;
        case 3:
            {   // Red with no alpha
                dwColor = 0xffff0000; 
            	sprintf(msgString,"%sMesh: Red, Alpha: None",msgString);
                bAlpha = false;
            }
            break;
        case 4:
            {   // Black with no alpha
                if (bBlack)
                {
                    dwColor = 0xff000000;
            	    sprintf(msgString,"%sMesh: Black, Alpha: None",msgString);
                }
                else // White with no alpha
                {
                    dwColor = 0xffffffff; 
            	    sprintf(msgString,"%sMesh: White, Alpha: None",msgString);
                }
                bAlpha = false;
            }
            break;
    }

	VertexList[0].x = 9.5f;
	VertexList[0].y = 9.5f;
	VertexList[0].z = 0.9f;
	VertexList[0].rhw = 1.0f / 0.9f;
    VertexList[0].color = (bAlpha) ? 0x00ffffff & dwColor : dwColor;
	VertexList[0].tu = 0.0f;
	VertexList[0].tv = 0.0f;
	VertexList[0].tw = 0.0f;

	VertexList[1].x = 265.5f;
	VertexList[1].y = 9.5f;
	VertexList[1].z = 0.9f;
	VertexList[1].rhw = 1.0f / 0.9f;
	VertexList[1].color = dwColor;
	VertexList[1].tu = 1.0f;
	VertexList[1].tv = 0.0f;
	VertexList[1].tw = 0.0f;

	VertexList[2].x = 265.5f;
	VertexList[2].y = 265.5f;
	VertexList[2].z = 0.9f;
	VertexList[2].rhw = 1.0f / 0.9f;
	VertexList[2].color = dwColor;
	VertexList[2].tu = 1.0f;
	VertexList[2].tv = 1.0f;
	VertexList[2].tw = 1.0f;

	VertexList[3].x = 9.5f;
	VertexList[3].y = 9.5f;
	VertexList[3].z = 0.9f;
	VertexList[3].rhw = 1.0f / 0.9f;
    VertexList[3].color = (bAlpha) ? 0x00ffffff & dwColor : dwColor;
	VertexList[3].tu = 0.0f;
	VertexList[3].tv = 0.0f;
	VertexList[3].tw = 0.0f;

	VertexList[4].x = 265.5f;
	VertexList[4].y = 265.5f;
	VertexList[4].z = 0.9f;
	VertexList[4].rhw = 1.0f / 0.9f;
	VertexList[4].color = dwColor;
	VertexList[4].tu = 1.0f;
	VertexList[4].tv = 1.0f;
	VertexList[4].tw = 1.0f;

	VertexList[5].x = 9.5f;
	VertexList[5].y = 265.5f;
	VertexList[5].z = 0.9f;
	VertexList[5].rhw = 1.0f / 0.9f;
    VertexList[5].color = (bAlpha) ? 0x00ffffff & dwColor : dwColor;
	VertexList[5].tu = 0.0f;
	VertexList[5].tv = 1.0f;
	VertexList[5].tw = 1.0f;
}

bool CVolBlendTest::SetupBlendMode(int nMode)
{
	DWORD	dwPasses;

    switch (nMode)
    {
        case 0:
           	sprintf(msgString,"%sDecal, ",msgString);
            SetTextureStageState(0,D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
            SetTextureStageState(0,D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);

		    if (!ValidateDevice(&dwPasses))
		    {
    		    WriteToLog("ValidateDevice() failed: TextureOp SelectArg1, HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
				return false;
		    }

            break;
        case 1:
           	sprintf(msgString,"%sModulate, ",msgString);
            SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
            SetTextureStageState(0,D3DTSS_COLOROP,(DWORD)D3DTOP_MODULATE);
            SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_ALPHAOP,(DWORD)D3DTOP_SELECTARG1);

		    if (!ValidateDevice(&dwPasses))
		    {
    		    WriteToLog("ValidateDevice() failed: TextureOp Modulate, HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			    return false;
		    }

            break;
        case 2:
           	sprintf(msgString,"%sDecalAlpha, ",msgString);
            SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
            SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_BLENDTEXTUREALPHA);
            SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
            SetTextureStageState(0,D3DTSS_ALPHAOP,  (DWORD)D3DTOP_SELECTARG2);

		    if (!ValidateDevice(&dwPasses))
		    {
    		    WriteToLog("ValidateDevice() failed: TextureOp BlendTextureAlpha, HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			    return false;
		    }

            break;
        case 3:
           	sprintf(msgString,"%sModulateAlpha, ",msgString);
            SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
            SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_MODULATE);
            SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
            SetTextureStageState(0,D3DTSS_ALPHAOP,  (DWORD)D3DTOP_MODULATE);

		    if (!ValidateDevice(&dwPasses))
		    {
    		    WriteToLog("ValidateDevice() failed: TextureOp Modulate, HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			    return false;
		    }

            break;
        case 4:
           	sprintf(msgString,"%sAdd, ",msgString);
            SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
            SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_ADD);
            SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
            SetTextureStageState(0,D3DTSS_ALPHAOP,  (DWORD)D3DTOP_SELECTARG2);

		    if (!ValidateDevice(&dwPasses))
		    {
    		    WriteToLog("ValidateDevice() failed: TextureOp Add, HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			    return false;
		    }

            break;
    }

    return true;
}