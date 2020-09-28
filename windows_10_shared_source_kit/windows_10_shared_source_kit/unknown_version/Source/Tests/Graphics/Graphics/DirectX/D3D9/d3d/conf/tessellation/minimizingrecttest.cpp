#define D3D_OVERLOADS

#include "Tessellation.h"
#include "cd3dx.h"		//for D3DXAssembleShader9

CMinimizingRectTest::CMinimizingRectTest()
{
	m_szTestName = "Minimizing Rect Test";
	m_szCommandKey = "RectMin";

	m_pVB = NULL;
	m_pShader = NULL;
	m_pDecl = NULL;

	m_UseDeclShader = false;
	m_UseFVFShader  = false;
	m_UseFVFFixed   = false;
	m_UseDeclFixed  = false;
	if( !KeySet(_T("decl-sh" )) && 
		!KeySet(_T("decl-fix")) && 
		!KeySet(_T("fvf-fix" )) && 
		!KeySet(_T("fvf-sh"  ))  )
		m_UseFVFFixed   = true;
	else if ( KeySet(_T("decl-sh")) )
		m_UseDeclShader = true;
	else if ( KeySet(_T("fvf-sh")) )
		m_UseFVFShader  = true;
	else if ( KeySet(_T("decl-fix")) )
		m_UseDeclFixed  = true;
	else
		m_UseFVFFixed = true;
}

UINT CMinimizingRectTest::TestInitialize(void)
{
	HRESULT hr;
	m_nBasises = sizeof(Basises) / sizeof(D3DBASISTYPE);
	m_nDegrees = sizeof(Degrees) / sizeof(D3DDEGREETYPE);
	m_nTessLevels = 3;//sizeof(RectMinTessLevels) / (4*sizeof(float));
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
	tcCount.NumPatches = 7; 

    TESTCASE tc;
	tc.nTest = 0;

	ClearRangeList();
	
	for ( tc.TessLevels = 0; tc.TessLevels < tcCount.TessLevels; tc.TessLevels++ )
	for ( tc.NumPatches = 3; tc.NumPatches < tcCount.NumPatches; tc.NumPatches++ )
	for ( tc.Basis = 0;      tc.Basis      < tcCount.Basis;      tc.Basis++      )
	for ( tc.Degree = 0;     tc.Degree     < tcCount.Degree;     tc.Degree++     )
	{
		//quadratic degree isn't supported for rtpatches
		if (Degrees[tc.Degree] == D3DDEGREE_QUADRATIC)
			continue;
		//check caps for D3DDEGREE_QUINTIC
		if (Degrees[tc.Degree] == D3DDEGREE_QUINTIC && !m_UseQuinticRT)
			continue;
		//for rect patches D3DBASIS_CATMULL_ROM can only be used with D3DDEGREE_CUBIC
		if (Basises[tc.Basis] == D3DBASIS_CATMULL_ROM && Degrees[tc.Degree] != D3DDEGREE_CUBIC)
			continue;
		tc.View = 0;
		AddTestRange( (tc.nTest + 1), (tc.nTest + tcCount.View) );
	}
	
	return D3DTESTINIT_RUN;
}

