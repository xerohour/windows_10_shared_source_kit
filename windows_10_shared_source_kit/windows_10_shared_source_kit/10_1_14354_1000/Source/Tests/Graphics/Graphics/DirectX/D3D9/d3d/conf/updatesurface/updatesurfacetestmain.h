#ifndef CUPDATESURFACETESTMAIN_INCLUDED
#define CUPDATESURFACETESTMAIN_INCLUDED

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
class CUpdateSurfaceTestMain;

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
struct _ValidSurfaces
{
	clsSurfaceSpecArray ValidSourceSpecs;
	clsSurfaceSpecArray ValidDestSpecs;
}
ValidSurfaces;

typedef
struct _FormatPair
{
	UINT uiSourceFormatIndex, uiDestFormatIndex, uiNoOfTestCases;
}
FormatPair;

typedef struct _FormatPairListElement FormatPairListElement;

struct _FormatPairListElement
{
	FormatPair FormatPairData;
	FormatPairListElement *pNext;
};

class clsFormatPairList
{
private:
	friend class clsFormatPairListScanner;
	FormatPairListElement *pFirst, *pLast;
public:
	FormatPair* AddNewElement();
	void DeleteAll();
	clsFormatPairList() {pFirst = (pLast = NULL);}
	~clsFormatPairList() {DeleteAll();}
};

class clsFormatPairListScanner
{
private:
	FormatPairListElement *pCurrentElement;
public:
	clsFormatPairListScanner() {pCurrentElement = NULL;}
	~clsFormatPairListScanner() {}
	void Reset(clsFormatPairList *pTargetList) {if (pTargetList) pCurrentElement = pTargetList -> pFirst;}
	void Advance() {if (pCurrentElement) pCurrentElement = pCurrentElement -> pNext;}
	FormatPair* GetCurrent() {return (pCurrentElement ? &(pCurrentElement -> FormatPairData) : NULL);}
};

class clsSourceSurfaceSet
{
private:
	bool bInitialized;
public:
	DWORD dwMainSurfaceSpec;
	clsTestSurface *pSourceTestSurface;
	SURFACEDESC sdSourceSurfaceDesc;
	char szDescription[cuiMaxSurfaceDescriptionLength];
	//clsTextureSurface texsurfSourceLoader;

	void ReleaseAllSurfaces();
	clsSourceSurfaceSet() {pSourceTestSurface = NULL; bInitialized = false; dwMainSurfaceSpec = 0;}
	~clsSourceSurfaceSet() {ReleaseAllSurfaces();}
	bool PrepareFromDWORD(CUpdateSurfaceTestMain *pTestEnv, DWORD dwSurfaceSpec);
};

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
	bool PrepareFromDWORD(CUpdateSurfaceTestMain *pTestEnv, DWORD dwSurfaceSpec);
	UINT GetNoOfSurfacePixels() {return uiNoOfSurfacePixels;}
};

class CUpdateSurfaceTestMain : public CD3DTest, public clsWrappedSurfaceClient
{
private:
	// Private data members
	//clsSurfaceSpecArray SourceSurfaceSpecArray, DestSurfaceSpecArray;
	//clsSurfaceSpecArray SurfaceSpecArray;

	// This is an array comprising one element for each format.  Its members specify the valid
	// source and destination surface specifications for the corresponding format.
	ValidSurfaces *ValidSurfacesByFormat;

	/*
	// This array is used to record the indices of elements in DXFilterTypes[] that represent
	// valid StretchRect filter types for the tested device.
	UINT *ValidFilterRecordIndices;
	UINT uiNoOfValidFilterRecordIndices;
	*/

	// This list is used by ExecuteTest to interpret the test case ID.  Each element represents
	// source-format/destination-format pair and the (nonzero) number of test cases associated
	// with the pair.
	clsFormatPairList TestCasesByFormatPair;



	// D3D resources that provide support for MyLoadSurfaceFromTexture.

	// Lockable surface for rendering video memory textures.
	CSurface				*psurfAuxiliaryRenderTargetTest;
	// Lockable surface for rendering the reference image (from a managed texture surface)
	CSurface				*psurfAuxiliaryRenderTargetRef;
	// Texture for the second stage, when rendering a primitive with a bump/luminance map.
	CTexture				*ptexAuxiliaryTexture;


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



