#define D3D_OVERLOADS
#include <d3d.h>
#include "AlphaCmp.h"

// Global Test and App instance
CD3DWindowFramework		App;
CAlphaNeverTest			AlphaNever;
CAlphaLessTest			AlphaLess;
CAlphaEqualTest			AlphaEqual;
CAlphaLessEqualTest		AlphaLessEqual;
CAlphaGreaterTest		AlphaGreater;
CAlphaNotEqualTest		AlphaNotEqual;
CAlphaGreaterEqualTest	AlphaGreaterEqual;
CAlphaAlwaysTest		AlphaAlways;

CAlphaCmpTest::CAlphaCmpTest()
{
    // Init framework options
    SetTestType(TESTTYPE_CONF);
    m_pFramework->GetImageCmp()->SetMinPixels(0);

	if (KeySet("Stencil"))
	{
		m_Options.ModeOptions.ZBufferFmt = FMT_ALL;
		m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ALL;
	}

    // Init to flat shading
	bGouraud = false;

	// Init Alpha value array
	nAlphaValue[0] = 1;
	nAlphaValue[1] = 85;
	nAlphaValue[2] = 128;
	nAlphaValue[3] = 170;
	nAlphaValue[4] = 254;
}

CAlphaCmpTest::~CAlphaCmpTest()
{
}

bool CAlphaCmpTest::CapsCheck(void)
{
	// Check the caps we are interested in
	DWORD dwAlphaCmpCaps = m_pSrcDevice->GetCaps()->dwAlphaCmpCaps;

	// Check to see if Alpha testing is supported at all
	if ((dwAlphaCmpCaps == D3DPCMPCAPS_NEVER) || (dwAlphaCmpCaps == D3DPCMPCAPS_ALWAYS))
	{
		WriteToLog("Alpha testing is not supported.\n");
		return false;
	}

	// Check the appropriate Alpha Compare cap
	if (!(dwAlphaCmpCaps & dwAlphaCmpCap))
	{
		WriteToLog("Device capability not supported: %s.\n",m_szTestName);
		return false;
	}

	// On Dx9, need to check for UsagePostPixelShader_Blending on the backbuffer
	if (m_dwVersion >= 0x0900)
	{
		HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
											 USAGE_QUERY_POSTPIXELSHADER_BLENDING, RTYPE_TEXTURE, m_pCommonBackBufferFormats[m_pCurrentMode->nBackBufferFormat]);
		if (FAILED(Result))
		{
			WriteToLog("Unable to Alpha test this backbuffer. CheckDeviceFormat(PostPixelShader_Blending) failed: %s.\n",m_pD3D->HResultToString(Result));
			return false;
		}
	}

	return true;
}

UINT CAlphaCmpTest::TestInitialize(void)
{
	// Are we using Gouraud?
    if (KeySet("Gouraud"))
		bGouraud = true;

	// Setup the Test range
	if (bGouraud)
		SetTestRange(1,MAX_TESTS);
	else
		SetTestRange(1,MAX_TESTS * 3);

	return D3DTESTINIT_RUN;
}

bool CAlphaCmpTest::ExecuteTest(UINT uTestNum)
{
	int nTest, nRefValue;

	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

	if (bGouraud)
	{
		nTest = uTestNum - 1;
		nRefValue = nAlphaValue[nTest];

		if (nRefValue == 1)
			nRefValue = 0;
		if (nRefValue == 254)
			nRefValue = 255;
	}
	else
	{
		nTest = (uTestNum - 1) / 3;
		nRefValue = nAlphaValue[nTest] + ((uTestNum - 1) % 3) - 1;
	}

	// Set the Reference Alpha Value
	SetRenderState(D3DRENDERSTATE_ALPHAREF, (DWORD)nRefValue);

	// Coverage Hack
	if (KeySet("Stencil"))
		SetRenderState(D3DRENDERSTATE_STENCILENABLE, (DWORD)TRUE);

	// Let's build some triangles
	DrawTriangle(nAlphaValue[nTest]);

	// Update the log and status window
	if (bGouraud)
//		sprintf(msgString, "%sAlphaRef: %d, Green Triangle: top %d, bottom %d",msgString,nRefValue,nAlphaValue[nTest]-1,nAlphaValue[nTest]+1);
		sprintf(msgString, "%sAlphaRef: %d, Green Triangle: top 0, bottom 255",msgString,nRefValue);
	else
		sprintf(msgString, "%sAlphaRef: %d, Green Triangle: %d",msgString,nRefValue,nAlphaValue[nTest]);

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    return true;
}

void CAlphaCmpTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}
