// FILE:        sprite.cpp
// DESC:        point sprite class methods for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:    for issues regarding point sprites, see .\sprite.doc

#include <math.h>
#include "sprite.h"

#ifndef RGB_MAKE
#define RGB_MAKE(r, g, b) ((D3DCOLOR) (((r) << 16) | ((g) << 8) | (b)))
#endif

CD3DWindowFramework   App;
CSpriteAttenuateFVFL  SpriteAttenuateFVFL;
CSpriteAttenuateFVFLS SpriteAttenuateFVFLS;
CSpriteScaleFVFL      SpriteScaleFVFL;
CSpriteScaleFVFLS     SpriteScaleFVFLS;
CSpriteScaleFVFTL     SpriteScaleFVFTL;
CSpriteScaleFVFTLS    SpriteScaleFVFTLS;
CSpriteWrapTrans      WrapTrans;
CSpriteUClip          SpriteUClip;
CSpriteVClip          SpriteVClip;
CSpriteCull           SpriteCull;
CSpriteFill           SpriteFill;
CSpriteBatch          SpriteBatch;
CSpriteCap            SpriteCaps;
CSpriteMipMap         SpriteMipMap;
CSpriteMipMapMulti    SpriteMipMapMulti;

// NAME:        CSprite()
// DESC:        constructor for sprite class
// INPUT:       none
// OUTPUT:      none

CSprite::CSprite(VOID)
{   
    SetTestType(TESTTYPE_CONF);

    sprintf(m_szFormat, "NA");
    ClearStatus();

    m_fFixed = 0.0f;

    m_dvAngle = 55.0f*pi/180.0f;
    m_dvFar = 90.0f;
    m_dvNear = 1.0f;

    m_At.x = 0.0f;
    m_At.y = 0.0f;
    m_At.z = 1.0f;

    m_From.x = 0.0f;
    m_From.y = 0.0f;
    m_From.z = -1.0f;

    m_Up.x = 0.0f;
    m_Up.y = 1.0f;
    m_Up.z = 0.0f;

    m_dwMinLevelStd = (DWORD) 0;
    m_dwMinLevelYUV = (DWORD) 0;
    m_dwMinLevel = (DWORD) 0;
    m_dwMaxLevel = (DWORD) 0;

    m_iNumStagesSet = 1;

	// Initialize image compare settings required for point sprite test
    m_pFramework->GetImageCmp()->SetMinPixels(0);
	m_pFramework->GetImageCmp()->SetImgDeltaWght(0.92f);

    m_Options.ModeOptions.CubeFmt = FMT_ANY;
    m_Options.ModeOptions.VolumeFmt = FMT_ANY;
    m_Options.ModeOptions.ZBufferFmt = FMT_ANY;
    m_Options.ModeOptions.dwTexInclude = (PF_RGB | PF_DXTN | PF_YUV | PF_LUMINANCE | PF_ALPHA);
    m_Options.ModeOptions.dwTexExclude = (DDPF_PALETTEINDEXED1 |
                                          DDPF_PALETTEINDEXED2 |
                                          DDPF_PALETTEINDEXED4 |
                                          DDPF_PALETTEINDEXED8 |
                                          DDPF_PALETTEINDEXEDTO8);

    m_Options.D3DOptions.bZBuffer = true;
    m_Options.D3DOptions.fMinDXVersion = 8.0f;
}

// NAME:        ~CSprite()
// DESC:        destructor for sprite class
// INPUT:       none
// OUTPUT:      none

CSprite::~CSprite(VOID)
{
}


// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CSprite::CommandLineHelp(void)
{
    WriteCommandLineHelp("$yextents: $wdraw viewport extents $c($wOn$c/Off)");
    WriteCommandLineHelp("$yflat: $wflat shade $c($wOn$c/Off)");
    WriteCommandLineHelp("$ymipmap: $wmipmap $c($wOn$c/Off)");
    WriteCommandLineHelp("$ynotex: $wno textures $c($wYes$c/No)");
    WriteCommandLineHelp("$yfixed: $wfixed max point size $c(default=0)");
    WriteCommandLineHelp("$ystages: $wmax simultaneous textures $c(default=caps)");
    WriteCommandLineHelp("$yvariations: $wvariations per texture $c(default=%d)", NVDEF);
}


// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSprite::TestInitialize(VOID)
{
    DWORD dwWidth = (DWORD) MAXSIZE, dwHeight = (DWORD) MAXSIZE;
    UINT i, j;

    SetTestRange((UINT) 1, (UINT) 1);    

    m_Rect.top = 0;
    m_Rect.left = 0;
    m_Rect.bottom = m_pCurrentMode->nBackBufferHeight;
    m_Rect.right = m_pCurrentMode->nBackBufferWidth;

    m_dcBackground = RGB_MAKE(255, 255, 255);
    
    m_iFormat = (INT) NOFORMAT;    

    m_nVariationsPerFormat = (UINT) 0;    

    for (i = 0; i < D3DDP_MAXTEXCOORD; i++)
    {
        m_rgpTexture[i] = NULL;

        for (j = 0; j < MIPLEVELS; j++)
            m_rgpImage[i][j] = NULL;
    }

    m_Sprite.Ka.f = 1.0f;
    m_Sprite.Kb.f = 0.0f;
    m_Sprite.Kc.f = 0.0f;
    m_Sprite.PointSize.f = 1.0f;
    m_Sprite.PointSizeMax.f = 1.0f;
    m_Sprite.PointSizeMin.f = 1.0f;

    ProcessArgs();

    CAPS RefCaps, SrcCaps; // raw device caps
    m_pD3D->GetDeviceCaps(m_pRefDevice->GetAdapterID(), m_pRefDevice->GetDevType(), &RefCaps);
    m_pD3D->GetDeviceCaps(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), &SrcCaps);

    DEVICEDESC *pRefDesc, *pSrcDesc; // device description after vertex processing behavior applied
    pRefDesc = m_pRefDevice->GetCaps();
    pSrcDesc = m_pSrcDevice->GetCaps();

    if ((m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_SCISSORTEST) &&
        (m_pRefDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_SCISSORTEST))
        m_Flags.Set(CF_SCISSOR);

    if (SrcCaps.MaxPointSize <= 1.0f)
    {
        WriteToLog("Device does NOT support point size (testing emulation).\n");
        m_Flags.Set(CF_EMULATING);

        // point sprites are not required to be supported but cap must be set to 1.0f

        if (SrcCaps.MaxPointSize < 1.0f)
        {
            if (m_pSrcDevice->GetDDI() <= 0x007)
            {
                WriteToLog("Runtime failure: MaxPointSize should = 1.0f for this hardware.\n");
                return D3DTESTINIT_ABORT;
            }
            else 
            {
                WriteToLog("Drivers must set MaxPointSize = 1.0f to indicate no support.\n");
                return D3DTESTINIT_ABORT;
            }
        }

        SrcCaps.MaxPointSize = pSrcDesc->dvMaxPointSize;
    }

    if (SrcCaps.MaxPointSize < RefCaps.MaxPointSize)
        m_Sprite.PointSizeMax.f = SrcCaps.MaxPointSize;
    else
        m_Sprite.PointSizeMax.f = RefCaps.MaxPointSize;    

    // allow fixed max point size if specified in (1,max]

    if (m_fFixed > 1.0f)
        m_Sprite.PointSizeMax.f = (m_Sprite.PointSizeMax.f > m_fFixed) ? m_fFixed : m_Sprite.PointSizeMax.f;

    if (m_Flags.Check(CF_VERTEXPOINTSIZE))
    {
        if (!(m_pSrcDevice->GetCaps()->dwFVFCaps & D3DFVFCAPS_PSIZE))
        {
            WriteToLog("Src device does not support point size in vertex data.\n");
            m_Flags.Clear(CF_VERTEXPOINTSIZE);
        }

        if (!(m_pRefDevice->GetCaps()->dwFVFCaps & D3DFVFCAPS_PSIZE))
        {
            WriteToLog("Ref device does not support point size in vertex data.\n");
            m_Flags.Clear(CF_VERTEXPOINTSIZE);
        }
    }
    
    // caps check tests that this value is actually correct ie > MIN_MAXPOINTSIZE

    if (m_Sprite.PointSizeMax.f <= 1.0f)
    {
        // if we have no point size then abort the test

        WriteToLog("Device does NOT support point sprites.\n");

        // sprites are not required to be supported but the cap must be set to 1.0f

        if (!(m_pSrcDevice->GetBehaviors() & D3DCREATE_SOFTWARE_VERTEXPROCESSING))
        {
            if (m_Sprite.PointSizeMax.f < 1.0f)
            {
                if (m_pSrcDevice->GetDDI() <= 0x007)
                {
                    WriteToLog("Runtime failure: MaxPointSize should = 1.0f for this hardware.\n");
                    return D3DTESTINIT_ABORT;
                }
                else
                {
                    WriteToLog("Drivers must set MaxPointSize = 1.0f to indicate no support.\n");
                    return D3DTESTINIT_ABORT;
                }
            }
            else
            {
                WriteToLog("Drivers for TNL hw are allowed to set MaxPointSize = 1.0f.\n");
                return D3DTESTINIT_SKIPALL;
            }
        }
        else
        {
            if (m_Sprite.PointSizeMax.f <= 1.0f)
            {
                WriteToLog("Runtime failure: Point sprites should be emulated for this hardware.\n");
                return D3DTESTINIT_ABORT;
            }
        }
    }

    if (!(m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT) && m_Flags.Check(CF_MIPMAP))
    {
        WriteToLog("Src device does not support mip mapping.\n");
        return D3DTESTINIT_SKIPALL;
    }

    // find max point that can be contained within the port

    m_dvPointSizeMax = (D3DVALUE) ((m_pCurrentMode->nBackBufferWidth > m_pCurrentMode->nBackBufferHeight) ? m_pCurrentMode->nBackBufferHeight : m_pCurrentMode->nBackBufferWidth);
    m_dvPointSizeMax = (m_Sprite.PointSizeMax.f < m_dvPointSizeMax) ? m_Sprite.PointSizeMax.f : m_dvPointSizeMax;

    DWORD pdwColors[4] =
    {
        0xFFFF0000,
        0xFF00FF00,
        0xFF0000FF,
        0xFFFFFFFF
    };    

    m_rgpImage[0][0] = new CImage;

    if (!m_rgpImage[0][0])
    {
        WriteToLog("Unable to create image 0.\n");
        return D3DTESTINIT_ABORT;
    }

    // load gradient color for top mip level (stage 0)

    if (!m_rgpImage[0][0]->LoadGradient(dwWidth, dwHeight, pdwColors))
    {
        WriteToLog("Unable to load image.\n");
        return D3DTESTINIT_ABORT;
    }
    
    for (i = 1; i < m_iNumStagesSet; i++)
    {
        DWORD pdwStripes[D3DDP_MAXTEXCOORD];
        
        m_rgpImage[i][0] = new CImage;

        if (!m_rgpImage[i][0])
        {
            WriteToLog("Unable to create image 0.\n");
            return D3DTESTINIT_ABORT;
        }
    
        // load vertical stripe for top mip level (stage > 0)

        memset(pdwStripes, 0xffffffff, sizeof(pdwStripes));
        pdwStripes[i] = pdwColors[i%4];
        
        if (!m_rgpImage[i][0]->LoadStripes(dwWidth, dwHeight, m_iNumStagesSet, pdwStripes, true ) )
        {
            WriteToLog("Unable to load image %d.\n", i);
            return D3DTESTINIT_ABORT;
        }
    }

    if (m_Flags.Check(CF_MIPMAP))
    {
        DWORD dwMipWidth, dwMipHeight;
        DWORD pdwStageStripes[D3DDP_MAXTEXCOORD];
        DWORD pdwStripes[MIPLEVELS] =
        {
            { 0xFF000000 },
            { 0xFFFFFF00 },
            { 0xFFFF00FF },
            { 0xFFFF0000 },
            { 0xFF00FF00 },
            { 0xFF0000FF },
            { 0xFF00FFFF },
            { 0xFFF0000F },
            { 0xFF0F00F0 }
        };

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

        for (m_dwMaxLevel =         0; (MAXSIZE >> m_dwMaxLevel) > dwMaxTextureWidth;  m_dwMaxLevel++);
        for (m_dwMinLevel = MIPLEVELS; (MAXSIZE >> m_dwMinLevel) < dwMinTextureHeight; m_dwMinLevel--);

        m_dwMinLevelStd = m_dwMinLevel;
        m_dwMinLevelYUV = m_dwMinLevel;

        // Matrox drivers fail YUV mipmap texture creation when width is less than 8 texels

        while ((MAXSIZE >> m_dwMinLevelYUV) < 8)
            m_dwMinLevelYUV--;

        WriteToLog("(INFO) Max texture size supported (up to %dx%d): %dx%d.\n",
                   MAXSIZE, MAXSIZE, MAXSIZE >> m_dwMaxLevel, MAXSIZE >> m_dwMaxLevel);

        WriteToLog("(INFO) Min texture size supported (down to 1x1): %dx%d.\n",
                   MAXSIZE >> m_dwMinLevel, MAXSIZE >> m_dwMinLevel);

        for (i = 0; i < m_iNumStagesSet; i++)
        {
            dwMipWidth = MAXSIZE >> m_dwMaxLevel;
            dwMipHeight = MAXSIZE >> m_dwMaxLevel;

            for (j = 1; j <= m_dwMinLevel; j++)
            {
                m_rgpImage[i][j] = new CImage;

                if (!m_rgpImage[i][j])
                {
                    WriteToLog("Unable to create image %d.\n", i);
                    return D3DTESTINIT_ABORT;
                }

                dwMipWidth = (1 > (dwMipWidth >> 1)) ? 1 : dwMipWidth >> 1;
                dwMipHeight = (1 > (dwMipHeight >> 1)) ? 1 : dwMipHeight >> 1;

                if (0 == i) // first stage
                {
                    // load solid color in each mip level (stage 0)

                    if (!m_rgpImage[i][j]->LoadStripes(dwMipWidth, dwMipHeight, 1, &pdwStripes[j], false))
                    {
                        WriteToLog("Unable to load image %d.\n", j);
                        return D3DTESTINIT_ABORT;
                    }
                }
                else
                {
                    memset(pdwStageStripes, 0xffffffff, sizeof(pdwStageStripes));
                    pdwStageStripes[i] = pdwStripes[(D3DDP_MAXTEXCOORD - i) % MIPLEVELS];                    

                    // load horizontal stripe in each mip level (stage > 0)

                    if (!m_rgpImage[i][j]->LoadStripes(dwMipWidth, dwMipHeight, m_iNumStagesSet, pdwStageStripes, false))
                    {
                        WriteToLog("Unable to load image %d.\n", i);
                        return D3DTESTINIT_ABORT;
                    }
                }
            }
        }
    }

