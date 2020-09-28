//////////////////////////////////////////////////////////////////////
// Blend.cpp: implementation of the CCubeBlend class.
//////////////////////////////////////////////////////////////////////

#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "Cubemap.h"

#ifdef FAST_TEST
const int MAX_ANGLES = 3;
const int MAX_MATERIALS = 3;
const int BLEND_MODES = 5;
#else
const int MAX_ANGLES    = 6;
const int MAX_MATERIALS = 3;
const int BLEND_MODES   = 5;
#endif

CCubeBlendTest::CCubeBlendTest()
{
	m_szTestName = "Cube Blend";
	m_szCommandKey = "Blend";

 	// Set degree increase per test
	fDegInc = 135.0f; 
}

bool CCubeBlendTest::SetDefaultRenderStates(void)
{
	SetRenderState(D3DRS_AMBIENT,(DWORD)0xFFFFFFFF);

	// Set the appropriate alpha blending renderstates
	SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
	SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

    return true;
}

UINT CCubeBlendTest::TestInitialize(void)
{
    // Setup the Test range
	SetTestRange(1, MAX_ANGLES * MAX_MATERIALS * BLEND_MODES);

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

	// Reset some image compare variables to defaults
    m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.15f);
    m_pFramework->GetImageCmp()->SetTolPerPixel(0.10f);
    m_pFramework->GetImageCmp()->SetImgDeltaWght(1.00f);

    // Setup some variables 
	int i;
	D3DCOLOR Stripes[8];
	D3DCOLOR AlphaStripes[8];
    TCHAR szBuffer[80];
	bool bResult = true;
    UINT nImageLength = 64;

	// Setup the colors for the Stripes array
	Stripes[0] = RGBA_MAKE(255,255,255,255);	// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);		// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);		// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);		// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);		// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);		// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);		// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);	// Grey

	// Allocate/Load the CImages
	for (i=0; i<6; i++)
	{
		pImage[i] = new CImage();
		if (NULL == pImage[i])
		{
			WriteToLog("TestInitialize() - Out of memory.\n");
			return D3DTESTINIT_ABORT;
		}

		pImage[i]->LoadStripes(nImageLength,nImageLength,8,Stripes,false);
	}

    // Setup the colors for the AlphaStripes array
	AlphaStripes[0] = RGBA_MAKE(255,255,255,0);		// White
	AlphaStripes[1] = RGBA_MAKE(255,0,0,32);		// Red
	AlphaStripes[2] = RGBA_MAKE(0,255,0,64);		// Green
	AlphaStripes[3] = RGBA_MAKE(0,0,255,127);		// Blue
	AlphaStripes[4] = RGBA_MAKE(255,255,0,128);		// Yellow
	AlphaStripes[5] = RGBA_MAKE(255,0,255,176);		// Magenta
	AlphaStripes[6] = RGBA_MAKE(0,255,255,224);		// Cyan
	AlphaStripes[7] = RGBA_MAKE(128,128,128,255);	// Grey

	// Allocate/Load the CImages
	for (i=0; i<6; i++)
	{
		pAlphaImage[i] = new CImage();
		if (NULL == pAlphaImage[i])
		{
			WriteToLog("TestInitialize() - Out of memory.\n");
			return D3DTESTINIT_ABORT;
		}

		pAlphaImage[i]->LoadStripes(nImageLength,nImageLength,8,AlphaStripes,false);
	}

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

