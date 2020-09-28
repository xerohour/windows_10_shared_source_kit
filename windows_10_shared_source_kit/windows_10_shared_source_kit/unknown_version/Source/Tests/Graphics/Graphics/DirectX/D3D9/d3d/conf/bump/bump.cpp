// FILE:        bump.cpp
// DESC:        bump class methods for
//                  D3DTOP_BUMPENVMAP
//                  D3DTOP_BUMPENVMAPLUMINANCE
//              conformance tests
// AUTHOR:      Todd M. Frost

#include <math.h>
#include "bump.h"

CD3DWindowFramework App;
CBumpAddress        BumpAddress;
CBumpBlend          BumpBlend;
CBumpMatrix         BumpMatrix;
CBumpMix            BumpMix;
CBumpOffset         BumpOffset;
CBumpScale          BumpScale;

UINT guBumps = (UINT) DEFAULT_BUMPS;

// NAME:        fBumpDu()
// DESC:        bumpenvmap du
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      fx = df/dx = cos(x)*sin(y)
// COMMENTS:    (u,v) scaled from [0,0]x[1,1] to [-guBumps*pi/2,guBumps*pi/2]
//              [0,1] maps to [-127,127] in LoadAlgorithms() for signed channels (8 bits)

FLOAT fBumpDu(FLOAT u, FLOAT v)
{
    FLOAT fTemp, x, y;

    fTemp = (FLOAT) (guBumps*pi); 
    x = fTemp*(u - 0.5f);
    y = fTemp*(v - 0.5f);
    return (FLOAT) (0.5f*(cos(x)*sin(y) + 1.0f));
}

// NAME:        fBumpDv()
// DESC:        bumpenvmap dv
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      fy = df/dy = sin(x)*cos(y)
// COMMENTS:    (u,v) scaled from [0,0]x[1,1] to [-guBumps*pi/2,guBumps*pi/2]
//              [0,1] maps to [-127,127] in LoadAlgorithms() for signed channels (8 bits)

FLOAT fBumpDv(FLOAT u, FLOAT v)
{
    FLOAT fTemp, x, y;

    fTemp = (FLOAT) (guBumps*pi); 
    x = fTemp*(u - 0.5f);
    y = fTemp*(v - 0.5f);
    return (FLOAT) (0.5f*(sin(x)*cos(y) + 1.0f));
}

// NAME:        fBumpLuminance()
// DESC:        bumpenvmap luminance
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      f= 0.2*sin(x)*sin(y)
// COMMENTS:    (u,v) scaled from [0,0]x[1,1] to [-guBumps*pi/2,guBumps*pi/2]
//              [0,1] maps to [0,255] in LoadAlgorithms() for unsigned channels (8 bits)

FLOAT fBumpLuminance(FLOAT u, FLOAT v)
{
    FLOAT fTemp, x, y;

    fTemp = (FLOAT) (guBumps*pi);
    x = fTemp*(u - 0.5f);
    y = fTemp*(v - 0.5f);
    return (FLOAT) (0.5f*(sin(x)*sin(y) + 1.0f));
}

// NAME:        CBump()
// DESC:        constructor for bump class
// INPUT:       none
// OUTPUT:      none

CBump::CBump(VOID)
{
    SetTestType(TESTTYPE_CONF);

    m_szTestName = "Bump";
    m_szCommandKey = "Bump";

    lstrcpy(m_szBaseFormat, "NA");
    lstrcpy(m_szBumpFormat, "NA");
    ClearStatus();

    m_dwModeCapsVals[0][0] = D3DPTADDRESSCAPS_BORDER;
    m_dwModeCapsVals[0][1] = D3DTADDRESS_BORDER;
    m_dwModeCapsVals[1][0] = D3DPTADDRESSCAPS_CLAMP;
    m_dwModeCapsVals[1][1] = D3DTADDRESS_CLAMP;
    m_dwModeCapsVals[2][0] = D3DPTADDRESSCAPS_MIRROR;
    m_dwModeCapsVals[2][1] = D3DTADDRESS_MIRROR;
    m_dwModeCapsVals[3][0] = D3DPTADDRESSCAPS_WRAP;
    m_dwModeCapsVals[3][1] = D3DTADDRESS_WRAP;

    m_dwBaseFormat = NOFORMAT;
    m_dwBumpFormat = NOFORMAT;
    m_dwCommonBumpFormats = (DWORD) 0;
    m_dwInclusion = (DWORD) 0;

    m_pBaseImage = NULL;
    m_pBumpImage = NULL;

    m_pBaseTexture = NULL;
    m_pBumpTexture = NULL;

    m_Options.ModeOptions.CubeFmt = FMT_ANY;
    m_Options.ModeOptions.VolumeFmt = FMT_ANY;
    m_Options.ModeOptions.ZBufferFmt = FMT_ANY;
    m_Options.ModeOptions.dwTexInclude = (PF_RGB | PF_BUMPDUDV);

	// Force test to use relevant DirectX versions for WHQL
	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_7;

    m_Options.D3DOptions.bZBuffer = true;
    m_Options.D3DOptions.fMinDXVersion = 6.0f;
}

