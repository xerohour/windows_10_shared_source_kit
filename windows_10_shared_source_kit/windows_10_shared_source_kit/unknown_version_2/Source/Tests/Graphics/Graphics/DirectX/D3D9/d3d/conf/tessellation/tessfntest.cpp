#define D3D_OVERLOADS

#include "Tessellation.h"
#include "cd3dx.h"		//for D3DXAssembleShader9

CTessFNTest::CTessFNTest()
{
	m_szTestName = "TessFactor N-Patch Test";
	m_szCommandKey = "tfn";

	m_pVCircle  = NULL;
	m_pICircle  = NULL;
	m_pVEllipse = NULL;
	m_pIEllipse = NULL;
	m_pShader   = NULL;
	m_pDecl     = NULL;

	m_UseDeclShader = true;
	m_UseFVFShader  = false;
	m_UseFVFFixed   = false;
	m_UseDeclFixed  = true;
	if( KeySet(_T("decl-fix"))  )
		m_UseDeclShader   = false;
	if( KeySet(_T("decl-sh"))  )
		m_UseDeclFixed = false;
}

UINT CTessFNTest::TestInitialize(void)
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
		if (//( m_UseFVFFixed   && tc.VPMethod == 0 ) ||
		    ( m_UseDeclShader && tc.VPMethod == 1 ) ||
		    ( m_UseDeclFixed  && tc.VPMethod == 2 ) )//||
		    //( m_UseFVFShader  && tc.VPMethod == 3 )  )
		for ( tc.TessLevels = 0; tc.TessLevels <= tcCount.TessLevels; tc.TessLevels++ ) //tessLevel
		{
			for ( tc.PatchStyle = 0; tc.PatchStyle <= tcCount.PatchStyle; tc.PatchStyle++ ) //patch style
			{
                for ( tc.PosDegree = 0; tc.PosDegree <= tcCount.PosDegree; tc.PosDegree++ ) //position degree
				{
					if (/*Degrees[tc.PosDegree] == D3DDEGREE_LINEAR ||*/ Degrees[tc.PosDegree] == D3DDEGREE_CUBIC )
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

bool CTessFNTest::ExecuteTest(UINT iTest)
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

	float TessLevel = 2.f+tc.TessLevels;// + 0.25*(tc.TessLevels%4);
	TessLevel = min (TessLevel, m_MaxTess);
	SetRenderState(D3DRS_POSITIONDEGREE, Degrees[tc.PosDegree]);
	SetRenderState(D3DRS_NORMALDEGREE, Degrees[tc.NormDegree]);
	SetRenderState(D3DRS_PATCHEDGESTYLE, tc.PatchStyle );
	SetNPatchMode( TessLevel );
		
	sprintf( msgString, "%d patches, %s Pos, %s Norm, %s style, Tess=%.2f, decl-%s",
		tc.isEntire ? 1 : 4, 
		Degrees[tc.PosDegree] == D3DDEGREE_LINEAR ? "LINEAR" : "CUBIC",
		Degrees[tc.NormDegree] == D3DDEGREE_LINEAR ? "LINEAR" : "QUADRATIC",
		tc.PatchStyle == D3DPATCHEDGE_DISCRETE ? "DISCRETE" : "CONTINUOUS",
		TessLevel,
		(tc.VPMethod == 0 || tc.VPMethod == 2) ? "fixed" : "shader" );

	BeginTestCase( msgString, iTest);
	
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

	//draw tessellated circle
	SetNewMaterial(1);
	if (!KeySet("!c2"))
	if ( !DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 2, m_pICircle, FMT_INDEX16, m_pVCircle, sizeof(TESSVERTEX)) )
		WriteToLog("%d DrawIndexedPrimitiveUP failed.\n", iTest);

	SetNewMaterial(2);
	if (!KeySet("!k"))
	{
		if (tc.isEntire)
		{
            if ( !DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 4, m_pIEllipse, FMT_INDEX16, m_pVEllipse, sizeof(TESSVERTEX)) )
				WriteToLog("%d DrawPrimitiveUP failed.\n", iTest);
		}
		else
		{
			 if ( !DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 1, m_pIEllipse, FMT_INDEX16, m_pVEllipse, sizeof(TESSVERTEX)) )
				WriteToLog("%d DrawPrimitiveUP failed.\n", iTest);
			 if ( !DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 1, m_pIEllipse+3, FMT_INDEX16, m_pVEllipse, sizeof(TESSVERTEX)) )
				WriteToLog("%d DrawPrimitiveUP failed.\n", iTest);
			 if ( !DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 1, m_pIEllipse+6, FMT_INDEX16, m_pVEllipse, sizeof(TESSVERTEX)) )
				WriteToLog("%d DrawPrimitiveUP failed.\n", iTest);
			 if ( !DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 1, m_pIEllipse+9, FMT_INDEX16, m_pVEllipse, sizeof(TESSVERTEX)) )
				WriteToLog("%d DrawPrimitiveUP failed.\n", iTest);
		}
	}

	EndScene();
	return true;
}
             
