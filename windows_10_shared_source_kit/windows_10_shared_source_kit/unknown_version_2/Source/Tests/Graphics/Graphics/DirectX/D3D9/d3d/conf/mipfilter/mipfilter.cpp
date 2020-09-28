#define D3D_OVERLOADS
#include <d3d.h>
#include "MipFilter.h"

// Global Test and App instance
CD3DWindowFramework		App;
#if FAST_TEST
// MipFilter = POINT cases
//
CPoint_MinPointTest			Point_MinPoint;
CPoint_MinLinearTest		Point_MinLinear;
CPoint_NonSquareWTest		Point_NonSquareWidth;
CPoint_NonSquareHTest		Point_NonSquareHeight;

CPoint_FullSquareUTTest		Point_FullSquareUT;

CPoint_FullNonSquareWCRTest	Point_FullNonSquareWidthCR;

CPoint_FullNonSquareHLCTest	Point_FullNonSquareHeightLC;
CPoint_FullNonSquareHUSTest	Point_FullNonSquareHeightUS;

CPoint_MaxMipLevelTest		Point_MaxMipLevel;
CPoint_LODBiasTest			Point_LODBias;

// MipFilter = LINEAR cases
//
CLinear_MinPointTest		    Linear_MinPoint;
CLinear_MinLinearTest		    Linear_MinLinear;
CLinear_NonSquareWTest		    Linear_NonSquareWidth;
CLinear_NonSquareHTest		    Linear_NonSquareHeight;

CLinear_MaxMipLevelTest		    Linear_MaxMipLevel;
CLinear_LODBiasTest			    Linear_LODBias;

// MipFilter = NONE cases
//
CNone_FullSquareTest		None_FullSquare;
CNone_MaxMipLevelTest		None_MaxMipLevel;
CNone_LODBiasTest			None_LODBias;
#else//FAST_TEST
// MipFilter = POINT cases
//
CPoint_MinPointTest			Point_MinPoint;
CPoint_MinLinearTest		Point_MinLinear;
CPoint_NonSquareWTest		Point_NonSquareWidth;
CPoint_NonSquareHTest		Point_NonSquareHeight;
CPoint_FullSquareUTTest		Point_FullSquareUT;
CPoint_FullSquareCRTest		Point_FullSquareCR;
CPoint_FullSquareLCTest		Point_FullSquareLC;
CPoint_FullSquareUSTest		Point_FullSquareUS;
CPoint_FullNonSquareWUTTest	Point_FullNonSquareWidthUT;
CPoint_FullNonSquareWCRTest	Point_FullNonSquareWidthCR;
CPoint_FullNonSquareWLCTest	Point_FullNonSquareWidthLC;
CPoint_FullNonSquareWUSTest	Point_FullNonSquareWidthUS;
CPoint_FullNonSquareHUTTest	Point_FullNonSquareHeightUT;
CPoint_FullNonSquareHCRTest	Point_FullNonSquareHeightCR;
CPoint_FullNonSquareHLCTest	Point_FullNonSquareHeightLC;
CPoint_FullNonSquareHUSTest	Point_FullNonSquareHeightUS;
CPoint_MaxMipLevelTest		Point_MaxMipLevel;
CPoint_LODBiasTest			Point_LODBias;

// MipFilter = LINEAR cases
//
CLinear_MinPointTest		    Linear_MinPoint;
CLinear_MinLinearTest		    Linear_MinLinear;
CLinear_NonSquareWTest		    Linear_NonSquareWidth;
CLinear_NonSquareHTest		    Linear_NonSquareHeight;
CLinear_FullSquareUTTest	    Linear_FullSquareUT;
CLinear_FullSquareCRTest	    Linear_FullSquareCR;
CLinear_FullSquareLCTest	    Linear_FullSquareLC;
CLinear_FullSquareUSTest	    Linear_FullSquareUS;
CLinear_FullNonSquareWUTTest	Linear_FullNonSquareWidthUT;
CLinear_FullNonSquareWCRTest	Linear_FullNonSquareWidthCR;
CLinear_FullNonSquareWLCTest	Linear_FullNonSquareWidthLC;
CLinear_FullNonSquareWUSTest	Linear_FullNonSquareWidthUS;
CLinear_FullNonSquareHUTTest	Linear_FullNonSquareHeightUT;
CLinear_FullNonSquareHCRTest	Linear_FullNonSquareHeightCR;
CLinear_FullNonSquareHLCTest	Linear_FullNonSquareHeightLC;
CLinear_FullNonSquareHUSTest	Linear_FullNonSquareHeightUS;
CLinear_MaxMipLevelTest		    Linear_MaxMipLevel;
CLinear_LODBiasTest			    Linear_LODBias;

