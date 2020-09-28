#include "SurfaceTypes.h"

#include <stdio.h>
#include "auxiliary.h"

#include "dxfmt.h"
#include "dxpool.h"
#include "dxusage.h"
#include "dxmultisample.h"

#include "colorfilltest.h"

clsWrappedSurfaceClient::clsWrappedSurfaceClient()
{
}

clsWrappedSurfaceClient::~clsWrappedSurfaceClient()
{
}

/*
void clsTestSurface::ReleaseAllDX8Objects()
{
	TSListElement *pCurrentElement;

	if (pCurrentElement = pFirstElement)
		do
		{
			(pCurrentElement -> pTestSurface) -> ReleaseDX8Objects();
		}
		while (pCurrentElement = pCurrentElement -> pNextElement);
}

void clsTestSurface::AddToClassList()
{
	TSListElement *pNewElement = new TSListElement;

	pNewElement -> pTestSurface = this;
	pNewElement -> pNextElement = pFirstElement;
	pFirstElement = pNewElement;
	++uiNoOfElements;
}

void clsTestSurface::RemoveFromClassList()
{
	TSListElement **ppCurrentElement, *pCurrentElement;

	ppCurrentElement = &pFirstElement;

	if ((pCurrentElement = *ppCurrentElement) -> pTestSurface != this)
		do
		{
			ppCurrentElement = &(pCurrentElement -> pNextElement);
			pCurrentElement = *ppCurrentElement;
		}
		while (pCurrentElement -> pTestSurface != this);

	*ppCurrentElement = pCurrentElement -> pNextElement;
	delete pCurrentElement;
	--uiNoOfElements;
}
*/

clsTestSurface::~clsTestSurface()
{
	//RemoveFromClassList();
}

void clsTestSurface::ReleaseDXObjects()
{
	if (lpd3dsurf)
	{
		lpd3dsurf -> Release();
		lpd3dsurf = NULL;
	}

	ReleaseSurfaceContainer();
}

void clsTestSurface::Describe(char *szDescription)
{
	HRESULT hresult;
	SURFACEDESC d3dsd;

	if (FAILED((hresult = GetSurfacePointer() -> GetDesc(&d3dsd))))
		sprintf(szDescription, "(Unable to retrieve surface details.  GetDesc failed.)");
	else
	{
		const D3DFORMAT_RECORD *pFormatRecord;
		const D3DPOOL_RECORD *pPoolRecord;
		const D3DMULTISAMPLE_TYPE_RECORD *pMultiSampleRecord;

		pFormatRecord = FindFormatRecord((FMT)((d3dsd.Format).d3dfFormat));
		pMultiSampleRecord = FindMultiSampleRecord((MULTISAMPLE_TYPE)(d3dsd.MultiSampleType));
		pPoolRecord = FindPoolRecord(d3dsd.Pool);

		sprintf(szDescription, "Container/Type: %s; Dimensions: %dx%d; Format: %s; MultiSample Type: %s; Pool: %s; Usage Flags: ",
		SurfaceTypeName(), d3dsd.dwWidth, d3dsd.dwHeight,
		(pFormatRecord ? pFormatRecord -> szName : "(Unrecognized Format)"),
		(pMultiSampleRecord ? pMultiSampleRecord -> szName : "(Unrecognized MultiSample Type)"),
		(pPoolRecord ? pPoolRecord -> szName : "(Unrecognized Pool Type)"));
		ListUsages(d3dsd.Usage, EndOfString(szDescription));
	}
}

////////////////////////////////////////////////////////////////////////////////
// clsTextureSurface methods

void clsTextureSurface::ReleaseSurfaceContainer()
{
	if (lpd3dtex)
	{
		lpd3dtex -> Release();
		lpd3dtex = NULL;
	}
}

const char *clsTextureSurface::SurfaceTypeName()
{
	return "Texture";
}

bool clsTextureSurface::Initialize(clsWrappedSurfaceClient *pWSClient, UINT uiWidth, UINT uiHeight, FMT fmt, DWORD dwUsage, DWORD dwPool)
{
	HRESULT hresult;

	if
	(
		SUCCEEDED
		(
			(
				hresult =
				pWSClient -> WSCCreateTexture
				(
					uiWidth,
					uiHeight,
					1,
					dwUsage /*| USAGE_HEAVYWEIGHT*/,
					fmt,
					dwPool,
					&lpd3dtex
				)
			)
		)
	)
	{
		if (SUCCEEDED((hresult = lpd3dtex -> GetSurfaceLevel(0, &lpd3dsurf))))
			return true;

		lpd3dsurf = NULL;

		lpd3dtex -> Release();
		lpd3dtex = NULL;
	}

	return false;
}