bool CMinimizingRectTest::ExecuteTest(UINT iTest)
{
	TESTCASE tc;
	tc.nTest = iTest-1;
	DWORD NumPatches = 1 << tc.NumPatches;
	if(	tc.NumPatches >= tcCount.NumPatches ||
		tc.View       >= tcCount.View       ||
		tc.View       >= MAX_VIEW           || //prefast error
        tc.Degree     >= tcCount.Degree     ||
        tc.Basis      >= tcCount.Basis      ||
		tc.TessLevels >= tcCount.TessLevels  )
    {
        WriteToLog( "No such test (%d)\n", iTest);
		return false;
    }

	for( UINT i = 0; i < m_numPatches; i++ )
	{
		DeletePatch(SURFACEHANDLEBASE + i);
	}
	m_numPatches = 0;

	RELEASE(m_pVB);
	//RELEASE(m_ColorBuffer);
	if (!GenerateBuffers(tc))
	{
		WriteToLog("%d Failed to create the vertex buffer.\n", iTest);
		SkipTestRange(iTest, iTest+4);
		return false;
	}
	sprintf( msgString, TEXT("%d patches, %s Basis, %s Degree, w=%d, h=%d, Tess=(%.2f, %.2f)"),
		NumPatches, BasisStr[tc.Basis], DegreeStr[tc.Degree], m_uPatchWidth, m_uPatchHeight,
		m_TessLevels[tc.TessLevels][0], m_TessLevels[tc.TessLevels][1]
	);
	
	BeginTestCase( msgString, iTest);

	D3DRECTPATCH_INFO RectInfo;
	float f[4];
	f[0]=m_TessLevels[tc.TessLevels][0]; 
	f[1]=m_TessLevels[tc.TessLevels][1]; 
	f[2]=m_TessLevels[tc.TessLevels][0]; 
	f[3]=m_TessLevels[tc.TessLevels][1];

	if ( !SetStreamSource(0, m_pVB, sizeof(VERTEX)) )
		WriteToLog("%d Set stream source failed.\n", iTest);

	//set stransformation matrices
	D3DXMATRIX mat;
	if ( m_UseFVFShader || m_UseDeclShader )
	{
		D3DXMatrixMultiply( &mat, &m_ViewMatrices[tc.View], &m_ProjMatrix); 
		D3DXMatrixTranspose( &mat, &mat );   
		SetVertexShaderConstant(0, &mat, 4); 
	}
	else
	{
		mat = m_ViewMatrices[tc.View];
		SetTransform(D3DTS_VIEW, &mat); 
	}

	BeginScene();

	float AdaptiveTess_x = m_WVMatrices[tc.View].m[0][2];
	float AdaptiveTess_y = m_WVMatrices[tc.View].m[1][2];
	float AdaptiveTess_z = m_WVMatrices[tc.View].m[2][2];
	float AdaptiveTess_w = m_WVMatrices[tc.View].m[3][2];
	
	SetRenderState(D3DRS_ADAPTIVETESS_X, *(DWORD*)&AdaptiveTess_x);
	SetRenderState(D3DRS_ADAPTIVETESS_Y, *(DWORD*)&AdaptiveTess_y);
	SetRenderState(D3DRS_ADAPTIVETESS_Z, *(DWORD*)&AdaptiveTess_z);
	SetRenderState(D3DRS_ADAPTIVETESS_W, *(DWORD*)&AdaptiveTess_w);

	m_numPatches = NumPatches;
	for( int i = 0; i < m_numPatches; i++ ) 
	{
		SetNewMaterial(i);
		//memset(&RectInfo, 0, sizeof(D3DRECTPATCH_INFO));
		if (tc.Basis == D3DBASIS_BEZIER)
		{
       		RectInfo.StartVertexOffsetWidth  =  (i * (m_uPatchWidth-1) )%(m_uBufferWidth-1);
			RectInfo.StartVertexOffsetHeight = ((i * (m_uPatchWidth-1) )/(m_uBufferWidth-1))*(m_uPatchHeight-1);
		}
		else
		{
			RectInfo.StartVertexOffsetWidth  =  (i * (m_uPatchWidth-Degrees[tc.Degree]) )%(m_uBufferWidth-Degrees[tc.Degree]);
			RectInfo.StartVertexOffsetHeight = ((i * (m_uPatchWidth-Degrees[tc.Degree]) )/(m_uBufferWidth-Degrees[tc.Degree]))*(m_uPatchHeight-Degrees[tc.Degree]);
		}
		RectInfo.Width = m_uPatchWidth;
		RectInfo.Height = m_uPatchHeight;
		RectInfo.Stride = m_uBufferWidth; // verticies to next row of verticies
		RectInfo.Basis = Basises[tc.Basis]; 
		RectInfo.Degree = Degrees[tc.Degree];
		if ( !DrawRectPatch(SURFACEHANDLEBASE+i, f, &RectInfo))
			WriteToLog("%d DrawRectPatch failed.\n", iTest);
	}
	
	EndScene();
	return true;
}

