#define D3D_OVERLOADS
#include <d3d9.h>
#include <math.h>
#include "types.h"
#include "AlphaBld.h"

// Global App instance
CD3DWindowFramework		App;

#ifdef FAST_TEST
CAlphaBldTest AlphaBldTest;
#else
// Global False Test instance
CFalseTest				False;

// Global Source:Zero Test instances
CZeroZeroTest			ZeroZero;
CZeroOneTest			ZeroOne;
CZeroSrcColorTest		ZeroSrcColor;
CZeroInvSrcColorTest	ZeroInvSrcColor;
CZeroSrcAlphaTest		ZeroSrcAlpha;
CZeroInvSrcAlphaTest	ZeroInvSrcAlpha;
CZeroDestAlphaTest		ZeroDestAlpha;
CZeroInvDestAlphaTest	ZeroInvDestAlpha;
CZeroDestColorTest		ZeroDestColor;
CZeroInvDestColorTest	ZeroInvDestColor;
CZeroSrcAlphaSatTest	ZeroSrcAlphaSat;
CZeroBlendFactorTest	ZeroBlendFactor;
CZeroInvBlendFactorTest	ZeroInvBlendFactor;

// Global Source:One Test instances
COneZeroTest			OneZero;
COneOneTest				OneOne;
COneSrcColorTest		OneSrcColor;
COneInvSrcColorTest		OneInvSrcColor;
COneSrcAlphaTest		OneSrcAlpha;
COneInvSrcAlphaTest		OneInvSrcAlpha;
COneDestAlphaTest		OneDestAlpha;
COneInvDestAlphaTest	OneInvDestAlpha;
COneDestColorTest		OneDestColor;
COneInvDestColorTest	OneInvDestColor;
COneSrcAlphaSatTest		OneSrcAlphaSat;
COneBlendFactorTest		OneBlendFactor;
COneInvBlendFactorTest	OneInvBlendFactor;

// Global Source:SrcColor Test instances
CSrcColorZeroTest			SrcColorZero;
CSrcColorOneTest			SrcColorOne;
CSrcColorSrcColorTest		SrcColorSrcColor;
CSrcColorInvSrcColorTest	SrcColorInvSrcColor;
CSrcColorSrcAlphaTest		SrcColorSrcAlpha;
CSrcColorInvSrcAlphaTest	SrcColorInvSrcAlpha;
CSrcColorDestAlphaTest		SrcColorDestAlpha;
CSrcColorInvDestAlphaTest	SrcColorInvDestAlpha;
CSrcColorDestColorTest		SrcColorDestColor;
CSrcColorInvDestColorTest	SrcColorInvDestColor;
CSrcColorSrcAlphaSatTest	SrcColorSrcAlphaSat;
CSrcColorBlendFactorTest	SrcColorBlendFactor;
CSrcColorInvBlendFactorTest	SrcColorInvBlendFactor;

// Global Source:InvSrcColor Test instances
CInvSrcColorZeroTest			InvSrcColorZero;
CInvSrcColorOneTest				InvSrcColorOne;
CInvSrcColorSrcColorTest		InvSrcColorSrcColor;
CInvSrcColorInvSrcColorTest		InvSrcColorInvSrcColor;
CInvSrcColorSrcAlphaTest		InvSrcColorSrcAlpha;
CInvSrcColorInvSrcAlphaTest		InvSrcColorInvSrcAlpha;
CInvSrcColorDestAlphaTest		InvSrcColorDestAlpha;
CInvSrcColorInvDestAlphaTest	InvSrcColorInvDestAlpha;
CInvSrcColorDestColorTest		InvSrcColorDestColor;
CInvSrcColorInvDestColorTest	InvSrcColorInvDestColor;
CInvSrcColorSrcAlphaSatTest		InvSrcColorSrcAlphaSat;
CInvSrcColorBlendFactorTest		InvSrcColorBlendFactor;
CInvSrcColorInvBlendFactorTest	InvSrcColorInvBlendFactor;

// Global Source:SrcAlpha Test instances
CSrcAlphaZeroTest			SrcAlphaZero;
CSrcAlphaOneTest			SrcAlphaOne;
CSrcAlphaSrcColorTest		SrcAlphaSrcColor;
CSrcAlphaInvSrcColorTest	SrcAlphaInvSrcColor;
CSrcAlphaSrcAlphaTest		SrcAlphaSrcAlpha;
CSrcAlphaInvSrcAlphaTest	SrcAlphaInvSrcAlpha;
CSrcAlphaDestAlphaTest		SrcAlphaDestAlpha;
CSrcAlphaInvDestAlphaTest	SrcAlphaInvDestAlpha;
CSrcAlphaDestColorTest		SrcAlphaDestColor;
CSrcAlphaInvDestColorTest	SrcAlphaInvDestColor;
CSrcAlphaSrcAlphaSatTest	SrcAlphaSrcAlphaSat;
CSrcAlphaBlendFactorTest	SrcAlphaBlendFactor;
CSrcAlphaInvBlendFactorTest	SrcAlphaInvBlendFactor;

