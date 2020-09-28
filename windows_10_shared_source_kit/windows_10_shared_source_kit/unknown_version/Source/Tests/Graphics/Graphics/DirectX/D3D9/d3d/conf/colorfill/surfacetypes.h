#ifndef SURFACE_TYPES_INCLUDED
#define SURFACE_TYPES_INCLUDED

#include "FrameworkTypes.h"
#include "CSurface.h"
#include "CTextures.h"
#include "CSwapChain.h"

class clsWrappedSurfaceClient
{
public:
	clsWrappedSurfaceClient();
	~clsWrappedSurfaceClient();
	virtual HRESULT WSCCreateTexture(UINT uiWidth, UINT uiHeight, UINT uiLevels, DWORD dwUsage, FMT fmt, DWORD dwPool, CTexture **ppTexture) = 0;
	virtual HRESULT WSCCreateOffscreenPlainSurface(UINT uiWidth, UINT uiHeight, FMT fmt, DWORD dwPool, CSurface **ppSurface) = 0;
	virtual HRESULT WSCCreateRenderTarget(UINT uiWidth, UINT uiHeight, FMT fmt, MULTISAMPLE_TYPE MultiSample, DWORD dwMultisampleQuality, BOOL bLockable, CSurface **ppSurface) = 0;
	virtual HRESULT WSCCreateAdditionalSwapChain(PRESENT_PARAMETERS* pPresentationParameters, CSwapChain** ppSwapChain) = 0;

	virtual HRESULT MyLoadSurfaceFromTexture(CSurface *pSysmemSurface, CTexture *pVidmemTexture) = 0;
	virtual HRESULT MyLoadSurfaceFromRT(CSurface *pSysmemSurface, CSurface *pRTSurface) = 0;
	virtual HRESULT MyLoadSurfaceFromOSP(CSurface *pSysmemSurface, CSurface *pOSPSurface) = 0;
	virtual bool CompareRenderedTextures(CTexture *pReferenceTexture, CTexture *pTestTexture, float *pfSurfaceConformanceFraction) = 0;
	virtual bool CompareOffscreenPlainToTexture(CTexture *pManagedTexture, CSurface *pOffscreenPlainSurface, float *pfSurfaceConformanceFraction) = 0;
	virtual bool CompareRTToTexture(CTexture *pManagedTexture, CSurface *pRTSurface, float *pfSurfaceConformanceFraction) = 0;
};


class clsTestSurface
{
/*
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
*/

// Non-static "object" members.
/*
private:
	void AddToClassList();
	void RemoveFromClassList();
*/
protected:
	CSurface *lpd3dsurf;
	virtual void ReleaseSurfaceContainer() = 0;
public:
	clsTestSurface() {lpd3dsurf = NULL; /*AddToClassList();*/}
	virtual ~clsTestSurface();
	CSurface *GetSurfacePointer() {return lpd3dsurf;}
	void ReleaseDXObjects();
	virtual const char *SurfaceTypeName() = 0;
	void Describe(char *szDescription);
	virtual bool DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface) = 0;
	virtual bool VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction) = 0;
};

class clsTextureSurface:public clsTestSurface
{
protected:
	CTexture *lpd3dtex;
	virtual void ReleaseSurfaceContainer();
public:
	clsTextureSurface() {lpd3dtex = NULL;}
	~clsTextureSurface() {ReleaseDXObjects();}

	CTexture *GetTexturePointer() {return lpd3dtex;}

	virtual const char *SurfaceTypeName();
	bool Initialize(clsWrappedSurfaceClient *pWSClient, UINT uiWidth, UINT uiHeight, FMT fmt, DWORD dwUsage, DWORD dwPool);
	virtual bool DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface);
	virtual bool VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction);
};

class clsOffscreenPlainSurface:public clsTestSurface
{
protected:
	CSurface *lpd3dsurfOffscreenPlain;
	virtual void ReleaseSurfaceContainer();
public:
	clsOffscreenPlainSurface() {lpd3dsurfOffscreenPlain = NULL;}
	~clsOffscreenPlainSurface() {ReleaseDXObjects();}

	virtual const char *SurfaceTypeName();
	bool Initialize(clsWrappedSurfaceClient *pWSClient, UINT uiWidth, UINT uiHeight, FMT fmt, DWORD dwPool);
	virtual bool DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface);
	virtual bool VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction);
};

class clsRenderTargetSurface:public clsTestSurface
{
protected:
	static const char * const szWindowClassName;
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	// CSurface *lpd3dsurfRenderTarget;
	CSwapChain *pSwapChain;
	HWND hwndSwapChainWindow;

	BOOL bSurfaceLockable;
	virtual void ReleaseSurfaceContainer();
public:

	static bool bClassInitialized;
	static void InitializeClass();

	clsRenderTargetSurface() {pSwapChain = NULL; hwndSwapChainWindow = NULL;}


	//clsRenderTargetSurface() {lpd3dsurfRenderTarget = NULL;}
	~clsRenderTargetSurface() {ReleaseDXObjects();}

	virtual const char *SurfaceTypeName();
	bool Initialize(clsWrappedSurfaceClient *pWSClient, UINT uiWidth, UINT uiHeight, FMT fmt, MULTISAMPLE_TYPE mst, BOOL bLockable);
	virtual bool DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface);
	virtual bool VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction);
};

extern void InitializeSurfaceTypeClasses();

#endif