#ifdef FAST_TEST
    m_uCommonTextureFormats = 1;
#endif

    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSprite::ExecuteTest(UINT uTest)
{
    bool bFound = false, bValid = true;
    int iFormat, iStage;

    iFormat = (uTest - 1) / m_nVariationsPerFormat;
    iFormat = iFormat % (m_uCommonTextureFormats + 1);

    if (iFormat < m_uCommonTextureFormats)
    {
        if (!m_Flags.Check(CF_TEXTURE) && (m_Flags.Saved() & CF_TEXTURE))
            m_Flags.Set(CF_TEXTURE); // restore if necessary

        bFound = true;
    }
    else if (iFormat == m_uCommonTextureFormats)
    {
        m_Flags.Clear(CF_TEXTURE);
        bFound = true;
    } // allows texturing to be turned off if test range is m_nVariationsPerFormat*(m_uCommonTextureFormats + 1)

    if (!bFound)
        return false;

    for (iStage = 0; iStage < m_iNumStagesSet; iStage++)
    {
        if (m_Flags.Check(CF_TEXTURE))
        {
            if ((iFormat != m_iFormat) || (NULL == m_rgpTexture[iStage]))
            {
                DWORD dwLevels = (DWORD) 1;
                DWORD dwPool = FMWK_DEFAULT;
                DWORD dwUsage = (DWORD) 0;
                DWORD dwWidth = (DWORD) MAXSIZE;
                DWORD dwHeight = (DWORD) MAXSIZE;

                m_pCurrentMode->nTextureFormat = iFormat;
                FORMAT *pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
                FmtToString(FORMAT_TEXTURE, pFormat, m_szFormat);
                RELEASE(m_rgpTexture[iStage]);

                // precision issues with 332 formats when compared to reference
                // (reduce required conformance percentage)

                if ((FMT_R3G3B2 == pFormat->d3dfFormat) || (FMT_A8R3G3B2 == pFormat->d3dfFormat))
                    m_Flags.Set(CF_PRECISION);
                else
                    m_Flags.Clear(CF_PRECISION);

                if (m_Flags.Check(CF_MIPMAP))
                {
                    if ((D3DFMT_UYVY == pFormat->d3dfFormat) || (D3DFMT_YUY2 == pFormat->d3dfFormat))
                        m_dwMinLevel = m_dwMinLevelYUV;
                    else
                        m_dwMinLevel = m_dwMinLevelStd;

                    dwLevels = m_dwMinLevel - m_dwMaxLevel + 1;

					// On Dx9.L, need to check for the special A1 format
					if ((m_pD3D->GetVersion() >= 9.1f) && (pFormat->d3dfFormat == FMT_A1))
						dwLevels = 1;

                    // lightweight mipmaps need to be complete
                    // appropriate lightweight/heavyweight selected if default usage is specified

                    if (dwLevels < MIPLEVELS)
                        dwUsage = FMWK_DEFAULT;
                }

                // ASSUMPTION: sprite max is not much greater than 128 (currently 64)
                //
                // Image array:
                //   standard texture at level 0 (256*256)
                //   striped mip map textures in level [1,dwLevels - 1]
                 
                if (!CreateTexture(dwWidth, dwHeight, dwLevels, dwUsage, *pFormat, dwPool, &(m_rgpTexture[iStage])))
                {
					sprintf(m_szBuffer, "(Ka,Kb,Kc)=(%f,%f,%f), Format %s", m_Sprite.Ka.f, m_Sprite.Kb.f, m_Sprite.Kc.f, m_szFormat);
					BeginTestCase(m_szBuffer, uTest);
					WriteToLog("Unable to create base texture %s, hr = %s. \n", m_szFormat, GLE(GetLastError()));
                    Fail();
                    return false;
                }

                if (!m_rgpTexture[iStage]->LoadImage((m_Flags.Check(CF_MIPMAP) ? 0 : 1), &(m_rgpImage[iStage][0])))
                {
					sprintf(m_szBuffer, "(Ka,Kb,Kc)=(%f,%f,%f), Format %s", m_Sprite.Ka.f, m_Sprite.Kb.f, m_Sprite.Kc.f, m_szFormat);
					BeginTestCase(m_szBuffer, uTest);
					WriteToLog("Unable to load base texture %s, hr = %s. \n", m_szFormat, GLE(GetLastError()));
                    Fail();
                    return false;
                }
            }

            if (m_Flags.Check(CF_PRECISION))
                m_pFramework->GetImageCmp()->SetPassRequired(0.80f);
            else
                m_pFramework->GetImageCmp()->SetPassRequired(0.85f);

            DWORD dwMode = (DWORD) D3DTADDRESS_WRAP;
            
            if ((m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureAddressCaps & D3DPTADDRESSCAPS_CLAMP) &&
                (m_pRefDevice->GetCaps()->dpcTriCaps.dwTextureAddressCaps & D3DPTADDRESSCAPS_CLAMP))
                dwMode = (DWORD) D3DTADDRESS_CLAMP;

            SetSamplerState(iStage, SAMP_ADDRESSU, dwMode);
            SetSamplerState(iStage, SAMP_ADDRESSV, dwMode);

            DWORD dwFilter = (DWORD) TEXF_POINT;

            if ((m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR) &&
                (m_pRefDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))
			{
                dwFilter = (DWORD) TEXF_LINEAR;

				// On Dx9, need to check for UsageQueryFilter
				if (m_dwVersion >= 0x0900)
				{
					if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
														 USAGE_QUERY_FILTER, RTYPE_TEXTURE, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat])))
			            dwFilter = (DWORD) TEXF_POINT;
				}
			}

            SetSamplerState(iStage, SAMP_MAGFILTER, (DWORD) dwFilter);

            dwFilter = (DWORD) TEXF_POINT;

            if ((m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) &&
                (m_pRefDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))
			{
                dwFilter = (DWORD) TEXF_LINEAR;

				// On Dx9, need to check for UsageQueryFilter
				if (m_dwVersion >= 0x0900)
				{
					if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
														 USAGE_QUERY_FILTER, RTYPE_TEXTURE, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat])))
			            dwFilter = (DWORD) TEXF_POINT;
				}
			}

            SetSamplerState(iStage, SAMP_MINFILTER, (DWORD) dwFilter);

			// On Dx9c, need to check for UsageQueryWrapAndMip
			if ((m_pD3D->GetRuntimeVersion() >= 9.029f) && (m_Flags.Check(CF_MIPMAP)))
			{
				HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
													USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat]);
				if (FAILED(Result))
				{
					sprintf(m_szBuffer, "(Ka,Kb,Kc)=(%f,%f,%f), Format %s", m_Sprite.Ka.f, m_Sprite.Kb.f, m_Sprite.Kc.f, m_szFormat);
					BeginTestCase(m_szBuffer, uTest);
					WriteToLog("Unable to mipmap this format. CheckDeviceFormat(WrapAndMip) failed: %s.\n",GLE(Result));
                    Skip();
                    return false;
				}
			}

			dwFilter = (DWORD) TEXF_POINT;