// Global Source:InvSrcAlpha Test instances
CInvSrcAlphaZeroTest			InvSrcAlphaZero;
CInvSrcAlphaOneTest				InvSrcAlphaOne;
CInvSrcAlphaSrcColorTest		InvSrcAlphaSrcColor;
CInvSrcAlphaInvSrcColorTest		InvSrcAlphaInvSrcColor;
CInvSrcAlphaSrcAlphaTest		InvSrcAlphaSrcAlpha;
CInvSrcAlphaInvSrcAlphaTest		InvSrcAlphaInvSrcAlpha;
CInvSrcAlphaDestAlphaTest		InvSrcAlphaDestAlpha;
CInvSrcAlphaInvDestAlphaTest	InvSrcAlphaInvDestAlpha;
CInvSrcAlphaDestColorTest		InvSrcAlphaDestColor;
CInvSrcAlphaInvDestColorTest	InvSrcAlphaInvDestColor;
CInvSrcAlphaSrcAlphaSatTest		InvSrcAlphaSrcAlphaSat;
CInvSrcAlphaBlendFactorTest		InvSrcAlphaBlendFactor;
CInvSrcAlphaInvBlendFactorTest	InvSrcAlphaInvBlendFactor;

// Global Source:DestAlpha Test instances
CDestAlphaZeroTest				DestAlphaZero;
CDestAlphaOneTest				DestAlphaOne;
CDestAlphaSrcColorTest			DestAlphaSrcColor;
CDestAlphaInvSrcColorTest		DestAlphaInvSrcColor;
CDestAlphaSrcAlphaTest			DestAlphaSrcAlpha;
CDestAlphaInvSrcAlphaTest		DestAlphaInvSrcAlpha;
CDestAlphaDestAlphaTest			DestAlphaDestAlpha;
CDestAlphaInvDestAlphaTest		DestAlphaInvDestAlpha;
CDestAlphaDestColorTest			DestAlphaDestColor;
CDestAlphaInvDestColorTest		DestAlphaInvDestColor;
CDestAlphaSrcAlphaSatTest		DestAlphaSrcAlphaSat;
CDestAlphaBlendFactorTest		DestAlphaBlendFactor;
CDestAlphaInvBlendFactorTest	DestAlphaInvBlendFactor;

// Global Source:InvDestAlpha Test instances
CInvDestAlphaZeroTest			InvDestAlphaZero;
CInvDestAlphaOneTest			InvDestAlphaOne;
CInvDestAlphaSrcColorTest		InvDestAlphaSrcColor;
CInvDestAlphaInvSrcColorTest	InvDestAlphaInvSrcColor;
CInvDestAlphaSrcAlphaTest		InvDestAlphaSrcAlpha;
CInvDestAlphaInvSrcAlphaTest	InvDestAlphaInvSrcAlpha;
CInvDestAlphaDestAlphaTest		InvDestAlphaDestAlpha;
CInvDestAlphaInvDestAlphaTest	InvDestAlphaInvDestAlpha;
CInvDestAlphaDestColorTest		InvDestAlphaDestColor;
CInvDestAlphaInvDestColorTest	InvDestAlphaInvDestColor;
CInvDestAlphaSrcAlphaSatTest	InvDestAlphaSrcAlphaSat;
CInvDestAlphaBlendFactorTest	InvDestAlphaBlendFactor;
CInvDestAlphaInvBlendFactorTest	InvDestAlphaInvBlendFactor;

// Global Source:DestColor Test instances
CDestColorZeroTest				DestColorZero;
CDestColorOneTest				DestColorOne;
CDestColorSrcColorTest			DestColorSrcColor;
CDestColorInvSrcColorTest		DestColorInvSrcColor;
CDestColorSrcAlphaTest			DestColorSrcAlpha;
CDestColorInvSrcAlphaTest		DestColorInvSrcAlpha;
CDestColorDestAlphaTest			DestColorDestAlpha;
CDestColorInvDestAlphaTest		DestColorInvDestAlpha;
CDestColorDestColorTest			DestColorDestColor;
CDestColorInvDestColorTest		DestColorInvDestColor;
CDestColorSrcAlphaSatTest		DestColorSrcAlphaSat;
CDestColorBlendFactorTest		DestColorBlendFactor;
CDestColorInvBlendFactorTest	DestColorInvBlendFactor;

