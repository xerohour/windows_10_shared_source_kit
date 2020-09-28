#include "GetDC.h"

bool CGetDC::SetPixels(HDC hDC)
{
	COLORREF rgb;
	COLORREF rgbSet;
	COLORREF rgbGet;
	BYTE red, green, blue;
	for (int i=0; i<m_uSurfaceWidth; i++)
	{
		for (int j=0; j<m_uSurfaceHeight; j++)
		{
			red = (i + 128) % 256;
			green = (j + 64) % 256;
			blue = ((i + j) * 4) % 256;
			rgb = RGB(red, green, blue);
			rgbSet = SetPixel(hDC, i, j, rgb);
			if (-1 == rgbSet)
			{
				//ERROR
				DPF( 1, "SetPixel() failed on pixel %d,%d (returned -1)\n", i, j );
				return false;
			}
			if (rgb != rgbSet)
			{
				//That's fine, SetPixel can change the value if it doesn't fit the color format (BPP)
			}

			rgbGet = GetPixel(hDC, i, j);
			if (rgbGet != rgbSet)
			{
				//ERROR
				DPF( 1, "SetPixels(rgb=0x%x=(%d,%d,%d), x=%d, y=%d) BUT GetPixel() returns 0x%x=(%d,%d,%d)\n", rgbSet, GetRValue(rgbSet), GetGValue(rgbSet), GetBValue(rgbSet), i, j, rgbGet, GetRValue(rgbGet), GetGValue(rgbGet), GetBValue(rgbGet));
				return false;
			}
		}
	}


	return true;
}

bool CGetDC::CreateComparisonDC()
{
	//TODO: I could use a comparison DC per BPP and only create them once.

	//Create the comparison DC
	if ( !CreateWindowsDC(m_hComparisonDC, m_hComparisonBitmap, &m_pComparisonBitmap, m_hComparisonDCLastObj) )
	{
		DPF( 1, "CreateComparisonDC() Could not create Comparison DC\n" );
		return false;
	}

	//Fill the comparison DC with the pattern
	if ( !SetPixels(m_hComparisonDC) )
	{
		DPF( 1, "Could not fill the comparison DC\n" );
		ReleaseComparisonDC();
		return false;
	}

	return true;
}

bool CGetDC::CreateWindowsDC(HDC &hWindowsDC, HBITMAP &hBitmap, PVOID *ppBitmap, HGDIOBJ &hWindowsDCLastObject)
{
	BITMAPINFO *pbi = NULL;
	DIBSECTION ds;
	UINT uBPP;
	UINT uSizeBI = sizeof(BITMAPINFO);
	bool bRes = false;
	HDC hScreen = NULL;

	switch (m_dwSurfaceFormat)
	{
		case FMT_X1R5G5B5 :
		case FMT_R5G6B5 :
			uBPP = 16;
			uSizeBI = sizeof(BITMAPINFO) + 3 * sizeof(RGBQUAD);
			break;

		case FMT_R8G8B8 :
			uBPP = 24;
			break;

		case FMT_X8R8G8B8 :
			uBPP = 32;
			break;

		default :
			DPF( 1, "CreateWindowsDC() Invalid pixel format\n" );
			return false;
	}
	pbi = (BITMAPINFO*)new LPBYTE[uSizeBI];

	TCHAR szDeviceName[32];
	if( !m_pFramework->GetDisplayString( m_pSrcDevice->GetAdapterID(), szDeviceName, 32 ) )
	{
	    DPF( 0, TEXT("CreateWindowsDC() GetDisplayString failed.\n") );
		goto error;
	}

	hScreen = CreateDC(szDeviceName, NULL, NULL, NULL);
	if ( !hScreen )
	{
		DPF( 1, "CreateWindowsDC() could not CreateDC\n" );
		goto error;
	}
	hWindowsDC = CreateCompatibleDC(hScreen);
	if ( !hWindowsDC )
	{
		DPF( 1, "CreateWindowsDC() could not CreateCompatibleDC\n" );
		goto error;
	}
	pbi->bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
	pbi->bmiHeader.biWidth = m_uSurfaceWidth;
	pbi->bmiHeader.biHeight = - (int)m_uSurfaceHeight;  //Negative so that that 0,0 is upper-left corner
	pbi->bmiHeader.biPlanes = 1;
	pbi->bmiHeader.biBitCount = (WORD)uBPP;
	pbi->bmiHeader.biCompression = BI_RGB;
	pbi->bmiHeader.biSizeImage = 0;
	pbi->bmiHeader.biXPelsPerMeter = 0;
	pbi->bmiHeader.biYPelsPerMeter = 0;
	pbi->bmiHeader.biClrUsed = 0;
	pbi->bmiHeader.biClrImportant = 0;
	//Set the correct bitdepth R5G6B5 or X1R5G5B5
	if (uBPP == 16)
	{
		pbi->bmiHeader.biCompression = BI_BITFIELDS;
		RGBQUAD *pRGBQuad = pbi->bmiColors;
		memset( pRGBQuad, 0, 3*sizeof(RGBQUAD) );
		LPDWORD lpdw = (LPDWORD)&(pbi->bmiColors);
		if (m_dwSurfaceFormat == FMT_X1R5G5B5)
		{
			*(lpdw+0)	= 0x7c00;	//Red
			*(lpdw+1)	= 0x03e0;	//Green
			*(lpdw+2)	= 0x001f;	//Blue
		}
		else if (m_dwSurfaceFormat == FMT_R5G6B5)
		{
			*(lpdw+0)	= 0xf800;	//Red
			*(lpdw+1)	= 0x07e0;	//Green
			*(lpdw+2)	= 0x001f;	//Blue
		}
	}
	::SetLastError(0);
	hBitmap = CreateDIBSection(hWindowsDC, pbi, DIB_RGB_COLORS, ppBitmap, NULL, 0);
	if (!hBitmap)
	{
		//DebugBreak();
		hBitmap = CreateDIBSection(hWindowsDC, pbi, DIB_RGB_COLORS, ppBitmap, NULL, 0);
		DWORD dwError = ::GetLastError();
		DPF( 1, "CreateWindowsDC() could not CreateDIBSection : %x\n", dwError);
		goto error;
	}
	if (!GetObject(hBitmap, sizeof(ds), &ds))
	{
		DPF( 1, "CreateWindowsDC() could not get the DIBSECTION\n" );
		goto error;
	}
	hWindowsDCLastObject = SelectObject(hWindowsDC, hBitmap);

	bRes = true;

error:
	if (pbi)
	{
		delete[] pbi;
		pbi = NULL;
	}

	if (hScreen)
	{
		DeleteDC(hScreen);
	}

	return bRes;
}

