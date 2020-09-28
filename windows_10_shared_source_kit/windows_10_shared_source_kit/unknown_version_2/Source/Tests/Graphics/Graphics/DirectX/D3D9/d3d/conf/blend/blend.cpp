#define D3D_OVERLOADS
#include <d3d.h>
#include "Blend.h"

// Global Test and App instance
CD3DWindowFramework	App;
CDefaultTest		Default;
CDecalTest			Decal;
CModulateTest		Modulate;
CDecalAlphaTest		DecalAlpha;
CModulateAlphaTest	ModulateAlpha;
CCopyTest			Copy;
CAddTest			Add;
 
CBlendTest::CBlendTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);
    m_Options.D3DOptions.nBackBufferWidth = (DWORD) 160;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD) 140;
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE | PF_ALPHA;
	m_Options.ModeOptions.dwTexExclude = PF_METN;

	// Force test to use relevant DirectX versions
	m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_8_OR_81 | DXVERSION_7 | DXVERSION_6;

    // Init texture variable
    pTexture = NULL;

    // Init variables
    szTextureOp = "";
	szTextureOp2 = "";
}

CBlendTest::~CBlendTest()
{
}

UINT CBlendTest::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

    // Setup the Test range
	SetTestRange(1, MAX_TESTS * m_uCommonTextureFormats);

    // Check the caps we are interested in
	DWORD dwSrcBlendCaps = m_pSrcDevice->GetCaps()->dwSrcBlendCaps;
	DWORD dwDestBlendCaps = m_pSrcDevice->GetCaps()->dwDestBlendCaps;
	DWORD dwShadeCaps = m_pSrcDevice->GetCaps()->dwShadeCaps;

    if (m_dwVersion >= 0x0800)
    {
    	DWORD dwTextureOpCaps = m_pSrcDevice->GetCaps()->dwTextureOpCaps;

        // Check the appropriate Texture Op caps
	    if (!(dwTextureOpCaps & dwTextureOpCap))
	    {
		    WriteToLog("Device capability not supported: TextureOp %s.\n",szTextureOp);

            // These are required caps
            if ((dwBlendCap == D3DPTBLENDCAPS_DECAL) ||
                (dwBlendCap == D3DPTBLENDCAPS_MODULATE))
            {
		        return D3DTESTINIT_ABORT;
            }

            return D3DTESTINIT_SKIPALL;
	    }

        // If needed, check other Texture Op caps
        if (dwTextureOpCap2 && !(dwTextureOpCaps & dwTextureOpCap2))
	    {
		    WriteToLog("Device capability not supported: TextureOp %s.\n",szTextureOp2);

            // This is a required cap
            if (dwBlendCap == D3DPTBLENDCAPS_MODULATE) 
            {
		        return D3DTESTINIT_ABORT;
            }

		    return D3DTESTINIT_SKIPALL;
	    }
    }
    else // dwVersion <= 0x0700
    {
	    DWORD dwTextureBlendCaps = m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureBlendCaps;

        // Check the appropriate Blend caps
	    if (!(dwTextureBlendCaps & dwBlendCap))
	    {
		    WriteToLog("Device capability not supported: %s.\n",m_szTestName);

            // These are required caps
            if ((dwBlendCap == D3DPTBLENDCAPS_DECAL) ||
                (dwBlendCap == D3DPTBLENDCAPS_MODULATE))
            {
		        return D3DTESTINIT_ABORT;
            }

            return D3DTESTINIT_SKIPALL;
	    }
    }

	// Also check the Alpha Blending mode being used
	dwSrcAlphaCap		= (dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA);
	dwInvSrcAlphaCap	= (dwDestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA);
	dwAlphaGouraudCap	= (dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND);

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

    return D3DTESTINIT_RUN;
}

bool CBlendTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

