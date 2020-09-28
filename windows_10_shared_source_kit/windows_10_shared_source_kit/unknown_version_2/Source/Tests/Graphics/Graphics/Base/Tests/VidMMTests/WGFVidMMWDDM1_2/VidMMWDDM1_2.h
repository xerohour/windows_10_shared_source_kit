ormat == m_uCommonTextureFormats)
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
        { (float) (m_Port.Left() 