bool CCubeBlendTest::ExecuteTest(UINT uTestNum)
{
 	// Initialize some variables
	int nTest = uTestNum - 1;
    int nAngle = nTest % MAX_ANGLES;
	int nMaterial = nTest / MAX_ANGLES;
	int nBlendMode = nMaterial / MAX_MATERIALS;
	float fAngle = fDegInc * (float)nAngle;				// Get angle for current test
	while (fAngle>=360.0f) { fAngle-=360.0f; }
    D3DXMATRIX Matrix, RotMatrix, TransMatrix;
	msgString[0] = '\0';
	szStatus[0] = '\0';

	// Tweak the angle a touch to avoid not drawing anything for 1 bit alpha textures
	if ((fAngle == 315.0f) && (m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].d3dfFormat == FMT_A1R5G5B5 || 
							   m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].d3dfFormat == FMT_DXT1))
		fAngle = 300.f;

    if (!SetupBlendMode(nBlendMode))
    {
		SkipTests(MAX_ANGLES * MAX_MATERIALS);
        return false;
    }

    if (!ApplyMaterial((nMaterial % MAX_MATERIALS), (nBlendMode == 4)))
    {
  		TestFinish(D3DTESTFINISH_ABORT);
        return false;
    }

    sprintf(msgString,"%s%.2f degrees",msgString,fAngle);

    // Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    // Determine which image data to use and load
    if ((nMaterial % MAX_MATERIALS) == 0)
	{
		// Load the alpha images
		if (!pCubeTexture->LoadImage(1, pAlphaImage))
		{
			WriteToLog("LoadImage(pAlphaImage) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
	}
	else
	{
		// Load the images
		if (!pCubeTexture->LoadImage(1, pImage))
		{
			WriteToLog("LoadImage(pImage) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
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

    // Setup the world matrix for the translating/rotating sphere
    D3DXMatrixIdentity(&Matrix);
    D3DXMatrixTranslation(&TransMatrix, 0.0f, 0.0f, fZDepthMin);
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

bool CCubeBlendTest::TestTerminate()
{
    CCubeTest::TestTerminate();

	// Cleanup image data
    for (int i=0; i<6 ;i++)
	{
		SAFEDELETE(pImage[i]);
		SAFEDELETE(pAlphaImage[i]);
	}

	return true;
}

bool CCubeBlendTest::ApplyMaterial(int nMaterial, bool bBlack)
{
    // Release previous material
    SAFEDELETE(pMaterial);

	// Init the material
    pMaterial = new MATERIAL;
	memset(pMaterial, NULL, sizeof(MATERIAL));

    switch (nMaterial)
    {
        case 0:
            {   // Black with alpha
                if (bBlack)
                {
                    pMaterial->Diffuse.a = 0.5f;
	                pMaterial->Ambient.r = 0.0f;
	                pMaterial->Ambient.g = 0.0f;
	                pMaterial->Ambient.b = 0.0f;
            	    sprintf(msgString,"%sBlack (alpha=0.5), ",msgString);
                }
                else // White with alpha
                {
                    pMaterial->Diffuse.a = 0.5f;
	                pMaterial->Ambient.r = 1.0f;
	                pMaterial->Ambient.g = 1.0f;
	                pMaterial->Ambient.b = 1.0f;
            	    sprintf(msgString,"%sWhite (alpha=0.5), ",msgString);
                }
            }
            break;
        case 1:
            {   // Black with no alpha
                if (bBlack)
                {
                    pMaterial->Diffuse.a = 1.0f;
	                pMaterial->Ambient.r = 0.0f;
	                pMaterial->Ambient.g = 0.0f;
	                pMaterial->Ambient.b = 0.0f;
            	    sprintf(msgString,"%sBlack (alpha=1.0), ",msgString);
                }
                else // White with no alpha
                {
                    pMaterial->Diffuse.a = 1.0f;
	                pMaterial->Ambient.r = 1.0f;
	                pMaterial->Ambient.g = 1.0f;
	                pMaterial->Ambient.b = 1.0f;
            	    sprintf(msgString,"%sWhite (alpha=1.0), ",msgString);
                }
            }
            break;
        case 2:
            {   // Red with no alpha
                pMaterial->Diffuse.a = 1.0f;
	            pMaterial->Ambient.r = 1.0f;
	            pMaterial->Ambient.g = 0.0f;
	            pMaterial->Ambient.b = 0.0f;
            	sprintf(msgString,"%sRed (alpha=1.0), ",msgString);
            }
            break;
    }

	// Set the appropriate material
	SetMaterial(pMaterial);

    return true;
}

bool CCubeBlendTest::SetupBlendMode(int nMode)
{
    DWORD dwPasses = 0;

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
				SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_TEXTURE);
				SetTextureStageState(0,D3DTSS_COLOROP,(DWORD)D3DTOP_SELECTARG2);

			    if (!ValidateDevice(&dwPasses))
				{
	    		    WriteToLog("ValidateDevice() failed: ColorOp Texture, AlphaOp Texture, HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
					return false;
				}
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
				SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_TEXTURE);
				SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_CURRENT);
				SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_MODULATE);

				if (!ValidateDevice(&dwPasses))
				{
	    		    WriteToLog("ValidateDevice() failed: ColorOp Modulate, AlphaOp Texture, HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
					return false;
				}
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
	    		WriteToLog("ValidateDevice() failed: ColorOp BlendTextureAlpha, AlphaOp Current = %s.\n",m_pD3D->HResultToString(GetLastError()));
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
				SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_TEXTURE);
				SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_CURRENT);
				SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_MODULATE);
				SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_TEXTURE);
				SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_CURRENT);
				SetTextureStageState(0,D3DTSS_ALPHAOP,  (DWORD)D3DTOP_MODULATE);

				if (!ValidateDevice(&dwPasses))
				{
	    		    WriteToLog("ValidateDevice() failed: ColorOp Modulate, AlphaOp Modulate, HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
					return false;
				}
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
				SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_TEXTURE);
				SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_CURRENT);
				SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_ADD);

				if (!ValidateDevice(&dwPasses))
				{
	    		    WriteToLog("ValidateDevice() failed: ColorOp Add, AlphaOp Current, HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
					return false;
				}
			}

            break;
		default:
			WriteToLog("Unknown blend mode (%d).\n",nMode);
			return false;
    }

    return true;
}