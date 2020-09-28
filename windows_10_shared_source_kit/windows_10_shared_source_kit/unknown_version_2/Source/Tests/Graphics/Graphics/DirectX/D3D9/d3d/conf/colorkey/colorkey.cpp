#define D3D_OVERLOADS
#include <d3d.h>
#include "ColorKey.h"

// Global Test and App instance
CD3DWindowFramework	App;
CDefaultTest		Default;
CWhiteTest			White;
CRedTest			Red;
CGreenTest			Green;
CBlueTest			Blue;
CYellowTest			Yellow;
CMagentaTest		Magenta;
CCyanTest			Cyan;
CGreyTest			Grey;
CBlackTest			Black;

CColorKeyTest::CColorKeyTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);
    m_Options.D3DOptions.fMaxDXVersion = 7.0f;
    m_Options.D3DOptions.nBackBufferWidth = (DWORD) 160;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD) 140;
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_LUMINANCE;
	m_Options.ModeOptions.dwTexExclude = PF_PALETTEINDEXED4 | PF_PALETTEINDEXED8; 

    // Set compare function to require 88%
	m_pFramework->GetImageCmp()->SetPassRequired(0.88f);

    // Init texture variable
    pTexture = NULL;

    // Init the default variable
	bDefault = false;
}

CColorKeyTest::~CColorKeyTest()
{
}

UINT CColorKeyTest::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

    // Setup the Test range
	if (bDefault)
		SetTestRange(1, m_uCommonTextureFormats);
	else
		SetTestRange(1, MAX_TESTS * m_uCommonTextureFormats);

	// Make sure we are not on Dx8 or better
	if (m_dwVersion >= 0x0800)
	{
		WriteToLog("This test is not valid on Dx8 or greater.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Check the caps we are interested in
	DWORD dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;

	// Check the appropriate Texture cap
	if (!(dwTextureCaps & D3DPTEXTURECAPS_TRANSPARENCY))
	{
		WriteToLog("Device capability not supported: Texture Transparency.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Setup the colors for the Stripes array
	Stripes[0] = RGBA_MAKE(255,255,255,255);// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);	// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);	// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);// Grey

	return D3DTESTINIT_RUN;
}

bool CColorKeyTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

bool CColorKeyTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	int nTest = ((uTestNum-1) / m_uCommonTextureFormats) + 1;
    TCHAR szBuffer[80];
    FORMAT *pFormat;
	bool bResult;
 	dwVertexCount = 0;
	msgString[0] = '\0';
  	szStatus[0]  = '\0';

	// Cleanup previous texture
	SetTexture(0, NULL);
    RELEASE(pTexture);

	if (bDefault)
	{
		sprintf(msgString, "%sBlend: Default, ", msgString);
		sprintf(msgString, "%sColorKey: Default, ", msgString);
	}
	else
	{
		switch (nTest)
		{
			case 1: // BlendEnable = TRUE, ColorKeyEnable = TRUE
					sprintf(msgString, "%sBlend: TRUE, ", msgString);
					sprintf(msgString, "%sColorKey: TRUE, ", msgString);

					SetRenderState(D3DRENDERSTATE_BLENDENABLE, (DWORD)TRUE);
					SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, (DWORD)TRUE);
					break;
			case 2: // BlendEnable = TRUE, ColorKeyEnable = FALSE
					sprintf(msgString, "%sBlend: TRUE, ", msgString);
					sprintf(msgString, "%sColorKey: FALSE, ", msgString);

					SetRenderState(D3DRENDERSTATE_BLENDENABLE, (DWORD)TRUE);
					SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, (DWORD)FALSE);
					break;
			case 3: // BlendEnable = FALSE, ColorKeyEnable = TRUE
					sprintf(msgString, "%sBlend: FALSE, ", msgString);
					sprintf(msgString, "%sColorKey: TRUE, ", msgString);

					SetRenderState(D3DRENDERSTATE_BLENDENABLE, (DWORD)FALSE);
					SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, (DWORD)TRUE);
					break;
			case 4: // BlendEnable = FALSE, ColorKeyEnable = FALSE
					sprintf(msgString, "%sBlend: FALSE, ", msgString);
					sprintf(msgString, "%sColorKey: FALSE, ", msgString);

					SetRenderState(D3DRENDERSTATE_BLENDENABLE, (DWORD)FALSE);
					SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, (DWORD)FALSE);
					break;
			case 5: // ColorKeyEnable = TRUE, BlendEnable = TRUE
					sprintf(msgString, "%sColorKey: TRUE, ", msgString);
					sprintf(msgString, "%sBlend: TRUE, ", msgString);

					SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, (DWORD)TRUE);
					SetRenderState(D3DRENDERSTATE_BLENDENABLE, (DWORD)TRUE);
					break;
			case 6: // ColorKeyEnable = TRUE, BlendEnable = FALSE
					sprintf(msgString, "%sColorKey: TRUE, ", msgString);
					sprintf(msgString, "%sBlend: FALSE, ", msgString);

					SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, (DWORD)TRUE);
					SetRenderState(D3DRENDERSTATE_BLENDENABLE, (DWORD)FALSE);
					break;
			case 7: // ColorKeyEnable = FALSE, BlendEnable = TRUE
					sprintf(msgString, "%sColorKey: FALSE, ", msgString);
					sprintf(msgString, "%sBlend: TRUE, ", msgString);

					SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, (DWORD)FALSE);
					SetRenderState(D3DRENDERSTATE_BLENDENABLE, (DWORD)TRUE);
					break;
			case 8: // ColorKeyEnable = FALSE, BlendEnable = FALSE
					sprintf(msgString, "%sColorKey: FALSE, ", msgString);
					sprintf(msgString, "%sBlend: FALSE, ", msgString);

					SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, (DWORD)FALSE);
					SetRenderState(D3DRENDERSTATE_BLENDENABLE, (DWORD)FALSE);
					break;
		}
	}

	// Let's build some triangles.
	DrawGrid(nTest);

	// Cycle the texture formats
	m_pCurrentMode->nTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;

    pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

    // Tell log what format we are using
    FmtToString(FORMAT_TEXTURE,pFormat,szBuffer);
    sprintf(msgString, "%sTex: %s",msgString,szBuffer);

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    // Create the texture
    bResult = CreateTexture(64,64,1,0,*pFormat,FMWK_DEFAULT,&pTexture);

	// Tell log about CreateTexture failure
	if (!bResult || (NULL == pTexture))
	{
		WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

	// Set the palette if needed
	if (pTexture->IsPalettized())
		pTexture->SetPalette(NULL);

    // Load a stripe image into the texture
    if (!pTexture->LoadStripes(1,64,64,8,Stripes,false))
	{
		WriteToLog("LoadStripes() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

	// Indicate which load option was used
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

    // Set the ColorKey value
	if (!pTexture->SetColorKey(DDCKEY_SRCBLT,&dwColorKey))
	{
		WriteToLog("SetColorKey() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

    // Set the valid texture
	if (!SetTexture(0,pTexture))
	{
		WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

    return true;
}

void CColorKeyTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

void CColorKeyTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what load method we are using
	WriteStatus("$wTexture Load",szStatus);
}

bool CColorKeyTest::TestTerminate()
{
	// Cleanup texture data
	SetTexture(0, NULL);
    RELEASE(pTexture);

	return true;
}
