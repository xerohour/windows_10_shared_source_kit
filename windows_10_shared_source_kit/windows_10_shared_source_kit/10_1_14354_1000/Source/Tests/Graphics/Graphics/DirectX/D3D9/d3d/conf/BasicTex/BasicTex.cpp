//-----------------------------------------------------------------------------
// File: BasicTex.cpp
//
// Desc: Base class for BasicText test groups
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "BasicTex.h"

//---Test Functions-----------------------------------------------------------
//----------------------------------------------------------------------------
// Name: CBasicText()
// Desc: Construct
//----------------------------------------------------------------------------
CBasicTex::CBasicTex(void)
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);
    
    // Include all tex formats
    m_Options.ModeOptions.TextureFmt = FMT_ALL;    

	// Request all texture formats except bumpmap
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE;

    // Exclude multi-element texture formats
    m_Options.ModeOptions.dwTexExclude = PF_METN;

    // Texture variables (Public)
    m_nTextureSize      = 256;  // Size of our texture m_nTextureSize X m_nTextureSize
    m_pTexture          = NULL; // The texture
    m_dwTexturePool     = FMWK_DEFAULT; // What POOL to create the texture in
    m_bUseStripeImage   = false; // Use a strip or a gradent image
    m_uvTopLeft         = UV(0.0f, 0.0f);   // Top Left tex coord
    m_uvTopRight        = UV(1.0f,0.0f);    // Top Right tex coord
    m_uvBottomLeft      = UV(0.0f,1.0f);    // Bottom Left tex coord
    m_uvBottomRight     = UV(1.0f,1.0f);    // Bottom Right tex coord

    // Grid variables (Public)
    m_nGridXDiv         = 2;    // Number of square in the X of the mesh
    m_nGridYDiv         = 2;    // Number of square in the Y of the mesh

    // Grid variables (Private)
    m_dwVertexCount     = 0;    // Number of vertex that make up our mesh

    // Status variables (Private)
	m_szTSTName[0]       = '\0'; // Test name string
    m_szTexLoadStatus[0] = '\0'; // To update the log with the texture load method

    // Number of tests torun (Public)
    m_uNumTests          = 0;    // The number of tests to run. Chilled must set this

#if FAST_TEST
    // Add some more space to fit all test cases in one back buffer
    m_Options.D3DOptions.nBackBufferWidth = (DWORD)1024;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD)1024;
#endif //FAST_TEST
}

//---D3D Framework Derived Functions----------------------------------------------
//----------------------------------------------------------------------------
// Name: CapsCheck()
// Desc: Check caps requred by test. 
//----------------------------------------------------------------------------
bool CBasicTex::CapsCheck(void)
{
    // Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

    // Find Min and Max texture size caps
    DWORD dwMinTextureWidth  = m_pSrcDevice->GetCaps()->dwMinTextureWidth;
	DWORD dwMinTextureHeight = m_pSrcDevice->GetCaps()->dwMinTextureHeight;
	DWORD dwMaxTextureWidth  = m_pSrcDevice->GetCaps()->dwMaxTextureWidth;
	DWORD dwMaxTextureHeight = m_pSrcDevice->GetCaps()->dwMaxTextureHeight;

    // See if the MinTextureWidth is supported
	if (m_nTextureSize < dwMinTextureWidth)
	{
		WriteToLog("Texture width (%d) < MinTextureWidth (%d)\n",m_nTextureSize,dwMinTextureWidth);
		return false;
	}

	// See if the MinTextureHeight is supported
	if (m_nTextureSize < dwMinTextureHeight)
	{
		WriteToLog("Texture height (%d) < MinTextureHeight (%d)\n",m_nTextureSize,dwMinTextureHeight);
		return false;
	}

	// See if the MaxTextureWidth is supported
	if (m_nTextureSize > dwMaxTextureWidth)
	{
		WriteToLog("Texture width (%d) > MaxTextureWidth (%d)\n",m_nTextureSize,dwMaxTextureWidth);
		return false;
	}

	// See if the MaxTextureHeight is supported
	if (m_nTextureSize > dwMaxTextureHeight)
	{
		WriteToLog("Texture height (%d) > MaxTextureHeight (%d)\n",m_nTextureSize,dwMaxTextureHeight);
		return false;
	}

    // Check for subpixel
    DWORD dwMinFilterCap, dwMagFilterCap;
    if (m_dwVersion < 0x0800)
    {
        // Check the SubPixel caps
	    DWORD dwRasterCaps = m_pSrcDevice->GetCaps()->dwRasterCaps;	    
	    if (!(dwRasterCaps & D3DPRASTERCAPS_SUBPIXEL) &&
		    !(dwRasterCaps & D3DPRASTERCAPS_SUBPIXELX))
	    {
		    WriteToLog("Device capability not supported: Raster Subpixel.\n");
		    return false;
	    }
    }

    return true;
}

