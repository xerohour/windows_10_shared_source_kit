#ifndef _LOSTDEV_H_
#define _LOSTDEV_H_

#define D3D_OVERLOADS

//#define USE_FUS

#include "CD3DTest.h"
#include "CasesManager.h"
#include "CFlagCombinations.h"
#include "CTestFactor.h"
#include "CTextures.h"
#include "CSwapChain.h"
//#include "CCommandLine.h"
#include "cd3d.h"
#include "d3d9.h"
#include <windows.h>

//Pools
#define TEST_SYSTEMMEM		0x00000001
#define TEST_MANAGED		0x00000002
#define TEST_DEFAULT		0x00000004
#define TEST_SCRATCH		0x00000008
#define TEST_ALLPOOLS		TEST_SYSTEMMEM | TEST_MANAGED | TEST_DEFAULT | TEST_SCRATCH

//Individual tests cases
const DWORD TEST_BLTDXTODX				= 0x00000001;
const DWORD TEST_BLTGDITODX				= 0x00000002;
const DWORD TEST_BLTDXTOGDI				= 0x00000004;
const DWORD TEST_BLTSCREENTODX			= 0x00000008;
const DWORD TEST_BLTDXTOSCREEN			= 0x00000010;

const DWORD TEST_SETPIXEL				= 0x00010000;
const DWORD TEST_ATTRIBUTES				= 0x00020000;
const DWORD TEST_HIGLEVELGDI			= 0x00040000;
const DWORD TEST_CREATEWHILELOST		= 0x00080000;
const DWORD TEST_ALIASING				= 0x00100000;
const DWORD TEST_BEGINENDSCENE			= 0x00200000;
const DWORD TEST_RENDERTHENGETDC		= 0x00000020;
const DWORD TEST_GETDCTHENRENDER		= 0x00000040;

const DWORD TEST_LOCKTHENGETDC			= 0x00400000;
const DWORD TEST_GETDCTHENLOCK			= 0x00800000;

const DWORD TEST_RELEASEDDCUNUSABLE		= 0x01000000;
const DWORD TEST_RELEASEDCTWICE			= 0x02000000;
const DWORD TEST_RELEASEDCONOTHERSURF	= 0x04000000;
const DWORD TEST_GETMULTIPLEDCS			= 0x08000000;

const DWORD TEST_MEMORYLEAKS			= 0x10000000;
const DWORD TEST_RELEASEBOGUSDC			= 0x20000000;
const DWORD TEST_GETDCBADHANDLEPTR		= 0x40000000;

const DWORD TEST_CREATEDCONLOSTDEV		= 0x00000100;
const DWORD TEST_USEDCONLOSTDEV			= 0x00000200;
const DWORD TEST_USEDCAFTERBEGINSCENE	= 0x00000400;
const DWORD TEST_GDIATTRIBUTES			= 0x00000800;
const DWORD TEST_GDICLIPPING			= 0x00001000;
const DWORD TEST_HIGHLEVELGDIOPER		= 0x00002000;

const DWORD SURF_RENDERTARGET		= 0x00000002;
const DWORD SURF_TEXTURE			= 0x00000004;
const DWORD SURF_OFFSCREENPLAIN		= 0x00000080;
const DWORD SURF_SWAPCHAIN			= 0x00000010;
const DWORD SURF_BACKBUFFER			= 0x00000020;

const DWORD SIZE_1X1				= 0x00000001;
const DWORD SIZE_2X2				= 0x00000002;
const DWORD SIZE_55X55				= 0x00000004;
const DWORD SIZE_63X64				= 0x00000008;
const DWORD SIZE_64X64				= 0x00000010;
const DWORD SIZE_257X64				= 0x00000020;
const DWORD SIZE_1256X1256			= 0x00000030;

//const DWORD BUFFER_SWAPCHAIN		= 0x00000002;
#if FAST_TEST
#define MAXFORMAT 1
#define MAXSIZES 1
#define MAXSURFACES 2
#else//FAST_TEST
#define MAXFORMAT 4
#define MAXSIZES 6
#define MAXSURFACES 2
#endif//FAST_TEST

// ----------------------------------------------------------------------------
//class CGetDC
// ----------------------------------------------------------------------------

class CGetDC : public CD3DTest
{
protected:
	static CCase		m_FormatList[MAXFORMAT];
	static CSizeCase	m_SizeList[MAXSIZES];
	DISPLAY_DEVICE	m_DisplayDevice;
	DEVMODE			m_DevMode;

	PRESENT_PARAMETERS m_PresParams;

	LPTSTR			m_TestName;
	char			m_szFullName[255];

	UINT			m_nSurfNeededForTest;

	LPSURFACES		m_pSurfaces[ MAXSURFACES ];
	LPSURFACE		m_pSurf[ MAXSURFACES ];
	LPTEXTURES		m_pTextures[ MAXSURFACES ];
	LPSWAPCHAINS	m_pSwapChains[ MAXSURFACES ];

	HDC				m_hDXDC[ MAXSURFACES ];

	TCHAR			m_szSurfaceType[MAX_PATH];
	TCHAR			m_szSurfaceFormat[MAX_PATH];
	TCHAR			m_szUsage[MAX_PATH];
	TCHAR			m_szPool[MAX_PATH];
	TCHAR			m_szSize[MAX_PATH];
	TCHAR			m_szTestCase[MAX_PATH];

