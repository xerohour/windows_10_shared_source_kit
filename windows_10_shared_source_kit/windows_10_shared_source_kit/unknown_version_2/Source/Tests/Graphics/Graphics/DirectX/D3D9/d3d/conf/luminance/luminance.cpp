// FILE:        luminance.cpp
// DESC:        luminance class methods for [alpha]luminance texture format conformance tests
// AUTHOR:      Todd M. Frost

#include <math.h>
#include "luminance.h"

CD3DWindowFramework App;
CLuminanceAddress   LuminanceAddress;

UINT guBumps = (UINT) DEFAULT_BUMPS; // global helper variable for fAlpha(), fLuminance()

// NAME:        fAlpha()
// DESC:        alpha
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      f = sin(u)*sin(v)
// COMMENTS:    (u,v) scaled from [0,0]x[1,1] to [-guBumps*pi/2,guBumps*pi/2]
//              [0,1] maps to [0,255] in LoadAlgorithms() for unsigned channels (8 bit)

FLOAT fAlpha(FLOAT u, FLOAT v)
{
    FLOAT fTemp, x, y;

    fTemp = (FLOAT) (guBumps*pi);
    x = fTemp*(u - 0.5f);
    y = fTemp*(v - 0.5f);
    return (FLOAT) (0.5f*(sin(x)*sin(y) + 1.0f));
}

#define P0 1.0f
#define P1 -1.0f
#define P2 1.0f
#define P3 -1.0f

// NAME:        fLuminance()
// DESC:        luminance
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      bilinear blend:
//                  (1 - v)[(1 - u)*P0 + u*P3] + v*[(1 - u)*P1 + u*P2]
//                    P1 P2
//                  v P0 P3
//                  + u
// COMMENTS:    [0,1] maps to [0,255] in LoadAlgorithms() for unsigned channels (8 bit)

FLOAT fLuminance(FLOAT u, FLOAT v)
{
    FLOAT s = 1.0f - u;

    return (FLOAT) (0.5f*((1 - v)*(s*P0 + u*P3) + v*(s*P1 + u*P2) + 1.0f));
}

#undef P0
#undef P1
#undef P2
#undef P3

// NAME:        CLuminance()
// DESC:        constructor for luminance class
// INPUT:       none
// OUTPUT:      none

CLuminance::CLuminance(VOID)
{
    SetTestType(TESTTYPE_CONF);

    m_szTestName = "Luminance";
    m_szCommandKey = "Luminance";

    lstrcpy(m_szBaseFormat, "NA");
    lstrcpy(m_szLuminanceFormat, "NA");
    ClearStatus();

    m_dwModeCapsVals[0][0] = D3DPTADDRESSCAPS_BORDER;
    m_dwModeCapsVals[0][1] = D3DTADDRESS_BORDER;
    m_dwModeCapsVals[1][0] = D3DPTADDRESSCAPS_CLAMP;
    m_dwModeCapsVals[1][1] = D3DTADDRESS_CLAMP;
    m_dwModeCapsVals[2][0] = D3DPTADDRESSCAPS_MIRROR;
    m_dwModeCapsVals[2][1] = D3DTADDRESS_MIRROR;
    m_dwModeCapsVals[3][0] = D3DPTADDRESSCAPS_WRAP;
    m_dwModeCapsVals[3][1] = D3DTADDRESS_WRAP;

    m_dwModes[0] = DEFAULT_MODE;
    m_dwModes[1] = DEFAULT_MODE;

    m_dwCommonLuminanceFormats = (DWORD) 0;
    m_dwBaseFormat = NOFORMAT;
    m_dwLuminanceFormat = NOFORMAT;
    m_dwStages = (DWORD) 1;
    m_dwStagesMax = (DWORD) 0;

    m_pBaseImage = NULL;
    m_pLuminanceImage = NULL;

    m_pBaseTexture = NULL;
    m_pLuminanceTexture = NULL;

    m_Options.ModeOptions.dwTexInclude = (PF_RGB | PF_LUMINANCE);
    m_Options.ModeOptions.CubeFmt = FMT_ANY;
    m_Options.ModeOptions.VolumeFmt = FMT_ANY;
    m_Options.ModeOptions.ZBufferFmt = FMT_ANY;

    m_Options.D3DOptions.bZBuffer = true;
    m_Options.D3DOptions.fMinDXVersion = 6.0f;
}

// NAME:        ~CLuminance()
// DESC:        destructor for luminance class
// INPUT:       none
// OUTPUT:      none

