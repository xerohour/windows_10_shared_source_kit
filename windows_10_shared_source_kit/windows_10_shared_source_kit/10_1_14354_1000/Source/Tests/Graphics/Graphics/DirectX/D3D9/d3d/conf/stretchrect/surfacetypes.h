#ifndef SURFACE_TYPES_INCLUDED
#define SURFACE_TYPES_INCLUDED

#include "FrameworkTypes.h"
#include "CSurface.h"
#include "CTextures.h"

class clsWrappedSurfaceClient
{
public:
	clsWrappedSurfaceClient();
	~clsWrappedSurfaceClient();
	virtual HRESULT WSCCreateTexture(UINT uiWidth, UINT uiHeight, UINT uiLevels, DWORD dwUsage, FMT fmt, DWORD dwPool, CTexture **ppTexture) = 0;
	virtual HRESULT WSCCreateCubeTexture(UINT uiEdgeLength, UINT uiLevels, DWORD dwUsage, FMT fmt, DWORD dwPool, CCubeTexture **ppCubeTexture) = 0;
	virtual HRESULT WSCCreateOffscreenPlainSurface(UINT uiWidth, UINT uiHeight, FMT fmt, DWORD dwPool, CSurface **ppSurface) = 0;
	virtual HRESULT WSCCreateRenderTarget(UINT uiWidth, UINT uiHeight, FMT fmt, MULTISAMPLE_TYPE MultiSample, DWORD dwMultisampleQuality, BOOL bLockable, CSurface **ppSurface) = 0;
	virtual bool WSCRetrieveBackBufferZero(CSurface **ppBackBuffer) = 0;
	virtual bool WSCRetrieveBackBufferN(CSurface **ppBackBuffer) = 0;

	virtual HRESULT MyLoadSurfaceFromTexture(CSurface *pSysmemSurface, CTexture *pVidmemTexture) = 0;
	virtual HRESULT MyLoadSurfaceFromCubeTexture(CSurface *pSysmemSurface, CCubeTexture *pVidmemTexture) = 0;
	virtual HRESULT MyLoadSurfaceFromRT(CSurface *pSysmemSurface, CSurface *pRTSurface) = 0;
	virtual HRESULT MyLoadSurfaceFromOSP(CSurface *pSysmemSurface, CSurface *pOSPSurface) = 0;
	virtual bool CompareRenderedTextures(CTexture *pReferenceTexture, CTexture *pTestTexture, float *pfSurfaceConformanceFraction) = 0;
	virtual bool CompareOffscreenPlainToTexture(CTexture *pManagedTexture, CSurface *pOffscreenPlainSurface, float *pfSurfaceConformanceFraction) = 0;
	virtual bool CompareRTToTexture(CTexture *pManagedTexture, CSurface *pRTSurface, float *pfSurfaceConformanceFraction) = 0;
	virtual bool CompareCubeTextureToTexture(CTexture *pManagedTexture, CCubeTexture *pCubeTexture, float *pfSurfaceConformanceFraction) = 0;
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
	CSurface *lpd3dsurfRenderTarget;
	BOOL bSurfaceLockable;
	virtual void ReleaseSurfaceContainer();
public:
	clsRenderTargetSurface() {lpd3dsurfRenderTarget = NULL;}
	~clsRenderTargetSurface() {ReleaseDXObjects();}

	virtual const char *SurfaceTypeName();
	bool Initialize(clsWrappedSurfaceClient *pWSClient, UINT uiWidth, UINT uiHeight, FMT fmt, MULTISAMPLE_TYPE mst, BOOL bLockable);
	virtual bool DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface);
	virtual bool VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction);
};

class clsCubeMapSurface:public clsTestSurface
{
protected:
	CCubeTexture *lpd3dcube;
	virtual void ReleaseSurfaceContainer();
	bool ClearCubeSurface(CSurface *pSurface);
public:
	clsCubeMapSurface() {lpd3dcube = NULL;}
	~clsCubeMapSurface() {ReleaseDXObjects();}

	virtual const char *SurfaceTypeName();
	bool Initialize(clsWrappedSurfaceClient *pWSClient, UINT uiEdgeLength, FMT fmt, DWORD dwUsage, DWORD dwPool);
	virtual bool DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface);
	virtual bool VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction);
};

class clsBBZeroSurface: public clsTestSurface
{
protected:
	virtual void ReleaseSurfaceContainer();
public:
	clsBBZeroSurface() {}
	~clsBBZeroSurface() {ReleaseDXObjects();}

	virtual const char *SurfaceTypeName();
	bool Initialize(clsWrappedSurfaceClient *pWSClient);
	virtual bool DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface);
	virtual bool VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction);
};

class clsBBNSurface: public clsTestSurface
{
protected:
	virtual void ReleaseSurfaceContainer();
public:
	clsBBNSurface() {}
	~clsBBNSurface() {ReleaseDXObjects();}

	virtual const char *SurfaceTypeName();
	bool Initialize(clsWrappedSurfaceClient *pWSClient);
	virtual bool DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface);
	virtual bool VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction);
};

#endif
