#define D3D_OVERLOADS
#include "Lines.h"

//-----------------------------------------------------------------------------

CBasicTest::CBasicTest(bool isAntiAliased, bool withTexture, bool withPattern, D3DSHADEMODE shading)
		   :Lines(isAntiAliased, withTexture, withPattern, shading)
{
	TCHAR szTemp[150];
	sprintf(szTemp, "Basic: %s", szTestName);
	strcpy(szTestName, szTemp);
	sprintf(szTemp, "b%s", szCommandKey);
	strcpy(szCommandKey, szTemp);

	m_szTestName   = _T(szTestName);
	m_szCommandKey = _T(szCommandKey);
	
	m_pTexture  = NULL;
	m_nScissors = 10;
}

//-----------------------------------------------------------------------------

bool CBasicTest::CapsCheck()
{
	if ( !Lines::CapsCheck() )
		return false;

	const DEVICEDESC* pCaps = m_pSrcDevice->GetCaps();

	if ( m_dwVersion < 0x0900  || !(pCaps->dwRasterCaps & D3DPRASTERCAPS_SCISSORTEST) )
    {
		WriteToLog("CapsCheck: Src device does NOT support scissor.\n");
		m_nScissors = 0;
	}

	return true;
}

//-----------------------------------------------------------------------------

bool CBasicTest::Setup()
{
	Lines::Setup();

	if ( m_withTexture )
		return GenerateTexture();

	return true;
}

//-----------------------------------------------------------------------------

