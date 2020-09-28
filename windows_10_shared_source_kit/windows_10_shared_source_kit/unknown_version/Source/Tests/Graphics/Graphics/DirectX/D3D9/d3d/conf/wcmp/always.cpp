#define D3D_OVERLOADS
#include <d3d.h>
#include "WCmp.h"

//************************************************************************
// Never Test functions

CWNeverTest::CWNeverTest()
{
	m_szTestName = "W Compare Never";
	m_szCommandKey = "Never";

	// Tell parent class what cap to look for
	dwWCmpCap = D3DPCMPCAPS_NEVER;
}

bool CWNeverTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using the W Buffer and compare func Never
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)TRUE);
	SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_USEW);
	SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_NEVER);
	return true;
}

//************************************************************************
// Always Test functions

CWAlwaysTest::CWAlwaysTest()
{
	m_szTestName = "W Compare Always";
	m_szCommandKey = "Always";

	// Tell parent class what cap to look for
	dwWCmpCap = D3DPCMPCAPS_ALWAYS;
}

bool CWAlwaysTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using the W Buffer and compare func Always
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)TRUE);
	SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_USEW);
	SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_ALWAYS);
	return true;
}

//************************************************************************
// Internal API:    AlwaysTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CWCmpTest::AlwaysTests(int nTest)
{
	switch (nTest)
	{
		case (1):	// Left Handed Flat Triangles
			if (dwWCmpCap == D3DPCMPCAPS_NEVER)
				sprintf(msgString, "%sFour flat triangles (left), Visible: None",msgString);
			else
				sprintf(msgString, "%sFour flat triangles (left), Visible: All",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f, 10.0f,0.9f),(1.0f - 0.9f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f, 10.0f,0.9f),(1.0f - 0.9f), RGBA_MAKE(255,0,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f,110.0f,0.9f),(1.0f - 0.9f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f, 70.0f,0.5f),(1.0f - 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f, 70.0f,0.5f),(1.0f - 0.5f), RGBA_MAKE(0,255,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f,170.0f,0.5f),(1.0f - 0.5f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,130.0f,0.9f),(1.0f - 0.9f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,130.0f,0.9f),(1.0f - 0.9f), RGBA_MAKE(0,0,255,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,230.0f,0.9f),(1.0f - 0.9f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			// Yellow Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 90.0f, 45.0f,0.5f),(1.0f - 0.5f), RGBA_MAKE(255,255,0,255),0, 0.0f, 0.25f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f, 10.0f,0.5f),(1.0f - 0.5f), RGBA_MAKE(255,255,0,255),0, 1.0f, 0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(195.0f,150.0f,0.5f),(1.0f - 0.5f), RGBA_MAKE(255,255,0,255),0, 0.75f,1.0f);
			break;
		case (2):	// Right Handed Flat Triangles
			if (dwWCmpCap == D3DPCMPCAPS_NEVER)
				sprintf(msgString, "%sFour flat triangles (right), Visible: None",msgString);
			else
				sprintf(msgString, "%sFour flat triangles (right), Visible: All",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f, 10.0f,0.35f),(1.0f - 0.35f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f,110.0f,0.35f),(1.0f - 0.35f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,110.0f,0.35f),(1.0f - 0.35f), RGBA_MAKE(255,0,0,255),0, 0.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f, 70.0f,0.4f),(1.0f - 0.4f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f,170.0f,0.4f),(1.0f - 0.4f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f,170.0f,0.4f),(1.0f - 0.4f), RGBA_MAKE(0,255,0,255),0, 0.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,130.0f,0.3f),(1.0f - 0.3f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,230.0f,0.3f),(1.0f - 0.3f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,230.0f,0.3f),(1.0f - 0.3f), RGBA_MAKE(0,0,255,255),0, 0.0f,1.0f);
			// Yellow Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 45.0f, 90.0f,0.5f),(1.0f - 0.5f), RGBA_MAKE(255,255,0,255),0, 0.25f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f,195.0f,0.5f),(1.0f - 0.5f), RGBA_MAKE(255,255,0,255),0, 1.0f, 0.75f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,230.0f,0.5f),(1.0f - 0.5f), RGBA_MAKE(255,255,0,255),0, 0.0f, 1.0f);
			break;
		case (3):	// Single Flat Triangles
			if (dwWCmpCap == D3DPCMPCAPS_NEVER)
				sprintf(msgString, "%sFour flat triangles, Visible: None",msgString);
			else
				sprintf(msgString, "%sFour flat triangles, Visible: All",msgString);

			// Magenta Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f, 10.0f,0.01f),(1.0f - 0.01f), RGBA_MAKE(255,0,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(300.0f,270.0f,0.01f),(1.0f - 0.01f), RGBA_MAKE(255,0,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  0.0f,270.0f,0.01f),(1.0f - 0.01f), RGBA_MAKE(255,0,255,255),0, 0.0f,1.0f);
			// Teal Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f, 20.0f,0.0f),(1.0f - 0.0f), RGBA_MAKE(0,255,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(280.0f,260.0f,0.0f),(1.0f - 0.0f), RGBA_MAKE(0,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 20.0f,260.0f,0.0f),(1.0f - 0.0f), RGBA_MAKE(0,255,255,255),0, 0.0f,1.0f);
			// White Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f, 30.0f,0.9f),(1.0f - 0.9f), RGBA_MAKE(255,255,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(260.0f,250.0f,0.9f),(1.0f - 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 40.0f,250.0f,0.9f),(1.0f - 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,1.0f);
			// Red Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f, 40.0f,0.999f),0.01f, RGBA_MAKE(255,0,0,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(240.0f,240.0f,0.999f),0.01f, RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 60.0f,240.0f,0.999f),0.01f, RGBA_MAKE(255,0,0,255),0, 0.0f,1.0f);
			break;
		case (4):	// Left Handed Non-Flat Triangles
			if (dwWCmpCap == D3DPCMPCAPS_NEVER)
				sprintf(msgString, "%sFour non-flat triangles (left), Visible: None",msgString);
			else
				sprintf(msgString, "%sFour non-flat triangles (left), Visible: All",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f, 10.0f,0.9f),(1.0f - 0.9f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f, 10.0f,0.0f),(1.0f - 0.0f), RGBA_MAKE(255,0,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f,110.0f,0.5f),(1.0f - 0.5f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f, 70.0f,0.0f), (1.0f - 0.0f),  RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f, 70.0f,0.01f),(1.0f - 0.01f), RGBA_MAKE(0,255,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f,170.0f,0.9f), (1.0f - 0.9f),  RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,130.0f,0.1f),(1.0f - 0.1f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,130.0f,0.2f),(1.0f - 0.2f), RGBA_MAKE(0,0,255,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,230.0f,0.3f),(1.0f - 0.3f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			// Yellow Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 90.0f, 45.0f,0.25f), (1.0f - 0.25f), RGBA_MAKE(255,255,0,255),0, 0.0f, 0.25f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f, 10.0f,0.55f), (1.0f - 0.55f), RGBA_MAKE(255,255,0,255),0, 1.0f, 0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(195.0f,150.0f,0.05f), (1.0f - 0.05f), RGBA_MAKE(255,255,0,255),0, 0.75f,1.0f);
			break;
		case (5):	// Right Handed Non-Flat Triangles
			if (dwWCmpCap == D3DPCMPCAPS_NEVER)
				sprintf(msgString, "%sFour non-flat triangles (right), Visible: None",msgString);
			else
				sprintf(msgString, "%sFour non-flat triangles (right), Visible: All",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f, 10.0f,0.0805f), (1.0f - 0.0805f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f,110.0f,0.309f),  (1.0f - 0.309f),  RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,110.0f,0.351f),  (1.0f - 0.351f),  RGBA_MAKE(255,0,0,255),0, 0.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f, 70.0f,0.123f), (1.0f - 0.123f),  RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f,170.0f,0.0987f),(1.0f - 0.0987f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f,170.0f,0.4444f),(1.0f - 0.4444f), RGBA_MAKE(0,255,0,255),0, 0.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,130.0f,0.099f),(1.0f - 0.099f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,230.0f,0.75f), (1.0f - 0.75f),  RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,230.0f,0.4f),  (1.0f - 0.4f),   RGBA_MAKE(0,0,255,255),0, 0.0f,1.0f);
			// Yellow Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 45.0f, 90.0f,0.5001f),(1.0f - 0.5001f), RGBA_MAKE(255,255,0,255),0, 0.25f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f,195.0f,0.07f),  (1.0f - 0.07f),   RGBA_MAKE(255,255,0,255),0, 1.0f, 0.75f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,230.0f,0.2f),   (1.0f - 0.2f),    RGBA_MAKE(255,255,0,255),0, 0.0f, 1.0f);
			break;
		case (6):	// Combo Test: 0.0f
		case (7):	// Combo Test: 0.1f
		case (8):	// Combo Test: 0.2f
		case (9):	// Combo Test: 0.3f	
		case (10):	// Combo Test: 0.4f	
		case (11):	// Combo Test: 0.5f
		case (12):	// Combo Test: 0.6f
		case (13):	// Combo Test: 0.7f	
		case (14):	// Combo Test: 0.8f
		case (15):	// Combo Test: 0.9f
			if (dwWCmpCap == D3DPCMPCAPS_NEVER)
				sprintf(msgString, "%sTwo triangles (%d%% overlap), Visible: None",msgString,10 * (nTest - 6));
			else
				sprintf(msgString, "%sTwo triangles (%d%% overlap), Visible: 100%% Green",msgString,10 * (nTest - 6));

			ComboTests(0.1f * (nTest - 6));
			break;
		case (16):	// Combo Test: 0.99f
			if (dwWCmpCap == D3DPCMPCAPS_NEVER)
				sprintf(msgString, "%sTwo triangles (100%% overlap), Visible: None",msgString);
			else
				sprintf(msgString, "%sTwo triangles (100%% overlap), Visible: 100%% Green",msgString);

			ComboTests(0.99f);
			break;
		default:
			break;
	}
}