//----------------------------------------------------------------------------
// Name: Setup()
// Desc: Setup device specific settings, variables, ect.
// Called when devices are restored(group init, device lost, etc)
//----------------------------------------------------------------------------
bool CBasicTex::Setup(void)
{
    // Init return
    bool bResults = false;

    // Create Texture
	if( false == CreateTex() )
        return false;

    // Load Image
    if( false == LoadTexImage() )
       return false;

    // Indicate what load option is being used
    if( false == UpdateTexLoadStatus() )
        return false;

    // Should Render states be set in SetDefaultRenderStates() ?
    // Turn off culling
    SetRenderState(D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_NONE);
  
    // Make sure that perspection is off
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)false);

    // DX9C: Need to check for USAGE_QUERY_WRAPANDMIP. If no we need to clamp
   	if(m_pD3D->GetRuntimeVersion() >= 9.029f)
	{
		HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
											 USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat]);
		if (FAILED(Result))
        {
            WriteToLog("Test will use CLAMP because CheckDeviceFormat(WrapAndMip) failed.\n");
			SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, (DWORD)D3DTADDRESS_CLAMP);
        }
	}
 
    return true;
}

//----------------------------------------------------------------------------
// Name: TestInitialize()
// Desc: Setup non-device specific settings, variables, ect.
// Called once for each group if CapsCheck succeeds.
//----------------------------------------------------------------------------
UINT CBasicTex::TestInitialize(void)
{
#if FAST_TEST
    SetTestRange(1, 1);
#else
    SetTestRange(1, m_uNumTests);
#endif

	return D3DTESTINIT_RUN;
}

//----------------------------------------------------------------------------
// Name: ExecuteTest(UINT uTestNum)
// Desc: Called once for each test defined by SetTestRange() call
//----------------------------------------------------------------------------
#if FAST_TEST
bool CBasicTex::ExecuteTest(UINT uTestNum)
{

    // Create Mesh
    DrawGrid();

    // Render the test.
    RenderTest(uTestNum); // Should ExecuteTest() return fails if this fails?

    return true;
}
#else//FAST_TEST
bool CBasicTex::ExecuteTest(UINT uTestNum)
{
    // Initialize some variables
    HRESULT hrResult = S_OK;
    m_szTSTName[0] = '\0';

    // Create Mesh
    DrawGrid();

    // Tell the log that we are starting
    sprintf(m_szTSTName, "TexSize: %ix%i GridDiv: %ix%i", m_nTextureSize, m_nTextureSize, m_nGridXDiv, m_nGridYDiv );
    sprintf(m_szTSTName, "%s TexMap: (%.1f,%.1f)(%.1f,%.1f)(%.1f,%.1f)(%.1f,%.1f)", 
                                                                m_szTSTName, m_uvTopLeft.u, m_uvTopLeft.v,
                                                                             m_uvTopRight.u, m_uvTopRight.v,
                                                                             m_uvBottomLeft.u, m_uvBottomLeft.v,
                                                                             m_uvBottomRight.u, m_uvBottomRight.v);
    BeginTestCase(m_szTSTName, uTestNum);

    // Render the test.
    RenderTest(); // Should ExecuteTest() return fails if this fails?

    return true;
}
#endif//FAST_TEST

//----------------------------------------------------------------------------
// Name: ClearFrame()
// Desc: Clear our buffers
//----------------------------------------------------------------------------
bool CBasicTex::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

