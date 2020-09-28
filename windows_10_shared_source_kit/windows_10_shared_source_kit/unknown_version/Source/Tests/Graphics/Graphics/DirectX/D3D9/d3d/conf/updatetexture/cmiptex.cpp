#include "CUpdateTex.h"
#include "TestCases.h"
#include "UtilFormats.h"

#include <d3dx9.h>

CMipTex::CMipTex(
	CUpdateTexBase *pTestApp,
	CDevice* pDevice,
	D3DFORMAT Format, 
	D3DPOOL Pool,
	UINT uSize,
	UINT cLevels,
	DWORD dwUsage)
:
	CBaseTex(pTestApp, pDevice, D3DRTYPE_TEXTURE),
	m_pTexture(NULL)
{
	HRESULT hr = E_FAIL;

	FORMAT FrameworkFormat((FMT)Format);

	pTestApp->WriteToLog("Width: %d; Height: %d; Levels: %d\n", uSize, uSize, cLevels);


	bool bCreateTextureResult = pTestApp->CreateTexture(
		uSize + pTestApp->uiWidthAdjustment,
		uSize + pTestApp->uiHeightAdjustment,
		cLevels,
		dwUsage,
		FrameworkFormat,
		Pool,
		&m_pTexture);

	if (bCreateTextureResult != true)
	{
		pTestApp->WriteToLog("CreateTexture on main texture failed.\n");
	}

	// Create an "auxiliary" system memory texture.  The contents of this surface serve
	// as the source of "dirty rectangle" data.

	hr = (m_pTestApp->m_pSrcDevice)->CreateTexture(
		GENERICTEXSIZE,
		GENERICTEXSIZE,
		GENERICTEXLEVEL,
		0,
		FrameworkFormat,
		POOL_SYSTEMMEM,
		&m_pAuxTexture);

	if (FAILED(hr))
		pTestApp->WriteToLog("CreateTexture on auxiliary texture failed.\n");
}


BOOL CMipTex::ValidateTexture ()
{
	HRESULT hresult;
	SURFACEDESC sd;

	if (!m_pTexture)
		return FALSE;

	if (!m_pAuxTexture)
		return FALSE;

	if (m_pTexture->GetLevelDesc(0, &sd) != true)
	{
		if (m_pTestApp)
			m_pTestApp->WriteToLog("GetLevelDesc failed in CMipTex::ValidateTexture.\n");

		return FALSE;
	}

	return TRUE;
}


CMipTex::~CMipTex()
{
	SafeRelease (m_pTexture);
}


CMipTex:: operator CBaseTexture* ()
{
	return (CBaseTexture*) m_pTexture;
}


LPBASETEXTURES CMipTex::GetLPTEXTURES()
{
	return (LPBASETEXTURES)m_pTexture;
}


