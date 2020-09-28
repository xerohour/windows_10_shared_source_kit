//-----------------------------------------------------------------------------
// File: Mapping.cpp
//
// Desc: Mapping test group. Cycles texture mappings
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "BasicTex.h"

//---Test Functions-----------------------------------------------------------
//----------------------------------------------------------------------------
// Name: CMapping()
// Desc: Construct
//----------------------------------------------------------------------------
CMapping::CMapping(void)
{
    // Init Framework options
    m_szTestName = _T("Mapping");
	m_szCommandKey = _T("Mapping");
 
    // Initialize the UV array's values
    m_uvlstTopLeft[0] = UV(0.0f, 0.0f);
    m_uvlstTopLeft[1] = UV(0.0f, 1.0f);
    m_uvlstTopLeft[2] = UV(1.0f,  1.0f);
    m_uvlstTopLeft[3] = UV(1.0f,  0.0f);

    m_uvlstTopRight[0] = UV(1.0f,  0.0f);
    m_uvlstTopRight[1] = UV(0.0f, 0.0f);
    m_uvlstTopRight[2] = UV(0.0f, 1.0f);
    m_uvlstTopRight[3] = UV(1.0f,  1.0f);

    m_uvlstBottomLeft[0] = UV(0.0f, 1.0f);
    m_uvlstBottomLeft[1] = UV(1.0f,  1.0f);
    m_uvlstBottomLeft[2] = UV(1.0f,  0.0f);
    m_uvlstBottomLeft[3] = UV(0.0f, 0.0f);

    m_uvlstBottomRight[0] = UV(1.0f,  1.0f);
    m_uvlstBottomRight[1] = UV(1.0f,  0.0f);
    m_uvlstBottomRight[2] = UV(0.0f, 0.0f);
	m_uvlstBottomRight[3] = UV(0.0f, 1.0f);

    // Use a gradient
    m_bUseStripeImage = false;

    // Get Grid div from command line.
    TCHAR	tcsBuffer[512];
    ReadString(_T("GridDiv"),_T("256x256"), tcsBuffer, ARRAYSIZE(tcsBuffer));
    ParseGridDiv(tcsBuffer, &m_nGridXDiv, &m_nGridYDiv);

    // Make sure we are within range
    if( GRIDSIZE_MAX < m_nGridXDiv || 0 == m_nGridXDiv )
        m_nGridXDiv = GRIDSIZE_MAX;

    if( GRIDSIZE_MAX < m_nGridYDiv || 0 == m_nGridYDiv )
        m_nGridYDiv = GRIDSIZE_MAX;

    // Get Texture Size from command line
    ReadInteger(_T("TexSize"), 256, (int*)&m_nTextureSize, false);

    // Add to the number of tests.
    m_uNumTests = MAPPINGTESTS + MAPPINGTESTS_TILE;
}

//----------------------------------------------------------------------------
// Name: ParseGridDiv(TCHAR *pcsGrid, int *pX, int *pY)
// Desc: Parses a grid string NUMxNUM
//----------------------------------------------------------------------------
bool CMapping::ParseGridDiv(TCHAR *pcsGrid, int *pX, int *pY)
{
    bool  bRet = false;
    TCHAR *sGrid = NULL;
	TCHAR *sX, *sY;

    if(NULL == pcsGrid)
		return false;

    COPYSTRING(sGrid, pcsGrid);
    if(NULL == sGrid)
        return false;

	// Get the Y
    sY = _tcsrchr(sGrid, _T('x'));
    if(NULL == sY)
	{
        WriteToLog("sY == NULL\n");
		delete [] sGrid;
		return false;
	}
	*sY = _T('\0'); sY++;

    // Get the X
    sX = sGrid;

    // Covert the values
	if(pY && atoi(sY) != 0)
	{
		*pY = atoi(sY);
		bRet = true;
	}
    if(pX && atoi(sX) != 0)
	{
		*pX = atoi(sX);
		bRet = true;
	}

    SAFEDELETEA(sGrid);
    return bRet;
}


