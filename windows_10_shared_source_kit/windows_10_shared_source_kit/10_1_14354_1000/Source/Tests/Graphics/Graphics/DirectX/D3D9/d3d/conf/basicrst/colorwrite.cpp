#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "BasicRst.h"

//************************************************************************
// ColorWrite Test functions

CColorWriteTest::CColorWriteTest()
{
	m_szTestName = "BasicRst ColorWrite";
	m_szCommandKey = "ColorWrite";

    // Init Framework options
    m_Options.D3DOptions.fMinDXVersion = 8.0f;
}

CColorWriteTest::~CColorWriteTest()
{
}

UINT CColorWriteTest::TestInitialize(void)
{
	// Setup the Test range
	SetTestRange(1,COLORWRITE_TESTS * 3);

	// Make sure we are on at lease DX8
	if (m_dwVersion <= 0x0700)
	{
		WriteToLog("This test requires at least Dx8.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Check the caps we are interested in
	DWORD dwPrimitiveMiscCaps = m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps;

	// Check the ColorWriteEnable cap
	if (!(dwPrimitiveMiscCaps & D3DPMISCCAPS_COLORWRITEENABLE))
	{
		WriteToLog("Device capability not supported: Misc ColorWriteEnable.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Get the current render target
    if (FAILED(m_pSrcDevice->GetRenderTarget(&pSrcTarget)))
    {
		WriteToLog("GetRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		RELEASE(pSrcTarget);
    	return D3DTESTINIT_ABORT;
    }

    // Get the current z buffer
    if (FAILED(m_pSrcDevice->GetDepthStencilSurface(&pSrcZBuffer)))
    {
		WriteToLog("GetDepthStencilSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		RELEASE(pSrcTarget);
		RELEASE(pSrcZBuffer);
    	return D3DTESTINIT_ABORT;
    }

	// Get the default color write enable state
	GetRenderState(D3DRS_COLORWRITEENABLE, &dwDefault, CD3D_REF);
	
    // Build up a temporary buffer with alpha channel
    if (CreateDestBuffer())
		bDestBuffer = true;
    else
        bDestBuffer = false;

    // Set compare function to require 85%
	m_pFramework->GetImageCmp()->SetPassRequired(0.85f);

	return D3DTESTINIT_RUN;
}

bool CColorWriteTest::ClearFrame(void)
{
    return true;
}

bool CColorWriteTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
    int nTest = ((uTestNum-1) % COLORWRITE_TESTS) + 1;
    int nGroup = (uTestNum-1) / COLORWRITE_TESTS;
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Set the render target to render to our temporary surface.
    if (bDestBuffer)
    {
	    if (FAILED(m_pSrcDevice->SetRenderTarget(pTempSurface, pSrcZBuffer)))
	    {
		    WriteToLog("SetRenderTarget() to TempSurface failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		    RELEASE(pTempSurface);
		    RELEASE(pSrcZBuffer);
		    TestFinish(D3DTESTFINISH_ABORT);
		    return false;
	    }
    }

	// Let's build some triangles.
	DrawColorWriteTests();

    // Update clear color for logfile and status window
    if (nGroup == 0)
    {
	    Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,0,0), 1.0f, 0);
        dwClearAlpha = 0;
    	sprintf(msgString, "%sClear: RGBA(0,0,0,0)", msgString);
    }
    else if (nGroup == 1)
    {
	    Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(128,128,128,128), 1.0f, 0);
        dwClearAlpha = 128;
    	sprintf(msgString, "%sClear: RGBA(128,128,128,128)", msgString);
    }
    else // nGroup == 2
    {
	    Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(255,255,255,255), 1.0f, 0);
        dwClearAlpha = 255;
    	sprintf(msgString, "%sClear: RGBA(255,255,255,255)", msgString);
    }

    // By default, don't check alpha values
    bAlphaMask = false;

	// Set the appropriate renderstate
	switch (nTest)
	{
		case (1):
			sprintf(msgString, "%s, ColorWrite: Default", msgString);
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)dwDefault);
			break;
		case (2):
			sprintf(msgString, "%s, ColorWrite: Red", msgString);
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED);
			break;
		case (3):
			sprintf(msgString, "%s, ColorWrite: Green", msgString);
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_GREEN);
			break;
		case (4):
			sprintf(msgString, "%s, ColorWrite: Blue", msgString);
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_BLUE);
			break;
		case (5):
			sprintf(msgString, "%s, ColorWrite: Alpha", msgString);
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_ALPHA);
			break;
		case (6):
			sprintf(msgString, "%s, ColorWrite: Red|Green", msgString);
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN);
			break;
		case (7):
			sprintf(msgString, "%s, ColorWrite: Red|Blue", msgString);
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_BLUE);
			break;
		case (8):
			sprintf(msgString, "%s, ColorWrite: Red|Alpha", msgString);
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_ALPHA);
			break;
		case (9):
			sprintf(msgString, "%s, ColorWrite: Green|Blue", msgString);
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE);
			break;
		case (10):
			sprintf(msgString, "%s, ColorWrite: Green|Alpha", msgString);
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_ALPHA);
			break;
		case (11):
			sprintf(msgString, "%s, ColorWrite: Blue|Alpha", msgString);
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
			break;
		case (12):
			sprintf(msgString, "%s, ColorWrite: Red|Green|Blue", msgString);
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE);
			break;
		case (13):
			sprintf(msgString, "%s, ColorWrite: Red|Green|Alpha", msgString);
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_ALPHA);
			break;
		case (14):
			sprintf(msgString, "%s, ColorWrite: Red|Blue|Alpha", msgString);
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
			break;
		case (15):
			sprintf(msgString, "%s, ColorWrite: Green|Blue|Alpha", msgString);
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
			break;
		case (16):
			sprintf(msgString, "%s, ColorWrite: Red|Green|Blue|Alpha", msgString);
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
			break;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    // Render
	if (BeginScene())
	{
        // Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();

        if (bDestBuffer)
        {
            HRESULT hr;

            // Load the image back to the original back buffer
            hr = m_pSrcDevice->D3DXLoadSurfaceFromSurface(
		             pSrcTarget,
		             NULL, 
                     NULL,					    
			         pTempSurface, 
			         NULL,					    
			         NULL,					   
			         D3DX_FILTER_POINT, 	  
                     0);

            if (FAILED(hr))
            {
                WriteToLog("Source D3DXLoadSurfaceFromSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
		        TestFinish(D3DTESTFINISH_ABORT);
		        return false;
            }

	        // Set the render target to our original surface
	        if (FAILED(m_pSrcDevice->SetRenderTarget(pSrcTarget, pSrcZBuffer)))
	        {
		        WriteToLog("SetRenderTarget() back to pSrcTarget failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		        RELEASE(pSrcTarget);
		        RELEASE(pSrcZBuffer);
		        TestFinish(D3DTESTFINISH_ABORT);
		        return false;
	        }
        }
	}	

    return true;
}

void CColorWriteTest::SceneRefresh(void)
{
}

bool CColorWriteTest::ProcessFrame(void)
{
    DWORD   dwAlphaValue;
    bool	bCompareResult;
    bool    bAlphaResult = true;
	int		nBadAlphaPixels = 0;

     // Did we pass the color compare
    if (!CD3DTest::ProcessFrame())
    {
	    WriteToLog("ProcessFrame() failed.\n");
        return false;
    }

    bCompareResult = m_pFramework->m_DXGLog.GetCurrentResult() == PASS;

    if (bDestBuffer)
    {
        int         nPitch;
	    RECT	    rSurf;
        LOCKED_RECT LockRect;
        HRESULT     hr;
	    void	    *pBuf, *pColor;

	    // Build the subrect we want
	    rSurf.top = 10;
	    rSurf.left = 10;
	    rSurf.bottom = 270;
	    rSurf.right = 310;

	    // Get the Surface data pointer for our subrect
        hr = pTempSurface->LockRect(&LockRect,&rSurf,NULL);

        if (FAILED(hr))
	    {
		    WriteToLog("ProcessFrame: LockRect() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
            Fail();
            return false;
	    }

        pBuf = LockRect.pBits;
        nPitch = LockRect.iPitch;

	    // Look for Alpha pixel matches
	    for (int j=0; j < 260; j++)		// Height loop
	    {
		    pColor = (char*)pBuf + (nPitch * j);

		    for (int i=0; i < 300; i++)	// Width loop
		    {
			    DWORD dwColor = pTempSurface->ReadColor(&pColor);
			    DWORD dwAlpha = RGBA_GETALPHA(dwColor);

                if (bAlphaMask)
                {
                    if (dwAlpha != 240)
                    {
						nBadAlphaPixels++;
                        bAlphaResult = false;
                        dwAlphaValue = dwAlpha;
                    }
                }
                else
                {
                    if (dwAlpha != dwClearAlpha)
                    {
						nBadAlphaPixels++;
                        bAlphaResult = false;
                        dwAlphaValue = dwAlpha;
                    }
                }
		    }
	    }

        hr = pTempSurface->UnlockRect();

        if (FAILED(hr))
	    {
		    WriteToLog("ProcessFrame: UnlockRect() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
            Fail();
            return false;
	    }
    }

	// Tell the logfile about invalid alpha values
    if (!bAlphaResult)
    {
        if (bAlphaMask)
		    WriteToLog("Found invalid Alpha value (%d, expected 240) on destination surface.\n",dwAlphaValue);
        else
		    WriteToLog("Found invalid Alpha value (%d, expected %d) on destination surface.\n",dwAlphaValue,dwClearAlpha);

		m_fPassPercentage = 1.0f - (nBadAlphaPixels / (300.0f * 260.0f));
        Fail();
        return false;
    }

    return bCompareResult;
}

bool CColorWriteTest::TestTerminate()
{
	// Cleanup buffers that we created
	RELEASE(pTempSurface);
	RELEASE(pSrcTarget);
	RELEASE(pSrcZBuffer);

	return true;
}

bool CColorWriteTest::CreateDestBuffer(void)
{
    HRESULT hr;
    SURFACEDESC Desc;

    // Get target dimensions
    pSrcTarget->GetDesc(&Desc);

    // Create a temp surface for rendering
    hr = m_pSrcDevice->CreateRenderTarget(Desc.dwWidth, 
                                          Desc.dwHeight,
                                          FMT_A8R8G8B8,
                                          MULTISAMPLE_NONE,
                                          true,
                                          &pTempSurface); 
    if (FAILED(hr))
    {
	    WriteToLog("CreateRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
		RELEASE(pTempSurface);
		GetLastError();
		return false;
	}

    return true;
}

//************************************************************************
// Internal API:    DrawColorWriteTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CColorWriteTest::DrawColorWriteTests(void)
{
    D3DVECTOR v0 = { 10.0f, 10.0f, 0.5f};
    D3DVECTOR v1 = {160.0f, 10.0f, 0.5f};
    D3DVECTOR v2 = { 10.0f,270.0f, 0.5f};
    D3DVECTOR v3 = { 10.0f,270.0f, 0.5f};
    D3DVECTOR v4 = {160.0f, 10.0f, 0.5f};
    D3DVECTOR v5 = {160.0f,270.0f, 0.5f};
    D3DVECTOR v6 = {160.0f,270.0f, 0.5f};
    D3DVECTOR v7 = {160.0f, 10.0f, 0.5f};
    D3DVECTOR v8 = {310.0f,270.0f, 0.5f};
    D3DVECTOR v9 = {160.0f, 10.0f, 0.5f};
    D3DVECTOR v10 = {310.0f, 10.0f, 0.5f};
    D3DVECTOR v11 = {310.0f,270.0f, 0.5f};

    // RGB Triangle
	VertexList[dwVertexCount++] = D3DTLVERTEX(v0, (1.0f / 0.5f), RGBA_MAKE(0,255,0,240), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(v1, (1.0f / 0.5f), RGBA_MAKE(0,0,255,240), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(v2, (1.0f / 0.5f), RGBA_MAKE(255,0,0,240), 0, 0.0f,1.0f);

    // White Triangle
	VertexList[dwVertexCount++] = D3DTLVERTEX(v3, (1.0f / 0.5f), RGBA_MAKE(255,255,255,240), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(v4, (1.0f / 0.5f), RGBA_MAKE(255,255,255,240), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(v5, (1.0f / 0.5f), RGBA_MAKE(255,255,255,240), 0, 0.0f,1.0f);

    // Black Triangle
	VertexList[dwVertexCount++] = D3DTLVERTEX(v6, (1.0f / 0.5f), RGBA_MAKE(0,0,0,240), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(v7, (1.0f / 0.5f), RGBA_MAKE(0,0,0,240), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(v8, (1.0f / 0.5f), RGBA_MAKE(0,0,0,240), 0, 0.0f,1.0f);

    // CMY Triangle
	VertexList[dwVertexCount++] = D3DTLVERTEX(v9,  (1.0f / 0.5f), RGBA_MAKE(0,255,255,240), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(v10, (1.0f / 0.5f), RGBA_MAKE(255,0,255,240), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(v11, (1.0f / 0.5f), RGBA_MAKE(255,255,0,240), 0, 0.0f,1.0f);
}