BOOL CMipTex::ModifyDirtyRect(RECT *pRectDest)
{
	HRESULT hresult = E_FAIL;
	SURFACEDESC sdTexSurfDesc;

	if (m_pTexture)
	// Call GetLevelDesc.  Information about the pool and format are necessary.
	if (m_pTexture->GetLevelDesc(0, &sdTexSurfDesc) != true)
	{
		m_pTestApp->WriteToLog(TEXT("CMipTex::ModifyDirtyRect: GetLevelDesc failed.\n"));
	}
	else
	{
		UINT uiBitsPerHorizontalCoordinate;

		uiBitsPerHorizontalCoordinate =
		sdTexSurfDesc.Format.ddpfPixelFormat.dwRGBBitCount;

		if (!(uiBitsPerHorizontalCoordinate & 7))
		// Verify that the operation isn't being applied to a video memory texture.
		if (sdTexSurfDesc.Pool == POOL_DEFAULT)
			m_pTestApp->WriteToLog(TEXT("CMipTex::ModifyDirtyRect called for a POOL_DEFAULT texture.\n"));
		else
		{
			UINT uiLevels;

			if (!(uiLevels = m_pTexture->GetLevelCount()))
				m_pTestApp->WriteToLog(TEXT("CMipTex::ModifyDirtyRect: GetLevelCount returned zero.\n"));
			else
			{
				RECT rectDest;
				UINT uiLevelIndex = 0;

				if (pRectDest)
					rectDest = *pRectDest;
				else
				{
					rectDest.left = (rectDest.top = 0);
					rectDest.right = sdTexSurfDesc.Width;
					rectDest.bottom = sdTexSurfDesc.Height;
				}

				for (;;)
				{
					RECT rectScaled;

					// Compute scaled rect and adjust, if necessary, for DXTn or YUV alignment
					// restrictions.

					// When the driver does not implement TEXBLT, the DX9 runtime performs an
					// UpdateTexture operation by copying the contents of the dirty rectangles
					// from the surfaces of the system memory texture to the corresponding
					// surfaces of the video memory texture.  The computations below are
					// based on the scaling performed by the runtime during this procedure.
					// (Refer to CBaseTexture::ScaleRectDown in texture.hpp.)

					// Scale the rectangle coordinates on the basis of the level index.
					// left and top are "rounded down".
					rectScaled.top = rectDest.top >> uiLevelIndex;
					rectScaled.left = rectDest.left >> uiLevelIndex;

					// right and bottom are "rounded up".
					rectScaled.right = (rectDest.right + ((1 << uiLevelIndex) - 1)) >>
					uiLevelIndex;
					rectScaled.bottom = (rectDest.bottom + ((1 << uiLevelIndex) - 1)) >>
					uiLevelIndex;

					if (bIsDXTnFormat((sdTexSurfDesc.Format).d3dfFormat))
					// Adjust the rectScaled coordinates for DXTn (4x4) alignment.
					{
						rectScaled.left &= ~3;
						rectScaled.top &= ~3;
						rectScaled.right = (rectScaled.right + 3) & (~3);
						rectScaled.bottom = (rectScaled.bottom + 3) & (~3);
					}
					else
						if (bIsUYVYFormat((sdTexSurfDesc.Format).d3dfFormat))
						// Adjust the rectScaled coordinates for YUV (2x1) alignment.
						{
							rectScaled.left &= ~1;
							rectScaled.right = (rectScaled.right + 1) & ~1;
						}

					// Obtain the dimensions of the current surface level.
					if (m_pTexture->GetLevelDesc(uiLevelIndex, &sdTexSurfDesc) != true)
					{
						m_pTestApp->WriteToLog(TEXT("CMipTex::ModifyDirtyRect: GetLevelDesc failed.\n"));
						break;
					}

					// Verify that the rectangle is valid and properly contained within
					// the surface.
					if
					(
						(rectScaled.left >= 0) &&
						(rectScaled.top >= 0) &&
						(rectScaled.right <= sdTexSurfDesc.Width) &&
						(rectScaled.bottom <= sdTexSurfDesc.Height) &&
						(rectScaled.left < rectScaled.right) &&
						(rectScaled.top < rectScaled.bottom)
					)
					{
						UINT uiNoOfRows, uiBytesPerRow;
						UINT uiDeviceIndex;
						BOOL bSurfaceUpdatedOnAllDevices;

						uiBytesPerRow = (uiBitsPerHorizontalCoordinate *
						(rectScaled.right - rectScaled.left)) >> 3;

						// For DXTn formats, the "number of rows" means the number of
						// rows of adjacent 4x4 tiles.  For other formats, it is simply
						// the height of rectScaled.

						uiNoOfRows = (rectScaled.bottom - rectScaled.top) /
						sdTexSurfDesc.Format.ddpfPixelFormat.dwRowsPerByteSequence;

						bSurfaceUpdatedOnAllDevices = TRUE;

						{
							LOCKED_RECT lr, lrAux;

							//DPF(1, "(%d, %d, %d, %d)\n", rectScaled.left, rectScaled.top, rectScaled.right, rectScaled.bottom);

							// Lock the scaled dirty rectangle.  Note that LOCK_NO_DIRTY_UPDATE is used so that
							// this surface modification step doesn't affect the set of dirty rectangles.
							if (m_pTexture->LockRect(uiLevelIndex, &lr, &rectScaled, LOCK_NO_DIRTY_UPDATE) != true)
							{
								m_pTestApp->WriteToLog(TEXT("CMipTex::ModifyDirtyRect: Unexpected LockRect failure.\n"));
								bSurfaceUpdatedOnAllDevices = FALSE;
								break;
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
								if (FAILED((hresult = m_pAuxTexture->LockRect(uiLevelIndex, &lrAux, &rectScaled, LOCK_NO_DIRTY_UPDATE))))
								{
									m_pTestApp->WriteToLog(TEXT("CMipTex::ModifyDirtyRect: Unexpected LockRect failure (aux).\n"));
									bSurfaceUpdatedOnAllDevices = FALSE;
								}
								else
								{
									BYTE *pRowStart, *pRowStartAux;
									UINT uiRowsRemaining;

									//DPF(1, "LockRect succeeded.\n");

									// Modify bytes.
									pRowStart = (BYTE *)(lr.pBits);
									pRowStartAux = (BYTE *)(lrAux.pBits);
									if (uiRowsRemaining = uiNoOfRows)
									do
									{
										BYTE *pRowByte = pRowStart;
										BYTE *pRowByteAux = pRowStartAux;
										UINT uiBytesRemaining = uiBytesPerRow;

										do {*(pRowByte++) = *(pRowByteAux++);} while (--uiBytesRemaining);
										pRowStart += lr.iPitch;
										pRowStartAux += lrAux.iPitch;
									}
									while (--uiRowsRemaining);

									m_pAuxTexture->UnlockRect(uiLevelIndex);
								}

								m_pTexture->UnlockRect(uiLevelIndex);
							}

							if (!bSurfaceUpdatedOnAllDevices)
								break;
						}

						if (!bSurfaceUpdatedOnAllDevices)
						{
							m_pTestApp->WriteToLog(TEXT("CMipTex::ModifyDirtyRect: Unable to modify texture on all devices.\n"));
							break;
						}
					}

					if ((++uiLevelIndex) == uiLevels)
						return TRUE;
				}
			}
		}
	}

	return FALSE;
}


