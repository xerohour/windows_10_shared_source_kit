#define D3D_OVERLOADS
#include "Lines.h"

#define W_FOG 0
#define Z_FOG 1

//-----------------------------------------------------------------------------

CPFogTest::CPFogTest(bool isAntiAliased, bool withTexture, bool withPattern, D3DSHADEMODE shading)
		  :Lines(isAntiAliased, withTexture, withPattern, shading)
{
	TCHAR szTemp[150];
	sprintf(szTemp, "PFog: %s", szTestName);
	strcpy(szTestName, szTemp);
	sprintf(szTemp, "pf%s", szCommandKey);
	strcpy(szCommandKey, szTemp);

	m_szTestName = _T(szTestName);
	m_szCommandKey = _T(szCommandKey);
	
	m_pTexture = NULL;
	m_nX = 5;
	m_nZ = 3;
}

//-----------------------------------------------------------------------------

bool CPFogTest::Setup()
{
	Lines::Setup();

	if ( m_withTexture )
		return GenerateTexture();

	return true;
}

//-----------------------------------------------------------------------------

UINT CPFogTest::TestInitialize(void)
{
	SetTestRange(1, m_nViewMatrices*m_nPrimTypes*m_nX*m_nZ*3*2*2 + 1);
	m_nVertices = 0;
	m_dwPattern = 0;
	m_uPrevTestNumber = 0;

	return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------

bool CPFogTest::ExecuteTest(UINT iTest)
{
	D3DPRIMITIVETYPE primType = (D3DPRIMITIVETYPE)(((iTest-2)/m_nViewMatrices)%3+2); //exclude 1st case: special buffers
	D3DFOGMODE       fogMode  = (D3DFOGMODE)(((iTest-2)/(m_nViewMatrices*m_nPrimTypes*m_nX*m_nZ*2))%3+1); //exclude 1st case: special buffers
	WORD             ZFog     = ((iTest-2)/(m_nViewMatrices*m_nPrimTypes*m_nX*m_nZ))%2; //exclude 1st case: special buffers
	DWORD            dwX, dwZ;

	dwX = ((iTest-2)/(m_nViewMatrices*m_nPrimTypes))%m_nX + 20; //exclude 1st case: special buffers
	dwZ = ((iTest-2)/((m_nViewMatrices*m_nPrimTypes)*m_nX))%m_nZ + 15; //exclude 1st case: special buffers
	bool isTL = (iTest - 1) > (m_uTotalTests - 1)/2; //exclude 1st case: special buffers
	
	TCHAR szPattern[50];
	
	msgString[0] = '\0';
	szPattern[0] = '\0';

	//get pattern
	if ( m_withPattern )
	{
		GetPattern(szPattern);
	}
	
	if (iTest == 1)
	{
		_stprintf( msgString, _T("Grids and Swirls, %s"), szPattern);
	}
	else
	{
		_stprintf( msgString, TEXT("%s %s, %s, %s%s PrimType, dwX = dwY = %d, dwZ = %d"), 
				   fogMode == D3DFOG_LINEAR ? "LINEAR" : (fogMode == D3DFOG_EXP ? "EXP" : "EXP2"),
  				   ZFog == Z_FOG ? "Z_Fog" : "W_Fog",
				   isTL ? "Transf" : "Untransf", szPattern,
				   primType == D3DPT_LINELIST ? "LINELIST" : (primType == D3DPT_LINESTRIP ? "LINESTRIP" : "TRIANGLELIST/STRIP/FAN"), 
				   dwX, dwZ );
	}

	BeginTestCase(msgString, iTest);

	if (iTest == 1)
	{
		if ( !GenerateSpecialBuffer() )
		{
			WriteToLog( "GenerateSpecialBuffer() failed.\n" );
			m_uPrevTestNumber = iTest;
			Fail();
			return false;
		}
		ZFog = Z_FOG;
	}
	else
	{
		if ( (iTest-2)%m_nViewMatrices == 0 || (m_uPrevTestNumber != iTest - 1) || m_pVB == NULL )
		{
			if ( !GenerateBuffers( primType, dwX, dwZ, m_mFogTransform[ZFog], isTL) )
			{
				WriteToLog("ExecuteTest: GenerateBuffers failed.\n");
				UINT end = iTest + m_nViewMatrices - 1;
				end = end - (end-1) % m_nViewMatrices;
				m_uPrevTestNumber = iTest;
				Fail();
				if (iTest+1 <= end)
					SkipTestRange( iTest+1, end);
				return false;
			}
		}
	}

	//set new pattern
	if ( m_withPattern && (iTest == 1 || ((iTest-2)%m_nViewMatrices == 0 && primType == D3DPT_LINELIST) || (m_uPrevTestNumber != iTest - 1)) )
		SetRenderState((D3DRENDERSTATETYPE)10/*D3DRS_LINEPATTERN*/, m_dwPattern);
	
	//settings for fog
	if ( iTest == 1 || ((iTest-2)%m_nViewMatrices == 0 && primType == D3DPT_LINELIST && dwX== 20 && dwZ == 15) || (m_uPrevTestNumber != iTest - 1) )
	{
		float Start, End;

		//only set this state once to avoid duplicating render state
		if (ZFog == W_FOG || m_uPrevTestNumber != iTest - 1)
			SetRenderState(D3DRS_FOGTABLEMODE, fogMode);

		if ( ZFog == Z_FOG ) //z in device space
		{
			Start = 0.5f*m_mFogTransform[Z_FOG].m[2][2] + m_mFogTransform[Z_FOG].m[3][2];
			End   = 0.8f*m_mFogTransform[Z_FOG].m[2][2] + m_mFogTransform[Z_FOG].m[3][2];

			if ( !(m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_ZFOG) )
			{
				WriteToLog("ExecuteTest: ZFOG is not supported by the driver.\n");
				UINT end = iTest + m_nViewMatrices*m_nPrimTypes*m_nX*m_nZ - 1;
				end = end - (end-1) %(m_nViewMatrices*m_nPrimTypes*m_nX*m_nZ);
				if (iTest <= end)
					SkipTestRange( iTest, iTest == 1 ? iTest : end);
				m_uPrevTestNumber = iTest;
				return true;
			}
			else
				SetTransform(D3DTS_PROJECTION, &m_ProjMatrices[Z_FOG]);
		}
		else //w in device space
		{
			Start = 0.5f*m_mFogTransform[W_FOG].m[2][3] + m_mFogTransform[W_FOG].m[3][3];
			End   = 0.8f*m_mFogTransform[W_FOG].m[2][3] + m_mFogTransform[W_FOG].m[3][3];
			
			if ( !(m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_WFOG) )
			{
				WriteToLog("ExecuteTest: WFOG is not supported by the driver.\n");
				UINT end = iTest + m_nViewMatrices*m_nPrimTypes*m_nX*m_nZ - 1;
				end = end - (end-1) %(m_nViewMatrices*m_nPrimTypes*m_nX*m_nZ);
				if (iTest <= end)
					SkipTestRange( iTest, iTest == 1 ? iTest : end);
				m_uPrevTestNumber = iTest;
				return true;
			}
			else
				SetTransform(D3DTS_PROJECTION, &m_ProjMatrices[W_FOG]);
		}

		if( fogMode == D3DFOG_LINEAR )
		{
			SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
			SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&End));
		}
		else if (ZFog == W_FOG || m_uPrevTestNumber != iTest - 1)
		{
			float Density;
			if( fogMode == D3DFOG_EXP )
				Density = 0.3f;
			else 
				Density = 0.9f;

			SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&Density));
		}
	}

	BeginScene();

	if (iTest == 1)
	{
		if ( !DrawSpecialLines())
		{
			WriteToLog("DrawWithSpecialBuffer() failed.\n");			
			EndScene();
			return false;
		}
	}
	else
	{
		if ( !DrawLines(isTL, dwX, dwZ, primType))
		{
			WriteToLog("DrawLines() failed.\n");			
			EndScene();
			return false;
		}
	}

	EndScene();

	m_uPrevTestNumber = iTest;

	return true;
}