//----------------------------------------------------------------------------
// Name: UpdateStatus()
// Desc: Called once each frame to update status window text
//----------------------------------------------------------------------------
void CBasicTex::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

    // Tell status window what load method we are using
	WriteStatus("$wTexture Load", m_szTexLoadStatus);
}

//----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Cleanup device specific settings, variables, ect. 
// Tied with Setup()
//----------------------------------------------------------------------------
void CBasicTex::Cleanup(void)
{   
    // Cleanup texture 
    RELEASE(m_pTexture);
    m_pTexture = NULL;
}

//---Test Functions-----------------------------------------------------------
//----------------------------------------------------------------------------
// Name: CreateTex()
// Desc: Creates a texture (m_pTexture)
//----------------------------------------------------------------------------
bool CBasicTex::CreateTex(void)
{
    // Initialize tex format variables
    FORMAT* pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
    TCHAR szFormat[80];
    FmtToString(FORMAT_TEXTURE, pFormat, szFormat);
    
    // Cleanup previous texture
    SetTexture(0, NULL);
    RELEASE(m_pTexture);
    m_pTexture = NULL;

    // Create the texture
    bool bResult = CreateTexture(m_nTextureSize, m_nTextureSize, 1, 0, *pFormat, m_dwTexturePool, &m_pTexture);

    // Tell log about CreateTexture failure
	if( !bResult || (NULL == m_pTexture) )
    {
        // Tell log about failure
        HRESULT hrResult = GetLastError();
		WriteToLog(_T("CreateTexture() failed with HResult = %s \n"), m_pD3D->HResultToString( hrResult ));
                    
        // Tell log about create texture input
        TCHAR szPool[80]; // Get memory pool string
        if( false == PoolToString( m_dwTexturePool, szPool) )
            WriteToLog(_T("Test Error: Memory pool is invalid.\n"));

        WriteToLog(_T("Creating texture with - Size: %ix%i, Levles: 1, Usage: 0, Format: %s, Pool: %s\n"), 
                m_nTextureSize, m_nTextureSize, szFormat, szPool );

        // Clean our texture pointer
        RELEASE(m_pTexture);
        m_pTexture = NULL;

        return false;
	}

    // Set the palette if needed
	if( m_pTexture->IsPalettized() )
		m_pTexture->SetPalette(NULL);

    return true;
}

//----------------------------------------------------------------------------
// Name: LoadTexImage()
// Desc: Loads an image onto m_pTexture using LoadGradient or LoadStripes
// depending  on m_bUseStripeImage
//----------------------------------------------------------------------------
bool CBasicTex::LoadTexImage()
{
    // Make sure our textur is not null
    if( NULL == m_pTexture )
    {
        WriteToLog(_T("Test Error: m_pTexture is NULL"));
        return false;
    }

    // Load image onto the texture...
    if( true == m_bUseStripeImage )
    {   // ... using strips
        // Load colors
        D3DCOLOR d3dColorsStripe[] = {RGBA_MAKE(255,255,255,255),RGBA_MAKE(255,0,0,255),       // White, Red
                                        RGBA_MAKE(0,255,0,255), RGBA_MAKE(0,0,255,255),        // Green, Blue
                                        RGBA_MAKE(255,255,0,255), RGBA_MAKE(255,0,255,255),    // Yellow, Magenta
                                        RGBA_MAKE(0,255,255,255), RGBA_MAKE(128,128,128,255)}; // Cyan, Grey

        if (!m_pTexture->LoadStripes(1, (DWORD)m_nTextureSize, (DWORD)m_nTextureSize, (DWORD)8, d3dColorsStripe, false))
        {
            WriteToLog(_T("LoadStripes() failed with HResult = %s.\n"),m_pD3D->HResultToString(GetLastError()));
            return false;
         }
    }
    else
    {   // ... using gradient
        // Load colors
        D3DCOLOR d3dColorsGradient[] = {RGBA_MAKE(255,0,0,255),RGBA_MAKE(0,255,0,255),      // Red, Green
                                        RGBA_MAKE(0,0,255,255),RGBA_MAKE(255,255,255,255)}; // Blue, White

        // Load
        if (!m_pTexture->LoadGradient(1, (DWORD)m_nTextureSize, (DWORD)m_nTextureSize, d3dColorsGradient))
        {
                WriteToLog(_T("LoadGradient() failed with HResult = %s.\n"),m_pD3D->HResultToString(GetLastError()));
                return false;
        }
    }
   
    // Set the valid texture
	if( false == SetTexture(0, m_pTexture) )
	{
		WriteToLog(_T("SetTexture() failed with HResult = %s.\n"),m_pD3D->HResultToString(GetLastError()));
		return false;
	}

    return true;
}