/*
            // linear blend between mip levels exposes hw limitations beyond point sprite test scope
            // moved back to point filtering (i.e. one level or another, not a blend betwen two)

            if ((m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR) &&
                (m_pRefDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
			{
                dwFilter = (DWORD) TEXF_LINEAR;

				// On Dx9, need to check for UsageQueryFilter
				if (m_dwVersion >= 0x0900)
				{
					if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
														 USAGE_QUERY_FILTER, RTYPE_TEXTURE, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat])))
			            dwFilter = (DWORD) TEXF_POINT;
				}
			}
*/
            SetSamplerState(iStage, SAMP_MIPFILTER, (DWORD) m_Flags.Check(CF_MIPMAP) ? dwFilter : TEXF_NONE);

            SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_MODULATE);
            SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
            SetTextureStageState(iStage, D3DTSS_COLORARG2, (DWORD) D3DTA_CURRENT);

            SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
            SetTextureStageState(iStage, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
            SetTextureStageState(iStage, D3DTSS_ALPHAARG2, (DWORD) D3DTA_CURRENT);

            SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 0);
            SetTexture(iStage, m_rgpTexture[iStage]);            
        }
        else
        {
            SetTexture(iStage, NULL);
            sprintf(m_szFormat, "NA");
            RELEASE(m_rgpTexture[iStage]);

            m_pFramework->GetImageCmp()->SetPassRequired(0.85f);
        }
    }

    m_iFormat = iFormat;

    if (iStage < D3DDP_MAXTEXCOORD)
    {
        SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
        SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);
    }

    sprintf(m_szStatus[0][0], "$gFormat\t");
    sprintf(m_szStatus[0][1], "$y%s", m_szFormat);
    sprintf(m_szStatus[1][0], "$gSize\t");
    sprintf(m_szStatus[1][1], "$y%f", m_Sprite.PointSize.f);
    sprintf(m_szStatus[2][0], "$gMin\t");
    sprintf(m_szStatus[2][1], "$y%f", m_Sprite.PointSizeMin.f);
    sprintf(m_szStatus[3][0], "$gMax\t");
    sprintf(m_szStatus[3][1], "$y%f", m_Sprite.PointSizeMax.f);
    sprintf(m_szStatus[4][0], "$gKa\t");
    sprintf(m_szStatus[4][1], "$y%f", m_Sprite.Ka.f);
    sprintf(m_szStatus[5][0], "$gKb\t");
    sprintf(m_szStatus[5][1], "$y%f", m_Sprite.Kb.f);
    sprintf(m_szStatus[6][0], "$gKc\t");
    sprintf(m_szStatus[6][1], "$y%f", m_Sprite.Kc.f);

    if (m_Flags.Check(CF_SCISSOR))
    {
        sprintf(m_szStatus[7][0], "$gScissor\t");
        sprintf(m_szStatus[7][1], "$y(%d,%d,%d,%d)", m_Rect.left, m_Rect.top, m_Rect.right, m_Rect.bottom);
    }
    else
    {
        sprintf(m_szStatus[7][0], "$gScissor\t");
        sprintf(m_szStatus[7][1], "$yNA");
    }

    sprintf(m_szStatus[8][0], "$gTarget\t");
    sprintf(m_szStatus[8][1], "$y(0,0,%d,%d)", m_pCurrentMode->nBackBufferWidth, m_pCurrentMode->nBackBufferHeight);

    SetRenderState(D3DRS_POINTSIZE_MAX, m_Sprite.PointSizeMax.dw);
    SetRenderState(D3DRS_POINTSIZE, m_Sprite.PointSize.dw);
    SetRenderState(D3DRS_POINTSIZE_MIN, m_Sprite.PointSizeMin.dw);
    SetRenderState(D3DRS_POINTSCALE_A, m_Sprite.Ka.dw);
    SetRenderState(D3DRS_POINTSCALE_B, m_Sprite.Kb.dw);
    SetRenderState(D3DRS_POINTSCALE_C, m_Sprite.Kc.dw);
    SetRenderState(D3DRS_SHADEMODE, (DWORD) (m_Flags.Check(CF_FLAT) ? D3DSHADE_FLAT : D3DSHADE_GOURAUD));
    // SetRenderState(D3DRS_CLIPPING, (DWORD) FALSE);

	if (m_Flags.Check(CF_INVALID))
    {
        sprintf(m_szStatus[9][0], "$gINVALID");
        sprintf(m_szStatus[9][1], "$yComparison disabled");
        SetCompareSrcRef(false);
    }
    else
    {
        sprintf(m_szStatus[9][0], "$gVALID\t");
        sprintf(m_szStatus[9][1], "$yComparison enabled");
        SetCompareSrcRef(true);
    }

    return true;
}

