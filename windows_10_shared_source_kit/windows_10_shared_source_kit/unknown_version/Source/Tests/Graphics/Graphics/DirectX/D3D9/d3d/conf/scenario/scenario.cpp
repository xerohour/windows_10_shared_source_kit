// FILE:        scenario.cpp
// DESC:        scenario class methods for driver scenario conformance test

#include "scenario.h"

CD3DWindowFramework App;
CScenario           Scenario;

const char *g_pszMessage[] =
{
    "                                                                             \n",
    "  Refresh rate test failing because:                                         \n",
    "    refresh rate of at least 85 Hz not detected                              \n",
    "  or                                                                         \n",
    "    process of detecting refresh rate failed.                                \n",
    "                                                                             \n",
    "  DDraw (win9x) can only set 0 Hz for rate indicating driver can use default.\n",
    "  For test to pass, default refresh rate must be at least 85 Hz.             \n",
    "  Method by which default refresh rate is set is not important.              \n",
    "  Common method is to select desired refresh rate thru control panel applet. \n",
    "  Try re-running test after setting default refresh rate to at least 85 Hz.  \n",
    "  In order for this adapter to be compliant, it MUST pass this test.         \n",
    "                                                                             \n",
    NULL
};

const char *g_pszImages[] =
{
    "checker256.bmp",
    "checker128.bmp",
    "checker64.bmp",
    "checker32.bmp",
    "checker16.bmp",
    "checker8.bmp",
    "checker4.bmp",
    "checker2.bmp",
    "checker1.bmp"
};

// NAME:        CScenario()
// DESC:        constructor
// INPUT:       none
// OUTPUT:      none

CScenario::CScenario()
{
    SetTestType(TESTTYPE_CONF);

    m_szTestName = "Scenario";
    m_szCommandKey = "Scenario";

    ClearStatus();

    m_fGradientAlpha = 1.0f;
    m_fMipmapAlpha = 1.0f;

    m_dwMinLevel = (DWORD) 0;
    m_dwMaxLevel = (DWORD) 0;
    m_dwTest = (DWORD) 0;

    m_pGradientImage = NULL;
    m_pPlaneImage = NULL;
    m_pSphereImage = NULL;

    for (UINT i = 0; i < NLEVELS; i++)
        m_pMipMapImages[i] = NULL;

    m_pGradientTexture = NULL;
    m_pMipMapTexture = NULL;
    m_pPlaneTexture = NULL;
    m_pSphereTexture = NULL;

    m_pDefTarget = NULL;
    m_pDefZBuffer = NULL;

    m_pTexTarget = NULL;
    m_pTexZBuffer = NULL;

    m_pFramework->GetImageCmp()->SetMinPixels(0); // no pixels touched during NORENDER scenarios

    m_Options.ModeOptions.CubeFmt = FMT_ANY;
    m_Options.ModeOptions.VolumeFmt = FMT_ANY;
    m_Options.ModeOptions.ZBufferFmt = FMT_ANY;
    m_Options.ModeOptions.MultisampType = MULTISAMPLE_ANY;
    m_Options.ModeOptions.dwTexInclude = (PF_RGB |  PF_ALPHAPIXELS);
    m_Options.ModeOptions.dwTexExclude = (PF_PALETTEINDEXED1 |
                                          PF_PALETTEINDEXED2 |
                                          PF_PALETTEINDEXED4 |
                                          PF_PALETTEINDEXED8 |
                                          PF_PALETTEINDEXEDTO8);

    m_Options.D3DOptions.bZBuffer = true;
    m_Options.D3DOptions.nBackBufferWidth = (DWORD) 160;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD) 140;

	m_Options.D3DOptions.dwPSGPRegChange = PSGPREGCHANGE_DISABLE;
	m_Options.D3DOptions.dwRefPSGPRegChange = PSGPREGCHANGE_DISABLE;

	// Force test to use relevant DirectX versions when run in WLP mode
	if (KeySet("WLP"))
	{
        m_szTestName = "WLP RGB Rasterization";
	    m_Options.D3DOptions.dwWinMode = (WINMODE_WINDOWED | WINMODE_FULLSCREEN);
	}
}

// NAME:        ~CScenario()
// DESC:        destructor
// INPUT:       none
// OUTPUT:      none

