#define D3D_OVERLOADS
#include "Lines.h"

TCHAR           * g_szTexAddresses[] = { "TADDRESS_CLAMP",
									   "TADDRESS_WRAP",
									   "TADDRESS_MIRROR",
									   //"TADDRESS_BORDER",
									   "TADDRESS_MIRRORONCE"
									 };

D3DTEXTUREADDRESS g_TexAddresses[] = { D3DTADDRESS_CLAMP,
									   D3DTADDRESS_WRAP,
									   D3DTADDRESS_MIRROR,
									   //D3DTADDRESS_BORDER,
									   D3DTADDRESS_MIRRORONCE
									 };

DWORD g_TexAddressCaps[] = { D3DPTADDRESSCAPS_CLAMP,
                             D3DPTADDRESSCAPS_WRAP,
                             D3DPTADDRESSCAPS_MIRROR,
                             //D3DPTADDRESSCAPS_BORDER,
                             D3DPTADDRESSCAPS_MIRRORONCE
						   };

#define g_nTexAddress ( sizeof(g_TexAddresses) / (sizeof(g_TexAddresses[0])) )

//-----------------------------------------------------------------------------

CMultiTexTest::CMultiTexTest(bool isAntiAliased, bool withPattern, D3DSHADEMODE shading)
			  :Lines(isAntiAliased, true, withPattern, shading)
{
	TCHAR szTemp[150];
	sprintf(szTemp, "MultiTex: %s", szTestName);
	strcpy(szTestName, szTemp);
	sprintf(szTemp, "m%s", szCommandKey);
	strcpy(szCommandKey, szTemp);

	m_szTestName = _T(szTestName);
	m_szCommandKey = _T(szCommandKey);
	
	m_pTextures    = NULL;
	m_pTexAddress  = NULL;
	m_szTexAddress = NULL;
	m_nTexAddress  = 0;
	m_nMaxTextures = 0;
	m_nTextures    = 0;
	m_nOperations  = 2;

	m_Options.ModeOptions.dwTexExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN;
}

//-----------------------------------------------------------------------------

bool CMultiTexTest::CapsCheck()
{
	if ( !Lines::CapsCheck() )
		return false;

	m_nTexAddress = 0;
	DEVICEDESC* DeviceCaps = m_pSrcDevice->GetCaps();	

	//determine how many texture address modes are supported
	for (UINT i = 0; i < g_nTexAddress; i++)
	{
		if ( g_TexAddressCaps[i] == (g_TexAddressCaps[i] & DeviceCaps->dwTextureAddressCaps) )
			m_nTexAddress++;
	}
	
    m_pTexAddress = new D3DTEXTUREADDRESS[m_nTexAddress];
	m_szTexAddress = new TCHAR*[m_nTexAddress];

	UINT j = 0;
	for (UINT i = 0; i < g_nTexAddress; i++)
	{
		if ( g_TexAddressCaps[i] == (g_TexAddressCaps[i] & DeviceCaps->dwTextureAddressCaps) )
		{
			m_pTexAddress[j]    = g_TexAddresses[i];
			m_szTexAddress[j++] = g_szTexAddresses[i];
		}
	}

	//determine how many texture stages we can use
	m_nMaxTextures = DeviceCaps->wMaxSimultaneousTextures;
	//we use COLORARG1 = TEXTURE (default), COLORARG2 = DIFFUSE(0th stage)/CURRENT(other stages(default))
	SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE) ;
	//we only use one texcoordinates set.
	for (UINT i = 1; i < m_nMaxTextures; i++)
	{
		SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		SetTextureStageState(i, D3DTSS_ALPHAARG1, (DWORD)D3DTA_DIFFUSE);
	}

	return true;
}

//-----------------------------------------------------------------------------

