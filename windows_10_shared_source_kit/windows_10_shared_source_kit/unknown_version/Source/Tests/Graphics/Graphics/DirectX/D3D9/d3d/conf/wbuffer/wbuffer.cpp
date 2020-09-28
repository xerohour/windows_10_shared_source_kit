#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "WBuffer.h"

#define FVF_LVERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 )

// Global Test and App instance
CD3DWindowFramework	App;
CWBufferTest	WBuffer;

CWBufferTest::CWBufferTest()
{
	m_szTestName = "W Buffer";
	m_szCommandKey = "WBuffer";

    // Init framework options
    SetTestType(TESTTYPE_CONF);
    m_Options.ModeOptions.ZBufferFmt = FMT_ALL;

	// Force test to use relevant DirectX versions for WHQL
	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_7;
}

CWBufferTest::~CWBufferTest()
{
}

bool CWBufferTest::CapsCheck(void)
{
	// Check the caps we are interested in
	DWORD dwRasterCaps = m_pSrcDevice->GetCaps()->dwRasterCaps;
 
	// Make sure the driver supports W Buffering.
	if (!(dwRasterCaps & D3DPRASTERCAPS_WBUFFER))
	{
		WriteToLog("Capability not supported: W Buffer.\n");
		return false;
	}

	return true;
}

bool CWBufferTest::SetDefaultRenderStates(void)
{
	// Turn off Lighting for DX7
	if (m_dwVersion >= 0x0700)
		SetRenderState(D3DRS_LIGHTING, (DWORD)FALSE);

	// Make sure that we are using the W Buffer
	SetRenderState(D3DRS_ZENABLE, (DWORD)D3DZB_USEW);
	return true;
}