	DWORD			m_dwSurfaceFormat;
	DWORD			m_dwSurfaceType;
	DWORD			m_dwUsage;
	DWORD			m_dwPool;
	UINT			m_uSurfaceWidth;
	UINT			m_uSurfaceHeight;
	DWORD			m_dwTestCase;

	bool			m_bSkipTest;
	bool			m_bDynamicSupported;
	bool			m_bMipMapSupported;

	RFactor			m_rCasesManager;

	RFactor			m_rSurface;
	RFactor			m_rPool;
	RFactor			m_rPoolOffscreenPlain;
	RFactor			m_rSize;
	RFactor			m_rSizeTexture;
	RFactor			m_rUseMipLevel;
	RFactor			m_rPixelFormatRT;
	RFactor			m_rPixelFormatTexture;
	RFactor			m_rPixelFormatSwapChain;
	RFactor			m_rPixelFormatOffscreenPlain;
	RFactor			m_rTexUsage;
	RFactor			m_rBackBuffer;

	RFactor			m_rTestCase;
	RFactor			m_rAPITestCase;
	RFactor			m_rOtherAPITestCase;  //These are test cases that doesn't need to be tested for every size, format, etc..

	CCasesList		m_casesSurface;
	CCasesList		m_casesPool;
	CCasesList		m_casesPoolOffscreenPlain;
	CCasesList		m_casesSize;
	CCasesList		m_casesSizeTexture;
	CCasesList		m_casesPixelFormatRT;
	CCasesList		m_casesPixelFormatTexture;
	CCasesList		m_casesPixelFormatOffscreenPlain;
	CCasesList		m_casesPixelFormatSwapChain;
	CCasesList		m_casesBackBuffer;
	CCasesList		m_casesTexUsage;
	CCasesList		m_casesTestCase;
	CCasesList		m_casesAPITestCase;
	CCasesList		m_casesOtherAPITestCase; //These are test cases that doesn't need to be tested for every size, format, etc..

	//member to create a gdi dc and DIB section
	HGDIOBJ			m_hComparisonDCLastObj;
	HDC				m_hComparisonDC;
	PVOID			m_pComparisonBitmap;
	HBITMAP			m_hComparisonBitmap;
	HWND			m_hScreenWindow;

	bool			m_bNeedToReset;


	//Utilities
	bool SetPixels(HDC hDC);
	bool CreateComparisonDC();
	bool ReleaseComparisonDC();
	bool CompareSurfaceToDC(LPSURFACE pSurf);
	bool CreateWindowsDC(HDC &hWindowsDC, HBITMAP &hBitmap, PVOID *ppBitmap, HGDIOBJ &hWindowsDCLastObject);
	bool ReleaseWindowsDC(HDC hWindowsDC, HGDIOBJ hWindowsDCLastObject, HBITMAP hBitmap);
	bool CompareDCs(HDC hDC1, HDC hDC2, bool bShouldSucceed = true);
	bool RenderQuad();
	bool CreateSurface(ULONG lDev = CD3D_SRC);


	void ValidatePixelFormat();
	void ValidatePools();
	void ValidateSizes();
	void ValidateLostWays();
	void ValidateTextureUsage();
	void ValidateTestCases();

	bool CreateTestFactors();

	void DumpTestDetails();

	// Methods used by the tests
	bool SetTestCase(UINT uTestNumber);
	void SetCurrentSurfaceType();
	bool SetTestFactors();
	bool RunTest();


 
public: // class functions
	CGetDC();
	~CGetDC();

    void ReleaseTest();
   
	// Framework overloads
    UINT TestInitialize(void);
    bool ExecuteTest(UINT);
	bool CapsCheck();
	void CommandLineHelp(void);

	virtual void EndTestCase(void);
	bool TestTerminate(void);

	// The different tests
	bool TestSetPixel(LPSURFACE pSurf);
	bool TestBltGDIToDX();
	bool TestBltDXToGDI();
	bool TestBltDXToDX();
	bool TestBltDXToScreen();
	bool TestBltScreenToDX();
	bool TestLockThenGetDC(LPSURFACE pSurf);
	bool TestGetDCThenLock(LPSURFACE pSurf);
	bool TestRenderThenGetDC(LPSURFACE pSurf);
	bool TestGetDCThenRender(LPSURFACE pSurf);

	bool TestCreateDCOnLostDev(LPSURFACE pSurf);
	bool TestUseDCOnLostDev();
	bool TestUseDCAfterBeginScene();
	bool TestGDIAttributes();
	bool TestGDIClipping();
	bool TestHighLevelGDIOperations();

	bool TestGetMultipleDCs(LPSURFACE pSurf);
	bool TestReleasedDCUnusable(LPSURFACE pSurf);
	bool TestReleaseDCTwice(LPSURFACE pSurf);
	bool TestReleaseDCOnOtherSurf(LPSURFACE pSurf1, LPSURFACE pSurf2);

	bool TestGetDCBadHandlePtr(LPSURFACE pSurf);
	bool TestReleaseBogusDC(LPSURFACE pSurf);
	bool TestMemoryLeaks(LPSURFACE pSurf);
};

#endif