// NAME:        TestTerminate()
// DESC:        terminate test
// INPUT:       none
// OUTPUT:      true

bool CSprite::TestTerminate(VOID)
{
    int i, j;
    
    for (i = 0; i < D3DDP_MAXTEXCOORD; i++) 
    {
        RELEASE(m_rgpTexture[i]);

        for (j = 0; j < MIPLEVELS; j++)
            SAFEDELETE(m_rgpImage[i][j]);
    }

    return true;
}

// NAME:        ClearFrame()
// DESC:        clear frame
// INPUT:       none
// OUTPUT:      Clear()

bool CSprite::ClearFrame(VOID)
{
    bool bResult = Clear(m_dcBackground);
    return bResult;
}

// NAME:        BeginScene()
// DESC:        begin scene
// INPUT:       none
// OUTPUT:      CD3DTest::BeginScene()
// COMMENTS:    overload CD3DTest::BeginScene() to draw viewport if requested

bool CSprite::BeginScene(VOID)
{
    bool bResult = CD3DTest::BeginScene();

    if (m_Flags.Check(CF_DRAWPORT))
    {
        DrawViewport();

        if (m_Flags.Check(CF_SCISSOR))
            DrawScissor();
    }

    return bResult;
}

// NAME:        UpdateStatus()
// DESC:        update status
// INPUT:       none
// OUTPUT:      none

VOID CSprite::UpdateStatus(VOID)
{
    CD3DTest::UpdateStatus();

    for (UINT i = 0; i < NSTATUS; i++)
        WriteStatus(m_szStatus[i][0], m_szStatus[i][1]);
}

// NAME:        ClearStatus()
// DESC:        clear status
// INPUT:       none
// OUTPUT:      none

VOID CSprite::ClearStatus(VOID)
{
    for (UINT i = 0; i < NSTATUS; i++)
    {
        sprintf(m_szStatus[i][0], "$gNA\t");
        sprintf(m_szStatus[i][1], "$y-");
    }
}

// NAME:        Clear()
// DESC:        clear render target and depth buffer
// INPUT:       dwColor..clear color
// OUTPUT:      Clear()

bool CSprite::Clear(DWORD dwColor)
{
    D3DVALUE dvZ = 1.0f;
    DWORD dwFlags = (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER);
    DWORD dwStencil = (DWORD) 0;

    return CD3DTest::Clear(0, NULL, dwFlags, dwColor, dvZ, dwStencil);
}

// NAME:        SetDefaultMatrices()
// DESC:        setup default matrices
// INPUT:       none
// OUTPUT:      none

bool CSprite::SetDefaultMatrices(VOID)
{
    D3DVALUE dvAspect;
    D3DXMATRIX Matrix;

    dvAspect = ((float) m_pCurrentMode->nBackBufferWidth) /
               ((float) m_pCurrentMode->nBackBufferHeight);
    D3DXMatrixIdentity(&Matrix);

    if (!SetTransform(D3DTS_WORLD, (D3DMATRIX *) &Matrix))
    {
        WriteToLog("SetTransform(WORLD) failed: %s.\n", GLE(GetLastError()));
        return false;
    }

    D3DXMatrixPerspectiveFovLH(&m_Projection, m_dvAngle, dvAspect, m_dvNear, m_dvFar);

    float fTemp = (float) 1.0/m_Projection.m[2][3];

    for (UINT i = 0; i < 4; i++)
        for (UINT j = 0; j < 4; j++)
            m_Projection.m[i][j] *= fTemp;

    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &m_Projection))
    {
        WriteToLog("SetTransform(PROJECTION) failed: %s.\n", GLE(GetLastError()));
        return false;
    }

    D3DXMatrixLookAtLH(&m_View, &m_From, &m_At, &m_Up);

    if (!SetTransform(D3DTS_VIEW, (D3DMATRIX *) &m_View))
    {
        WriteToLog("SetTransform(VIEW) failed: %s.\n", GLE(GetLastError()));
        return false;
    }

    return SetViewport(0, 0, m_pCurrentMode->nBackBufferWidth, m_pCurrentMode->nBackBufferHeight);
}

// NAME:        SetDefaultRenderStates()
// DESC:        set default render states
// INPUT:       none
// OUTPUT:      true.....if SetRenderState() succeeds
//              false....otherwise

bool CSprite::SetDefaultRenderStates(VOID)
{
    SetRenderState(D3DRS_POINTSCALEENABLE, (DWORD) FALSE);
    SetRenderState(D3DRS_LIGHTING, (DWORD) FALSE);
    SetRenderState(D3DRS_POINTSPRITEENABLE, (DWORD) TRUE);
    SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_CCW);
    return true;
}

// NAME:        ProcessArgs()
// DESC:        process args
// INPUT:       none
// OUTPUT:      none

VOID CSprite::ProcessArgs(VOID)
{
    int iDefault, iResult;
 
    m_Flags.Replace(m_Group.Get());
    m_Flags.Save();

    if (KeySet("FLAT"))
        m_Flags.Set(CF_FLAT);

    if (KeySet("INVALID"))
        m_Flags.Set(CF_INVALID);

    if (!KeySet("NOTEX"))
        m_Flags.Set(CF_TEXTURE);

    if (KeySet("WHQL"))
    {
        m_nVariationsPerFormat = (UINT) NVDEF;
        m_Flags.Save();
        return;
    }

    // fixed
    // default:  0

    iDefault = (int) 0;
    ReadInteger("fixed", iDefault, &iResult);
    m_fFixed = (D3DVALUE) iResult;

    // stages
    // default:  m_iNumStagesSet

    iDefault = m_iNumStagesSet;
    ReadInteger("stages", iDefault, &iResult);
    m_iNumStagesSet = ((iResult < 1) ? 1 : ((iResult > m_iNumStagesSet) ? m_iNumStagesSet: iResult));

    // variations
    // default:  NVDEF 

    iDefault = (int) NVDEF;
    ReadInteger("variations", iDefault, &iResult);
    m_nVariationsPerFormat = ((iResult < NVMIN) ? NVMIN : ((iResult >= NVMAX) ? NVMAX: iResult));

    // extents
    // default:  false

    if (KeySet("extents"))
        m_Flags.Set(CF_DRAWPORT);

    m_Flags.Save();
}

// NAME:        SetPosition()
// DESC:        set position (x,y)
// INPUT:       uPosition....position index
//              pfX..........x coordinate
//              pfY..........y coordinate
// COMMENTS:    all values in SetPosition should be in pixels

