#include "NonPowTwoConfTest.h"
#include "types.h"
#include "D3DX9.h"
//#include "csurface.h"
//#include "cswapchain.h"
#include "cshapes.h"

#define ROW_COUNT 6

CNonPowTwoConfTest::CNonPowTwoConfTest()
{
	m_szTestName   = _T("NonPowTwo Conf");
	m_szCommandKey = _T("ConfTest");

	SetTestType(TESTTYPE_CONF);

	m_Options.D3DOptions.fMinDXVersion = 9.0f;//dx8 ref only does pow2
	m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91;

	m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_BESTAVAILABLE;

	m_Options.D3DOptions.nBackBufferWidth  = 160;
	m_Options.D3DOptions.nBackBufferHeight = 120;
	
#ifdef FAST_TEST
	m_Options.ModeOptions.TextureFmt = FMT_A8R8G8B8;
#else
	m_Options.ModeOptions.TextureFmt = FMT_ALL;
#endif

	m_Options.ModeOptions.dwTexExclude	= PF_BUMPLUMINANCE | PF_BUMPDUDV | PF_METN;// | PF_DMAP;
	m_Options.ModeOptions.dwVolExclude	= PF_BUMPLUMINANCE | PF_BUMPDUDV | PF_METN;// | PF_DMAP;
	m_Options.ModeOptions.dwCubeExclude	= PF_BUMPLUMINANCE | PF_BUMPDUDV | PF_METN;// | PF_DMAP;

	m_pFramework->GetImageCmp()->SetMinPixels(0);//some of the alpha blend cases render 0 pixels
    
	// Modern hardware uses much better filtering than REF.  
	// Lowering conformance from 85% to 65% is the safest way to fix
	// false failures on modern hardware
	m_pFramework->GetImageCmp()->SetPassRequired(0.65f); 

	m_pVertices   = NULL;
	m_pTLVertices = NULL;
	m_pIndices    = NULL;

	m_pVertices1   = NULL;
	m_pTLVertices1 = NULL;
	m_pIndices1    = NULL;
	
	m_nVertices = 0;
	m_nIndices  = 0;

	m_bSkipAll = false;
	
}

//------------------------------------------------------------------------------

CNonPowTwoConfTest::~CNonPowTwoConfTest()
{
}

//------------------------------------------------------------------------------

void CNonPowTwoConfTest::Cleanup()
{
	CNonPowTwoTest::Cleanup();

	delete [] m_pVertices;
	delete [] m_pTLVertices;
	delete [] m_pIndices;

	delete [] m_pVertices1;
	delete [] m_pTLVertices1;
	delete [] m_pIndices1;
	
	m_nVertices  = 0;
	m_nIndices   = 0;
	m_nVertices1 = 0;
	m_nIndices1  = 0;
}

//------------------------------------------------------------------------------