	clsSourceSurfaceSet SourceSurfaceSet;
	clsDestSurfaceSet DestSurfaceSet;

	RECT rectReferenceDisplay, rectTestDisplay;

	static const float fMinimumConformanceFraction;

public:
	// Public data members
	LPVOID BitmapDataForeground;
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
	void FindValidTextureSpecs(UINT uiFormatIndex, DWORD *dwCandidateUsages, UINT uiNoOfCandidateUsages, UINT uiPoolIndex, clsSurfaceSpecList *pSpecList);
	void FindValidCubeTextureSpecs(UINT uiFormatIndex, DWORD *dwCandidateUsages, UINT uiNoOfCandidateUsages, UINT uiPoolIndex, clsSurfaceSpecList *pSpecList);
	void FindValidOffscreenPlainSpecs(UINT uiFormatIndex, UINT uiPoolIndex, clsSurfaceSpecList *pSpecList);
	void FindValidRTSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList);
	//void FindValidSurfaceSpecs();
	void FindValidSourceSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pSourceSpecArray);
	void FindValidDestSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pDestSpecArray);
	

public:
	// Public Methods
	CUpdateSurfaceTestMain();
	~CUpdateSurfaceTestMain();
	UINT TestInitialize(void);
	bool ExecuteTest(UINT uiTestCaseId);
	bool TestTerminate(void);
	virtual HRESULT WSCCreateTexture(UINT uiWidth, UINT uiHeight, UINT uiLevels, DWORD dwUsage, FMT fmt, DWORD dwPool, CTexture **ppTexture);
	virtual HRESULT WSCCreateCubeTexture(UINT uiEdgeLength, UINT uiLevels, DWORD dwUsage, FMT fmt, DWORD dwPool, CCubeTexture **ppCubeTexture);
	virtual HRESULT WSCCreateOffscreenPlainSurface(UINT uiWidth, UINT uiHeight, FMT fmt, DWORD dwPool, CSurface **ppSurface);
	virtual HRESULT WSCCreateRenderTarget(UINT uiWidth, UINT uiHeight, FMT fmt, MULTISAMPLE_TYPE MultiSample, DWORD dwMultisampleQuality, BOOL bLockable, CSurface **ppSurface);
	clsTestSurface *CreateSurfaceFromDWORD(DWORD dwSpec);
	bool InitializeCompatibleSMSurface(SURFACEDESC sdDescription, clsTextureSurface *pSystemMemSurface);
	bool InitializeCompatibleMMSurface(SURFACEDESC sdDescription, clsTextureSurface *pManagedMemSurface);
	bool TestUpdateSurfaceOperation(RECT rectSource, POINT pointDest, float fSurfaceMinimumConformanceFraction);
	bool ReferenceUpdateSurface(CSurface *pSourceSurface, RECT *pSourceRects, CSurface *pDestinationSurface, POINT *pDestPoint);
	bool DisplayFrame(void);
	HRESULT MyLoadSurfaceFromTexture(CSurface *pSysmemSurface, CTexture *pVidmemTexture);
	HRESULT MyLoadSurfaceFromCubeTexture(CSurface *pSysmemSurface, CCubeTexture *pVidmemTexture);
	HRESULT MyLoadSurfaceFromRT(CSurface *pSysmemSurface, CSurface *pRTSurface);
	HRESULT MyLoadSurfaceFromOSP(CSurface *pSysmemSurface, CSurface *pOSPSurface);
	bool CompareRenderedTextures(CTexture *pReferenceTexture, CTexture *pTestTexture, float *pfSurfaceConformanceFraction);
	bool CompareOffscreenPlainToTexture(CTexture *pManagedTexture, CSurface *pOffscreenPlainSurface, float *pfSurfaceConformanceFraction);
	bool CompareRTToTexture(CTexture *pManagedTexture, CSurface *pRTSurface, float *pfSurfaceConformanceFraction);
	bool CompareCubeTextureToTexture(CTexture *pManagedTexture, CCubeTexture *pCubeTexture, float *pfSurfaceConformanceFraction);
	bool CompareRenderedCubeMaps(CCubeTexture *pReferenceTexture, CCubeTexture *pTestTexture, float *pfSurfaceConformanceFraction);
	void ProcessError(HRESULT hr);
};

#endif