CScenario::~CScenario()
{
    SAFEDELETE(m_pGradientImage);
    SAFEDELETE(m_pPlaneImage);
    SAFEDELETE(m_pSphereImage);

    for (UINT i = 0; i < NLEVELS; i++)
        SAFEDELETE(m_pMipMapImages[i]);

    RELEASE(m_pGradientTexture);
    RELEASE(m_pMipMapTexture);
    RELEASE(m_pPlaneTexture);
    RELEASE(m_pSphereTexture);

    RELEASE(m_pTexTarget);
    RELEASE(m_pTexZBuffer);

    RELEASE(m_pDefTarget);
    RELEASE(m_pDefZBuffer);
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CScenario::CommandLineHelp(VOID)
{
    WriteCommandLineHelp("$y-Dump $wDump test list to log");
    WriteCommandLineHelp("$y-Long $wRuns all frames of each scenario (vs frames 2 and 5)");
    WriteCommandLineHelp("$y-TCI0 $wTexture coordinate indices default to 0 (all stages)");
    WriteCommandLineHelp("$y-Max $wMax candidate texture render target list applied");
    WriteCommandLineHelp("$y-Mobile $wAdjusts WLP refresh rate test for mobile chipsets");
    WriteCommandLineHelp("$y-WLP $wDisables Target, WBuffer, and Modulate tests");
    WriteCommandLineHelp("$y-!Alpha $wDo not run alpha blend tests");
    WriteCommandLineHelp("$y-!Caps $wSkip caps check");
    WriteCommandLineHelp("$y-!Correction $wDo not expose color perspective correction errors");
    WriteCommandLineHelp("$y-!Dither $wDo not run dither tests");
    WriteCommandLineHelp("$y-!Fog $wDo not run fog tests");
    WriteCommandLineHelp("$y-!Specular $wDo not run specular tests");
    WriteCommandLineHelp("$y-!Texture $wDo not run texture tests");
    WriteCommandLineHelp("$y-!Filter $wDisables MagFilter, MinFilter and MipFilter tests");
    WriteCommandLineHelp("$y-!MagFilter $wDo not run mag filtering tests");
    WriteCommandLineHelp("$y-!MinFilter $wDo not run min filtering tests");
    WriteCommandLineHelp("$y-!MipFilter $wDo not run mipmapping tests");
    WriteCommandLineHelp("$y-!Modulate $wDo not run modulate tests");
    WriteCommandLineHelp("$y-!Target $wDo not run texture as render target tests");
    WriteCommandLineHelp("$y-!Depth $wDisables ZBuffer and WBuffer tests");
    WriteCommandLineHelp("$y-!ZBuffer $wDo not run z-buffer tests");
    WriteCommandLineHelp("$y-!WBuffer $wDo not run w-buffer tests");
    WriteCommandLineHelp("$y-!NoRender $wDo not run rendering suppressed tests");
    WriteCommandLineHelp("$y-GradientAlpha: $wgradient texture alpha override $c(default=1.0)"); 
    WriteCommandLineHelp("$y-MipmapAlpha: $wmipmap texture alpha override $c(default=1.0)"); 
}

// NAME:        FilterResolution()
// DESC:        filter resolution
// INPUT:       pMode....address of DISPLAYMODE data
// OUTPUT:      true.....for 1024x768x32 modes (refresh rate adjusted)
//              false....otherwise

bool CScenario::FilterResolution(PDISPLAYMODE pMode)
{
	if (KeySet("WLP"))
	{
		if (KeySet("Mobile"))
		{
			ADAPTER     *pAdapter = m_pD3D->GetAdapter(m_pSrcDevice->GetAdapterID());
			
			// For Mobile use the current desktop mode
			if ((pAdapter->DesktopMode.dwWidth == pMode->dwWidth) && (pAdapter->DesktopMode.dwHeight == pMode->dwHeight) && (pAdapter->DesktopMode.dwBPP == pMode->dwBPP))
			{
				pMode->dwRefreshRate = 0; // mobile chipsets not required to support 85 Mhz

				return true;
			}
			else
				return false;
		}

		if ((1024 == pMode->dwWidth) && (768 == pMode->dwHeight) && (32 == pMode->dwBPP))
		{
			OSVERSIONINFO VerInfo;

			memset(&VerInfo, 0, sizeof(OSVERSIONINFO));
			VerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&VerInfo);

			if (VerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
				pMode->dwRefreshRate = 0; // can't set refresh rate on win9x
			else
				pMode->dwRefreshRate = 85;

			return true;
		}
		else
			return false;
	}
	else
		return CD3DTest::FilterResolution(pMode);
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      true.....if caps check is successful
//              false....otherwise

bool CScenario::CapsCheck(VOID)
{
    bool bCaps = true;
    float fTemp;

    // clear flags

    m_Flags.Clear(0xFFFFFFFF);

    // set default flags

    m_Flags.Set(CF_ALPHA);
    m_Flags.Set(CF_CORRECT);
    m_Flags.Set(CF_DITHER);
    m_Flags.Set(CF_FOG);
    m_Flags.Set(CF_MAGFILTER);
    m_Flags.Set(CF_MINFILTER);
    m_Flags.Set(CF_MIPFILTER);
    m_Flags.Set(CF_MODULATE);
    m_Flags.Set(CF_NORENDER);
    m_Flags.Set(CF_REFRESH);
    m_Flags.Set(CF_SPECULAR);
    m_Flags.Set(CF_TARGET);
    m_Flags.Set(CF_TEXTURE);
    m_Flags.Set(CF_WBUFFER);
    m_Flags.Set(CF_ZBUFFER);

    // clear flags wrt command line options

    if (KeySet("WLP"))
        m_Flags.Clear(CF_CORRECT | CF_DITHER | CF_MODULATE | CF_TARGET | CF_WBUFFER);

    if (KeySet("!Alpha"))
        m_Flags.Clear(CF_ALPHA);

    if (KeySet("!Correction") || KeySet("WHQL"))
        m_Flags.Clear(CF_CORRECT);

    if (KeySet("!Dither"))
        m_Flags.Clear(CF_DITHER);

    if (KeySet("!Fog"))
        m_Flags.Clear(CF_FOG);

    if (KeySet("!NoRender"))
        m_Flags.Clear(CF_NORENDER);

    if (KeySet("!Specular"))
        m_Flags.Clear(CF_SPECULAR);

    if (KeySet("!Modulate"))
        m_Flags.Clear(CF_MODULATE);

    if (KeySet("!Filter"))
        m_Flags.Clear(CF_MINFILTER | CF_MAGFILTER | CF_MIPFILTER);

    if (KeySet("!MinFilter"))
        m_Flags.Clear(CF_MINFILTER);

    if (KeySet("!MagFilter"))
        m_Flags.Clear(CF_MAGFILTER);

    if (KeySet("!MipFilter"))
        m_Flags.Clear(CF_MIPFILTER);

    if (KeySet("!Target"))
        m_Flags.Clear(CF_TARGET);

    if (KeySet("!Texture"))
        m_Flags.Clear(CF_TEXTURE);

    if (KeySet("!Depth"))
        m_Flags.Clear(CF_ZBUFFER | CF_WBUFFER);

    if (KeySet("!ZBuffer"))
        m_Flags.Clear(CF_ZBUFFER);

    if (KeySet("!WBuffer"))
        m_Flags.Clear(CF_WBUFFER);

    ReadString("GradientAlpha", m_szBuffer, ARRAYSIZE(m_szBuffer));
    m_fGradientAlpha = (D3DVALUE) (sscanf(m_szBuffer, "%f", &fTemp) == 1) ? fTemp : 1.0f;

    ReadString("MipmapAlpha", m_szBuffer, ARRAYSIZE(m_szBuffer));
    m_fMipmapAlpha = (D3DVALUE) (sscanf(m_szBuffer, "%f", &fTemp) == 1) ? fTemp : 1.0f;

    // clear flags wrt caps

    if (!KeySet("!Caps") && !KeySet("WLP"))
    {
        if ((0 == m_pSrcDevice->GetCaps()->dwMinTextureWidth) ||
            (0 == m_pSrcDevice->GetCaps()->dwMinTextureHeight) ||
            (0 == m_pSrcDevice->GetCaps()->dwMaxTextureWidth) ||
            (0 == m_pSrcDevice->GetCaps()->dwMaxTextureHeight))
        {
            WriteToLog("(WLP Failure) Texture size limit(s) equal to 0.\n");
            bCaps = false;
        }

        if (!((m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT) ||
              (m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)))
        {
            if (!(m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))
                WriteToLog("(WLP Failure) D3DPTFILTERCAPS_MIPFPOINT not exposed, disabling tests.\n");

            if (!(m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
                WriteToLog("(WLP Failure) D3DPTFILTERCAPS_MIPFLINEAR not exposed, disabling tests.\n");

            m_Flags.Clear(CF_MIPFILTER);
            bCaps = false;
        }

        if (!(m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_FOGVERTEX))
        {
            WriteToLog("(WLP Failure) D3DPRASTERCAPS_FOGVERTEX not exposed, disabling tests.\n");
            m_Flags.Clear(CF_FOG);
            bCaps = false;
        }

        if (!((m_pSrcDevice->GetCaps()->dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND) &&
              (m_pSrcDevice->GetCaps()->dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA) &&
              (m_pSrcDevice->GetCaps()->dwDestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA)))
        {
            WriteToLog("(WLP Failure) Device does not support alpha blending, disabling tests.\n");

            if (!(m_pSrcDevice->GetCaps()->dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND))
                WriteToLog("D3DPSHADECAPS_ALPHAGOURAUDBLEND not exposed.\n");

            if (!(m_pSrcDevice->GetCaps()->dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA))
                WriteToLog("SRC D3DPBLENDCAPS_SRCALPHA not exposed.\n");

            if (!(m_pSrcDevice->GetCaps()->dwDestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA))
                WriteToLog("DST D3DPBLENDCAPS_INVSRCALPHA not exposed.\n");

            m_Flags.Clear(CF_ALPHA);
            bCaps = false;
        }

        if (!(m_pSrcDevice->GetCaps()->dwShadeCaps & D3DPSHADECAPS_SPECULARGOURAUDRGB))
        {
            WriteToLog("(WLP Failure) D3DPSHADECAPS_SPECULARGOURAUDRGB not exposed, disabling tests.\n");
            m_Flags.Clear(CF_SPECULAR);
            bCaps = false;
        }

        if (!(m_pSrcDevice->GetCaps()->dwShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB))
        {
            WriteToLog("(WLP Failure) D3DPSHADECAPS_COLORGOURAUDRGB not exposed.\n");
            bCaps = false;
        }

        if (!(m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_DITHER))
        {
            WriteToLog("(INFO) Device does not support dithering, disabling tests.\n");
            m_Flags.Clear(CF_DITHER);
        }

        if (!(m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE))
        {
            WriteToLog("(WLP Failure) D3DPTEXTURECAPS_PERSPECTIVE not exposed.\n");
            bCaps = false;
        }

        if (m_dwVersion < 0x0800)
        {
            if (!(m_pSrcDevice->GetCaps()->dwDeviceRenderBitDepth & DDBD_16))
            {
                WriteToLog("(WLP Failure) Device does not expose 16 bit render target.\n");
                bCaps = false;
            }

            if (!(m_pSrcDevice->GetCaps()->dwDeviceZBufferBitDepth & DDBD_16))
            {
                WriteToLog("(WLP Failure) Device does not expose 16 bit Z buffer.\n");
                m_Flags.Clear(CF_ZBUFFER);
                bCaps = false;
            }
        }

        if (!(m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_WBUFFER))
        {
            WriteToLog("(INFO) D3DPRASTERCAPS_WBUFFER not exposed, disabling tests.\n");
            m_Flags.Clear(CF_WBUFFER);
        }

        if (m_pSrcDevice->GetCaps()->wMaxTextureBlendStages < 2)
        {
            WriteToLog("(INFO) Insufficient blend stages for modulate, disabling tests.\n");
            m_Flags.Clear(CF_MODULATE);
        }

        if (m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures < 2)
        {
            WriteToLog("(INFO) Insufficient simultaneous textures for modulate, disabling tests.\n");
            m_Flags.Clear(CF_MODULATE);
        }

        if (!(m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_MODULATE))
        {
            WriteToLog("(INFO) Insufficient blend ops for modulate, disabling tests.\n");
            m_Flags.Clear(CF_MODULATE);
        }

        if (!(m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_COLORPERSPECTIVE))
        {
            WriteToLog("(INFO) Iterated color perspective correction not supported, disabling tests.\n");
            m_Flags.Clear(CF_CORRECT);
        }
    }

    if (KeySet("WLP"))
    {
        if (!bCaps)
        {
            WriteToLog("(WLP Failure) WLP caps not set.\n");
            return false;
        }

        if (KeySet("Mobile"))
            WriteToLog("Refresh rate test passed (mobile not required to support 85 Hz).\n");
		else if (m_dwVersion > 0x0700)
            WriteToLog("Refresh rate test passed (not able to verify with interfaces above Dx7).\n");
        else
        {
            if (!CheckRefreshRate())
            {
                UINT i = (UINT) 0;

                m_Flags.Set(CF_REFRESH);

                while (g_pszMessage[i])
                {
                    WriteToLog("%s", g_pszMessage[i]);
                    i++;
                }
            }
            else
                WriteToLog("Refresh rate test passed (at least 85 Hz detected).\n");
        }
    }

    // select WLP-supported format
    // ordered preferences: 8:8:8:8, 4:4:4:4, 1:5:5:5, X:8:8:8

    m_pnTargetFormats[N8888] = (INT) -1;
    m_pnTargetFormats[N4444] = (INT) -1;
    m_pnTargetFormats[N1555] = (INT) -1;
    m_pnTargetFormats[NX888] = (INT) -1;

    for (UINT nFormat = 0; nFormat < m_uCommonTextureFormats; nFormat++)
    {
        DWORD dwABits = 0, dwRBits = 0, dwGBits = 0, dwBBits = 0;
        PIXELFORMAT Format = m_pCommonTextureFormats[nFormat].ddpfPixelFormat;

        dwABits = BitCount(Format.dwRGBAlphaBitMask);
        dwRBits = BitCount(Format.dwRBitMask);
        dwGBits = BitCount(Format.dwGBitMask);
        dwBBits = BitCount(Format.dwBBitMask);

        if ((dwABits == 8) && (dwRBits == 8) && (dwGBits == 8) && (dwBBits == 8))
            m_pnTargetFormats[N8888] = (INT) nFormat;

        if ((dwABits == 4) && (dwRBits == 4) && (dwGBits == 4) && (dwBBits == 4))
            m_pnTargetFormats[N4444] = (INT) nFormat;

        if ((dwABits == 1) && (dwRBits == 5) && (dwGBits == 5) && (dwBBits == 5))
            m_pnTargetFormats[N1555] = (INT) nFormat;

        if ((dwABits == 0) && (dwRBits == 8) && (dwGBits == 8) && (dwBBits == 8))
            m_pnTargetFormats[NX888] = (INT) nFormat;
    }

    if ((-1 == m_pnTargetFormats[N1555]) && KeySet("WLP"))
    {
        WriteToLog("(WLP Failure) Device does not expose 1:5:5:5 ARGB texture format.\n");
        return false;
    }

    if ((-1 == m_pnTargetFormats[N4444]) && KeySet("WLP"))
    {
        WriteToLog("(WLP Failure) Device does not expose 4:4:4:4 ARGB texture format.\n");
        return false;
    }

    if ((-1 == m_pnTargetFormats[N8888]) && KeySet("WLP") && !KeySet("Mobile"))
    {
        WriteToLog("(WLP Failure) Device does not expose 8:8:8:8 ARGB texture format.\n");
        return false;
    }

	if (m_pnTargetFormats[N1555] != -1)
        m_pCurrentMode->nTextureFormat = m_pnTargetFormats[N1555];

    if (m_pnTargetFormats[N4444] != -1)
        m_pCurrentMode->nTextureFormat = m_pnTargetFormats[N4444];

    if (m_pnTargetFormats[N8888] != -1)
        m_pCurrentMode->nTextureFormat = m_pnTargetFormats[N8888];

    // check mipmap level caps

    DWORD dwMinTextureWidth = m_pSrcDevice->GetCaps()->dwMinTextureWidth;
    DWORD dwMaxTextureWidth = m_pSrcDevice->GetCaps()->dwMaxTextureWidth;
    DWORD dwMinTextureHeight = m_pSrcDevice->GetCaps()->dwMinTextureHeight;
    DWORD dwMaxTextureHeight = m_pSrcDevice->GetCaps()->dwMaxTextureHeight;

    // equalize texture width and height

    if (dwMaxTextureWidth > dwMaxTextureHeight)
        dwMaxTextureWidth = dwMaxTextureHeight;
    else
        dwMaxTextureHeight = dwMaxTextureWidth;

    if (dwMinTextureWidth < dwMinTextureHeight)
        dwMinTextureWidth = dwMinTextureHeight;
    else
        dwMinTextureHeight = dwMinTextureWidth;

    // find appropriate max and min mip levels

    for (m_dwMaxLevel =       0; (MAXSIZE >> m_dwMaxLevel) > dwMaxTextureWidth;  m_dwMaxLevel++);
    for (m_dwMinLevel = NLEVELS; (MAXSIZE >> m_dwMinLevel) < dwMinTextureHeight; m_dwMinLevel--);

    wsprintf(m_szBuffer, "(INFO) Max texture size supported (up to %dx%d): %dx%d.\n",
             MAXSIZE, MAXSIZE, MAXSIZE >> m_dwMaxLevel, MAXSIZE >> m_dwMaxLevel);
    WriteToLog(m_szBuffer);

    wsprintf(m_szBuffer, "(INFO) Min texture size supported (down to 1x1): %dx%d.\n",
             MAXSIZE >> m_dwMinLevel, MAXSIZE >> m_dwMinLevel);
    WriteToLog(m_szBuffer);
    return true;
}

// NAME:        Setup()
// DESC:        setup
// INPUT:       none
// OUTPUT:      true.....if setup is successful
//              false....otherwise

bool CScenario::Setup(VOID)
{
    if (m_Flags.Check(CF_TEXTURE))
    {
        if (!CreateImages())
        {
            WriteToLog("Unable to create images.\n");
            return false;
        }

        if (!CreateTextures())
        {
            WriteToLog("Unable to create textures.\n");
            return false;
        }
    }

    if (!SetupLights())
    {
        WriteToLog("Unable to set lights.\n");
        return false;
    }

    if (!SetupMaterials())
    {
        WriteToLog("Unable to set materials.\n");
        return false;
    }

    if (!SetupMatrices())
    {
        WriteToLog("Unable to set matrices.\n");
        return false;
    }

    if (!SetupRenderStates())
    {
        WriteToLog("Unable to set render states.\n");
        return false;
    }

    return true;
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CScenario::TestInitialize(VOID)
{
    if (!CreateShapes())
    {
        WriteToLog("Unable to create shapes.\n");
        return D3DTESTINIT_ABORT;
    }

    if (!CreateTests())
    {
        WriteToLog("Unable to create tests.\n");
        return D3DTESTINIT_ABORT;
    }

    // caps based test elimination

    if (!m_Flags.Check(CF_TEXTURE))
        EliminateTests(CF_TEXTURE);

    if (!m_Flags.Check(CF_MINFILTER))
        EliminateTests(CF_MINFILTER);

    if (!m_Flags.Check(CF_MAGFILTER))
        EliminateTests(CF_MAGFILTER);

    if (!m_Flags.Check(CF_MIPFILTER))
        EliminateTests(CF_MIPFILTER);

    if (!m_Flags.Check(CF_MODULATE))
        EliminateTests(CF_MODULATE);

    // validation based test elimination

    ValidateTextureStageStates();

    if (!m_Flags.Check(CF_SPECULAR))
        EliminateTests(CF_SPECULAR);

    if (!m_Flags.Check(CF_ALPHA))
        EliminateTests(CF_ALPHA);

    if (!m_Flags.Check(CF_TARGET))
        EliminateTests(CF_TARGET);

    if (!m_Flags.Check(CF_ZBUFFER))
        EliminateTests(CF_ZBUFFER);

    if (!m_Flags.Check(CF_WBUFFER))
        EliminateTests(CF_WBUFFER);

    if (!m_Flags.Check(CF_FOG))
        EliminateTests(CF_FOG);

    if (!m_Flags.Check(CF_CORRECT))
        EliminateTests(CF_CORRECT);

    if (!m_Flags.Check(CF_DITHER))
        EliminateTests(CF_DITHER);

    // dump tests

    if (KeySet("Dump"))
    {
        DumpTests();
        return D3DTESTINIT_ABORT;
    }

    SetTestRange((UINT) 1, (UINT) m_Tests.size());

    return D3DTESTINIT_RUN;
}

// NAME:        ClearFrame()
// DESC:        clear frame
// INPUT:       none
// OUTPUT:      Clear()

bool CScenario::ClearFrame(VOID)
{
    return Clear(0, NULL, FMWK_DEFAULT, RGB_MAKE(128, 0, 128), 1.0f, (DWORD) 0);
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CScenario::ExecuteTest(UINT uTest)
{
    UINT i;

    // set test counter

    m_dwTest = uTest;

    // addition check for some strangeness encountered when -range specified

    if (CHECK(CF_SKIP, m_Tests[m_dwTest - 1].dwFlags))
        return false;

    // set appropriate parameters for test

    DescribeTest(uTest - 1, m_szBuffer);
    BeginTestCase(m_szBuffer, uTest);

    // update cube positions (fOrbitPos = Base + Offset)

    for (i = 0; i < NCUBES; i++)
    {
        m_pCubes[i].m_fOrbitOffset = (pi/(float) NCUBES)*(float) i;
        m_pCubes[i].m_fOrbitPos = (2.0f*pi/(float) NCUBES)*(float) i + 
                                  (2.0f*pi/(float) NSTEPS)*m_Tests[uTest - 1].bFrame;
    }

    // set appropriate renderstates/texture stage states

    SetRenderState(D3DRS_LIGHTING, (DWORD) TRUE);
    SetRenderState(D3DRS_AMBIENT, (DWORD) 0x22222222);

    if (CHECK(CF_DITHER, m_Tests[uTest - 1].dwFlags))
        SetRenderState(D3DRS_DITHERENABLE, (DWORD) TRUE);
    else
        SetRenderState(D3DRS_DITHERENABLE, (DWORD) FALSE);

    if (CHECK(CF_SPECULAR, m_Tests[uTest - 1].dwFlags)) 
        SetRenderState(D3DRS_SPECULARENABLE, (DWORD) TRUE);
    else
        SetRenderState(D3DRS_SPECULARENABLE, (DWORD) FALSE);

    if (CHECK(CF_ALPHA, m_Tests[uTest - 1].dwFlags))
    {
        SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD) TRUE);
        SetRenderState(D3DRS_SRCBLEND, (DWORD) D3DBLEND_SRCALPHA);
        SetRenderState(D3DRS_DESTBLEND, (DWORD) D3DBLEND_INVSRCALPHA);
    }
    else
    {
        SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD) FALSE);
        SetRenderState(D3DRS_SRCBLEND, (DWORD) D3DBLEND_ONE);
        SetRenderState(D3DRS_DESTBLEND, (DWORD) D3DBLEND_ZERO);
    }

    if (CHECK(CF_WBUFFER, m_Tests[uTest - 1].dwFlags))
    {
        SetRenderState(D3DRS_ZWRITEENABLE, (DWORD) TRUE);
        SetRenderState(D3DRS_ZENABLE, (DWORD) D3DZB_USEW);
        SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_LESSEQUAL);
    }
    else if (CHECK(CF_ZBUFFER, m_Tests[uTest - 1].dwFlags))
    {
        SetRenderState(D3DRS_ZWRITEENABLE, (DWORD) TRUE);
        SetRenderState(D3DRS_ZENABLE, (DWORD) D3DZB_TRUE);
        SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_LESSEQUAL);
    }
    else
    {
        SetRenderState(D3DRS_ZWRITEENABLE, (DWORD) FALSE);
        SetRenderState(D3DRS_ZENABLE, (DWORD) FALSE);
    }
      
    HRESULT hResult; 

    if (D3D_OK != (hResult = GetLastError()))
    {
        WriteToLog("ExecuteTest() failed: %s.\n", GLE(hResult));
        Fail();
        return false;
    } 

    return true;
}

