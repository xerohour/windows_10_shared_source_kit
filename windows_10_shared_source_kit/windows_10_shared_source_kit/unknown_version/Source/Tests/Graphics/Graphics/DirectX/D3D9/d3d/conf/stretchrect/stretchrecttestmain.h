#ifndef CSTRETCHRECTTESTMAIN_INCLUDED
#define CSTRETCHRECTTESTMAIN_INCLUDED

#include <d3d9.h>
#include "Framework.h"
#include "CBuffers.h"

#include "SurfaceTypes.h"

// Disable warning 4250 ("'<X>' : inherits '<Y>::<Y>::<Z>' via dominance")
#pragma warning( disable : 4250)

static CONST UINT cuiMaxLogfilePathLength = 512;
static CONST UINT cuiMaxTestRunSummaryLength = 512;
static CONST UINT cuiMaxErrorMessageLength = 512;
static CONST UINT cuiMaxSurfaceDescriptionLength = 512;
static CONST UINT cuiMaxLogLineLength = 512;

class clsSurfaceSpecArray;
class CStretchRectTestMain;

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
	void Clear();
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
	DWORD dwMainSurfaceSpec;
public:
	clsTestSurface *pSourceTestSurface;
	SURFACEDESC sdSourceSurfaceDesc;
	char szDescription[cuiMaxSurfaceDescriptionLength];
	clsOffscreenPlainSurface texsurfSourceLoader;

	void ReleaseAllSurfaces();
	clsSourceSurfaceSet() {pSourceTestSurface = NULL; bInitialized = false; dwMainSurfaceSpec = 0;}
	~clsSourceSurfaceSet() {ReleaseAllSurfaces();}
	bool PrepareFromDWORD(CStretchRectTestMain *pTestEnv, DWORD dwSurfaceSpec);
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
    clsOffscreenPlainSurface texsurfDestLoader;
	clsTextureSurface  texsurfReference, texsurfDestCopy;

	void ReleaseAllSurfaces();
	clsDestSurfaceSet() {pDestTestSurface = NULL; bInitialized = false; dwMainSurfaceSpec = 0;}
	~clsDestSurfaceSet() {ReleaseAllSurfaces();}
	bool PrepareFromDWORD(CStretchRectTestMain *pTestEnv, DWORD dwSurfaceSpec);
	UINT GetNoOfSurfacePixels() {return uiNoOfSurfacePixels;}
};

class CStretchRectTestMain : public CD3DTest, public clsWrappedSurfaceClient
{
protected:
	// Private data members
	//clsSurfaceSpecArray SourceSurfaceSpecArray, DestSurfaceSpecArray;
	//clsSurfaceSpecArray SurfaceSpecArray;

	bool bDeviceInfoGathered;
	bool bSurfaceSpecsEnumerated;

	// This is an array comprising one element for each format.  Its members specify the valid
	// source and destination surface specifications for the corresponding format.
	ValidSurfaces *ValidSurfacesByFormat;

	// This array is used to record the indices of elements in DXFilterTypes[] that represent
	// valid StretchRect filter types for the tested device.
	UINT *ValidFilterRecordIndices;
	UINT uiNoOfValidFilterRecordIndices;

	// This list is used by ExecuteTest to interpret the test case ID.  Each element represents
	// source-format/destination-format pair and the (nonzero) number of test cases associated
	// with the pair.
	clsFormatPairList TestCasesByFormatPair;

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

	// Offscreen plain surface used to initialize back buffer 0 with the source surface
	// image.  This initialization is performed at the beginning of a test case in which
	// the source surface is a "back buffer N surface".
	clsOffscreenPlainSurface opsBBZeroInitializer;

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
	static LPVOID BitmapDataForeground;
	static LPVOID BitmapDataBackground;

	UINT uiSrcDeviceAdapterOrdinal;
	DWORD dwSrcDeviceType;
	CAPS SrcDeviceCaps;
	DISPLAYMODE displaymodeAdapter;

	UINT uiSkipDestinationTarget;
	UINT uiSkipSourceTarget;

protected:

	void InitializeTestGroup();
	void SetScreenDisplayRects();
	bool GatherDeviceInfo();

	virtual bool VerifyDeviceCompatibility();

	// Private Methods
	void ReportInvalidTCID(UINT uiTestCaseId)
	{
		WriteToLog("Unable to interpret test case identifier: %d\n", uiTestCaseId);
		SkipTestRange(uiTestCaseId, uiTestCaseId);
	}

	bool InitializeAuxiliaryResources();
	void ClearAuxiliaryResources();

	bool InitializeBBCompatibleOSP();

	void ClearFormatSurfaceSpecs();
	virtual void FindValidSurfaceSpecsPerFormat();

	virtual UINT RegisterTestableFormatPair(UINT uiSourceFormatIndex, UINT uiDestFormatIndex) = 0;