bool clsTextureSurface::DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface)
{
	HRESULT hresult;

	if (SUCCEEDED((hresult = pWSClient -> MyLoadSurfaceFromTexture(pSysMemSurface, lpd3dtex))))
		return true;

	return false;
}

bool clsTextureSurface::VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction)
{
	return pWSClient -> CompareRenderedTextures(pManagedTexture, lpd3dtex, pfSurfaceConformanceFraction);
}

////////////////////////////////////////////////////////////////////////////////
// clsOffscreenPlainSurface methods

void clsOffscreenPlainSurface::ReleaseSurfaceContainer()
{
	if (lpd3dsurfOffscreenPlain)
	{
		lpd3dsurfOffscreenPlain -> Release();
		lpd3dsurfOffscreenPlain = NULL;
	}
}

const char *clsOffscreenPlainSurface::SurfaceTypeName()
{
	return "Offscreen Plain";
}

bool clsOffscreenPlainSurface::Initialize(clsWrappedSurfaceClient *pWSClient, UINT uiWidth, UINT uiHeight, FMT fmt, DWORD dwPool)
{
	HRESULT hresult;

	if
	(
		SUCCEEDED
		(
			(
				hresult =
				pWSClient -> WSCCreateOffscreenPlainSurface
				(
					uiWidth,
					uiHeight,
					fmt,
					dwPool,
					&lpd3dsurfOffscreenPlain
				)
			)
		)
	)
	{
		hresult = (lpd3dsurf = lpd3dsurfOffscreenPlain) -> AddRef();
		return true;
	}

	return false;
}

bool clsOffscreenPlainSurface::DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface)
{
	HRESULT hresult;

	if (SUCCEEDED((hresult = pWSClient -> MyLoadSurfaceFromOSP(pSysMemSurface, lpd3dsurfOffscreenPlain))))
		return true;

	return false;
}

bool clsOffscreenPlainSurface::VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction)
{
	return pWSClient -> CompareOffscreenPlainToTexture(pManagedTexture, lpd3dsurfOffscreenPlain, pfSurfaceConformanceFraction);
}

////////////////////////////////////////////////////////////////////////////////
// clsRenderTargetSurface methods

bool clsRenderTargetSurface::bClassInitialized = false;
const char * const clsRenderTargetSurface::szWindowClassName = "SwapChainWindowClass";

LRESULT CALLBACK clsRenderTargetSurface::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void clsRenderTargetSurface::InitializeClass()
{
	if (!bClassInitialized)
	{
		// Define and register a class for the swap chain window.

		WNDCLASS wndclass;

		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = (WNDPROC)(&WndProc);
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = TestApp.m_hInstance;
		wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = szWindowClassName;

		RegisterClass(&wndclass);

		bClassInitialized = true;
	}
}

void clsRenderTargetSurface::ReleaseSurfaceContainer()
{
	if (pSwapChain)
	{
		pSwapChain -> Release();
		pSwapChain = NULL;
	}

	if (hwndSwapChainWindow)
	{
		DestroyWindow(hwndSwapChainWindow);
		hwndSwapChainWindow = NULL;
	}
}

const char *clsRenderTargetSurface::SurfaceTypeName()
{
	if (bSurfaceLockable)
		return "Render Target (Lockable)";
	else
		return "Render Target (Not Lockable)";
}

