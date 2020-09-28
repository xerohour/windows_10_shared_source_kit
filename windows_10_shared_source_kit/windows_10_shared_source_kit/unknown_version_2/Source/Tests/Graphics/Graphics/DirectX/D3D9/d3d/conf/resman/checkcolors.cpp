#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "ResMan.h"

//-----------------------------------------------------------------------------

bool ResMan::CheckColors(CSurface *pSrcSurf, DWORD primColor, void *pBuf, int nPitch, D3DCOLOR *pColors, UINT index, UINT wnum, UINT hnum)
{
	HRESULT   hr       = S_OK;
	bool      result   = true;
	TCHAR	  szErrorMessage[ MAX_ERROR_MESSAGE ];
	DWORD     Stripe[] = { primColor };
	CImage   *pImage   = NULL;
	CSurface *pSurf    = NULL;
	UINT      uWidth   = m_Options.D3DOptions.nBackBufferWidth;
	UINT      uHeight  = m_Options.D3DOptions.nBackBufferHeight;
	FLOAT	  wcell    = 2.f/wnum, hcell = 2.f/hnum;
	D3DCOLOR  color    = 0;
	LONG      position[4];

 	//if Colors == NULL this means that we only use this surface once (in Basic test for example) and thus we don't need to keep its color
	//if Colors[0] is black then it means that this surface in used 1st time and we need to record its color
	//otherwise we just retrive its color to use in CheckColorsInRect()
	if ( pColors == NULL || pColors[0] == 0 )
	{
		hr = m_pSrcDevice->CreateImageSurface(4, 4, m_ScreenFormat, &pSurf );
		if (FAILED(hr))
		{
			WriteToLog("CheckColors: CreateImageSurface failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		pImage = new CImage;

		if (!pImage->LoadStripes( 4, 4, 1, Stripe, false, false ))
		{
			WriteToLog("CheckColors: Out of memory on LoadStripes.\n");
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		hr = pSurf->UpdateSurface(pImage);
		if (FAILED(hr))
		{
			WriteToLog("CheckColors: UpdateSurface() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}
	else
		color = pColors[0];

	FLOAT x1 = 1-(index%wnum)*wcell;  
	FLOAT x2 = 1-(index%wnum+1)*wcell;
	FLOAT y1 = 1-(index/wnum)*hcell;
	FLOAT y2 = 1-(index/wnum+1)*hcell;
	x1 = (uWidth/2)*(1-x1)+1.5f;
	x2 = (uWidth/2)*(1-x2)+0.5f;
	y1 = (uHeight/2)*(1-y1)+1.5f;
	y2 = (uHeight/2)*(1-y2)+0.5f;

	position[0] = (LONG)x1;
	position[1] = (LONG)y1;
	position[2] = (LONG)x2;
	position[3] = (LONG)y2;

	szErrorMessage[ 0 ] = '\0';
	
	if ( !CheckColorsInRect(pSrcSurf, pBuf, nPitch, pSurf, &color, NULL, position, szErrorMessage, NULL ) )
	{
		result = false;
		WriteToLog( szErrorMessage );
	}

	if ( pColors != NULL )
		pColors[0] = color;

Exit:
	if (FAILED(hr))
	{
		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			SkipTestRange(m_uTestNumber, m_uTestNumber);
		}
		else
		{
			Fail();
			result = false;
		}
	}

	RELEASE(pSurf);
	SAFEDELETE(pImage);
		
	return result;
}

//-----------------------------------------------------------------------------

bool ResMan::CheckTexture(CSurface *pSrcSurf, void *pBuf, int nPitch, CTexture *pTexture, bool *pCheckColor, D3DCOLOR *pColors, UINT startIndex, UINT wnum, UINT hnum)
{
	HRESULT       hr       = S_OK;
	bool          result   = true;
	bool		  bFirstSliceWhite = false;
	UINT		  uCountWhiteSlices = 0;
	TCHAR		  szErrorMessage[ MAX_ERROR_MESSAGE ];
	CSurface     *pTexSurf = NULL;
	CSurface	 *pNewSurf = NULL;
	PALETTEENTRY *pPal     = NULL;
	CTexture	 *tempTex  = NULL;
	UINT          uWidth   = m_Options.D3DOptions.nBackBufferWidth;
	UINT          uHeight  = m_Options.D3DOptions.nBackBufferHeight;
	UINT          uLevels;
	FLOAT	      wcell = 2.f/wnum, hcell = 2.f/hnum;
	LONG          position[4];
	SURFACEDESC   Desc;
	
	if (NULL == pTexture)
		return false;

	pTexture->GetLevelDesc(0, &Desc);
	uLevels = pTexture->GetLevelCount();
	szErrorMessage[ 0 ] = '\0';

	//if Colors == NULL this means that we only use this surface once (in Basic test for example) and thus we don't need to keep its color
	//if Colors[0] is black then it means that this surface in used 1st time and we need to record its color
	//otherwise we just retrive its color to use in CheckColorsInRect()
	if ( pColors == NULL || pColors[0] == 0 )
	{
		hr = m_pSrcDevice->CreateTexture(Desc.Width, Desc.Height, uLevels, 0, m_ScreenFormat, POOL_MANAGED, &tempTex);
		if (FAILED(hr))
		{
			WriteToLog("CheckTexture: CreateTexture() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
		
		if (pTexture->IsPalettized())
		{
			DWORD numOfEntries = 1 << Desc.Format.ddpfPixelFormat.dwRGBBitCount;
			
			pPal = new PALETTEENTRY[numOfEntries];
			if (NULL == pPal)
			{
				WriteToLog("CheckTexture: Out of memory on allocating palette.\n");
				hr = E_OUTOFMEMORY;
				goto Exit;
			}

			ZeroMemory(pPal, sizeof(PALETTEENTRY) * numOfEntries);

			if ( m_dwVersion >= 0x0800 )
			{
				hr = m_pSrcDevice->GetPaletteEntries(0, pPal);
				if (FAILED(hr))
				{
					WriteToLog("CheckTexture: GetPaletteEntries() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
					goto Exit;
				}
			}
			else
			{
				hr = pTexture->GetSurfaceLevel( 0, &pTexSurf );
				if (FAILED(hr))
				{
					WriteToLog("CheckTexture: GetSurfaceLevel() failed on level %d hr=%s(%x).\n", 0, m_pD3D->HResultToString(hr), hr);
					goto Exit;
				}

				hr = pTexSurf->GetPalette(pPal, numOfEntries);
				if (FAILED(hr))
				{
					WriteToLog("CheckTexture: GetPalette() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
					goto Exit;
				}
			}
		}
	}

	for (UINT LevelIndex = startIndex; LevelIndex < startIndex + uLevels; LevelIndex++)
	{
		RELEASE(pTexSurf);
		RELEASE(pNewSurf);

		if (NULL != pCheckColor && false == pCheckColor[LevelIndex - startIndex])
			continue;

		FLOAT x1 = 1-(LevelIndex%wnum)*wcell;  
		FLOAT x2 = 1-(LevelIndex%wnum+1)*wcell;
		FLOAT y1 = 1-(LevelIndex/wnum)*hcell;
		FLOAT y2 = 1-(LevelIndex/wnum+1)*hcell;
		x1 = (uWidth/2)*(1-x1)+1.5f;
		x2 = (uWidth/2)*(1-x2)+0.5f;
		y1 = (uHeight/2)*(1-y1)+1.5f;
		y2 = (uHeight/2)*(1-y2)+0.5f;
		position[0] = (LONG)x1;
		position[1] = (LONG)y1;
		position[2] = (LONG)x2;
		position[3] = (LONG)y2;

		if ( NULL == pColors || 0 == pColors[LevelIndex - startIndex] )
		{
			ASSERT(NULL != tempTex);
			hr = pTexture->GetSurfaceLevel( LevelIndex-startIndex, &pTexSurf );
			if (FAILED(hr))
			{
				WriteToLog("CheckTexture: GetSurfaceLevel() failed on pTexture on level %d hr=%s(%x).\n", LevelIndex-startIndex, m_pD3D->HResultToString(hr), hr);
				goto Exit;
			}

			hr = tempTex->GetSurfaceLevel( LevelIndex-startIndex, &pNewSurf );
			if (FAILED(hr))
			{
				WriteToLog("CheckTexture: GetSurfaceLevel() failed on tempTex on level %d hr=%s(%x).\n", LevelIndex-startIndex, m_pD3D->HResultToString(hr), hr);
				goto Exit;
			}

			if ( !ConvertSurf( pSrcSurf, pBuf, nPitch, pTexSurf, pNewSurf, pColors ? &(pColors[LevelIndex - startIndex]) : NULL, pPal, position, szErrorMessage, &uCountWhiteSlices ))
			{
				result = false;
				_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sCheckTexture: failed on level %d.\n"), szErrorMessage, LevelIndex - startIndex );
				//WriteToLog("CheckTexture: failed on level %d.\n", LevelIndex-startIndex);

				if(NULL != pCheckColor && !KeySet("logallfailures"))
					pCheckColor[LevelIndex - startIndex] = false;
			}
			
			if ( LevelIndex == startIndex && uCountWhiteSlices )
				bFirstSliceWhite = true;
		}
		else
		{
			if ( !CheckColorsInRect( pSrcSurf, pBuf, nPitch, NULL, pColors ? &(pColors[LevelIndex - startIndex]) : NULL, NULL, position, szErrorMessage, &uCountWhiteSlices ))
			{
				result = false;
				_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sCheckTexture: failed on level %d.\n"), szErrorMessage, LevelIndex - startIndex );
				//WriteToLog("CheckTexture: failed on level %d.\n", LevelIndex-startIndex);

				if(NULL != pCheckColor && !KeySet("logallfailures"))
					pCheckColor[LevelIndex - startIndex] = false;
			}
			if ( LevelIndex == startIndex && uCountWhiteSlices )
				bFirstSliceWhite = true;
		}
	}

	//bug 71971 first slice is white
	//bug 71683 all slices are white
	if ( !KeySet(_T("logallfailures")) && m_pD3D->GetRuntimeVersion() < 9.029f && uLevels > 1 && ((bFirstSliceWhite && uCountWhiteSlices == 1) || uCountWhiteSlices == uLevels ))
		result = true;
	else
	{
		TCHAR *pTempMessage = szErrorMessage;//[ MAX_ERROR_MESSAGE ];
		TCHAR *pSubStr;
		TCHAR szTemp[ MAX_ERROR_MESSAGE ];
		while ( NULL != (pSubStr = strstr( pTempMessage, _T("\n") )))
		{
			UINT len = strlen( pTempMessage ) - strlen( pSubStr );
			len = min( len + 1, MAX_ERROR_MESSAGE  - 1 );
			strncpy( szTemp, pTempMessage, len );
			szTemp[ len ] = '\0';
		
			WriteToLog( szTemp );
			pTempMessage += len;
		}
		if ( strlen( pTempMessage ))
			WriteToLog( pTempMessage );
	}

Exit:
	if (FAILED(hr))
	{
		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			SkipTestRange(m_uTestNumber, m_uTestNumber);
		}
		else
		{
			Fail();
			result = false;
		}
	}

	if (NULL != pPal)
		delete [] pPal;
	RELEASE(pTexSurf);
	RELEASE(pNewSurf);
	RELEASE(tempTex);

	return result;
}

//-----------------------------------------------------------------------------

bool ResMan::CheckCubeMap(CSurface *pSrcSurf, void *pBuf, int nPitch, CCubeTexture *pCubeMap, bool *pCheckColor, D3DCOLOR *pColors, UINT startIndex, UINT wnum, UINT hnum)
{
	HRESULT       hr           = S_OK;
	bool          result       = true;
	bool		  bFirstSliceWhite = false;
	UINT		  uCountWhiteSlices = 0;
	TCHAR		  szErrorMessage[ MAX_ERROR_MESSAGE ];
	PALETTEENTRY *pPal         = NULL;
	CCubeTexture *tempCubeTex  = NULL;
	CSurface     *pCubeMapFace = NULL;
	CSurface     *pNewSurf     = NULL;
	UINT          uWidth  = m_Options.D3DOptions.nBackBufferWidth;
	UINT          uHeight = m_Options.D3DOptions.nBackBufferHeight;
	UINT          uLevels;
	FLOAT		  wcell = 2.f/wnum, hcell = 2.f/hnum;
	LONG          position[4];
	SURFACEDESC   pDesc;

	if (NULL == pCubeMap)
		return false;

	pCubeMap->GetLevelDesc(0, &pDesc);
	uLevels = pCubeMap->GetLevelCount();
	szErrorMessage[ 0 ] = '\0';

	//if Colors == NULL this means that we only use this surface once (in Basic test for example) and thus we don't need to keep its color
	//if Colors[0] is black then it means that this surface in used 1st time and we need to record its color
	//otherwise we just retrive its color to use in CheckColorsInRect()
	if ( pColors == NULL || pColors[0] == 0 )
	{
		hr = m_pSrcDevice->CreateCubeTexture( pDesc.Width, uLevels, 0, m_ScreenFormat, POOL_MANAGED, &tempCubeTex );
		if (FAILED(hr))
		{
			WriteToLog("CheckCubeMap: CreateCubeTexture() failed on tempCubeTex hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
		
		if (pCubeMap->IsPalettized())
		{
			DWORD numOfEntries = 1 << pDesc.Format.ddpfPixelFormat.dwRGBBitCount;
			
			pPal = new PALETTEENTRY[numOfEntries];
			if (NULL == pPal)
			{
				WriteToLog("CheckCubeMap: Out of memory on allocating pPal.\n");
				hr = E_OUTOFMEMORY;
				goto Exit;
			}

			ZeroMemory(pPal, sizeof(PALETTEENTRY) * numOfEntries);

			if ( m_dwVersion >= 0x0800 )
			{
				hr = m_pSrcDevice->GetPaletteEntries(0, pPal);
				if (FAILED(hr))
				{
					WriteToLog("CheckCubeMap: GetPaletteEntries() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
					goto Exit;
				}
			}
			else
			{
				hr = pCubeMap->GetCubeMapSurface( (CUBEMAP_FACES)0, 0, &pCubeMapFace );
				if (FAILED(hr))
				{
					WriteToLog("CheckCubeMap: GetCubeMapSurface() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
					goto Exit;
				}

				hr = pCubeMapFace->GetPalette(pPal, numOfEntries);
				if (FAILED(hr))
				{
					WriteToLog("CheckCubeMap: GetPalette() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
					goto Exit;
				}
			}
		}
	}

	for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++)
	{
		for (UINT FaceIndex = 0; FaceIndex < 6; FaceIndex++)
		{
			RELEASE(pCubeMapFace);
			RELEASE(pNewSurf);

			if (NULL != pCheckColor && false == pCheckColor[LevelIndex*6 + FaceIndex])
				continue;

			FLOAT x1 = 1-((startIndex+6*LevelIndex+FaceIndex)%wnum  )*wcell;  
			FLOAT y1 = 1-((startIndex+6*LevelIndex+FaceIndex)/wnum  )*hcell;
			FLOAT x2 = 1-((startIndex+6*LevelIndex+FaceIndex)%wnum+1)*wcell;
			FLOAT y2 = 1-((startIndex+6*LevelIndex+FaceIndex)/wnum+1)*hcell;
			x1 = (uWidth/2)*(1-x1)+1.5f;
			y1 = (uHeight/2)*(1-y1)+1.5f;
			x2 = (uWidth/2)*(1-x2)+0.5f;
			y2 = (uHeight/2)*(1-y2)+0.5f;
			position[0] = (LONG)x1;
			position[1] = (LONG)y1;
			position[2] = (LONG)x2;
			position[3] = (LONG)y2;

			if ( pColors == NULL || pColors[LevelIndex*6 + FaceIndex] == 0 )
			{
				ASSERT(NULL != tempCubeTex);
				hr = pCubeMap->GetCubeMapSurface( (CUBEMAP_FACES)FaceIndex, LevelIndex, &pCubeMapFace );
				if (FAILED(hr))
				{
					WriteToLog("CheckCubeMap: GetCubeMapSurface() failed on pCubeMap on level %d and face %d hr=%s(%x).\n", LevelIndex, FaceIndex, m_pD3D->HResultToString(hr), hr);
					goto Exit;
				}

				hr = tempCubeTex->GetCubeMapSurface( (CUBEMAP_FACES)FaceIndex, LevelIndex, &pNewSurf );
				if (FAILED(hr))
				{
					WriteToLog("CheckCubeMap: GetCubeMapSurface() failed on tempCubeMap on level %d and face %d hr=%s(%x).\n", LevelIndex, FaceIndex, m_pD3D->HResultToString(hr), hr);
					goto Exit;
				}

				if ( !ConvertSurf( pSrcSurf, pBuf, nPitch, pCubeMapFace, pNewSurf, pColors ? &(pColors[LevelIndex*6 + FaceIndex]) : NULL, pPal, position, szErrorMessage, &uCountWhiteSlices ))
				{
					result = false;
					_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sCheckCubeMap: failed on level %d & face %d.\n"), szErrorMessage, LevelIndex, FaceIndex );
                                        // Fix Missing String Termination (OACR-T)
            	                        szErrorMessage[MAX_ERROR_MESSAGE - 1] = 0;

					//WriteToLog("CheckCubeMap: failed on level %d & face %d.\n", LevelIndex, FaceIndex);
					
					if( NULL != pCheckColor && !KeySet("logallfailures") )
						pCheckColor[LevelIndex*6 + FaceIndex] = false;
				}

				if ( LevelIndex == 0 && FaceIndex == 0 && uCountWhiteSlices )
					bFirstSliceWhite = true;
			}
			else 
			{
				if ( !CheckColorsInRect( pSrcSurf, pBuf, nPitch, NULL, pColors ? &(pColors[LevelIndex*6 + FaceIndex]) : NULL, NULL, position, szErrorMessage, &uCountWhiteSlices ))
				{
					result = false;
					_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sCheckCubeMap: failed on level %d & face %d.\n"), szErrorMessage, LevelIndex, FaceIndex );
                                        // Fix Missing String Termination (OACR-T)
            	                        szErrorMessage[MAX_ERROR_MESSAGE - 1] = 0;
					//WriteToLog("CheckCubeMap: failed on level %d & face %d.\n", LevelIndex, FaceIndex);
					
					if( NULL != pCheckColor && !KeySet("logallfailures") )
						pCheckColor[LevelIndex*6 + FaceIndex] = false;
				}

				if ( LevelIndex == 0 && FaceIndex == 0 && uCountWhiteSlices )
					bFirstSliceWhite = true;
			}
		}
	}

	//bug 71971 first slice is white
	//bug 71683 all slices are white
	if ( !KeySet(_T("logallfailures")) && m_pD3D->GetRuntimeVersion() < 9.029f && uLevels > 1 && ((bFirstSliceWhite && uCountWhiteSlices == 1) || uCountWhiteSlices == 6 * uLevels ))
		result = true;
	else
	{
		TCHAR *pTempMessage = szErrorMessage;//[ MAX_ERROR_MESSAGE ];
		TCHAR *pSubStr;
		TCHAR szTemp[ MAX_ERROR_MESSAGE ];
		while ( NULL != (pSubStr = strstr( pTempMessage, _T("\n") )))
		{
			UINT len = strlen( pTempMessage ) - strlen( pSubStr );
			len = min( len + 1, MAX_ERROR_MESSAGE  - 1 );
			strncpy( szTemp, pTempMessage, len );
			szTemp[ len ] = '\0';
		
			WriteToLog( szTemp );
			pTempMessage += len;
		}
		if ( strlen( pTempMessage ))
			WriteToLog( pTempMessage );
	}
	
Exit:
	if (FAILED(hr))
	{
		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			SkipTestRange(m_uTestNumber, m_uTestNumber);
		}
		else
		{
			Fail();
			result = false;
		}
	}

	if (NULL != pPal)
		delete [] pPal;
	RELEASE(pCubeMapFace);
	RELEASE(pNewSurf);
	RELEASE(tempCubeTex);

	return result;
}

//-----------------------------------------------------------------------------

bool ResMan::CheckVolumeTexture(CSurface *pSrcSurf, void *pBuf, int nPitch, CVolTexture *pVolTexture, bool *pCheckColor, D3DCOLOR *pColors, UINT startIndex, UINT wnum, UINT hnum)
{
	CVolume	     *pTexVolume = NULL;
	PALETTEENTRY *pPal       = NULL;
	HRESULT       hr         = S_OK;
	bool          result     = true;
	bool		  bFirstSliceWhite = false;
	UINT		  uCountAllWhiteLevels = 0;
	TCHAR		  szErrorMessage[ MAX_ERROR_MESSAGE ];
	FLOAT         wcell      = 2.f/wnum;   
	FLOAT         hcell      = 2.f/hnum; 
	D3DCOLOR      color      = 0;
	UINT		  uOffset    = 0;
	UINT          uLevels;
	
	if (NULL == pVolTexture)
		return false;

	if (pVolTexture->IsPalettized())
	{
		PALETTEENTRY Palette[256];
		
		ZeroMemory(Palette, sizeof(PALETTEENTRY) * 256);
		
		hr = m_pSrcDevice->GetPaletteEntries(0, Palette);
		if (FAILED(hr))
		{
			WriteToLog("CheckVolumeTexture: GetPaletteEntries() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
		pPal = Palette;
	}
	
	uLevels = pVolTexture->GetLevelCount();
	szErrorMessage[ 0 ] = '\0';

	for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++)
	{
		VOLUME_DESC Desc;

		hr = pVolTexture->GetLevelDesc(LevelIndex, &Desc);
		if (FAILED(hr))
		{
			WriteToLog("CheckVolumeTexture: GetLevelDesc() failed on level %d hr=%s(%x).\n", LevelIndex, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		//if Colors == NULL this means that we only use this surface once (in Basic test for example) and thus we don't need to keep its color
		//if Colors[0] is black then it means that this surface in used 1st time and we need to record its color
		//otherwise we just retrive its color to use in CheckColorsInRect()
		if ( pColors == NULL || pColors[uOffset] == 0 )
		{
			hr = pVolTexture->GetVolumeLevel(LevelIndex, &pTexVolume);
			if (FAILED(hr))
			{
				WriteToLog("CheckVolumeTexture: GetVolumeLevel() failed on level %d hr=%s(%x).\n", LevelIndex, m_pD3D->HResultToString(hr), hr);
				goto Exit;
			}
		}

		if (!CheckColorsInVolume(pTexVolume, pSrcSurf, (pCheckColor ? pCheckColor+uOffset : pCheckColor), (pColors ? pColors+uOffset : pColors), &Desc, pPal, pBuf, nPitch, startIndex, wnum, hnum, wcell, hcell, szErrorMessage, LevelIndex ? NULL : &bFirstSliceWhite, &uCountAllWhiteLevels ) )
		{
			result = false;
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sCheckVolumeTexture: failed on level %d.\n"), szErrorMessage, LevelIndex );
			//WriteToLog( "CheckVolumeTexture: failed on level %d\n", LevelIndex );
		}

		uOffset    += Desc.Depth;
		startIndex += Desc.Depth;
		RELEASE(pTexVolume);
	}

	//bug 71971 first slice is white
	//bug 71683 all slices are white
	if ( !KeySet(_T("logallfailures")) && m_pD3D->GetRuntimeVersion() < 9.029f && uLevels > 1 && (bFirstSliceWhite || uCountAllWhiteLevels == uLevels) )
		result = true;
	else
	{
		TCHAR *pTempMessage = szErrorMessage;//[ MAX_ERROR_MESSAGE ];
		TCHAR *pSubStr;
		TCHAR szTemp[ MAX_ERROR_MESSAGE ];
		while ( NULL != (pSubStr = strstr( pTempMessage, _T("\n") )))
		{
			UINT len = strlen( pTempMessage ) - strlen( pSubStr );
			len = min( len + 1, MAX_ERROR_MESSAGE  - 1 );
			strncpy( szTemp, pTempMessage, len );
			szTemp[ len ] = '\0';
		
			WriteToLog( szTemp );
			pTempMessage += len;
		}
		if ( strlen( pTempMessage ))
			WriteToLog( pTempMessage );
	}

Exit:
	if (FAILED(hr))
	{
		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			SkipTestRange(m_uTestNumber, m_uTestNumber);
		}
		else
		{
			Fail();
			result = false;
		}
	}

	return result;
}

//-----------------------------------------------------------------------------

bool ResMan::ConvertSurf(CSurface *pSrcSurf, void *pBuf, int nPitch, CSurface *pSurf, CSurface *pNewSurf, D3DCOLOR *pColor, PALETTEENTRY *pPal, LONG *position, TCHAR *szErrorMessage, UINT *puCountWhiteSlices)
{
	bool        result = true;
	HRESULT     hr     = S_OK;
	CImage     *pImage = NULL;
	SURFACEDESC Desc;

	if (NULL == pSurf || NULL == pNewSurf)
		return false;

	hr = pSurf->GetDesc(&Desc);
	if (FAILED(hr))
	{
		_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: GetSurfDesc() failed hr=%s(%x).\t"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
		//WriteToLog("ConvertSurf: GetSurfDesc() failed hr=%s(%x).\t", m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	if ( Desc.Format.d3dfFormat == FMT_A8 || Desc.Format.d3dfFormat == FMT_A1 )
	{
		RECT	     rSurf1;
		LOCKED_RECT  LockRect1;
		void	    *pBuf1 = NULL;
		DWORD        color;
		
		rSurf1.top    = 0;
		rSurf1.left   = 0;
		rSurf1.bottom = 1;
		rSurf1.right  = 1;

		// Width must be multiple of 8 for the A1 format
		if ( Desc.Format.d3dfFormat == FMT_A1 )
			rSurf1.right = 8;

		hr = pSurf->LockRect( &LockRect1, &rSurf1, NULL );
		if (FAILED(hr))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: LockRect() failed hr=%s(%x).\t"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("ConvertSurf: LockRect() failed hr=%s(%x).\t", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		pBuf1 = LockRect1.pBits;

		if (Desc.Format.ddpfPixelFormat.dwRGBBitCount == 8)
		{
			PBYTE pRead = (PBYTE) LockRect1.pBits;
			color = RGBA_MAKE(*pRead, *pRead, *pRead, *pRead);
		}
		else
		{
			char *pRead = (char *) LockRect1.pBits;
			color = RGBA_MAKE(*pRead, *pRead, *pRead, *pRead);
		}

		hr = pSurf->UnlockRect();
		if (FAILED(hr))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: UnlockRect() failed hr=%s(%x).\t"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("ConvertSurf: UnlockRect() failed hr=%s(%x).\t", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		pImage = new CImage;
		if (NULL == pImage)
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: Out of memory on allocating pImage.\t"), szErrorMessage );
			//WriteToLog("ConvertSurf: Out of memory on allocating pImage.\t");
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		DWORD Stripe[] = {color};

		if (!pImage->LoadStripes( Desc.Width, Desc.Height, 1, Stripe, false, false ))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: Out of memory on LoadStripes().\t"), szErrorMessage );
			//WriteToLog("ConvertSurf: Out of memory on LoadStripes().\t");
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		hr = pNewSurf->UpdateSurface(pImage);
		if (FAILED(hr))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: UpdateSurface() failed hr=%s(%x).\t"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("ConvertSurf: UpdateSurface() failed hr=%s(%x).\t", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		if ( !CheckColorsInRect( pSrcSurf, pBuf, nPitch, pNewSurf, pColor, pPal, position, szErrorMessage, puCountWhiteSlices ) )
			result = false;

		SAFEDELETE(pImage);

		return result;
	}

	//check colors for dx8 and dx9 textures
	if ( m_dwVersion >= 0x0800 )
	{
		RECT Dest, Src;

		Dest.left   = 0;
		Dest.top    = 0;
		Dest.right  = Desc.Width;
		Dest.bottom = Desc.Height;
		Src.left    = 0;
		Src.top     = 0;
		Src.right   = Desc.Width;
		Src.bottom  = Desc.Height;

		//format conversion
		hr = m_pSrcDevice->D3DXLoadSurfaceFromSurface( pNewSurf, NULL, &Dest, pSurf, pPal, &Src, FILTER_NONE, 0 );
		if (FAILED(hr))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: D3DXLoadSurfaceFromSurface() failed hr=%s(%x).\t"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("ConvertSurf: D3DXLoadSurfaceFromSurface() failed hr=%s(%x).\t", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
		//color checking
		if ( !CheckColorsInRect( pSrcSurf, pBuf, nPitch, pNewSurf, pColor, pPal, position, szErrorMessage, puCountWhiteSlices ) )
			result = false;

		return result;
	}

	//convert colors for DXTn in dx7 and dx6
	if ( Desc.Format.ddpfPixelFormat.dwFourCC == FMT_DXT1 ||
		 Desc.Format.ddpfPixelFormat.dwFourCC == FMT_DXT2 ||
		 Desc.Format.ddpfPixelFormat.dwFourCC == FMT_DXT3 ||
		 Desc.Format.ddpfPixelFormat.dwFourCC == FMT_DXT4 ||
		 Desc.Format.ddpfPixelFormat.dwFourCC == FMT_DXT5  )
	{
		RECT Dest, Src;
		POINT DestArray;

		Dest.left   = 0;
		Dest.top    = 0;
		Dest.right  = Desc.Width;
		Dest.bottom = Desc.Height;
		
		Src.left    = 0;
		Src.top     = 0;
		Src.right   = Desc.Width;
		Src.bottom  = Desc.Height;

		DestArray.x = 0;
		DestArray.y = 0;

		hr = m_pSrcDevice->CopyRects(pSurf, &Src, 1, pNewSurf, &DestArray);
		if(FAILED(hr))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: CopyRects() failed hr=%s(%x).\t"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("ConvertSurf: CopyRects() failed hr=%s(%x).\t", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		if ( !CheckColorsInRect( pSrcSurf, pBuf, nPitch, pNewSurf, pColor, pPal, position, szErrorMessage, puCountWhiteSlices ) )
			result = false;

		return result;
	}

	//do color conversion dx6 and dx7 palettized textures 
	if ( pPal )
	{
		DWORD        color;
		WORD         index1;
		WORD         index2;
		PBYTE        pRead;
		RECT	     rSurf1;
		LOCKED_RECT  LockRect1;
		void	    *pBuf1 = NULL;
		
		rSurf1.top    = 0;
		rSurf1.left   = 0;
		rSurf1.bottom = 1;
		rSurf1.right  = 1;

		hr = pSurf->LockRect( &LockRect1, &rSurf1, NULL );
		if (FAILED(hr))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: LockRect() failed hr=%s(%x).\t"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("ConvertSurf: LockRect() failed hr=%s(%x).\t", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		pBuf1 = LockRect1.pBits;
		
        UINT bitCount = (Desc.Format.d3dfFormat == D3DFMT_A8P8)? 8 : Desc.Format.ddpfPixelFormat.dwRGBBitCount;
		switch (bitCount)
		{
			case 8:
				pRead = (PBYTE) LockRect1.pBits;
				color = RGBA_MAKE((*(pPal+*pRead)).peRed, (*(pPal+*pRead)).peGreen, (*(pPal+*pRead)).peBlue, 0xff);
				break;
			case 4:
				pRead = (PBYTE) LockRect1.pBits;
				index1 = (*pRead) >> 4;
				index2 = (*pRead) & 0xf;
				color = RGBA_MAKE((*(pPal+index1)).peRed, (*(pPal+index1)).peGreen, (*(pPal+index1)).peBlue, 0xff);
				break;
			case 2:
				pRead = (PBYTE) LockRect1.pBits;
				index1 = (*pRead) >> 6;
				color = RGBA_MAKE((*(pPal+index1)).peRed, (*(pPal+index1)).peGreen, (*(pPal+index1)).peBlue, 0xff);
				break;
			default:
				pRead = (PBYTE) LockRect1.pBits;
				index1 = (*pRead) >> 7;
				color = RGBA_MAKE((*(pPal+index1)).peRed, (*(pPal+index1)).peGreen, (*(pPal+index1)).peBlue, 0xff);
				break;
		}

		hr = pSurf->UnlockRect();
		if (FAILED(hr))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: UnlockRect() failed hr=%s(%x).\t"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("ConvertSurf: UnlockRect() failed hr=%s(%x).\t", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		pImage = new CImage;
		if (NULL == pImage)
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: Out of memory on allocating pImage.\t"), szErrorMessage );
			//WriteToLog("ConvertSurf: Out of memory on allocating pImage.\t");
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		DWORD Stripe[] = {color};

		if (!pImage->LoadStripes( Desc.Width, Desc.Height, 1, Stripe, false, false ))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: Out of memory on LoadStripes().\t"), szErrorMessage );
			//WriteToLog("ConvertSurf: Out of memory on LoadStripes().\t");
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		hr = pNewSurf->UpdateSurface(pImage);
		if (FAILED(hr))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: UpdateSurface() failed hr=%s(%x).\t"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("ConvertSurf: UpdateSurface() failed hr=%s(%x).\t", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		if ( !CheckColorsInRect( pSrcSurf, pBuf, nPitch, pNewSurf, pColor, pPal, position, szErrorMessage, puCountWhiteSlices ) )
			result = false;
		
		SAFEDELETE(pImage);
		
		return result;
	}

	//do color conversion for dx6 and dx7 YUV textures 
	if ( Desc.Format.ddpfPixelFormat.dwFourCC == FMT_UYVY || 
		 Desc.Format.ddpfPixelFormat.dwFourCC == FMT_YUY2  )
	{
		LOCKED_RECT  LockRect1;
		void	    *pBuf1 = NULL;

		hr = pSurf->LockRect( &LockRect1, NULL, NULL );
		if (FAILED(hr))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: LockRect() failed hr=%s(%x).\t"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("ConvertSurf: LockRect() failed hr=%s(%x).\t", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		pBuf1 = LockRect1.pBits;
		FLOAT color1[4], color2[4];
		UINT16 *pus = (UINT16 *)pBuf1;
		int m_uYShift = 0;
		int m_uUVShift = 8;

		if (Desc.Format.ddpfPixelFormat.dwFourCC == FMT_UYVY)
		{
			m_uYShift = 8;
			m_uUVShift = 0;
		}

		FLOAT fY0 = (FLOAT) ((pus[0] >> m_uYShift)  & 0xff) -  16.0f;
		FLOAT fU  = (FLOAT) ((pus[0] >> m_uUVShift) & 0xff) - 128.0f;
		FLOAT fY1 = (FLOAT) ((pus[1] >> m_uYShift)  & 0xff) -  16.0f;
		FLOAT fV  = (FLOAT) ((pus[1] >> m_uUVShift) & 0xff) - 128.0f;
			
		color1[0] = 0.00456621f * fY0                    + 0.00625893f * fV; //red
		color1[1] = 0.00456621f * fY0 - 0.00153632f * fU - 0.00318811f * fV; //green
		color1[2] = 0.00456621f * fY0 + 0.00791071f * fU; //blue
		color1[3] = 1.0f; //alpha
		color1[0] = (color1[0] < 0.0f) ? 0.0f : ((color1[0] > 1.0f) ? 1.0f : color1[0]);
		color1[1] = (color1[1] < 0.0f) ? 0.0f : ((color1[1] > 1.0f) ? 1.0f : color1[1]);
		color1[2] = (color1[2] < 0.0f) ? 0.0f : ((color1[2] > 1.0f) ? 1.0f : color1[2]);
			
		color2[0] = 0.00456621f * fY1                    + 0.00625893f * fV;
		color2[1] = 0.00456621f * fY1 - 0.00153632f * fU - 0.00318811f * fV;
		color2[2] = 0.00456621f * fY1 + 0.00791071f * fU;
		color2[3] = 1.0f;
		color2[0] = (color2[0] < 0.0f) ? 0.0f : ((color2[0] > 1.0f) ? 1.0f : color2[0]);
		color2[1] = (color2[1] < 0.0f) ? 0.0f : ((color2[1] > 1.0f) ? 1.0f : color2[1]);
		color2[2] = (color2[2] < 0.0f) ? 0.0f : ((color2[2] > 1.0f) ? 1.0f : color2[2]);
		
		hr = pSurf->UnlockRect();
		if (FAILED(hr))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: UnlockRect() failed hr=%s(%x).\t"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("ConvertSurf: UnlockRect() failed hr=%s(%x).\t", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		color1[0] *= 255; color1[1] *= 255; color1[2] *= 255;
		color2[0] *= 255; color2[1] *= 255; color2[2] *= 255;

		DWORD c1 = RGBA_MAKE((WORD)color1[0], (WORD)color1[1], (WORD)color1[2],  255);
		
		pImage = new CImage;
		if (NULL == pImage)
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: Out of memory on allocating pImage.\t"), szErrorMessage );
			//WriteToLog("ConvertSurf: Out of memory on allocating pImage.\t");
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		DWORD Stripe[] = {c1};

		if (!pImage->LoadStripes( Desc.Width, Desc.Height, 1, Stripe, false, false ))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: Out of memory on LoadStripes().\t"), szErrorMessage );
			//WriteToLog("ConvertSurf: Out of memory on LoadStripes().\t");
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		hr = pNewSurf->UpdateSurface(pImage);
		if (FAILED(hr))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sConvertSurf: UpdateSurface() failed hr=%s(%x).\t"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("ConvertSurf: UpdateSurface() failed hr=%s(%x).\t", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
		if ( !CheckColorsInRect( pSrcSurf, pBuf, nPitch, pNewSurf, pColor, pPal, position, szErrorMessage, puCountWhiteSlices ) )
			result = false;

		SAFEDELETE(pImage);
		
		return result;
	}

	//all other formats: direct comparison
	if ( !CheckColorsInRect( pSrcSurf, pBuf, nPitch, pSurf, pColor, pPal, position, szErrorMessage, puCountWhiteSlices ) )
		result = false;

Exit:
	if (FAILED(hr))
	{
		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			SkipTestRange(m_uTestNumber, m_uTestNumber);
		}
		else
		{
			Fail();
			result = false;
		}
	}

	SAFEDELETE(pImage);
	
	return result;
}

//-----------------------------------------------------------------------------

bool ResMan::CheckColorsInRect( CSurface *pSrcSurf, void *pBuf, int nPitch, CSurface *pSurf, D3DCOLOR *color, PALETTEENTRY *pPal, LONG *position,
								TCHAR *szErrorMessage, UINT *puCountWhiteSlices )
{
	HRESULT      hr;
	bool         result = true;
	UINT		 count_white = 0;
	UINT         count_false = 0;
	DWORD        textureColor;
   	DWORD        screenColor;
	void	    *pBuf1   = NULL;
	void	    *pColor  = NULL;
	void        *pColor1 = NULL;
	UINT         amount = (UINT)(100.f*m_pFramework->m_ImageCmp.GetTolPerPixelChannel());
    int          nPitch1;
	LOCKED_RECT  LockRect1;
	
	//pSurf and color can't be NULL at the same time
	if ( (NULL == pSurf && NULL == color) || NULL == pSrcSurf)
		return false;

	//if NULL == pSurf that means that we already know what its color
	if ( NULL != pSurf )
	{
		//get color from texture surface
		hr = pSurf->LockRect( &LockRect1, NULL, NULL );
		if (FAILED(hr))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sCheckColorsInRect: LockRect() failed hr=%s(%x).\t"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("CheckColorsInRect: LockRect() failed hr=%s(%x).\t", m_pD3D->HResultToString(hr), hr);
			
			if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
				return true;
			}
			else
			{
				Fail();
				return false;
			}
		}

		pBuf1   = LockRect1.pBits;
		nPitch1 = LockRect1.iPitch;
		pColor1 = (char*)pBuf1;

		textureColor = pSurf->ReadColor(&pColor1);
		//need only to perform conversion for dx7 interface since for higher interfaces we already used D3DXLoadSurfaceFromSurface
		if (m_dwVersion < 0x0800)
			textureColor = pSrcSurf->ColorConvert(textureColor);
		if (NULL != color)
		{
			*color = textureColor;
		}

		hr = pSurf->UnlockRect();
		if (FAILED(hr))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sCheckColorsInRect: UnlockRect() failed hr=%s(%x).\t"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("CheckColorsInRect: UnlockRect() failed hr=%s(%x).\t", m_pD3D->HResultToString(hr), hr);
			
			if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
				return true;
			}
			else
			{
				Fail();
				return false;
			}
		}
	}
	else
	{
		textureColor = *color;
	}

	//get color from pixels on the screen and compare them with texture color
	for (UINT j = position[1]; j < position[3]; j++)		// Height loop
	{
		pColor = (char*)pBuf + (nPitch * j);

		UINT i;
		for (i = 0; i < position[0]; i++)
			screenColor = pSrcSurf->ReadColor(&pColor);

		for (i = position[0]; i < position[2]; i++)	// Width loop
		{
			screenColor = pSrcSurf->ReadColor(&pColor);
			int r = RGBA_GETRED  (screenColor) - (RGBA_GETRED  (textureColor));
			int g = RGBA_GETGREEN(screenColor) - (RGBA_GETGREEN(textureColor));
			int b = RGBA_GETBLUE (screenColor) - (RGBA_GETBLUE (textureColor));
			
			if ( abs((float)r) > amount || abs((float)g) > amount || abs((float)b) > amount )
			{
				result = false;
				count_false++;
			}

			if ( RGBA_GETRED  (screenColor) == 0xff && RGBA_GETGREEN(screenColor) == 0xff && RGBA_GETBLUE (screenColor) == 0xff )
				count_white++;
		}
	}

	if (count_false < abs((position[2]-position[0])*(position[3]-position[1]))*(1.f-m_pFramework->m_ImageCmp.GetPassRequired())) 
		result = true;

	//this mip level is all white
	if ( count_white == abs((position[2]-position[0])*(position[3]-position[1])) && puCountWhiteSlices )
		(*puCountWhiteSlices)++;

	if (!result)
	{
		_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%ssurf = 0x%x, screen = 0x%x\t"), szErrorMessage, textureColor, screenColor );
		//WriteToLog("surf = 0x%x, screen = 0x%x\t", textureColor, screenColor);
		//DebugBreak();
	}

   	return result;
}

//-----------------------------------------------------------------------------

bool ResMan::CheckColorsInVolume( CVolume *pTexVolume, CSurface *pSrcSurf, bool *pCheckColor, D3DCOLOR *pColors, VOLUME_DESC *pDesc, PALETTEENTRY *pPal, void *pBuf, int nPitch, 
								  UINT startIndex, UINT wnum, UINT hnum, FLOAT wcell, FLOAT hcell, TCHAR *szErrorMessage, bool *pbOnlyFirstSliceWhite, UINT *puCountAllWhiteLevels )
{
	HRESULT      hr          = S_OK;
	bool         result      = true;
	UINT		 uCountWhiteSlices = 0;
	void	    *pColor      = NULL;
	void        *pVolColor   = NULL;
	CVolume     *pNewVolume  = NULL;
	CVolTexture *tempVol     = NULL;
	DWORD        textureColor;
   	DWORD        screenColor;
	LOCKED_BOX   LockedBox;
    UINT		 amount  = (UINT)(100.f*m_pFramework->m_ImageCmp.GetTolPerPixelChannel());
	UINT         uHeight = m_Options.D3DOptions.nBackBufferHeight;
	UINT         uWidth  = m_Options.D3DOptions.nBackBufferWidth;
	
	
	//pTexVolume and pColors can't be NULL at the same time
	if ((NULL == pTexVolume && NULL == pColors) || NULL == pSrcSurf)
		return false;

	//pTexVolume == NULL when we already locked the volume before and thus already know its colors
	if (NULL != pTexVolume)
	{
		hr = m_pSrcDevice->CreateVolumeTexture( pDesc->Width, pDesc->Height, pDesc->Depth, 1, 0, m_ScreenFormat, POOL_MANAGED, &tempVol);
		if (FAILED(hr))
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sCheckColorsInVolume: CreateVolumeTexture() failed hr=%s(%x).\n"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("CheckColorsInVolume: CreateVolumeTexture() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	
		if ( (pDesc->Format).d3dfFormat == FMT_A8 )
		{
			pNewVolume = pTexVolume;
		}
		else
		{
			hr = tempVol->GetVolumeLevel(0, &pNewVolume);
			if (FAILED(hr))
			{
				_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sCheckColorsInVolume: GetVolumeLevel() failed for tempVol hr=%s(%x).\n"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
				//WriteToLog("CheckColorsInVolume: GetVolumeLevel() failed for tempVol hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				goto Exit;
			}

			hr = m_pSrcDevice->D3DXLoadVolumeFromVolume(pNewVolume, NULL, NULL, pTexVolume, pPal, NULL, FILTER_NONE, 0 );
			if (FAILED(hr))
			{
				_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sCheckColorsInVolume: D3DXLoadVolumeFromVolume() failed hr=%s(%x).\n"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
				//WriteToLog("CheckColorsInVolume: D3DXLoadVolumeFromVolume() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				goto Exit;
			}
		}
		
		hr = pNewVolume->LockBox( &LockedBox, 0, 0 );
		if( FAILED(hr) )
		{
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%sCheckColorsInVolume: LockBox() failed hr=%s(%x).\n"), szErrorMessage, m_pD3D->HResultToString(hr), hr );
			//WriteToLog("CheckColorsInVolume: LockBox() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

	for (UINT index = 0; index < pDesc->Depth; index++)
	{
		bool currentResult = true;
		UINT uCountWhite = 0;
		UINT uCountFalse = 0;

		if (NULL != pCheckColor && false == pCheckColor[index/* - startIndex*/])
		{
			continue;
		}
		
		FLOAT fx1 = 1-((index+startIndex)%wnum)*wcell;  
		FLOAT fx2 = 1-(((index+startIndex)%wnum)+1)*wcell;
		FLOAT fy1 = 1-((index+startIndex)/wnum)*hcell;
		FLOAT fy2 = 1-((index+startIndex)/wnum+1)*hcell;

		fx1 = (uWidth/2)*(1-fx1)+1.5f;
		fx2 = (uWidth/2)*(1-fx2)+0.5f;
		fy1 = (uHeight/2)*(1-fy1)+1.5f;
		fy2 = (uHeight/2)*(1-fy2)+0.5f;

		WORD x1 = (WORD)fx1, x2 = (WORD)fx2, y1 = (WORD)fy1, y2 = (WORD)fy2;
	
		if (NULL != pTexVolume)
		{
			pVolColor    = (char*)LockedBox.pBits + (LockedBox.SlicePitch*(index));
			textureColor = pNewVolume->ReadColor(&pVolColor);
			if ( (pDesc->Format).d3dfFormat == FMT_A8 )
			{
				WORD alpha = (WORD)RGBA_GETALPHA (textureColor);
				textureColor = RGBA_MAKE(alpha, alpha, alpha, 255);
			}
			if ( NULL != pColors )
				pColors[index] = textureColor;
		}
		else
			textureColor = pColors[index];

		for (UINT j = y1; j < y2; j++)		// Height loop
		{
			pColor = (char*)pBuf + (nPitch * j);

			UINT i;
			for (i = 0; i < x1; i++)
				screenColor = pSrcSurf->ReadColor(&pColor);

			for ( i = x1; i < x2; i++)	// Width loop
			{
				screenColor = pSrcSurf->ReadColor(&pColor);

				int r = RGBA_GETRED  (screenColor) - RGBA_GETRED  (textureColor);
				int g = RGBA_GETGREEN(screenColor) - RGBA_GETGREEN(textureColor);
				int b = RGBA_GETBLUE (screenColor) - RGBA_GETBLUE (textureColor);

				if ( abs((float)r) > amount || abs((float)g) > amount || abs((float)b) > amount )
				{
					currentResult = false;
					uCountFalse++;
				}

				if ( RGBA_GETRED(screenColor) == 0xff && RGBA_GETGREEN(screenColor) == 0xff  && RGBA_GETBLUE(screenColor) == 0xff )
					uCountWhite++;
			}
		}

		if (uCountFalse < abs((y2-y1)*(x2-x1))*(1.f-m_pFramework->m_ImageCmp.GetPassRequired()))
			currentResult = true;

		if ( uCountWhite == (y2-y1)*(x2-x1) )
			uCountWhiteSlices++;

		if ( pbOnlyFirstSliceWhite && index == 0 && uCountWhiteSlices )
			*pbOnlyFirstSliceWhite = true;

		if (!currentResult)
		{
			result = false;
			_snprintf( szErrorMessage, MAX_ERROR_MESSAGE - 1, _T("%ssurf = 0x%x, screen = 0x%x on slice %d\n"), szErrorMessage, textureColor, screenColor, index );
			//WriteToLog("surf = 0x%x, screen = 0x%x on slice %d\n", textureColor, screenColor, index);
			if(NULL != pCheckColor && !KeySet("logallfailures"))
				pCheckColor[index] = false;
		}
	}

	if ( uCountWhiteSlices == pDesc->Depth )
		(*puCountAllWhiteLevels)++;

	if ( pbOnlyFirstSliceWhite && *pbOnlyFirstSliceWhite && uCountWhiteSlices > 1 )
		*pbOnlyFirstSliceWhite = false;

    if (NULL != pTexVolume)
	{
		hr = pNewVolume->UnlockBox();
		if( FAILED(hr) )
		{
			WriteToLog("CheckColorsForVolTexture: UnlockBox failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

Exit:
	if (FAILED(hr))
	{
		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			SkipTestRange(m_uTestNumber, m_uTestNumber);
		}
		else
		{
			Fail();
			result = false;
		}
	}

	if ( (pDesc->Format).d3dfFormat != FMT_A8 )
		RELEASE(pNewVolume);
	RELEASE(tempVol);

   	return result;
}


