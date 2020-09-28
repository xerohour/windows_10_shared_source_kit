e3D<%s> u%d; "), (m_DataTypeUAV[i]).c_str(), i+1);
		else
		{
			WriteToLog(_T("CMemoryBarrierPSTest::SetupPixelShader(): Wrong UAV resource type."));
			return RESULT_FAIL;
		}
	}

	// Generate UAV write code
	for (UINT i = 0; i < 2; i++)
	{
		if ( m_TypeUAV[i] == RAW_BUFFER)
			UAVWriteStr += FormatString(_T("u%d.Store(pid*4, pid); "), i+1);
		else if ( m_TypeUAV[i] == TEXTURE1D || m_TypeUAV[i] == STRUCTURED_BUFFER 
			     || m_TypeUAV[i] == TYPED_BUFFER )
		{
			UAVWriteStr += FormatString(_T("uint index%d = {pid}; "), i+1);	
			UAVWriteStr += FormatString(_T("u%d[index%d] = pid; "), i+1, i+1);	
		}
		else if ( m_TypeUAV[i] == TEXTURE1DARRAY || m_TypeUAV[i] == TEXTURE2D)
		{
			UAVWriteStr += FormatString(_T("uint2 index%d = {pid, 1}; "), i+1);	
			UAVWriteStr += FormatString(_T("u%d[index%d] = pid;  "), i+1, i+1);	
		}
		else if ( m_TypeUAV[i] == TEXTURE2DARRAY || m_TypeUAV[i] == TEXTURE3D)
		{
			UAVWriteStr += FormatString(_T("uint3 index%d = {pid, 1, 1}; "), i+1);	
			UAVWriteStr += FormatString(_T("u%d[index%d] = pid; "), i+1, i+1);	
		}
		if ( i == 0 )
			UAVWriteStr += _T("DeviceMemoryBarrier(); ");
	}

	// Generate UAV read code
	for (int i = 1; i >= 0; i--)
	{
		if ( m_TypeUAV[i] == RAW_BUFFER)
			UAVReadStr += FormatString(_T("uint sig_%d = u%d.Load(otherid*4);  "), i+1, i+1);
		else if ( m_TypeUAV[i] == TEXTURE1D || m_TypeUAV[i] == STRUCTURED_BUFFER 
			     || m_TypeUAV[i] == TYPED_BUFFER )
		{ 
			UAVReadStr += FormatString(_T("uint indexOther%d = {otherid};  "), i+1);	
			UAVReadStr += FormatString(_T("uint sig_%d = u%d[indexOther%d];  "), i+1, i+1, i+1);	
		}
		else if ( m_TypeUAV[i] == TEXTURE1DARRAY || m_TypeUAV[i] == TEXTURE2D)
		{
			UAVReadStr += FormatString(_T("uint2 indexOther%d = {otherid, 1};  "), i+1);	
			UAVReadStr += FormatString(_T("uint sig_%d = u%d[indexOther%d];  "), i+1, i+1, i+1);
		}
		else if ( m_TypeUAV[i] == TEXTURE2DARRAY || m_TypeUAV[i] == TEXTURE3D)
		{
			UAVReadStr += FormatString(_T("uint3 indexOther%d = {otherid, 1, 1};  "), i+1);	
			UAVReadStr += FormatString(_T("uint sig_%d = u%d[indexOther%d];  "), i+1, i+1, i+1);
		}
		if ( i == 1 )
			UAVReadStr += _T("DeviceMemoryBarrier(); ");
	}

	// to finish the Macro define
	UAVDclsStr += _T("\n");
	UAVWriteStr += _T("\n");
	UAVReadStr += _T("\n");

	D3D10_SHADER_MACRO macroSet[] =
	{
		{ "MACRO_SET", "true" },
		{ "VIEWPORT_WIDTH", ViewWidthStr.c_str() },
		{ "VIEWPORT_HEIGHT", ViewHeightStr.c_str() },
		{ "UAV_DCLS", UAVDclsStr.c_str() },
		{ "UAV_WRITE", UAVWriteStr.c_str() },
		{ "UAV_READ", UAVReadStr.c_str() },
		NULL
	};

	hr = CompileAndCreatePixelShaderFromResource (
		_T("RC_MEMORY_BARRIER_PS_FX"), 
		_T("PSMain"), 
		D3D_FEATURE_LEVEL_11_0, 
		&m_pPixelShader,
		macroSet
		);

	if (FAILED(hr))
	{
		WriteToLog( _T( "CMemoryBarrierPSTest::SetupPixelShader: CompileAndCreatePixelShaderFromResource() failed."));
		tr = RESULT_FAIL;
		goto shader_done;
			
	}

shader_done:
	return tr;
}

//-------------------------------------------------------------------------------------------------------------
#define NUM_FAILED_PIXELS_IN_LOG 4  // Only log first 4 error pixels
TEST_RESULT CMemoryBarrierPSTest::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;

	UINT clearZero[4] = {0, 0, 0, 0};
	for ( UINT i = 0; i < 2; ++i )
	{
		if ( m_ppUAVs[i] )
			GetEffectiveContext()->ClearUnorderedAccessViewUint(m_ppUAVs[i], clearZero);
	}
	UINT clearOne[4] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
	// Clear the result UAV to all 1s
	if ( m_ppUAVs[2] )
			GetEffectiveContext()->ClearUnorderedAccessViewUint(m_ppUAVs[2], clearOne);

	GetEffectiveContext()->Draw( 6, 0 );
	ExecuteEffectiveContext();

	GetImmediateContext()->CopyResource( m_pResultCopyBuffer, m_pResultUAVBuffer );
	
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	hr = GetImmediateContext()->Map( m_pResultCopyBuffer, 0, D3D11_MAP_READ, 0, &mappedRes );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CMemoryBarrierPSTest::ExecuteTestCase() - Map failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	UINT* pUintData = (UINT*)mappedRes.pData;
	UINT numErrMsg = NUM_FAILED_PIXELS_IN_LOG; // Only log first 4 error pixels
	UINT numFailPixels = 0;
	for( UINT i = 0; i < m_ViewWidth * m_ViewHeight; i++)
	{
		if ( pUintData[i] != 0 )
		{
			numFailPixels++;
			if ( numErrMsg > 0 )
			{
				numErrMsg--;
				WriteToLog( _T("CMemoryBarrierPSTest::ExecuteTestCase(): %d faults found in pixel %d."), 
						   pUintData[i], i );
			}
			tr = RESULT_FAIL;
		}
	}
	if ( ( tr == RESULT_FAIL ) && ( numFailPixels > NUM_FAILED_PIXELS_IN_LOG ) )
	{
		WriteToLog( _T("CMemoryBarrierPSTest::ExecuteTestCase(): More failed pixels following.\
					   %d failed pixels in total."), numFailPixels );
	}

		
	GetImmediateContext()->Unmap( m_pResultCopyBuffer, 0 );

	return tr;
}



//-------------------------------------------------------------------------------------------------------------
void CMemoryBarrierPSTest::CleanupTestCase()
{
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pResultUAVBuffer);
	SAFE_RELEASE(m_pResultCopyBuffer);
	SAFE_RELEASE(m_pRTTex2D);
	SAFE_RELEASE(m_pRTView);

	for(UINT i = 0; i < 3; i++)
		SAFE_RELEASE(m_ppUAVs[i]);

	for(UINT i = 0; i < 2; i++)
	{
		SAFE_RELEASE(m_ppUAVBuf[i]);
		SAFE_RELEASE(m_ppUAVTex1D[i]);
		SAFE_RELEASE(m_ppUAVTex2D[i]);
		SAFE_RELEASE(m_ppUAVTex3D[i]);
	}

}

