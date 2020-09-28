#include "CUpdateTex.h"
#include "TestCases.h"
#include "UtilFormats.h"

#include <d3dx9.h>

CUBEMAP_FACES CCubeTex::m_CubemapFaces[] = 
{
    CUBEMAP_FACE_POSITIVE_X,
    CUBEMAP_FACE_NEGATIVE_X,
    CUBEMAP_FACE_POSITIVE_Y,
    CUBEMAP_FACE_NEGATIVE_Y,
    CUBEMAP_FACE_POSITIVE_Z,
    CUBEMAP_FACE_NEGATIVE_Z
};


CCubeTex::CCubeTex
(
	CUpdateTexBase *pTestApp,
	CDevice* pDevice,
	D3DFORMAT Format, 
	D3DPOOL Pool,
	UINT uSize,
	UINT cLevels,
	DWORD dwUsage
)
:
	m_TotalFaces(6),
	CBaseTex(pTestApp, pDevice, D3DRTYPE_CUBETEXTURE)
{
	HRESULT hr = E_FAIL;

	FORMAT FrameworkFormat((FMT)Format);

	m_pTexture = NULL;
	bool bCreateCubeTextureResult = m_pTestApp->CreateCubeTexture(
		uSize,
		cLevels,
		dwUsage,
		FrameworkFormat,
		Pool,
		&m_pTexture);

	if (bCreateCubeTextureResult != true)
		pTestApp->WriteToLog("CreateCubeTexture failed.\n");

	// Create an "auxiliary" system memory texture.  The contents of this surface serve
	// as the source of "dirty rectangle" data.

	hr = (m_pTestApp->m_pSrcDevice)->CreateTexture
	(
		GENERICTEXSIZE,
		GENERICTEXSIZE,
		GENERICTEXLEVEL,
		0,
		FrameworkFormat,
		POOL_SYSTEMMEM,
		&m_pAuxTexture
	);

	if (FAILED(hr))
		pTestApp->WriteToLog("CreateTexture (auxiliary) failed.\n");
}


BOOL CCubeTex::ValidateTexture ()
{
	if (!m_pAuxTexture)
		return FALSE;

	return (m_pTexture != NULL) ? TRUE : FALSE;
}


CCubeTex::~CCubeTex()
{
	SafeRelease (m_pTexture);
}


CCubeTex:: operator CBaseTexture* ()
{
	return (CBaseTexture*) m_pTexture;
}


LPBASETEXTURES CCubeTex::GetLPTEXTURES()
{
	return (LPBASETEXTURES)m_pTexture;
}