//----------------------------------------------------------------------------
// Name: UpdateTexLoadStatus()
// Desc: Updates m_szTexLoadStatus to have the tex load option used.
//----------------------------------------------------------------------------
bool CBasicTex::UpdateTexLoadStatus(void)
{
    // Clean status string
    m_szTexLoadStatus[0] = '\0';

    // Indicate which load option was used
	switch (m_pSrcDevice->GetTexLoad())
	{    
		case TEXLOAD_UPDATETEXTURE:
        	WriteToLog(_T("Using texture load option: UpdateTex\n"));
        	sprintf(m_szTexLoadStatus, "%s$aUpdateTex",m_szTexLoadStatus);
			break;
		case TEXLOAD_COPYRECTS:
        	WriteToLog(_T("Using texture load option: CopyRects\n"));
        	sprintf(m_szTexLoadStatus, "%s$aCopyRects",m_szTexLoadStatus);
			break;
		case TEXLOAD_LOCKCOPY:
        	WriteToLog(_T("Using texture load option: LockCopy\n"));
        	sprintf(m_szTexLoadStatus, "%s$aLockCopy",m_szTexLoadStatus);
			break;
		case TEXLOAD_UPDATESURFACE:
        	WriteToLog(_T("Using texture load option: UpdateSurf\n"));
        	sprintf(m_szTexLoadStatus, "%s$aUpdateSurf",m_szTexLoadStatus);
			break;
		default:
			WriteToLog(_T("Error: Unknown texture load option (%x).\n"),m_pSrcDevice->GetTexLoad());
        	sprintf(m_szTexLoadStatus, "%s$rUnknown (%x)",m_szTexLoadStatus,m_pSrcDevice->GetTexLoad());
			return false;
	}

    return true;
}

//----------------------------------------------------------------------------
// Name: PoolToString()
// Desc: Converts memory pool to string
//----------------------------------------------------------------------------
bool CBasicTex::PoolToString(DWORD dwPool, LPTSTR szOut)
{
    // Make sure we have a output string
    if(NULL == szOut)
    {
        sprintf(szOut, _T("Test Error: Bad input to PoolToString()"));
        return false;
    }
    
    // If framework default, get the default
    if( FMWK_DEFAULT == dwPool )
    {
        // Not sure how to get framework default yet.
        sprintf(szOut, _T("FMWK_DEFAULT"));
        return true;;
    }

    // Convert to string
    switch(dwPool)
    {
        case POOL_DEFAULT:
            sprintf(szOut, _T("POOL_DEFAULT"));
            break;
        case POOL_MANAGED:
            sprintf(szOut, _T("POOL_MANAGED"));
            break;
        case POOL_SYSTEMMEM:
            sprintf(szOut, _T("POOL_SYSTEMMEM"));
            break;
        case POOL_SCRATCH:
            sprintf(szOut, _T("POOL_SCRATCH"));
            break;
        case POOL_VIDEO:
            sprintf(szOut, _T("POOL_VIDEO"));
            break;
        case POOL_AGP:
            sprintf(szOut, _T("POOL_AGP"));
            break;
        case POOL_OFFSCREEN:
            sprintf(szOut, _T("POOL_OFFSCREEN"));
            break;
        default: // Error
            sprintf(szOut, _T("Unknown memory pool"));
            return false;
            break;
    }
    return true;
}

