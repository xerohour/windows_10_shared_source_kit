#define D3D_OVERLOADS

#include "Tessellation.h"
#include "cd3dx.h"		//for D3DXAssembleShader9

CMinMaxNTest::CMinMaxNTest()
{
	m_szTestName = "MinMax N-Patch Test";
	m_szCommandKey = "mmn";

	m_pVCircle  = NULL;
	m_pVEllipse = NULL;
	m_pIEllipse = NULL;
	m_pShader   = NULL;
	m_pDecl     = NULL;

	//We want to compare HAL against HAL and set diff tess levels on these devices. 
	//Otherwise, it would be hard to avaluate MaxTessLevel
	m_Options.D3DOptions.bRefEqualsSrc = true;
	m_Options.D3DOptions.nBackBufferWidth  = 160;
	m_Options.D3DOptions.nBackBufferHeight = 120;
	
}

// ----------------------------------------------------------------------------

UINT CMinMaxNTest::TestInitialize(void)
{
	HRESULT hr;
	
	m_nDegrees = sizeof(Degrees) / sizeof(D3DDEGREETYPE);

	tcCount.isEntire   = 1;
	tcCount.NormDegree = m_nDegrees - 1;
	tcCount.PosDegree  = m_nDegrees - 1;
	tcCount.PatchStyle = 1;
	tcCount.TessLevels = (UINT) m_MaxTess - 1;
	tcCount.VPMethod   = 3;
	
	NEWTESTCASE tc;
    tc.nTest = 0;
	ClearRangeList();
	
	for ( tc.VPMethod = 0; tc.VPMethod <= tcCount.VPMethod; tc.VPMethod++ ) //method
	{
		if (( m_UseFVFFixed   && tc.VPMethod == 0 ) ||
		    ( m_UseDeclShader && tc.VPMethod == 1 ) ||
		    ( m_UseDeclFixed  && tc.VPMethod == 2 ) ||
		    ( m_UseFVFShader  && tc.VPMethod == 3 )  )
		for ( tc.TessLevels = 0; tc.TessLevels <= tcCount.TessLevels; tc.TessLevels++ ) //tessLevel
		{
			for ( tc.PatchStyle = 0; tc.PatchStyle <= tcCount.PatchStyle; tc.PatchStyle++ ) //patch style
			{
                for ( tc.PosDegree = 0; tc.PosDegree <= tcCount.PosDegree; tc.PosDegree++ ) //position degree
				{
					if (Degrees[tc.PosDegree] == D3DDEGREE_LINEAR || Degrees[tc.PosDegree] == D3DDEGREE_CUBIC )
						for ( tc.NormDegree = 0; tc.NormDegree <= tcCount.NormDegree; tc.NormDegree++ ) //normal degree
						{
							if (Degrees[tc.NormDegree] == D3DDEGREE_LINEAR || Degrees[tc.NormDegree] == D3DDEGREE_QUADRATIC )
								AddTestRange( (tc.nTest + 1), (tc.nTest + tcCount.isEntire + 1) );
							//need to break out of NormDegree loop manually
							//it ocupies only two bits -> can only have values up to 3 and we can get into infinite loop
							if ( 3 == tc.NormDegree )
								break;
						}
					//need to break out of PosDegree loop manually
					//it ocupies only two bits -> can only have values up to 3 and we can get into infinite loop
					if ( 3 == tc.PosDegree )
						break;
				}
				//need to break out of PatchStyle loop manually
				//it ocupies only one bit -> can only have values 0 or 1 and we can get into infinite loop
				if ( 1 == tc.PatchStyle )
					break;
			}
		}
		//need to break out of VPMethod loop manually
		//it ocupies only two bits -> can only have values up to 3 and we can get into infinite loop
		if ( 3 == tc.VPMethod )
			break;
	}
	
	return D3DTESTINIT_RUN;
}

// ----------------------------------------------------------------------------

