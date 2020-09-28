#ifndef CCOLORFILLTESTMAIN_INCLUDED
#define CCOLORFILLTESTMAIN_INCLUDED

#include <d3d9.h>
#include "Framework.h"
#include "CBuffers.h"

#include "SurfaceTypes.h"

static CONST UINT cuiMaxLogfilePathLength = 256;
static CONST UINT cuiMaxTestRunSummaryLength = 256;
static CONST UINT cuiMaxErrorMessageLength = 256;
static CONST UINT cuiMaxSurfaceDescriptionLength = 256;
static CONST UINT cuiMaxLogLineLength = 256;

class clsSurfaceSpecArray;
class CColorFillTestMain;

typedef struct _SurfaceSpecListElement SurfaceSpecListElement;
struct _SurfaceSpecListElement
{
	DWORD dwSurfaceSpec;
	SurfaceSpecListElement *pNext;
};

class clsSurfaceSpecList
{
private:
	SurfaceSpecListElement *pFirst, *pLast;
	unsigned int uiNoOfElements;

public:
	clsSurfaceSpecList() {pFirst = (pLast = NULL); uiNoOfElements = 0;}
	~clsSurfaceSpecList();
	void AddSurfaceSpec(DWORD dwSurfaceSpec);
	void InitializeSurfaceSpecArray(clsSurfaceSpecArray *pTargetArray);
};

class clsSurfaceSpecArray
{
private:
	friend class clsSurfaceSpecList;
	DWORD *pdwSurfaceSpecs;
	unsigned int uiNoOfElements;
public:
	clsSurfaceSpecArray() {pdwSurfaceSpecs = NULL; uiNoOfElements = 0;}
	~clsSurfaceSpecArray();
	unsigned int GetNumberOfElements() {return uiNoOfElements;}
	DWORD GetElementValue(unsigned int uiIndex);
};

typedef
struct _FormatCases
{
	UINT uiFormatIndex, uiNoOfTestCases;
}
FormatCases;

typedef struct _FormatCasesListElement FormatCasesListElement;

struct _FormatCasesListElement
{
	FormatCases FormatCasesData;
	FormatCasesListElement *pNext;
};

class clsFormatCasesList
{
private:
	friend class clsFormatCasesListScanner;
	FormatCasesListElement *pFirst, *pLast;
public:
	FormatCases* AddNewElement();
	void DeleteAll();
	clsFormatCasesList() {pFirst = (pLast = NULL);}
	~clsFormatCasesList() {DeleteAll();}
};

class clsFormatCasesListScanner
{
private:
	FormatCasesListElement *pCurrentElement;
public:
	clsFormatCasesListScanner() {pCurrentElement = NULL;}
	~clsFormatCasesListScanner() {}
	void Reset(clsFormatCasesList *pTargetList) {if (pTargetList) pCurrentElement = pTargetList -> pFirst;}
	void Advance() {if (pCurrentElement) pCurrentElement = pCurrentElement -> pNext;}
	FormatCases* GetCurrent() {return (pCurrentElement ? &(pCurrentElement -> FormatCasesData) : NULL);}
};

/*
class clsSourceSurfaceSet
{
private:
	bool bInitialized;
	DWORD dwMainSurfaceSpec;
public:
	clsTestSurface *pSourceTestSurface;
	SURFACEDESC sdSourceSurfaceDesc;
	char szDescription[cuiMaxSurfaceDescriptionLength];
	clsTextureSurface texsurfSourceLoader;

	void ReleaseAllSurfaces();
	clsSourceSurfaceSet() {pSourceTestSurface = NULL; bInitialized = false; dwMainSurfaceSpec = 0;}
	~clsSourceSurfaceSet() {ReleaseAllSurfaces();}
	bool PrepareFromDWORD(CColorFillTestMain *pTestEnv, DWORD dwSurfaceSpec);
};
*/

class clsDestSurfaceSet
{
private:
	bool bInitialized;
	DWORD dwMainSurfaceSpec;
	UINT uiNoOfSurfacePixels;

public:
	clsTestSurface *pDestTestSurface;
	SURFACEDESC sdDestSurfaceDesc;
	char szDescription[cuiMaxSurfaceDescriptionLength];
	clsTextureSurface texsurfDestLoader, texsurfReference, texsurfDestCopy;

	void ReleaseAllSurfaces();
	clsDestSurfaceSet() {pDestTestSurface = NULL; bInitialized = false; dwMainSurfaceSpec = 0;}
	~clsDestSurfaceSet() {ReleaseAllSurfaces();}
	bool PrepareFromDWORD(CColorFillTestMain *pTestEnv, DWORD dwSurfaceSpec);
	UINT GetNoOfSurfacePixels() {return uiNoOfSurfacePixels;}
};

class CColorFillTestMain : public CD3DTest, public clsWrappedSurfaceClient
{
private:
	// Private data members
	//clsSurfaceSpecArray SourceSurfaceSpecArray, DestSurfaceSpecArray;
	//clsSurfaceSpecArray SurfaceSpecArray;

	// This is an array comprising one element for each format.  Its members specify the valid
	// destination surface specifications for the corresponding format.
	clsSurfaceSpecArray *ValidSurfacesByFormat;