BOOL CCubeTex::ModifyDirtyRect(RECT *pRectDest, CUBEMAP_FACES cmFace)
{
	HRESULT hresult = E_FAIL;
	SURFACEDESC sdTexSurfDesc;

	if (m_pTexture)
	// Call GetLevelDesc.  Information about the pool and format are necessary.
	if (m_pTexture->GetLevelDesc(0, &sdTexSurfDesc) != true)
		m_pTestApp->WriteToLog(TEXT("CCubeTex::ModifyDirtyRect: GetLevelDesc failed.\n"));
	else
	{
		UINT uiBitsPerHorizontalCoordinate =
			sdTexSurfDesc.Format.ddpfPixelFormat.dwRGBBitCount;

		if (!(uiBitsPerHorizontalCoordinate & 7))
		// Verify that the operation isn't being applied to a video memory texture.
		if (sdTexSurfDesc.Pool == POOL_DEFAULT)
			m_pTestApp->WriteToLog(TEXT("CMipTex::ModifyDirtyRect called for a POOL_DEFAULT texture.\n"));
		else
		{
			UINT uiLevels = 0;

			if (!(uiLevels = m_pTexture->GetLevelCount()))
				m_pTestApp->WriteToLog(TEXT("CCubeTex::ModifyDirtyRect: GetLevelCount returned zero.\n"));
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
							if (m_pTexture->LockRect(cmFace, uiLevelIndex, &lr, &rectScaled, LOCK_NO_DIRTY_UPDATE) != true)
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
									m_pTestApp->WriteToLog(TEXT("CCube::ModifyDirtyRect: Unexpected LockRect failure (aux).\n"));
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

								m_pTexture->UnlockRect(cmFace, uiLevelIndex);
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

BOOL CCubeTex::SetSrcTex(CBaseTex *pTex)
{	
	return TRUE;
}


//
//	Fill the texture with pattern depending on Value
//

BOOL CCubeTex::SetTexValue (SURFACEVALUE Value)
{
	BOOL bRet;
	LPSURFACES pTexSurface = NULL;
	HRESULT hr;
	SURFACEDESC Desc;
	LPCUBETEXTURES pWorkingTexture = NULL;

	if (!LoadFileResources())
	{
		m_pTestApp->WriteToLog("LoadFileResources failed in CCubeTex::SetSurfaceValue.\n");
		return FALSE;
	}

	m_OriginalValue = Value;
	bRet = TRUE;

	if (!m_pTexture)
		return FALSE;

	// Because of alignment issue, Desc.size is not precisely how many bytes a surface contain, work around
	// but this doesn't work for texture with top level 1*1

	bool bGetLevelDescriptor = m_pTexture->GetLevelDesc(0, &Desc);
	if (bGetLevelDescriptor != true)
	{
		m_pTestApp->WriteToLog(TEXT("GetLevelDesc failed in CCubeTex::SetTexValue.\n"));
		return FALSE;
	}

	DWORD cLevels = m_pTexture->GetLevelCount();

	if (cLevels > cuiNoOfBackResources)
	{
		m_pTestApp->WriteToLog(TEXT("Internal error: Mip level count exceeds number of resources.\n"));
		return FALSE;
	}

	if ((Desc.Pool) == POOL_DEFAULT)
	{
		if
		(
			m_pTestApp->CreateCubeTexture
			(
				Desc.Width,
				cLevels,
				0,
				Desc.Format,
				POOL_SYSTEMMEM,
				&pWorkingTexture
			) != true
		)
		{
			m_pTestApp->WriteToLog(TEXT("CreateCubeTexture (temp sysmem) failed in CCubeTex::SetTexValue.\n"));
			return FALSE;
		}

	}
	else
		(pWorkingTexture = m_pTexture)->AddRef();

	if ((Desc.Usage) & USAGE_AUTOGENMIPMAP)
		cLevels = 1;

	// Initialize the surfaces of the auxiliary texture used for "dirty" rect initialization.
	for (UINT iLevel = 0;iLevel<cLevels; iLevel++)
	{
			CSurface *pAuxSurface = NULL;

			if (FAILED((hr = m_pAuxTexture->GetSurfaceLevel(iLevel, &pAuxSurface))))
			{
				m_pTestApp->WriteToLog(TEXT ("GetSurfaceLevel (auxiliary) failed in CCubeTex::SetTexValue\n"));
				bRet = FALSE;
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

	if (bRet)
	{
		for (UINT iFaceType = 0;iFaceType<m_TotalFaces;iFaceType++)
		{
			for (UINT iLevel = 0;iLevel<cLevels; iLevel++)
			{
				bool bGetCubeMapSurfaceResult = pWorkingTexture->GetCubeMapSurface( m_CubemapFaces[iFaceType],iLevel,&pTexSurface, CD3D_ALL);
				if (bGetCubeMapSurfaceResult != true)
				{
					m_pTestApp->WriteToLog(TEXT ("GetCubeMapSurface failed in CCubeTex::SetTexValue.\n"));
					bRet = FALSE;
					break;
				}


				//
				//	Set the surface depending on Value
				//

				bRet = SetSurfaceValue(pTexSurface, (Value == SEQ)? BackResources[iLevel]: NULL);
				SafeRelease (pTexSurface);

				if (!bRet)
					break;
			}
			if (!bRet)
				break;
		}
	}

	SafeRelease(pTexSurface);


	if (bRet)
	{
		if ((Desc.Pool) == POOL_DEFAULT)
		{
			if (m_pTestApp->UpdateTexture(pWorkingTexture, m_pTexture) != true)
			{
				bRet = FALSE;
			}
		}
	}

	if (pWorkingTexture)
	{
		pWorkingTexture->Release();
		pWorkingTexture = NULL;
	}

	return bRet;
}


BOOL CCubeTex::LockWithNULL(DWORD flag)
{
	BOOL bRet = TRUE;

	for (UINT iFaceType = 0;iFaceType<m_TotalFaces;iFaceType++)
	{
		LOCKED_RECT LockedRect;
		bool bLockRectangleResult = m_pTexture->LockRect(
			m_CubemapFaces[iFaceType],
			0,
			&LockedRect,
			NULL,
			flag
			);

		if (bLockRectangleResult == true)
		{
			bool bUnlockRectangleResult = m_pTexture->UnlockRect(m_CubemapFaces[iFaceType], 0);

			ModifyDirtyRect(NULL, m_CubemapFaces[iFaceType]);
		}
		else 
		{
			bRet = FALSE;
			break;
		}
	}

	return bRet;
}


BOOL CCubeTex::LockManyAreas(UINT TotalRects)
{
	BOOL bRet = FALSE;
	LOCKED_RECT	LockedCubeRect;
	SURFACEDESC Desc;
	RECT Rect;

	bool bGetLevelDescriptorResult = m_pTexture->GetLevelDesc (0,&Desc);
	if (bGetLevelDescriptorResult != true)
	{
		m_pTestApp->WriteToLog(TEXT("GetLevelDesc failed in CCubeTex::LockManyAreas"));
		return FALSE;
	}

	assert(TotalRects <= TOTALRECTS);


	bRet = TRUE;
	for (UINT iFaceType = 0;iFaceType<m_TotalFaces;iFaceType++)
	{
		for (UINT iCubeRect = 0; iCubeRect<TotalRects;iCubeRect++)
		{
			if (bIsDXTnFormat((Desc.Format).d3dfFormat))
				Rect = CDXTnMULTIRECTS[iCubeRect];
			else
			{
				if (bIsUYVYFormat((Desc.Format).d3dfFormat))
					Rect = CUYVYMULTIRECTS[iCubeRect];
				else
					Rect = CMULTIRECTS[iCubeRect];
			}

			bool bLockRectangleResult = m_pTexture->LockRect(
				m_CubemapFaces[iFaceType],
				0,
				&LockedCubeRect,
				&Rect,
				0
				);

			if (bLockRectangleResult != true)
			{
				bRet = FALSE;
				break;
			}


			bool bUnlockRectanlgeResult = m_pTexture->UnlockRect(m_CubemapFaces[iFaceType], 0);

			ModifyDirtyRect(&Rect, m_CubemapFaces[iFaceType]);
		}

		if (!bRet)
			break;
	}

	return bRet;
}


BOOL CCubeTex::AddDirtyWithNULL()
{
	BOOL bRet = TRUE;

	for (UINT iFaceType = 0;iFaceType<m_TotalFaces;iFaceType++)
	{
		bool bAddDirtyRectangleResult = m_pTexture->AddDirtyRect(m_CubemapFaces[iFaceType],NULL);
		if (bAddDirtyRectangleResult != true)
		{
			bRet = FALSE;
			break;
		}

		ModifyDirtyRect(NULL, m_CubemapFaces[iFaceType]);
	}

	return bRet;
}


BOOL CCubeTex::AddDirtyManyAreas(UINT TotalRects)
{
	BOOL bRet = FALSE;
	SURFACEDESC Desc;
	RECT Rect;

	bool bGetLevelDescriptorResult = m_pTexture->GetLevelDesc(0, &Desc);
	if (bGetLevelDescriptorResult != true)
	{
		m_pTestApp->WriteToLog(TEXT("GetLevelDesc failed in CCubeTex::AddDirtyManyAreas"));
		return FALSE;
	}

	assert(TotalRects <= TOTALRECTS);

	bRet = TRUE;
	for (UINT iFaceType = 0;iFaceType<m_TotalFaces;iFaceType++)
	{
		for (UINT iCubeRect = 0; iCubeRect<TotalRects;iCubeRect++)
		{
			if (bIsDXTnFormat((Desc.Format).d3dfFormat))
				Rect = CDXTnMULTIRECTS[iCubeRect];
			else
			{
				if (bIsUYVYFormat((Desc.Format).d3dfFormat))
					Rect = CUYVYMULTIRECTS[iCubeRect];
				else
					Rect = CMULTIRECTS[iCubeRect];
			}

			bool bAddDirtyRectangleResult = m_pTexture->AddDirtyRect(m_CubemapFaces[iFaceType], &Rect);
			if (bAddDirtyRectangleResult != true)
			{
				bRet = FALSE;
				break;
			}

			ModifyDirtyRect(&Rect, m_CubemapFaces[iFaceType]);
		}

		if (!bRet)
			break;
	}

	return bRet;
}


BOOL CCubeTex::LockSubLevel ()
{
	DWORD cLevels = m_pTexture->GetLevelCount();

	for (UINT iFaceType = 0;iFaceType<m_TotalFaces;iFaceType++)
	{
		for (UINT iLevel = 1;iLevel<cLevels;iLevel++)
		{
			LOCKED_RECT	LockedRect;
			bool bLockRectangleResult = m_pTexture->LockRect(
				m_CubemapFaces[iFaceType],
				iLevel,
				&LockedRect,
				NULL,
				0
				);

			if (bLockRectangleResult == true)
			{
				bool bUnlockRectangleResult = m_pTexture->UnlockRect (m_CubemapFaces[iFaceType],iLevel);
			}
			else 
				return FALSE;
		}

		// No dirty rects should have been recorded.
		// Modify the entire texture so that, if any part of it is erroneously copied, the
		// test case will fail.
		ModifyDirtyRect(NULL, m_CubemapFaces[iFaceType]);
	}

	return TRUE;
}


//
// It doesn't make sense to make a cube map nonsquare, so do nothing
//
BOOL CCubeTex::MakeNonSquare()
{
		return TRUE;
}


BOOL CCubeTex::LockWithoutUnlock()
{
	LOCKED_RECT LockedRect;
	bool bLockRectangleResult = m_pTexture->LockRect(
		m_CubemapFaces[0],
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


BOOL CCubeTex::Unlock()
{
	bool bUnlockRectangleResult = m_pTexture->UnlockRect(m_CubemapFaces[0], 0);

	if (bUnlockRectangleResult == true)
		return TRUE;
	else
		return FALSE;
}

BOOL CCubeTex::CopyRects(UINT TotalRects, D3DPOOL Pool)
{
	BOOL bRet = FALSE;
	HRESULT hr = E_FAIL;
	UINT iFaceType = 0;
	UINT iCubeRect = 0;
	SURFACEDESC Desc;
	RECT Rect;
	LPSURFACES pAnotherSurf = NULL;
	LPSURFACES pSrcSurface = NULL;
	LPCUBETEXTURES pAnotherCubeTex = NULL;

	bool bGetLevelDescriptorResult = m_pTexture->GetLevelDesc(0, &Desc);
	if (bGetLevelDescriptorResult != true)
	{
		m_pTestApp->WriteToLog(TEXT("GetLevelDesc failed in CCubeTex::AddDirtyManAreas"));
		return FALSE;
	}

	bool bCreateCubeTextureResult = m_pTestApp->CreateCubeTexture(Desc.dwWidth,0,Desc.Usage,Desc.Format,Pool,&pAnotherCubeTex);
	if (bCreateCubeTextureResult != true)
	{
		m_pTestApp->WriteToLog(TEXT("CreateCubeTexture failed in CCubeTex::CopyRects\n"));
		return FALSE;
	}

	assert(TotalRects <= TOTALRECTS);

	bRet = TRUE;
	for (iFaceType = 0;iFaceType<m_TotalFaces;iFaceType++)
	{
		// Copy the source texture top level to pSurface

		bool bGetCubeMapSurface0Result = m_pTexture->GetCubeMapSurface(m_CubemapFaces[iFaceType],0,&pSrcSurface);
		if (bGetCubeMapSurface0Result != true)
		{
			m_pTestApp->WriteToLog(TEXT("First call to GetCubeMapSurface failed in CCubeTex::CopyRects\n"));
			return FALSE;
		}	

		bool bGetCubeMapSurface1Result = pAnotherCubeTex->GetCubeMapSurface(m_CubemapFaces[iFaceType],0,&pAnotherSurf);
		if (bGetCubeMapSurface1Result != true)
		{
			m_pTestApp->WriteToLog(TEXT("Second call to GetCubeMapSurface failed in CCubeTex::CopyRects\n"));
			return FALSE;
		}	


		Rect.top = 0;
		Rect.left = 0;
		Rect.right = Desc.dwWidth;
		Rect.bottom = Desc.dwHeight;

		bool bCopyRectsResult = m_pTestApp->CopyRects(pSrcSurface,&Rect,1,pAnotherSurf,NULL);
		if (bCopyRectsResult != true)
		{
			m_pTestApp->WriteToLog(TEXT("CopyRects failed in CMipTex::CopyRects\n"));
			return FALSE;
		}	


		for (iCubeRect = 0; iCubeRect<TotalRects;iCubeRect++)
		{
			if (bIsDXTnFormat((Desc.Format).d3dfFormat))
				Rect = CDXTnMULTIRECTS[iCubeRect];
			else
			{
				if (bIsUYVYFormat((Desc.Format).d3dfFormat))
					Rect = CUYVYMULTIRECTS[iCubeRect];
				else
					Rect = CMULTIRECTS[iCubeRect];
			}

			bool bCopyRectanglesResult = m_pTestApp->CopyRects(pAnotherSurf,&Rect,1,pSrcSurface,NULL);
			if (bCopyRectanglesResult != true)
			{
				m_pTestApp->WriteToLog(TEXT("CopyRects failed in CCubeTex::CopyRects\n"));
				bRet = FALSE;
				break;
			}

			// Modify the texture surface to match the dirty region.
			ModifyDirtyRect(&Rect, m_CubemapFaces[iFaceType]);
		}

		SafeRelease (pSrcSurface);
		SafeRelease (pAnotherSurf);

		if (!bRet)
			break;

	}

	SafeRelease (pAnotherCubeTex);
	return bRet;
}

HRESULT CCubeTex::Render(UINT uiMipLevel)
{
	((UpdateTex*)m_pTestApp) ->
	RenderFaceLevelToRect(m_pTexture, uiMipLevel, ((UpdateTex*)m_pTestApp)->rectBB,
	(CUBEMAP_FACES)(((UpdateTex*)m_pTestApp)->uiCubeFaceIndex));

	/*
	((UpdateTex*)m_pTestApp)->RenderEntireFace(m_pTexture, CUBEMAP_FACE_POSITIVE_X);
	((UpdateTex*)m_pTestApp)->RenderEntireFace(m_pTexture, CUBEMAP_FACE_NEGATIVE_X);
	((UpdateTex*)m_pTestApp)->RenderEntireFace(m_pTexture, CUBEMAP_FACE_POSITIVE_Y);
	((UpdateTex*)m_pTestApp)->RenderEntireFace(m_pTexture, CUBEMAP_FACE_NEGATIVE_Y);
	((UpdateTex*)m_pTestApp)->RenderEntireFace(m_pTexture, CUBEMAP_FACE_POSITIVE_Z);
	((UpdateTex*)m_pTestApp)->RenderEntireFace(m_pTexture, CUBEMAP_FACE_NEGATIVE_Z);
	*/

	return D3D_OK;
}

void CCubeTex::DirtyEntireTexture()
{
	m_pTexture->AddDirtyRect(CUBEMAP_FACE_POSITIVE_X, NULL);
	m_pTexture->AddDirtyRect(CUBEMAP_FACE_NEGATIVE_X, NULL);
	m_pTexture->AddDirtyRect(CUBEMAP_FACE_POSITIVE_Y, NULL);
	m_pTexture->AddDirtyRect(CUBEMAP_FACE_NEGATIVE_Y, NULL);
	m_pTexture->AddDirtyRect(CUBEMAP_FACE_POSITIVE_Z, NULL);
	m_pTexture->AddDirtyRect(CUBEMAP_FACE_NEGATIVE_Z, NULL);
}
