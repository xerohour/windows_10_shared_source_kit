#include "CUpdateTex.h"
#include "TestCases.h"
#include "UtilFormats.h"

#include "resource.h"

#include <d3dx9.h>

extern CD3DWindowFramework App;

LPVOID CBaseTex::BackResources[] =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

const UINT CBaseTex::cuiNoOfBackResources = sizeof(CBaseTex::BackResources) / sizeof(CBaseTex::BackResources[0]);

LPVOID CBaseTex::ForeResources[] =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

const UINT CBaseTex::cuiNoOfForeResources = sizeof(CBaseTex::ForeResources) / sizeof(CBaseTex::ForeResources[0]);

bool CBaseTex::bResourcesLoaded = false;

bool CBaseTex::LoadFileResources()
{
	if (!bResourcesLoaded)
	{
		HRSRC hrsrcFindResourceResult;
		HGLOBAL hgBitmapData;
		UINT uiIndex;

		static LPSTR BackResourceIDs[] =
		{
			MAKEINTRESOURCE(IDR_DIBFILETYPE_BACK0),
			MAKEINTRESOURCE(IDR_DIBFILETYPE_BACK1),
			MAKEINTRESOURCE(IDR_DIBFILETYPE_BACK2),
			MAKEINTRESOURCE(IDR_DIBFILETYPE_BACK3),
			MAKEINTRESOURCE(IDR_DIBFILETYPE_BACK4),
			MAKEINTRESOURCE(IDR_DIBFILETYPE_BACK5),
			MAKEINTRESOURCE(IDR_DIBFILETYPE_BACK6)
		};

		static LPSTR ForeResourceIDs[] =
		{
			MAKEINTRESOURCE(IDR_DIBFILETYPE_FORE0),
			MAKEINTRESOURCE(IDR_DIBFILETYPE_FORE1),
			MAKEINTRESOURCE(IDR_DIBFILETYPE_FORE2),
			MAKEINTRESOURCE(IDR_DIBFILETYPE_FORE3),
			MAKEINTRESOURCE(IDR_DIBFILETYPE_FORE4),
			MAKEINTRESOURCE(IDR_DIBFILETYPE_FORE5),
			MAKEINTRESOURCE(IDR_DIBFILETYPE_FORE6)
		};

		for (uiIndex = 0; uiIndex < cuiNoOfBackResources; ++uiIndex)
		{
			hrsrcFindResourceResult = FindResource(App.m_hInstance, BackResourceIDs[uiIndex],
			"DIBFILETYPE");
			hgBitmapData = LoadResource(App.m_hInstance, hrsrcFindResourceResult);
			BackResources[uiIndex] = LockResource(hgBitmapData);
		}

		for (uiIndex = 0; uiIndex < cuiNoOfForeResources; ++uiIndex)
		{
			hrsrcFindResourceResult = FindResource(App.m_hInstance, ForeResourceIDs[uiIndex],
			"DIBFILETYPE");
			hgBitmapData = LoadResource(App.m_hInstance, hrsrcFindResourceResult);
			ForeResources[uiIndex] = LockResource(hgBitmapData);
		}

		bResourcesLoaded = true;
	}

	return bResourcesLoaded;
}

CBaseTex::CBaseTex (CUpdateTexBase *pTestApp,
					CDevice* pDevice,
					D3DRESOURCETYPE ResType)
{
	m_pTestApp=pTestApp;
	m_ResType=ResType;

	m_pAuxTexture = NULL;
}



CBaseTex::~CBaseTex ()
{
	SafeRelease(m_pAuxTexture);
}

D3DRESOURCETYPE	CBaseTex::GetResType()
{
	return m_ResType;
}


BOOL CBaseTex::bIsDXTnFormat(D3DFORMAT Format)
{
	return
	(
		(Format == D3DFMT_DXT1) ||
		(Format == D3DFMT_DXT2) ||
		(Format == D3DFMT_DXT3) ||
		(Format == D3DFMT_DXT4) ||
		(Format == D3DFMT_DXT5)
	);
}

BOOL CBaseTex::bIsUYVYFormat(D3DFORMAT Format)
{
	return
	(
		(Format == D3DFMT_UYVY) ||
		(Format == D3DFMT_YUY2) ||
		(Format == D3DFMT_R8G8_B8G8) ||
		(Format == D3DFMT_G8R8_G8B8)
	);
}

//
//	Fill the dest mip texture with certain pattern depending on Value
//

