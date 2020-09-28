#define D3D_OVERLOADS
#include <d3d.h>
#include "BasicRst.h"

#define FVF_VERTEX ( D3DFVF_XYZRHW | D3DFVF_TEX1 )

//************************************************************************
// NoDiffuse Test functions

CNoDiffuseTest::CNoDiffuseTest()
{
	m_szTestName = "BasicRst NoDiffuse";
	m_szCommandKey = "NoDiffuse";
}

CNoDiffuseTest::~CNoDiffuseTest()
{
}

UINT CNoDiffuseTest::TestInitialize(void)
{
    PRENDERPRIMITIVEAPI m_pFullAPIList;
    DWORD               m_dwFullAPIListSize;

    if (m_dwVersion >= 0x0800)
    {
        m_pFullAPIList = m_pRPFullDX8APIList;
        m_dwFullAPIListSize = m_dwRPFullDX8APIListSize;
    }
    else if (m_dwVersion >= 0x0700)
    {
        m_pFullAPIList = m_pRPFullDX7APIList;
        m_dwFullAPIListSize = m_dwRPFullDX7APIListSize;
    }
    else
    {
        m_pFullAPIList = m_pRPFullDX6APIList;
        m_dwFullAPIListSize = m_dwRPFullDX6APIListSize;
    }

    // Setup the Test range
	SetTestRange(1, m_dwFullAPIListSize);

    // Tell RenderPrimitive to cycle through all APIs
    SetAPI(m_pFullAPIList, m_dwFullAPIListSize, 1);

    // Set compare function to require 85%
	m_pFramework->GetImageCmp()->SetPassRequired(0.85f);

	return D3DTESTINIT_RUN;
}

bool CNoDiffuseTest::ExecuteTest(UINT uTestNum)
{
    DWORD dwAPI;

	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Let's build some triangles.
	DrawNonDiffuseGrid();

	// Get the current API
    dwAPI = m_pSrcDevice->RemapAPI(GetAPI(), D3DPT_TRIANGLELIST, FVF_VERTEX);

    // Tell log which API we are using
    switch (dwAPI)
    {
        case RP_BE:             sprintf(msgString, "%sFVF: 0x%08x, API: Begin/Vertex/End", msgString, FVF_VERTEX);
                                break;
        case RP_BIE:            sprintf(msgString, "%sFVF: 0x%08x, API: BeginIndexed/Index/End", msgString, FVF_VERTEX);
                                break;
        case RP_DP:             sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitive", msgString, FVF_VERTEX);
                                break;
        case RP_DIP:            sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitive", msgString, FVF_VERTEX);
                                break;
        case RP_DPS:            sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveStrided", msgString, FVF_VERTEX);
                                break;
        case RP_DIPS:           sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveStrided", msgString, FVF_VERTEX);
                                break;
        case RP_DPVB:           sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB", msgString, FVF_VERTEX);
                                break;
        case RP_DIPVB:          sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB", msgString, FVF_VERTEX);
                                break;
        case RP_DPVB_PV:        sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB after ProcessVertices", msgString, FVF_VERTEX);
                                break;
        case RP_DIPVB_PV:       sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB after ProcessVertices", msgString, FVF_VERTEX);
                                break;
        case RP_DPVB_OPT:       sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB with Optimized VB", msgString, FVF_VERTEX);
                                break;
        case RP_DIPVB_OPT:      sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB with Optimized VB", msgString, FVF_VERTEX);
                                break;
        case RP_DPVB_OPT_PV:    sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB after ProcessVert with Optimized VB", msgString, FVF_VERTEX);
                                break;
        case RP_DIPVB_OPT_PV:   sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB after ProcessVert with Optimized VB", msgString, FVF_VERTEX);
                                break;
        case RP_DPVB_PVS:       sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB after ProcessVerticesStrided", msgString, FVF_VERTEX);
                                break;
        case RP_DIPVB_PVS:      sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB after ProcessVerticesStrided", msgString, FVF_VERTEX);
                                break;
        default:                sprintf(msgString, "%sError: Unknown API", msgString);
                                break;
    }

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    return true;
}

void CNoDiffuseTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,FVF_VERTEX,NoDiffuseList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

//************************************************************************
// Internal API:    DrawNonDiffuseGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CNoDiffuseTest::DrawNonDiffuseGrid(void)
{
	// Use standard ((0,0)(1,0)(0,1)(1,1)) mapping
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(D3DVECTOR(  9.5f,   9.5f, 0.9f), (1.0f / 0.9f), 0.0f,0.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(D3DVECTOR(265.5f, 265.5f, 0.9f), (1.0f / 0.9f), 1.0f,1.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(D3DVECTOR(  9.5f, 265.5f, 0.9f), (1.0f / 0.9f), 0.0f,1.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(D3DVECTOR(  9.5f,   9.5f, 0.9f), (1.0f / 0.9f), 0.0f,0.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(D3DVECTOR(265.5f,   9.5f, 0.9f), (1.0f / 0.9f), 1.0f,0.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(D3DVECTOR(265.5f, 265.5f, 0.9f), (1.0f / 0.9f), 1.0f,1.0f);
}

