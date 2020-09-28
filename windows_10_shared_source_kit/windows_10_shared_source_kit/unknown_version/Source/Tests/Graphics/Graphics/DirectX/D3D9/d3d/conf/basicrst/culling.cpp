#define D3D_OVERLOADS
#include <d3d.h>
#include "BasicRst.h"

//************************************************************************
// Culling Test functions

CCullingTest::CCullingTest()
{
	m_szTestName = "BasicRst Culling";
	m_szCommandKey = "Culling";
}

CCullingTest::~CCullingTest()
{
}

UINT CCullingTest::TestInitialize(void)
{
	// Setup the Test range
	SetTestRange(1,CULLING_TESTS);

	// Check the caps we are interested in
	DWORD dwPrimitiveMiscCaps = m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps;

	// Check the appropriate Culling caps
	if (!(dwPrimitiveMiscCaps & D3DPMISCCAPS_CULLNONE))
	{
		WriteToLog("Device capability not supported: Cull None.\n");
		return D3DTESTINIT_SKIPALL;
	}

	if (!(dwPrimitiveMiscCaps & D3DPMISCCAPS_CULLCW))
	{
		WriteToLog("Device capability not supported: Cull CW.\n");
		return D3DTESTINIT_SKIPALL;
	}

	if (!(dwPrimitiveMiscCaps & D3DPMISCCAPS_CULLCCW))
	{
		WriteToLog("Device capability not supported: Cull CCW.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Set compare function to require 85%
	m_pFramework->GetImageCmp()->SetPassRequired(0.85f);

	return D3DTESTINIT_RUN;
}

bool CCullingTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Let's build some triangles.
	DrawCullTests();

	// Set the appropriate renderstate
	switch (uTestNum)
	{
		case (1):
			sprintf(msgString, "%sNone, Visible: 24 triangles (All)", msgString);
			SetRenderState(D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_NONE);
			break;
		case (2):
			sprintf(msgString, "%sClockwise, Visible: 12 triangles (Right half)", msgString);
			SetRenderState(D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_CW);
			break;
		case (3):
			sprintf(msgString, "%sCounterClockwise, Visible: 12 triangles (Left half)", msgString);
			SetRenderState(D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_CCW);
			break;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    return true;
}


//************************************************************************
// Internal API:    DrawCullTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CCullingTest::DrawCullTests(void)
{
	int nRow, delta;

	// Vertices for culling triangles
	float x1 = 5.0f;
	float x2[] = {5.0f, 5.0f, 25.0f, 25.0f};
	float x3 = 45.0f;
	float y1[] = {45.0f, 105.0f, 165.0f, 205.0f};
	float y2[] = { 5.0f,  65.0f, 125.0f, 185.0f};
	float y3[] = {45.0f,  85.0f, 165.0f, 225.0f};

	// Color of triangles
	D3DCOLOR Color[] = { RGBA_MAKE(255,0,0,255),		// Red
						 RGBA_MAKE(0,255,0,255),		// Green
						 RGBA_MAKE(0,0,255,255),		// Blue
						 RGBA_MAKE(255,255,0,255) };	// Yellow


	// Build the 24 triangles for the culling tests
	for (nRow=0; nRow<4; nRow++)
	{
		// Column 0
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x1,      y1[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x2[nRow],y2[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 1.0f,1.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x3,      y3[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,1.0f);
	}

	delta = 50;
	for (nRow=0; nRow<4; nRow++)
	{
		// Column 1
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x2[nRow]+delta,y2[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x3+delta,      y3[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 1.0f,1.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x1+delta,      y1[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,1.0f);
	}

	delta = 2*50;
	for (nRow=0; nRow<4; nRow++)
	{
		// Column 2
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x3+delta,      y3[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x1+delta,      y1[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 1.0f,1.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x2[nRow]+delta,y2[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,1.0f);
	}

	delta = 3*50;
	for (nRow=0; nRow<4; nRow++)
	{
		// Column 3
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x3+delta,      y3[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x2[nRow]+delta,y2[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 1.0f,1.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x1+delta,      y1[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,1.0f);
	}

	delta = 4*50;
	for (nRow=0; nRow<4; nRow++)
	{
		// Column 4
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x2[nRow]+delta,y2[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x1+delta,      y1[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 1.0f,1.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x3+delta,      y3[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,1.0f);
	}

	delta = 5*50;
	for (nRow=0; nRow<4; nRow++)
	{
		// Column 5
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x1+delta,      y1[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x3+delta,      y3[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 1.0f,1.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x2[nRow]+delta,y2[nRow],0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,1.0f);
	}
}
