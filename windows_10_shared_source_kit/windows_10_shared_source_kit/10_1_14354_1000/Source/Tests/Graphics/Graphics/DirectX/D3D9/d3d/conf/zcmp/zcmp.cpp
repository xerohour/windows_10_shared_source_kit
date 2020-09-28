#define D3D_OVERLOADS
#include <d3d.h>
#include "ZCmp.h"

// Global Test and App instance
CD3DWindowFramework	App;
CZNeverTest			ZNever;
CZLessTest			ZLess;
CZEqualTest			ZEqual;
CZLessEqualTest		ZLessEqual;
CZGreaterTest		ZGreater;
CZNotEqualTest		ZNotEqual;
CZGreaterEqualTest	ZGreaterEqual;
CZAlwaysTest		ZAlways;

CZCmpTest::CZCmpTest()
{
    // Init framework options
    SetTestType(TESTTYPE_CONF);
	m_pFramework->GetImageCmp()->SetMinPixels(0);

#ifdef FAST_TEST
	m_Options.ModeOptions.ZBufferFmt = FMT_D16;
#else
	m_Options.ModeOptions.ZBufferFmt = FMT_ALL;
#endif

	// Force test to cycle desktop resolutions 16bpp/32bpp
	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ALL;

	// Init variables
	bCapAbort = false;
}

CZCmpTest::~CZCmpTest()
{
}

bool CZCmpTest::CapsCheck(void)
{
	// Check the caps we are interested in
	DWORD dwZCmpCaps = m_pSrcDevice->GetCaps()->dwZCmpCaps;

	// Check the appropriate Z Compare caps
	if (!(dwZCmpCaps & dwZCmpCap))
	{
		WriteToLog("Device capability not supported: %s.\n",m_szTestName);

        // All ZCmp caps are required
		bCapAbort = true;
	}

	return true;
}

UINT CZCmpTest::TestInitialize(void)
{
    // Setup the Test range
	if ((dwZCmpCap == D3DPCMPCAPS_EQUAL) || (dwZCmpCap == D3DPCMPCAPS_NOTEQUAL))
		SetTestRange(1,MAX_TESTS-2);
	else
		SetTestRange(1,MAX_TESTS);
 
	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	if (KeySet("ZWriteOff"))
		SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, (DWORD)FALSE);

	if (KeySet("AlphaTest"))
		SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, (DWORD) TRUE);

	// Init some variables
    TCHAR szBuffer[80];
	msgString[0] = '\0';

	// Log the current Z Buffer format
    FmtToString(FORMAT_ZBUFFER,&m_pCommonZBufferFormats[m_pCurrentMode->nZBufferFormat],szBuffer);
	sprintf(msgString, "%sZ Buffer Format: %s.\n",msgString,szBuffer);

    WriteToLog(msgString);

	return D3DTESTINIT_RUN;
}

bool CZCmpTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

bool CZCmpTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Let's build some triangles.
	switch (dwZCmpCap)
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

void CZCmpTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}