UINT CMultiTexTest::TestInitialize(void)
{
#ifdef FAST_TEST
	m_uCommonTextureFormats = 1;
#endif
	SetTestRange(1, m_uCommonTextureFormats*m_nViewMatrices*m_nPrimTypes*(m_nOperations+m_nTexAddress)*2 + 1);
	m_nVertices = 0;
	m_dwPattern = 0;
	m_nTextures = 0;
	m_uPrevTestNumber = 0;
	
	return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------

bool CMultiTexTest::ExecuteTest(UINT iTest)
{
	TCHAR            szBuffer[50];
	D3DPRIMITIVETYPE primType;
	UINT             index;
	bool             isTL;
	DWORD            dwX, dwZ;
	TCHAR            szPattern[50];
	
	msgString[0] = '\0';
	szPattern[0] = '\0';

	//get pattern
	if ( m_withPattern )
	{
		GetPattern(szPattern);
	}

	//figure out test case parameters and title
	if (iTest == 1)
	{
		index = 0;//D3DTOP_ADD;
		m_pCurrentMode->nTextureFormat = 0;
		FmtToString(FORMAT_TEXTURE, GetCurrentTextureFormat(), szBuffer);
		_stprintf( msgString, _T("Grids and Swirls, %s, %s"), szBuffer, szPattern);
	}
	else
	{
        index = ((iTest-2)/(m_nViewMatrices*m_nPrimTypes)) % (m_nOperations+m_nTexAddress);   //exclude 1st case: special buffers
		primType = (D3DPRIMITIVETYPE)(((iTest-2)/m_nViewMatrices)%m_nPrimTypes+2);            //exclude 1st case: special buffers
		isTL = (iTest -1) > (m_uTotalTests - 1)/2;                                            //exclude 1st case: special buffers
		dwX = 30;
		dwZ = 15;

		UINT TexIndex = ((iTest-2)/(m_nViewMatrices*m_nPrimTypes*(m_nOperations+m_nTexAddress)))%m_uCommonTextureFormats; //exclude 1st case: special buffers
		FmtToString(FORMAT_TEXTURE, &m_pCommonTextureFormats[TexIndex], szBuffer);
		
		_stprintf( msgString, TEXT("%s, %s, %s, %s%s PrimType"), szBuffer,
				   index < m_nOperations ? (index ? "TOP_MODULATE" : "TOP_ADD") : m_szTexAddress[index - m_nOperations],
  				   isTL ? "Transf" : "Untransf", szPattern,
				   primType == D3DPT_LINELIST ? "LINELIST" : (primType == D3DPT_LINESTRIP ? "LINESTRIP" : "TRIANGLELIST/STRIP/FAN") );
	}

	BeginTestCase(msgString, iTest);

	if (iTest == 1)
	{
		if ( !GenerateSpecialBuffer() )
		{
			WriteToLog( "GenerateSpecialBuffer() failed.\n" );
			Fail();
			m_uPrevTestNumber = iTest;
			return false;
		}
	}
	else
	{
		if ( (iTest-2)%m_nViewMatrices == 0 || (m_uPrevTestNumber != iTest - 1) || m_pVB == NULL ) //exclude 1st case: special buffers
		{
			if ( !GenerateBuffers( primType, dwX, dwZ, m_mTransform[0], isTL, index >= m_nOperations ? true : false ) )
			{
				WriteToLog("ExecuteTest: GenerateBuffers failed.\n" );
				UINT end = iTest + m_nViewMatrices - 1;
				end = end - (end-1) % m_nViewMatrices;
				Fail();
				if (iTest+1 <= end)
					SkipTestRange( iTest+1, end);
				m_uPrevTestNumber = iTest;
				return false;
			}
		}
	}

	//generate textures of current format
	if ( iTest == 1 || ((iTest-2)%m_nViewMatrices == 0 && primType == D3DPT_LINELIST) || (m_uPrevTestNumber != iTest - 1) || m_nTextures == 0 )
	{
		RemoveTextures();

		if (iTest == 1)
			m_pCurrentMode->nTextureFormat = 0;
		else
			m_pCurrentMode->nTextureFormat = ((iTest-2)/(m_nViewMatrices*m_nPrimTypes*(m_nOperations+m_nTexAddress)))%m_uCommonTextureFormats; //exclude 1st case: special buffers

		m_nTextures = (iTest == 1 ? m_nMaxTextures : m_nMaxTextures - ((iTest-2)/(m_nViewMatrices*m_nPrimTypes))%m_nMaxTextures);

		if ( !SetupTextures((index & 1) ? D3DTOP_MODULATE : D3DTOP_ADD) )
		{
			WriteToLog("ExecuteTest: GenerateTextures failed.\n" );
			//fail this test case and skipp the rest of acases with current format
			UINT end = iTest + m_nViewMatrices*m_nPrimTypes*(m_nOperations+m_nTexAddress) - 1;
			end = end - (end-1) %(m_nViewMatrices*m_nPrimTypes*(m_nOperations+m_nTexAddress));
			Fail();
			if (iTest+1 <= end)
				SkipTestRange( iTest+1, end);
			m_nTextures = 0;
			m_uPrevTestNumber = iTest;
			return false;
		}
		/*if (KeySet("index"))
		{
			if( D3DTADDRESS_BORDER == m_pTexAddress[index - m_nOperations] )
			{
				WORD c = (m_pCurrentMode->nTextureFormat*10) % (256-100) + 100;

				FORMAT *Format = GetCurrentTextureFormat();

				//bug 88079
				if ( Format->d3dfFormat == FMT_A8 )
					c = 0;
				if ( Format->d3dfFormat == FMT_R16F || Format->d3dfFormat == FMT_R32F ||
				     Format->d3dfFormat == FMT_G16R16F || Format->d3dfFormat == FMT_G32R32F || Format->d3dfFormat == FMT_G16R16 ||
					 Format->d3dfFormat == FMT_L8 || Format->d3dfFormat == FMT_L16 || Format->d3dfFormat == FMT_A4L4 || Format->d3dfFormat == FMT_A8L8 )
					c = 255;

				for (int i = 0; i < m_nTextures; i++)
					SetSamplerState(i, SAMP_BORDERCOLOR, D3DCOLOR_ARGB(c, c, c, c));
			}
		}
		else
		{
			if( index >= m_nOperations && D3DTADDRESS_BORDER == m_pTexAddress[index - m_nOperations] )
			{
				WORD c = (m_pCurrentMode->nTextureFormat*10) % (256-100) + 100;
				for (int i = 0; i < m_nTextures; i++)
					SetSamplerState(i, SAMP_BORDERCOLOR, D3DCOLOR_ARGB(c, c, c, c));
			}
		}*/
	}

	//set texture address modes
	if ( index >= m_nOperations )
	{
		if (m_pD3D->GetRuntimeVersion() >= 9.029f && m_pTexAddress[index - m_nOperations] != D3DTADDRESS_CLAMP &&
			FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
											 USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, *(GetCurrentTextureFormat()))) )
		 {
			WriteToLog("ExecuteTest: USAGE_QUERY_WRAPANDMIP is not set for %s format.\n", szBuffer );
			//skip cases with address modes if WRAPANDMIP flag is not set for current format
			UINT end = iTest + m_nViewMatrices*m_nPrimTypes*(m_nOperations+m_nTexAddress) - 1;
			end = end - (end-1) %(m_nViewMatrices*m_nPrimTypes*(m_nOperations+m_nTexAddress));
			if (iTest <= end)
				SkipTestRange( iTest, end);
			m_uPrevTestNumber = iTest;
			return true;
		}
		
		if (D3DTADDRESS_WRAP != m_pTexAddress[index - m_nOperations])
		{
			for (int i = 0; i < m_nTextures; i++)
			{
				SetSamplerState(i, SAMP_ADDRESSU, m_pTexAddress[index - m_nOperations]);
				SetSamplerState(i, SAMP_ADDRESSV, m_pTexAddress[index - m_nOperations]);
			}
		}
	}

	//set new pattern
	if ( m_withPattern && (iTest == 1 || ((iTest-2)%m_nViewMatrices == 0 && primType == D3DPT_LINELIST) || (m_uPrevTestNumber != iTest - 1)) )
		SetRenderState((D3DRENDERSTATETYPE)10/*D3DRS_LINEPATTERN*/, m_dwPattern);

	DWORD dwPasses = 1;
	HRESULT hr;
	if (!m_pSrcDevice->IsPure() && FAILED( hr = m_pSrcDevice->ValidateDevice(&dwPasses) ))
	{
		SkipTestRange( iTest, iTest + m_nViewMatrices - 1);
		WriteToLog("ValidateDevice failed. hr=%s(%x)\n", m_pD3D->HResultToString(hr), hr);			
		return false;
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

	if ( index >= m_nOperations )
	{
		if (D3DTADDRESS_WRAP != m_pTexAddress[index - m_nOperations])
		{
			for (int i = 0; i < m_nTextures; i++)
			{
				SetSamplerState(i, SAMP_ADDRESSU, D3DTADDRESS_WRAP);
				SetSamplerState(i, SAMP_ADDRESSV, D3DTADDRESS_WRAP);
			}
		}
	}

	m_uPrevTestNumber = iTest;

	return true;
}

