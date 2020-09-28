#define D3D_OVERLOADS
#include "Lines.h"

// Global Test and App instance
CD3DWindowFramework App;

#ifndef FAST_TEST
CBasicTest		BasicTest1 (false, false, false, D3DSHADE_FLAT);		//standard lines, flat shading
CBasicTest      BasicTest2 (false, false, false, D3DSHADE_GOURAUD);		//standard lines, gouraud shading
#endif
CBasicTest      BasicTest3 (false, true,  false, D3DSHADE_FLAT);		//tex      lines, flat shading
CBasicTest      BasicTest4 (false, true,  false, D3DSHADE_GOURAUD);		//tex      lines, gouraud shading
#ifndef FAST_TEST
CBasicTest      BasicTest5 (false, false, true,  D3DSHADE_FLAT);		//pattern  lines, flat shading
CBasicTest      BasicTest6 (false, false, true,  D3DSHADE_GOURAUD);		//pattern  lines, gouraud shading
CBasicTest      BasicTest7 (false, true,  true,  D3DSHADE_FLAT);		//tex/patt lines, flat shading
CBasicTest      BasicTest8 (false, true,  true,  D3DSHADE_GOURAUD);		//tex/patt lines, gouraud shading
CBasicTest      BasicTest9 (true,  false, false, D3DSHADE_FLAT);		//AA       lines, flat shading
CBasicTest		BasicTest10(true,  false, false, D3DSHADE_GOURAUD);		//AA       lines, gouraud shading
CBasicTest		BasicTest11(true,  true,  false, D3DSHADE_FLAT);		//AA/tex   lines, flat shading
CBasicTest		BasicTest12(true,  true,  false, D3DSHADE_GOURAUD);		//AA/tex   lines, gouraud shading

CPFogTest		PFogTest1 (false, false, false, D3DSHADE_FLAT);			//standard lines, flat shading
CPFogTest		PFogTest2 (false, false, false, D3DSHADE_GOURAUD);		//standard lines, gouraud shading
CPFogTest		PFogTest3 (false, true,  false, D3DSHADE_FLAT);			//tex      lines, flat shading
CPFogTest		PFogTest4 (false, true,  false, D3DSHADE_GOURAUD);		//tex      lines, gouraud shading
CPFogTest		PFogTest5 (false, false, true,  D3DSHADE_FLAT);			//pattern  lines, flat shading
CPFogTest		PFogTest6 (false, false, true,  D3DSHADE_GOURAUD);		//pattern  lines, gouraud shading
CPFogTest		PFogTest7 (false, true,  true,  D3DSHADE_FLAT);			//tex/patt lines, flat shading
CPFogTest		PFogTest8 (false, true,  true,  D3DSHADE_GOURAUD);		//tex/patt lines, gouraud shading
CPFogTest		PFogTest9 (true,  false, false, D3DSHADE_FLAT);			//AA       lines, flat shading
CPFogTest		PFogTest10(true,  false, false, D3DSHADE_GOURAUD);		//AA       lines, gouraud shading
CPFogTest		PFogTest11(true,  true,  false, D3DSHADE_FLAT);			//AA/tex   lines, flat shading
CPFogTest		PFogTest12(true,  true,  false, D3DSHADE_GOURAUD);		//AA/tex   lines, gouraud shading
#endif

CMultiTexTest	MultiTexTest1(false, false, D3DSHADE_FLAT);		//tex      lines, flat shading
#ifndef FAST_TEST
CMultiTexTest	MultiTexTest2(false, false, D3DSHADE_GOURAUD);	//tex      lines, gouraud shading
CMultiTexTest	MultiTexTest3(false, true,  D3DSHADE_FLAT);		//tex/patt lines, flat shading
CMultiTexTest	MultiTexTest4(false, true,  D3DSHADE_GOURAUD);	//tex/patt lines, gouraud shading
CMultiTexTest	MultiTexTest5(true,  false, D3DSHADE_FLAT);		//AA/tex   lines, flat shading
CMultiTexTest	MultiTexTest6(true,  false, D3DSHADE_GOURAUD);	//AA/tex   lines, gouraud shading
#endif

WORD g_wPatterns[] = { 0xAAAA, //1010101010101010
					   0xDB6D, //1101101101101101
					   0x2492, //0010010010010010
					   0x7777, //0111011101110111
					   0x8887, //1000100010000111
					   0xF7BD, //1111011110111101
					   0x842,  //0000100001000010
					   0xD6B5, //1101011010110101
					   0xED76, //1110110101110110
					   0xEDB7, //1110110110110111
                       0x6996, //0110100110010110
					   0xE3c7, //1110001111000111
					   0x5665, //0101011001100101 
					   0xF00F, //1111000000001111
					   0x3333  //0011001100110011
					 };