UINT CBasicTest::TestInitialize(void)
{
	SetTestRange(1, m_nViewMatrices*m_nPrimTypes*(m_nX*m_nZ+m_nScissors)*2 + 1);
		
	m_nVertices = 0;
	m_dwPattern = 0;
	m_uPrevTestNumber = 0;

	return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------

bool CBasicTest::ExecuteTest(UINT iTest)
{
	TCHAR szScissor[100];
	TCHAR szPattern[50];
	szScissor[0] = '\0';
	msgString[0] = '\0';
	szPattern[0] = '\0';

	D3DPRIMITIVETYPE primType = (D3DPRIMITIVETYPE)(((iTest-2)/m_nViewMatrices)%m_nPrimTypes+2); ////exclude 1st case: special buffers
	bool isTL = (iTest - 1) > (m_uTotalTests - 1)/2; //exclude 1st case: special buffers
	
	if (isTL)
		iTest -= m_nViewMatrices*m_nPrimTypes*(m_nX*m_nZ+m_nScissors) + 1;
	else
		iTest--;  //exclude 1st case: special buffers

	bool withScissor = ((iTest-1) >= m_nViewMatrices*m_nPrimTypes*(m_nX*m_nZ) && iTest != 0);

	DWORD dwX, dwZ;
	if ( withScissor )
	{
		//need to use rand() for generating scissor parameters
		srand(iTest);
		dwX = 20;
		dwZ = 15;
		SetScissorParam(szScissor);
	}
	else
	{
		dwX = ((iTest-1)/(m_nViewMatrices*m_nPrimTypes))%m_nX+3;        
		dwZ = ((iTest-1)/((m_nViewMatrices*m_nPrimTypes)*m_nX))%m_nZ+2;
	}
	
	//get pattern
	if ( m_withPattern )
	{
		GetPattern(szPattern);
	}
	
	if (m_uTestNumber == 1)
	{
		_stprintf( msgString, _T("Grids and Swirls, %s"), szPattern);
	}
	else
	{
		_stprintf( msgString, TEXT("%s, %s%s PrimType, %sdwX = dwY = %d, dwZ = %d"), 
				   isTL ? "Transf" : "Untransf",		szPattern,
				   primType == D3DPT_LINELIST ? "LINELIST" : (primType == D3DPT_LINESTRIP ? "LINESTRIP" : "TRIANGLELIST/STRIP/FAN"), 
				   szScissor, dwX, dwZ );
	}
	
	BeginTestCase(msgString, m_uTestNumber);

	if (m_uTestNumber == 1)
	{
		if ( !GenerateSpecialBuffer() )
		{
			WriteToLog( "ExecuteTest: GenerateSpecialBuffer() failed.\n" );
			m_uPrevTestNumber = m_uTestNumber;
			Fail();
			return false;
		}
	}
	else
	{
		if ( (m_uTestNumber-2)%m_nViewMatrices == 0 || (m_uPrevTestNumber != m_uTestNumber - 1) || m_pVB == NULL ) //exclude 1st case: special buffers
		{
			if ( !GenerateBuffers( primType, dwX, dwZ, m_mTransform[0], isTL ) )
			{
				WriteToLog( "ExecuteTest: GenerateBuffers() failed.\n" );
				UINT end = m_uTestNumber + m_nViewMatrices - 1;
				end = end - (end-1) % m_nViewMatrices;
				m_uPrevTestNumber = m_uTestNumber;
				Fail();
				if (m_uTestNumber+1 <= end)
					SkipTestRange( m_uTestNumber+1, end);
				return false;
			}
		}
	}

	//set new pattern
	if ( m_withPattern && (m_uTestNumber == 1 || ((m_uTestNumber-2)%m_nViewMatrices == 0 && primType == D3DPT_LINELIST) || (m_uPrevTestNumber != m_uTestNumber - 1)) )
		SetRenderState((D3DRENDERSTATETYPE)10/*D3DRS_LINEPATTERN*/, m_dwPattern);

	//inable Scissor for Scissor cases
	if ( withScissor && ( iTest-1 == m_nViewMatrices*m_nPrimTypes*(m_nX*m_nZ) || m_uPrevTestNumber != m_uTestNumber - 1) )
	{
		SetRenderState(D3DRS_SCISSORTESTENABLE, (DWORD)TRUE);
		SetScissorRect(&m_Scissor);
		m_pFramework->GetImageCmp()->SetMinPixels(0);
	}
	if ( !withScissor && ( m_uTestNumber-1 == m_nViewMatrices*m_nPrimTypes*(m_nX*m_nZ+m_nScissors) + 1 || m_uPrevTestNumber != m_uTestNumber - 1) )
	{
		SetRenderState(D3DRS_SCISSORTESTENABLE, (DWORD)FALSE);
		m_pFramework->GetImageCmp()->SetMinPixels(1);
	}
	//set Scissor
	if ( withScissor )
	{
		SetScissorRect(&m_Scissor);
	}

	DWORD NumPass = 1;
	HRESULT hr;
	if (!m_pSrcDevice->IsPure() && FAILED( hr = m_pSrcDevice->ValidateDevice(&NumPass) ))
	{
		WriteToLog("ValidateDevice failed. hr=%s(%x)\n", m_pD3D->HResultToString(hr), hr);			
		return false;
	}

	BeginScene();

	if ( withScissor && KeySet("showscissor"))
	{
		TLVERTEX tlv[5];
		tlv[0] = TLVERTEX(D3DXVECTOR4( (FLOAT)m_Scissor.left, (FLOAT)m_Scissor.top, 0.0, 1.0 ), 0xff00ff00, 0.0, 1.0);
		tlv[1] = TLVERTEX(D3DXVECTOR4( (FLOAT)m_Scissor.right-1, (FLOAT)m_Scissor.top, 0.0, 1.0 ), 0xff00ff00, 0.0, 1.0);
		tlv[2] = TLVERTEX(D3DXVECTOR4( (FLOAT)m_Scissor.right-1, (FLOAT)m_Scissor.bottom-1, 0.0, 1.0 ), 0xff00ff00, 0.0, 1.0);
		tlv[3] = TLVERTEX(D3DXVECTOR4( (FLOAT)m_Scissor.left, (FLOAT)m_Scissor.bottom-1, 0.0, 1.0 ), 0xff00ff00, 0.0, 1.0);
		tlv[4] = TLVERTEX(D3DXVECTOR4( (FLOAT)m_Scissor.left, (FLOAT)m_Scissor.top, 0.0, 1.0 ), 0xff00ff00, 0.0, 1.0);
		SetVertexShader(FVF_TLVERTEX);
		DrawPrimitiveUP(D3DPT_LINESTRIP, 4, tlv, GetVertexSize(FVF_TLVERTEX));	
		if (!isTL)
			SetVertexShader(D3DFVF_VERTEX);
	}

	if (m_uTestNumber == 1)
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

	m_uPrevTestNumber = m_uTestNumber;

	return true;
}

//-----------------------------------------------------------------------------

void CBasicTest::Cleanup()
{
	RELEASE(m_pTexture);
}

//-----------------------------------------------------------------------------

bool CBasicTest::TestTerminate()
{
	Lines::TestTerminate();
	Cleanup();

	return true;
}

//-----------------------------------------------------------------------------

bool CBasicTest::GenerateTexture()
{
	HRESULT hr;
	FORMAT Format;
	TCHAR szBuffer[50];

	Format = m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

	FmtToString(FORMAT_TEXTURE, &Format, szBuffer);

	if(!CreateTexture(128, 128, 1, 0, Format, POOL_MANAGED, &m_pTexture))
	{
		WriteToLog("GenerateTexture::CreateTextures failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
		Fail();
		return false;
	}

	if(m_pTexture->IsPalettized())
		m_pTexture->SetPalette(NULL);

	if(!m_pTexture->LoadTexture(0, "tex0.bmp"))
	{
		WriteToLog("GenerateTextures: LoadTexture failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
		Fail();
		RELEASE(m_pTexture);
		return false;
	}

	SetTexture(0, m_pTexture);
	
	return true;
}

//-----------------------------------------------------------------------------

void CBasicTest::SetScissorParam(TCHAR *szScissor)
{
	UINT Width  = m_Options.D3DOptions.nBackBufferWidth;
	UINT Height = m_Options.D3DOptions.nBackBufferHeight;

	m_Scissor.left   = rand() % Width;   //left [0, Width)
	m_Scissor.top    = rand() % Height;  //top  [0, Height) 
	if (m_Scissor.left < Width-1)
		m_Scissor.right  = rand() % (Width - (m_Scissor.left+1))+ (m_Scissor.left+1);  //right [left+1, Widht)
	else
		m_Scissor.right  = m_Scissor.left + 1;
	if (m_Scissor.top < Height-1)
		m_Scissor.bottom = rand() % (Height - (m_Scissor.top+1))+ (m_Scissor.top+1); //bottom [top + 1, Height)
	else
		m_Scissor.bottom  = m_Scissor.top + 1;
	
	int t;
	if (ReadInteger("sleft", &t))
		m_Scissor.left   = t;
	if (ReadInteger("stop", &t))
		m_Scissor.top    = t;
	if (ReadInteger("sright", &t))
		m_Scissor.right  = t;
	if (ReadInteger("sbottom", &t))
		m_Scissor.bottom = t;

	_stprintf(szScissor, _T("Scissor = (%d, %d, %d, %d), "), m_Scissor.left, m_Scissor.top, m_Scissor.right, m_Scissor.bottom);
	
}