// NAME:        SceneRefresh()
// DESC:        scene refresh
// INPUT:       none
// OUTPUT:      none 

VOID CScenario::SceneRefresh(VOID)
{
    DWORD dwMagFilter, dwMinFilter, dwMipFilter;
    HRESULT hResult;
    UINT i, nScene, nSceneStart = (UINT) 1;
 
    TESTDEF &Test = m_Tests[m_dwTest - 1];

    dwMagFilter = (DWORD) (CHECK(CF_MAGFILTER, Test.dwFlags) ? TEXF_LINEAR : TEXF_POINT);
    dwMinFilter = (DWORD) (CHECK(CF_MINFILTER, Test.dwFlags) ? TEXF_LINEAR : TEXF_POINT);
    dwMipFilter = (DWORD) TEXF_NONE;

    if (CHECK(CF_MIPFILTER, Test.dwFlags))
    {
        if ((m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) &&
            (m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
            dwMipFilter = (DWORD) TEXF_LINEAR;
    else
            dwMipFilter = (DWORD) TEXF_POINT;
    }

    if (m_Flags.Check(CF_NORENDER) && CHECK(CF_NORENDER, Test.dwFlags))
    {
        if (BeginScene())
        {
            if (!EndScene())
            {
                WriteToLog("EndScene() failed: %s.\n", GLE(GetLastError()));
                m_Tests[m_dwTest - 1].dwFlags |= CF_FAIL;
                Fail();
            }
            else
                Pass();
        }
        else
        {
            WriteToLog("BeginScene() failed: %s.\n", GLE(GetLastError()));
            m_Tests[m_dwTest - 1].dwFlags |= CF_FAIL;
            Fail();
        }

        return;
    }

    if (CHECK(CF_TARGET, Test.dwFlags))
        nSceneStart = (UINT) 0; // first scene drawn without textures
    else
        nSceneStart = (UINT) 1; // first scene drawn with    textures
    
    for (nScene = nSceneStart; nScene < 2; nScene++)
    {
        if (CHECK(CF_TEXTURE, Test.dwFlags))
        {
            if (CHECK(CF_TARGET, Test.dwFlags))
            {
                if (0 == nScene)
                {
                    CnSurface *pTarget = NULL;

                    if (!m_pTexTarget->GetSurfaceLevel((UINT) 0, &pTarget))
                    {
                        WriteToLog("GetSurfaceLevel() failed.\n");
                        Fail();
                        return;
                    }
                    else if (!SetRenderTarget(pTarget, m_pTexZBuffer))
                    {
                        WriteToLog("SetRenderTarget() failed (texture render target): %s.\n",
                                   GLE(GetLastError()));
                        RELEASE(pTarget);
                        Fail();
                        return;
                    }

                    CSurface *pSurface = NULL;
                    SURFACEDESC Desc;
                    VIEWPORT Viewport;

                    if (!pTarget->GetSurface(&pSurface, (UINT) 0))
                    {
                        WriteToLog("GetSurface() failed (texture render target).\n");
                        RELEASE(pTarget);
                        Fail();
                        return;
                    }
                    else if (DD_OK != pSurface->GetDesc(&Desc))
                    {
                        WriteToLog("GetDesc() failed (texture render target): %s.\n",
                                   GLE(GetLastError()));
                        RELEASE(pSurface);
                        RELEASE(pTarget);
                        Fail();
                        return;
                    }
                    
                    Viewport.X = (DWORD) 0;
                    Viewport.Y = (DWORD) 0;
                    Viewport.Width = Desc.dwWidth;
                    Viewport.Height = Desc.dwHeight;
                    Viewport.MinZ = 0.0f;
                    Viewport.MaxZ = 1.0f;

                    SetViewport(&Viewport);

                    RELEASE(pSurface);
                    RELEASE(pTarget);
                }
                else
                {
                    if (!SetRenderTarget(m_pDefTarget, m_pDefZBuffer))
                    {
                        WriteToLog("SetRenderTarget() failed (default render target): %s.\n",
                                   GLE(GetLastError()));
                        Fail();
                        return;
                    }

                    CSurface *pSurface;
                    SURFACEDESC Desc;
                    VIEWPORT Viewport;

                    if (!m_pDefTarget->GetSurface(&pSurface, (UINT) 0))
                    {
                        WriteToLog("GetSurface() failed (default render target).\n");
                        Fail();
                        return;
                    }
                    else if (DD_OK != pSurface->GetDesc(&Desc))
                    {
                        WriteToLog("GetDesc() failed (default render target): %s.\n",
                                   GLE(GetLastError()));
                        RELEASE(pSurface);
                        Fail();
                        return;
                    }

                    Viewport.X = (DWORD) 0;
                    Viewport.Y = (DWORD) 0;
                    Viewport.Width = Desc.dwWidth;
                    Viewport.Height = Desc.dwHeight;
                    Viewport.MinZ = 0.0f;
                    Viewport.MaxZ = 1.0f;

                    SetViewport(&Viewport);

                    RELEASE(pSurface);
                }
            }
        }

        if (CHECK(CF_TARGET, Test.dwFlags))
            ClearFrame();
        
        if ((hResult = GetLastError()) != D3D_OK)
        {
            WriteToLog("ClearFrame() failed: %s.\n", GLE(hResult));
            Fail();
            return;
        }

        if (BeginScene())
        {
            D3DXMATRIX M, Rx, Ry, Rz, T;
    
            if (CHECK(CF_FOG, Test.dwFlags))
            {
                DWORDFLOAT dwfStart, dwfEnd, dwfDensity;

                dwfStart.fValue = 5.0f;
                dwfEnd.fValue = 15.0f;
                dwfDensity.fValue = 1.0f;

                SetRenderState(D3DRS_FOGENABLE, (DWORD) TRUE);
                SetRenderState(D3DRS_FOGCOLOR, (DWORD) 0xFFFFFFFF);
                SetRenderState(D3DRS_FOGSTART, dwfStart.dwValue);
                SetRenderState(D3DRS_FOGEND, dwfEnd.dwValue);
                SetRenderState(D3DRS_FOGDENSITY, dwfDensity.dwValue);

                if (m_dwVersion >= 0x0700) 
                    SetRenderState(D3DRS_FOGVERTEXMODE, (DWORD) D3DFOG_LINEAR);
                else
                    SetRenderState(D3DRS_FOGTABLEMODE, (DWORD) D3DFOG_NONE);
            }
            else
                SetRenderState(D3DRS_FOGENABLE, (DWORD) FALSE);

            // draw cubes
    
            SetMaterial(&m_DefaultMaterial); 
            
            for (i = 0; i < NCUBES; i++)
            {
                D3DXMatrixIdentity(&M);
                D3DXMatrixRotationZ(&Rz, -m_pCubes[i].m_fOrbitOffset);
                D3DXMatrixMultiply(&M, &Rz, &M);
                D3DXMatrixRotationY(&Ry, -m_pCubes[i].m_fOrbitPos);
                D3DXMatrixMultiply(&M, &Ry, &M);
                D3DXMatrixTranslation(&T, 6.0f, 0.0f, 0.0f);
                D3DXMatrixMultiply(&M, &T, &M);
   
                if (!SetTransform(D3DTS_WORLD, (D3DMATRIX *) &M))
                {
                    WriteToLog("SetTransform(WORLD) failed: %s.\n", GLE(GetLastError()));
                    Fail();
                    return;
                } 
                 
                if ((1 == nScene) && CHECK(CF_TEXTURE, Test.dwFlags))
                {
                    SetTextureStageState(0, D3DTSS_COLOROP, (DWORD) D3DTOP_SELECTARG1);
                    SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);

                    if ((0 == i) && CHECK(CF_TARGET, Test.dwFlags))
                    {
                        SetSamplerState(0, SAMP_MAGFILTER, (DWORD) TEXF_POINT);
                        SetSamplerState(0, SAMP_MINFILTER, (DWORD) TEXF_POINT);
                        SetSamplerState(0, SAMP_MIPFILTER, (DWORD) TEXF_NONE);
                        SetTexture(0, m_pTexTarget);
                    }
                    else
                    {
                        SetSamplerState(1, SAMP_MAGFILTER, dwMagFilter);
                        SetSamplerState(1, SAMP_MINFILTER, dwMinFilter);
                        SetSamplerState(0, SAMP_MIPFILTER, dwMipFilter);
                        SetTexture(0, m_pMipMapTexture);
                    }

                    if (!CHECK(CF_MODULATE, Test.dwFlags))
                    {
                        SetTextureStageState(1, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
                        SetTextureStageState(1, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);
                        SetTexture(1, NULL);
                    }
                    else
                    {
                        SetTextureStageState(1, D3DTSS_COLOROP, (DWORD) D3DTOP_MODULATE);
                        SetTextureStageState(1, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG2);
                        SetSamplerState(1, SAMP_MAGFILTER, dwMagFilter);
                        SetSamplerState(1, SAMP_MINFILTER, dwMinFilter);
                        SetSamplerState(1, SAMP_MIPFILTER, (DWORD) TEXF_NONE);
                        SetTexture(1, m_pGradientTexture);
                    }
                }
                else
                {
                    SetTextureStageState(0, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
                    SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);
                    SetTexture(0, NULL);
                }
                
                hResult = ValidateDevice();

                if (DD_OK != hResult)
                {
                    if (!EndScene())
                    {
                        WriteToLog("EndScene() failed: %s.\n", GLE(GetLastError()));
                        m_Tests[m_dwTest - 1].dwFlags |= CF_FAIL;
                        Fail();
                    }

                    Test.dwFlags |= CF_SKIP;
                    WriteToLog("(SKIP) Blend operation could not be performed: %s.\n",
                               GLE(hResult));
                    SkipTests((UINT) 1);

                    if (!SetRenderTarget(m_pDefTarget, m_pDefZBuffer))
                    {
                        WriteToLog("SetRenderTarget() failed (default render target): %s.\n",
                                   GLE(GetLastError()));
                        Fail();
                        return;
                    }

                    CSurface *pSurface;
                    SURFACEDESC Desc;
                    VIEWPORT Viewport;

                    if (!m_pDefTarget->GetSurface(&pSurface, (UINT) 0))
                    {
                        WriteToLog("GetSurface() failed (default render target).\n");
                        Fail();
                        return;
                    }
                    else if (DD_OK != pSurface->GetDesc(&Desc))
                    {
                        WriteToLog("GetDesc() failed (default render target): %s.\n",
                                   GLE(GetLastError()));
                        RELEASE(pSurface);
                        Fail();
                        return;
                    }

                    Viewport.X = (DWORD) 0;
                    Viewport.Y = (DWORD) 0;
                    Viewport.Width = Desc.dwWidth;
                    Viewport.Height = Desc.dwHeight;
                    Viewport.MinZ = 0.0f;
                    Viewport.MaxZ = 1.0f;

                    SetViewport(&Viewport);

                    RELEASE(pSurface);
                    return;
                }

                bool bResult = RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                               m_pCubes[i].m_pVertices, m_pCubes[i].m_nVertices,
                                               m_pCubes[i].m_pIndices, m_pCubes[i].m_nIndices,
                                               NULL);

                if (!bResult)
                {
                    WriteToLog("RenderPrimitive() failed (cube): %s.\n", GLE(GetLastError()));
                    m_Tests[m_dwTest - 1].dwFlags |= CF_FAIL;
                    Fail();
                }
                
            }
    
            // draw planes
   
            for (i = 0; i < NPLANES; i++) 
            {
                D3DVALUE dvT[NPLANES][3] = 
                {
                    { -4.0f, 0.0f, -4.0f },
                    {  0.0f, 0.0f,  0.0f },
                    { 12.0f, 0.0f,  8.0f }
                };

                SetMaterial(&m_AlphaMaterial); 
                D3DXMatrixTranslation(&M, dvT[i][0], dvT[i][1], dvT[i][2]);
 
                if (CHECK(CF_CORRECT, Test.dwFlags))
                {
                    D3DXMatrixRotationX(&Rx, (float) (pi/3.0));
                    D3DXMatrixMultiply(&M, &Rx, &M);
                }
 
                if (!SetTransform(D3DTS_WORLD, (D3DMATRIX *) &M))
                {
                    WriteToLog("SetTransform(WORLD) failed: %s.\n", GLE(GetLastError()));
                    Fail();
                    return;
                }

                if ((1 == nScene) && CHECK(CF_TEXTURE, Test.dwFlags))
                {
                    if (CHECK(CF_TARGET, Test.dwFlags))
                    {
                        SetSamplerState(0, SAMP_MAGFILTER, (DWORD) TEXF_POINT);
                        SetSamplerState(0, SAMP_MINFILTER, (DWORD) TEXF_POINT);
                        SetSamplerState(0, SAMP_MIPFILTER, (DWORD) TEXF_NONE);
                        SetTexture(0, m_pTexTarget);
                    }
                    else
                    {
                        SetSamplerState(1, SAMP_MAGFILTER, dwMagFilter);
                        SetSamplerState(1, SAMP_MINFILTER, dwMinFilter);
                        SetSamplerState(1, SAMP_MIPFILTER, (DWORD) TEXF_NONE);
                        SetTexture(0, m_pPlaneTexture);
                    }

                    if (!CHECK(CF_MODULATE, Test.dwFlags))
                    {
                        SetTextureStageState(0, D3DTSS_COLOROP, (DWORD) D3DTOP_MODULATE);
                        SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);

                        SetTextureStageState(1, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
                        SetTextureStageState(1, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);
                        SetTexture(1, NULL);
                    }
                    else
                    {
                        SetTextureStageState(0, D3DTSS_COLOROP, (DWORD) D3DTOP_SELECTARG1);
                        SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);

                        SetTextureStageState(1, D3DTSS_COLOROP, (DWORD) D3DTOP_MODULATE);
                        SetTextureStageState(1, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG2);
                        SetSamplerState(1, SAMP_MAGFILTER, dwMagFilter);  
                        SetSamplerState(1, SAMP_MINFILTER, dwMinFilter);
                        SetSamplerState(1, SAMP_MIPFILTER, (DWORD) TEXF_NONE);
                        SetTexture(1, m_pGradientTexture);
                    }
                }
                else
                {
                    SetTextureStageState(0, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
                    SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);
                    SetTexture(0, NULL);
                }
        
                if (!RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                     m_pPlanes[i].m_pVertices, m_pPlanes[i].m_nVertices,
                                     m_pPlanes[i].m_pIndices, m_pPlanes[i].m_nIndices,
                                     NULL))
                {
                    WriteToLog("RenderPrimitive() failed (plane): %s.\n", GLE(GetLastError()));
                    m_Tests[m_dwTest - 1].dwFlags |= CF_FAIL;
                    Fail();
                }
            }

            SetRenderState(D3DRS_FOGENABLE, (DWORD)FALSE);
    
            // draw sphere

            SetMaterial(&m_AlphaMaterial); 
            D3DXMatrixIdentity(&M);

            if (!SetTransform(D3DTS_WORLD, (D3DMATRIX *) &M))
            {
                WriteToLog("SetTransform(WORLD) failed: %s.\n", GLE(GetLastError()));
                Fail();
                return;
            }
            
            if ((1 == nScene) && CHECK(CF_TEXTURE, Test.dwFlags))    
            {
                if (CHECK(CF_TARGET, Test.dwFlags))
                {
                    SetSamplerState(0, SAMP_MAGFILTER, (DWORD) TEXF_POINT);
                    SetSamplerState(0, SAMP_MINFILTER, (DWORD) TEXF_POINT);
                    SetSamplerState(0, SAMP_MIPFILTER, (DWORD) TEXF_NONE);
                    SetTexture(0, m_pTexTarget);
                }
                else
                {
                    SetTexture(0, m_pSphereTexture);
                }

                SetTextureStageState(0, D3DTSS_COLOROP, (DWORD) D3DTOP_MODULATE);
                SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);

                SetTextureStageState(1, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
                SetTextureStageState(1, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);
            }
            else
            {
                SetTextureStageState(0, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
                SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);
                SetTexture(0, NULL);
            }

            if (!RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                 m_Sphere.m_pVertices, m_Sphere.m_nVertices,
                                 m_Sphere.m_pIndices, m_Sphere.m_nIndices,
                                 NULL))
            {
                WriteToLog("RenderPrimitive() failed (sphere): %s.\n", GLE(GetLastError()));
                m_Tests[m_dwTest - 1].dwFlags |= CF_FAIL;
                Fail();
            }

            if (!EndScene())
            {
                WriteToLog("EndScene() failed: %s.\n", GLE(GetLastError()));
                m_Tests[m_dwTest - 1].dwFlags |= CF_FAIL;
                Fail();
            }
        }
        else
        {
            WriteToLog("BeginScene() failed: %s.\n", GLE(GetLastError()));
            m_Tests[m_dwTest - 1].dwFlags |= CF_FAIL;
            Fail();
        }
    }
}