VOID CSprite::SetPosition(UINT nPosition, float *pfX, float *pfY)
{
    int nHalf = (int) ((m_Sprite.PointSize.f + 0.5)/2.0); // half point size
    float fSize = m_Sprite.PointSize.f;
    float fPosition[NCLIPTESTS][2] =
    {
        { (float) (m_Port.Left() + nHalf) , (float) (m_Port.Top() + nHalf)    }, // inside
        { (float) (m_Port.CenterX())      , (float) (m_Port.Top() + nHalf)    },
        { (float) (m_Port.Right() - nHalf), (float) (m_Port.Top() + nHalf)    },
        { (float) (m_Port.Right() - nHalf), (float) (m_Port.CenterY())        },
        { (float) (m_Port.Right() - nHalf), (float) (m_Port.Bottom() - nHalf) },
        { (float) (m_Port.CenterX())      , (float) (m_Port.Bottom() - nHalf) },
        { (float) (m_Port.Left() + nHalf) , (float) (m_Port.Bottom() - nHalf) },
        { (float) (m_Port.Left() + nHalf) , (float) (m_Port.CenterY())        },
        { (float) (m_Port.Left() - 1)     , (float) (m_Port.Top() - 1)        }, // just outside
        { (float) (m_Port.CenterX())      , (float) (m_Port.Top() - 1)        },
        { (float) (m_Port.Right() + 1)    , (float) (m_Port.Top() - 1)        },
        { (float) (m_Port.Right() + 1)    , (float) (m_Port.CenterY())        },
        { (float) (m_Port.Right() + 1)    , (float) (m_Port.Bottom() + 1)     },
        { (float) (m_Port.CenterX())      , (float) (m_Port.Bottom() + 1)     },
        { (float) (m_Port.Left() - 1)     , (float) (m_Port.Bottom() + 1)     },
        { (float) (m_Port.Left() - 1)     , (float) (m_Port.CenterY())        },
        { (float) (m_Port.Left() - fSize) , (float) (m_Port.Top() - fSize)    }, // outside
        { (float) (m_Port.CenterX())      , (float) (m_Port.Top() - fSize)    },
        { (float) (m_Port.Right() + fSize), (float) (m_Port.Top() - fSize)    },
        { (float) (m_Port.Right() + fSize), (float) (m_Port.CenterY())        },
        { (float) (m_Port.Right() + fSize), (float) (m_Port.Bottom() + fSize) },
        { (float) (m_Port.CenterX())      , (float) (m_Port.Bottom() + fSize) },
        { (float) (m_Port.Left() - fSize) , (float) (m_Port.Bottom() + fSize) },
        { (float) (m_Port.Left() - fSize) , (float) (m_Port.CenterY())        },
    };
    UINT n = (nPosition < NCLIPTESTS) ? nPosition : 0;

    *pfX = fPosition[n][0];
    *pfY = fPosition[n][1];
}

// NAME:        SetPositionWorld()
// DESC:        Sets the world co-ordinate(x,y)
// INPUT:       nPosition....position index
//              pfX..........x coordinate
//              pfY..........y coordinate
// OUTPUT:      pfX and pfY set to requested world coords for nPosition

void CSprite::SetPositionWorld(UINT nPosition, float *pfX, float *pfY)
{
    D3DXVECTOR3 dvectPos;

    SetPosition(nPosition, pfX, pfY);
    dvectPos.x = *pfX;
    dvectPos.y = *pfY;
    dvectPos.z = 0.0f;
    AdjustVertex(&dvectPos);
    *pfX = dvectPos.x;
    *pfY = dvectPos.y;
}

// NAME:        AdjustVertex()
// DESC:        Adjust vertex (x,y) to avoid precision errors in sprite placement
// INPUT:       pVector..screen coords to be adjusted
// OUTPUT:      true.....if vertex is appropriately adjusted
//              false....otherwise
// COMMENTS:    point size must be an even number in order to avoid all possible precision issues
//              (see AdjustVertexAttenuated()

bool CSprite::AdjustVertex(D3DXVECTOR3 *pVector)
{
    bool bSuccess = true, bIncX = true, bIncY = true;
    float fEpsilon, fPosInPixel, fWorldPixelSize;
    UINT i;

    if (pVector->x < m_Port.Left())
        bIncX = false;

    if (pVector->y < m_Port.Top())
        bIncY = false;

    D3DXVec3TransformCoord(pVector, pVector, &m_ScreenToWorld);
    fWorldPixelSize = CalcWorldPixelSize();
    fEpsilon = fWorldPixelSize/10.0f;

    for (i = 0; i < MAX_ADJUSTMENTS; i++)
    {
        fPosInPixel = pVector->x/fWorldPixelSize;
        fPosInPixel = fPosInPixel - (long) fPosInPixel;
        fPosInPixel *= fWorldPixelSize;
        fPosInPixel = fabs(fPosInPixel);

        if ((fPosInPixel < fEpsilon) ||
             (fPosInPixel > (fWorldPixelSize - fEpsilon)) ||
             ((fPosInPixel > (0.5f*fWorldPixelSize - fEpsilon)) &&
              (fPosInPixel < (0.5f*fWorldPixelSize + fEpsilon))))
        {
            if (bIncX)
                pVector->x += (float) fEpsilon;
            else
                pVector->x -= (float) fEpsilon;
        }
        else
            break;
    }

    bSuccess = (MAX_ADJUSTMENTS == i) ? false : bSuccess;
    
    for (i = 0; i < MAX_ADJUSTMENTS; i++)
    {
        fPosInPixel = pVector->y/fWorldPixelSize;
        fPosInPixel = fPosInPixel - (long) fPosInPixel;
        fPosInPixel *= fWorldPixelSize;
        fPosInPixel = fabs(fPosInPixel);

        if ((fPosInPixel < fEpsilon) ||
            (fPosInPixel > (fWorldPixelSize - fEpsilon)) ||
            ((fPosInPixel > (0.5f*fWorldPixelSize - fEpsilon)) &&
             (fPosInPixel < (0.5f*fWorldPixelSize + fEpsilon))))
        {
            if (bIncY)
                pVector->y += (float) fEpsilon;
            else
                pVector->y -= (float) fEpsilon;
        }
        else
            break;
    }

    bSuccess = (MAX_ADJUSTMENTS == i) ? false : bSuccess;
    return bSuccess;
}

// NAME:        CalcWorldPixelSize()
// DESC:        calculate size of pixel in world coords
// INPUT:       none
// OUTPUT:      size of pixel in world coords

