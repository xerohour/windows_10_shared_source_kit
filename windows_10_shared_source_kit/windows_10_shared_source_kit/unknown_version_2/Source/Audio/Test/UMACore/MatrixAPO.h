2->LoadTexture(0, i, "tex3.bmp"))
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
	g_cubeV[10] = VOLVERTEX(D3DXVECTOR3(