bool CMinimizingRectTest::GenerateBuffers(TESTCASE tc)
{
	VOID *pV = NULL;
	float x, y, z;
	VERTEX *VBarray = NULL;
	UINT k = 1<<tc.NumPatches;
	m_nPatchesPerRow = (UINT) ceil(sqrt(float(k)));
	if (tc.Basis == D3DBASIS_BEZIER)
		m_uPatchWidth = m_uPatchHeight = Degrees[tc.Degree]+1;
	else if (tc.Basis == D3DBASIS_BSPLINE)
	{
		m_uPatchWidth  = Degrees[tc.Degree]+1;//((tcCount.NumPatches>>1)-tc.NumPatches);
		m_uPatchHeight = Degrees[tc.Degree]+1;//(tc.NumPatches);
	}
	else
	{
		m_uPatchWidth  = Degrees[tc.Degree]+1;//(tc.NumPatches);
		m_uPatchHeight = Degrees[tc.Degree]+1;//((tcCount.NumPatches>>1)-tc.NumPatches);
	}
	m_nVertices = m_uPatchWidth * m_uPatchHeight;
	if (tc.Basis == D3DBASIS_BEZIER)
	{
		m_uBufferWidth = m_uPatchWidth  + (m_uPatchWidth - 1)*(m_nPatchesPerRow - 1);
		m_uBufferHeight =m_uPatchHeight + (m_uPatchHeight - 1)*(m_nPatchesPerRow - 1);
	}
	else
	{
		m_uBufferWidth = m_uPatchWidth  + (m_uPatchWidth - Degrees[tc.Degree])*(m_nPatchesPerRow - 1);
		m_uBufferHeight =m_uPatchHeight + (m_uPatchHeight - Degrees[tc.Degree])*(m_nPatchesPerRow - 1);
	}

	VBarray = new VERTEX[m_uBufferHeight*m_uBufferWidth];
	
	if( !CreateVertexBuffer( m_uBufferHeight*m_uBufferWidth*sizeof(VERTEX), D3DUSAGE_RTPATCHES, m_UseFVFFixed || m_UseFVFShader ? FVF_VERTEX : 0, POOL_MANAGED, &m_pVB )) 
	{
		WriteToLog("%d: Can't create vertex buffer.\n", tc.nTest);
		return false;
	}
	if (!m_pVB->Lock( 0, m_uBufferHeight*m_uBufferWidth*sizeof(VERTEX), (BYTE**)&pV, 0))
	{
		WriteToLog("%d: Failed to lock m_pVB.\n", tc.nTest);
		return false;
	}
	for (int j = 0; j < m_uBufferHeight; j++)
	{
		y = 1.f - 2.f*((float)j/(m_uBufferHeight   -1.f));
		if (tc.View == 1)
			y = 3.f - 6.f*((float)j/(m_uBufferHeight   -1.f));
		else 
			y = 1.f - 2.f*((float)j/(m_uBufferHeight   -1.f));
		for (int i = 0; i < m_uBufferWidth; i++)
		{
			if (tc.View == 0)
				x = -3.f + 5.f*((float)i/(m_uBufferWidth-1.f));
			else 
				x = -1.f + 2.f*((float)i/(m_uBufferWidth-1.f));
			z = 1;
			VBarray[j*m_uBufferWidth+i] = VERTEX(D3DXVECTOR3( x, y, z), D3DXVECTOR3( 0.f, 0.f, -1.f), (1.f+x)/2.f, (1.f-y)/2.f); // 0xffff0000);
		}
	}
	memcpy( pV, VBarray, m_uBufferHeight*m_uBufferWidth*sizeof(VERTEX));
	if (!m_pVB->Unlock())
	{
		WriteToLog("%d: Failed to unlock m_pVB.\n", tc.nTest);
		return false;
	}
	delete [] VBarray;
	return true;
}