// MipFilter = NONE cases
//
CNone_FullSquareTest		None_FullSquare;
CNone_MaxMipLevelTest		None_MaxMipLevel;
CNone_LODBiasTest			None_LODBias;
#endif//FAST_TEST

CMipFilterTest::CMipFilterTest()
{
    // Init framework options
    SetTestType(TESTTYPE_CONF);
	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE | PF_ALPHA;
	m_Options.ModeOptions.dwTexExclude = PF_METN;

#if FAST_TEST
    m_Options.ModeOptions.TextureFmt = FMT_A8R8G8B8;
    // Add some more space to fit all test cases in one back buffer
    m_Options.D3DOptions.nBackBufferWidth = (DWORD)512;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD)512;
    // Set compare function to require 98.5%
    m_pFramework->GetImageCmp()->SetPassRequired(0.985f);
#endif

	// Force test to use relevant DirectX versions for WHQL
	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_7;

    // Initialize some variables
	bMax = false;
	bFull = false;
	bBias = false;
	bMipNone = false;
	bMipLinear = false;
	bMinLinear = false;
	bNonSquareW = false;
	bNonSquareH = false;
    pTexture = NULL;
    dwLoadOption = TEXLOAD_DEFAULT;

	// Default size array values
	SizeArray[0] = 248.0f;
	SizeArray[1] =  32.0f;
	SizeArray[2] =  48.0f;
	SizeArray[3] =  56.0f;
	SizeArray[4] =  96.0f;
	SizeArray[5] = 104.0f;
	SizeArray[6] = 120.0f;
	SizeArray[7] = 192.0f;
	SizeArray[8] = 216.0f;
}

CMipFilterTest::~CMipFilterTest()
{
}

bool CMipFilterTest::SetDefaultRenderStates(void)
{
	// Turn off Texture Perspective for all tests
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);

	// Setup our default Texture Stage State
	SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);

	if (bMinLinear)
		SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
	else
		SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);

	if (bMipNone)
	{
		SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
		SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
	}
	else
	{
		if (bMipLinear)
			SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);
		else
			SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
	}
	
	return true;
}

UINT CMipFilterTest::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

#if FAST_TEST
    if (bFull)
        m_totalNumTestCases = (MAX_TESTS * 2);
    else if (bBias && !bMipLinear)
        m_totalNumTestCases = (MAX_TESTS - 2);
    else
        m_totalNumTestCases = MAX_TESTS;

    SetTestRange(1, 1);
#else//FAST_TEST
	// Setup the Test range
	if (bFull)
		SetTestRange(1, MAX_TESTS * 2);
	else if (bBias && !bMipLinear)
		SetTestRange(1, MAX_TESTS - 2);
	else
		SetTestRange(1, MAX_TESTS);
