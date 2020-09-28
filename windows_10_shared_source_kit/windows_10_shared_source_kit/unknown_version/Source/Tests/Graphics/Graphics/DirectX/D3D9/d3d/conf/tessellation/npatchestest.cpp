#define D3D_OVERLOADS

#include "Tessellation.h"
#include "cd3dx.h"		//for D3DXAssembleShader9

CNPatchesTest::CNPatchesTest()
{
	m_szTestName = "N-Patches Test";
	m_szCommandKey = "N";

	m_pVB = NULL;
	m_pIB = NULL;
	m_pShader = NULL;
	m_pDecl = NULL;
}

UINT CNPatchesTest::TestInitialize(void)
{
	HRESULT hr;
	m_nBasises = sizeof(Basises) / sizeof(D3DBASISTYPE);
	m_nDegrees = sizeof(Degrees) / sizeof(D3DDEGREETYPE);
	m_nTessLevels = 4;//sizeof(m_TessLevels) / (sizeof(float));
	m_nPatchesPerRow = 0;
	m_nVertices = 0;
	m_uPatchWidth = 0;
	m_uPatchHeight = 0;
	m_uBufferWidth = 0;
	m_uBufferHeight = 0;
	m_numPatches = 0;

	tcCount.View = m_nViewMatrices; 
	tcCount.TessLevels = m_nTessLevels;
	tcCount.Degree = m_nDegrees;
	tcCount.Basis = m_nBasises;
	tcCount.NumPatches = 6; 
	tcCount.VPMethod = 4; //fvf-fixed, decl-shader, decl-fixed, fvf-shader

    TESTCASE tc;
    tc.nTest = 0;

	ClearRangeList();
	
	for ( tc.VPMethod = 0; tc.VPMethod < tcCount.VPMethod; tc.VPMethod++ )
	{
		if ( !m_UseFVFFixed   && tc.VPMethod == 0 )
			continue;
		if ( !m_UseDeclShader && tc.VPMethod == 1 )
			continue;
		if ( !m_UseDeclFixed  && tc.VPMethod == 2 )
			continue;
		if ( !m_UseFVFShader  && tc.VPMethod == 3 )
			continue;
	
		for ( tc.TessLevels = 0; tc.TessLevels < tcCount.TessLevels; tc.TessLevels++ )
		for ( tc.NumPatches = 0; tc.NumPatches < tcCount.NumPatches; tc.NumPatches++ )
		for ( tc.Basis      = 0; tc.Basis      < tcCount.Basis;      tc.Basis++      )
		{
			if(Basises[tc.Basis] != D3DBASIS_BEZIER) 
				continue;
			for( tc.Degree = 0; tc.Degree < tcCount.Degree; tc.Degree++ )
			{
				tc.View = 0;
				AddTestRange( (tc.nTest + 1), (tc.nTest + tcCount.View) );
			}
		}
	}

	return D3DTESTINIT_RUN;
}