bool CMinMaxNTest::ExecuteTest(UINT iTest)
{
	NEWTESTCASE tc;
	tc.nTest = iTest-1;
	if(	tc.VPMethod   > tcCount.VPMethod   ||
		tc.TessLevels > tcCount.TessLevels ||
		tc.PatchStyle > tcCount.PatchStyle ||
		tc.NormDegree > tcCount.NormDegree ||
		tc.PosDegree  > tcCount.PosDegree  ||
		tc.isEntire   > tcCount.isEntire    )
    {
        WriteToLog( "No such test (%d)\n", iTest);
		return false;
    }

	float TessLevel = 1.f + (float)tc.TessLevels;
	TessLevel = min (TessLevel, m_MaxTess);
	SetRenderState(D3DRS_POSITIONDEGREE, Degrees[tc.PosDegree]);
	SetRenderState(D3DRS_NORMALDEGREE, Degrees[tc.NormDegree]);
	SetRenderState(D3DRS_PATCHEDGESTYLE, tc.PatchStyle );
	SetRenderState(D3DRS_MINTESSELLATIONLEVEL, *(DWORD*)&TessLevel);
	SetRenderState(D3DRS_MAXTESSELLATIONLEVEL, *(DWORD*)&TessLevel);
	m_pSrcDevice->SetNPatchMode( max(1.f, TessLevel - 2.f));
	if ( NULL != m_pRefDevice )
		m_pRefDevice->SetNPatchMode( TessLevel + 2.f);
		
	sprintf( msgString, "%d patches, %s Pos, %s Norm, %s style, Tess=%.2f, %s-%s",
		tc.isEntire ? 1 : 4, 
		Degrees[tc.PosDegree] == D3DDEGREE_LINEAR ? "LINEAR" : "CUBIC",
		Degrees[tc.NormDegree] == D3DDEGREE_LINEAR ? "LINEAR" : "QUADRATIC",
		tc.PatchStyle == D3DPATCHEDGE_DISCRETE ? "DISCRETE" : "CONTINUOUS",
		TessLevel,
		(tc.VPMethod == 0 || tc.VPMethod == 3) ? "fvf" : "decl",
		(tc.VPMethod == 0 || tc.VPMethod == 2) ? "fixed" : "shader" );

	BeginTestCase( msgString, iTest);
	
	if (tc.VPMethod == 0 || tc.VPMethod == 3)
	{
		if ( !SetVertexShader(FVF_VERTEX) ) 
		{
			WriteToLog("SetVertexShader(FVF_VERTEX) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	else
	{
		if ( !SetVertexDeclaration(m_pDecl) )
		{
			WriteToLog("SetVertexDeclaration(m_pDecl) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	if (tc.VPMethod == 0 || tc.VPMethod == 2)
	{
		if ( !SetVertexShader((CnVertexShader*)NULL) )
		{
			WriteToLog("SetVertexShader((CnVertexShader*)NULL) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	else
	{
		if ( !SetVertexShader(m_pShader) )
		{
			WriteToLog("SetVertexShader(m_pShader) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}

	BeginScene();

	if (KeySet("m0"))
		SetNewMaterial(0);
	else if (KeySet("m1"))
		SetNewMaterial(1);
	else if (KeySet("m6"))
		SetNewMaterial(6);
	else
		SetNewMaterial(2);
	if (!KeySet("!k"))
	{
		if (tc.isEntire)
		{
            if ( !DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 4, m_pIEllipse, FMT_INDEX16, m_pVEllipse, sizeof(VERTEX)) )
				WriteToLog("%d DrawPrimitiveUP failed.\n", iTest);
		}
		else
		{
			 if ( !DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 1, m_pIEllipse, FMT_INDEX16, m_pVEllipse, sizeof(VERTEX)) )
				WriteToLog("%d DrawPrimitiveUP failed.\n", iTest);
			 if ( !DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 1, m_pIEllipse+3, FMT_INDEX16, m_pVEllipse, sizeof(VERTEX)) )
				WriteToLog("%d DrawPrimitiveUP failed.\n", iTest);
			 if ( !DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 1, m_pIEllipse+6, FMT_INDEX16, m_pVEllipse, sizeof(VERTEX)) )
				WriteToLog("%d DrawPrimitiveUP failed.\n", iTest);
			 if ( !DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 1, m_pIEllipse+9, FMT_INDEX16, m_pVEllipse, sizeof(VERTEX)) )
				WriteToLog("%d DrawPrimitiveUP failed.\n", iTest);
		}
	}

	EndScene();
	return true;
}
             
// ----------------------------------------------------------------------------

bool CMinMaxNTest::GenerateBuffers()
{
	VOID   *pV = NULL;

	float a = 4.f;
	float cosPI_4 = cos(D3DX_PI/4.f);
	float sinPI_4 = sin(D3DX_PI/4.f);

	m_pVCircle = new VERTEX[4];
	m_pVCircle[0] = VERTEX(D3DXVECTOR3( -cosPI_4, sinPI_4, 0.f ), D3DXVECTOR3( -cosPI_4, sinPI_4, 0.f ), 0.f, 0.f); 
	m_pVCircle[1] = VERTEX(D3DXVECTOR3( cosPI_4, sinPI_4, 0.f ), D3DXVECTOR3( cosPI_4, sinPI_4, 0.f ), 0.f, 0.f); 
	m_pVCircle[2] = VERTEX(D3DXVECTOR3( -cosPI_4, -sinPI_4, 0.f  ), D3DXVECTOR3( -cosPI_4, -sinPI_4, 0.f ), 0.f, 0.f); 
	m_pVCircle[3] = VERTEX(D3DXVECTOR3( cosPI_4, -sinPI_4, 0.f ), D3DXVECTOR3( cosPI_4, -sinPI_4, 0.f ), 0.f, 0.f); 
	
	m_pVEllipse = new VERTEX[5];

	m_pVEllipse[0] = VERTEX(D3DXVECTOR3( 0.f, 0.f, -a ), D3DXVECTOR3( 0.f, 0.f, -1.f ), 0.f, 0.f); 
	m_pVEllipse[1] = VERTEX(D3DXVECTOR3( -cosPI_4, sinPI_4, 0.f ), D3DXVECTOR3( -cosPI_4, sinPI_4, 0.f ), 0.f, 0.f); 
	m_pVEllipse[2] = VERTEX(D3DXVECTOR3( cosPI_4, sinPI_4, 0.f ), D3DXVECTOR3( cosPI_4, sinPI_4, 0.f ), 0.f, 0.f); 
	m_pVEllipse[3] = VERTEX(D3DXVECTOR3( -cosPI_4, -sinPI_4, 0.f ), D3DXVECTOR3( -cosPI_4, -sinPI_4, 0.f ), 0.f, 0.f); 
	m_pVEllipse[4] = VERTEX(D3DXVECTOR3( cosPI_4, -sinPI_4, 0.f  ), D3DXVECTOR3( cosPI_4, -sinPI_4, 0.f ), 0.f, 0.f); 

	m_pIEllipse = new WORD[12];
	m_pIEllipse[0] = 1;
	m_pIEllipse[1] = 2;
	m_pIEllipse[2] = 0;
	m_pIEllipse[3] = 2;
	m_pIEllipse[4] = 4;
	m_pIEllipse[5] = 0;
	m_pIEllipse[6] = 0;
	m_pIEllipse[7] = 4;
	m_pIEllipse[8] = 3;
	m_pIEllipse[9] = 1;
	m_pIEllipse[10] = 0;
	m_pIEllipse[11] = 3;
	
	return true;
}

// ----------------------------------------------------------------------------

bool CMinMaxNTest::GenerateVertexShader()
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
		//"mov oD0, c4 \n"
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

// ----------------------------------------------------------------------------

bool CMinMaxNTest::CapsCheck()
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
	if (!m_AdapTess || !(caps.DevCaps2 & D3DDEVCAPS2_ADAPTIVETESSNPATCH)) 
	{
        WriteToLog("CapsCheck: adaptive tesselation is not supported for N-patches.\n");
        return false;
    }

	m_nTessLevels = (DWORD)m_MaxTess;
	
	return true;
}

// ----------------------------------------------------------------------------

bool CMinMaxNTest::Setup()
{
	//Tessellation::Setup();
	SetDefaultMatrices();
	SetDefaultRenderStates();
	SetDefaultLights();

	float lightDir[4] = {0.f, 0.0f, -1.0f, 0}; 
	SetVertexShaderConstant( 7, lightDir, 1 );

	//set vertex shaders
	if ((m_UseDeclShader || m_UseDeclFixed || m_UseFVFShader) && !GenerateVertexShader())
	{
		WriteToLog("CMinMaxNTest::Setup - GenerateVertexShader failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
	}
	
	//generate D3DX tessellated mesh
	if (!GenerateBuffers())
		return false;

	D3DXMATRIX Matrix, Matrix1;
	float fZn = -5.f;
	float fZf = 5.f;
	D3DXMatrixOrthoLH(&m_ProjMatrix, 2.f, 2.f, fZn, fZf);
		
	D3DXVECTOR3 vFrom = D3DXVECTOR3( 0.0f,  0.0f, -5.0f);
	D3DXVECTOR3 vAt   = D3DXVECTOR3( 0.0f,  0.0f, 0.0f);
	D3DXVECTOR3 vUp   = D3DXVECTOR3( 0.0f,  1.0f, 0.0f);	
	
	D3DXMatrixLookAtLH(&(m_ViewMatrices[0]), &vFrom, &vAt, &vUp);
	D3DXMatrixMultiply(&(m_WVMatrices[0]), &m_WorldMatrix, &(m_ViewMatrices[0]));
	D3DXMATRIX mat;
	D3DXMatrixMultiply( &mat, &m_WVMatrices[0], &m_ProjMatrix);
	D3DXMatrixTranspose( &mat, &mat );   
	SetVertexShaderConstant(0, &mat, 4); 
	SetTransform(D3DTS_VIEW, &m_ViewMatrices[0]);
	SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);
	SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	
	float AdaptiveTess_x = 0.f;
	float AdaptiveTess_y = 0.f;
	float AdaptiveTess_z = -0.5f;//-2/a  //2.f;
	float AdaptiveTess_w = 1.f;
	SetRenderState(D3DRS_ADAPTIVETESS_X, *(DWORD*)&AdaptiveTess_x);
	SetRenderState(D3DRS_ADAPTIVETESS_Y, *(DWORD*)&AdaptiveTess_y);
	SetRenderState(D3DRS_ADAPTIVETESS_Z, *(DWORD*)&AdaptiveTess_z);
	SetRenderState(D3DRS_ADAPTIVETESS_W, *(DWORD*)&AdaptiveTess_w);
	return true;
}

// ----------------------------------------------------------------------------

void CMinMaxNTest::Cleanup()
{
	Tessellation::Cleanup();

	delete [] m_pVCircle;
	m_pVCircle = NULL;
	delete [] m_pVEllipse;
	m_pVEllipse = NULL;
	delete [] m_pIEllipse;
	m_pIEllipse = NULL;
	RELEASE(m_pShader);
	RELEASE(m_pDecl);
}