bool CTessFNTest::GenerateBuffers()
{
	VOID   *pV = NULL;

	float a = 4.f;
	float cosPI_4 = cos(D3DX_PI/4.f);
	float sinPI_4 = sin(D3DX_PI/4.f);

	m_pVCircle = new TESSVERTEX[4];
	m_pVCircle[0] = TESSVERTEX(D3DXVECTOR3( -cosPI_4,  sinPI_4, 0.f ), D3DXVECTOR3( -cosPI_4,  sinPI_4, 0.f ), 0.f, 0.f, 1.f); 
	m_pVCircle[1] = TESSVERTEX(D3DXVECTOR3(  cosPI_4,  sinPI_4, 0.f ), D3DXVECTOR3(  cosPI_4,  sinPI_4, 0.f ), 0.f, 0.f, 1.f); 
	m_pVCircle[2] = TESSVERTEX(D3DXVECTOR3( -cosPI_4, -sinPI_4, 0.f ), D3DXVECTOR3( -cosPI_4, -sinPI_4, 0.f ), 0.f, 0.f, 1.f); 
	m_pVCircle[3] = TESSVERTEX(D3DXVECTOR3(  cosPI_4, -sinPI_4, 0.f ), D3DXVECTOR3(  cosPI_4, -sinPI_4, 0.f ), 0.f, 0.f, 1.f); 
	
	m_pICircle = new WORD[6];
	m_pICircle[0] = 0;
	m_pICircle[1] = 1;
	m_pICircle[2] = 3;
	m_pICircle[3] = 0;
	m_pICircle[4] = 3;
	m_pICircle[5] = 2;

	m_pVEllipse = new TESSVERTEX[5];

	m_pVEllipse[0] = TESSVERTEX(D3DXVECTOR3( 0.f, 0.f, -a ), D3DXVECTOR3( 0.f, 0.f, -1.f ), 0.f, 0.f, 0.4f); 
	m_pVEllipse[1] = TESSVERTEX(D3DXVECTOR3( -cosPI_4,  sinPI_4, 0.f ), D3DXVECTOR3( -cosPI_4,  sinPI_4, 0.f ), 0.f, 0.f, 0.4f); 
	m_pVEllipse[2] = TESSVERTEX(D3DXVECTOR3(  cosPI_4,  sinPI_4, 0.f ), D3DXVECTOR3(  cosPI_4,  sinPI_4, 0.f ), 0.f, 0.f, 0.4f); 
	m_pVEllipse[3] = TESSVERTEX(D3DXVECTOR3( -cosPI_4, -sinPI_4, 0.f ), D3DXVECTOR3( -cosPI_4, -sinPI_4, 0.f ), 0.f, 0.f, 0.4f); 
	m_pVEllipse[4] = TESSVERTEX(D3DXVECTOR3(  cosPI_4, -sinPI_4, 0.f ), D3DXVECTOR3(  cosPI_4, -sinPI_4, 0.f ), 0.f, 0.f, 0.4f); 

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

bool CTessFNTest::GenerateVertexShader()
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
		WriteToLog("CTessFNTest::GenerateVertexShader - CreateVertexDeclaration() failed.\n");
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
		//"dp3  r0.x,   v1, c7  \n"    // perform lighting N dot L calculation in world space
		//"mul    r1, r0.x,  c4  \n"	  //calculate pixel color from light intensity and interpolated diffuse vertex color 
		//"add   oD0,   r1,  c6  \n"    //add ambient color -> final pixel color
		"mov oD0, c4 \n"
		"mov   oT0,   v2       \n"
	);
			
	hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
	if (FAILED(hr))
	{
		WriteToLog(_T("CTessFNTest::GenerateVertexShader - D3DXAssembleShader(vertex) failed, hr=%s\n"),
					m_pD3D->HResultToString(hr));
		RELEASE(m_pDecl);
		return false;
	}
	
	if (!CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &(m_pShader)))
	{
		WriteToLog("CTessFNTest::GenerateVertexShader - CreateVertexShader() failed.\n");
		pShader->Release();
		RELEASE(m_pDecl);
		return false;
	}
	pShader->Release();

	return true;
}

bool CTessFNTest::CapsCheck()
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

	m_nTessLevels = (DWORD)m_MaxTess - 1;
	
	return true;
}

