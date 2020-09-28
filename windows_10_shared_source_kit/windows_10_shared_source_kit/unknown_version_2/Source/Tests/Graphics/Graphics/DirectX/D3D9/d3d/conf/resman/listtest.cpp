#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "ResMan.h"

//-----------------------------------------------------------------------------

void ListTest::UpdateStatus()
{
	CD3DTest::UpdateStatus();
	WriteMessage(1, m_Message);
}

//-----------------------------------------------------------------------------

void ListTest::CommandLineHelp(void)
{
	ResMan::CommandLineHelp();

	WriteCommandLineHelp(_T("$w\toutput     \t$gOutput data to file."));
    WriteCommandLineHelp(_T("$w\tinput       \t$gInput data from file."));
}

//-----------------------------------------------------------------------------

UINT ListTest::FillVideoMemory()
{
    DWORD width = m_pSrcDevice->GetCaps()->dwMaxTextureWidth;
    DWORD height = m_pSrcDevice->GetCaps()->dwMaxTextureHeight;
    HRESULT hr;
    bool go = true;
    UINT totalMemory = 0, curMem = 0;
    CTexture *tempT = NULL;
    FORMAT format;
    format = m_pCommonTextureFormats[0];
    for (int i = 1; i < m_uCommonTextureFormats; i++)
        if (m_pCommonTextureFormats[i].ddpfPixelFormat.dwRGBBitCount > format.ddpfPixelFormat.dwRGBBitCount)
            format = m_pCommonTextureFormats[i];
    totalMemory = GetAvailableTextureMem();

#if !_WIN64
	{
		const UINT VIDMEM_CLAMP_2GB = ((UINT)((((ULONGLONG)2) * 1024 * 1024 * 1024) - (64 * 1024)));
		if (totalMemory > VIDMEM_CLAMP_2GB)
			totalMemory = VIDMEM_CLAMP_2GB;
	}
#endif

	if (KeySet("showmem"))
		WriteToLog("FillVideoMemory: initial memory=%u.\n", totalMemory);
	while(go)
	{
		if ((format.ddpfPixelFormat.dwRGBBitCount/8)*width*height + curMem <= totalMemory/2 && width > 256)
		{
			hr = m_pSrcDevice->CreateTexture(width, height, 1, 0, format, POOL_DEFAULT, &tempT);
			{
				if (FAILED(hr)) 
				{
					if (width <= 256)
						go = false;
					width >>= 1;
					height >>= 1;
					hr = m_pSrcDevice->GetLastError();
				}
				else
				{
					curMem += (format.ddpfPixelFormat.dwRGBBitCount/8)*width*height;
					m_DefaultTexList.insert(m_DefaultTexList.begin(), tempT);
				}
			}
		}
		else
		{
			if (width <= 256)
				go = false;
			width >>= 1;
			height >>= 1;
		}
	}
	if (KeySet("showmem"))
		WriteToLog("FillVideoMemory: occupied memory=%u.\n", curMem);
	return curMem;
}

//-----------------------------------------------------------------------------

