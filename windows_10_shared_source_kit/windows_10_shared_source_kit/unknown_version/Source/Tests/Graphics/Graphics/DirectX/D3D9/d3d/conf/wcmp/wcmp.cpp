#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "WCmp.h"

// Global Test and App instance
CD3DWindowFramework	App;
CWNeverTest			WNever;
CWLessTest			WLess;
CWEqualTest			WEqual;
CWLessEqualTest		WLessEqual;
CWGreaterTest		WGreater;
CWNotEqualTest		WNotEqual;
CWGreaterEqualTest	WGreaterEqual;
CWAlwaysTest		WAlways;

CWCmpTest::CWCmpTest()
{
    // Init framework options
    SetTestType(TESTTYPE_CONF);
	m_pFramework->GetImageCmp()->SetMinPixels(0);
    m_Options.ModeOptions.ZBufferFmt = FMT_ALL;

	// Force test to cycle desktop resolutions 16bpp/32bpp
	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ALL;
}

CWCmpTest::~CWCmpTest()
{
}

bool CWCmpTest::SetDefaultMatrices(void)
{
	D3DXMATRIX Matrix;

	// Setup a dummy Projection Matrix for W buffering
    D3DXMatrixPerspectiveFovLH(&Matrix, pi/3.0f, 1, 1.0f, 100.0f);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

bool CWCmpTest::CapsCheck(void)
{
	// Check the caps we are interested in
	DWORD dwRasterCaps = m_pSrcDevice->GetCaps()->dwRasterCaps;
	DWORD dwZCmpCaps = m_pSrcDevice->GetCaps()->dwZCmpCaps;

	// Make sure the driver supports W Buffering.
	if (!(dwRasterCaps & D3DPRASTERCAPS_WBUFFER))
	{
		WriteToLog("Capability not supported: W Buffer.\n");
		return false;
	}

	// Check the appropriate Compare caps
	if (!(dwZCmpCaps & dwWCmpCap))
	{
		WriteToLog("Device capability not supported: %s.\n",m_szTestName);
		return false;
	}

	return true;
}

UINT CWCmpTest::TestInitialize(void)
{
	// Setup the Test range
	if ((dwWCmpCap == D3DPCMPCAPS_EQUAL) || (dwWCmpCap == D3DPCMPCAPS_NOTEQUAL))
		SetTestRange(1,MAX_TESTS-2);
	else
		SetTestRange(1,MAX_TESTS);

	// Init some variables
    TCHAR szBuffer[80];
	msgString[0] = '\0';

	// Log the current W Buffer format
    FmtToString(FORMAT_ZBUFFER,&m_pCommonZBufferFormats[m_pCurrentMode->nZBufferFormat],szBuffer);
	sprintf(msgString, "%sW Buffer Format: %s.\n",msgString,szBuffer);

    WriteToLog(msgString);

	return D3DTESTINIT_RUN;
}

bool CWCmpTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

bool CWCmpTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Let's build some triangles.
	switch (dwWCmpCap)
	{
		case D3DPCMPCAPS_NEVER:
			AlwaysTests((int)uTestNum);
			break;
		case D3DPCMPCAPS_LESS:
			LessTests((int)uTestNum);
			break;
		case D3DPCMPCAPS_EQUAL:
			EqualTests((int)uTestNum);
			break;
		case D3DPCMPCAPS_LESSEQUAL:
			LessTests((int)uTestNum);
			break;
		case D3DPCMPCAPS_GREATER:
			GreaterTests((int)uTestNum);
			break;
		case D3DPCMPCAPS_NOTEQUAL:
			EqualTests((int)uTestNum);
			break;
		case D3DPCMPCAPS_GREATEREQUAL:
			GreaterTests((int)uTestNum);
			break;
		case D3DPCMPCAPS_ALWAYS:
			AlwaysTests((int)uTestNum);
			break;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    return true;
}

void CWCmpTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}