bool CNPatchesTest::ExecuteTest(UINT iTest)
{
	TESTCASE tc;
	tc.nTest = iTest-1;
	DWORD NumPatches = 1 << tc.NumPatches;
	D3DPRIMITIVETYPE prim = (D3DPRIMITIVETYPE)((tc.NumPatches+tc.Degree)%3 + 4);

	if(	tc.NumPatches >= tcCount.NumPatches ||
		tc.View       >= tcCount.View       ||
        tc.Degree     >= tcCount.Degree     ||
		tc.Degree     >= MAX_DEGREE         || //prefast error
        tc.Basis      >= tcCount.Basis      ||
		tc.TessLevels >= tcCount.TessLevels ||
		tc.VPMethod   >= tcCount.VPMethod    )
    {
        WriteToLog( "No such test (%d)\n", iTest);
		return false;
    }

	if (tc.View == 0 || iTest == m_pRangeList->uStart || (m_pConstraints != NULL && iTest == m_pConstraints->uStart))
	{
		RELEASE(m_pVB);
		RELEASE(m_pIB);
		
		if (!GenerateBuffers(tc, prim))
		{
			SkipTestRange(iTest, iTest+4);
			return false;
		}
		if ( !SetStreamSource(0, m_pVB, sizeof(VERTEX)) )
			WriteToLog("%d Set stream source failed.\n", iTest);
		if ( (tc.NumPatches+tc.Degree)%2 && !SetIndices( m_pIB, 0 ) )
		{
			WriteToLog("%d SetIndices failed.\n", iTest);
			return false;
		}
		
		if ( Degrees[tc.Degree] == D3DDEGREE_LINEAR || Degrees[tc.Degree] == D3DDEGREE_CUBIC )
			SetRenderState(D3DRS_POSITIONDEGREE, Degrees[tc.Degree]);
		if ( Degrees[tc.Degree] == D3DDEGREE_LINEAR || Degrees[tc.Degree] == D3DDEGREE_QUADRATIC )
			SetRenderState(D3DRS_NORMALDEGREE, Degrees[tc.Degree]);
		
		sprintf( msgString, TEXT("%d patches, w=%d, h=%d, Tess=%.2f, %s %s, %s-%s"),
			NumPatches, m_uPatchWidth, m_uPatchHeight, m_TessLevels[tc.TessLevels][0],
			(tc.NumPatches+tc.Degree)%2 ? "DrawIndexPrim" : "DrawPrim",
			prim == D3DPT_TRIANGLELIST ? "LIST" : (prim == D3DPT_TRIANGLESTRIP ? "STRIP" : "FAN"),
			(tc.VPMethod == 0 || tc.VPMethod == 3) ? "fvf" : "decl",
			(tc.VPMethod == 0 || tc.VPMethod == 2) ? "fixed" : "shader" );
		if (tc.VPMethod == 0 || tc.VPMethod == 3)
		{
			if ( !SetVertexShader(FVF_VERTEX) ) 
			{
				WriteToLog("SetVertexShader(FVF_VERTEX) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
				SkipTestRange(iTest, iTest+4);
				return false;
			}
		}
		else
		{
			if ( !SetVertexDeclaration(m_pDecl) )
			{
				WriteToLog("SetVertexDeclaration(m_pDecl) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
				SkipTestRange(iTest, iTest+4);
				return false;
			}
		}
		if (tc.VPMethod == 0 || tc.VPMethod == 2)
		{
			if ( !SetVertexShader((CnVertexShader*)NULL) )
			{
				WriteToLog("SetVertexShader((CnVertexShader*)NULL) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
				SkipTestRange(iTest, iTest+4);
				return false;
			}
		}
		else
		{
			if ( !SetVertexShader(m_pShader) )
			{
				WriteToLog("SetVertexShader(m_pShader) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
				SkipTestRange(iTest, iTest+4);
				return false;
			}
		}
	}

	BeginTestCase( msgString, iTest);

	float AdaptiveTess_x;
	float AdaptiveTess_y;
	float AdaptiveTess_z;
	float AdaptiveTess_w;
	switch (tc.View)
	{
		case 0:
			AdaptiveTess_x = 0.f;
			AdaptiveTess_y = 0.f;
			AdaptiveTess_z = 2.f;
			AdaptiveTess_w = 1.f;
			break;
		case 1:
			AdaptiveTess_x = -1.f;
			AdaptiveTess_y = 0.f;
			AdaptiveTess_z = 0.f;
			AdaptiveTess_w = 2.f;
			break;
		case 2:
			AdaptiveTess_x = 0.f;
			AdaptiveTess_y = -1.f;
			AdaptiveTess_z = 0.f;
			AdaptiveTess_w = 2.f;
			break;
		case 3:
			AdaptiveTess_x = 1.f;
			AdaptiveTess_y = 0.f;
			AdaptiveTess_z = 0.f;
			AdaptiveTess_w = 2.f;
			break;
		case 5:
			AdaptiveTess_x = 0.f;
			AdaptiveTess_y = 1.f;
			AdaptiveTess_z = 0.f;
			AdaptiveTess_w = 2.f;
			break;
	}
	SetRenderState(D3DRS_ADAPTIVETESS_X, *(DWORD*)&AdaptiveTess_x);
	SetRenderState(D3DRS_ADAPTIVETESS_Y, *(DWORD*)&AdaptiveTess_y);
	SetRenderState(D3DRS_ADAPTIVETESS_Z, *(DWORD*)&AdaptiveTess_z);
	SetRenderState(D3DRS_ADAPTIVETESS_W, *(DWORD*)&AdaptiveTess_w);

	//set stransformation matrices
	D3DXMATRIX mat;
	if ( tc.VPMethod == 1 || tc.VPMethod == 3 )
	{
		D3DXMatrixMultiply( &mat, &m_ViewMatrices[tc.View], &m_ProjMatrix); 
		D3DXMatrixTranspose( &mat, &mat );   
		SetVertexShaderConstant(0, &mat, 4); 
	}
	else
	{
		mat = m_ViewMatrices[tc.View];
		if (!SetTransform(D3DTS_VIEW, &mat))
		{
			WriteToLog("Can't set up a veiw matrix.\n");			
			return false;
		}
	}

    BeginScene();

	float oldValue, newValue;
	oldValue = GetNPatchMode();
	if ( !SetNPatchMode( m_TessLevels[tc.TessLevels][0] ) )
	{
		WriteToLog("SetNPatchMode failed on %d.\n", m_TessLevels[tc.TessLevels][0] );
		newValue = GetNPatchMode();
		if ( newValue != oldValue)
			WriteToLog("GetNPatchMode returned %d, expected %d\n", newValue, oldValue);
		return false;
	}
	newValue = GetNPatchMode();
	if (newValue != m_TessLevels[tc.TessLevels][0] )
	{
		WriteToLog("GetNPatchMode returned %d, expected %d\n", newValue, m_TessLevels[tc.TessLevels][0]);
		return false;
	}

	switch ( prim )
	{
		case D3DPT_TRIANGLESTRIP:
		{
			for (int j = 0; j < NumPatches; j++)
			{
				SetNewMaterial(j);
				UINT MinIndex = (m_uPatchHeight-1)*m_uBufferWidth*(j/m_nPatchesPerRow) + (j%m_nPatchesPerRow)*(m_uPatchWidth-1);
				UINT NumVertices = m_uPatchWidth+(m_uPatchHeight-1)*m_uBufferWidth;
				UINT StartIndex = j*2*m_uPatchWidth;
				UINT PrimitiveCount = 2*m_uPatchWidth-2;

				if ((tc.NumPatches+tc.Degree)%2)
				{
					if ( !DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, MinIndex, NumVertices, StartIndex, PrimitiveCount, 0, CD3D_BOTH) )
						WriteToLog("%d DrawIndexedPrimitve failed.\n", iTest);
				}
				else
				{
					if ( !DrawPrimitive(D3DPT_TRIANGLESTRIP, StartIndex, PrimitiveCount, CD3D_BOTH) )
						WriteToLog("%d DrawIndexedPrimitve failed.\n", iTest);
				}
			}
			break;
		}
		case D3DPT_TRIANGLELIST:
		{
			for (int j = 0; j < NumPatches; j++)
			{
				SetNewMaterial(j);
				UINT MinIndex = (m_uPatchHeight-1)*m_uBufferWidth*(j/m_nPatchesPerRow) + (j%m_nPatchesPerRow)*(m_uPatchWidth-1);
				UINT NumVertices = m_uPatchWidth+(m_uPatchHeight-1)*m_uBufferWidth;
				UINT StartIndex = j*6*(m_uPatchWidth-1)*(m_uPatchHeight-1);
				UINT PrimitiveCount = 2*(m_uPatchWidth-1)*(m_uPatchHeight-1);

				if ((tc.NumPatches+tc.Degree)%2)
				{
					if ( !DrawIndexedPrimitive(D3DPT_TRIANGLELIST, MinIndex, NumVertices, StartIndex, PrimitiveCount, 0, CD3D_BOTH) )
						WriteToLog("%d DrawIndexedPrimitve failed.\n", iTest);
				}
				else
				{
					if ( !DrawPrimitive(D3DPT_TRIANGLELIST, StartIndex, PrimitiveCount, CD3D_BOTH) )
						WriteToLog("%d DrawIndexedPrimitve failed.\n", iTest);
				}
			}
			break;
		}
		case D3DPT_TRIANGLEFAN:
		{
			for (int j = 0; j < NumPatches; j++)
			{
				SetNewMaterial(j);
				UINT MinIndex = (m_uPatchHeight-1)*m_uBufferWidth*(j/m_nPatchesPerRow) + (j%m_nPatchesPerRow)*(m_uPatchWidth-1);
				UINT NumVertices = m_uPatchWidth+(m_uPatchHeight-1)*m_uBufferWidth;
				UINT StartIndex = j*(2 + 2*(m_uPatchWidth + m_uPatchHeight-2));
				UINT PrimitiveCount = 2*(m_uPatchWidth + m_uPatchHeight-2);

				if ((tc.NumPatches+tc.Degree)%2)
				{
					if ( !DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, MinIndex, NumVertices, StartIndex, PrimitiveCount, 0, CD3D_BOTH) )
						WriteToLog("%d DrawIndexedPrimitve failed.\n", iTest);
				}
				else
				{
					if ( !DrawPrimitive(D3DPT_TRIANGLEFAN, StartIndex, PrimitiveCount, CD3D_BOTH) )
						WriteToLog("%d DrawIndexedPrimitve failed.\n", iTest);
				}
			}
			break;
		}
	}

	EndScene();
	return true;
}
             
bool CNPatchesTest::GenerateBuffers(TESTCASE tc, D3DPRIMITIVETYPE primType)
{
	PBYTE   pBuffer2 = NULL;
	VOID   *pV = NULL;
	VERTEX *VBarray = NULL, *VBarray1 = NULL;
	WORD   *IBarray = NULL;
	float   x, y, z, nx, ny, nz;
	bool    DrawIndPrim = (tc.NumPatches+tc.Degree)%2 ? true : false;
	
	UINT k = 1<<tc.NumPatches;
	m_nPatchesPerRow = (UINT) ceil(sqrt(float(k)));
	switch (primType)
	{
		case D3DPT_TRIANGLELIST: 
		{
			m_uPatchHeight = m_uPatchWidth = Degrees[tc.Degree] + 1;
			m_nVertices = 6*(m_uPatchWidth-1)*(m_uPatchHeight-1)*(m_nPatchesPerRow*m_nPatchesPerRow); 
			break;
		}
		case D3DPT_TRIANGLESTRIP:
		{
			m_uPatchWidth = Degrees[tc.Degree] + 1;
			m_uPatchHeight = 2;
			m_nVertices = 2 * m_uPatchWidth * (m_nPatchesPerRow*m_nPatchesPerRow); 
			break;
		}
		case D3DPT_TRIANGLEFAN:
		{
			m_uPatchHeight = m_uPatchWidth = Degrees[tc.Degree] + 1;
			m_nVertices = (2 + 2*(m_uPatchWidth + m_uPatchHeight-2))*(m_nPatchesPerRow*m_nPatchesPerRow); 
			break;
		}
		default:
		{
			WriteToLog("Invalid primitive type %d\n", primType);
			return false;
		}
	}
	m_uBufferWidth = m_uPatchWidth  + (m_uPatchWidth - 1)*(m_nPatchesPerRow - 1);
	m_uBufferHeight =m_uPatchHeight + (m_uPatchHeight -1)*(m_nPatchesPerRow - 1);

	VBarray = new VERTEX[m_uBufferHeight*m_uBufferWidth];
	for (int j = 0; j < m_uBufferHeight; j++)
	{
		y = 1.f - 2.f*((float)j/(m_uBufferHeight-1.f));
		for (int i = 0; i < m_uBufferWidth; i++)
		{
			if (KeySet(_T("straight")))
			{
				x = -1.f + 2.f*((float)i/(m_uBufferWidth-1.f));
				z = 0.f;
				nx = ny = 0.0f;
				nz = -1.0f;
			}
			else
			{
				x = cos( (m_uBufferWidth-1.f-i) * (D3DX_PI/(m_uBufferWidth-1.f)) );
				z = sqrt( 1.f - x*x );
				nx = -x;
				ny = 0.0f;
				nz = -z;
			}
			VBarray[j*m_uBufferWidth+i] = VERTEX(D3DXVECTOR3( x, y, z ), D3DXVECTOR3( nx, ny, nz ), (1.f+x)/2.f, (1.f-y)/2.f); 
		}
	}
	if (DrawIndPrim)
	{
		if( !CreateVertexBuffer( m_uBufferHeight*m_uBufferWidth*sizeof(VERTEX), D3DUSAGE_NPATCHES, tc.VPMethod == 1 || tc.VPMethod == 2 ? 0 : FVF_VERTEX, POOL_MANAGED, &m_pVB )) 
		{
			WriteToLog("%d: Can't create vertex buffer.\n", tc.nTest);
			return false;
		}
		if( !CreateIndexBuffer( m_nVertices*sizeof(WORD), D3DUSAGE_NPATCHES, FMT_INDEX16, POOL_MANAGED, &m_pIB )) 
		{
			WriteToLog("%d: Can't create index buffer.\n", tc.nTest);
			return false;
		}
		if (!m_pVB->Lock( 0, m_uBufferHeight*m_uBufferWidth*sizeof(VERTEX), (BYTE**)&pV, 0))
		{
			WriteToLog("%d: Failed to lock m_pVB.\n", tc.nTest);
			return false;
		}
		memcpy( pV, VBarray, m_uBufferHeight*m_uBufferWidth*sizeof(VERTEX));
		if (!m_pVB->Unlock())
		{
			WriteToLog("%d: Failed to unlock m_pVB.\n", tc.nTest);
			return false;
		}
		if (!m_pIB->Lock( 0, 0, &pBuffer2, 0))
		{
			WriteToLog("%d: Failed to lock m_pIB.\n", tc.nTest);
			return false;
		}
		IBarray = new WORD[m_nVertices];
	}
	else
	{
		if( !CreateVertexBuffer( m_nVertices*sizeof(VERTEX), D3DUSAGE_NPATCHES, tc.VPMethod == 1 || tc.VPMethod == 2 ? 0 : FVF_VERTEX, POOL_MANAGED, &m_pVB )) 
		{
			WriteToLog("%d: Can't create vertex buffer.\n", tc.nTest);
			return false;
		}
		if (!m_pVB->Lock( 0, m_nVertices*sizeof(VERTEX), (BYTE**)&pV, 0))
		{
			WriteToLog("%d: Failed to lock m_pVB.\n", tc.nTest);
			return false;
		}
		VBarray1 = new VERTEX[m_nVertices];
	}
	
	WORD* pIndices = (WORD*)pBuffer2;
	UINT offset, x_offset, y_offset, offset1, offset2;
	WORD index = 0;
	switch (primType)
	{
		case D3DPT_TRIANGLELIST:
		{
			for (UINT PatchInd = 0; PatchInd < m_nPatchesPerRow*m_nPatchesPerRow; PatchInd++)
			{
				if (index >= m_nVertices)
					WriteToLog("more vertices than expected\n");

				offset = PatchInd*6*(m_uPatchWidth-1)*(m_uPatchHeight - 1);
				x_offset = (PatchInd % m_nPatchesPerRow)*(m_uPatchWidth  - 1);
				y_offset = (PatchInd / m_nPatchesPerRow)*(m_uPatchHeight - 1);
				for (int j = 0; j < m_uPatchHeight; j++)
				{
					offset1 = (j-1)*6*(m_uPatchWidth-1) + offset;
					offset2 = (j-0)*6*(m_uPatchWidth-1) + offset;
		            
					for (int i = 0; i < m_uPatchWidth; i++)
					{
						index = i + x_offset + ( j + y_offset) * m_uBufferWidth;
						if (DrawIndPrim)
						{
							if ( j > 0 ) //triangles on the top
							{
								if (i > 0)
								{
									IBarray[ offset1 + 6*(i-1) + 1] = index;
									IBarray[ offset1 + 6*(i-1) + 3] = index;
								}
								if (i < m_uPatchWidth - 1)
								{
									IBarray[ offset1 + 6*i + 2] = index;
								}
							}
							if ( j < m_uPatchHeight - 1 ) //triangles on the bottom
							{
								if (i > 0) //triangle on the left
								{
									IBarray[ offset2 + 6*(i-1) + 5] = index;
								}
								if (i < m_uPatchWidth - 1) //triangle on the right
								{
									IBarray[ offset2 + 6*i + 0] = index;
									IBarray[ offset2 + 6*i + 4] = index;
								}
							}
						}
						else
						{
							if ( j > 0 ) //triangles on the top
							{
								if (i > 0)
								{
									VBarray1[ offset1 + 6*(i-1) + 1] = VBarray[index];
									VBarray1[ offset1 + 6*(i-1) + 3] = VBarray[index];
								}
								if (i < m_uPatchWidth - 1)
								{
									VBarray1[ offset1 + 6*i + 2] = VBarray[index];
								}
							}
							if ( j < m_uPatchHeight - 1 ) //triangles on the bottom
							{
								if (i > 0) //triangle on the left
								{
									VBarray1[ offset2 + 6*(i-1) + 5] = VBarray[index];
								}
								if (i < m_uPatchWidth - 1) //triangle on the right
								{
									VBarray1[ offset2 + 6*i + 0] = VBarray[index];
									VBarray1[ offset2 + 6*i + 4] = VBarray[index];
								}
							}
						}
						index++;
					}
				}
			}
			break;
		}
		case D3DPT_TRIANGLESTRIP:
		{
			for (UINT PatchInd = 0; PatchInd < m_nPatchesPerRow*m_nPatchesPerRow; PatchInd++)
			{
  				offset = PatchInd * 2 * m_uPatchWidth;
				x_offset = (PatchInd % m_nPatchesPerRow)*(m_uPatchWidth  - 1);
				y_offset =  PatchInd / m_nPatchesPerRow;
				for (int i = 0; i < m_uPatchWidth; i++)
				{
					if ( DrawIndPrim )
					{
						IBarray[ offset + 2*i+0 ] = x_offset + i + (y_offset+1) * m_uBufferWidth;
						IBarray[ offset + 2*i+1 ] = x_offset + i + (y_offset+0) * m_uBufferWidth;
					}
					else
					{
						VBarray1[ offset + 2*i+0 ] = VBarray[ x_offset + i + (y_offset+1) * m_uBufferWidth ];
						VBarray1[ offset + 2*i+1 ] = VBarray[ x_offset + i + (y_offset+0) * m_uBufferWidth ];
					}
				}
			}
			break;
		}
		case D3DPT_TRIANGLEFAN:
		{
			for (UINT PatchInd = 0; PatchInd < m_nPatchesPerRow*m_nPatchesPerRow; PatchInd++)
			{
				offset = PatchInd*(2 + 2*(m_uPatchWidth + m_uPatchHeight-2));
				x_offset = (PatchInd % m_nPatchesPerRow)*(m_uPatchWidth  - 1);
				y_offset = (PatchInd / m_nPatchesPerRow)*(m_uPatchHeight - 1);
				if ( DrawIndPrim )
					IBarray[ offset ] = x_offset + m_uPatchWidth/2 + (y_offset + m_uPatchHeight/2) * m_uBufferWidth;
				else
					VBarray1[ offset ] = VBarray[ x_offset + m_uPatchWidth/2 + (y_offset + m_uPatchHeight/2) * m_uBufferWidth ];
				for (int i = 0; i < m_uPatchWidth; i++)
				{
					if ( DrawIndPrim )
					{
						IBarray[ offset + 1 + i ] = x_offset + i + y_offset * m_uBufferWidth;
					    IBarray[ offset + 2*(m_uPatchWidth-1) + m_uPatchHeight - i ] = x_offset + i + (y_offset + m_uPatchHeight - 1) * m_uBufferWidth;
					}
					else
					{
						VBarray1[ offset + 1 + i ] = VBarray[ x_offset + i + y_offset * m_uBufferWidth ];
					    VBarray1[ offset + 2*(m_uPatchWidth-1) + m_uPatchHeight - i ] = VBarray[ x_offset + i + (y_offset + m_uPatchHeight - 1) * m_uBufferWidth ];
					}
				}
				for (int j = 1; j < m_uPatchHeight - 1; j++)
				{
					if ( DrawIndPrim )
					{
						IBarray[ offset + m_uPatchWidth + j ] = x_offset + m_uPatchWidth - 1 + (j + y_offset) * m_uBufferWidth;
					    IBarray[ offset + 2*(m_uPatchWidth + m_uPatchHeight - 2) + 1 - j ] = x_offset + (j + y_offset) * m_uBufferWidth;
					}
					else
					{
						VBarray1[ offset + m_uPatchWidth + j ] = VBarray[ x_offset + m_uPatchWidth - 1 + (j + y_offset) * m_uBufferWidth ];
					    VBarray1[ offset + 2*(m_uPatchWidth + m_uPatchHeight - 2) + 1 - j ] = VBarray[ x_offset + (j + y_offset) * m_uBufferWidth ];
					}
				}
				if ( DrawIndPrim )
					IBarray[ offset + 2*(m_uPatchWidth + m_uPatchHeight - 2) + 1 ] = x_offset + y_offset * m_uBufferWidth;
				else
					VBarray1[ offset + 2*(m_uPatchWidth + m_uPatchHeight - 2) + 1 ] = VBarray[ x_offset + y_offset * m_uBufferWidth ];
			}
			break;
		}
	}
	if ( DrawIndPrim )
	{
		memcpy( pIndices, IBarray, m_nVertices*sizeof(WORD) );
		if (!m_pIB->Unlock())
		{
			WriteToLog("%d: Failed to unlock m_pIB.\n", tc.nTest);
			return false;
		}
		delete [] IBarray;
	}
	else
	{
		memcpy( pV, VBarray1, m_nVertices*sizeof(VERTEX));
		if (!m_pVB->Unlock())
		{
			WriteToLog("%d: Failed to unlock m_pVB.\n", tc.nTest);
			return false;
		}
		delete [] VBarray1;
	}

	delete [] VBarray;
	
	return true;
}

bool CNPatchesTest::GenerateVertexShader()
{
	HRESULT hr;
	LPD3DXBUFFER pShader = NULL;
	bool result = true;

	//create declaration
	VERTEXELEMENT decl[] = 
	{
		{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
		{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
		//{0, 24, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
		{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
		DECL_END()
	};

	if(!CreateVertexDeclaration( decl, &(m_pDecl)))
	{
		WriteToLog( "GenerateVertexShader - CreateVertexDeclaration() failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()) );
		m_UseDeclFixed = m_UseDeclShader = false;
		result = false;
	}

	CAPS caps;
	m_pSrcDevice->GetDeviceCaps(&caps);    // initialize m_pd3dDevice before using
	if( caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
	{
		WriteToLog("GenerateVertexShader: vertex shaders version 1.1 is not supported\n");
		m_UseDeclShader = m_UseFVFShader = false;
		return result;
	}

	if( caps.MaxVertexShaderConst < 7 )
	{
		WriteToLog("GenerateVertexShader: only %d vertex shader constants are supported. need at least 7\n", caps.MaxVertexShaderConst);
		m_UseDeclShader = m_UseFVFShader = false;
		return result;
	}
	
	TCHAR *szShader = _T(
		"vs.1.1            \n"
		"dcl_position v0   \n"
		"dcl_normal   v1   \n"
		"dcl_texcoord v2   \n"
		//"dcl_diffuse  v3   \n"

		"m4x4 oPos,   v0,  c0  \n"
		"dp3  r0.x,   v1, c7  \n"    // perform lighting N dot L calculation in world space
		                              
		"mul    r1, r0.x,  c4  \n"	  //calculate pixel color from light intensity and interpolated diffuse vertex color 
		"add   oD0,   r1,  c6  \n"    //add ambient color -> final pixel color
		"mov   oT0,   v2       \n"
	);
	hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
	if (FAILED(hr))
	{
		WriteToLog("GenerateVertexShader::D3DXAssembleShader(vertex) failed, hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
		m_UseDeclShader = m_UseFVFShader = false;
		return false;
	}
		
	if ( !CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &m_pShader) )
	{
		WriteToLog("GenerateVertexShader::CreateVertexShader() failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
		pShader->Release();
		m_UseDeclShader = m_UseFVFShader = false;
		return false;
	}
	pShader->Release();

	return result;
}

bool CNPatchesTest::CapsCheck()
{
	Tessellation::CapsCheck();
	CAPS caps;
	m_pSrcDevice->GetDeviceCaps(&caps); 
	//if this Tri patches aren't supported -> skip the test
	if(!(caps.DevCaps & D3DDEVCAPS_NPATCHES))
	{
        WriteToLog("CapsCheck: N-patches are not supported.\n");
        return false;
    } 
	//if use adaptive tesselation but it's not supported for n-patches -> skip
	if (m_AdapTess && !(caps.DevCaps2 & D3DDEVCAPS2_ADAPTIVETESSNPATCH)) 
	{
        WriteToLog("CapsCheck: adaptive tesselation is not supported for N-patches.\n");
        return false;
    }

	m_TessLevels[0][0] = 1.f;
	m_TessLevels[1][0] = m_MaxTess/4.f+1;
	m_TessLevels[2][0] = m_MaxTess/2.f;
	m_TessLevels[3][0] = m_MaxTess;

	return true;
}

bool CNPatchesTest::Setup()
{
	Tessellation::Setup();
	
	//set vertex shaders
	if ((m_UseDeclShader || m_UseDeclFixed || m_UseFVFShader) && !GenerateVertexShader())
	{
		WriteToLog("CNPatchesTest::Setup - GenerateVertexShader() failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
		return false;
	}

	return true;
}

void CNPatchesTest::Cleanup()
{
	Tessellation::Cleanup();
	
	RELEASE(m_pVB);
	RELEASE(m_pIB);
	RELEASE(m_pShader);
	RELEASE(m_pDecl);
}
