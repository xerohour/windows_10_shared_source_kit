#define D3D_OVERLOADS
#include <d3d8.h>
#include "types.h"
#include "ZBuffer.h"

//************************************************************************
// ZNeverOn Test functions

CZNeverOnTest::CZNeverOnTest()
{
	m_szTestName = "Z Buffer NeverOn";
	m_szCommandKey = "NeverOn";
		
	bEnable = true;
}

//************************************************************************
// ZNeverOff Test functions

CZNeverOffTest::CZNeverOffTest()
{
	m_szTestName = "Z Buffer NeverOff";
	m_szCommandKey = "NeverOff";
	
	bEnable = true;

    // Disable ZBuffers
    m_Options.D3DOptions.bZBuffer = false;
    m_Options.ModeOptions.ZBufferFmt = FMT_ANY;
}

bool CZNeverTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Let's build a triangle.
	DrawTriangle();

	// Set the Z Compare mode to never
	SetRenderState(D3DRS_ZFUNC, (DWORD)D3DCMP_NEVER);

	// Set the appropriate renderstate
	switch (uTestNum)
	{
		case (1):
            sprintf(msgString, "%sZEnable: False, Green Triangle Visible: Yes", msgString);
			SetRenderState(D3DRS_ZENABLE, (DWORD)D3DZB_FALSE);
			break;
		case (2):
			if (m_Options.D3DOptions.bZBuffer)
				sprintf(msgString, "%sZEnable: True, Green Triangle Visible: No", msgString);
			else
				sprintf(msgString, "%sZEnable: True, Green Triangle Visible: Yes", msgString);

			SetRenderState(D3DRS_ZENABLE, (DWORD)D3DZB_TRUE);
            break;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    return true;
}

//************************************************************************
// Internal API:    DrawTriangle
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CZNeverTest::DrawTriangle(void)
{
    D3DVECTOR v0 = {115.0f,  40.0f, 0.5f};
    D3DVECTOR v1 = {205.0f, 225.0f, 0.5f};
    D3DVECTOR v2 = { 25.0f, 225.0f, 0.5f};

    // Green Triangle
	VertexList[dwVertexCount++] = D3DTLVERTEX(v0, (1.0f / 0.5f), RGBA_MAKE(0,255,0,255), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(v1, (1.0f / 0.5f), RGBA_MAKE(0,255,0,255), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(v2, (1.0f / 0.5f), RGBA_MAKE(0,255,0,255), 0, 0.0f,1.0f);
}