float CSprite::CalcWorldPixelSize(void)
{
    D3DXVECTOR3 dvecPixel1(0.0f, 0.0f, 0.0f), dvecPixel2(1.0f, 0.0f, 0.0f);
    
    D3DXVec3TransformCoord(&dvecPixel1, &dvecPixel1, &m_ScreenToWorld);
    D3DXVec3TransformCoord(&dvecPixel2, &dvecPixel2, &m_ScreenToWorld);
    dvecPixel1.x -= dvecPixel2.x;
    dvecPixel1.y -= dvecPixel2.y;
    dvecPixel1.z -= dvecPixel2.z;
    return sqrt((dvecPixel1.x*dvecPixel1.x) + (dvecPixel1.y*dvecPixel1.y) + (dvecPixel1.z*dvecPixel1.z));     
}

// NAME:        GetWorldPosition()
// DESC:        get world position from screen position and screen to world xform
// INPUT:       pScreen..address of D3DXVECTOR3 screen point
// OUTPUT:      none

void CSprite::GetWorldPosition(D3DXVECTOR3 *pScreen)
{
    D3DXVec3TransformCoord(pScreen, pScreen, &m_ScreenToWorld);
}

// NAME:        UpdateScreenToWorldMatrix()
// DESC:        update screen to world matrix
// INPUT:       none
// OUTPUT:      none
// COMMENTS:    s = w*(M*P*V*S*T)
//              w = s*(M*P*V*S*T)inverse
//
//              M = model matrix
//              P = projection matrix
//              V = view matrix
//              S = viewport scale matrix*
//              T = viewport translation matrix*
//
//              Mi = M = I assumed
//
//              *see Dx8.1 SDK on viewport scaling

void CSprite::UpdateScreenToWorldMatrix(void)
{
    D3DXMATRIX C, S, T;

    D3DXMatrixScaling(&S, (float) (m_Port.Width() >> 1),
                          (float) -(m_Port.Height() >> 1),
                          1.0f);
    D3DXMatrixTranslation(&T, (float) (m_Port.Left() + (m_Port.Width() >> 1)),
                              (float) (m_Port.Top() + (m_Port.Height() >> 1)),
                              -1.0f);    
    D3DXMatrixMultiply(&C, &S, &T);                // S*T
    D3DXMatrixMultiply(&C, &m_View, &C);           // V*S*T
    D3DXMatrixMultiply(&C, &m_Projection, &C);     // M*P*V*S*T
    D3DXMatrixInverse(&m_ScreenToWorld, NULL, &C); // (M*P*V*S*T)inverse
}

// NAME:        AdjustVertexAttenuated()
// DESC:        adjust vertex for attenuated point sprite
// INPUT:       pWorld...address of D3DXVECTOR3 world point
//              dvSi.....per vertex value or render state point size
// OUTPUT:      true.....if vertex is appropriately adjusted
//              false....otherwise
// COMMENTS:    m_Sprite coefficients must be set
//                  Ss = Vh*Si*sqrt(1/(A + B*De + C*(De*De))
//                  Si is per vertex value or RS point size or vertex point size
//
// AdjustVertexAttenuated is designed to alleviate problems that arise from
// round off error during rasterization. Values that cause these errors are
// values that are close to integer and values close to 1/2 that is any value X.5.
// The reason that these values are problematic is that they can induce
// different rounding error in the ref and hal devices. This is not an error in
// either device it is just a reflection of their precision.
//
// ASSUMPTION: (x,y) position is at a known non-problematic position and not adjusted

bool CSprite::AdjustVertexAttenuated(D3DXVECTOR3 *pWorld, D3DVALUE dvSi)
{
    D3DVALUE dvSize, dvTemp, dvLower, dvUpper;
    D3DXVECTOR3 V;
    UINT i, j;

    for (i = 0; i < MAX_ADJUSTMENTS; i++)
    {
        D3DXVec3TransformCoord(&V, pWorld, &m_View);
        dvTemp = sqrt(V.x*V.x + V.y*V.y + V.z*V.z); // distance from eye to point
        dvTemp = m_Sprite.Ka.f + dvTemp*(m_Sprite.Kb.f+ dvTemp*m_Sprite.Kc.f);
        dvTemp = sqrt(1.0f/dvTemp);
        dvSize = dvSi*dvTemp*((D3DVALUE) m_pCurrentMode->nBackBufferHeight);

        // avoid log boundaries eg 2**4.5 or 2**5.5 (X.5 powers are gray areas for mipmapping)
        // ASSUMPTION: mipmap textures are assumed to be square

        if (m_Flags.Check(CF_MIPMAP))
        {
            float t;

            for (j = 0; j < MIPLEVELS; j++)
            {
                t = (float) j;
                dvLower = pow(2.0f, (t+0.4f));
                dvUpper = pow(2.0f, (t+0.6f));

                if ((dvLower <= dvSize) && (dvSize <= dvUpper))
                {
                    pWorld->z += 1.0; 
                    break;
                }
            }

            if (j != MIPLEVELS)
                continue;
        }

        dvSize /= 2.0f;

        if (dvSize < 1.0f)
            return false;

        dvTemp = (int) (dvSize + 1.0f);

        if ((dvTemp - dvSize) < (MINDIFF) ||
            (dvTemp - dvSize) > (1.0f - MINDIFF) ||
            ((dvTemp - dvSize) > (0.5f - MINDIFF) &&
             (dvTemp - dvSize) < (0.5f + MINDIFF)))
            pWorld->z += DISTINC;
        else
        {
            sprintf(m_szStatus[10][0], "$gPixelSize");
            sprintf(m_szStatus[10][1], "$y%f", 2.0f*dvSize);
            return true;
        }
    }

    return false;
}

// NAME:        SetViewport()
// DESC:        set viewport
// INPUT:       nX.......x coordinate
//              nY.......y coordinate
//              nWidth...viewport width
//              nHeight..viewport height
// OUTPUT:      CD3DTest::SetViewport()
// COMMENTS:    save viewport extents before calling CD3DTest::SetViewport()

bool CSprite::SetViewport(UINT nX, UINT nY, UINT nWidth, UINT nHeight)
{
    bool bResult;
    VIEWPORT Viewport;

    Viewport.X = (DWORD) nX;
    Viewport.Y = (DWORD) nY;
    Viewport.Width = (DWORD) nWidth;
    Viewport.Height = (DWORD) nHeight;
    Viewport.MinZ = 0.0f;
    Viewport.MaxZ = 1.0f;

    m_Port.SetPort(nX, nY, nWidth, nHeight);
    bResult = CD3DTest::SetViewport(&Viewport);
    UpdateScreenToWorldMatrix();
    return bResult;
}

// NAME:        DrawScissor()
// DESC:        draw scissor
// INPUT:       none
// OUTPUT:      none