void CTessFNTest::TransformVertex(D3DXMATRIX Matrix, D3DXVECTOR3 *Vertices, D3DXVECTOR3 *NewVertices, DWORD Size)
{
	float x, y, z, w;

	for (int i = 0; i < Size; i++)
	{
		x = Vertices[i].x*Matrix(0,0) +
			Vertices[i].y*Matrix(1,0) +
			Vertices[i].z*Matrix(2,0) +
			Matrix(3,0); //Vertices[i].w*Matrix(3, 0);
		y = Vertices[i].x*Matrix(0, 1) +
			Vertices[i].y*Matrix(1, 1) +
			Vertices[i].z*Matrix(2, 1) +
			Matrix(3,1); //Vertices[i].w*Matrix(3, 1);
		z = Vertices[i].x*Matrix(0, 2) +
			Vertices[i].y*Matrix(1, 2) +
			Vertices[i].z*Matrix(2, 2) +
			Matrix(3,2); //Vertices[i].w*Matrix(3, 2);
		w = Vertices[i].x*Matrix(0,3) +
			Vertices[i].y*Matrix(1,3) +
			Vertices[i].z*Matrix(2,3) +
			Matrix(3,3); //Vertices[i].w*Matrix(3, 3);
		x /= w;
		y /= w;
		z /= w;
		w = 1.f/w; 

        NewVertices[i].x = x;
		NewVertices[i].y = y;
		NewVertices[i].z = z;
	}
}