bool CMinimizingRectTest::GenerateVertexShader()
{
	HRESULT hr;
	LPD3DXBUFFER pShader = NULL;

	//create declaration
	if ( m_UseDeclFixed || m_UseDeclShader )
	{
		VERTEXELEMENT decl[] = 
		{
			{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
			//{0, 24, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
			{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
			DECL_END()
		};

		if( !CreateVertexDeclaration( decl, &(m_pDecl)) )
		{
			WriteToLog( "GenerateVertexShader - CreateVertexDeclaration() failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()) );
			return false;
		}
	}

	if (!m_UseDeclShader && !m_UseFVFShader )
		return true;

	CAPS caps;
	m_pSrcDevice->GetDeviceCaps(&caps);    // initialize m_pd3dDevice before using
	if( caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
	{
		WriteToLog("GenerateVertexShader: vertex shaders version 1.1 is not supported\n");
		return false;
	}

	if( caps.MaxVertexShaderConst < 7 )
	{
		WriteToLog("GenerateVertexShader: only %d vertex shader constants are supported. need at least 7\n", caps.MaxVertexShaderConst);
		return false;
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
		return false;
	}
		
	if ( !CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &m_pShader) )
	{
		WriteToLog("GenerateVertexShader::CreateVertexShader() failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
		pShader->Release();
		return false;
	}
	pShader->Release();

	return true;
}

bool CMinimizingRectTest::CapsCheck()
{
	Tessellation::CapsCheck();
	CAPS caps;
	m_pSrcDevice->GetDeviceCaps(&caps);
	//if this rect patches aren't supported -> skip the test
	if(!(caps.DevCaps & D3DDEVCAPS_RTPATCHES))
	{
        WriteToLog("CapsCheck: rect patches are not supported.\n");
        return false;
    } 

	//if use adaptive tesselation but it's not supported for rect patches -> skip
	if (m_AdapTess && !(caps.DevCaps2 & D3DDEVCAPS2_ADAPTIVETESSRTPATCH)) 
	{
        WriteToLog("CapsCheck: adaptive tesselation is not supported for rect patches.\n");
        return false;
    } 
	
	m_TessLevels[0][0] = 1.f;
	m_TessLevels[0][1] = 1.f;
	m_TessLevels[1][0] = 2.f*m_MaxTess/3.f;
	m_TessLevels[1][1] = m_MaxTess;
	m_TessLevels[2][0] = m_MaxTess-1;
	m_TessLevels[2][1] = 2.f*m_MaxTess/3.f;

	return true;
}

bool CMinimizingRectTest::SetDefaultMatrices(void)
{   
	D3DXMATRIX	Matrix;
    HRESULT     hr;
	float 		fAspect = 1.33f;
	float		fFOV = pi/4.0f;
	D3DXVECTOR3 	m_vAt, m_vUp;

	m_vUp  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

    D3DXMatrixIdentity( &m_ProjMatrix );
	D3DXMatrixPerspectiveFovLH(&m_ProjMatrix, fFOV, fAspect, 1.0f, 1000.0f);
    if (!SetTransform(D3DTS_PROJECTION, &m_ProjMatrix))
        return false;
	
    D3DXMatrixIdentity( &Matrix );
	if (!SetTransform(D3DTS_WORLD, &Matrix))
        return false;

	D3DXVECTOR3 vFrom[] = {
		D3DXVECTOR3( 3.5f,  0.0f, -1.0f),
		D3DXVECTOR3( 0.0f,  5.0f, -1.0f),
	};
	D3DXVECTOR3 vAt[] = {
		D3DXVECTOR3( 1.0f, 0.0f, 1.0f),
		D3DXVECTOR3( 0.0f, 1.0f, 1.0f),
	};
	
	m_nWVMatrices = m_nViewMatrices = sizeof(vFrom)/sizeof(D3DXVECTOR3);
	for (UINT i = 0; i < m_nViewMatrices; i++)
	{
		D3DXMatrixIdentity( &Matrix );
        D3DXMatrixLookAtLH(&(m_ViewMatrices[i]), &(vFrom[i]), &vAt[i], &m_vUp);
		D3DXMatrixMultiply(&(m_WVMatrices[i]), &Matrix, &(m_ViewMatrices[i]));
	}
	
    return true;
}

bool CMinimizingRectTest::Setup()
{
	Tessellation::Setup();
	
	//set vertex shaders
	if ( !m_UseFVFFixed && !GenerateVertexShader())
	{
		WriteToLog("CMinimizingRectTest::Setup - GenerateVertexShader() failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
		return false;
	}
	if (m_UseFVFFixed || m_UseFVFShader)
	{
		if ( !SetVertexShader(FVF_VERTEX) ) 
		{
			WriteToLog("CMinimizingRectTest::Setup - SetVertexShader(FVF_VERTEX) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	else
	{
		if ( !SetVertexDeclaration(m_pDecl) )
		{
			WriteToLog("CMinimizingRectTest::Setup - SetVertexDeclaration(m_pDecl) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	if (m_UseFVFFixed || m_UseDeclFixed)
	{
		if ( !SetVertexShader((CnVertexShader*)NULL) )
		{
			WriteToLog("CMinimizingRectTest::Setup - SetVertexShader((CnVertexShader*)NULL) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	else
	{
		if ( !SetVertexShader(m_pShader) )
		{
			WriteToLog("CMinimizingRectTest::Setup - SetVertexShader(m_pShader) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
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

void CMinimizingRectTest::Cleanup()
{
	Tessellation::Cleanup();
	
	RELEASE(m_pVB);
	RELEASE(m_pShader);
	RELEASE(m_pDecl);

	for( UINT i = 0; i < m_numPatches; i++ ) 
        DeletePatch(SURFACEHANDLEBASE + i);
	m_numPatches = 0;
}