// Global Source:InvDestColor Test instances
CInvDestColorZeroTest			InvDestColorZero;
CInvDestColorOneTest			InvDestColorOne;
CInvDestColorSrcColorTest		InvDestColorSrcColor;
CInvDestColorInvSrcColorTest	InvDestColorInvSrcColor;
CInvDestColorSrcAlphaTest		InvDestColorSrcAlpha;
CInvDestColorInvSrcAlphaTest	InvDestColorInvSrcAlpha;
CInvDestColorDestAlphaTest		InvDestColorDestAlpha;
CInvDestColorInvDestAlphaTest	InvDestColorInvDestAlpha;
CInvDestColorDestColorTest		InvDestColorDestColor;
CInvDestColorInvDestColorTest	InvDestColorInvDestColor;
CInvDestColorSrcAlphaSatTest	InvDestColorSrcAlphaSat;
CInvDestColorBlendFactorTest	InvDestColorBlendFactor;
CInvDestColorInvBlendFactorTest	InvDestColorInvBlendFactor;

// Global Source:SrcAlphaSat Test instances
CSrcAlphaSatZeroTest			SrcAlphaSatZero;
CSrcAlphaSatOneTest				SrcAlphaSatOne;
CSrcAlphaSatSrcColorTest		SrcAlphaSatSrcColor;
CSrcAlphaSatInvSrcColorTest		SrcAlphaSatInvSrcColor;
CSrcAlphaSatSrcAlphaTest		SrcAlphaSatSrcAlpha;
CSrcAlphaSatInvSrcAlphaTest		SrcAlphaSatInvSrcAlpha;
CSrcAlphaSatDestAlphaTest		SrcAlphaSatDestAlpha;
CSrcAlphaSatInvDestAlphaTest	SrcAlphaSatInvDestAlpha;
CSrcAlphaSatDestColorTest		SrcAlphaSatDestColor;
CSrcAlphaSatInvDestColorTest	SrcAlphaSatInvDestColor;
CSrcAlphaSatSrcAlphaSatTest		SrcAlphaSatSrcAlphaSat;
CSrcAlphaSatBlendFactorTest		SrcAlphaSatBlendFactor;
CSrcAlphaSatInvBlendFactorTest	SrcAlphaSatInvBlendFactor;

// Global Source:BothSrcAlpha Test instance
CBothSrcAlphaTest		BothSrcAlpha;

// Global Source:BothInvSrcAlpha Test instance
CBothInvSrcAlphaTest	BothInvSrcAlpha;

// Global Source:BlendFactor Test instances
CBlendFactorZeroTest			BlendFactorZero;
CBlendFactorOneTest				BlendFactorOne;
CBlendFactorSrcColorTest		BlendFactorSrcColor;
CBlendFactorInvSrcColorTest		BlendFactorInvSrcColor;
CBlendFactorSrcAlphaTest		BlendFactorSrcAlpha;
CBlendFactorInvSrcAlphaTest		BlendFactorInvSrcAlpha;
CBlendFactorDestAlphaTest		BlendFactorDestAlpha;
CBlendFactorInvDestAlphaTest	BlendFactorInvDestAlpha;
CBlendFactorDestColorTest		BlendFactorDestColor;
CBlendFactorInvDestColorTest	BlendFactorInvDestColor;
CBlendFactorSrcAlphaSatTest		BlendFactorSrcAlphaSat;
CBlendFactorBlendFactorTest		BlendFactorBlendFactor;
CBlendFactorInvBlendFactorTest	BlendFactorInvBlendFactor;

// Global Source:InvBlendFactor Test instances
CInvBlendFactorZeroTest				InvBlendFactorZero;
CInvBlendFactorOneTest				InvBlendFactorOne;
CInvBlendFactorSrcColorTest			InvBlendFactorSrcColor;
CInvBlendFactorInvSrcColorTest		InvBlendFactorInvSrcColor;
CInvBlendFactorSrcAlphaTest			InvBlendFactorSrcAlpha;
CInvBlendFactorInvSrcAlphaTest		InvBlendFactorInvSrcAlpha;
CInvBlendFactorDestAlphaTest		InvBlendFactorDestAlpha;
CInvBlendFactorInvDestAlphaTest		InvBlendFactorInvDestAlpha;
CInvBlendFactorDestColorTest		InvBlendFactorDestColor;
CInvBlendFactorInvDestColorTest		InvBlendFactorInvDestColor;
CInvBlendFactorSrcAlphaSatTest		InvBlendFactorSrcAlphaSat;
CInvBlendFactorBlendFactorTest		InvBlendFactorBlendFactor;
CInvBlendFactorInvBlendFactorTest	InvBlendFactorInvBlendFactor;

// Global Alpha Target False Test instance
CAlphaTargetFalseTest		AlphaTargetFalse;

