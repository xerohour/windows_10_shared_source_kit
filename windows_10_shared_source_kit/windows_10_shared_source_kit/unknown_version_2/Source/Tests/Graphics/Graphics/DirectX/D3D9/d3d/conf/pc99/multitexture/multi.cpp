// FILE:        multi.cpp
// DESC:        WLP multi texturing conformance test (WLP multi texturing blend modes)

#define D3D_OVERLOADS
#include <d3d.h>
#include <d3d9types.h>
#include "multi.h"

CD3DWindowFramework App;
CMulti              Test;

BLENDMODE gpModes[] =
{
    { "Modulate RGB",				MODULATE_RGB   },
    { "Modulate Alpha",				MODULATE_ALPHA },
    { "Add",						ADD            },
    { "Blend Factor Alpha",			BLEND_REGISTER },
    { "Blend Diffuse Alpha",		BLEND_DIFFUSE  },
	{ "Modulate Alpha add Color",	MODULATE_ALPHA_ADDCOLOR }
};

MYCOLOR gpColors[] =
{
    { "gray",    WLP_GRAY    },
    { "yellow",  WLP_YELLOW  },
    { "cyan",    WLP_CYAN    },
    { "magenta", WLP_MAGENTA },
    { "white",   WLP_WHITE   }
};

// NAME:        CMulti()
// DESC:        constructor
// INPUT:       none
// OUTPUT:      none

CMulti::CMulti()
{
    SetTestType(TESTTYPE_CONF);

    m_szTestName = "WLP Multi-Texturing";
    m_szCommandKey = "Range";

    ClearStatus();

    m_pImages[0] = NULL;
    m_pImages[1] = NULL;

    m_pTextures[0] = NULL;
    m_pTextures[1] = NULL;

    m_pFramework->GetImageCmp()->SetMinPixels(0); // no pixels touched during NORENDER scenarios

    m_Options.ModeOptions.CubeFmt = FMT_ANY;
    m_Options.ModeOptions.VolumeFmt = FMT_ANY;
    m_Options.ModeOptions.ZBufferFmt = FMT_ANY;
    m_Options.ModeOptions.dwTexInclude = PF_RGB;
    m_Options.ModeOptions.dwTexExclude = (PF_PALETTEINDEXED1 |
                                          PF_PALETTEINDEXED2 |
                                          PF_PALETTEINDEXED4 |
                                          PF_PALETTEINDEXED8 |
                                          PF_PALETTEINDEXEDTO8);

    m_Options.D3DOptions.bZBuffer = true;
    m_Options.D3DOptions.dwWinMode = (WINMODE_WINDOWED | WINMODE_FULLSCREEN);
}

// NAME:        ~CMulti()
// DESC:        destructor
// INPUT:       none
// OUTPUT:      none