// NAME:        ProcessFrame()
// DESC:        process frame
// INPUT:       none
// OUTPUT:      bResult..result of src:ref image comparison

bool CScenario::ProcessFrame(VOID)
{
    if (!m_Flags.Check(CF_REFRESH))
    {
        WriteToLog("(WLP Failure) Test failed refresh rate check.\n");
        m_Tests[m_dwTest - 1].dwFlags |= CF_FAIL;
        m_Flags.Set(CF_REFRESH);
        Fail();
    }

    WriteCurrentStatus();
    return CD3DTest::ProcessFrame();
}

// NAME:        Cleanup()
// DESC:        cleanup
// INPUT:       none
// OUTPUT:      none

VOID CScenario::Cleanup(VOID)
{
    RELEASE(m_pSphereTexture);
    RELEASE(m_pPlaneTexture);
    RELEASE(m_pMipMapTexture);
    RELEASE(m_pGradientTexture);

    RELEASE(m_pDefTarget);
    RELEASE(m_pDefZBuffer);

    RELEASE(m_pTexTarget);
    RELEASE(m_pTexZBuffer);
}

// NAME:        UpdateStatus()
// DESC:        update status
// INPUT:       none
// OUTPUT:      none

VOID CScenario::UpdateStatus(VOID)
{
    CD3DTest::UpdateStatus();

    for (UINT i = 0; i < NSTATUS; i++)
        WriteStatus(m_szStatus[i][0], m_szStatus[i][1]);
}