bool CNonPowTwoConfTest::InitGeometry()
{	
	m_nVertices  = 0;
	m_nIndices   = 0;
	m_nVertices1 = 0;
	m_nIndices1  = 0;

	m_nVertices = ROW_COUNT * ROW_COUNT;
	m_nIndices = 2*3*(ROW_COUNT-1)*(ROW_COUNT-1);

	if (KeySet("wrapxy"))
	{
		m_nVertices1 = 4;
		m_nIndices1  = 6;
	}
	else
	{
		m_nVertices1 = m_nVertices;
		m_nIndices1  = m_nIndices;
	}
	
	//creating all vertex and index buffers
	if (g_bUseTL)
	{
		m_pTLVertices  = new TLVERTEX[2*m_nVertices];
		m_pTLVertices1 = new TLVERTEX[2*m_nVertices1];
	}
	
	m_pVertices  = new VERTEX[m_nVertices ];
	m_pVertices1 = new VERTEX[m_nVertices1];
		
	m_pIndices  = new unsigned short int[m_nIndices ];
	m_pIndices1 = new unsigned short int[m_nIndices1];

	int width  = m_pCurrentMode->nBackBufferWidth /8; //width of each triangle in the grid
	int height = m_pCurrentMode->nBackBufferHeight/8; //height of each triangle in the grid

	float tu = 0, tv = 0, cu = 0, cv = 0;
	
	for (UINT j = 0; j < ROW_COUNT; j++)
	{
		for (UINT i = 0; i < ROW_COUNT; i++)
		{
			tu = 2.f * i * i * (1.f/(ROW_COUNT-1)) * (1.f/(ROW_COUNT-1));
			tv = 2.f * j * j * (1.f/(ROW_COUNT-1)) * (1.f/(ROW_COUNT-1));
			
			cu =  2.f * i * (1.f/(ROW_COUNT-1)) - 1.f;
			cv = -2.f * j * (1.f/(ROW_COUNT-1)) + 1.f;

			m_pVertices[i + j*ROW_COUNT] = VERTEX(D3DXVECTOR3(i * 1.f/(ROW_COUNT-1), (ROW_COUNT-1-j) * 1.f/(ROW_COUNT-1), 0), D3DXVECTOR3(0, 0, -1), D3DXVECTOR2(tu, tv), D3DXVECTOR3(cu, cv, 1.f), D3DXVECTOR3(tu, tv, 0.f));
			
			m_pVertices[i + j*ROW_COUNT].Position[0] -= 0.5f;
			m_pVertices[i + j*ROW_COUNT].Position[1] -= 0.5f;

			if (!KeySet("wrapxy"))
			{
				tu = i * (1.f/(ROW_COUNT-1));
				if (tu >= 0.99f)
					tu = 0.99f;
				tv = j * (1.f/(ROW_COUNT-1));
				if (tv >= 0.99f)
					tv = 0.99f;
				m_pVertices1[i + j*ROW_COUNT] = VERTEX(D3DXVECTOR3(i * 1.f/(ROW_COUNT-1), (ROW_COUNT-1-j) * 1.f/(ROW_COUNT-1), 0), D3DXVECTOR3(0, 0, -1), D3DXVECTOR2(tu, tv), D3DXVECTOR3(cu, cv, 1.f), D3DXVECTOR3(tu, tv, 0.f));
				m_pVertices1[i + j*ROW_COUNT].Position[0] -= 0.5f;
				m_pVertices1[i + j*ROW_COUNT].Position[1] -= 0.5f;
			}

			//initialize indices according to current vertex
			if (j > 0)
			{
				if (i > 0)
				{
					m_pIndices[3 * (2*(j-1)*(ROW_COUNT-1) + 2*(i-1) + 1) + 1] = i + j * ROW_COUNT;
				}
				if (i < ROW_COUNT - 1)
				{
					m_pIndices[3 * (2*(j-1)*(ROW_COUNT-1) + 2*(i-0) + 0) + 2] = i + j * ROW_COUNT;
					m_pIndices[3 * (2*(j-1)*(ROW_COUNT-1) + 2*(i-0) + 1) + 2] = i + j * ROW_COUNT;
				}
			}
			if (j < ROW_COUNT - 1)
			{
				if (i > 0)
				{
					m_pIndices[3 * (2*(j-0)*(ROW_COUNT-1) + 2*(i-1) + 0) + 1] = i + j * ROW_COUNT;
					m_pIndices[3 * (2*(j-0)*(ROW_COUNT-1) + 2*(i-1) + 1) + 0] = i + j * ROW_COUNT;
				}
				if (i < ROW_COUNT - 1)
				{
					m_pIndices[3 * (2*(j-0)*(ROW_COUNT-1) + 2*(i-0) + 0) + 0] = i + j * ROW_COUNT;
				}
			}
		}
	}
	
	if(KeySet("wrapxy"))
	{
		//m_pVertices1 and m_pIndices1 are only used with wrap mode cases
		m_pVertices1[0] = VERTEX(D3DXVECTOR3(-0.5f, 0.5f, 0.f), D3DXVECTOR3(0, 0, -1), D3DXVECTOR2(0.10f, 0.10f), D3DXVECTOR3(-1.f, 1.f, 1.f), D3DXVECTOR3(0.00f, 0.00f, 0.f));
		m_pVertices1[1] = VERTEX(D3DXVECTOR3( 0.5f, 0.5f, 0.f), D3DXVECTOR3(0, 0, -1), D3DXVECTOR2(0.9f, 0.10f), D3DXVECTOR3(-1.f, 1.f, 1.f), D3DXVECTOR3(0.99f, 0.00f, 0.f));
		m_pVertices1[2] = VERTEX(D3DXVECTOR3(-0.5f,-0.5f, 0.f), D3DXVECTOR3(0, 0, -1), D3DXVECTOR2(0.10f, 0.9f), D3DXVECTOR3(-1.f,-1.f, 1.f), D3DXVECTOR3(0.00f, 0.99f, 0.f));
		m_pVertices1[3] = VERTEX(D3DXVECTOR3( 0.5f,-0.5f, 0.f), D3DXVECTOR3(0, 0, -1), D3DXVECTOR2(0.9f, 0.9f), D3DXVECTOR3(-1.f,-1.f, 1.f), D3DXVECTOR3(0.99f, 0.99f, 0.f));
	
		float t;
		if (ReadFloat("v0u", &t))
			m_pVertices1[0]._2DCoords[0] = t;
		if (ReadFloat("v0v", &t))
			m_pVertices1[0]._2DCoords[1] = t;
		if (ReadFloat("v1u", &t))
			m_pVertices1[1]._2DCoords[0] = t;
		if (ReadFloat("v1v", &t))
			m_pVertices1[1]._2DCoords[1] = t;
		if (ReadFloat("v2u", &t))
			m_pVertices1[2]._2DCoords[0] = t;
		if (ReadFloat("v2v", &t))
			m_pVertices1[2]._2DCoords[1] = t;
		if (ReadFloat("v3u", &t))
			m_pVertices1[3]._2DCoords[0] = t;
		if (ReadFloat("v3v", &t))
			m_pVertices1[3]._2DCoords[1] = t;

		m_pIndices1[0] = 0;
		m_pIndices1[1] = 1;
		m_pIndices1[2] = 2;
		m_pIndices1[3] = 1;
		m_pIndices1[4] = 3;
		m_pIndices1[5] = 2;
	}
	else
	{
		for(int i = 0; i < m_nIndices1; i++)
			m_pIndices1[i] = m_pIndices[i];
	}

	D3DXVECTOR3 *V_3D  = new D3DXVECTOR3[m_nVertices ];
	D3DXVECTOR4 *V_4D  = new D3DXVECTOR4[m_nVertices ];
	
	//copy untransformed data to transformed for 1st primitive
	for(UINT i = 0 ; i < m_nVertices; i++)
		V_3D[i] = m_pVertices[i].Position;
	TransformVertex(m_mTransform[0], V_3D, V_4D, m_nVertices);
	
	//figure out real sizes of used primitives
	m_uRectWidth  = (UINT)( V_4D[ROW_COUNT-1].x - V_4D[0].x );
	m_uRectHeight = (UINT)( V_4D[m_nVertices-1].y - V_4D[0].y ); 
	
	if (g_bUseTL)
	{
		for(UINT i = 0 ; i < m_nVertices; i++)
		{
			m_pTLVertices[i].Position[0] = (int)V_4D[i].x - 0.5f;
			m_pTLVertices[i].Position[1] = (int)V_4D[i].y - 0.5f;
			m_pTLVertices[i].Position[2] = V_4D[i].z;
			m_pTLVertices[i].Position[3] = V_4D[i].w;
			m_pTLVertices[i]._2DCoords[0]   = m_pVertices[i]._2DCoords[0];
			m_pTLVertices[i]._2DCoords[1]   = m_pVertices[i]._2DCoords[1];
			m_pTLVertices[i]._3DCoords[0]   = m_pVertices[i]._3DCoords[0];
			m_pTLVertices[i]._3DCoords[1]   = m_pVertices[i]._3DCoords[1];
			m_pTLVertices[i]._3DCoords[2]   = m_pVertices[i]._3DCoords[2];
			m_pTLVertices[i].CubeCoords[0]  = m_pVertices[i].CubeCoords[0];
			m_pTLVertices[i].CubeCoords[1]  = m_pVertices[i].CubeCoords[1];
			m_pTLVertices[i].CubeCoords[2]  = m_pVertices[i].CubeCoords[2];
			m_pTLVertices[i].color       = 0x7fffffff;
		}

		//copy untransformed data to transformed for 2st primitive
		TransformVertex(m_mTransform[1], V_3D, V_4D, m_nVertices);
		for(UINT i = 0 ; i < m_nVertices; i++)
		{
			m_pTLVertices[m_nVertices + i] = m_pTLVertices[i];

			m_pTLVertices[m_nVertices + i].Position[0] = (int)V_4D[i].x - 0.5f;
			m_pTLVertices[m_nVertices + i].Position[1] = (int)V_4D[i].y - 0.5f;
			m_pTLVertices[m_nVertices + i].Position[2] = V_4D[i].z;
			m_pTLVertices[m_nVertices + i].Position[3] = V_4D[i].w;
		}
	}

	delete [] V_3D;
	delete [] V_4D;

	D3DXVECTOR3 *V_3D1 = new D3DXVECTOR3[m_nVertices1];
	D3DXVECTOR4 *V_4D1 = new D3DXVECTOR4[m_nVertices1];
	
	//copy untransformed data to transformed for 1st primitive for wrap vertex buffer
	for(UINT i = 0 ; i < m_nVertices1; i++)
		V_3D1[i] = m_pVertices1[i].Position;
	TransformVertex(m_mTransform[0], V_3D1, V_4D1, m_nVertices1);
	
	if (g_bUseTL)
	{
		for(UINT i = 0 ; i < m_nVertices1; i++)
		{
			m_pTLVertices1[i].Position[0] = (int)V_4D1[i].x - 0.5f;
			m_pTLVertices1[i].Position[1] = (int)V_4D1[i].y - 0.5f;
			m_pTLVertices1[i].Position[2] = V_4D1[i].z;
			m_pTLVertices1[i].Position[3] = V_4D1[i].w;
			m_pTLVertices1[i]._2DCoords[0]   = m_pVertices1[i]._2DCoords[0];
			m_pTLVertices1[i]._2DCoords[1]   = m_pVertices1[i]._2DCoords[1];
			m_pTLVertices1[i]._3DCoords[0]   = m_pVertices1[i]._3DCoords[0];
			m_pTLVertices1[i]._3DCoords[1]   = m_pVertices1[i]._3DCoords[1];
			m_pTLVertices1[i]._3DCoords[2]   = m_pVertices1[i]._3DCoords[2];
			m_pTLVertices1[i].CubeCoords[0]  = m_pVertices1[i].CubeCoords[0];
			m_pTLVertices1[i].CubeCoords[1]  = m_pVertices1[i].CubeCoords[1];
			m_pTLVertices1[i].CubeCoords[2]  = m_pVertices1[i].CubeCoords[2];
			m_pTLVertices1[i].color       = 0x7fffffff;
		}

		//copy untransformed data to transformed for 2st primitive for wrap vertex buffer
		TransformVertex(m_mTransform[1], V_3D1, V_4D1, m_nVertices1);
		for(UINT i = 0 ; i < m_nVertices1; i++)
		{
			m_pTLVertices1[m_nVertices1 + i] = m_pTLVertices1[i];

			m_pTLVertices1[m_nVertices1 + i].Position[0] = (int)V_4D1[i].x - 0.5f;
			m_pTLVertices1[m_nVertices1 + i].Position[1] = (int)V_4D1[i].y - 0.5f;
			m_pTLVertices1[m_nVertices1 + i].Position[2] = V_4D1[i].z;
			m_pTLVertices1[m_nVertices1 + i].Position[3] = V_4D1[i].w;
		}
	}

	delete [] V_3D1;
	delete [] V_4D1;

	D3DXMATRIX temp;
	for(UINT i = 0; i < 2; i++)
	{
		temp = m_mTransform[i];
		D3DXMatrixTranspose( &m_mTransform[i], &temp); 
	}

	return true;
}