// Global SwapEffect Test instances
CSwapEffectCopyTest			SwapEffectCopy;
CSwapEffectFlipTest			SwapEffectFlip;
CSwapEffectDiscardTest		SwapEffectDiscard;
#endif // FAST_TEST

#if FAST_TEST
CAlphaBldTest::CAlphaBldTest(void)
{
	// Init Framework options
	SetTestType(TESTTYPE_CONF);
	m_pFramework->GetImageCmp()->SetMinPixels(0);

	// Add some more space to fit all test cases in one back buffer
	m_Options.D3DOptions.nBackBufferWidth = (DWORD)256;
	m_Options.D3DOptions.nBackBufferHeight = (DWORD)256;
	// Set compare function to require 98.5%
	m_pFramework->GetImageCmp()->SetPassRequired(0.985f);

	// Force test to cycle desktop resolutions 16bpp/32bpp
	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ALL;

	// Init local variables
	nAlphaEnable = TRUE;
	bChecker = false;
	bSwapEffectCap = false;
	bBlendFactor = false;
	bSeparateAlpha = false;

	// Init Alpha value array
	nAlphaValue[0] = 0;
	nAlphaValue[1] = 128;
	nAlphaValue[2] = 255;

	// Init Blend Factor color array
	dwBlendFactor[0] = D3DCOLOR_ARGB(255,0,0,0);
	dwBlendFactor[1] = D3DCOLOR_ARGB(0,0,0,255);
	dwBlendFactor[2] = D3DCOLOR_ARGB(128,128,128,128);

	// Init Blend Factor Alpha array
	dwBlendFactorAlpha[0] = D3DCOLOR_ARGB(255,0,0,0);
	dwBlendFactorAlpha[1] = D3DCOLOR_ARGB(64,0,0,255);
	dwBlendFactorAlpha[2] = D3DCOLOR_ARGB(0,128,128,128);

	// Check for Checker key
	if (KeySet("checker"))
		bChecker = true;

	// Init the Src and Dest names to empty string
	szSrcName = "";
	szDestName = "";

	m_szTestName = "AlphaBld";
	m_szCommandKey = "AlphaBld";

	dwSrcCap = 0xffffffff;
	dwDestCap = 0xffffffff;

	// Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;
}
#else
CAlphaBldTest::CAlphaBldTest(void)
{
	// Init Framework options
	SetTestType(TESTTYPE_CONF);
	m_pFramework->GetImageCmp()->SetMinPixels(0);

	// Use 96x84 as a default resolution
	m_Options.D3DOptions.nBackBufferWidth = (DWORD) 80;
	m_Options.D3DOptions.nBackBufferHeight = (DWORD) 70;

	// Force test to cycle desktop resolutions 16bpp/32bpp
	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ALL;

	// Init local variables
	nAlphaEnable = TRUE;
	bChecker = false;
	bSwapEffectCap = false;
	bBlendFactor = false;
	bSeparateAlpha = false;

	// Init Alpha value array
	nAlphaValue[0] = 0;
	nAlphaValue[1] = 64;
	nAlphaValue[2] = 128;
	nAlphaValue[3] = 192;
	nAlphaValue[4] = 255;

	// Init Blend Factor color array
	dwBlendFactor[0] = D3DCOLOR_ARGB(255,0,0,0);
	dwBlendFactor[1] = D3DCOLOR_ARGB(0,255,0,0);
	dwBlendFactor[2] = D3DCOLOR_ARGB(0,0,255,0);
	dwBlendFactor[3] = D3DCOLOR_ARGB(0,0,0,255);
	dwBlendFactor[4] = D3DCOLOR_ARGB(128,128,128,128);

	// Init Blend Factor Alpha array
	dwBlendFactorAlpha[0] = D3DCOLOR_ARGB(255,0,0,0);
	dwBlendFactorAlpha[1] = D3DCOLOR_ARGB(192,255,0,0);
	dwBlendFactorAlpha[2] = D3DCOLOR_ARGB(128,0,255,0);
	dwBlendFactorAlpha[3] = D3DCOLOR_ARGB(64,0,0,255);
	dwBlendFactorAlpha[4] = D3DCOLOR_ARGB(0,128,128,128);

	// Check for Checker key
	if (KeySet("checker"))
		bChecker = true;

	// Init the Src and Dest names to empty string
	szSrcName = "";
	szDestName = "";
}
#endif