CMulti::~CMulti()
{
    SAFEDELETE(m_pImages[0]);
    SAFEDELETE(m_pImages[1]);
    RELEASE(m_pTextures[0]);
    RELEASE(m_pTextures[1]);
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      true.....if caps check is successful
//              false....otherwise

bool CMulti::CapsCheck(VOID)
{
    bool bPass = true;

    if (m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures < 2)
    {
        WriteToLog("Device must support at least 2 simultaneous textures.\n");
        WriteToLog("wMaxSimultaneousTextures (%d) < 2.\n",
                   m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures);
        bPass = false;
    }

    if (m_pSrcDevice->GetCaps()->wMaxTextureBlendStages < 2)
    {
        WriteToLog("Device must support at least 2 blending stages.\n");
        WriteToLog("wMaxTextureBlendStages (%d) < 2.\n",
                    m_pSrcDevice->GetCaps()->wMaxTextureBlendStages);
        bPass = false;
    }

    if ((m_pSrcDevice->GetCaps()->dwMaxTextureWidth < 16) ||
        (m_pSrcDevice->GetCaps()->dwMaxTextureHeight < 16))
    {
        WriteToLog("Device must support a texture size of at least than 16x16.\n");
        WriteToLog("dwMaxTextureWidth (%d) < 16.\n", m_pSrcDevice->GetCaps()->dwMaxTextureWidth);
        WriteToLog("dwMaxTextureHeight (%d) < 16.\n", m_pSrcDevice->GetCaps()->dwMaxTextureHeight);
        bPass = false;
    }

    if (!(m_pSrcDevice->GetCaps()->dwShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB) ||
        !(m_pSrcDevice->GetCaps()->dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND))
    {
        WriteToLog("Device must support Gouraud shading.\n");
        WriteToLog("D3DPSHADECAPS_COLORGOURAUDRGB must be set.\n");
        WriteToLog("D3DPSHADECAPS_ALPHAGOURAUDBLEND must be set.\n");
        bPass = false;
    }

    if (!(m_pSrcDevice->GetCaps()->dwTextureAddressCaps & D3DPTADDRESSCAPS_WRAP))
    {
        WriteToLog("Device must support wrap texture addressing mode.\n");
        WriteToLog("D3DPTADDRESSCAPS_WRAP must be set.\n");
        bPass = false;
    }

    if (!(m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT) ||
		!(m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))
    {
        WriteToLog("Device must support point-sampled filtering.\n");
        WriteToLog("D3DPTFILTERCAPS_MAGFPOINT and D3DPTFILTERCAPS_MINFPOINT must be set.\n");
        bPass = false;
    }

    if (!(m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_ADD))
    {
        WriteToLog("Device must support add texture op.\n");
        WriteToLog("D3DTEXOPCAPS_ADD must be set.\n");
        bPass = false;
    }

	if (!(m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR))
    {
        WriteToLog("Device must support modulatealpha texture op.\n");
        WriteToLog("D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR must be set.\n");
        bPass = false;
    }

	if (!(m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_MODULATE))
    {
        WriteToLog("Device must support modulate texture op.\n");
        WriteToLog("D3DTEXOPCAPS_MODULATE must be set.\n");
        bPass = false;
    }

    if (!(m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_SELECTARG2))
    {
        WriteToLog("Device must support select arg 2 texture op.\n");
        WriteToLog("D3DTEXOPCAPS_SELECTARG2 must be set.\n");
        bPass = false;
    }

    if (!(m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_SELECTARG1))
    {
        WriteToLog("Device must support select arg 1 texture op.\n");
        WriteToLog("D3DTEXOPCAPS_SELECTARG1 must be set.\n");
        bPass = false;
    }

    if (!(m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_BLENDFACTORALPHA))
    {
        WriteToLog("Device must support blend factor alpha texture op.\n");
        WriteToLog("D3DTEXOPCAPS_BLENDFACTORALPHA must be set.\n");
        bPass = false;
    }

    if (!(m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_BLENDDIFFUSEALPHA))
    {
        WriteToLog("Device must support blend diffuse alpha texture op.\n");
        WriteToLog("D3DTEXOPCAPS_BLENDDIFFUSEALPHA must be set.\n");
        bPass = false;
    }

    if ((WORD)(m_pSrcDevice->GetCaps()->dwFVFCaps & D3DFVFCAPS_TEXCOORDCOUNTMASK) < 2)
    {
        WriteToLog("Device must support at least 2 simultaneous coord sets.\n");
        WriteToLog("(dwFVFCaps & D3DFVFCAPS_TEXCOORDCOUNTMASK) < 2.\n");
        bPass = false;
    }

	if (!(m_pSrcDevice->GetCaps()->dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA) ||
		!(m_pSrcDevice->GetCaps()->dwDestBlendCaps & D3DPBLENDCAPS_INVDESTALPHA))
    {
        WriteToLog("Device must support required alpha blending modes.\n");
		WriteToLog("D3DPBLENDCAPS_SRCALPHA must be set in dwSrcBlendCaps.\n");
        WriteToLog("D3DPBLENDCAPS_INVDESTALPHA must be set in dwDestBlendCaps.\n");
        bPass = false;
    }

    return bPass;
}

// NAME:        Setup()
// DESC:        setup
// INPUT:       none
// OUTPUT:      true.....if setup is successful
//              false....otherwise

bool CMulti::Setup(VOID)
{
    if (!SetupMatrices())
    {
        WriteToLog("Unable to set matrices.\n");
        return false;
    }

    // scan for WLP formats:  (4:4:4:4), (1:5:5:5), (8:8:8:8)

    int n555Format = -1;
    int n444Format = -1;
	int n888Format = -1;

    for (int i = 0; i < m_uCommonTextureFormats; i++)
    {
        PIXELFORMAT Format = m_pCommonTextureFormats[i].ddpfPixelFormat;

        if ((Format.dwFlags & PF_RGB) && (Format.dwFlags & PF_ALPHAPIXELS))
        {
            DWORD dwBits[4];

            dwBits[0] = Bits(Format.dwRGBAlphaBitMask);
            dwBits[1] = Bits(Format.dwRBitMask);
            dwBits[2] = Bits(Format.dwGBitMask);
            dwBits[3] = Bits(Format.dwBBitMask);

            if ((dwBits[0] == 4) && (dwBits[1] == 4) && (dwBits[2] == 4) && (dwBits[3] == 4))
                n444Format = i;

            if ((dwBits[0] == 1) && (dwBits[1] == 5) && (dwBits[2] == 5) && (dwBits[3] == 5))
                n555Format = i;

			if ((dwBits[0] == 8) && (dwBits[1] == 8) && (dwBits[2] == 8) && (dwBits[3] == 8))
                n888Format = i;
        }
    }

    if (n555Format == -1)
    {
        WriteToLog("Device does NOT support a 1:5:5:5 ARGB texture format.\n");
        return false;
    }

    if (n444Format == -1)
    {
        WriteToLog("Device does NOT support a 4:4:4:4 ARGB texture format.\n");
        return false;
    }
	
	if ( !KeySet( "mobile" ) )
	{
		if (n888Format == -1)
		{
			WriteToLog("Device does NOT support a 8:8:8:8 ARGB texture format.\n");
			return false;
		}
	}

    m_pCurrentMode->nTextureFormat = n444Format;
    return true;
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CMulti::TestInitialize(VOID)
{
    m_nSeed = GetTickCount();
    srand(m_nSeed);
    SetTestRange((UINT) 1, (UINT) (NTEXTURES*NMODES*NALPHAS*NINDICES*NFOG));
    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true....if test was executed successfully
//              false...otherwise

bool CMulti::ExecuteTest(UINT uTest)
{
    bool bResult = true;
    DWORD dwLevels = (DWORD) 1;
    DWORD dwPool = FMWK_DEFAULT;
    DWORD dwUsage = (DWORD) 0;
    DWORD dwWidth = (DWORD) 64, dwHeight = (DWORD) 64;
    FORMAT *pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
	bool bFog;
	if ( 1 == ((uTest - 1)/(NTEXTURES*NMODES*NALPHAS*NINDICES)) % NFOG )
		 bFog = true;

    m_nTextures = ((uTest - 1) % NTEXTURES) + 1;
    m_fAlpha = 0.25f*(((uTest - 1)/ NTEXTURES) % NALPHAS);
    m_dwIndex = ((uTest - 1)/(NTEXTURES*NALPHAS)) % NINDICES;
    m_nBlend = ((uTest - 1)/(NTEXTURES*NALPHAS*NINDICES)) % NMODES;
    
    BeginTestCase("WLP Multitexturing", uTest);

    if (m_pSrcDevice->GetCaps()->dwMaxTextureWidth < dwWidth)
        dwWidth = m_pSrcDevice->GetCaps()->dwMaxTextureWidth;

    if (m_pSrcDevice->GetCaps()->dwMaxTextureHeight < dwHeight)
        dwHeight = m_pSrcDevice->GetCaps()->dwMaxTextureHeight;
     
    SetTexture(0, NULL);
    SetTexture(1, NULL);

    SAFEDELETE(m_pImages[0]); 
    SAFEDELETE(m_pImages[1]); 
    RELEASE(m_pTextures[0]); 
    RELEASE(m_pTextures[1]); 

    m_dwColors[0] = m_dwIndex;
    m_dwColors[1] = m_dwIndex;
    m_dwColors[2] = (m_dwIndex + 1) % NINDICES;
    m_dwColors[3] = (m_dwIndex + 1) % NINDICES;

    DWORD dwColors[4];

    for (int i = 0; i < 4; i++)
        if (m_nTextures > 1)
            dwColors[i] = WLP_ALPHA(gpColors[m_dwColors[i]].dwColor, m_fAlpha);
        else
            dwColors[i] = WLP_ALPHA(gpColors[m_dwColors[i]].dwColor, 1.0f);

    m_pImages[0] = new CImage;

    if (!m_pImages[0])
    {
        WriteToLog("Unable to create gradient image.\n");
        return false;
    }

    if (!m_pImages[0]->LoadGradient(dwWidth, dwHeight, dwColors))
    {
        WriteToLog("Unable to load gradient image.\n");
        return false;
    }

    if (!CreateTexture(dwWidth, dwHeight, dwLevels, dwUsage, *pFormat, dwPool, &m_pTextures[0]))
    {
        WriteToLog("Unable to create gradient texture.\n");
        return false;
    }

    if (!m_pTextures[0]->LoadImage(dwLevels, &m_pImages[0]))
    {
        WriteToLog("Unable to load gradient texture.\n");
        return false;
    }
    
    SetTexture(0, m_pTextures[0]);

    if (m_nTextures > 1)
    {
        DWORD pdwStripes[] =
        {
            { (DWORD)WLP_ALPHA(WLP_RED,     1.00f) },
            { (DWORD)WLP_ALPHA(WLP_YELLOW,  1.00f) },
            { (DWORD)WLP_ALPHA(WLP_GREEN,   1.00f) },
            { (DWORD)WLP_ALPHA(WLP_CYAN,    1.00f) },
            { (DWORD)WLP_ALPHA(WLP_BLUE,    1.00f) },
            { (DWORD)WLP_ALPHA(WLP_MAGENTA, 1.00f) },
            { (DWORD)WLP_ALPHA(WLP_GRAY,    1.00f) },
            { (DWORD)WLP_ALPHA(WLP_WHITE,   1.00f) },
            { (DWORD)WLP_ALPHA(WLP_RED,     0.45f) },
            { (DWORD)WLP_ALPHA(WLP_YELLOW,  0.45f) },
            { (DWORD)WLP_ALPHA(WLP_GREEN,   0.45f) },
            { (DWORD)WLP_ALPHA(WLP_CYAN,    0.45f) },
            { (DWORD)WLP_ALPHA(WLP_BLUE,    0.45f) },
            { (DWORD)WLP_ALPHA(WLP_MAGENTA, 0.45f) },
            { (DWORD)WLP_ALPHA(WLP_GRAY,    0.45f) },
            { (DWORD)WLP_ALPHA(WLP_WHITE,   0.45f) }
        };

        DWORD dwCount = sizeof(pdwStripes)/sizeof(DWORD);

        m_pImages[1] = new CImage;

        if (!m_pImages[1])
        {
            WriteToLog("Unable to create stipes image.\n");
            return false;
        }

        if (!m_pImages[1]->LoadStripes(dwWidth, dwHeight, dwCount, pdwStripes, true))
        {
            WriteToLog("Unable to load stipes image.\n");
            return false;
        }

        if (!CreateTexture(dwWidth, dwHeight, dwLevels, dwUsage, *pFormat, dwPool, &m_pTextures[1]))
        {
            WriteToLog("Unable to create stripes texture.\n");
            return false;
        }

        if (!m_pTextures[1]->LoadImage(dwLevels, &m_pImages[1]))
        {
            WriteToLog("Unable to load stripes texture.\n");
            return false;
        }

        LOG(SetTexture(1, m_pTextures[1]));
    }
    else
    {
        LOG(SetTexture(1, NULL));
    }

    LOG(SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD) TRUE));
    LOG(SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD) TRUE));
    LOG(SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD) D3DBLEND_SRCALPHA));
    LOG(SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD) D3DBLEND_INVSRCALPHA));

    if (3 == m_nTextures)
    {
        LOG(SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, (DWORD) 1));
    }
    else
    {
        LOG(SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, (DWORD) 0));
    }

    LOG(SetTextureStageState(0, D3DTSS_MAGFILTER, (DWORD) TEXF_POINT));
    LOG(SetTextureStageState(1, D3DTSS_MAGFILTER, (DWORD) TEXF_POINT));
    LOG(SetTextureStageState(1, D3DTSS_ADDRESSU, (DWORD) D3DTADDRESS_WRAP));
    LOG(SetTextureStageState(1, D3DTSS_ADDRESSV, (DWORD) D3DTADDRESS_WRAP));

    switch (gpModes[m_nBlend].eBlend)
    {
        case MODULATE_RGB:
            SetupModulateRGB();
        break;

        case MODULATE_ALPHA:
            SetupModulateAlpha();
        break;

        case ADD:
            SetupAdd();
        break;

        case BLEND_REGISTER:
            SetupBlendFactor();
        break;

        case BLEND_DIFFUSE:
            SetupBlendDiffuse();
        break;

		case MODULATE_ALPHA_ADDCOLOR:
			SetupModulateAlphaAddColor();
		break;
    }

	SetupFog(bFog);

    return true;
}

