//
// SRT.CPP - Implementation of the SetRenderTarget test
//

#define D3D_OVERLOADS
#include <d3d.h>
#include "Srt.h"

// Global Test and App instance
CD3DWindowFramework	App;
CSRTTexture    	    Texture;
CSRTNoZTexture	    NoZTexture;
CSRTOffscreen       Offscreen;
CSRTNoZOffscreen    NoZOffscreen;
CSRTNull			Null;
CSRTNoZNull			NoZNull;
CSRTInvalid         Invalid;

#define SRT_BLACK   D3DRGBA(0.0, 0.0, 0.0, 1.0)
#define SRT_GREEN   D3DRGBA(0.0, 1.0, 0.0, 1.0)
#define SRT_RED     D3DRGBA(1.0, 0.0, 0.0, 1.0)
#define SRT_BLUE    D3DRGBA(0.0, 0.0, 1.0, 1.0)
#define SRT_WHITE   D3DRGBA(1.0, 1.0, 1.0, 0.0)
#define SRT_WHITENA D3DRGBA(1.0, 1.0, 1.0, 1.0)

SRT_TLVERTEX g_SmallTris[] =
    { { 0.5f,  256.5f, 0.1f, 10.0f, SRT_RED,   0.0f, 1.0f },
      { 43.1f, 0.5f,   0.1f, 10.0f, SRT_WHITE, 0.5f, 0.0f },
      { 85.7f, 256.5f, 0.1f, 10.0f, SRT_RED,   1.0f, 1.0f }, 

      { 85.7f,  256.5f, 0.1f, 10.0f, SRT_GREEN, 0.0f, 1.0f },
      { 128.3f, 0.5f,   0.1f, 10.0f, SRT_WHITE, 0.5f, 0.0f },
      { 170.9f, 256.5f, 0.1f, 10.0f, SRT_GREEN, 1.0f, 1.0f }, 

      { 170.9f, 256.5f, 0.1f, 10.0f, SRT_BLUE,  0.0f, 1.0f },
      { 213.5f, 0.5f,   0.1f, 10.0f, SRT_WHITE, 0.5f, 0.0f },
      { 256.1f, 256.5f, 0.1f, 10.0f, SRT_BLUE,  1.0f, 1.0f } };

SRT_TLVERTEX g_BigQuad[] =
    { { 0.0f,   279.5f, 0.1f, 10.0f, SRT_WHITENA, 0.0f, 1.0f },
      { 0.0f,   0.0f,   0.1f, 10.0f, SRT_WHITENA, 0.0f, 0.0f },
      { 319.5f, 279.5f, 0.1f, 10.0f, SRT_WHITENA, 1.0f, 1.0f },
      { 319.5f, 0.0f,   0.1f, 10.0f, SRT_WHITENA, 1.0f, 0.0f } };

CSRT::CSRT()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);
	m_Options.ModeOptions.dwTexInclude = PF_RGB;
	m_Options.ModeOptions.dwTexExclude = PF_PALETTEINDEXED4 | PF_PALETTEINDEXED8; 

	// Force test to cycle desktop resolutions 16bpp/32bpp
	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ALL;

	// Force test to use relevant DirectX versions for WHQL
	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_8_OR_81 | DXVERSION_7;

    // Init variables
    nWidthRT = SRT_SIZE;
    nHeightRT = SRT_SIZE;
    bInvalid = false;
    bOffscreen = false;
	bNull = false;
    bValidate = true;

    // Init pointer variables
    pTextureRT = NULL;
    pOffScreenRT = NULL;
    pTempRT = NULL;
    pTempZ = NULL;
    pOriginalRT = NULL;
    pOriginalZ = NULL;

    if (KeySet("NoValidate"))
        bValidate = false;
}

CSRT::~CSRT()
{
}

void CSRT::CommandLineHelp(void)
{
    WriteCommandLineHelp("$y-Width:$wn\t\t$wSets the texture width to n (default 256)");
    WriteCommandLineHelp("$y-Height:$wn\t\t$wSets the texture height to n (default 256)");
}
   