bool CAlphaBldTest::PreModeSetup(void)
{
	// User is asking to override our backbuffer format, let them
	if (m_Options.ModeOptions.BackBufferFmt != FMT_ANY)
		return true;

    // If >= Dx8 and device supports an alpha backbuffer, use it
    if (m_dwVersion >= 0x0800)
    {
        HRESULT hr;

		hr = m_pD3D->CheckDeviceType(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(),
									 m_pCurrentMode->pDisplay->Format, (FORMAT)FMT_A8R8G8B8, m_pCurrentMode->bWindowed);

		if (SUCCEEDED(hr))
			GetBackBufferFormatIndex(FMT_A8R8G8B8, m_pCommonBackBufferFormats, NULL, (UINT*)&(m_pCurrentMode->nBackBufferFormat));
    }

    return true;
}

bool CAlphaBldTest::CapsCheck(void)
{
	// Check the caps we are interested in
	DWORD dwSrcBlendCaps = m_pSrcDevice->GetCaps()->dwSrcBlendCaps;
	DWORD dwDestBlendCaps = m_pSrcDevice->GetCaps()->dwDestBlendCaps;
	DWORD dwPrimitiveMiscCaps = m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps;

	// Check to see if Separate Alpha Blending is supported
	if ((m_dwVersion >= 0x0900) && (dwPrimitiveMiscCaps & D3DPMISCCAPS_SEPARATEALPHABLEND))
		bSeparateAlphaCap = true;
	else
		bSeparateAlphaCap = false;

	// Check the appropriate Source Blend caps
	if (!(dwSrcBlendCaps & dwSrcCap))
	{
		WriteToLog("Device capability not supported: Source Blend %s.\n",szSrcName);
		return false;
	}

	// Check the appropriate Dest Blend caps
	if (!(dwDestBlendCaps & dwDestCap))
	{
		WriteToLog("Device capability not supported: Destination Blend %s.\n",szDestName);
		return false;
	}

	// Check to see if Blend Op is supported
    if ((m_dwVersion >= 0x0800) && (dwPrimitiveMiscCaps & D3DPMISCCAPS_BLENDOP))
        bBlendOps = true;
	else
        bBlendOps = false;

	// Need to check a special cap for some swap effect tests (Flip and Discard)
	if (bSwapEffectCap)
	{
		DWORD dwCaps3 = m_pSrcDevice->GetCaps()->dwCaps3;

		if (!(dwCaps3 & D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD))
		{
			WriteToLog("Device capability not supported: Caps3_Alpha_Fullscreen_Flip_Or_Discard.\n");
			return false;
		}
	}

	return true;
}

UINT CAlphaBldTest::TestInitialize(void)
{
#ifdef FAST_TEST
	SetTestRange(1, NUM_ALPHA_BLEND_TESTS);

	// Check if the BlendOps are supported
	if (bBlendOps)
	{
		// Use all file BlendOps
		dwBlendOp[0] = D3DBLENDOP_ADD;
		dwBlendOp[1] = D3DBLENDOP_SUBTRACT;
		dwBlendOp[2] = D3DBLENDOP_REVSUBTRACT;
		dwBlendOp[3] = D3DBLENDOP_MIN;
		dwBlendOp[4] = D3DBLENDOP_MAX;

		// Only use SeparateAlphaBlending when supported and on 32bpp
		if (bSeparateAlphaCap && bSeparateAlpha && (m_DeviceList[0]->GetAdapter()->DesktopMode.dwBPP == 32))
		{
			// Set test range for SeparateAlpha and all five BlendOps
			numBlendOps = 10;
		}
		else
		{
			numBlendOps = 5;
		}
	}
	else
	{
		numBlendOps = 1;
	}
#else
	// Check if the BlendOps are supported
	if (bBlendOps)
	{
		// Use all file BlendOps
		dwBlendOp[0] = D3DBLENDOP_ADD;
		dwBlendOp[1] = D3DBLENDOP_SUBTRACT;
		dwBlendOp[2] = D3DBLENDOP_REVSUBTRACT;
		dwBlendOp[3] = D3DBLENDOP_MIN;
		dwBlendOp[4] = D3DBLENDOP_MAX;

		// Only use SeparateAlphaBlending when supported and on 32bpp
		if (bSeparateAlphaCap && bSeparateAlpha && (m_DeviceList[0]->GetAdapter()->DesktopMode.dwBPP == 32))
		{
			// Set test range for SeparateAlpha and all five BlendOps
			SetTestRange(1, MAX_TESTS * 10);
		}
		else
		{
			// Set test range for all five BlendOps 
			SetTestRange(1, MAX_TESTS * 5);
		}
	}
	else
	{
		// Only use default BlendOp
		dwBlendOp[0] = D3DBLENDOP_ADD;

		// Set test range for default BlendOp
		SetTestRange(1, MAX_TESTS);
	}
#endif
	// Generate the checkerboard background 
    if (bChecker)
    	DrawBackground();

	// Tell log about our back buffer
	if (m_dwVersion >= 0x0800)
		BackBufferFormat = m_pCommonBackBufferFormats[m_pCurrentMode->nBackBufferFormat];
	else
		BackBufferFormat = m_pCurrentMode->pDisplay->Format;

	switch (BackBufferFormat.d3dfFormat)
	{
		case FMT_A2R10G10B10:	WriteToLog("AlphaTarget: True, A2R10G10B10.\n");
								break;
		case FMT_A8R8G8B8:		WriteToLog("AlphaTarget: True, A8R8G8B8.\n");
								break;
		case FMT_X8R8G8B8:		WriteToLog("AlphaTarget: False, X8R8G8B8.\n");
								break;
		case FMT_A1R5G5B5:		WriteToLog("AlphaTarget: True, A1R5G5B5.\n");
								break;
		case FMT_X1R5G5B5:		WriteToLog("AlphaTarget: False, X1R5G5B5.\n");
								break;
		case FMT_R5G6B5:		WriteToLog("AlphaTarget: False, R5G6B5.\n");
								break;
		default:				TCHAR szBuffer[80];
								FmtToString(FORMAT_TEXTURE,&BackBufferFormat,szBuffer);
								WriteToLog("Error: Unknown BackBufferFormat: %d (%s).\n",BackBufferFormat.d3dfFormat,szBuffer);
								return D3DTESTINIT_ABORT;
	}

	return D3DTESTINIT_RUN;
}

