#include "GetDC.h"
#include "CBuffers.h"


bool CGetDC::RunTest()
{
	HRESULT hr = NULL;
	bool result = true;
	m_bNeedToReset = false;

	m_hScreenWindow = m_DeviceList[0]->GetPresParams()->bWindowed ? m_DeviceList[0]->GetPresParams()->hDeviceWindow : NULL;

	if (m_rTestCase->Active())
	{
		//Create a comparison DC
		if ( !CreateComparisonDC() )
		{
			DPF( 1, "Could not create the Comparison DC\n" ) ;
			Skip();
			return true;
		}

		//Get the DC for all surfaces
		for (int i=0; i<m_nSurfNeededForTest; i++)
		{
			m_hDXDC[i] = NULL;

			hr = m_pSurf[i]->GetDC(&m_hDXDC[i]);
			if ( FAILED( hr ) || m_hDXDC[i] == NULL)
			{
				//ERROR, GetDC should Succeed
				DPF( 1, "GetDC() failed\n" ) ;
				return false;
			}

		}

		switch (m_dwTestCase)
		{
			case TEST_SETPIXEL :
				result = TestSetPixel(m_pSurf[0]);
				break;

			case TEST_BLTGDITODX :
				result = TestBltGDIToDX();
				break;

			case TEST_BLTDXTOGDI :
				result = TestBltDXToGDI();
				break;

			case TEST_BLTDXTODX :
				result = TestBltDXToDX();
				break;

			case TEST_BLTDXTOSCREEN :
				result = TestBltDXToScreen();
				break;

			case TEST_BLTSCREENTODX :
				result = TestBltScreenToDX();
				break;

			case TEST_RENDERTHENGETDC :
				result = TestRenderThenGetDC(m_pSurf[0]);
				break;

			case TEST_GETDCTHENRENDER :
				result = TestGetDCThenRender(m_pSurf[0]);
				break;

			case TEST_CREATEDCONLOSTDEV :
				result = TestCreateDCOnLostDev(m_pSurf[0]);
				break;

			case TEST_USEDCONLOSTDEV :
				result = TestUseDCOnLostDev();
				break;

			case TEST_USEDCAFTERBEGINSCENE :
				result = TestUseDCAfterBeginScene();
				break;

			case TEST_GDIATTRIBUTES :
				result = TestGDIAttributes();
				break;

			case TEST_GDICLIPPING :
				result = TestGDIClipping();
				break;

			case TEST_HIGHLEVELGDIOPER :
				result = TestHighLevelGDIOperations();
				break;

			default :
				DPF( 1, "TEST ERROR: Invalid Test Case\n" );
				return false;
		}

		for (int i=0; i<m_nSurfNeededForTest; i++)
		{
			//Release the DC
			if (m_hDXDC[i])
			{
				hr = m_pSurf[i]->ReleaseDC(m_hDXDC[i]);
				if ( FAILED(hr) )
				{
					DPF( 1, "ReleaseDC() failed\n" ) ;
					result = false;
				}
			}
		}

		//This is a hack for the lost devices test
		//we need to release the surfaces but can't release them before releasing the DC
		if (m_bNeedToReset)
		{
			RELEASE(m_pSurf[0]);
			RELEASE(m_pSurfaces[0]);
			RELEASE(m_pTextures[0]);
			RELEASE(m_pSwapChains[0]);
			//We need to release the D3DXFont otherwise Reset can fail
			m_DeviceList[0]->D3DXFontOnLostDevice();
			if (!Reset(m_DeviceList[0]->GetPresParams()))
			{
				result = false;
			}
			m_DeviceList[0]->D3DXFontOnResetDevice();
		}
		
		ReleaseComparisonDC();
	}
	else if (m_rAPITestCase->Active() || m_rOtherAPITestCase->Active())
	{
		switch (m_dwTestCase)
		{
			case TEST_LOCKTHENGETDC :
				result = TestLockThenGetDC(m_pSurf[0]);
				break;

			case TEST_GETDCTHENLOCK :
				result = TestGetDCThenLock(m_pSurf[0]);
				break;

			case TEST_GETMULTIPLEDCS :
				result = TestGetMultipleDCs(m_pSurf[0]);
				break;

			case TEST_RELEASEDDCUNUSABLE :
				result = TestReleasedDCUnusable(m_pSurf[0]);
				break;

			case TEST_RELEASEDCTWICE :
				result = TestReleaseDCTwice(m_pSurf[0]);
				break;

			case TEST_RELEASEDCONOTHERSURF :
				result = TestReleaseDCOnOtherSurf(m_pSurf[0], m_pSurf[1]);
				break;

			case TEST_RELEASEBOGUSDC :
				result = TestReleaseBogusDC(m_pSurf[0]);
				break;

			case TEST_GETDCBADHANDLEPTR :
				result = TestGetDCBadHandlePtr(m_pSurf[0]);
				break;

			case TEST_MEMORYLEAKS :
				result = TestMemoryLeaks(m_pSurf[0]);
				break;

			default :
				DPF( 1, "TEST ERROR: Invalid API Test Case\n" );
				return false;
		}
	}

	return result;
}