// NAME:        SetupFog()
// DESC:        SetupFog
// INPUT:       none
// OUTPUT:      none

void CMulti::SetupFog( bool bFogOn )
{
	if ( bFogOn )
	{
		SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) true);
		SetRenderState(D3DRENDERSTATE_FOGCOLOR, WLP_WHITE);
	}
	else
	{
		SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) false);
	}
}

// NAME:        Cleanup()
// DESC:        cleanup
// INPUT:       none
// OUTPUT:      none

VOID CMulti::Cleanup(VOID)
{
    RELEASE(m_pTextures[0]);
    RELEASE(m_pTextures[1]);
}

// NAME:        CommandLineHelp()
// DESC:        Note the command line options for the test
// INPUT:       none
// OUTPUT:      none

VOID CMulti::CommandLineHelp(VOID)
{
	WriteCommandLineHelp("$yMobile: $wSwitch test to mobile WLP requirements.");
}

// NAME:        SceneRefresh()
// DESC:        draws the scene 
// INPUT:       none
// OUTPUT:      none 

VOID CMulti::SceneRefresh(VOID)
{
    if (BeginScene())
    {
        DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
        DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
        FLEXVERT pVertices[4];

        pVertices[0].fPx = 1.0f; 
        pVertices[0].fPy = (float) (dwHeight - 1); 
        pVertices[0].fTs = 0.0f;
        pVertices[0].fTt = 1.0f;
        pVertices[0].fTu = 0.0f;
        pVertices[0].fTv = 3.0f;
		pVertices[0].dcSpecular = 0x77000000;

        pVertices[1].fPx = 1.0f;
        pVertices[1].fPy = 1.0f; 
        pVertices[1].fTs = 0.0f;
        pVertices[1].fTt = 0.0f;
        pVertices[1].fTu = 0.0f;
        pVertices[1].fTv = 0.0f;
		pVertices[0].dcSpecular = 0x00000000;

        pVertices[2].fPx = (float) (dwWidth - 1);
        pVertices[2].fPy = (float) (dwHeight - 1); 
        pVertices[2].fTs = 1.0f;
        pVertices[2].fTt = 1.0f;
        pVertices[2].fTu = 3.0f;
        pVertices[2].fTv = 3.0f;
		pVertices[0].dcSpecular = 0x77000000;

        pVertices[3].fPx = (float) (dwWidth - 1);
        pVertices[3].fPy = 1.0f; 
        pVertices[3].fTs = 1.0f;
        pVertices[3].fTt = 0.0f;
        pVertices[3].fTu = 3.0f;
        pVertices[3].fTv = 0.0f;
		pVertices[0].dcSpecular = 0x00000000;

        for (int i = 0; i < 4; i++)
        {
            pVertices[i].fPz = 0.1f;
            pVertices[i].fRHW = 10.0f;

            // maps colors from left to right on triangles,
            // which is 90 degrees offset from texture colors

            if (1 == m_nTextures)
                pVertices[i].dcDiffuse = WLP_ALPHA(gpColors[m_dwColors[i]].dwColor, m_fAlpha);
            else
                pVertices[i].dcDiffuse = WLP_ALPHA(gpColors[m_dwColors[i]].dwColor, 1.0f);
        }

        if (!RenderPrimitive(D3DPT_TRIANGLESTRIP, FLEXFMT, pVertices, 4, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() failed: %s.\n", GLE(GetLastError()));
            Fail();
        }

        if (!EndScene())
        {
            WriteToLog("EndScene() failed: %s.\n", GLE(GetLastError()));
            Fail();
        }
    }
    else
    {
        WriteToLog("BeginScene() failed: %s.\n", GLE(GetLastError()));
        Fail();
    }

    sprintf(m_szStatus[0][0], "$gTextures");
    sprintf(m_szStatus[0][1], "$y%d", m_nTextures > 2 ? 2 : m_nTextures);
    sprintf(m_szStatus[1][0], "$gTexture Coords");
    sprintf(m_szStatus[1][1], "$y%d", m_nTextures < 3 ? 1 : 2);
    sprintf(m_szStatus[2][0], "$gBlend Mode");
    sprintf(m_szStatus[2][1], "$y%s", gpModes[m_nBlend].szName);
    sprintf(m_szStatus[3][0], "$gAlpha Value");
    sprintf(m_szStatus[3][1], "$y%f", m_fAlpha);
    sprintf(m_szStatus[4][0], "$gDiffuse Colors");
    sprintf(m_szStatus[4][1], "$y%s -> %s, alpha %f horizontal gradient",
            gpColors[m_dwColors[0]].szName, gpColors[m_dwColors[2]].szName, m_nTextures == 1 ? m_fAlpha : 1);
    sprintf(m_szStatus[5][0], "$gTexture 0 Colors");
    sprintf(m_szStatus[5][1], "$y%s -> %s, alpha %f vertical gradient",
            gpColors[m_dwColors[0]].szName, gpColors[m_dwColors[2]].szName, m_nTextures > 1 ? m_fAlpha : 1);
    sprintf(m_szStatus[6][0], "$gTexture 1 Colors");
    sprintf(m_szStatus[6][1], "$y8 color stripes w/1.0 alpha, 8 color stripes w/0.45 alpha");
}

// NAME:        ProcessFrame()
// DESC:        process frame
// INPUT:       none
// OUTPUT:      GetCompareResult(0.15f, 0.78f, 0)

bool CMulti::ProcessFrame(VOID)
{
    static int nPass = 0;
    static int nFail = 0;
    bool bResult = false;
    TCHAR szBuffer[512];

    if (!CD3DTest::ProcessFrame())
        WriteToLog("Unable to process frame.\n");
    else
        bResult = (PASS == m_pFramework->m_DXGLog.GetCurrentResult());

    if (bResult)
    {
        nPass++;
        sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Pass();
    }
    else
    {
        nFail++;
        sprintf(szBuffer,
                "State info:\n"
                "# of Textures: %d\n"
                "# of Texture Coords: %d\n"
                "Blend Mode: %s\n"
                "Diffuse Colors: %s -> %s, Alpha %f Horiz. Gradient\n"
                "Texture 0 Colors: %s -> %s, Alpha %f Vert Gradient\n"
                "Texture 1 Colors: 8 color stripes w/alpha 1.0, 8 color stripes w/Alpha 0.45\n\n",
                m_nTextures > 2 ? 2 : m_nTextures,
                m_nTextures < 3 ? 1 : 2,
                gpModes[m_nBlend].szName,
                gpColors[m_dwColors[0]].szName,
                gpColors[m_dwColors[2]].szName,
                m_nTextures == 1 ? m_fAlpha : 1,
                gpColors[m_dwColors[0]].szName,
                gpColors[m_dwColors[2]].szName,
                m_nTextures > 1 ? m_fAlpha : 1);
        WriteToLog(szBuffer);
        sprintf(szBuffer, "$yPass: %d, Fail: %d", nPass, nFail);
        Fail();
    }

    EndTestCase();
    return bResult;
}

// NAME:        UpdateStatus()
// DESC:        update status
// INPUT:       none
// OUTPUT:      none

VOID CMulti::UpdateStatus(VOID)
{
    CD3DTest::UpdateStatus();

    for (UINT i = 0; i < NSTATUS; i++)
        WriteStatus(m_szStatus[i][0], m_szStatus[i][1]);
}

// NAME:        ClearStatus()
// DESC:        clear status
// INPUT:       none
// OUTPUT:      none

VOID CMulti::ClearStatus(VOID)
{
    for (UINT i = 0; i < NSTATUS; i++)
    {
        sprintf(m_szStatus[i][0], "$gNA\t");
        sprintf(m_szStatus[i][1], "$y-");
    }
}

// NAME:        SetupMatrices()
// DESC:        setup matrices
// INPUT:       none
// OUTPUT:      true.....if matrices set successfully
//              false....otherwise

bool CMulti::SetupMatrices(VOID)
{/*
    D3DXMATRIX Matrix;
    D3DVALUE dvAngle = (float) pi/2.0f;
    D3DVALUE dvAspect = ((float) m_pCurrentMode->nBackBufferWidth) /
                        ((float) m_pCurrentMode->nBackBufferHeight);
    D3DXVECTOR3 From(0.0f, 0.0f, -10.0f);
    D3DXVECTOR3 At(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);

    D3DXMatrixIdentity(&Matrix);

    if (!SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &Matrix))
    {
        WriteToLog("SetTransform(WORLD) failed: %s.\n", GLE(GetLastError()));
        return false;
    }

    D3DXMatrixPerspectiveFovLH(&Matrix, dvAngle, dvAspect, 1.0f, 20.0f);

    float fTemp = (float) 1.0/Matrix.m[2][3];

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            Matrix.m[i][j] *= fTemp;

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION, (D3DMATRIX *) &Matrix))
    {
        WriteToLog("SetTransform(PROJECTION) failed: %s.\n", GLE(GetLastError()));
        return false;
    }

    D3DXMatrixLookAtLH(&Matrix, &From, &At, &Up);

    if (!SetTransform(D3DTRANSFORMSTATE_VIEW, (D3DMATRIX *) &Matrix))
    {
        WriteToLog("SetTransform(VIEW) failed: %s.\n", GLE(GetLastError()));
        return false;
    }
	*/
    return true;
}

// NAME:        SetupModulateRGB()
// DESC:        setup modulate RGB
// INPUT:       none
// OUTPUT:      none

VOID CMulti::SetupModulateRGB(VOID)
{
    switch (m_nTextures)
    {
        case 1:
            LOG(SetTextureStageState(0, D3DTSS_COLOROP,     (DWORD)D3DTOP_MODULATE));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG1,   (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG2,   (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAOP,     (DWORD)D3DTOP_SELECTARG2));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG1,   (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG2,   (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(1, D3DTSS_COLOROP,     (DWORD)D3DTOP_DISABLE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAOP,     (DWORD)D3DTOP_DISABLE));

            ValidateDevice();

            WriteToLog("Texture Stage States:\n");
            WriteToLog("0, D3DTSS_COLOROP,     D3DTOP_MODULATE\n");
            WriteToLog("0, D3DTSS_COLORARG1,   D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_COLORARG2,   D3DTA_CURRENT\n");
            WriteToLog("0, D3DTSS_ALPHAOP,     D3DTOP_SELECTARG2\n");
            WriteToLog("0, D3DTSS_ALPHAARG1,   D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_ALPHAARG2,   D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_COLOROP,     D3DTOP_DISABLE\n");
            WriteToLog("1, D3DTSS_ALPHAOP,     D3DTOP_DISABLE\n");
        break;

        case 2:
        case 3:
            // turn off blending with diffuse
            LOG(SetTextureStageState(0, D3DTSS_COLOROP,   (DWORD)D3DTOP_SELECTARG1));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_SELECTARG1));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));

            // enable blending in second stage
            LOG(SetTextureStageState(1, D3DTSS_COLOROP,   (DWORD)D3DTOP_MODULATE));
            LOG(SetTextureStageState(1, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(1, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_SELECTARG2));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));

            ValidateDevice();

            WriteToLog("Texture Stage States:\n");
            WriteToLog("0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1\n");
            WriteToLog("0, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_COLORARG2, D3DTA_CURRENT\n");
            WriteToLog("0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1\n");
            WriteToLog("0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_COLOROP,   D3DTOP_MODULATE\n");
            WriteToLog("1, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("1, D3DTSS_COLORARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2\n");
            WriteToLog("1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("1, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
        break;
    }
}