//-------------------------------------------------------------------------------------------------------------
void CMemoryBarrierPSTest::Cleanup()
{
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pVertexBuffer);
}
﻿<?xml version='1.0' encoding='utf-8' standalone='yes'?>
<Content xmlns="http://microsoft.com/schemas/Windows/Kits/Manifest">
  <ContentGroup
      BuildArchitecture="each"
      Destination="$(_BuildArch)"
      >
    <FeatureReference Name="Device.Graphics"/>
    <BinaryFile
        Destination="nttest\windowstest\graphics\d3d\conf"
        Name="Overdraw.exe"
        Source="$(OBJ_PATH)\$(O)"
        />
  </ContentGroup>
</Content>
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


	///////////////////////////////////////////////////////////
// FILE:        TextureStage3.cpp
// DESC:        TextureStageTest 3rd generation.
// AUTHOR:      Bobby Paulose 
//				Bill Kristiansen
// Created:		June 11, 2002.
///////////////////////////////////////////////////////////


#include "TextureStage3.h"

////The test groups to run///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
CD3DWindowFramework TheApp;

CTSSTestEightStage g_TextureStageEfficientEightStage( "8Stages" );
CTSSTestSevenStage g_TextureStageEfficientSevenStage( "7Stages" );
CTSSTestSixStage g_TextureStageEfficientSixStage( "6Stages" );
CTSSTestFiveStage g_TextureStageEfficientFiveStage( "5Stages" );
CTSSTestFourStage g_TextureStageEfficientFourStage( "4Stages" );
CTSSTestThreeStage g_TextureStageEfficientThreeStage( "3Stages" );
CTSSTestTwoStage g_TextureStageEfficientTwoStage( "2Stages" );
CTSSTestOneStage g_TextureStageEfficientOneStage( "1Stages" );


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


CBaseTSSTest::CBaseTSSTest() :
	m_pVB4VrtxDST2( NULL ), 
	m_pVB4VrtxDST2_HalfDiffuse( NULL ), 
	m_pVB4VrtxDST2_ZeroDiffuse( NULL ), 
	m_pVB4VrtxDST2_DiffuseAlphaGradient( NULL ), 
	m_pVB4VrtxDST2T2( NULL ), 
	m_pVB4VrtxDST2T2half( NULL ), 
	m_pVB4VrtxDST2T3T3_forVolCube( NULL ), 
	m_pVB4VrtxDST2T3T3_forMipMap( NULL ),
	bPointFilter( false ),
	m_uTotalTestCycles( 0 ),
	m_dwPrimitiveMiscCaps( 0 ),
	m_dwRasterCaps( 0 ),
	m_dwTextureCaps( 0 ),
	m_dwTextureOpCaps( 0 ),
	m_dwSrcAlphaBlendCaps( 0 ),
	m_dwTextureStageCaps( 0 ),
	m_wMaxSimultaneousTextures( 0 ),
	m_uNumTestCases( 0 ),
	m_uIndexTestCase( 0 ),
	m_uFormatCycle( 0 )
{
	memset( m_IndxTriList, 0, sizeof( m_IndxTriList ) );
	memset( m_IndxTriStrip, 0, sizeof( m_IndxTriStrip ) );
	memset( m_IndxTriFan, 0, sizeof( m_IndxTriFan ) );
	memset( m_pStageFormat, 0, sizeof( m_pStageFormat ) );
    memset( m_pTextures, 0, sizeof( m_pTextures ) );
    
    SetTestType(TESTTYPE_CONF);
 
	// Set framework options
	m_Options.D3DOptions.fMinDXVersion = 8.0f;
    m_pFramework->GetImageCmp()->SetMinPixels(0);

	m_Options.D3DOptions.nBackBufferWidth = DISPLAY_WIDTH;
	m_Options.D3DOptions.nBackBufferHeight = DISPLAY_HEIGHT;

	bPointFilter = false;
    
    return;
}

CBaseTSSTest::~CBaseTSSTest()
{
}

UINT CBaseTSSTest::GetTestCaseIndexToRun(UINT uiTest, UINT *puFormatCycle)
{
	UINT uTotalTestCycles=0;

	for (UINT i=0; i<m_uNumTestCases; i++)
	{
		uTotalTestCycles = uTotalTestCycles + GetTestCaseNumCycles( i );

		if (uiTest <= uTotalTestCycles)
		{
			//we've found the test_case to run
			if (puFormatCycle) *puFormatCycle = uiTest - (uTotalTestCycles - GetTestCaseNumCycles( i ));
			return i;
		}
	}

	//control shud not reach here!
	if (puFormatCycle) *puFormatCycle = 0;
	return 0;
}

bool CBaseTSSTest::ExecuteTest( UINT uiTest )
{
	TCHAR sztBuffer[BUFFER_LENGTH];	

	//Destroy the textures created for the last test. The last test index
	// is now in m_uIndexTestCase.
	for( int i=0; i < GetNumStages(); i++ )
	{
		if( m_pTextures[i] )
		{
			//release this texture.
			RELEASE( m_pTextures[i] );
		}
	}

	m_uIndexTestCase = GetTestCaseIndexToRun( uiTest, &m_uFormatCycle );

	sprintf( sztBuffer, "<<%s>> [%dTexStages]", GetTestCaseDescription(m_uIndexTestCase), GetNumStages() );
	BeginTestCase( sztBuffer, uiTest );

	//we don't want to cull out any triangle drawn.
	SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);

	if( !ExecuteTSStestCase( m_uIndexTestCase, m_uFormatCycle ) )
	{
		WriteToLog( _T( "\n ExecuteTSStestCase failed for test-case %d, format cycle %d" ), m_uIndexTestCase, m_uFormatCycle );
		return false;
	}

	return true;
}
	
void CBaseTSSTest::SceneRefresh()
{
	if( !CBaseTSSTest::DrawPrimitiveID( GetTestCasePrimID( m_uIndexTestCase ) ) )
	{
		WriteToLog( "\n(ERROR) Failed DrawPrimitiveID. (%08X)", GetLastError() );
	}
}

