#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "ResMan.h"

//-----------------------------------------------------------------------------

CChangeColorsTest::CChangeColorsTest()
{
	m_szTestName = "ChangeColors Test";
	m_szCommandKey = "cc";
	m_PrimCount = 5;
}

//-----------------------------------------------------------------------------

UINT CChangeColorsTest::TestInitialize(void)
{
	srand(10000);
	m_prevTest = 0;
	SetTestRange( 1, m_uCommonTextureFormats + m_uCommonCubeFormats + m_uCommonVolumeFormats );

	UINT result = ResMan::TestInitialize();
	if ( result != D3DTESTINIT_RUN )
		return result;
	
	CheckChangeColorsTestCaps();
	
	if (KeySet("output"))
	{
		if (m_dwVersion >= 0x0900)
            m_output = fopen("cc9.txt", "w");
		else if (m_dwVersion >= 0x0800)
            m_output = fopen("cc8.txt", "w");
		else if (m_dwVersion >= 0x0700)
            m_output = fopen("cc7.txt", "w");
//		else
//            m_output = fopen("cc6.txt", "w");
		if (!m_output || fprintf(m_output, "%s\n", "      ") != 7)
			return D3DTESTINIT_ABORT;
	}
	else if (KeySet("input"))
	{
		if (m_dwVersion >= 0x0900)
            m_input = fopen("cc9.txt", "r");
		else if (m_dwVersion >= 0x0800)
            m_input = fopen("cc8.txt", "r");
		else if (m_dwVersion >= 0x0700)
            m_input = fopen("cc7.txt", "r");
//		else
//            m_input = fopen("cc6.txt", "r");
		if (!m_input)
			return D3DTESTINIT_ABORT;
	}
	
	return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------

bool CChangeColorsTest::ExecuteTest(UINT iTest)
{
	int result;
	msgString[0] = '\0';
	m_Message[0] = '\0';

	//need to call rand() for all skipped tests 
	//so that demensions will be the same no metter how you run the tests cases
	if (NULL == m_input)
	{
		for (int i = m_prevTest+1; i < iTest; i++)
		{
			if ( i <= m_uCommonTextureFormats )
			{
                if ( m_UseMipMaps && m_bWrapAndMipTex[i-1])
					rand();
				rand();
				rand();
			}
			else if ( i <= m_uCommonTextureFormats+m_uCommonCubeFormats )
			{
                if ( m_UseMipCubeMaps && m_bWrapAndMipCube[i-m_uCommonTextureFormats-1])
					rand();
				rand();
				rand();
			}
			else
			{
                if ( m_UseMipVolMaps && m_bWrapAndMipVol[i-m_uCommonTextureFormats-m_uCommonCubeFormats-1] )
					rand();
				rand(); rand(); 
				rand(); rand();
			}
		}
		m_prevTest = iTest;
	}

	if ( iTest <= m_uCommonTextureFormats )
	{
		result = AddTextureToList(iTest-1, 300, 200);
		startResInfo = m_ResInfosList.begin();
		if ( FAIL == result || SKIP == result )
		{
			BeginTestCase(msgString, iTest);
			DestroyAllResInfosFromList();
			if ( SKIP == result )
				SkipTestRange( iTest, iTest);
			else
			{
				Fail();
				DestroyAllTexturesFromList();
				DestroyAllVertexBuffersFromList();
			}
			return false;
		}
		else
		{
			delete [] (*startResInfo)->pColors;
			(*startResInfo)->pColors = NULL;
			startTexture = m_TexturesList.begin();
			startVBuffer = m_VBuffersList.begin();
			result = CreateTextures( *startTexture, iTest-1 );
			if ( FAIL == result || SKIP == result )
			{
				BeginTestCase(msgString, iTest);
				DestroyAllResInfosFromList();
				DestroyAllTexturesFromList();
				DestroyAllVertexBuffersFromList();
				if ( SKIP == result )
					SkipTestRange( iTest, iTest);
				else
					Fail();
				return false;
			}
		}
	}
	else if ( iTest <= m_uCommonTextureFormats+m_uCommonCubeFormats )
	{
		result = AddCubeMapToList(iTest-m_uCommonTextureFormats-1, 300, 200);
		startResInfo = m_ResInfosList.begin();
		if ( FAIL == result || SKIP == result )
		{
			BeginTestCase(msgString, iTest);
			DestroyAllResInfosFromList();
			if ( SKIP == result )
				SkipTestRange( iTest, iTest);
			else
			{
				Fail();
				DestroyAllCubeMapsFromList();
				DestroyAllVertexBuffersFromList();
			}
			return false;
		}
		else
		{
			delete [] (*startResInfo)->pColors;
			(*startResInfo)->pColors = NULL;
			startCubeMap = m_CubesList.begin();
			startVBuffer = m_VBuffersList.begin();
			result = CreateCubeTextures( *startCubeMap, iTest-m_uCommonTextureFormats-1 );
			if ( FAIL == result || SKIP == result )
			{
				BeginTestCase(msgString, iTest);
				DestroyAllResInfosFromList();
				DestroyAllCubeMapsFromList();
				DestroyAllVertexBuffersFromList();
				if ( SKIP == result )
					SkipTestRange( iTest, iTest);
				else
					Fail();
				return false;
			}
		}
	}
	else//if ( iTest >= m_uCommonTextureFormats+m_uCommonCubeFormats+1 )
	{
		result = AddVolTextureToList(iTest-(m_uCommonTextureFormats+m_uCommonCubeFormats)-1, 300, 200);
		startResInfo = m_ResInfosList.begin();
		if ( FAIL == result || SKIP == result )
		{
			BeginTestCase(msgString, iTest);
			DestroyAllResInfosFromList();
			if ( SKIP == result )
				SkipTestRange( iTest, iTest);
			else
			{
				Fail();
				DestroyAllVolTexturesFromList();
				DestroyAllVertexBuffersFromList();
			}
			return false;
		}
		else
		{
			delete [] (*startResInfo)->pColors;
			(*startResInfo)->pColors = NULL;
			startVolTex = m_VolumesList.begin();
			startVBuffer = m_VBuffersList.begin();
			result = CreateVolTextures( *startVolTex, iTest-(m_uCommonTextureFormats+m_uCommonCubeFormats)-1 );
			if ( FAIL == result || SKIP == result )
			{
				BeginTestCase(msgString, iTest);
				DestroyAllResInfosFromList();
				DestroyAllVolTexturesFromList();
				DestroyAllVertexBuffersFromList();
				if ( SKIP == result )
					SkipTestRange( iTest, iTest);
				else
					Fail();
				return false;
			}
		}
	}
	
	HRESULT hr;

	BeginTestCase(msgString, iTest);

	if (m_uPrevMaxMipLevel != 0)
	{
		m_uPrevMaxMipLevel = 0;
		SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
	}
	
    hr = m_pSrcDevice->BeginScene();
	if (FAILED(hr))
	{
		WriteToLog("BeginScene() failed hr=%s (%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		DestroyAllResInfosFromList();
		DestroyAllTexturesFromList();
		DestroyAllCubeMapsFromList();
		DestroyAllVolTexturesFromList();
		DestroyAllVertexBuffersFromList();
		return false;
	}
	
	DWORD LevelIndex = 0, newLevelIndex = 0;
	D3DXMATRIX	Matrix, TranslatedM, ScaledM;
	DWORD cellIndex = 0;
	bool newPrimitive = true;
	bool draw = true;
	DWORD ncell;
	FLOAT wcell, hcell;
	
	if ( m_uTestNumber <= m_uCommonTextureFormats  )
	{
		m_TotalNumberOfSlices *= m_PrimCount; 
		ncell = (DWORD)ceil(sqrt((float)m_TotalNumberOfSlices));
		wcell = 2.f/ncell; hcell = 2.f/ncell;

		hr = m_pSrcDevice->SetTexture(0, *startTexture);
		if (FAILED(hr))
		{
			WriteToLog("SetTexture() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
			m_pSrcDevice->EndScene();
			DestroyAllResInfosFromList();
			DestroyAllTexturesFromList();
			DestroyAllVertexBuffersFromList();
			if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
			{
				Fail();
			}
			return false;
		}

		if (((*startResInfo)->Format).d3dfFormat == FMT_A8 )
		{
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE );
		}

		D3DXMatrixTranslation(&TranslatedM, -1+wcell/2.f, 1+hcell/2.f, 0);
		D3DXMatrixScaling(&ScaledM, wcell/2, hcell/2, 1);
		D3DXMatrixMultiply(&Matrix, &ScaledM, &TranslatedM);

		for( DWORD row = 0; row < ncell; row++)
		{
			D3DXMatrixTranslation(&TranslatedM, 2, -hcell, 0);
			D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
			
			for(DWORD col = 0; col < ncell; col++)
			{
				if (row*ncell+col >= m_TotalNumberOfSlices)
					break;
				if( !m_UseLODBias || (((*startResInfo)->Format).ddpfPixelFormat.dwFlags & PF_FOURCC) )
				{
					if (m_uPrevMaxMipLevel != LevelIndex)
					{
						m_uPrevMaxMipLevel = LevelIndex;
						SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, LevelIndex);
					}
				}
				else if ((*startTexture)->GetLevelCount() > 1)
					(*startTexture)->SetLOD(LevelIndex); 

				D3DXMatrixTranslation(&TranslatedM, -wcell, 0, 0);
				D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);

				m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);
				
				if (!m_pSrcDevice->DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP, *startVBuffer, 
						0, 4, NULL, 0, 0, 0, POOL_MANAGED, false))
				{
					hr = m_pSrcDevice->GetLastError();
					
					WriteToLog("DrawPrimitiveVertexBuffer() failed on level %d hr=%s(%x).\n", LevelIndex, m_pD3D->HResultToString(hr), hr);
					(*startResInfo)->pCheckColor[LevelIndex] = false;

					if (D3DERR_OUTOFVIDEOMEMORY == hr)
					{
						m_pSrcDevice->GetLastError();
					}
					else
						Fail();
				}

				if (++LevelIndex == (*startTexture)->GetLevelCount())
				{
					LevelIndex = 0;
					if ( !ChangeTextureColors( *startTexture, m_uTestNumber-1, (WORD)((row*ncell+col+1)/(*startTexture)->GetLevelCount() )))
						Fail();
				}
			}
		}

		if (((*startResInfo)->Format).d3dfFormat == FMT_A8 )
		{
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		}

		startTexture++;
	}
	else if ( m_uTestNumber <= m_uCommonTextureFormats + m_uCommonCubeFormats )
	{
		m_TotalNumberOfSlices *= m_PrimCount;
		ncell = (DWORD)ceil(sqrt((float)m_TotalNumberOfSlices));
		wcell = 2.f/ncell; hcell = 2.f/ncell;

		hr = m_pSrcDevice->SetTexture(0, *startCubeMap);
		if (FAILED(hr))
		{
			WriteToLog("SetTexture() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
			m_pSrcDevice->EndScene();
			DestroyAllResInfosFromList();
			DestroyAllCubeMapsFromList();
			DestroyAllVertexBuffersFromList();
			if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
			{
				Fail();
			}
			return false;
		}

		if (((*startResInfo)->Format).d3dfFormat == FMT_A8 )
		{
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE );
		}

		D3DXMatrixTranslation(&TranslatedM, -1+wcell/2.f, 1+hcell/2.f, 0);
		D3DXMatrixScaling(&ScaledM, wcell/2, hcell/2, 1);
		D3DXMatrixMultiply(&Matrix, &ScaledM, &TranslatedM);

		for (DWORD row = 0; row <ncell; row++)
		{
			D3DXMatrixTranslation(&TranslatedM, 2, -hcell, 0);
			D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);

			for(DWORD col = 0; col < ncell; col++)
			{
				if (row*ncell+col >= m_TotalNumberOfSlices)
					break;
				if ((row*ncell+col)%6 == 0)
				{
					if( !m_UseLODBias || (((*startResInfo)->Format).ddpfPixelFormat.dwFlags & PF_FOURCC) )
					{
						if (m_uPrevMaxMipLevel != LevelIndex)
						{
							m_uPrevMaxMipLevel = LevelIndex;
							SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, LevelIndex);
						}
					}
					else if ((*startCubeMap)->GetLevelCount() > 1)
						(*startCubeMap)->SetLOD(LevelIndex);

					LevelIndex++;
				}
				D3DXMatrixTranslation(&TranslatedM, -wcell, 0, 0);
				D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
				m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);
				
				if (!m_pSrcDevice->DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP, *startVBuffer, 
						((row*ncell+col)%6)*4, 4, NULL, 0, 0, 0, POOL_MANAGED, false))
				{
					hr = m_pSrcDevice->GetLastError();
					
					WriteToLog("DrawPrimitiveVertexBuffer() failed on level %d on face %d hr=%s(%x).\n", LevelIndex-1, (row*ncell+col)%6, m_pD3D->HResultToString(hr), hr);
					(*startResInfo)->pCheckColor[6*(LevelIndex-1) + (row*ncell+col)%6] = false;

					if (D3DERR_OUTOFVIDEOMEMORY == hr)
					{
						m_pSrcDevice->GetLastError();
					}
					else
						Fail();
				}

				if (LevelIndex == (*startCubeMap)->GetLevelCount() && (row*ncell+col)%6 == 5)
				{
					LevelIndex = 0;
					if ( !ChangeCubeMapColors(*startCubeMap, m_uTestNumber-(m_uCommonTextureFormats+1), (WORD)((row*ncell+col+1)/(6*(*startCubeMap)->GetLevelCount()))))
						Fail();
				}
			}
		}

		if (((*startResInfo)->Format).d3dfFormat == FMT_A8 )
		{
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		}

		startCubeMap++;
	}
	else
	{
		DWORD numOfSlices = 0, SliceIndex = 0, SliceOffset = 0;
		VOLUME_DESC pDesc;
		(*startVolTex)->GetLevelDesc(0, &pDesc);
		m_TotalNumberOfSlices *= m_PrimCount;
		ncell = (DWORD)ceil(sqrt((float)m_TotalNumberOfSlices));
		wcell = 2.f/ncell; hcell = 2.f/ncell;
		newLevelIndex = pDesc.Depth;

		hr = m_pSrcDevice->SetStreamSource( 0, *startVBuffer, GetVertexSize(FVF_VOLVERTEX) );
		if (FAILED(hr))
		{
			WriteToLog("SetStreamSource() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
			m_pSrcDevice->EndScene();
			DestroyAllResInfosFromList();
			DestroyAllVolTexturesFromList();
			DestroyAllVertexBuffersFromList();
			if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
			{
				Fail();
			}
			return false;
		}

		hr = m_pSrcDevice->SetVertexShader( FVF_VOLVERTEX );
		if (FAILED(hr))
		{
			WriteToLog("SetVertexShader() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
			m_pSrcDevice->EndScene();
			DestroyAllResInfosFromList();
			DestroyAllVolTexturesFromList();
			DestroyAllVertexBuffersFromList();
			if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
			{
				Fail();
			}
			return false;
		}

		hr = m_pSrcDevice->SetTexture(0, *startVolTex);
		if (FAILED(hr))
		{
			WriteToLog("SetTexture() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
			m_pSrcDevice->EndScene();
			DestroyAllResInfosFromList();
			DestroyAllTexturesFromList();
			DestroyAllVertexBuffersFromList();
			if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
			{
				Fail();
			}
			return false;
		}
		if (((*startResInfo)->Format).d3dfFormat == FMT_A8 )
		{
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE );
		}

		D3DXMatrixTranslation(&TranslatedM, -1+wcell/2.f, 1+hcell/2.f, 0);
		D3DXMatrixScaling(&ScaledM, wcell/2, hcell/2, 1);
		D3DXMatrixMultiply(&Matrix, &ScaledM, &TranslatedM);

		for (DWORD row = 0; row <ncell; row++)
		{
			D3DXMatrixTranslation(&TranslatedM, 2, -hcell, 0);
			D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);

			for(DWORD col = 0; col < ncell; col++)
			{
				if (row*ncell+col >= m_TotalNumberOfSlices)
					break;
				if (newLevelIndex == row*ncell+col)
				{
					if( !m_UseLODBias || (((*startResInfo)->Format).ddpfPixelFormat.dwFlags & PF_FOURCC) )
					{
						if (m_uPrevMaxMipLevel != ++LevelIndex)
						{
							m_uPrevMaxMipLevel = LevelIndex;
							SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, LevelIndex);
						}
					}
					else 
						(*startVolTex)->SetLOD(++LevelIndex);

					SliceOffset += max(1, pDesc.Depth/(1<<(LevelIndex-1)));
					newLevelIndex += max(1, pDesc.Depth/(1<<LevelIndex));
					SliceIndex = 0;
				}

				D3DXMatrixTranslation(&TranslatedM, -wcell, 0, 0);
				D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
				m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);
				
				hr = m_pSrcDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (SliceOffset+SliceIndex++)*4,  2);
				if (FAILED(hr))
				{
					WriteToLog("DrawPrimitive() failed on level %d on slice %d hr=%s (%x).\n", LevelIndex, SliceIndex-1, m_pD3D->HResultToString(hr), hr);
					(*startResInfo)->pCheckColor[SliceOffset+SliceIndex - 1] = false;

					if (D3DERR_OUTOFVIDEOMEMORY == hr)
					{
						m_pSrcDevice->GetLastError();
					}
					else
						Fail();
				}

				if ( LevelIndex+1 == (*startVolTex)->GetLevelCount() && newLevelIndex == row*ncell+col+1 )
				{
					LevelIndex = 0;
					SliceIndex = 0;
					SliceOffset = 0;
					newLevelIndex += pDesc.Depth;
					if (m_uPrevMaxMipLevel != 0)
					{
						m_uPrevMaxMipLevel = 0;
						SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
					}
					(*startVolTex)->SetLOD(0);

					if ( !ChangeVolumeColors(*startVolTex, m_uTestNumber-(m_uCommonTextureFormats+m_uCommonCubeFormats+1), (WORD)((row*ncell+col+1)/(*startResInfo)->NumSlices)) )
						Fail();
				}
			}
		}

		if (((*startResInfo)->Format).d3dfFormat == FMT_A8 )
		{
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		}

		startVolTex++;
	}

	hr = m_pSrcDevice->EndScene();
	if (FAILED(hr))
	{
		WriteToLog("EndScene() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		DestroyAllResInfosFromList();
		DestroyAllTexturesFromList();
		DestroyAllCubeMapsFromList();
		DestroyAllVolTexturesFromList();
		DestroyAllVertexBuffersFromList();
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

bool CChangeColorsTest::ProcessFrame(void)
{
	bool result = true;

	result = CheckListColors(false);
	
	DestroyAllResInfosFromList();
	DestroyAllTexturesFromList();
	DestroyAllCubeMapsFromList();
	DestroyAllVolTexturesFromList();
	DestroyAllVertexBuffersFromList();
	
	if (result)
		Pass();
	else
		Fail();
	
	return result;
}

//-----------------------------------------------------------------------------

void CChangeColorsTest::CheckChangeColorsTestCaps()
{
	if ( !m_UseTextures )
	{
		WriteToLog("ChangeColors: textures are not supposed.\n");
		if (m_uCommonTextureFormats)
			SkipTestRange( 1, m_uCommonTextureFormats );
	}
	if ( !m_UseCubeMaps )
	{
		WriteToLog("ChangeColors: cube maps are not supposed.\n");
		if (m_uCommonCubeFormats)
			SkipTestRange( m_uCommonTextureFormats + 1, m_uCommonTextureFormats + m_uCommonCubeFormats );
	}
	if ( !m_UseVolMaps )
	{
		WriteToLog("ChangeColors: volume textures are not supposed.\n");
		if (m_uCommonVolumeFormats)
			SkipTestRange( m_uCommonTextureFormats + m_uCommonCubeFormats + 1, m_uCommonTextureFormats + m_uCommonCubeFormats + m_uCommonVolumeFormats );
	}
}

//-----------------------------------------------------------------------------

bool CChangeColorsTest::ChangeTextureColors(CTexture *pTexture, DWORD texIndex, WORD count)
{
	DWORD       Stripes[7];
	DWORD       color_offset = 100;
	CImage    **ppImages     = NULL;
	bool        result       = true;
	HRESULT     hr           = S_OK;
	DWORD       color_index  = (texIndex+count)*10;
	UINT        uWidth, uHeight;
	UINT        uLevels;
	SURFACEDESC Desc;

	if (NULL == pTexture)
		return false;
	
	uLevels = pTexture->GetLevelCount();
	pTexture->GetLevelDesc(0, &Desc);
	uWidth = Desc.Width;
	uHeight = Desc.Height;

	ppImages = new CImage*[uLevels];
	if (NULL == ppImages)
	{
		WriteToLog("ChangeTextureColors: Out of memory on allocating ppImages.\n");
		SkipTestRange(m_uTestNumber, m_uTestNumber);
		return true;
	}

	ZeroMemory(ppImages, sizeof(CImage *) * uLevels);

	for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++ )
	{
		WORD index = (WORD)(color_index+LevelIndex*20);
		WORD color = (WORD)(index % (256-color_offset) + color_offset);

		if ((Desc.Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) ||
			 Desc.Format.d3dfFormat == FMT_R16F || Desc.Format.d3dfFormat == FMT_R32F ||
			 Desc.Format.d3dfFormat == FMT_A8    )
		{
			Stripes[0] = Stripes[1] = Stripes[2] = Stripes[3] = 
			Stripes[4] = Stripes[5] = Stripes[6] = RGBA_MAKE( color, color, color, color);
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

		ppImages[LevelIndex] = new CImage();
		if (NULL == ppImages[LevelIndex])
		{
			WriteToLog("ChangeTextureColors: out of memory on allocating ppImages[%d].\n", LevelIndex);
			SkipTestRange(m_uTestNumber, m_uTestNumber);
			goto Exit;
		}

		if (!ppImages[LevelIndex]->LoadStripes(uWidth, uHeight, 1, Stripes+(texIndex+count)%7, false))
		{
			WriteToLog("ChangeTextureColors: out of memory on LoadStripes() on level %d.\n", LevelIndex);
			SkipTestRange(m_uTestNumber, m_uTestNumber);
			goto Exit;
		}

		uWidth  = max( 1, uWidth >>1);
		uHeight = max( 1, uHeight>>1);
	}

	hr = pTexture->LoadImage( 0, ppImages, TEXLOAD_DEFAULT );
	if (FAILED(hr))
	{
		WriteToLog("ChangeTextureColors: LoadImage() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			SkipTestRange(m_uTestNumber, m_uTestNumber);
		}
		else
			result = false;
		goto Exit;
	}

Exit:
	if (NULL != ppImages)
	for (UINT k = 0; k < uLevels; k++)
		SAFEDELETE( ppImages[k] );
	SAFEDELETEA( ppImages );

	return result;		
}

//-----------------------------------------------------------------------------

int CChangeColorsTest::CreateTextures(CTexture *pTexture, DWORD texIndex)
{
	HRESULT     hr           = S_OK;
	CImage    **ppImages     = NULL;
	int         result       = RUN;
	DWORD       color_offset = 100;
	DWORD       Stripes[7];
	DWORD       color_index;
	UINT        uWidth, uHeight;
	UINT        uLevels;
	SURFACEDESC Desc;
	
	if (NULL == pTexture)
		return FAIL;
	
	uLevels = pTexture->GetLevelCount();
	pTexture->GetLevelDesc(0, &Desc);

	ppImages = new CImage*[uLevels];
	if (NULL == ppImages)
	{
		WriteToLog("CreateTextures: Out of memory on allocating ppImages.\n");
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	ZeroMemory(ppImages, sizeof(CImage *) * uLevels);

	for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++ )
	{
		ppImages[LevelIndex] = new CImage();
		if (NULL == ppImages[LevelIndex])
		{
			WriteToLog("CreateTextures: Out of memory on allocating ppImages[%d].\n", LevelIndex);
			hr = E_OUTOFMEMORY;
			goto Exit;
		}
	}

	for (UINT count = 0; count < m_PrimCount; count++ )
	{
		CTexture  *Tex;
		color_index = (texIndex+count)*10;
		uWidth = Desc.dwWidth;
		uHeight = Desc.dwHeight;

		hr = m_pSrcDevice->CreateTexture(Desc.dwWidth, Desc.dwHeight, uLevels, 0, Desc.Format, POOL_MANAGED, &Tex);
		if (FAILED(hr))
		{
			WriteToLog("CreateTextures: CreateTexture() failed on %d texture hr=%s(%x).\n", count, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		if(Tex->IsPalettized())
			Tex->SetPalette(NULL);

		for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++ )
		{
			WORD index = (WORD)(color_index+LevelIndex*20);
			WORD color = (WORD)(index % (256-color_offset) + color_offset);

			if ((Desc.Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) ||
				 Desc.Format.d3dfFormat == FMT_R16F || Desc.Format.d3dfFormat == FMT_R32F ||
				 Desc.Format.d3dfFormat == FMT_A8    )
			{
				Stripes[0] = Stripes[1] = Stripes[2] = Stripes[3] = 
				Stripes[4] = Stripes[5] = Stripes[6] = RGBA_MAKE( color, color, color, color);
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

			if (!ppImages[LevelIndex]->LoadStripes(uWidth, uHeight, 1, Stripes+(texIndex+count)%7, false))
			{
				WriteToLog("CreateTextures: out of memory on LoadStripes() on %d texture on level %d\n", count, LevelIndex);
				hr = E_OUTOFMEMORY;
				goto Exit;
			}

			uWidth  = max( 1, uWidth >>1);
			uHeight = max( 1, uHeight>>1);
		}

		hr = Tex->LoadImage( 0, ppImages, TEXLOAD_DEFAULT );
		if (FAILED(hr))
		{
			WriteToLog("CreateTextures: LoadImage() failed on %d texture hr=%s(%x).\n", count, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
		m_TexturesList.insert(m_TexturesList.end(), Tex);
    }

Exit:
	if (FAILED(hr))
	if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
	{
		m_pSrcDevice->GetLastError();
		result = SKIP;
	}
	else
	{
		Fail();
		result = FAIL;
	}

	if (NULL != ppImages)
	for (UINT k = 0; k < uLevels; k++)
			SAFEDELETE( ppImages[k] );
		SAFEDELETEA( ppImages );

	return result;
}

//-----------------------------------------------------------------------------

bool CChangeColorsTest::ChangeCubeMapColors(CCubeTexture *pCubeTexture, DWORD texIndex, WORD count)
{
	DWORD       Stripes[6];
	DWORD       color_offset = 100;
	WORD        color_index  = (WORD)(texIndex+count)*20;
	CImage   ***pppImages    = NULL;
	bool        result       = true;
	HRESULT     hr           = S_OK;
	UINT        uWidth;
	UINT        uLevels;
	SURFACEDESC Desc;

	if (NULL == pCubeTexture)
		return false;
	
	uLevels = pCubeTexture->GetLevelCount();
	pCubeTexture->GetLevelDesc(0, &Desc);
	uWidth = Desc.Width;

	pppImages = new CImage**[uLevels];
	if (NULL == pppImages)
	{
		WriteToLog("ChangeCubeColors: out of memory on allocating pppImages.\n");
		SkipTestRange(m_uTestNumber, m_uTestNumber);
		return true;
	}

	ZeroMemory(pppImages, sizeof(CImage **) * uLevels);

	for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++)
	{
		WORD index = color_index+LevelIndex*20;
		WORD color = (WORD)(index % (256-color_offset) + color_offset);

		if ((Desc.Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) ||
			 Desc.Format.d3dfFormat == FMT_R16F || Desc.Format.d3dfFormat == FMT_R32F ||
			 Desc.Format.d3dfFormat == FMT_A8 )
		{
			for (int i = 0; i < 6; i++)
			{
				index = color_index+LevelIndex*20+i*20;
				color = (WORD)(index % (256-color_offset) + color_offset);
				Stripes[i] = RGBA_MAKE( color, color, color, color); 
			}
		}
		else
		{
			Stripes[0] = RGBA_MAKE(color, 0, 0, 255); 
			Stripes[1] = RGBA_MAKE(0, color, 0, 255); 
			Stripes[2] = RGBA_MAKE(0, 0, color, 255); 
			Stripes[3] = RGBA_MAKE(0, color, color, 255); 
			Stripes[4] = RGBA_MAKE(color, 0, color, 255); 
			Stripes[5] = RGBA_MAKE(color, color, 0, 255); 
		}

		pppImages[LevelIndex] = new CImage*[6];
		if (NULL == pppImages[LevelIndex])
		{
			WriteToLog("ChangeCubeMapColors: out of memory on allocating pppImages[%d].\n", LevelIndex);
			SkipTestRange(m_uTestNumber, m_uTestNumber);
			goto Exit;
		}

		ZeroMemory(pppImages[LevelIndex], sizeof(CImage *) * 6);

		for (UINT FaceIndex = 0; FaceIndex < 6; FaceIndex++)
		{
			pppImages[LevelIndex][FaceIndex] = new CImage();
			if (NULL == pppImages[LevelIndex][FaceIndex])
			{
				WriteToLog("ChangeCubeMapColors: out of memory on allocating pppImages[%d][%d].\n", LevelIndex, FaceIndex);
				SkipTestRange(m_uTestNumber, m_uTestNumber);
				goto Exit;
			}

			if (!pppImages[LevelIndex][FaceIndex]->LoadStripes(uWidth, uWidth, 1, Stripes+(FaceIndex+texIndex+count)%6, false))
			{
				WriteToLog("ChangeCubeMapColors: out of memory on LoadStripes() on level %d on face %d.\n", LevelIndex, FaceIndex);
				SkipTestRange(m_uTestNumber, m_uTestNumber);
				goto Exit;
			}
		}

		uWidth >>= 1;
	}

	hr = pCubeTexture->LoadImage(pppImages, TEXLOAD_DEFAULT);
	if (FAILED(hr))
	{
		WriteToLog("ChangeCubeMapColors: LoadImage() failed hr=%s(%x)\n", m_pD3D->HResultToString(hr), hr);
		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			SkipTestRange(m_uTestNumber, m_uTestNumber);
		}
		else
			result = false;
	}

Exit:
	if (NULL != pppImages)
	for (UINT k = 0; k < uLevels; k++)
	{
		if (NULL != pppImages[k])
		for (UINT j = 0; j < 6; j++)
			SAFEDELETE(pppImages[k][j]);
		SAFEDELETEA(pppImages[k]);
	}
	SAFEDELETEA(pppImages);

	return result;
}

//-----------------------------------------------------------------------------

int CChangeColorsTest::CreateCubeTextures( CCubeTexture *pCubeTexture, DWORD texIndex )
{
	DWORD       color_offset = 100;
	DWORD       Stripes[6];
	WORD        color_index;
	int         result    = RUN;
	HRESULT     hr        = S_OK;
	CImage   ***pppImages = NULL;
	SURFACEDESC Desc;
	UINT        uWidth;
	UINT        uLevels;

	if (NULL == pCubeTexture)
		return FAIL;

	uLevels = pCubeTexture->GetLevelCount();
	pCubeTexture->GetLevelDesc(0, &Desc);

	pppImages = new CImage**[uLevels];
	
	if (NULL == pppImages)
	{
		WriteToLog("CreateCubeTextures: out of memory on allocating pppImages.\n");
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	ZeroMemory(pppImages, sizeof(CImage **) * uLevels);

	for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++ )
	{
		pppImages[LevelIndex] = new CImage*[6];
		
		if (NULL == pppImages[LevelIndex])
		{
			WriteToLog("CreateCubeTextures: out of memory on allocating pppImages[%d].\n", LevelIndex);
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		ZeroMemory(pppImages[LevelIndex], sizeof(CImage *) * 6);

		for (UINT FaceIndex = 0; FaceIndex < 6; FaceIndex++)
		{
			pppImages[LevelIndex][FaceIndex] = new CImage();
			if (NULL == pppImages[LevelIndex][FaceIndex])
			{
				WriteToLog("CreateCubeTextures: out of memory on allocating pppImages[%d][%d].\n", LevelIndex, FaceIndex);
				hr = E_OUTOFMEMORY;
				goto Exit;
			}
		}
	}

	for (UINT count = 0; count < m_PrimCount; count++)
	{
		CCubeTexture *CubeTex;
		uWidth = Desc.Width;
		color_index = (WORD)(texIndex+count)*20;

		hr = m_pSrcDevice->CreateCubeTexture( uWidth, uLevels, 0, Desc.Format, POOL_MANAGED, &CubeTex );
		if (FAILED(hr))
		{
			WriteToLog("CreateCubeTextures: CreateCubeTexture() failed on %d texture hr=%s(%x).\n", count, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		if(CubeTex->IsPalettized())
			CubeTex->SetPalette(NULL);

		for ( int LevelIndex = 0; LevelIndex < uLevels; LevelIndex++ )
		{
			WORD index = color_index+LevelIndex*20;
			WORD color = (WORD)(index % (256-color_offset) + color_offset);

			if ((Desc.Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) ||
				 Desc.Format.d3dfFormat == FMT_R16F || Desc.Format.d3dfFormat == FMT_R32F ||
				 Desc.Format.d3dfFormat == FMT_A8    )
			{
				for (int i = 0; i < 6; i++)
				{
					index = color_index+LevelIndex*20+i*20;
					color = (WORD)(index % (256-color_offset) + color_offset);
					Stripes[i] = RGBA_MAKE( color, color, color, color); 
				}
			}
			else
			{
				Stripes[0] = RGBA_MAKE(color, 0, 0, 255); 
				Stripes[1] = RGBA_MAKE(0, color, 0, 255); 
				Stripes[2] = RGBA_MAKE(0, 0, color, 255); 
				Stripes[3] = RGBA_MAKE(0, color, color, 255); 
				Stripes[4] = RGBA_MAKE(color, 0, color, 255); 
				Stripes[5] = RGBA_MAKE(color, color, 0, 255); 
			}

			for ( int FaceIndex = 0; FaceIndex < 6; FaceIndex++)
			{
				if (!pppImages[LevelIndex][FaceIndex]->LoadStripes(uWidth, uWidth, 1, Stripes+(FaceIndex+texIndex+count)%6, false))
				{
					WriteToLog("CreateCubeTextures: out of memory on LoadStripes() on %d cube map on level %d and face %d.\n", count, LevelIndex, FaceIndex);
					hr = E_OUTOFMEMORY;
					goto Exit;
				}
			}

			uWidth >>= 1;
		}

		hr = CubeTex->LoadImage(pppImages, TEXLOAD_DEFAULT);
		if (FAILED(hr))
		{
			WriteToLog("CreateCubeTextures::LoadImage() failed on %d cube map hr=%s(%x).\n", count, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		m_CubesList.insert(m_CubesList.end(), CubeTex);
	}

Exit:
	if (FAILED(hr))
	if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
	{
		m_pSrcDevice->GetLastError();
		result = SKIP;
	}
	else
	{
		Fail();
		result = FAIL;
	}

	if (NULL != pppImages)
	for (UINT k = 0; k < uLevels; k++)
	{
		if (NULL != pppImages[k])
		for (UINT j = 0; j < 6; j++)
			SAFEDELETE(pppImages[k][j]);
		SAFEDELETEA(pppImages[k]);
	}
	SAFEDELETEA(pppImages);

	return result;
}

//-----------------------------------------------------------------------------

bool CChangeColorsTest::ChangeVolumeColors(CVolTexture *pVolTexture, DWORD texIndex, WORD count) 
{
	bool        result       = true;
	HRESULT     hr           = S_OK;
	DWORD       color_offset = 100;
	DWORD       color_index  = (texIndex+count)*20;
	CImage   ***pppImages    = NULL;
	UINT        uWidth, uHeight, uDepth;
	UINT        uLevels;
	DWORD       Stripe[1];
	VOLUME_DESC Desc;
	
	if (NULL == pVolTexture)
		return false;

	uLevels = pVolTexture->GetLevelCount();
	pVolTexture->GetLevelDesc(0, &Desc);
	uWidth = Desc.Width;
	uHeight = Desc.Height;
	uDepth = Desc.Depth;

	pppImages = new CImage**[uLevels];
	if (NULL == pppImages)
	{
		WriteToLog("ChangeVolumeColors: out of memory on allocating pppImages.\n");
		SkipTestRange(m_uTestNumber, m_uTestNumber);
		goto Exit;
	}

	ZeroMemory(pppImages, sizeof(CImage **) * uLevels);

    for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++)
	{
		DWORD temp = (texIndex+LevelIndex+count)%7;

		pppImages[LevelIndex] = new CImage*[uDepth];
		if (NULL == pppImages[LevelIndex])
		{
			WriteToLog("ChangeVolumesColors: out of memory on allocating pppImages[%d].\n", LevelIndex);
			SkipTestRange(m_uTestNumber, m_uTestNumber);
			goto Exit;
		}

		ZeroMemory(pppImages[LevelIndex], sizeof(CImage *) * uDepth);
   
		for (DWORD SliceIndex = 0; SliceIndex < uDepth; SliceIndex++)
		{
			WORD index = (WORD)(color_index+SliceIndex*20);
			WORD color = (WORD)(index % (256-color_offset) + color_offset);

			if ((Desc.Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) ||
				Desc.Format.d3dfFormat == FMT_R16F || Desc.Format.d3dfFormat == FMT_R32F ||
				Desc.Format.d3dfFormat == FMT_A8    )
			{
				index = (WORD)(color_index+SliceIndex*20+temp*20);
				color = (WORD)(index % (256-color_offset) + color_offset);
				Stripe[0] = RGBA_MAKE( color, color, color, color);
			}
			else
			{
				WORD   red = temp%2 == 0 ? color : 0;
				WORD green = (temp+1)%4 >= 2 ? color : 0;
				WORD  blue = temp/3 != 0 ? color : 0;
				Stripe[0] = RGBA_MAKE(red, green, blue, 255);
			}

			pppImages[LevelIndex][SliceIndex] = new CImage();
			if (NULL == pppImages[LevelIndex][SliceIndex])
			{
				WriteToLog("ChangeVolumesColors: out of memory on allocating pppImages[%d][%d].\n", LevelIndex, SliceIndex);
				SkipTestRange(m_uTestNumber, m_uTestNumber);
				goto Exit;
			}

			if (!pppImages[LevelIndex][SliceIndex]->LoadStripes(uWidth, uHeight, 1, Stripe, false))
			{
				WriteToLog("ChangeVolumesColors: out of memory on LoadStripes() on level %d on slice %d.\n", LevelIndex, SliceIndex);
				SkipTestRange(m_uTestNumber, m_uTestNumber);
				goto Exit;
			}
		}

		uWidth  = max( 1, uWidth >>1 );
		uHeight = max( 1, uHeight>>1 );
		uDepth  = max( 1, uDepth >>1 );
	}

	hr = pVolTexture->LoadImage(pppImages, TEXLOAD_DEFAULT);
	if (FAILED(hr))
	{
		WriteToLog("ChangeVolumesColors: LoadImage() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			SkipTestRange(m_uTestNumber, m_uTestNumber);
		}
		else
			result = false;
		goto Exit;
	}

Exit:
	uDepth = Desc.Depth;
	if (NULL != pppImages)
	for (UINT k = 0; k < uLevels; k++)
	{
		if (NULL != pppImages[k])
		for (UINT j=0; j<uDepth; j++)
			SAFEDELETE(pppImages[k][j]);
		SAFEDELETEA(pppImages[k]);
		uDepth = max( 1, uDepth>>1 );
	}
	SAFEDELETEA(pppImages);
	
	return result;
}	

//-----------------------------------------------------------------------------

int CChangeColorsTest::CreateVolTextures( CVolTexture *pVolTexture, DWORD texIndex ) 
{
	DWORD       color_offset = 100;
	CImage   ***pppImages    = NULL;
	int         result       = RUN;
	HRESULT     hr           = S_OK;
	DWORD       color_index;
	DWORD       Stripe[1];
	UINT        uLevels;
	UINT        uWidth, uHeight, uDepth;
	VOLUME_DESC Desc;
	

	if (NULL == pVolTexture)
		return FAIL;
	 
	uLevels = pVolTexture->GetLevelCount();
	pVolTexture->GetLevelDesc(0, &Desc);

	pppImages = new CImage**[uLevels];
	
	if (NULL == pppImages)
	{
		WriteToLog("CreateVolTextures: out of memory on allocating pppImages.\n");
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	ZeroMemory(pppImages, sizeof(CImage **) * uLevels);

	uDepth = Desc.Depth;

	for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++)
	{
		pppImages[LevelIndex] = new CImage*[uDepth];

		if (NULL == pppImages[LevelIndex])
		{
			WriteToLog("CreateVolTextures: out of memory on allocating pppImages[%d].\n", LevelIndex);
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		ZeroMemory(pppImages[LevelIndex], sizeof(CImage *) * uDepth);

		for (UINT SliceIndex = 0; SliceIndex < uDepth; SliceIndex++)
		{
			pppImages[LevelIndex][SliceIndex] = new CImage();

			if (NULL == pppImages[LevelIndex][SliceIndex])
			{
				WriteToLog("CreateVolTextures: out of memory on allocating pppImages[%d][%d].\n", LevelIndex, SliceIndex);
				hr = E_OUTOFMEMORY;
				goto Exit;
			}
		}

		uDepth  = max( 1, uDepth >>1 );
	}
	
	for (UINT count = 0; count < m_PrimCount; count++ )
	{
		CVolTexture *VolTex;
		uWidth = Desc.Width;
		uHeight = Desc.Height;
		uDepth = Desc.Depth;
		color_index = (texIndex+count)*20;

		hr = m_pSrcDevice->CreateVolumeTexture( uWidth, uHeight, uDepth, uLevels, 0, Desc.Format, POOL_MANAGED, &VolTex );
		if (FAILED(hr))
		{
			WriteToLog("CreateVolTextures: CreateVolumeTexture() failed on volume %d hr=%s(%x).\n", count, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		if(VolTex->IsPalettized())
			VolTex->SetPalette(NULL);

		for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++)
		{
			DWORD temp = (texIndex+LevelIndex+count)%7;

			for (UINT SliceIndex = 0; SliceIndex < uDepth; SliceIndex++)
			{
				WORD index = (WORD)(color_index+SliceIndex*20);
				WORD color = (WORD)(index % (256-color_offset) + color_offset);
				
				if ((Desc.Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) ||
					 Desc.Format.d3dfFormat == FMT_R16F || Desc.Format.d3dfFormat == FMT_R32F ||
					 Desc.Format.d3dfFormat == FMT_A8    )
				{
					index = (WORD)(color_index+SliceIndex*20+temp*20);
					color = (WORD)(index % (256-color_offset) + color_offset);
					Stripe[0] = RGBA_MAKE( color, color, color, color);
				}
				else
				{
					WORD   red = temp%2 == 0 ? color : 0;
					WORD green = (temp+1)%4 >= 2 ? color : 0;
					WORD  blue = temp/3 != 0 ? color : 0;
					Stripe[0] = RGBA_MAKE(red, green, blue, 255);
				}

				if (!pppImages[LevelIndex][SliceIndex]->LoadStripes(uWidth, uHeight, 1, Stripe, false))
				{
					WriteToLog("CreateVolTextures: out of memory on LoadStripes() on %d texture on level %d and slice %d.\n", count, LevelIndex, SliceIndex);
					hr = E_OUTOFMEMORY;
					goto Exit;
				}
			}

			uWidth  = max( 1, uWidth >>1 );
			uHeight = max( 1, uHeight>>1 );
			uDepth  = max( 1, uDepth >>1 );
		}

		hr = VolTex->LoadImage(pppImages, TEXLOAD_DEFAULT);
		if (FAILED(hr))
		{
			WriteToLog("ChangeVolumesColors: LoadImage() failed on %d volume hr=%s(%x).\n", count, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		m_VolumesList.insert(m_VolumesList.end(), VolTex);
	}

Exit:
	if (FAILED(hr))
	if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
	{
		m_pSrcDevice->GetLastError();
		result = SKIP;
	}
	else
	{
		Fail();
		result = FAIL;
	}

	uDepth = Desc.Depth;
	if (NULL != pppImages)
	for (UINT k = 0; k < uLevels; k++)
	{
		if (NULL != pppImages[k])
		for (UINT j=0; j<uDepth; j++)
			SAFEDELETE(pppImages[k][j]);
		SAFEDELETEA(pppImages[k]);
		uDepth = max( 1, uDepth>>1 );
	}
	SAFEDELETEA(pppImages);
	
	return result;
}	

//-----------------------------------------------------------------------------

bool CChangeColorsTest::TestTerminate()
{
	Cleanup();

	if (m_input)
		fclose( m_input);
	if (m_output)
		fclose(m_output);

	DestroyAllResInfosFromList();
	DestroyAllVertexBuffersFromList();
	DestroyAllTexturesFromList();
	DestroyAllCubeMapsFromList();
	DestroyAllVolTexturesFromList();

	return true;
}

