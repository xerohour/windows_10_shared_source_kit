#include "multisample.h"
#include "Csurface.h"
#include "CTextures.h"

void CSamples::GetPointsTest()
{
	HRESULT		hr = S_OK;
	float		fOffset = 0.0f;
	UINT		Y = 0, X = 0;
	DWORD		dwColor;

	CSurface	*pSaveSurface;
	
	if ( m_dwVersion <= 0x900 )
	{
		fOffset = -0.5f;
	}
	m_fSampleSize = SUBPIX_FOURBIT;

	FLEXVERTEX fvSubPix[] =
	{ 
		{ 3.0f,						3.0f,					1.00f, 1.00f, WHITE },
		{ 3.0f + m_fSampleSize,		3.0f,					1.00f, 1.00f, WHITE },
		{ 3.0f,						3.0f + m_fSampleSize,	1.00f, 1.00f, WHITE },
		{ 3.0f + m_fSampleSize,		3.0f + m_fSampleSize,	1.00f, 1.00f, WHITE }
	};

	if ( fOffset != 0.0f )
	{
		for ( UINT i = 0; i < 4; i++ )
		{
			fvSubPix[i].fPy += fOffset;
			fvSubPix[i].fPx += fOffset;
		}
	}

	
	hr = m_pSrcDevice->CreateImageSurface( int(3/m_fSampleSize) + 2, int(3/m_fSampleSize) + 2, FMT_A8R8G8B8, &pSaveSurface ); 
	//Add room for single pixel divider lines
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CSamples::GetPointsTest() - CreateImageSurface failed.\n" ) );
		goto Exit;
		Fail();
	}

	for ( Y = 0; Y < (1/m_fSampleSize); Y++ )
	{
		for ( X = 0; X < (1/m_fSampleSize); X++ )
		{
			hr = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET, BLACK, 1.0f, 0 );
			if ( FAILED( hr ) )
			{
				DPF( 1, _T( "CSamples::GetPointsTest() - Clear failed.\n" ) );
				Fail();
			}
			hr = m_pSrcDevice->BeginScene();
			if ( FAILED( hr ) )
			{
				DPF( 1, _T( "CSamples::GetPointsTest() - BeginScene failed.\n" ) );
				Fail();
			}

			for ( UINT i = 0; i < 3; i++ )
			{
				for ( UINT j = 0; j < 3; j++ )
				{
					switch (j)
					{
					case 0:
						switch (i)
						{
						case 0:
							dwColor = CYAN;
							break;
						case 1:
							dwColor = RED;
							break;
						case 2:
							dwColor = WHITE;
						}
						break;
					case 1:
						switch (i)
						{
						case 0:
							dwColor = GREEN;
							break;
						case 1:
							dwColor = BLACK;
							break;
						case 2:
							dwColor = BLUE;
						}
						break;
					case 2:
						switch (i)
						{
						case 0:
							dwColor = MAGENTA;
							break;
						case 1:
							dwColor = WHITE;
							break;
						case 2:
							dwColor = YELLOW;
							break;
						}
						break;
					}
					for ( UINT k = 0; k < 4; k++ )
					{
						fvSubPix[k].Diffuse = dwColor;
					}

					hr = m_pSrcDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, fvSubPix, sizeof(_FlexVert) );
					if ( FAILED( hr ) )
					{
						DPF( 1, _T( "CSamples::QuickTest() - DrawPrimitiveUP failed.\n" ) );
						goto Exit;
						Fail();
					}
					for ( UINT k = 0; k < 4; k++ )
					{
						fvSubPix[k].fPx += 1.0f;
					}
				}
				for ( UINT k = 0; k < 4; k++ )
				{
					fvSubPix[k].fPx -= 3.0f;
					fvSubPix[k].fPy += 1.0f;
				}
			}
			
			for ( UINT i = 0; i < 4; i++ )
			{
				//Shift to the first pixel, and adjust sub-pixel x location.
				fvSubPix[i].fPx += m_fSampleSize;
				fvSubPix[i].fPy -= 3.0f;
			}

			hr = m_pSrcDevice->EndScene();
			if ( FAILED( hr ) )
			{
				DPF( 1, _T( "CSamples::QuickTest() - EndScene failed.\n" ) );
				goto Exit;
				Fail();
			}
			
			if ( !DisplayFrame() )
			{
				goto Exit;
				Fail();
			}

			//Now determine which points were found, and update the surface
			FindPoints( pSaveSurface, X, Y );
		}

		//Onto the next Y location
		for ( UINT i = 0; i < 4; i++ )
		{
			fvSubPix[i].fPx -= 1.0f;
			fvSubPix[i].fPy += m_fSampleSize;
		}
	}

	DrawDividers( pSaveSurface );
	SaveSurface( pSaveSurface );

Exit:
	RELEASE( pSaveSurface );

	return;
}

