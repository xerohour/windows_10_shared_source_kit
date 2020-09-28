#define D3D_OVERLOADS

#include "Tessellation.h"
#include "cd3dx.h"		//for D3DXAssembleShader9

CTessFactorTriTest::CTessFactorTriTest()
{
	m_szTestName = "TessFactor Tri Test";
	m_szCommandKey = "TessFactorTri";
	
	m_pVB = NULL;
	m_pShader = NULL;
	m_pDecl = NULL;

	m_UseDeclShader = false;
	m_UseFVFShader  = false;
	m_UseFVFFixed   = false;
	m_UseDeclFixed  = false;
	if( KeySet(_T("decl-fix"))  )
		m_UseDeclFixed   = true;
	else 
		m_UseDeclShader = true;
}

UINT CTessFactorTriTest::TestInitialize(void)
{
	HRESULT hr;
	m_nBasises = sizeof(Basises) / sizeof(D3DBASISTYPE);
	m_nDegrees = sizeof(Degrees) / sizeof(D3DDEGREETYPE);
	m_nTessLevels = 3;//sizeof(TriTessFactorLevels) / (3*sizeof(float));
	m_nPatchesPerRow = 0;
	m_nVertices = 0;
	m_uPatchWidth = 0;
	m_uPatchHeight = 0;
	m_uBufferWidth = 0;
	m_uBufferHeight = 0;

	tcCount.View = m_nViewMatrices; 
	tcCount.TessLevels = m_nTessLevels;
	tcCount.Degree = m_nDegrees;
	tcCount.Basis = m_nBasises;
	tcCount.NumPatches = 6; 

    TESTCASE tc;
	tc.nTest = 0;

	ClearRangeList();
	
	for ( tc.TessLevels = 0; tc.TessLevels < tcCount.TessLevels; tc.TessLevels++ )
	for ( tc.NumPatches = 0; tc.NumPatches < tcCount.NumPatches; tc.NumPatches++ )
	for ( tc.Basis = 0;      tc.Basis      < tcCount.Basis;      tc.Basis++      )
	{
		//only bezier patches are used as tripatches
		if(Basises[tc.Basis] != D3DBASIS_BEZIER) 
			continue;
		for( tc.Degree = 0; tc.Degree < tcCount.Degree; tc.Degree++ )
		{
			//quadratic degree isn't supported for rtpatches
			if (Degrees[tc.Degree] == D3DDEGREE_QUADRATIC)
				continue;
			tc.View = 0;
			AddTestRange( (tc.nTest + 1), (tc.nTest + tcCount.View) );
		}
	}

	return D3DTESTINIT_RUN;
}

