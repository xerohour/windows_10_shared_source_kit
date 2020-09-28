#define D3D_OVERLOADS
#include <d3d.h>
#include "BasicRst.h"

//************************************************************************
// Dither Test functions

CDitherTest::CDitherTest()
{
	m_szTestName = "BasicRst Dither";
	m_szCommandKey = "Dither";
}

CDitherTest::~CDitherTest()
{
}

UINT CDitherTest::TestInitialize(void)
{
	// Setup the Test range
	SetTestRange(1,DITHER_TESTS);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_pSrcDevice->GetCaps()->dwRasterCaps;

	// Check the Dither cap
	if (!(dwRasterCaps & D3DPRASTERCAPS_DITHER))
	{
		WriteToLog("Device capability not supported: Raster Dither.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Set compare function to require 85%
	m_pFramework->GetImageCmp()->SetPassRequired(0.85f);

	return D3DTESTINIT_RUN;
}

bool CDitherTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Let's build some triangles.
	DrawDitherTests(uTestNum % 4);

	// Set the appropriate renderstate
	if (uTestNum <= 4)
	{
		sprintf(msgString, "%sTrue, ", msgString);
		SetRenderState(D3DRENDERSTATE_DITHERENABLE, (DWORD)TRUE);
	}
	else
	{
		sprintf(msgString, "%sFalse, ", msgString);
		SetRenderState(D3DRENDERSTATE_DITHERENABLE, (DWORD)FALSE);
	}

	switch (uTestNum % 4)
	{
		case (0):
			sprintf(msgString, "%sBlue Mesh", msgString);
			break;
		case (1):
			sprintf(msgString, "%sWhite Mesh", msgString);
			break;
		case (2):
			sprintf(msgString, "%sRed Mesh", msgString);
			break;
		case (3):
			sprintf(msgString, "%sGreen Mesh", msgString);
			break;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    return true;
}

//************************************************************************
// Internal API:    DrawDitherTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CDitherTest::DrawDitherTests(int nTest)
{
	// Color of triangles
	D3DCOLOR Color[] = { RGBA_MAKE(0,0,255,255),		// Blue
						 RGBA_MAKE(255,255,255,255),	// White
						 RGBA_MAKE(255,0,0,255),		// Red
						 RGBA_MAKE(0,255,0,255) };		// Green

	// Mesh with Black bottom right corner
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  0.0f,   0.0f, 0.9f),(1.0f / 0.9f), Color[nTest],         0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(256.0f, 256.0f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,255), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  0.0f, 256.0f, 0.9f),(1.0f / 0.9f), Color[nTest],         0, 0.0f,1.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  0.0f,   0.0f, 0.9f),(1.0f / 0.9f), Color[nTest],         0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(256.0f,   0.0f, 0.9f),(1.0f / 0.9f), Color[nTest],         0, 1.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(256.0f, 256.0f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,255), 0, 1.0f,1.0f);
}