// NAME:        ClearStatus()
// DESC:        clear status
// INPUT:       none
// OUTPUT:      none

VOID CScenario::ClearStatus(VOID)
{
    for (UINT i = 0; i < NSTATUS; i++)
    {
        lstrcpy(m_szStatus[i][0], "$gNA\t");
        lstrcpy(m_szStatus[i][1], "$y-");
    }
}

// NAME:        TestTerminate()
// DESC:        terminate test
// INPUT:       none
// OUTPUT:      true

bool CScenario::TestTerminate(VOID)
{
    InterpretTestResults();
    DestroyTests();
    Cleanup();
    return true;
}

// NAME:        SetupLights()
// DESC:        setup lights
// INPUT:       none
// OUTPUT:      true.....if lights set successfully
//              false....otherwise

bool CScenario::SetupLights(VOID)
{
    LIGHT Light;

    Light.dltType = D3DLIGHT_POINT;

    Light.dcvAmbient.r = 0.0f;
    Light.dcvAmbient.g = 0.0f;
    Light.dcvAmbient.b = 0.0f;
    Light.dcvAmbient.a = 0.0f;

    Light.dcvDiffuse.r = 1.0f;
    Light.dcvDiffuse.g = 1.0f;
    Light.dcvDiffuse.b = 1.0f;
    Light.dcvDiffuse.a = 1.0f;

    Light.dcvSpecular.r = 0.0f;
    Light.dcvSpecular.g = 0.0f;
    Light.dcvSpecular.b = 0.0f;
    Light.dcvSpecular.a = 0.0f;

    Light.dvPosition.x = 10.0f;
    Light.dvPosition.y = 0.0f;
    Light.dvPosition.z = -10.0f;

    Light.dvDirection.x = 0.0;
    Light.dvDirection.y = 0.0;
    Light.dvDirection.z = 0.0;

    Light.dvAttenuation0 = 1.0f;
    Light.dvAttenuation1 = 0.0f;
    Light.dvAttenuation2 = 0.0f;

    Light.dvFalloff = 0.0f;
    Light.dvPhi = 0.0f;
    Light.dvRange = 100.0f;
    Light.dvTheta = 0.0f;

    if (!SetLight((DWORD) 0, &Light))
    {
        WriteToLog("SetLight() failed (white): %s.\n", GLE(GetLastError()));
        return false;
    }
    else if (!LightEnable((DWORD) 0, TRUE))
    {
        WriteToLog("LightEnable() failed (white): %s.\n", GLE(GetLastError()));
        return false;
    }

    Light.dcvDiffuse.r = 0.0f;
    Light.dcvDiffuse.g = 1.0f;
    Light.dcvDiffuse.b = 0.0f;
    Light.dcvDiffuse.a = 1.0f;

    Light.dvPosition.x = -10.0f;
    Light.dvPosition.y = 0.0f;
    Light.dvPosition.z = -10.0f;

    if (!SetLight((DWORD) 1, &Light))
    {
        WriteToLog("SetLight() failed (red): %s.\n", GLE(GetLastError()));
        return false;
    }
    else if (!LightEnable((DWORD) 1, TRUE))
    {
        WriteToLog("LightEnable() failed (red): %s.\n", GLE(GetLastError()));
        return false;
    }

    return true;
}

// NAME:        SetupMaterials()
// DESC:        setup materials
// INPUT:       none
// OUTPUT:      true.....if materials set successfully
//              false....otherwise

bool CScenario::SetupMaterials(VOID)
{
    m_DefaultMaterial.dcvEmissive.r = 0.0f;
    m_DefaultMaterial.dcvEmissive.g = 0.0f;
    m_DefaultMaterial.dcvEmissive.b = 0.0f;
    m_DefaultMaterial.dcvEmissive.a = 0.0f;

    m_DefaultMaterial.dcvAmbient.r = 0.2f;
    m_DefaultMaterial.dcvAmbient.g = 0.2f;
    m_DefaultMaterial.dcvAmbient.b = 0.2f;
    m_DefaultMaterial.dcvAmbient.a = 0.0f;

    m_DefaultMaterial.dcvDiffuse.r = 1.0f;
    m_DefaultMaterial.dcvDiffuse.g = 1.0f;
    m_DefaultMaterial.dcvDiffuse.b = 0.0f;
    m_DefaultMaterial.dcvDiffuse.a = 1.0f;

    m_DefaultMaterial.dcvSpecular.r = 0.0f;
    m_DefaultMaterial.dcvSpecular.g = 0.0f;
    m_DefaultMaterial.dcvSpecular.b = 0.0f;
    m_DefaultMaterial.dcvSpecular.a = 1.0f;

    m_DefaultMaterial.dvPower = 1.0f;

    memcpy(&m_AlphaMaterial, &m_DefaultMaterial, sizeof(m_AlphaMaterial));
    m_AlphaMaterial.dcvDiffuse.a = 0.5f;

    if (!SetMaterial(&m_DefaultMaterial))
    {
        WriteToLog("SetMaterial() failed (default): %s.\n", GLE(GetLastError()));
        return false;
    }

    if (!SetMaterial(&m_AlphaMaterial))
    {
        WriteToLog("SetMaterial() failed (alpha): %s.\n", GLE(GetLastError()));
        return false;
    }

    return true;
}

// NAME:        SetupMatrices()
// DESC:        setup matrices
// INPUT:       none
// OUTPUT:      true.....if matrices set successfully
//              false....otherwise

bool CScenario::SetupMatrices(VOID)
{
    D3DXMATRIX Matrix;
    D3DVALUE dvAngle = (float) pi/2.0f;
    D3DVALUE dvAspect = ((float) m_pCurrentMode->nBackBufferWidth) /
                        ((float) m_pCurrentMode->nBackBufferHeight);
    D3DXVECTOR3 From(0.0f, 0.0f, -10.0f);
    D3DXVECTOR3 At(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);

    D3DXMatrixIdentity(&Matrix);

    if (!SetTransform(D3DTS_WORLD, (D3DMATRIX *) &Matrix))
    {
        WriteToLog("SetTransform(WORLD) failed: %s.\n", GLE(GetLastError()));
        return false;
    }

    D3DXMatrixPerspectiveFovLH(&Matrix, dvAngle, dvAspect, 1.0f, 100.0f);

    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
    {
        WriteToLog("SetTransform(PROJECTION) failed: %s.\n", GLE(GetLastError()));
        return false;
    }

    D3DXMatrixLookAtLH(&Matrix, &From, &At, &Up);

    if (!SetTransform(D3DTS_VIEW, (D3DMATRIX *) &Matrix))
    {
        WriteToLog("SetTransform(VIEW) failed: %s.\n", GLE(GetLastError()));
        return false;
    }

    return true;
}

// NAME:        SetupRenderStates()
// DESC:        setup render states
// INPUT:       none
// OUTPUT:      true.....if render states set successfully
//              false....otherwise