// NAME:        ~CBump()
// DESC:        destructor for bump class
// INPUT:       none
// OUTPUT:      none

CBump::~CBump(VOID)
{
    SAFEDELETE(m_pBaseImage);
    SAFEDELETE(m_pBumpImage);
    RELEASE(m_pBaseTexture);
    RELEASE(m_pBumpTexture);
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CBump::CommandLineHelp(VOID)
{
    WriteCommandLineHelp("$ybumps: $wbumps per texture $c(default=6)");
    WriteCommandLineHelp("$yvalidate: $wvalidate $c(On/$wOff$c)");
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      true.....if caps check is successful
//              false....otherwise

bool CBump::CapsCheck(VOID)
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

    // scan for bump textures

    m_dwCommonBumpFormats = (DWORD) 0;

    for (i = 0; i < m_uCommonTextureFormats; i++)
    {
#ifdef FAST_TEST
		if (m_pCommonTextureFormats[i].d3dfFormat != D3DFMT_Q8W8V8U8)
			continue;
#endif

		// For Dx9 use CheckDeviceFormat()
		if (m_dwVersion >= 0x0900)
		{
			HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
														USAGE_QUERY_LEGACYBUMPMAP, RTYPE_TEXTURE, m_pCommonTextureFormats[i]);
			if (D3D_OK == Result)
			{
				dwFlags = m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags;

				if ((dwFlags & m_dwInclusion) == m_dwInclusion)
					m_dwCommonBumpFormats++;
			}
		}
		// For pre-Dx9 just check the flags of the pixel format
		else
		{
			dwFlags = m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags;

			if ((dwFlags & m_dwInclusion) == m_dwInclusion)
				m_dwCommonBumpFormats++;
		}
    }

    if (0 == m_dwCommonBumpFormats)
    {
		if (m_dwVersion >= 0x0900)
	        WriteToLog("CheckDeviceFormat(Query_LegacyBumpMap) did not find any formats available.\n");
		else
			WriteToLog("No common BUMPDUDV formats enumerated.\n");

        return false;
    }

    // check caps and log cap/texture enumeration mismatches

    m_Flags.Clear(CF_SYSTEM_ALLOWED | CF_SYSTEM_CREATED);

    if (m_pSrcDevice->GetCaps()->dwDevCaps & D3DDEVCAPS_TEXTURESYSTEMMEMORY)
        m_Flags.Set(CF_SYSTEM_ALLOWED);

    if (m_pSrcDevice->GetCaps()->wMaxTextureBlendStages < 2)
    {
        WriteToLog("Device does NOT support sufficient number of texture stages (2).\n");
        return false;
    }

    m_dwTexAddressCaps = m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureAddressCaps;
    m_dwTexOpCaps = m_pSrcDevice->GetCaps()->dwTextureOpCaps;

    if (!(m_dwTexOpCaps & D3DTEXOPCAPS_BUMPENVMAP))
        WriteToLog("BUMPDUDV textures enumerated, D3DTOP_BUMPENVMAP not supported.\n");

    if (!(m_dwTexOpCaps & D3DTEXOPCAPS_BUMPENVMAPLUMINANCE))
        for (i = 0; i < m_uCommonTextureFormats; i++)
            if (m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags & PF_BUMPLUMINANCE)
                WriteToLog("BUMPLUMINANCE textures enumerated, D3DTOP_BUMPENVMAPLUMINANCE not supported.\n");

    if (m_dwTexAddressCaps & D3DPTADDRESSCAPS_INDEPENDENTUV)
        m_Flags.Set(CF_INDEPENDENT);

    return true;
}

// NAME:        Setup()
// DESC:        setup
// INPUT:       none
// OUTPUT:      true.....if setup is successful
//              false....otherwise