BOOL CBaseTex::SetSurfaceValue(LPSURFACES pDestSurface, LPVOID lpFileInMemory)
{
	// AndrewLu; 2003/10/16; The CnSurface class interface doesn't
	// mimic that of CSurface.  It's necessary to retrieve the "wrapped" CSurface pointers
	// one-by-one and apply the operation to each.

	HRESULT hr;
	UINT uiDeviceIndex;
	BOOL bRet = TRUE;

	if (!pDestSurface)
		return FALSE;

	for (uiDeviceIndex = 0; (uiDeviceIndex < (m_pTestApp -> m_uDeviceList)) && (bRet); ++uiDeviceIndex)
	{
		CSurface *pWrappedSurface;

		if (!(pDestSurface -> GetSurface(&pWrappedSurface, uiDeviceIndex)))
			bRet = FALSE;
		else
			if (pWrappedSurface)
			{
				SURFACEDESC			Desc;

				hr = pWrappedSurface -> GetDesc (&Desc);
				if (FAILED(hr))
				{
					m_pTestApp->WriteToLog (TEXT("GetDesc failed in SetSurfaceValue\n"));
					bRet = FALSE;
				}
				else
				// AndrewLu: This method should be called to initialize only SYSTEMMEM surfaces.
				if (Desc.Pool==D3DPOOL_DEFAULT)
				{
					m_pTestApp -> WriteToLog(TEXT("Test Error: SetSurfaceValue called with D3DPOOL_DEFAULT surface argument.\n"));
					bRet = FALSE;
				}
				else
					//if (!bIsDXTnFormat((Desc.Format).d3dfFormat) || ((Desc.dwWidth >= 4) && (Desc.dwHeight >= 4)))

					// The surface is known to be lockable.  Proceed with the initialization.

					if (lpFileInMemory)
					{
						D3DXIMAGE_INFO d3dxii;
						CDevice *pSurfaceDevice = NULL;

						if (FAILED((hr = pWrappedSurface -> GetDevice(&pSurfaceDevice))))
						{
							m_pTestApp -> WriteToLog("Unable to retrieve CDevice from CSurface in SetSurfaceValue.\n");
							bRet = FALSE;
						}
						else
							if (!pSurfaceDevice)
							{
								m_pTestApp -> WriteToLog("CSurface returned NULL CDevice in SetSurfaceValue.\n");
								bRet = FALSE;
							}
							else
								// Load the surface data from a file in memory.
								if
								(
									FAILED
									(
										(
											hr =
											pSurfaceDevice ->
											D3DXLoadSurfaceFromFileInMemory
											(
												pWrappedSurface,
												NULL,
												NULL,
												lpFileInMemory,
												((PBITMAPFILEHEADER)lpFileInMemory) -> bfSize,
												NULL,
												D3DX_FILTER_LINEAR | D3DX_FILTER_MIRROR,
												0,
												&d3dxii
											)
										)
									)
								)
								{
									m_pTestApp -> WriteToLog("D3DXLoadSurfaceFromFileInMemory failed in SetSurfaceValue.\n");
									bRet = FALSE;
								}
					}
					else
					// Lock, clear and unlock the surface.
					{
						UINT uiBitsPerHorizontalCoordinate;

						uiBitsPerHorizontalCoordinate =
						((Desc.Format).ddpfPixelFormat).dwRGBBitCount;

						if (!(uiBitsPerHorizontalCoordinate & 7))
						{
							UINT uiNoOfRows, uiBytesPerRow;
							UINT uiDeviceIndex;

							uiBytesPerRow = (UINT)(((DWORD)(uiBitsPerHorizontalCoordinate *
							Desc.Width)) >> 3);

							// For DXTn formats, the "number of rows" means the number of
							// rows of adjacent 4x4 tiles.  For other formats, it is simply
							// the height of rectScaled.

							uiNoOfRows = Desc.Height /
							((Desc.Format).ddpfPixelFormat).dwRowsPerByteSequence;

							{
								LOCKED_RECT lr;

								if (FAILED((hr = pWrappedSurface -> LockRect(&lr, NULL, LOCK_NO_DIRTY_UPDATE))))
								{
									m_pTestApp -> WriteToLog(TEXT("CBaseTex::SetSurfaceValue: Unexpected LockRect failure.\n"));
									bRet = FALSE;
								}
								else
								// Determine the number of rows and the number of bytes per row involved
								// in the surface update.

								// Notwithstanding YUV and DXTn formats, the update can be performed
								// very simply if by "row" is meant "a set of texels represented by
								// consecutive bytes in memory."  For most formats, this is a row of
								// adjacent texels.  For DXTn formats, this is a row of adjacent
								// 4x4 "tiles".
								{
									BYTE *pRowStart;
									UINT uiRowsRemaining;

									//DPF(1, "LockRect succeeded.\n");

									// Modify bytes.
									pRowStart = (BYTE *)(lr.pBits);
									if (uiRowsRemaining = uiNoOfRows)
									do
									{
										BYTE *pRowByte = pRowStart;
										UINT uiBytesRemaining = uiBytesPerRow;

										do {*(pRowByte++) = 0;} while (--uiBytesRemaining);
										pRowStart += lr.iPitch;
									}
									while (--uiRowsRemaining);

									pWrappedSurface -> UnlockRect();
								}
							}
						}
					}

				pWrappedSurface -> Release();
			}

		if (!bRet)
		{
			m_pTestApp -> WriteToLog(TEXT("CMipTex::ModifyDirtyRect: Unable to modify texture on all devices.\n"));
			break;
		}
	}

	return bRet;
}

DWORD CBaseTex::FindMask (D3DFORMAT Format)
{
	int i;
	for (i=0;i<TOTALFORMATS;i++)
		if (Format==FORMATS[i].Format)
			return FORMATS[i].Mask;
	assert (FALSE);
	return 0xffffffff;
}
