// FILE:        lightmap.cpp
// DESC:        lightmap class methods for D3DTOP_DOTPRODUCT3 conformance test
// AUTHOR:      Todd M. Frost

#include <math.h>
#include "lightmap.h"
#include "global.h"

bool gbSouthern = false;

// NAME:        fGetVectorCoord()
// DESC:        get vector coord from hemisphere vector field
// INPUT:       ci...coord index
//              u....u texture coord
//              v....v texture coord
// OUTPUT:      fVector[coord]
// COMMENTS:    vector field based on sphere equation:
//                  x**2 + y**2 + z**2 = r**2
//              where
//                  x = u - r
//                  y = v - r
//                  z = sqrt(r**2 - x**2 - y**2)
//                  z = 0 if x**2 + y**2 > r**2 
//
//              result in [-1,1] mapped to [0,1] (see fVectorX() and fVectorY() COMMENTS)

FLOAT fGetVectorCoord(COORDINDEX ci, FLOAT u, FLOAT v)
{
    double fTemp;
    double fVector[3];

    fVector[CIX] = u - 0.5;
    fVector[CIY] = v - 0.5;
    fVector[CIZ] = 0.0;

    fTemp = fVector[CIX]*fVector[CIX] + fVector[CIY]*fVector[CIY];

    if (fTemp < ZERO)
    {
        fVector[CIX] = 0.0;
        fVector[CIY] = 0.0;
        fVector[CIZ] = RADIUS;
    }
    else if (fTemp < RADIUS_SQUARED)
        fVector[CIZ] = sqrt(RADIUS_SQUARED - fTemp);
    
    if (gbSouthern)
        fVector[CIZ] = -fVector[CIZ];

    fTemp = sqrt(fTemp + fVector[CIZ]*fVector[CIZ]);

    return (FLOAT) (0.5f*(fVector[ci]/fTemp + 1.0f));
}

// NAME:        fVectorX()
// DESC:        vector field x coord 
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      fGetVectorCoord(CIX, u, v)
// COMMENTS:    [0,1] maps to [0,255] in LoadAlgorithms() for unsigned channels (8 bit)

FLOAT fVectorX(FLOAT u, FLOAT v)
{
    return fGetVectorCoord(CIX, u, v);
}

// NAME:        fVectorY()
// DESC:        vector field y coord
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      fGetVectorCoord(CIY, u, v)
// COMMENTS:    [0,1] maps to [0,255] in LoadAlgorithms() for unsigned channels (8 bit)

FLOAT fVectorY(FLOAT u, FLOAT v)
{
    return fGetVectorCoord(CIY, u, v);
}

// NAME:        fVectorZ()
// DESC:        vector field z coord
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      fGetVectorCoord(CIZ, u, v)
// COMMENTS:    [0,1] maps to [0,255] in LoadAlgorithms() for unsigned channels (8 bit)

FLOAT fVectorZ(FLOAT u, FLOAT v)
{
    return fGetVectorCoord(CIZ, u, v);
}

// NAME:        CLightmap()
// DESC:        constructor for lightmap class
// INPUT:       none
// OUTPUT:      none

CLightmap::CLightmap(VOID)
{
    SetTestType(TESTTYPE_CONF);

    m_szTestName="Lightmap";
    m_szCommandKey="Lightmap";

    lstrcpy(m_szFormat, "NA");
    ClearStatus();

    m_fFactor = 0.0f;

    m_dwFormat = NOFORMAT;

    m_dwIntervals = (DWORD) 0;
    m_dwOp = (DWORD) 0;
    m_dwStages = (DWORD) 1;

    m_pNorthernImage = NULL;
    m_pSouthernImage = NULL;

    m_pNorthernTexture = NULL;
    m_pSouthernTexture = NULL;

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
    m_Options.D3DOptions.fMinDXVersion = 6.0f;
}

// NAME:        ~CLightmap()
// DESC:        destructor for lightmap class
// INPUT:       none
// OUTPUT:      none