	//void FindValidSourceSurfaceSpecs();
	//void FindValidDestSurfaceSpecs();
	//void FindValidTextureSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList);
	void FindValidTextureSpecs(UINT uiFormatIndex, UINT *CandidateUsages, UINT uiNoOfCandidateUsages, UINT uiPoolIndex, clsSurfaceSpecList *pSpecList);
	void FindValidCubeTextureSpecs(UINT uiFormatIndex, UINT *CandidateUsages, UINT uiNoOfCandidateUsages, UINT uiPoolIndex, clsSurfaceSpecList *pSpecList);
	void FindValidOffscreenPlainSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList);
	void FindValidBBZeroSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList);
	void FindValidBBNSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList);

	virtual void FindValidRTSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList) = 0;
	virtual void FindValidSourceSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pSourceSpecArray) = 0;
	virtual void FindValidDestSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pDestSpecArray) = 0;

	virtual UINT EnumerateTestCases() = 0;

	static void AdjustRectForDXTn(RECT *pRect, UINT uiSurfaceWidth, UINT uiSurfaceHeight);
	static void AdjustRectForUYVY(RECT *pRect, UINT uiSurfaceWidth, UINT uiSurfaceHeight);
	static bool TestDXTn(FORMAT fmtTest);
	static bool TestUYVY(FORMAT	fmtTest);

public:
	// Public Methods
	CStretchRectTestMain();
	~CStretchRectTestMain();
	UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT uiTestCaseId) = 0;
	bool TestTerminate(void);
	virtual HRESULT WSCCreateTexture(UINT uiWidth, UINT uiHeight, UINT uiLevels, DWORD dwUsage, FMT fmt, DWORD dwPool, CTexture **ppTexture);
	virtual HRESULT WSCCreateCubeTexture(UINT uiEdgeLength, UINT uiLevels, DWORD dwUsage, FMT fmt, DWORD dwPool, CCubeTexture **ppCubeTexture);
	virtual HRESULT WSCCreateOffscreenPlainSurface(UINT uiWidth, UINT uiHeight, FMT fmt, DWORD dwPool, CSurface **ppSurface);
	virtual HRESULT WSCCreateRenderTarget(UINT uiWidth, UINT uiHeight, FMT fmt, MULTISAMPLE_TYPE MultiSample, DWORD dwMultisampleQuality, BOOL bLockable, CSurface **ppSurface);
	virtual bool WSCRetrieveBackBufferZero(CSurface **ppBackBuffer);
	virtual bool WSCRetrieveBackBufferN(CSurface **ppBackBuffer);
	clsTestSurface *CreateSurfaceFromDWORD(DWORD dwSpec);
	
    bool InitializeCompatibleSMSurface(SURFACEDESC sdDescription, clsTextureSurface *pSystemMemSurface);
    bool InitializeCompatibleSMSurface(SURFACEDESC sdDescription, clsOffscreenPlainSurface *pSystemMemSurface);
	
    bool InitializeCompatibleMMSurface(SURFACEDESC sdDescription, clsOffscreenPlainSurface *pManagedMemSurface);
	bool InitializeCompatibleMMSurface(SURFACEDESC sdDescription, clsTextureSurface *pManagedMemSurface);
	
    bool TestStretchRectOperation(RECT *prectSource, RECT *prectDest, UINT uiFilterRecordIndex, float fSurfaceMinimumConformanceFraction);
	bool ReferenceStretchRect(CSurface *pSourceSurface, CONST RECT *pSourceRect, CSurface *pDestSurface, CONST RECT *pDestRect, DWORD dwD3DXFilter);
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
	HRESULT MyRenderTexture(CTexture *ptex, CSurface *psurfRT, VIEWPORT *pVP);
	void ProcessError(HRESULT hr);
};



class CStretchRectTestNoMSRTs: virtual public CStretchRectTestMain
{
public:
	CStretchRectTestNoMSRTs();
	~CStretchRectTestNoMSRTs();
	virtual void FindValidRTSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList);
};

class CStretchRectTestMSRTs: virtual public CStretchRectTestMain
{
public:
	CStretchRectTestMSRTs();
	~CStretchRectTestMSRTs();
	virtual void FindValidRTSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList);
};

class CStretchRectTestGeneral: virtual public CStretchRectTestMain
{
public:
	CStretchRectTestGeneral();
	~CStretchRectTestGeneral();
	virtual UINT RegisterTestableFormatPair(UINT uiSourceFormatIndex, UINT uiDestFormatIndex);
	virtual UINT EnumerateTestCases();
	virtual bool ExecuteTest(UINT uiTestCaseId);
};

class CStretchRectTestMainNonMS: public CStretchRectTestNoMSRTs, public CStretchRectTestGeneral
{
public:
	CStretchRectTestMainNonMS();
	~CStretchRectTestMainNonMS();
	virtual void FindValidSourceSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pSourceSpecArray);
	virtual void FindValidDestSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pDestSpecArray);
};

class CStretchRectTestMainMS: public CStretchRectTestMSRTs, public CStretchRectTestGeneral
{
public:
	CStretchRectTestMainMS();
	~CStretchRectTestMainMS();
	virtual void FindValidSourceSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pSourceSpecArray);
	virtual void FindValidDestSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pDestSpecArray);
};

class CStretchRectTestMainLDDM: public CStretchRectTestNoMSRTs
{
public:
	CStretchRectTestMainLDDM();
	~CStretchRectTestMainLDDM();
	virtual bool VerifyDeviceCompatibility();
	virtual void FindValidSourceSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pSourceSpecArray);
	virtual void FindValidDestSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pDestSpecArray);
	virtual UINT RegisterTestableFormatPair(UINT uiSourceFormatIndex, UINT uiDestFormatIndex);
	virtual UINT EnumerateTestCases();
	virtual bool ExecuteTest(UINT uiTestCaseId);
};

#endif