bool CBaseTSSTest::ExecuteTSStestCase( UINT uTestCaseIndex, UINT uFormatCycle )
{
	//Create textures required for this test case.
	if( !CreateTexturesForThisTestCase( uTestCaseIndex, uFormatCycle ) )
	{
		WriteToLog(_T("\nCreateTextures failed for test-case %d, format cycle %d"), m_uIndexTestCase,m_uFormatCycle);
		return false;
	}

	//Set RenderStates
	int i;
	for(i = 0; i < NUM_RENDER_STATES; i++ )
	{
		D3DRENDERSTATETYPE rs;
		DWORD value;
		
		GetTestCaseRenderStateValue( uTestCaseIndex, i, rs, value );
		
		if( rs != 0 )
		{
			SetRenderState( rs, value );
		}
	}
	
	//Adapt to RenderState options and caps.
	if( !( m_dwSrcAlphaBlendCaps ) )
	{
		SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD)FALSE );
	}


	//Set TextureStageStates and Textures.
	for( i = 0; i < GetNumStages(); i++)
	{
		//Set TextureOp and Args
		TextureStageState const &stage = GetTestCaseStage( uTestCaseIndex, i );
			
		SetTextureStageState( i, D3DTSS_COLOROP, stage.ColorOp );
		SetTextureStageState( i, D3DTSS_COLORARG1, stage.ColorArg1 );
		SetTextureStageState( i, D3DTSS_COLORARG2, stage.ColorArg2 );
		SetTextureStageState( i, D3DTSS_COLORARG0, stage.ColorArg0 );
								
		SetTextureStageState( i, D3DTSS_ALPHAOP, stage.AlphaOp );
		SetTextureStageState( i, D3DTSS_ALPHAARG1, stage.AlphaArg1 );
		SetTextureStageState( i, D3DTSS_ALPHAARG2, stage.AlphaArg2 );
		SetTextureStageState( i, D3DTSS_ALPHAARG0, stage.AlphaArg0 );

		// BUGBUG - Wrapping is enabled by default in the DirectX runtime. However,
		// with the addition of the new USAGE_QUERY_WRAPANDMIP query some (Nvidia)
		// parts are failing the bump map cases which can push texture coordinates
		// outside the range of [0,1].  I have implemented a hack to handle this special
		// case using the below variables.
		if (m_pD3D->GetRuntimeVersion() >= 9.029f)
		{
			if( i > 0 && m_pStageFormat[i] )
			{
				TextureStageState const &prevstage = GetTestCaseStage( uTestCaseIndex, i - 1 );
				
				if( prevstage.ColorOp == D3DTOP_BUMPENVMAP || prevstage.ColorOp == D3DTOP_BUMPENVMAPLUMINANCE )
				{
					HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
														USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, *m_pStageFormat[i]);
					if (FAILED(Result))
					{
						SetTextureStageState( i, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
						SetTextureStageState( i, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
					}
				}
			}
		}
		else
		{
			SetTextureStageState( i, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
			SetTextureStageState( i, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
		}
	
	

		//Redirect output of this stage to TEMP or CURRENT as specified.
		SetTextureStageState( i, D3DTSS_RESULTARG, stage.ResultTo );


		//Set texture coordinate index for this stage.
		SetTextureStageState( i, D3DTSS_TEXCOORDINDEX, stage.TexCoordIndex );


		// If supported, reset MipMapLODBias back to default state
		if( m_dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS )
		{
			SetTextureStageState( i, D3DTSS_MIPMAPLODBIAS, (DWORD) 0 );
		}

		//Set other TextureStageStates
		for( int j = 0; j < NUM_TEXSTAGE_STATES; j++ )
		{
			D3DTEXTURESTAGESTATETYPE tss = GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].TSS;
			
			if( tss != 0 )
			{
				if( ( D3DTSS_BUMPENVMAT00 == tss ) ||
					( D3DTSS_BUMPENVMAT11 == tss ) ||
					( D3DTSS_BUMPENVMAT10 == tss ) ||
					( D3DTSS_BUMPENVMAT01 == tss ) ||
					( D3DTSS_BUMPENVLSCALE == tss ) ||
					( D3DTSS_BUMPENVLOFFSET == tss ) ||
					( D3DTSS_MIPMAPLODBIAS == tss) )
				{
					//we need to send a float value disguised as a DWORD.
					float fval = (float)( GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value)/100.0f;
					
					if( (GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value) >> ( sizeof(DWORD)*8 - 1 ) )
					{
						//Negative value. Take two's complement.
						fval = -( (float)((~(GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value) ) + 1) )/100.0f;
					}
					
					SetTextureStageState( i, tss, *((DWORD*)&(fval) ) );
				}
				else
				{
					// check if we need to force point sampling
					if (((tss == D3DTSS_MIPFILTER) ||
						(tss == D3DTSS_MAGFILTER) ||
						(tss == D3DTSS_MINFILTER)) &&
						(GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value == D3DTEXF_LINEAR))
					{
						SetTextureStageState(i, tss, (bPointFilter) ? D3DTEXF_POINT : GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value);
					}
					else
					{
						SetTextureStageState(i, tss, GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value);
					}
				}
			}
		}


		//Set Texture for this stage
		TextureID TexID = GetTestCaseStage( uTestCaseIndex, i ).TexID;
/*		if( GetTestCaseStage( uTestCaseIndex, i ).ColorOp == D3DTOP_DISABLE )
		{
			// Leave garbage in texture stage
		}
		else
*/		if( TexID != TEX_NULL && 
			TexID != TEX_PREVIOUS_STAGE && 
			TexID != TEX_ONE_STAGE_ABOVE && 
			TexID != TEX_TWO_STAGE_ABOVE && 
			TexID != TEX_THREE_STAGE_ABOVE && 
			TexID != TEX_FOUR_STAGE_ABOVE && 
			TexID != TEX_FIVE_STAGE_ABOVE && 
			TexID != TEX_SIX_STAGE_ABOVE && 
			TexID != TEX_SEVEN_STAGE_ABOVE && 
			GetTestCaseStage( uTestCaseIndex, i ).CycID != CYC_DONT_CARE 
			)
		{
			SetTexture(i, m_pTextures[i]);
		}
		else if( TexID == TEX_PREVIOUS_STAGE || 
					TexID == TEX_ONE_STAGE_ABOVE || 
					TexID == TEX_TWO_STAGE_ABOVE || 
					TexID == TEX_THREE_STAGE_ABOVE || 
					TexID == TEX_FOUR_STAGE_ABOVE || 
					TexID == TEX_FIVE_STAGE_ABOVE || 
					TexID == TEX_SIX_STAGE_ABOVE || 
					TexID == TEX_SEVEN_STAGE_ABOVE
				)
		{
			//We must find the texture for this stage from a previous stage.
			if( TexID == TEX_PREVIOUS_STAGE )
			{
				//We must find the texture for this stage from the nearest previous stage which has a texture.
				SetTexture( i, NULL);
				for( int index = i - 1; index >= 0; index-- )
				{
					if( m_pTextures[index] != NULL )
					{
						SetTexture( i, m_pTextures[index] );
						break;
					}
				}
			}
			else
			{
				//We must find the texture for this stage from the specified previous stage.
				SetTexture(i, NULL);
				int index = i - ( TexID - TEX_ONE_STAGE_ABOVE + 1 );
				if( index >= 0 )
				{
					SetTexture( i, m_pTextures[index] );
				}
			}
		}
		else 
		{
			SetTexture( i, NULL );
		}
	}

	//Disable the rest of the TextureStages in the hardware and refrast.
	for( i = GetNumStages(); i < MAX_TEX_STAGES; i++ )
	{
		//Set TextureOp to DISABLE
		SetTextureStageState( i, D3DTSS_COLOROP, D3DTOP_DISABLE );
		SetTextureStageState( i, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

		//Set Texture for this stage to NULL
		SetTexture( i, NULL );
	}

	// Make sure that all is fine before rendering
	DWORD dwPasses = 0;
	if( !ValidateDevice( &dwPasses ) )
	{
		// Issue a skip for this case but continue rendering (becomes a driver stability case)
		WriteToLog("ValidateDevice() failed (HResult = %s), skipping case.\n",m_pD3D->HResultToString(GetLastError()));
		Skip();
	}

	return true;
}
	
bool CBaseTSSTest::CreateTexturesForThisTestCase( UINT uTestCaseIndex, UINT uFormatCycle )
{
	m_pStageFormat[0] =
	m_pStageFormat[1] =
	m_pStageFormat[2] =
	m_pStageFormat[3] =
	m_pStageFormat[4] =
	m_pStageFormat[5] =
	m_pStageFormat[6] =
	m_pStageFormat[7] = NULL;
	
	UINT uQuotient=uFormatCycle,uFmtListIndex=0;

	int i;
	for (i=0; i<GetNumStages(); i++)
	{
		TextureID TexID = GetTestCaseStage( uTestCaseIndex, i ).TexID;
		CycleID CycID = GetTestCaseStage( uTestCaseIndex, i ).CycID;
		
		if( TexID != TEX_NULL && 
			TexID != TEX_PREVIOUS_STAGE && 
			TexID != TEX_ONE_STAGE_ABOVE && 
			TexID != TEX_TWO_STAGE_ABOVE && 
			TexID != TEX_THREE_STAGE_ABOVE && 
			TexID != TEX_FOUR_STAGE_ABOVE && 
			TexID != TEX_FIVE_STAGE_ABOVE && 
			TexID != TEX_SIX_STAGE_ABOVE && 
			TexID != TEX_SEVEN_STAGE_ABOVE && 
			CycID != CYC_DONT_CARE
			)
		{
			//We need to Create Texture for this stage.
			//Let's find out what format the texture need to be.
			if( CycID != CYC_PREVIOUS_STAGE_DEPENDENT &&
				CycID != CYC_ONE_STAGE_ABOVE_DEPENDENT &&
				CycID != CYC_TWO_STAGE_ABOVE_DEPENDENT &&
				CycID != CYC_THREE_STAGE_ABOVE_DEPENDENT &&
				CycID != CYC_FOUR_STAGE_ABOVE_DEPENDENT &&
				CycID != CYC_FIVE_STAGE_ABOVE_DEPENDENT &&
				CycID != CYC_SIX_STAGE_ABOVE_DEPENDENT &&
				CycID != CYC_SEVEN_STAGE_ABOVE_DEPENDENT
				)
			{
				//We need to compute and find the format to create the texture.
				uFmtListIndex = uQuotient % m_VariousTexFormatCycles[CycID].uNumFormats;
				uQuotient = uQuotient / m_VariousTexFormatCycles[CycID].uNumFormats;

				m_pStageFormat[i] = &(m_VariousTexFormatCycles[CycID].pFormats[uFmtListIndex]);
			}
			else
			{
				//We can find the format to create the texture from a previous stage.
				if (CycID == CYC_PREVIOUS_STAGE_DEPENDENT)
				{
					for (int index=i-1; index>=0; index--)
					{
						if (m_pStageFormat[index] != NULL)
						{
							m_pStageFormat[i] = m_pStageFormat[index];
							break;
						}
					}
				}
				else
				{
					int index = i - (CycID - CYC_ONE_STAGE_ABOVE_DEPENDENT + 1);
					if (index >= 0)
					{
						m_pStageFormat[i] = m_pStageFormat[index];
					}
				}
			}


			//We've found the format. Now Create the texture.
			if (NULL != m_pStageFormat[i])
			{
				//create the texture for this stage.
				if ( (TexID != TEX_MIPS) && (TexID != TEX_VOLUME) && (TexID != TEX_CUBE) )
				{
					//create the texture for this stage.
					RELEASE(m_pTextures[i]);

					if (TexID < TEX_LAST_PLACEHOLDER_CIMAGE)
					{
						// Need to check the Filter Query for Dx9
						if (m_dwVersion >= 0x0900)
						{
							// Assume we can filter the format
							bPointFilter = false;

							if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
																USAGE_QUERY_FILTER, RTYPE_TEXTURE, *m_pStageFormat[i])))
							{
								// Unable to filter this format, use point sampling
								for (int j=0; j<NUM_TEXSTAGE_STATES; j++)
								{
									D3DTEXTURESTAGESTATETYPE tss = GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].TSS;

									if (((tss == D3DTSS_MIPFILTER) ||
										(tss == D3DTSS_MAGFILTER) ||
										(tss == D3DTSS_MINFILTER)) &&
										(GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value == D3DTEXF_LINEAR))
									{
										bPointFilter = true;
									}
								}
							}
						}

						// Tell log about our texture
						TCHAR szBuffer[80], msgString[80];
						msgString[0] = '\0';

						FmtToString(FORMAT_TEXTURE,m_pStageFormat[i],szBuffer);
						sprintf(msgString, "TexFmt: %s",szBuffer);

						WriteToLog(_T("CreateTexture(), %s\n"),msgString);

						// Create Texture and Load Texture from CImage.
						if (!CreateTexture(
							m_TexImages[TexID].GetWidth(), 
							m_TexImages[TexID].GetHeight(), 
							1, (DWORD) 0, *m_pStageFormat[i], FMWK_DEFAULT, (CnTexture**)&(m_pTextures[i])
							))
						{
							WriteToLog(_T("\n(ERROR) Failed to create texture.TexID:%d"), TexID);
							return false;
						}

						//Load Texture from CImage.
						if (!(((CnTexture*)m_pTextures[i])->LoadImage(1, &m_TexImages[TexID])))
						{
							WriteToLog(_T("\nUnable to load texture from CImage ID:%d"),TexID);
							return false;
						}

						//Set Palette if required.
						if (((CnTexture*)m_pTextures[i])->IsPalettized())
						{
							if (!(((CnTexture*)m_pTextures[i])->SetPalette(NULL)))
							{
								WriteToLog("SetPalette() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
								Fail();
								return false;
							}
						}
					}
					else
					{
					/* This ability to load texture from a file is disabled for now because it must be supported in lower DX versions too.
					//Create Texture from file.
						if (!D3DXCreateTextureFromFileEx(
							g_TexFileNames[TexID - TEX_LAST_PLACEHOLDER_CIMAGE],
							DEFAULT_TEXWIDTH,
							DEFAULT_TEXHEIGHT,
							1, (DWORD) 0, *m_pStageFormat[i], FMWK_DEFAULT,
							0,0,0,NULL,NULL,
							&(m_pTextures[i])
							))

						{
							WriteToLog(_T("\nUnable to Create texture from file :%s"),g_TexFileNames[TexID - TEX_LAST_PLACEHOLDER_CIMAGE]);
							return false;
						}
					*/
					WriteToLog(_T("\nUnable to Create texture from file."));
					}



				}
				else //create mips, volume, cube texture
				{
					//create the texture for this stage.
					RELEASE(m_pTextures[i]);

					if (TexID == TEX_MIPS)
					{
						if (TexID < TEX_LAST_PLACEHOLDER_CIMAGE)
						{
							// Tell log about our texture
							TCHAR szBuffer[80], msgString[80];
							msgString[0] = '\0';

							FmtToString(FORMAT_TEXTURE,m_pStageFormat[i],szBuffer);
							sprintf(msgString, "TexFmt: %s",szBuffer);

							WriteToLog(_T("CreateTexture(), %s\n"),msgString);

							// On Dx9.L, need to check for the special A1 format
							if ((m_pD3D->GetVersion() >= 9.1f) && (m_pStageFormat[i]->d3dfFormat == FMT_A1))
							{
								WriteToLog("Unable to mipmap this format. D3DFMT_A1 does not support mipmapping.\n");
								Skip();
								return true;
							}

							// On Dx9c, need to check for UsageQueryWrapAndMip
							if (m_pD3D->GetRuntimeVersion() >= 9.029f)
							{
								HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
																	USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, *m_pStageFormat[i]);
								if (FAILED(Result))
								{
									WriteToLog("Unable to mipmap this format. CheckDeviceFormat(WrapAndMip) failed: %s.\n",m_pD3D->HResultToString(Result));
									Skip();
									return true;
								}
							}

							// Need to check the Filter Query for Dx9
							if (m_dwVersion >= 0x0900)
							{
								// Assume we can filter the format
								bPointFilter = false;

								if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
																	USAGE_QUERY_FILTER, RTYPE_TEXTURE, *m_pStageFormat[i])))
								{
									// Unable to filter this format, use point sampling
									for (int j=0; j<NUM_TEXSTAGE_STATES; j++)
									{
										D3DTEXTURESTAGESTATETYPE tss = GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].TSS;
										
										if (((tss == D3DTSS_MIPFILTER) ||
											(tss == D3DTSS_MAGFILTER) ||
											(tss == D3DTSS_MINFILTER)) &&
											(GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value == D3DTEXF_LINEAR))
										{
											bPointFilter = true;
										}
									}
								}
							}

							// Create Texture and Load Texture from CImage.
							if (!CreateTexture(
								0x4 << MIP_LEVELS, 
								0x4 << MIP_LEVELS, 
								MIP_LEVELS, FMWK_DEFAULT, *m_pStageFormat[i], FMWK_DEFAULT, (CnTexture**)&(m_pTextures[i])
								))
							{
								WriteToLog(_T("\n(ERROR) Failed to create texture.TexID:%d"),TexID);
								return false;
							}

							//Load Texture from CImage.
							CImage *p_MipImages[MIP_LEVELS];
							for (int miplevel=0; miplevel<MIP_LEVELS; miplevel++)
							{
								p_MipImages[miplevel] = &m_TexImages[TexID + miplevel];
							}
							if (!(((CnTexture*)m_pTextures[i])->LoadImage(0, p_MipImages)))
							{
								WriteToLog(_T("\nUnable to load Mips texture :%d"),TexID);
								return false;
							}

							//Set Palette if required.
							if (((CnTexture*)m_pTextures[i])->IsPalettized())
							{
								if (!(((CnTexture*)m_pTextures[i])->SetPalette(NULL)))
								{
									WriteToLog("SetPalette() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
									Fail();
									return false;
								}
							}
						}
						else
						{
							// The ability to load texture from a file is disabled for now because it must be supported in lower DX versions too.
							WriteToLog(_T("\nUnable to Create texture from file."));
						}
					} // end TEX_MIPS
					else if (TexID == TEX_VOLUME)
					{
						if (TexID < TEX_LAST_PLACEHOLDER_CIMAGE)
						{
							// Need to check the Filter Query for Dx9
							if (m_dwVersion >= 0x0900)
							{
								// Assume we can filter the format
								bPointFilter = false;

								if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
																	USAGE_QUERY_FILTER, RTYPE_VOLUMETEXTURE, *m_pStageFormat[i])))
								{
									// Unable to filter this format, use point sampling
									for (int j=0; j<NUM_TEXSTAGE_STATES; j++)
									{
										D3DTEXTURESTAGESTATETYPE tss = GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].TSS;
										
										if (((tss == D3DTSS_MIPFILTER) ||
											(tss == D3DTSS_MAGFILTER) ||
											(tss == D3DTSS_MINFILTER)) &&
											(GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value == D3DTEXF_LINEAR))
										{
											bPointFilter = true;
										}
									}
								}
							}

							// Tell log about our volume texture
							TCHAR szBuffer[80], msgString[80];
							msgString[0] = '\0';

							FmtToString(FORMAT_TEXTURE,m_pStageFormat[i],szBuffer);
							sprintf(msgString, "VolFmt: %s",szBuffer);

							WriteToLog(_T("CreateVolumeTexture(), %s\n"),msgString);

							// Create Volume Texture and Load Volume Texture from CImage.
							if (!CreateVolumeTexture(
								m_TexImages[TexID].GetWidth(), 
								m_TexImages[TexID].GetHeight(), 
								VOLUME_DEPTH, VOLUME_MIP_LEVELS, (DWORD) 0, *m_pStageFormat[i], FMWK_DEFAULT, (CnVolTexture**)&(m_pTextures[i])
								))
							{
								WriteToLog(_T("\n(ERROR) Failed to create texture.TexID:%d"),TexID);
								return false;
							}

							//Load Texture from CImage.
							CImage *p_VolumeImages[VOLUME_MIP_LEVELS*6];
							//Note: Not sure which "for" must come first.
							//Since we won't be using volume mip map for now, its ok.
							for(int vol_level=0; vol_level< VOLUME_DEPTH; vol_level++)
							for (int miplevel=0; miplevel<VOLUME_MIP_LEVELS; miplevel++)
							{
								p_VolumeImages[vol_level*VOLUME_MIP_LEVELS + miplevel] = &m_TexImages[TexID + (vol_level*VOLUME_MIP_LEVELS) + miplevel];
							}
							if (!(((CnVolTexture*)m_pTextures[i])->LoadImage(0, p_VolumeImages)))
							{
								WriteToLog(_T("\nUnable to load Volume texture :%d"),TexID);
								return false;
							}

							//Set Palette if required.
							if (((CnVolTexture*)m_pTextures[i])->IsPalettized())
							{
								if (!(((CnVolTexture*)m_pTextures[i])->SetPalette(NULL)))
								{
									WriteToLog("SetPalette() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
									Fail();
									return false;
								}
							}
						}
						else
						{
							// The ability to load texture from a file is disabled for now because it must be supported in lower DX versions too.
							WriteToLog(_T("\nUnable to Create texture from file."));
						}
					} // end TEX_VOLUME
					else if (TexID == TEX_CUBE)
					{
						if (TexID < TEX_LAST_PLACEHOLDER_CIMAGE)
						{
							// Need to check the Filter Query for Dx9
							if (m_dwVersion >= 0x0900)
							{
								// Assume we can filter the format
								bPointFilter = false;

								if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
																	USAGE_QUERY_FILTER, RTYPE_CUBETEXTURE, *m_pStageFormat[i])))
								{
									// Unable to filter this format, use point sampling
									for (int j=0; j<NUM_TEXSTAGE_STATES; j++)
									{
										D3DTEXTURESTAGESTATETYPE tss = GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].TSS;
										
										if (((tss == D3DTSS_MIPFILTER) ||
											(tss == D3DTSS_MAGFILTER) ||
											(tss == D3DTSS_MINFILTER)) &&
											(GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value == D3DTEXF_LINEAR))
										{
											bPointFilter = true;
										}
									}
								}
							}

							// Tell log about our cube texture
							TCHAR szBuffer[80], msgString[80];
							msgString[0] = '\0';

							FmtToString(FORMAT_TEXTURE,m_pStageFormat[i],szBuffer);
							sprintf(msgString, "CubeFmt: %s",szBuffer);

							WriteToLog(_T("CreateCubeTexture(), %s\n"),msgString);

							// Create Cube Texture and Load Cube Texture from CImage.
							if (!CreateCubeTexture(
								m_TexImages[TexID].GetWidth(), 
								CUBE_MIP_LEVELS, (DWORD) 0, *m_pStageFormat[i], FMWK_DEFAULT, (CnCubeTexture**)&(m_pTextures[i])
								))
							{
								WriteToLog(_T("\n(ERROR) Failed to create texture.TexID:%d"),TexID);
								return false;
							}

							//Load Texture from CImage.
							CImage *p_CubeImages[CUBE_MIP_LEVELS*6];
							for(int face=0; face< 6; face++)
							for (int miplevel=0; miplevel<CUBE_MIP_LEVELS; miplevel++)
							{
								p_CubeImages[face*CUBE_MIP_LEVELS + miplevel] = &m_TexImages[TexID + (face*CUBE_MIP_LEVELS) + miplevel];
							}
							if (!(((CnCubeTexture*)m_pTextures[i])->LoadImage(0, p_CubeImages)))
							{
								WriteToLog(_T("\nUnable to load Cube texture :%d"),TexID);
								return false;
							}

							//Set Palette if required.
							if (((CnCubeTexture*)m_pTextures[i])->IsPalettized())
							{
								if (!(((CnCubeTexture*)m_pTextures[i])->SetPalette(NULL)))
								{
									WriteToLog("SetPalette() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
									Fail();
									return false;
								}
							}
						}
						else
						{
							// The ability to load texture from a file is disabled for now because it must be supported in lower DX versions too.
							WriteToLog(_T("\nUnable to Create texture from file."));
						}
					} // end TEX_CUBE

				} //end - create mips, volume, cube texture

			}
			else
			{
				//we should not reach here! TestCase Description error.
				WriteToLog(_T("\nCannot find texture creation format for TexStage %d. Possible TestCase Description error."), i);
			}
		}
		//else if (TexID != TEX_NULL)
		//{
			//We can use the same texture used in a previous stage.
			//No need to Create texture.
		//}
	} //for loop end.

	return true;
}


