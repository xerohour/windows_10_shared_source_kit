#define D3D_OVERLOADS
#include <d3d8.h>
#include "types.h"
#include "ZBuffer.h"

//************************************************************************
// ZEnable Test functions

CZEnableTest::CZEnableTest()
{
	m_szTestName = "Z Buffer Enable";
	m_szCommandKey = "Enable";

	bEnable = true;
}

bool CZEnableTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Let's build a triangle.
	DrawTriangle();

	// Set the appropriate renderstate
	switch (uTestNum)
	{
		case (1):
            // Turn off Ref compares (Manbug #26983 not being fixed on Dx7)
            if (m_dwVersion == 0x0700)
				SetCompareSrcRef(false);

            // On Dx8, check for TL clip cap
            if (m_dwVersion >= 0x0800)
            {
            	DWORD dwPrimitiveMiscCaps = m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps;

	            if (!(dwPrimitiveMiscCaps & D3DPMISCCAPS_CLIPTLVERTS))
	            {
		            WriteToLog("Device capability not supported: Misc ClipTLVerts.\n");  
                    SkipTests(1);
		            return false;
	            }
            }

            sprintf(msgString, "%sZEnable: True, Green Triangle Visible: No", msgString);
			SetRenderState(D3DRS_ZENABLE, (DWORD)D3DZB_TRUE);
			break;
		case (2):
            // Turn ref compares back on
            if (m_dwVersion == 0x0700)
				SetCompareSrcRef(true);

			sprintf(msgString, "%sZEnable: False, Green Triangle Visible: Yes", msgString);
			SetRenderState(D3DRS_ZENABLE, (DWORD)D3DZB_FALSE);
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

void CZEnableTest::DrawTriangle(void)
{
    D3DVECTOR v0 = {115.0f,  40.0f, 500.0f};
    D3DVECTOR v1 = {205.0f, 225.0f, 500.0f};
    D3DVECTOR v2 = { 25.0f, 225.0f, 500.0f};

    // Green Triangle
	VertexList[dwVertexCount++] = D3DTLVERTEX(v0, (1.0f / 500.0f), RGBA_MAKE(0,255,0,255), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(v1, (1.0f / 500.0f), RGBA_MAKE(0,255,0,255), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(v2, (1.0f / 500.0f), RGBA_MAKE(0,255,0,255), 0, 0.0f,1.0f);
}