bool CGetDC::ReleaseComparisonDC()
{
	return ReleaseWindowsDC(m_hComparisonDC, m_hComparisonDCLastObj, m_hComparisonBitmap);
}

bool CGetDC::ReleaseWindowsDC(HDC hWindowsDC, HGDIOBJ hWindowsDCLastObject, HBITMAP hBitmap)
{
	SelectObject(hWindowsDC, hWindowsDCLastObject);	//Make sure there is no GDI leak on win9x
	BOOL bResult = DeleteObject(hBitmap);
	bResult = DeleteDC(hWindowsDC);

	return true;
}

bool CGetDC::CompareSurfaceToDC(LPSURFACE pSurf)
{
	LOCKED_RECT lockRectSurf;
	DIBSECTION dibSect;
	SURFACEDESC	Desc;

	//Compare using GetPixel()
	if ( ! CompareDCs(m_hDXDC[0], m_hComparisonDC) )
	{
		//Error message generated by CompareDCs
		return false;
	}

	//Don't run the mem comparison code since some driver puts anything in the X of a format (X8R8G8B8)
	//so this makes memcmp fail.
#if 0
	//We need to release the DC because we need to lock the surface
	if ( FAILED( pSurf->ReleaseDC(m_hDXDC[0]) ) )
	{
		DPF(1, _T("CompareSurfaceToDC() ReleaseDC failed\n"));
		return false;
	}
	m_hDXDC[0] = NULL;


	int ret = GetObject(m_hComparisonBitmap, sizeof(DIBSECTION), &dibSect);

	UINT uBmpPitch = dibSect.dsBm.bmWidthBytes;
	//if the bmWidthBytes is not LONG aligned, we need to align it (windows bug #669032)
	UINT uAlign = uBmpPitch % sizeof(LONG);
	if (uAlign)
		uBmpPitch += uAlign;

	if ( FAILED( pSurf->GetDesc(&Desc) ) )
	{
		DPF( 1, _T("CompareSurfaceToDC() GetDesc failed\n"));
		return false;
	}
	if ( FAILED( pSurf->LockRect(&lockRectSurf, NULL, 0) ) )
	{
		DPF( 1, _T("CompareSurfaceToDC() LockRect failed\n"));
		return false;
	}
	//Compare surface to DC
	DWORD   dwY=0;
	DWORD   dwX;
	PBYTE   pSrc=(PBYTE)lockRectSurf.pBits;
	PBYTE	pDest=(PBYTE)m_pComparisonBitmap;
	DWORD	WidthSize = Desc.dwWidth * Desc.Format.ddpfPixelFormat.dwRGBBitCount / 8;

	do
	{
		if ( 0 != memcmp(pDest, pSrc, WidthSize) )
		{
			DPF( 1, "CompareSurfaceToDC() memcmp of the DC and surface failed at line %d\n", dwY );
			DPF( 1, "    pSrc[0] = %x, pDest[0] = %x\n", ((LPDWORD)pSrc)[0], ((LPDWORD)pDest)[0] );
			if ( FAILED( pSurf->UnlockRect() ) )
			{
				DPF( 1, _T("CompareSurfaceToDC() UnlockRect failed\n"));
			}
			return false;
		}

		// Increment to the next line.

		pSrc += lockRectSurf.iPitch;
		pDest += uBmpPitch;

		dwY++;
	}
	while (dwY < Desc.dwHeight);

	if ( FAILED( pSurf->UnlockRect() ) )
	{
		DPF( 1, _T("CompareSurfaceToDC() UnlockRect failed\n"));
		return false;
	}

	//Reget the DC
	pSurf->GetDC(&m_hDXDC[0]);
#endif

	return true;
}