HRESULT ListTest::DrawFunction(bool add1st)
{
	RESINFOLIST::iterator ri;
	VBUFFERLIST::iterator vb;
	IBUFFERLIST::iterator ib;
	TEXTURELIST::iterator t;
	CUBEMAPLIST::iterator cm;
	VOLTEXLIST::iterator vt;
	ri = startResInfo;
	vb = startVBuffer;
	ib = startIBuffer;
	t = startTexture;
	cm = startCubeMap;
	vt = startVolTex;

	bool newPrimitive = true;
	bool draw = true;
	int PrimIndex = -1;
	DWORD cellIndex = 0;
	DWORD ncell = (DWORD)ceil(sqrt((float)m_TotalNumberOfSlices));
	FLOAT wcell = 2.f/ncell, hcell = 2.f/ncell; 
	DWORD numOfSlices = 0, SliceIndex = 0, SliceOffset = 0;
	DWORD LevelIndex = 0, LevelCount = 0;
	TCHAR szBuffer[30];
	VOLUME_DESC VolDesc;
	HRESULT hr;

	D3DXMATRIX	Matrix, ScaledM, TranslatedM;
	
	D3DXMatrixTranslation(&TranslatedM, -1+wcell/2.f, 1+hcell/2.f, 0);
	D3DXMatrixScaling(&ScaledM, wcell/2, hcell/2, 1);
	D3DXMatrixMultiply(&Matrix, &ScaledM, &TranslatedM);

	while ( cellIndex < m_TotalNumberOfSlices )
	{
		if (newPrimitive)
		{
			newPrimitive = false;
			PrimIndex++;

			if (ri == m_ResInfosList.end())
				ri = m_ResInfosList.begin();
			else if (add1st)
				ri++;

			if ((*ri)->Type == IS_NULL)
			{
				draw = false;
				newPrimitive = true;
				if (!add1st)
					ri++;
			}
			else
			{
				if (vb == m_VBuffersList.end())
					vb = m_VBuffersList.begin();
				else if (add1st)
					vb++;

				FmtToString(FORMAT_TEXTURE, &((*ri)->Format), szBuffer);

				switch( (*ri)->Type )
				{
					case IS_16INDEXBUFFER: case IS_32INDEXBUFFER:
					{
						if (ib == m_IBuffersList.end())
							ib = m_IBuffersList.begin();
						else if (add1st)
							ib++;

						hr = m_pSrcDevice->SetTexture(0, NULL);
						
						if (*ib == NULL)
						{
							WriteToLog("(%d %s IB) index buffer is NULL (row %d, column %d).\n", PrimIndex, szBuffer, cellIndex/ncell, cellIndex%ncell);
							newPrimitive = true;
							draw = false;
							if (!add1st)
							{ ri++; vb++; ib++; }
						}
						else if (*vb == NULL)
						{
							WriteToLog("(%d %s IB) vertex buffer is NULL (row %d, column %d).\n", PrimIndex, szBuffer, cellIndex/ncell, cellIndex%ncell);
							newPrimitive = true;
							draw = false;
							if (!add1st)
							{ ri++; vb++; ib++; }
						}
						break;
					}
					case IS_TLVERTEXBUFFER:  case IS_LVERTEXBUFFER: case IS_VERTEXBUFFER:
					{
						hr = m_pSrcDevice->SetTexture(0, NULL);

						if (*vb == NULL)
						{
							WriteToLog("%d VB) vertex buffer is NULL (row %d, column %d).\n", PrimIndex, cellIndex/ncell, cellIndex%ncell);
							newPrimitive = true;
							newPrimitive = true;
							draw = false;
							if (!add1st)
							{ ri++; vb++; }
						}
						else if ( (*ri)->Type == IS_LVERTEXBUFFER )
						{
							hr = m_pSrcDevice->SetRenderState(D3DRS_LIGHTING, (DWORD)FALSE);
							if (FAILED(hr))
							{
								WriteToLog("(%d LVB) SetRenderState(LIGHTING, FALSE) failed hr=%s(%x) (row %d, column %d).\n", PrimIndex, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
								Fail();
							}
						}
						/*else if ( (*ri)->Type == IS_TLVERTEXBUFFER )
						{
							if (FAIL == ChangeTLVertexBufferInList(vb, cellIndex/ncell, cellIndex%ncell, hcell, wcell))
							{
								WriteToLog("Can not change %d tl vertex buffer info\n", PrimIndex);
								Fail();
								newPrimitive = true;
								newPrimitive = true;
								draw = false;
								if (!add1st)
								{ ri++; vb++; }
							}
						}*/
						break;
					}
					case IS_TEXTURE:
					{
						if (t == m_TexturesList.end())
							t = m_TexturesList.begin();
						else if (add1st)
							t++;

						if (*t == NULL)
						{
							WriteToLog("(%d %s T) texture is NULL (row %d, column %d).\n", PrimIndex, szBuffer, cellIndex/ncell, cellIndex%ncell);
							newPrimitive = true;
							draw = false;
							if (!add1st)
							{ ri++; vb++; t++; }
						}
						else if (*vb == NULL)
						{
							WriteToLog("(%d %s T) vertex buffer is NULL (row %d, column %d).\n", PrimIndex, szBuffer, cellIndex/ncell, cellIndex%ncell);
							newPrimitive = true;
							draw = false;
							if (!add1st)
							{ ri++; vb++; t++; }
						}
						else
						{
							if ( ((*ri)->Format).d3dfFormat == FMT_A8 )
							{
								SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
								SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE );
							}

							if (m_uPrevMaxMipLevel != 0)
							{
								m_uPrevMaxMipLevel = 0;
								SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
							}
							LevelCount = (*ri)->NumSlices;
							if (LevelCount > 1)	(*t)->SetLOD(0);

							hr = m_pSrcDevice->SetTexture(0, *t);
							if (FAILED(hr))
							{
								WriteToLog("(%d %s T) SetTexture() failed hr=%s(%x) (row %d, column %d).\n", PrimIndex, szBuffer, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
								Fail();
							}
						}
						break;
					}
					case IS_CUBEMAP:
					{
						if (cm == m_CubesList.end())
							cm = m_CubesList.begin();
						else if (add1st)
							cm++;

						if (*cm == NULL)
						{
							WriteToLog("(%d %s CM) cube map is NULL (row %d, column %d).\n", PrimIndex, szBuffer, cellIndex/ncell, cellIndex%ncell);
							newPrimitive = true;
							draw = false;
							if (!add1st)
							{ ri++; vb++; cm++; }
						}
						else if (*vb == NULL)
						{
							WriteToLog("(%d %s CM) vertex buffer is NULL (row %d, column %d).\n", PrimIndex, szBuffer, cellIndex/ncell, cellIndex%ncell);
							newPrimitive = true;
							draw = false;
							if (!add1st)
							{ ri++; vb++; cm++; }
						}
						else
						{
							if (((*ri)->Format).d3dfFormat == FMT_A8)
							{
								SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
								SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE );
							}

							if (m_uPrevMaxMipLevel != 0)
							{
								m_uPrevMaxMipLevel = 0;
								SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
							}
							LevelCount = (*cm)->GetLevelCount();
							if (LevelCount > 1)	(*cm)->SetLOD(0);

							hr = m_pSrcDevice->SetTexture(0, *cm);
							if (FAILED(hr))
							{
								WriteToLog("(%d %s CM) SetTexture() failed hr=%s(%x) (row %d, column %d).\n", PrimIndex, szBuffer, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
								Fail();
							}
						}
						break;
					}
					case IS_VOLUME:
					{
						if (vt == m_VolumesList.end())
							vt = m_VolumesList.begin();
						else if (add1st)
							vt++;

						if (*vt == NULL)
						{
							WriteToLog("(%d %s VOL) volume texture is NULL (row %d, column %d).\n", PrimIndex, szBuffer, cellIndex/ncell, cellIndex%ncell);
							newPrimitive = true;
							draw = false;
							if (!add1st)
							{ ri++; vb++; vt++; }
						}
						else if (*vb == NULL)
						{
							WriteToLog("(%d %s VOL) vertex buffer is NULL (row %d, column %d).\n", PrimIndex, szBuffer, cellIndex/ncell, cellIndex%ncell);
							newPrimitive = true;
							draw = false;
							if (!add1st)
							{ ri++; vb++; vt++; }
						}
						else
						{
							if (((*ri)->Format).d3dfFormat == FMT_A8)
							{
								SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
								SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE );
							}

							if (m_uPrevMaxMipLevel != 0)
							{
								m_uPrevMaxMipLevel = 0;
								SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
							}
							LevelCount = (*vt)->GetLevelCount();
							(*vt)->SetLOD(0);

							hr = m_pSrcDevice->SetTexture(0, *vt);
							if (FAILED(hr))
							{
								WriteToLog("(%d %s VOL) SetTexture() failed hr=%s(%x) (row %d, column %d).\n", PrimIndex, szBuffer, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
								Fail();
							}

							hr = m_pSrcDevice->SetStreamSource( 0, *vb, GetVertexSize(FVF_VOLVERTEX) );
							if (FAILED(hr))
							{
								WriteToLog("(%d %s VOL) SetStreamSource() failed hr=%s(%x) (row %d, column %d).\n", PrimIndex, szBuffer, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
								Fail();
							}

							hr = m_pSrcDevice->SetVertexShader( FVF_VOLVERTEX );
							if (FAILED(hr))
							{
								WriteToLog("(%d %s VOL) SetVertexShader() failed hr=%s(%x) (row %d, column %d).\n", PrimIndex, szBuffer, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
								Fail();
							}

							(*vt)->GetLevelDesc(0, &VolDesc);
							numOfSlices = VolDesc.Depth;
							SliceOffset = 0;
						}
						break;
					}
				}
			}
		}
		if (draw)
		{
			if (cellIndex%ncell == 0)
			{
				D3DXMatrixTranslation(&TranslatedM, 2, -hcell, 0);
				D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
			}
			D3DXMatrixTranslation(&TranslatedM, -wcell, 0, 0);
			D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);

			m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);

			switch ( (*ri)->Type )
			{
				case IS_16INDEXBUFFER: case IS_32INDEXBUFFER:
				{
					DWORD NumberOfVertices = (*vb)->GetSize()/sizeof(VERTEX);
					m_pSrcDevice->SetVertexShader(FVF_VERTEX);

					hr = m_pSrcDevice->SetStreamSource( 0, *vb, GetVertexSize(FVF_VERTEX) );
					if (FAILED(hr))
					{
						WriteToLog("(%d %s IB) SetStreamSource() failed hr=%s(%x) (row %d, column %d).\n", PrimIndex, szBuffer, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
						Fail();
					}

					hr = m_pSrcDevice->SetIndices( *ib, 0 );
					if (FAILED(hr))
					{
						WriteToLog("(%d %s IB) SetIndices() failed hr=%s(%x) (row %d, column %d).\n", PrimIndex, szBuffer, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
						Fail();
					}

					hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, NumberOfVertices, 0, NumberOfVertices-2);
					if (FAILED(hr))
					{
						WriteToLog("(%d %s IB) DrawIndexedPrimitive() failed hr=%s(%x) (row %d, column %d).\n", PrimIndex, szBuffer, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
						(*ri)->pCheckColor[0] = false;

						if (D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
						{
							m_pSrcDevice->GetLastError();
                            Skip();
                            return hr;
						}
						else
							Fail();
					}

					newPrimitive = true;

					if (!add1st)
					{ ri++; vb++; ib++; }
					break;
				}
				case IS_TLVERTEXBUFFER:
				{
					if (PASS != ChangeTLVertexBufferInList(vb, (WORD)(cellIndex/ncell), (WORD)(cellIndex%ncell), (FLOAT)ncell/*hcell*/, (FLOAT)wcell))
					{
						WriteToLog("Can not change %d tl vertex buffer info\n", PrimIndex);
						Fail();
						newPrimitive = true;

						if (!add1st)
						{ ri++; vb++; }
						break;
					}

					DWORD NumberOfVertices = (*vb)->GetSize()/sizeof(D3DTLVERTEX);
					
					if (!m_pSrcDevice->DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP, *vb, 
											0, NumberOfVertices, NULL, 0, 0, 0, POOL_MANAGED, false))
					{
						hr = m_pSrcDevice->GetLastError();
						
						WriteToLog("(%d TLVB) DrawPrimitiveVertexBuffer() failed hr=%s(%x) (row %d, column %d).\n", PrimIndex, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
						(*ri)->pCheckColor[0] = false;

						if (D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
						{
							m_pSrcDevice->GetLastError();
                            Skip();
                            return hr;
						}
						else
							Fail();
					}

					newPrimitive = true;

					if (!add1st)
					{ ri++; vb++; }
					break;
				}
				case IS_LVERTEXBUFFER: 
				{
					DWORD NumberOfVertices = (*vb)->GetSize()/sizeof(LVERTEX);
					
					if (!m_pSrcDevice->DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP, *vb, 
											0, NumberOfVertices, NULL, 0, 0, 0, POOL_MANAGED, false))
					{
						hr = m_pSrcDevice->GetLastError();
						
						WriteToLog("(%d LVB) DrawPrimitiveVertexBuffer() failed hr=%s(%x) (row %d, column %d).\n", PrimIndex, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
						(*ri)->pCheckColor[0] = false;

						if (D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
						{
							m_pSrcDevice->GetLastError();
                            Skip();
                            return hr;
						}
						else
							Fail();
					}

					hr = m_pSrcDevice->SetRenderState(D3DRS_LIGHTING, (DWORD)TRUE);
					if (FAILED(hr))
					{
						WriteToLog("(%d LVB) SetRenderState(LIGHTING, TRUE) failed hr=%s(%x) (row %d, column %d).\n", PrimIndex, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
						Fail();
					}

					newPrimitive = true;

					if (!add1st)
					{ ri++; vb++; }
					break;
				}
				case IS_VERTEXBUFFER:
				{
					DWORD NumberOfVertices = (*vb)->GetSize()/sizeof(VERTEX);
					
					if (!m_pSrcDevice->DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP, *vb, 
										0, NumberOfVertices, NULL, 0, 0, 0, POOL_MANAGED, false))
					{
						hr = m_pSrcDevice->GetLastError();
						
						WriteToLog("(%d VB) DrawPrimitiveVertexBuffer() failed hr=%s(%x) (row %d, column %d).\n", PrimIndex, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
						(*ri)->pCheckColor[0] = false;

						if (D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
						{
							m_pSrcDevice->GetLastError();
                            Skip();
                            return hr;
						}
						else
							Fail();
					}

					newPrimitive = true;

					if (!add1st)
					{ ri++; vb++; }
					break;
				}
				case IS_TEXTURE:
				{
					if(!m_UseLODBias ||	(((*ri)->Format).ddpfPixelFormat.dwFlags & PF_FOURCC))
					{
						if (m_uPrevMaxMipLevel != LevelIndex)
						{
							m_uPrevMaxMipLevel = LevelIndex;
							SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, LevelIndex);
						}
					}
					else if (LevelCount > 1)
						(*t)->SetLOD(LevelIndex);

					if (!m_pSrcDevice->DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP, *vb, 
								0, 4, NULL, 0, 0, 0, POOL_MANAGED, false))
					{
						hr = m_pSrcDevice->GetLastError();
						
						WriteToLog("(%d %s T) DrawPrimitiveVertexBuffer() failed on level %d hr=%s(%x) (row %d, column %d).\n", PrimIndex, szBuffer, LevelIndex, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
						(*ri)->pCheckColor[LevelIndex] = false;

						if (D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
						{
							m_pSrcDevice->GetLastError();
                            Skip();
                            return hr;
						}
						else
							Fail();
					}

					if ( ++LevelIndex == (*ri)->NumSlices )
					{
						if ( ((*ri)->Format).d3dfFormat == FMT_A8 )
						{
							SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
							SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE );
						}

						LevelIndex = 0;
						newPrimitive = true;

						if (!add1st)
						{ ri++; vb++; t++; }
					}
					break;
				}
				case IS_CUBEMAP:
				{
					if (!m_pSrcDevice->DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP, *vb, 
						SliceIndex*4, 4, NULL, 0, 0, 0, POOL_MANAGED, false))
					{
						hr = m_pSrcDevice->GetLastError();
						
						WriteToLog("(%d %s CM) DrawPrimitiveVertexBuffer() failed on level %d and face %d hr=%s(%x) (row %d, column %d).\n", PrimIndex, szBuffer, LevelIndex, SliceIndex, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
						(*ri)->pCheckColor[LevelIndex*6 + SliceIndex] = false;

						if (D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
						{
							m_pSrcDevice->GetLastError();
                            Skip();
                            return hr;
						}
						else
							Fail();
					}

					if (++SliceIndex == 6)
					{
						SliceIndex = 0;
						if (++LevelIndex >= LevelCount)
						{
							if ( ((*ri)->Format).d3dfFormat == FMT_A8 )
							{
								SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
								SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE );
							}

							LevelIndex = 0;
							newPrimitive = true;

							if (!add1st)
							{ ri++; vb++; cm++; }
						}
						else
						{
							if( !m_UseLODBias || (((*ri)->Format).ddpfPixelFormat.dwFlags & PF_FOURCC))
							{
								if (m_uPrevMaxMipLevel != LevelIndex)
								{
									m_uPrevMaxMipLevel = LevelIndex;
									SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, LevelIndex);
								}
							}
							else if (LevelCount > 1)
								(*cm)->SetLOD(LevelIndex);
						}
					}
					break;
				}
				case IS_VOLUME:
				{
					hr = m_pSrcDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (SliceOffset + SliceIndex)*4, 2);
					if (FAILED(hr))
					{
						WriteToLog("(%d %s VOL) DrawPrimitive() failed on level %d and slice %d hr=%s(%x) (row %d, column %d).\n", PrimIndex, szBuffer, LevelIndex, SliceIndex, m_pD3D->HResultToString(hr), hr, cellIndex/ncell, cellIndex%ncell);
						(*ri)->pCheckColor[SliceOffset + SliceIndex] = false;
							
						if (D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
						{
							m_pSrcDevice->GetLastError();
                            Skip();
                            return hr;
						}
						else
							Fail();
					}

					if (++SliceIndex == numOfSlices)
					{
						SliceIndex = 0;
						SliceOffset += numOfSlices;

						if (++LevelIndex >= LevelCount)
						{
							if ( ((*ri)->Format).d3dfFormat == FMT_A8 )
							{
								SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
								SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE );
							}

							LevelIndex = 0;
							newPrimitive = true;

							if (!add1st)
							{ ri++; vb++; vt++; }
						}
						else
						{
							numOfSlices = max(1, numOfSlices/2);
							if( !m_UseLODBias || (((*ri)->Format).ddpfPixelFormat.dwFlags & PF_FOURCC))
							{
								if (m_uPrevMaxMipLevel != LevelIndex)
								{
									m_uPrevMaxMipLevel = LevelIndex;
									SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, LevelIndex);
								}
							}
							else
								(*vt)->SetLOD(LevelIndex);
						}
					}
					break;
				}
			}
			cellIndex++;
		}
		else draw = true;
	}

    return S_OK;
}

//-----------------------------------------------------------------------------

bool ListTest::CheckListColors(bool add1st)
{
	RESINFOLIST::iterator ri;
	VBUFFERLIST::iterator vb;
	IBUFFERLIST::iterator ib;
	TEXTURELIST::iterator t;
	CUBEMAPLIST::iterator cm;
	VOLTEXLIST::iterator vt;
	ri = startResInfo;
	vb = startVBuffer;
	ib = startIBuffer;
	t = startTexture;
	cm = startCubeMap;
	vt = startVolTex;

	CSurface    *pSrcSurf = NULL;
	LOCKED_RECT  LockRect;
	HRESULT      hr;
	bool         result = true;
	int          nPitch;
	void         *pBuf;
	DWORD        ncell  = (DWORD)ceil(sqrt((float)m_TotalNumberOfSlices));
	DWORD		 cellIndex = 0;
	int          primIndex = -1;

	hr = m_pSrcDevice->GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &pSrcSurf);
	if(FAILED(hr))
	{
		WriteToLog("CheckListColors: GetBackBuffer() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
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
	hr = pSrcSurf->LockRect( &LockRect, NULL, NULL );
    if (FAILED(hr))
	{
		WriteToLog("CheckListColors: LockRect() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		RELEASE(pSrcSurf);
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
    // Get the Surface data pointer for our subrect
	pBuf = LockRect.pBits;
    nPitch = LockRect.iPitch;

	while (cellIndex < m_TotalNumberOfSlices )
	{
		primIndex++;

		if (ri == m_ResInfosList.end())
			ri = m_ResInfosList.begin();
		else if (add1st) ri++;

		if ((*ri)->Type == IS_NULL)
		{
			if (!add1st) ri++;
			continue;
		}
		else
		{
			if (vb == m_VBuffersList.end())
				vb = m_VBuffersList.begin();
			else if (add1st) vb++;
			switch( (*ri)->Type )
			{
				case IS_16INDEXBUFFER: case IS_32INDEXBUFFER:
				{
					if (ib == m_IBuffersList.end())
						ib = m_IBuffersList.begin();
					else if (add1st) ib++;
					if ((*ri)->pCheckColor[0] && !CheckColors(pSrcSurf, 0xffffffff, pBuf, nPitch, (*ri)->pColors, cellIndex, ncell, ncell))
					{
						if (!KeySet("logallfailures"))
							(*ri)->pCheckColor[0] = false;
						result = false;
						switch ( (*ri)->Type )
						{
							case IS_16INDEXBUFFER:  WriteToLog("%d 16-BIT INDEX BUFFER FAILED. (row %d, column %d)\n", primIndex, cellIndex/ncell, cellIndex%ncell); break;
							case IS_32INDEXBUFFER:  WriteToLog("%d 32-BIT INDEX BUFFER FAILED. (row %d, column %d)\n", primIndex, cellIndex/ncell, cellIndex%ncell); break;
						}
						
					}
					cellIndex++;
					if (!add1st)
					{ ri++; vb++; ib++; }
					break;
				}
				case IS_TLVERTEXBUFFER:  case IS_LVERTEXBUFFER: case IS_VERTEXBUFFER:
				{
					DWORD primColor;
					switch ( (*ri)->Type )
					{
						case IS_VERTEXBUFFER:   primColor =  0xffffffff; break;
						case IS_TLVERTEXBUFFER: primColor =  0xff00ff00; break;
						case IS_LVERTEXBUFFER:  primColor =  0xff0000ff; break;
					}
					if ((*ri)->pCheckColor[0] && !CheckColors(pSrcSurf, primColor, pBuf, nPitch, (*ri)->pColors, cellIndex, ncell, ncell))
					{
						if (!KeySet("logallfailures"))
							(*ri)->pCheckColor[0] = false;
						result = false;
						switch ( (*ri)->Type )
						{
							case IS_VERTEXBUFFER:   WriteToLog("%d UNTRANSFORMED UNLIT VERTEX BUFFER FAILED. (row %d, column %d)\n", primIndex, cellIndex/ncell, cellIndex%ncell); break;
							case IS_TLVERTEXBUFFER: WriteToLog("%d TRANSFORMED LIT VERTEX BUFFER FAILED. (row %d, column %d)\n", primIndex, cellIndex/ncell, cellIndex%ncell); break;
							case IS_LVERTEXBUFFER:  WriteToLog("%d UNTRANSFORMED LIT VERTEX BUFFER FAILED. (row %d, column %d)\n", primIndex, cellIndex/ncell, cellIndex%ncell); break;
						}
					}
					cellIndex++;
					if (!add1st)
					{ ri++; vb++; }
					break;
				}
				case IS_TEXTURE:
				{
					if (t == m_TexturesList.end())
						t = m_TexturesList.begin();
					else if (add1st) t++;
					if (*t && *vb)
					{
						if (!CheckTexture(pSrcSurf, pBuf, nPitch, *t, (*ri)->pCheckColor, (*ri)->pColors, cellIndex, ncell, ncell))
						{
							result = false;
							TCHAR szBuffer[30];
							FmtToString(FORMAT_TEXTURE, &((*ri)->Format), szBuffer);
							SURFACEDESC Desc;
							(*t)->GetLevelDesc(0, &Desc);
							WriteToLog("%d TEXTURE %s %dx%d %d FAILED. (row %d, column %d)\n", primIndex, szBuffer, Desc.Width, Desc.Height, (*ri)->NumSlices, cellIndex/ncell, cellIndex%ncell);
						}
						cellIndex += (*ri)->NumSlices;
					}
					else if (*t == NULL )
						WriteToLog("%d TEXTURE IS NULL!!! (row %d, column %d)\n", primIndex, cellIndex/ncell, cellIndex%ncell);
					else if (*vb == NULL)
						WriteToLog("%d VBUFFER FOR TEX IS NULL!!! (row %d, column %d)\n", primIndex, cellIndex/ncell, cellIndex%ncell);
					if (!add1st)
					{ ri++; vb++; t++; }
					break;
				}
				case IS_CUBEMAP:
				{
					if (cm == m_CubesList.end())
						cm = m_CubesList.begin();
					else if (add1st) cm++;
					if (*cm && *vb)
					{
						if (!CheckCubeMap(pSrcSurf, pBuf, nPitch, *cm, (*ri)->pCheckColor, (*ri)->pColors, cellIndex, ncell, ncell))
						{
							result = false;
							TCHAR szBuffer[30];
							FmtToString(FORMAT_TEXTURE, &((*ri)->Format), szBuffer);
							SURFACEDESC Desc;
							(*cm)->GetLevelDesc(0, &Desc);
							WriteToLog("%d CUBE MAP %s %d %d FAILED. (row %d, column %d)\n", primIndex, szBuffer, Desc.Width, (*ri)->NumSlices/6, cellIndex/ncell, cellIndex%ncell);
						}
						cellIndex += (*ri)->NumSlices;
					}
					else if (*cm == NULL )
						WriteToLog("%d CUBE MAP IS NULL!!! (row %d, column %d)\n", primIndex, cellIndex/ncell, cellIndex%ncell);
					else if (*vb == NULL)
						WriteToLog("%d VBUFFER FOR CUBE IS NULL!!! (row %d, column %d)\n", primIndex, cellIndex/ncell, cellIndex%ncell);
					if (!add1st)
					{ ri++; vb++; cm++; }
					break;
				}
				case IS_VOLUME:
				{
					if (vt == m_VolumesList.end())
						vt = m_VolumesList.begin();
					else if (add1st)
						vt++;
					if (*vt && *vb)
					{
						if (!CheckVolumeTexture(pSrcSurf, pBuf, nPitch, *vt, (*ri)->pCheckColor, (*ri)->pColors, cellIndex, ncell, ncell))
						{
							result = false;
							TCHAR szBuffer[30];
							FmtToString(FORMAT_TEXTURE, &((*ri)->Format), szBuffer);
							VOLUME_DESC Desc;
							(*vt)->GetLevelDesc(0, &Desc);
							WriteToLog( "%d VOLUME TEXTURE %s %dx%dx%d %d FAILED. (row %d, column %d)\n", primIndex, szBuffer, Desc.Width, Desc.Height, Desc.Depth, (*vt)->GetLevelCount(), cellIndex/ncell, cellIndex%ncell );
						}
						cellIndex += (*ri)->NumSlices;
					}
					else if (*vt == NULL )
						WriteToLog("%d VOLUME IS NULL!!! (row %d, column %d)\n", primIndex, cellIndex/ncell, cellIndex%ncell);
					else if (*vb == NULL)
						WriteToLog("%d VBUFFER FOR VOL IS NULL!!! (row %d, column %d)\n", primIndex, cellIndex/ncell, cellIndex%ncell);
					if (!add1st)
					{ ri++; vb++; vt++; }
					break;
				}
			}
		}
	}

	hr = pSrcSurf->UnlockRect();
    if (FAILED(hr))
    {
	    WriteToLog("CheckListColors: UnlockRect() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
        Fail();
		RELEASE(pSrcSurf);
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

	RELEASE(pSrcSurf);
	return result;
}