#define D3D_OVERLOADS

#include "Tessellation.h"
#include "cd3dx.h"		//for D3DXAssembleShader9
 
CMaxMinRectTest::CMaxMinRectTest()
{
	m_szTestName = "MaxMin Rect Patches Test";
	m_szCommandKey = "MaxMinRect";

	m_pVB = NULL;
	m_pShader = NULL;
	m_pDecl = NULL;

	m_nTessLevels = 14;
	for (int i = 0; i < m_nTessLevels; i++)
		m_TessLevels[i] = 1.f;

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

UINT CMaxMinRectTest::TestInitialize(void)
{
	HRESULT hr;
	m_nBasises = sizeof(Basises) / sizeof(D3DBASISTYPE);
	m_nDegrees = sizeof(Degrees) / sizeof(D3DDEGREETYPE);
	//m_nTessLevels = 0;
	m_nMaxMinTessLevels = MAX_TESS_LEVEL;//sizeof(MaxMinRectTessLevels) / (2*sizeof(float));
	m_nPatchesPerRow = 0;
	m_nVertices = 0;
	m_uPatchWidth = 0;
	m_uPatchHeight = 0;
	m_uBufferWidth = 0;
	m_uBufferHeight = 0;
	m_doComparison = true;

	//tcCount.View = m_nViewMatrices; 
	tcCount.MaxMinTessLevels = m_nMaxMinTessLevels;
	//tcCount.Degree = m_nDegrees;
	//tcCount.Basis = m_nBasises;
	//tcCount.NumPatches = 2 << 4; 

    MAXMINTESTCASE tc;
	tc.nTest = 0;

	ClearRangeList();
	
	for( tc.MaxMinTessLevels = 0; tc.MaxMinTessLevels < tcCount.MaxMinTessLevels; tc.MaxMinTessLevels++ )
	{
		//invalid tess levels min < 1 or max < 1
		if ( m_MaxMinTessLevels[tc.MaxMinTessLevels][0] < 1.f ||
			 m_MaxMinTessLevels[tc.MaxMinTessLevels][1] < 1.f  )
		{
			if ( !m_pD3D->GetDebug() )
				continue;
			tcCount.TessLevels = 1; //only need one case to varify failtures
			tcCount.NumPatches = 1;
			tcCount.Degree = 1;
			tcCount.Basis = 1;
			tcCount.View = 1;
			//m_TessLevels[0] = m_TessLevels[1] = 1;
		}
		//invalid tess levels min > max
		else if (m_MaxMinTessLevels[tc.MaxMinTessLevels][0] > m_MaxMinTessLevels[tc.MaxMinTessLevels][1] )
		{
			tcCount.TessLevels = 1; //only need one case to varify failtures
			tcCount.NumPatches = 1;
			tcCount.Degree = 1;
			tcCount.Basis = 1;
			tcCount.View = 1;
			//m_TessLevels[0] = m_TessLevels[1] = 1;
		}
		//for valid tessellation values
		else
		{
			tcCount.TessLevels = m_nTessLevels / 2;
			tcCount.NumPatches = 5;
			tcCount.Degree = m_nDegrees;
			tcCount.Basis = m_nBasises;
			tcCount.View = m_nViewMatrices;
			//ChooseTessLevels(tc);
		}
		for ( tc.TessLevels = 0; tc.TessLevels < tcCount.TessLevels; tc.TessLevels++ )
		for ( tc.NumPatches = 0; tc.NumPatches < tcCount.NumPatches; tc.NumPatches++ )
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
	}

	return D3DTESTINIT_RUN;
}

bool CMaxMinRectTest::ExecuteTest(UINT iTest)
{
	MAXMINTESTCASE tc;
	tc.nTest = iTest-1;
	DWORD NumPatches = 1 << tc.NumPatches;
	if(	tc.MaxMinTessLevels >= tcCount.MaxMinTessLevels ||
		tc.MaxMinTessLevels >= MAX_TESS_LEVEL           || //prefast error
		tc.NumPatches >= tcCount.NumPatches             ||
		tc.View       >= tcCount.View                   ||
        tc.Degree     >= tcCount.Degree                 ||
        tc.Basis      >= tcCount.Basis                  ||
		tc.TessLevels >= tcCount.TessLevels              )
    {
        WriteToLog( "No such test (%d)\n", iTest);
		Fail();
		return false;
    }

	if (tc.TessLevels == 0 || iTest == m_pRangeList->uStart || (m_pConstraints != NULL && iTest == m_pConstraints->uStart))
	{
		//invalid tess levels
		if ( m_MaxMinTessLevels[tc.MaxMinTessLevels][0] < 1.f ||  //min < 1
			 m_MaxMinTessLevels[tc.MaxMinTessLevels][1] < 1.f ||  //max < 1
			 m_MaxMinTessLevels[tc.MaxMinTessLevels][0] > m_MaxMinTessLevels[tc.MaxMinTessLevels][1] ) //min > max
		{
			m_TessLevels[0] = m_TessLevels[1] = 1;
		}
		//for valid tessellation values
		else
		{
			ChooseTessLevels(tc);
		}
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
			WriteToLog("%d Failed to create the vertex buffer.\n", iTest);
			SkipTestRange(iTest, iTest+4);
			Fail();
			return false;
		}
		sprintf( msgString, TEXT("%d %s %s patches, w=%d, h=%d, tess=(%.2f, %.2f), Min=%.2f, Max=%.2f"),
			NumPatches, BasisStr[tc.Basis], DegreeStr[tc.Degree], m_uPatchWidth, m_uPatchHeight,
			m_TessLevels[tc.TessLevels*2+0],	m_TessLevels[tc.TessLevels*2+1],
			m_MaxMinTessLevels[tc.MaxMinTessLevels][0], m_MaxMinTessLevels[tc.MaxMinTessLevels][1]
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

	float MinTessLevel = m_MaxMinTessLevels[tc.MaxMinTessLevels][0];
	float MaxTessLevel = m_MaxMinTessLevels[tc.MaxMinTessLevels][1];
	DWORD NumPass = 1;
	//validate ligal min(max) tessellation levels
	if (MinTessLevel >= 1.f && MaxTessLevel >= 1.f && MinTessLevel <= MaxTessLevel)
	{
		if ( !SetRenderState(D3DRS_MINTESSELLATIONLEVEL, *(DWORD*)&MinTessLevel) )
		{
			WriteToLog("Failed to set MINTESSELLATIONLEVEL %d\n", (int)MinTessLevel);
			return false;
		}
		if ( !SetRenderState(D3DRS_MAXTESSELLATIONLEVEL, *(DWORD*)&MaxTessLevel) )
		{
			WriteToLog("Failed to set D3DRS_MAXTESSELLATIONLEVEL %d\n", (int)MaxTessLevel);
			return false;
		}
		float newValue;
		if ( NULL != m_pRefDevice && !m_pRefDevice->IsPure() )
		{
			if ( FAILED(m_pRefDevice->GetRenderState(D3DRS_MINTESSELLATIONLEVEL, (DWORD*)&newValue)) )
			{
				WriteToLog("Failed to get MINTESSELLATIONLEVEL %d on ref device.\n", MinTessLevel);
				return false;
			}
			if ( newValue != MinTessLevel )
			{
				WriteToLog("GetRenderState for D3DRS_MINTESSELLATIONLEVEL on ref: return value %d doesn't equal expected value %d.\n", newValue, MinTessLevel);
				Fail();
				return false;
			}
			if ( FAILED(m_pRefDevice->GetRenderState(D3DRS_MAXTESSELLATIONLEVEL, (DWORD*)&newValue)) )
			{
				WriteToLog("Failed to get MAXTESSELLATIONLEVEL %d on ref device.\n", MaxTessLevel);
				return false;
			}
			if ( newValue != MaxTessLevel )
			{
				WriteToLog("GetRenderState for D3DRS_MAXTESSELLATIONLEVEL : return value %d doesn't equal expected value %d.\n", newValue, MaxTessLevel);
				Fail();
				return false;
			}
		}
		if ( !m_pSrcDevice->IsPure() )
		{
			if ( FAILED(m_pSrcDevice->GetRenderState(D3DRS_MINTESSELLATIONLEVEL, (DWORD*)&newValue)) )
			{
				WriteToLog("Failed to get MINTESSELLATIONLEVEL %d on src device.\n", MinTessLevel);
				return false;
			}
			if ( newValue != MinTessLevel )
			{
				WriteToLog("GetRenderState for D3DRS_MINTESSELLATIONLEVEL on src: return value %d doesn't equal expected value %d.\n", newValue, MinTessLevel);
				Fail();
				return false;
			}
			if ( FAILED(m_pSrcDevice->GetRenderState(D3DRS_MAXTESSELLATIONLEVEL, (DWORD*)&newValue)) )
			{
				WriteToLog("Failed to get MAXTESSELLATIONLEVEL %d on src device.\n", MaxTessLevel);
				return false;
			}
			if ( newValue != MaxTessLevel )
			{
				WriteToLog("GetRenderState for D3DRS_MAXTESSELLATIONLEVEL : return value %d doesn't equal expected value %d.\n", newValue, MaxTessLevel);
				Fail();
				return false;
			}
		}
	}
	//illigal min(max) tessellation levels
	else
	{
		if (MaxTessLevel < 1.f)
		{
			float oldValue, newValue;
			if ( !m_pSrcDevice->IsPure())
			{
				if ( FAILED(m_pSrcDevice->GetRenderState(D3DRS_MAXTESSELLATIONLEVEL, (DWORD*)&oldValue)) )
				{
					WriteToLog("Failed to get old MAXTESSELLATIONLEVEL %d on src device.\n", MaxTessLevel);
					return false;
				}
				if ( SUCCEEDED(m_pSrcDevice->SetRenderState(D3DRS_MAXTESSELLATIONLEVEL, *(DWORD*)&MaxTessLevel)) )
				{
					WriteToLog("Succeeded to set MAXTESSELLATIONLEVEL %d on src device\n", (int)MaxTessLevel);
					Fail();
					return false;
				}
				if ( FAILED(m_pSrcDevice->GetRenderState(D3DRS_MAXTESSELLATIONLEVEL, (DWORD*)&newValue)) )
				{
					WriteToLog("Failed to get new MAXTESSELLATIONLEVEL %d on src device.\n", MaxTessLevel);
					return false;
				}
				if ( newValue != oldValue )
				{
					WriteToLog("GetRenderState for D3DRS_MAXTESSELLATIONLEVEL on src device: return value %d doesn't equal expected value %d.\n", newValue, oldValue);
					Fail();
					return false;
				}
			}

			if ( NULL != m_pRefDevice && !m_pRefDevice->IsPure() )
			{
				if ( FAILED(m_pRefDevice->GetRenderState(D3DRS_MAXTESSELLATIONLEVEL, (DWORD*)&oldValue)) )
				{
					WriteToLog("Failed to get old MAXTESSELLATIONLEVEL %d on ref device.\n", MaxTessLevel);
					return false;
				}
				if ( SUCCEEDED(m_pRefDevice->SetRenderState(D3DRS_MAXTESSELLATIONLEVEL, *(DWORD*)&MaxTessLevel)) )
				{
					WriteToLog("Suceeded to set MaxTessLevel %d on ref device\n", (int)MaxTessLevel);
					Fail();
					return false;
				}
				if ( FAILED(m_pRefDevice->GetRenderState(D3DRS_MAXTESSELLATIONLEVEL, (DWORD*)&newValue)) )
				{
					WriteToLog("Failed to get new MAXTESSELLATIONLEVEL %d on ref device.\n", MaxTessLevel);
					return false;
				}
				if ( newValue != oldValue )
				{
					WriteToLog("GetRenderState for D3DRS_MAXTESSELLATIONLEVEL on ref device: return value %d doesn't equal expected value %d.\n", newValue, oldValue);
					Fail();
					return false;
				}
			}

			GetLastError();
			m_doComparison = false;
			return true;
		}
		if ( MinTessLevel < 1.f )
		{
			float oldValue, newValue;
			if ( !m_pSrcDevice->IsPure() )
			{
				if ( FAILED(m_pSrcDevice->GetRenderState(D3DRS_MINTESSELLATIONLEVEL, (DWORD*)&oldValue)) )
				{
					WriteToLog("Failed to get old MINTESSELLATIONLEVEL %d on src device.\n", MinTessLevel);
					return false;
				}
				if ( SUCCEEDED(m_pSrcDevice->SetRenderState(D3DRS_MINTESSELLATIONLEVEL, *(DWORD*)&MinTessLevel)) )
				{
					WriteToLog("Succeeded to set MINTESSELLATIONLEVEL %d on src device\n", (int)MinTessLevel);
					Fail();
					return false;
				}
				if ( FAILED(m_pSrcDevice->GetRenderState(D3DRS_MINTESSELLATIONLEVEL, (DWORD*)&newValue)) )
				{
					WriteToLog("Failed to get new MINTESSELLATIONLEVEL %d on src device.\n", MinTessLevel);
					return false;
				}
				if ( newValue != oldValue )
				{
					WriteToLog("GetRenderState for D3DRS_MINTESSELLATIONLEVEL on src device: return value %d doesn't equal expected value %d.\n", newValue, oldValue);
					Fail();
					return false;
				}
			}
			if ( NULL != m_pRefDevice && !m_pRefDevice->IsPure() )
			{
				if ( FAILED(m_pRefDevice->GetRenderState(D3DRS_MINTESSELLATIONLEVEL, (DWORD*)&oldValue)) )
				{
					WriteToLog("Failed to get old MINTESSELLATIONLEVEL %d on ref device.\n", MinTessLevel);
					return false;
				}
				if ( SUCCEEDED(m_pRefDevice->SetRenderState(D3DRS_MINTESSELLATIONLEVEL, *(DWORD*)&MinTessLevel)) )
				{
					WriteToLog("Succeeded to set MinTessLevel %d on ref device\n", (int)MinTessLevel);
					Fail();
					return false;
				}
				if ( FAILED(m_pRefDevice->GetRenderState(D3DRS_MINTESSELLATIONLEVEL, (DWORD*)&newValue)) )
				{
					WriteToLog("Failed to get new MINTESSELLATIONLEVEL %d on ref device.\n", MinTessLevel);
					return false;
				}
				if ( newValue != oldValue )
				{
					WriteToLog("GetRenderState for D3DRS_MINTESSELLATIONLEVEL on ref device: return value %d doesn't equal expected value %d.\n", newValue, oldValue);
					Fail();
					return false;
				}
			}
			GetLastError();
			m_doComparison = false;
			return true;
		}
		
		if (MinTessLevel > MaxTessLevel)
		{
			if ( !SetRenderState(D3DRS_MINTESSELLATIONLEVEL, *(DWORD*)&MinTessLevel) )
			{
				WriteToLog("Failed to set MinTessLevel %d\n", (int)MinTessLevel);
				return false;
			}
			if ( !SetRenderState(D3DRS_MAXTESSELLATIONLEVEL, *(DWORD*)&MaxTessLevel) )
			{
				WriteToLog("Failed to set MaxTessLevel %d\n", (int)MaxTessLevel);
				return false;
			}
			if (!m_pSrcDevice->IsPure() && SUCCEEDED(m_pSrcDevice->ValidateDevice(&NumPass)))
			{
				WriteToLog("ValidateDevice succeeded on illigal MINTESSELLATIONLEVEL=%d > MAXTESSELLATIONLEVEL=%d on Src device\n", (int)MinTessLevel, (int)MaxTessLevel);
				Fail();
				return false;
			}
			if (NULL != m_pRefDevice && !m_pRefDevice->IsPure() && SUCCEEDED(m_pRefDevice->ValidateDevice(&NumPass)))
			{
				WriteToLog("ValidateDevice succeeded on illigal MINTESSELLATIONLEVEL=%d > MAXTESSELLATIONLEVEL=%d on ref device\n", (int)MinTessLevel, (int)MaxTessLevel);
				Fail();
				return false;
			}
			GetLastError();
			m_doComparison = false;
			return true;
		}
	}

	D3DRECTPATCH_INFO RectInfo;
	float f[4];
	f[0] = f[2] = m_TessLevels[tc.TessLevels*2 + 0];
	f[1] = f[3] = m_TessLevels[tc.TessLevels*2 + 1];

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

	if ( tc.View == 0 )
	{
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
	}
	else
	{
		for( int i = 0; i < m_numPatches; i++ ) 
		{
			SetNewMaterial(i);
			if ( !DrawRectPatch(SURFACEHANDLEBASE+i, f, NULL))
				WriteToLog("%d DrawRectPatch failed.\n", iTest);
		}
	}

	EndScene();
	return true;
}

bool CMaxMinRectTest::GenerateBuffers(MAXMINTESTCASE tc)
{
	VOID *pV = NULL;
	float x, y, z, nx, ny, nz;
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
	VBarray = new VERTEX[m_uBufferHeight*m_uBufferWidth];
	for (int j = 0; j < m_uBufferHeight; j++)
	{
		y = 1.f - 2.f*((float)j/(m_uBufferHeight - 1.f));
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

			VBarray[j*m_uBufferWidth+i] = VERTEX(D3DXVECTOR3( x, y, z), D3DXVECTOR3( nx, ny, nz), (1.f+x)/2.f, (1.f-y)/2.f); 
		}
	}
	memcpy( pV, VBarray, m_uBufferHeight*m_uBufferWidth*sizeof(VERTEX));
	if (!m_pVB->Unlock())
	{
		WriteToLog("%d: Failed to unlock m_pVB.\n", tc.nTest);
		delete [] VBarray;
		return false;
	}
	delete [] VBarray;
	
	//create colors buffer
	/*DWORD nSize = GetVertexSize(D3DFVF_DIFFUSE);
	if(!CreateVertexBuffer(m_uBufferHeight*m_uBufferWidth*nSize, D3DUSAGE_RTPATCHES, D3DFVF_DIFFUSE, D3DPOOL_SYSTEMMEM, &m_ColorBuffer))
	{
		WriteToLog("%d: Failed to create color buffer.\n", tc.nTest);
		return false;
	}
	PBYTE pBuffer = NULL;
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

void CMaxMinRectTest::ChooseTessLevels(MAXMINTESTCASE tc)
{
	float min = m_MaxMinTessLevels[tc.MaxMinTessLevels][0];
	float max = m_MaxMinTessLevels[tc.MaxMinTessLevels][1];
	
	//0: want to set min-5	
	if (min - 5 >= 1)
		m_TessLevels[0] = min - 5;
	else if (min - 4 >= 1)
		m_TessLevels[0] = min - 4;
	else //if (min -3 >= 1)
		m_TessLevels[0] = max(min - 3, 1);
	//1: want to set min-3
	if (min - 3 >= 1)
		m_TessLevels[1] = min - 3;
	else //if (min - 2 >= 1)
		m_TessLevels[1] = max(min - 2, 1);
	//2: want to set min-1
	m_TessLevels[2] = max(min - 1, 1);
	//3: want to set min
	m_TessLevels[3] = max(min, 1);
	//4: want to set min+1
	m_TessLevels[4] = max(min + 1, 1);
	//5-8: want to set some inter-values
	if (min + 1 < max - 1)
	{
		float range = max - min - 2.f;
		m_TessLevels[5] = min + 1.f + 1.f*range/5.f;
		m_TessLevels[6] = min + 1.f + 2.f*range/5.f;
		m_TessLevels[7] = min + 1.f + 3.f*range/5.f;
		m_TessLevels[8] = min + 1.f + 4.f*range/5.f;
	}
	else
		m_TessLevels[5] = m_TessLevels[6] = m_TessLevels[7] = m_TessLevels[8] = 1;
	//9: want to set max-1
	//if (min  < max - 1 && max > 1)
	m_TessLevels[9] = max(1, max - 1);
	//10: want to set max
	m_TessLevels[10] = max(max, 1);
	//10: want to set max + 1
	m_TessLevels[11] = max + 1;
	//11: want to set max + 5
	m_TessLevels[12] = max + 5;
	//12: want to set max + 10
	m_TessLevels[13] = max + 10;
}

bool CMaxMinRectTest::ProcessFrame(void)
{
	if (m_doComparison)
		return CD3DTest::ProcessFrame();
	else
	{
		m_doComparison = true;
		return true;
	}
}

bool CMaxMinRectTest::GenerateVertexShader()
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

bool CMaxMinRectTest::CapsCheck()
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
		
    //invalid values
	m_MaxMinTessLevels[0][0] = -1.f; m_MaxMinTessLevels[0][1] = -2.f; 
	m_MaxMinTessLevels[1][0] =  2.f; m_MaxMinTessLevels[1][1] = -1.f; 
	m_MaxMinTessLevels[2][0] =  2.f; m_MaxMinTessLevels[2][1] =  1.f; 
	m_MaxMinTessLevels[3][0] =  0.f; m_MaxMinTessLevels[3][1] = 0.5f; 
	m_MaxMinTessLevels[4][0] = 0.5f; m_MaxMinTessLevels[4][1] = 10.f; 
	//valid values
	m_MaxMinTessLevels[5][0] = 1.f; m_MaxMinTessLevels[5][1] = 1.f; 
	m_MaxMinTessLevels[6][0] = m_MaxTess/2.f-1.f; m_MaxMinTessLevels[6][1] = m_MaxTess/2.f; 
	m_MaxMinTessLevels[7][0] = m_MaxTess/4.f;     m_MaxMinTessLevels[7][1] = m_MaxTess/2.f; 
	m_MaxMinTessLevels[8][0] = m_MaxTess/4.f;     m_MaxMinTessLevels[8][1] = m_MaxTess-1.f; 

	return true;
}

bool CMaxMinRectTest::Setup()
{
	Tessellation::Setup();
	
	//set vertex shaders
	if ( !m_UseFVFFixed && !GenerateVertexShader())
	{
		WriteToLog("CMaxMinRectTest::Setup - GenerateVertexShader() failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
		return false;
	}
	if (m_UseFVFFixed || m_UseFVFShader)
	{
		if ( !SetVertexShader(FVF_VERTEX) ) 
		{
			WriteToLog("CMaxMinRectTest::Setup - SetVertexShader(FVF_VERTEX) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	else
	{
		if ( !SetVertexDeclaration(m_pDecl) )
		{
			WriteToLog("CMaxMinRectTest::Setup - SetVertexDeclaration(m_pDecl) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	if (m_UseFVFFixed || m_UseDeclFixed)
	{
		if ( !SetVertexShader((CnVertexShader*)NULL) )
		{
			WriteToLog("CMaxMinRectTest::Setup - SetVertexShader((CnVertexShader*)NULL) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	else
	{
		if ( !SetVertexShader(m_pShader) )
		{
			WriteToLog("CMaxMinRectTest::Setup - SetVertexShader(m_pShader) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
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

void CMaxMinRectTest::Cleanup()
{
	Tessellation::Cleanup();
	
	RELEASE(m_pVB);
	RELEASE(m_pShader);
	RELEASE(m_pDecl);

	for( UINT i = 0; i < m_numPatches; i++ ) 
        DeletePatch(SURFACEHANDLEBASE + i);
	m_numPatches = 0;
}