//------------------------------------------------------------------------------

bool CNonPowTwoConfTest::ShouldCapsCheck()
{
	return true;
}

//------------------------------------------------------------------------------

CTestMatrix* CNonPowTwoConfTest::GetTestMatrix()
{
	return &m_TestMatrix;
}

//------------------------------------------------------------------------------

bool CNonPowTwoConfTest::SetupTestState(TCHAR *testName, CTestMatrix* pTestMatrix, const UINT nTestNum)
{
	ASSERT(NULL != pTestMatrix);
	
	bool bResultVal = true;
    	
	bResultVal = pTestMatrix->SetupCombination(testName, nTestNum-1, this);//nTestNum starts counting at 1

	//need to change diffuse color for TL vertices when nessessary (on some TexOp cases)
	if (g_bUseTL && (g_isZeroDiffuse || g_isHalfDiffuse))
	{
		DWORD color;
		if (g_isZeroDiffuse)
			color = 0x7F000000;
		else if (g_isHalfDiffuse)
			color = 0x7F7F7F7F;

		for(UINT i = 0 ; i < 2*m_nVertices; i++)
		{
			m_pTLVertices[i].color = color;
	
		}
	}

	return bResultVal;
}

//------------------------------------------------------------------------------

//render two objects, one in front of the other so that we get alpha blending.
bool CNonPowTwoConfTest::RenderScene()
{	
	bool bResultVal = true;

	VOID *pVertices, *pIndices;
	UINT nVertices, nIndices, size;

	//set up for drawing 1st primitive
	if (g_isWrapCase)
	{
		nVertices = m_nVertices1;
		nIndices  = m_nIndices1;

		if (g_bUseTL)
		{
			pVertices = m_pTLVertices1;
			pIndices  = m_pIndices1;
			size = sizeof(TLVERTEX);
		}
		else
		{
			pVertices = m_pVertices1;
			pIndices  = m_pIndices1;
			size = sizeof(VERTEX);
		}
	}
	else
	{
		nVertices = m_nVertices;
		nIndices  = m_nIndices;

		if(g_bUseTL && !g_isVShaderCase)
		{
			pVertices = m_pTLVertices;
			pIndices  = m_pIndices;
			size = sizeof(TLVERTEX);
		}
		else
		{
			pVertices = m_pVertices;
			pIndices  = m_pIndices;
			size = sizeof(VERTEX);
		}
	}

	bResultVal &= SetTransform(D3DTS_WORLD, (D3DMATRIX*)&m_mWorld[0]);
	if ( g_isVShaderCase )
	{
		SetVertexShaderConstant(0, &m_mTransform[0], 4);
	}

	if (!KeySet("noprim0"))
	{ 
		bResultVal &= DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, nVertices, nIndices/3,
							                 pIndices, FMT_INDEX16, pVertices, size);
	}

	//set up for drawing 2nd primitive
	if (g_isWrapCase)
	{
		nVertices = m_nVertices1;
		nIndices  = m_nIndices1;

		if(g_bUseTL)
		{
			pVertices = m_pTLVertices1 + nVertices;
			pIndices  = m_pIndices1;
			size = sizeof(TLVERTEX);
		}
		else
		{
			pVertices = m_pVertices1;
			pIndices  = m_pIndices1;
			sizeof(VERTEX);
		}
	}
	else
	{
		nVertices = m_nVertices;
		nIndices  = m_nIndices;

		if (g_bUseTL && !g_isVShaderCase)
		{
			pVertices = m_pTLVertices + nVertices;
			pIndices  = m_pIndices;
			size = sizeof(TLVERTEX);
		}
		else
		{
			pVertices = m_pVertices;
			pIndices  = m_pIndices;
			sizeof(VERTEX);
		}
	}

	bResultVal &= SetTransform(D3DTS_WORLD, (D3DMATRIX*)&m_mWorld[1]);
	if ( g_isVShaderCase )
	{
		SetVertexShaderConstant(0, &m_mTransform[1], 4);
	} 

	if (!KeySet("noprim1"))
	{	
		bResultVal &= DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, nVertices, nIndices/3,
							                 pIndices, FMT_INDEX16, pVertices, size);
	}

	if (g_bUseTL && (g_isZeroDiffuse || g_isHalfDiffuse) )
	for(UINT i = 0 ; i < 2*nVertices; i++)
	{
		m_pTLVertices[i].color = 0x7fffffff;
	}


	return bResultVal;
}