bool CWBufferTest::SetDefaultMatrices(void)
{
	D3DXMATRIX Matrix;

	// Setup a Projection Matrix for W buffering
    D3DXMatrixPerspectiveFovLH(&Matrix, pi/3.0f, 1, 0.01f, 100.0f);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

	// Use identity matrix for viewport
    D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_VIEW, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ViewMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

UINT CWBufferTest::TestInitialize(void)
{
	// Setup the Test range
	SetTestRange(1,MAX_TESTS);

	// Initialize the Plane
	PlaneList[0] = LVERTEX(D3DXVECTOR3(-4.0f, -4.0f, +1.0f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
	PlaneList[1] = LVERTEX(D3DXVECTOR3(-4.0f, +4.0f, +1.0f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
	PlaneList[2] = LVERTEX(D3DXVECTOR3(+4.0f, +4.0f, +1.0f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
	PlaneList[3] = LVERTEX(D3DXVECTOR3(+4.0f, -4.0f, +1.0f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);

	// Initialize the Waffle
	int i;
	for (i=0; i < 40; i+=8)
	{
		WaffleList[0][i]   = LVERTEX(D3DXVECTOR3(-5.0f + (0.25f * i), -5.0f, +1.0f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
		WaffleList[0][i+1] = LVERTEX(D3DXVECTOR3(-5.0f + (0.25f * i), +5.0f, +1.0f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
		WaffleList[0][i+2] = LVERTEX(D3DXVECTOR3(-4.5f + (0.25f * i), -5.0f,  0.0f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
		WaffleList[0][i+3] = LVERTEX(D3DXVECTOR3(-4.5f + (0.25f * i), +5.0f,  0.0f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
		WaffleList[0][i+4] = LVERTEX(D3DXVECTOR3(-4.0f + (0.25f * i), -5.0f, -1.0f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
		WaffleList[0][i+5] = LVERTEX(D3DXVECTOR3(-4.0f + (0.25f * i), +5.0f, -1.0f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
		WaffleList[0][i+6] = LVERTEX(D3DXVECTOR3(-3.5f + (0.25f * i), -5.0f,  0.0f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
		WaffleList[0][i+7] = LVERTEX(D3DXVECTOR3(-3.5f + (0.25f * i), +5.0f,  0.0f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);

		WaffleList[1][i]   = LVERTEX(D3DXVECTOR3(-5.0f + (0.25f * i), -5.0f, -1.0f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
		WaffleList[1][i+1] = LVERTEX(D3DXVECTOR3(-5.0f + (0.25f * i), +5.0f, -1.0f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
		WaffleList[1][i+2] = LVERTEX(D3DXVECTOR3(-4.5f + (0.25f * i), -5.0f,  0.0f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
		WaffleList[1][i+3] = LVERTEX(D3DXVECTOR3(-4.5f + (0.25f * i), +5.0f,  0.0f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
		WaffleList[1][i+4] = LVERTEX(D3DXVECTOR3(-4.0f + (0.25f * i), -5.0f, +1.0f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
		WaffleList[1][i+5] = LVERTEX(D3DXVECTOR3(-4.0f + (0.25f * i), +5.0f, +1.0f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
		WaffleList[1][i+6] = LVERTEX(D3DXVECTOR3(-3.5f + (0.25f * i), -5.0f,  0.0f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
		WaffleList[1][i+7] = LVERTEX(D3DXVECTOR3(-3.5f + (0.25f * i), +5.0f,  0.0f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
	}

	WaffleList[0][i]   = LVERTEX(D3DXVECTOR3(-5.0f + (0.25f * i), -5.0f, +1.0f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
	WaffleList[0][i+1] = LVERTEX(D3DXVECTOR3(-5.0f + (0.25f * i), +5.0f, +1.0f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
	WaffleList[1][i]   = LVERTEX(D3DXVECTOR3(-5.0f + (0.25f * i), -5.0f, -1.0f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
	WaffleList[1][i+1] = LVERTEX(D3DXVECTOR3(-5.0f + (0.25f * i), +5.0f, -1.0f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);

	// Init some variables
    TCHAR szBuffer[80];
	msgString[0] = '\0';

	// Log the current W Buffer format
    FmtToString(FORMAT_ZBUFFER,&m_pCommonZBufferFormats[m_pCurrentMode->nZBufferFormat],szBuffer);
	sprintf(msgString, "%sW Buffer Format: %s.\n",msgString,szBuffer);

    WriteToLog(msgString);

	return D3DTESTINIT_RUN;
}

bool CWBufferTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

bool CWBufferTest::ExecuteTest(UINT uTestNum)
{
	float		fRotate;
    D3DXMATRIX  Matrix, RotMatrix, TransMatrix;
	msgString[0] = '\0';

	// Calculate current rotation
	fRotate = 30.5f + (uTestNum - 1);

	if (fRotate > 360.0f ) 
		fRotate -= 360.0f;

	// Convert to radians
	fRotate = (fRotate * 2 * pi) / 360.0f;

	// Update the logfile
	sprintf(msgString,"Depth: %0.2f, Rotation: %0.2f",(float)uTestNum, fRotate);

	// Setup the world matrix
	D3DXMatrixScaling(&Matrix, 0.5f, 0.5f, 1.0f);
    D3DXMatrixRotationZ(&RotMatrix, -fRotate);
    D3DXMatrixMultiply(&Matrix, &RotMatrix, &Matrix);
    D3DXMatrixTranslation(&TransMatrix, 0.0f, 0.0f, (float)uTestNum + 0.01f);
    D3DXMatrixMultiply(&Matrix, &TransMatrix, &Matrix);
    if (!SetTransform(D3DTS_WORLD, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		TestFinish(D3DTESTFINISH_ABORT);
        return false;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    return true;
}

void CWBufferTest::SceneRefresh(void)
{
 	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLESTRIP,FVF_LVERTEX,WaffleList[0],42,NULL,0,0);
		RenderPrimitive(D3DPT_TRIANGLESTRIP,FVF_LVERTEX,WaffleList[1],42,NULL,0,0);
		RenderPrimitive(D3DPT_TRIANGLEFAN,FVF_LVERTEX,PlaneList,4,NULL,0,0);

		EndScene();
	}
}