CLuminance::~CLuminance(VOID)
{
    SAFEDELETE(m_pBaseImage);
    SAFEDELETE(m_pLuminanceImage);
    RELEASE(m_pBaseTexture);
    RELEASE(m_pLuminanceTexture);
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CLuminance::CommandLineHelp(VOID)
{
    WriteCommandLineHelp("$ybumps: $wbumps per texture $c(default=8)");
    WriteCommandLineHelp("$yvalidate: $wvalidate $c(On/$wOff$c)");
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      true.....if caps check is successful
//              false....otherwise

bool CLuminance::CapsCheck(VOID)
{
    DWORD dwCount = (DWORD) 0, dwFlags = (DWORD) 0;
    UINT i;

    ProcessArgs();

    // scan for base texture (non-palettized rgb texture with largest number of green bits)

    for (i = 0; i < m_uCommonTextureFormats; i++)
    {
        dwFlags = m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags;

        if ((dwFlags & PF_RGB) &&
            !((dwFlags & PF_ALPHAPIXELS) ||
              (dwFlags & PF_PALETTEINDEXED1) ||
              (dwFlags & PF_PALETTEINDEXED2) ||
              (dwFlags & PF_PALETTEINDEXED4) ||
              (dwFlags & PF_PALETTEINDEXED8) ||
              (dwFlags & PF_PALETTEINDEXEDTO8)))
        if ((m_pCommonTextureFormats[i].ddpfPixelFormat.dwRBitMask > 0) &&
            (m_pCommonTextureFormats[i].ddpfPixelFormat.dwBBitMask > 0) &&
            (dwCount < GetCountFromMask(m_pCommonTextureFormats[i].ddpfPixelFormat.dwGBitMask)))
        {
            dwCount = GetCountFromMask(m_pCommonTextureFormats[i].ddpfPixelFormat.dwGBitMask);
            m_dwBaseFormat = i;
        }
    }

    if (0 == dwCount)
    {
        WriteToLog("No common non-palettized RGB formats enumerated.\n");
        return false;
    }

    // scan for [alpha]luminance textures

    m_dwCommonLuminanceFormats = (DWORD) 0;

    for (i = 0; i < m_uCommonTextureFormats; i++)
        if (m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags & PF_LUMINANCE)
            m_dwCommonLuminanceFormats++;

    if (0 == m_dwCommonLuminanceFormats)
    {
        WriteToLog("No common [alpha]luminance formats enumerated.\n");
        return false;
    }

    // check caps

    m_dwStagesMax = (DWORD) m_pSrcDevice->GetCaps()->wMaxTextureBlendStages;
    m_dwStagesMax = (DWORD) ((m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures < 2) ? 1 : m_dwStagesMax);
    m_dwTexAddressCaps = m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureAddressCaps;
    m_dwTexOpCaps = m_pSrcDevice->GetCaps()->dwTextureOpCaps;

    if (m_dwTexAddressCaps & D3DPTADDRESSCAPS_INDEPENDENTUV)
        m_Flags.Set(CF_INDEPENDENT);

    return true;
}

// NAME:        Setup()
// DESC:        setup
// INPUT:       none
// OUTPUT:      true.....if setup is successful
//              false....otherwise

bool CLuminance::Setup(VOID)
{
	if( !CD3DTest::Setup() )
		return false;
		
    if (!m_pBaseImage)
    {
        DWORD pdwColors[] =
        {
            RGBA_MAKE( 255, 255, 255, 255),
            RGBA_MAKE( 255,   0,   0, 255),
            RGBA_MAKE(   0, 255,   0, 255),
            RGBA_MAKE(   0,   0, 255, 255)
        };


        m_pBaseImage = new CImage;

        if (!m_pBaseImage)
        {
            WriteToLog("Unable to create base image.\n");
            return false;
        }

        if (!m_pBaseImage->LoadGradient(TEXWIDTH, TEXHEIGHT, pdwColors))
        {
            WriteToLog("Unable to load base image.\n");
            SAFEDELETE(m_pBaseImage);
            return false;
        }        
    }

    if (!m_pLuminanceImage)
    {
        CHANNELFUNCS Data = { 0 };
        Data.fAlpha = fAlpha;
        Data.fRed = NULL;
        Data.fGreen = fLuminance;
        Data.fBlue = NULL;

        m_pLuminanceImage = new CImage;

        if (!m_pLuminanceImage)
        {
            WriteToLog("Unable to create luminance image.\n");
            return false;
        }

        if (!m_pLuminanceImage->LoadAlgorithms(TEXWIDTH, TEXHEIGHT, &Data))
        {
            WriteToLog("Unable to load luminance image.\n");
            SAFEDELETE(m_pLuminanceImage);
            return false;
        }
    }

    m_pCurrentMode->nTextureFormat = m_dwBaseFormat;
    DWORD dwLevels = (DWORD) 1;
    DWORD dwPool = FMWK_DEFAULT;
    DWORD dwUsage = (DWORD) 0;
    FORMAT *pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
    FmtToString(FORMAT_TEXTURE, pFormat, m_szBaseFormat);

    if (!CreateTexture(TEXWIDTH, TEXHEIGHT, dwLevels, dwUsage, *pFormat, dwPool, &m_pBaseTexture))
    {
        WriteToLog("Unable to create base texture.\n");
        return false;
    }

    if (!m_pBaseTexture->LoadImage(dwLevels, &m_pBaseImage))
    {
        WriteToLog("Unable to load base texture.\n");
        return false;
    }

    return true;
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CLuminance::TestInitialize(VOID)
{
    m_dwModes[0] = DEFAULT_MODE;
    m_dwModes[1] = DEFAULT_MODE;

    m_dwLuminanceFormat = NOFORMAT;

    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CLuminance::ExecuteTest(UINT uTest)
{
    bool bResult = true, bValid = true;
    int iStage;
    CHAR pszMode[NMODES][7] =
    {
        "wrap",
        "mirror",
        "clamp",
        "border"
    };
    CHAR pszStages[NSTAGES][33] =
    {
        "1 stage, blend(diffuse:texture)",
        "2 stage, blend(gradient:texture)",
        "3 stage, blend(gradient:texture)"
    };
    D3DCOLOR pdwColors[NSTAGES] =
    {
        RGBA_MAKE(  0,   0, 255, 255),
        RGBA_MAKE(  0, 255,   0, 255),
        RGBA_MAKE(  0, 255, 255,   0)
    };
    D3DTEXTUREOP dwOp = D3DTOP_MODULATE;

    if (m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].ddpfPixelFormat.dwFlags & PF_ALPHAPIXELS)
        if (m_dwTexOpCaps & D3DTEXOPCAPS_BLENDTEXTUREALPHA)
            dwOp = D3DTOP_BLENDTEXTUREALPHA;

    // variation independent, texture stage state settings

    for (iStage = 0; iStage < 3; iStage++)
    {
        if (m_Flags.Check(CF_INDEPENDENT))
        {
            SetSamplerState(iStage, SAMP_ADDRESSU, (DWORD) m_dwModes[0]);
            SetSamplerState(iStage, SAMP_ADDRESSV, (DWORD) m_dwModes[1]);
        }
        else
        {
            SetSamplerState(iStage, SAMP_ADDRESSU, (DWORD) m_dwModes[0]);
            SetSamplerState(iStage, SAMP_ADDRESSV, (DWORD) m_dwModes[0]);
        }

        SetSamplerState(iStage, SAMP_BORDERCOLOR, pdwColors[iStage]);
        SetSamplerState(iStage, SAMP_MAGFILTER, (DWORD) TEXF_LINEAR);
        SetSamplerState(iStage, SAMP_MINFILTER, (DWORD) TEXF_LINEAR);
        SetSamplerState(iStage, SAMP_MIPFILTER, (DWORD) TEXF_NONE);
        SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
        SetTextureStageState(iStage, D3DTSS_COLORARG2, (DWORD) D3DTA_CURRENT);
        SetTextureStageState(iStage, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
        SetTextureStageState(iStage, D3DTSS_ALPHAARG2, (DWORD) D3DTA_CURRENT);
        SetTexture(iStage, NULL);
    }
    
    // variation dependent, texture stage state settings

    iStage = 0;

    switch (m_dwStages)
    {
        case 1:
            SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) dwOp);
            SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
            SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 1);
            SetTexture(iStage, m_pLuminanceTexture);
            iStage++;
        break;

        case 2:
            SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_MODULATE);
            SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
            SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 0);
            SetTexture(iStage, m_pBaseTexture);
            iStage++;

            SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) dwOp);
            SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
            SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 1);
            SetTexture(iStage, m_pLuminanceTexture);
            iStage++;
        break;
 
        case 3:
            SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_SELECTARG1);
            SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
            SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 0);
            SetTexture(iStage, m_pBaseTexture);
            iStage++;

            SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) dwOp);
            SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
            SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 1);
            SetTexture(iStage, m_pLuminanceTexture);
            iStage++;

            SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_MODULATE);
            SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_DIFFUSE);
            SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG2);
            iStage++;
        break;
    }

    SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
    SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);

    wsprintf(m_szBuffer, "%s, %s (%s,%s)", m_szLuminanceFormat,
                                           pszStages[m_dwStages - 1],
                                           pszMode[m_dwModes[0] - 1], pszMode[m_dwModes[1] - 1]);
    BeginTestCase(m_szBuffer, uTest);

    if (m_Flags.Check(CF_VALIDATE))
    {
        DWORD dwPasses;

        dwPasses = (DWORD) 0xDEADBEEF;

        if (!ValidateDevice(&dwPasses, CD3D_SRC) || (dwPasses > 1))
        {
            WriteToLog("Src requires 0x%08X passes:  %s.\n", dwPasses, GLE(GetLastError()));
            bValid = false;
        }

        dwPasses = (DWORD) 0xDEADBEEF;
		
		// BUGBUG - Why do I have to do this?
		// While fixing this code to correctly handle lost device I found that leaving
		// the following call out resulted in ValidateDevice() failing with the debug
		// error message "Invalid Vertex Declaration".
		if( m_pSrcDevice->GetInterface() >= 8.0 )
			SetFVF( FLEXFMT );    

        if (!ValidateDevice(&dwPasses, CD3D_REF) || (dwPasses > 1))
        {
            WriteToLog("Ref requires 0x%08X passes:  %s.\n", dwPasses, GLE(GetLastError()));
            bValid = false;
        }
    }

    if (!bValid)
    {
        SkipTests((UINT) 1);
        return false;
    }

    if (BeginScene())
    {
        D3DCOLOR dcDiffuse = RGB_MAKE(255, 255, 255);
        FLEXVERT pVertices[4];

        pVertices[0].fPx = 0.0f;
        pVertices[0].fPy = (D3DVALUE) m_pCurrentMode->nBackBufferHeight;
        pVertices[0].fPz = 0.5f;
        pVertices[0].fRHW = 2.0f;
        pVertices[0].dcDiffuse = dcDiffuse;
        pVertices[0].fSu = -1.0f;
        pVertices[0].fSv = -1.0f;
        pVertices[0].fTu = -1.0f;
        pVertices[0].fTv = -1.0f;

        pVertices[1].fPx = 0.0f;
        pVertices[1].fPy = 0.0f;
        pVertices[1].fPz = 0.5f;
        pVertices[1].fRHW = 2.0f;
        pVertices[1].dcDiffuse = dcDiffuse;
        pVertices[1].fSu = -1.0f;
        pVertices[1].fSv = 2.0f;
        pVertices[1].fTu = -1.0f;
        pVertices[1].fTv = 2.0f;

        pVertices[2].fPx = (D3DVALUE) m_pCurrentMode->nBackBufferWidth;
        pVertices[2].fPy = 0.0f;
        pVertices[2].fPz = 0.5f;
        pVertices[2].fRHW = 2.0f;
        pVertices[2].dcDiffuse = dcDiffuse;
        pVertices[2].fSu = 2.0f;
        pVertices[2].fSv = 2.0f;
        pVertices[2].fTu = 2.0f;
        pVertices[2].fTv = 2.0f;

        pVertices[3].fPx = (D3DVALUE) m_pCurrentMode->nBackBufferWidth;
        pVertices[3].fPy = (D3DVALUE) m_pCurrentMode->nBackBufferHeight;
        pVertices[3].fPz = 0.5f;
        pVertices[3].fRHW = 2.0f;
        pVertices[3].dcDiffuse = dcDiffuse;
        pVertices[3].fSu = 2.0f;
        pVertices[3].fSv = -1.0f;
        pVertices[3].fTu = 2.0f;
        pVertices[3].fTv = -1.0f;

        if (!RenderPrimitive(D3DPT_TRIANGLEFAN, FLEXFMT, (LPVOID) pVertices, 4, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() failed: %s.\n", GLE(GetLastError()));
            Fail();
            bResult = false;
        }

        if (!EndScene())
        {
            WriteToLog("EndScene() failed: %s.\n", GLE(GetLastError()));
            Fail();
            bResult = false;
        }
    }
    else
    {
        WriteToLog("BeginScene() failed: %s.\n", GLE(GetLastError()));
        Fail();
        bResult = false;
    }

    sprintf(m_szStatus[0][0], "$gBase Fmt");
    sprintf(m_szStatus[0][1], "$y%s", m_szBaseFormat);
    sprintf(m_szStatus[1][0], "$gLuminance Fmt"); 
    sprintf(m_szStatus[1][1], "$y%s", m_szLuminanceFormat);
    sprintf(m_szStatus[2][0], "$gMode\t");
    sprintf(m_szStatus[2][1], "$y%s", pszStages[m_dwStages - 1]);
    sprintf(m_szStatus[3][0], "$gU Mode\t");
    sprintf(m_szStatus[3][1], "$y%s", pszMode[m_dwModes[0] - 1]);
    sprintf(m_szStatus[4][0], "$gV Mode\t");
    sprintf(m_szStatus[4][1], "$y%s", pszMode[m_dwModes[1] - 1]);
    return bResult;
}

