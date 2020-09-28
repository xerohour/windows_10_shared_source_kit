#ifndef CCOPYRECTS_INCLUDED
#define CCOPYRECTS_INCLUDED

#include <d3d8.h>
#include "CD3DTest.h"
#include "CSurface.h"
#include "CTextures.h"

#include "dx8fmt.h"

static CONST UINT cuiMaxLogfilePathLength = 256;
static CONST UINT cuiMaxTestRunSummaryLength = 256;
static CONST UINT cuiMaxErrorMessageLength = 256;
static CONST UINT cuiMaxSurfaceDescriptionLength = 256;
static CONST UINT cuiMaxLogLineLength = 256;

typedef enum _TestCaseOutcomeType {TCPassed, TCFailed, TCAborted} TestCaseOutcomeType;

class clsTestSurface
{
// Static "class" members.
private:
	struct TSListElement
	{
		clsTestSurface *pTestSurface;
		TSListElement *pNextElement;
	};
	static TSListElement *pFirstElement;
	static UINT uiNoOfElements;
public:
	static void ReleaseAllDX8Objects();

// Non-static "object" members.
private:
	void AddToClassList();
	void RemoveFromClassList();
protected:
	CSurface *lpd3dsurf8;
	DWORD dwSpec;
	virtual void ReleaseSurfaceContainer() = 0;
public:
	CSurface *GetSurfacePointer() {return lpd3dsurf8;}
	void ReleaseDX8Objects();
	clsTestSurface() {lpd3dsurf8 = NULL; AddToClassList();}
	virtual ~clsTestSurface() {RemoveFromClassList();}
	virtual const char *SurfaceTypeName() = 0;
	void Describe(char *szDescription);
	void SetDWORDSpec(DWORD dwSetSpec) {dwSpec = dwSetSpec;}
	DWORD GetDWORDSpec() {return dwSpec;}
	D3DMULTISAMPLE_TYPE GetMultisampleType();
};


class clsTextureSurface:public clsTestSurface
{
protected:
	CTexture *lpd3dtex8;
	virtual void ReleaseSurfaceContainer();
public:
	clsTextureSurface() {lpd3dtex8 = NULL;}
	~clsTextureSurface() {ReleaseDX8Objects();}

	virtual const char *SurfaceTypeName();
	bool Initialize(UINT uiWidth, UINT uiHeight, D3DFORMAT d3dfmt, DWORD dwUsage, D3DPOOL d3dpool);
};

class clsRenderTargetSurface:public clsTestSurface
{
protected:
	CSurface *lpd3dsurfRenderTarget;
	virtual void ReleaseSurfaceContainer();
public:
	clsRenderTargetSurface() {lpd3dsurfRenderTarget = NULL;}
	~clsRenderTargetSurface() {ReleaseDX8Objects();}

	virtual const char *SurfaceTypeName();
	bool Initialize(UINT uiWidth, UINT uiHeight, D3DFORMAT d3dfmt, D3DMULTISAMPLE_TYPE d3dmst, BOOL bLockable);
};

class clsSurfaceSpecArray;

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

class clsSourceSurfaceSet
{
private:
	bool bInitialized;
	D3DFORMAT d3dfmtSetFormat;
	DWORD dwMainSurfaceSpec;
public:
	clsTestSurface *pSourceTestSurface;
	char szDescription[cuiMaxSurfaceDescriptionLength];
	clsTextureSurface texsurfSourceLoader;

	void ReleaseAllSurfaces();
	clsSourceSurfaceSet() {pSourceTestSurface = NULL; bInitialized = false; d3dfmtSetFormat = D3DFMT_UNKNOWN; dwMainSurfaceSpec = 0;}
	~clsSourceSurfaceSet() {ReleaseAllSurfaces();}
	bool PrepareFromDWORD(DWORD dwSurfaceSpec, D3DFORMAT d3dfmt);
};

class clsDestSurfaceSet
{
private:
	bool bInitialized;
	D3DFORMAT d3dfmtSetFormat;
	DWORD dwMainSurfaceSpec;
public:
	clsTestSurface *pDestTestSurface;
	char szDescription[cuiMaxSurfaceDescriptionLength];
	clsTextureSurface texsurfDestLoader, texsurfReference, texsurfDestCopy;

	void ReleaseAllSurfaces();
	clsDestSurfaceSet() {pDestTestSurface = NULL; bInitialized = false; d3dfmtSetFormat = D3DFMT_UNKNOWN; dwMainSurfaceSpec = 0;}
	~clsDestSurfaceSet() {ReleaseAllSurfaces();}
	bool PrepareFromDWORD(DWORD dwSurfaceSpec, D3DFORMAT d3dfmt);
};

class CCopyRects: public CD3DTest
{
private:
	// Data members
	clsSurfaceSpecArray TestableSurfaceTypesByFormat[50];
	clsSourceSurfaceSet SourceSurfaceSet;
	clsDestSurfaceSet DestSurfaceSet;
	RECT rectReferenceDisplay, rectTestDisplay;

	// Methods
	void ReportInvalidTCID(UINT uiTestCaseId)
	{
		WriteToLog("Unable to interpret test case identifier: %d\n", uiTestCaseId);
		SkipTestRange(uiTestCaseId, uiTestCaseId);
	}
	void FindValidTextureSpecs(D3DFORMAT d3dfmt, clsSurfaceSpecList *pSpecList);
	void FindValidRTSpecs(D3DFORMAT d3dfmt, clsSurfaceSpecList *pSpecList);
	void InitializeSurfaceList(D3DFORMAT d3dfmt, clsSurfaceSpecArray *pSpecArray);
	bool ExecuteNumberedTest(UINT uiSystemIndependentTCID);
	bool TestCopyOperation(clsSourceSurfaceSet *pSourceSurfaceSet, clsDestSurfaceSet *pDestSurfaceSet, UINT uiRectSetIndex, DWORD dwTestCaseID);

public:
	// Data members
	LPVOID BitmapDataForeground;
	LPVOID BitmapDataBackground;
	UINT uiTestedAdapterOrdinal;
	D3DDEVTYPE d3ddevtypeTestedType;
	CONST D3DFORMAT_RECORD *pd3dfmtrecDesktop;

	// Methods
	CCopyRects();
	~CCopyRects();
	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
	HRESULT LogCreateTexture
	(
		UINT Width,
		UINT Height,
		UINT  Levels,
		DWORD Usage,
		D3DFORMAT Format,
		D3DPOOL Pool,
		CTexture **ppTexture
		//LPDIRECT3DTEXTURE8 *ppTexture
	);
	HRESULT LogCreateRenderTarget
	(
		UINT Width,
		UINT Height,
		D3DFORMAT Format,
		D3DMULTISAMPLE_TYPE MultiSample,
		BOOL Lockable,
		CSurface **ppSurface
	);

	CDevice *GetCurrentSrcDevice() {return m_pSrcDevice;}
	bool DisplayFrame(void);
	void ProcessError(HRESULT hr);
};

#endif
