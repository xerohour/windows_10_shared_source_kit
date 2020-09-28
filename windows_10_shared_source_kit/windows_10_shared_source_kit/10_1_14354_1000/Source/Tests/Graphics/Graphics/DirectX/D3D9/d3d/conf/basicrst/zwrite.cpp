#define D3D_OVERLOADS
#include <d3d.h>
#include "BasicRst.h"

//************************************************************************
// ZWrite Test functions

CZWriteTest::CZWriteTest()
{
	m_szTestName = "BasicRst ZWrite";
	m_szCommandKey = "ZWrite";
}

CZWriteTest::~CZWriteTest()
{
}

UINT CZWriteTest::TestInitialize(void)
{
	// Setup the Test range
	SetTestRange(1,ZWRITE_TESTS);

    // Set compare function to require 85%
	m_pFramework->GetImageCmp()->SetPassRequired(0.85f);

	return D3DTESTINIT_RUN;
}

bool CZWriteTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Let's build some triangles.
	DrawZWriteTests();

	// Set the appropriate renderstate
	switch (uTestNum)
	{
		case (1):
			sprintf(msgString, "%sZEnable: True, ZWriteEnable: True, Visible: Green on Blue", msgString);
			SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_TRUE);
			SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, (DWORD)TRUE);
			break;
		case (2):
			sprintf(msgString, "%sZEnable: True, ZWriteEnable: False, Visible: Blue on Green", msgString);
			SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_TRUE);
			SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, (DWORD)FALSE);
			break;
		case (3):
			sprintf(msgString, "%sZEnable: False, ZWriteEnable: True, Visible: Blue on Green", msgString);
			SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_FALSE);
			SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, (DWORD)TRUE);
			break;
		case (4):
			sprintf(msgString, "%sZEnable: False, ZWriteEnable: False, Visible: Blue on Green", msgString);
			SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_FALSE);
			SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, (DWORD)FALSE);
			break;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    return true;
}

//************************************************************************
// Internal API:    DrawZWriteTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CZWriteTest::DrawZWriteTests(void)
{
	// Green Triangle
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(115.0f, 40.0f,0.5f),(1.0f / 0.5f), RGBA_MAKE(0,255,0,255), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(205.0f,225.0f,0.5f),(1.0f / 0.5f), RGBA_MAKE(0,255,0,255), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 25.0f,225.0f,0.5f),(1.0f / 0.5f), RGBA_MAKE(0,255,0,255), 0, 0.0f,1.0f);

	// Blue Triangle
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(130.0f, 40.0f,0.7f),(1.0f / 0.7f), RGBA_MAKE(0,0,255,255), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(225.0f,240.0f,0.7f),(1.0f / 0.7f), RGBA_MAKE(0,0,255,255), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 40.0f,240.0f,0.7f),(1.0f / 0.7f), RGBA_MAKE(0,0,255,255), 0, 0.0f,1.0f);
}