bool CGetDC::TestSetPixel(LPSURFACE pSurf)
{
	bool result = false;

	//Set the pixels of the DX DC
	if ( !SetPixels(m_hDXDC[0]) )
	{
		goto Finalize;
	}

	result = CompareSurfaceToDC(pSurf);

Finalize:
	return result;
}

bool CGetDC::TestBltDXToGDI()
{
	bool result = false;

	HDC		hGDIDC = NULL;
	HGDIOBJ	hGDIDCLastObj = NULL;
	HBITMAP hGDIBitmap = NULL;
	PVOID	pGDIBitmap = NULL;

	//Set the pixels of the DX DC
	if ( !SetPixels(m_hDXDC[0]) )
	{
		goto Finalize;
	}

	//Create a DC to Blit to
	if ( !CreateWindowsDC(hGDIDC, hGDIBitmap, &pGDIBitmap, hGDIDCLastObj) )
	{
		goto Finalize;
	}

	//Blit from DX DC to GDI DC
	if ( !BitBlt(hGDIDC, 0, 0, m_uSurfaceWidth, m_uSurfaceHeight, m_hDXDC[0], 0, 0, SRCCOPY) )
	{
		DWORD dwError = ::GetLastError();
		DPF( 1, "TestBltDXToGDI() Could not BitBlt : %x\n", dwError );
		goto Finalize;
	}

	//Compare DCs
	if ( !CompareDCs(hGDIDC, m_hDXDC[0]) )
	{
		goto Finalize;
	}

	result = true;
	//result = CompareSurfaceToDC(pSurf);

Finalize:
	ReleaseWindowsDC(hGDIDC, hGDIDCLastObj, hGDIBitmap);
	return result;
}

bool CGetDC::TestBltGDIToDX()
{
	bool result = false;

	HDC		hGDIDC;
	HGDIOBJ	hGDIDCLastObj;
	HBITMAP hGDIBitmap;
	PVOID	pGDIBitmap;

	//Create a DC to Blit from
	if ( !CreateWindowsDC(hGDIDC, hGDIBitmap, &pGDIBitmap, hGDIDCLastObj) )
	{
		goto Finalize;
	}

	//Set the pixels of the windows DC
	if ( !SetPixels(hGDIDC) )
	{
		goto Finalize;
	}

	//Blit from GDI DC to DX DC
	if ( !BitBlt(m_hDXDC[0], 0, 0, m_uSurfaceWidth, m_uSurfaceHeight, hGDIDC, 0, 0, SRCCOPY) )
	{
		DPF( 1, "TestBltGDIToDX() Could not BitBlt\n" );
	}

	//Compare DCs
	result = CompareDCs(m_hComparisonDC, hGDIDC);

Finalize:
	ReleaseWindowsDC(hGDIDC, hGDIDCLastObj, hGDIBitmap);
	return result;
}