#define g_nPatterns ( sizeof(g_wPatterns) / (sizeof(g_wPatterns[0])) )

//-----------------------------------------------------------------------------

Lines::Lines(bool isAntiAliased, bool withTexture, bool withPattern, D3DSHADEMODE shading)
{
	//m_szTestName   = "Lines";
	//m_szCommandKey = "Lines";
	m_szLogName    = "Lines";
	SetTestType(TESTTYPE_CONF);
	m_Options.D3DOptions.fMinDXVersion = 9.f;

	m_isAntiAliased = isAntiAliased;
	m_withTexture   = withTexture;
	m_withPattern   = withPattern;
	m_bPatRepeat    = withPattern;
	m_ShadingMode   = shading;

	szTestName[0] = '\0';
	szCommandKey[0] = '\0';

	if ( m_isAntiAliased )
	{
		sprintf(szTestName, "%s Anti-Aliased Lines", szTestName);
		sprintf(szCommandKey, "%saa", szCommandKey);
	}
	else
	{
		sprintf(szTestName, "%s Lines", szTestName);
		sprintf(szCommandKey, "%sa", szCommandKey);
	}
	
	if ( m_withTexture )
	{
		sprintf(szTestName, "%s with Tex", szTestName);
		sprintf(szCommandKey, "%st", szCommandKey);
	}
	
	if ( m_withPattern )
	{
		m_Options.D3DOptions.fMaxDXVersion = 8.1f; //patterns are not supported in DX9
		m_Options.D3DOptions.fMinDXVersion = 8.0f;

        sprintf(szTestName, "%s with Pattern", szTestName);
		sprintf(szCommandKey, "%sp", szCommandKey);

		if ( KeySet("nopattern") )
		{
			m_withPattern = false;
			m_bPatRepeat  = false;
		}
		if ( KeySet("norepeat") )
		{
			//m_withPattern = false;
			m_bPatRepeat  = false;
		}
	}	

	if ( m_ShadingMode == D3DSHADE_FLAT )
	{
		sprintf(szTestName, "%s, D3DSHADE_FLAT", szTestName);
		sprintf(szCommandKey, "%sf", szCommandKey);
	}
	else
	{
		sprintf(szTestName, "%s, D3DSHADE_GOURAUD", szTestName);
		sprintf(szCommandKey, "%sg", szCommandKey);
	}

	//m_pVB = NULL;
	//m_pIB = NULL;
	m_pVB        = NULL;
	m_pVBTL      = NULL;
	m_pIB        = NULL;
	m_nPrimTypes = 3;

#ifdef FAST_TEST
	m_nX = 1;
	m_nZ = 1;
#else
	m_nX = 15;
	m_nZ = 10;
#endif

}

//-----------------------------------------------------------------------------

bool Lines::CapsCheck()
{
	const DEVICEDESC* pCaps = m_pSrcDevice->GetCaps();

	if ( m_isAntiAliased && !(pCaps->dwLineCaps & D3DLINECAPS_ANTIALIAS) )
	{
		WriteToLog("CapsCheck: AntiAliased lines are not supported by the driver.\n");
		return false;
	}
	if ( m_withTexture && !(pCaps->dwLineCaps & D3DLINECAPS_TEXTURE) )
	{
		WriteToLog("CapsCheck: Texture-mapping is not supported by the driver.\n");
		return false;
	}

	if ( m_withPattern && !(pCaps->dwRasterCaps & 0x00000008L/*D3DPRASTERCAPS_PAT*/))
    {
        WriteToLog("CapsCheck: Line patterns are not supported.\n");
		return false;
    }

	if ( m_withPattern && !(pCaps->dwPrimitiveMiscCaps & 0x00000004L/*D3DPMISCCAPS_LINEPATTERNREP*/))
    {
        WriteToLog("CapsCheck: Pattern repetition is not supported.\n");
		m_bPatRepeat = false;
	}

	return true;
}

//-----------------------------------------------------------------------------