void CSamples::FindPoints( CSurface *pSurface, UINT X, UINT Y )
{
	HRESULT		hr = S_OK;

    CSurface	*pFBSurf = NULL;
	BYTE		*pFB;
	LOCKED_RECT	*pFBLock;

	BYTE		*pDisplay = NULL;
	LOCKED_RECT	pDLock;

	DWORD		dwPixelStride = 0;

	DWORD		dwVals[3][3];

	//Get results
	hr = GetScene( &pFBLock, &pFBSurf );
	if(FAILED(hr))
	{
		DPF(1, _T("CSamples::FindPoints() - GetScene failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		Fail();
		goto Exit;
	}

	//Check results

	for ( UINT Ypos = 0; Ypos < 3; Ypos++ )
	{
		pFB = (BYTE*) pFBLock->pBits;
		//We're looking at an area that isn't an edge
		pFB += pFBLock->iPitch * 3 + pFBLock->iPitch * Ypos;
		//Vertical or horizontal
		pFB += 12;
		
		for ( UINT Xpos = 0; Xpos < 3; Xpos++ )
		{
			dwVals[Ypos][Xpos] = *((DWORD*)pFB);
			pFB += 4;
		}
	}
	
	hr = pFBSurf->UnlockRect();
	if(FAILED(hr))
	{
		DPF(1, _T("CSamples::FindPoints() - UnLockRect failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
	}

	
	hr = pSurface->LockRect( &pDLock, NULL, NULL );
	if(FAILED(hr))
	{
		DPF(1, _T("CSamples::FindPoints() - LockRect failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
	}

	dwPixelStride = int( 1/m_fSampleSize ) + 1;
	for ( UINT Ypos = 0; Ypos < 3; Ypos++ )
	{
		//This sets us to the correct pixel vertically
		pDisplay = (BYTE*)pDLock.pBits + pDLock.iPitch * Ypos * dwPixelStride;
		
		//This drops us to the correct offset for the sub-pixel location
		pDisplay += pDLock.iPitch * ( Y - 1 );
		//Slides horizontally into the correct sub-pixel spot
		pDisplay += 4 * ( X - 1 );

		for ( UINT Xpos = 0; Xpos < 3; Xpos++ )
		{
			*((DWORD*)pDisplay) = dwVals[Ypos][Xpos];

			//Moves us over to the next pixel
			pDisplay += 4 * dwPixelStride;
		}
	}

	hr = pSurface->UnlockRect();
	if(FAILED(hr))
	{
		DPF(1, _T("CSamples::FindPoints() - UnlockRect failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
	}
Exit:
	RELEASE( pFBSurf );
}

void CSamples::SaveSurface( CSurface *pSurface )
{
	TCHAR *sTemp = NULL, *sGroupName = NULL;
	TCHAR sFileName[MAX_PATH], sDir[MAX_PATH];

	HRESULT hr = S_OK;

	GetExpandedPath(_T("%USERPROFILE%\\dxlogs\\"), sDir);

	// Make a local copy of the group name and remove invalid filename chars
	COPYSTRING(sGroupName, m_pFramework->m_DXGLog.GetCurrentGroup());
	if(NULL == sGroupName)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}
	
	sTemp = sGroupName;
	while(*sTemp != _T('\0'))
	{
		switch(*sTemp)
		{
			case _T('/'):
			case _T('\\'):
			case _T(':'):
			case _T('<'):
			case _T('>'):
			case _T('|'):
				*sTemp = _T('-');
				break;
			default:
				break;
		}

		sTemp++;
	}

	if(_sntprintf(sFileName, MAX_PATH-1, _T("%sVersion #%X - %s.bmp"), sDir, m_dwVersion, sGroupName ) < 0)
			sFileName[MAX_PATH-1] = _T('\0');

	hr = m_pSrcDevice->D3DXSaveSurfaceToFileA(sFileName, (D3DXIMAGE_FILEFORMAT)0/* D3DXIFF_BMP */, pSurface, NULL, NULL);
	if(FAILED(hr))
	{
		DPF(1, _T("CD3DTest::DeviceCapture() - D3DXSaveSurfaceToFile failed for %s.\n"),sFileName);
		DPF(1, _T("CD3DTest::DeviceCapture() - hr = %s(%x).\n"), m_pD3D->HResultToString(hr), hr);
	}

Exit:
	SAFEDELETEA( sGroupName );

	return;
}

void CSamples::DrawDividers( CSurface *pSurface )
{
	HRESULT hr = S_OK;
	BYTE		*pDisplay = NULL;
	LOCKED_RECT	pDLock;

	hr = pSurface->LockRect( &pDLock, NULL, NULL );
	if(FAILED(hr))
	{
		DPF(1, _T("CSamples::FindPoints() - LockRect failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		goto Exit;
	}

	pDisplay = (BYTE*)pDLock.pBits;
	for ( UINT Y = 1; Y <= ( int(3/m_fSampleSize) + 2 ); Y++ )
	{
		pDisplay += int(1/m_fSampleSize) * 4;
		memset( pDisplay, 0xFF, 4 );
		pDisplay += int(1/m_fSampleSize) * 4 + 4;
		memset( pDisplay, 0xFF, 4 );		

		pDisplay = (BYTE*)pDLock.pBits;
		pDisplay += pDLock.iPitch * Y;
	}

	pDisplay = (BYTE*)pDLock.pBits;
	//Do the horizontal lines
	pDisplay += pDLock.iPitch * int(1/m_fSampleSize);
	memset( pDisplay, 0xFF, 4 * ( int(3/m_fSampleSize) + 2 ) );
	pDisplay += pDLock.iPitch * ( int(1/m_fSampleSize) + 1 );
	memset( pDisplay, 0xFF, 4 * ( int(3/m_fSampleSize) + 2 ) );

	hr = pSurface->UnlockRect();
	if(FAILED(hr))
	{
		DPF(1, _T("CSamples::FindPoints() - UnlockRect failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
	}

Exit:
	return;
}