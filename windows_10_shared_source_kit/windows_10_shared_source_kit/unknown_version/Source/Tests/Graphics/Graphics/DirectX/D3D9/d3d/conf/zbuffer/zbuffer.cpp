#define D3D_OVERLOADS
#include <d3d.h>
#include "ZBuffer.h"

// Global Test and App instance
CD3DWindowFramework	App;
CZVerifyTest		ZVerify;
CZEnableTest		ZEnable;
CZNeverOnTest		ZNeverOn;
CZNeverOffTest		ZNeverOff;
CZNoDiscardTest		ZNoDiscard;
CZDiscardTest		ZDiscard;
CZPrintTest			ZPrint;

CZBufferTest::CZBufferTest()
{
    // Init framework options
    SetTestType(TESTTYPE_CONF);
	m_pFramework->GetImageCmp()->SetMinPixels(0);
    m_Options.ModeOptions.ZBufferFmt = FMT_ALL;

	// Force test to cycle desktop resolutions 16bpp/32bpp
	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ALL;

	// Force test to use relevant DirectX versions for WHQL
	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_8_OR_81 | DXVERSION_7;

    // Init variables
	bEnable = false;
}

CZBufferTest::~CZBufferTest()
{
}

UINT CZBufferTest::TestInitialize(void)
{
	// Setup the Test range
	if (bEnable)
		SetTestRange(1, MAX_TESTS + 1);
	else
		SetTestRange(1, MAX_TESTS);

	// Init some variables
    TCHAR szBuffer[80];
	msgString[0] = '\0';

	// Log the current Z Buffer format
	if (m_Options.D3DOptions.bZBuffer)
	{
		FmtToString(FORMAT_ZBUFFER,&m_pCommonZBufferFormats[m_pCurrentMode->nZBufferFormat],szBuffer);
		sprintf(msgString, "%sZ Buffer Format: %s.\n",msgString,szBuffer);
	}
	else
		sprintf(msgString, "%sZ Buffer Disabled.\n",msgString);

	WriteToLog(msgString);

	return D3DTESTINIT_RUN;
}

bool CZBufferTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

void CZBufferTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