	// This list is used by ExecuteTest to interpret the test case ID.  Each element represents
	// a format and the (nonzero) number of test cases associated with the format.
	clsFormatCasesList TestCasesByFormat;



	// D3D resources that provide support for MyLoadSurfaceFromTexture.

	/*
	// Lockable surface for rendering video memory textures.
	CSurface				*psurfAuxiliaryRenderTarget;
	*/

	// Lockable surface for rendering video memory textures.
	CSurface				*psurfAuxiliaryRenderTargetTest;
	// Lockable surface for rendering the reference image (from a managed texture surface)
	CSurface				*psurfAuxiliaryRenderTargetRef;

	// Vertex buffer used for texture rendering operations, along with vertex structure
	// definition and buffer intialization data.
	CVertexBuffer			*pvbAuxiliaryVertexBuffer;
	typedef
	struct _BASICTEXTUREVERTEX
	{
		FLOAT x, y, z;
		FLOAT nx, ny, nz;
		FLOAT u, v;
	}
	BASICTEXTUREVERTEX;
	static BASICTEXTUREVERTEX QuadVertices[];
	static CONST DWORD dwD3DFVF_VERTEX;



	// clsSourceSurfaceSet SourceSurfaceSet;
	clsDestSurfaceSet DestSurfaceSet;

	RECT rectReferenceDisplay, rectTestDisplay;

	static const float fMinimumConformanceFraction;

public:
	// Public data members
	LPVOID BitmapDataBackground;
	UINT uiSrcDeviceAdapterOrdinal;
	DWORD dwSrcDeviceType;
	SURFACEDESC sdSrcBackBuffer;
	CAPS SrcDeviceCaps;
	DISPLAYMODE displaymodeAdapter;

private:
	// Private Methods
	void ReportInvalidTCID(UINT uiTestCaseId)
	{
		WriteToLog("Unable to interpret test case identifier: %d\n", uiTestCaseId);
		SkipTestRange(uiTestCaseId, uiTestCaseId);
	}

	bool InitializeAuxiliaryResources();
	void ClearAuxiliaryResources();

	//void FindValidSourceSurfaceSpecs();
	//void FindValidDestSurfaceSpecs();
	void FindValidTextureSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList);
	void FindValidOffscreenPlainSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList);
	void FindValidRTSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList);
	//void FindValidSurfaceSpecs();
	//void FindValidSourceSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pSourceSpecArray);
	void FindValidDestSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pDestSpecArray);

	static void AdjustRectForDXTn(RECT *pRect, UINT uiSurfaceWidth, UINT uiSurfaceHeight);
	static void AdjustRectForUYVY(RECT *pRect, UINT uiSurfaceWidth, UINT uiSurfaceHeight);
	static bool TestDXTn(FORMAT fmtTest);
	static bool TestUYVY(FORMAT	fmtTest);

public:
	// Public Methods
	CColorFillTestMain();
	~CColorFillTestMain();
	UINT TestInitialize(void);
	bool ExecuteTest(UINT uiTestCaseId);
	bool TestTerminate(void);
	virtual HRESULT WSCCreateTexture(UINT uiWidth, UINT uiHeight, UINT uiLevels, DWORD dwUsage, FMT fmt, DWORD dwPool, CTexture **ppTexture);
	virtual HRESULT WSCCreateOffscreenPlainSurface(UINT uiWidth, UINT uiHeight, FMT fmt, DWORD dwPool, CSurface **ppSurface);
	virtual HRESULT WSCCreateRenderTarget(UINT uiWidth, UINT uiHeight, FMT fmt, MULTISAMPLE_TYPE MultiSample, DWORD dwMultisampleQuality, BOOL bLockable, CSurface **ppSurface);
	virtual HRESULT WSCCreateAdditionalSwapChain(PRESENT_PARAMETERS* pPresentationParameters, CSwapChain** ppSwapChain);

	clsTestSurface *CreateSurfaceFromDWORD(DWORD dwSpec);
	bool InitializeCompatibleSMSurface(SURFACEDESC sdDescription, clsTextureSurface *pSystemMemSurface);
	bool InitializeCompatibleMMSurface(SURFACEDESC sdDescription, clsTextureSurface *pManagedMemSurface);
	bool TestColorFillOperation(RECT rectDest, UINT uiColorIndex, float fSurfaceMinimumConformanceFraction);
	bool ReferenceColorFill(CSurface *pSurface, CONST RECT *pRect, DWORD color);
	bool DisplayFrame(void);
	HRESULT MyLoadSurfaceFromTexture(CSurface *pSysmemSurface, CTexture *pVidmemTexture);
	HRESULT MyLoadSurfaceFromRT(CSurface *pSysmemSurface, CSurface *pRTSurface);
	HRESULT MyLoadSurfaceFromOSP(CSurface *pSysmemSurface, CSurface *pOSPSurface);
	bool CompareRenderedTextures(CTexture *pReferenceTexture, CTexture *pTestTexture, float *pfSurfaceConformanceFraction);
	bool CompareOffscreenPlainToTexture(CTexture *pManagedTexture, CSurface *pOffscreenPlainSurface, float *pfSurfaceConformanceFraction);
	bool CompareRTToTexture(CTexture *pManagedTexture, CSurface *pRTSurface, float *pfSurfaceConformanceFraction);
	void ProcessError(HRESULT hr);
};

#endif