void CBaseTSSTest::CommandLineHelp(void)
{
    WriteCommandLineHelp("$y-#nStages\t$wRun test-cases for testing exactly #n texture-stages.");

    WriteCommandLineHelp("$y-NoAlpha\t$wNo alpha blending. Alpha computed not used");
    WriteCommandLineHelp("$y-NoCubeMap\t$wRemove all tests using CubeMap texture.");
    WriteCommandLineHelp("$y-NoVolumeMap\t$wRemove all tests using VolumeMap texture.");
    WriteCommandLineHelp("$y-NoMipMap\t$wRemove all tests using MipMap texture.");
    WriteCommandLineHelp("$y-MaxSimultTextures:#n\t$wLimit all tests to use a max of #n textures\n in different texture stages simultaneously.");

    WriteCommandLineHelp("$y-TexOpRemove:D3DTOP_X,D3DTOP_Y,...\t$wRemove all test-cases using\n texop D3DTOP_X and D3DTOP_Y and....");
    WriteCommandLineHelp("$y-TexArgRemove:D3DTA_X,D3DTA_Y,...\t$wRemove all test-cases using\n texarg D3DTA_X and D3DTA_Y and....");
}

/*
TCHAR *g_TexFileNames[TEX_LAST_PLACEHOLDER - TEX_LAST_PLACEHOLDER_CIMAGE] =
{
	"nonExistentFile.tex",
	"test.bmp",
};
*/