bool Lines::ProcessFrame()
{
	bool result = CD3DTest::ProcessFrame();

	if (KeySet("failall"))
	{
		Fail();
		result = false;
	}
	else
	//If anti-aliasing is turned on and 1st test case fails(swirls and grids) then we skip the rest of current test group.
	//This way we avoid a lot of failures on anti-aliasing.
	if ( m_uTestNumber == 1 && m_isAntiAliased && m_pFramework->m_DXGLog.GetCurrentResult() == WTT_TESTCASE_RESULT_FAIL )
		SkipTestRange(2, m_uTotalTests);

	return result;
}

//-----------------------------------------------------------------------------

bool Lines::TestTerminate()
{
	if (m_pVB != NULL)
	{
		delete [] m_pVB;
		m_pVB = NULL;
	}
	if (m_pVBTL != NULL)
	{
		delete [] m_pVBTL;
		m_pVBTL = NULL;
	}
	if (m_pIB != NULL)
	{
		delete [] m_pIB;
		m_pIB = NULL;
	}

	return true;
}

//-----------------------------------------------------------------------------

bool Lines::TransformVertex(bool isTL, D3DXMATRIX Matrix, D3DVERTEX *Vertices, TLVERTEX *TLVertices, DWORD Size)
{
    float x, y, z, w;

	D3DXMATRIX Inverse;
	FLOAT determinant;
	D3DXMatrixInverse( &Inverse, &determinant, &Matrix );

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
		//w = 1.f/w;

		//want to zero out anything after 4 bits (for float part)
		x = (int)x + ((int)((x-(int)x)*16.f+0.5f))/16.f;
		y = (int)y + ((int)((y-(int)y)*16.f+0.5f))/16.f;
        
		/*if (x-(int)x == 0.5)
			x += 1/16.f;
			
		if (y-(int)y == 0.5)
			y += 1/16.f;*/

		if (!isTL)
		{
			x *= w;
			y *= w;
			z *= w;
		
			Vertices[i].x = x*Inverse(0,0) + y*Inverse(1,0) + z*Inverse(2,0) + w*Inverse(3,0); 
			Vertices[i].y = x*Inverse(0,1) + y*Inverse(1,1) + z*Inverse(2,1) + w*Inverse(3,1); 
			Vertices[i].z = x*Inverse(0,2) + y*Inverse(1,2) + z*Inverse(2,2) + w*Inverse(3,2); 
		}
		else
		{
            w = 1.f/w;
            TLVertices[i] = TLVERTEX(D3DXVECTOR4( x, y, z, w ), 0xffffffff, Vertices[i].tu, Vertices[i].tv);
		}
	}

	return true;
}

//-----------------------------------------------------------------------------