bool CTessFactorTriTest::ExecuteTest(UINT iTest)
{
	TESTCASE tc;
	tc.nTest = iTest-1;
	DWORD NumPatches = 1 << tc.NumPatches;
	if(	tc.NumPatches >= tcCount.NumPatches ||
		tc.View       >= tcCount.View       ||
        tc.Degree     >= tcCount.Degree     ||
        tc.Basis      >= tcCount.Basis      ||
		tc.TessLevels >= tcCount.TessLevels  )
    {
        WriteToLog( "No such test (%d)\n", iTest);
		return false;
    }

	if ( tc.View == 0 || iTest == m_pRangeList->uStart || (m_pConstraints != NULL && iTest == m_pConstraints->uStart))
	{
		for( UINT i = 0; i < m_numPatches; i++ )
		{
			DeletePatch(SURFACEHANDLEBASE + i);
		}
		m_numPatches = 0;
		RELEASE(m_pVB);
		//RELEASE(m_ColorBuffer);
		if (!GenerateBuffers(tc))
		{
			SkipTestRange(iTest, iTest+4);
			return false;
		}
		sprintf( msgString, TEXT("%d %s %s patches, TessLevels=(%.2f, %.2f, %.2f)"),
			NumPatches, BasisStr[tc.Basis], DegreeStr[tc.Degree],
			m_TessLevels[tc.TessLevels][0], m_TessLevels[tc.TessLevels][1], m_TessLevels[tc.TessLevels][2]
		);
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

	if ( !SetStreamSource(0, m_pVB, sizeof(TESSVERTEX)) )
			WriteToLog("%d Set stream source failed.\n", iTest);

	//set stransformation matrices
	D3DXMATRIX mat;
	if (m_UseDeclShader)
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
			EndScene();
			return false;
		}
	}
	
	D3DTRIPATCH_INFO TriInfo;
	float f[3];
	f[0]=m_TessLevels[tc.TessLevels][0]; 
	f[1]=m_TessLevels[tc.TessLevels][1]; 
	f[2]=m_TessLevels[tc.TessLevels][2]; 

    BeginScene();

	if ( tc.View == 0 )
	{
		m_numPatches = NumPatches;
		for( int i = 0; i < m_numPatches; i++ ) 
		{
			SetNewMaterial(i);
   			memset(&TriInfo, 0, sizeof(D3DTRIPATCH_INFO));
			TriInfo.StartVertexOffset = i*m_nVertices;
			TriInfo.NumVertices = m_nVertices; 
			TriInfo.Basis = Basises[tc.Basis]; 
			TriInfo.Degree = Degrees[tc.Degree];
			if (!DrawTriPatch(SURFACEHANDLEBASE+i, f, &TriInfo))
				WriteToLog("%d Failed to DrawTriPatch\n", iTest);
		}
	}
	else
	{
		for( int i = 0; i < m_numPatches; i++ ) 
		{
			SetNewMaterial(i);
			if (!DrawTriPatch(SURFACEHANDLEBASE+i, f, NULL))
				WriteToLog("%d Failed to DrawTriPatch\n", iTest);
		}
	}
	
	EndScene();
	return true;
}