CLightmap::~CLightmap(VOID)
{
    SAFEDELETE(m_pNorthernImage)
    SAFEDELETE(m_pSouthernImage)
    RELEASE(m_pNorthernTexture);
    RELEASE(m_pSouthernTexture);
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CLightmap::CommandLineHelp(VOID)
{
    WriteCommandLineHelp("$yalpha:  $wuse alpha blending $c($wOn$c/Off)");
    WriteCommandLineHelp("$yalphars:  $wuse alpha blending thru render state $c($wOn$c/Off)");
    WriteCommandLineHelp("$yfactor:  $wuse texture factor $c($wOn$c/Off)");
    WriteCommandLineHelp("$ymix:  $wmix sysmem/vidmem textures $c($wOn$c/Off)");
    WriteCommandLineHelp("$ymodulate:  $wuse D3DTOP_MODULATE (requires -mix) $c($wOn$c/Off)");
    WriteCommandLineHelp("$ymagnitude: $wlemniscate = sin(magnitude*theta) $c(default=2)");
    WriteCommandLineHelp("$yintervals: $wintervals per test $c(default=8)");
    WriteCommandLineHelp("$yvalidate: $wvalidate $c(On/$wOff$c)");
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      true.....if caps check is successful
//              false....otherwise

bool CLightmap::CapsCheck(VOID)
{
    ProcessArgs();

    if (0 == m_uCommonTextureFormats)
    {
        WriteToLog("No non-palettized RGB texture formats enumerated.\n");
        return false;
    }

    if (m_pSrcDevice->GetCaps()->dwDevCaps & D3DDEVCAPS_TEXTURESYSTEMMEMORY)
        m_Flags.Set(CF_SYSTEM_ALLOWED);

    m_dwStages = (DWORD) ((m_pSrcDevice->GetCaps()->wMaxTextureBlendStages < 2) ? 1 : 2);
    m_dwStages = (DWORD) ((m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures < 2) ? 1 : m_dwStages);

    if (m_Flags.Check(CF_MIX))
    {
        if (m_pSrcDevice->GetCaps()->dwTextureOpCaps &  D3DTEXOPCAPS_DOTPRODUCT3)
            m_dwOp = m_Flags.Check(CF_MODULATE) ? D3DTOP_MODULATE : D3DTOP_DOTPRODUCT3;
        else
            m_dwOp = D3DTOP_MODULATE;
    }
    else
    {
        if (m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3)
            m_dwOp = D3DTOP_DOTPRODUCT3;
        else
        {
            WriteToLog("Device does NOT support D3DTOP_DOTPRODUCT3.\n");
            return false;
        }
    }

    if (!(m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_BLENDCURRENTALPHA))
        m_Flags.Set(CF_ALPHARS);

    return true;
}

// NAME:        Setup()
// DESC:        setup
// INPUT:       none
// OUTPUT:      true.....if setup is successful
//              false....otherwise

bool CLightmap::Setup(VOID)
{
    CHANNELFUNCS Data = { 0 };
    Data.fAlpha = NULL;
    Data.fRed = fVectorX;
    Data.fGreen = fVectorY;
    Data.fBlue = fVectorZ;

    if (!m_pNorthernImage)
    {
        m_pNorthernImage = new CImage;

        if (!m_pNorthernImage)
        {
            WriteToLog("Unable to create northern image.\n");
            return false;
        }

        gbSouthern = false;

        if (!m_pNorthernImage->LoadAlgorithms(TEXWIDTH, TEXHEIGHT, &Data))
        {
            WriteToLog("Unable to load northern image: %s\n");
            SAFEDELETE(m_pNorthernImage);
            return false;
        }
    }

    if (!m_pSouthernImage)
    {
       m_pSouthernImage = new CImage;

        if (!m_pSouthernImage)
        {
            WriteToLog("Unable to create southern image.\n");
            return false;
        }

        gbSouthern = true;

        if (!m_pSouthernImage->LoadAlgorithms(TEXWIDTH, TEXHEIGHT, &Data))
        {
            WriteToLog("Unable to load southern image: %s\n");
            SAFEDELETE(m_pSouthernImage);
            return false;
        }
    }

    return true;
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CLightmap::TestInitialize(VOID)
{
    m_dwFormat = NOFORMAT;
#ifdef FAST_TEST
	m_uCommonTextureFormats = 1;
#endif
    SetTestRange((UINT) 1, (UINT) (m_dwStages*m_uCommonTextureFormats*(m_dwIntervals + 1)));
    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise
// COMMENTS:    diffuse color used as normal for D3DTOP_DOTPRODUCT3
//              note:   [  0,127] maps to [-1.0, 0.0]
//                      [128,255] maps to [ 0.0, 1.0]

bool CLightmap::ExecuteTest(UINT uTest)
{
	bool bFailedSetTexture = false;
    bool bFound = false, bResult = true, bSkip = false, bValid = true;
    int iStage = (DWORD) 0;
    CHAR szStages[NSTAGES][34] = {
        "one stage, blend(diffuse:texture)",
        "two stage, blend(texture:texture)"
    };
    DOUBLE fRadius, fTheta = 0.0;
    DOUBLE pfOffset[2];
    DWORD dwArg = (DWORD) (m_Flags.Check(CF_FACTOR) ? D3DTA_TFACTOR : D3DTA_CURRENT);
    DWORD dwFormat = (DWORD) 0, dwStages;
    UINT i, j, k, uPoint = (UINT) 0, uTemp = (UINT) 1;
	bool bPointFilter;

    for (i = 1; (i <= m_dwStages) && !bFound; i++)
        for (j = 0; (j < m_uCommonTextureFormats) && !bFound; j++)
            for (k = 0; (k <= m_dwIntervals) && !bFound; k++, uTemp++)
                if (uTemp == uTest)
                {
                    dwStages = i;
                    dwFormat = j;
                    uPoint = k;
                    bFound = true;
                }

    if (!bFound)
        return false;

    // compute texture coord offset from scaled lemniscate:  radius = sin(2*theta)

    if (m_dwIntervals > 0)
    {
        fTheta = (2.0f*pi*uPoint)/((DOUBLE) m_dwIntervals);
        fTheta = (fTheta > 2.0f*pi) ? (fTheta - 2.0f*pi) : fTheta;
    }

    fRadius = LEMNISCATE_SCALE*sin(m_fFactor*fTheta);
    pfOffset[0] = -fRadius*cos(fTheta);
    pfOffset[1] = -fRadius*sin(fTheta);

	bPointFilter = false;

	if (m_dwVersion >= 0x0900)
	{
		if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
											 USAGE_QUERY_FILTER, RTYPE_TEXTURE, m_pCommonTextureFormats[dwFormat])))
		{
			bPointFilter = true;
		}
	}

    if (dwFormat != m_dwFormat)
    {
        DWORD dwLevels = (DWORD) 1;
        DWORD dwPool = FMWK_DEFAULT;
        DWORD dwUsage = (DWORD) 0;

        m_dwFormat = dwFormat; 
        m_pCurrentMode->nTextureFormat = dwFormat;
        FORMAT *pFormat = &m_pCommonTextureFormats[dwFormat];
        FmtToString(FORMAT_TEXTURE, pFormat, m_szFormat);
        RELEASE(m_pNorthernTexture);
        RELEASE(m_pSouthernTexture);

        m_Flags.Clear(CF_SYSTEM_NORTH);
        dwPool = m_Flags.Check(CF_MIX) ? (!(dwFormat & 1) ? POOL_SYSTEMMEM: FMWK_DEFAULT) : dwPool;

        if (POOL_SYSTEMMEM == dwPool)
            m_Flags.Set(CF_SYSTEM_NORTH);

        if (!CreateTexture(TEXWIDTH, TEXHEIGHT, dwLevels, dwUsage, *pFormat, dwPool, &m_pNorthernTexture))
        {
            WriteToLog("Unable to create northern texture: %s\n", GLE(GetLastError()));
            return false;
        }

        m_Flags.Clear(CF_SYSTEM_SOUTH);
        dwPool = m_Flags.Check(CF_MIX) ? ((dwFormat & 1) ? POOL_SYSTEMMEM: FMWK_DEFAULT) : dwPool;

        if (POOL_SYSTEMMEM == dwPool)
            m_Flags.Set(CF_SYSTEM_SOUTH);

        if (!CreateTexture(TEXWIDTH, TEXHEIGHT, dwLevels, dwUsage, *pFormat, dwPool, &m_pSouthernTexture))
        {
            WriteToLog("Unable to create southern texture: %s\n", GLE(GetLastError()));
            return false;
        }

        if (!m_pNorthernTexture->LoadImage(dwLevels, &m_pNorthernImage))
        {
            WriteToLog("Unable to load northern texture.\n");
            return false;
        }

        if (!m_pSouthernTexture->LoadImage(dwLevels, &m_pSouthernImage))
        {
            WriteToLog("Unable to load southern texture.\n");
            return false;
        }
    }

    if (m_Flags.Check(CF_ALPHA) && (m_Flags.Check(CF_ALPHARS) || (dwStages < 2)))
    {
        SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD) TRUE);
        SetRenderState(D3DRS_SRCBLEND, (DWORD) D3DBLEND_INVSRCALPHA);
    }
    else
    {
        SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD) FALSE);
        SetRenderState(D3DRS_SRCBLEND, (DWORD) D3DBLEND_ONE);
    }

    DWORD dwFactor = (DWORD) ((dwStages < 2) ? RGB_POSITIVEZ : RGB_NEGATIVEZ);
    SetRenderState(D3DRS_TEXTUREFACTOR, dwFactor);

    SetSamplerState(iStage, SAMP_ADDRESSU, (DWORD) D3DTADDRESS_CLAMP);
    SetSamplerState(iStage, SAMP_ADDRESSV, (DWORD) D3DTADDRESS_CLAMP);
    SetSamplerState(iStage, SAMP_MAGFILTER, (DWORD) (bPointFilter) ? TEXF_POINT : TEXF_LINEAR);
    SetSamplerState(iStage, SAMP_MINFILTER, (DWORD) (bPointFilter) ? TEXF_POINT : TEXF_LINEAR);
    SetSamplerState(iStage, SAMP_MIPFILTER, (DWORD) TEXF_NONE);

    if ((m_Flags.Check(CF_ALPHA) && !m_Flags.Check(CF_ALPHARS)) || (dwStages < 2))
        SetTextureStageState(iStage, D3DTSS_COLOROP, m_dwOp);
    else
        SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_SELECTARG1);

    SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_COLORARG2, dwArg);

    SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG2);
    SetTextureStageState(iStage, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_ALPHAARG2, (DWORD) D3DTA_CURRENT);

    SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 0);
    
	if (m_Flags.Check(CF_SYSTEM_NORTH) && !m_Flags.Check(CF_SYSTEM_ALLOWED))
    {
		// invalid parameter scenario blocked by driver/runtime at set texture time
		// Win7: 569610
        bSkip = true;
    }

    if (!SetTexture(iStage, m_pNorthernTexture))
    {
		WriteToLog("Failed to SetTexture: m_pNorthernTexture (0x%I64x). CF_SYSTEM_NORTH = %d, CF_SYSTEM_ALLOWED = %d\n", (UINT64)m_pNorthernTexture, m_Flags.Check(CF_SYSTEM_NORTH), m_Flags.Check(CF_SYSTEM_ALLOWED));
        if (m_Flags.Check(CF_SYSTEM_NORTH) && !m_Flags.Check(CF_SYSTEM_ALLOWED))
			WriteToLog("This is expected because the D3DDEVCAPS_TEXTURESYSTEMMEMORY cap is not set.\n");

		bFailedSetTexture = true;
    }

    iStage++;

    if (dwStages > 1)
    {
        SetSamplerState(iStage, SAMP_ADDRESSU, (DWORD) D3DTADDRESS_WRAP);
        SetSamplerState(iStage, SAMP_ADDRESSV, (DWORD) D3DTADDRESS_WRAP);
        SetSamplerState(iStage, SAMP_MAGFILTER, (DWORD) (bPointFilter) ? TEXF_POINT : TEXF_LINEAR);
        SetSamplerState(iStage, SAMP_MINFILTER, (DWORD) (bPointFilter) ? TEXF_POINT : TEXF_LINEAR);
        SetSamplerState(iStage, SAMP_MIPFILTER, (DWORD) TEXF_NONE);

        if (m_Flags.Check(CF_ALPHA) && !m_Flags.Check(CF_ALPHARS))
        {
            SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_BLENDCURRENTALPHA);
            SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
            SetTextureStageState(iStage, D3DTSS_COLORARG2, (DWORD) D3DTA_CURRENT);
        }
        else
        {
            SetTextureStageState(iStage, D3DTSS_COLOROP, m_dwOp);
            SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
            SetTextureStageState(iStage, D3DTSS_COLORARG2, dwArg);
        }

        SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG2);
        SetTextureStageState(iStage, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
        SetTextureStageState(iStage, D3DTSS_ALPHAARG2, (DWORD) D3DTA_CURRENT);

        SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 1);

		if (!m_Flags.Check(CF_SYSTEM_ALLOWED) && m_Flags.Check(CF_SYSTEM_SOUTH))
		{
			// invalid parameter scenario blocked by driver/runtime at set texture time
			// Win7: 569610
			bSkip = true;
		}

        if (!SetTexture(iStage, m_pSouthernTexture))
        {
            WriteToLog("Failed to SetTexture: m_pSouthernTexture (0x%I64x). CF_SYSTEM_SOUTH = %d, CF_SYSTEM_ALLOWED = %d\n", (UINT64)m_pSouthernTexture, m_Flags.Check(CF_SYSTEM_SOUTH), m_Flags.Check(CF_SYSTEM_ALLOWED));
			if (m_Flags.Check(CF_SYSTEM_SOUTH) && !m_Flags.Check(CF_SYSTEM_ALLOWED))
				WriteToLog("This is expected because the D3DDEVCAPS_TEXTURESYSTEMMEMORY cap is not set.\n");

			bFailedSetTexture = true;
        }

        iStage++;
    }
    
    SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
    SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);

    lstrcpy(m_szBuffer, szStages[dwStages - 1]);
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
            WriteToLog("Src requires 0x%08X passes:  %s.\n", dwPasses, GLE(GetLastError()));
            bValid = false;
        }

        dwPasses = (DWORD) 0xDEADBEEF;

        if (!ValidateDevice(&dwPasses, CD3D_REF) || (dwPasses > 1))
        {
            WriteToLog("Ref requires 0x%08X passes:  %s.\n", dwPasses, GLE(GetLastError()));
            bValid = false;
        }
    }

    bool bSystem = false;

    if (m_Flags.Check(CF_SYSTEM_NORTH) || (m_Flags.Check(CF_SYSTEM_SOUTH) && (dwStages > 1)))
        bSystem = true;

    if (!bValid)
    {
        if (bSystem && m_Flags.Check(CF_SYSTEM_ALLOWED))
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
    else
    {
        if (bSystem && !m_Flags.Check(CF_SYSTEM_ALLOWED))
        {
            WriteToLog("System memory texture used, D3DDEVCAPS_TEXTURESYSTEMMEMORY not set.\n");
            WriteToLog("Validation must fail or D3DDEVCAPS_TEXTURESYSTEMMEMORY must be set.\n");
            Fail();
        }
    }

    if (BeginScene())
    {
        D3DCOLOR dcDiffuse = m_Flags.Check(CF_FACTOR) ? RGB_ZEROS : RGB_POSITIVEZ;
        FLEXVERT pVertices[4];

        pVertices[0].fPx = 0.0f;
        pVertices[0].fPy = (D3DVALUE) m_pCurrentMode->nBackBufferHeight;
        pVertices[0].fPz = 0.5f;
        pVertices[0].fRHW = 2.0f;
        pVertices[0].dcDiffuse = dcDiffuse;
        pVertices[0].fSu = (D3DVALUE) (0.0 + pfOffset[0]);
        pVertices[0].fSv = (D3DVALUE) (0.0 + pfOffset[1]);
        pVertices[0].fTu = 0.0f;
        pVertices[0].fTv = 0.0f;

        pVertices[1].fPx = 0.0f;
        pVertices[1].fPy = 0.0f;
        pVertices[1].fPz = 0.5f;
        pVertices[1].fRHW = 2.0f;
        pVertices[1].dcDiffuse = dcDiffuse;
        pVertices[1].fSu = (D3DVALUE) (0.0 + pfOffset[0]);
        pVertices[1].fSv = (D3DVALUE) (1.0 + pfOffset[1]);
        pVertices[1].fTu = 0.0f;
        pVertices[1].fTv = 1.0f;

        pVertices[2].fPx = (D3DVALUE) m_pCurrentMode->nBackBufferWidth;
        pVertices[2].fPy = 0.0f;
        pVertices[2].fPz = 0.5f;
        pVertices[2].fRHW = 2.0f;
        pVertices[2].dcDiffuse = dcDiffuse;
        pVertices[2].fSu = (D3DVALUE) (1.0 + pfOffset[0]);
        pVertices[2].fSv = (D3DVALUE) (1.0 + pfOffset[1]);
        pVertices[2].fTu = 1.0f;
        pVertices[2].fTv = 1.0f;

        pVertices[3].fPx = (D3DVALUE) m_pCurrentMode->nBackBufferWidth;
        pVertices[3].fPy = (D3DVALUE) m_pCurrentMode->nBackBufferHeight;
        pVertices[3].fPz = 0.5f;
        pVertices[3].fRHW = 2.0f;
        pVertices[3].dcDiffuse = dcDiffuse;
        pVertices[3].fSu = (D3DVALUE) (1.0f + pfOffset[0]);
        pVertices[3].fSv = (D3DVALUE) (0.0f + pfOffset[1]);
        pVertices[3].fTu = 1.0f;
        pVertices[3].fTv = 0.0f;

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

    sprintf(m_szStatus[0][0], "$gFormat\t");
    sprintf(m_szStatus[0][1], "$y%s", m_szFormat);
    sprintf(m_szStatus[1][0], "$gMode\t");
    sprintf(m_szStatus[1][1], "$y%s", szStages[dwStages - 1]);
    return bResult;
}

// NAME:        Cleanup()
// DESC:        cleanup
// INPUT:       none
// OUTPUT:      none

VOID CLightmap::Cleanup(VOID)
{
    RELEASE(m_pNorthernTexture);
    RELEASE(m_pSouthernTexture);
    m_dwFormat = NOFORMAT;
}

// NAME:        UpdateStatus()
// DESC:        update status
// INPUT:       none
// OUTPUT:      none

VOID CLightmap::UpdateStatus(VOID)
{
    CD3DTest::UpdateStatus();

    for (UINT i = 0; i < NSTATUS; i++)
        WriteStatus(m_szStatus[i][0], m_szStatus[i][1]);
}

// NAME:        ClearStatus()
// DESC:        clear status
// INPUT:       none
// OUTPUT:      none

VOID CLightmap::ClearStatus(VOID)
{
    for (UINT i = 0; i < NSTATUS; i++)
    {
        sprintf(m_szStatus[i][0], "$gNA\t");
        sprintf(m_szStatus[i][1], "$y-");
    }
}

// NAME:        ProcessArgs()
// DESC:        process args
// INPUT:       none
// OUTPUT:      none

VOID CLightmap::ProcessArgs(VOID)
{
    int iDefault, iResult;

    // alpha
    // default:  off

    if (KeySet("alpha"))
        m_Flags.Set(CF_ALPHA);

    // alphars
    // default:  off

    if (KeySet("alphars"))
        m_Flags.Set(CF_ALPHARS);

    // factor
    // default:  off

    if (KeySet("factor"))
        m_Flags.Set(CF_FACTOR);

    // mix
    // default:  off

    if (KeySet("mix"))
        m_Flags.Set(CF_MIX);

    // modulate
    // default:  off

    if (KeySet("modulate"))
        m_Flags.Set(CF_MODULATE);

    // magnitude
    // default:  DEFAULT_MAGNITUDE

    iDefault = (int) DEFAULT_MAGNITUDE;
    ReadInteger("magnitude", iDefault, &iResult);
    m_fFactor = (DOUBLE) ((iResult < MIN_MAGNITUDE) ? MIN_MAGNITUDE: ((iResult >= MAX_MAGNITUDE) ? MAX_MAGNITUDE: iResult));

    // intervals
    // default:  DEFAULT_INTERVALS

    iDefault = (int) DEFAULT_INTERVALS;
    ReadInteger("intervals", iDefault, &iResult);
    m_dwIntervals = (DWORD) ((iResult < MIN_INTERVALS) ? MIN_INTERVALS : ((iResult >= MAX_INTERVALS) ? MAX_INTERVALS: iResult));

    // validate
    // default:  true

    if (!KeySet("!validate"))
        m_Flags.Set(CF_VALIDATE);
}