bool CTessFNTest::ProcessFrame(void)
{
	CSurface *pSurf1 = NULL, *pSurf2 = NULL;
	HRESULT   hr;
	bool      result = true;

    if (KeySet("standard") || NULL == m_pRefDevice)
		return CD3DTest::ProcessFrame();

	hr = m_pSrcDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pSurf1);
	if(FAILED(hr))
	{
		WriteToLog("CheckPrimitiveColor: GetBackBufferCached failed with result hr=%s(%x).\n",	m_pD3D->HResultToString(hr), hr);
		return false;
	}
	hr = m_pRefDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pSurf2);
	if(FAILED(hr))
	{
		WriteToLog("CheckPrimitiveColor: GetBackBufferCached failed with result hr=%s(%x).\n",	m_pD3D->HResultToString(hr), hr);
		RELEASE(pSurf1);
		return false;
	}

    int         nPitch1, nPitch2;
    LOCKED_RECT LockRect1, LockRect2;
    void	    *pBuf1, *pBuf2;
	DWORD RedMask1   = pSurf1->m_Mask.dwRedMask;
	DWORD GreenMask1 = pSurf1->m_Mask.dwGreenMask;
	DWORD BlueMask1  = pSurf1->m_Mask.dwBlueMask;
	DWORD amount = 0;

    // Get the Surface data pointer for our subrect
    hr = pSurf1->LockRect( &LockRect1, NULL, NULL );
    if (FAILED(hr))
	{
		WriteToLog("CheckPrimitiveColor: LockRect failed with HResult = %s(%x).\n", m_pD3D->HResultToString(hr), hr);
		RELEASE(pSurf1);
		RELEASE(pSurf2);
        return false;
	}
	pBuf1 = LockRect1.pBits;
    nPitch1 = LockRect1.iPitch;
	hr = pSurf2->LockRect( &LockRect2, NULL, NULL );
    if (FAILED(hr))
	{
		WriteToLog("CheckPrimitiveColor: LockRect failed with HResult = %s(%x).\n", m_pD3D->HResultToString(hr), hr);
		RELEASE(pSurf1);
		RELEASE(pSurf2);
        return false;
	}
	pBuf2 = LockRect2.pBits;
    nPitch2 = LockRect2.iPitch;
	DWORD RedMask2   = pSurf2->m_Mask.dwRedMask;
	DWORD GreenMask2 = pSurf2->m_Mask.dwGreenMask;
	DWORD BlueMask2  = pSurf2->m_Mask.dwBlueMask;

	DWORD Width  = m_Options.D3DOptions.nBackBufferWidth;
	DWORD Height = m_Options.D3DOptions.nBackBufferHeight;
	void *pColor1, *pColor2;
	DWORD c1, c2;
	DWORD count_holes1 = 0, count_holes2 = 0;
	DWORD count_diffcolors1 = 0, count_diffcolors2 = 0;
	DWORD color1_r = 0, color1_g = 0, color1_b = RGBA_GETBLUE(0xff000032) & BlueMask1;
	DWORD color2_r = 0, color2_g = RGBA_GETGREEN(0xff00ff00) & GreenMask1, color2_b = 0;
	DWORD color3_r = 0, color3_g = 0, color3_b = RGBA_GETBLUE(0xff0000ff) & BlueMask1;
	DWORD count11 = 0, count21 = 0, count31 = 0;
	DWORD count12 = 0, count22 = 0, count32 = 0;

	for (int y=0; y < Height; y++)		// Height loop
	{
		pColor1 = (char*)pBuf1 + (nPitch1 * y);
		pColor2 = (char*)pBuf2 + (nPitch2 * y);
		//outside of the circle by y should be background only
		if ( y < m_iY[0] || y >= m_iY[3] )
		{
			for (int x = 0; x < Width; x++)	// Width loop
			{
				c1 = pSurf1->ReadColor(&pColor1);
				c2 = pSurf2->ReadColor(&pColor2);
				
				if ( abs(float((RGBA_GETRED(c1)   & RedMask1  ) - color1_r)) > amount ||
					 abs(float((RGBA_GETGREEN(c1) & GreenMask1) - color1_g)) > amount ||
					 abs(float((RGBA_GETBLUE(c1)  & BlueMask1 ) - color1_b)) > amount  )
					count_diffcolors1++;
				if ( abs(float((RGBA_GETRED(c2)   & RedMask2  ) - color1_r)) > amount ||
					 abs(float((RGBA_GETGREEN(c2) & GreenMask2) - color1_g)) > amount ||
					 abs(float((RGBA_GETBLUE(c2)  & BlueMask2 ) - color1_b)) > amount  )
					count_diffcolors2++;
			}
		}
		else
		{
			//count how many time the color segments were presented in a line
			DWORD left11 = 0, left12 = 0, left21 = 0, left22 = 0, left31 = 0, left32 = 0;
			DWORD curc1 = 0, curc2 = 0, prevc1 = 0, prevc2 = 0; 

			//inside the circle but outside the square by y
			if ( y < m_iY[1] || y >= m_iY[2] )
			{
				for (int x = 0; x < Width; x++)	// Width loop
				{
					c1 = pSurf1->ReadColor(&pColor1);
					c2 = pSurf2->ReadColor(&pColor2);

					if ( x == 0 )
					{	curc1 = c1; curc2 = c2; }

					//outside the circle by x and y
					if ( x < m_iX[0] || x >= m_iX[3] )
					{
						if ( abs(float((RGBA_GETRED(c1)   & RedMask1  ) - color1_r)) > amount ||
							 abs(float((RGBA_GETGREEN(c1) & GreenMask1) - color1_g)) > amount ||
							 abs(float((RGBA_GETBLUE(c1)  & BlueMask1 ) - color1_b)) > amount  )
							count_diffcolors1++;
						if ( abs(float((RGBA_GETRED(c2)   & RedMask2  ) - color1_r)) > amount ||
							 abs(float((RGBA_GETGREEN(c2) & GreenMask2) - color1_g)) > amount ||
							 abs(float((RGBA_GETBLUE(c2)  & BlueMask2 ) - color1_b)) > amount  )
							count_diffcolors2++;
					}
					else
					{
						if ( abs(float((RGBA_GETRED(c1)   & RedMask1  ) - color1_r)) <= amount &&
							 abs(float((RGBA_GETGREEN(c1) & GreenMask1) - color1_g)) <= amount &&
							 abs(float((RGBA_GETBLUE(c1)  & BlueMask1 ) - color1_b)) <= amount  ) 
							count11++;
						else if ( abs(float((RGBA_GETRED(c1)   & RedMask1  ) - color2_r)) <= amount &&
								  abs(float((RGBA_GETGREEN(c1) & GreenMask1) - color2_g)) <= amount &&
								  abs(float((RGBA_GETBLUE(c1)  & BlueMask1 ) - color2_b)) <= amount  ) 
							count21++;
						else if ( abs(float((RGBA_GETRED(c1)   & RedMask1  ) - color3_r)) <= amount &&
								  abs(float((RGBA_GETGREEN(c1) & GreenMask1) - color3_g)) <= amount &&
								  abs(float((RGBA_GETBLUE(c1)  & BlueMask1 ) - color3_b)) <= amount  ) 
							count31++;
						else
							count_diffcolors1++;
						prevc1 = curc1; 
						curc1 = c1;
						if ( prevc1 != curc1 )
						{
							if ( abs(float((RGBA_GETRED(prevc1)   & RedMask1  ) - color1_r)) <= amount &&
								 abs(float((RGBA_GETGREEN(prevc1) & GreenMask1) - color1_g)) <= amount &&
								 abs(float((RGBA_GETBLUE(prevc1)  & BlueMask1 ) - color1_b)) <= amount  )
							{
								if (left11 > 1)
									count_holes1++;
								else
									left11++;
							}
							else if ( abs(float((RGBA_GETRED(prevc1)   & RedMask1  ) - color2_r)) <= amount &&
								      abs(float((RGBA_GETGREEN(prevc1) & GreenMask1) - color2_g)) <= amount &&
								      abs(float((RGBA_GETBLUE(prevc1)  & BlueMask1 ) - color2_b)) <= amount  ) 
							{
								if (left21 > 1)
									count_holes1++;
								else
									left21++;
							}
							else if ( abs(float((RGBA_GETRED(prevc1)   & RedMask1  ) - color3_r)) <= amount &&
								      abs(float((RGBA_GETGREEN(prevc1) & GreenMask1) - color3_g)) <= amount &&
								      abs(float((RGBA_GETBLUE(prevc1)  & BlueMask1 ) - color3_b)) <= amount  ) 
							{
								if (left31 > 0)
									count_holes1++;
								else
									left31++;
							}
						}

						if ( abs(float((RGBA_GETRED(c2)   & RedMask2  ) - color1_r)) <= amount &&
							 abs(float((RGBA_GETGREEN(c2) & GreenMask2) - color1_g)) <= amount &&
							 abs(float((RGBA_GETBLUE(c2)  & BlueMask2 ) - color1_b)) <= amount  )
							count12++;
						else if ( abs(float((RGBA_GETRED(c2)   & RedMask2  ) - color2_r)) <= amount &&
							      abs(float((RGBA_GETGREEN(c2) & GreenMask2) - color2_g)) <= amount &&
							      abs(float((RGBA_GETBLUE(c2)  & BlueMask2 ) - color2_b)) <= amount  ) 
							count22++;
						else if ( abs(float((RGBA_GETRED(c2)   & RedMask2  ) - color3_r)) <= amount &&
							      abs(float((RGBA_GETGREEN(c2) & GreenMask2) - color3_g)) <= amount &&
							      abs(float((RGBA_GETBLUE(c2)  & BlueMask2 ) - color3_b)) <= amount  ) 
							count32++;
						else
							count_diffcolors2++;
						prevc2 = curc2;
						curc2 = c2;
						if ( prevc2 != curc2 )
						{
							if ( abs(float((RGBA_GETRED(prevc2)   & RedMask2  ) - color1_r)) <= amount &&
								 abs(float((RGBA_GETGREEN(prevc2) & GreenMask2) - color1_g)) <= amount &&
								 abs(float((RGBA_GETBLUE(prevc2)  & BlueMask2 ) - color1_b)) <= amount  )
							{
								if (left12 > 1)
									count_holes2++;
								else
									left12++;
							}
							else if ( abs(float((RGBA_GETRED(prevc2)   & RedMask2  ) - color2_r)) <= amount &&
								      abs(float((RGBA_GETGREEN(prevc2) & GreenMask2) - color2_g)) <= amount &&
								      abs(float((RGBA_GETBLUE(prevc2)  & BlueMask2 ) - color2_b)) <= amount  ) 
							{
								if (left22 > 1)
									count_holes2++;
								else
									left22++;
							}
							else if ( abs(float((RGBA_GETRED(prevc2)   & RedMask2  ) - color3_r)) <= amount &&
								      abs(float((RGBA_GETGREEN(prevc2) & GreenMask2) - color3_g)) <= amount &&
								      abs(float((RGBA_GETBLUE(prevc2)  & BlueMask2 ) - color3_b)) <= amount  ) 
							{
								if (left32 > 0)
									count_holes2++;
								else
									left32++;
							}
						}
					}
				}
			}
			else
			{
				for (int x = 0; x < Width; x++)	// Width loop
				{
					c1 = pSurf1->ReadColor(&pColor1);
					c2 = pSurf2->ReadColor(&pColor2);
					if ( x == 0 )
					{	curc1 = c1; curc2 = c2; }

					//outside the circle
					if ( x < m_iX[0] || x >= m_iX[3] )
					{
						if ( abs(float((RGBA_GETRED(c1)   & RedMask1  ) - color1_r)) > amount ||
						     abs(float((RGBA_GETGREEN(c1) & GreenMask1) - color1_g)) > amount ||
						     abs(float((RGBA_GETBLUE(c1)  & BlueMask1 ) - color1_b)) > amount  )
							count_diffcolors1++;
						if ( abs(float((RGBA_GETRED(c2)   & RedMask2  ) - color1_r)) > amount ||
						     abs(float((RGBA_GETGREEN(c2) & GreenMask2) - color1_g)) > amount ||
						     abs(float((RGBA_GETBLUE(c2)  & BlueMask2 ) - color1_b)) > amount  )
							count_diffcolors2++;
					}
					else if ( x < m_iX[1] || x >= m_iX[2] )
					{
						if ( abs(float((RGBA_GETRED(c1)   & RedMask1  ) - color1_r)) <= amount &&
							 abs(float((RGBA_GETGREEN(c1) & GreenMask1) - color1_g)) <= amount &&
							 abs(float((RGBA_GETBLUE(c1)  & BlueMask1 ) - color1_b)) <= amount  ) 
							count11++;
						else if ( abs(float((RGBA_GETRED(c1)   & RedMask1  ) - color2_r)) <= amount &&
								  abs(float((RGBA_GETGREEN(c1) & GreenMask1) - color2_g)) <= amount &&
								  abs(float((RGBA_GETBLUE(c1)  & BlueMask1 ) - color2_b)) <= amount  ) 
							count21++;
						else if ( abs(float((RGBA_GETRED(c1)   & RedMask1  ) - color3_r)) <= amount &&
								  abs(float((RGBA_GETGREEN(c1) & GreenMask1) - color3_g)) <= amount &&
								  abs(float((RGBA_GETBLUE(c1)  & BlueMask1 ) - color3_b)) <= amount  ) 
							count31++;
						else
							count_diffcolors1++;
						prevc1 = curc1; 
						curc1 = c1;
						if ( prevc1 != curc1 )
						{
							if ( abs(float((RGBA_GETRED(prevc1)   & RedMask1  ) - color1_r)) <= amount &&
								 abs(float((RGBA_GETGREEN(prevc1) & GreenMask1) - color1_g)) <= amount &&
								 abs(float((RGBA_GETBLUE(prevc1)  & BlueMask1 ) - color1_b)) <= amount  )
							{
								if (left11 > 1)
									count_holes1++;
								else
									left11++;
							}
							else if ( abs(float((RGBA_GETRED(prevc1)   & RedMask1  ) - color2_r)) <= amount &&
								      abs(float((RGBA_GETGREEN(prevc1) & GreenMask1) - color2_g)) <= amount &&
								      abs(float((RGBA_GETBLUE(prevc1)  & BlueMask1 ) - color2_b)) <= amount  ) 
							{
								if (left21 > 1)
									count_holes1++;
								else
									left21++;
							}
							else if ( abs(float((RGBA_GETRED(prevc1)   & RedMask1  ) - color3_r)) <= amount &&
								      abs(float((RGBA_GETGREEN(prevc1) & GreenMask1) - color3_g)) <= amount &&
								      abs(float((RGBA_GETBLUE(prevc1)  & BlueMask1 ) - color3_b)) <= amount  ) 
							{
								if (left31 > 0)
									count_holes1++;
								else
									left31++;
							}
						}

						if ( abs(float((RGBA_GETRED(c2)   & RedMask2  ) - color1_r)) <= amount &&
							 abs(float((RGBA_GETGREEN(c2) & GreenMask2) - color1_g)) <= amount &&
							 abs(float((RGBA_GETBLUE(c2)  & BlueMask2 ) - color1_b)) <= amount  )
							count12++;
						else if ( abs(float((RGBA_GETRED(c2)   & RedMask2  ) - color2_r)) <= amount &&
							      abs(float((RGBA_GETGREEN(c2) & GreenMask2) - color2_g)) <= amount &&
							      abs(float((RGBA_GETBLUE(c2)  & BlueMask2 ) - color2_b)) <= amount  ) 
							count22++;
						else if ( abs(float((RGBA_GETRED(c2)   & RedMask2  ) - color3_r)) <= amount &&
							      abs(float((RGBA_GETGREEN(c2) & GreenMask2) - color3_g)) <= amount &&
							      abs(float((RGBA_GETBLUE(c2)  & BlueMask2 ) - color3_b)) <= amount  ) 
							count32++;
						else
							count_diffcolors2++;
						prevc2 = curc2;
						curc2 = c2;
						if ( prevc2 != curc2 )
						{
							if ( abs(float((RGBA_GETRED(prevc2)   & RedMask2  ) - color1_r)) <= amount &&
								 abs(float((RGBA_GETGREEN(prevc2) & GreenMask2) - color1_g)) <= amount &&
								 abs(float((RGBA_GETBLUE(prevc2)  & BlueMask2 ) - color1_b)) <= amount  )
							{
								if (left12 > 1)
									count_holes2++;
								else
									left12++;
							}
							else if ( abs(float((RGBA_GETRED(prevc2)   & RedMask2  ) - color2_r)) <= amount &&
								      abs(float((RGBA_GETGREEN(prevc2) & GreenMask2) - color2_g)) <= amount &&
								      abs(float((RGBA_GETBLUE(prevc2)  & BlueMask2 ) - color2_b)) <= amount  ) 
							{
								if (left22 > 1)
									count_holes2++;
								else
									left22++;
							}
							else if ( abs(float((RGBA_GETRED(prevc2)   & RedMask2  ) - color3_r)) <= amount &&
								      abs(float((RGBA_GETGREEN(prevc2) & GreenMask2) - color3_g)) <= amount &&
								      abs(float((RGBA_GETBLUE(prevc2)  & BlueMask2 ) - color3_b)) <= amount  ) 
							{
								if (left32 > 0)
									count_holes2++;
								else
									left32++;
							}
						}
					}
					else
					{
						if ( abs(float((RGBA_GETRED(c1)   & RedMask1  ) - color3_r)) > amount ||
						     abs(float((RGBA_GETGREEN(c1) & GreenMask1) - color3_g)) > amount ||
						     abs(float((RGBA_GETBLUE(c1)  & BlueMask1 ) - color3_b)) > amount  )
							count_diffcolors1++;
						if ( abs(float((RGBA_GETRED(c2)   & RedMask2  ) - color3_r)) > amount ||
						     abs(float((RGBA_GETGREEN(c2) & GreenMask2) - color3_g)) > amount ||
						     abs(float((RGBA_GETBLUE(c2)  & BlueMask2 ) - color3_b)) > amount  )
							count_diffcolors2++;
					}
				}
			}
		}
	}

	hr = pSurf1->UnlockRect();
    if (FAILED(hr))
    {
	    WriteToLog("CheckPrimitiveColor: UnlockRect failed with HResult = %s(%x).\n", m_pD3D->HResultToString(hr), hr);
		pSurf2->UnlockRect();
		RELEASE(pSurf1);
		RELEASE(pSurf2);
        return false;
    }
	RELEASE(pSurf1);
	hr = pSurf2->UnlockRect();
    if (FAILED(hr))
    {
	    WriteToLog("CheckPrimitiveColor: UnlockRect failed with HResult = %s(%x).\n", m_pD3D->HResultToString(hr), hr);
		RELEASE(pSurf1);
		RELEASE(pSurf2);
        return false;
    }
	RELEASE(pSurf2);

	if ((int)(count32 - count31) > 0.01*count32)
	{
		WriteToLog("Blue on HAL %d should be higher than on REF %d.\n", count31, count32);
		Fail();
		result = false;
	}
	if ( (int)(count11 - count12) > 0.01*count12 )
	{
		WriteToLog("Background on REF %d should be higher than on HAL %d.\n", count12, count11);
		Fail();
		result = false;
	}
	if (count21 == 0)
	{
		WriteToLog("Tess Factor doesn't work on HAL.\n");
		Fail();
		result = false;
	}
	if (count22 == 0)
	{
		WriteToLog("Tess Factor doesn't work on REF.\n");
		Fail();
		result = false;
	}
	if (count_holes1)
	{
		WriteToLog("have %d holes on HAL device.\n", count_holes1);
		Fail();
		result = false;
	}
	if (count_holes2)
	{
		WriteToLog("have %d holes on REF device.\n", count_holes2);
		Fail();
		result = false;
	}
	if (count_diffcolors1)
	{
		WriteToLog("have %d color mismatches on HAL device.\n", count_diffcolors1);
		Fail();
		result = false;
	}
	if (count_diffcolors2)
	{
		WriteToLog("have %d color mismatches on REF device.\n", count_diffcolors2);
		Fail();
		result = false;
	}
	//WriteToLog("count11=%d\tcount21=%d\tcount31=%d\n", count11,count21,count31);
	//WriteToLog("count12=%d\tcount22=%d\tcount32=%d\n", count12,count22,count32);
	//WriteToLog("count_diffcolors1=%d\tcount_holes1=%d\n", count_diffcolors1, count_holes1);
	//WriteToLog("count_diffcolors2=%d\tcount_holes2=%d\n", count_diffcolors2, count_holes2);
	//Fail();
	//GetLastError();
    
	if (result)
		Pass();
	else
		Fail();
	return result;
}

