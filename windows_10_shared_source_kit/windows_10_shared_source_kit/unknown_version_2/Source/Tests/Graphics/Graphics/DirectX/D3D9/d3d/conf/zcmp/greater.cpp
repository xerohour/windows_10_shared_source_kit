#define D3D_OVERLOADS
#include <d3d.h>
#include "ZCmp.h"

//************************************************************************
// Greater Test functions

CZGreaterTest::CZGreaterTest()
{
	m_szTestName = "Z Compare Greater";
	m_szCommandKey = "Greater";

	// Tell parent class what cap to look for
	dwZCmpCap = D3DPCMPCAPS_GREATER;
}

bool CZGreaterTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using the Z Buffer and compare func Greater
	SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_TRUE);
	SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_GREATER);
	return true;
}

bool CZGreaterTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 0.0f, 0);
}

//************************************************************************
// GreaterEqual Test functions

CZGreaterEqualTest::CZGreaterEqualTest()
{
	m_szTestName = "Z Compare GreaterEqual";
	m_szCommandKey = "GreaterEqual";

	// Tell parent class what cap to look for
	dwZCmpCap = D3DPCMPCAPS_GREATEREQUAL;
}

bool CZGreaterEqualTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using the Z Buffer and compare func GreaterEqual
	SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_TRUE);
	SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_GREATEREQUAL);
	return true;
}

bool CZGreaterEqualTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 0.0f, 0);
}

//************************************************************************
// Internal API:    GreaterTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CZCmpTest::GreaterTests(int nTest)
{
	switch (nTest)
	{
		case (1):	// Left Handed Flat Triangles
			if (dwZCmpCap == D3DPCMPCAPS_GREATER)
				sprintf(msgString, "%sThree flat triangles (left), Visible: Green, Red, & Blue",msgString);
			else
				sprintf(msgString, "%sThree flat triangles (left), Visible: Blue, Green, & Red",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f, 10.0f,0.5f),(1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f, 10.0f,0.5f),(1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f,110.0f,0.5f),(1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f, 70.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f, 70.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f,170.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,130.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,130.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,230.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			break;
		case (2):	// Right Handed Flat Triangles
			if (dwZCmpCap == D3DPCMPCAPS_GREATER)
				sprintf(msgString, "%sThree flat triangles (right), Visible: Green, Red, & Blue",msgString);
			else
				sprintf(msgString, "%sThree flat triangles (right), Visible: Blue, Green, & Red",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f, 10.0f,0.7f),(1.0f / 0.7f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f,110.0f,0.7f),(1.0f / 0.7f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,110.0f,0.7f),(1.0f / 0.7f), RGBA_MAKE(255,0,0,255),0, 0.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f, 70.0f,0.75f),(1.0f / 0.75f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f,170.0f,0.75f),(1.0f / 0.75f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f,170.0f,0.75f),(1.0f / 0.75f), RGBA_MAKE(0,255,0,255),0, 0.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,130.0f,0.75f),(1.0f / 0.75f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,230.0f,0.75f),(1.0f / 0.75f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,230.0f,0.75f),(1.0f / 0.75f), RGBA_MAKE(0,0,255,255),0, 0.0f,1.0f);
			break;
		case (3):	// Single Flat Triangles
			if (dwZCmpCap == D3DPCMPCAPS_GREATER)
				sprintf(msgString, "%sThree flat triangles, Visible: Cyan & Magenta",msgString);
			else
				sprintf(msgString, "%sThree flat triangles, Visible: White, Cyan, & Magenta",msgString);

			// Magenta Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f, 10.0f,0.1001f),(1.0f / 0.1001f), RGBA_MAKE(255,0,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(300.0f,270.0f,0.1001f),(1.0f / 0.1001f), RGBA_MAKE(255,0,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  0.0f,270.0f,0.1001f),(1.0f / 0.1001f), RGBA_MAKE(255,0,255,255),0, 0.0f,1.0f);
			// Cyan Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f, 20.0f,0.1002f),(1.0f / 0.1002f), RGBA_MAKE(0,255,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(280.0f,260.0f,0.1002f),(1.0f / 0.1002f), RGBA_MAKE(0,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 20.0f,260.0f,0.1002f),(1.0f / 0.1002f), RGBA_MAKE(0,255,255,255),0, 0.0f,1.0f);
			// White Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f, 30.0f,0.1002f),(1.0f / 0.1002f), RGBA_MAKE(255,255,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(260.0f,250.0f,0.1002f),(1.0f / 0.1002f), RGBA_MAKE(255,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 40.0f,250.0f,0.1002f),(1.0f / 0.1002f), RGBA_MAKE(255,255,255,255),0, 0.0f,1.0f);
			break;
		case (4):	// Left Handed Non-Flat Triangles
			sprintf(msgString, "%sThree intersecting triangles (left), Visible: All",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f, 10.0f,0.8f), (1.0f / 0.8f),  RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f, 10.0f,0.75f),(1.0f / 0.75f), RGBA_MAKE(255,0,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f,110.0f,0.75f),(1.0f / 0.75f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f, 70.0f,0.8f), (1.0f / 0.8f),  RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f, 70.0f,0.65f),(1.0f / 0.65f), RGBA_MAKE(0,255,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f,170.0f,0.65f),(1.0f / 0.65f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,130.0f,0.7f), (1.0f / 0.7f),  RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,130.0f,0.65f),(1.0f / 0.65f), RGBA_MAKE(0,0,255,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,230.0f,0.65f),(1.0f / 0.65f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			break;
		case (5):	// Right Handed Non-Flat Triangles
			sprintf(msgString, "%sThree intersecting triangles (right), Visible: All",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f, 10.0f,0.8f), (1.0f / 0.8f),  RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f,110.0f,0.75f),(1.0f / 0.75f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,110.0f,0.8f), (1.0f / 0.8f),  RGBA_MAKE(255,0,0,255),0, 0.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f, 70.0f,0.8f), (1.0f / 0.8f),  RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(170.0f,170.0f,0.65f),(1.0f / 0.65f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 70.0f,170.0f,0.7f), (1.0f / 0.7f),  RGBA_MAKE(0,255,0,255),0, 0.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,130.0f,0.7f), (1.0f / 0.7f),  RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(230.0f,230.0f,0.65f),(1.0f / 0.65f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f,230.0f,0.65f),(1.0f / 0.65f), RGBA_MAKE(0,0,255,255),0, 0.0f,1.0f);
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
			if (nTest == 6)
				sprintf(msgString, "%sTwo triangles (%d%% overlap), Visible: 100%% Red",msgString,10 * (nTest - 6));
			else
				sprintf(msgString, "%sTwo triangles (%d%% overlap), Visible: %d%% Green & %d%% Red",msgString,(10 * (nTest - 6)),(10 * (nTest - 6)),100 - (10 * (nTest - 6)));

			ComboTests(0.1f * (nTest - 6));
			break;
		case (16):	// Combo Test: 0.9999999f
			sprintf(msgString, "%sTwo triangles (100%% overlap), Visible: 100%% Green",msgString);

			ComboTests(0.9999999f);
			break;
		default:
			break;
	}
}
