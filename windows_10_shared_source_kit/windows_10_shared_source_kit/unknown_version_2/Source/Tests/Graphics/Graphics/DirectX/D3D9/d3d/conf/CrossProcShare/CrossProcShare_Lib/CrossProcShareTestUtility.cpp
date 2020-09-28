
#include "CrossProcShareTest.h"
#include "CSurface.h"
#include "CTextures.h"
#include "CBuffers.h"
#include "CQuery.h"

// ----------------------------------------------------------------------------

bool CCrossProcShareTest::FillSurface(SURFACEDESC *pDesc, LOCKED_RECT *pLockedRect, bool bUseSubRectLock, UINT uPattern)
{
    PBYTE ptr;
    ptr = (PBYTE)pLockedRect->pBits;
    int PixelSize = pDesc->Format.ddpfPixelFormat.dwRGBBitCount / 8;
    UINT offset;

    DWORD dwHeight = pDesc->dwHeight;
    DWORD dwWidth = pDesc->dwWidth;
    if( true == bUseSubRectLock )
    {
        dwHeight -= pDesc->dwHeight - (pDesc->dwHeight / 4);
        dwWidth -= pDesc->dwWidth - (pDesc->dwWidth / 4);
    }

    int x, y;
    try
    {
        for( y=0; y<dwHeight; y++ )
        {
            for( x=0; x<dwWidth; x++ )
            {
                for( int k=0; k<PixelSize; k++ )
                {
                    offset=pLockedRect->iPitch*y+x*PixelSize+k;
                    switch( uPattern )
                    {
                        case 0:
                            *(ptr+offset)=(BYTE)(((y*k) / (x+k+1) * PixelSize * x)%255)+1;
                            break;

                        case 1:
                            *(ptr+offset)=(BYTE)((y*dwWidth + x*PixelSize+k)%255)+1;
                            break;
                    }
                }
            }
        }
    }
    catch (...)
    {
        Log(1, _T("EXCEPTION occured on fill surface on pixel %d,%d\n"), x, y);
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------

bool CCrossProcShareTest::FillSurface(LPSURFACE pSurface, bool bUseSubRectLock, UINT uPattern)
{
    PBYTE ptr;
    SURFACEDESC	Desc;
    LOCKED_RECT LockedRect;
    HRESULT hr = S_OK;

    if( FAILED(pSurface->GetDesc(&Desc)) )
    {
        Log( 1, _T("FillSurface() GetDesc failed with %s\n"),  m_pD3D->HResultToString(GetLastError()));
        return false;
    }

    if( FAILED( hr = pSurface->LockRect(&LockedRect, NULL, 0) ) )
    {
        Log( 1, _T("FillSurface() LockRect failed with %s\n"),  m_pD3D->HResultToString(hr));
        return false;
    }

    if( false == FillSurface(&Desc, &LockedRect, bUseSubRectLock, uPattern) )
    {
        Log( 1, _T("FillSurface() FillSurface failed\n") );
        return false;
    }

    if( FAILED(hr = pSurface->UnlockRect()) )
    {
        Log( 1, _T("FillSurface() UnlockRect failed with %s\n"),  m_pD3D->HResultToString(hr) );
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------

bool CCrossProcShareTest::FillSurface(LPTEXTURE pTexture, bool bUseSubRectLock, UINT uPattern)
{
    PBYTE ptr;
    SURFACEDESC	Desc;
    LOCKED_RECT LockedRect;
    HRESULT hr = S_OK;

    if( FAILED(hr = pTexture->GetLevelDesc(0, &Desc)) )
    {
        Log( 1, _T("FillSurface() GetDesc failed with %s\n"),  m_pD3D->HResultToString(hr));
        return false;
    }

    if( FAILED( hr = pTexture->LockRect(0, &LockedRect, NULL, 0) ) )
    {
        Log( 1, _T("FillSurface() LockRect failed with %s\n"),  m_pD3D->HResultToString(hr));
        return false;
    }

    if( false == FillSurface(&Desc, &LockedRect, bUseSubRectLock, uPattern) )
    {
        Log( 1, _T("FillSurface() FillSurface failed\n") );
        return false;
    }

    if( FAILED( hr = pTexture->UnlockRect(0) ) )
    {
        Log( 1, _T("FillSurface() UnlockRect failed with %s\n"),  m_pD3D->HResultToString(hr));
        return false;
    }

    return true;
}

bool CCrossProcShareTest::FillVolumeTexture(RESOURCE_CASE *pResCase, LPVOLUMETEXTURE pTexture, bool bUseSubRectLock, UINT uPattern)
{
    DWORD colors[4];
    UINT uIncrement = 256 / (pResCase->uDepth - 1);
    for( int iSlice=0; iSlice<pResCase->uDepth; iSlice++ )
    {
        int iValue = uIncrement * iSlice;
        switch( uPattern )
        {
            case 0:
                colors[0] = RGBA_MAKE(0, 0, 0, 0);
                colors[1] = RGBA_MAKE(255, 0, iValue, 0);
                colors[2] = RGBA_MAKE(0, 255, 0, 0);
                colors[3] = RGBA_MAKE(255, 255, iValue, 0);
                break;

            case 1:
                colors[0] = RGBA_MAKE(255, 255, 255, 0);
                colors[1] = RGBA_MAKE(255 - iValue, 0, 0, 0);
                colors[2] = RGBA_MAKE(0, 0, iValue, 0);
                colors[3] = RGBA_MAKE(0, 255 - iValue, 255, 0);
                break;
        }

        if( FAILED(pTexture->LoadGradient(0, iSlice, pResCase->uWidth, pResCase->uHeight, colors, TEXLOAD_DEFAULT)) )
            return false;
    }

    return true;
}

bool CCrossProcShareTest::FillCubeTexture(RESOURCE_CASE *pResCase, LPCUBETEXTURE pTexture, UINT uPattern)
{
    DWORD colors[3];
    switch( uPattern )
    {
        case 0:
            colors[0] = RGBA_MAKE(0,   0,   0,   0);
            colors[1] = RGBA_MAKE(255, 0,   0,   0);
            colors[2] = RGBA_MAKE(0,   255, 0,   0);
            break;

        case 1:
            colors[0] = RGBA_MAKE(255, 255, 255, 0);
            colors[1] = RGBA_MAKE(0,   255, 255, 0);
            colors[2] = RGBA_MAKE(255, 0,   255, 0);
            break;
    }

    if( FAILED(pTexture->LoadStripes(0, (CUBEMAP_FACES)0, pResCase->uWidth, pResCase->uHeight, 3, colors, true, false, TEXLOAD_DEFAULT)) )
        return false;

    return true;
}

bool CCrossProcShareTest::FillVertexBuffer(RESOURCE_CASE *pResCase, LPVERTEXBUFFER pVB, UINT uPattern)
{
    XYZRHWDIFFUSEVERTEX *pVertices = NULL;
    if( FAILED( pVB->Lock(0, 0, (VOID**)&pVertices, 0) ) )
    {
        Log( 1, _T("FillVertexBuffer() pVB->Lock() failed with %s\n"),  m_pD3D->HResultToString(GetLastError()));
        return false;
    }

    pVertices[0].x=10.0f; pVertices[0].y=50.0f; pVertices[0].z=1.0f; pVertices[0].rhw=1.0f;
    pVertices[1].x=10.0f; pVertices[1].y=10.0f; pVertices[1].z=1.0f; pVertices[1].rhw=1.0f;
    pVertices[2].x=50.0f; pVertices[2].y=50.0f; pVertices[2].z=1.0f; pVertices[2].rhw=1.0f;
    pVertices[3].x=50.0f; pVertices[3].y=10.0f; pVertices[3].z=1.0f; pVertices[3].rhw=1.0f;

    switch( uPattern )
    {
        case 0:
            pVertices[0].color = RGBA_MAKE(255,0,0,255);
            pVertices[1].color = RGBA_MAKE(255,0,0,255);
            pVertices[2].color = RGBA_MAKE(255,0,0,255);
            pVertices[3].color = RGBA_MAKE(255,0,0,255);
            break;

        case 1:
            pVertices[0].color = RGBA_MAKE(0,255,0,255);
            pVertices[1].color = RGBA_MAKE(0,255,0,255);
            pVertices[2].color = RGBA_MAKE(0,255,0,255);
            pVertices[3].color = RGBA_MAKE(0,255,0,255);
            break;
    }

    if( FAILED( pVB->Unlock() ) )
    {
        Log( 1, _T("FillVertexBuffer() pVB->Unlock() failed with %s\n"),  m_pD3D->HResultToString(GetLastError()));
        return false;
    }

    return true;
}

bool CCrossProcShareTest::FillIndexBuffer(RESOURCE_CASE *pResCase, LPINDEXBUFFER pIB, UINT uPattern)
{
    short *pIndices;

    if( FAILED(pIB->Lock(0, 0, (BYTE**)&pIndices, 0)) )
    {
        Log( 1, _T("FillIndexBuffer() pIB->Lock() failed with %s\n"),  m_pD3D->HResultToString(GetLastError()));
        return false;
    }

    switch (uPattern)
    {
        case 0:
            pIndices[0] = 1;
            pIndices[1] = 3;
            pIndices[2] = 2;
            break;

        case 1:
            pIndices[0] = 0;
            pIndices[1] = 1;
            pIndices[2] = 2;
            break;
    }

    if( FAILED(pIB->Unlock()) )
    {
        Log( 1, _T("FillIndexBuffer() pIB->Unlock() failed with %s\n"),  m_pD3D->HResultToString(GetLastError()));
        return false;
    }

    return true;
}

bool CCrossProcShareTest::FillResource(RESOURCE_CASE *pResCase, CObject* pResource, UINT uPattern)
{
    LPSURFACE pSurface = NULL;
    LPSURFACE pOrigRT = NULL;
    LPTEXTURE pTexture = NULL;
    LPTEXTURE pDefaultTex = NULL;
    LPTEXTURE pSysmemTex = NULL;
    LPCUBETEXTURE pCubeTexture = NULL;
    LPVOLUMETEXTURE pVolumeTexture = NULL;
    bool bResult = true;
    HRESULT hr = E_FAIL;

    D3DCOLOR d3dColorsPatter0[] = {RGBA_MAKE(255,0,0,255),RGBA_MAKE(0,255,0,255)};
    D3DCOLOR d3dColorsPatter1[] = {RGBA_MAKE(0,0,255,255),RGBA_MAKE(255,255,0,255)};

    if( NULL == pResource || NULL == pResCase )
    {
        Log(1, _T("FillResource() - Bad Input" ) );
        return false;
    }

    switch( pResCase->dwRType )
    {
        case TYPE_OFFSCREENPLAIN :
        case TYPE_RENDERTARGET :
        case TYPE_DEPTHSTENCIL :
            pSurface = (LPSURFACE)pResource;
            break;

        case TYPE_TEXTURE :
            pTexture = (LPTEXTURE)pResource;

            if( 0 == uPattern )
                hr = pTexture->LoadStripes(1, (DWORD)pResCase->uWidth, (DWORD)pResCase->uHeight, (DWORD)2, d3dColorsPatter0, true, false,0);
            else
                hr = pTexture->LoadStripes(1, (DWORD)pResCase->uWidth, (DWORD)pResCase->uHeight, (DWORD)2, d3dColorsPatter1, true, false,0);

            return SUCCEEDED(hr);

        case TYPE_CUBETEXTURE :
            return FillCubeTexture(pResCase, (LPCUBETEXTURE)pResource, uPattern);

        case TYPE_VOLUMETEXTURE :
            return FillVolumeTexture(pResCase, (LPVOLUMETEXTURE)pResource, false, uPattern);

        case TYPE_VERTEXBUFFER :
            return FillVertexBuffer(pResCase, (LPVERTEXBUFFER)pResource, uPattern);

        case TYPE_INDEXBUFFER :
            return FillIndexBuffer(pResCase, (LPINDEXBUFFER)pResource, uPattern);
	}

    if( pResCase->dwUsage & USAGE_RENDERTARGET )
    {
        if( FAILED(m_pSrcDevice->CreateTexture(pResCase->uWidth, pResCase->uHeight, 1, 0, pResCase->format, POOL_DEFAULT, &pDefaultTex, NULL)) )
        {
            Log( 1, _T("FillResource() - CreateTexture(pDefaultTex) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }

		if( 0 == uPattern )
			hr = pDefaultTex->LoadStripes(1, (DWORD)pResCase->uWidth, (DWORD)pResCase->uHeight, (DWORD)2, d3dColorsPatter0, true, false,0);
		else
			hr = pDefaultTex->LoadStripes(1, (DWORD)pResCase->uWidth, (DWORD)pResCase->uHeight, (DWORD)2, d3dColorsPatter1, true, false,0);

        if( FAILED(hr) )
        {
            Log( 1, _T("FillResource() - LoadStripes() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }

        if( false == DrawTexture(pDefaultTex, pSurface, 0, 0, pResCase->uWidth) )
        {
            Log( 1, _T("FillResource() - DrawTexture failed\n") );
            bResult = false;
            goto Exit;
        }

    }
    else if( TYPE_TEXTURE == pResCase->dwRType && POOL_DEFAULT == pResCase->dwPool && !(pResCase->dwUsage & USAGE_DEPTHSTENCIL) )
    {
        if( FAILED(m_pSrcDevice->UpdateTexture(pSysmemTex, pTexture)) )
        {
            Log( 1, _T("FillResource() UpdateTexture failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }
    }
    else
    {
        if( false == FillSurface(pSurface, false, uPattern) )
        {
            Log( 1, _T("FillResource() FillSurface failed\n"));
            bResult = false;
            goto Exit;
        }
    }

Exit :
    RELEASE(pOrigRT);
    RELEASE(pDefaultTex);
    RELEASE(pSysmemTex);

    if( pTexture || pCubeTexture )
        RELEASE(pSurface);

    return bResult;
}

bool CCrossProcShareTest::VerifyContent(RESOURCE_CASE *pResCase, CObject* pResource, UINT uPattern)
{
    bool bResult = true;                    // Return status
    CObject* pNonSharedResource = NULL;     // Object to compare with
    LPSURFACE pCmpSurf = NULL;              // Compare surface
    LPSURFACE pRTFromSharedSurf = NULL;     // Rendertarget for shared resource to be rendered on
    LPSURFACE pRTFromNonSharedSurf = NULL;  // Rendertarget for non-shared resource to be rendered on
        
    FMT fmtRT = m_EnumResource.GetCompareRTFormat(); // Format for our compare render target
    UINT uWidth = pResCase->uWidth;                  // Width for compare render target
    UINT uHeight = pResCase->uHeight;                // Height for compare render target

    // Create a non shared resource for comparison
    HRESULT hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &pNonSharedResource, NULL);
    if( FAILED(hr) )
    {
        Log( 1, _T("VerifyContent() CreateResource failed : %s\n"), m_pD3D->HResultToString(GetLastError()) );
        bResult = false;
        goto Exit;
    }

    // Fill the non-shared resource
    if( false == FillResource(pResCase, pNonSharedResource, uPattern) )
    {
        Log( 1, _T("VerifyContent() FillResource failed\n") );
        bResult = false;
        RELEASE(pNonSharedResource);
        goto Exit;
    }

    // Settings need for resource type
    switch( pResCase->dwRType )
    {
        case TYPE_OFFSCREENPLAIN :
        case TYPE_DEPTHSTENCIL :
        case TYPE_TEXTURE :
        case TYPE_CUBETEXTURE :
        case TYPE_VOLUMETEXTURE :
            break;

        case TYPE_RENDERTARGET :
            fmtRT = pResCase->format;
            break;

        case TYPE_VERTEXBUFFER :
        case TYPE_INDEXBUFFER :
            uWidth = 800; uHeight = 600;
            break;
    }

    // Create non-multisampled RT for compare
    if( FAILED(m_pSrcDevice->CreateRenderTarget(uWidth, uHeight, fmtRT, MULTISAMPLE_NONE, 0, true, &pRTFromSharedSurf, NULL)) )
    {
        Log( 1, _T("VerifyContent() CreateRenderTarget(pRTFromSharedSurf) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }
    if( FAILED(m_pSrcDevice->CreateRenderTarget(uWidth, uHeight, fmtRT, MULTISAMPLE_NONE, 0, true, &pRTFromNonSharedSurf, NULL)) )
    {
        Log( 1, _T("VerifyContent() CreateRenderTarget(pRTFromNonSharedSurf) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    // The compare
    if( TYPE_TEXTURE == pResCase->dwRType ||
        TYPE_VOLUMETEXTURE == pResCase->dwRType ||
        TYPE_CUBETEXTURE == pResCase->dwRType ||
        TYPE_VERTEXBUFFER == pResCase->dwRType ||
        TYPE_INDEXBUFFER == pResCase->dwRType )
    {
        // Draw resources onto non-multisample render target
        if( false ==  DrawResource(pResCase, pResource, pRTFromSharedSurf) )
        {
            Log( 1, _T("VerifyContent() DrawResource(pRTFromSharedSurf) failed\n"));
            bResult = false;
            goto Exit;
        }
        if( false == DrawResource(pResCase, pNonSharedResource, pRTFromNonSharedSurf) )
        {
            Log( 1, _T("VerifyContent() DrawResource(pNonSharedResource) failed\n"));
            bResult = false;
            goto Exit;
        }
        
        // Compare images
        if( FAILED(m_pFramework->GetImageCmp()->CompareImages(pRTFromSharedSurf, pRTFromNonSharedSurf, pCmpSurf)) )
        {
            Log(1, _T("VerifyContent() - CompareImages failed.\n"));
            bResult = false;
            goto Exit;
        }
    }
    else if( TYPE_RENDERTARGET == pResCase->dwRType )
    {
        // Copy data onto our non-multisample RT
        if( FAILED(m_pSrcDevice->StretchRect((LPSURFACE)pResource, NULL, pRTFromSharedSurf, NULL, TEXF_NONE)) )
        {
            Log( 1, _T("VerifyContent() StretchRect(pRTFromSharedSurf) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }
        if( FAILED(m_pSrcDevice->StretchRect((LPSURFACE)pNonSharedResource, NULL, pRTFromNonSharedSurf, NULL, TEXF_NONE)) )
        {
            Log( 1, _T("VerifyContent() StretchRect(pRTFromNonSharedSurf) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }

        // Compare
        if( FAILED(m_pFramework->GetImageCmp()->CompareImages(pRTFromSharedSurf, pRTFromNonSharedSurf, pCmpSurf)) )
        {
            Log(1, _T("VerifyContent() - CompareImages failed.\n"));
            bResult = false;
            goto Exit;
        }
    }
	else if( TYPE_OFFSCREENPLAIN == pResCase->dwRType )
	{
		RELEASE(pRTFromSharedSurf);
		RELEASE(pRTFromNonSharedSurf);
		// create compare surfaces
		if( FAILED(m_EnumResource.CreateResource(pResCase->dwRType, pResCase, (CObject**)&pRTFromSharedSurf, NULL)) )
		{
			Log( 1, _T("VerifyContent() CreateResource() pRTFromSharedSurf\n") );
			bResult = false;
			goto Exit;
		}
		if( FAILED(m_EnumResource.CreateResource(pResCase->dwRType, pResCase, (CObject**)&pRTFromNonSharedSurf, NULL)) )
		{
			Log( 1, _T("VerifyContent() CreateResource() pNonSharedResource\n") );
			bResult = false;
			goto Exit;
		}

		// Copy data onto our non-multisample RT
        if( FAILED(m_pSrcDevice->StretchRect((LPSURFACE)pResource, NULL, pRTFromSharedSurf, NULL, TEXF_NONE)) )
        {
            Log( 1, _T("VerifyContent() StretchRect(pRTFromSharedSurf) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }
        if( FAILED(m_pSrcDevice->StretchRect((LPSURFACE)pNonSharedResource, NULL, pRTFromNonSharedSurf, NULL, TEXF_NONE)) )
        {
            Log( 1, _T("VerifyContent() StretchRect(pRTFromNonSharedSurf) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }

        // Compare
        if( FAILED(m_pFramework->GetImageCmp()->CompareImages(pRTFromSharedSurf, pRTFromNonSharedSurf, pCmpSurf)) )
        {
            Log(1, _T("VerifyContent() - CompareImages failed.\n"));
            bResult = false;
            goto Exit;
        }
	}
    else
    {
        // Compare surface
        if( FAILED(m_pFramework->GetImageCmp()->CompareImages((LPSURFACE)pResource, (LPSURFACE)pNonSharedResource, pCmpSurf)) )
        {
            Log(1, _T("VerifyContent() - CompareImages failed.\n"));
            bResult = false;
            goto Exit;
        }
    }

    // Check the results
    m_fPassPercentage = m_pFramework->GetImageCmp()->GetCompareResult();
    if( 0.90f > m_fPassPercentage )
    {
        Log(1, _T("VerifyContent() - Resources are different : Pass %% = %f\n"), m_fPassPercentage);
        bResult = false;
        goto Exit;
    }

Exit:
    RELEASE(pRTFromSharedSurf);
    RELEASE(pRTFromNonSharedSurf);
    RELEASE(pCmpSurf);
    RELEASE(pNonSharedResource);

    return bResult;
}

bool CCrossProcShareTest::UseSurface(RESOURCE_CASE *pResCase, LPSURFACE pSurf, bool bReadOnlyResource)
{
    LPTEXTURE pTempTex = NULL;
    LPSURFACE pSysmemSurf = NULL;
    LPSURFACE pDefaultSurf = NULL;
    HRESULT hr;
    bool bResult = true;

	//Create Sysmem surface for UpdateSurface and GetRenderTargetData
	if (pResCase->bCanUpdateSurface || (pResCase->bCanRender && pResCase->multiSample == 0))
	{
		if (FAILED(m_pSrcDevice->CreateTexture(pResCase->uWidth, pResCase->uHeight, 1, 0, pResCase->format, POOL_SYSTEMMEM, &pTempTex)))
		{
			Log(1, _T("UseSurface() : CreateTexture(SYSTEMMEM) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
			bResult = false;
		}
		if (pTempTex && FAILED(pTempTex->GetSurfaceLevel(0, &pSysmemSurf)))
		{
			Log(1, _T("UseSurface() : GetSurfaceLevel(SYSTEMMEM) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
			bResult = false;
		}
	}

	//UpdateSurface
	if (pResCase->bCanUpdateSurface && pSysmemSurf)
	{
		try
		{
			if( FAILED(m_pSrcDevice->UpdateSurface(pSysmemSurf, NULL, pSurf, NULL)) )
			{
				Log(1, _T("UseSurface() : UpdateSurface failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
				bResult = false;
			}
		}
		catch (...)
		{
			Log(1, _T("EXCEPTION occured in UpdateSurface()\n"));
			bResult = false;
		}
	}

	//StretchRect
	if( pResCase->bCanStretchRect )
	{
		if( pResCase->dwRType == TYPE_OFFSCREENPLAIN )
		{
			if (FAILED(m_pSrcDevice->CreateOffscreenPlainSurface(pResCase->uWidth, pResCase->uHeight, pResCase->format, POOL_DEFAULT, &pDefaultSurf)))
			{
				Log(1, _T("UseSurface() : CreateOffscreenPlainSurface(DEFAULT) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
				bResult = false;
			}
		}
		else if( pResCase->dwRType == TYPE_DEPTHSTENCIL )
		{
			if( FAILED(m_pSrcDevice->CreateDepthStencilSurface(pResCase->uWidth, pResCase->uHeight, pResCase->format, MULTISAMPLE_NONE, 0, false, &pDefaultSurf)))
			{
				Log(1, _T("UseSurface() : CreateRenderTarget() failed with %s\n"), m_pD3D->HResultToString(GetLastError()) );
				bResult = false;
			}
		}
		else
		{
			if( FAILED(m_pSrcDevice->CreateRenderTarget(pResCase->uWidth, pResCase->uHeight, pResCase->format, MULTISAMPLE_NONE, 0, true, &pDefaultSurf)))
			{
				Log(1, _T("UseSurface() : CreateRenderTarget() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
				bResult = false;
			}
		}

		try
		{
			hr = m_pSrcDevice->StretchRect(pDefaultSurf, NULL, pSurf, NULL, TEXF_NONE);
			if( false == bReadOnlyResource && FAILED(hr) )
			{
				Log(1, _T("UseSurface() : StretchRect(non-shared -> shared) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
				bResult = false;
			}
			else if( true == bReadOnlyResource && SUCCEEDED(hr) )
			{
				Log(1, _T("UseSurface() : StretchRect(non-shared -> shared(READ-ONLY)) succeeded on a read-only surface\n") );
				bResult = false;
			}
		}
		catch (...)
		{
			Log(1, _T("EXCEPTION occured in StretchRect(Non-Shared -> shared)\n"));
			bResult = false;
		}

		try
		{
			if (FAILED(m_pSrcDevice->StretchRect(pSurf, NULL, pDefaultSurf, NULL, TEXF_NONE)))
			{
				Log(1, _T("UseSurface() : StretchRect(shared -> non-shared) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
				bResult = false;
			}
		}
		catch (...)
		{
			Log(1, _T("EXCEPTION occured in StretchRect(Shared -> non-shared)\n"));
			bResult = false;
		}
		RELEASE(pDefaultSurf);
	}

	//ColorFill
	if( pResCase->bCanColorFill )
	{
		try
		{
			hr = m_pSrcDevice->ColorFill(pSurf, NULL, (D3DCOLOR)0x80ff238a);
			if( false == bReadOnlyResource && FAILED(hr) )
			{
				Log(1, _T("UseSurface() : ColorFill failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
				bResult = false;
			}
			else if( true == bReadOnlyResource && SUCCEEDED(hr) )
			{
				Log(1, _T("UseSurface() : ColorFill succeeded on a read-only surface\n") );
				bResult = false;
			}
		}
		catch (...)
		{
			Log(1, _T("EXCEPTION occured in ColorFill()\n"));
			bResult = false;
		}
	}

	//SetRenderTarget -> render on it
	if (pResCase->bCanRender)
	{
		try
		{
			bool bDrawSucceeded = DrawTexture(NULL, pSurf, 0, 0, pResCase->uWidth);
			if( false == bReadOnlyResource && false == bDrawSucceeded )
			{
				Log(1, _T("UseSurface() : DrawTexture failed\n") );
				bResult = false;
			}
			else if( true == bReadOnlyResource && true == bDrawSucceeded )
			{
				Log(1, _T("UseSurface() : DrawTexture succeeded on a read-only resource\n") );
				bResult = false;
			}
		}
		catch (...)
		{
			Log(1, _T("EXCEPTION occured when drawing texture to Shared RT()\n"));
			bResult = false;
		}
	}

	//GetRenderTargetData
	if( pResCase->bCanRender && pResCase->multiSample == 0 && pSysmemSurf )
	{
		try
		{
			if( FAILED(m_pSrcDevice->GetRenderTargetData(pSurf, pSysmemSurf)) )
			{
				Log(1, _T("UseSurface() : GetRenderTargetData failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
				bResult = false;
			}
		}
		catch (...)
		{
			Log(1, _T("EXCEPTION occured in GetRenderTargetData()\n"));
			bResult = false;
		}
	}

	RELEASE(pTempTex);
	RELEASE(pSysmemSurf);
	RELEASE(pDefaultSurf);

	return bResult;
}

bool CCrossProcShareTest::UseTexture(RESOURCE_CASE *pResCase, LPTEXTURE pTexture, bool bReadOnlyResource)
{
	LPSURFACE pSurf = NULL;
	LPTEXTURE pSysmemTex = NULL;
	LOCKED_RECT lockedRect;
	SURFACEDESC desc;
	HRESULT hr;
	
	bool bResult = true;

	// UpdateTexture
	if( !(pResCase->dwUsage & USAGE_DEPTHSTENCIL) )
	{
		if( FAILED(m_pSrcDevice->CreateTexture(pResCase->uWidth, pResCase->uHeight, 1, pResCase->dwUsage & ~(USAGE_AUTOGENMIPMAP|USAGE_RENDERTARGET), pResCase->format, POOL_SYSTEMMEM, &pSysmemTex)) )
		{
			Log(1, _T("UseTexture() - CreateTexture(SYSTEMMEM) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
			bResult = false;
		}
		else
		{
			try
			{
				if( FAILED(m_pSrcDevice->UpdateTexture(pSysmemTex, pTexture)) )
				{
					Log(1, _T("UseTexture() : UpdateTexture(pSysmemTex, pTexture) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
					bResult = false;
				}
			}
			catch (...)
			{
				Log(1, _T("EXCEPTION occured in UpdateTexture(pSysmemTex, pTexture)\n"));
				bResult = false;
			}
		}
	}

	try
	{
		// SetTexture -> render with it
		if( false == DrawTexture(pTexture, NULL, 0, 0, pResCase->uWidth) )
		{
			Log(1, _T("UseTexture() : DrawTexture failed\n") );
			bResult = false;
		}
	}
	catch (...)
	{
		Log(1, _T("EXCEPTION occured in DrawTexture(pTexture)\n"));
		bResult = false;
	}

	// Do the surface tests with the miplevel
	if( FAILED(pTexture->GetSurfaceLevel(0, &pSurf)) )
	{
		Log(1, _T("UseTexture() : GetSurfaceLevel failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
		bResult = false;
	}
	if( false == UseSurface(pResCase, pSurf, bReadOnlyResource) )
	{
		Log(1, _T("UseTexture() : UseSurface failed\n") );
		bResult = false;
	}

	RELEASE(pSysmemTex);
	RELEASE(pSurf);

	return bResult;
}

bool CCrossProcShareTest::UseVolumeTexture(RESOURCE_CASE *pResCase, LPVOLUMETEXTURE pVolTexture, bool bReadOnlyResource)
{
	LPSURFACE pSurf = NULL;
	LPVOLUMETEXTURE pSysmemTex = NULL;
	LOCKED_RECT lockedRect;
	SURFACEDESC desc;
	HRESULT hr;
	
	bool bResult = true;

    try
    {
        for( int iSlice = 0; iSlice < pResCase->uDepth; iSlice++ )
        {
	        if( false == DrawVolumeTextureSlice(pVolTexture, NULL, iSlice, pResCase->uDepth, 0, 0, pResCase->uWidth) )
            {
                Log(1, _T("UseVolumeTexture() - DrawVolumeTextureSlice failed\n") );
			    bResult = false;
                break;
            }
        }
    }
    catch(...)
    {
        Log(1, _T("UseVolumeTexture() - EXCEPTION occured in DrawVolumeTextureSlice(pVolTexture)") );
        bResult = false;
    }

	return bResult;
}

bool CCrossProcShareTest::UseCubeTexture(RESOURCE_CASE *pResCase, LPCUBETEXTURE pCubeTexture, bool bReadOnlyResource)
{
	LPSURFACE pSurf = NULL;
	LPCUBETEXTURE pSysmemTex = NULL;
	LOCKED_RECT lockedRect;
	SURFACEDESC desc;
	HRESULT hr;
	
	bool bResult = true;

    try
    {
        for( int iSlice = 0; iSlice < pResCase->uDepth; iSlice++ )
        {
	        if( false == DrawCubeTexture(pCubeTexture, NULL, 0, 0, pResCase->uWidth) )
            {
                Log(1, _T("UseVolumeTexture() - DrawCubeTexture failed\n") );
			    bResult = false;
                break;
            }
        }
    }
    catch(...)
    {
        Log(1, _T("UseCubeTexture() - EXCEPTION occured in DrawVolumeTextureSlice(pVolTexture)") );
        bResult = false;
    }

	return bResult;
}

bool CCrossProcShareTest::UseResource(DWORD dwRTYPE, RESOURCE_CASE *pResCase, CObject *pResource, bool bReadOnlyResource)
{
    try
    {
        switch (dwRTYPE)
        {
            case TYPE_OFFSCREENPLAIN :
            case TYPE_RENDERTARGET :
            case TYPE_DEPTHSTENCIL :
                return UseSurface(pResCase, (LPSURFACE)pResource, bReadOnlyResource);

            case TYPE_TEXTURE :
                return UseTexture(pResCase, (LPTEXTURE)pResource, bReadOnlyResource);

            case TYPE_CUBETEXTURE :
                return UseCubeTexture(pResCase, (LPCUBETEXTURE)pResource, bReadOnlyResource);

            case TYPE_VOLUMETEXTURE :
                return UseVolumeTexture(pResCase, (LPVOLUMETEXTURE)pResource, bReadOnlyResource);

            case TYPE_VERTEXBUFFER :
            case TYPE_INDEXBUFFER :
                return DrawResource(pResCase, pResource);

            default :
                Log( 1, _T("UseResource() - resource type is unknown") );
                return false;
        }
    }
    catch (...)
    {
        Log(0, _T("Exception caught in UseResource()") );
    }

    return false;
}

bool CCrossProcShareTest::DrawResource(RESOURCE_CASE *pResCase, CObject* pResource, LPSURFACE pRT)
{
    bool bRT = false;
    ClearFrame();
    m_pSrcDevice->Present(NULL, NULL, NULL, NULL);			

    try
    {
        switch (pResCase->dwRType)
        {
            case TYPE_DEPTHSTENCIL :			
				return false;
				break;

			case TYPE_OFFSCREENPLAIN :
            case TYPE_RENDERTARGET :
				bRT = DrawSurface((LPSURFACE)pResource, pRT, 0, 0, pResCase->uWidth);
				break;

            case TYPE_TEXTURE :
                bRT = DrawTexture((LPTEXTURE)pResource, pRT, 0, 0, pResCase->uWidth);
                break;

            case TYPE_CUBETEXTURE :
                bRT = DrawCubeTexture((LPCUBETEXTURE)pResource, pRT, 0, 0, pResCase->uWidth);
                break;

            case TYPE_VOLUMETEXTURE :
                bRT = DrawVolumeTextureSlice((LPVOLUMETEXTURE)pResource, pRT, 0, 1, 0, 0, pResCase->uWidth);
                break;

            case TYPE_VERTEXBUFFER :
                bRT = DrawVertexBuffer((LPVERTEXBUFFER)pResource, pRT);
                break;

            case TYPE_INDEXBUFFER :
                bRT = DrawIndexBuffer((LPINDEXBUFFER)pResource, pRT);
                break;

            default :
                Log( 1, _T("DrawResource() - resource type is unknown") );
                return false;
        }
    }
    catch (...)
    {
        Log(0, _T("Exception caught in DrawResource()") );
    }

    if( true == bRT )
        m_pSrcDevice->Present(NULL, NULL, NULL, NULL);

    return bRT;
}

bool CCrossProcShareTest::DrawSurface(LPSURFACE pSurface, LPSURFACE pRT, float x, float y, UINT size)
{
    LPSURFACE pOrigRT = NULL;
    bool bResult = true;

	if( NULL == pSurface )
		return false;

	if( NULL != pRT )
	{
		if( FAILED(m_pSrcDevice->StretchRect(pSurface, NULL, pRT, NULL, TEXF_NONE)) )
        {
            Log( 1, _T("DrawSurface() StretchRect(pRT) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            return false;
        }
	}
	else
	{
		if( FAILED(m_pSrcDevice->GetRenderTarget(0, &pOrigRT)) )
		{
			 Log( 1, _T("DrawSurface() GetRenderTarget failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
			 return false;
		}

		if( FAILED(m_pSrcDevice->StretchRect(pSurface, NULL, pOrigRT, NULL, TEXF_NONE)) )
        {
            Log( 1, _T("DrawSurface() StretchRect(pOrigRT) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
			RELEASE(pOrigRT)
            return false;
        }

		if( FAILED(m_pSrcDevice->SetRenderTarget((DWORD)0, pOrigRT)) )
        {
            Log( 1, _T("DrawSurface() SetRenderTarget(pOrigRT) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
			RELEASE(pOrigRT)
            return false;
        }

		RELEASE(pOrigRT)
	}

    return bResult;
}

bool CCrossProcShareTest::DrawTexture(LPTEXTURE pTexture, LPSURFACE pRT, float x, float y, UINT size)
{
    MYDISPLAYVERTEX v[4];
    LPSURFACE pOrigRT = NULL;
    bool bResult = true;

    float left = x;
    float right = x + size;
    float top = y;
    float bottom = y + size;

    float minUV = 0.5f / size;
    float maxUV = (size + 0.5f) / size;

    v[0].x=left;   v[0].y =bottom;  v[0].z=0;  v[0].rhw=1.0; v[0].tu=minUV; v[0].tv=maxUV; v[0].tw=0.0;  
    v[1].x=left;   v[1].y =top;     v[1].z=0;  v[1].rhw=1.0; v[1].tu=minUV; v[1].tv=minUV; v[1].tw=0.0;  
    v[2].x=right;  v[2].y =top;     v[2].z=0;  v[2].rhw=1.0; v[2].tu=maxUV; v[2].tv=minUV; v[2].tw=0.0;  
    v[3].x=right;  v[3].y =bottom;  v[3].z=0;  v[3].rhw=1.0; v[3].tu=maxUV; v[3].tv=maxUV; v[3].tw=0.0;  

    if ( FAILED( m_pSrcDevice->SetTexture(0, pTexture) ) )
    {
        Log( 1, _T("DrawTexture() SetTexture failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if (pRT && FAILED( m_pSrcDevice->GetRenderTarget(0, &pOrigRT) ) )
    {
        Log( 1, _T("DrawTexture() GetRenderTarget failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if (pRT && FAILED( m_pSrcDevice->SetRenderTarget((DWORD)0, pRT) ) )
    {
        Log( 1, _T("DrawTexture() SetRenderTarget failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if ( FAILED( m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1) ) )
    {
        Log( 1, _T("DrawTexture() SetTextureStageState(COLOROP) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if ( FAILED( m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE) ) )
    {
        Log( 1, _T("DrawTexture() SetTextureStageState(COLORARG1) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if ( FAILED( m_pSrcDevice->BeginScene() ) )
    {
        Log( 1, _T("DrawTexture() BeginScene failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if ( FAILED( m_pSrcDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0) ) ) )
    {
        Log( 1, _T("DrawTexture() SetFVF() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if ( FAILED( m_pSrcDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, v, sizeof(MYDISPLAYVERTEX)) ) )
    {
        Log( 1, _T("DrawTexture() DrawPrimitiveUP() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if ( FAILED( m_pSrcDevice->EndScene() ) )
    {
        Log( 1, _T("DrawTexture() EndScene() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if (pRT && FAILED( m_pSrcDevice->SetRenderTarget((DWORD)0, pOrigRT) ) )
    {
        Log( 1, _T("DrawTexture() SetRenderTarget(pOrigRT) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

Exit:
    if (!bResult)
        m_pSrcDevice->EndScene();

    RELEASE(pOrigRT);

    return bResult;
}

bool CCrossProcShareTest::DrawVolumeTextureSlice(LPVOLUMETEXTURE pVolumeTexture, LPSURFACE pRT, int iSlice, UINT depth, float x, float y, UINT size)
{
    MYDISPLAYVERTEX v[4];
    LPSURFACE pOrigRT = NULL;
    bool bResult = true;

    float left = x;
    float right = x + size;
    float top = y;
    float bottom = y + size;

    float minUV = 0.5f / size;
    float maxUV = (size + 0.5f) / size;
    float w = (float)iSlice / (float)(depth - 1);

    v[0].x=left;   v[0].y =bottom;  v[0].z=0;  v[0].rhw=1.0; v[0].tu=minUV; v[0].tv=maxUV; v[0].tw=w;  
    v[1].x=left;   v[1].y =top;     v[1].z=0;  v[1].rhw=1.0; v[1].tu=minUV; v[1].tv=minUV; v[1].tw=w;  
    v[2].x=right;  v[2].y =top;     v[2].z=0;  v[2].rhw=1.0; v[2].tu=maxUV; v[2].tv=minUV; v[2].tw=w;  
    v[3].x=right;  v[3].y =bottom;  v[3].z=0;  v[3].rhw=1.0; v[3].tu=maxUV; v[3].tv=maxUV; v[3].tw=w;  

    if ( FAILED( m_pSrcDevice->SetTexture(0, pVolumeTexture) ) )
    {
        Log( 1, _T("DrawVolumeTextureSlice() SetTexture failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( NULL != pRT )
    {
        if( FAILED(m_pSrcDevice->GetRenderTarget(0, &pOrigRT)) )
        {
            Log( 1, _T("DrawVolumeTextureSlice() GetRenderTarget failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }

        if( FAILED(m_pSrcDevice->SetRenderTarget((DWORD)0, pRT)) )
        {
            Log( 1, _T("DrawVolumeTextureSlice() SetRenderTarget failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }
    }

    if( FAILED( m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1) ) )
    {
        Log( 1, _T("DrawVolumeTextureSlice() SetTextureStageState(COLOROP) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE) ) )
    {
        Log( 1, _T("DrawTexture() SetTextureStageState(COLORARG1) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->BeginScene() ) )
    {
        Log( 1, _T("DrawVolumeTextureSlice() BeginScene failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0) ) ) )
    {
        Log( 1, _T("DrawVolumeTextureSlice() SetFVF() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    try
    {
        if( FAILED( m_pSrcDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, v, sizeof(MYDISPLAYVERTEX)) ) )
        {
            Log( 1, _T("DrawVolumeTextureSlice() DrawPrimitiveUP() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }
    }
    catch(...)
    {
        Log( 1, _T("Exception Caught in DrawVolumeTextureSlice() when calling DrawPrimitiveUP()\n") );
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->EndScene() ) )
    {
        Log( 1, _T("DrawTexture() EndScene() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( NULL != pRT )
    {
        if( FAILED( m_pSrcDevice->SetRenderTarget((DWORD)0, pOrigRT) ) )
        {
            Log( 1, _T("DrawTexture() SetRenderTarget(pOrigRT) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }
    }

Exit:
    if( false == bResult )
        m_pSrcDevice->EndScene();

    RELEASE(pOrigRT);

    return bResult;
}

bool CCrossProcShareTest::DrawCubeTexture(LPCUBETEXTURE pCubeTexture, LPSURFACE pRT, float x, float y, UINT size)
{
    MYDISPLAYVERTEX v[4];
    LPSURFACE pOrigRT = NULL;
    bool bResult = true;

    float left = x;
    float right = x + size;
    float top = y;
    float bottom = y + size;

    float minUV = 0.5f / size;
    float maxUV = (size + 0.5f) / size;

    v[0].x=left;   v[0].y =bottom;  v[0].z=0;  v[0].rhw=1.0; v[0].tu=minUV; v[0].tv=maxUV; v[0].tw=0;  
    v[1].x=left;   v[1].y =top;     v[1].z=0;  v[1].rhw=1.0; v[1].tu=minUV; v[1].tv=minUV; v[1].tw=0;  
    v[2].x=right;  v[2].y =top;     v[2].z=0;  v[2].rhw=1.0; v[2].tu=maxUV; v[2].tv=minUV; v[2].tw=0;  
    v[3].x=right;  v[3].y =bottom;  v[3].z=0;  v[3].rhw=1.0; v[3].tu=maxUV; v[3].tv=maxUV; v[3].tw=0;  

    if( FAILED( m_pSrcDevice->SetTexture(0, pCubeTexture) ) )
    {
        Log( 1, _T("DrawCubeTexture() SetTexture failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( NULL != pRT )
    {
        if( FAILED(m_pSrcDevice->GetRenderTarget(0, &pOrigRT)) )
        {
            Log( 1, _T("DrawCubeTexture() GetRenderTarget failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }

        if( FAILED(m_pSrcDevice->SetRenderTarget((DWORD)0, pRT)) )
        {
            Log( 1, _T("DrawCubeTexture() SetRenderTarget failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }
    }

    if( FAILED( m_pSrcDevice->BeginScene() ) )
    {
        Log( 1, _T("DrawTexture() BeginScene failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0) ) ) )
    {
        Log( 1, _T("DrawCubeTexture() SetFVF() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, v, sizeof(MYDISPLAYVERTEX)) ) )
    {
        Log( 1, _T("DrawCubeTexture() DrawPrimitiveUP() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->EndScene() ) )
    {
        Log( 1, _T("DrawCubeTexture() EndScene() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( NULL != pRT )
    {
        if( FAILED( m_pSrcDevice->SetRenderTarget((DWORD)0, pOrigRT) ) )
        {
            Log( 1, _T("DrawCubeTexture() SetRenderTarget(pOrigRT) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }
    }

Exit:
    if( false == bResult )
        m_pSrcDevice->EndScene();

    RELEASE(pOrigRT);

    return bResult;
}

bool CCrossProcShareTest::DrawVertexBuffer(LPVERTEXBUFFER pVB, LPSURFACE pRT)
{
    bool bResult = true;
    LPSURFACE pOrigRT = NULL;

    // Switch render target
    if( NULL != pRT )
    {
        if( FAILED(m_pSrcDevice->GetRenderTarget(0, &pOrigRT)) )
        {
            Log( 1, _T("DrawVertexBuffer() GetRenderTarget failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }

        if( FAILED(m_pSrcDevice->SetRenderTarget((DWORD)0, pRT)) )
        {
            Log( 1, _T("DrawVertexBuffer() SetRenderTarget failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }
    }

    // Render
    ClearFrame();
    if( FAILED( m_pSrcDevice->SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE) ) )
    {
        Log( 1, _T("DrawVertexBuffer() SetRenderState failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->SetTexture(0, NULL) ) )
    {
        Log( 1, _T("DrawVertexBuffer() SetTexture failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->SetStreamSource(0, pVB, sizeof(XYZRHWDIFFUSEVERTEX), 0 ) ) )
    {
        Log( 1, _T("DrawVertexBuffer() SetStreamSource failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_DIFFUSE ) ) )
    {
        Log( 1, _T("DrawVertexBuffer() SetFVF() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->BeginScene() ) )
	{
		Log( 1, _T("DrawVertexBuffer() BeginScene failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
		bResult = false;
		goto Exit;
	}

    if( FAILED( m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 ) ) )
    {
        Log( 1, _T("DrawVertexBuffer() DrawPrimitive() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->EndScene() ) )
    {
        Log( 1, _T("DrawVertexBuffer() EndScene() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

Exit:
    if( false == bResult )
        m_pSrcDevice->EndScene();

    // Go back to the orig renter target
    if( NULL != pRT )
    {
        if( FAILED( m_pSrcDevice->SetRenderTarget((DWORD)0, pOrigRT) ) )
        {
            Log( 1, _T("DrawVertexBuffer() SetRenderTarget(pOrigRT) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
        }
    }

    RELEASE(pOrigRT);

    return bResult;
}

bool CCrossProcShareTest::DrawIndexBuffer(LPINDEXBUFFER pIB, LPSURFACE pRT)
{
    bool bResult = true;
    LPVERTEXBUFFER pVB = NULL;
    LPSURFACE pOrigRT = NULL;

    // Make a vertex buffer to use.
    if( FAILED( m_pSrcDevice->CreateVertexBuffer(4 * sizeof(XYZRHWDIFFUSEVERTEX), USAGE_WRITEONLY, D3DFVF_XYZRHW|D3DFVF_DIFFUSE, POOL_DEFAULT, &pVB, NULL) ) )
    {
        Log( 1, _T("DrawIndexBuffer() CreateVertexBuffer failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( false == FillVertexBuffer(NULL, pVB, 0) )
    {
        Log( 1, _T("DrawIndexBuffer() FillVertexBuffer failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    // Switch render targets
    if( NULL != pRT )
    {
        if( FAILED(m_pSrcDevice->GetRenderTarget(0, &pOrigRT)) )
        {
            Log( 1, _T("DrawIndexBuffer() GetRenderTarget failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }

        if( FAILED(m_pSrcDevice->SetRenderTarget((DWORD)0, pRT)) )
        {
            Log( 1, _T("DrawIndexBuffer() SetRenderTarget failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
            goto Exit;
        }
    }

    // Render
    if( FAILED( m_pSrcDevice->SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE) ) )
    {
        Log( 1, _T("DrawIndexBuffer() SetRenderState failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->SetTexture(0, NULL) ) )
    {
        Log( 1, _T("DrawIndexBuffer() SetTexture failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->SetStreamSource(0, pVB, sizeof(XYZRHWDIFFUSEVERTEX), 0 ) ) )
    {
        Log( 1, _T("DrawIndexBuffer() SetStreamSource failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_DIFFUSE ) ) )
    {
        Log( 1, _T("DrawIndexBuffer() SetFVF() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->SetIndices(pIB, 0) ) )
    {
        Log( 1, _T("DrawIndexedBuffer() SetIndices failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->BeginScene() ) )
    {
        Log( 1, _T("DrawIndexBuffer() BeginScene failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 4, 0, 1, 0 ) ) )
    {
        Log( 1, _T("DrawIndexedBuffer() DrawIndexedPrimitive() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

    if( FAILED( m_pSrcDevice->EndScene() ) )
    {
        Log( 1, _T("DrawIndexBuffer() EndScene() failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
        bResult = false;
        goto Exit;
    }

Exit:
    if( false == bResult )
        m_pSrcDevice->EndScene();

    // Switch render targets back
    if( NULL != pRT )
    {
        if( FAILED( m_pSrcDevice->SetRenderTarget((DWORD)0, pOrigRT) ) )
        {
            Log( 1, _T("DrawIndexBuffer() SetRenderTarget(pOrigRT) failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            bResult = false;
        }
    }

    RELEASE(pVB);
    RELEASE(pOrigRT);

    return bResult;
}

bool CCrossProcShareTest::LockResource(DWORD dwRTYPE, CObject *pResource)
{
    HRESULT hr;
    LOCKED_RECT lockedRect;
    LOCKED_BOX lockedBox;
    LPVOID pData;
    switch (dwRTYPE)
    {
        case TYPE_OFFSCREENPLAIN :
        case TYPE_RENDERTARGET :
        case TYPE_DEPTHSTENCIL :
            hr = ((LPSURFACE)pResource)->LockRect(&lockedRect, NULL, 0);
            break;

        case TYPE_TEXTURE :
            hr = ((LPTEXTURE)pResource)->LockRect(0, &lockedRect, NULL, 0);
            break;

        case TYPE_CUBETEXTURE :
            hr = ((LPCUBETEXTURE)pResource)->LockRect((CUBEMAP_FACES)0, 0, &lockedRect, NULL, 0);
            break;

        case TYPE_VOLUMETEXTURE :
            hr = ((LPVOLUMETEXTURE)pResource)->LockBox(0, &lockedBox, NULL, 0);
            break;

        case TYPE_VERTEXBUFFER :
            hr = ((LPVERTEXBUFFER)pResource)->Lock(0, 0, &pData, 0);
            break;

        case TYPE_INDEXBUFFER :
            hr = ((LPINDEXBUFFER)pResource)->Lock(0, 0, &pData, 0);
            break;
    }

    return SUCCEEDED(hr);
}

bool CCrossProcShareTest::UnLockResource(DWORD dwRTYPE, CObject *pResource)
{
    HRESULT hr;
    switch( dwRTYPE )
    {
        case TYPE_OFFSCREENPLAIN :
        case TYPE_RENDERTARGET :
        case TYPE_DEPTHSTENCIL :
            hr = ((LPSURFACE)pResource)->UnlockRect();
            break;

        case TYPE_TEXTURE :
            hr = ((LPTEXTURE)pResource)->UnlockRect(0);
            break;

        case TYPE_CUBETEXTURE :
            hr = ((LPCUBETEXTURE)pResource)->UnlockRect((CUBEMAP_FACES)0, 0);
            break;

        case TYPE_VOLUMETEXTURE :
            hr = ((LPVOLUMETEXTURE)pResource)->UnlockBox(0);
            break;

        case TYPE_VERTEXBUFFER :
            hr = ((LPVERTEXBUFFER)pResource)->Unlock();
            break;

        case TYPE_INDEXBUFFER :
            hr = ((LPINDEXBUFFER)pResource)->Unlock();
            break;
    }

    return SUCCEEDED(hr);
}

UINT CCrossProcShareTest::FillResource_FromSharedMem(UINT uPattern)
{
    if( false == FillResource(&m_pSharedMemory->resCase, m_pResource, uPattern) )
    {
        Log( 1, _T("FillResource_FromSharedMem() - FillResource(Pattern %d) failed.\n"), uPattern );
        return TEST_FAIL;
    }
    if( false == VerifyContent(&m_pSharedMemory->resCase, m_pResource, uPattern) )
    {
        Log( 1, _T("FillResource_FromSharedMem() - VerifyContent(Pattern %d) failed after FillResource(Pattern %d) passed.\n"), uPattern, uPattern );
        return TEST_FAIL;
    }

    return TEST_SUCCESS;
}

UINT CCrossProcShareTest::VerifyResource_FromSharedMem(UINT uPattern)
{
    if( false == VerifyContent(&m_pSharedMemory->resCase, m_pResource, uPattern) )
    {
        Log( 1, _T("VerifyResource_FromSharedMem() - VerifyContent(Pattern %d) failed.\n"), uPattern );
        return TEST_FAIL;
    }

    return TEST_SUCCESS;
}

bool CCrossProcShareTest::FillVerifyNonShared_FromSharedMem(UINT uPattern)
{
	// Get case from server
    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;
    bool bReadOnlyResource = m_pSharedMemory->bReadOnlyResource;

	CObject* pNonSharedResource = NULL;
	bool bRT = false;

	// Make sure we can create the resouce without being shared
    if( FAILED(m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &pNonSharedResource,  NULL)) )
    {
        pNonSharedResource = NULL;
        Log( 1, _T("FillVerifyNonShared_FromSharedMem() - Create of a non-shared resource failed.\n") );
        goto cleanup;
    }

	if( false == FillResource(pResCase, pNonSharedResource, uPattern) )
    {
        Log( 1, _T("FillVerifyNonShared_FromSharedMem() - FillResource() failed on non-shared resource.\n") );
        goto cleanup;
    }

	if( false == VerifyContent(pResCase, pNonSharedResource, uPattern) )
    {
        Log( 1, _T("FillVerifyNonShared_FromSharedMem() - VerifyContent() failed on non-shared resource.\n") );
        goto cleanup;
    }

	bRT = true;

cleanup:
    RELEASE(pNonSharedResource);

	return bRT;
}

UINT CCrossProcShareTest::CreateResource_FromSharedMem(bool bSecure)
{
    HRESULT hr;

    // Get case from server
    if (NULL == m_pSharedMemory) 
    {
        Log( 1, _T("CreateResource_FromSharedMem() m_pSharedMemory is NULL"));
        return TEST_FAIL;
    }

    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;
    bool bReadOnlyResource = m_pSharedMemory->bReadOnlyResource;

    // We will need to update the useage but dont want it to affect the case.
    DWORD dwUsage = pResCase->dwUsage; // save usage

    // Make sure the resource are clean
    RELEASE(m_pNonSharedResource);
    RELEASE(m_pResource);
    m_pSharedMemory->hShared = NULL;

    // Make sure we can create the resouce without being shared
    CObject* pNonSharedResource = NULL;
    hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &pNonSharedResource,  NULL);
    if( FAILED(hr) )
    {
        pNonSharedResource = NULL;
        Log( 1, _T("CreateResource_FromSharedMem() - Create of a non-shared resource failed with %s\n"), m_pD3D->HResultToString(hr) );
        return TEST_SKIP;
    }	
    RELEASE(pNonSharedResource);

    // Create Resource
    if( true == bReadOnlyResource )
    {
        // RT and DS can not be read-only
        if( TYPE_RENDERTARGET == pResCase->dwRType  || 
            TYPE_DEPTHSTENCIL == pResCase->dwRType )
            return TEST_SKIP;

        // Create a non shared resource
        pResCase->dwPool = POOL_SYSTEMMEM;
        hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &m_pNonSharedResource,  NULL);
        if( FAILED(hr) )
        {
            Log( 1, _T("CreateResource_FromSharedMem() - Create SysMem resource for read-only case failed with %s\n"), m_pD3D->HResultToString(hr) );
            m_pNonSharedResource = NULL;
            return TEST_SKIP;
        }

        // Fill read-only with pattern 0.
        if( false == FillResource(pResCase, m_pNonSharedResource, 0) )
        {
            Log( 1, _T("CreateResource_FromSharedMem() - FillResource() failed on system memory non-shared resource.\n") );
            RELEASE(m_pNonSharedResource);
            return TEST_SKIP;
        }

        // Update usage for the create of read-only
        if( false == bSecure )
            pResCase->dwUsage = pResCase->dwUsage | D3DUSAGE_NONSECURE;

        // Try to create a read-only resource
        m_pResource = m_pNonSharedResource;
        pResCase->dwPool = POOL_DEFAULT;
        hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &m_pResource, &m_pSharedMemory->hShared);
        if( FAILED(hr) )
        {
            Log( 1, _T("CreateResource_FromSharedMem() - Create default Read-Only resource failed with %s\n"), m_pD3D->HResultToString(hr) );
            RELEASE(m_pNonSharedResource);
            m_pResource = NULL;
            pResCase->dwUsage = dwUsage;
            
            // failing return codes check.
            if( E_FAIL != hr && D3DERR_NOTAVAILABLE != hr && D3DERR_OUTOFVIDEOMEMORY != hr &&
		E_OUTOFMEMORY != hr && D3DERR_INVALIDCALL != hr && E_INVALIDARG != hr)
            {
                Log( 1, _T("CreateResource_FromSharedMem() - CreateResource failed with %s. Failing return code must be E_FAIL, D3DERR_INVALIDCALL, D3DERR_NOTAVAILABLE, or E_OUTOFMEMORY.\n"), m_pD3D->HResultToString(hr) );
                return TEST_FAIL;
            }

            if( false == pResCase->bRequiredShareable )
                return TEST_SKIP;

            return TEST_FAIL;
        }

        // Restore usage
        pResCase->dwUsage = dwUsage;
    }
	else //read-write case
    {
        // Update usage if needed
        if( false == bSecure )
            pResCase->dwUsage = pResCase->dwUsage | D3DUSAGE_NONSECURE;

        // Try to create a read-write resource
        hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &m_pResource, &m_pSharedMemory->hShared);
        if( FAILED(hr) )
        {
            Log( 1, _T("CreateResource_FromSharedMem() - Create Read-Write resource failed with %s\n"), m_pD3D->HResultToString(hr) );
            pResCase->dwUsage = dwUsage;
            m_pResource = NULL;

            if( E_FAIL != hr && D3DERR_NOTAVAILABLE != hr && D3DERR_OUTOFVIDEOMEMORY != hr &&
		E_OUTOFMEMORY != hr && D3DERR_INVALIDCALL != hr && E_INVALIDARG != hr)
            {
                Log( 1, _T("CreateResource_FromSharedMem() - CreateResource failed with %s. Failing return code must be E_FAIL, D3DERR_INVALIDCALL, D3DERR_NOTAVAILABLE, or E_OUTOFMEMORY.\n"), m_pD3D->HResultToString(hr) );
                return TEST_FAIL;
            }

            if( false == pResCase->bRequiredShareable )
                return TEST_SKIP;
            
            return TEST_FAIL;
        }

        // Fill pattern 0
        pResCase->dwUsage = dwUsage;
        if( false == FillResource(pResCase, m_pResource, 0) )
        {
            Log( 1, _T("CreateResource_FromSharedMem() - FillResource() failed on shared resource in proccess that created the resource.\n") );
            RELEASE(m_pResource);
            return TEST_SKIP;
        }
	}

    if( NULL == m_pSharedMemory->hShared )
    {
        Log( 1, _T("CreateResource_FromSharedMem() - *pSharedHandle = NULL after create of shared resource.\n"));
        RELEASE(m_pResource);
        return TEST_FAIL;
    }
    
	// Sync
	if( false == DrawResource(&m_pSharedMemory->resCase, m_pResource) )
	{
		// Draw did not work try lock
		if( true == m_pSharedMemory->resCase.bLockable && false == bReadOnlyResource )
		{
			if( true == LockResource(pResCase->dwRType, m_pResource) )
				UnLockResource(pResCase->dwRType, m_pResource);
		}

	}
	else if( SUCCEEDED(m_pSrcDevice->CreateQuery(QUERYTYPE_EVENT, NULL)) ) // Draw works, try query
	{
		// Force the driver to execute the draw commands from the command buffer
		HRESULT hrQuery = E_FAIL;
		CQuery* pOcclusionQuery = NULL;
		DWORD numberOfPixelsDrawn = 0;

		hrQuery = m_pSrcDevice->CreateQuery(QUERYTYPE_OCCLUSION, &pOcclusionQuery);
		if( SUCCEEDED(hrQuery) &&  NULL != pOcclusionQuery )
		{
			pOcclusionQuery->Issue(ISSUE_BEGIN);
				DrawResource(&m_pSharedMemory->resCase, m_pResource);
			pOcclusionQuery->Issue(ISSUE_END);

			for( UINT i = 0; i < 100; i++ )
			{
				if(S_FALSE != pOcclusionQuery->GetData(&numberOfPixelsDrawn, sizeof(DWORD), GETDATA_FLUSH) )
					break;
				else
					Sleep(100);
			}
			RELEASE(pOcclusionQuery)
		}
	}
	else
	{
		Log( 1, _T("CreateResource_FromSharedMem() - unable to sync.\n"));
	}

    return TEST_SUCCESS;
}

UINT CCrossProcShareTest::OpenResource_Test()
{
    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;
    HANDLE hShared = m_pSharedMemory->hShared;
    CObject* pNonSharedResource = NULL;

    if( NULL == hShared )
    {
        Log( 1, _T("OpenResource_Test() - hShared == NULL\n") );
        return TEST_FAIL;
    }

    HRESULT hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &pNonSharedResource, &hShared);
    if( FAILED(hr) )
    {
        pNonSharedResource = NULL;
        Log( 1, _T("OpenResource_Test() - Open of shared resource failed with %s\n"), m_pD3D->HResultToString(hr) );
        return TEST_FAIL;
    }
    RELEASE(pNonSharedResource);

    return TEST_SUCCESS;
}

UINT CCrossProcShareTest::OpenResource_FromSharedMem()
{
    HRESULT hr = E_FAIL;

    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;
    bool bReadOnlyResource = m_pSharedMemory->bReadOnlyResource;
    HANDLE hSharedHandle = m_pSharedMemory->hShared;
    RELEASE(m_pResource);


    hr = m_EnumResource.CreateResource(pResCase->dwRType, pResCase, &m_pResource, &hSharedHandle );
    if( FAILED(hr) )
    {
        m_pResource = NULL;
        Log( 1, _T("OpenResource_FromSharedMem() - CreateResource() Failed with %s\n"), m_pD3D->HResultToString(hr) );
        return TEST_FAIL;
    }

	DrawResource(&m_pSharedMemory->resCase, m_pResource);

    return TEST_SUCCESS;
}

UINT CCrossProcShareTest::LockResource_FromSharedMem()
{
    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;

    if( false == LockResource(pResCase->dwRType, m_pResource) )
	{
		return TEST_FAIL;
	}

    return TEST_SUCCESS;
}

UINT CCrossProcShareTest::UnLockResource_FromSharedMem()
{
    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;

    if( false == UnLockResource(pResCase->dwRType, m_pResource) )
	{
		return TEST_FAIL;
	}

    return TEST_SUCCESS;
}

UINT CCrossProcShareTest::UseResource_FromSharedMem()
{
    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;

    if( false == UseResource(pResCase->dwRType, pResCase, m_pResource, true) )
    {
        return TEST_FAIL;
    }

    return TEST_SUCCESS;
}


BOOL CCrossProcShareTest::ResetBind()
{
    BOOL bRT = FALSE;
    LPVERTEXBUFFER pVB = NULL;
    LPINDEXBUFFER pIB = NULL;
    RESOURCE_CASE *pResCase = &m_pSharedMemory->resCase;

    ClearFrame();
    m_pSrcDevice->Present(NULL, NULL, NULL, NULL);	

    // Reset texture
    ClearFrame();
    m_pSrcDevice->SetTexture(0, NULL);

    if( pResCase->dwRType == TYPE_VERTEXBUFFER )
    {
        // Create vertex buffer
        if( FAILED( m_pSrcDevice->CreateVertexBuffer(4 * sizeof(XYZRHWDIFFUSEVERTEX), USAGE_WRITEONLY, D3DFVF_XYZRHW|D3DFVF_DIFFUSE, POOL_DEFAULT, &pVB, NULL) ) )
        {
            Log( 1, _T("ResetBind(): CreateVertexBuffer failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            goto cleanup;
        }

        // fill vertex buffer
        if( false == FillVertexBuffer(NULL, pVB, 0) )
        {
            Log( 1, _T("ResetBind(): FillVertexBuffer failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            goto cleanup;
        }

        // Draw vertex buffer
        if( false == DrawVertexBuffer(pVB, NULL) )
        {
            Log( 1, _T("ResetBind(): DrawVertexBuffer failed\n") );
            goto cleanup;
        }    
    }
    else if( pResCase->dwRType == TYPE_INDEXBUFFER )
    {
        // Create index buffer
        if( FAILED( m_pSrcDevice->CreateIndexBuffer( 6, USAGE_DYNAMIC | USAGE_WRITEONLY, FMT_INDEX16, POOL_DEFAULT, &pIB, NULL) ) )
        {
            Log( 1, _T("ResetBind(): CreateIndexBuffer failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            goto cleanup;
        }

        // Fill index buffer
        if( false == FillIndexBuffer(NULL, pIB, 0) )
        {
            Log( 1, _T("ResetBind(): FillIndexBuffer failed with %s\n"), m_pD3D->HResultToString(GetLastError()));
            goto cleanup;
        }

        // Draw index buffer
        if( false == DrawIndexBuffer(pIB, NULL) )
        {
            Log( 1, _T("ResetBind(): DrawIndexBuffer failed\n") );
            goto cleanup;
        }   
    }

    m_pSrcDevice->Present(NULL, NULL, NULL, NULL);	

    bRT = TRUE;

cleanup:
    RELEASE(pVB);
    RELEASE(pIB);

    return bRT;
}

