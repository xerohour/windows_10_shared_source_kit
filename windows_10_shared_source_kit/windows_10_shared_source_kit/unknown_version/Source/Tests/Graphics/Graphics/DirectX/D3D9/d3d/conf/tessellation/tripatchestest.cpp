#define D3D_OVERLOADS

#include "Tessellation.h"
#include "cd3dx.h"		//for D3DXAssembleShader9

CTriPatchesTest::CTriPatchesTest()
{
	m_szTestName = "Tri Patches Test";
	m_szCommandKey = "Tri";

	m_pVB     = NULL;
	m_pShader = NULL;
	m_pDecl   = NULL;
}

UINT CTriPatchesTest::TestInitialize(void)
{
	HRESULT hr;
	m_nBasises = sizeof(Basises) / sizeof(D3DBASISTYPE);
	m_nDegrees = sizeof(Degrees) / sizeof(D3DDEGREETYPE);
	m_nTessLevels = 4;//sizeof(TriTessLevels) / (3*sizeof(float));
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
	}

	return D3DTESTINIT_RUN;
}

bool CTriPatchesTest::ExecuteTest(UINT iTest)
{
	TESTCASE tc;
	tc.nTest = iTest-1;
	DWORD NumPatches = 1 << tc.NumPatches;
	if(	tc.NumPatches >= tcCount.NumPatches ||
		tc.View       >= tcCount.View       ||
        tc.Degree     >= tcCount.Degree     ||
        tc.Basis      >= tcCount.Basis      ||
		tc.TessLevels >= tcCount.TessLevels ||
		tc.VPMethod   >= tcCount.VPMethod    )
    {
        WriteToLog( "No such test (%d)\n", iTest);
		return false;
    }

	if (tc.View == 0 || iTest == m_pRangeList->uStart || (m_pConstraints != NULL && iTest == m_pConstraints->uStart))
	{
		for( UINT i = 0; i < m_numPatches; i++ )
		{
			DeletePatch(SURFACEHANDLEBASE + i);
		}
		m_numPatches = 0;
		RELEASE(m_pVB);
		if (!GenerateBuffers(tc))
		{
			SkipTestRange(iTest, iTest+4);
			return false;
		}
		if ( !SetStreamSource(0, m_pVB, sizeof(VERTEX)) )
			WriteToLog("%d Set stream source failed.\n", iTest);
		sprintf( msgString, TEXT("%d %s %s patches, Tess=(%.2f, %.2f, %.2f), %s-%s")/*(%.2f, %.2f, %.2f)*/,
			NumPatches, BasisStr[tc.Basis], DegreeStr[tc.Degree],
			m_TessLevels[tc.TessLevels][0], m_TessLevels[tc.TessLevels][1], m_TessLevels[tc.TessLevels][2],
			(tc.VPMethod == 0 || tc.VPMethod == 3) ? "fvf" : "decl",
			(tc.VPMethod == 0 || tc.VPMethod == 2) ? "fixed" : "shader"
		);
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

	D3DTRIPATCH_INFO TriInfo;
	float f[3];
	f[0]=m_TessLevels[tc.TessLevels][0]; 
	f[1]=m_TessLevels[tc.TessLevels][1]; 
	f[2]=m_TessLevels[tc.TessLevels][2]; 

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

	if (tc.View == 0)
	{
		m_numPatches = NumPatches;
		for( int i = 0; i < m_numPatches; i++ ) 
		{
			SetNewMaterial(i);
			TriInfo.StartVertexOffset = i*m_nVertices;
			TriInfo.NumVertices = m_nVertices; 
			TriInfo.Basis = Basises[tc.Basis]; 
			TriInfo.Degree = Degrees[tc.Degree];
			DrawTriPatch(SURFACEHANDLEBASE+i, f, &TriInfo);
		}
	}
	else
	{
		for( int i = 0; i < m_numPatches; i++ ) 
		{
			SetNewMaterial(i);
			DrawTriPatch(SURFACEHANDLEBASE+i, f, NULL);
		}
	}
	
	EndScene();
	return true;
}

bool CTriPatchesTest::GenerateBuffers(TESTCASE tc)
{
	DWORD size = 0;
	VOID *pV = NULL;
	VERTEX *VBarray = NULL;
	float x, y, z, nx, ny, nz;
	
	m_nPatchesPerRow = ((int)ceil(sqrt((1<<tc.NumPatches)/2.f))) << 1;
	m_nVertices = ((Degrees[tc.Degree]+1)*(Degrees[tc.Degree]+2)) >> 1;
	m_uPatchHeight = m_uPatchWidth = Degrees[tc.Degree] + 1;
	m_uBufferWidth = m_uPatchWidth  + (m_uPatchWidth - 1)*((m_nPatchesPerRow>>1) - 1);
	m_uBufferHeight =m_uPatchHeight + (m_uPatchHeight -1)*((m_nPatchesPerRow>>1) - 1);
	
	if( !CreateVertexBuffer( m_nVertices*m_nPatchesPerRow*(m_nPatchesPerRow>>1)*sizeof(VERTEX), D3DUSAGE_RTPATCHES, tc.VPMethod == 1 || tc.VPMethod == 2 ? 0 : FVF_VERTEX, POOL_MANAGED, &m_pVB )) 
	{
		WriteToLog("%d: Can't create vertex buffer.\n", tc.nTest);
		return false;
	}
	if (!m_pVB->Lock( 0, m_nVertices*m_nPatchesPerRow*(m_nPatchesPerRow>>1)*sizeof(VERTEX), (BYTE**)&pV, 0))
	{
		WriteToLog("%d: Failed to lock m_pVB.\n", tc.nTest);
		return false;
	}
	VBarray = new VERTEX[m_nVertices*m_nPatchesPerRow*(m_nPatchesPerRow>>1)];
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
					
				VBarray[index++] = VERTEX(D3DXVECTOR3( x, y, z), D3DXVECTOR3( nx, ny, nz ), (1.f+x)/2.f, (1.f-y)/2.f); // 0xffff0000);

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
				
				VBarray[index++] =  VERTEX(D3DXVECTOR3( x, y, z ), D3DXVECTOR3( nx, ny, nz ), (1.f+x)/2.f, (1.f-y)/2.f); // 0xffff0000);
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
	memcpy( pV, VBarray, m_nVertices*m_nPatchesPerRow*(m_nPatchesPerRow>>1)*sizeof(VERTEX));
	if (!m_pVB->Unlock())
	{
		WriteToLog("%d: Failed to unlock m_pVB.\n", tc.nTest);
		delete [] VBarray;
		return false;
	}
	delete [] VBarray;

	return true;
}


bool CTriPatchesTest::GenerateVertexShader()
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

bool CTriPatchesTest::CapsCheck()
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
	m_TessLevels[1][1] = m_MaxTess/4.f+1.f;
	m_TessLevels[1][2] = m_MaxTess/2.f;
	m_TessLevels[2][0] = m_MaxTess/2.f-1;
	m_TessLevels[2][1] = m_MaxTess-2.f;
	m_TessLevels[2][2] = m_MaxTess/4.f;
	m_TessLevels[3][0] = m_MaxTess/4.f;
	m_TessLevels[3][1] = m_MaxTess/2.f+1;
	m_TessLevels[3][2] = m_MaxTess;

	return true;
}

bool CTriPatchesTest::Setup()
{
	Tessellation::Setup();

	//set vertex shaders
	if ((m_UseDeclShader || m_UseDeclFixed || m_UseFVFShader) && !GenerateVertexShader())
	{
		WriteToLog("CTriPatchesTest::Setup - GenerateVertexShader() failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
		return false;
	}

	return true;
}

void CTriPatchesTest::Cleanup()
{
	Tessellation::Cleanup();
	
	RELEASE(m_pVB);
	RELEASE(m_pShader);
	RELEASE(m_pDecl);

	for( UINT i = 0; i < m_numPatches; i++ ) 
        DeletePatch(SURFACEHANDLEBASE + i);
	m_numPatches = 0;
}