//---D3D Framework Derived Functions----------------------------------------------
//----------------------------------------------------------------------------
// Name: CommandLineHelp(void)
// Desc: Command line help for the test
//----------------------------------------------------------------------------
void CMapping::CommandLineHelp(void)
{
    WriteCommandLineHelp(_T("-TexSize:SIZE     Texture will be created with SIZExSIZE"));
    WriteCommandLineHelp(_T("-GriDiv:WxH       Grid will be made up of WxH"));

    WriteCommandLineHelp(_T("This test group maps a texture of size %ix%i onto a "), m_nTextureSize, m_nTextureSize);
    WriteCommandLineHelp(_T("grid of %ix%i uses Liner filtering."), m_nGridXDiv, m_nGridYDiv);
    WriteCommandLineHelp(_T("This test group cycles texture mapping coordinates and texture formats."));
    WriteCommandLineHelp(_T("The test name indicates what mapping is being used. For example:"));
    WriteCommandLineHelp(_T("(0.0,0.0)(1.0,0.0)(0.0,1.0)(1.0,1.0) Indicates the top left corner of the grid is mapped to "));
    WriteCommandLineHelp(_T("u0,v0 in the texture, the top right is mapped to u1,v0, the "));
    WriteCommandLineHelp(_T("bottom left is mapped to u0,v1, and the bottom right is mapped to "));
    WriteCommandLineHelp(_T("u1,v1. Based on the mapping, each intersect of the grid is then calculated."));
    WriteCommandLineHelp(_T("Note: Grid is drawn CCW."));
}

//----------------------------------------------------------------------------
// Name: CapsCheck()
// Desc: Check caps requred by test. 
//----------------------------------------------------------------------------
bool CMapping::CapsCheck(void)
{
    // Texture filter and raster caps
    DWORD dwMinFilterCap, dwMagFilterCap;
    DWORD dwTextureFilterCaps = m_pSrcDevice->GetCaps()->dwTextureFilterCaps;
    if(m_dwVersion < 0x0800)
    {
        dwMinFilterCap = D3DPTFILTERCAPS_LINEAR;
        dwMagFilterCap = D3DPTFILTERCAPS_LINEAR;
    }
    else
    {
        dwMinFilterCap = D3DPTFILTERCAPS_MINFLINEAR;
	    dwMagFilterCap = D3DPTFILTERCAPS_MAGFLINEAR;
    }
    
	// Check the appropriate Min Filter cap
	if(!(dwTextureFilterCaps & dwMinFilterCap))
	{
		WriteToLog(_T("Device capability not supported: (Min) Texture Filter %s.\n"),m_szCommandKey);
		return false;
	}

	// Check the appropriate Mag Filter cap
	if(!(dwTextureFilterCaps & dwMagFilterCap))
	{
		WriteToLog(_T("Device capability not supported: (Mag) Texture Filter %s.\n"),m_szCommandKey);
		return false;
	}

    return CBasicTex::CapsCheck();
}

//----------------------------------------------------------------------------
// Name: Setup()
// Desc: Setup device specific settings, variables, ect.
// Called when devices are restored(group init, device lost, etc)
//----------------------------------------------------------------------------
bool CMapping::Setup(void)
{
    // See if we can use linear texture filtering. If not fall back to point
    if (m_dwVersion >= 0x0900)
	{
		HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
											 USAGE_QUERY_FILTER, RTYPE_TEXTURE, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat]);
		if(FAILED(Result))
        {
            // Try to use point filter
            DWORD dwTextureFilterCaps = m_pSrcDevice->GetCaps()->dwTextureFilterCaps;
            if( 0 == (dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT) || 0 == (dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT) )
            {
                // Not sure what to do if this fails
                WriteToLog("Unable to user Point or Linear filter\n");
                return false;
            }
            else
            {                
                // Use Point sample filtering;
                WriteToLog("Using Point Filter\n");
	            SetRenderState(D3DRENDERSTATE_TEXTUREMAG, (DWORD)D3DFILTER_NEAREST);
	            SetRenderState(D3DRENDERSTATE_TEXTUREMIN, (DWORD)D3DFILTER_NEAREST);
            }
			
        }
        else
        {
            // Use linear filtering
            WriteToLog("Using Linear Filter\n");
	        SetRenderState(D3DRENDERSTATE_TEXTUREMAG, (DWORD)D3DFILTER_LINEAR);
	        SetRenderState(D3DRENDERSTATE_TEXTUREMIN, (DWORD)D3DFILTER_LINEAR);
        }
	}

	return CBasicTex::Setup();
}