bool CBlendTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	bool bAlphaTexture = false;
    bool bAlphaStripes = false;
	HRESULT Result = S_OK;
	int nTest = ((uTestNum-1) / m_uCommonTextureFormats) + 1;
    TCHAR szBuffer[80];
    FORMAT *pFormat;
 	dwVertexCount = 0;
	msgString[0] = '\0';
	szStatus[0] = '\0';

	// Cleanup previous texture
	SetTexture(0, NULL);
    RELEASE(pTexture);

	// Cycle the texture formats
	m_pCurrentMode->nTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;

    pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

	// Check the texture for an alpha channel
	if ((pFormat->ddpfPixelFormat.dwFlags & PF_ALPHAPIXELS) ||
        (pFormat->ddpfPixelFormat.dwFlags & PF_ALPHA))
		bAlphaTexture = true;

	// Set the default alpha blending renderstates
	SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	// Let's build some triangles.
	switch (nTest)
	{
		case (1): // Use no Alpha Blending
			sprintf(msgString, "%sAlpha (Tex): Default, ",msgString);
			DrawBlendingGrid(255);

            // Use Alpha Stripes
            bAlphaStripes = true;
			break;
		case (2): // Use texture Alpha Blending
			sprintf(msgString, "%sAlpha (Tex): SrcAlpha/InvSrcAlpha, ",msgString);
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
			sprintf(msgString, "%sAlpha (Mesh): SrcAlpha/InvSrcAlpha, ",msgString);
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

			// Make sure device can do interpolated mesh alpha
			if (!dwAlphaGouraudCap)
			{
				bool bAlphaSkip = false;

				if (m_dwVersion >= 0x0700)
				{
					if ((dwBlendCap == D3DPTBLENDCAPS_DECALALPHA)   || 
						(dwBlendCap == D3DPTBLENDCAPS_MODULATEALPHA)||
						(dwBlendCap == D3DPTBLENDCAPS_ADD))
					{
						bAlphaSkip = true;
					}
				}
				else
				{
					if (!((dwBlendCap == D3DPTBLENDCAPS_DECAL) ||
						  (dwBlendCap == D3DPTBLENDCAPS_COPY)  ||
						  ((dwBlendCap == D3DPTBLENDCAPS_MODULATE) && bAlphaTexture) ||
						  ((dwBlendCap == 0xffffffff) && bAlphaTexture)))
					{
						bAlphaSkip = true;
					}
				}

				if (bAlphaSkip)
				{
					WriteToLog("Device capability not supported: Shade - AlphaGouraudBlend.\n");

					if ((dwBlendCap == D3DPTBLENDCAPS_MODULATE) || (dwBlendCap == 0xffffffff))
						Skip();
					else
						SkipTests(m_uCommonTextureFormats);

					return false;
				}
			}

            // Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
			break;
		case (4): // Use mesh and texture Alpha Blending
			sprintf(msgString, "%sAlpha (Both): SrcAlpha/InvSrcAlpha, ",msgString);
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

			// Make sure device can do interpolated mesh alpha
			if (!dwAlphaGouraudCap)
			{
				bool bAlphaSkip = false;

				if (m_dwVersion >= 0x0700)
				{
					if ((dwBlendCap == D3DPTBLENDCAPS_DECALALPHA)   || 
						(dwBlendCap == D3DPTBLENDCAPS_MODULATEALPHA)||
						(dwBlendCap == D3DPTBLENDCAPS_ADD))
					{
						bAlphaSkip = true;
					}
				}
				else
				{
					if (!((dwBlendCap == D3DPTBLENDCAPS_DECAL) ||
						  (dwBlendCap == D3DPTBLENDCAPS_COPY)  ||
						  ((dwBlendCap == D3DPTBLENDCAPS_MODULATE) && bAlphaTexture) ||
						  ((dwBlendCap == 0xffffffff) && bAlphaTexture)))
					{
						bAlphaSkip = true;
					}
				}

				if (bAlphaSkip)
				{
					WriteToLog("Device capability not supported: Shade - AlphaGouraudBlend.\n");

					if ((dwBlendCap == D3DPTBLENDCAPS_MODULATE) || (dwBlendCap == 0xffffffff))
						Skip();
					else
						SkipTests(m_uCommonTextureFormats);

					return false;
				}
			}

            // Use Alpha Stripes
            bAlphaStripes = true;

            // Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
			break;
		default:
			Result = DrawBasicGrid(uTestNum,m_uCommonTextureFormats);
			break;
	}

    // Tell log what format we are using
    FmtToString(FORMAT_TEXTURE,pFormat,szBuffer);
    sprintf(msgString, "%sTex: %s",msgString,szBuffer);

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

	// Need to skip the test from within the test case
	if (FAILED(Result))
	{
		WriteToLog("Unable to use this format. CheckDeviceFormat(WrapAndMip) failed: %s.\n",m_pD3D->HResultToString(Result));
		Skip();
		return false;
	}

    // Create the texture
    CreateTexture(256,256,1,0,*pFormat,FMWK_DEFAULT,&pTexture);

	// Tell log about CreateTexture failure
	if (NULL == pTexture)
	{
		WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

	// Set the palette if needed
	if(pTexture->IsPalettized())
		pTexture->SetPalette(NULL);

    // Load appropriate stripe image into the texture
    if (!pTexture->LoadStripes(1,256,256,8,(bAlphaStripes)?AlphaStripes:Stripes,false))
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

	// Make an ubscure check with ValidateDevice
    if ((m_dwVersion >= 0x0700) && (dwBlendCap != 0xffffffff))
	{
		if (!SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)dwBlendState))
		{
			WriteToLog("ValidateDevice() failed with HResult: %s\n",m_pD3D->HResultToString(GetLastError()));
			Skip();
			return false;
		}
	}

    return true;
}

void CBlendTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Render the data.
	    RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

void CBlendTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what load method we are using
	WriteStatus("$wTexture Load",szStatus);
}

bool CBlendTest::TestTerminate()
{
	// Cleanup texture & image data
	SetTexture(0, NULL);
    RELEASE(pTexture);

	return true;
}
