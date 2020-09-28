#define D3D_OVERLOADS
#include <d3d.h>
#include "TexFormats.h"
#include "FloatPrecision.h"

// Global Test and App instance
CD3DWindowFramework	App;
CUpdateTexTest				UpdateTex;
CCopyRectsTest      		CopyRects;
CLockCopyTest				LockCopy;
CUpdateSurfTest				UpdateSurf;
CVerifyTest					Verify;
CSysMemTest         		SysMem;
CFVF3Test           		FVF3;
CFVF4Test           		FVF4;
CPrintTest					Print;
CFloatTexRTPrecisionTest20	FloatRT20;
CFloatTexRTPrecisionTest30	FloatRT30;
CFloatTexPrecisionTest20	FloatP20;
CFloatTexPrecisionTest30	FloatP30;
CFloatTexBadValuesTest20	FloatBV20;
CFloatTexBadValuesTest30	FloatBV30;
CFloatMultipleTexTest		FloatMT;
CPackedRGBTest				PackedRGB;

CTexFormatsTest::CTexFormatsTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE | PF_ALPHA;
	m_Options.ModeOptions.dwTexExclude = PF_METN;

	// Force test to cycle desktop resolutions 16bpp/32bpp
	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ALL;

	// Force test to use relevant DirectX versions
	m_Options.D3DOptions.fMinDXVersion = 6.0;

    // Init variables
    dwLoadOption = TEXLOAD_DEFAULT;
    pTexture = NULL;
    bSysMem = false;
    bFVF    = false;
}

CTexFormatsTest::~CTexFormatsTest()
{
}
 
UINT CTexFormatsTest::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

#if FAST_TEST
        // Setup the Test range
    if (bSysMem || bFVF)
        m_totalNumTestCases = m_uCommonTextureFormats;
    else
        m_totalNumTestCases = MAX_TESTS * m_uCommonTextureFormats;

    SetTestRange(1, 1);
#else
    // Setup the Test range
    if (bSysMem || bFVF)
    	SetTestRange(1, m_uCommonTextureFormats);
    else
    	SetTestRange(1, MAX_TESTS * m_uCommonTextureFormats);
#endif

    // Don't run the system memory tests on a hardware device
    if (bSysMem && 
        (m_pSrcDevice->GetDevType() == DEVICETYPE_HAL ||
         m_pRefDevice->GetDevType() == DEVICETYPE_HAL))
    {
	    WriteToLog("System memory tests not valid for hardware rasterizers.\n");
	    return D3DTESTINIT_SKIPALL;
    }

	// Check the dynamic textures cap
	if (m_dwVersion >= 0x0800 && dwLoadOption == TEXLOAD_LOCKCOPY)
	{
		if (!(m_pSrcDevice->GetCaps()->dwCaps2 & DDCAPS2_DYNAMICTEXTURES))
		{
			WriteToLog("Dynamic textures not supported.\n");
			return D3DTESTINIT_SKIPALL;
		}
	}

	// Setup the colors for the Stripes array
	Stripes[0] = RGBA_MAKE(255,255,255,255);// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);	// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);	// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);// Grey

	return D3DTESTINIT_RUN;
}

bool CTexFormatsTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

#if FAST_TEST
bool CTexFormatsTest::ExecuteTest(UINT uTestNum)
{
    // Tell the log that we are starting
    BeginTestCase("Tex Formats", uTestNum);

    auto& drawLamda = [=](UINT testIndex) -> bool
    {
        // Initialize some variables
        int nTest = ((testIndex - 1) / m_uCommonTextureFormats) + 1;
        DWORD Pool = (bSysMem) ? POOL_SYSTEMMEM : FMWK_DEFAULT;
        DWORD Usage = FMWK_DEFAULT;
        TCHAR szBuffer[80];
        FORMAT *pFormat;
        bool bResult;
        dwVertexCount = 0;

        // Cleanup previous texture
        SetTexture(0, NULL);
        RELEASE(pTexture);

        // Let's build some triangles.
        if (bFVF)
            DrawFVFGrid();
        else
            DrawGrid(nTest);

        // Cycle the common texture formats
        m_pCurrentMode->nTextureFormat = (testIndex - 1) % m_uCommonTextureFormats;

        pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

        // Make sure the Usage field is correct for the load method
        if (dwLoadOption == TEXLOAD_LOCKCOPY)
            Usage = USAGE_DYNAMIC;

        // Create the texture
        bResult = CreateTexture(64, 64, 1, Usage, *pFormat, Pool, &pTexture);

        // Tell log about CreateTexture failure
        if (!bResult || (NULL == pTexture))
        {
            Fail();
            return false;
        }

        // Set the palette if needed
        if (pTexture->IsPalettized())
        {
            if (!pTexture->SetPalette(NULL))
            {
                Fail();
                return false;
            }
        }

        // Load a stripe image into the texture
        if (!pTexture->LoadStripes(1, 64, 64, 8, Stripes, false, false, dwLoadOption))
        {
            Fail();
            return false;
        }

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

                float scaleX = (widthPerOp / float(dwWidth));
                float scaleY = (widthPerOp / float(dwHeight));

                float transX = (i % numColumns) * widthPerOp;
                float transY = (i / numColumns) * heightPerOp;

                TransformAndDraw(scaleX, scaleY, transX, transY);
            }

            EndScene();
        }
    }

    return pass;
}

