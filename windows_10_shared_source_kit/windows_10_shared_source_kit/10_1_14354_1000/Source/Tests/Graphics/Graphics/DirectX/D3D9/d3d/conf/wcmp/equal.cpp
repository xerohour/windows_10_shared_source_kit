#define D3D_OVERLOADS
#include <d3d.h>
#include "WCmp.h"

//************************************************************************
// Equal Test functions

CWEqualTest::CWEqualTest()
{
	m_szTestName = "W Compare Equal";
	m_szCommandKey = "Equal";

	// Tell parent class what cap to look for
	dwWCmpCap = D3DPCMPCAPS_EQUAL;
}

bool CWEqualTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using the W Buffer and compare func Equal
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)TRUE);
	SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_USEW);
	SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_EQUAL);
	return true;
}

void CWEqualTest::SetupBackground(void)
{
	float Zval = 0.1f;
	float RHW  = 1.0f - Zval;
	float fWidth  = m_pCurrentMode->nBackBufferWidth * 1.0f;
	float fHeight = m_pCurrentMode->nBackBufferHeight * 1.0f;

	// Black Background Triangle
	BackgroundList[0] = D3DTLVERTEX(D3DVECTOR(fWidth, 0.0f,    Zval), RHW, RGBA_MAKE(0,0,100,255),0, 1.0f,0.0f);
	BackgroundList[1] = D3DTLVERTEX(D3DVECTOR(fWidth, fHeight, Zval), RHW, RGBA_MAKE(0,0,100,255),0, 1.0f,1.0f);
	BackgroundList[2] = D3DTLVERTEX(D3DVECTOR(0.0f,   0.0f,    Zval), RHW, RGBA_MAKE(0,0,100,255),0, 0.0f,0.0f);
	BackgroundList[3] = D3DTLVERTEX(D3DVECTOR(0.0f,   fHeight, Zval), RHW, RGBA_MAKE(0,0,100,255),0, 0.0f,1.0f);
}

void CWEqualTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		// Special setup for initing the background
		SetupBackground();

		// Special render state needed
		SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_LESSEQUAL);

		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,BackgroundList,4,NULL,0,0);

		// Set the appropriate renderstate
		SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_EQUAL);

		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

//************************************************************************
// NotEqual Test functions

CWNotEqualTest::CWNotEqualTest()
{
	m_szTestName = "W Compare NotEqual";
	m_szCommandKey = "NotEqual";

	// Tell parent class what cap to look for
	dwWCmpCap = D3DPCMPCAPS_NOTEQUAL;
}

bool CWNotEqualTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using the W Buffer and compare func NotEqual
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)TRUE);
	SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_USEW);
	SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_NOTEQUAL);
	return true;
}

void CWNotEqualTest::SetupBackground(void)
{
	float Zval = 0.1f;
	float RHW  = 1.0f - Zval;
	float fWidth  = m_pCurrentMode->nBackBufferWidth * 1.0f;
	float fHeight = m_pCurrentMode->nBackBufferHeight * 1.0f;

	// Black Background Triangle
	BackgroundList[0] = D3DTLVERTEX(D3DVECTOR(fWidth, 0.0f,    Zval), RHW, RGBA_MAKE(0,0,100,255),0, 1.0f,0.0f);
	BackgroundList[1] = D3DTLVERTEX(D3DVECTOR(fWidth, fHeight, Zval), RHW, RGBA_MAKE(0,0,100,255),0, 1.0f,1.0f);
	BackgroundList[2] = D3DTLVERTEX(D3DVECTOR(0.0f,   0.0f,    Zval), RHW, RGBA_MAKE(0,0,100,255),0, 0.0f,0.0f);
	BackgroundList[3] = D3DTLVERTEX(D3DVECTOR(0.0f,   fHeight, Zval), RHW, RGBA_MAKE(0,0,100,255),0, 0.0f,1.0f);
}

void CWNotEqualTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Special setup for initing the background
		SetupBackground();

		// Special render state needed
		SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_LESSEQUAL);

		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,BackgroundList,4,NULL,0,0);

		// Set the appropriate renderstate
		SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_NOTEQUAL);

		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

//************************************************************************
// Internal API:    EqualTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