//----------------------------------------------------------------------------
// Name: ExecuteTest(UINT uTestNum)
// Desc: Called once for each test defined by SetTestRange() call
//----------------------------------------------------------------------------
#if FAST_TEST
bool CMapping::ExecuteTest(UINT uTestNum)
{    

    BeginTestCase("Mapping - Fast", uTestNum);

    bool success = true;
    for (size_t uTestNum = 1; uTestNum <= m_uNumTests; uTestNum++)
    {
        // Decide which UV coordinates to use
        int nMappingTest = ((uTestNum-1) % (MAPPINGTESTS + MAPPINGTESTS_TILE)) + 1;
        if(nMappingTest <= MAPPINGTESTS)
        {
            int nTL = ((nMappingTest - 1) / 64) % 4;
            int nTR = ((nMappingTest - 1) / 16) % 4;
            int nBL = ((nMappingTest - 1) / 4) % 4;
            int nBR = ((nMappingTest - 1) / 1) % 4;
            m_uvTopLeft     = m_uvlstTopLeft[nTL];
            m_uvTopRight    = m_uvlstTopRight[nTR];
            m_uvBottomLeft  = m_uvlstBottomLeft[nBL];
            m_uvBottomRight = m_uvlstBottomRight[nBR];
        }
        else
        {
            int nRot;
            float fTile;

            // Decide which rotation to use
            nRot = (nMappingTest - (MAPPINGTESTS+1)) / 15;

            // Decide how much tiling to use
            if( nMappingTest <= (nRot * 15 + 264) )
                fTile = (nMappingTest - (nRot * 15 + 256.0f)) / 8.0f;
            else
                fTile = nMappingTest - (nRot * 15 + 263.0f);

            m_uvTopLeft     = UV(fTile*m_uvlstTopLeft[nRot].u,    fTile*m_uvlstTopLeft[nRot].v);
            m_uvTopRight    = UV(fTile*m_uvlstTopRight[nRot].u,   fTile*m_uvlstTopRight[nRot].v);
            m_uvBottomLeft  = UV(fTile*m_uvlstBottomLeft[nRot].u, fTile*m_uvlstBottomLeft[nRot].v);
            m_uvBottomRight = UV(fTile*m_uvlstBottomRight[nRot].u,fTile*m_uvlstBottomRight[nRot].v);

        }

        success =  CBasicTex::ExecuteTest(uTestNum);
        if (!success)
        {
            break;
        }
    }

    return success;
}
#else //FAST_TEST
bool CMapping::ExecuteTest(UINT uTestNum)
{    
    // Decide which UV coordinates to use
    int nMappingTest = ((uTestNum-1) % (MAPPINGTESTS + MAPPINGTESTS_TILE)) + 1;
    if(nMappingTest <= MAPPINGTESTS)
    {
	    int nTL = ((nMappingTest - 1) / 64) % 4;
	    int nTR = ((nMappingTest - 1) / 16) % 4;
	    int nBL = ((nMappingTest - 1) / 4) % 4;
	    int nBR = ((nMappingTest - 1) / 1) % 4;
        m_uvTopLeft     = m_uvlstTopLeft[nTL];
        m_uvTopRight    = m_uvlstTopRight[nTR];
        m_uvBottomLeft  = m_uvlstBottomLeft[nBL];
        m_uvBottomRight = m_uvlstBottomRight[nBR];
    }
    else
    {
        int nRot;
		float fTile;

		// Decide which rotation to use
		nRot = (nMappingTest - (MAPPINGTESTS+1)) / 15;

		// Decide how much tiling to use
		if( nMappingTest <= (nRot * 15 + 264) )
			fTile = (nMappingTest - (nRot * 15 + 256.0f)) / 8.0f;
		else
			fTile = nMappingTest - (nRot * 15 + 263.0f);

        m_uvTopLeft     = UV(fTile*m_uvlstTopLeft[nRot].u,    fTile*m_uvlstTopLeft[nRot].v);
        m_uvTopRight    = UV(fTile*m_uvlstTopRight[nRot].u,   fTile*m_uvlstTopRight[nRot].v);
        m_uvBottomLeft  = UV(fTile*m_uvlstBottomLeft[nRot].u, fTile*m_uvlstBottomLeft[nRot].v);
        m_uvBottomRight = UV(fTile*m_uvlstBottomRight[nRot].u,fTile*m_uvlstBottomRight[nRot].v);

    }

    return CBasicTex::ExecuteTest(uTestNum);
}
#endif //FAST_TEST