UINT CSRT::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

#ifdef FAST_TEST
	m_uCommonZBufferFormats = 1;
	m_uCommonTextureFormats = 1;
#endif

    if (m_Options.D3DOptions.bZBuffer)
	{
		if (bNull)
	        SetTestRange(1, m_uCommonZBufferFormats);
		else
			SetTestRange(1, m_uCommonTextureFormats * m_uCommonZBufferFormats);
	}
    else
	{
		if (bNull)
	        SetTestRange(1, 1);
		else
		    SetTestRange(1, m_uCommonTextureFormats);
	}

    // Get the current render target
    if (!GetRenderTarget(&pOriginalRT))
    {
		WriteToLog("GetRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		RELEASE(pOriginalRT);
    	return D3DTESTINIT_ABORT;
    }

    if (m_Options.D3DOptions.bZBuffer)
    {
        // Get the current z buffer
        if (!GetDepthStencilSurface(&pOriginalZ))
        {
		    WriteToLog("GetDepthStencilSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		    RELEASE(pOriginalZ);
		    RELEASE(pOriginalRT);
    	    return D3DTESTINIT_ABORT;
        }

        // Remember the original format used
        nOriginalZFormat = m_pCurrentMode->nZBufferFormat;
    }

	if (bNull)
	{
		// Need the display formats texture index
		GetFormatIndex((FMT)m_pCurrentMode->pDisplay->Format.d3dfFormat, m_pCommonTextureFormats, (UINT*)&(nTexFormat));
	}
	else
	{
		ReadInteger("Width", SRT_SIZE, &nWidthRT);
		ReadInteger("Height", SRT_SIZE, &nHeightRT);
	}

    // Initialize the render target viewport
	ViewportRT.X = 0;    
	ViewportRT.Y = 0;    
	ViewportRT.Width = nWidthRT;
	ViewportRT.Height = nHeightRT;
	ViewportRT.MinZ = 0.f; 
	ViewportRT.MaxZ = 1.f; 

    // Initialize data
    g_SmallTris[0].y = nHeightRT - 1.0f;
    g_SmallTris[2].y = nHeightRT - 1.0f;
    g_SmallTris[3].y = nHeightRT - 1.0f;
    g_SmallTris[5].y = nHeightRT - 1.0f;
    g_SmallTris[6].y = nHeightRT - 1.0f;
    g_SmallTris[8].y = nHeightRT - 1.0f;

    g_SmallTris[1].x = nWidthRT / 6.0f;
    g_SmallTris[2].x = nWidthRT / 3.0f;
    g_SmallTris[3].x = nWidthRT / 3.0f;
    g_SmallTris[4].x = nWidthRT / 2.0f;
    g_SmallTris[5].x = nWidthRT / 1.5f;
    g_SmallTris[6].x = nWidthRT / 1.5f;
    g_SmallTris[7].x = nWidthRT / 1.2f;
    g_SmallTris[8].x = nWidthRT / 1.0f;

    return D3DTESTINIT_RUN;
}

bool CSRT::ClearFrame(void)
{
	// Clear to dark blue
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

bool CSRT::ExecuteTest(UINT uTestNum)
{
    TCHAR buf[256];
 	DWORD	dwPasses;
    FORMAT *pFormatRT, *pFormatZ;
    bool bZBuffer, bResult, bDoExtraAddRef = false;
	DWORD Usage = (bOffscreen || bNull) ? FMWK_DEFAULT : USAGE_RENDERTARGET;

    // Cleanup previous texture and zbuffers
    SetTexture(0, NULL);
    RELEASE(pTempZ);
    RELEASE(pTempRT);
    RELEASE(pTextureRT);
    RELEASE(pOffScreenRT);

    if (m_Options.D3DOptions.bZBuffer)
    {
		if (!bNull)
			nTexFormat = (uTestNum-1) / m_uCommonZBufferFormats;

        nZFormat = (uTestNum-1) % m_uCommonZBufferFormats;
        bZBuffer = true;
    }
    else
    {
		if (!bNull)
			nTexFormat = uTestNum-1;

        nZFormat = 0;
        bZBuffer = false;
    }

    // Start the log
    DescribeTest(buf, nTexFormat, nZFormat);
    BeginTestCase(buf,uTestNum);

    // Tell framework what formats to use
    m_pCurrentMode->nTextureFormat = nTexFormat;
    pFormatRT = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

	// On Dx7 interface, 8 bit color channel rendering was not supported for the Reference rasterizer
	if ((m_dwVersion == 0x0700) && ((pFormatRT->d3dfFormat == FMT_R3G3B2) || (pFormatRT->d3dfFormat == FMT_A8R3G3B2)))
	{
		if (pFormatRT->d3dfFormat == FMT_R3G3B2)
			WriteToLog("On Dx7 interfaces, R3G3B2 rendering is not support by the Reference rasterizer.\n");
		else
			WriteToLog("On Dx7 interfaces, A8R3G3B2 rendering is not support by the Reference rasterizer.\n");

		SkipTests((bZBuffer) ? m_uCommonZBufferFormats : 1);
        return false;
	}

	if (bZBuffer)
    {
        m_pCurrentMode->nZBufferFormat = nZFormat;
        pFormatZ = &m_pCommonZBufferFormats[m_pCurrentMode->nZBufferFormat];

        // Don't create a new ZBuffer for the original
        if (nZFormat == nOriginalZFormat)
        {
            bZBuffer = false;
            pTempZ = pOriginalZ;
			pTempZ->AddRef();

			if (m_dwVersion == 0x0700)
				bDoExtraAddRef = true;
        }
    }

	// Create any necessary temp buffers
    if (!CreateBuffers(bOffscreen, bZBuffer))
    {
        WriteToLog("CreateBuffers() failed.\n");
    	SkipTests(1);
        return false;
    }

    // Create the texture
    bResult = CreateTexture(nWidthRT,nHeightRT,1,Usage,*pFormatRT,FMWK_DEFAULT,&pTextureRT);
 
    if (!bResult || (NULL == pTextureRT))
    {
		WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
    	SkipTests(1);
        return false;
    }

    // Setup temp pointers based on test
    if (bOffscreen)
    {
        pTempRT = pOffScreenRT;
        pTempRT->AddRef();
    }
    else if (!bNull)
    {
        pTextureRT->GetSurfaceLevel(0,&pTempRT);
    }

    // Set the texture as the render target
    if (!SetRenderTarget(pTempRT, pTempZ))
    {
		// Reset device to original targets
		SetRenderTarget(pOriginalRT, pOriginalZ);

		if (bDoExtraAddRef && (GetLastError() == DDERR_CURRENTLYNOTAVAIL))
		{
			CSurface *pRealZ;
			pTempZ->GetSurface(&pRealZ,0);
			pRealZ->GetD3DSurface7()->AddRef();
			RELEASE(pRealZ);
		}

		WriteToLog("SetRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        SkipTests(1);
        return false;
    }

    // Setup the viewport for our new render target
    if (!bOffscreen)
        SetViewport(&ViewportRT);

    if (bInvalid)
    {
        SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE);
        SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
        SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
        SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
        SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE);

		if (!SetTexture(0,pTextureRT))
		{
			WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
    } 
    else 
    {
        SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_DISABLE);
        SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_DISABLE);

		if (!SetTexture(0,NULL))
		{
			WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
    }

    SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE);
    SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
    SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	if (bValidate && !ValidateDevice(&dwPasses))
    {
        // Validate failed, reset device and skip
        SetRenderTarget(pOriginalRT, pOriginalZ);

        // Reset to the original viewport
        if (!bOffscreen)
            SetDefaultViewport();

        // Under Dx8 non-offscreen path, consider a failure
        if (!bOffscreen && (m_dwVersion == 0x0800))
        {
    		WriteToLog("ValidateDevice() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
            Fail();
            return false;
        }

  		WriteToLog("ValidateDevice() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        SkipTests(1);
        return false;
    }

	// Clear to dark blue
	Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);

    // Draw to the new render target
    if (BeginScene())
    {
	    RenderPrimitive(D3DPT_TRIANGLELIST,SRT_FVFTLVERTEX,g_SmallTris,9,NULL,0,NULL);

        EndScene();
    }
    
    // If using offscreen surface, copy rect to texture
    if (bOffscreen || bNull)
    {
        LPSURFACES pSurface;
        RECT Rect = {0,0,nWidthRT,nHeightRT};

        pTextureRT->GetSurfaceLevel(0, &pSurface);

		if (bNull)
		{
			pTempRT = pOriginalRT;
			pTempRT->AddRef();
		}

        if (!CopyRects(pTempRT,&Rect,1,pSurface,NULL))
	    {
			// Reset device to original targets
			SetRenderTarget(pOriginalRT, pOriginalZ);

			// Reset to the original viewport
			if (!bOffscreen)
				SetDefaultViewport();

            WriteToLog("CopyRects() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
            RELEASE(pSurface);
            Fail();
		    return false;
	    }

        RELEASE(pSurface);
    }

    // Reset to the original render target
    if (!SetRenderTarget(pOriginalRT, pOriginalZ))
    {
		WriteToLog("SetRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
        return false;
    }

    // Reset to the original viewport
    if (!bOffscreen)
        SetDefaultViewport();

    SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE);

    SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
    SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
    SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

	if (!SetTexture(0,pTextureRT))
	{
		WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		Fail();
		return false;
	}

    // This shouldn't ever happen but it doesn't hurt to check
	if (bValidate && !ValidateDevice(&dwPasses))
    {
        // Under Dx8 non-offscreen path, consider a failure
        if (!bOffscreen && (m_dwVersion == 0x0800))
        {
    		WriteToLog("ValidateDevice() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
            Fail();
            return false;
        }

  		WriteToLog("ValidateDevice() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        SkipTests(1);
        return false;
    }

    // Draw to the real target
    if (BeginScene())
    {
	    RenderPrimitive(D3DPT_TRIANGLESTRIP,SRT_FVFTLVERTEX,g_BigQuad,4,NULL,0,NULL);

        EndScene();
    }

    return true;
}

bool CSRT::ProcessFrame()
{
    bool    bResult = true;

    // Use the standard 15% for non Invalid cases
    if (bInvalid)
        m_fPassPercentage = 1.0;
    else
    {
         // Did we pass the color compare
        if (!CD3DTest::ProcessFrame())
        {
	        WriteToLog("ProcessFrame() failed.\n");
            return false;
        }

        bResult = m_pFramework->m_DXGLog.GetCurrentResult() == PASS;
    }

    // Tell the logfile how the compare went
    if (bResult)
        m_vSupportedFormats.push_back( std::pair<DWORD, DWORD>(nTexFormat, nZFormat) );

    return bResult;
}

bool CSRT::TestTerminate()
{
    //  Write out list of supported formats
    std::vector< std::pair<DWORD, DWORD> >::iterator iPairs = m_vSupportedFormats.begin();

	if (iPairs != m_vSupportedFormats.end() )
	{
		WriteToLog("\nSummary of supported formats\n");
		WriteToLog("----------------------------\n");
	}

    while(iPairs != m_vSupportedFormats.end())
    {
        TCHAR   buf[256];
        DescribeTest(buf, iPairs->first, iPairs->second);
        WriteToLog("%s\n", buf);
        iPairs++;
    }
    m_vSupportedFormats.clear();

    // Clean up memory
    SetTexture(0, NULL);

    RELEASE(pTempZ);

    if (m_Options.D3DOptions.bZBuffer)
        RELEASE(pOriginalZ);

    RELEASE(pTempRT);
    RELEASE(pTextureRT);
    RELEASE(pOffScreenRT);
    RELEASE(pOriginalRT);

    return true;
}