bool CWCmpTest::EqualTests(int nTest)
{
	switch (nTest)
	{
		case (1):	// Left Handed Flat Triangles
			if (dwWCmpCap == D3DPCMPCAPS_EQUAL)
				sprintf(msgString, "%sFour flat triangles (left), Visible: Yellow & Green",msgString);
			else
				sprintf(msgString, "%sFour flat triangles (left), Visible: Red (g/y) & Blue (y)",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f, 10.0f,0.0f),(1.0f - 0.0f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f, 10.0f,0.0f),(1.0f - 0.0f), RGBA_MAKE(255,0,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f,110.0f,0.0f),(1.0f - 0.0f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f, 70.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f, 70.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(0,255,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f,170.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,130.0f,0.2f),(1.0f - 0.2f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,130.0f,0.2f),(1.0f - 0.2f), RGBA_MAKE(0,0,255,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,230.0f,0.2f),(1.0f - 0.2f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			// Yellow Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 90.0f, 45.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(255,255,0,255),0, 0.0f, 0.25f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f, 10.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(255,255,0,255),0, 1.0f, 0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(195.0f,150.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(255,255,0,255),0, 0.75f,1.0f);
			break;
		case (2):	// Right Handed Flat Triangles
			if (dwWCmpCap == D3DPCMPCAPS_EQUAL)
				sprintf(msgString, "%sFour flat triangles (right), Visible: Blue & Green",msgString);
			else
				sprintf(msgString, "%sFour flat triangles (right), Visible: Red (g) & Yellow",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f, 10.0f,0.05f),(1.0f - 0.05f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f,110.0f,0.05f),(1.0f - 0.05f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,110.0f,0.05f),(1.0f - 0.05f), RGBA_MAKE(255,0,0,255),0, 0.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f, 70.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f,170.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f,170.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(0,255,0,255),0, 0.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,130.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,230.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,230.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(0,0,255,255),0, 0.0f,1.0f);
			// Yellow Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 45.0f, 90.0f,0.05f),(1.0f - 0.05f), RGBA_MAKE(255,255,0,255),0, 0.25f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f,195.0f,0.05f),(1.0f - 0.05f), RGBA_MAKE(255,255,0,255),0, 1.0f, 0.75f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,230.0f,0.05f),(1.0f - 0.05f), RGBA_MAKE(255,255,0,255),0, 0.0f, 1.0f);
			break;
		case (3):	// Single Flat Triangles
			if (dwWCmpCap == D3DPCMPCAPS_EQUAL)
				sprintf(msgString, "%sFour flat triangles, Visible: Red & White",msgString);
			else
				sprintf(msgString, "%sFour flat triangles, Visible: White, Cyan, & Magenta",msgString);

			// Magenta Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f, 10.0f,0.11f),(1.0f - 0.11f), RGBA_MAKE(255,0,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(300.0f,270.0f,0.11f),(1.0f - 0.11f), RGBA_MAKE(255,0,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  0.0f,270.0f,0.11f),(1.0f - 0.11f), RGBA_MAKE(255,0,255,255),0, 0.0f,1.0f);
			// Teal Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f, 20.0f,0.09f),(1.0f - 0.09f), RGBA_MAKE(0,255,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(280.0f,260.0f,0.09f),(1.0f - 0.09f), RGBA_MAKE(0,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 20.0f,260.0f,0.09f),(1.0f - 0.09f), RGBA_MAKE(0,255,255,255),0, 0.0f,1.0f);
			// White Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f, 30.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(255,255,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(260.0f,250.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(255,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 40.0f,250.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(255,255,255,255),0, 0.0f,1.0f);
			// Red Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f, 40.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(255,0,0,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(240.0f,240.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 60.0f,240.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(255,0,0,255),0, 0.0f,1.0f);
			break;
		case (4):	// Combo Test: 0.0f
		case (5):	// Combo Test: 0.1f
		case (6):	// Combo Test: 0.2f
		case (7):	// Combo Test: 0.3f	
		case (8):	// Combo Test: 0.4f	
		case (9):	// Combo Test: 0.5f
		case (10):	// Combo Test: 0.6f
		case (11):	// Combo Test: 0.7f	
		case (12):	// Combo Test: 0.8f
		case (13):	// Combo Test: 0.9f
			if (dwWCmpCap == D3DPCMPCAPS_EQUAL)
			{
				if (nTest == 5)
					sprintf(msgString, "%sTwo triangles (%d%% overlap), Visible: 100%% Green",msgString,10 * (nTest - 4));
				else
					sprintf(msgString, "%sTwo triangles (%d%% overlap), Visible: None",msgString,10 * (nTest - 4));
			}
			else
				sprintf(msgString, "%sTwo triangles (%d%% overlap), Visible: 100%% Green",msgString,10 * (nTest - 4));

			ComboTests(0.1f * (nTest - 4));
			break;
		case (14):	// Combo Test: 0.99f
			if (dwWCmpCap == D3DPCMPCAPS_EQUAL)
				sprintf(msgString, "%sTwo triangles (100%% overlap), Visible: None",msgString);
			else
				sprintf(msgString, "%sTwo triangles (100%% overlap), Visible: 100%% Green",msgString);

			ComboTests(0.99f);
			break;
		default:
			break;
	}

	return TRUE;
}
