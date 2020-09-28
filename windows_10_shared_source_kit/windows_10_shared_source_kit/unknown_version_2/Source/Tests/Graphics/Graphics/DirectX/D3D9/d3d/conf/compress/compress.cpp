#define D3D_OVERLOADS
#include <d3d.h>
#include "Compress.h"

#include "DXTSize.h"

// Global Test and App instance
CD3DWindowFramework	App;
CDXT1Test			DXT1;
CDXT2Test			DXT2;
CDXT3Test			DXT3;
CDXT4Test			DXT4;
CDXT5Test			DXT5;

//This test was added by blakep to catch a known bug in an S3 part
CDXTSizeTest		g_DXTSizeTest;
 
CCompressTest::CCompressTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);
    m_Options.D3DOptions.nBackBufferWidth = (DWORD) 160;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD) 140;
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_DXTN;

    // Init texture variable
    pTexture = NULL;
}

CCompressTest::~CCompressTest()
{
}

bool CCompressTest::SetDefaultRenderStates(void)
{
	// Turn off texture perspection
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	return true;
}

UINT CCompressTest::TestInitialize(void)
{
	UINT uTmp = 0;
	FORMAT *pTmp = NULL;

    // Check for the appropriate compressed texture format
	bool bValidFormat = CheckDXTnFormat((DWORD)FourCC);

    // Pair down to just RGB textures 
	if (!FilterFormats(m_pCommonTextureFormats, PF_RGB, 0, &pTmp, &uTmp))
	{
		WriteToLog("FilterFormats() failed for RGB textures.\n");
		SAFEDELETEA(pTmp);
		return D3DTESTINIT_ABORT;
	}
      
    // Reset the common list to our paired down list
	SAFEDELETEA(m_pCommonTextureFormats);
#if FAST_TEST
    m_pCommonTextureFormats = pTmp;
    m_uCommonTextureFormats = 1;
#else
    m_pCommonTextureFormats = pTmp;
    m_uCommonTextureFormats = uTmp;
#endif

	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No RGB texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

    // Setup the Test range
	SetTestRange(1, MAX_TESTS * m_uCommonTextureFormats);

	if (!bValidFormat)
	{
		WriteToLog("%s compressed texture format not supported.\n",m_szCommandKey);
		return D3DTESTINIT_SKIPALL;
	}

	// Check the caps we are interested in
	dwPerspectiveCap	= (m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE);
	dwSrcAlphaCap		= (m_pSrcDevice->GetCaps()->dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA);
	dwOneCap			= (m_pSrcDevice->GetCaps()->dwSrcBlendCaps & D3DPBLENDCAPS_ONE);
	dwInvSrcAlphaCap	= (m_pSrcDevice->GetCaps()->dwDestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA);

	// Setup the colors for the Stripes array
	Stripes[0] = RGBA_MAKE(255,255,255,255);// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);	// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);	// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);// Grey

	// Setup the colors for the AlphaStripes array
	AlphaStripes[0] = RGBA_MAKE(255,255,255,0);		// White
	AlphaStripes[1] = RGBA_MAKE(255,0,0,32);		// Red
	AlphaStripes[2] = RGBA_MAKE(0,255,0,64);		// Green
	AlphaStripes[3] = RGBA_MAKE(0,0,255,127);		// Blue
	AlphaStripes[4] = RGBA_MAKE(255,255,0,128);		// Yellow
	AlphaStripes[5] = RGBA_MAKE(255,0,255,176);		// Magenta
	AlphaStripes[6] = RGBA_MAKE(0,255,255,224);		// Cyan
	AlphaStripes[7] = RGBA_MAKE(128,128,128,255);	// Grey

	// Setup the colors for the AlphaWhite array
	AlphaWhite[0] = RGBA_MAKE(255,255,255,0);	// Full Alpha
	AlphaWhite[1] = RGBA_MAKE(255,255,255,85);	// 33% Alpha
	AlphaWhite[2] = RGBA_MAKE(255,255,255,170);	// 66% Alpha
	AlphaWhite[3] = RGBA_MAKE(255,255,255,255);	// No Alpha

	return D3DTESTINIT_RUN;
}

bool CCompressTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