//----------------------------------------------------------------------------
// Name: DrawGrid()
// Desc: Draws a grid based on m_nGridXDiv by m_nGridYDiv 
//----------------------------------------------------------------------------
void CBasicTex::DrawGrid()
{   
    // Reset vertex count
    m_dwVertexCount = 0;
      
    // Find what spacing we need to use
    float fGridSize  = 256.0f; // Size of mesh grid  
    float fXSpaceing = fGridSize / m_nGridXDiv;
    float fYSpaceing = fGridSize / m_nGridYDiv;

    float fOffSet = 9.5f;

    // Build grid.
    float fU, fV;
    for(int ny = 0; ny < m_nGridYDiv; ny++)
    {
        float fyTop = fYSpaceing * (float)ny;
        float fyBottom = fYSpaceing * (float)(ny+1);
        for(int nx = 0; nx <= m_nGridXDiv; nx++)
        {
            float fx = fXSpaceing * (float)nx;
        
            // Top
            CalcUV(nx, ny, &fU, &fV);
            m_VertexList[m_dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fx+fOffSet, fyTop+fOffSet, 0.9f), (1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,fU,fV);

            // Bottom
            CalcUV(nx, ny+1, &fU, &fV);
            m_VertexList[m_dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fx+fOffSet, fyBottom+fOffSet, 0.9f), (1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,fU,fV);

            // Move down.
            if(nx == m_nGridXDiv && ny+1 != m_nGridYDiv)
            {
                // Bottom Left
                CalcUV(nx, ny+1, &fU, &fV);
                m_VertexList[m_dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fx+fOffSet, fyBottom+fOffSet, 0.9f), (1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,fU,fV);

                // Bottom Right
                CalcUV(0, ny+1, &fU, &fV);
                m_VertexList[m_dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fOffSet, fyBottom+fOffSet, 0.9f), (1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,fU,fV);
            }

        }
    }
}

//----------------------------------------------------------------------------
// Name: CalcUV()
// Desc: Returns the UV coords for a row col mix based on m_uv*
//----------------------------------------------------------------------------
void CBasicTex::CalcUV(int nX, int nY, float* uOut, float* vOut)
{   
    // Left
    UV uvLeft;
    uvLeft.u = CalcCoord(m_uvTopLeft.u, m_uvBottomLeft.u, nY, m_nGridXDiv);
    uvLeft.v = CalcCoord(m_uvTopLeft.v, m_uvBottomLeft.v, nY, m_nGridYDiv);

    // Right
    UV uvRight;
    uvRight.u = CalcCoord(m_uvTopRight.u, m_uvBottomRight.u, nY, m_nGridXDiv);
    uvRight.v = CalcCoord(m_uvTopRight.v, m_uvBottomRight.v, nY, m_nGridYDiv);

    // Final
    *uOut = CalcCoord(uvLeft.u, uvRight.u, nX, m_nGridXDiv);
    *vOut = CalcCoord(uvLeft.v, uvRight.v, nX, m_nGridXDiv);
}

//----------------------------------------------------------------------------
// Name: CalcCoord()
// Desc: Helper divide for CalcUV
//----------------------------------------------------------------------------
float CBasicTex::CalcCoord(float fFirst, float fSecond, int nStepping, int nGridDivide)
{
    // If the same make sticky
    if( fFirst == fSecond )
        return fFirst;

    // Get divide increment
    float DI = (fSecond-fFirst)/(float)nGridDivide;

    return fFirst + (DI*(float)nStepping);
}