TCHAR *g_TexOpNames[] =
{
    // Control
	_T(""),		//This makes the names offset by their values.
    _T("D3DTOP_DISABLE"),              // = 1,      // disables stage
    _T("D3DTOP_SELECTARG1"),           // = 2,      // the default
    _T("D3DTOP_SELECTARG2"),           // = 3,

    // Modulate
    _T("D3DTOP_MODULATE"),             // = 4,      // multiply args together
    _T("D3DTOP_MODULATE2X"),           // = 5,      // multiply and  1 bit
    _T("D3DTOP_MODULATE4X"),           // = 6,      // multiply and  2 bits

    // Add
    _T("D3DTOP_ADD"),                  // =  7,   // add arguments together
    _T("D3DTOP_ADDSIGNED"),            // =  8,   // add with -0.5 bias
    _T("D3DTOP_ADDSIGNED2X"),          // =  9,   // as above but left  1 bit
    _T("D3DTOP_SUBTRACT"),             // = 10,   // Arg1 - Arg2, with no saturation
    _T("D3DTOP_ADDSMOOTH"),            // = 11,   // add 2 args, subtract product
                                        // Arg1 + Arg2 - Arg1*Arg2
                                        // = Arg1 + (1-Arg1)*Arg2

    // Linear alpha blend: Arg1*(Alpha) + Arg2*(1-Alpha)
    _T("D3DTOP_BLENDDIFFUSEALPHA"),    // = 12, // iterated alpha
    _T("D3DTOP_BLENDTEXTUREALPHA"),    // = 13, // texture alpha
    _T("D3DTOP_BLENDFACTORALPHA"),     // = 14, // alpha from D3DRS_TEXTUREFACTOR

    // Linear alpha blend with pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)
    _T("D3DTOP_BLENDTEXTUREALPHAPM"),  // = 15, // texture alpha
    _T("D3DTOP_BLENDCURRENTALPHA"),    // = 16, // by alpha of current color

    // Specular mapping
    _T("D3DTOP_PREMODULATE"),            // = 17,     // modulate with next texture before use
    _T("D3DTOP_MODULATEALPHA_ADDCOLOR"), // = 18,     // Arg1.RGB + Arg1.A*Arg2.RGB
                                            // COLOROP only
    _T("D3DTOP_MODULATECOLOR_ADDALPHA"), // = 19,     // Arg1.RGB*Arg2.RGB + Arg1.A
                                            // COLOROP only
    _T("D3DTOP_MODULATEINVALPHA_ADDCOLOR"), // = 20,  // (1-Arg1.A)*Arg2.RGB + Arg1.RGB
                                            // COLOROP only
    _T("D3DTOP_MODULATEINVCOLOR_ADDALPHA"), // = 21,  // (1-Arg1.RGB)*Arg2.RGB + Arg1.A
                                            // COLOROP only

    // Bump mapping
    _T("D3DTOP_BUMPENVMAP"),           // = 22, // per pixel env map perturbation
    _T("D3DTOP_BUMPENVMAPLUMINANCE"),  // = 23, // with luminance channel

    // This can do either diffuse or specular bump mapping with correct input.
    // Performs the function (Arg1.R*Arg2.R + Arg1.G*Arg2.G + Arg1.B*Arg2.B)
    // where each component has been scaled and offset to make it signed.
    // The result is replicated into all four (including alpha) channels.
    // This is a valid COLOROP only.
    _T("D3DTOP_DOTPRODUCT3"),          // = 24,

    // Triadic ops
    _T("D3DTOP_MULTIPLYADD"),          // = 25, // Arg0 + Arg1*Arg2
    _T("D3DTOP_LERP"),                 // = 26, // (Arg0)*Arg1 + (1-Arg0)*Arg2

    _T("D3DTOP_FORCE_DWORD"), // = 0x7fffffff,
};