bool CGetDC::TestBltDXToDX()
{
	//Can't do this test with backbuffer, since the 2 surfaces will be the same.
	if (m_dwSurfaceType == SURF_BACKBUFFER)
		return true;

	bool result = false;

	//Set the pixels of the first DX DC
	if ( !SetPixels(m_hDXDC[0]) )
	{
		goto Finalize;
	}

	//Blit from DX DC to GDI DC
	if ( !BitBlt(m_hDXDC[1], 0, 0, m_uSurfaceWidth, m_uSurfaceHeight, m_hDXDC[0], 0, 0, SRCCOPY) )
	{
		DPF( 1, "TestBltDXToDX() Could not BitBlt\n" );
	}

	//Compare DCs
	result = CompareDCs(m_hComparisonDC, m_hDXDC[1]);

Finalize:
	return result;
}

bool CGetDC::TestBltDXToScreen()
{
	//This test will fail if there is another window over the framework window
	//This test will fail if the desktop pixel format is diffent than the surface pixel format so SKIP it
	if (m_DeviceList[0]->GetPresParams()->BackBufferFormat.d3dfFormat != (FMT)m_dwSurfaceFormat)
		return true;

	HDC hScreenDC = NULL;
	//if the window is smaller than the size of the device, change the size because we can't write outside the window
	UINT uPrevSurfaceWidth = m_uSurfaceWidth;
	UINT uPrevSurfaceHeight = m_uSurfaceHeight;
	int nWindowWidth = m_pFramework->m_pDisplayWindow->m_nWidth;
	int nWindowHeight = m_pFramework->m_pDisplayWindow->m_nHeight;
	bool bWindowed = m_pCurrentMode->bWindowed;
	if (bWindowed && ((nWindowWidth < m_uSurfaceWidth) || (nWindowHeight < m_uSurfaceHeight)))
	{
		m_uSurfaceWidth = nWindowWidth;
		m_uSurfaceHeight = nWindowHeight;
	}

	bool result = false;

	//Set the pixels of the DX DC
	if ( !SetPixels(m_hDXDC[0]) )
	{
		goto Finalize;
	}

	//Get the current windows DC
	hScreenDC = GetDC(m_hScreenWindow);
	if (!hScreenDC)
	{
		DPF( 1, "Could not get the Screen DC\n" );
		goto Finalize;
	}

	//Blit from DX DC to Screen DC
	if ( !BitBlt(hScreenDC, 0, 0, m_uSurfaceWidth, m_uSurfaceHeight, m_hDXDC[0], 0, 0, SRCCOPY) )
	{
		DPF( 1, "TestBltDXToScreen() Could not BitBlt\n" );
	}

	//Compare DCs
	result = CompareDCs(m_hComparisonDC, hScreenDC);

	if (!result)
	{
		DPF( 1, "NOTE: This test will fail if another window is over the test window.\n" );
		DPF( 1, "NOTE: If this is the case, minimize the other window and rerun the test.\n" );
	}

Finalize:
	m_uSurfaceWidth = uPrevSurfaceWidth;
	m_uSurfaceHeight = uPrevSurfaceHeight;

	if (!ReleaseDC(m_hScreenWindow, hScreenDC))
	{
		DPF( 1, "The Screen DC could not be released\n" );
	}
	return result;
}