bool CBump::Setup(VOID)
{
    if (!m_pBaseImage)
    {
        D3DCOLOR pdwColors[] =
        {
            RGBA_MAKE(255, 255, 255, 255),
            RGBA_MAKE(255,   0,   0, 255),
            RGBA_MAKE(  0, 255,   0, 255),
            RGBA_MAKE(  0,   0, 255, 255)
        };
        DWORD dwCount = sizeof(pdwColors)/sizeof(D3DCOLOR);

        m_pBaseImage = new CImage;

        if (!m_pBaseImage)
        {
            WriteToLog("Unable to create base image.\n");
            return false;
        }

        if (!m_pBaseImage->LoadStripes(TEXWIDTH, TEXHEIGHT, dwCount, pdwColors, true, true))
        {
            WriteToLog("Unable to load base image.\n");
            SAFEDELETE(m_pBaseImage);
            return false;
        }
    }

    if (!m_pBumpImage)
    {
        CHANNELFUNCS Data = { 0 };
        Data.fAlpha = fBumpLuminance;
        Data.fRed   = fBumpDu;
        Data.fGreen = fBumpDv;
        Data.fBlue  = NULL;

        m_pBumpImage = new CImage;

        if (!m_pBumpImage)
        {
            WriteToLog("Unable to create bump image.\n");
            return false;
        }

        if (!m_pBumpImage->LoadAlgorithms(TEXWIDTH, TEXHEIGHT, &Data))
        {
            WriteToLog("Unable to load bump image.\n");
            SAFEDELETE(m_pBumpImage);
            return false;
        }
    }

    m_pCurrentMode->nTextureFormat = m_dwBaseFormat;
    DWORD dwLevels = (DWORD) 1;
    DWORD dwPool = FMWK_DEFAULT;
    DWORD dwUsage = (DWORD) 0;
    FORMAT *pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
    FmtToString(FORMAT_TEXTURE, pFormat, m_szBaseFormat);

    if (!(CreateTexture(TEXWIDTH, TEXHEIGHT, dwLevels, dwUsage, *pFormat, dwPool, &m_pBaseTexture)))
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

UINT CBump::TestInitialize(VOID)
{
    m_dwModes[0] = DEFAULT_MODE;
    m_dwModes[1] = DEFAULT_MODE;

    m_dwBumpFormat = NOFORMAT;

    m_fOffset.f = DEFAULT_OFFSET;
    m_fScale.f = DEFAULT_SCALE;

    SetBumpMatrix((DWORD) MATRIX_SCALE_DUDV, DEFAULT_MAG, DEFAULT_MAG);

    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CBump::ExecuteTest(UINT uTest)
{
    bool bResult = true, bSkip = false, bValid = true;
	bool bFailedSetTexture = false;
    int iStage = (int) 0;
    CHAR szBlend[NBLENDS+1][11] =
    {
        "MODULATE",
        "MODULATE2X",
        "MODULATE4X",
        "SELECTARG1"
    };
    CHAR szMatrixFormat[NMATRICES][14] =
    {
        "scale Du",
        "scale Dv",
        "scale DuDv",
        "skew Du",
        "skew Dv",
        "skew DuDv"
    };
    CHAR szMode[NMODES][7] =
    {
        "wrap",
        "mirror",
        "clamp",
        "border"
    };
    D3DCOLOR dwColor;
    D3DTEXTUREOP dwOp;
    UINT uBlend;

    if (m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].ddpfPixelFormat.dwFlags & PF_BUMPLUMINANCE)
        dwOp = D3DTOP_BUMPENVMAPLUMINANCE;
    else
        dwOp = D3DTOP_BUMPENVMAP;

    dwColor = RGBA_MAKE(128, 128, 128, 255);    // (Du,Dv,Lu) = (0.0,0.0,0.5)
    SetSamplerState(iStage, SAMP_BORDERCOLOR, dwColor);

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

    SetSamplerState(iStage, SAMP_MAGFILTER, (DWORD) TEXF_LINEAR);
    SetSamplerState(iStage, SAMP_MINFILTER, (DWORD) TEXF_LINEAR);
    SetSamplerState(iStage, SAMP_MIPFILTER, (DWORD) TEXF_NONE);

    SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) dwOp);
    SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_COLORARG2, (DWORD) D3DTA_DIFFUSE);
    SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
    SetTextureStageState(iStage, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_ALPHAARG2, (DWORD) D3DTA_DIFFUSE);
    SetTextureStageState(iStage, D3DTSS_BUMPENVMAT00, m_fMatrix[0][0].dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVMAT01, m_fMatrix[0][1].dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVMAT10, m_fMatrix[1][0].dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVMAT11, m_fMatrix[1][1].dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVLOFFSET, m_fOffset.dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVLSCALE, m_fScale.dw);
    SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 1);

	if (m_Flags.Check(CF_SYSTEM_CREATED) && !m_Flags.Check(CF_SYSTEM_ALLOWED))
    {
		// invalid parameter scenario blocked by driver/runtime at set texture time
		// Win7: 569610
        bSkip = true;
    }

	if (!SetTexture(iStage, m_pBumpTexture))
    {
		WriteToLog("Failed to SetTexture: m_pBumpTexture (0x%I64x). CF_SYSTEM_CREATED = %d, CF_SYSTEM_ALLOWED = %d\n", (UINT64)m_pBumpTexture, m_Flags.Check(CF_SYSTEM_CREATED), m_Flags.Check(CF_SYSTEM_ALLOWED));
        if (m_Flags.Check(CF_SYSTEM_CREATED) && !m_Flags.Check(CF_SYSTEM_ALLOWED))
			WriteToLog("This is expected because the D3DDEVCAPS_TEXTURESYSTEMMEMORY cap is not set.\n");

		bFailedSetTexture = true;
    }

    iStage++;

    if (m_Flags.Check(CF_MODULATE))
    {
        dwOp = D3DTOP_MODULATE;
        uBlend = (UINT) 0;
    }
    else if (m_Flags.Check(CF_MODULATE2X))
    {
        dwOp = D3DTOP_MODULATE2X;
        uBlend = (UINT) 1;
    }
    else if (m_Flags.Check(CF_MODULATE4X))
    {
        dwOp = D3DTOP_MODULATE4X;
        uBlend = (UINT) 2;
    }
    else
    {
        dwOp = D3DTOP_SELECTARG1;
        uBlend = (UINT) 3;
    }

    dwColor = RGBA_MAKE(0, 0, 255, 255);
    SetSamplerState(iStage, SAMP_BORDERCOLOR, dwColor);

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

    SetSamplerState(iStage, SAMP_MAGFILTER, (DWORD) TEXF_LINEAR);
    SetSamplerState(iStage, SAMP_MINFILTER, (DWORD) TEXF_LINEAR);
    SetSamplerState(iStage, SAMP_MIPFILTER, (DWORD) TEXF_NONE);

    SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) dwOp);
    SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_COLORARG2, (DWORD) D3DTA_CURRENT);
    SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
    SetTextureStageState(iStage, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_ALPHAARG2, (DWORD) D3DTA_CURRENT);
    SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 0);

    if (!SetTexture(iStage, m_pBaseTexture))
    {
		WriteToLog("Failed to SetTexture: m_pBaseTexture (0x%I64x).\n", (UINT64)m_pBaseTexture);
        bFailedSetTexture = true;
	}

    iStage++;

    SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
    SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);

    sprintf(m_szBuffer, "%s, %s (%s,%s)", m_szBumpFormat,
                                          szBlend[uBlend],
                                          szMode[m_dwModes[0] - 1], szMode[m_dwModes[1] - 1]);
    BeginTestCase(m_szBuffer, uTest);

    // skip delayed until BeginTestCase() called

    if (bSkip)
    {
        SkipTests((UINT) 1);
        return false;
    }

	if (bFailedSetTexture)
	{
		Fail();
		return false;
	}

    if (m_Flags.Check(CF_VALIDATE))
    {
        DWORD dwPasses;

        dwPasses = (DWORD) 0xDEADBEEF;

        if (!ValidateDevice(&dwPasses, CD3D_SRC) || (dwPasses > 1))
        {
            WriteToLog("Src requires 0x%08X passes: %s.\n", dwPasses, GLE(GetLastError()));
            bValid = false;
        }

        dwPasses = (DWORD) 0xDEADBEEF;

        if (!ValidateDevice(&dwPasses, CD3D_REF) || (dwPasses > 1))
        {
            WriteToLog("Ref requires 0x%08X passes: %s.\n", dwPasses, GLE(GetLastError()));
            bValid = false;
        }
    }

    if (!bValid)
    {
        if (m_Flags.Check(CF_SYSTEM_CREATED) && m_Flags.Check(CF_SYSTEM_ALLOWED))
        {
            WriteToLog("System memory texture used, D3DDEVCAPS_TEXTURESYSTEMMEMORY set.\n");
            WriteToLog("Validation must pass or D3DDEVCAPS_TEXTURESYSTEMMEMORY must be unset.\n");
            Fail();
        }
        else
        {
            SkipTests((UINT) 1);
            return false;
        }
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

    sprintf(m_szStatus[0][0], "$gBase Format");
    sprintf(m_szStatus[0][1], "$y%s", m_szBaseFormat);
    sprintf(m_szStatus[1][0], "$gBump Format");
    sprintf(m_szStatus[1][1], "$y%s", m_szBumpFormat);
    sprintf(m_szStatus[2][0], "$gMatrix\t");
    sprintf(m_szStatus[2][1], "$y%s", szMatrixFormat[m_dwMatrixFormat]);
    sprintf(m_szStatus[3][0], "$gOffset\t");
    sprintf(m_szStatus[3][1], "$y%f", m_fOffset.f);
    sprintf(m_szStatus[4][0], "$gScale\t");
    sprintf(m_szStatus[4][1], "$y%f", m_fScale.f);
    sprintf(m_szStatus[5][0], "$gBlend\t");
    sprintf(m_szStatus[5][1], "$y%s", szBlend[uBlend]);
    sprintf(m_szStatus[6][0], "$gU Mode\t");
    sprintf(m_szStatus[6][1], "$y%s", szMode[m_dwModes[0] - 1]);
    sprintf(m_szStatus[7][0], "$gV Mode\t");
    sprintf(m_szStatus[7][1], "$y%s", szMode[m_dwModes[1] - 1]);
    return bResult;
}