bool Lines::GenerateBuffers( D3DPRIMITIVETYPE primType, DWORD dwX, DWORD dwZ, D3DXMATRIX Matrix, bool isTL, bool isTexAddress)
{
	float      x, y, z, nx, ny, nz, u, v;
	DWORD      ind = 0;
	float      deltaZ = 1.f/(dwZ-1.f);
	int        indX, indY, indZ;

	//other values might cause errors
	ASSERT(dwX >= 3);
	ASSERT(dwZ >= 2);

	if (m_pVB != NULL)
	{
		delete [] m_pVB;
		m_pVB = NULL;
	}
	if (m_pVBTL != NULL)
	{
		delete [] m_pVBTL;
		m_pVBTL = NULL;
	}
	if (m_pIB != NULL)
	{
		delete [] m_pIB;
		m_pIB = NULL;
	}

	//initialize vertex vuffer
	if ( isTL )
	{
		m_pVBTL = new TLVERTEX [dwX*(dwZ-1) + 1];
		m_pVB   = new D3DVERTEX[dwX*(dwZ-1) + 1];
	}
	else
		m_pVB   = new D3DVERTEX[dwX*(dwZ-1) + 1];

	switch ( primType )
	{
		case D3DPT_LINELIST:
			m_nVertices = 2*dwX + (dwZ>2 ? 6*dwX*(dwZ-2) : 0) + 2*dwX;
			break;
		case D3DPT_LINESTRIP:
			m_nVertices = dwX*(2*dwZ - 1) + (dwZ - 2)*(dwX + 1);
			break;
		case D3DPT_TRIANGLELIST:
			m_nVertices = 2 + dwX + (2*dwX + 2)*((dwZ - 1)/2) + 6*dwX*((dwZ - 2)/2);
			break;
		default:
			WriteToLog("unknow D3DPT type %d.\n", primType);
			Fail();
			return false;
	}

	DWORD Width  = m_Options.D3DOptions.nBackBufferWidth;
	DWORD Height = m_Options.D3DOptions.nBackBufferHeight;
	for ( indZ = 0; indZ < dwZ; indZ++ )
	{
		if ( indZ == 0 )
			m_pVB[ind++] = D3DVERTEX(D3DXVECTOR3( 0.f, 0.f, 1.f ), D3DXVECTOR3( 0.f, 0.f, -1.f ), 0.5f, 0.5f);
		else
		{
			if (indZ == dwZ-1)
				z = 0.f;
			else
				z = sqrt( 1 - (indZ*deltaZ)*(indZ*deltaZ) );
			for (indX = 0; indX < dwX; indX++)
			{
				x = sqrt(1-z*z) * cos( (2.f*D3DX_PI*indX)/dwX );
				y = sqrt(1-z*z) * sin( (2.f*D3DX_PI*indX)/dwX );
				nx = -x;
				ny = -y;
				nz = -z;
				if ( isTexAddress )
				{
					u = 2*x;
					v = 2*y;
				}
				else
				{
					u = (1.f+x)/2.05f;
					v = (1.f-y)/2.05f;
				}
				
				m_pVB[ind++] = D3DVERTEX(D3DXVECTOR3( x, y, z ), D3DXVECTOR3( nx, ny, nz ), u, v); 
			}
		}
	}
	
	//for untransformed data snap to 4 bits of precisios (since it's REF limitation)
	//for TL data perform the transfer
	TransformVertex(isTL, Matrix, m_pVB, m_pVBTL, dwX*(dwZ-1) + 1);

	if (isTL)
	{
		DWORD colores[7][3] ={{ 1, 1, 1},  //grey
							  { 1, 0, 0},  //red
							  { 0, 1, 0},  //green
							  { 0, 0, 1},  //blue
							  { 0, 1, 1},  //cyan
							  { 1, 0, 1},  //magenta
							  { 1, 1, 0}}; //yellow;
		DWORD c, color;
		ind = 0;
				
		for (indZ = 0; indZ < dwZ; indZ++ )
		{
			c = 255 - indZ*10;
			for (indX = 0; indX < dwX; indX++)
			{
				c -= 10*indX;
				if (c < 100)
					c = 255;
				color = RGBA_MAKE(colores[indZ%7][0]*c, colores[indZ%7][1]*c, colores[indZ%7][2]*c, 255);
				m_pVBTL[ind++].color = color;
				if (indZ == 0) //we only have one vertex for inddZ==0
					break;
			}
		}
	}

	//initialize idnex buffer
	m_pIB = new WORD[m_nVertices];

	WORD index = 0, offset;
	switch (primType)
	{
		case D3DPT_LINELIST:
		{
			for ( indX = 0; indX < dwX; indX++ )
			{
				m_pIB[index++] = 0;
				m_pIB[index++] = indX + 1;
			}
			for( indZ = 1; indZ < dwZ-1; indZ++ )
			{
				offset = (WORD)( dwX*(indZ-1) + 1 );
				for ( indX = 0; indX < dwX-1; indX++ )
				{
					m_pIB[index++] = offset;
					m_pIB[index++] = offset + (WORD)dwX;
					m_pIB[index++] = offset;
					m_pIB[index++] = offset + (WORD)dwX + 1;
					m_pIB[index++] = offset;
					m_pIB[index++] = offset + 1;
					offset++;
				}
				m_pIB[index++] = offset;
				m_pIB[index++] = offset + (WORD)dwX;
				m_pIB[index++] = offset;
				m_pIB[index++] = offset + 1;
				m_pIB[index++] = offset;
				m_pIB[index++] = offset - (WORD)dwX + 1;
			}
			offset = (WORD)( dwX*(dwZ-2) + 1 );
			for ( indX = 0; indX < dwX-1; indX++ )
			{
				m_pIB[index++] = offset;
				m_pIB[index++] = offset + 1;
				offset++;
			}

			m_pIB[index++] = offset;
			m_pIB[index++] = offset - (WORD)dwX + 1;
			break;
		}
		case D3DPT_LINESTRIP:
		{
			for ( indX = 0; indX < dwX; indX++ )
			{
				m_pIB[index++] = 0;
				for ( indZ = 1; indZ < dwZ; indZ++ )
				{
					/*if ( indX < abs(dwX - indZ + 1) )
						IBarray[index++] = dwX*(indZ - 1) + indZ + indX;
					else
						IBarray[index++] = dwX*(indZ - 2) + indZ + indX;*/
					m_pIB[index++] = (WORD)( (indX+indZ-1)%dwX + dwX*(indZ - 1) + 1 );
				}
				if (m_pIB[index-1] != dwX*(dwZ-1) )//( indX != dwX - (dwZ - 1) )
					m_pIB[index++] = m_pIB[index-1] + 1;
				else
					m_pIB[index++] = (WORD)( dwX*(dwZ - 2) + 1 );

				for( indZ = dwZ - 2; indZ >= 1; indZ-- )
					m_pIB[index++] = m_pIB[index-1] - (WORD)dwX;
			}
			offset = 1;
			for( indZ = 1; indZ < dwZ - 1; indZ++ )
			{
				for ( indX = 0; indX < dwX; indX++ )
					m_pIB[index++] = offset++;
				m_pIB[index++] = offset - (WORD)dwX;
			}
			break;
		}
		case D3DPT_TRIANGLELIST:
		{
			//triangle fan
			m_pIB[index++] = 0;
			for ( indX = 0; indX < dwX; indX++ )
				m_pIB[index++] = indX + 1;
			m_pIB[index++] = 1;
			//triangle strips
			for( indZ = 1; indZ < dwZ - 1; indZ += 2 )
			{
				offset = (WORD)( dwX*(indZ-1) + 1 );
				for ( indX = 0; indX < dwX; indX++ )
				{
					m_pIB[index++] = offset + indX + (WORD)dwX;
					m_pIB[index++] = offset + indX;
				}
				m_pIB[index++] = offset + (WORD)dwX;
				m_pIB[index++] = offset;
			}
			//triagnle lists
			for( indZ = 2; indZ < dwZ - 1; indZ += 2 )
			{
				offset = (WORD)( dwX*(indZ-1) + 1 );
				for ( indX = 0; indX < dwX-1; indX++ )
				{
					m_pIB[index++] = offset + indX;
					m_pIB[index++] = offset + indX + (WORD)dwX + 1;
					m_pIB[index++] = offset + indX + (WORD)dwX;
					m_pIB[index++] = offset + indX;
					m_pIB[index++] = offset + indX + 1;
					m_pIB[index++] = offset + indX + (WORD)dwX + 1;
				}
				m_pIB[index++] = offset + indX;
				m_pIB[index++] = offset + indX + 1;
				m_pIB[index++] = offset + indX + (WORD)dwX;
				m_pIB[index++] = offset + indX;
				m_pIB[index++] = offset;
				m_pIB[index++] = offset + indX + 1;
			}
			break;
		}
	}
	
	if ( isTL )
	{
		if ( !SetVertexShader(FVF_TLVERTEX) )
		{
			WriteToLog("GenerateBuffers: SetVertexShader failed. hr=%s\n", m_pD3D->HResultToString(GetLastError()) );
			Fail();
			return false;
		}
	}
	else
	{
		if ( !SetVertexShader(D3DFVF_VERTEX) )
		{
			WriteToLog("GenerateBuffers: SetVertexShader failed. hr=%s\n", m_pD3D->HResultToString(GetLastError()) );
			Fail();
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------

bool Lines::GenerateSpecialBuffer()
{
	TLVERTEX  *grid1   = NULL;
	TLVERTEX  *grid2   = NULL;
	TLVERTEX  *swirl1 = NULL;
	TLVERTEX  *swirl2 = NULL;
	float      x, y, t;
	
	UINT size   = 2 * (GRID_SIZE + SWIRL_SIZE);
	UINT Width  = m_Options.D3DOptions.nBackBufferWidth;
	UINT Height = m_Options.D3DOptions.nBackBufferHeight;

	if (m_pVBTL != NULL)
	{
		delete [] m_pVBTL;
		m_pVBTL = NULL;
	}		
	
	m_pVBTL = new TLVERTEX [size];
	
	grid1 = new TLVERTEX[GRID_SIZE];
	int k = 0;
	int increment = 1;
	t= 0.f;

	//vertical lines
	for (UINT i = 0; i < GRID_SIZE; i+=2)
	{
		if(k == 3)
			increment = -1;
		if(k == -3)
			increment = 1;

		k += increment;
		x = (i/2 + 1) * 3.f;
		grid1[i + 0] = TLVERTEX(D3DXVECTOR4( x,   1.f + k,          0.f, 1.f ), 0xffffffff, t, 0.00f);
		grid1[i + 1] = TLVERTEX(D3DXVECTOR4( x+k, Height/2.f - 1.f, 0.f, 1.f ), 0xffffffff, t, 0.99f);
		t += 1.f / (GRID_SIZE/2.f - 1.f);
	}

	memcpy( m_pVBTL, grid1, GRID_SIZE * sizeof(TLVERTEX));
	
	grid2 = new TLVERTEX[GRID_SIZE];

	k = 0;
	increment = 1;
	t = 0.f;

	//horizontal lines
	for (UINT i = 0; i < GRID_SIZE; i+=2)
	{
		if(k == 3)
			increment = -1;
		if(k == -3)
			increment = 1;

		k += increment;
		y = Height/2 + (i/2 + 1) * 3.f;
		grid2[i + 0] = TLVERTEX(D3DXVECTOR4( Width/2.f + 1.f+ k, y, 0.f, 1.f ), 0xffffffff, 0.00f, t);
		grid2[i + 1] = TLVERTEX(D3DXVECTOR4( Width     - 1.f,  y+k, 0.f, 1.f ), 0xffffffff, 0.99f, t);
		t += 1.f / (GRID_SIZE/2.f - 1.f);
	}

	memcpy( m_pVBTL + GRID_SIZE, grid2, GRID_SIZE * sizeof(TLVERTEX));

	FLOAT alpha = 0.f, dist = 3.f, radius = 3.f, radius_delta = 0.05f;
	
	float j;
	if (ReadFloat("dist", &j))
		dist = j;
	if (ReadFloat("radius", &j))
		radius = j;
	if (ReadFloat("r_delta", &j))
		radius_delta = j;

	swirl1 = new TLVERTEX[SWIRL_SIZE];

	//start point 
	x = Width - Width / 4.f;
	y = Height / 4.f;
	swirl1[ 0 ] = TLVERTEX(D3DXVECTOR4( x, y, 0.f, 1.f ), 0xffffffff, 0.f, 0.f);

	for (UINT i = 1; i < SWIRL_SIZE; i++)
	{
		radius += radius_delta;
		alpha -= 2.f * asin(dist/(2.f*radius));
		x = cos(alpha) * radius + Width - Width / 4.f;
		y = sin(alpha) * radius + Height / 4.f;
		swirl1[ i ] = TLVERTEX(D3DXVECTOR4( x, y, 0.f, 1.f ), 0xffffffff, cos(alpha), sin(alpha));
	}

	memcpy( m_pVBTL + 2*GRID_SIZE, swirl1, SWIRL_SIZE * sizeof(TLVERTEX));

	swirl2 = new TLVERTEX[SWIRL_SIZE];

	alpha = 0.f; 
	dist = 3.f;
	radius = 3.f;
	radius_delta = 0.05f;

	if (ReadFloat("dist", &j))
		dist = j;
	if (ReadFloat("radius", &j))
		radius = j;
	if (ReadFloat("r_delta", &j))
		radius_delta = j;
	
	//start point 
	x = Width / 4.f;
	y = Height - Height / 4.f;
	swirl2[ 0 ] = TLVERTEX(D3DXVECTOR4( x, y, 0.f, 1.f ), 0xffffffff, 0.f, 0.f);

	for (UINT i = 1; i < SWIRL_SIZE; i++)
	{
		radius += radius_delta;
		alpha += 2.f * asin(dist/(2.f*radius));
		x = cos(alpha) * radius + Width / 4.f;
		y = sin(alpha) * radius + Height - Height / 4.f;
		swirl2[ i ] = TLVERTEX(D3DXVECTOR4( x, y, 0.f, 1.f ), 0xffffffff, cos(alpha), sin(alpha));
	}

	memcpy( m_pVBTL + 2*GRID_SIZE + SWIRL_SIZE, swirl2, SWIRL_SIZE * sizeof(TLVERTEX));

	delete [] grid1;
	delete [] grid2;
	delete [] swirl1;
	delete [] swirl2;
	
	if ( !SetVertexShader(FVF_TLVERTEX) )
	{
		WriteToLog("GenerateSpecialBuffer: SetVertexShader failed. hr=%s\n", m_pD3D->HResultToString(GetLastError()) );
		Fail();
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

bool Lines::DrawLines(bool isTL, DWORD dwX, DWORD dwZ, D3DPRIMITIVETYPE primType)
{
	VOID *pVertices;
	UINT uStride;
		
	if ( isTL )
	{
		pVertices = m_pVBTL;
		uStride = GetVertexSize(FVF_TLVERTEX);
	}
	else
	{
		pVertices = m_pVB;
		uStride = GetVertexSize(D3DFVF_VERTEX);

		if (!SetTransform(D3DTS_VIEW, &m_ViewMatrices[(m_uTestNumber-2)%m_nViewMatrices]))
		{
			WriteToLog("SetTransform for D3DTS_VIEW failed. hr=%s\n", m_pD3D->HResultToString(GetLastError()) );			
			Fail();
			return false;
		}
	}
		
	switch (primType)
	{
		case D3DPT_LINELIST:
		{
			if (! DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, dwX*(dwZ-1) + 1, m_nVertices/2,
									     m_pIB, FMT_INDEX16, pVertices, uStride) )
			{
				WriteToLog("DrawPrimitive failed on D3DPT_LINELIST. hr=%s\n", m_pD3D->HResultToString(GetLastError()));
				Fail();
				return false;
			}
			break;
		}
		case D3DPT_LINESTRIP:
		{
			DWORD offset = 0;
			for ( int indX = 0; indX < dwX; indX++ )
			{
				if (! DrawIndexedPrimitiveUP(D3DPT_LINESTRIP, 0, dwX*(dwZ-1) + 1, 2*dwZ-2,
											 m_pIB + offset, FMT_INDEX16, pVertices, uStride) )
				{
					WriteToLog("DrawPrimitive failed on D3DPT_LINESTRIP on indX=%d. hr=%s\n", indX, m_pD3D->HResultToString(GetLastError()));
					Fail();
					return false;
				}
				offset += 2*dwZ - 1;
			}
			for ( int indZ = 1; indZ < dwZ-1; indZ++ )
			{
				if (KeySet("offset"))
				{
					if (! DrawIndexedPrimitiveUP(D3DPT_LINESTRIP, dwX*(indZ-1) + 1, dwX, dwX,
											 m_pIB + offset, FMT_INDEX16, pVertices, uStride) )
					{
						WriteToLog("DrawPrimitive failed on D3DPT_LINESTRIP on indZ=%d. hr=%s\n", indZ, m_pD3D->HResultToString(GetLastError()));
						Fail();
						return false;
					}
					offset += dwX + 1;
				}
				else
				{
					VOID *pTempVertices = NULL;
					if( isTL )
					{
						pTempVertices = m_pVBTL + (dwX*(indZ-1) + 1);
					}
					else
					{
						pTempVertices = m_pVB + (dwX*(indZ-1) + 1);
					}
					for (int ind = 0; ind < dwX + 1; ind++)
					{
						m_pIB[offset+ind] -= (WORD)(dwX*(indZ-1) + 1);
					}
					if (! DrawIndexedPrimitiveUP(D3DPT_LINESTRIP, 0/*dwX*(indZ-1) + 1*/, dwX, dwX,
											 m_pIB + offset, FMT_INDEX16, pTempVertices, uStride) )
					{
						WriteToLog("DrawPrimitive failed on D3DPT_LINESTRIP on indZ=%d. hr=%s\n", indZ, m_pD3D->HResultToString(GetLastError()));
						Fail();
						return false;
					}
					offset += dwX + 1;
					
				}
			}
			break;
		}
		case D3DPT_TRIANGLELIST:
		{
			DWORD offset = 0;
			//triangle fan
			if (! DrawIndexedPrimitiveUP(D3DPT_TRIANGLEFAN, 0, dwX+1, dwX,
										 m_pIB, FMT_INDEX16, pVertices, uStride) )
			{
				WriteToLog("DrawPrimitive failed on D3DPT_TRIANGLEFAN. hr=%s\n", m_pD3D->HResultToString(GetLastError()));
				Fail();
				return false;
			}
			offset += dwX + 2;
			//triangle strips
			for (int indZ = 1; indZ < dwZ - 1; indZ += 2 )
			{
				if (KeySet("offset"))
				{
					if (! DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, dwX*(indZ-1) + 1, dwX*2, 2*dwX,
											 m_pIB + offset, FMT_INDEX16, pVertices, uStride) )
					{
						WriteToLog("DrawPrimitive failed on D3DPT_TRIANGLESTRIP on indZ=%d. hr=%s\n", indZ, m_pD3D->HResultToString(GetLastError()));
						Fail();
						return false;
					}
					offset += 2*dwX + 2;
					
				}
				else
				{
					VOID *pTempVertices = NULL;
					if( isTL )
					{
						pTempVertices = m_pVBTL + (dwX*(indZ-1) + 1);
					}
					else
					{
						pTempVertices = m_pVB + (dwX*(indZ-1) + 1);
					}
					for (int ind = 0; ind < 2*dwX+2; ind++)
					{
						m_pIB[offset+ind] -= (WORD)(dwX*(indZ-1) + 1);
					}
					if (! DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0/*dwX*(indZ-1) + 1*/, dwX*2, 2*dwX,
											 m_pIB + offset, FMT_INDEX16, pTempVertices, uStride) )
					{
						WriteToLog("DrawPrimitive failed on D3DPT_TRIANGLESTRIP on indZ=%d. hr=%s\n", indZ, m_pD3D->HResultToString(GetLastError()));
						Fail();
						return false;
					}
					offset += 2*dwX + 2;
				}
			}
			//triangle lists
			for (int indZ = 2; indZ < dwZ - 1; indZ += 2 )
			{
				if (KeySet("offset"))
				{
					if (! DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, dwX*(indZ-1) + 1, dwX*2, 2*dwX,
												m_pIB + offset, FMT_INDEX16, pVertices, uStride) )
					{
						WriteToLog("DrawPrimitive failed on D3DPT_TRIANGLELIST on indZ=%d. hr=%s\n", indZ, m_pD3D->HResultToString(GetLastError()));
						Fail();
						return false;
					}
					offset += 6*dwX;
				}
				else
				{
					VOID *pTempVertices = NULL;
					if( isTL )
					{
						pTempVertices = m_pVBTL + (dwX*(indZ-1) + 1);
					}
					else
					{
						pTempVertices = m_pVB + (dwX*(indZ-1) + 1);
					}
					for (int ind = 0; ind < 3*2*dwX; ind++)
					{
						m_pIB[offset+ind] -= (WORD)(dwX*(indZ-1) + 1);
					}
					if (! DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0/*dwX*(indZ-1) + 1*/, dwX*2, 2*dwX,
												m_pIB + offset, FMT_INDEX16, pTempVertices, uStride) )
					{
						WriteToLog("DrawPrimitive failed on D3DPT_TRIANGLELIST on indZ=%d. hr=%s\n", indZ, m_pD3D->HResultToString(GetLastError()));
						Fail();
						return false;
					}
					offset += 6*dwX;
				}
			}
			break;
		}
	}

	return true;
}