bool CGetDC::TestBltScreenToDX()
{
	//This test will fail if the desktop pixel format is diffent than the surface pixel format so SKIP it
	if (m_DeviceList[0]->GetPresParams()->BackBufferFormat.d3dfFormat != (FMT)m_dwSurfaceFormat)
		return true;

	//if the window is smaller than the size of the device, change the size because we can't write outside the window
	UINT uPrevSurfaceWidth = m_uSurfaceWidth;
	UINT uPrevSurfaceHeight = m_uSurfaceHeight;
	int nWindowWidth = m_pFramework->m_pDisplayWindow->m_nWidth;
	int nWindowHeight = m_pFramework->m_pDisplayWindow->m_nHeight;
	bool bWindowed = m_pCurrentMode->bWindowed;
	if (bWindowed && ((nWindowWidth < m_uSurfaceWidth) || (nWindowHeight < m_uSurfaceHeight)))
	{
		m_uSurfaceWidth = nWindowWidth;
		m_uSurfaceHeight = nWindowHeight;
	}

	bool result = false;

	//Get the current windows DC
	HDC hScreenDC = GetDC(m_hScreenWindow);
	if (!hScreenDC)
	{
		DPF( 1, "Could not get the Screen DC\n" );
		goto Finalize;
	}

	//Set the pixels of the Screen DC
	if ( !SetPixels(hScreenDC) )
	{
		goto Finalize;
	}

	//Blit from Screen DC to DX DC
	if ( !BitBlt(m_hDXDC[0], 0, 0, m_uSurfaceWidth, m_uSurfaceHeight, hScreenDC, 0, 0, SRCCOPY) )
	{
		DPF( 1, "TestBltScreenToDX() Could not BitBlt\n" );
	}

	//Compare DCs
	result = CompareDCs(m_hComparisonDC, m_hDXDC[0]);

Finalize:
	m_uSurfaceWidth = uPrevSurfaceWidth;
	m_uSurfaceHeight = uPrevSurfaceHeight;

	if (!ReleaseDC(m_hScreenWindow, hScreenDC))
	{
		DPF( 1, "The Screen DC could not be released\n" );
	}
	return result;
}

bool CGetDC::TestLockThenGetDC(LPSURFACE pSurf)
{
	bool bResult = false;
	LOCKED_RECT lockrect;
	HRESULT hr;
	
	//Lock the surface
	hr = pSurf->LockRect(&lockrect, NULL, 0);
	if ( FAILED ( hr ) )
	{
		//ERROR, Lock should Succeed
		DPF( 1, "TestLockThenGetDC() - LockRect() failed\n" ) ;
		return false;
	}

	//GetDC should fail after lock
	m_hDXDC[0] = NULL;
	hr = pSurf->GetDC(&m_hDXDC[0]);
	if ( SUCCEEDED(hr) || m_hDXDC[0])
	{
		//ERROR, GetDC should fail
		pSurf->ReleaseDC(m_hDXDC[0]);
		m_hDXDC[0] = NULL;
		DPF( 1, "TestLockThenGetDC() - GetDC succeeded after lock\n" ) ;
		goto Finalize;
	}

	bResult = true;
Finalize:

	hr = pSurf->UnlockRect();
	if ( FAILED(hr) )
	{
		//ERROR, UnLock should Succeed
		DPF( 1, "TestLockThenGetDC() - UnlockRect() failed\n" ) ;
	}

	return bResult;
}

bool CGetDC::TestGetDCThenLock(LPSURFACE pSurf)
{
	bool bResult = false;
	LOCKED_RECT lockrect;
	HRESULT hr;
	
	//GetDC
	hr = pSurf->GetDC(&m_hDXDC[0]);
	if ( FAILED(hr) )
	{
		//ERROR, GetDC should succeed
		DPF( 1, "TestGetDCThenLock() - GetDC failed\n" ) ;
		goto Finalize;
	}

	//Lock should fail after GetDC
	hr = pSurf->LockRect(&lockrect, NULL, 0);
	if ( SUCCEEDED( hr ) )
	{
		//ERROR, Lock should fail
		DPF( 1, "TestGetDCThenLock() - LockRect() succeeded after GetDC\n" ) ;
		return false;
	}


	bResult = true;
Finalize:

	//ReleaseDC should succeed
	hr = pSurf->ReleaseDC(m_hDXDC[0]);
	m_hDXDC[0] = NULL;
	if ( FAILED(hr) )
	{
		//ERROR, ReleaseDC failed
		DPF( 1, "TestGetDCThenLock() - ReleaseDC failed\n" ) ;
	}


	return bResult;
}

