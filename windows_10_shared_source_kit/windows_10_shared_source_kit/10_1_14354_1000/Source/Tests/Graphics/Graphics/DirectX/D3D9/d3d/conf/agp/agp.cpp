#define D3D_OVERLOADS
#include <d3d.h>
#include "AGP.h"

// Global Test and App instance
CD3DWindowFramework		App;
CTexture8x8Test			Texture8x8;
CTexture16x16Test		Texture16x16;
CTexture64x64Test		Texture64x64;
CTexture256x256Test		Texture256x256;
CTexture1024x1024Test	Texture1024x1024;

CAGPTest::CAGPTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);
    m_Options.D3DOptions.fMaxDXVersion = 7.0f;
    m_Options.D3DOptions.nBackBufferWidth = (DWORD) 160;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD) 140;

	// Request all texture formats except bumpmap
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE;

    // Init texture variable
    pTexture = NULL;
}

CAGPTest::~CAGPTest()
{
}

UINT CAGPTest::TestInitialize(void)
{
	UINT Result = D3DTESTINIT_RUN;

	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

    // Setup the Test range
	SetTestRange(1, MAX_TESTS * m_uCommonTextureFormats);

	// Make sure we are not on Dx8 or better
	if (m_dwVersion >= 0x0800)
	{
		WriteToLog("This test is not valid on Dx8 or greater.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Check the caps we are interested in
	DWORD dwDevCaps = m_pSrcDevice->GetCaps()->dwDevCaps;
	DWORD dwMinTextureWidth  = m_pSrcDevice->GetCaps()->dwMinTextureWidth;
	DWORD dwMinTextureHeight = m_pSrcDevice->GetCaps()->dwMinTextureHeight;
	DWORD dwMaxTextureWidth  = m_pSrcDevice->GetCaps()->dwMaxTextureWidth;
	DWORD dwMaxTextureHeight = m_pSrcDevice->GetCaps()->dwMaxTextureHeight;

	// Check the NonLocalVidMem cap for Hardware devices
	if (m_pSrcDevice->GetDevType() == DEVICETYPE_HAL)
	{
		if (!(dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM))
		{
			WriteToLog("Device capability not supported: TextureNonLocalVidMem.\n");
			return D3DTESTINIT_SKIPALL;
		}
	}

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

	// Setup the colors for the Stripes array
    Stripes[0] = RGBA_MAKE(255,255,255,255);// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);	// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);	// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);// Grey

    return Result;
}

bool CAGPTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

bool CAGPTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	int nTest = ((uTestNum-1) / m_uCommonTextureFormats) + 1;
    TCHAR szBuffer[80];
    FORMAT *pFormat;
	bool bResult;
 	dwVertexCount = 0;
	szStatus[0] = '\0';
	msgString[0] = '\0';

	// Cleanup previous texture
    SetTexture(0, NULL);
    RELEASE(pTexture);

	// Let's build some triangles.
	DrawGrid(nTest);

	// Cycle the common texture formats
	m_pCurrentMode->nTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;

    pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

    // Tell log what format and load method we are using
    FmtToString(FORMAT_TEXTURE,pFormat,szBuffer);
    sprintf(msgString, "%sTex: %s",msgString,szBuffer);

    // Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    // Create the texture
	if (m_pSrcDevice->GetDevType() == DEVICETYPE_HAL)
	{
		bUseAGP = true;
        bResult = CreateTexture(nTextureSize,nTextureSize,1,0,*pFormat,POOL_AGP,&pTexture);
	}
	else
	{
		bUseAGP = false;
        bResult = CreateTexture(nTextureSize,nTextureSize,1,0,*pFormat,POOL_SYSTEMMEM,&pTexture);
	}

	// Tell log about CreateTexture failure
	if (!bResult || (NULL == pTexture))
	{
		HRESULT hr = GetLastError();

        if (hr == DDERR_OUTOFVIDEOMEMORY)
		{
			WriteToLog("CreateTexture() failed with HResult = %s, skipping test.\n",m_pD3D->HResultToString(hr));
			SkipTests(1);
			return false;
        }

		WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
        Fail();
		return false;
	}

	// Set the palette if needed
	if (pTexture->IsPalettized())
		pTexture->SetPalette(NULL);

	// Load a stripe image into the texture
	if (!pTexture->LoadStripes(1,nTextureSize,nTextureSize,8,Stripes,false))
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

    // Set the valid texture
	if (!SetTexture(0,pTexture))
	{
		WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

    return true;
}

void CAGPTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

bool CAGPTest::ProcessFrame(void)
{
    SURFACEDESC Desc;

	// If using AGP memory, make sure our hardware texture is in AGP.
	if (bUseAGP)
	{
        pTexture->GetLevelDesc(0,&Desc);

		if (Desc.Pool != POOL_AGP)
		{
			WriteToLog("Texture Memory not in AGP, dwCaps are %x.\n",Desc.ddsCaps.dwCaps);

		    Fail();
            return false;
		}
	}

    return CD3DTest::ProcessFrame();
}

void CAGPTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what load method we are using
	WriteStatus("$wTexture Load",szStatus);
}

bool CAGPTest::TestTerminate()
{
	// Cleanup texture 
	SetTexture(0, NULL);
    RELEASE(pTexture);

	return true;
}