//-----------------------------------------------------------------------------

bool CMultiTexTest::TestTerminate()
{
	Lines::TestTerminate();

	if (m_pTexAddress != NULL)
		delete [] m_pTexAddress;
	if (m_szTexAddress != NULL)
		delete [] m_szTexAddress;

	for (int i = 0; i < m_nTextures; i++)
        RELEASE(m_pTextures[i]);
	if ( m_nTextures )
		SAFEDELETEA(m_pTextures);
	m_nTextures = 0;

	return true;
}

//-----------------------------------------------------------------------------

bool CMultiTexTest::SetupTextures(D3DTEXTUREOP top_op)
{
	HRESULT hr = S_OK;
	DWORD *Stripes, tempStripes[7];
	FORMAT *pFormat = GetCurrentTextureFormat();

	m_pTextures = new CnTexture*[m_nTextures];
	Stripes = new DWORD[m_nTextures];

	ZeroMemory(m_pTextures, sizeof(CnTexture*) * m_nTextures);

	for (int i = 0; i < m_nTextures; i++)
	{
		if(!CreateTexture(128, 128, 1, 0, *pFormat, POOL_MANAGED, &m_pTextures[i]))
		{
			hr = GetLastError();
			WriteToLog("SetupTextures: Create %d Texture failed. hr=%s(%x).\n", i, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		if(m_pTextures[i]->IsPalettized())
			m_pTextures[i]->SetPalette(NULL);

		//loading texture
		WORD color = 255-15*i;
		color = color % (256-150) + 150;
		tempStripes[0] = RGBA_MAKE(color, 0, 0, 255);
		tempStripes[1] = RGBA_MAKE(0, color, 0, 255);
		tempStripes[2] = RGBA_MAKE(0, 0, color, 255);
		tempStripes[3] = RGBA_MAKE(0, color, color, 255);
		tempStripes[4] = RGBA_MAKE(color, 0, color, 255);
		tempStripes[5] = RGBA_MAKE(color, color, 0, 255);
		tempStripes[6] = RGBA_MAKE(color, color, color, 255);
		
		for (int j = 0; j < m_nTextures; j++)
			if ( top_op == D3DTOP_ADD )
				Stripes[j] = 0x0;
			else
				Stripes[j] = 0xffffffff;
		Stripes[i] = tempStripes[i%7];

		if ( !m_pTextures[i]->LoadStripes(0, 128, 128, m_nTextures, Stripes, true, true) )
		{
			hr = GetLastError();
			WriteToLog("SetupTextures: LoadStripes failed on %d Texture. hr=%s(%x).\n", i, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		//by default linear filters are already set
		//however if current format doesn't support linear filters, we'll use point filters
		int nAdapter = m_pSrcDevice->GetAdapterID();
		DISPLAYMODE Mode;
		if ( m_dwVersion >= 0x0900 && !KeySet("pointf") &&
			(FAILED(m_pD3D->GetAdapterDisplayMode( m_pSrcDevice->GetAdapterID(), &Mode )) ||
			 FAILED(m_pD3D->CheckDeviceFormat(nAdapter, m_pSrcDevice->GetDevType(), Mode.Format, D3DUSAGE_QUERY_FILTER, RTYPE_TEXTURE, *pFormat))))
		{
			SetSamplerState(i, SAMP_MAGFILTER, (DWORD)TEXF_POINT);
			SetSamplerState(i, SAMP_MINFILTER, (DWORD)TEXF_POINT);
		}

		//setting up texture and texop parameters
		if ( !SetTexture(i, m_pTextures[i]) )
		{
			hr = GetLastError();
			WriteToLog("SetupTextures: SetTexture on %d stage failed. hr=%s(%x).\n", i, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		if ( m_isAntiAliased )
		{
			if (0 != i)
				SetTextureStageState(i, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
			//SetTextureStageState(i, D3DTSS_ALPHAARG1, (DWORD)D3DTA_DIFFUSE);
		}

		if (i) 
		{
			if ( !SetTextureStageState(i, D3DTSS_COLOROP, top_op == D3DTOP_ADD ? (DWORD)D3DTOP_ADD : (DWORD)D3DTOP_MODULATE) )
			{
				hr = GetLastError();
				WriteToLog("SetupTextures: Set D3DTSS_COLOROP on %d stage failed. hr=%s(%x).\n", i, m_pD3D->HResultToString(hr), hr);
				goto Exit;
			}
		}
		else
		{
			//SetTextureStageState(i, D3DTSS_COLOROP, KeySet(_T("add")) ? (DWORD)D3DTOP_SELECTARG1 : (DWORD)D3DTOP_MODULATE);
			if ( !SetTextureStageState(i, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1) )
			{
				hr = GetLastError();
				WriteToLog("SetupTextures: Set D3DTSS_COLOROP on %d stage failed. hr=%s(%x).\n", i, m_pD3D->HResultToString(hr), hr);
				goto Exit;
			}
        }
	}

Exit:
	if (FAILED(hr))
	{
		Fail();
		if (NULL != m_pTextures)
		for (int j = 0; j < m_nTextures; j++)
			RELEASE(m_pTextures[j]);
		SAFEDELETEA(m_pTextures);
		m_nTextures = 0;
	}

	delete [] Stripes;
	return true;
}

//-----------------------------------------------------------------------------

void CMultiTexTest::RemoveTextures()
{
	//by default linear filters are already set
	//however if previous format didn't support linear filters, point filters were used
	int nAdapter = m_pSrcDevice->GetAdapterID();
	DISPLAYMODE Mode;
	if (m_dwVersion >= 0x0900 && !KeySet("pointf") &&
		(FAILED(m_pD3D->GetAdapterDisplayMode( m_pSrcDevice->GetAdapterID(), &Mode )) ||
		 FAILED(m_pD3D->CheckDeviceFormat(nAdapter, m_pSrcDevice->GetDevType(), Mode.Format, D3DUSAGE_QUERY_FILTER, RTYPE_TEXTURE, *GetCurrentTextureFormat()))))
	{
		for (int i = 0; i < m_nTextures; i++)
		{
			SetSamplerState(i, SAMP_MAGFILTER, (DWORD)TEXF_LINEAR);
			SetSamplerState(i, SAMP_MINFILTER, (DWORD)TEXF_LINEAR);
		}
	}

	for (int i = 0; i < m_nTextures; i++)
	{
		SetTexture(i, NULL);
		SetTextureStageState(i, D3DTSS_COLOROP, (DWORD)D3DTOP_DISABLE);
		if ( m_isAntiAliased && i != 0 )
			SetTextureStageState(i, D3DTSS_ALPHAOP, (DWORD)D3DTOP_DISABLE);
	}

	for (int i = 0; i < m_nTextures; i++)
		RELEASE(m_pTextures[i]);
	if ( m_nTextures )
		SAFEDELETEA(m_pTextures);
	m_nTextures = 0;
}