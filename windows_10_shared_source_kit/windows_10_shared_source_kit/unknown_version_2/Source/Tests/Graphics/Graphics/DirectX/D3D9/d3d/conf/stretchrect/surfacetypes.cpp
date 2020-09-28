#include "SurfaceTypes.h"

#include <stdio.h>
#include "auxiliary.h"

#include "dxfmt.h"
#include "dxpool.h"
#include "dxusage.h"
#include "dxmultisample.h"

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

//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
// clsCubeMapSurface methods

void clsCubeMapSurface::ReleaseSurfaceContainer()
{
	if (lpd3dcube)
	{
		lpd3dcube -> Release();
		lpd3dcube = NULL;
	}
}

const char *clsCubeMapSurface::SurfaceTypeName()
{
	return "CubeTexture";
}

bool clsCubeMapSurface::ClearCubeSurface(CSurface *pSurface)
{
	bool bReturn = false;

	if (pSurface)
	{
		HRESULT hresult;
		SURFACEDESC sd;

		if (SUCCEEDED((hresult = pSurface -> GetDesc(&sd))))
		{
			UINT uiBitsPerHorizontalCoordinate = sd.Format.ddpfPixelFormat.dwRGBBitCount;

			if (!(uiBitsPerHorizontalCoordinate & 7))
			{
				CDevice *pDevice = NULL;

				if (SUCCEEDED((hresult = pSurface -> GetDevice(&pDevice))))
				{
					CTexture *pTemporarySysmemTexture = NULL;

					if (SUCCEEDED((hresult = pDevice -> CreateTexture(sd.Width, sd.Height, 1, 0, sd.Format, POOL_SYSTEMMEM, &pTemporarySysmemTexture))))
					{
						CSurface *pTexSurface = NULL;

						if (SUCCEEDED((hresult = pTemporarySysmemTexture -> GetSurfaceLevel(0, &pTexSurface))))
						{
							LOCKED_RECT lr;

							if (SUCCEEDED((hresult = pTexSurface -> LockRect(&lr, NULL, 0))))
							{
								UINT uiNoOfRows, uiBytesPerRow;
								BYTE *pRowStart;
								UINT uiRowsRemaining;

								uiBytesPerRow = (UINT)(((DWORD)(uiBitsPerHorizontalCoordinate * sd.Width)) >> 3);
								uiNoOfRows = sd.Height / sd.Format.ddpfPixelFormat.dwRowsPerByteSequence;

								//DPF(1, "LockRect succeeded.\n");

								// Modify bytes.
								pRowStart = (BYTE *)(lr.pBits);
								if (uiRowsRemaining = uiNoOfRows)
								do
								{
									BYTE *pRowByte = pRowStart;
									UINT uiBytesRemaining = uiBytesPerRow;

									//do {/**(pRowByte) ^= 255*/*(pRowByte) >>= 1; ++pRowByte;} while (--uiBytesRemaining);
									do {*(pRowByte++) = 0;} while (--uiBytesRemaining);
									pRowStart += lr.iPitch;
								}
								while (--uiRowsRemaining);

								/*
								UINT uiX, uiY;

								for (uiY = 0; uiY < sd.Height; ++uiY)
								{
									for (uiX = 0; uiX < sd.Width; ++uiX)
									{
										*(((BYTE *)lr.pBits) + (uiY * lr.iPitch + uiX)) = 0;
									}
								}
								*/

								pTexSurface -> UnlockRect();

								if (SUCCEEDED((hresult = pDevice -> UpdateSurface(pTexSurface, NULL, pSurface, NULL))))
									bReturn = true;
							}

							pTexSurface -> Release();
						}

						pTemporarySysmemTexture -> Release();
					}

					pDevice -> Release();
				}
			}
		}
	}

	return bReturn;
}