// NAME:        Cleanup()
// DESC:        cleanup
// INPUT:       none
// OUTPUT:      none

VOID CBump::Cleanup(VOID)
{
    RELEASE(m_pBaseTexture);
    RELEASE(m_pBumpTexture);
    m_dwBumpFormat = NOFORMAT;
}

// NAME:        UpdateStatus()
// DESC:        update status
// INPUT:       none
// OUTPUT:      none

VOID CBump::UpdateStatus(VOID)
{
    CD3DTest::UpdateStatus();

    for (UINT i = 0; i < NSTATUS; i++)
        WriteStatus(m_szStatus[i][0], m_szStatus[i][1]);
}

// NAME:        ClearStatus()
// DESC:        clear status
// INPUT:       none
// OUTPUT:      none

VOID CBump::ClearStatus(VOID)
{
    for (UINT i = 0; i < NSTATUS; i++)
    {
        sprintf(m_szStatus[i][0], "$gNA\t");
        sprintf(m_szStatus[i][1], "$y-");
    }
}

// NAME:        SetBumpTexture()
// DESC:        set bump texture
// INPUT:       dwFormat.....texture format
//              dwWidth......texture width
//              dwHeight.....texture height
// OUTPUT:      true.....if requested format is set successfully
//              false....otherwise