bool CTessFNTest::Setup()
{
	//Tessellation::Setup();
	SetDefaultMatrices();
	SetDefaultRenderStates();
	SetDefaultLights();


	//set vertex shaders
	if ((m_UseDeclShader || m_UseDeclFixed || m_UseFVFShader) && !GenerateVertexShader())
	{
		WriteToLog("CTessFNTest::Setup - GenerateVertexShader failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
	}
	if ( !SetVertexDeclaration(m_pDecl) )
	{
		WriteToLog("CTessFNTest::Setup - SetVertexDeclaration(m_pDecl) failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
		return false;
	}
	
	//generate D3DX tessellated mesh
	if (!GenerateBuffers())
		return false;

	float cosPI_4 = cos(D3DX_PI/4.f);
	float sinPI_4 = sin(D3DX_PI/4.f);
	D3DXMATRIX Matrix, Matrix1;
	float fZn = -5.f;
	float fZf = 5.f;
	D3DXMatrixOrthoLH(&m_ProjMatrix, 2.f, 2.f, fZn, fZf);
		
	D3DXVECTOR3 vFrom = D3DXVECTOR3( 0.0f,  0.0f, -5.0f);
	D3DXVECTOR3 vAt   = D3DXVECTOR3( 0.0f,  0.0f, 0.0f);
	D3DXVECTOR3 vUp   = D3DXVECTOR3( 0.0f,  1.0f, 0.0f);	
	
	D3DXMatrixLookAtLH(&(m_ViewMatrices[0]), &vFrom, &vAt, &vUp);
	D3DXMatrixMultiply(&(m_WVMatrices[0]), &m_WorldMatrix, &(m_ViewMatrices[0]));
	D3DXMatrixMultiply(&m_FinalMatrix, &m_WVMatrices[0], &m_ProjMatrix);
	//viewport matrix
    D3DXMatrixIdentity( &Matrix );
	Matrix(0,0) =  0.f + (FLOAT)( m_pCurrentMode->nBackBufferWidth/2 );
	Matrix(3,0) =  0.f + (FLOAT)( m_pCurrentMode->nBackBufferWidth/2 );
	Matrix(1,1) = -1.f * (FLOAT)( m_pCurrentMode->nBackBufferHeight/2);
	Matrix(3,1) =  0.f + (FLOAT)( m_pCurrentMode->nBackBufferHeight/2);
	Matrix(2,2) = 1.f - 0.f;
	Matrix(3,2) = 0.f;
	Matrix(3,3) = 1.f;
	Matrix1 = m_FinalMatrix;
	D3DXMatrixMultiply(&m_FinalMatrix, &Matrix1, &Matrix);
	D3DXMATRIX mat;
	D3DXMatrixMultiply( &mat, &m_WVMatrices[0], &m_ProjMatrix);
	D3DXMatrixTranspose( &mat, &mat );   
	SetVertexShaderConstant(0, &mat, 4); 
	SetTransform(D3DTS_VIEW, &m_ViewMatrices[0]);
	SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);
	SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	
	D3DXVECTOR3 fX[4], fY[4], newX[4], newY[4];

	fX[0] = D3DXVECTOR3(-1.f, 0.f, 0.f);
	fX[1] = D3DXVECTOR3(-cosPI_4, 0.f, 0.f);
	fX[2] = D3DXVECTOR3(cosPI_4, 0.f, 0.f);
	fX[3] = D3DXVECTOR3(1.f, 0.f, 0.f);
	fY[0] = D3DXVECTOR3(0.f, 1.f, 0.f);
	fY[1] = D3DXVECTOR3(0.f, sinPI_4, 0.f);
	fY[2] = D3DXVECTOR3(0.f, -sinPI_4, 0.f);
	fY[3] = D3DXVECTOR3(0.f, -1.f, 0.f);

	TransformVertex(m_FinalMatrix, fX, newX, 4);
	TransformVertex(m_FinalMatrix, fY, newY, 4);

	m_iX[0] = (int)floor(newX[0].x + 0.5f);
	m_iX[1] = (int)floor(newX[1].x + 0.5f);
	m_iX[2] = (int)floor(newX[2].x + 0.5f);
	m_iX[3] = (int)floor(newX[3].x + 0.5f);

	m_iY[0] = (int)floor(newY[0].y + 0.5f);
	m_iY[1] = (int)floor(newY[1].y + 0.5f);
	m_iY[2] = (int)floor(newY[2].y + 0.5f);
	m_iY[3] = (int)floor(newY[3].y + 0.5f);


	float AdaptiveTess_x = 0.f;
	float AdaptiveTess_y = 0.f;
	float AdaptiveTess_z = -0.5f;//-2/a  //2.f;
	float AdaptiveTess_w = 1.f;
	SetRenderState(D3DRS_ADAPTIVETESS_X, *(DWORD*)&AdaptiveTess_x);
	SetRenderState(D3DRS_ADAPTIVETESS_Y, *(DWORD*)&AdaptiveTess_y);
	SetRenderState(D3DRS_ADAPTIVETESS_Z, *(DWORD*)&AdaptiveTess_z);
	SetRenderState(D3DRS_ADAPTIVETESS_W, *(DWORD*)&AdaptiveTess_w);
	//m_pSrcDevice->SetRenderState(D3DRS_LIGHTING, (DWORD)FALSE);
	//m_pSrcDevice->SetRenderState(D3DRS_AMBIENT,0xffffffff);
	return true;
}

void CTessFNTest::Cleanup()
{
	delete [] m_pVCircle;
	m_pVCircle = NULL;
	delete [] m_pICircle;
	m_pICircle = NULL;
	delete [] m_pVEllipse;
	m_pVEllipse = NULL;
	delete [] m_pIEllipse;
	m_pIEllipse = NULL;
	RELEASE(m_pShader);
	RELEASE(m_pDecl);
}

void CTessFNTest::SetNewMaterial(UINT index)
{
	
	m_HOMat.emissive.r = 0.0f;
	m_HOMat.emissive.g = 0.0f;
	m_HOMat.emissive.b = 0.0f;
	m_HOMat.emissive.a = 1.0f;
	switch (index%3)
	{
		case 0: 
		{
            m_HOMat.emissive.r = 1.0f;
			break;
		}
		case 1: 
		{
            m_HOMat.emissive.g = 1.0f;   
			break;
		}
		case 2: 
		{
            m_HOMat.emissive.b = 1.0f;
			break;
		}
		
	}
	m_HOMat.power = 10.0f;
	SetMaterial(&m_HOMat);
	float emissive_color[4] = { m_HOMat.emissive.r, m_HOMat.emissive.g, m_HOMat.emissive.b, 0 };
	if ( !SetVertexShaderConstantF( 4, emissive_color, 1 ))
		WriteToLog("SetVertexShaderConstantF: Failed to set new emissive color.\n");
}