void CSprite::DrawScissor(void)
{
    DWORD dwColor = D3DCOLOR_RGBA(0, 0, 0, 255);
    MYD3DTLVERTEX Vertex[5];

    for (UINT i = 0; i < 5; i++)
    {
        Vertex[i].dvSZ = 0.5f;
        Vertex[i].dvRHW = 2.0f;
        Vertex[i].dcColor = dwColor;
        Vertex[i].dcSpecular = dwColor;
        Vertex[i].dvTU = 0.5f;
        Vertex[i].dvTV = 0.5f;
    }

    Vertex[0].dvSX = (float) m_Rect.left;
    Vertex[0].dvSY = (float) m_Rect.top;

    Vertex[1].dvSX = (float) m_Rect.right;
    Vertex[1].dvSY = (float) m_Rect.top;

    Vertex[2].dvSX = (float) m_Rect.right;
    Vertex[2].dvSY = (float) m_Rect.bottom;

    Vertex[3].dvSX = (float) m_Rect.left;
    Vertex[3].dvSY = (float) m_Rect.bottom;

    Vertex[4].dvSX = (float) m_Rect.left;
    Vertex[4].dvSY = (float) m_Rect.top;

    if (!RenderPrimitive(D3DPT_LINESTRIP, MYD3DFVF_TLVERTEX, Vertex, (DWORD) 5, NULL, 0, NULL))
        WriteToLog("RenderPrimitive() failed: %s\n", GLE(GetLastError()));
}

// NAME:        DrawViewport()
// DESC:        draw viewport
// INPUT:       none
// OUTPUT:      none

void CSprite::DrawViewport(void)
{
    DWORD dwColor = D3DCOLOR_RGBA(0, 0, 0, 255);
    MYD3DTLVERTEX Vertex[5];

    for (UINT i = 0; i < 5; i++)
    {
        Vertex[i].dvSZ = 0.5f;
        Vertex[i].dvRHW = 2.0f;
        Vertex[i].dcColor = dwColor;
        Vertex[i].dcSpecular = dwColor;
        Vertex[i].dvTU = 0.5f;
        Vertex[i].dvTV = 0.5f;
    }

    Vertex[0].dvSX = (float) m_Port.Left();
    Vertex[0].dvSY = (float) m_Port.Top();

    Vertex[1].dvSX = (float) m_Port.Right();
    Vertex[1].dvSY = (float) m_Port.Top();

    Vertex[2].dvSX = (float) m_Port.Right();
    Vertex[2].dvSY = (float) m_Port.Bottom();

    Vertex[3].dvSX = (float) m_Port.Left();
    Vertex[3].dvSY = (float) m_Port.Bottom();

    Vertex[4].dvSX = (float) m_Port.Left();
    Vertex[4].dvSY = (float) m_Port.Top();

    if (!RenderPrimitive(D3DPT_LINESTRIP, MYD3DFVF_TLVERTEX, Vertex, (DWORD) 5, NULL, 0, NULL))
        WriteToLog("RenderPrimitive() failed: %s\n", GLE(GetLastError()));
}

// NAME:        GetLastError()
// DESC:        get last error and filter per invalid parameter testing
// INPUT:       none
// OUTPUT:      NOERROR..if invalid parameter testing
//              CD3DTest::GetLastError() otherwise

HRESULT CSprite::GetLastError(void)
{
    if (m_Flags.Check(CF_INVALID))
        return NOERROR;
    else
        return CD3DTest::GetLastError();
}

// NAME:        DisableScissorTest()
// DESC:        disable scissor test
// INPUT:       none
// OUTPUT:      true.....if scissor test disabled successfully
//              false....otherwise

bool CSprite::DisableScissorTest(void)
{
    m_dcBackground = RGB_MAKE(255, 255, 255); // normal clear color for next clear

    m_Rect.top = 0;
    m_Rect.left = 0;
    m_Rect.bottom = m_pCurrentMode->nBackBufferHeight;
    m_Rect.right = m_pCurrentMode->nBackBufferWidth;
    
    return SetRenderState(D3DRS_SCISSORTESTENABLE, (DWORD) FALSE);
}

// NAME:        EnableScissorTest()
// DESC:        enable scissor test
// INPUT:       pRect....address of RECT scissor rect
// OUTPUT:      true.....if scissor test enabled successfully
//              false....otherwise

bool CSprite::EnableScissorTest(RECT *pRect)
{
    RECT Src, Ref;

    m_dcBackground = RGB_MAKE(0, 0, 255); // alternate clear color for next clear

    if (!pRect)
    {
        WriteToLog("ERROR: EnableScissorTest(): NULL scissor rect pointer.\n");
        return false;
    }

    // can call GetScissorRect() and SetScissorRect() from CD3DTest or,
    // call them from the src and ref devices directly (as below)

    if (!((m_pSrcDevice->GetBehaviors() & DEVICECREATE_PURE) ||
          (m_pRefDevice->GetBehaviors() & DEVICECREATE_PURE)))
    {
        if (D3D_OK != m_pSrcDevice->GetScissorRect(&Src))
        {
            WriteToLog("ERROR: GetScissorRect() failed on src device: %s.\n", GLE(GetLastError()));
            return false;
        }

        if (D3D_OK != m_pRefDevice->GetScissorRect(&Ref))
        {
            WriteToLog("ERROR: GetScissorRect() failed on ref device: %s.\n", GLE(GetLastError()));
            return false;
        }

        if ((Src.top != Ref.top) || (Src.left != Ref.left) ||
            (Src.bottom != Ref.bottom) || (Src.right != Ref.right))
        {
            WriteToLog("ERROR: Src and Ref scissor rects not identical.\n");
            WriteToLog("Src rect(%d,%d,%d,%d)\n", Src.top, Src.left, Src.bottom, Src.right);
            WriteToLog("Ref rect(%d,%d,%d,%d)\n", Ref.top, Ref.left, Ref.bottom, Ref.right);
            return false;
        }
    }

    m_Rect.top = pRect->top;
    m_Rect.left = pRect->left;
    m_Rect.bottom = pRect->bottom;
    m_Rect.right = pRect->right;

    if (D3D_OK != m_pSrcDevice->SetScissorRect(&m_Rect))
    {
        WriteToLog("ERROR: SetScissorRect() failed on src device: %s.\n", GLE(GetLastError()));
        return false;
    }
    else if (D3D_OK != m_pRefDevice->SetScissorRect(&m_Rect))
    {
        WriteToLog("ERROR: SetScissorRect() failed on ref device: %s.\n", GLE(GetLastError()));
        return false;
    }

    return SetRenderState(D3DRS_SCISSORTESTENABLE, (DWORD) TRUE);
}