// NAME:        Cleanup()
// DESC:        cleanup
// INPUT:       none
// OUTPUT:      none

VOID CLuminance::Cleanup(VOID)
{
    RELEASE(m_pBaseTexture);
    RELEASE(m_pLuminanceTexture);
    m_dwLuminanceFormat = NOFORMAT;
    
    CD3DTest::Cleanup();
}

// NAME:        UpdateStatus()
// DESC:        update status
// INPUT:       none
// OUTPUT:      none

VOID CLuminance::UpdateStatus(VOID)
{
    CD3DTest::UpdateStatus();

    for (UINT i = 0; i < NSTATUS; i++)
        WriteStatus(m_szStatus[i][0], m_szStatus[i][1]);
}

// NAME:        ClearStatus()
// DESC:        clear status
// INPUT:       none
// OUTPUT:      none

VOID CLuminance::ClearStatus(VOID)
{
    for (UINT i = 0; i < NSTATUS; i++)
    {
        sprintf(m_szStatus[i][0], "$gNA\t");
        sprintf(m_szStatus[i][1], "$y-");
    }
}

// NAME:        SetLuminanceTexture()
// DESC:        set luminance texture
// INPUT:       dwFormat.....texture format
//              dwWidth......texture width
//              dwHeight.....texture height
// OUTPUT:      true.....if requested format is set successfully
//              false....otherwise