bool CGetDC::CompareDCs(HDC hDC1, HDC hDC2, bool bShouldSucceed)
{
	bool bResult = true;
	//Compare using GetPixel()
	COLORREF colorDC1, colorDC2;
	for (int y=0; y<m_uSurfaceHeight; y++)
	{
		for (int x=0; x<m_uSurfaceWidth; x++)
		{
			colorDC1 = GetPixel(hDC1, x, y);
			colorDC2 = GetPixel(hDC2, x, y);

			if (colorDC1 != colorDC2)
			{
				if (bShouldSucceed)
				{
					DPF( 1, "CompareDCs() GetPixel() are different at %d,%d (DC1 = 0x%x = (%d,%d,%d), DC2 = 0x%x = (%d,%d,%d)))\n", x, y, colorDC1, GetRValue(colorDC1), GetGValue(colorDC1), GetBValue(colorDC1), colorDC2, GetRValue(colorDC2), GetGValue(colorDC2), GetBValue(colorDC2) );
				}
				return false;
				//colorDC2 = RGB(0,0,255);
				//SetPixel(hDC2, x, y, colorDC2);
				//bResult = false;
			}
		}
	}

	return bResult;
}

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex
	DWORD color;        // The vertex color
	FLOAT tu, tv;		// Mapping coordinates
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

bool CGetDC::RenderQuad()
{
	bool bRes = false;

	//We need to align the texel with the pixel
	float left = -0.5;
	float right = m_uSurfaceWidth - 0.5f;
	float top = -0.5;
	float bottom = m_uSurfaceHeight - 0.5f;
	CUSTOMVERTEX vertices[4] = 
	{
		{left, bottom, 0.5f, 1.0f, 0xffffffff, 0.0f, 0.0f},
		{left, top, 0.5f, 1.0f, 0xffffffff, 1.0f, 0.0f},
		{right, bottom, 0.5f, 1.0f, 0xffffffff, 1.0f, 1.0f},
		{right, top, 0.5f, 1.0f, 0xffffffff, 0.0f, 1.0f}
	};
	LONG lDev = CD3D_ALL;

	LPSURFACES pOrigRT = NULL;

	if (! GetRenderTarget(&pOrigRT, lDev) )
	{
		DPF( 1, _T("RenderQuad() SetRenderTarget() failed\n"));
		goto Exit;
	}

	if (! SetRenderTarget((DWORD)0, m_pSurfaces[0], lDev) )
	{
		DPF( 1, _T("RenderQuad() SetRenderTarget() failed\n"));
		goto Exit;
	}

	SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1, lDev);
    SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE, lDev);

	if (! BeginScene(lDev))
	{
		DPF( 1, _T("RenderQuad() BeginScene() failed\n"));
		goto Exit;
	}

	if (! SetVertexShader( D3DFVF_CUSTOMVERTEX, lDev ) )
	{
		DPF( 1, _T("RenderQuad() SetVertexShader() failed\n"));
		goto Exit;
	}

	if (! DrawPrimitiveUP( D3DPT_TRIANGLESTRIP,  2, vertices, sizeof(CUSTOMVERTEX), lDev) )
	{
		DPF( 1, _T("RenderQuad() DrawPrimitive() failed\n"));
		goto Exit;
	}

	if (! EndScene(lDev) )
	{
		DPF( 1, _T("RenderQuad() EndScene() failed\n"));
		goto Exit;
	}

	if (! SetRenderTarget((DWORD)0, pOrigRT, lDev) )
	{
		DPF( 1, _T("RenderQuad() SetRenderTarget(pOrigRT) failed\n"));
		goto Exit;
	}

	bRes = true;

Exit:
	RELEASE(pOrigRT);

	return bRes;
}

// ----------------------------------------------------------------------------