// NAME:        SetupModulateAlpha()
// DESC:        setup modulate alpha
// INPUT:       none
// OUTPUT:      none

VOID CMulti::SetupModulateAlpha(VOID)
{
    switch (m_nTextures)
    {
        case 1:
            // blend with diffuse
            LOG(SetTextureStageState(0, D3DTSS_COLOROP,   (DWORD)D3DTOP_MODULATE));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_MODULATE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(1, D3DTSS_COLOROP,   (DWORD)D3DTOP_DISABLE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_DISABLE));

            ValidateDevice();

            WriteToLog("Texture Stage States:\n");
            WriteToLog("0, D3DTSS_COLOROP,   D3DTOP_MODULATE\n");
            WriteToLog("0, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_COLORARG2, D3DTA_CURRENT\n");
            WriteToLog("0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE\n");
            WriteToLog("0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_COLOROP,   D3DTOP_DISABLE\n");
            WriteToLog("1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE\n");
        break;

        case 2:
        case 3:
            // turn off blending with diffuse
            LOG(SetTextureStageState(0, D3DTSS_COLOROP,   (DWORD)D3DTOP_SELECTARG1));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_SELECTARG1));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));

            // enable blending in second stage
            LOG(SetTextureStageState(1, D3DTSS_COLOROP,   (DWORD)D3DTOP_MODULATE));
            LOG(SetTextureStageState(1, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(1, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_MODULATE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));

            ValidateDevice();

            WriteToLog("Texture Stage States:\n");
            WriteToLog("0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1\n");
            WriteToLog("0, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_COLORARG2, D3DTA_CURRENT\n");
            WriteToLog("0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1\n");
            WriteToLog("0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_COLOROP,   D3DTOP_MODULATE\n");
            WriteToLog("1, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("1, D3DTSS_COLORARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE\n");
            WriteToLog("1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("1, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
        break;
    }
}

// NAME:        SetupModulateAlphaAddColor()
// DESC:        setup modulate alpha add color
// INPUT:       none
// OUTPUT:      none

VOID CMulti::SetupModulateAlphaAddColor(VOID)
{
    switch (m_nTextures)
    {
        case 1:
            // blend with diffuse
            LOG(SetTextureStageState(0, D3DTSS_COLOROP,   (DWORD)D3DTOP_MODULATEALPHA_ADDCOLOR));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_MODULATE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(1, D3DTSS_COLOROP,   (DWORD)D3DTOP_DISABLE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_DISABLE));

            ValidateDevice();

            WriteToLog("Texture Stage States:\n");
            WriteToLog("0, D3DTSS_COLOROP,   D3DTOP_MODULATEALPHA_ADDCOLOR\n");
            WriteToLog("0, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_COLORARG2, D3DTA_CURRENT\n");
            WriteToLog("0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE\n");
            WriteToLog("0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_COLOROP,   D3DTOP_DISABLE\n");
            WriteToLog("1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE\n");
        break;

        case 2:
        case 3:
            // turn off blending with diffuse
            LOG(SetTextureStageState(0, D3DTSS_COLOROP,   (DWORD)D3DTOP_SELECTARG1));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_SELECTARG1));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));

            // enable blending in second stage
            LOG(SetTextureStageState(1, D3DTSS_COLOROP,   (DWORD)D3DTOP_MODULATEALPHA_ADDCOLOR));
            LOG(SetTextureStageState(1, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(1, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_MODULATE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));

            ValidateDevice();

            WriteToLog("Texture Stage States:\n");
            WriteToLog("0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1\n");
            WriteToLog("0, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_COLORARG2, D3DTA_CURRENT\n");
            WriteToLog("0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1\n");
            WriteToLog("0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_COLOROP,   D3DTOP_MODULATEALPHA_ADDCOLOR\n");
            WriteToLog("1, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("1, D3DTSS_COLORARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE\n");
            WriteToLog("1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("1, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
        break;
    }
}

// NAME:        SetupAdd()
// DESC:        setup add
// INPUT:       none
// OUTPUT:      none

VOID CMulti::SetupAdd(VOID)
{
    switch (m_nTextures)
    {
        case 1:
            // blend with diffuse
            LOG(SetTextureStageState(0, D3DTSS_COLOROP,   (DWORD)D3DTOP_ADD));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_SELECTARG2));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(1, D3DTSS_COLOROP,   (DWORD)D3DTOP_DISABLE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_DISABLE));

            ValidateDevice();

            WriteToLog("Texture Stage States:\n");
            WriteToLog("0, D3DTSS_COLOROP,   D3DTOP_ADD\n");
            WriteToLog("0, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_COLORARG2, D3DTA_CURRENT\n");
            WriteToLog("0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2\n");
            WriteToLog("0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_COLOROP,   D3DTOP_DISABLE\n");
            WriteToLog("1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE\n");
        break;

        case 2:
        case 3:
            // turn off blending with diffuse
            LOG(SetTextureStageState(0, D3DTSS_COLOROP,   (DWORD)D3DTOP_SELECTARG1));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_SELECTARG1));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));

            // enable blending in second stage
            LOG(SetTextureStageState(1, D3DTSS_COLOROP,   (DWORD)D3DTOP_ADD));
            LOG(SetTextureStageState(1, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(1, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_SELECTARG2));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));

            ValidateDevice();

            WriteToLog("Texture Stage States:\n");
            WriteToLog("0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1\n");
            WriteToLog("0, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_COLORARG2, D3DTA_CURRENT\n");
            WriteToLog("0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1\n");
            WriteToLog("0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_COLOROP,   D3DTOP_ADD\n");
            WriteToLog("1, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("1, D3DTSS_COLORARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2\n");
            WriteToLog("1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("1, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
        break;
    }
}