#endif//FAST_TEST

	// Check the caps we are interested in
	DWORD dwTextureFilterCaps = m_pSrcDevice->GetCaps()->dwTextureFilterCaps;
	DWORD dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;
	DWORD dwRasterCaps = m_pSrcDevice->GetCaps()->dwRasterCaps;
	DWORD dwMaxTextureAspectRatio = m_pSrcDevice->GetCaps()->dwMaxTextureAspectRatio;

    // Check the appropriate Mip Filter cap
	if (!bMipLinear && !(dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))
	{
		WriteToLog("Device capability not supported: (Mip) Texture Filter Point.\n");
		return D3DTESTINIT_SKIPALL;
	}
	else if (bMipLinear && !(dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
	{
		WriteToLog("Device capability not supported: (Mip) Texture Filter Linear.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Check the appropriate Min Filter cap
	if (!bMinLinear && !(dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))
	{
		WriteToLog("Device capability not supported: (Min) Texture Filter Point.\n");
		return D3DTESTINIT_ABORT;
	}
	else if (bMinLinear && !(dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))
	{
		WriteToLog("Device capability not supported: (Min) Texture Filter Linear.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Check the appropriate Mag Filter cap
	if (!(dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))
	{
		WriteToLog("Device capability not supported: (Mag) Texture Filter Point.\n");
		return D3DTESTINIT_ABORT;
	}

	// Check for ability to do non-square textures
	if ((bNonSquareW || bNonSquareH) && (dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY))
	{
		WriteToLog("Device does not supported non-square textures.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Check for LODBias cap
	if (bBias && !(dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS))
	{
		WriteToLog("Device capability not supported: Raster MipMapLODBias.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Check the dynamic textures cap for Dx8 LockCopy
	if (m_dwVersion >= 0x0800 && dwLoadOption == TEXLOAD_LOCKCOPY)
	{
		if(!(m_pSrcDevice->GetCaps()->dwCaps2 & DDCAPS2_DYNAMICTEXTURES))
		{
			WriteToLog("Dynamic textures not supported.\n");
			return D3DTESTINIT_SKIPALL;
		}
	}

	// On Dx9.L, need to check for the special A1 format
	if ((m_pD3D->GetVersion() >= 9.1f) && (m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_A1))
	{
		WriteToLog("Unable to mipmap this format. D3DFMT_A1 does not support mipmapping.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// On Dx9c, need to check for UsageQueryWrapAndMip
	if ((m_pD3D->GetRuntimeVersion() >= 9.029f) && (!bMipNone))
	{
		HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
											 USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat]);
		if (FAILED(Result))
		{
			WriteToLog("Unable to mipmap this format. CheckDeviceFormat(WrapAndMip) failed: %s.\n",m_pD3D->HResultToString(Result));
			return D3DTESTINIT_SKIPALL;
		}
	}

	// On Dx9, need to check for UsageQueryFilter
	if ((m_dwVersion >= 0x0900) && (bMipLinear || bMinLinear))
	{
		HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
											 USAGE_QUERY_FILTER, RTYPE_TEXTURE, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat]);
		if (FAILED(Result))
		{
			WriteToLog("Unable to filter this format. CheckDeviceFormat(Filter) failed: %s.\n",m_pD3D->HResultToString(Result));
			return D3DTESTINIT_SKIPALL;
		}
	}

    // Initialize the size array based on test class
	InitSize();

	// Initialize the MipImage pointer based on test class
	InitMipImage(dwMaxTextureAspectRatio);

	return D3DTESTINIT_RUN;
}

bool CMipFilterTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

#if FAST_TEST
bool CMipFilterTest::ExecuteTest(UINT uTestNum)
{
    msgString[0] = '\0';
    szStatus[0] = '\0';

    // Tell the log that we are starting
    BeginTestCase("Fast", uTestNum);

    auto& drawLamda = [=](UINT testIndex) -> bool
    {
        // Initialize some variables
        float fSize = SizeArray[((bFull) ? testIndex % 18 : testIndex % 9)];
        int nWidth = (int)fSize;
        int nHeight = (int)fSize;
        int nLevels = (bFull || bBias || bMax) ? 0 : 3;
        DWORD Usage = FMWK_DEFAULT;
        DWORD dwMipLevels;
        bool bResult;
        dwVertexCount = 0;

        // Cleanup previous texture
        SetTexture(0, NULL);
        RELEASE(pTexture);

        // Let's build some triangles.
        DrawGrid(fSize, fSize);

        // Get the top level Width & Height
        nWidth = pMipImage[0]->GetWidth();
        nHeight = pMipImage[0]->GetHeight();

        //
        //  Before Dx9 there was some confusion as to how to properly fill out
        //  small YUV textures.  With the runtime and reference rasterizer fixes with
        //  Manbugs 52173, 55114, and 55518 we can now test these.
        //
        if (m_pD3D->GetMaxVersion() < 9.0f)
        {
            // Skip all YUV texture formats for the cases with small mip levels
            if (bFull || bBias || bMax)
            {
                if (m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == MAKEFOURCC('U', 'Y', 'V', 'Y'))
                {
                    TestFinish(D3DTESTFINISH_SKIPREMAINDER);
                    return false;
                }

                if (m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == MAKEFOURCC('Y', 'U', 'Y', '2'))
                {
                    TestFinish(D3DTESTFINISH_SKIPREMAINDER);
                    return false;
                }
            }
        }

        // Make sure the Usage field is correct for the load method
        if (dwLoadOption == TEXLOAD_LOCKCOPY)
            Usage = USAGE_DYNAMIC;

        // Create the texture
        bResult = CreateTexture(nWidth, nHeight, nLevels, Usage, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], FMWK_DEFAULT, &pTexture);

        // Tell log about CreateTexture failure
        if (!bResult || (NULL == pTexture))
        {
            Fail();
            return false;
        }

        // Set the palette if needed
        if (pTexture->IsPalettized())
            pTexture->SetPalette(NULL);

        // Load image into the texture
        if (!pTexture->LoadImage(0, pMipImage, dwLoadOption))
        {
            Fail();
            return false;
        }

        // Get the real number of mip levels created
        dwMipLevels = pTexture->GetLevelCount(CD3D_SRC);

        // Indicate which load option was used
        switch (m_pSrcDevice->GetTexLoad())
        {
        case TEXLOAD_UPDATETEXTURE:
        case TEXLOAD_COPYRECTS:
        case TEXLOAD_LOCKCOPY:
        case TEXLOAD_UPDATESURFACE:
            break;
        default:
            Fail();
            return false;
        }

        // Set the valid texture
        if (!SetTexture(0, pTexture))
        {
            Fail();
            return false;
        }

        // Setup the TSS for MipMapLODBias tests
        if (bBias)
        {
            if (bMipLinear)
            {
                float fLodBias[] = { 1.0f, 0.67f, 0.33f, 0.25f, 0.0f, -0.25f, -0.33f, -0.67f, -1.0f };

                SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((DWORD*)(&fLodBias[testIndex - 1])));
            }
            else
            {
                float fLodBias[] = { 3.0f, 2.0f, 1.0f, 0.0f, -1.0f, -2.0f, -3.0f };

                SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((DWORD*)(&fLodBias[testIndex - 1])));
            }
        }

        // Setup the TSS for MaxMipLevel tests
        if (bMax)
        {
            DWORD	dwPasses = 0, dwMaxMipLevel = 0;
            HRESULT hResult;

            SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, (DWORD)(dwMipLevels - testIndex));

            // Since there is no cap for MaxMipLevel, we need to check with ValidateDevice
            if (!ValidateDevice(&dwPasses))
            {
                hResult = GetLastError();

                GetTextureStageState(0, D3DTSS_MAXMIPLEVEL, &dwMaxMipLevel, CD3D_REF);

                TestFinish(D3DTESTFINISH_SKIPREMAINDER);
                return false;
            }
        }

        return true;
    };

    bool pass = true;

    const DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
    const DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
    UINT numColumns = (UINT)ceil(sqrt((float)m_totalNumTestCases));
    UINT numRows = (UINT)ceil(sqrt((float)m_totalNumTestCases));;
    const float widthPerOp = float(dwWidth) / float(numColumns);
    const float heightPerOp = float(dwHeight) / float(numRows);

    for (size_t i = 0; i < m_totalNumTestCases; i++)
    {
        pass = drawLamda(i + 1);//Plus 1 because tests are 1 indexed
        if (pass == false)
        {
            break;
        }

        if (BeginScene())
        {
            // Transform and scale the vertices so they all fit on the screen
            {
                const DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
                const DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
                UINT numColumns = (UINT)ceil(sqrt((float)m_totalNumTestCases));
                UINT numRows = numColumns;
                const float widthPerOp = float(dwWidth) / float(numColumns);
                const float heightPerOp = float(dwHeight) / float(numRows);

                float scaleX = (widthPerOp / float(dwWidth)) + 0.25f;
                float scaleY = (widthPerOp / float(dwHeight)) + 0.25f;

                float transX = (i % numColumns) * widthPerOp;
                float transY = (i / numColumns) * heightPerOp;
                for (size_t i = 0; i < dwVertexCount; i++)
                {
                    VertexList[i].sx *= scaleX;
                    VertexList[i].sx += transX;

                    VertexList[i].sy *= scaleY;
                    VertexList[i].sy += transY;
                }
            }

            // Render the data.
            RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, VertexList, dwVertexCount, NULL, 0, 0);

            EndScene();
        }
    }

    return pass;
}
#else //FAST_TEST
bool CMipFilterTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	float fSize = SizeArray[((bFull) ? uTestNum % 18 : uTestNum % 9)];
	int nWidth = (int)fSize;
	int nHeight = (int)fSize;
	int nLevels = (bFull || bBias || bMax) ? 0 : 3;
	DWORD Usage = FMWK_DEFAULT;
    TCHAR szBuffer[80];
	DWORD dwMipLevels;
	bool bResult;
	dwVertexCount = 0;
	msgString[0] = '\0';
 	szStatus[0]  = '\0';

	// Cleanup previous texture
	SetTexture(0, NULL);
    RELEASE(pTexture);
 
	// Let's build some triangles.
	DrawGrid(fSize, fSize);

	sprintf(msgString, "%sMesh:%dx%d, ",msgString,nWidth,nHeight);

    // Tell log what format we are using
    FmtToString(FORMAT_TEXTURE,&m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat],szBuffer);
    sprintf(msgString, "%sTex: %s",msgString,szBuffer);

    // Get the top level Width & Height
	nWidth = pMipImage[0]->GetWidth();
	nHeight = pMipImage[0]->GetHeight();

	sprintf(msgString, "%s, %dx%d",msgString,nWidth,nHeight);

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

	//
	//  Before Dx9 there was some confusion as to how to properly fill out
	//  small YUV textures.  With the runtime and reference rasterizer fixes with
	//  Manbugs 52173, 55114, and 55518 we can now test these.
	//
	if (m_pD3D->GetMaxVersion() < 9.0f)
	{
		// Skip all YUV texture formats for the cases with small mip levels
		if (bFull || bBias || bMax)
		{
			if (m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == MAKEFOURCC('U','Y','V','Y'))
			{
				WriteToLog("Ignoring this case with FourCC (UYVY) texture.\n");
				TestFinish(D3DTESTFINISH_SKIPREMAINDER);
				return false;
			}
	         
			if (m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == MAKEFOURCC('Y','U','Y','2'))
			{
				WriteToLog("Ignoring this case with FourCC (YUY2) texture.\n");
				TestFinish(D3DTESTFINISH_SKIPREMAINDER);
				return false;
			}
		}
	}

	// Make sure the Usage field is correct for the load method
	if (dwLoadOption == TEXLOAD_LOCKCOPY)
		Usage = USAGE_DYNAMIC;

    // Create the texture
    bResult = CreateTexture(nWidth,nHeight,nLevels,Usage,m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat],FMWK_DEFAULT,&pTexture);

	// Tell log about CreateTexture failure
	if (!bResult || (NULL == pTexture))
	{
		WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

	// Set the palette if needed
	if (pTexture->IsPalettized())
		pTexture->SetPalette(NULL);

    // Load image into the texture
    if (!pTexture->LoadImage(0,pMipImage,dwLoadOption))
	{
		WriteToLog("LoadImage() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

    // Get the real number of mip levels created
    dwMipLevels = pTexture->GetLevelCount(CD3D_SRC);

	// Indicate which load option was used
    switch (m_pSrcDevice->GetTexLoad())
    {    
        case TEXLOAD_UPDATETEXTURE:
        	WriteToLog("TexLoad: UpdateTex, Lvls: %d\n",dwMipLevels);
        	sprintf(szStatus, "%s$aTexLoad: UpdateTex, Lvls: %d",szStatus,dwMipLevels);
            break;
        case TEXLOAD_COPYRECTS:
        	WriteToLog("TexLoad: CopyRects, Lvls: %d\n",dwMipLevels);
        	sprintf(szStatus, "%s$aTexLoad: CopyRects, Lvls: %d",szStatus,dwMipLevels);
            break;
        case TEXLOAD_LOCKCOPY:
        	WriteToLog("TexLoad: LockCopy, Lvls: %d\n",dwMipLevels);
        	sprintf(szStatus, "%s$aTexLoad: LockCopy, Lvls: %d",szStatus,dwMipLevels);
            break;
        case TEXLOAD_UPDATESURFACE:
        	WriteToLog("TexLoad: UpdateSurf, Lvls: %d\n",dwMipLevels);
        	sprintf(szStatus, "%s$aTexLoad: UpdateSurf, Lvls: %d",szStatus,dwMipLevels);
            break;
        default:
		    WriteToLog("Error: Unknown texture load option (%x).\n",m_pSrcDevice->GetTexLoad());
        	sprintf(szStatus, "%s$rTexLoad: Unknown (%x)",szStatus,m_pSrcDevice->GetTexLoad());
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

	// Setup the TSS for MipMapLODBias tests
	if (bBias)
	{
		if (bMipLinear)
		{
			float fLodBias[] = {1.0f, 0.67f, 0.33f, 0.25f, 0.0f, -0.25f, -0.33f, -0.67f, -1.0f};

			SetTextureStageState(0,D3DTSS_MIPMAPLODBIAS,*((DWORD*)(&fLodBias[uTestNum-1])));
			WriteToLog("Bias: %.2f\n",fLodBias[uTestNum-1]);
			sprintf(szStatus, "%s, Bias: %.2f",szStatus,fLodBias[uTestNum-1]);
		}
		else
		{
			float fLodBias[] = {3.0f, 2.0f, 1.0f, 0.0f, -1.0f, -2.0f, -3.0f};

			SetTextureStageState(0,D3DTSS_MIPMAPLODBIAS,*((DWORD*)(&fLodBias[uTestNum-1])));
			WriteToLog("Bias: %.2f\n",fLodBias[uTestNum-1]);
			sprintf(szStatus, "%s, Bias: %.2f",szStatus,fLodBias[uTestNum-1]);
		}		
	}

    // Setup the TSS for MaxMipLevel tests
	if (bMax)
	{
		DWORD	dwPasses = 0, dwMaxMipLevel = 0;
        HRESULT hResult;

        SetTextureStageState(0,D3DTSS_MAXMIPLEVEL,(DWORD)(dwMipLevels - uTestNum));

        // Since there is no cap for MaxMipLevel, we need to check with ValidateDevice
		if (!ValidateDevice(&dwPasses))
		{
            hResult = GetLastError();
            
    	    GetTextureStageState(0,D3DTSS_MAXMIPLEVEL,&dwMaxMipLevel,CD3D_REF);

        	WriteToLog("ValidateDevice(MaxMipLevel=%d) failed with Passes = %d and HResult = %s.\n",dwMaxMipLevel,dwPasses,m_pD3D->HResultToString(hResult));
			TestFinish(D3DTESTFINISH_SKIPREMAINDER);
			return false;
		}

		WriteToLog("Max: %d\n",dwMipLevels - uTestNum);
		sprintf(szStatus, "%s, Max: %d",szStatus,dwMipLevels - uTestNum);
    }

    return true;
}
#endif //FAST_TEST

void CMipFilterTest::SceneRefresh(void)
{
#ifndef FAST_TEST
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
#endif
}

void CMipFilterTest::UpdateStatus(void)
{
    CD3DTest::UpdateStatus();

	// Update the status window with our test options
	WriteStatus("$wTest Options",szStatus);
}

bool CMipFilterTest::TestTerminate(void)
{
	// Cleanup texture
	SetTexture(0, NULL);
    RELEASE(pTexture);

    // Cleanup image data for mips
	for (int i=0; i < 9; i++)
	    SAFEDELETE(pMipImage[i]);

	return true;
}

void CMipFilterTest::InitSize(void)
{
	if (bFull)
	{
		// Init the Size array for all Full tests
		SizeArray[0] =  248.0f;
		SizeArray[1] =    1.0f;
		SizeArray[2] =    2.0f;
		SizeArray[3] =    3.0f;
		SizeArray[4] =    4.0f;
		SizeArray[5] =    6.0f; 
		SizeArray[6] =    7.0f;
		SizeArray[7] =   12.0f;
		SizeArray[8] =   14.0f;
		SizeArray[9] =   24.0f;
		SizeArray[10] =  30.0f;
		SizeArray[11] =  48.0f;
		SizeArray[12] =  56.0f;
		SizeArray[13] =  96.0f;
		SizeArray[14] = 104.0f;
		SizeArray[15] = 120.0f;
		SizeArray[16] = 192.0f;
		SizeArray[17] = 216.0f;
	}
	else if (bMax)
	{
		// Init the Size array for MaxMipLevel tests
		for (int i=0; i < 9; i++)
			SizeArray[i] = 248.0f;
	}
	else if (bBias)
	{
		// Init the Size array for LODBias tests
		for (int i=0; i < 9; i++)
			SizeArray[i] = 30.0f; // 32.0f;
	}

	// else use default values
}

void CMipFilterTest::InitMipImage(DWORD dwAspectRatio)
{
	int	nMinSize, nMipWidth, nMipHeight;

	if (bFull || bBias || bMax)
	{
		// Setup the color array
        D3DCOLOR Color[9][1];

		Color[0][0] = RGBA_MAKE(255,255,255,255);  // White
		Color[1][0] = RGBA_MAKE(255,255,0,255);    // Yellow
		Color[2][0] = RGBA_MAKE(255,0,255,255);    // Magenta
		Color[3][0] = RGBA_MAKE(255,0,0,255);      // Red
		Color[4][0] = RGBA_MAKE(0,255,255,255);    // Cyan
		Color[5][0] = RGBA_MAKE(0,255,0,255);      // Green
		Color[6][0] = RGBA_MAKE(0,0,255,255);      // Blue
		Color[7][0] = RGBA_MAKE(128,128,128,255);  // Grey
		Color[8][0] = RGBA_MAKE(128,128,0,255);    // Brown

		// Determine Width and Height
		if (dwAspectRatio && ((256 / dwAspectRatio) > 16))
			nMinSize = 256 / dwAspectRatio;
		else
			nMinSize = 16;

		nMipWidth = (bNonSquareW) ? nMinSize : 256;
		nMipHeight = (bNonSquareH) ? nMinSize : 256;

		// Load images for the full level cases
        for (int i=0; i < 9; i++)
        {
            pMipImage[i] = new CImage;
            pMipImage[i]->LoadStripes(nMipWidth/(int)((DWORD)1 << i),nMipHeight/(int)((DWORD)1 << i),1,Color[i],false);
        }
	}
	else
	{
		// Setup the colors for the RedStripes array
		D3DCOLOR		RedStripes[8];

		RedStripes[0] = RGBA_MAKE(255,255,255,255);	// White
		RedStripes[1] = RGBA_MAKE(255,0,0,255);		// Red
		RedStripes[2] = RGBA_MAKE(255,255,255,255);	// White
		RedStripes[3] = RGBA_MAKE(255,0,0,255);		// Red
		RedStripes[4] = RGBA_MAKE(255,255,255,255);	// White
		RedStripes[5] = RGBA_MAKE(255,0,0,255);		// Red
		RedStripes[6] = RGBA_MAKE(255,255,255,255);	// White
		RedStripes[7] = RGBA_MAKE(255,0,0,255);		// Red

		// Setup the colors for the GreenStripes array
		D3DCOLOR		GreenStripes[8];

		GreenStripes[0] = RGBA_MAKE(255,255,255,255);	// White
		GreenStripes[1] = RGBA_MAKE(0,255,0,255);		// Green
		GreenStripes[2] = RGBA_MAKE(255,255,255,255);	// White
		GreenStripes[3] = RGBA_MAKE(0,255,0,255);		// Green
		GreenStripes[4] = RGBA_MAKE(255,255,255,255);	// White
		GreenStripes[5] = RGBA_MAKE(0,255,0,255);		// Green
		GreenStripes[6] = RGBA_MAKE(255,255,255,255);	// White
		GreenStripes[7] = RGBA_MAKE(0,255,0,255);		// Green

		// Setup the colors for the BlueStripes array
		D3DCOLOR		BlueStripes[8];

		BlueStripes[0] = RGBA_MAKE(255,255,255,255);// White
		BlueStripes[1] = RGBA_MAKE(0,0,255,255);	// Blue
		BlueStripes[2] = RGBA_MAKE(255,255,255,255);// White
		BlueStripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
		BlueStripes[4] = RGBA_MAKE(255,255,255,255);// White
		BlueStripes[5] = RGBA_MAKE(0,0,255,255);	// Blue
		BlueStripes[6] = RGBA_MAKE(255,255,255,255);// White
		BlueStripes[7] = RGBA_MAKE(0,0,255,255);	// Blue

		// Determine Width and Height
		if (dwAspectRatio && ((256 / dwAspectRatio) > 64))
			nMinSize = 256 / dwAspectRatio;
		else
			nMinSize = 64;

		nMipWidth = (bNonSquareW) ? nMinSize : 256;
		nMipHeight = (bNonSquareH) ? nMinSize : 256;

		// Load images for the three level cases
        pMipImage[0] = new CImage;
        pMipImage[0]->LoadStripes(nMipWidth,nMipHeight,8,RedStripes,false);
        pMipImage[1] = new CImage;
        pMipImage[1]->LoadStripes(nMipWidth/2,nMipHeight/2,8,GreenStripes,false);
        pMipImage[2] = new CImage;
        pMipImage[2]->LoadStripes(nMipWidth/4,nMipHeight/4,8,BlueStripes,false);
	}
}