bool CGetDC::TestRenderThenGetDC(LPSURFACE pSurf)
{
	//Do this test only on render targets
	if ( ! (m_dwSurfaceType == SURF_BACKBUFFER || 
			m_dwSurfaceType == SURF_RENDERTARGET ||
			m_dwSurfaceType == SURF_SWAPCHAIN ) )
		return true;

	bool bRes = false;

	//Release the previous DC
	HRESULT hr = pSurf->ReleaseDC(m_hDXDC[0]);
	m_hDXDC[0] = NULL;
	if ( FAILED(hr) )
	{
		//ERROR, ReleaseDC should succeed
		DPF( 1, "TestRenderThenGetDC() - ReleaseDC failed\n" ) ;
		goto Exit;
	}

	if (! RenderQuad())
	{
		DPF( 1, "TestRenderThenGetDC() - RenderQuad() failed\n" ) ;
		goto Exit;
	}

	//GetDC
	hr = pSurf->GetDC(&m_hDXDC[0]);
	if ( FAILED(hr) )
	{
		//ERROR, GetDC should succeed
		DPF( 1, "TestRenderThenGetDC() - GetDC failed\n" ) ;
		goto Exit;
	}

	//Set the pixels of the DX DC
	if ( !SetPixels(m_hDXDC[0]) )
	{
		DPF( 1, "TestRenderThenGetDC() - SetPixels failed\n" ) ;
		goto Exit;
	}

	//Release DC
	hr = pSurf->ReleaseDC(m_hDXDC[0]);
	m_hDXDC[0] = NULL;
	if ( FAILED(hr) )
	{
		//ERROR, GetDC should succeed
		DPF( 1, "TestRenderThenGetDC() - GetDC failed\n" ) ;
		goto Exit;
	}

	//This is used to Flush the 3D pipeline
	if (FAILED( m_pSrcDevice->Present(NULL, NULL, NULL, NULL) ) )
	{
		DPF( 1, "TestRenderThenGetDC() - Present failed\n" ) ;
		goto Exit;
	}

	//Re-GetDC
	hr = pSurf->GetDC(&m_hDXDC[0]);
	if ( FAILED(hr) )
	{
		//ERROR, GetDC should succeed
		DPF( 1, "TestRenderThenGetDC() - GetDC failed\n" ) ;
		goto Exit;
	}

	//Compare DCs
	bRes = CompareDCs(m_hComparisonDC, m_hDXDC[0]);

Exit:
	return bRes;
}

bool CGetDC::TestGetDCThenRender(LPSURFACE pSurf)
{
	//Do this test only on render targets
	if ( ! (m_dwSurfaceType == SURF_BACKBUFFER || 
			m_dwSurfaceType == SURF_RENDERTARGET ||
			m_dwSurfaceType == SURF_SWAPCHAIN ) )
		return true;

	bool bRes = false;

	//Set the pixels of the DX DC
	if ( !SetPixels(m_hDXDC[0]) )
	{
		DPF( 1, "TestGetDCThenRender() - SetPixels failed\n" ) ;
		goto Exit;
	}

	//Release DC
	HRESULT hr = pSurf->ReleaseDC(m_hDXDC[0]);
	m_hDXDC[0] = NULL;
	if ( FAILED(hr) )
	{
		//ERROR, GetDC should succeed
		DPF( 1, "TestGetDCThenRender() - GetDC failed\n" ) ;
		goto Exit;
	}

	if (! RenderQuad())
	{
		DPF( 1, "TestGetDCThenRender() - RenderQuad() failed\n" ) ;
		goto Exit;
	}

	//This is used to Flush the 3D pipeline
	if (FAILED( m_pSrcDevice->Present(NULL, NULL, NULL, NULL) ) )
	{
		DPF( 1, "TestGetDCThenRender() - Present failed\n" ) ;
		goto Exit;
	}

	//Re-GetDC
	hr = pSurf->GetDC(&m_hDXDC[0]);
	if ( FAILED(hr) )
	{
		//ERROR, GetDC should succeed
		DPF( 1, "TestGetDCThenRender() - GetDC failed\n" ) ;
		goto Exit;
	}

	//Compare DCs ( the comparison should fail since we rendered over the dc )
	if (CompareDCs(m_hComparisonDC, m_hDXDC[0], false))
	{
		DPF( 1, "TestGetDCThenRender() - The DC as not been overwritten by rendering\n" ) ;
		goto Exit;
	
	}

	bRes = true;

Exit:
	return bRes;
}