void CTexFormatsTest::TransformAndDraw(float scaleX, float scaleY, float transX, float transY)
{
    for (size_t i = 0; i < dwVertexCount; i++)
    {
        VertexList[i].sx *= scaleX;
        VertexList[i].sx += transX;

        VertexList[i].sy *= scaleY;
        VertexList[i].sy += transY;
    }

    // Render the data.
    RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, VertexList, dwVertexCount, NULL, 0, NULL);
}

#else //FAST_TEST
bool CTexFormatsTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	int nTest = ((uTestNum-1) / m_uCommonTextureFormats) + 1;
    DWORD Pool = (bSysMem) ? POOL_SYSTEMMEM : FMWK_DEFAULT;
	DWORD Usage = FMWK_DEFAULT;
    TCHAR szBuffer[80];
    FORMAT *pFormat;
	bool bResult;
 	dwVertexCount = 0;
	msgString[0] = '\0';
  	szStatus[0]  = '\0';

	// Cleanup previous texture
	SetTexture(0, NULL);
	RELEASE(pTexture);

	// Let's build some triangles.
    if (bFVF)
        DrawFVFGrid();
    else
	    DrawGrid(nTest);

	// Cycle the common texture formats
	m_pCurrentMode->nTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;

    pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

    // Tell log what format we are using
    FmtToString(FORMAT_TEXTURE,pFormat,szBuffer);
    sprintf(msgString, "%sTex: %s",msgString,szBuffer);

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

	// Make sure the Usage field is correct for the load method
	if (dwLoadOption == TEXLOAD_LOCKCOPY)
		Usage = USAGE_DYNAMIC;

    // Create the texture
    bResult = CreateTexture(64,64,1,Usage,*pFormat,Pool,&pTexture);

	// Tell log about CreateTexture failure
	if (!bResult || (NULL == pTexture))
	{
		WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

	// Set the palette if needed
	if (pTexture->IsPalettized())
	{
		if (!pTexture->SetPalette(NULL))
		{
			WriteToLog("SetPalette() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
	}

    // Load a stripe image into the texture
    if (!pTexture->LoadStripes(1,64,64,8,Stripes,false,false,dwLoadOption))
	{
		WriteToLog("LoadStripes() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

	// Indicate which load option was used
    switch (m_pSrcDevice->GetTexLoad())
    {    
        case TEXLOAD_UPDATETEXTURE:
        	WriteToLog("TexLoad: UpdateTex\n");
        	sprintf(szStatus, "%s$aUpdateTex",szStatus);
            break;
        case TEXLOAD_COPYRECTS:
        	WriteToLog("TexLoad: CopyRects\n");
        	sprintf(szStatus, "%s$aCopyRects",szStatus);
            break;
        case TEXLOAD_LOCKCOPY:
        	WriteToLog("TexLoad: LockCopy\n");
        	sprintf(szStatus, "%s$aLockCopy",szStatus);
            break;
         case TEXLOAD_UPDATESURFACE:
        	WriteToLog("TexLoad: UpdateSurf\n");
        	sprintf(szStatus, "%s$aUpdateSurf",szStatus);
            break;
       default:
		    WriteToLog("Error: Unknown texture load option (%x).\n",m_pSrcDevice->GetTexLoad());
        	sprintf(szStatus, "%s$rUnknown (%x)",szStatus,m_pSrcDevice->GetTexLoad());
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

    return true;
}
#endif //FAST_TEST

void CTexFormatsTest::SceneRefresh(void)
{
#ifndef FAST_TEST
	if (BeginScene())
	{
		// Render the data.
	    RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,NULL);
 
		EndScene();
	}
#endif
}

void CTexFormatsTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what load method we are using
	WriteStatus("$wTexture Load",szStatus);
}

bool CTexFormatsTest::TestTerminate(void)
{
	// Cleanup texture
	SetTexture(0, NULL);
    RELEASE(pTexture);

	return true;
}


#if FAST_TEST
bool CTexFormatsTest::AddFormatModes(DISPLAYMODE *pMode, bool bWindowed)
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
            if (
                m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_A8R8G8B8 ||
                m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_X4R4G4B4 ||
                m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_A2B10G10R10 ||
                m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_DXT1 ||
                m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_R16F ||
                m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_A16B16G16R16F ||
                m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_R32F ||
                m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_A32B32G32R32F)
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