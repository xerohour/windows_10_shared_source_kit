#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "CTextures.h"
#include "ZBuffer.h"

//************************************************************************
// NoDiscard Test functions

CZNoDiscardTest::CZNoDiscardTest()
{
	m_szTestName = "No Discard Z Buffer Formats";
	m_szCommandKey = "NoDiscard";

    // Init framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;
	m_Options.D3DOptions.dwPresentFlags = PRESENTFLAG_LOCKABLE_BACKBUFFER;
	m_Options.ModeOptions.dwZExclude = PF_LOCKABLE;

	// Init Discard flag
	bDiscard = false;
}

//************************************************************************
// Discard Test functions

CZDiscardTest::CZDiscardTest()
{
	m_szTestName = "Discard Z Buffer Formats";
	m_szCommandKey = "Discard";

    // Init framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;
	m_Options.D3DOptions.dwPresentFlags = PRESENTFLAG_LOCKABLE_BACKBUFFER | PRESENTFLAG_DISCARD_DEPTHSTENCIL;
	m_Options.ModeOptions.dwZExclude = PF_LOCKABLE;

	// Init Discard flag
	bDiscard = true;
}

UINT CZDiscardableTest::TestInitialize(void)
{
    TCHAR szBuffer[80];
    SURFACEDESC Desc;
    CSurface *pTemp;
	msgString[0] = '\0';

	// Setup the Test range
	SetTestRange(1, MAX_DISCARDABLE_TESTS);

    // Get the current render target
    if (!GetRenderTarget(&pOriginalRT))
    {
		WriteToLog("GetRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		RELEASE(pOriginalRT);
    	return D3DTESTINIT_ABORT;
    }

	// Get some info from the current render target
    pOriginalRT->GetSurface(&pTemp,1);
    pTemp->GetDesc(&Desc);
    RELEASE(pTemp);

	// Create a new render target for certain test cases
    if (!CreateRenderTarget(Desc.Width, Desc.Height, Desc.Format, MULTISAMPLE_NONE, 0, true, &pNewRT)) 
    {
	    WriteToLog("CreateRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		RELEASE(pOriginalRT);
		RELEASE(pNewRT);
		return D3DTESTINIT_ABORT;
	}

	// Log the current Z Buffer format
	FmtToString(FORMAT_ZBUFFER,&m_pCommonZBufferFormats[m_pCurrentMode->nZBufferFormat],szBuffer);
	sprintf(msgString, "%sZ Buffer Format: %s.\n",msgString,szBuffer);

    // Get the current z buffer
    if (!GetDepthStencilSurface(&pOriginalZ))
    {
		WriteToLog("GetDepthStencilSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		RELEASE(pOriginalZ);
		RELEASE(pOriginalRT);
		RELEASE(pNewRT);
    	return D3DTESTINIT_ABORT;
    }

	WriteToLog(msgString);

	// Get some info from the current Z buffer
    pOriginalZ->GetSurface(&pTemp,1);
    pTemp->GetDesc(&Desc);
    RELEASE(pTemp);

	// Create a new Z Buffer for certain test cases
	if (!CreateDepthStencilSurface(Desc.Width, Desc.Height, Desc.Format, MULTISAMPLE_NONE, 0, true, &pNewZ))
	{
	    WriteToLog("CreateDepthStencilSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		RELEASE(pNewZ);
		RELEASE(pOriginalZ);
		RELEASE(pOriginalRT);
		RELEASE(pNewRT);
		return D3DTESTINIT_ABORT;
	}

	return D3DTESTINIT_RUN;
}

bool CZDiscardableTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

bool CZDiscardableTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	bool bExtraDraw = false;
	msgString[0] = '\0';

	// Tell log about the test
	switch (uTestNum)
	{
		case (1):
			if (bDiscard)
				sprintf(msgString, "%sZDiscard: True, Green Triangle Visible", msgString);
			else
				sprintf(msgString, "%sZDiscard: False, Green & Blue Triangles Visible", msgString);
            break;
		case (2):
			if (bDiscard)
				sprintf(msgString, "%sZDiscard: True, Green & Blue Triangles Visible", msgString);
			else
				sprintf(msgString, "%sZDiscard: False, Green & Blue Triangles Visible", msgString);
            break;
		case (3):
			if (bDiscard)
				sprintf(msgString, "%sZDiscard: True, Green Triangle Visible", msgString);
			else
				sprintf(msgString, "%sZDiscard: False, Green & Blue Triangles Visible", msgString);
            break;
		case (4):
			if (bDiscard)
				sprintf(msgString, "%sZDiscard: True, Green & Blue Triangles Visible", msgString);
			else
				sprintf(msgString, "%sZDiscard: False, Green, Blue, & Red Triangles Visible", msgString);
            break;
		case (5):
			if (bDiscard)
				sprintf(msgString, "%sZDiscard: True, Green & Blue Triangles Visible", msgString);
			else
				sprintf(msgString, "%sZDiscard: False, Green, Blue, & Red Triangles Visible", msgString);
            break;
		case (6):
			if (bDiscard)
				sprintf(msgString, "%sZDiscard: True, Green & Blue Triangles Visible", msgString);
			else
				sprintf(msgString, "%sZDiscard: False, Green & Blue Triangles Visible", msgString);
            break;
		case (7):
			if (bDiscard)
				sprintf(msgString, "%sZDiscard: True, Green & Red Triangles Visible", msgString);
			else
				sprintf(msgString, "%sZDiscard: False, Green & Red Triangles Visible", msgString);
            break;
		case (8):
			if (bDiscard)
				sprintf(msgString, "%sZDiscard: True, Green Triangle Visible", msgString);
			else
				sprintf(msgString, "%sZDiscard: False, Green & Red Triangles Visible", msgString);
            break;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

	// Draw the green triangle (z==0.3f)
	DrawTriangle(1);

	// Render the scene
	if (BeginScene())
	{
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,3,NULL,0,0);
		EndScene();
	}	

	// Perform the appropriate test case
	switch (uTestNum)
	{
		case (1): // Present
		    CD3DTest::DisplayFrame();
			break;
		case (2): // Set with same Z
			SetDepthStencilSurface(pOriginalZ);
			break;
		case (3): // Switch to discard Z and switch Z back
			SetDepthStencilSurface(pNewZ);
			SetDepthStencilSurface(pOriginalZ);
			break;
		case (4): // Switch to discard Z, draw, and switch Z back
			SetDepthStencilSurface(pNewZ);
			Clear(0, NULL, CLEARF_ZBUFFER, 0, 1.0f, 0);

			// Draw the blue triangle (z==0.7f)
			DrawTriangle(2);
			bExtraDraw = true;

			// Render the scene
			if (BeginScene())
			{
				RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,3,NULL,0,0);
				EndScene();
			}	

			SetDepthStencilSurface(pOriginalZ);
			break;
		case (5): // Switch to discard Z, draw/present, and switch Z back
			SetDepthStencilSurface(pNewZ);
			Clear(0, NULL, CLEARF_ZBUFFER, 0, 1.0f, 0);

			// Draw the blue triangle (z==0.7f)
			DrawTriangle(2);
			bExtraDraw = true;

			// Render the scene
			if (BeginScene())
			{
				RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,3,NULL,0,0);
				EndScene();
			}	

			CD3DTest::DisplayFrame();

			SetDepthStencilSurface(pOriginalZ);
			break;
		case (6): // Switch to new RT and switch RT back
  			SetRenderTarget((DWORD)0,pNewRT);
			SetRenderTarget((DWORD)0,pOriginalRT);
			break;
		case (7): // Switch to new RT, draw, and switch RT back
  			SetRenderTarget((DWORD)0,pNewRT);
			Clear(0, NULL, CLEARF_TARGET, RGBA_MAKE(0,100,0,255), 0.0f, 0);

			// Draw the blue triangle (z==0.7f)
			DrawTriangle(2);
			bExtraDraw = true;

			// Render the scene
			if (BeginScene())
			{
				RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,3,NULL,0,0);
				EndScene();
			}	

			SetRenderTarget((DWORD)0,pOriginalRT);
			break;
		case (8): // Switch to new RT, draw/present, and switch RT back
  			SetRenderTarget((DWORD)0,pNewRT);
			Clear(0, NULL, CLEARF_TARGET, RGBA_MAKE(0,100,0,255), 0.0f, 0);

			// Draw the blue triangle (z==0.7f)
			DrawTriangle(2);
			bExtraDraw = true;

			// Render the scene
			if (BeginScene())
			{
				RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,3,NULL,0,0);
				EndScene();
			}	

			CD3DTest::DisplayFrame();

			SetRenderTarget((DWORD)0,pOriginalRT);
			break;
	}

	// If third triangle draw the red triangle (z==0.1f), else draw the blue triangle (z==0.7f)
	DrawTriangle((bExtraDraw)?0:2);

	// Render the scene
	if (BeginScene())
	{
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,3,NULL,0,0);
		EndScene();
	}	

    return true;
}