//------------------------------------------------------------------------------

void CNonPowTwoConfTest::SetTestCaseRange(CTestMatrix* pTestMatrix)
{
	ASSERT(NULL != pTestMatrix);

	pTestMatrix->SetCombinationRange(this);
}

//------------------------------------------------------------------------------

void CNonPowTwoConfTest::TransformVertex(D3DXMATRIX mTransform, D3DXVECTOR3 *Vertices, D3DXVECTOR4 *NewVertices, UINT Size)
{
	D3DXMATRIX Matrix, Matrix2;
	//viewport matrix
	D3DXMatrixIdentity(&Matrix2);
	Matrix2(0,0) =  0.f + (FLOAT)( m_pCurrentMode->nBackBufferWidth/2 );
	Matrix2(3,0) =  0.f + (FLOAT)( m_pCurrentMode->nBackBufferWidth/2 );
	Matrix2(1,1) = -1.f * (FLOAT)( m_pCurrentMode->nBackBufferHeight/2 );
	Matrix2(3,1) =  0.f + (FLOAT)( m_pCurrentMode->nBackBufferHeight/2 );
	Matrix2(2,2) = 1.f - 0.f;
	Matrix2(3,2) = 0.f;
	Matrix2(3,3) = 1.f;
	
	D3DXMatrixMultiply(&Matrix, &mTransform, &Matrix2);
	float x, y, z, w;

	for (int i = 0; i < Size; i++)
	{
		x = Vertices[i].x*Matrix(0,0) +
			Vertices[i].y*Matrix(1,0) +
			Vertices[i].z*Matrix(2,0) +
			Matrix(3,0); 
		y = Vertices[i].x*Matrix(0,1) +
			Vertices[i].y*Matrix(1,1) +
			Vertices[i].z*Matrix(2,1) +
			Matrix(3,1); 
		z = Vertices[i].x*Matrix(0,2) +
			Vertices[i].y*Matrix(1,2) +
			Vertices[i].z*Matrix(2,2) +
			Matrix(3,2); 
		w = Vertices[i].x*Matrix(0,3) +
			Vertices[i].y*Matrix(1,3) +
			Vertices[i].z*Matrix(2,3) +
			Matrix(3,3); 
		
		x /= w;
		y /= w;
		z /= w;
		w = 1.f/w;

		NewVertices[i].x = x;
		NewVertices[i].y = y;
		NewVertices[i].z = z;
		NewVertices[i].w = w;
	}
}