BOOL CMipTex::SetSrcTex(CBaseTex *pTex)
{
	return TRUE;
}


//
//	Fill the texture with pattern depending on Value
//

BOOL CMipTex::SetTexValue (SURFACEVALUE Value)
{
	BOOL bRet = FALSE;
	LPSURFACES pTexSurface=NULL;
	HRESULT hr = E_FAIL;
	SURFACEDESC Desc;
	LPTEXTURES pWorkingTexture = NULL;
	LPTEXTURES pTemporarySysMemTexture = NULL;

	if (!LoadFileResources())
	{
		m_pTestApp->WriteToLog("LoadFileResources failed in CMipTex::SetSurfaceValue.\n");
		return FALSE;
	}

	m_OriginalValue = Value;
	bRet = TRUE;

	DWORD cLevels = m_pTexture->GetLevelCount();

	if (cLevels > cuiNoOfBackResources)
	{
		m_pTestApp->WriteToLog (TEXT("Internal error: Mip level count exceeds number of resources.\n"));
		return FALSE;
	}

	bool bGetLevelDescriptorResult = m_pTexture->GetLevelDesc(0, &Desc);
	if (bGetLevelDescriptorResult != true)
	{
		m_pTestApp->WriteToLog (TEXT("GetLevelDesc failed in CMipTex::SetTexValue"));
		return FALSE;
	}

	if (Desc.Pool == POOL_DEFAULT)
	{
		if (m_pTestApp->CreateTexture(Desc.dwWidth,Desc.dwHeight,cLevels,0,Desc.Format,POOL_SYSTEMMEM,&pTemporarySysMemTexture) != true)
		{
			m_pTestApp->WriteToLog (TEXT("Failed to create temporary system memory texture in CMipTex::SetTexValue.\n"));
			return FALSE;
		}

		(pWorkingTexture = pTemporarySysMemTexture)->AddRef();
	}
	else
		(pWorkingTexture = m_pTexture)->AddRef();


	// AndrewLu: If the usage flags include USAGE_AUTOGENMIPMAP, set the surface content
	// for only the top level.
	for (UINT iLevel = 0;iLevel < (((Desc.Usage) & USAGE_AUTOGENMIPMAP) ? 1 : cLevels); iLevel++)
	{
		
		bool bGetSurfaceLevelResult = pWorkingTexture->GetSurfaceLevel(iLevel, &pTexSurface);
		if (bGetSurfaceLevelResult != true)
		{
			m_pTestApp->WriteToLog (TEXT ("GetSurfaceLevel failed in CMipTex::SetTexValue\n"));
			bRet = FALSE;
			break;
		}

		//
		//	Set the surface depending on Value
		//

		bRet= SetSurfaceValue(pTexSurface, (Value == SEQ)? BackResources[iLevel]: NULL);
		SafeRelease (pTexSurface);

		// Set the auxiliary surface.
		{
			CSurface *pAuxSurface = NULL;

			if (FAILED((hr = m_pAuxTexture->GetSurfaceLevel(iLevel, &pAuxSurface))))
			{
				m_pTestApp->WriteToLog (TEXT ("GetSurfaceLevel (auxiliary) failed in CMipTex::SetTexValue\n"));
				bRet=FALSE;
			}
			else
			{
				D3DXIMAGE_INFO d3dxii;

				if
				(
					FAILED
					(
						(
							hr =
							(m_pTestApp->m_pSrcDevice) ->
							D3DXLoadSurfaceFromFileInMemory
							(
								pAuxSurface,
								NULL,
								NULL,
								ForeResources[iLevel],
								((PBITMAPFILEHEADER)ForeResources[iLevel])->bfSize,
								NULL,
								D3DX_FILTER_LINEAR | D3DX_FILTER_MIRROR,
								0,
								&d3dxii
							)
						)
					)
				)
				{
					m_pTestApp->WriteToLog("D3DXLoadSurfaceFromFileInMemory (auxiliary) failed in SetTexValue.\n");
					bRet = FALSE;
				}

				pAuxSurface->Release();
			}
		}

		if (!bRet)
			break;
	}

	SafeRelease (pTexSurface);

	if (bRet)
	{
		if (Desc.Pool == POOL_DEFAULT)
		{
			// A temporary sysmem texture was created to facilitate the initialization.
			// Use AddDirtyRect to "dirty" its entire top level and call UpdateTexture to
			// initialize the vidmem texture.

			if (pTemporarySysMemTexture->AddDirtyRect(NULL) != true)
			{
				m_pTestApp->WriteToLog (TEXT ("AddDirtyRect (on the temporary sysmem texture) failed in CMipTex::SetTexValue\n"));
				bRet = FALSE;
			}
			else
			{
				if (m_pTestApp->UpdateTexture(pTemporarySysMemTexture, m_pTexture) != true)
				{
					m_pTestApp->WriteToLog(TEXT ("UpdateTexture failed in CMipTex::SetTexValue\n"));
					bRet = FALSE;
				}
			}
		}
	}

	if (pWorkingTexture)
	{
		pWorkingTexture->Release();
		pWorkingTexture = NULL;
	}

	if (pTemporarySysMemTexture)
	{
		pTemporarySysMemTexture->Release();
		pTemporarySysMemTexture = NULL;
	}

	return bRet;
}