bool CAlphaBldTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(116,116,116,255), 1.0f, 0);
}

#ifdef FAST_TEST
bool CAlphaBldTest::ExecuteTest(UINT uTestNum)
{
	switch (uTestNum)
	{
	case D3DBLEND_BOTHINVSRCALPHA_TEST:
		dwSrcBlend = D3DBLEND_BOTHINVSRCALPHA;
		dwDestBlend = 0;
		bBlendFactor = false;
		bSeparateAlpha = false;
		break;
	case D3DBLEND_BOTHSRCALPHA_TEST:
		dwSrcBlend = D3DBLEND_BOTHSRCALPHA;
		dwDestBlend = 0;
		bBlendFactor = false;
		bSeparateAlpha = false;
		break;
	default:
		// Note: uTestNum starts at 1, not 0
		dwDestBlend = (D3DBLEND)g_cPermutableBlendStates[(uTestNum - 1) % NUM_PERMUTABLE_BLEND_TYPES];
		dwSrcBlend = (D3DBLEND)g_cPermutableBlendStates[(uTestNum - 1) / NUM_PERMUTABLE_BLEND_TYPES];
		bBlendFactor = true;
		bSeparateAlpha = true;
		break;
	}

	numBlendOps = bSeparateAlpha ? 10 : 5;

	// Tell the log that we are starting
	BeginTestCase(msgString, uTestNum);

	DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
	DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
	const DWORD GridWidth = dwWidth / (COLOR_VARIATIONS * numBlendOps);
	const DWORD GridHeight = dwHeight / COLOR_VARIATIONS;

	// Render the scene
	if (BeginScene())
	{
		for (UINT backVariation = 0; backVariation < COLOR_VARIATIONS; backVariation++)
		{
			for (UINT frontVariation = 0; frontVariation < COLOR_VARIATIONS; frontVariation++)
			{
				for (UINT opIndex = 0; opIndex < numBlendOps; opIndex++)
				{
					// Initialize some variables
					int nFactor = frontVariation;
					int nOp = opIndex % NUM_BLEND_OPS;
					bool bAlphaOnly = opIndex > NUM_BLEND_OPS ? true : false;
					dwVertexCount = 0;
					msgString[0] = '\0';

					int gridLeft = (frontVariation * numBlendOps + opIndex) * GridWidth;
					int gridTop = backVariation * GridHeight;
					DrawGrid(nAlphaValue[frontVariation], nAlphaValue[backVariation], gridLeft, gridTop, gridLeft + GridWidth, gridTop + GridHeight);

					// Render the Background image
					if (bChecker)
					{
						SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
						RenderPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, BackgroundList, 400, NULL, 0, 0);
					}

					// Set the appropriate render states
					if (bAlphaOnly)
					{
						// Setup the color blend to a known state
						SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
						SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_ONE);
						SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_ZERO);
						SetRenderState(D3DRS_BLENDOP, (DWORD)D3DBLENDOP_ADD);

						// Setup the separate alpha blend according to the test case
						SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, (DWORD)nAlphaEnable);
						SetRenderState(D3DRS_SRCBLENDALPHA, dwSrcBlend);
						SetRenderState(D3DRS_DESTBLENDALPHA, dwDestBlend);
						SetRenderState(D3DRS_BLENDOPALPHA, (DWORD)dwBlendOp[nOp]);

						// If BlendFactor supported, set the appropriate BlendFactor color
						if (bBlendFactor)
							SetRenderState(D3DRS_BLENDFACTOR, (DWORD)dwBlendFactorAlpha[nFactor]);
					}
					else
					{
						// Set the appropriate AlphaBlendEnable render state.
						SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)nAlphaEnable);

						// Set the appropriate Source/Destination blend render states.
						SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);

						if (dwDestBlend)
							SetRenderState(D3DRS_DESTBLEND, dwDestBlend);

						// If BlendOps supported, set the appropriate BlendOp
						if (bBlendOps)
							SetRenderState(D3DRS_BLENDOP, (DWORD)dwBlendOp[nOp]);

						// If BlendFactor supported, set the appropriate BlendFactor color
						if (bBlendFactor)
							SetRenderState(D3DRS_BLENDFACTOR, (DWORD)dwBlendFactor[nFactor]);
					}

					// Render the appropriate test case.
					RenderPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, VertexList, 4, NULL, 0, 0);
					RenderPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, VertexList + 4, dwVertexCount - 4, NULL, 0, 0);
				}
			}
		}
	}

	EndScene();

	return true;
}
#else
bool CAlphaBldTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	int nTest = (uTestNum - 1) % MAX_TESTS;
	int nOp = ((uTestNum - 1) / MAX_TESTS) % 5;
	int nFactor = nTest / 6;
	bool bCircle = (nTest % 6) ? false : true;
	bool bAlphaOnly = (uTestNum > MAX_TESTS * 5) ? true : false;
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Tell log that were are using the separate alpha blends
	if (bAlphaOnly)
		sprintf(msgString, "%sSeparate, ",msgString);

    // Tell log which Blend Op we are using
    switch (dwBlendOp[nOp])
    {
        case D3DBLENDOP_ADD: sprintf(msgString, "%sOp:Add, ",msgString);
                             break;
        case D3DBLENDOP_SUBTRACT: sprintf(msgString, "%sOp:Subtract, ",msgString);
                                  break;
        case D3DBLENDOP_REVSUBTRACT: sprintf(msgString, "%sOp:RevSubtract, ",msgString);
                                     break;
        case D3DBLENDOP_MIN: sprintf(msgString, "%sOp:Min, ",msgString);
                             break;
        case D3DBLENDOP_MAX: sprintf(msgString, "%sOp:Max, ",msgString);
                             break;
        default: WriteToLog("Error: Unknown BlendOp: %d.\n",dwBlendOp[nOp]);
                 TestFinish(D3DTESTFINISH_ABORT);
                 return false;
    }

	// If BlendFactor supported, tell log which Blend Factor color we are using
	if (bBlendFactor)
	{
		if (bAlphaOnly)
		{
			switch (dwBlendFactorAlpha[nFactor])
			{
				case D3DCOLOR_ARGB(255,0,0,0): sprintf(msgString, "%sFactor:ARGB(255,0,0,0), ",msgString);
											   break;
				case D3DCOLOR_ARGB(192,255,0,0): sprintf(msgString, "%sFactor:ARGB(192,255,0,0), ",msgString);
												 break;
				case D3DCOLOR_ARGB(128,0,255,0): sprintf(msgString, "%sFactor:ARGB(128,0,255,0), ",msgString);
												 break;
				case D3DCOLOR_ARGB(64,0,0,255): sprintf(msgString, "%sFactor:ARGB(64,0,0,255), ",msgString);
												break;
				case D3DCOLOR_ARGB(0,128,128,128): sprintf(msgString, "%sFactor:ARGB(0,128,128,128), ",msgString);
													 break;
				default: WriteToLog("Error: Unknown BlendFactor: %x.\n",dwBlendFactorAlpha[nFactor]);
						 TestFinish(D3DTESTFINISH_ABORT);
						 return false;
			}
		}
		else
		{
			switch (dwBlendFactor[nFactor])
			{
				case D3DCOLOR_ARGB(255,0,0,0): sprintf(msgString, "%sFactor:ARGB(255,0,0,0), ",msgString);
											   break;
				case D3DCOLOR_ARGB(0,255,0,0): sprintf(msgString, "%sFactor:ARGB(0,255,0,0), ",msgString);
											   break;
				case D3DCOLOR_ARGB(0,0,255,0): sprintf(msgString, "%sFactor:ARGB(0,0,255,0), ",msgString);
											   break;
				case D3DCOLOR_ARGB(0,0,0,255): sprintf(msgString, "%sFactor:ARGB(0,0,0,255), ",msgString);
											   break;
				case D3DCOLOR_ARGB(128,128,128,128):sprintf(msgString, "%sFactor:ARGB(128,128,128,128), ",msgString);
													break;
				default: WriteToLog("Error: Unknown BlendFactor: %x.\n",dwBlendFactor[nFactor]);
						 TestFinish(D3DTESTFINISH_ABORT);
						 return false;
			}
		}
	}

	// Let's build some triangles.
	if (bCircle)
		sprintf(msgString, "%sCircles:%d",msgString,nAlphaValue[nTest/6]);
	else
	{
		DrawGrid(nTest, (nTest / 6));

		sprintf(msgString, "%sMesh:%d, Triangles:%d",msgString,nAlphaValue[nTest/6],nAlphaValue[(nTest%6)-1]);
	}

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    // Render the scene
	if (BeginScene())
	{
		// Render the Background image
        if (bChecker)
        {
		    SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
		    RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,BackgroundList,400,NULL,0,0);
        }

		// Set the appropriate render states
		if (bAlphaOnly)
		{
			// Setup the color blend to a known state
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_ONE);
			SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_ZERO);
            SetRenderState(D3DRS_BLENDOP, (DWORD)D3DBLENDOP_ADD);

			// Setup the separate alpha blend according to the test case
			SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, (DWORD)nAlphaEnable);
			SetRenderState(D3DRS_SRCBLENDALPHA, dwSrcBlend);
			SetRenderState(D3DRS_DESTBLENDALPHA, dwDestBlend);
			SetRenderState(D3DRS_BLENDOPALPHA, (DWORD)dwBlendOp[nOp]);

			// If BlendFactor supported, set the appropriate BlendFactor color
			if (bBlendFactor)
				SetRenderState(D3DRS_BLENDFACTOR, (DWORD)dwBlendFactorAlpha[nFactor]);
		}
		else
		{
			// Set the appropriate AlphaBlendEnable render state.
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)nAlphaEnable);

			// Set the appropriate Source/Destination blend render states.
			SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);

			if (dwDestBlend)
				SetRenderState(D3DRS_DESTBLEND, dwDestBlend);

			// If BlendOps supported, set the appropriate BlendOp
			if (bBlendOps)
				SetRenderState(D3DRS_BLENDOP, (DWORD)dwBlendOp[nOp]);

			// If BlendFactor supported, set the appropriate BlendFactor color
			if (bBlendFactor)
				SetRenderState(D3DRS_BLENDFACTOR, (DWORD)dwBlendFactor[nFactor]);
		}

		// Render the appropriate test case.
		if (bCircle)
		{
			D3DVECTOR Center;
			float     fRadius;
			DWORD	  dwWidth = m_pCurrentMode->nBackBufferWidth;
			DWORD	  dwHeight = m_pCurrentMode->nBackBufferHeight;
			D3DCOLOR  ColorArray[] = {RGBA_MAKE(255,0,0,nAlphaValue[nTest/6]),
									  RGBA_MAKE(0,255,0,nAlphaValue[nTest/6]),
									  RGBA_MAKE(0,0,255,nAlphaValue[nTest/6])};

			fRadius = ((dwWidth < dwHeight) ? dwWidth : dwHeight) * 0.335f;

			Center.x = 0.65f * dwWidth;
			Center.y = 0.50f * dwHeight;
			Center.z = 0.50f;
			DrawCircle(&Center, fRadius, 36, ColorArray[0]);
			Center.x = 0.35f * dwWidth;
			Center.y = 0.70f * dwHeight;
			Center.z = 0.50f;
			DrawCircle(&Center, fRadius, 36, ColorArray[1]);
			Center.x = 0.35f * dwWidth;
			Center.y = 0.30f * dwHeight;
			Center.z = 0.50f;
			DrawCircle(&Center, fRadius, 36, ColorArray[2]);
		}
		else
		{
			RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,VertexList,8,NULL,0,0);
			RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList+8,dwVertexCount-8,NULL,0,0);
		}

		EndScene();
	}	

    return true;
}
#endif

void CAlphaBldTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window about our back buffer
	switch (BackBufferFormat.d3dfFormat)
	{
		case FMT_A2R10G10B10:	WriteStatus("AlphaTarget", "$aTrue, A2R10G10B10");
								break;
		case FMT_A8R8G8B8:		WriteStatus("AlphaTarget", "$aTrue, A8R8G8B8");
								break;
		case FMT_X8R8G8B8:		WriteStatus("AlphaTarget", "$aFalse, X8R8G8B8");
								break;
		case FMT_A1R5G5B5:		WriteStatus("AlphaTarget", "$aTrue, A1R5G5B5");
								break;
		case FMT_X1R5G5B5:		WriteStatus("AlphaTarget", "$aFalse, X1R5G5B5");
								break;
		case FMT_R5G6B5:		WriteStatus("AlphaTarget", "$aFalse, R5G6B5");
								break;
		default:				WriteStatus("AlphaTarget", "$rUnknown");
								break;
	}

}
