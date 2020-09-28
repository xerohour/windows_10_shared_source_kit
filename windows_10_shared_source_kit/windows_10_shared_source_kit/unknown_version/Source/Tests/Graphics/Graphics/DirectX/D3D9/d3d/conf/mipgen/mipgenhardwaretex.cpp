#include <d3d9.h>
#include <d3dx9.h>
#include "MipGen.h"

CMipGenHWTexture::CMipGenHWTexture()
{
	m_szTestName = "AutoGenMipMap with textures";
	m_szCommandKey = "Texture";

	// Init framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Initialize some variables
    m_pSrcTexture = NULL;
    m_pRefTexture = NULL;
	m_pPatternTexture = NULL;
	m_pPatternSurface = NULL;

	m_RTYPECurrent = RTYPE_TEXTURE;
}

bool CMipGenHWTexture::CapsCheck()
{
	if (! CMipGenHardware::CapsCheck() )
		return false;

	return CheckTextureCaps();
}

bool CMipGenHWTexture::CreateBaseTexture()
{
	RELEASE(m_pSrcTexture);
	RELEASE(m_pRefTexture);
    // Create the Src VidMem texture
	HRESULT hr = m_pSrcDevice->CreateTexture(256,256,0,m_pCurrentCase->dwUsage,m_pCurrentCase->format,m_pCurrentCase->dwPool,(LPTEXTURE*)&m_pSrcTexture);

	// Indicate a CreateTexture failure
	if (FAILED(hr) || (NULL == m_pSrcTexture))
	{
		if (hr == D3DERR_OUTOFVIDEOMEMORY)
			Skip();
		WriteToLog("CreateBaseTexture(m_pSrcTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

	// Create the Ref VidMem texture
	hr = m_pRefDevice->CreateTexture(256,256,0,USAGE_RENDERTARGET,m_pCurrentCase->format,POOL_DEFAULT,(LPTEXTURE*)&m_pRefTexture);

	// Indicate a CreateTexture failure
	if (FAILED(hr) || (NULL == m_pRefTexture))
	{
		if (hr == D3DERR_OUTOFVIDEOMEMORY)
			Skip();
		WriteToLog("CreateBaseTexture(m_pRefTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		return false;
	}

	return true;
}

bool CMipGenHWTexture::CreatePatternSurface()
{
	ReleasePatternSurface();

	if (FAILED(m_pSrcDevice->CreateTexture(256, 256, 1, 0, m_pCurrentCase->format, POOL_SYSTEMMEM, &m_pPatternTexture)))
	{
		WriteToLog("CreatePatternSurface() - CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		return false;
	}
	//Get the top level of the texture
	if (FAILED(m_pPatternTexture->GetSurfaceLevel(0, &m_pPatternSurface)))
	{
		WriteToLog("CreatePatternSurface() - GetSurfaceLevel(m_pPatternSurface) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		RELEASE(m_pPatternTexture);
		return false;
	}

	if (!WritePatternOnSurface(m_pPatternSurface, m_pCurrentCase->dwPattern))
	{
		WriteToLog("CreatePatternSurface() - WritePatternOnSurface() failed.\n");
		return false;
	}

	return true;
}

void CMipGenHWTexture::ReleasePatternSurface()
{
	RELEASE(m_pPatternSurface);
	RELEASE(m_pPatternTexture);
}


bool CMipGenHWTexture::WritePatternOnRef()
{
	LPSURFACE pTempSurf = NULL;
	LPTEXTURE pTempTex = NULL;
	bool bResult = false;

	if (FAILED(m_pRefDevice->CreateTexture(256, 256, 0, 0, m_pCurrentCase->format, POOL_SYSTEMMEM, &pTempTex)))
	{
		WriteToLog("WritePatternOnRef() - CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	if (FAILED(pTempTex->GetSurfaceLevel(0, &pTempSurf)))
	{
		WriteToLog("WritePatternOnRef() - GetSurfaceLevel(pTempSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}
	
	//If we use subrect, then colorfill the surface
	if (m_pCurrentCase->bUseSubRect)
	{
		if (! CopyFromSurfAToSurfB(m_pColorFilledSurface, pTempSurf, false) )
		{
			WriteToLog("WritePatternOnRef() CopyFromSurfAToSurfB(From m_pColorFilledSurface) failed");
			goto Exit;
		}
	}

	if (! CopyFromSurfAToSurfB(m_pPatternSurface, pTempSurf, m_pCurrentCase->bUseSubRect) )
	{
		WriteToLog("WritePatternOnRef() CopyFromSurfAToSurfB(From m_pPatternSurface) failed");
		goto Exit;
	}

	if (FAILED(m_pRefDevice->UpdateTexture(pTempTex, m_pRefTexture)))
	{
		WriteToLog("WritePatternOnRef() - UpdateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	bResult = true;

Exit:
	RELEASE(pTempTex);
	RELEASE(pTempSurf);

	return bResult;
}

bool CMipGenHWTexture::ColorFillTopLevel()
{
	//Get the top level
	LPSURFACE pSrcSurf = NULL;
	if (FAILED(((LPTEXTURE)m_pSrcTexture)->GetSurfaceLevel(0, &pSrcSurf)))
	{
		WriteToLog("ColorFillTopLevel() - GetSurfaceLevel(pSrcSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		return false;
	}

	if (m_pCurrentCase->dwPool == POOL_DEFAULT)
	{
		//Update the top level with the color filled surf
		if (FAILED(m_pSrcDevice->UpdateSurface(m_pColorFilledSurface, NULL, pSrcSurf, NULL)))
		{
			WriteToLog("ColorFillTopLevel() - UpdateSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			RELEASE(pSrcSurf);
			return false;
		}
	}
	else //POOL_MANAGED
	{
		//fill top level with color
		if (! CopyFromSurfAToSurfB(m_pColorFilledSurface, pSrcSurf) )
		{
			WriteToLog("ColorFillTopLevel() CopyFromSurfAToSurfB(m_pColorFilledSurface, pSrcSurf) failed");
			RELEASE(pSrcSurf);
			return false;
		}
	}
	
	RELEASE(pSrcSurf);
	return true;
}

bool CMipGenHWTexture::SetAutoGenFilter()
{
	if (FAILED(((LPTEXTURE)m_pSrcTexture)->SetAutoGenFilterType(m_pCurrentCase->dwAutoGenFilter)))
	{
		WriteToLog("SetAutoGenFilter() SetAutoGenFilterType() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		return false;
	}

	return true;
}

bool CMipGenHWTexture::RenderRefMipLevels()
{
	HRESULT hr;
	LPSURFACE pRTSurf = NULL, pOriginalRT = NULL;
	LPTEXTURE pTempTex = NULL;
	LPTEXTURE pPreviousTex = NULL;
	LPTEXTURE pTempRTTex = NULL;
	LPSURFACE pTempSurf = NULL;
	LPSURFACE pPreviousSurf = NULL;
	bool bResult = false;

	// Get the current render target
	if (FAILED(m_pRefDevice->GetRenderTarget(&pOriginalRT)))
	{
		WriteToLog("RenderRefMipLevels - GetRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	//Set the min filter (autogen filter) (also set the mag filter since some driver have problems with that)
	m_pRefDevice->SetSamplerState(0, SAMP_MAXMIPLEVEL, 0);
    m_pRefDevice->SetSamplerState(0, SAMP_MIPFILTER, TEXF_NONE);
	m_pRefDevice->SetSamplerState(0, SAMP_MINFILTER, m_pCurrentCase->dwAutoGenFilter);
	if ( m_bMagTexFilSupportedRef[m_pCurrentCase->uAutoGenFilterIndex] )
		m_pRefDevice->SetSamplerState(0, SAMP_MAGFILTER, m_pCurrentCase->dwAutoGenFilter);

	UINT size = 256;

	if (m_pCurrentCase->dwGenFunc == FUNC_SETRENDERTARGET)
	{
		// Create the temp texture
		hr = m_pRefDevice->CreateTexture(size,size,1,USAGE_RENDERTARGET,m_pCurrentCase->format,POOL_DEFAULT,&pPreviousTex);
		if (FAILED(hr) || (NULL == pPreviousTex))
		{
			WriteToLog("RenderRefMipLevels - CreateTexture(pPreviousTex) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}

		// Get the temp texture top level surface
		hr = pPreviousTex->GetSurfaceLevel(0, &pTempSurf);
		if (FAILED(hr) || (NULL == pTempSurf))
		{
			WriteToLog("RenderRefMipLevels - GetSurfaceLevel(pTempSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}

		m_pRefDevice->SetTexture(0, m_pRefTexture);

		// Set the current surface as the render target
		if (FAILED(m_pRefDevice->SetRenderTarget((DWORD)0, pTempSurf)))
		{
			WriteToLog("RenderRefMipLevels - SetRenderTarget(pTempSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}

		//Render on the temp texture
		DrawTexture(0, 0, 256, CD3D_REF);
	}
	else
	{
		// Create the temp texture
		hr = m_pRefDevice->CreateTexture(size,size,1,0,m_pCurrentCase->format,POOL_MANAGED,&pPreviousTex);
		if (FAILED(hr) || (NULL == pPreviousTex))
		{
			WriteToLog("RenderRefMipLevels - CreateTexture(pPreviousTex) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}

		// Get the temp texture top level surface
		hr = pPreviousTex->GetSurfaceLevel(0, &pTempSurf);
		if (FAILED(hr) || (NULL == pTempSurf))
		{
			WriteToLog("RenderRefMipLevels - GetSurfaceLevel(pTempSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}

		//If we use subrect, then colorfill the surface
		if (m_pCurrentCase->bUseSubRect)
		{
			if (! CopyFromSurfAToSurfB(m_pColorFilledSurface, pTempSurf, false) )
			{
				WriteToLog("MethodLock() CopyFromSurfAToSurfB(From m_pColorFilledSurface) failed");
				goto Exit;
			}
		}

		if (! CopyFromSurfAToSurfB(m_pPatternSurface, pTempSurf, m_pCurrentCase->bUseSubRect) )
		{
			WriteToLog("RenderRefMipLevels - CopyFromSurfAToSurfB() failed.\n");
			goto Exit;
		}
	}

	RELEASE(pTempSurf);

	for (int i = 1; i < 9; ++i)
	{
		// Use the previous level to generate the current level
		size = 1 << (9 - i);

		// Create the temp texture
		hr = m_pRefDevice->CreateTexture(size,size,1,0,m_pCurrentCase->format,POOL_SYSTEMMEM,&pTempTex);
		if (FAILED(hr) || (NULL == pTempTex))
		{
			WriteToLog("RenderRefMipLevels - CreateTexture(pTempTex) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}

		// Create the temp RT texture
		hr = m_pRefDevice->CreateTexture(size,size,1,USAGE_RENDERTARGET,m_pCurrentCase->format,POOL_DEFAULT,&pTempRTTex);
		if (FAILED(hr) || (NULL == pTempRTTex))
		{
			WriteToLog("RenderRefMipLevels - CreateTexture(pTempRTTex) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}

		// Get the temp texture top level surface
		hr = pTempTex->GetSurfaceLevel(0, &pTempSurf);
		if (FAILED(hr) || (NULL == pTempSurf))
		{
			WriteToLog("RenderRefMipLevels - GetSurfaceLevel(pTempSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}

		// Get the previous mip surface
		hr = ((LPTEXTURE)m_pRefTexture)->GetSurfaceLevel(i-1, &pPreviousSurf);
		if (FAILED(hr) || (NULL == pPreviousSurf))
		{
			WriteToLog("RenderRefMipLevels - GetSurfaceLevel(pPreviousSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}

		// Get the current mip surface
		hr = ((LPTEXTURE)m_pRefTexture)->GetSurfaceLevel(i, &pRTSurf);
		if (FAILED(hr) || (NULL == pRTSurf))
		{
			WriteToLog("RenderRefMipLevels - GetSurfaceLevel(pRTSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}

		//Copy the previous level to the temp texture
		if ( FAILED(m_pRefDevice->GetRenderTargetData(pPreviousSurf, pTempSurf)) )
		{
			WriteToLog("RenderRefMipLevels - GetRenderTargetData() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}
		if ( FAILED(m_pRefDevice->UpdateTexture(pTempTex, pTempRTTex)) )
		{
			WriteToLog("RenderRefMipLevels - UpdateTexture(pTempTex, pTempRTTex) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}

		//Render the previous level on the current one
		m_pRefDevice->SetTexture(0, pTempRTTex);

		// Set the current surface as the render target
		if (FAILED(m_pRefDevice->SetRenderTarget((DWORD)0, pRTSurf)))
		{
			WriteToLog("RenderRefMipLevels - SetRenderTarget(pRTSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}

		size = 1 << (8 - i);

		//Render on the ref texture mip level
		DrawTexture(0, 0, size, CD3D_REF);

		// Set the current surface as the render target
		//if (FAILED(m_pRefDevice->SetRenderTarget((DWORD)0, pTempSurf)))
		//{
		//	WriteToLog("RenderRefMipLevels - SetRenderTarget(pTempSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		//	goto Exit;
		//}

		////Render on the temp texture
		//DrawTexture(0, 0, size, CD3D_REF);

		////Set the previous texture
		//RELEASE(pPreviousTex);
		//pPreviousTex = pTempTex;
		//pTempTex = NULL;

		RELEASE(pRTSurf);
		RELEASE(pPreviousSurf);
		RELEASE(pTempSurf);
		RELEASE(pTempTex);
		RELEASE(pTempRTTex);
	}

	bResult = true;

Exit:

	m_pRefDevice->SetTexture(0, m_pRefTexture);

	// Set render target back to original
	if (FAILED(m_pRefDevice->SetRenderTarget((DWORD)0, pOriginalRT)))
	{
		WriteToLog("RenderRefMipLevels - SetRenderTarget(pOriginalRT) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		bResult = false;
	}

	// Cleanup surfaces
	RELEASE(pRTSurf);
	RELEASE(pTempSurf);
	RELEASE(pTempTex);
	RELEASE(pPreviousTex);
	RELEASE(pOriginalRT);

	return bResult;
}


bool CMipGenHWTexture::RenderScene()
{
	return RenderTextureScene();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//		Methods for all the Generation Functions
//
/////////////////////////////////////////////////////////////////////////////////////////////////

bool CMipGenHWTexture::MethodLock()
{
	LPSURFACE pSrcSurf = NULL;
	bool bResult = false;

	//Get the top level
	if (FAILED(((LPTEXTURE)m_pSrcTexture)->GetSurfaceLevel(0, &pSrcSurf)))
	{
		WriteToLog("MethodLock() - GetSurfaceLevel(pSrcSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	//Copy pattern on top level
	if (! CopyFromSurfAToSurfB(m_pPatternSurface, pSrcSurf) )
	{
		WriteToLog("MethodLock() CopyPatternOnSurface(pSrcSurf) failed");
		goto Exit;
	}

	bResult = WritePatternOnRef();

	//bResult = true;
Exit:

	RELEASE(pSrcSurf);

	return bResult;
}


bool CMipGenHWTexture::MethodUpdateSurface()
{
	LPSURFACE pSrcSurf = NULL;
	bool bResult = false;
	RECT rect = {64, 64, 192, 192};
	POINT point = {64, 64};

	RECT *pRect = NULL;
	POINT * pPoint = NULL;
	if (m_pCurrentCase->bUseSubRect)
	{
		pRect = &rect;
		pPoint = &point;
	}

	//Get the top level
	if (FAILED(((LPTEXTURE)m_pSrcTexture)->GetSurfaceLevel(0, &pSrcSurf)))
	{
		WriteToLog("MethodUpdateSurface() - GetSurfaceLevel(pSrcSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	//Update the top level with the pattern surf
	if (FAILED(m_pSrcDevice->UpdateSurface(m_pPatternSurface, pRect, pSrcSurf, pPoint)))
	{
		WriteToLog("MethodUpdateSurface() - UpdateSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	bResult = WritePatternOnRef();

	//bResult = true;
Exit:

	RELEASE(pSrcSurf);

	return bResult;
}

bool CMipGenHWTexture::MethodUpdateTexture()
{
	LPTEXTURE pTempTex = NULL;
	LPSURFACE pTempSurf = NULL;
	bool bResult = false;

	//Create a temp texture
	if (FAILED(m_pSrcDevice->CreateTexture(256, 256, 1, 0, m_pCurrentCase->format, POOL_SYSTEMMEM, &pTempTex)))
	{
		WriteToLog("MethodUpdateTexture() - CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}
	
	//Get the top level of the temp texture
	if (FAILED(pTempTex->GetSurfaceLevel(0, &pTempSurf)))
	{
		WriteToLog("MethodUpdateTexture() - GetSurfaceLevel(pSrcSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	//copy pattern on temp surface
	if (! CopyFromSurfAToSurfB(m_pPatternSurface, pTempSurf) )
	{
		WriteToLog("MethodUpdateTexture() CopyPatternOnSurface(pTempSurf) failed");
		goto Exit;
	}

	//Update the texture with the temp tex
	if (FAILED(m_pSrcDevice->UpdateTexture(pTempTex, m_pSrcTexture)))
	{
		WriteToLog("MethodUpdateTexture() - UpdateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	bResult = WritePatternOnRef();

	//bResult = true;
Exit:

	RELEASE(pTempSurf);
	RELEASE(pTempTex);

	return bResult;
}

bool CMipGenHWTexture::MethodStretchRect()
{
	LPSURFACE pSrcSurf = NULL;
	LPSURFACE pTempSurf = NULL;
	bool bResult = false;
	RECT rect = {64, 64, 192, 192};

	RECT *pRect = NULL;
	if (m_pCurrentCase->bUseSubRect)
	{
		pRect = &rect;
	}

	//Get the top level
	if (FAILED(((LPTEXTURE)m_pSrcTexture)->GetSurfaceLevel(0, &pSrcSurf)))
	{
		WriteToLog("MethodStretchRect() - GetSurfaceLevel(pSrcSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	//Create a temp surface
	if (m_pCurrentCase->bUseOffscreenPlain)
	{
		if (FAILED(m_pSrcDevice->CreateOffscreenPlainSurface(256, 256, m_pCurrentCase->format, POOL_DEFAULT, &pTempSurf)))
		{
			WriteToLog("MethodStretchRect() - CreateOffscreenPlainSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}
	}
	else if (m_pCurrentCase->bUsePlainRT)
	{
		if (FAILED(m_pSrcDevice->CreateRenderTarget(256, 256, m_pCurrentCase->format, (MULTISAMPLE_TYPE)0, 0, true, &pTempSurf)))
		{
			WriteToLog("MethodStretchRect() - CreateRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			goto Exit;
		}
	}

	//Copy pattern on temp surface
	if (! CopyFromSurfAToSurfB(m_pPatternSurface, pTempSurf) )
	{
		WriteToLog("MethodStretchRect() CopyPatternOnSurface(pTempSurf) failed");
		goto Exit;
	}

	//StretchRect with only a part stretch factor
	if (FAILED(m_pSrcDevice->StretchRect(pTempSurf, pRect, pSrcSurf, pRect, TEXF_NONE)))
	{
		WriteToLog("MethodStretchRect() - StretchRect() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	bResult = WritePatternOnRef();

	//bResult = true;
Exit:

	RELEASE(pSrcSurf);
	RELEASE(pTempSurf);

	return bResult;
}

bool CMipGenHWTexture::MethodSetRenderTarget()
{
	LPTEXTURE pTempTex = NULL;
	LPSURFACE pTempSurf = NULL;
	LPSURFACE pSrcSurf = NULL;
	LPSURFACE pOriginalRT = NULL;
	bool bResult = false;

	// Get the current render target
	if (FAILED(m_pSrcDevice->GetRenderTarget(&pOriginalRT)))
	{
		WriteToLog("MethodSetRenderTarget - GetRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	//Create a temp texture
	if (FAILED(m_pSrcDevice->CreateTexture(256, 256, 1, 0, m_pCurrentCase->format, POOL_MANAGED, &pTempTex)))
	{
		WriteToLog("MethodSetRenderTarget() - CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}
	
	//Get the top level of the temp texture
	if (FAILED(pTempTex->GetSurfaceLevel(0, &pTempSurf)))
	{
		WriteToLog("MethodSetRenderTarget() - GetSurfaceLevel(pSrcSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	//copy pattern on temp surface
	if (! CopyFromSurfAToSurfB(m_pPatternSurface, pTempSurf) )
	{
		WriteToLog("MethodSetRenderTarget() CopyPatternOnSurface(pTempSurf) failed");
		goto Exit;
	}

	//Set the min filter (autogen filter) (also set the mag filter since some driver have problems with that)
    m_pSrcDevice->SetSamplerState(0, SAMP_MIPFILTER, TEXF_NONE);
	m_pSrcDevice->SetSamplerState(0, SAMP_MINFILTER, m_pCurrentCase->dwAutoGenFilter);
	if ( m_bMagTexFilSupported[m_pCurrentCase->uAutoGenFilterIndex] )
		m_pSrcDevice->SetSamplerState(0, SAMP_MAGFILTER, m_pCurrentCase->dwAutoGenFilter);

	//Set the texture to render from
	m_pSrcDevice->SetTexture(0, pTempTex);

	//Get the top level
	if (FAILED(((LPTEXTURE)m_pSrcTexture)->GetSurfaceLevel(0, &pSrcSurf)))
	{
		WriteToLog("MethodSetRenderTarget() - GetSurfaceLevel(pSrcSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	//Set the top level as the render target
	if (FAILED(m_pSrcDevice->SetRenderTarget((DWORD)0, pSrcSurf)))
	{
		WriteToLog("MethodSetRenderTarget() - SetRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	//Render
	DrawTexture(0, 0, 256, CD3D_SRC);

	bResult = WritePatternOnRef();

	//bResult = true;
Exit:

	// Set render target back to original
	if (FAILED(m_pSrcDevice->SetRenderTarget((DWORD)0, pOriginalRT)))
	{
		WriteToLog("MethodSetRenderTarget - SetRenderTarget(pOriginalRT) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		bResult = false;
	}

	//Reset the original texture
	m_pSrcDevice->SetTexture(0, m_pSrcTexture);

	RELEASE(pSrcSurf);
	RELEASE(pTempSurf);
	RELEASE(pTempTex);
	RELEASE(pOriginalRT);

	return bResult;
}

bool CMipGenHWTexture::MethodGetDC()
{
	return true;
}

bool CMipGenHWTexture::MethodColorFill()
{
	LPSURFACE pSrcSurf = NULL;
	bool bResult = false;
	RECT rect = {64,64,192,192};

	if (FAILED(((LPTEXTURE)m_pSrcTexture)->GetSurfaceLevel(0, &pSrcSurf)))
	{
		WriteToLog("MethodColorFill() - GetSurfaceLevel(pSrcSurf) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}
	
	if (FAILED(m_pSrcDevice->ColorFill(pSrcSurf, &rect, D3DCOLOR_ARGB(0x00,0x80,0x40,0xb0) )))
	{
		WriteToLog("MethodColorFill() ColorFill(pSrcSurf) failed");
		goto Exit;
	}

	bResult = WritePatternOnRef();

	//bResult = true;
Exit:

	RELEASE(pSrcSurf);

	return bResult;
}

bool CMipGenHWTexture::MethodEvictManagedResources()
{
	bool bResult = false;

	if ( !MethodLock())
	{
		WriteToLog("MethodEvictManagedResources() MethodLock() failed");
		goto Exit;
	}

	((LPTEXTURE)m_pSrcTexture)->GenerateMipSubLevels();

	if (FAILED(m_pSrcDevice->EvictManagedResources()))
	{
		WriteToLog("MethodEvictManagedResources() - EvictManagedResources() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	bResult = true;

Exit:

	return bResult;
}

bool CMipGenHWTexture::MethodLostDevice()
{
	return true;
}