BOOL CMipTex::LockWithNULL(DWORD flag)
{
	LOCKED_RECT LockedRect;
	bool bLockRectangleResult = m_pTexture->LockRect(
		0,
		&LockedRect,
		NULL,
		flag
		);

	if (bLockRectangleResult == true)
	{
		bool bUnlockRectangleResult = m_pTexture->UnlockRect(0);

		// Modify the texture surface to match the dirty region.
		ModifyDirtyRect(NULL);

		return TRUE;
	}

	return FALSE;
}


BOOL CMipTex::LockManyAreas(UINT TotalRects)
{
	BOOL bRet = FALSE;
	RECT Rect;

	SURFACEDESC Desc;
	bool bGetLevelDescriptorResult = m_pTexture ->GetLevelDesc (0,&Desc);
	if (bGetLevelDescriptorResult != true)
	{
		m_pTestApp->WriteToLog (TEXT("GetLevelDesc failed in CMipTex::LockManyAreas"));
		return FALSE;
	}

	assert (TotalRects<=TOTALRECTS);

	bRet = TRUE;
	for (UINT iRect = 0; iRect < TotalRects; iRect++)
	{

		if (bIsDXTnFormat((Desc.Format).d3dfFormat))
			Rect = CDXTnMULTIRECTS[iRect];
		else
		{
			if (bIsUYVYFormat((Desc.Format).d3dfFormat))
				Rect = CUYVYMULTIRECTS[iRect];
			else
				Rect = CMULTIRECTS[iRect];
		}

		LOCKED_RECT LockedRect;
		bool bLockRectangleResult = m_pTexture->LockRect(
			0,
			&LockedRect,
			&Rect,
			0
			);

		if (bLockRectangleResult != true)
		{
			bRet = FALSE;
			break;
		}

		bool bUnlockRectangleResult = m_pTexture->UnlockRect(0);

		// Modify the texture surface to match the dirty region.
		ModifyDirtyRect(&Rect);
	}

	return bRet;
}