//-----------------------------------------------------------------------------

void CPFogTest::Cleanup()
{
	RELEASE(m_pTexture);
}

//-----------------------------------------------------------------------------

bool CPFogTest::TestTerminate()
{
	Lines::TestTerminate();
	Cleanup();

	return true;
}

//-----------------------------------------------------------------------------

bool CPFogTest::GenerateTexture()
{
	HRESULT hr;
	FORMAT Format;
	TCHAR szBuffer[80];

	Format = m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

	FmtToString(FORMAT_TEXTURE, &Format, szBuffer);

	if(!CreateTexture(128, 128, 1, 0, Format, POOL_MANAGED, &m_pTexture))
	{
		WriteToLog(_T("GenerateTexture: CreateTexture failed. hr=%s.\n"), m_pD3D->HResultToString(GetLastError()));
		Fail();
		return false;
	}

	if(m_pTexture->IsPalettized())
		m_pTexture->SetPalette(NULL);

	if(!m_pTexture->LoadTexture(0, "tex0.bmp"))
	{
		WriteToLog(_T("GenerateTextures: LoadTexture failed. hr=%s.\n"), m_pD3D->HResultToString(GetLastError()));
		Fail();
		RELEASE(m_pTexture);
		return false;
	}

	SetTexture(0, m_pTexture);

	return true;
}