//-----------------------------------------------------------------------------

bool Lines::DrawSpecialLines()
{
	if ( !DrawPrimitiveUP(D3DPT_LINELIST, GRID_SIZE/2, m_pVBTL, GetVertexSize(FVF_TLVERTEX)) )
	{
		WriteToLog("DrawPrimitiveUP failed on grid0. hr=%s\n", m_pD3D->HResultToString(GetLastError()));
		Fail();
		return false;
	}

	if ( !DrawPrimitiveUP(D3DPT_LINELIST, GRID_SIZE/2, m_pVBTL + GRID_SIZE, GetVertexSize(FVF_TLVERTEX)) )
	{
		WriteToLog("DrawPrimitiveUP failed on grid1. hr=%s\n", m_pD3D->HResultToString(GetLastError()));
		Fail();
		return false;
	}

	if ( !DrawPrimitiveUP(D3DPT_LINESTRIP, SWIRL_SIZE-1, m_pVBTL + 2*GRID_SIZE, GetVertexSize(FVF_TLVERTEX)) )
	{
		WriteToLog("DrawPrimitiveUP failed on swirl0. hr=%s\n", m_pD3D->HResultToString(GetLastError()));
		Fail();
		return false;
	}

	if ( !DrawPrimitiveUP(D3DPT_LINESTRIP, SWIRL_SIZE-1, m_pVBTL + 2*GRID_SIZE + SWIRL_SIZE, GetVertexSize(FVF_TLVERTEX)) )
	{
		WriteToLog("DrawPrimitiveUP failed on swirl1. hr=%s\n", m_pD3D->HResultToString(GetLastError()));
		Fail();
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

void Lines::GetPattern(TCHAR *szPattern)
{
	UINT index;
	WORD wRepeat;
	if (m_uTestNumber == 1)
		index = 0;
	else
		index = (m_uTestNumber-2)/(m_nViewMatrices*m_nPrimTypes);

	if (m_uTestNumber == 1 || !m_bPatRepeat)
		wRepeat = 1;
	else
		wRepeat = (WORD)(1 + (m_uTestNumber-2)/(m_nViewMatrices*m_nPrimTypes*g_nPatterns));

	m_dwPattern = (g_wPatterns[index%g_nPatterns] << 16) | wRepeat;
	_stprintf(szPattern, _T("Pattern = 0x%x, "), m_dwPattern);
	//return SetRenderState((D3DRENDERSTATETYPE)10/*D3DRS_LINEPATTERN*/, m_dwPattern);
}


	