bool CGetDC::TestReleasedDCUnusable(LPSURFACE pSurf)
{
	//This test fails on win9x, so don't run it.
	OSVERSIONINFO VersionInfo;
	memset(&VersionInfo, 0, sizeof(OSVERSIONINFO));
	VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&VersionInfo);
	if(VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) // Windows 9x Variants
	{
		return true;
	}

	//Get the DC
	HDC hDirectXDC = NULL;
	HRESULT hr = pSurf->GetDC(&hDirectXDC);
	if ( FAILED( hr ) || hDirectXDC == NULL)
	{
		//ERROR, GetDC should Succeed
		DPF( 1, "GetDC() failed\n" ) ;
		return false;
	}

	//Release the DC
	hr = pSurf->ReleaseDC(hDirectXDC);
	if (FAILED(hr))
	{
		DPF( 1, "TestReleasedDCUnusable() ReleaseDC failed\n" );
		return false;
	}

	//Try to use the released DC
	COLORREF rgb = RGB(0x80, 0xb0, 0xff);
	COLORREF rgbSet = SetPixel(hDirectXDC, 1, 1, rgb);
	if (rgbSet != -1)
	{
		DPF( 1, "TestReleasedDCUnusable() SetPixel Succeeded after ReleaseDC\n" );
		return false;
	}

	return true;
}

bool CGetDC::TestReleaseDCTwice(LPSURFACE pSurf)
{
	//Get the DC
	HDC hDirectXDC = NULL;
	HRESULT hr = pSurf->GetDC(&hDirectXDC);
	if ( FAILED( hr ) || hDirectXDC == NULL)
	{
		//ERROR, GetDC should Succeed
		DPF( 1, "GetDC() failed\n" ) ;
		return false;
	}

	//Release the DC
	hr = pSurf->ReleaseDC(hDirectXDC);
	if (FAILED(hr))
	{
		DPF( 1, "ReleaseDCTwice() first ReleaseDC failed\n" );
		return false;
	}

	//Release it again.
	hr = pSurf->ReleaseDC(hDirectXDC);
	if (SUCCEEDED(hr))
	{
		DPF( 1, "ReleaseDCTwice() second ReleaseDC failed\n" );
		return false;
	}

	return true;
}

bool CGetDC::TestReleaseDCOnOtherSurf(LPSURFACE pSurf1, LPSURFACE pSurf2)
{
	//Can't do this test with backbuffer, since the 2 surfaces will be the same.
	if (m_dwSurfaceType == SURF_BACKBUFFER)
		return true;

	//Get the DC
	HDC hDirectXDC = NULL;
	HRESULT hr = pSurf1->GetDC(&hDirectXDC);
	if ( FAILED( hr ) || hDirectXDC == NULL)
	{
		//ERROR, GetDC should Succeed
		DPF( 1, "GetDC() failed\n" ) ;
		return false;
	}

	try
	{
		//Release the DC on the other surface
		hr = pSurf2->ReleaseDC(hDirectXDC);
		if (SUCCEEDED(hr))
		{
			DPF( 1, "ReleaseDCOnOtherSurf() ReleaseDC on other surface succeeded\n" );
			return false;
		}
		//Release the DC on the good surface
		hr = pSurf1->ReleaseDC(hDirectXDC);
		if (FAILED(hr))
		{
			DPF( 1, "ReleaseDCOnOtherSurf() ReleaseDC failed on the good surface\n" );
			return false;
		}
	}
	catch(...)
	{
		DPF( 1, "ReleaseDCOnOtherSurf() ReleaseDC FAULTED.\n" );
		return false;
	}

	return true;
}