bool CTessFactorTriTest::GenerateBuffers(TESTCASE tc)
{
	VOID *pV = NULL;
	float x, y, z, nx, ny, nz;
	TESSVERTEX  *VBarray = NULL;
	
	m_nPatchesPerRow = ((int)ceil(sqrt((1<<tc.NumPatches)/2.f))) << 1;
	m_nVertices = ((Degrees[tc.Degree]+1)*(Degrees[tc.Degree]+2)) >> 1;
	m_uPatchHeight = m_uPatchWidth = Degrees[tc.Degree] + 1;
	m_uBufferWidth = m_uPatchWidth  + (m_uPatchWidth - 1)*((m_nPatchesPerRow>>1) - 1);
	m_uBufferHeight =m_uPatchHeight + (m_uPatchHeight -1)*((m_nPatchesPerRow>>1) - 1);
	
	if( !CreateVertexBuffer( m_nVertices*m_nPatchesPerRow*(m_nPatchesPerRow>>1)*sizeof(TESSVERTEX), D3DUSAGE_RTPATCHES, 0, POOL_MANAGED, &m_pVB )) 
	{
		WriteToLog("%d: Can't create vertex buffer.\n", tc.nTest);
		return false;
	}
	if (!m_pVB->Lock( 0, m_nVertices*m_nPatchesPerRow*(m_nPatchesPerRow>>1)*sizeof(TESSVERTEX), (BYTE**)&pV, 0))
	{
		WriteToLog("%d: Failed to lock m_pVB.\n", tc.nTest);
		return false;
	}
	VBarray = new TESSVERTEX[m_nVertices*m_nPatchesPerRow*(m_nPatchesPerRow>>1)];
	int tempWidth, X_Offset = 0, Y_Offset = 0, index = 0;
	for (UINT PatchInd = 0; PatchInd < ((m_nPatchesPerRow>>1)*(m_nPatchesPerRow>>1))<<1; PatchInd += 2)
	{
		tempWidth = 1;
		for (int j = Y_Offset; j < Y_Offset + (int)m_uPatchHeight; j++)
		{
			y = 1.f-2.f*((float)j/(m_uBufferHeight-1.f));
			for (int i = X_Offset; i < X_Offset + tempWidth; i++)
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
					
				VBarray[index++] = TESSVERTEX (D3DXVECTOR3( x, y, z ), D3DXVECTOR3( nx, ny, nz ), (1.f+x)/2.f, (1.f-y)/2.f, z*5.f);
			}
			tempWidth++;
		}
		tempWidth = 1;
		for (int j = Y_Offset + (int)m_uPatchHeight - 1; j >= Y_Offset; j--)
		{
			y = 1.f-2.f*((float)j/(m_uBufferHeight-1.f));
			for (int i = X_Offset + (int)m_uPatchWidth - 1; i >= X_Offset + (int)m_uPatchWidth - tempWidth; i--)
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
					
				VBarray[index++] = TESSVERTEX (D3DXVECTOR3( x, y, z ), D3DXVECTOR3( nx, ny, nz ), (1.f+x)/2.f, (1.f-y)/2.f, z*5.f);
			}
			tempWidth++;
		}
		X_Offset += m_uPatchWidth-1;
		if (X_Offset >= m_uBufferWidth-1)
		{
			X_Offset = 0;
			Y_Offset += m_uPatchHeight-1;
		}
	}
	m_uBufferWidth  = m_nVertices*m_nPatchesPerRow;
	m_uBufferHeight = m_nPatchesPerRow >> 1;
	memcpy( pV, VBarray, m_nVertices*m_nPatchesPerRow*(m_nPatchesPerRow>>1)*sizeof(TESSVERTEX));
	if (!m_pVB->Unlock())
	{
		WriteToLog("%d: Failed to unlock m_pVB.\n", tc.nTest);
		delete [] VBarray;
		return false;
	}
	delete [] VBarray;

	/*//create colors buffer
	PBYTE pBuffer = NULL;
	DWORD nSize = GetVertexSize(D3DFVF_DIFFUSE);
	if(!CreateVertexBuffer(m_uBufferHeight*m_uBufferWidth*nSize, D3DUSAGE_RTPATCHES, D3DFVF_DIFFUSE, D3DPOOL_SYSTEMMEM, &m_ColorBuffer))
	{
		WriteToLog("%d: Failed to create color buffer.\n", tc.nTest);
		return false;
	}
	if(!m_ColorBuffer->Lock(0 ,0, &pBuffer, 0))
	{
		WriteToLog("%d: Failed to lock color buffer.\n", tc.nTest);
		return false;
	}
	D3DCOLOR* pColor = (D3DCOLOR*)pBuffer;
    for(int i=0; i<m_uBufferHeight*m_uBufferWidth; i++)
	{
		DWORD dwVal = i * (0xff/(m_uBufferHeight*m_uBufferWidth));
		*pColor = D3DCOLOR_XRGB(dwVal, 0, 0xff - dwVal);//dwVal, 0xff - dwVal, 0);
		pColor++;
	}
    if (!m_ColorBuffer->Unlock())
	{
		WriteToLog("%d: Failed to unlock color buffer.\n", tc.nTest);
		return false;
	}*/
	return true;
}