//----------------------------------------------------------------------------
// Name: RenderTest()
// Desc: Renders the frame
//----------------------------------------------------------------------------
#if FAST_TEST
bool CBasicTex::RenderTest( UINT testNum )
{
    // Init return
    bool bResult = false;

    // Try BeginScene
    if( false == BeginScene() )
    {
        // Tell log about failure
        WriteToLog("BeginScene() failed.");

        // Fail Test?
        return bResult;
    }

    // Render the data.
    if( 0 != m_dwVertexCount )
    {
        // Transform and scale the vertices so they all fit on the screen
        {
            const DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
            const DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
            UINT numColumns = (UINT)ceil(sqrt((float)m_uNumTests));
            UINT numRows = numColumns;
            const float widthPerOp = float(dwWidth) / float(numColumns);
            const float heightPerOp = float(dwHeight) / float(numRows);

            float scaleX = (widthPerOp / float(dwWidth)) + 0.1f;//add some scale factor if they are too small
            float scaleY = (widthPerOp / float(dwHeight)) + 0.1f;

            float transX = (testNum % numColumns) * widthPerOp;
            float transY = (testNum / numColumns) * heightPerOp;
            for (size_t i = 0; i < m_dwVertexCount; i++)
            {
                m_VertexList[i].sx *= scaleX;
                m_VertexList[i].sx += transX;

                m_VertexList[i].sy *= scaleY;
                m_VertexList[i].sy += transY;
            }
        }

        bResult = RenderPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, m_VertexList, m_dwVertexCount, NULL, 0,0);
        if( false == bResult )
        {
            // Tell log about failure
            WriteToLog(_T("RenderPrimitive() failed with HResult = %s "), m_pD3D->HResultToString(GetLastError()));

            // Fail Test?
        }
    }

    // Done
    EndScene();

    return bResult;
}
#else //FAST_TEST
bool CBasicTex::RenderTest(void)
{
    // Init return
    bool bResult = false;

    // Try BeginScene
    if( false == BeginScene() )
	{
		// Tell log about failure
		WriteToLog("BeginScene() failed.");
        
        // Fail Test?
        return bResult;
    }

    // Render the data.
    if( 0 != m_dwVertexCount )
    {
        bResult = RenderPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, m_VertexList, m_dwVertexCount, NULL, 0,0);
		if( false == bResult )
        {
            // Tell log about failure
            WriteToLog(_T("RenderPrimitive() failed with HResult = %s "), m_pD3D->HResultToString(GetLastError()));

            // Fail Test?
        }
    }

    // Done
	EndScene();

    return bResult;
}
#endif //FAST_TEST