bool CBaseTSSTest::ReadAndFilterCapsBasedOnOptions()
{
	//read hardware caps.
	m_dwPrimitiveMiscCaps = m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps;
	m_dwRasterCaps = m_pSrcDevice->GetCaps()->dwRasterCaps;
	m_dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;
	m_dwTextureOpCaps = m_pSrcDevice->GetCaps()->dwTextureOpCaps;
	m_dwSrcAlphaBlendCaps = m_pSrcDevice->GetCaps()->dwSrcBlendCaps;
	m_wMaxSimultaneousTextures = m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures;

	//modify caps based on options specified for test run.

	//option NoAlpha
	if (KeySet(_T("NoAlpha")))
    {
		//clear alpha blending caps.
        m_dwSrcAlphaBlendCaps = (DWORD)0;
    }

	//option NoCubeMap
	if (KeySet(_T("NoCubeMap")))
    {
		//clear cube map caps.
        m_dwTextureCaps = m_dwTextureCaps & (~((DWORD)D3DPTEXTURECAPS_CUBEMAP));
    }

	//option NoVolumeMap
	if (KeySet(_T("NoVolumeMap")))
    {
		//clear volume map caps.
        m_dwTextureCaps = m_dwTextureCaps & (~((DWORD)D3DPTEXTURECAPS_VOLUMEMAP));
    }

	//option NoMipMap
	if (KeySet(_T("NoMipMap")))
    {
		//clear mip map caps.
        m_dwTextureCaps = m_dwTextureCaps & (~((DWORD)D3DPTEXTURECAPS_MIPMAP));
    }


	//option MaxSimultTextures
	int nMaxSimultTextures = MAX_TEX_STAGES;
	if (ReadInteger(_T("MaxSimultTextures"), &nMaxSimultTextures))
	{
		if (nMaxSimultTextures < 0)
		{
			nMaxSimultTextures = 0;
		}
		if (nMaxSimultTextures < m_wMaxSimultaneousTextures)
		{
			m_wMaxSimultaneousTextures = (WORD)nMaxSimultTextures;
		}
	}


	TCHAR sztOptionBuffer[OPTION_BUFFER_LENGTH];
	TCHAR *pSeparator = _T("|,;");
	//note: blank cannot be a texop separator since it is a key separator.
	TCHAR *pStart, *pEnd;
	UINT len=0,option_len=0;

	//option TexOpRemove
	if (ReadString(_T("TexOpRemove"), sztOptionBuffer, ARRAYSIZE(sztOptionBuffer)))
	{
		//force null terminate if option specified is too long.
		sztOptionBuffer[OPTION_BUFFER_LENGTH-1] = (TCHAR)0;
		pStart = pEnd = sztOptionBuffer;
		option_len = _tcslen(sztOptionBuffer);
		while (pStart < (sztOptionBuffer + option_len))
		{
			if ((len = _tcscspn(pStart, pSeparator)) > 0)
			{
				//we've found a tex_op.
				pEnd = pStart + len;
				*pEnd = (TCHAR)0;

				//search the tex_op found in the list and update caps.
				for (int i=0; i<D3DTOP_LAST_PLACEHOLDER; i++)
				{
					if (_tcscmp(g_TexOpNames[i], pStart) == 0)
					{
						//we've found the tex_op in the list.
						//Now update caps.
						m_dwTextureOpCaps = ( m_dwTextureOpCaps & (~((DWORD)(1 << (i - 1) ))) );
					}
				}


				pStart = pEnd;
			}

			//advance
			pStart++;
		}
	}




	//option TexArgRemove
	len=0;
	option_len=0;
	if (ReadString(_T("TexArgRemove"), sztOptionBuffer, ARRAYSIZE(sztOptionBuffer)))
	{
		//force null terminate if option specified is too long.
		sztOptionBuffer[OPTION_BUFFER_LENGTH-1] = (TCHAR)0;
		pStart = pEnd = sztOptionBuffer;
		option_len = _tcslen(sztOptionBuffer);
		while (pStart < (sztOptionBuffer + option_len))
		{
			if ((len = _tcscspn(pStart, pSeparator)) > 0)
			{
				//we've found a tex_arg.
				pEnd = pStart + len;
				*pEnd = (TCHAR)0;

				if (_tcscmp(_T("D3DTA_TEMP"), pStart) == 0)
				{
					//clear caps D3DPMISCCAPS_TSSARGTEMP
					m_dwPrimitiveMiscCaps = m_dwPrimitiveMiscCaps & (~((DWORD)D3DPMISCCAPS_TSSARGTEMP));
				}
				else if (_tcscmp(_T("D3DTA_CONSTANT"), pStart) == 0)
				{
					//clear caps D3DPMISCCAPS_PERSTAGECONSTANT
					m_dwPrimitiveMiscCaps = m_dwPrimitiveMiscCaps & (~((DWORD)D3DPMISCCAPS_PERSTAGECONSTANT));
				}
				else if (_tcscmp(_T("D3DTA_SPECULAR"), pStart) == 0)
				{
					//the remove D3DTA_SPECULAR is for intel i81x hardware.
					//They can't do D3DTA_SPECULAR.
					m_dwTextureStageCaps = m_dwTextureStageCaps & (~((DWORD)D3DTSCAPS_SPECULAR));
				}
				else if (_tcscmp(_T("D3DTA_COMPLEMENT"), pStart) == 0)
				{
					//most pre dx8 hardware can't do D3DTA_COMPLEMENT.
					m_dwTextureStageCaps = m_dwTextureStageCaps & (~((DWORD)D3DTSCAPS_COMPLEMENT));
				}
				else if (_tcscmp(_T("D3DTA_ALPHAREPLICATE"), pStart) == 0)
				{
					//most pre dx8 hardware can't do D3DTA_ALPHAREPLICATE.
					m_dwTextureStageCaps = m_dwTextureStageCaps & (~((DWORD)D3DTSCAPS_ALPHAREPLICATE));
				}


				pStart = pEnd;
			}

			//advance
			pStart++;
		}
	}





	return true;
}