void CZDiscardableTest::SceneRefresh(void)
{
}

bool CZDiscardableTest::ProcessFrame(void)
{
	// Don't do the image compare if trying to discard Z on retail runtime.
	if (bDiscard && !m_pD3D->GetDebug())
	{
		Pass();
		return true;
	}
	else
		return CD3DTest::ProcessFrame();
}

bool CZDiscardableTest::TestTerminate(void)
{
	// Cleanup buffers
    RELEASE(pNewZ);
    RELEASE(pOriginalZ);
    RELEASE(pOriginalRT);
    RELEASE(pNewRT);

	return true;
}


//************************************************************************
// Internal API:    DrawTriangle
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CZDiscardableTest::DrawTriangle(int nColor)
{
	if (nColor == 0) // Red
	{
		D3DVECTOR r0 = {145.0f,  40.0f, 0.1f};
		D3DVECTOR r1 = {245.0f, 255.0f, 0.1f};
		D3DVECTOR r2 = { 55.0f, 255.0f, 0.1f};

		// Red Triangle
		VertexList[0] = D3DTLVERTEX(r0, (1.0f / r0.z), RGBA_MAKE(255,0,0,255), 0, 0.5f,0.0f);
		VertexList[1] = D3DTLVERTEX(r1, (1.0f / r1.z), RGBA_MAKE(255,0,0,255), 0, 1.0f,1.0f);
		VertexList[2] = D3DTLVERTEX(r2, (1.0f / r2.z), RGBA_MAKE(255,0,0,255), 0, 0.0f,1.0f);

	}
	else if (nColor == 1) // Green
	{
		D3DVECTOR g0 = {115.0f,  40.0f, 0.3f};
		D3DVECTOR g1 = {205.0f, 225.0f, 0.3f};
		D3DVECTOR g2 = { 25.0f, 225.0f, 0.3f};

		// Green Triangle
		VertexList[0] = D3DTLVERTEX(g0, (1.0f / g0.z), RGBA_MAKE(0,255,0,255), 0, 0.5f,0.0f);
		VertexList[1] = D3DTLVERTEX(g1, (1.0f / g1.z), RGBA_MAKE(0,255,0,255), 0, 1.0f,1.0f);
		VertexList[2] = D3DTLVERTEX(g2, (1.0f / g2.z), RGBA_MAKE(0,255,0,255), 0, 0.0f,1.0f);
	}
	else // Blue
	{
		D3DVECTOR b0 = {130.0f,  40.0f, 0.7f};
		D3DVECTOR b1 = {225.0f, 240.0f, 0.7f};
		D3DVECTOR b2 = { 40.0f, 240.0f, 0.7f};

		// Blue Triangle
		VertexList[0] = D3DTLVERTEX(b0, (1.0f / b0.z), RGBA_MAKE(0,0,255,255), 0, 0.5f,0.0f);
		VertexList[1] = D3DTLVERTEX(b1, (1.0f / b1.z), RGBA_MAKE(0,0,255,255), 0, 1.0f,1.0f);
		VertexList[2] = D3DTLVERTEX(b2, (1.0f / b2.z), RGBA_MAKE(0,0,255,255), 0, 0.0f,1.0f);
	}

}