BOOL CMipTex::AddDirtyWithNULL()
{
	bool bAddDirtyRectangleResult = m_pTexture->AddDirtyRect(NULL);
	if (bAddDirtyRectangleResult == true)
	{
		// Modify the texture surface to match the dirty region.
		ModifyDirtyRect(NULL);

		return TRUE;
	}

	return FALSE;
}


BOOL CMipTex::AddDirtyManyAreas(UINT TotalRects)
{
	BOOL bRet = FALSE;
	RECT Rect;

	SURFACEDESC Desc;
	bool bGetLevelDescriptorResult = m_pTexture ->GetLevelDesc(0, &Desc);
	if (bGetLevelDescriptorResult != true)
	{
		m_pTestApp->WriteToLog (TEXT("GetLevelDesc failed in CMipTex::AddDirtyManyArea\n"));
		return FALSE;
	}

	assert(TotalRects <= TOTALRECTS);

	bRet = TRUE;
	for (UINT iRect=0; iRect<TotalRects;iRect++)
	{
		if (bIsDXTnFormat((Desc.Format).d3dfFormat))
			Rect=CDXTnMULTIRECTS[iRect];
		else
			if (bIsUYVYFormat((Desc.Format).d3dfFormat))
				Rect=CUYVYMULTIRECTS[iRect];
			else
				Rect=CMULTIRECTS[iRect];

		bool bAddDirtyRectangleResult = m_pTexture->AddDirtyRect(&Rect);
		if (bAddDirtyRectangleResult != true)
		{
			bRet = FALSE;
			break;
		}

		// Modify the texture surface to match the dirty region.
		ModifyDirtyRect(&Rect);
	}

	return bRet;
}


BOOL CMipTex::LockSubLevel ()
{
	DWORD cLevels = m_pTexture->GetLevelCount();

	for (UINT iLevel=1;iLevel<cLevels;iLevel++)
	{
		LOCKED_RECT LockedRect;
		bool bLockRectangleResult = m_pTexture->LockRect(
			iLevel,
			&LockedRect,
			NULL,
			0
			);

		if (bLockRectangleResult == true)
		{
			bool bUnlockRectangleResult = m_pTexture->UnlockRect (iLevel);
		}
		else 
			return FALSE;
	}

	// No dirty rects should have been recorded.
	// Modify the entire texture so that, if any part of it is erroneously copied, the
	// test case will fail.
	ModifyDirtyRect(NULL);

	return TRUE;
}


BOOL CMipTex::MakeNonSquare()
{
	SURFACEDESC Desc;
	bool bGetLevelDescriptorResult = m_pTexture->GetLevelDesc(0, &Desc);
	if (bGetLevelDescriptorResult != true)
	{
		m_pTestApp->WriteToLog (TEXT("GetLevelDesc failed in CMipTex::MakeNonSquare"));
		return FALSE;
	}

	UINT cLevels = m_pTexture->GetLevelCount();

	SafeRelease (m_pTexture);
	
	bool bCreateTextureResult = m_pTestApp->CreateTexture(
		Desc.dwWidth,
		Desc.dwHeight >> 1,
		cLevels,
		Desc.Usage,
		Desc.Format,
		Desc.Pool,
		&m_pTexture);

	if (bCreateTextureResult != true)
	{
		return FALSE;
	}

	SetTexValue(m_OriginalValue);
	return TRUE;
}


BOOL CMipTex::LockWithoutUnlock()
{
	LOCKED_RECT LockedRect;
	bool bLockRectangleResult = m_pTexture->LockRect(
		0,
		&LockedRect,
		NULL,
		0
		);

	if (bLockRectangleResult == true)
		return TRUE;
	else
		return FALSE;
}