bool CCompressTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	int nTest = ((uTestNum-1) / m_uCommonTextureFormats) + 1;
    bool bAlphaStripes = false;
    bool bAlphaWhite = false;
    bool bLoadResult = false;
    TCHAR szBuffer[80];
    FORMAT *pFormat;
	bool bResult;
    FORMAT DXTnFormat(FourCC);
 	dwVertexCount = 0;
	msgString[0] = '\0';
	szStatus[0] = '\0';

	// Cleanup previous textures
	SetTexture(0, NULL);
    RELEASE(pTexture);
	RELEASE(pDXTnTexture);
	RELEASE(pSysDXTnTexture);

	// Cycle the texture formats
	m_pCurrentMode->nTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;

    pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

	// Let's build some triangles.
	switch (nTest)
	{
		case (1): // Use no Alpha Blending
			sprintf(msgString, "%sAlpha (Tex): Default",msgString);
			DrawBlendingGrid(255);

            // Use Alpha Stripes
            bAlphaStripes = true;
			break;
		case (2): // Use texture Alpha Blending
			sprintf(msgString, "%sAlpha (Tex): SrcAlpha/InvSrcAlpha",msgString);
			DrawBlendingGrid(255);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

            // Use Alpha Stripes
            bAlphaStripes = true;

            // Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
			break;
		case (3): // Use mesh Alpha Blending
			sprintf(msgString, "%sAlpha (Mesh): SrcAlpha/InvSrcAlpha",msgString);
			DrawBlendingGrid(0);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
			break;
		case (4): // Use mesh and texture Alpha Blending
			sprintf(msgString, "%sAlpha (Both): SrcAlpha/InvSrcAlpha",msgString);
			DrawBlendingGrid(0);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

            // Use Alpha Stripes
            bAlphaStripes = true;

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
			break;
		case (5): // Use texture Alpha Blending
			sprintf(msgString, "%sAlpha (Tex 4x4): SrcAlpha/InvSrcAlpha",msgString);
			DrawBlendingGrid(255);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

            // Use Alpha White
            bAlphaWhite = true;

            // Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
			break;
		case (6): // Use texture Alpha Blending
			sprintf(msgString, "%sAlpha (Tex): One/InvSrcAlpha",msgString);
			DrawBlendingGrid(255);

			// Make sure device supports the proper alpha blending modes
			if (!dwOneCap || !dwInvSrcAlphaCap)
			{
				if (!dwOneCap)
					WriteToLog("Device capability not supported: SrcBlend - One.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

            // Use Alpha Stripes
            bAlphaStripes = true;

            // Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
			break;
		case (7): // Use mesh Alpha Blending
			sprintf(msgString, "%sAlpha (Mesh): One/InvSrcAlpha",msgString);
			DrawBlendingGrid(0);

			// Make sure device supports the proper alpha blending modes
			if (!dwOneCap || !dwInvSrcAlphaCap)
			{
				if (!dwOneCap)
					WriteToLog("Device capability not supported: SrcBlend - One.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
			break;
		case (8): // Use mesh and texture Alpha Blending
			sprintf(msgString, "%sAlpha (Both): One/InvSrcAlpha",msgString);
			DrawBlendingGrid(0);

			// Make sure device supports the proper alpha blending modes
			if (!dwOneCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - One.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

            // Use Alpha Stripes
            bAlphaStripes = true;

            // Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
			break;
		default:
			DrawBasicGrid(nTest-8,m_uCommonTextureFormats);

			// Set the default alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

			// Set the texture perspective renderstate
			if (nTest >= 15)
			{
				// Make sure device supports texture perspection
				if (!dwPerspectiveCap)
				{
					WriteToLog("Device capability not supported: Texture Perspective.\n");
					SkipTests(m_uCommonTextureFormats);
					return false;
				}

				SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)TRUE);
			}

            break;
	}

    // Tell log what format we are using
    FmtToString(FORMAT_TEXTURE,pFormat,szBuffer);
    sprintf(msgString, "%s, Tex: %s",msgString,szBuffer);

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    // Create the texture
    int nWidth = (bAlphaWhite) ? 4 : 256;
    int nHeight = (bAlphaWhite) ? 4 : 256;

    bResult = CreateTexture(nWidth,nHeight,1,0,*pFormat,POOL_SYSTEMMEM,&pTexture);

	if (!bResult || (NULL == pTexture))
	{
		WriteToLog("CreateTexture(pTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

    bResult = CreateTexture(nWidth,nHeight,1,0,DXTnFormat,FMWK_DEFAULT,&pDXTnTexture);

	if (!bResult || (NULL == pDXTnTexture))
	{
		WriteToLog("CreateTexture(pDXTnTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

    if (m_dwVersion >= 0x0800)
    {
        bResult = CreateTexture(nWidth,nHeight,1,0,DXTnFormat,POOL_SYSTEMMEM,&pSysDXTnTexture);

        if (!bResult || (NULL == pSysDXTnTexture))
	    {
		    WriteToLog("CreateTexture(pSysDXTnTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
            Fail();
		    return false;
	    }
    }

	// Set the palette if needed
	if (pTexture->IsPalettized())
		pTexture->SetPalette(NULL);

    // Load the appropriate image into the texture
    if (bAlphaWhite)
        bLoadResult = pTexture->LoadStripes(1,nWidth,nHeight,4,AlphaWhite,false);
    else if (bAlphaStripes)
        bLoadResult = pTexture->LoadStripes(1,nWidth,nHeight,8,AlphaStripes,false);
    else
        bLoadResult = pTexture->LoadStripes(1,nWidth,nHeight,8,Stripes,false);

    if (!bLoadResult)
	{
		WriteToLog("LoadStripes() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

    // Load from RGB surface to DXTn surface
    if (m_dwVersion < 0x0800)
    {
        LPSURFACES pDXTnSurface, pSurface;

        pDXTnTexture->GetSurfaceLevel(0, &pDXTnSurface);
        pTexture->GetSurfaceLevel(0, &pSurface);

        // Use Framework CopyRects....to get Blt()
        if (!CopyRects(pSurface,NULL,0,pDXTnSurface,NULL))
	    {
            WriteToLog("Blt() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
            RELEASE(pDXTnSurface);
            RELEASE(pSurface);
		    SkipTests(1);
		    return false;
	    }

		// Indicate which load option was used
        WriteToLog("TexLoad: CopyRects\n");
        sprintf(szStatus, "%s$aCopyRects",szStatus);

        RELEASE(pDXTnSurface);
        RELEASE(pSurface);
    }
    else
    {
        PALETTEENTRY *pPal = NULL;
        LPSURFACES pSysSurface, pSurface;

	    if (pTexture->IsPalettized())
        {
            PALETTEENTRY	Palette[256];
		    ZeroMemory(Palette, sizeof(PALETTEENTRY) * 256);

            if (!GetPaletteEntries(0, Palette))
            {
                WriteToLog("GetPaletteEntries() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
                Fail();
		        return false;
            }

            pPal = Palette;
        }

        pSysDXTnTexture->GetSurfaceLevel(0, &pSysSurface);
        pTexture->GetSurfaceLevel(0, &pSurface);

        if (!D3DXLoadSurfaceFromSurface(pSysSurface,NULL,NULL,pSurface,pPal,NULL,FILTER_POINT,0))
        {
            WriteToLog("D3DXLoadSurfaceFromSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
            RELEASE(pSysSurface);
            RELEASE(pSurface);
            Fail();
		    return false;
        }

        // Update the Source texture
        if (!UpdateTexture(pSysDXTnTexture, pDXTnTexture))
        {
            WriteToLog("UpdateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
            RELEASE(pSysSurface);
            RELEASE(pSurface);
            Fail();
		    return false;
        }

		// Indicate which load option was used
        WriteToLog("TexLoad: UpdateTex\n");
        sprintf(szStatus, "%s$aUpdateTex",szStatus);

        RELEASE(pSysSurface);
        RELEASE(pSurface);
    }

	// Tell the framework what texture to use
	if (!SetTexture(0,pDXTnTexture))
	{
		WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

    return true;
}

void CCompressTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

void CCompressTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what load method we are using
	WriteStatus("$wTexture Load",szStatus);
}

bool CCompressTest::TestTerminate()
{
	// Cleanup textures
	SetTexture(0, NULL);
    RELEASE(pTexture);
	RELEASE(pDXTnTexture);
	RELEASE(pSysDXTnTexture);

	return true;
}

bool CCompressTest::CheckDXTnFormat(DWORD dwDXTn)
{
	bool	bFoundDXTn = false;

	for (int i=0; !bFoundDXTn && i < m_uCommonTextureFormats; i++)
	{
        FORMAT *pFormat = &m_pCommonTextureFormats[i];

		if (pFormat->ddpfPixelFormat.dwFourCC == dwDXTn)
			bFoundDXTn = true;
	}

	return bFoundDXTn;
}