bool CBaseTSSTest::GenerateTextureImages()
{
	//test texture.
    if ( !m_TexImages[TEX_TEST].Load("test.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load test.bmp.  Aborting.");
        return false;
    }

	//black and white checker board.
    if ( !m_TexImages[TEX_CHECKER].Load("checker.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load checker.bmp.  Aborting.");
        return false;
    }

	//high frequency black and white checker board.
    if ( !m_TexImages[TEX_CHECKER_HIGHFREQ].Load("checker_highfreq.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load checker_highfreq.bmp.  Aborting.");
        return false;
    }


	//8.1 dx logo for bump.
    if ( !m_TexImages[TEX_DX_LOGOBUMP].Load("dxlogo_bump.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dxlogo_bump.bmp.  Aborting.");
        return false;
    }

	//dx logo for bump2.
    if ( !m_TexImages[TEX_DX_LOGOBUMP2].Load("dxlogo_bump2.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dxlogo_bump2.bmp.  Aborting.");
        return false;
    }

	//8.1 dx logo.
    if ( !m_TexImages[TEX_DX_LOGO].Load("dxlogo.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dxlogo.bmp.  Aborting.");
        return false;
    }

	//8.1 dx logo 2.
    if ( !m_TexImages[TEX_DX_LOGO2].Load("dxlogo2.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dxlogo2.bmp.  Aborting.");
        return false;
    }

	//dx logo 3.
    if ( !m_TexImages[TEX_DX_LOGO3].Load("dxlogo3.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dxlogo3.bmp.  Aborting.");
        return false;
    }

	//dx logo 4.
    if ( !m_TexImages[TEX_DX_LOGO4].Load("dxlogo4.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dxlogo4.bmp.  Aborting.");
        return false;
    }

	//dx logo 5.
    if ( !m_TexImages[TEX_DX_LOGO5].Load("dxlogo5.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dxlogo5.bmp.  Aborting.");
        return false;
    }

	//decoration.
    if ( !m_TexImages[TEX_DECOR].Load("decoration.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load decoration.bmp.  Aborting.");
        return false;
    }

	//decoration 2.
    if ( !m_TexImages[TEX_DECOR2].Load("decoration2.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load decoration2.bmp.  Aborting.");
        return false;
    }

	//dx text1.
    if ( !m_TexImages[TEX_TEXT1].Load("text1.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load text1.bmp.  Aborting.");
        return false;
    }

	//dx text2.
    if ( !m_TexImages[TEX_TEXT2].Load("text2.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load text2.bmp.  Aborting.");
        return false;
    }

	//dx text3.
    if ( !m_TexImages[TEX_TEXT3].Load("text3.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load text3.bmp.  Aborting.");
        return false;
    }

	//dx text4.
    if ( !m_TexImages[TEX_TEXT4].Load("text4.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load text4.bmp.  Aborting.");
        return false;
    }

	//dx text5.
    if ( !m_TexImages[TEX_TEXT5].Load("text5.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load text5.bmp.  Aborting.");
        return false;
    }

	//dx text6.
    if ( !m_TexImages[TEX_TEXT6].Load("text6.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load text6.bmp.  Aborting.");
        return false;
    }

	//colors_blend.
    if ( !m_TexImages[TEX_COLORS_BLEND].Load("colors_blend.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load colors_blend.bmp.  Aborting.");
        return false;
    }

	//windows logo.
    if ( !m_TexImages[TEX_WIN_LOGO].Load("winlogo.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load winlogo.bmp.  Aborting.");
        return false;
    }

	//windows logo 2.
    if ( !m_TexImages[TEX_WIN_LOGO2].Load("winlogo2.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load winlogo2.bmp.  Aborting.");
        return false;
    }

	//windows logo 3.
    if ( !m_TexImages[TEX_WIN_LOGO3].Load("winlogo3.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load winlogo3.bmp.  Aborting.");
        return false;
    }

	//tex stage0.
    if ( !m_TexImages[TEX_STAGE0].Load("stage0.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage0.bmp.  Aborting.");
        return false;
    }

	//tex stage1.
    if ( !m_TexImages[TEX_STAGE1].Load("stage1.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage1.bmp.  Aborting.");
        return false;
    }

	//tex stage2.
    if ( !m_TexImages[TEX_STAGE2].Load("stage2.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage2.bmp.  Aborting.");
        return false;
    }

	//tex stage3.
    if ( !m_TexImages[TEX_STAGE3].Load("stage3.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage3.bmp.  Aborting.");
        return false;
    }

	//tex stage4.
    if ( !m_TexImages[TEX_STAGE4].Load("stage4.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage4.bmp.  Aborting.");
        return false;
    }

	//tex stage5.
    if ( !m_TexImages[TEX_STAGE5].Load("stage5.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage5.bmp.  Aborting.");
        return false;
    }

	//tex stage6.
    if ( !m_TexImages[TEX_STAGE6].Load("stage6.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage6.bmp.  Aborting.");
        return false;
    }

	//tex stage7.
    if ( !m_TexImages[TEX_STAGE7].Load("stage7.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage7.bmp.  Aborting.");
        return false;
    }


	//tex stage_all.
    if ( !m_TexImages[TEX_STAGE_ALL].Load("stage_all.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage_all.bmp.  Aborting.");
        return false;
    }


	//"The New TextureStage".
    if ( !m_TexImages[TEX_NEWTS].Load("newTS.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load newTS.bmp.  Aborting.");
        return false;
    }


	//dots1.
    if ( !m_TexImages[TEX_DOTS1].Load("dots1.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dots1.bmp.  Aborting.");
        return false;
    }


	
	//////////////////////////generated textures/////////////////////////

	//Stripes
	D3DCOLOR pdwColors[] = {
        D3DRGBA(1, 1, 1, 1),	//white
        D3DRGBA(1, 0, 0, 1),	//red
        D3DRGBA(0, 1, 0, 1),	//green
        D3DRGBA(0, 0, 1, 1),	//blue
		D3DRGBA(1, 1, 0, 1),	//yellow
		D3DRGBA(0, 0, 0, 1),	//black
        D3DRGBA(0, 1, 1, 1),	//Brown
	    D3DRGBA(1, 0, 1, 1)
    };
	if ( !m_TexImages[TEX_STRIPES].LoadStripes(LARGE_TEXWIDTH, LARGE_TEXHEIGHT, 4, pdwColors, true, true) )
    {
        WriteToLog("(ERROR) Failed to load TEX_STRIPES.  Aborting.\n");
        return false;
    }

/*
	//Stripes for D3DTA_CONSTANT test.
	//The idea is to use the color channels and rendering space to magnify 
	//	error as much as possible.
    pdwColors[0] = D3DRGBA(0, 0.5f, 0.5f, 0.5f);
    pdwColors[1] = D3DRGBA(1, 0.5f, 0.5f, 0.5f);
    pdwColors[2] = D3DRGBA(0.5f, 0, 0.5f, 0.5f);
    pdwColors[3] = D3DRGBA(0.5f, 1, 0.5f, 0.5f);
    pdwColors[4] = D3DRGBA(0.5f, 0.5f, 0, 0.5f);
    pdwColors[5] = D3DRGBA(0.5f, 0.5f, 1, 0.5f);
    pdwColors[6] = D3DRGBA(0.5f, 0.5f, 0.5f, 0);
    pdwColors[7] = D3DRGBA(0.5f, 0.5f, 0.5f, 1);
	if ( !m_TexImages[TEX_STRIPES_FOR_CONST].LoadStripes(LARGE_TEXWIDTH, LARGE_TEXHEIGHT, 8, pdwColors, false) )
    {
        WriteToLog("(ERROR) Failed to load TEX_STRIPES_FOR_CONST.  Aborting.\n");
        return false;
    }
*/

	// Gradient BLUE_WHITE
	DWORD dwGradColors[4] =
    {
        D3DRGBA(0, 0, 1, 1),
        D3DRGBA(0, 0, 1, 1),
        D3DRGBA(1, 1, 1, 0),
        D3DRGBA(1, 1, 1, 0)
    };
    if ( !m_TexImages[TEX_GRADIENT_BLUE_WHITE].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_GRADIENT_BLUE_WHITE gradient data.  Aborting.\n");
        return false;
    }

	// Gradient BLACK_WHITE
    dwGradColors[0] = D3DRGBA(0, 0, 0, 0);
    dwGradColors[1] = D3DRGBA(0, 0, 0, 0);
    dwGradColors[2] = D3DRGBA(1, 1, 1, 1);
    dwGradColors[3] = D3DRGBA(1, 1, 1, 1);
    if ( !m_TexImages[TEX_GRADIENT_BLACK_WHITE].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_GRADIENT_BLACK_WHITE gradient data.  Aborting.\n");
        return false;
    }

	// Gradient ALPHA
    dwGradColors[0] = D3DRGBA(1, 1, 1, 0);
    dwGradColors[1] = D3DRGBA(1, 1, 1, 0);
    dwGradColors[2] = D3DRGBA(1, 1, 1, 1);
    dwGradColors[3] = D3DRGBA(1, 1, 1, 1);
    if ( !m_TexImages[TEX_GRADIENT_ALPHA].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_GRADIENT_ALPHA gradient data.  Aborting.\n");
        return false;
    }

	// Gradient ALPHA 2
    dwGradColors[0] = D3DRGBA(1, 0, 1, 1);
    dwGradColors[1] = D3DRGBA(1, 1, 0, 0);
    dwGradColors[2] = D3DRGBA(0, 1, 1, 1);
    dwGradColors[3] = D3DRGBA(1, 1, 1, 0);
    if ( !m_TexImages[TEX_GRADIENT_ALPHA2].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_GRADIENT_ALPHA2 gradient data.  Aborting.\n");
        return false;
    }

	//TEX_HALF_WHITE_HALF_ALPHA
    dwGradColors[0] = D3DRGBA(0.5, 0.5, 0.5, 0.5);
    dwGradColors[1] = D3DRGBA(0.5, 0.5, 0.5, 0.5);
    dwGradColors[2] = D3DRGBA(0.5, 0.5, 0.5, 0.5);
    dwGradColors[3] = D3DRGBA(0.5, 0.5, 0.5, 0.5);
    if ( !m_TexImages[TEX_HALF_WHITE_HALF_ALPHA].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_HALF_WHITE_HALF_ALPHA gradient data.  Aborting.\n");
        return false;
    }

	//TEX_GRADIENT_FIRE_WHITE
    dwGradColors[0] = D3DRGBA(0.0, 0.0, 0.0, 0.0);
    dwGradColors[1] = D3DRGBA(0.0, 0.0, 0.0, 0.0);
    dwGradColors[2] = D3DRGBA(0.99, 0.72, 0.15, 1.0);
    dwGradColors[3] = D3DRGBA(0.99, 0.72, 0.15, 1.0);
    if ( !m_TexImages[TEX_GRADIENT_FIRE_WHITE].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_GRADIENT_FIRE_WHITE gradient data.  Aborting.\n");
        return false;
    }

	//TEX_00FFFFFF
    dwGradColors[0] = D3DRGBA(1, 1, 1, 0);
    dwGradColors[1] = D3DRGBA(1, 1, 1, 0);
    dwGradColors[2] = D3DRGBA(1, 1, 1, 0);
    dwGradColors[3] = D3DRGBA(1, 1, 1, 0);
    if ( !m_TexImages[TEX_00FFFFFF].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_00FFFFFF gradient data.  Aborting.\n");
        return false;
    }

	//TEX_GRADIENT_FOR_BUMP
   