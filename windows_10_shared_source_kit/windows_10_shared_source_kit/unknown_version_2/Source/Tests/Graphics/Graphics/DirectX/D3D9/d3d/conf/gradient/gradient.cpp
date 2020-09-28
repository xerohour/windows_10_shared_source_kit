#define D3D_OVERLOADS
#include <d3d.h>
#include "Gradient.h"

// Global Test and App instance
CD3DWindowFramework		App;
CTexture8x8Test			Texture8x8;
CTexture16x16Test		Texture16x16;
CTexture32x32Test		Texture32x32;
CTexture64x64Test		Texture64x64;
CTexture128x128Test		Texture128x128;
CTexture256x256Test		Texture256x256;
CTexture512x512Test		Texture512x512;
CTexture1024x1024Test	Texture1024x1024;

CGradientTest::CGradientTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);

    // Init variables
    pTexture = NULL;
}

CGradientTest::~CGradientTest()
{
}

UINT CGradientTest::TestInitialize(void)
{
	UINT Result = D3DTESTINIT_RUN;

	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

    // Setup the Test range
	SetTestRange(1, MAX_TESTS);
 
	// Check the caps we are interested in
	DWORD dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;
	DWORD dwMinTextureWidth  = m_pSrcDevice->GetCaps()->dwMinTextureWidth;
	DWORD dwMinTextureHeight = m_pSrcDevice->GetCaps()->dwMinTextureHeight;
	DWORD dwMaxTextureWidth  = m_pSrcDevice->GetCaps()->dwMaxTextureWidth;
	DWORD dwMaxTextureHeight = m_pSrcDevice->GetCaps()->dwMaxTextureHeight;
	DWORD dwMaxTextureRepeat = m_pSrcDevice->GetCaps()->dwMaxTextureRepeat;

	// Special check to make sure the texture size caps are filled out
	if ((dwMinTextureWidth == 0) && (dwMinTextureHeight == 0) &&
		(dwMaxTextureWidth == 0) && (dwMaxTextureHeight == 0))
	{
		// Log an error that we MUST set the texture size caps
		WriteToLog("Must set Min/Max Texture Width/Height caps.\n");
		return D3DTESTINIT_ABORT;
	}

	// See if the MinTextureWidth is supported
	if (nTextureSize < dwMinTextureWidth)
	{
		WriteToLog("Texture width (%d) < MinTextureWidth (%d)\n",nTextureSize,dwMinTextureWidth);
		Result = D3DTESTINIT_SKIPALL;
	}

	// See if the MinTextureHeight is supported
	if (nTextureSize < dwMinTextureHeight)
	{
		WriteToLog("Texture height (%d) < MinTextureHeight (%d)\n",nTextureSize,dwMinTextureHeight);
		Result = D3DTESTINIT_SKIPALL;
	}

	// See if the MaxTextureWidth is supported
	if (nTextureSize > dwMaxTextureWidth)
	{
		WriteToLog("Texture width (%d) > MaxTextureWidth (%d)\n",nTextureSize,dwMaxTextureWidth);
		Result = D3DTESTINIT_SKIPALL;
	}

	// See if the MaxTextureHeight is supported
	if (nTextureSize > dwMaxTextureHeight)
	{
		WriteToLog("Texture height (%d) > MaxTextureHeight (%d)\n",nTextureSize,dwMaxTextureHeight);
		Result = D3DTESTINIT_SKIPALL;
	}

    // We require support for at least 256x256 textures
    if (Result == D3DTESTINIT_SKIPALL)
    {
        if (nTextureSize <= 256)
            Result = D3DTESTINIT_ABORT;

        return Result;
    }

	// Init the Maximum Scale variable
	nMaxScale = 4;

	// See if we need to adjust the maximum scale value
	if (!(dwTextureCaps & D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE))
	{
		// Make sure MaxTextureRepeat cap is set
		if (!dwMaxTextureRepeat)
		{
			WriteToLog("Must set MaxTextureRepeat cap if TexRepeatNotScaledBySize not set.\n");
		}
		else if ((nTextureSize * 4) > dwMaxTextureRepeat)
		{
			nMaxScale = nTextureSize / (dwMaxTextureRepeat / 4);
		}
	}

	// Setup the colors for the Gradient array
	bool bResult;
	D3DCOLOR Gradient[4];

    Gradient[0] = RGBA_MAKE(0,0,0,255);		// Black
	Gradient[1] = RGBA_MAKE(255,0,0,255);	// Red
	Gradient[2] = RGBA_MAKE(0,255,0,255);	// Green
	Gradient[3] = RGBA_MAKE(255,255,0,255);	// Yellow

	// Pick the fist 16-bit texture enumerated
	for (int i=0; i < m_uCommonTextureFormats; i++)
	{
		if (m_pCommonTextureFormats[i].ddpfPixelFormat.dwRGBBitCount == 16)
		{
			m_pCurrentMode->nTextureFormat = i;
			break;
		}
	}

    // Create the texture
    bResult = CreateTexture(nTextureSize,nTextureSize,1,0,m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat],FMWK_DEFAULT,&pTexture);

	// Indicate a CreateTexture failure
	if (!bResult || (NULL == pTexture))
	{
		HRESULT hr = GetLastError();

		WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));

        if (hr == DDERR_OUTOFVIDEOMEMORY)
            return D3DTESTINIT_SKIPALL;
		else
            return D3DTESTINIT_ABORT;
	}

	// Set the palette if needed
	if (pTexture->IsPalettized())
		pTexture->SetPalette(NULL);

    // Load a gradient image into the texture
    if (!pTexture->LoadGradient(1,nTextureSize,nTextureSize,Gradient))
 	{
		WriteToLog("LoadGradient() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

    // Set the valid texture
	if (!SetTexture(0,pTexture))
	{
		WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

	return Result;
}

bool CGradientTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,0,255), 1.0f, 0);
}

bool CGradientTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	int nTest = ((uTestNum-1) / 1) + 1;
    TCHAR szBuffer[80];
 	dwVertexCount = 0;
	msgString[0] = '\0';
	szStatus[0] = '\0';

	// Let's build some triangles.
	DrawGrid(nTest);
 
    // Tell log what format and load method we are using
    FmtToString(FORMAT_TEXTURE,&m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat],szBuffer);
  	sprintf(msgString, "%sTex: %s",msgString,szBuffer);

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

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

    return true;
}

void CGradientTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

void CGradientTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what load method we are using
	WriteStatus("$wTexture Load",szStatus);
}

bool CGradientTest::TestTerminate()
{
	// Cleanup texture
	SetTexture(0, NULL);
    RELEASE(pTexture);

	return true;
}

 