bool clsCubeMapSurface::Initialize(clsWrappedSurfaceClient *pWSClient, UINT uiEdgeLength, FMT fmt, DWORD dwUsage, DWORD dwPool)
{
	HRESULT hresult;

	if
	(
		SUCCEEDED
		(
			(
				hresult =
				pWSClient -> WSCCreateCubeTexture
				(
					uiEdgeLength,
					1,
					dwUsage /*| USAGE_HEAVYWEIGHT*/,
					fmt,
					dwPool,
					&lpd3dcube
				)
			)
		)
	)
	{
		CSurface *pClearSurface = NULL;

		if (SUCCEEDED((hresult = lpd3dcube -> GetCubeMapSurface(CUBEMAP_FACE_POSITIVE_X, 0, &pClearSurface))))
		{
			ClearCubeSurface(pClearSurface);
			pClearSurface -> Release();
		}


		if (SUCCEEDED((hresult = lpd3dcube -> GetCubeMapSurface(CUBEMAP_FACE_POSITIVE_Y, 0, &pClearSurface))))
		{
			ClearCubeSurface(pClearSurface);
			pClearSurface -> Release();
		}

		if (SUCCEEDED((hresult = lpd3dcube -> GetCubeMapSurface(CUBEMAP_FACE_POSITIVE_Z, 0, &pClearSurface))))
		{
			ClearCubeSurface(pClearSurface);
			pClearSurface -> Release();
		}

		if (SUCCEEDED((hresult = lpd3dcube -> GetCubeMapSurface(CUBEMAP_FACE_NEGATIVE_X, 0, &pClearSurface))))
		{
			ClearCubeSurface(pClearSurface);
			pClearSurface -> Release();
		}

		if (SUCCEEDED((hresult = lpd3dcube -> GetCubeMapSurface(CUBEMAP_FACE_NEGATIVE_Y, 0, &pClearSurface))))
		{
			ClearCubeSurface(pClearSurface);
			pClearSurface -> Release();
		}

		if (SUCCEEDED((hresult = lpd3dcube -> GetCubeMapSurface(CUBEMAP_FACE_NEGATIVE_Z, 0, &pClearSurface))))
		{
			ClearCubeSurface(pClearSurface);
			pClearSurface -> Release();
		}



		// D3DCUBEMAP_FACE_POSITIVE_X
		if (SUCCEEDED((hresult = lpd3dcube -> GetCubeMapSurface(CUBEMAP_FACE_POSITIVE_Z, 0, &lpd3dsurf))))
			return true;

		lpd3dsurf = NULL;

		lpd3dcube -> Release();
		lpd3dcube = NULL;
	}

	return false;
}

bool clsCubeMapSurface::DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface)
{
	HRESULT hresult;

	if (SUCCEEDED((hresult = pWSClient -> MyLoadSurfaceFromCubeTexture(pSysMemSurface, lpd3dcube))))
		return true;

	return false;
}

bool clsCubeMapSurface::VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction)
{
	return pWSClient -> CompareCubeTextureToTexture(pManagedTexture, lpd3dcube, pfSurfaceConformanceFraction);
}
//******************************************************************************

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

void clsRenderTargetSurface::ReleaseSurfaceContainer()
{
	if (lpd3dsurfRenderTarget)
	{
		lpd3dsurfRenderTarget -> Release();
		lpd3dsurfRenderTarget = NULL;
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

bool clsRenderTargetSurface::DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface)
{
	HRESULT hresult;

	if (SUCCEEDED((hresult = pWSClient -> MyLoadSurfaceFromRT(pSysMemSurface, lpd3dsurfRenderTarget))))
		return true;

	return false;
}

bool clsRenderTargetSurface::VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction)
{
	return pWSClient -> CompareRTToTexture(pManagedTexture, lpd3dsurfRenderTarget, pfSurfaceConformanceFraction);
}


////////////////////////////////////////////////////////////////////////////////
// clsBBZeroSurface methods

void clsBBZeroSurface::ReleaseSurfaceContainer()
{
}

const char *clsBBZeroSurface::SurfaceTypeName()
{
	return "Back Buffer Zero";
}

bool clsBBZeroSurface::Initialize(clsWrappedSurfaceClient *pWSClient)
{
	return pWSClient -> WSCRetrieveBackBufferZero(&lpd3dsurf);
}

bool clsBBZeroSurface::DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface)
{
	return false;
}

bool clsBBZeroSurface::VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction)
{
	return pWSClient -> CompareRTToTexture(pManagedTexture, lpd3dsurf, pfSurfaceConformanceFraction);
}


////////////////////////////////////////////////////////////////////////////////
// clsBBNSurface methods

void clsBBNSurface::ReleaseSurfaceContainer()
{
}

const char *clsBBNSurface::SurfaceTypeName()
{
	return "Back Buffer N";
}

bool clsBBNSurface::Initialize(clsWrappedSurfaceClient *pWSClient)
{
	return pWSClient -> WSCRetrieveBackBufferN(&lpd3dsurf);
}

bool clsBBNSurface::DumpContentToSysMem(clsWrappedSurfaceClient *pWSClient, CSurface *pSysMemSurface)
{
	return false;
}

bool clsBBNSurface::VerifyAgainstSysMemTexture(clsWrappedSurfaceClient *pWSClient, CTexture *pManagedTexture, float *pfSurfaceConformanceFraction)
{
	return false;
}