// NAME:        SetupBlendFactor()
// DESC:        setup blend factor
// INPUT:       none
// OUTPUT:      none

VOID CMulti::SetupBlendFactor(VOID)
{
    LOG(SetRenderState(D3DRENDERSTATE_TEXTUREFACTOR, (DWORD) WLP_ALPHA(0, m_fAlpha)));

    switch (m_nTextures)
    {
        case 1:
            // blend with diffuse
            LOG(SetTextureStageState(0, D3DTSS_COLOROP,   (DWORD)D3DTOP_BLENDFACTORALPHA));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_SELECTARG2));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(1, D3DTSS_COLOROP,   (DWORD)D3DTOP_DISABLE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_DISABLE));

            ValidateDevice();

            WriteToLog("Texture Stage States:\n");
            WriteToLog("0, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA\n");
            WriteToLog("0, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_COLORARG2, D3DTA_DIFFUSE\n");
            WriteToLog("0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2\n");
            WriteToLog("0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_COLOROP,   D3DTOP_DISABLE\n");
            WriteToLog("1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE\n");
        break;

        case 2:
        case 3:
            // turn off blending with diffuse
            LOG(SetTextureStageState(0, D3DTSS_COLOROP,   (DWORD)D3DTOP_SELECTARG1));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_SELECTARG1));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));

            // enable blending in second stage
            LOG(SetTextureStageState(1, D3DTSS_COLOROP,   (DWORD)D3DTOP_BLENDFACTORALPHA));
            LOG(SetTextureStageState(1, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(1, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_SELECTARG2));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));

            ValidateDevice();

            WriteToLog("Texture Stage States:\n");
            WriteToLog("0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1\n");
            WriteToLog("0, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_COLORARG2, D3DTA_CURRENT\n");
            WriteToLog("0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1\n");
            WriteToLog("0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA\n");
            WriteToLog("1, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("1, D3DTSS_COLORARG2, D3DTA_DIFFUSE\n");
            WriteToLog("1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2\n");
            WriteToLog("1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("1, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
        break;
    }
}