// Creates a surface (or 2 if needed for this case) based on what kind of surface we are testing.
bool CGetDC::CreateSurface(ULONG lDev)
{
	for (int i=0; i<MAXSURFACES; i++)
	{
		m_pSurfaces[i] = NULL;
	}

	UINT nLevels = 1;
	UINT uLevel = 0;
	UINT uWidth = m_uSurfaceWidth;
	UINT uHeight = m_uSurfaceHeight;

	for (int iNbSurfNeeded=0; iNbSurfNeeded<m_nSurfNeededForTest; iNbSurfNeeded++)
	{
		switch(m_dwSurfaceType)
		{
			case SURF_RENDERTARGET :
				if (!CreateRenderTarget(m_uSurfaceWidth, m_uSurfaceHeight, (FMT)m_dwSurfaceFormat, MULTISAMPLE_NONE, true, &m_pSurfaces[iNbSurfNeeded]))
				{
					DPF(1, _T("Can't create RenderTarget\n"));
					return false;
				}
				break;

			case SURF_OFFSCREENPLAIN :
				if (!CreateOffscreenPlainSurface(m_uSurfaceWidth, m_uSurfaceHeight, (FMT)m_dwSurfaceFormat, m_dwPool, &m_pSurfaces[iNbSurfNeeded]))
				{
					DPF(1, _T("Can't create OffscreenPlain Surface\n"));
					return false;
				}
				break;

			case SURF_TEXTURE :
				//DXTn formats requires size is a multiple of 4 
				if (m_dwSurfaceFormat >= FMT_DXT1 && m_dwSurfaceFormat <= FMT_DXT5)
				{
					if (m_uSurfaceWidth % 4 || m_uSurfaceHeight % 4)
					{
						m_bSkipTest = true;
						return false;
					}
				}
				if (m_dwPool == POOL_DEFAULT)
				{
					if ( !m_bDynamicSupported )
					{
						m_bSkipTest = true;
						return false;
					}

					m_dwUsage = D3DUSAGE_DYNAMIC;
					_tcscpy( m_szUsage, _T("Dynamic") );
				}

				//Set default if we don't use a mip level
				nLevels = 1;
				uLevel = 0;
				uWidth = m_uSurfaceWidth;
				uHeight = m_uSurfaceHeight;
				if (m_rUseMipLevel->Value() == 2)
				{
					nLevels = 0;
					uLevel = 1;
					//multiply the size by 2 so that the size of the first mip is the size requested
					uWidth *= 2;
					uHeight *= 2;
				}
				if (!CreateTexture(uWidth, uHeight, nLevels, m_dwUsage, (FMT)m_dwSurfaceFormat, m_dwPool, &m_pTextures[iNbSurfNeeded]))
				{
					DPF(1, _T("Can't create Texture\n"));
					return false;
				}
				if (!m_pTextures[iNbSurfNeeded]->GetSurfaceLevel(uLevel, &m_pSurfaces[iNbSurfNeeded]))
				{
					DPF(1, _T("Can't get the surface from the texture\n"));
					return false;
				}
				break;

			case SURF_BACKBUFFER :
				if (!GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pSurfaces[iNbSurfNeeded]))
				{
					DPF(1, _T("Can't get BackBuffer\n"));
					return false;
				}
				break;

			case SURF_SWAPCHAIN :
				// Set the Present Parameters
				memcpy(&m_PresParams, m_DeviceList[0]->GetPresParams(), sizeof(PRESENT_PARAMETERS));
				m_PresParams.BackBufferFormat = (FMT)m_dwSurfaceFormat;
				//m_PresParams.hDeviceWindow = m_hWindow;
				m_PresParams.uHeight = m_uSurfaceHeight;
				m_PresParams.uWidth = m_uSurfaceWidth;
				m_PresParams.uBackBufferCount = 1;
				m_PresParams.AutoDepthStencilFormat = FMT_ANY;
				m_PresParams.bAutoDepthStencil = false;
				m_PresParams.bCachedBackBuffers = false;
				//m_PresParams.bWindowed = IsWindowed?
				m_PresParams.SwapEffect = D3DSWAPEFFECT_COPY;  // Must be copy to compare the back buffers 
				m_PresParams.dwFlags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

				if (!CreateAdditionalSwapChain(&m_PresParams, &m_pSwapChains[iNbSurfNeeded]))
				{
					DPF(1, _T("Can't create Additional SwapChain\n"));
					return false;
				}

				if (!m_pSwapChains[iNbSurfNeeded]->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pSurfaces[iNbSurfNeeded]) )
				{
					DPF(1, _T("Can't get backBuffer from swapChain\n"));
					return false;
				}
				break;
		}
	}
	return true;
}