bool clsRenderTargetSurface::Initialize(clsWrappedSurfaceClient *pWSClient, UINT uiWidth, UINT uiHeight, FMT fmt, MULTISAMPLE_TYPE mst, BOOL bLockable)
{
	HRESULT hresult;
	PRESENT_PARAMETERS FmwkPresentParams;

	FORMAT fmtBB(fmt);

	ReleaseDXObjects();

	// Create a window for the swap chain.

	hwndSwapChainWindow =
	CreateWindow
	(
		szWindowClassName,
		"Swap Chain Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		32,
		32,
		NULL,
		NULL,
		TestApp.m_hInstance,
		NULL
	);

	ShowWindow(hwndSwapChainWindow, SW_SHOWNORMAL);
	UpdateWindow(hwndSwapChainWindow);

	// Initialize the PRESENT_PARAMETERS structure.

	FmwkPresentParams.uWidth = uiWidth;
	FmwkPresentParams.uHeight = uiHeight;
	FmwkPresentParams.BackBufferFormat = fmtBB;
	FmwkPresentParams.uBackBufferCount = 1;
	FmwkPresentParams.MultiSample = mst;
    FmwkPresentParams.MultiSampleQuality = 0;
	FmwkPresentParams.SwapEffect = SWAPEFFECT_DISCARD;
	FmwkPresentParams.hDeviceWindow = hwndSwapChainWindow;
	FmwkPresentParams.bWindowed = true;
	FmwkPresentParams.bAutoDepthStencil = false;
	FmwkPresentParams.bCachedBackBuffers = false;	// Do the BackBuffers get cached
	//FmwkPresentParams.AutoDepthStencilFormat;
	FmwkPresentParams.dwFlags = (bLockable ? (DWORD)PRESENTFLAG_LOCKABLE_BACKBUFFER: (DWORD)0);
	FmwkPresentParams.uFullscreenRefresh = 0;

	FmwkPresentParams.uFullscreenPresentInterval = PRESENT_INTERVAL_DEFAULT;

	//FmwkPresentParams.dwInternalFlags;
	//FmwkPresentParams.DesktopFormat;		// Format of the mode the device will be created in

	if (SUCCEEDED((hresult = pWSClient -> WSCCreateAdditionalSwapChain(&FmwkPresentParams, &pSwapChain))))
	{
		DebugOut.Write("CreateAdditionalSwapChain succeeded.\n");

		if (SUCCEEDED((hresult = pSwapChain -> GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &lpd3dsurf))))
			DebugOut.Write("GetBackBuffer succeeded.\n");

		return true;
	}

	/*
	if
	(
		SUCCEEDED
		(
			(
				hresult =
				pWSClient -> WSCCreateRenderTarget
				(
					uiWidth,
					uiHeight,
					fmt,
					mst,
					0, // For now, default to a multisample quality level of 0.
					bLockable,
					&lpd3dsurfRenderTarget
				)
			)
		)
	)
	{
		hresult = (lpd3dsurf = lpd3dsurfRenderTarget) -> AddRef();
		bSurfaceLockable = bLockable;
		return true;
	}
	*/

	return false;
}

/*
bool clsRenderTargetSurface::Initialize(clsWrappedSurfaceClient *pWSClient, UINT uiWidth, UINT uiHeight, FMT fmt, MULTISAMPLE_TYPE mst, BOOL bLockable)
{
	HRESULT hresult;
	PRESENT_PARAMETERS FmwkPresentParams;

	ReleaseDXObjects();

	// Create a window for the swap chain.

	hwndSwapChainWindow =
	CreateWindow
	(
		szWindowClassName,
		"Swap Chain Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		TestApp.m_hInstance,
		NULL
	);

	ShowWindow(hwndSwapChainWindow, SW_SHOWNORMAL);
	UpdateWindow(hwndSwapChainWindow);

	// Initialize the PRESENT_PARAMETERS structure.

	//FmwkPresentParams

	if (SUCCEEDED((hresult = pWSClient -> WSCCreateAdditionalSwapChain(&FmwkPresentParams, &pSwapChain))))
	{
		return true;
	}

	if
	(
		SUCCEEDED
		(
			(
				hresult =
				pWSClient -> WSCCreateRenderTarget
				(
					uiWidth,
					uiHeight,
					fmt,
					mst,
					0, // For now, default to a multisample quality level of 0.
					bLockable,
					&lpd3dsurfRenderTarget
				)
			)
		)
	)
	{
		hresult = (lpd3dsurf = lpd3dsurfRenderTarget) -> AddRef();
		bSurfaceLockable = bLockable;
		return true;
	}

	return false;
}
*/

bool clsRenderTargetSurface::DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface)
{
	HRESULT hresult;

	//if (SUCCEEDED((hresult = pWSClient -> MyLoadSurfaceFromRT(pSysMemSurface, lpd3dsurfRenderTarget))))
	if (SUCCEEDED((hresult = pWSClient -> MyLoadSurfaceFromRT(pSysMemSurface, lpd3dsurf))))
		return true;

	return false;
}

bool clsRenderTargetSurface::VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction)
{
	//return pWSClient -> CompareRTToTexture(pManagedTexture, lpd3dsurfRenderTarget, pfSurfaceConformanceFraction);
	return pWSClient -> CompareRTToTexture(pManagedTexture, lpd3dsurf, pfSurfaceConformanceFraction);
}

////////////////////////////////////////////////////////////////////////////////
// Module's public functions


void InitializeSurfaceTypeClasses()
{
	clsRenderTargetSurface::InitializeClass();
}