bool CLuminance::SetLuminanceTexture(DWORD dwFormat, DWORD dwWidth, DWORD dwHeight)
{
    UINT i, j;

    if (dwFormat == m_dwLuminanceFormat)
        return true;

    if (dwFormat >= m_dwCommonLuminanceFormats)
        return false;

    for (i = 0, j = 0; i < m_uCommonTextureFormats; i++)
    {
        if (m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags & PF_LUMINANCE)
        {
            if (j == dwFormat)
                break;

            j++;
        }
    }

    if (j != dwFormat)
    {
        WriteToLog("No such [alpha]luminance format: %d\n", dwFormat);
        return false;
    }

    DWORD dwLevels = (DWORD) 1;
    DWORD dwPool = FMWK_DEFAULT;
    DWORD dwUsage = (DWORD) 0;
    m_pCurrentMode->nTextureFormat = i;
    FORMAT *pFormat = &m_pCommonTextureFormats[i];
    FmtToString(FORMAT_TEXTURE, pFormat, m_szLuminanceFormat);
    RELEASE(m_pLuminanceTexture);

    if (!CreateTexture(dwWidth, dwHeight, dwLevels, dwUsage, *pFormat, dwPool, &m_pLuminanceTexture))
    {
        WriteToLog("Unable to create [alpha]luminance texture.\n");
        return false;
    }

    if (!m_pLuminanceTexture->LoadImage(dwLevels, &m_pLuminanceImage))
    {
        WriteToLog("Unable to load luminance texture.\n");
        return false;
    }

    m_dwLuminanceFormat = dwFormat;
    return true;
}

// NAME:        GetCountFromMask()
// DESC:        get count from mask
// INPUT:       dwMask...bit mask
// OUTPUT:      dwCount...bit count

DWORD CLuminance::GetCountFromMask(DWORD dwMask)
{
    DWORD dwCount, dwTemp;

    for (dwCount = (DWORD) 0, dwTemp = dwMask; dwTemp; dwTemp >>= 1) if (dwTemp & 1) dwCount++;
    return dwCount;
}

// NAME:        ProcessArgs()
// DESC:        process args
// INPUT:       none
// OUTPUT:      none

VOID CLuminance::ProcessArgs(VOID)
{
    int iDefault, iResult;

    // bumps
    // default:  DEFAULT_BUMPS

    iDefault = (int) DEFAULT_BUMPS;
    ReadInteger("bumps", iDefault, &iResult);

    if (iResult < MIN_BUMPS)
        guBumps = (UINT) MIN_BUMPS;
    else if (iResult > MAX_BUMPS)
        guBumps = (UINT) MAX_BUMPS;
    else
        guBumps = (UINT) iResult;

    // validate
    // default:  true

    if (!KeySet("!validate"))
        m_Flags.Set(CF_VALIDATE);
}