BOOL CMipTex::Unlock()
{
	bool bUnlockRectangleResult = m_pTexture->UnlockRect(0);

	if (bUnlockRectangleResult == true)
		return TRUE;
	else
		return FALSE;

}


BOOL CMipTex::CopyRects(UINT TotalRects, D3DPOOL Pool)
{
	BOOL bRet = FALSE;
	HRESULT hr = E_FAIL;
	SURFACEDESC Desc;
	RECT Rect;
	LPSURFACES pSrcSurface = NULL;
	LPSURFACES pAnotherSurf = NULL;
	LPTEXTURES pAnotherTex = NULL;

	assert(TotalRects <= TOTALRECTS);

	bool bGetLevelDescriptorResult = m_pTexture->GetLevelDesc (0,&Desc);
	if (bGetLevelDescriptorResult != true)
	{
		m_pTestApp->WriteToLog (TEXT("GetLevelDesc failed in CMipTex::CopyRects\n"));
		return FALSE;
	}

	bool bCreateTextureResult = m_pTestApp->CreateTexture(Desc.dwWidth, Desc.dwHeight, 0, Desc.Usage, Desc.Format, Pool, &pAnotherTex);
	if (bCreateTextureResult != true)
	{
		m_pTestApp->WriteToLog (TEXT("CreateTexture failed in CMipTex::CopyRects\n"));
		return FALSE;
	}

	// Copy the source texture top level to pSurface

	bool bFirstGetSurfaceLevelResult = m_pTexture->GetSurfaceLevel(0, &pSrcSurface);
	if (bFirstGetSurfaceLevelResult != true)
	{
		m_pTestApp->WriteToLog (TEXT("GetSurfaceLevel failed in CMipTex::CopyRects\n"));
		return FALSE;
	}	

	bool bSecondGetSurfaceLevelResult = pAnotherTex->GetSurfaceLevel(0, &pAnotherSurf);
	if (bSecondGetSurfaceLevelResult != true)
	{
		m_pTestApp->WriteToLog (TEXT("GetSurfaceLevel failed in CMipTex::CopyRects\n"));
		return FALSE;
	}	


	Rect.top = 0;
	Rect.left = 0;
	Rect.right = Desc.dwWidth;
	Rect.bottom = Desc.dwHeight;

	// make pAnotherSurf look the same as pSrcSurface
	bool bFirstCopyRectanglesResult = m_pTestApp->CopyRects(pSrcSurface, &Rect, 1, pAnotherSurf, NULL);
	if (bFirstCopyRectanglesResult != true)
	{
		m_pTestApp->WriteToLog (TEXT("CopyRects failed in CMipTex::CopyRects\n"));
		return FALSE;
	}	
	

	bRet = TRUE;
	for (UINT iRect=0; iRect<TotalRects;iRect++)
	{
		if (bIsDXTnFormat((Desc.Format).d3dfFormat))
			Rect = CDXTnMULTIRECTS[iRect];
		else
			if (bIsUYVYFormat((Desc.Format).d3dfFormat))
				Rect = CUYVYMULTIRECTS[iRect];
			else
				Rect = CMULTIRECTS[iRect];

		bool bSecondCopyRectanglesResult = m_pTestApp->CopyRects(pAnotherSurf, &Rect, 1, pSrcSurface, NULL);
		if (bSecondCopyRectanglesResult != true)
		{
			m_pTestApp->WriteToLog (TEXT("CopyRects failed in CMipTex::CopyRects\n"));
			bRet = FALSE;
			break;
		}

		// Modify the texture surface to match the dirty region.
		ModifyDirtyRect(&Rect);
	}

	SafeRelease (pAnotherSurf);
	SafeRelease (pAnotherTex);
	SafeRelease (pSrcSurface);

	return bRet;
}


HRESULT CMipTex::Render(UINT uiMipLevel)
{
	//return (((UpdateTex*)m_pTestApp)->RenderEntireTexture(m_pTexture));

	return (((UpdateTex*)m_pTestApp)->RenderTextureLevelToRect(m_pTexture, uiMipLevel, ((UpdateTex*)m_pTestApp)->rectBB));
}

void CMipTex::DirtyEntireTexture()
{
	m_pTexture->AddDirtyRect(NULL);
}