bool CTessFactorTriTest::GenerateVertexShader()
{
	HRESULT hr;
	LPD3DXBUFFER pShader = NULL;

	CAPS caps;
	m_pSrcDevice->GetDeviceCaps(&caps);    // initialize m_pd3dDevice before using
	if( caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
		return false;

	VERTEXELEMENT decl[] = 
	{
		{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
		{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
		//{0, 24, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
		{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
		{0, 32, (DECLTYPE)D3DDECLTYPE_FLOAT1,   DECLMETHOD_DEFAULT, D3DDECLUSAGE_TESSFACTOR, 0},
		DECL_END()
	};

	if(!CreateVertexDeclaration(decl, &(m_pDecl)))
	{
		WriteToLog("GenerateVertexShader - CreateVertexDeclaration() failed.\n");
		return false;
	}

	if (!m_UseDeclShader)
		return true;

	TCHAR *szShader = _T(
		"vs.1.1            \n"
		"dcl_position v0   \n"
		"dcl_normal   v1   \n"
		//"dcl_diffuse  v2 \n"
		"dcl_texcoord v2   \n"

		"m4x4 oPos,   v0,  c0  \n"
		"dp3  r0.x,   v1, c7  \n"    // perform lighting N dot L calculation in world space
		                              
		"mul    r1, r0.x,  c4  \n"	  //calculate pixel color from light intensity and interpolated diffuse vertex color 
		"add   oD0,   r1,  c6  \n"    //add ambient color -> final pixel color
		"mov   oT0,   v2       \n"
	);
			
	hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
	if (FAILED(hr))
	{
		WriteToLog(_T("Tessellation::GenerateVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
					m_pD3D->HResultToString(hr));
		RELEASE(m_pDecl);
		return false;
	}
	
	if (!CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &(m_pShader)))
	{
		WriteToLog("Tessellation::GenerateVertexShader -- CreateVertexShader() failed.\n");
		pShader->Release();
		RELEASE(m_pDecl);
		return false;
	}
	pShader->Release();

	return true;
}

bool CTessFactorTriTest::CapsCheck()
{
	Tessellation::CapsCheck();
	CAPS caps;
	m_pSrcDevice->GetDeviceCaps(&caps);
	//if tri patches aren't supported -> skip
	if(!(caps.DevCaps & D3DDEVCAPS_RTPATCHES))
	{
        WriteToLog("CapsCheck: tri patches are not supported.\n");
        return false;
    } 
	//if use adaptive tesselation but it's not supported for this kind of patches -> skip
	if (m_AdapTess && !(caps.DevCaps2 & D3DDEVCAPS2_ADAPTIVETESSRTPATCH))
	{
        WriteToLog("CapsCheck: adaptive tesselation is not supported for tri patches.\n");
        return false;
    }
	
	m_TessLevels[0][0] = 1.f;
	m_TessLevels[0][1] = 1.f;
	m_TessLevels[0][2] = 1.f;
	m_TessLevels[1][0] = m_MaxTess-1.f;
	m_TessLevels[1][1] = m_MaxTess/3.f-1.f;
	m_TessLevels[1][2] = m_MaxTess/3.f+1.f;
	m_TessLevels[2][0] = m_MaxTess/3.f;
	m_TessLevels[2][1] = m_MaxTess-2.f;
	m_TessLevels[2][2] = m_MaxTess;

	return true;
}

bool CTessFactorTriTest::Setup()
{
	Tessellation::Setup();
	
	//set vertex shaders
	if ( !GenerateVertexShader() )
	{
		WriteToLog("CTessFactorTriTest::Setup - Failed to create vertex shader.\n");
		return false;
	}

	if ( !SetVertexDeclaration(m_pDecl) )
	{
		WriteToLog("CTessFactorTriTest::Setup - SetVertexDeclaration(m_pDecl) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
		return false;
	}
	if (m_UseDeclFixed)
	{
		if ( !SetVertexShader((CnVertexShader*)NULL) )
		{
			WriteToLog("CTessFactorTriTest::Setup - SetVertexShader((CnVertexShader*)NULL) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	else
	{
		if ( !SetVertexShader(m_pShader) )
		{
			WriteToLog("CTessFactorTriTest::Setup - SetVertexShader(m_pShader) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	if( KeySet(_T("solid")) )
		SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_SOLID);
	else
        SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_WIREFRAME);
	
	if ( !KeySet(_T("tex")) )
		SetTexture( 0, NULL );

	return true;
}

void CTessFactorTriTest::Cleanup()
{
	Tessellation::Cleanup();
	
	RELEASE(m_pVB);
	RELEASE(m_pShader);
	RELEASE(m_pDecl);

	for( UINT i = 0; i < m_numPatches; i++ ) 
        DeletePatch(SURFACEHANDLEBASE + i);
	m_numPatches = 0;
}