bool CBump::SetBumpTexture(DWORD dwFormat, DWORD dwWidth, DWORD dwHeight)
{
    DWORD dwFlags;
    UINT i, j;

    if (dwFormat == m_dwBumpFormat)
        return true;

    if (dwFormat >= m_dwCommonBumpFormats)
        return false;

    for (i = 0, j = 0; i < m_uCommonTextureFormats; i++)
    {
        dwFlags = m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags;
        
        if ((dwFlags & m_dwInclusion) == m_dwInclusion)
        {
            if (j == dwFormat)
                break;

            j++;
        }
    }

    if (j != dwFormat)
    {
        WriteToLog("No such bump format: %d\n", dwFormat);
        return false;
    }

    DWORD dwLevels = (DWORD) 1;
    DWORD dwPool = m_Flags.Check(CF_SYSTEM_CREATED) ? POOL_SYSTEMMEM : FMWK_DEFAULT;
    DWORD dwUsage = (DWORD) 0;
    m_pCurrentMode->nTextureFormat = i;
    FORMAT *pFormat = &m_pCommonTextureFormats[i];
    FmtToString(FORMAT_TEXTURE, pFormat, m_szBumpFormat);
    RELEASE(m_pBumpTexture);

    if (!CreateTexture(dwWidth, dwHeight, dwLevels, dwUsage, *pFormat, dwPool, &m_pBumpTexture))
    {
        WriteToLog("Unable to create bump texture.\n");
        return false;
    }

    if (!m_pBumpTexture->LoadImage(dwLevels, &m_pBumpImage))
    {
        WriteToLog("Unable to load bump texture.\n");
        return false;
    }

    m_dwBumpFormat = dwFormat;
    return true;
}