bool CScenario::SetupRenderStates(VOID)
{
    SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD) D3DTA_DIFFUSE);
    SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD) D3DTA_DIFFUSE);

    SetTextureStageState(1, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(1, D3DTSS_COLORARG2, (DWORD) D3DTA_CURRENT);
    SetTextureStageState(1, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(1, D3DTSS_ALPHAARG2, (DWORD) D3DTA_CURRENT);

    SetTextureStageState(2, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
    SetTextureStageState(2, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);

    // default for Dx6: TCI(n) = 0
    // default for Dx7: TCI(n) = 0
    // default for Dx8: TCI(n) = n

    if (KeySet("TCI0"))
    {
        if (KeySet("WHQL"))
            WriteToLog("(WARN) TCI0 not allowed with -whql, ignoring...\n");
        else
        {
            for (int i = 0; i < 8; i++)
                SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, (DWORD) 0);
        }
    }

    return (D3D_OK == GetLastError()) ? true : false;
}

// NAME:        CreateImages()
// DESC:        create images
// INPUT:       none
// OUTPUT:      true.....if images are created successfully
//              false....otherwise

bool CScenario::CreateImages(VOID)
{
    UINT i;

    if (!m_pGradientImage && m_Flags.Check(CF_MODULATE))
    {
        m_pGradientImage = new CImage;

        if (!m_pGradientImage)
        {
            WriteToLog("Unable to create modulate image.\n");
            return false;
        }

        if (!m_pGradientImage->Load(BMP_LUMINANCE, m_fGradientAlpha))
        {
            WriteToLog("Unable to load modulate image.\n");
            return false;
        }
    }

    if (!m_pPlaneImage)
    {
        m_pPlaneImage = new CImage;

        if (!m_pPlaneImage)
        {
            WriteToLog("Unable to create plane image.\n");
            return false;
        }

        if (!m_pPlaneImage->Load(BMP_PLANE, 0.5f))
        {
            WriteToLog("Unable to load plane image.\n");
            return false;
        }
    }

    if (!m_pSphereImage)
    {
        m_pSphereImage = new CImage;

        if (!m_pSphereImage)
        {
            WriteToLog("Unable to create sphere image.\n");
            return false;
        }

        if (!m_pSphereImage->Load(BMP_SPHERE, 0.5f))
        {
            WriteToLog("Unable to load sphere image.\n");
            return false;
        }
    }

    if (!m_pMipMapImages[0])
    {
        for (i = 0; i < NLEVELS; i++)
        {
            m_pMipMapImages[i] = new CImage;

            if (!m_pMipMapImages[i])
            {
                WriteToLog("Unable to create %s image.\n", g_pszImages[i]);
                return false;
            }

            if (!m_pMipMapImages[i]->Load(g_pszImages[i], m_fMipmapAlpha))
            {
                WriteToLog("Unable to load %s image.\n", g_pszImages[i]);
                return false;
            }
        }
    }

    return true;
}

// NAME:        CreateShapes()
// DESC:        create shapes
// INPUT:       none
// OUTPUT:      true.....if shapes are created successfully
//              false....otherwise

bool CScenario::CreateShapes(VOID)
{
    UINT i;

    for (i = 0; i < NCUBES; i++)
    {
        if (!m_pCubes[i].NewShape(CS_BOX, 1, 1, 1))
        {
            WriteToLog("Unable to create cube %d.\n", i);
            return false;
        }

        m_pCubes[i].Scale(3.0f);
    }

    if (!m_Sphere.NewShape(CS_SPHERE, 32, 16, 0))
    {
        WriteToLog("Unable to create sphere.\n");
        return false;
    }

    m_Sphere.Scale(7.0);

    for (i = 0; i < NPLANES; i++)
        if (!m_pPlanes[i].NewShape(CS_MESH, 4, 4, 0))
        {
            WriteToLog("Unable to create plane %d.\n", i);
            return false;
        }

    m_pPlanes[0].Scale(3.0f, 12.0f, 1.0f);
    m_pPlanes[1].Scale(4.0f, 20.0f, 1.0f);
    m_pPlanes[2].Scale(6.0f, 35.0f, 1.0f);
    return true;
}

// NAME:        CreateTextures()
// DESC:        create textures
// INPUT:       none
// OUTPUT:      true.....if textures are created successfully
//              false....otherwise

bool CScenario::CreateTextures(VOID)
{
    DWORD dwLevels, dwPool, dwUsage, dwWidth, dwHeight;
    FORMAT *pFormat;

    WriteToLog("(INFO) Creating mipmap texture for cubes...\n");
    dwLevels = (DWORD) (m_Flags.Check(CF_MIPFILTER) ? (m_dwMinLevel - m_dwMaxLevel + 1) : 1);
    dwPool = FMWK_DEFAULT;
    dwUsage = (DWORD) 0;
    dwWidth = MAXSIZE >> m_dwMaxLevel;
    dwHeight = MAXSIZE >> m_dwMaxLevel;
    pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

    if (!CreateTexture(dwWidth, dwHeight, dwLevels, dwUsage, *pFormat, dwPool, &m_pMipMapTexture))
    {
        WriteToLog("Failed to create mipmap texture: %s.\n", GLE(GetLastError()));
        return false;
    }

    if (m_Flags.Check(CF_MIPFILTER))
    {
        if (!m_pMipMapTexture->LoadImage((DWORD) 0, &m_pMipMapImages[0]))
        {
            WriteToLog("Unable to load texture.\n");
            return false;
        }
    }
    else
    {
        if (!m_pMipMapTexture->LoadImage((DWORD) 1, &m_pMipMapImages[m_dwMaxLevel]))
        {
            WriteToLog("Unable to load mipmap texture.\n");
            return false;
        }
    }

    WriteToLog("(INFO) Creating sphere texture...\n");
    dwLevels = (DWORD) 1;
    dwPool = FMWK_DEFAULT;
    dwUsage = (DWORD) 0;
    dwWidth = (DWORD) 256;
    dwHeight = (DWORD) 256;
    pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

    if (!CreateTexture(dwWidth, dwHeight, dwLevels, dwUsage, *pFormat, dwPool, &m_pSphereTexture))
    {
        WriteToLog("Failed to create sphere texture: %s.\n", GLE(GetLastError()));
        return false;
    }

    if (!m_pSphereTexture->LoadImage(dwLevels, &m_pSphereImage))
    {
        WriteToLog("Unable to load sphere texture.\n");
        return false;
    }

    WriteToLog("(INFO) Creating plane texture...\n");
    dwLevels = (DWORD) 1;
    dwPool = FMWK_DEFAULT;
    dwUsage = (DWORD) 0;
    dwWidth = (DWORD) 16;
    dwHeight = (DWORD) 16;
    pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

    if (!CreateTexture(dwWidth, dwHeight, dwLevels, dwUsage, *pFormat, dwPool, &m_pPlaneTexture))
    {
        WriteToLog("Failed to create plane texture: %s.\n", GLE(GetLastError()));
        return false;
    }

    if (!m_pPlaneTexture->LoadImage(dwLevels, &m_pPlaneImage))
    {
        WriteToLog("Unable to load plane texture.\n");
        return false;
    }

    WriteToLog("(INFO) Creating modulate texture...\n");
    dwLevels = (DWORD) 1;
    dwPool = FMWK_DEFAULT;
    dwUsage = (DWORD) 0;
    dwWidth = (DWORD) 256;
    dwHeight = (DWORD) 256;
    pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

    if (m_Flags.Check(CF_MODULATE))
    {
        if (!CreateTexture(dwWidth, dwHeight, dwLevels, dwUsage, *pFormat, dwPool, &m_pGradientTexture))
        {
            WriteToLog("Failed to create modulate texture: %s.\n", GLE(GetLastError()));
            return false;
        }

        if (!m_pGradientTexture->LoadImage(dwLevels, &m_pGradientImage))
        {
            WriteToLog("Unable to load modulate texture.\n");
            return false;
        }
    }

    WriteToLog("(INFO) Getting default render target and z buffer...\n");

    if (!GetRenderTarget(&m_pDefTarget))
    {
        WriteToLog("Unable to get default render target: %s\n", GLE(GetLastError()));
        return false;
    }

    if (!GetDepthStencilSurface(&m_pDefZBuffer))
    {
        WriteToLog("Unable to get default z buffer: %s\n", GLE(GetLastError()));
        return false;
    }

    if (m_Flags.Check(CF_TARGET))
    {
        bool bAbort = false, bSuccess = true;
        CHAR szFormat[MAXFORMAT];
        MULTISAMPLE_TYPE Type;
        UINT i, nFormat, nTargets = KeySet("Max") ? MAXTARGET : MINTARGET;

        dwLevels = (DWORD) 1;
        dwPool = FMWK_DEFAULT;
        dwUsage = USAGE_RENDERTARGET;
        Type = MULTISAMPLE_NONE;
        nFormat = m_pCurrentMode->nTextureFormat;

        WriteToLog("(INFO) Creating texture as render target...\n");

        for (i = 0; (i < nTargets) && !m_pTexTarget; i++)
        {
            if (m_pnTargetFormats[i] != -1)
            {
                bSuccess = true;
                m_pCurrentMode->nTextureFormat = m_pnTargetFormats[i];
                pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
                FmtToString(FORMAT_TEXTURE, pFormat, szFormat);

                if (!CreateTexture(256, 256, dwLevels, dwUsage, *pFormat, dwPool, &m_pTexTarget))
                {
                    WriteToLog("(INFO) CreateTexture(%s) as render target failed: %s.\n",
                               szFormat, GLE(GetLastError()));
                    bSuccess = false;
                }
                else if (!CreateDepthStencilSurface(256, 256, FMT_D16, Type, &m_pTexZBuffer))
                {
                    WriteToLog("(INFO) CreateDepthStencilSurface() failed: %s.\n",
                               GLE(GetLastError()));
                    bSuccess = false;
                }
                else if (!ValidateTextureAsRenderTarget(&bAbort, szFormat) || bAbort)
                    bSuccess = false;
                else
                    WriteToLog("(INFO) CreateTexture(%s) as render target passed.\n", szFormat);

                if (bAbort || !bSuccess)
                {
                    RELEASE(m_pTexZBuffer);
					RELEASE(m_pTexTarget);

                    if (bAbort)
                        return false;
                }
            }
        }

        if (!m_pTexTarget)
        {
            WriteToLog("(INFO) Unable to set texture render target, disabling tests.\n");
            m_Flags.Clear(CF_TARGET);
            m_pCurrentMode->nTextureFormat = nFormat;
        }
    }

    return true;
}

// NAME:        CreateTests()
// DESC:        create tests
// INPUT:       none
// OUTPUT:      true.....if tests are created successfully
//              false....otherwise
#if FAST_TEST
bool CScenario::CreateTests(VOID)
{
    struct _Entry
    {
        DWORD dwFlags;
        DWORD dwDependency;
    } Entries[] =
    {
        //{ CF_SPECULAR,  0          },
        { CF_ALPHA,     0          },
        { CF_TEXTURE,   0          },
        //{ CF_MINFILTER, CF_TEXTURE },//Mip map test should take care of these
        //{ CF_MAGFILTER, CF_TEXTURE },
        //{ CF_MIPFILTER, CF_TEXTURE },
        //{ CF_MODULATE,  CF_TEXTURE },
        { CF_TARGET,    CF_TEXTURE },
        { CF_ZBUFFER,   0          },
        //{ CF_WBUFFER,   CF_ZBUFFER },
        { CF_FOG,       0          },
        //{ CF_DITHER,    0          },
        //{ CF_CORRECT,   0          },
    };

    //m_Tests.push_back(TESTDEF(CF_NORENDER, 0));
    m_Tests.push_back(TESTDEF(0, 0));

    DWORD dwEntries = (DWORD) (sizeof(Entries)/sizeof(_Entry));

    for (UINT i = 0; i < dwEntries; i++)
    {
        DWORD dwTests = m_Tests.size();

        for (UINT j = 0; j < dwTests; j++)
        if (!Entries[i].dwDependency || (m_Tests[j].dwFlags & Entries[i].dwDependency))
            ADD(Entries[i].dwFlags, m_Tests, j);
    }

    return true;
}
#else //FAST_TEST
bool CScenario::CreateTests(VOID)
{
    struct _Entry
    {
        DWORD dwFlags;
        DWORD dwDependency;
    } Entries[] =
    {
        { CF_SPECULAR,  0          },
        { CF_ALPHA,     0          },
        { CF_TEXTURE,   0          },
        { CF_MINFILTER, CF_TEXTURE },
        { CF_MAGFILTER, CF_TEXTURE },
        { CF_MIPFILTER, CF_TEXTURE },
        { CF_MODULATE,  CF_TEXTURE },
        { CF_TARGET,    CF_TEXTURE },
        { CF_ZBUFFER,   0          },
        { CF_WBUFFER,   CF_ZBUFFER },
        { CF_FOG,       0          },
        { CF_DITHER,    0          },
        { CF_CORRECT,   0          },
    };

    m_Tests.push_back(TESTDEF(CF_NORENDER, 0));

    for (BYTE bFrame = 0; bFrame < 8; bFrame++)
    {
        if (!KeySet("Long") && (bFrame != 2) && (bFrame != 5))
            m_Tests.push_back(TESTDEF(CF_SKIP, bFrame));
        else
            m_Tests.push_back(TESTDEF(0, bFrame));
    }

    DWORD dwEntries = (DWORD) (sizeof(Entries)/sizeof(_Entry));

    for (UINT i = 0; i < dwEntries; i++)
    {
        DWORD dwTests = m_Tests.size();

        for (UINT j = 1; j < dwTests; j++)
            if (!Entries[i].dwDependency || (m_Tests[j].dwFlags & Entries[i].dwDependency))
                ADD(Entries[i].dwFlags, m_Tests, j);
    }

    return true;
}
#endif

// NAME:        DestroyTests()
// DESC:        destroy tests
// INPUT:       none
// OUTPUT:      none

VOID CScenario::DestroyTests(VOID)
{
    m_Tests.clear();
}

// NAME:        DumpTests
// DESC:        dump tests
// INPUT:       none
// OUTPUT:      none

VOID CScenario::DumpTests(VOID)
{
    for (DWORD i = 0; i < m_Tests.size(); i++)
    {
        DescribeTest(i, m_szBuffer);
        WriteToLog("Test %06d: %s.\n", (i + 1), m_szBuffer);
    }
}

// NAME:        EliminateTests()
// DESC:        eliminate tests
// INPUT:       dwFlags..flags
// OUTPUT:      none

VOID CScenario::EliminateTests(DWORD dwFlags)
{
    WriteToLog("(INFO) Eliminating tests with flags %08X.\n", dwFlags);

    for (DWORD i = 0; i < m_Tests.size(); i++)
        if (CHECK(dwFlags, m_Tests[i].dwFlags))
            m_Tests[i].dwFlags |= CF_SKIP;
}

// NAME:        DescribeTest()
// DESC:        describe test
// INPUT:       dwTest...test number
//              pszDesc..address of null-terminated description string
// OUTPUT:

VOID CScenario::DescribeTest(DWORD dwTest, LPTSTR pszDesc)
{
    DWORD Flags = m_Tests[dwTest].dwFlags;

    wsprintf(pszDesc,
             "%1d %4s %5s %3s %3s %3s %3s %3s %2s %4s %3s %3s %6s %6s %4s",
             m_Tests[dwTest].bFrame,
             CHECK(CF_SPECULAR,  Flags) ? "SPEC"   : "",
             CHECK(CF_ALPHA,     Flags) ? "ALPHA"  : "",
             CHECK(CF_TEXTURE,   Flags) ? "TEX"    : "",
             CHECK(CF_MINFILTER, Flags) ? "MIN"    : "",
             CHECK(CF_MAGFILTER, Flags) ? "MAG"    : "",
             CHECK(CF_MIPFILTER, Flags) ? "MIP"    : "",
             CHECK(CF_MODULATE,  Flags) ? "MOD"    : "",
             CHECK(CF_TARGET,    Flags) ? "RT"     : "",
             CHECK(CF_WBUFFER,   Flags) ? "WBUF"   : (CHECK(CF_ZBUFFER, Flags) ? "ZBUF" : ""),
             CHECK(CF_FOG,       Flags) ? "FOG"    : "",
             CHECK(CF_CORRECT,   Flags) ? "CPC"    : "",
             CHECK(CF_DITHER,    Flags) ? "DITHER" : "",
             CHECK(CF_NORENDER,  Flags) ? "NOREND" : "",
             CHECK(CF_SKIP,      Flags) ? "SKIP"   : "");
}

// NAME:        WriteCurrentStatus()
// DESC:        write current status
// INPUT:       none
// OUTPUT:      none

VOID CScenario::WriteCurrentStatus(VOID)
{
    TESTDEF &Test = m_Tests[m_dwTest - 1];

    lstrcpy(m_szStatus[0][0], "$gFrame\t");
    wsprintf(m_szStatus[0][1], "$y%d", Test.bFrame);

    if (m_Flags.Check(CF_SPECULAR))
    {
        lstrcpy(m_szStatus[1][0], "$gSpecular");
        lstrcpy(m_szStatus[1][1], CHECK(CF_SPECULAR, Test.dwFlags) ? "$yEnabled" : "$rDisabled");
    }

    if (m_Flags.Check(CF_ALPHA))
    {
        lstrcpy(m_szStatus[2][0], "$gAlpha Blending");
        lstrcpy(m_szStatus[2][1], CHECK(CF_ALPHA, Test.dwFlags) ? "$yEnabled" : "$rDisabled");
    }

    if (m_Flags.Check(CF_TEXTURE))
    {
        lstrcpy(m_szStatus[3][0], "$gTexturing");
        lstrcpy(m_szStatus[3][1], CHECK(CF_TEXTURE, Test.dwFlags) ? "$yEnabled" : "$rDisabled");

        if (m_Flags.Check(CF_MINFILTER))
        {
            lstrcpy(m_szStatus[4][0], "$gMin Filter");
            lstrcpy(m_szStatus[4][1], CHECK(CF_MINFILTER, Test.dwFlags) ? "$yEnabled" : "$rDisabled");
        }

        if (m_Flags.Check(CF_MAGFILTER))
        {
            lstrcpy(m_szStatus[5][0], "$gMag Filter");
            lstrcpy(m_szStatus[5][1], CHECK(CF_MAGFILTER, Test.dwFlags) ? "$yEnabled" : "$rDisabled");
        }

        if (m_Flags.Check(CF_MIPFILTER))
        {
            lstrcpy(m_szStatus[6][0], "$gMip Filter");
            lstrcpy(m_szStatus[6][1], CHECK(CF_MIPFILTER, Test.dwFlags) ? "$yEnabled" : "$rDisabled");
        }

        if (m_Flags.Check(CF_MODULATE))
        {
            lstrcpy(m_szStatus[7][0], "$gLightmap");
            lstrcpy(m_szStatus[7][1], CHECK(CF_MODULATE, Test.dwFlags) ? "$yEnabled" : "$rDisabled");
        }

        if (m_Flags.Check(CF_TARGET))
        {
            lstrcpy(m_szStatus[8][0], "$gTexture Target");
            lstrcpy(m_szStatus[8][1], CHECK(CF_TARGET, Test.dwFlags) ? "$yEnabled" : "$rDisabled");
        }
    }

    if (m_Flags.Check(CF_ZBUFFER) || m_Flags.Check(CF_WBUFFER))
    {
        lstrcpy(m_szStatus[9][0], "$gDepth Buffer");
        lstrcpy(m_szStatus[9][1], CHECK(CF_WBUFFER, Test.dwFlags) ? "$yW-Buffer" : (CHECK(CF_ZBUFFER, Test.dwFlags) ? "$yZ-buffer" : "$rDisabled"));
    }

    if (m_Flags.Check(CF_FOG))
    {
        lstrcpy(m_szStatus[10][0], "$gFog\t");
        lstrcpy(m_szStatus[10][1], CHECK(CF_FOG, Test.dwFlags) ? "$yEnabled" : "$rDisabled");
    }

    if (m_Flags.Check(CF_DITHER))
    {
        lstrcpy(m_szStatus[11][0], "$gDithering");
        lstrcpy(m_szStatus[11][1], CHECK(CF_DITHER, Test.dwFlags) ? "$yEnabled" : "$rDisabled");
    }

    if (m_Flags.Check(CF_CORRECT))
    {
        lstrcpy(m_szStatus[12][0], "$gCorrection");
        lstrcpy(m_szStatus[12][1], CHECK(CF_CORRECT, Test.dwFlags) ? "$yEnabled" : "$rDisabled");
    }

    if (m_Flags.Check(CF_NORENDER))
    {
        lstrcpy(m_szStatus[13][0], "$gRendering");
        lstrcpy(m_szStatus[13][1], CHECK(CF_NORENDER, Test.dwFlags) ? "$rDisabled" : "$yEnabled");
    }

    lstrcpy(m_szStatus[14][0], "$gTotal Test Cases");
    wsprintf(m_szStatus[14][1], "$y%d", m_Tests.size());
}

// NAME:        InterpretTestResults()
// DESC:        interpret test results
// INPUT:       none
// OUTPUT:      none

VOID CScenario::InterpretTestResults(VOID)
{
    struct
    {
        char *szName;
        DWORD dwFlag;
        DWORD dwCount;
        DWORD dwFail;
    } Results[] =
    {
        { "Specular",                     CF_SPECULAR,  0, 0 },
        { "Alpha",                        CF_ALPHA,     0, 0 },
        { "Texturing",                    CF_TEXTURE,   0, 0 },
        { "Texture Min Filter",           CF_MINFILTER, 0, 0 },
        { "Texture Mag Filter",           CF_MAGFILTER, 0, 0 },
        { "Texture Mip Filter",           CF_MIPFILTER, 0, 0 },
        { "Modulate",                     CF_MODULATE,  0, 0 },
        { "Texture Render Target",        CF_TARGET,    0, 0 },
        { "Z Buffer",                     CF_ZBUFFER,   0, 0 },
        { "W Buffer",                     CF_WBUFFER,   0, 0 },
        { "Fog",                          CF_FOG,       0, 0 },
        { "Dithering",                    CF_DITHER,    0, 0 },
        { "Color Perspective Correction", CF_CORRECT,   0, 0 },
        { "No Rendering",                 CF_NORENDER,  0, 0 },
        { NULL,                           0,            0, 0 }
    };
    DWORD dwFailed = 0;

    for (DWORD i = 0; i < m_Tests.size(); i++)
    {
        if (m_Tests[i].dwFlags & CF_SKIP)
            continue;

        if (m_Tests[i].dwFlags & CF_FAIL)
            dwFailed++;

        int j = 0;

        while (Results[j].szName)
        {
            if (m_Tests[i].dwFlags & Results[j].dwFlag)
            {
                Results[j].dwCount++;

                if (m_Tests[i].dwFlags & CF_FAIL)
                    Results[j].dwFail++;
            }

            j++;
        }
    }

    if (!dwFailed)
        return;

    WriteToLog("Failed Tests Interpretation:\n");
    int j = 0;

    while (Results[j].szName)
    {
        float fTemp = 100.0f*Results[j].dwFail;

        WriteToLog("%32s: Total: %5d/%5d (%6.2f%%)  Feature: %5d/%5d (%6.2f%%)\n",
                   Results[j].szName,
                   Results[j].dwFail,
                   dwFailed,
                   ((dwFailed == 0) ? 0 : fTemp/(float) (dwFailed)),
                   Results[j].dwFail,
                   Results[j].dwCount,
                   ((Results[j].dwCount == 0) ? 0 : fTemp/(float) (Results[j].dwCount)));
        j++;
    }
}

// NAME:        ValidateTextureStageStates()
// DESC:        validate texture state states
// INPUT:       none
// OUTPUT:      none

VOID CScenario::ValidateTextureStageStates(VOID)
{
    HRESULT hResult;

    SetSamplerState(0, SAMP_MAGFILTER, (DWORD) TEXF_POINT);
    SetSamplerState(0, SAMP_MINFILTER, (DWORD) TEXF_POINT);
    SetSamplerState(0, SAMP_MIPFILTER, (DWORD) TEXF_NONE);

    SetTexture(0, NULL);
    SetTexture(1, NULL);

    if (m_Flags.Check(CF_TEXTURE))
    {
        SetTextureStageState(0, D3DTSS_COLOROP, (DWORD) D3DTOP_MODULATE);
        SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
        SetTexture(0, m_pMipMapTexture);

        SetTextureStageState(1, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
        SetTextureStageState(1, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);

        if (D3D_OK != (hResult = ValidateDevice()))
        {
            m_Flags.Clear(CF_TEXTURE);
            EliminateTests(CF_TEXTURE);
            WriteToLog("(WLP Failure) Modulate blend not supported: %s.\n", GLE(hResult));
            return;
        }

        if (m_Flags.Check(CF_MAGFILTER))
        {
            SetSamplerState(0, SAMP_MAGFILTER, (DWORD) TEXF_LINEAR);
            SetSamplerState(0, SAMP_MINFILTER, (DWORD) TEXF_POINT);
            SetSamplerState(0, SAMP_MIPFILTER, (DWORD) TEXF_NONE);

            if (D3D_OK != (hResult = ValidateDevice()))
            {
                m_Flags.Clear(CF_MAGFILTER);
                EliminateTests(CF_MAGFILTER);
                WriteToLog("(INFO) Linear mag filter not supported: %s.\n", GLE(hResult));
                return;
            }
        }

        if (m_Flags.Check(CF_MINFILTER))
        {
            SetSamplerState(0, SAMP_MAGFILTER, (DWORD) TEXF_POINT);
            SetSamplerState(0, SAMP_MINFILTER, (DWORD) TEXF_LINEAR);
            SetSamplerState(0, SAMP_MIPFILTER, (DWORD) TEXF_NONE);

            if (D3D_OK != (hResult = ValidateDevice()))
            {
                m_Flags.Clear(CF_MINFILTER);
                EliminateTests(CF_MINFILTER);
                WriteToLog("(INFO) Linear min filter not supported: %s.\n", GLE(hResult));
                return;
            }
        }

        if (m_Flags.Check(CF_MINFILTER))
        {
            SetSamplerState(0, SAMP_MAGFILTER, (DWORD) TEXF_POINT);
            SetSamplerState(0, SAMP_MINFILTER, (DWORD) TEXF_POINT);

            if ((m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) &&
                (m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
                SetSamplerState(0, SAMP_MIPFILTER, (DWORD) TEXF_LINEAR);
            else
                SetSamplerState(0, SAMP_MIPFILTER, (DWORD) TEXF_POINT);

            if (D3D_OK != (hResult = ValidateDevice()))
            {
                m_Flags.Clear(CF_MIPFILTER);
                EliminateTests(CF_MIPFILTER);
                WriteToLog("(INFO) Linear mip filter not supported: %s.\n", GLE(hResult));
                return;
            }
        }

        SetSamplerState(0, SAMP_MAGFILTER, (DWORD) TEXF_POINT);
        SetSamplerState(0, SAMP_MINFILTER, (DWORD) TEXF_POINT);
        SetSamplerState(0, SAMP_MIPFILTER, (DWORD) TEXF_NONE);

        if (m_Flags.Check(CF_MODULATE))
        {
            SetTextureStageState(0, D3DTSS_COLOROP, (DWORD) D3DTOP_SELECTARG1);
            SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);

            SetTextureStageState(1, D3DTSS_COLOROP, (DWORD) D3DTOP_MODULATE);
            SetTextureStageState(1, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG2);
            SetTexture(1, m_pGradientTexture);

            if (D3D_OK != (hResult = ValidateDevice()))
            {
                m_Flags.Clear(CF_MODULATE);
                EliminateTests(CF_MODULATE);
                WriteToLog("(INFO) Modulate scenario not supported: %s.\n", GLE(hResult));
                return;
            }
        }
    }
}

// NAME:        ValidateTextureAsRenderTarget()
// DESC:        validate texture as render target
// INPUT:       pbAbort......flag indicating current render target is not valid
//              pszFormat....address of null terminated format string
// OUTPUT:      true.....if texture can be used as a render target
//              false....otherwise

bool CScenario::ValidateTextureAsRenderTarget(bool *pbAbort, LPTSTR pszFormat)
{
    bool bValid = true;
    CnSurface *pTarget = NULL;

    *pbAbort = false;

    if (!m_pTexTarget->GetSurfaceLevel(0, &pTarget))
    {
        WriteToLog("GetSurfaceLevel() failed.\n");
        *pbAbort = true;
        return false;
    }

    if (!SetRenderTarget(pTarget, m_pTexZBuffer))
    {
        WriteToLog("(INFO) SetRenderTarget() failed (%s texture render target): %s.\n",
                   pszFormat, GLE(GetLastError()));
        bValid = false;
    }

    RELEASE(pTarget);

    if (!SetRenderTarget(m_pDefTarget, m_pDefZBuffer))
    {
        WriteToLog("SetRenderTarget() failed (default render target): %s.\n", GLE(GetLastError()));
        *pbAbort = true;
    }

    return bValid;
}

// NAME:        CheckRefreshRate()
// DESC:        check refresh rate (verifies adapter handles 85 Hz refresh rate or higher for WLP)
// INPUT:       none
// OUTPUT:      true.....if adapter handles 85 Hz or more
//              false....otherwise

bool CScenario::CheckRefreshRate(VOID)
{
    __int64 st, ft, freq;
    int nOldThreadPriority;
    BOOL bBlank = FALSE;
    DWORD dwOldClass;
    HANDLE hProcess, hThread;
    HRESULT hResult;
    USHORT usCount = (USHORT) 0;
    ULONG ulRate;

    if (!GetVerticalBlankStatus(&bBlank))
    {
        hResult = GetLastError();
 
        if (E_NOTIMPL == hResult) 
        {
            WriteToLog("Unable to measure refresh rate thru interfaces greater than Dx7.\n");
            return false;
        }

        if (hResult != DD_OK)
        {
            WriteToLog("GetVerticalBlankStatus FAILED: %s.\n", GLE(hResult));
            return false;
        }
    }

    hProcess = GetCurrentProcess();
    dwOldClass = GetPriorityClass(hProcess);

    // boost thread priority and process class for loops

    SetPriorityClass(hProcess, /* HIGH_PRIORITY_CLASS */ REALTIME_PRIORITY_CLASS);
    hThread = GetCurrentThread();
    nOldThreadPriority = GetThreadPriority(hThread);
    SetThreadPriority(hThread, /* THREAD_PRIORITY_HIGHEST */ THREAD_PRIORITY_TIME_CRITICAL);

    if (!QueryPerformanceCounter((LARGE_INTEGER *) &st))
    {
        WriteToLog("QueryPerformanceCounter failed.\n");
        SetThreadPriority(hThread, nOldThreadPriority);
        SetPriorityClass(hProcess, dwOldClass);
        return false;
    }

    // time how long it takes to switch between blank to draw 250 times

    do
    {
        do
        {
            GetVerticalBlankStatus(&bBlank);
        } while (bBlank);

        do
        {
            GetVerticalBlankStatus(&bBlank);
        } while (!bBlank);

        usCount++;
    } while (usCount != 250);

    QueryPerformanceCounter((LARGE_INTEGER *) &ft);

    if (!QueryPerformanceFrequency((LARGE_INTEGER *) &freq))
    {
        WriteToLog("QueryPerformanceCounter failed.\n");
        SetThreadPriority(hThread, nOldThreadPriority);
        SetPriorityClass(hProcess, dwOldClass);
        return false;
    }

    SetThreadPriority(hThread, nOldThreadPriority);
    SetPriorityClass(hProcess, dwOldClass);

    #pragma warning (disable :4244)

    ulRate = freq / ((ft - st) / 250);

    if (ulRate > 300)
    {
        WriteToLog("GetVerticalBlankStatus FAILED!!!\n");
        return false;
    }

    #pragma warning (default :4244)

    WriteToLog("(INFO) Measured refresh rate was %d Hz.\n", (USHORT) ulRate);
    return (((USHORT) ulRate) >= 83 ? true : false);
}

// NAME:        BitCount()
// DESC:        bit count
// INPUT:       dwMask...bit mask
// OUTPUT:      dwBits...number of bits in bit mask

DWORD CScenario::BitCount(DWORD dwMask)
{
    DWORD dwBits = (DWORD) 0, dwTemp = dwMask;

    if (dwTemp != 0)
    {
        while (0 == (dwTemp & 1))
            dwTemp >>= 1;

        while (dwTemp & 1)
        {
            dwTemp >>= 1;
            dwBits++;
        }
    }

    return dwBits;
}

// NAME:        ValidateDevice()
// DESC:        validate device
// INPUT:       none
// OUTPUT:      hResult..return value from ValidateDevice() call thru src or ref

HRESULT CScenario::ValidateDevice(VOID)
{
    DWORD dwPasses;
    HRESULT hSrc = D3D_OK, hRef = D3D_OK;

    dwPasses = (DWORD) 0xDEADBEEF;

    if (!CD3DTest::ValidateDevice(&dwPasses, CD3D_SRC) || (dwPasses > 1))
    {
        hSrc = GetLastError();
        WriteToLog("Src requires 0x%08X passes: %s.\n", dwPasses, GLE(hSrc));
    }

    dwPasses = (DWORD) 0xDEADBEEF;

    if (!CD3DTest::ValidateDevice(&dwPasses, CD3D_REF) || (dwPasses > 1))
    {
        hRef = GetLastError();
        WriteToLog("Ref requires 0x%08X passes: %s.\n", dwPasses, GLE(hRef));
    }

    return ((D3D_OK == hSrc) ? hRef : hSrc);
}