bool CGetDC::TestReleaseBogusDC(LPSURFACE pSurf)
{
	bool result = true;

	//Get the DC
	HDC hDirectXDC = NULL;
	HRESULT hr = pSurf->GetDC(&hDirectXDC);
	if ( FAILED( hr ) || hDirectXDC == NULL)
	{
		//ERROR, GetDC should Succeed
		DPF( 1, "GetDC() failed\n" ) ;
		result = false;
	}

	HDC hBogusDCs[] = 
	{
		(HDC)0x087564ae,
		(HDC)(LONG)0xffffffff,
		(HDC)(LONG)0xcdcdcdcd,
		(HDC)hDirectXDC + 1,
		(HDC)hDirectXDC - 1,
		(HDC)NULL
	};
	UINT nBogusDC = sizeof(hBogusDCs) / sizeof(HDC);

	//Try to release the bogus DCs
	for (int i=0; i<nBogusDC; i++)
	{
		hr = pSurf->ReleaseDC(hBogusDCs[i]);
		if (SUCCEEDED(hr))
		{
			DPF( 1, "ReleaseBogusDC() ReleaseDC succeeded with DC 0x%x\n", hBogusDCs[i] );
			result = false;
		}
	}

	//Release the DC.
	hr = pSurf->ReleaseDC(hDirectXDC);
	if (FAILED(hr))
	{
		DPF( 1, "ReleaseBogusDC() ReleaseDC failed after trying to release bogus DCs\n" );
		result = false;
	}

	return result;
}

bool CGetDC::TestGetDCBadHandlePtr(LPSURFACE pSurf)
{
	bool result = true;
	HRESULT hr;

	HDC hBogusDCs[] = 
	{
		(HDC)0x087564ae,
		(HDC)(LONG)0xffffffff,
		(HDC)(LONG)0xcdcdcdcd,
		(HDC)NULL
	};
	UINT nBogusDC = sizeof(hBogusDCs) / sizeof(HDC);

	//Try to get the DC with bogus DCs
	for (int i=0; i<nBogusDC; i++)
	{
		//Get the DC
		hr = pSurf->GetDC(&hBogusDCs[i]);
		if ( FAILED( hr ) || hBogusDCs[i] == NULL)
		{
			//ERROR, GetDC should Succeed
			DPF( 1, "GetDCBadHandlePtrs() GetDC failed with DC 0x%x\n", hBogusDCs[i] );
			result = false;
		}

		//Release the DC
		hr = pSurf->ReleaseDC(hBogusDCs[i]);
		if ( FAILED( hr ) )
		{
			//ERROR, ReleaseDC should Succeed
			DPF( 1, "GetDCBadHandlePtrs() ReleaseDC failed with DC 0x%x\n", hBogusDCs[i] );
			result = false;
		}
	}

	HDC* phBogusPtrs[] = 
	{
		(HDC*)0x087564ae,
		(HDC*)(LONG)0xffffffff,
		(HDC*)(LONG)0xcdcdcdcd,
		(HDC*)NULL
	};
	UINT nBogusPtrs = sizeof(phBogusPtrs) / sizeof(HDC*);
	for (int i=0; i<nBogusPtrs; i++)
	{
		//Get the DC.
		hr = pSurf->GetDC(phBogusPtrs[i]);
		if (SUCCEEDED(hr))
		{
			DPF( 1, "GetDCBadHandlePtrs() GetDC succeeded with bad handle ptr 0x%x\n", phBogusPtrs[i] );
			result = false;
		}
	}

	return true;
}

bool CGetDC::TestGetMultipleDCs(LPSURFACE pSurf)
{
	//Get the DC
	HDC hDirectXDC1 = NULL;
	HDC hDirectXDC2 = NULL;
	HRESULT hr = pSurf->GetDC(&hDirectXDC1);
	if ( FAILED( hr ) || hDirectXDC1 == NULL)
	{
		//ERROR, GetDC should Succeed
		DPF( 1, "GetDC() failed\n" ) ;
		return false;
	}

	//Try to get another DC
	hr = pSurf->GetDC(&hDirectXDC2);
	if ( SUCCEEDED( hr ) || hDirectXDC2 != NULL)
	{
		//ERROR, GetDC should fail the second time
		DPF( 1, "TestGetMultipleDCs() The second GetDC() succeeded\n" ) ;
		return false;
	}

	//Release the DC
	hr = pSurf->ReleaseDC(hDirectXDC1);
	if (FAILED(hr))
	{
		DPF( 1, "ReleaseDCTwice() first ReleaseDC failed\n" );
		return false;
	}

	return true;
}

