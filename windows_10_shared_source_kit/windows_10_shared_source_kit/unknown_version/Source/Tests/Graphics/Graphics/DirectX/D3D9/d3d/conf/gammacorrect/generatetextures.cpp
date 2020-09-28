#define D3D_OVERLOADS
#include "GammaCorrect.h"

bool GammaCorrect::GenerateTextures()
{
	CnTexture *pTexture1 = NULL;
	CnTexture *pTexture2 = NULL;
	CImage   **ppImages  = NULL;
	HRESULT    hr        = S_OK; 
	FORMAT Format;
	TCHAR szBuffer[30];

	Format = m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

	FmtToString(FORMAT_TEXTURE, &Format, szBuffer);

	if(!CreateTexture(256, 256, m_uLevels, 0, Format, m_Pool, &pTexture1))
	{
		hr = GetLastError();
		WriteToLog(_T("GenerateTextures::CreateTextures() - CreateTexture1 failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
		goto Exit;
	}
	
	if(!CreateTexture(256, 256, m_uLevels, 0, Format, m_Pool, &pTexture2))
	{
		hr = GetLastError();
		WriteToLog(_T("GenerateTextures::CreateTextures() - CreateTexture2 failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
		goto Exit;
	}

	if(pTexture1->IsPalettized())
	{
		pTexture1->SetPalette(NULL);
		pTexture2->SetPalette(NULL);
	}

	if ( KeySet(_T("solid")) || (FMT)Format.d3dfFormat == FMT_A8)
	{
		DWORD    Stripes[7];
		SURFACEDESC pDesc;

		ppImages = new CImage*[m_uLevels];

		if (ppImages == NULL)
		{
			WriteToLog("GenerateTextures - Out of memory on allocating ppImages.\n");
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		ZeroMemory(ppImages, sizeof(CImage *) * m_uLevels);

		for (UINT uLevel = 0; uLevel < m_uLevels; uLevel++ )
		{
			ppImages[uLevel] = new CImage();

			if (ppImages[uLevel] == NULL)
			{
				WriteToLog("GenerateTextures - Out of memory on allocating ppImages[%d].\n", uLevel);
				hr = E_OUTOFMEMORY;
				goto Exit;
			}
		
			pTexture1->GetLevelDesc(uLevel, &pDesc);

			WORD index = uLevel*20;
			WORD color = index % (256-150) + 150;

			if ( (FMT)Format.d3dfFormat == FMT_R16F || (FMT)Format.d3dfFormat == FMT_R32F )
			{
				Stripes[0] = Stripes[1] = Stripes[2] = Stripes[3] = 
				Stripes[4] = Stripes[5] = Stripes[6] = RGBA_MAKE(color, 0, 0, 0xff);
			}
			else if ( Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE )
			{
				Stripes[0] = Stripes[1] = Stripes[2] = Stripes[3] = 
				Stripes[4] = Stripes[5] = Stripes[6] = RGBA_MAKE(0, color, 0, 0xff);
			}
			else if ( (FMT)Format.d3dfFormat == FMT_A8 )
			{
				Stripes[(0+uLevel)%7] = RGBA_MAKE(0, 0, 0, 20*0); 
				Stripes[(1+uLevel)%7] = RGBA_MAKE(0, 0, 0, 20*1); 
				Stripes[(2+uLevel)%7] = RGBA_MAKE(0, 0, 0, 20*2); 
				Stripes[(3+uLevel)%7] = RGBA_MAKE(0, 0, 0, 20*3); 
				Stripes[(4+uLevel)%7] = RGBA_MAKE(0, 0, 0, 20*4); 
				Stripes[(5+uLevel)%7] = RGBA_MAKE(0, 0, 0, 20*5); 
				Stripes[(6+uLevel)%7] = RGBA_MAKE(0, 0, 0, 20*6); 
			}
			else
			{
				Stripes[0] = RGBA_MAKE(color, 0, 0, 255); 
				Stripes[1] = RGBA_MAKE(0, color, 0, 255); 
				Stripes[2] = RGBA_MAKE(0, 0, color, 255); 
				Stripes[3] = RGBA_MAKE(0, color, color, 255); 
				Stripes[4] = RGBA_MAKE(color, 0, color, 255); 
				Stripes[5] = RGBA_MAKE(color, color, 0, 255); 
				Stripes[6] = RGBA_MAKE(color, color, color, 255); 
			}
			
			if (!ppImages[uLevel]->LoadStripes(pDesc.Width, pDesc.Height, (FMT)Format.d3dfFormat == FMT_A8? 7 : 1, Stripes+((FMT)Format.d3dfFormat == FMT_A8?0:uLevel)%7, false))
			{
				WriteToLog("GenerateTextures - LoadStripes failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()) );
				hr = E_OUTOFMEMORY;
				goto Exit;
			}
		}

		if ( ! pTexture1->LoadImage( 0, ppImages, TEXLOAD_DEFAULT ) )
		{
			hr = GetLastError();
			WriteToLog("GenerateTextures - LoadStripes failed on pTexture1. hr=%s.\n", m_pD3D->HResultToString(hr) );
			goto Exit;
		}
		if ( ! pTexture2->LoadImage( 0, ppImages, TEXLOAD_DEFAULT ) )
		{
			hr = GetLastError();
			WriteToLog("GenerateTextures - LoadStripes failed on pTexture2. hr=%s.\n", m_pD3D->HResultToString(hr) );
			goto Exit;
		}
	}
	else	
	{
		if(!pTexture1->LoadTexture(0, "tex0.bmp"))
		{
			hr = GetLastError();
			WriteToLog(_T("GenerateTextures::LoadTextures() - LoadTexture1() failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
			return false;
		}
		if(!pTexture2->LoadTexture(0, "tex1.bmp"))
		{
			hr = GetLastError();
			WriteToLog(_T("GenerateTextures::LoadTextures() - LoadTexture2() failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
			return false;
		}
	}

Exit:
    if (NULL != ppImages)
	for (DWORD k = 0; k < m_uLevels; k++)
		SAFEDELETE( ppImages[k] );
	SAFEDELETEA( ppImages );

	if (FAILED(hr))
	{
		RELEASE(pTexture1);
		RELEASE(pTexture2);

		return false;
	}
	else
	{
		m_pTexture1 = pTexture1;
		m_pTexture2 = pTexture2;

		SetTexture(0, m_pTexture1);
		SetTexture(1, m_pTexture2);
	}	
				
	return true;
}

bool GammaCorrect::GenerateCubeTextures()
{
	CnCubeTexture *pTexture1 = NULL;
	CnCubeTexture *pTexture2 = NULL;
	CImage      ***pppImages = NULL;
	HRESULT        hr        = S_OK; 
	
	FORMAT Format;
	TCHAR szBuffer[30];
	DWORD width = 128;

	Format = m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat];

	FmtToString(FORMAT_TEXTURE, &Format, szBuffer);

    if(!CreateCubeTexture(width, m_uLevels, 0, Format, m_Pool, &pTexture1))
	{
		hr = GetLastError();
		WriteToLog(_T("GenerateCubeTextures::CreateCubeTextures() - CreateTexture1 failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
		goto Exit;
	}
	
	if(!CreateCubeTexture(width, m_uLevels, 0, Format, m_Pool, &pTexture2))
	{
		hr = GetLastError();
		WriteToLog(_T("GenerateCubeTextures::CreateCubeTextures() - CreateTexture2 failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
		goto Exit;
	}


	if(pTexture1->IsPalettized())
	{
		pTexture1->SetPalette(NULL);
		pTexture2->SetPalette(NULL);
	}

	if ( KeySet(_T("solid")) || (FMT)Format.d3dfFormat == FMT_A8)
	{
		D3DCOLOR  Stripes[6];
		
		pppImages = new CImage**[m_uLevels];
		if (pppImages == NULL)
		{
			WriteToLog("GenerateCubeTextures: out of memory on allocating image.\n");
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		ZeroMemory(pppImages, sizeof(CImage**) * m_uLevels);

		for(UINT uLevel = 0; uLevel < m_uLevels; uLevel++)
		{
			if ((FMT)m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].d3dfFormat == FMT_R16F || 
				(FMT)m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].d3dfFormat == FMT_R32F  )
			{
				WORD color = 255 - uLevel*15;
				for (int i = 0; i < 6; i++)
				{
					Stripes[i] = RGBA_MAKE(color, 0, 0, 0xff); 
					color -= 15;
				}
			}
			else if (m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].ddpfPixelFormat.dwFlags & PF_LUMINANCE)
			{
				WORD color = 255 - uLevel*15;
				for (int i = 0; i < 6; i++)
				{
					Stripes[i] = RGBA_MAKE(0, color, 0, 0xff); 
					color -= 15;
				}
			}
			else if ( (FMT)m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].d3dfFormat == FMT_A8 )
			{
				WORD color = 255 - uLevel*15;
				for (int i = 0; i < 6; i++)
				{
					Stripes[i] = RGBA_MAKE(0, 0, 0, color); 
					color -= 15;
				}
			}
			else
			{
				Stripes[0] = RGBA_MAKE(255 - uLevel*15, 0, 0, 255);
				Stripes[1] = RGBA_MAKE(0, 255 - uLevel*15, 0, 255);
				Stripes[2] = RGBA_MAKE(0, 0, 255 - uLevel*15, 255);
				Stripes[3] = RGBA_MAKE(0, 255 - uLevel*15, 255 - uLevel*15, 255);
				Stripes[4] = RGBA_MAKE(255 - uLevel*15, 0, 255 - uLevel*15, 255);
				Stripes[5] = RGBA_MAKE(255 - uLevel*15, 255 - uLevel*15, 0, 255);
			}

			pppImages[uLevel] = new CImage*[6];
			if (pppImages[uLevel] == NULL)
			{
				WriteToLog("GenerateCubesTextures: out of memory on allocating image on level %d.\n", uLevel);
				hr = E_OUTOFMEMORY;
				goto Exit;
			}

			ZeroMemory(pppImages[uLevel], sizeof(CImage*) * 6);

			for ( int FaceIndex = 0; FaceIndex < 6; FaceIndex++)
			{
				pppImages[uLevel][FaceIndex] = new CImage();
				if ( pppImages[uLevel][FaceIndex] == NULL )
				{
					WriteToLog("GenerateCubesTextures: out of memory on allocating image on level %d on face %d.\n", uLevel, FaceIndex);
					hr = E_OUTOFMEMORY;
					goto Exit;
				}

				if (!pppImages[uLevel][FaceIndex]->LoadStripes( width, width, 1, Stripes+(uLevel+FaceIndex)%6, false, false ))
				{
					WriteToLog("GenerateCubeTexture::LoadStripes() on Image failed on face %d for level %d\n", FaceIndex, uLevel);
					hr = E_OUTOFMEMORY;
					goto Exit;
				}
			}
			width  >>= 1;
		}

		if ( !pTexture1->LoadImage(pppImages, TEXLOAD_DEFAULT ))
		{
			hr = GetLastError();
			WriteToLog("GenerateCubeTextures: LoadImage() to cube map1 failed with HResult = %s.\n", m_pD3D->HResultToString(hr));
			goto Exit;
		}
		if ( !pTexture2->LoadImage(pppImages, TEXLOAD_DEFAULT ))
		{
			hr = GetLastError();
			WriteToLog("GenerateCubeTextures: LoadImage() to cube map2 failed with HResult = %s.\n", m_pD3D->HResultToString(hr));
			goto Exit;
		}
	}
	else
	{
		if(!pTexture1->LoadTexture(0, (CUBEMAP_FACES)CUBEMAP_FACE_ALL, "tex0.bmp"))
		{
			hr = GetLastError();
			WriteToLog(_T("GenerateCubeTextures::LoadTextures() - LoadTexture1() failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
			goto Exit;
		}

		if(!pTexture2->LoadTexture(0, (CUBEMAP_FACES)CUBEMAP_FACE_ALL, "tex2.bmp"))
		{
			hr = GetLastError();
			WriteToLog(_T("GenerateCubeTextures::LoadTextures() - LoadTexture2() failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
			goto Exit;
		}
	}

Exit:
	if (NULL != pppImages)
	for (DWORD k = 0; k < m_uLevels; k++)
	{
		if (NULL != pppImages[k])
		for (DWORD j = 0; j < 6; j++)
			SAFEDELETE(pppImages[k][j]);
		SAFEDELETEA(pppImages[k]);
	}
	SAFEDELETEA(pppImages);

	if (FAILED(hr))
	{
		RELEASE(pTexture1);
		RELEASE(pTexture2);

		return false;
	}
	else
	{
		m_pTexture1 = pTexture1;
		m_pTexture2 = pTexture2;

		SetTexture(0, m_pTexture1);
		SetTexture(1, m_pTexture2);
	}

	return true;
}

bool GammaCorrect::GenerateVolTextures()
{
	CnVolTexture *pTexture1 = NULL;
	CnVolTexture *pTexture2 = NULL;
	CImage     ***pppImages = NULL;
	HRESULT       hr        = S_OK;
	FORMAT Format;
	TCHAR szBuffer[80];
	DWORD texDepth = 16, depth = texDepth, width = 128;

	Format = m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat];

	FmtToString(FORMAT_TEXTURE, &Format, szBuffer);

    if(!CreateVolumeTexture(width, width, texDepth, m_uLevels, 0, Format, m_Pool, &pTexture1))
	{
		hr = GetLastError();
		WriteToLog(_T("GenerateVolTextures::CreateVolumeTextures() - CreateTexture1 failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
		goto Exit;
	}
	
	if(!CreateVolumeTexture(width, width, texDepth, m_uLevels, 0, Format, m_Pool, &pTexture2))
	{
		hr = GetLastError();
		WriteToLog(_T("GenerateVolTextures::CreateVolumeTextures() - CreateTexture2 failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
		goto Exit;
	}


	if(pTexture1->IsPalettized())
	{
		pTexture1->SetPalette(NULL);
		pTexture2->SetPalette(NULL);
	}

	if ( KeySet(_T("solid")) || (FMT)Format.d3dfFormat == FMT_A8)
	{
		D3DCOLOR     Stripe[1];

		pppImages = new CImage**[m_uLevels];
		if (NULL == pppImages)
		{
			WriteToLog("GenerateVolTextures() - Out of memory creating image.\n");
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		ZeroMemory(pppImages, sizeof(CImage**) * m_uLevels);

		for (UINT uLevel = 0; uLevel < m_uLevels; uLevel++)
		{
			DWORD temp = (uLevel)%8;
			
			pppImages[uLevel] = new CImage*[depth];
			if (NULL == pppImages[uLevel])
			{
				WriteToLog("GenerateVolTextures() - Out of memory on volume %s on level %d.\n", szBuffer, uLevel);
				hr = E_OUTOFMEMORY;
				goto Exit;
			}

			ZeroMemory(pppImages[uLevel], sizeof(CImage**) * depth);

    		for (DWORD SliceIndex = 0; SliceIndex < depth; SliceIndex++)
			{
				if ((FMT)m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat].d3dfFormat == FMT_R16F || 
					(FMT)m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat].d3dfFormat == FMT_R32F  )
				{
					DWORD index = SliceIndex*20+temp*20;
					DWORD color = index % (255-100) + 100;
					Stripe[0] = RGBA_MAKE(color, 0, 0, 0xff);
				}
				else if (m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat].ddpfPixelFormat.dwFlags & PF_LUMINANCE)
				{
					DWORD index = SliceIndex*20+temp*20;
					DWORD color = index % (255-100) + 100;
					Stripe[0] = RGBA_MAKE(0, color, 0, 0xff);
				}
				else if ( (FMT)m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat].d3dfFormat == FMT_A8 )
				{
					DWORD index = SliceIndex*20+temp*20;
					DWORD color = index % (255-100) + 100;
					Stripe[0] = RGBA_MAKE(0, 0, 0, color);
				}
				else
				{
					WORD index = (WORD)SliceIndex*20;
					WORD color = index % (255-150) + 150;
					WORD   red = temp%2 == 0 ? color : 0;
					WORD green = (temp+1)%4 >= 2 ? color : 0;
					WORD  blue = temp/3 != 0 ? color : 0;
					Stripe[0] = RGBA_MAKE(red, green, blue, 255);
				}

				pppImages[uLevel][SliceIndex] = new CImage();
				if (NULL == pppImages[uLevel][SliceIndex])
				{
					WriteToLog("GenerateVolTextures() - Out of memory on volume %s level %d and slice %d.\n", szBuffer, uLevel, SliceIndex);
					hr = E_OUTOFMEMORY;
					goto Exit;
				}

				if (!pppImages[uLevel][SliceIndex]->LoadStripes(width, width, 1, Stripe, false))
				{
					WriteToLog("GenerateVolTextures(): LoadStripes failed on volume %s on level %d and slice %d\n", szBuffer, uLevel, SliceIndex);
					hr = E_OUTOFMEMORY;
					goto Exit;
				}
			}
			width  = max( 1, width>>1 );
			depth  = max( 1, depth>>1 );
		}

		if ( !pTexture1->LoadImage(pppImages, TEXLOAD_DEFAULT))
		{
			hr = GetLastError();
			WriteToLog("GenerateVolTextures(): LoadImage() failed on volume %s with HResult = %s.\n", szBuffer, m_pD3D->HResultToString(hr));
			goto Exit;
		}
		if ( !pTexture2->LoadImage(pppImages, TEXLOAD_DEFAULT))
		{
			hr = GetLastError();
			WriteToLog("GenerateVolTextures(): LoadImage() failed on volume %s with HResult = %s.\n", szBuffer, m_pD3D->HResultToString(hr));
			goto Exit;
		}
	}
	else
	{
		for (int i = 0; i < texDepth; i++)
		{
			if(!pTexture1->LoadTexture(0, i, "tex0.bmp"))
			{
				hr = GetLastError();
				WriteToLog(_T("GenerateVolTextures::LoadTextures() - LoadTexture1() failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
				goto Exit;
			}
			if(!pTexture2->LoadTexture(0, i, "tex3.bmp"))
			{
				hr = GetLastError();
				WriteToLog(_T("GenerateVolTextures::LoadTextures() - LoadTexture2() failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
				goto Exit;
			}
		}
	}

Exit:
	UINT nDepth = texDepth;
	if (NULL != pppImages)
	for (DWORD i = 0; i < m_uLevels; i++)
	{
		if (NULL != pppImages[i])
		for (DWORD j = 0; j < nDepth; j++)
			SAFEDELETE(pppImages[i][j]);
		SAFEDELETEA(pppImages[i]);
		nDepth = max( 1, nDepth>>1 );
	}
	SAFEDELETEA(pppImages);

	if (FAILED(hr))
	{
		RELEASE(pTexture1);
		RELEASE(pTexture2);

		return false;
	}
	else
	{
		m_pTexture1 = pTexture1;
		m_pTexture2 = pTexture2;

		SetTexture(0, m_pTexture1);
		SetTexture(1, m_pTexture2);
	}

	return true;
}

bool GammaCorrect::GenerateVertexBuffer()
{
	DWORD width  = m_Options.D3DOptions.nBackBufferWidth;

	D3DVERTEX g_V3[4*MAX_LEVEL_COUNT];
	float factor = 1.f;
	
	ASSERT(m_uLevels <= MAX_LEVEL_COUNT);
		
	for (int i = 0; i < m_uLevels; i++)
	{
		g_V3[i*4+0]  = D3DVERTEX(D3DXVECTOR3(  1.f/factor, -1.f/factor, 0.5f), D3DXVECTOR3( 0.f, 0.f, 1.f),     0.f,  0.99f);
		g_V3[i*4+1]  = D3DVERTEX(D3DXVECTOR3(  1.f/factor,  1.f/factor, 0.5f), D3DXVECTOR3( 0.f, 0.f, 1.f),     0.f,    0.f);
		g_V3[i*4+2]  = D3DVERTEX(D3DXVECTOR3( -1.f/factor, -1.f/factor, 0.5f), D3DXVECTOR3( 0.f, 0.f, 1.f),   0.99f,  0.99f);
		g_V3[i*4+3]  = D3DVERTEX(D3DXVECTOR3( -1.f/factor,  1.f/factor, 0.5f), D3DXVECTOR3( 0.f, 0.f, 1.f),   0.99f,    0.f);
		factor *= 2.f;
	}

	if( !(CreateVertexBuffer( m_uLevels*4*GetVertexSize(D3DFVF_VERTEX), 0, D3DFVF_VERTEX, POOL_MANAGED, &m_pVertexBuffer)))
	    return false;
	
	VOID* pV3;
	if (!m_pVertexBuffer->Lock( 0, m_uLevels*4*GetVertexSize(D3DFVF_VERTEX), (BYTE**)&pV3, 0))
	{
		WriteToLog( "Failed to lock m_pVertexBuffer.\n" );
		return false;
	}

	memcpy( pV3, g_V3, m_uLevels*4*GetVertexSize(D3DFVF_VERTEX) );

	if (!m_pVertexBuffer->Unlock())
	{
		WriteToLog( "Failed to unlock m_pVertexBuffer.\n" );
		return false;
	}

	SetStreamSource(0, m_pVertexBuffer, GetVertexSize(D3DFVF_VERTEX));
	SetVertexShader(D3DFVF_VERTEX);

	return true;
}

bool GammaCorrect::GenerateCubeVertexBuffer()
{
	HRESULT hr;
	VOLVERTEX g_cubeV[24];

	g_cubeV[0]  = VOLVERTEX(D3DXVECTOR3(  0.33f,  1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( 0.99f,  0.99f, -0.99f));
	g_cubeV[1]  = VOLVERTEX(D3DXVECTOR3(  0.33f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( 0.99f, -0.99f, -0.99f));
	g_cubeV[2]  = VOLVERTEX(D3DXVECTOR3(  1.00f,  1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( 0.99f,  0.99f,  0.99f));
	g_cubeV[3]  = VOLVERTEX(D3DXVECTOR3(  1.00f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( 0.99f, -0.99f,  0.99f));
	g_cubeV[4]  = VOLVERTEX(D3DXVECTOR3( -0.33f,  1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(-0.99f,  0.99f,  0.99f));
	g_cubeV[5]  = VOLVERTEX(D3DXVECTOR3( -0.33f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(-0.99f, -0.99f,  0.99f));
	g_cubeV[6]  = VOLVERTEX(D3DXVECTOR3(  0.33f,  1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(-0.99f,  0.99f, -0.99f));
	g_cubeV[7]  = VOLVERTEX(D3DXVECTOR3(  0.33f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(-0.99f, -0.99f, -0.99f));
	g_cubeV[8]  = VOLVERTEX(D3DXVECTOR3( -1.00f,  1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(  0.99f, 0.99f, -0.99f));
	g_cubeV[9]  = VOLVERTEX(D3DXVECTOR3( -1.00f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(  0.99f, 0.99f,  0.99f));
	g_cubeV[10] = VOLVERTEX(D3DXVECTOR3( -0.33f,  1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( -0.99f, 0.99f, -0.99f));
	g_cubeV[11] = VOLVERTEX(D3DXVECTOR3( -0.33f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( -0.99f, 0.99f,  0.99f));
	g_cubeV[12] = VOLVERTEX(D3DXVECTOR3(  0.33f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(  0.99f,-0.99f,  0.99f));
	g_cubeV[13] = VOLVERTEX(D3DXVECTOR3(  0.33f, -1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(  0.99f,-0.99f, -0.99f));
	g_cubeV[14] = VOLVERTEX(D3DXVECTOR3(  1.00f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( -0.99f,-0.99f,  0.99f));
	g_cubeV[15] = VOLVERTEX(D3DXVECTOR3(  1.00f, -1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( -0.99f,-0.99f, -0.99f));
	g_cubeV[16] = VOLVERTEX(D3DXVECTOR3( -0.33f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(  0.99f,  0.99f,  0.99f));
	g_cubeV[17] = VOLVERTEX(D3DXVECTOR3( -0.33f, -1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(  0.99f, -0.99f,  0.99f));
	g_cubeV[18] = VOLVERTEX(D3DXVECTOR3(  0.33f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( -0.99f,  0.99f,  0.99f));
	g_cubeV[19] = VOLVERTEX(D3DXVECTOR3(  0.33f, -1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( -0.99f, -0.99f,  0.99f));
	g_cubeV[20] = VOLVERTEX(D3DXVECTOR3( -1.00f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( -0.99f,  0.99f, -0.99f));
	g_cubeV[21] = VOLVERTEX(D3DXVECTOR3( -1.00f, -1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( -0.99f, -0.99f, -0.99f));
	g_cubeV[22] = VOLVERTEX(D3DXVECTOR3( -0.33f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(  0.99f,  0.99f, -0.99f));
	g_cubeV[23] = VOLVERTEX(D3DXVECTOR3( -0.33f, -1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(  0.99f, -0.99f, -0.99f));

	if ( !CreateVertexBuffer( 24*sizeof(VOLVERTEX), 0, FVF_VOLVERTEX, POOL_MANAGED, &m_pVertexBuffer ) )
	{
		Fail();
		WriteToLog("GenerateCubeVertexBuffer: failed to create vertex buffer with result %s.\n", m_pD3D->HResultToString(GetLastError()));
		return false;
	}

	VOID* pV;
	if ( !m_pVertexBuffer->Lock( 0, 24*sizeof(VOLVERTEX), (BYTE**)&pV, 0) )
	{
		Fail();
		WriteToLog( "GenerateCubeVertexBuffer: failed to lock buffer with result %s.\n", m_pD3D->HResultToString(GetLastError()) );
		RELEASE(m_pVertexBuffer);
		return false;
	}

	memcpy( pV, g_cubeV, 24*sizeof(VOLVERTEX));

	if ( !m_pVertexBuffer->Unlock() )
	{
		
		WriteToLog( "GenerateCubeVertexBuffer: failed to unlock buffer with result %s.\n", m_pD3D->HResultToString(GetLastError()) );
		RELEASE(m_pVertexBuffer);
		return false;
	}

	SetStreamSource(0, m_pVertexBuffer, sizeof(VOLVERTEX));
	SetVertexShader(FVF_VOLVERTEX);

	return true;
}
	
bool GammaCorrect::GenerateVolVertexBuffer()
{
	HRESULT hr;
	UINT offset = 0;
	DWORD texDepth = 16;
	UINT NumberOfSlices = 16+8+4+2+1;
	
	VOLVERTEX *g_V = new VOLVERTEX[4*NumberOfSlices];
	
	for (UINT index = texDepth; index >= 1; index /= 2)
	{
		DWORD NumPerRow = (DWORD)ceil(sqrt((float)index));
		float length = 2.f / NumPerRow;
		for (UINT sliceIndex = 0; sliceIndex < index; sliceIndex++)
		{
			FLOAT w = (FLOAT)sliceIndex/index + 1.0f/(2.0f*index);
			DWORD x = sliceIndex % NumPerRow;
			DWORD y = sliceIndex / NumPerRow;
			float u1 = 0.f, u2 = 0.99f, v2 = 0.99f;
			if (index == texDepth)
			{
				u1 = 0.5; 
			}
			g_V[offset+sliceIndex*4+0] = VOLVERTEX(D3DXVECTOR3( 1.f-(x+0)*length, 1.f-(y+1)*length, 0.5f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(u1,  v2, w)); 
			g_V[offset+sliceIndex*4+1] = VOLVERTEX(D3DXVECTOR3( 1.f-(x+1)*length, 1.f-(y+1)*length, 0.5f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(u2,  v2, w)); 
			g_V[offset+sliceIndex*4+2] = VOLVERTEX(D3DXVECTOR3( 1.f-(x+0)*length, 1.f-(y+0)*length, 0.5f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(u1,  u1, w)); 
			g_V[offset+sliceIndex*4+3] = VOLVERTEX(D3DXVECTOR3( 1.f-(x+1)*length, 1.f-(y+0)*length, 0.5f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(u2,  u1, w)); 

/*			g_V[offset+sliceIndex*4]    = VOLVERTEX(D3DXVECTOR3(  0.f, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(0.00f,  0.99f, w)); 
			g_V[offset+sliceIndex*4+1]  = VOLVERTEX(D3DXVECTOR3(  0.f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(0.00f,  0.00f, w)); 
			g_V[offset+sliceIndex*4+2]  = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(0.99f,  0.99f, w)); 
			g_V[offset+sliceIndex*4+3]  = VOLVERTEX(D3DXVECTOR3( -1.f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(0.99f,  0.00f, w)); */
		}
		offset += 4*index;
	}

	if ( !CreateVertexBuffer( 4*NumberOfSlices*sizeof(VOLVERTEX), 0, FVF_VOLVERTEX, POOL_MANAGED, &m_pVertexBuffer ) )
	{
		Fail();
		WriteToLog("GenerateVolVertexBuffer: failed to create vertex buffer with result %s.\n", m_pD3D->HResultToString(GetLastError()));
		return false;
	}

	VOID* pV;
	if ( !m_pVertexBuffer->Lock( 0, 4*NumberOfSlices*sizeof(VOLVERTEX), (BYTE**)&pV, 0) )
	{
		Fail();
		WriteToLog( "GenerateVolVertexBuffer: failed to lock buffer with result %s.\n", m_pD3D->HResultToString(GetLastError()) );
		RELEASE(m_pVertexBuffer);
		return false;
	}

	memcpy( pV, g_V, 4*NumberOfSlices*sizeof(VOLVERTEX));

	if ( !m_pVertexBuffer->Unlock() )
	{
		
		WriteToLog( "GenerateVolVertexBuffer: failed to unlock buffer with result %s.\n", m_pD3D->HResultToString(GetLastError()) );
		RELEASE(m_pVertexBuffer);
		return false;
	}

	SetStreamSource(0, m_pVertexBuffer, sizeof(VOLVERTEX));
	SetVertexShader(FVF_VOLVERTEX);
	delete [] g_V;

	return true;
}