// NAME:        SetBumpMatrix()
// DESC:        set bump matrix
// INPUT:       dwType...matrix type
//              fDu......du
//              fDv......dv
// OUTPUT:      none

VOID CBump::SetBumpMatrix(DWORD dwType, FLOAT fDu, FLOAT fDv)
{
    switch (dwType)
    {
        case MATRIX_SCALE_DU:
            m_fMatrix[0][0].f = fDu;
            m_fMatrix[0][1].f = 0.0f;
            m_fMatrix[1][0].f = 0.0f;
            m_fMatrix[1][1].f = 0.0f;
            m_dwMatrixFormat = MATRIX_SCALE_DU;
        break;

        case MATRIX_SCALE_DV:
            m_fMatrix[0][0].f = 0.0f;
            m_fMatrix[0][1].f = 0.0f;
            m_fMatrix[1][0].f = 0.0f;
            m_fMatrix[1][1].f = fDv;
            m_dwMatrixFormat = MATRIX_SCALE_DV;
        break;

        case MATRIX_SCALE_DUDV:
        default:
            m_fMatrix[0][0].f = fDu;
            m_fMatrix[0][1].f = 0.0f;
            m_fMatrix[1][0].f = 0.0f;
            m_fMatrix[1][1].f = fDv;
            m_dwMatrixFormat = MATRIX_SCALE_DUDV;
        break;

        case MATRIX_SKEW_DU:
            m_fMatrix[0][0].f = 0.0f;
            m_fMatrix[0][1].f = fDu;
            m_fMatrix[1][0].f = 0.0f;
            m_fMatrix[1][1].f = 0.0f;
            m_dwMatrixFormat = MATRIX_SKEW_DU;
        break;

        case MATRIX_SKEW_DV:
            m_fMatrix[0][0].f = 0.0f;
            m_fMatrix[0][1].f = 0.0f;
            m_fMatrix[1][0].f = fDv;
            m_fMatrix[1][1].f = 0.0f;
            m_dwMatrixFormat = MATRIX_SKEW_DV;
        break;

        case MATRIX_SKEW_DUDV:
            m_fMatrix[0][0].f = 0.0f;
            m_fMatrix[0][1].f = fDu;
            m_fMatrix[1][0].f = fDv;
            m_fMatrix[1][1].f = 0.0f;
            m_dwMatrixFormat = MATRIX_SKEW_DUDV;
        break;
    }
}

// NAME:        GetCountFromMask()
// DESC:        get count from mask 
// INPUT:       dwMask...bit mask
// OUTPUT:      dwCount...bit count

DWORD CBump::GetCountFromMask(DWORD dwMask)
{
    DWORD dwCount, dwTemp;

    for (dwCount = (DWORD) 0, dwTemp = dwMask; dwTemp; dwTemp >>= 1) if (dwTemp & 1) dwCount++;
    return dwCount;
}

// NAME:        Normalize()
// DESC:        normalize distance of dwCurrent along [dwMin, dwMax]
// INPUT:       dwCurrent....current interval location
//              dwMin........min interval endpoint
//              dwMax........max interval endpoint
// OUTPUT:      fRatio.......normalized distance of dwCurrent along [dwMin, dwMax]

FLOAT CBump::Normalize(DWORD dwCurrent, DWORD dwMin, DWORD dwMax)
{
    DWORD dwClamped;
    FLOAT fTemp = 0.0f;

    if (dwMin == dwMax)
        return fTemp;

    if (dwCurrent > dwMax)
        dwClamped = dwMax;
    else if (dwCurrent < dwMin)
        dwClamped = dwMin;
    else
        dwClamped = dwCurrent;

    fTemp = ((FLOAT) (dwClamped - dwMin)) / ((FLOAT) (dwMax - dwMin));
    return fTemp;
}

// NAME:        ProcessArgs()
// DESC:        process args
// INPUT:       none
// OUTPUT:      none

VOID CBump::ProcessArgs(VOID)
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

    // variations
    // default:  DEFAULT_VARIATIONS

    iDefault = (int) DEFAULT_VARIATIONS;
    ReadInteger("variations", iDefault, &iResult);

    if (iResult < MIN_VARIATIONS)
        m_uVariations = (UINT) MIN_VARIATIONS;
    else if (iResult > MAX_VARIATIONS)
        m_uVariations = (UINT) MAX_VARIATIONS;
    else
        m_uVariations = (UINT) iResult;

    // validate
    // default:  true

    if (!KeySet("!validate"))
        m_Flags.Set(CF_VALIDATE);
}