//-----------------------------------------------------------------------------

bool CPFogTest::CapsCheck()
{
	if ( !Lines::CapsCheck() )
		return false;

	const DEVICEDESC* pCaps = m_pSrcDevice->GetCaps();
	
	if ( !(pCaps->dwLineCaps & D3DLINECAPS_FOG) )
	{
		WriteToLog("CapsCheck: fog is not supported by the driver.\n");
		return false;
	}
	if ( !(pCaps->dwRasterCaps & D3DPRASTERCAPS_FOGTABLE) )
	{
		WriteToLog("CapsCheck: pixel fog is not supported by the driver.\n");
		return false;
	}
	if (  m_ShadingMode == D3DSHADE_GOURAUD && !(pCaps->dwShadeCaps & D3DPSHADECAPS_FOGGOURAUD) )
	{
		WriteToLog("CapsCheck: pixel fog is not supported for GOURAUD shade mode by the driver.\n");
		return false;
	}

	SetRenderState(D3DRS_FOGENABLE, (DWORD)TRUE);
	SetRenderState(D3DRS_FOGCOLOR, 0x0000ff00);

	//create projection matrix for W-Fog
	float fAspect = 1.33f;
	float  fFOV = pi/4.0f;
	D3DXMatrixIdentity( &m_ProjMatrices[W_FOG] );
	D3DXMatrixPerspectiveFovLH(&m_ProjMatrices[W_FOG], fFOV, fAspect, 1.0f, 1000.0f);
	//create projecttion matrix for Z-Fog
	float fZn = 2.4f;
	float fZf = 3.6f;
	D3DXMatrixIdentity( &m_ProjMatrices[Z_FOG] );
	D3DXMatrixOrthoLH( &m_ProjMatrices[Z_FOG], 2.f, 2.f, fZn, fZf);

	//get transformation matrices World*View*Proj*Viewport
	D3DXVECTOR3 vLoc, vAt, vUp;
	D3DXMATRIX Matrix, TMatrix, WorldMatrix, ViewMatrix;

	vLoc = D3DXVECTOR3(0.0f, 0.0f, -2.5f);
	vAt  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vUp  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMatrixIdentity( &ViewMatrix );
    D3DXMatrixLookAtLH(&ViewMatrix, &vLoc, &vAt, &vUp);

    D3DXMatrixIdentity( &WorldMatrix );

	for (int i = 0; i < 2; i++)
	{
		D3DXMatrixIdentity( &m_mFogTransform[i] );
		D3DXMatrixIdentity( &Matrix ); //world matrix is identity matrix
		D3DXMatrixMultiply(&m_mFogTransform[i], &Matrix, &ViewMatrix);
		Matrix = m_mFogTransform[i];
		D3DXMatrixMultiply(&m_mFogTransform[i], &Matrix, &m_ProjMatrices[i]);
	
		//viewport matrix
		D3DXMatrixIdentity(&Matrix);
		Matrix(0,0) =  0.f + (FLOAT)( m_pCurrentMode->nBackBufferWidth/2 );
		Matrix(3,0) =  0.f + (FLOAT)( m_pCurrentMode->nBackBufferWidth/2 );
		Matrix(1,1) = -1.f * (FLOAT)( m_pCurrentMode->nBackBufferHeight/2 );
		Matrix(3,1) =  0.f + (FLOAT)( m_pCurrentMode->nBackBufferHeight/2 );
		Matrix(2,2) = 1.f - 0.f;
		Matrix(3,2) = 0.f;
		Matrix(3,3) = 1.f;
	
		TMatrix = m_mFogTransform[i];
		D3DXMatrixMultiply(&m_mFogTransform[i], &TMatrix, &Matrix);
	}	
		    
	return true;
}