bool CGetDC::TestMemoryLeaks(LPSURFACE pSurf)
{
    MEMORYSTATUS memstat;
	SIZE_T nMemBefore, nMemAfter, nMemLeaked;

    // check memory
    GlobalMemoryStatus( &memstat );
	nMemBefore = memstat.dwAvailVirtual;

	for (int i=0; i<500000; i++)
	{
		//Get the DC
		HDC hDirectXDC = NULL;
		HRESULT hr = pSurf->GetDC(&hDirectXDC);
		if ( FAILED( hr ) || hDirectXDC == NULL)
		{
			//ERROR, GetDC should Succeed
			DPF( 1, "TestMemoryLeaks() GetDC failed\n" ) ;
		}

		//Release the DC
		hr = pSurf->ReleaseDC(hDirectXDC);
		if (FAILED(hr))
		{
			DPF( 1, "TestMemoryLeaks() ReleaseDC failed\n" );
			return false;
		}
	}

	// set the after and test that we didn't leak
	nMemAfter = memstat.dwAvailVirtual;
	nMemLeaked = nMemAfter - nMemBefore;
	if ( nMemLeaked > 0 )
	{
		DPF( 1, "TestMemoryLeaks() leaked %d bytes\n", nMemLeaked );
		return false;
	}

	return true;
}

bool CGetDC::TestCreateDCOnLostDev(LPSURFACE pSurf)
{
	bool result = false;
	m_bNeedToReset = true;

	//Release the previous DC
	HRESULT hr = pSurf->ReleaseDC(m_hDXDC[0]);
	m_hDXDC[0] = NULL;
	if ( FAILED(hr) )
	{
		DPF( 1, "TestCreateDCOnLostDev() The DC could not be released\n" );
		return false;
	}

	//Lose the device
	if ( !LoseTheDevice() )
	{
		DPF( 1, "TestCreateDCOnLostDev() Could not lose the device\n");
		goto Finalize;
	}

	//Get a DC
	hr = pSurf->GetDC(&m_hDXDC[0]);
	if ( FAILED(hr) )
	{
		DPF( 1, "TestCreateDCOnLostDev() Could not Get a DC : %s\n", m_pSrcDevice->GetD3D()->HResultToString(hr));
		goto Finalize;
	}

	//Set the pixels of the DirectX DC
	if ( !SetPixels(m_hDXDC[0]) )
	{
		goto Finalize;
	}

	//Compare DCs
	result = CompareDCs(m_hComparisonDC, m_hDXDC[0]);

Finalize:

	return result;
}

bool CGetDC::TestUseDCOnLostDev()
{
	bool result = false;
	m_bNeedToReset = true;

	//Set the pixels of the DX DC
	if ( !SetPixels(m_hDXDC[0]) )
	{
		goto Finalize;
	}

	//Lose the device
	if ( !LoseTheDevice() )
	{
		DPF( 1, "TestCreateDCOnLostDev() Could not lose the device\n");
		goto Finalize;
	}

	//Compare DCs
	//Except on pool default since the dc now contains garbage (caused by lostdev)
	if (m_dwPool == POOL_DEFAULT)
		return true;

	if ( !CompareDCs(m_hComparisonDC, m_hDXDC[0]) )
	{
		DPF( 1, "TestCreateDCOnLostDev() the DCs are different\n");
		goto Finalize;
	}

	//Set the pixels of the DX DC to make sure it doesn't crash!
	if ( !SetPixels(m_hDXDC[0]) )
	{
		DPF( 1, "TestCreateDCOnLostDev() SetPixels() after lost device failed\n");
		goto Finalize;
	}

	//Compare DCs
	result = CompareDCs(m_hComparisonDC, m_hDXDC[0]);

Finalize:

	return result;
}

bool CGetDC::TestUseDCAfterBeginScene()
{
	BeginScene();

	//Set the pixels of the Screen DC
	if ( !SetPixels(m_hDXDC[0]) )
	{
		goto Finalize;
	}

	//Compare DCs
	bool result = CompareDCs(m_hComparisonDC, m_hDXDC[0]);

Finalize:
	EndScene();

	return result;
}

bool CGetDC::TestGDIAttributes()
{
	return true;
}

bool CGetDC::TestGDIClipping()
{
	return true;
}

bool CGetDC::TestHighLevelGDIOperations()
{
	return true;
}