// NAME:        SetupBlendDiffuse()
// DESC:        setup blend diffuse
// INPUT:       none
// OUTPUT:      none

VOID CMulti::SetupBlendDiffuse(VOID)
{
    switch (m_nTextures)
    {
        case 1:
            // blend with diffuse
            LOG(SetTextureStageState(0, D3DTSS_COLOROP,   (DWORD)D3DTOP_BLENDDIFFUSEALPHA));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_SELECTARG2));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(1, D3DTSS_COLOROP,   (DWORD)D3DTOP_DISABLE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_DISABLE));
            ValidateDevice();
            WriteToLog("Texture Stage States:\n");
            WriteToLog("0, D3DTSS_COLOROP,   D3DTOP_BLENDDIFFUSEALPHA\n");
            WriteToLog("0, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_COLORARG2, D3DTA_DIFFUSE\n");
            WriteToLog("0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2\n");
            WriteToLog("0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_COLOROP,   D3DTOP_DISABLE\n");
            WriteToLog("1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE\n");
        break;

        case 2:
        case 3:
            // turn off blending with diffuse
            LOG(SetTextureStageState(0, D3DTSS_COLOROP,   (DWORD)D3DTOP_SELECTARG1));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_SELECTARG1));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));

            // enable blending in second stage
            LOG(SetTextureStageState(1, D3DTSS_COLOROP,   (DWORD)D3DTOP_BLENDDIFFUSEALPHA));
            LOG(SetTextureStageState(1, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(1, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAOP,   (DWORD)D3DTOP_SELECTARG2));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE));
            LOG(SetTextureStageState(1, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT));

            ValidateDevice();

            WriteToLog("Texture Stage States:\n");
            WriteToLog("0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1\n");
            WriteToLog("0, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_COLORARG2, D3DTA_CURRENT\n");
            WriteToLog("0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1\n");
            WriteToLog("0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("0, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
            WriteToLog("1, D3DTSS_COLOROP,   D3DTOP_BLENDDIFFUSEALPHA\n");
            WriteToLog("1, D3DTSS_COLORARG1, D3DTA_TEXTURE\n");
            WriteToLog("1, D3DTSS_COLORARG2, D3DTA_DIFFUSE\n");
            WriteToLog("1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2\n");
            WriteToLog("1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE\n");
            WriteToLog("1, D3DTSS_ALPHAARG2, D3DTA_CURRENT\n");
        break;
    }
}

// NAME:        ValidateDevice()
// DESC:        validate device
// INPUT:       none
// OUTPUT:      none

VOID CMulti::ValidateDevice(VOID)
{
    DWORD dwPasses;

    dwPasses = (DWORD) 0xDEADBEEF;

    if (!CD3DTest::ValidateDevice(&dwPasses, CD3D_SRC) || (dwPasses > 1))
    {
        WriteToLog("Src requires 0x%08X passes:  %s.\n", dwPasses, GLE(GetLastError()));
    }

    dwPasses = (DWORD) 0xDEADBEEF;

    if (!CD3DTest::ValidateDevice(&dwPasses, CD3D_REF) || (dwPasses > 1))
    {
        WriteToLog("Ref requires 0x%08X passes:  %s.\n", dwPasses, GLE(GetLastError()));
    }
}