#if FAST_TEST
bool CBasicTex::AddFormatModes(DISPLAYMODE *pMode, bool bWindowed)
{
    UINT uTexture = 0;
    UINT uZBuffer = 0;
    UINT uCube = 0;
    UINT uVolume = 0;
    UINT uMS = 0;
    UINT uBB = 0;

    // Convert the formats into indexes into the common lists
    if (!GetFormatIndex(m_Options.ModeOptions.TextureFmt, m_pCommonTextureFormats, &uTexture) ||
        !GetZBufferFormatIndex(m_Options.ModeOptions.ZBufferFmt, m_pCommonZBufferFormats, &(pMode->Format), &uZBuffer) ||
        !GetFormatIndex(m_Options.ModeOptions.CubeFmt, m_pCommonCubeFormats, &uCube) ||
        !GetFormatIndex(m_Options.ModeOptions.VolumeFmt, m_pCommonVolumeFormats, &uVolume) ||
        !GetFormatIndex(m_Options.ModeOptions.MultisampType, m_pCommonMultiSampleFormats, m_uCommonMultiSampleFormats, &uMS) ||
        !GetBackBufferFormatIndex(m_Options.ModeOptions.BackBufferFmt, m_pCommonBackBufferFormats, &(pMode->Format), &uBB) ||
        // We need to validate MS here.  If the device doens't support this MS type, we don't want to add it as a mode
        (m_pCommonMultiSampleFormats && m_pCommonZBufferFormats &&
        (FAILED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format, bWindowed, m_pCommonMultiSampleFormats[uMS], NULL)) ||
        FAILED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonZBufferFormats[uZBuffer], bWindowed, m_pCommonMultiSampleFormats[uMS], NULL)))))
    {
        DPF(2, _T("Requested fmt not supported by device.\n"));
        return false;
    }

    // If only one mode was requested, add it and return
    if (m_Options.ModeOptions.TextureFmt != FMT_ALL &&
        m_Options.ModeOptions.ZBufferFmt != FMT_ALL &&
        m_Options.ModeOptions.CubeFmt != FMT_ALL &&
        m_Options.ModeOptions.VolumeFmt != FMT_ALL &&
        m_Options.ModeOptions.BackBufferFmt != FMT_ALL &&
        m_Options.ModeOptions.MultisampType != MULTISAMPLE_ALL)
    {
        return AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, uVolume, uMS, 0, uBB);
    }

    if (m_Options.ModeOptions.BackBufferFmt == FMT_ALL)
    {
        for (UINT nBB = 0; nBB < m_uCommonBackBufferFormats; nBB++)
        {
            // Filter out unsupported BB modes based on ModeOptions
            if (m_pD3D->GetVersion() < 8.f)
            {
                if (pMode->Format.d3dfFormat != m_pCommonBackBufferFormats[nBB].d3dfFormat)
                    continue;
            }
            else if (FAILED(m_pD3D->CheckDeviceType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format,
                m_pCommonBackBufferFormats[nBB], bWindowed)))
                continue;

            if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, uVolume, uMS, 0, nBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.TextureFmt == FMT_ALL)
    {
        for (UINT nTexture = 0; nTexture < m_uCommonTextureFormats; nTexture++)
        {
            if (m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_A8R8G8B8 || m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_X4R4G4B4
                || m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_DXT1 || m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_A2B10G10R10)
            {
                if (!AddMode(pMode, bWindowed, nTexture, uZBuffer, uCube, uVolume, uMS, 0, uBB))
                {
                    DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                    return false;
                }
            }
        }
    }

    if (m_Options.ModeOptions.ZBufferFmt == FMT_ALL)
    {
        for (UINT nZBuffer = 0; nZBuffer < m_uCommonZBufferFormats; nZBuffer++)
        {
            if (!AddMode(pMode, bWindowed, uTexture, nZBuffer, uCube, uVolume, uMS, 0, uBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.CubeFmt == FMT_ALL)
    {
        for (UINT nCube = 0; nCube < m_uCommonCubeFormats; nCube++)
        {
            if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, nCube, uVolume, uMS, 0, uBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.VolumeFmt == FMT_ALL)
    {
        for (UINT nVolume = 0; nVolume < m_uCommonVolumeFormats; nVolume++)
        {
            if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, nVolume, uMS, 0, uBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.MultisampType == FMT_ALL)
    {
        DWORD dwMSQuality = 0;

        for (UINT nMS = 0; nMS < m_uCommonMultiSampleFormats; nMS++)
        {
            int nValidBB = -1, nValidZBuffer = -1;

            // For each MS type, find a BB and ZBuffer that support that type
            for (UINT nBB = 0; nBB < m_uCommonBackBufferFormats; nBB++)
            for (UINT nZBuffer = 0; nZBuffer < m_uCommonZBufferFormats; nZBuffer++)
            {
                // If specific modes are requested, only allow those modes to be matched.
                if (m_Options.ModeOptions.BackBufferFmt != FMT_ALL && m_Options.ModeOptions.BackBufferFmt != FMT_ANY &&
                    nBB != uBB)
                    continue;
                if (m_Options.ModeOptions.ZBufferFmt != FMT_ALL && m_Options.ModeOptions.ZBufferFmt != FMT_ANY &&
                    nZBuffer != uZBuffer)
                    continue;

                // Filter out unsupported BB modes based on ModeOptions
                if (SUCCEEDED(m_pD3D->CheckDeviceType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format, m_pCommonBackBufferFormats[nBB], bWindowed)) &&
                    SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonBackBufferFormats[nBB], bWindowed, m_pCommonMultiSampleFormats[nMS], &dwMSQuality)) &&
                    SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonZBufferFormats[nZBuffer], bWindowed, m_pCommonMultiSampleFormats[nMS], &dwMSQuality)))
                {
                    nValidBB = nBB;
                    nValidZBuffer = nZBuffer;
                    break;
                }
            }

            if (nValidBB == -1 || nValidZBuffer == -1)
                continue;

            for (UINT nMSQ = 0; nMSQ < dwMSQuality; nMSQ++)
            {
                if (!AddMode(pMode, bWindowed, uTexture, nValidZBuffer, uCube, uVolume, nMS, nMSQ, nValidBB))
                {
                    DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                    return false;
                }
            }
        }
    }

    return true;
}
#endif //FAST_TEST